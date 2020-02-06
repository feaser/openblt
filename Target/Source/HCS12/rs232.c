/************************************************************************************//**
* \file         Source/HCS12/rs232.c
* \brief        Bootloader RS232 communication interface source file.
* \ingroup      Target_HCS12
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
/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Structure type with the layout of the UART related control registers. */
typedef volatile struct
{
  volatile blt_int8u  scibdh;            /**< baudrate control register [SBR 12..8]    */
  volatile blt_int8u  scibdl;            /**< baudrate control register [SBR  8..0]    */
  volatile blt_int8u  scicr1;            /**< control register 1                       */
  volatile blt_int8u  scicr2;            /**< control register 2                       */
  volatile blt_int8u  scisr1;            /**< status regsiter 1                        */
  volatile blt_int8u  scisr2;            /**< status register 2                        */
  volatile blt_int8u  scidrh;            /**< data register high (for ninth bit)       */
  volatile blt_int8u  scidrl;            /**< data regsiter low                        */
} tUartRegs;                             /**< sci related registers                    */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout time for the reception of a CTO packet. The timer is started upon
 *         reception of the first packet byte.
 */
#define RS232_CTO_RX_PACKET_TIMEOUT_MS (100u)
/** \brief Timeout for transmitting a byte in milliseconds. */
#define RS232_BYTE_TX_TIMEOUT_MS       (10u)

#if (BOOT_COM_RS232_CHANNEL_INDEX == 0)
/** \brief Set UART base address to SCI0. */
#define UART_REGS_BASE_ADDRESS  (0x00c8)
#elif (BOOT_COM_RS232_CHANNEL_INDEX == 1)
/** \brief Set UART base address to SCI1. */
#define UART_REGS_BASE_ADDRESS  (0x00d0)
#endif
/** \brief Macro for accessing the UART related control registers. */
#define UART                    ((volatile tUartRegs *)UART_REGS_BASE_ADDRESS)


/****************************************************************************************
* Register definitions
****************************************************************************************/
/** \brief SCICR2 - transmitter enable bit. */
#define TE_BIT     (0x08)
/** \brief SCICR2 - receiver enable bit. */
#define RE_BIT     (0x04)
/** \brief SCISR1 - receiver data register full bit. */
#define RDRF_BIT   (0x20)
/** \brief SCISR1 - transmit data register empty bit. */
#define TDRE_BIT   (0x80)


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
  blt_int16u baudrate_sbr0_12;

  /* the current implementation supports SCI0 and SCI1. throw an assertion error in
   * case a different UART channel is configured.
   */
  ASSERT_CT((BOOT_COM_RS232_CHANNEL_INDEX == 0) || (BOOT_COM_RS232_CHANNEL_INDEX == 1));
  /* reset the SCI subsystem's configuration, which automatically configures it for
   * 8,n,1 communication mode.
   */
  UART->scicr2 = 0;
  UART->scicr1 = 0;
  UART->scibdh = 0;
  UART->scibdl = 0;
  /* configure the baudrate from BOOT_COM_RS232_BAUDRATE */
  baudrate_sbr0_12 = (BOOT_CPU_SYSTEM_SPEED_KHZ * 1000ul) / 16 / BOOT_COM_RS232_BAUDRATE;
  /* baudrate register value cannot be more than 13 bits */
  ASSERT_RT((baudrate_sbr0_12 & 0xe000) == 0);
  /* write first MSB then LSB for the baudrate to latch */
  UART->scibdh = (blt_int8u)(baudrate_sbr0_12 >> 8);
  UART->scibdl = (blt_int8u)baudrate_sbr0_12;
  /* enable the transmitted and receiver */
  UART->scicr2 |= (TE_BIT | RE_BIT);
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

  /* verify validity of the len-paramenter */
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
  /* check if a new byte was received by means of the RDRF-bit */
  if ((UART->scisr1 & RDRF_BIT) != 0)
  {
    /* store the received byte */
    data[0] = UART->scidrl;
    /* inform caller of the newly received byte */
    return BLT_TRUE;
  }
  /* inform caller that no new data was received */
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

  /* check if tx holding register can accept new data */
  if ((UART->scisr1 & TDRE_BIT) == 0)
  {
    /* UART not ready. should not happen */
    return BLT_FALSE;
  }
  /* write byte to transmit holding register */
  UART->scidrl = data;
  /* set timeout time to wait for transmit completion. */
  timeout = TimerGet() + RS232_BYTE_TX_TIMEOUT_MS;
  /* wait for tx holding register to be empty */
  while ((UART->scisr1 & TDRE_BIT) == 0)
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
} /*** end of Rs232TransmitByte ***/
#endif /* BOOT_COM_RS232_ENABLE > 0 */


/*********************************** end of rs232.c ************************************/
