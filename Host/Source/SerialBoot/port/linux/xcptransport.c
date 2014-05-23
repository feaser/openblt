/************************************************************************************//**
* \file         port\linux\xcptransport.c
* \brief        XCP transport layer interface source file.
* \ingroup      SerialBoot
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2014  by Feaser    http://www.feaser.com    All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 3 of the License, or (at your option) any later
* version.
*
* OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE. See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with OpenBLT.
* If not, see <http://www.gnu.org/licenses/>.
*
* A special exception to the GPL is included to allow you to distribute a combined work
* that includes OpenBLT without being obliged to provide the source code for any
* proprietary components. The exception text is included at the bottom of the license
* file <license.html>.
*
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include <assert.h>                                   /* assertion module              */
#include <sb_types.h>                                 /* C types                       */
#include <stdio.h>                                    /* standard I/O library          */
#include <string.h>                                   /* string function definitions   */
#include <unistd.h>                                   /* UNIX standard functions       */
#include <fcntl.h>                                    /* file control definitions      */
#include <errno.h>                                    /* error number definitions      */
#include <termios.h>                                  /* POSIX terminal control        */
#include "xcpmaster.h"                                /* XCP master protocol module    */
#include "timeutil.h"                                 /* time utility module           */



/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Invalid UART device/file handle. */
#define UART_INVALID_HANDLE      (-1)

/** \brief maximum number of bytes in a transmit/receive XCP packet in UART. */
#define XCP_MASTER_UART_MAX_DATA ((XCP_MASTER_TX_MAX_DATA>XCP_MASTER_RX_MAX_DATA) ? \
                                  (XCP_MASTER_TX_MAX_DATA+1) : (XCP_MASTER_RX_MAX_DATA+1))

/** \brief The smallest time in millisecond that the UART is configured for. */
#define UART_RX_TIMEOUT_MIN_MS   (100)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static speed_t   XcpTransportGetBaudrateMask(sb_uint32 baudrate);


/****************************************************************************************
* Local data declarations
****************************************************************************************/
static tXcpTransportResponsePacket responsePacket;
static sb_int32 hUart = UART_INVALID_HANDLE;


/************************************************************************************//**
** \brief     Initializes the communication interface used by this transport layer.
** \param     device Serial communication device name. For example "COM4".
** \param     baudrate Communication speed in bits/sec.
** \return    SB_TRUE if successful, SB_FALSE otherwise.
**
****************************************************************************************/
sb_uint8 XcpTransportInit(sb_char *device, sb_uint32 baudrate)
{
  struct termios options;

  /* open the port */
  hUart = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
  /* verify the result */
  if (hUart == UART_INVALID_HANDLE)
  {
    return SB_FALSE;
  }
  /* configure the device to block during read operations */
  if (fcntl(hUart, F_SETFL, 0) == -1)
  {
    XcpTransportClose();
    return SB_FALSE;
  }
  /* get the current options for the port */
  if (tcgetattr(hUart, &options) == -1)
  {
    XcpTransportClose();
    return SB_FALSE;
  }
  /* configure the baudrate */
  if (cfsetispeed(&options, XcpTransportGetBaudrateMask(baudrate)) == -1)
  {
    XcpTransportClose();
    return SB_FALSE;
  }
  if (cfsetospeed(&options, XcpTransportGetBaudrateMask(baudrate)) == -1)
  {
    XcpTransportClose();
    return SB_FALSE;
  }
  /* enable the receiver and set local mode */
  options.c_cflag |= (CLOCAL | CREAD);
  /* configure 8-n-1 */
  options.c_cflag &= ~PARENB;
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;
  /* disable hardware flow control */
  options.c_cflag &= ~CRTSCTS;
  /* configure raw input */
  options.c_lflag &= ~(ICANON | ISIG);
  /* configure raw output */
  options.c_oflag &= ~OPOST;
  /* configure timeouts */
  options.c_cc[VMIN]  = 0;
  options.c_cc[VTIME] = UART_RX_TIMEOUT_MIN_MS/100; /* 1/10th of a second */
  /* set the new options for the port */
  if (tcsetattr(hUart, TCSAFLUSH, &options) == -1)
  {
    XcpTransportClose();
    return SB_FALSE;
  }
  /* success */
  return SB_TRUE;
} /*** end of XcpTransportInit ***/


