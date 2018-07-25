/************************************************************************************//**
* \file         port/windows/serialport.c
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
#include <windows.h>                        /* for windows library                     */
#include "serialport.h"                     /* serial port module                      */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
#define UART_TX_BUFFER_SIZE      (1024u)              /**< transmission buffer size    */
#define UART_RX_BUFFER_SIZE      (1024u)              /**< reception buffer size       */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Serial port handle. */
static HANDLE hUart;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static uint32_t SerialConvertBaudrate(tSerialPortBaudrate baudrate);


/************************************************************************************//**
** \brief     Initializes the serial port module.
**
****************************************************************************************/
void SerialPortInit(void)
{
  /* Invalidate the port handle. */
  hUart = INVALID_HANDLE_VALUE;
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
** \param     portname The name of the serial port to open, i.e. COM4.
** \param     baudrate The desired communication speed.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool SerialPortOpen(char const * portname, tSerialPortBaudrate baudrate)
{
  bool result = false;
  COMMTIMEOUTS timeouts = { 0 };
  DCB dcbSerialParams = { 0 };
  char portStr[64] = "\\\\.\\";

  /* Check parameters. */
  assert(portname != NULL);

  /* Only continue if parameters are valid. */
  if (portname != NULL)
  {
    /* Assume the result to be okay from here on and only set it to error when a problem
     * was detected.
     */
    result = true;
    /* Construct the COM port name as a string. */
    strcat(portStr, portname);

    /* Obtain access to the COM port. */
    hUart = CreateFile(portStr, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL, 0);
    /* Validate COM port handle. */
    if (hUart == INVALID_HANDLE_VALUE)
    {
      result = false;
    }

    /* Get current COM port configuration. */
    if (result)
    {
      dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
      if (!GetCommState(hUart, &dcbSerialParams))
      {
        (void)CloseHandle(hUart);
        result = false;
      }
    }

    /* Configure the baudrate and 8,n,1. */
    if (result)
    {
      dcbSerialParams.BaudRate = SerialConvertBaudrate(baudrate);
      dcbSerialParams.ByteSize = 8;
      dcbSerialParams.StopBits = ONESTOPBIT;
      dcbSerialParams.Parity = NOPARITY;
      dcbSerialParams.fOutX = FALSE;
      dcbSerialParams.fInX = FALSE;
      dcbSerialParams.fRtsControl = RTS_CONTROL_DISABLE;
      dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;
      if (!SetCommState(hUart, &dcbSerialParams))
      {
        (void)CloseHandle(hUart);
        result =false;
      }
    }

    /* Set communication timeout parameters. */
    if (result)
    {
      timeouts.ReadIntervalTimeout = 0;
      timeouts.ReadTotalTimeoutConstant = 0;
      timeouts.ReadTotalTimeoutMultiplier = 100;
      timeouts.WriteTotalTimeoutConstant = 0;
      timeouts.WriteTotalTimeoutMultiplier = 100;
      if (!SetCommTimeouts(hUart, &timeouts))
      {
        (void)CloseHandle(hUart);
        result = false;
      }
    }

    /* Set transmit and receive buffer sizes. */
    if (result)
    {
      if (!SetupComm(hUart, UART_RX_BUFFER_SIZE, UART_TX_BUFFER_SIZE))
      {
        (void)CloseHandle(hUart);
        result = false;
      }
    }

    /* Empty the transmit and receive buffers. */
    if (result)
    {
      if (!FlushFileBuffers(hUart))
      {
        (void)CloseHandle(hUart);
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
  /* Close the COM port handle if valid. */
  if (hUart != INVALID_HANDLE_VALUE)
  {
    (void)CloseHandle(hUart);
  }

  /* Set handles to invalid. */
  hUart = INVALID_HANDLE_VALUE;
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
  DWORD dwWritten = 0;

  /* Check parameters. */
  assert(data != NULL);
  assert(length > 0);

  /* Only continue with valid parameters. */
  if ( (data != NULL) && (length > 0) )
  {
    /* Submit the data for transmission with the serial port. */
    if (WriteFile(hUart, data, length, &dwWritten, NULL))
    {
      /* Double check that all bytes were actually transmitted. */
      if (dwWritten == length)
      {
        result = true;
      }
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
  DWORD dwRead = 0;

  /* Check parameters. */
  assert(data != NULL);
  assert(length > 0);

  /* Only continue with valid parameters. */
  if ( (data != NULL) && (length > 0) )
  {
    /* Attempt to read data from the serial port. */
    if (ReadFile(hUart, data, length, &dwRead, NULL))
    {
      /* Double check that all bytes were actually read. */
      if (dwRead == length)
      {
        result = true;
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of SerialPortRead ***/


/************************************************************************************//**
** \brief     Opens the connection with the serial port configured as 8,N,1 and no flow
**            control.
** \param     baudrate The desired communication speed.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static uint32_t SerialConvertBaudrate(tSerialPortBaudrate baudrate)
{
  uint32_t result;

  switch (baudrate)
  {
    case SERIALPORT_BR9600:
      result = CBR_9600;
      break;
    case SERIALPORT_BR19200:
      result = CBR_19200;
      break;
    case SERIALPORT_BR38400:
      result = CBR_38400;
      break;
    case SERIALPORT_BR57600:
      result = CBR_57600;
      break;
    case SERIALPORT_BR115200:
      result = CBR_115200;
      break;
    default:
      result = CBR_9600;
      break;
  }
  return result;
} /*** end of SerialConvertBaudrate ***/


/*********************************** end of serialport.c *******************************/

