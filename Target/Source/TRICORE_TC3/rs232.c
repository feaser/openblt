/************************************************************************************//**
* \file         Source/TRICORE_TC3/rs232.c
* \brief        Bootloader RS232 communication interface source file.
* \ingroup      Target_TRICORE_TC3
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2022  by Feaser    http://www.feaser.com    All rights reserved
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
#include "IfxAsclin.h"                           /* ASCLIN basic driver                */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout time for the reception of a CTO packet. The timer is started upon
 *         reception of the first packet byte.
 */
#define RS232_CTO_RX_PACKET_TIMEOUT_MS (200u)
/** \brief Timeout for transmitting a byte in milliseconds. */
#define RS232_BYTE_TX_TIMEOUT_MS       (1000u)
/* Map the configured UART channel index to the TriCore's ASCLIN peripheral. */
#if (BOOT_COM_RS232_CHANNEL_INDEX == 0)
/** \brief Set UART base address to ASCLIN0. */
#define ASCLIN_CHANNEL   &MODULE_ASCLIN0
#elif (BOOT_COM_RS232_CHANNEL_INDEX == 1)
/** \brief Set UART base address to ASCLIN1. */
#define ASCLIN_CHANNEL   &MODULE_ASCLIN1
#elif (BOOT_COM_RS232_CHANNEL_INDEX == 2)
/** \brief Set UART base address to ASCLIN2. */
#define ASCLIN_CHANNEL   &MODULE_ASCLIN2
#elif (BOOT_COM_RS232_CHANNEL_INDEX == 3)
/** \brief Set UART base address to ASCLIN3 */
#define ASCLIN_CHANNEL   &MODULE_ASCLIN3
#elif (BOOT_COM_RS232_CHANNEL_INDEX == 4)
/** \brief Set UART base address to ASCLIN4. */
#define ASCLIN_CHANNEL   &MODULE_ASCLIN4
#elif (BOOT_COM_RS232_CHANNEL_INDEX == 5)
/** \brief Set UART base address to ASCLIN5. */
#define ASCLIN_CHANNEL   &MODULE_ASCLIN5
#elif (BOOT_COM_RS232_CHANNEL_INDEX == 6)
/** \brief Set UART base address to ASCLIN6. */
#define ASCLIN_CHANNEL   &MODULE_ASCLIN6
#elif (BOOT_COM_RS232_CHANNEL_INDEX == 7)
/** \brief Set UART base address to ASCLIN7. */
#define ASCLIN_CHANNEL   &MODULE_ASCLIN7
#elif (BOOT_COM_RS232_CHANNEL_INDEX == 8)
/** \brief Set UART base address to ASCLIN8. */
#define ASCLIN_CHANNEL   &MODULE_ASCLIN8
#elif (BOOT_COM_RS232_CHANNEL_INDEX == 9)
/** \brief Set UART base address to ASCLIN9. */
#define ASCLIN_CHANNEL   &MODULE_ASCLIN9
#elif (BOOT_COM_RS232_CHANNEL_INDEX == 10)
/** \brief Set UART base address to ASCLIN10. */
#define ASCLIN_CHANNEL   &MODULE_ASCLIN10
#elif (BOOT_COM_RS232_CHANNEL_INDEX == 11)
/** \brief Set UART base address to ASCLIN11. */
#define ASCLIN_CHANNEL   &MODULE_ASCLIN11
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
  /* The current implementation supports ASCLIN0 - ASCLIN11. Throw an assertion error in
   * case a different channel is configured.
   */
  ASSERT_CT((BOOT_COM_RS232_CHANNEL_INDEX == 0)  ||
            (BOOT_COM_RS232_CHANNEL_INDEX == 1)  ||
            (BOOT_COM_RS232_CHANNEL_INDEX == 2)  ||
            (BOOT_COM_RS232_CHANNEL_INDEX == 3)  ||
            (BOOT_COM_RS232_CHANNEL_INDEX == 4)  ||
            (BOOT_COM_RS232_CHANNEL_INDEX == 5)  ||
            (BOOT_COM_RS232_CHANNEL_INDEX == 6)  ||
            (BOOT_COM_RS232_CHANNEL_INDEX == 7)  ||
            (BOOT_COM_RS232_CHANNEL_INDEX == 8)  ||
            (BOOT_COM_RS232_CHANNEL_INDEX == 9)  ||
            (BOOT_COM_RS232_CHANNEL_INDEX == 10) ||
            (BOOT_COM_RS232_CHANNEL_INDEX == 11));

  /* Enter initialization mode. */
  IfxAsclin_setFrameMode(ASCLIN_CHANNEL, IfxAsclin_FrameMode_initialise);
  /* Temporarily enable the clock source for the baudrate configuration. */
  IfxAsclin_setClockSource(ASCLIN_CHANNEL, IfxAsclin_ClockSource_ascFastClock);
  /* Configure the baudrate generator prescaler. */
  IfxAsclin_setPrescaler(ASCLIN_CHANNEL, 1);
  /* Configure the communication speed, while using an oversampling of 16 bits and sample
   * three bits in the middle (7,8 and 9).
   */
  (void)IfxAsclin_setBitTiming(ASCLIN_CHANNEL, (float32)BOOT_COM_RS232_BAUDRATE,
                               IfxAsclin_OversamplingFactor_16,
                               IfxAsclin_SamplePointPosition_9,
                               IfxAsclin_SamplesPerBit_three);
  /* Disable the clock again for now. */
  IfxAsclin_setClockSource(ASCLIN_CHANNEL, IfxAsclin_ClockSource_noClock);
  /* Disable loopback mode. */
  IfxAsclin_enableLoopBackMode(ASCLIN_CHANNEL, FALSE);
  /* Configure shift direction. */
  IfxAsclin_setShiftDirection(ASCLIN_CHANNEL, IfxAsclin_ShiftDirection_lsbFirst);
  /* Disable idle delay. */
  IfxAsclin_setIdleDelay(ASCLIN_CHANNEL, IfxAsclin_IdleDelay_0);
  /* Configure 8,N,1 format. */
  IfxAsclin_enableParity(ASCLIN_CHANNEL, FALSE);
  IfxAsclin_setStopBit(ASCLIN_CHANNEL, IfxAsclin_StopBit_1);
  IfxAsclin_setDataLength(ASCLIN_CHANNEL, IfxAsclin_DataLength_8);
  /* Configure transmit FIFO. Use the "Combined Mode", 8-bit wide write and generate a
   * refill event when the filling level falls to 15 or below.
   */
  IfxAsclin_setTxFifoInletWidth(ASCLIN_CHANNEL, IfxAsclin_TxFifoInletWidth_1);
  IfxAsclin_setTxFifoInterruptLevel(ASCLIN_CHANNEL, IfxAsclin_TxFifoInterruptLevel_15);
  IfxAsclin_setTxFifoInterruptMode(ASCLIN_CHANNEL, IfxAsclin_FifoInterruptMode_combined);
  /* Configure the receive FIFO. Use the "Combined Mode", 8-bit wide read and generate
   * a drain event when the filling level rises to 1 or above.
   */
  IfxAsclin_setRxFifoOutletWidth(ASCLIN_CHANNEL, IfxAsclin_RxFifoOutletWidth_1);
  IfxAsclin_setRxFifoInterruptLevel(ASCLIN_CHANNEL, IfxAsclin_RxFifoInterruptLevel_1);
  IfxAsclin_setRxFifoInterruptMode(ASCLIN_CHANNEL, IfxAsclin_FifoInterruptMode_combined);
  /* Leave initialization mode and switch to ASC mode. */
  IfxAsclin_setFrameMode(ASCLIN_CHANNEL, IfxAsclin_FrameMode_asc);
  /* Enable the clock source. */
  IfxAsclin_setClockSource(ASCLIN_CHANNEL, IfxAsclin_ClockSource_ascFastClock);
  /* Disable and clear all event flags. */
  IfxAsclin_disableAllFlags(ASCLIN_CHANNEL);
  IfxAsclin_clearAllFlags(ASCLIN_CHANNEL);
  /* Enable the transmit and receive FIFOs. */
  IfxAsclin_enableRxFifoInlet(ASCLIN_CHANNEL, TRUE);
  IfxAsclin_enableTxFifoOutlet(ASCLIN_CHANNEL, TRUE);
  /* Flush the FIFOs. */
  IfxAsclin_flushRxFifo(ASCLIN_CHANNEL);
  IfxAsclin_flushTxFifo(ASCLIN_CHANNEL);
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
  blt_bool result = BLT_FALSE;

  /* Poll the receive FIFO level to see if new data is available. */
  if (IfxAsclin_getRxFifoFillLevel(ASCLIN_CHANNEL) > 0)
  {
    /* Retrieve and store the newly received byte */
    (void)IfxAsclin_read8(ASCLIN_CHANNEL, data, 1);
    /* Update the result */
    result = BLT_TRUE;
  }
  /* Give the result back to the caller */
  return result;
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

  /* Write the data value to the ASCLIN peripheral's transmit FIFO. */
  (void)IfxAsclin_write8(ASCLIN_CHANNEL, &data, 1);

  /* Set timeout time to wait for transmit completion. */
  timeout = TimerGet() + RS232_BYTE_TX_TIMEOUT_MS;
  
  /* Wait for the transmit FIFO to be empty. */
  while (IfxAsclin_getTxFifoFillLevel(ASCLIN_CHANNEL) != 0)
  {
    /* Keep the watchdog happy */
    CopService();
    /* Break loop upon timeout. This would indicate a hardware failure. */
    if (TimerGet() > timeout)
    {
      break;
    }
  }
} /*** end of Rs232TransmitByte ***/
#endif /* BOOT_COM_RS232_ENABLE > 0 */


/*********************************** end of rs232.c ************************************/
