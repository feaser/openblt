/************************************************************************************//**
* \file         port\win32\xcptransport.c
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
* You have received a copy of the GNU General Public License along with OpenBLT. It 
* should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
* 
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include <assert.h>                                   /* assertion module              */
#include <sb_types.h>                                 /* C types                       */
#include <windows.h>                                  /* for WIN32 library             */
#include <string.h>                                   /* string library                */
#include "xcpmaster.h"                                /* XCP master protocol module    */
#include "timeutil.h"                                 /* time utility module           */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
#define UART_TX_BUFFER_SIZE      (1024)               /**< transmission buffer size    */
#define UART_RX_BUFFER_SIZE      (1024)               /**< reception buffer size       */

/** \brief maximum number of bytes in a transmit/receive XCP packet in UART. */
#define XCP_MASTER_UART_MAX_DATA ((XCP_MASTER_TX_MAX_DATA>XCP_MASTER_RX_MAX_DATA) ? \
                                   (XCP_MASTER_TX_MAX_DATA+1) : (XCP_MASTER_RX_MAX_DATA+1))


/****************************************************************************************
* Local data declarations
****************************************************************************************/
static tXcpTransportResponsePacket responsePacket;
static HANDLE hUart = INVALID_HANDLE_VALUE;


/************************************************************************************//**
** \brief     Initializes the communication interface used by this transport layer.
** \param     device Serial communication device name. For example "COM4".
** \param     baudrate Communication speed in bits/sec.
** \return    SB_TRUE if successful, SB_FALSE otherwise.
**
****************************************************************************************/
sb_uint8 XcpTransportInit(sb_char *device, sb_uint32 baudrate)
{
  COMMTIMEOUTS timeouts = { 0 };
  DCB dcbSerialParams = { 0 };
  char portStr[64] = "\\\\.\\\0"; 

  /* construct the COM port name as a string */
  strcat_s(portStr, 59, device);

  /* obtain access to the COM port */
  hUart = CreateFile(portStr, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 
                       FILE_ATTRIBUTE_NORMAL, 0);

  /* validate COM port handle */
  if (hUart == INVALID_HANDLE_VALUE)
  {
    return SB_FALSE;
  }

  /* get current COM port configuration */
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
  if (!GetCommState(hUart, &dcbSerialParams))
  {
    XcpTransportClose();
    return SB_FALSE;
  }

  /* configure the baudrate and 8,n,1 */
  dcbSerialParams.BaudRate = baudrate;
  dcbSerialParams.ByteSize = 8;
  dcbSerialParams.StopBits = ONESTOPBIT;
  dcbSerialParams.Parity = NOPARITY;
  dcbSerialParams.fOutX = FALSE;
  dcbSerialParams.fInX = FALSE;
  dcbSerialParams.fRtsControl = RTS_CONTROL_DISABLE;
  dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

  if (!SetCommState(hUart, &dcbSerialParams))
  {
    XcpTransportClose();
    return SB_FALSE;
  }

  /* set communication timeout parameters */
  timeouts.ReadIntervalTimeout = 0;
  timeouts.ReadTotalTimeoutConstant = 0;
  timeouts.ReadTotalTimeoutMultiplier = 100;
  timeouts.WriteTotalTimeoutConstant = 0;
  timeouts.WriteTotalTimeoutMultiplier = 100;

  if (!SetCommTimeouts(hUart, &timeouts))
  {
    XcpTransportClose();
    return SB_FALSE;
  }

  /* set transmit and receive buffer sizes */
  if(!SetupComm(hUart, UART_RX_BUFFER_SIZE, UART_TX_BUFFER_SIZE))
  {
    XcpTransportClose();
    return SB_FALSE;
  }

  /* empty the transmit and receive buffers */
  if (!FlushFileBuffers(hUart))
  {
    XcpTransportClose();
    return SB_FALSE;
  }
  /* successfully connected to the serial device */
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
  sb_uint32 dwWritten = 0;
  sb_uint32 dwRead = 0;
  sb_uint32 dwToRead;
  sb_uint16 cnt;
  static sb_uint8 xcpUartBuffer[XCP_MASTER_UART_MAX_DATA]; /* static to lower stack load */
  sb_uint16 xcpUartLen;
  sb_uint8 *uartReadDataPtr;
  sb_uint32 timeoutTime;

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

  /* first submit the XCP packet for transmission */
  if (!WriteFile(hUart, xcpUartBuffer, xcpUartLen, &dwWritten, SB_NULL)) 
  {
    return SB_FALSE;
  }
  /* double check that all bytes were actually transmitted */
  if (dwWritten != xcpUartLen)
  {
    return SB_FALSE;
  }

  /* ------------------------ XCP packet reception ----------------------------------- */
  /* determine timeout time */
  timeoutTime = TimeUtilGetSystemTimeMs() + timeOutMs + 100;

  /* read the first byte, which contains the length of the xcp packet that follows */
  dwToRead = 1;
  responsePacket.len = 0;
  while(dwToRead > 0)
  {
    dwRead = 0;
    if (ReadFile(hUart, &responsePacket.len, dwToRead, &dwRead, NULL))
    {
      /* one byte should be read and it should contain the packet length, which cannot be 0 */
      if ((dwRead == dwToRead) && (responsePacket.len > 0))
      {
        /* valid packet length received so stop this loop to continue with the reception 
         * remaining packet bytes
         */
        dwToRead = 0;
      }
    }
    /* check for timeout if not yet done */
    if ( (dwToRead > 0) && (TimeUtilGetSystemTimeMs() >= timeoutTime) )
    {
      /* timeout occurred */
      return SB_FALSE;
    }
  }

  /* read the rest of the packet */
  dwToRead = responsePacket.len;
  uartReadDataPtr = &responsePacket.data[0];
  while(dwToRead > 0)
  {
    dwRead = 0;
    if (ReadFile(hUart, uartReadDataPtr, dwToRead, &dwRead, NULL))
    {
      /* update the bytes that were already read */
      uartReadDataPtr += dwRead;
      dwToRead -= dwRead;
    }
    /* check for timeout if not yet done */
    if ( (dwToRead > 0) && (TimeUtilGetSystemTimeMs() >= timeoutTime) )
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
  if (hUart != INVALID_HANDLE_VALUE)
  {
    CloseHandle(hUart);
  }

  /* set handles to invalid */
  hUart = INVALID_HANDLE_VALUE;
} /*** end of XcpTransportClose ***/


/*********************************** end of xcptransport.c *****************************/
