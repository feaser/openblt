/************************************************************************************//**
* \file         tbxmb_uart.h
* \brief        Modbus UART interface header file.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2023 by Feaser     www.feaser.com     All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* This file is part of MicroTBX-Modbus. MicroTBX-Modbus is free software: you can
* redistribute it and/or modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* MicroTBX-Modbus is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
* PARTICULAR PURPOSE. See the GNU General Public License for more details.
*
* You have received a copy of the GNU General Public License along with MicroTBX-Modbus.
* If not, see www.gnu.org/licenses/.
*
* \endinternal
****************************************************************************************/
#ifndef TBXMB_UART_H
#define TBXMB_UART_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Enumerated type with all supported UART ports. */
typedef enum
{
  /* UART serial port number  1. */
  TBX_MB_UART_PORT1 = 0U,
  /* UART serial port number  2. */
  TBX_MB_UART_PORT2,
  /* UART serial port number  3. */
  TBX_MB_UART_PORT3,
  /* UART serial port number  4. */
  TBX_MB_UART_PORT4,
  /* UART serial port number  5. */
  TBX_MB_UART_PORT5,
  /* UART serial port number  6. */
  TBX_MB_UART_PORT6,
  /* UART serial port number  7. */
  TBX_MB_UART_PORT7,
  /* UART serial port number  8. */
  TBX_MB_UART_PORT8,
  /* Extra entry to obtain the number of elements. */
  TBX_MB_UART_NUM_PORT
} tTbxMbUartPort;


/** \brief Enumerated type with all supported UART baudrates. */
typedef enum
{
  /* Communication speed of 1200 bits per second. */
  TBX_MB_UART_1200BPS = 0U,
  /* Communication speed of 2400 bits per second. */
  TBX_MB_UART_2400BPS,
  /* Communication speed of 4800 bits per second. */
  TBX_MB_UART_4800BPS,
  /* Communication speed of 9600 bits per second. */
  TBX_MB_UART_9600BPS,
  /* Communication speed of 19200 bits per second. */
  TBX_MB_UART_19200BPS,
  /* Communication speed of 38400 bits per second. */
  TBX_MB_UART_38400BPS,
  /* Communication speed of 57600 bits per second. */
  TBX_MB_UART_57600BPS,
  /* Communication speed of 115200 bits per second. */
  TBX_MB_UART_115200BPS,
  /* Extra entry to obtain the number of elements. */
  TBX_MB_UART_NUM_BAUDRATE
} tTbxMbUartBaudrate;


/** \brief Enumerated type with all supported UART data bits modes. */
typedef enum
{
  /* 7 data bits mode. */
  TBX_MB_UART_7_DATABITS = 0U,
  /* 8 data bits mode. */
  TBX_MB_UART_8_DATABITS,
  /* Extra entry to obtain the number of elements. */
  TBX_MB_UART_NUM_DATABITS
} tTbxMbUartDatabits;


/** \brief Enumerated type with all supported UART stop bits modes. */
typedef enum
{
  /* 1 stop bit mode. */
  TBX_MB_UART_1_STOPBITS = 0U,
  /* 2 stop bits mode. */
  TBX_MB_UART_2_STOPBITS,
  /* Extra entry to obtain the number of elements. */
  TBX_MB_UART_NUM_STOPBITS
} tTbxMbUartStopbits;


/** \brief Enumerated type with all supported parity modes. */
typedef enum
{
  /* Odd parity mode. */
  TBX_MB_ODD_PARITY = 0U,
  /* Even parity mode. */
  TBX_MB_EVEN_PARITY,
  /* No parity mode. */
  TBX_MB_NO_PARITY,
  /* Extra entry to obtain the number of elements. */
  TBX_MB_UART_NUM_PARITY
} tTbxMbUartParity;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void TbxMbUartTransmitComplete(tTbxMbUartPort         port);

void TbxMbUartDataReceived    (tTbxMbUartPort         port, 
                               uint8_t        const * data, 
                               uint8_t                len);


#ifdef __cplusplus
}
#endif

#endif /* TBXMB_UART_H */
/*********************************** end of tbxmb_uart.h *******************************/
