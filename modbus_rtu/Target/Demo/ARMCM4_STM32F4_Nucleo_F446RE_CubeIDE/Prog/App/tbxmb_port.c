/************************************************************************************//**
* \file         tbxmb_port.c
* \brief        Modbus hardware specific port source file.
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
* SPDX-License-Identifier: MIT
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "microtbx.h"                            /* MicroTBX library                   */
#include "microtbxmodbus.h"                      /* MicroTBX-Modbus library            */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void TbxMbPortUartTxInterrupt(tTbxMbUartPort port);
void TbxMbPortUartRxInterrupt(tTbxMbUartPort port);


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Variable that groups together information to control a serial transmission
 *         for each serial port.
 */
static volatile struct
{
  uint8_t  const * data;                 /**< Pointer of the transmit data byte array. */
  uint16_t         nextIdx;              /**< Index of the next byte to transmit.      */
  uint16_t         totalLen;             /**< Total number of bytes to transmit.       */
} transmitInfo[TBX_MB_UART_NUM_PORT];


/************************************************************************************//**
** \brief     Initializes the UART channel.
** \param     port The serial port to use. The actual meaning of the serial port is
**            hardware dependent. It typically maps to the UART peripheral number. E.g. 
**            TBX_MB_UART_PORT1 = USART1 on an STM32.
** \param     baudrate The desired communication speed.
** \param     databits Number of databits for a character.
** \param     stopbits Number of stop bits at the end of a character.
** \param     parity Parity bit type to use.
**
****************************************************************************************/
void TbxMbPortUartInit(tTbxMbUartPort     port, 
                       tTbxMbUartBaudrate baudrate,
                       tTbxMbUartDatabits databits, 
                       tTbxMbUartStopbits stopbits,
                       tTbxMbUartParity   parity)
{
  TBX_UNUSED_ARG(port);
  TBX_UNUSED_ARG(baudrate);
  TBX_UNUSED_ARG(databits);
  TBX_UNUSED_ARG(stopbits);
  TBX_UNUSED_ARG(parity);

  /* TODO ##Port 
   * 
   * Perform the following steps to initialize the UART channel:
   *   - Enable the clock of the UART peripheral.
   *   - Configure the UART Rx and Tx GPIO pins for UART communication.
   *   - Switch the RS485 transceiver to reception mode (DE/NRE pins), if used.
   *   - Configure the baudrate, number of databits, number of stopbits, and parity mode.
   *   - Enable the UART transmitter and receiver.
   *   - Enable the receive data register full (RXNE) interrupt.
   */

} /*** end of TbxMbPortUartInit ***/


