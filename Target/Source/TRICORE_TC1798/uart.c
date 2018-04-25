/************************************************************************************//**
* \file         Source\TRICORE_TC1798\uart.c
* \brief        Bootloader UART communication interface source file.
* \ingroup      Target_TRICORE_TC1798
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2015  by Feaser    http://www.feaser.com    All rights reserved
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
#include "cpu_comp.h"                            /* compiler specific CPU definitions  */


#if (BOOT_COM_UART_ENABLE > 0)
/****************************************************************************************
* Type definitions
****************************************************************************************/
typedef struct
{
  blt_int32u      reserved0[0x1];     /* 0x0 */
  ASCn_PISEL_t    PISEL;              /* 0x4 */
  ASCn_ID_t       ID;                 /* 0x8 */
  blt_int32u      reserved3[0x1];     /* 0xc */
  ASCn_CON_t      CON;                /* 0x10 */
  ASCn_BG_t       BG;                 /* 0x14 */
  ASCn_FDV_t      FDV;                /* 0x18 */
  blt_int32u      reserved7[0x1];     /* 0x1c */
  ASCn_TBUF_t     TBUF;               /* 0x20 */
  ASCn_RBUF_t     RBUF;               /* 0x24 */
  blt_int32u      reserved10[0xa];    /* 0x28 */
  ASCn_WHBCON_t   WHBCON;             /* 0x50 */
  blt_int32u      reserved12[0x27];   /* 0x54 */
  ASCn_TSRC_t     TSRC;               /* 0xf0 */
  ASCn_RSRC_t     RSRC;               /* 0xf4 */
  ASCn_ESRC_t     ESRC;               /* 0xf8 */
  ASCn_TBSRC_t    TBSRC;              /* 0xfc */
} tUartRegs;


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
* Register definitions
****************************************************************************************/
#if (BOOT_COM_UART_CHANNEL_INDEX == 0)
/** \brief Set UART base address to ASC0. */
#define UARTx          ((tUartRegs *) (blt_int32u)0xf0000a00)
#elif (BOOT_COM_UART_CHANNEL_INDEX == 1)
/** \brief Set UART base address to ASC1. */
#define UARTx          ((tUartRegs *) (blt_int32u)0xf0000b00)
#else
/** \brief Set UART base address to ASC0 by default. */
#define UARTx          ((tUartRegs *) (blt_int32u)0xf0000a00)
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
  blt_int32u frequency, reload_value, fdv, dfreq;

  /* Compute system frequency and reload value for ASC */
  frequency = BOOT_CPU_SYSTEM_SPEED_KHZ * 1000;

  /* reload_value = fdv/512 * freq/16/baudrate -1  ==>
   * reload_value = (512*freq)/(baudrate * 512*16) - 1
   * fdv = (reload_value + 1) * (baudrate*512*16/freq)
   * reload_value = (frequency / 32) / baudrate - 1;
   */
  reload_value = (frequency / ((blt_int32u)BOOT_COM_UART_BAUDRATE * 16)) - 1;
  dfreq = frequency / (16*512);
  fdv = (reload_value + 1) * (blt_int32u)BOOT_COM_UART_BAUDRATE / dfreq;

  /* enable ASC module */
  CpuEnterInitMode();
  ASC0_CLC.bits.RMC = 1;
  ASC0_CLC.bits.DISR = 0;
  CpuLeaveInitMode();

  /* configure the ASC module for 8,n,1 */
  UARTx->CON.reg = 0;
  UARTx->BG.reg  = reload_value;
  UARTx->FDV.reg = fdv;

  UARTx->CON.bits.M = 0x01;
  UARTx->CON.bits.R = 1;
  UARTx->CON.bits.REN = 1;
  UARTx->CON.bits.FDE = 1;
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
  blt_bool result = BLT_FALSE;

  /* reception event pending? */
  if (UARTx->RSRC.bits.SRR != 0)
  {
    /* read out the newly received byte */
    *data = UARTx->RBUF.bits.RD_VALUE;
    /* reset the reception event flag */
    UARTx->RSRC.bits.CLRR = 1;
    /* set result to indicate that a new byte was received */
    result = BLT_TRUE;
  }

  /* inform caller about the result */
  return result;
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

  /* reset transmit buffer interrupt request */
  UARTx->TBSRC.bits.CLRR = 1;
  /* write byte to transmit buffer register */
  UARTx->TBUF.reg = data;
  /* set timeout time to wait for transmit completion. */
  timeout = TimerGet() + UART_BYTE_TX_TIMEOUT_MS;
  /* wait for transmit buffer register to be empty */
  while (UARTx->TBSRC.bits.SRR == 0)
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
