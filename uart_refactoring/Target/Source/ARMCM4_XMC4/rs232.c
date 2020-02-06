/************************************************************************************//**
* \file         Source/ARMCM4_XMC4/rs232.c
* \brief        Bootloader RS232 communication interface source file.
* \ingroup      Target_ARMCM4_XMC4
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2016  by Feaser    http://www.feaser.com    All rights reserved
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
#include "xmc_uart.h"                            /* UART driver header                 */


#if (BOOT_COM_RS232_ENABLE > 0)
/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout time for the reception of a CTO packet. The timer is started upon
 *         reception of the first packet byte.
 */
#define RS232_CTO_RX_PACKET_TIMEOUT_MS (100u)

/** \brief Timeout for transmitting a byte in milliseconds. */
#define RS232_BYTE_TX_TIMEOUT_MS       (10u)

/** \brief Macro for accessing the UART channel handle in the format that is expected
 *         by the XMClib UART driver.
 */
#define RS232_CHANNEL ((XMC_USIC_CH_t *)(rs232ChannelMap[BOOT_COM_RS232_CHANNEL_INDEX]))


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief Helper array to quickly convert the channel index, as specific in the boot-
 *         loader's configuration header, to the associated channel handle that the
 *         XMClib's UART driver requires.
 */
static const XMC_USIC_CH_t *rs232ChannelMap[] =
{
  XMC_UART0_CH0, /* BOOT_COM_RS232_CHANNEL_INDEX = 0 */
  XMC_UART0_CH1, /* BOOT_COM_RS232_CHANNEL_INDEX = 1 */
  XMC_UART1_CH0, /* BOOT_COM_RS232_CHANNEL_INDEX = 2 */
  XMC_UART1_CH1, /* BOOT_COM_RS232_CHANNEL_INDEX = 3 */
  XMC_UART2_CH0, /* BOOT_COM_RS232_CHANNEL_INDEX = 4 */
  XMC_UART2_CH1  /* BOOT_COM_RS232_CHANNEL_INDEX = 5 */
};


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static blt_bool Rs232ReceiveByte(blt_int8u *data);
static blt_bool Rs232TransmitByte(blt_int8u data);


/************************************************************************************//**
** \brief     Initializes the RS232 communication interface.
** \return    none.
**
****************************************************************************************/
void Rs232Init(void)
{
  XMC_UART_CH_CONFIG_t uart_config;

  /* the current implementation supports XMC_UART0_CH0 to XMC_UART2_CH1. throw an
   * assertion error in case a different CAN channel is configured.
   */
  ASSERT_CT((BOOT_COM_RS232_CHANNEL_INDEX >= 0) && (BOOT_COM_RS232_CHANNEL_INDEX <= 5));

  /* set configuration and initialize UART channel */
  uart_config.baudrate = BOOT_COM_RS232_BAUDRATE;
  uart_config.data_bits = 8;
  uart_config.frame_length = 8;
  uart_config.stop_bits = 1;
  uart_config.oversampling = 16;
  uart_config.parity_mode = XMC_USIC_CH_PARITY_MODE_NONE;
  XMC_UART_CH_Init(RS232_CHANNEL, &uart_config);
  /* configure small transmit and receive FIFO */
  XMC_USIC_CH_TXFIFO_Configure(RS232_CHANNEL, 16U, XMC_USIC_CH_FIFO_SIZE_16WORDS, 1U);
  XMC_USIC_CH_RXFIFO_Configure(RS232_CHANNEL,  0U, XMC_USIC_CH_FIFO_SIZE_16WORDS, 1U);
  /* start UART */
  XMC_UART_CH_Start(RS232_CHANNEL);
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
  blt_bool result;

  /* verify validity of the len-parameter */
  ASSERT_RT(len <= BOOT_COM_RS232_TX_MAX_DATA);

  /* first transmit the length of the packet */
  result = Rs232TransmitByte(len);
  ASSERT_RT(result == BLT_TRUE);

  /* transmit all the packet bytes one-by-one */
  for (data_index = 0; data_index < len; data_index++)
  {
    /* keep the watchdog happy */
    CopService();
    /* write byte */
    result = Rs232TransmitByte(data[data_index]);
    ASSERT_RT(result == BLT_TRUE);
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
  if (XMC_USIC_CH_RXFIFO_IsEmpty(RS232_CHANNEL) == 0)
  {
    /* retrieve and store the newly received byte */
    *data = (blt_int8u)XMC_UART_CH_GetReceivedData(RS232_CHANNEL);
    /* all done */
    return BLT_TRUE;
  }
  /* still here to no new byte received */
  return BLT_FALSE;
} /*** end of Rs232ReceiveByte ***/


/************************************************************************************//**
** \brief     Transmits a communication interface byte.
** \param     data Value of byte that is to be transmitted.
** \return    BLT_TRUE if the byte was transmitted, BLT_FALSE otherwise.
**
****************************************************************************************/
static blt_bool Rs232TransmitByte(blt_int8u data)
{
  blt_int32u timeout;
  blt_bool result = BLT_TRUE;

  /* check if tx fifo can accept new data */
  if (XMC_USIC_CH_TXFIFO_IsFull(RS232_CHANNEL) != 0)
  {
    /* tx fifo full. should not happen */
    return BLT_FALSE;
  }
  /* submit data for transmission */
  XMC_UART_CH_Transmit(RS232_CHANNEL, data);
  /* set timeout time to wait for transmit completion. */
  timeout = TimerGet() + RS232_BYTE_TX_TIMEOUT_MS;
  /* wait for transmission to be done */
  while( (XMC_USIC_CH_TXFIFO_GetEvent(RS232_CHANNEL) & XMC_USIC_CH_TXFIFO_EVENT_STANDARD) == 0)
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
  /* reset event */
  XMC_USIC_CH_TXFIFO_ClearEvent(RS232_CHANNEL, XMC_USIC_CH_TXFIFO_EVENT_STANDARD);
  /* give the result back to the caller */
  return result;
} /*** end of Rs232TransmitByte ***/
#endif /* BOOT_COM_RS232_ENABLE > 0 */


/*********************************** end of rs232.c ************************************/
