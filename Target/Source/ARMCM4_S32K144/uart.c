/************************************************************************************//**
* \file         Source/_template/uart.c
* \brief        Bootloader UART communication interface source file.
* \ingroup      Target__template_uart
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2019  by Feaser    http://www.feaser.com    All rights reserved
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

/************************************************************************************//**
* \defgroup   Target__template_uart RS232 UART driver of a port
* \brief      This module implements the RS232 UART driver of a microcontroller port. 
* \details    For the most parts, this driver is already implemented. The only parts that
*             need porting are the UART initialization, byte reception and byte
*             transmission.
* \ingroup    Target__template
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */
#if (BOOT_COM_UART_ENABLE > 0)
/* TODO ##Port Include microcontroller peripheral driver header files here. */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout time for the reception of a CTO packet. The timer is started upon
 *         reception of the first packet byte.
 */
#define UART_CTO_RX_PACKET_TIMEOUT_MS (100u)
/** \brief Timeout for transmitting a byte in milliseconds. */
#define UART_BYTE_TX_TIMEOUT_MS       (10u)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static blt_bool UartReceiveByte(blt_int8u *data);
static void     UartTransmitByte(blt_int8u data);


/************************************************************************************//**
** \brief     Initializes the UART communication interface.
** \return    none.
**
****************************************************************************************/
void UartInit(void)
{
  /* TODO ##Port Perform compile time assertion to check that the configured UART channel
   * is actually supported by this driver. The example is for a driver where UART
   * channels 0 - 2 are supported. 
   */
  ASSERT_CT((BOOT_COM_UART_CHANNEL_INDEX == 0) ||
            (BOOT_COM_UART_CHANNEL_INDEX == 1) ||
            (BOOT_COM_UART_CHANNEL_INDEX == 2));

  /* TODO ##Port Configure and initialize the UART peripheral for the configured UART
   * channel. The communication speed should be set to the value configured with
   * BOOT_COM_UART_BAUDRATE. Further communication settings are: 8 databits, no parity,
   * and 1 stopbit. Keep in mind that the bootloader runs in polling mode so without
   * interrupts. For this reason make sure not to configure the UART peripheral for
   * interrupt driven operation.
   */
} /*** end of UartInit ***/


/************************************************************************************//**
** \brief     Transmits a packet formatted for the communication interface.
** \param     data Pointer to byte array with data that it to be transmitted.
** \param     len  Number of bytes that are to be transmitted.
** \return    none.
**
****************************************************************************************/
void UartTransmitPacket(blt_int8u *data, blt_int8u len)
{
  blt_int16u data_index;

  /* verify validity of the len-paramenter */
  ASSERT_RT(len <= BOOT_COM_UART_TX_MAX_DATA);

  /* first transmit the length of the packet */
  UartTransmitByte(len);

  /* transmit all the packet bytes one-by-one */
  for (data_index = 0; data_index < len; data_index++)
  {
    /* keep the watchdog happy */
    CopService();
    /* write byte */
    UartTransmitByte(data[data_index]);
  }
} /*** end of UartTransmitPacket ***/


