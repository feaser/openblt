/************************************************************************************//**
* \file         port/linux/serialport.c
* \brief        Serial port source file.
* \ingroup      SerialPort
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
#include <assert.h>                         /* for assertions                          */
#include <stdint.h>                         /* for standard integer types              */
#include <stddef.h>                         /* for NULL declaration                    */
#include <stdbool.h>                        /* for boolean type                        */
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
/** \brief Serial port handle. */
static int32_t portHandle;


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
** \brief     Initializes the serial port module.
**
****************************************************************************************/
void SerialPortInit(void)
{
  /* Invalidate the port handle. */
  portHandle = SERIALPORT_INVALID_HANDLE;
} /*** end of SerialPortInit ***/


/************************************************************************************//**
** \brief     Terminates the serial port module.
**
****************************************************************************************/
void SerialPortTerminate(void)
{
  /* Make sure the serial port is closed. */
  SerialPortClose();
} /*** end of SerialPortTerminate ***/


/************************************************************************************//**
** \brief     Opens the connection with the serial port configured as 8,N,1 and no flow
**            control.
** \param     portname The name of the serial port to open, i.e. /dev/ttyUSB0.
** \param     baudrate The desired communication speed.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool SerialPortOpen(char const * portname, tSerialPortBaudrate baudrate) 
{
  bool result = false;
  struct termios options = { 0 }; 
  int32_t iFlags;

  /* Check parameters. */
  assert(portname != NULL);
  
  /* Only continue with valid parameters. */
  if (portname != NULL) /*lint !e774 */
  {
    /* Assume the result to be okay from here on and only set it to error when a problem
     * was detected.
     */
    result = true;
    /* Open the port. */
    portHandle = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);
    /* Check the result */
    if (portHandle == SERIALPORT_INVALID_HANDLE)
    {
      result = false;
    }
    
    /* Configure the device to block during read operations. */
    if (result)
    {
      if (fcntl(portHandle, F_SETFL, 0) == -1)
      {
        SerialPortClose();
        result = false;
      }
    }
    /* Get the current options for the port. */
    if (result)
    {
      if (tcgetattr(portHandle, &options) == -1)
      {
        SerialPortClose();
        result = false;
      }
    }
    /* Configure the baudrate. */
    if (result)
    {
      if (cfsetispeed(&options, baudrateLookup[baudrate]) == -1)
      {
        SerialPortClose();
        result = false;
      }
    }
    if (result)
    {
      if (cfsetospeed(&options, baudrateLookup[baudrate]) == -1)
      {
        SerialPortClose();
        result = false;
      }
    }

    if (result)
    {
      options.c_cflag |= ( CLOCAL | CREAD |  CS8);    // Configure the device : 8 bits, no parity, no control
      options.c_iflag |= ( IGNPAR | IGNBRK );
      options.c_cc[VTIME]=0;                          // Timer unused
      options.c_cc[VMIN]=0;                           // At least on character before satisfy reading
      if (tcsetattr(portHandle, TCSANOW, &options) == -1)
      {
        SerialPortClose();
        result = false;
      }
    }
  }
  
  /* Give the result back to the caller. */
  return result;
} /*** end of SerialPortOpen ***/


/************************************************************************************//**
** \brief     Closes the connection with the serial port.
**
****************************************************************************************/
void SerialPortClose(void)
{
  /* Close the port handle if valid. */
  if (portHandle != SERIALPORT_INVALID_HANDLE)
  {
    close(portHandle);
  }
  /* Invalidate handle. */
  portHandle = SERIALPORT_INVALID_HANDLE;
} /*** end of SerialPortClose ***/


/************************************************************************************//**
** \brief     Writes data to the serial port.
** \param     data Pointer to byte array with data to write.
** \param     length Number of bytes to write.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool SerialPortWrite(uint8_t const * data, uint32_t length)
{
  bool result = false;

  /* Check parameters. */
  assert(data != NULL);
  assert(length > 0);

  /* Only continue with valid parameters. */
  if ( (data != NULL) && (length > 0) ) /*lint !e774 */
  {
    /* Submit the data for sending. */
    if ((uint32_t)write(portHandle, data, length) == length)
    {
      result = true;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of SerialPortWrite ***/


/************************************************************************************//**
** \brief     Reads data from the serial port in a blocking manner.
** \param     data Pointer to byte array to store read data.
** \param     length Number of bytes to read.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool SerialPortRead(uint8_t * data, uint32_t length)
{
  bool result = false;

  /* Check parameters. */
  assert(data != NULL);
  assert(length > 0);
  
  /* Only continue with valid parameters. */
  if ( (data != NULL) && (length > 0) ) /*lint !e774 */
  {
    /* Attempt to read the requested data. */
    if ((uint32_t)read(portHandle, data, length) == length)
    {
      result = true;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of SerialPortRead ***/


/*********************************** end of serialport.c *******************************/

