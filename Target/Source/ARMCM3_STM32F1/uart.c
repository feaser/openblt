/************************************************************************************//**
* \file         Source\ARMCM3_STM32F1\uart.c
* \brief        Bootloader UART communication interface source file.
* \ingroup      Target_ARMCM3_STM32F1
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
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


#if (BOOT_COM_UART_ENABLE > 0)
/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief UART register layout. */
typedef struct
{
  volatile blt_int16u SR;                           /**< status register               */
  blt_int16u          RESERVED0;
  volatile blt_int16u DR;                           /**< data register                 */
  blt_int16u          RESERVED1;
  volatile blt_int16u BRR;                          /**< baudrate register             */
  blt_int16u          RESERVED2;
  volatile blt_int16u CR1;                          /**< control register 1            */
  blt_int16u          RESERVED3;
  volatile blt_int16u CR2;                          /**< control register 2            */
  blt_int16u          RESERVED4;
  volatile blt_int16u CR3;                          /**< control register 3            */
  blt_int16u          RESERVED5;
  volatile blt_int16u GTPR;                         /**< guard time and prescale reg.  */
  blt_int16u          RESERVED6;
} tUartRegs;                                        /**< UART register layout type     */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout time for the reception of a CTO packet. The timer is started upon
 *         reception of the first packet byte.
 */
#define UART_CTO_RX_PACKET_TIMEOUT_MS (100u)
/** \brief Timeout for transmitting a byte in milliseconds. */
#define UART_BYTE_TX_TIMEOUT_MS       (10u)
/** \brief USART enable bit. */
#define UART_BIT_UE    ((blt_int16u)0x2000)
/** \brief Transmitter enable bit. */
#define UART_BIT_TE    ((blt_int16u)0x0008)
/** \brief Receiver enable bit. */
#define UART_BIT_RE    ((blt_int16u)0x0004)
/** \brief Transmit data reg. empty bit. */
#define UART_BIT_TXE   ((blt_int16u)0x0080)
/** \brief Read data reg. not empty bit. */
#define UART_BIT_RXNE  ((blt_int16u)0x0020)


/****************************************************************************************
* Register definitions
****************************************************************************************/
#if (BOOT_COM_UART_CHANNEL_INDEX == 0)
/** \brief Set UART base address to USART1. */
#define UARTx          ((tUartRegs *) (blt_int32u)0x40013800)
#elif (BOOT_COM_UART_CHANNEL_INDEX == 1)
/** \brief Set UART base address to USART2. */
#define UARTx          ((tUartRegs *) (blt_int32u)0x40004400)
#else
/** \brief Set UART base address to USART1 by default. */
#define UARTx          ((tUartRegs *) (blt_int32u)0x40013800)
#endif


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static blt_bool UartReceiveByte(blt_int8u *data);
static blt_bool UartTransmitByte(blt_int8u data);


/************************************************************************************//**
** \brief     Initializes the UART communication interface.
** \return    none.
**
****************************************************************************************/
void UartInit(void)
{
  /* the current implementation supports USART1 and USART2. throw an assertion error in
   * case a different UART channel is configured.
   */
  ASSERT_CT((BOOT_COM_UART_CHANNEL_INDEX == 0) || (BOOT_COM_UART_CHANNEL_INDEX == 1));
  /* first reset the UART configuration. note that this already configures the UART
   * for 1 stopbit, 8 databits and no parity.
   */
  UARTx->BRR = 0;
  UARTx->CR1 = 0;
  UARTx->CR2 = 0;
  UARTx->CR3 = 0;
  UARTx->GTPR = 0;
  /* configure the baudrate, knowing that PCLKx is configured to be half of
   * BOOT_CPU_SYSTEM_SPEED_KHZ.
   */
  UARTx->BRR = ((BOOT_CPU_SYSTEM_SPEED_KHZ/2)*(blt_int32u)1000)/BOOT_COM_UART_BAUDRATE;
  /* enable the UART including the transmitter and the receiver */
  UARTx->CR1 |= (UART_BIT_UE | UART_BIT_TE | UART_BIT_RE);
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
  blt_bool result;

  /* verify validity of the len-paramenter */
  ASSERT_RT(len <= BOOT_COM_UART_TX_MAX_DATA);

  /* first transmit the length of the packet */
  result = UartTransmitByte(len);
  ASSERT_RT(result == BLT_TRUE);

  /* transmit all the packet bytes one-by-one */
  for (data_index = 0; data_index < len; data_index++)
  {
    /* keep the watchdog happy */
    CopService();
    /* write byte */
    result = UartTransmitByte(data[data_index]);
    ASSERT_RT(result == BLT_TRUE);
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
  /* check if a new byte was received by means of the RDR-bit */
  if ((UARTx->SR & UART_BIT_RXNE) != 0)
  {
    /* store the received byte */
    data[0] = UARTx->DR;
    /* inform caller of the newly received byte */
    return BLT_TRUE;
  }
  /* inform caller that no new data was received */
  return BLT_FALSE;
} /*** end of UartReceiveByte ***/


/************************************************************************************//**
** \brief     Transmits a communication interface byte.
** \param     data Value of byte that is to be transmitted.
** \return    BLT_TRUE if the byte was transmitted, BLT_FALSE otherwise.
**
****************************************************************************************/
static blt_bool UartTransmitByte(blt_int8u data)
{
  blt_int32u timeout;
  blt_bool result = BLT_TRUE;

  /* check if tx holding register can accept new data */
  if ((UARTx->SR & UART_BIT_TXE) == 0)
  {
    /* UART not ready. should not happen */
    return BLT_FALSE;
  }
  /* write byte to transmit holding register */
  UARTx->DR = data;
  /* set timeout time to wait for transmit completion. */
  timeout = TimerGet() + UART_BYTE_TX_TIMEOUT_MS;
  /* wait for tx holding register to be empty */
  while ((UARTx->SR & UART_BIT_TXE) == 0)
  {
    /* keep the watchdog happy */
    CopService();
    /* break loop upon timeout. this would indicate a hardware failure. */
    if (TimerGet() > timeout)
    {
      result = BLT_FALSE;
      break;
    }
  }
  /* give the result back to the caller */
  return result;
} /*** end of UartTransmitByte ***/
#endif /* BOOT_COM_UART_ENABLE > 0 */


/*********************************** end of uart.c *************************************/