/************************************************************************************//**
** \brief     Transmits an XCP packet on the transport layer and attemps to receive the
**            response within the given timeout. The data in the response packet is
**            stored in an internal data buffer that can be obtained through function
**            XcpTransportReadResponsePacket().
** \return    SB_TRUE is the response packet was successfully received and stored,
**            SB_FALSE otherwise.
**
****************************************************************************************/
sb_uint8 XcpTransportSendPacket(sb_uint8 *data, sb_uint8 len, sb_uint16 timeOutMs)
{
  sb_uint16 cnt;
  static sb_uint8 xcpUartBuffer[XCP_MASTER_UART_MAX_DATA]; /* static to lower stack load */
  sb_uint16 xcpUartLen;
  sb_int32 bytesSent;
  sb_int32 bytesToRead;
  sb_int32 bytesRead;
  sb_uint8 *uartReadDataPtr;
  sb_uint32 timeoutTime;
  sb_uint32 nowTime;
  ssize_t result;

  /* ------------------------ XCP packet transmission -------------------------------- */
  /* prepare the XCP packet for transmission on UART. this is basically the same as the
   * xcp packet data but just the length of the packet is added to the first byte.
   */
  xcpUartLen = len+1;
  xcpUartBuffer[0] = len;
  for (cnt=0; cnt<len; cnt++)
  {
    xcpUartBuffer[cnt+1] = data[cnt];
  }

  bytesSent = write(hUart, xcpUartBuffer, xcpUartLen);

  if (bytesSent != xcpUartLen)
  {
    return SB_FALSE;
  }

  /* ------------------------ XCP packet reception ----------------------------------- */
  /* determine timeout time */
  timeoutTime = TimeUtilGetSystemTimeMs() + timeOutMs + UART_RX_TIMEOUT_MIN_MS;

  /* read the first byte, which contains the length of the xcp packet that follows */
  bytesToRead = 1;
  uartReadDataPtr = &responsePacket.len;
  while(bytesToRead > 0)
  {
    result = read(hUart, uartReadDataPtr, bytesToRead);
    if (result != -1)
    {
      bytesRead = result;
      /* update the bytes that were already read */
      uartReadDataPtr += bytesRead;
      bytesToRead -= bytesRead;
    }
    /* check for timeout if not yet done */
    if ( (bytesToRead > 0) && (TimeUtilGetSystemTimeMs() >= timeoutTime) )
    {
      /* timeout occurred */
      return SB_FALSE;
    }
  }

  /* read the rest of the packet */
  bytesToRead = responsePacket.len;
  uartReadDataPtr = &responsePacket.data[0];
  while(bytesToRead > 0)
  {
    result = read(hUart, uartReadDataPtr, bytesToRead);
    if (result != -1)
    {
      bytesRead = result;
      /* update the bytes that were already read */
      uartReadDataPtr += bytesRead;
      bytesToRead -= bytesRead;
    }
    /* check for timeout if not yet done */
    if ( (bytesToRead > 0) && (TimeUtilGetSystemTimeMs() >= timeoutTime) )
    {
      /* timeout occurred */
      return SB_FALSE;
    }
  }
  /* still here so the complete packet was received */
  return SB_TRUE;
} /*** end of XcpMasterTpSendPacket ***/


/************************************************************************************//**
** \brief     Reads the data from the response packet. Make sure to not call this
**            function while XcpTransportSendPacket() is active, because the data won't be
**            valid then.
** \return    Pointer to the response packet data.
**
****************************************************************************************/
tXcpTransportResponsePacket *XcpTransportReadResponsePacket(void)
{
  return &responsePacket;
} /*** end of XcpTransportReadResponsePacket ***/


/************************************************************************************//**
** \brief     Closes the communication channel.
** \return    none.
**
****************************************************************************************/
void XcpTransportClose(void)
{
  /* close the COM port handle if valid */
  if (hUart != UART_INVALID_HANDLE)
  {
    close(hUart);
  }

  /* set handles to invalid */
  hUart = UART_INVALID_HANDLE;
} /*** end of XcpTransportClose ***/


/************************************************************************************//**
** \brief     Converts the baudrate value to a bitmask value used by termios. Currently
**            supports the most commonly used baudrates.
** \return    none.
**
****************************************************************************************/
static speed_t XcpTransportGetBaudrateMask(sb_uint32 baudrate)
{
  speed_t result;

  switch (baudrate)
  {
    case 115200:
      result = B115200;
      break;
    case 57600:
      result = B57600;
      break;
    case 38400:
      result = B38400;
      break;
    case 19200:
      result = B19200;
      break;
    case 9600:
    default:
      result = B9600;
      break;
  }
  return result;
} /*** end of XcpTransportGetBaudrateMask ***/


/*********************************** end of xcptransport.c *****************************/
