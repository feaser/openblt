/************************************************************************************//**
* \file         port\linux\serialport.c
* \brief        Serial port source file.
* \ingroup      SerialBoot
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2017  by Feaser    http://www.feaser.com    All rights reserved
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
#include <stddef.h>                         /* for NULL declaration                    */
#include <assert.h>                         /* for assertions                          */
#include <unistd.h>                         /* UNIX standard functions                 */
#include <termios.h>                        /* POSIX terminal control                  */
#include <fcntl.h>                          /* file control definitions                */
#include <sys/ioctl.h>                      /* system I/O control                      */
#include "serialport.h"                     /* serial port module                      */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Invalid serial port device handle. */
#define SERIALPORT_INVALID_HANDLE      (-1)


/****************************************************************************************
* Local data declarations
****************************************************************************************/
static int32_t portHandle = SERIALPORT_INVALID_HANDLE;


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief Lookup table for converting this module's generic baudrate value to a value
 *         supported by the low level interface.
 */
static const speed_t baudrateLookup[] =
{
  B9600,                                    /**< Index 0 = SERIALPORT_BR9600           */
  B19200,                                   /**< Index 1 = SERIALPORT_BR19200          */
  B38400,                                   /**< Index 2 = SERIALPORT_BR38400          */
  B57600,                                   /**< Index 3 = SERIALPORT_BR57600          */
  B115200                                   /**< Index 4 = SERIALPORT_BR115200         */
};


/************************************************************************************//**
** \brief     Opens the connection with the serial port configured as 8,N,1 and no flow
**            control.
** \param     portname The name of the serial port to open, i.e. /dev/ttyUSB0.
** \param     baudrate The desired communication speed.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool SerialPortOpen(char *portname, tSerialPortBaudrate baudrate)
{
  struct termios options;
  int32_t iFlags;

  /* check parameters */
  assert(portname != NULL);

  /* open the port */
  portHandle = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);
  /* check the result */
  if (portHandle == SERIALPORT_INVALID_HANDLE)
  {
    return false;
  }
  /* configure the device to block during read operations */
  if (fcntl(portHandle, F_SETFL, 0) == -1)
  {
    SerialPortClose();
    return false;
  }
  /* get the current options for the port */
  if (tcgetattr(portHandle, &options) == -1)
  {
    SerialPortClose();
    return false;
  }
  /* configure the baudrate */
  if (cfsetispeed(&options, baudrateLookup[baudrate]) == -1)
  {
    SerialPortClose();
    return false;
  }
  if (cfsetospeed(&options, baudrateLookup[baudrate]) == -1)
  {
    SerialPortClose();
    return false;
  }

  /* input modes - clear indicated ones giving: no break, no CR to NL,
   * no parity check, no strip char, no start/stop output (sic) control
   */
  options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  /* output modes - clear giving: no post processing such as NL to CR+NL */
  options.c_oflag &= ~(OPOST);
  /* control modes - set 8 bit chars */
  options.c_cflag |= (CS8);
  /* local modes - clear giving: echoing off, canonical off (no erase with
   * backspace, ^U,...),  no extended functions, no signal chars (^Z,^C)
   */
  options.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  /* configure timeouts */
  options.c_cc[VMIN]  = 0;
  options.c_cc[VTIME] = 1; /* in units of 1/10th of a second */
  /* set the new options for the port */
  if (tcsetattr(portHandle, TCSAFLUSH, &options) == -1)
  {
    SerialPortClose();
    return false;
  }
  /* turn on DTR */
  iFlags = TIOCM_DTR;
  ioctl(portHandle, TIOCMBIS, &iFlags);
  /* success */
  return true;
} /*** end of SerialPortOpen ***/


/************************************************************************************//**
** \brief     Closes the connection with the serial port.
** \return    None.
**
****************************************************************************************/
void SerialPortClose(void)
{
  /* close the port handle if valid */
  if (portHandle != SERIALPORT_INVALID_HANDLE)
  {
    close(portHandle);
  }
  /* invalidate handle */
  portHandle = SERIALPORT_INVALID_HANDLE;
} /*** end of SerialPortClose ***/


/************************************************************************************//**
** \brief     Writes data to the serial port.
** \param     data Pointer to byte array with data to write.
** \param     length Number of bytes to write.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool SerialPortWrite(uint8_t *data, uint32_t length)
{
  size_t bytesWritten;

  /* check parameters */
  assert(data != NULL);
  assert(length > 0);

  /* submit the data for sending */
  bytesWritten = write(portHandle, data, length);
  /* check and return the result */
  return (bytesWritten == length);
} /*** end of SerialPortWrite ***/


/************************************************************************************//**
** \brief     Reads data from the serial port in a blocking manner.
** \param     data Pointer to byte array to store read data.
** \param     length Number of bytes to read.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool SerialPortRead(uint8_t *data, uint32_t length)
{
  size_t bytesRead;

  /* check parameters */
  assert(data != NULL);
  assert(length > 0);

  /* attempt to read the requested data */
  bytesRead = read(portHandle, data, length);
  /* check and return the result */
  return (bytesRead == length);
} /*** end of SerialPortRead ***/


/*********************************** end of serialport.c *******************************/

