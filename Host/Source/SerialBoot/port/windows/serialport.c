/************************************************************************************//**
* \file         port\windows\serialport.c
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
#include <windows.h>                        /* for windows library                     */
#include <assert.h>                         /* for assertions                          */
#include "serialport.h"                     /* serial port module                      */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
#define UART_TX_BUFFER_SIZE      (1024)               /**< transmission buffer size    */
#define UART_RX_BUFFER_SIZE      (1024)               /**< reception buffer size       */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
static HANDLE hUart = INVALID_HANDLE_VALUE;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static uint32_t SerialConvertBaudrate(tSerialPortBaudrate baudrate);


/************************************************************************************//**
** \brief     Opens the connection with the serial port configured as 8,N,1 and no flow
**            control.
** \param     portname The name of the serial port to open, i.e. COM4.
** \param     baudrate The desired communication speed.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool SerialPortOpen(char *portname, tSerialPortBaudrate baudrate)
{
  COMMTIMEOUTS timeouts = { 0 };
  DCB dcbSerialParams = { 0 };
  char portStr[64] = "\\\\.\\\0";

  /* check parameters */
  assert(portname != NULL);

  /* construct the COM port name as a string */
  strcat_s(portStr, 59, portname);

  /* obtain access to the COM port */
  hUart = CreateFile(portStr, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 
                     FILE_ATTRIBUTE_NORMAL, 0);

  /* validate COM port handle */
  if (hUart == INVALID_HANDLE_VALUE)
  {
	  return false;
  }

  /* get current COM port configuration */
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
  if (!GetCommState(hUart, &dcbSerialParams))
  {
    CloseHandle(hUart);
    return false;
  }

  /* configure the baudrate and 8,n,1 */
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
    CloseHandle(hUart);
    return false;
  }

  /* set communication timeout parameters */
  timeouts.ReadIntervalTimeout = 0;
  timeouts.ReadTotalTimeoutConstant = 0;
  timeouts.ReadTotalTimeoutMultiplier = 100;
  timeouts.WriteTotalTimeoutConstant = 0;
  timeouts.WriteTotalTimeoutMultiplier = 100;

  if (!SetCommTimeouts(hUart, &timeouts))
  {
    CloseHandle(hUart);
    return false;
  }

  /* set transmit and receive buffer sizes */
  if (!SetupComm(hUart, UART_RX_BUFFER_SIZE, UART_TX_BUFFER_SIZE))
  {
    CloseHandle(hUart);
    return false;
  }

  /* empty the transmit and receive buffers */
  if (!FlushFileBuffers(hUart))
  {
    CloseHandle(hUart);
    return false;
  }
  /* successfully connected to the serial device */
  return true;
} /*** end of SerialPortOpen ***/


/************************************************************************************//**
** \brief     Closes the connection with the serial port.
** \return    None.
**
****************************************************************************************/
void SerialPortClose(void)
{
  /* close the COM port handle if valid */
  if (hUart != INVALID_HANDLE_VALUE)
  {
    CloseHandle(hUart);
  }

  /* set handles to invalid */
  hUart = INVALID_HANDLE_VALUE;
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
  uint32_t dwWritten = 0;

  /* check parameters */
  assert(data != NULL);
  assert(length > 0);

  /* submit the data for transmission with the serial port */
  if (!WriteFile(hUart, data, length, &dwWritten, NULL))
  {
    return false;
  }
  /* double check that all bytes were actually transmitted */
  if (dwWritten != length)
  {
    return false;
  }

  /* success */
  return true;
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
  uint32_t dwRead = 0;

  /* check parameters */
  assert(data != NULL);
  assert(length > 0);

  /* attempt to read data from the serial port */
  if (!ReadFile(hUart, data, length, &dwRead, NULL))
  {
    return false;
  }
  /* double check that all bytes were actually read */
  if (dwRead != length)
  {
    return false;
  }

  /* success */
  return true;
} /*** end of SerialPortRead ***/


/************************************************************************************//**
** \brief     Opens the connection with the serial port configured as 8,N,1 and no flow
**            control.
** \param     portname The name of the serial port to open, i.e. COM4.
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