/************************************************************************************//**
** \brief     Receives a communication interface packet if one is present.
** \param     data Pointer to byte array where the data is to be stored.
** \param     len Pointer where the length of the packet is to be stored.
** \return    BLT_TRUE if a packet was received, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool UartReceivePacket(blt_int8u *data, blt_int8u *len)
{
  static blt_int8u xcpCtoReqPacket[BOOT_COM_UART_RX_MAX_DATA+1];  /* one extra for length */
  static blt_int8u xcpCtoRxLength;
  static blt_bool  xcpCtoRxInProgress = BLT_FALSE;
  static blt_int32u xcpCtoRxStartTime = 0;

  /* start of cto packet received? */
  if (xcpCtoRxInProgress == BLT_FALSE)
  {
    /* store the message length when received */
    if (UartReceiveByte(&xcpCtoReqPacket[0]) == BLT_TRUE)
    {
      if ( (xcpCtoReqPacket[0] > 0) &&
           (xcpCtoReqPacket[0] <= BOOT_COM_UART_RX_MAX_DATA) )
      {
        /* store the start time */
        xcpCtoRxStartTime = TimerGet();
        /* reset packet data count */
        xcpCtoRxLength = 0;
        /* indicate that a cto packet is being received */
        xcpCtoRxInProgress = BLT_TRUE;
      }
    }
  }
  else
  {
    /* store the next packet byte */
    if (UartReceiveByte(&xcpCtoReqPacket[xcpCtoRxLength+1]) == BLT_TRUE)
    {
      /* increment the packet data count */
      xcpCtoRxLength++;

      /* check to see if the entire packet was received */
      if (xcpCtoRxLength == xcpCtoReqPacket[0])
      {
        /* copy the packet data */
        CpuMemCopy((blt_int32u)data, (blt_int32u)&xcpCtoReqPacket[1], xcpCtoRxLength);
        /* done with cto packet reception */
        xcpCtoRxInProgress = BLT_FALSE;
        /* set the packet length */
        *len = xcpCtoRxLength;
        /* packet reception complete */
        return BLT_TRUE;
      }
    }
    else
    {
      /* check packet reception timeout */
      if (TimerGet() > (xcpCtoRxStartTime + UART_CTO_RX_PACKET_TIMEOUT_MS))
      {
        /* cancel cto packet reception due to timeout. note that that automaticaly
         * discards the already received packet bytes, allowing the host to retry.
         */
        xcpCtoRxInProgress = BLT_FALSE;
      }
    }
  }
  /* packet reception not yet complete */
  return BLT_FALSE;
} /*** end of UartReceivePacket ***/


/************************************************************************************//**
** \brief     Receives a communication interface byte if one is present.
** \param     data Pointer to byte where the data is to be stored.
** \return    BLT_TRUE if a byte was received, BLT_FALSE otherwise.
**
****************************************************************************************/
static blt_bool UartReceiveByte(blt_int8u *data)
{
  blt_bool result = BLT_FALSE;

  /* TODO ##Port Check if a new byte was received on the configured channel. This is
   * typically done by checking the reception register not empty flag. If a new byte 
   * was received, read it out and store it in '*data'. Next, clear the reception flag
   * such that a new byte can be received again. Finally, set 'result' to BLT_TRUE to
   * indicate to the caller of this function that a new byte was received and stored.
   */
  if (1 == 0)
  {
    /* retrieve and store the newly received byte */
    *data = 0;
    /* update the result */
    result = BLT_TRUE;
  }
  
  /* give the result back to the caller */
  return result;
} /*** end of UartReceiveByte ***/


/************************************************************************************//**
** \brief     Transmits a communication interface byte.
** \param     data Value of byte that is to be transmitted.
** \return    none.
**
****************************************************************************************/
static void UartTransmitByte(blt_int8u data)
{
  blt_int32u timeout;

  /* TODO ##Port Write the byte value in 'data' to the transmit register of the UART 
   * peripheral such that the transmission of the byte value is started.
   */

  /* set timeout time to wait for transmit completion. */
  timeout = TimerGet() + UART_BYTE_TX_TIMEOUT_MS;
  
  /* TODO ##Port Wait in a loop, with timeout, until the UART peripheral reports that the
   * data was successfully completed. This is typically done by reading out a transmit
   * register empty flag.
   */
  
  /* wait for tx holding register to be empty */
  while (1 == 0)
  {
    /* keep the watchdog happy */
    CopService();
    /* break loop upon timeout. this would indicate a hardware failure. */
    if (TimerGet() > timeout)
    {
      break;
    }
  }
} /*** end of UartTransmitByte ***/
#endif /* BOOT_COM_UART_ENABLE > 0 */


/*********************************** end of uart.c *************************************/
