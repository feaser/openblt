/************************************************************************************//**
* \file         tbxmb_uart.c
* \brief        Modbus UART interface source file.
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

/****************************************************************************************
* Include files
****************************************************************************************/
#include "microtbx.h"                            /* MicroTBX module                    */
#include "microtbxmodbus.h"                      /* MicroTBX-Modbus module             */
#include "tbxmb_uart_private.h"                  /* MicroTBX-Modbus UART private       */


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Data type for grouping together UART channel specific information. */
typedef struct
{
  tTbxMbUartTransmitComplete transmitCompleteFcn;
  tTbxMbUartDataReceived     dataReceivedFcn;
} tTbxMbUartInfo;


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Array with UART port specific information. */
static volatile tTbxMbUartInfo uartInfo[TBX_MB_UART_NUM_PORT];


/************************************************************************************//**
** \brief     Initializes the UART channel.
** \param     port The serial port to use. The actual meaning of the serial port is
**            hardware dependent. It typically maps to the UART peripheral number. E.g. 
**            TBX_MB_UART_PORT1 = USART1 on an STM32.
** \param     baudrate The desired communication speed.
** \param     databits Number of databits for a character.
** \param     stopbits Number of stop bits at the end of a character.
** \param     parity Parity bit type to use.
** \param     transfer_complete_fcn Transport layer specific transfer complete callback
**            function or NULL if not used.
** \param     dataReceivedFcn Transport layer specific new data received callback
**            function or NULL if not used.
**
****************************************************************************************/
void TbxMbUartInit(tTbxMbUartPort             port, 
                   tTbxMbUartBaudrate         baudrate,
                   tTbxMbUartDatabits         databits, 
                   tTbxMbUartStopbits         stopbits,
                   tTbxMbUartParity           parity,
                   tTbxMbUartTransmitComplete transmitCompleteFcn,
                   tTbxMbUartDataReceived     dataReceivedFcn)
{
  /* Verify parameters. */
  TBX_ASSERT((port < TBX_MB_UART_NUM_PORT) && 
             (baudrate < TBX_MB_UART_NUM_BAUDRATE) &&
             (databits < TBX_MB_UART_NUM_DATABITS) &&
             (stopbits < TBX_MB_UART_NUM_STOPBITS) &&
             (parity < TBX_MB_UART_NUM_PARITY));

  /* Only continue with valid parameters. */
  if ((port < TBX_MB_UART_NUM_PORT) && 
      (baudrate < TBX_MB_UART_NUM_BAUDRATE) &&
      (databits < TBX_MB_UART_NUM_DATABITS) &&
      (stopbits < TBX_MB_UART_NUM_STOPBITS) &&
      (parity < TBX_MB_UART_NUM_PARITY))
  {
    /* Store the specified callback functions. */
    uartInfo[port].transmitCompleteFcn = transmitCompleteFcn;
    uartInfo[port].dataReceivedFcn = dataReceivedFcn;
    /* Request the port module to perform the low-level UART initialization. */
    TbxMbPortUartInit(port, baudrate, databits, stopbits, parity);
  }
} /*** end of TbxMbUartInit ***/  


/************************************************************************************//**
** \brief     Starts the transfer of len bytes from the data array on the specified 
**            serial port.
** \attention If this function succeeds, the data array should remain locked until the
**            transfer completed, as signalled by a call to TbxMbUartTransmitComplete().
**            Thanks to this approach it is not needed to copy and buffer the data,
**            resulting in better run-time performance.
** \param     port The serial port to start the data transfer on.
** \param     data Byte array with data to transmit.
** \param     len Number of bytes to transmit.
** \return    TBX_OK if successful, TBX_ERROR otherwise.  
**
****************************************************************************************/
uint8_t TbxMbUartTransmit(tTbxMbUartPort         port, 
                          uint8_t        const * data, 
                          uint16_t               len)
{
  uint8_t result = TBX_ERROR;

  /* Verify parameters. */
  TBX_ASSERT((port < TBX_MB_UART_NUM_PORT) && 
             (data != NULL) &&
             (len > 0U));

  /* Only continue with valid parameters. */
  if ((port < TBX_MB_UART_NUM_PORT) && 
      (data != NULL) &&
      (len > 0U))
  {
    /* Request the port module to perform the low-level UART data transmission. */
    result = TbxMbPortUartTransmit(port, data, len);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbUartTransmit ***/


/************************************************************************************//**
** \brief     Event function to signal to this module that the entire transfer, initiated
**            by TbxMbUartTransmit, completed.
** \attention This function should be called by the hardware specific UART port at Tx
**            interrupt level.
** \param     port The serial port that the transfer completed on.
**
****************************************************************************************/
void TbxMbUartTransmitComplete(tTbxMbUartPort port)
{
  /* Verify parameters. */
  TBX_ASSERT(port < TBX_MB_UART_NUM_PORT);

  /* Only continue with valid parameters. */
  if (port < TBX_MB_UART_NUM_PORT)
  {
    /* Pass the event on to the transport layer for further handling. */
    if (uartInfo[port].transmitCompleteFcn != NULL)
    {
      uartInfo[port].transmitCompleteFcn(port);
    }
  }
} /*** end of TbxMbUartTransmitComplete ***/


/************************************************************************************//**
** \brief     Event function to signal the reception of new data to this module.
** \attention This function should be called by the hardware specific UART port at Rx
**            interrupt level.
** \param     port The serial port that the new data was received on.
** \param     data Byte array with newly received data.
** \param     len Number of newly received bytes.
**
****************************************************************************************/
void TbxMbUartDataReceived(tTbxMbUartPort         port, 
                           uint8_t        const * data, 
                           uint8_t                len)
{
  /* Verify parameters. */
  TBX_ASSERT((port < TBX_MB_UART_NUM_PORT) && 
             (data != NULL) &&
             (len > 0U));

  /* Only continue with valid parameters. */
  if ((port < TBX_MB_UART_NUM_PORT) && 
      (data != NULL) &&
      (len > 0U))
  {
    /* Pass the event on to the transport layer for further handling. */
    if (uartInfo[port].dataReceivedFcn != NULL)
    {
      uartInfo[port].dataReceivedFcn(port, data, len);
    }
  }
} /*** end of TbxMbUartDataReceived ***/


/*********************************** end of tbxmb_uart.c *******************************/