/************************************************************************************//**
** \brief     Starts the transfer of len bytes from the data array on the specified 
**            serial port.
** \attention This function has mutual exclusive access to the bytes in the data[] array,
**            until this port module calls TbxMbUartTransmitComplete(). This means that
**            you do not need to copy the data bytes to a local buffer. This approach 
**            keeps RAM requirements low and benefits the run-time performance. Just make
**            sure to call TbxMbUartTransmitComplete() once all bytes are transmitted or
**            an error was detected, to release access to the data[] array.
** \param     port The serial port to start the data transfer on.
** \param     data Byte array with data to transmit.
** \param     len Number of bytes to transmit.
** \return    TBX_OK if successful, TBX_ERROR otherwise.  
**
****************************************************************************************/
uint8_t TbxMbPortUartTransmit(tTbxMbUartPort         port, 
                              uint8_t        const * data, 
                              uint16_t               len)
{
  uint8_t result = TBX_OK;

  /* Prepare the transmit information taking into account that this function will already
   * start the transmission of the first byte.
   */
  transmitInfo[port].data = data;
  transmitInfo[port].totalLen = len;
  transmitInfo[port].nextIdx = 1U;

  /* TODO ##Port 
   * 
   * - Switch the RS485 transceiver to transmission mode (DE/NRE pins), if used.
   * - Write the first byte (transmitInfo[port].data[0]) to the UART transmit data
   *   register.
   * - Enable the transmit complete (TC) interrupt if the total length is just one
   *   byte (len == 1), otherwise enable the transmit data register empty (TXE)
   *   interrupt.
   */

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbPortUartTransmit ***/


/************************************************************************************//**
** \brief     Obtains the free running counter value of a timer that runs at 20 kHz.
** \details   The Modbus RTU communication makes use of 1.5 (T1_5) and 3.5 (T3_5)
**            character timeouts. Time between packet characters should be <= T1_5 and
**            the time between packets should be >= T3_5. 
** 
**            To get these timings right a free running counter, incrementing every 50
**            microseconds, provides a time reference. This function obtains the current
**            value of this counter.
**
**            Ideally a timer is initialized to have its free running counter running at
**            20 kHz for this. The benefit of this approach is that the timer can still
**            be reused for other purposes (input capture, pwm, output compare, etc.) and
**            does not need a periodic timer interrupt.
**
**            Timers are a scarce resource on microcontrollers. Therefore it is also
**            possible to use the free running counter of a timer that runs at a
**            different frequency. Just make sure to adjust the counter value in this
**            function accordingly. For example, if you choose to reuse your RTOS'
**            1 millisecond SysTick, you need to multiply its tick counter value by 20 to
**            simulate a 20 kHz timer. This does of course have a negative impact on the
**            accuracy of the T1_5 and T3_5 timings, so there's a trade off involved.
** \return    Free running counter value.
**
****************************************************************************************/
uint16_t TbxMbPortTimerCount(void)
{
  /* TODO ##Port 
   * 
   * Read out the current value of the timer's free running counter and return it.
   * 
   * This assumes you already initialized a timer, during application initialization, to
   * have its free running counter counting upwards at 20 kHz. With other words, each
   * count of the free running counter equals 50 microseconds.
   * 
   * Theoretically you could also use a timer to generate an interrupt every 50 us. The 
   * interrupt service routine then increments a 16-bit unsigned integer counter. This
   * function would then return this counter value. However, this interrupt driven timer
   * approach would cause a high interrupt load. It is therefore better from a run-time
   * performance perspective to simply  configure your timer to increment its free
   * running counter every 50 microseconds.
   */

  return 0U;
} /*** end of TbxMbPortTimerCount ***/


/****************************************************************************************
*            I N T E R R U P T   S E R V I C E   R O U T I N E S
****************************************************************************************/
/************************************************************************************//**
** \brief     UART transmit complete / data register empty interrupt handler. Should be
**            called from your UART interrupt handler for the specified serial port.
** \param     port The serial port that generated the interrupt.
**
****************************************************************************************/
void TbxMbPortUartTxInterrupt(tTbxMbUartPort port)
{
  /* Still data left to send? */
  if (transmitInfo[port].nextIdx < transmitInfo[port].totalLen)
  {
    /* TODO ##Port 
     * 
     * - Write the next byte (transmitInfo.data[transmitInfo.nextIdx]) to the UART
     *   transmit data register.
     */

    /* Currently transmitting the last byte of the entire transfer? */
    if (transmitInfo[port].nextIdx == (transmitInfo[port].totalLen - 1U))
    {
      /* TODO ##Port 
       * 
       * - Disable the transmit data register empty (TXE) interrupt.
       * - Enable the transmit complete (TC) interrupt.
       */

    }
    /* Update the indexer to point to the next byte to transmit. */
    transmitInfo[port].nextIdx++;
  }
  /* No more data left to send. */
  else
  {
    /* TODO ##Port 
     * 
     * - Disable the transmit complete (TC) interrupt.
     * - Switch the RS485 transceiver to reception mode (DE/NRE pins), if used.
     */

    /* Inform the Modbus UART module about the transmission completed event. */
    TbxMbUartTransmitComplete(port);
  }
} /*** end of TbxMbPortUartTxInterrupt ***/


/************************************************************************************//**
** \brief     UART reception data register full interrupt handler. Should be called from
**            your UART interrupt handler for the specified serial port.
** \param     port The serial port that generated the interrupt.
**
****************************************************************************************/
void TbxMbPortUartRxInterrupt(tTbxMbUartPort port)
{
  uint8_t errorDetected = TBX_FALSE;
  uint8_t rxByte = 0U;

  /* TODO ##Port 
   * 
   * - If a parity, framing or noise error was detected during the data reception, set
   *   errorDetected = TBX_TRUE. 
   *   - Note that there is no need to set this error flag in case of a reception
   *     overrun.
   * - Clear all reception related error flags: parity, framing, noise and also 
   *   reception overrun.
   * - Read the newly received data from the UART reception data register and store it in
   *   rxByte.
   * - Clear the reception data register empty flag, if the UART peripheral does not 
   *   automatically do this after a read from the UART reception data register.
   */

  /* In case of a parity, framing or noise error, ignore the received data because it
   * will be invalid. Nothing that can be done to save or correct the data. It is
   * assumed that a higher module will be able to detect that data went missing. For
   * example due to a data stream being shorter than expected or an incorrect checksum.
   * In case of an overrun error, the data is still valid. Only the data in the
   * reception shift register gets lost.
   */
  if (errorDetected == TBX_FALSE)
  {
    /* Inform the Modbus UART module about the newly received data byte. */
    TbxMbUartDataReceived(port, &rxByte, 1U);
  }
} /*** end of TbxMbPortUartRxInterrupt ***/


/*********************************** end of tbxmb_port.c *******************************/
