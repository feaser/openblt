/************************************************************************************//**
* \file         Source/ARMCM4_STM32F4/rs232.c
* \brief        Bootloader RS232 communication interface source file.
* \ingroup      Target_ARMCM4_STM32F4
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2013  by Feaser    http://www.feaser.com    All rights reserved
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
#include "boot.h"                                /* bootloader generic header          */
#if (BOOT_COM_RS232_ENABLE > 0)
#include "stm32f4xx.h"                           /* STM32 CPU and HAL header           */
#include "stm32f4xx_ll_usart.h"                  /* STM32 LL USART header              */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout time for the reception of a CTO packet. The timer is started upon
 *         reception of the first packet byte.
 */
#define RS232_CTO_RX_PACKET_TIMEOUT_MS (100u)
/** \brief Timeout for transmitting a byte in milliseconds. */
#define RS232_BYTE_TX_TIMEOUT_MS       (10u)
/* map the configured UART channel index to the STM32's USART peripheral */
#if (BOOT_COM_RS232_CHANNEL_INDEX == 0)
/** \brief Set UART base address to USART1. */
#define USART_CHANNEL   USART1
#elif (BOOT_COM_RS232_CHANNEL_INDEX == 1)
/** \brief Set UART base address to USART2. */
#define USART_CHANNEL   USART2
#elif (BOOT_COM_RS232_CHANNEL_INDEX == 2)
/** \brief Set UART base address to USART3. */
#define USART_CHANNEL   USART3
#elif (BOOT_COM_RS232_CHANNEL_INDEX == 3)
/** \brief Set UART base address to USART4. */
#define USART_CHANNEL   USART4
#elif (BOOT_COM_RS232_CHANNEL_INDEX == 4)
/** \brief Set UART base address to USART5. */
#define USART_CHANNEL   USART5
#elif (BOOT_COM_RS232_CHANNEL_INDEX == 5)
/** \brief Set UART base address to USART6. */
#define USART_CHANNEL   USART6
#endif


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static blt_bool Rs232ReceiveByte(blt_int8u *data);
static void     Rs232TransmitByte(blt_int8u data);


/************************************************************************************//**
** \brief     Initializes the RS232 communication interface.
** \return    none.
**
****************************************************************************************/
void Rs232Init(void)
{
  LL_USART_InitTypeDef USART_InitStruct;

  /* the current implementation supports USART1 - USART5. throw an assertion error in
   * case a different UART channel is configured.
   */
  ASSERT_CT((BOOT_COM_RS232_CHANNEL_INDEX == 0) ||
            (BOOT_COM_RS232_CHANNEL_INDEX == 1) ||
            (BOOT_COM_RS232_CHANNEL_INDEX == 2) ||
            (BOOT_COM_RS232_CHANNEL_INDEX == 3) ||
            (BOOT_COM_RS232_CHANNEL_INDEX == 4) ||
            (BOOT_COM_RS232_CHANNEL_INDEX == 5));

  /* disable the UART peripheral */
  LL_USART_Disable(USART_CHANNEL);
  /* configure UART peripheral */
  USART_InitStruct.BaudRate = BOOT_COM_RS232_BAUDRATE;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  /* initialize the UART peripheral */
  LL_USART_Init(USART_CHANNEL, &USART_InitStruct);
  LL_USART_Enable(USART_CHANNEL);
} /*** end of Rs232Init ***/


/************************************************************************************//**
** \brief     Transmits a packet formatted for the communication interface.
** \param     data Pointer to byte array with data that it to be transmitted.
** \param     len  Number of bytes that are to be transmitted.
** \return    none.
**
****************************************************************************************/
void Rs232TransmitPacket(blt_int8u *data, blt_int8u len)
{
  blt_int16u data_index;

  /* verify validity of the len-paramenter */
  ASSERT_RT(len <= BOOT_COM_RS232_TX_MAX_DATA);

  /* first transmit the length of the packet */
  Rs232TransmitByte(len);

  /* transmit all the packet bytes one-by-one */
  for (data_index = 0; data_index < len; data_index++)
  {
    /* keep the watchdog happy */
    CopService();
    /* write byte */
    Rs232TransmitByte(data[data_index]);
  }
} /*** end of Rs232TransmitPacket ***/


/************************************************************************************//**
** \brief     Receives a communication interface packet if one is present.
** \param     data Pointer to byte array where the data is to be stored.
** \param     len Pointer where the length of the packet is to be stored.
** \return    BLT_TRUE if a packet was received, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool Rs232ReceivePacket(blt_int8u *data, blt_int8u *len)
{
  static blt_int8u xcpCtoReqPacket[BOOT_COM_RS232_RX_MAX_DATA+1];  /* one extra for length */
  static blt_int8u xcpCtoRxLength;
  static blt_bool  xcpCtoRxInProgress = BLT_FALSE;
  static blt_int32u xcpCtoRxStartTime = 0;

  /* start of cto packet received? */
  if (xcpCtoRxInProgress == BLT_FALSE)
  {
    /* store the message length when received */
    if (Rs232ReceiveByte(&xcpCtoReqPacket[0]) == BLT_TRUE)
    {
      if ( (xcpCtoReqPacket[0] > 0) &&
           (xcpCtoReqPacket[0] <= BOOT_COM_RS232_RX_MAX_DATA) )
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
    if (Rs232ReceiveByte(&xcpCtoReqPacket[xcpCtoRxLength+1]) == BLT_TRUE)
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
      if (TimerGet() > (xcpCtoRxStartTime + RS232_CTO_RX_PACKET_TIMEOUT_MS))
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
} /*** end of Rs232ReceivePacket ***/


/************************************************************************************//**
** \brief     Receives a communication interface byte if one is present.
** \param     data Pointer to byte where the data is to be stored.
** \return    BLT_TRUE if a byte was received, BLT_FALSE otherwise.
**
****************************************************************************************/
static blt_bool Rs232ReceiveByte(blt_int8u *data)
{
  if (LL_USART_IsActiveFlag_RXNE(USART_CHANNEL) != 0)
  {
    /* retrieve and store the newly received byte */
    *data = LL_USART_ReceiveData8(USART_CHANNEL);
    /* all done */
    return BLT_TRUE;
  }
  /* still here to no new byte received */
  return BLT_FALSE;
} /*** end of Rs232ReceiveByte ***/


/************************************************************************************//**
** \brief     Transmits a communication interface byte.
** \param     data Value of byte that is to be transmitted.
** \return    none.
**
****************************************************************************************/
static void Rs232TransmitByte(blt_int8u data)
{
  blt_int32u timeout;

  /* write byte to transmit holding register */
  LL_USART_TransmitData8(USART_CHANNEL, data);
  /* set timeout time to wait for transmit completion. */
  timeout = TimerGet() + RS232_BYTE_TX_TIMEOUT_MS;
  /* wait for tx holding register to be empty */
  while (LL_USART_IsActiveFlag_TXE(USART_CHANNEL) == 0)
  {
    /* keep the watchdog happy */
    CopService();
    /* break loop upon timeout. this would indicate a hardware failure. */
    if (TimerGet() > timeout)
    {
      break;
    }
  }
} /*** end of Rs232TransmitByte ***/
#endif /* BOOT_COM_RS232_ENABLE > 0 */


/*********************************** end of rs232.c ************************************/
