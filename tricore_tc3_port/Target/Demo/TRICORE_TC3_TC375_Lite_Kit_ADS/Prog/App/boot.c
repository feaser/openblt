/************************************************************************************//**
* \file         Demo/TRICORE_TC3_TC375_Lite_Kit_ADS/Prog/App/boot.c
* \brief        Demo program bootloader interface source file.
* \ingroup      Prog_TRICORE_TC3_TC375_Lite_Kit_ADS
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
#include "header.h"                                    /* generic header               */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
#if (BOOT_COM_RS232_ENABLE > 0)
static void BootComRs232Init(void);
static void BootComRs232CheckActivationRequest(void);
#endif


/************************************************************************************//**
** \brief     Initializes the communication interface.
** \return    none.
**
****************************************************************************************/
void BootComInit(void)
{
#if (BOOT_COM_RS232_ENABLE > 0)
  BootComRs232Init();
#endif
} /*** end of BootComInit ***/


/************************************************************************************//**
** \brief     Receives the CONNECT request from the host, which indicates that the
**            bootloader should be activated and, if so, activates it.
** \return    none.
**
****************************************************************************************/
void BootComCheckActivationRequest(void)
{
#if (BOOT_COM_RS232_ENABLE > 0)
  BootComRs232CheckActivationRequest();
#endif
} /*** end of BootComCheckActivationRequest ***/


/************************************************************************************//**
** \brief     Bootloader activation function.
** \return    none.
**
****************************************************************************************/
void BootActivate(void)
{
  /* trigger a system reset to activate the bootloader. */
  IfxScuRcu_performReset(IfxScuRcu_ResetType_system, 0);
} /*** end of BootActivate ***/


#if (BOOT_COM_RS232_ENABLE > 0)
/****************************************************************************************
*     U N I V E R S A L   A S Y N C H R O N O U S   R X   T X   I N T E R F A C E
****************************************************************************************/

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout time for the reception of a CTO packet. The timer is started upon
 *         reception of the first packet byte.
 */
#define RS232_CTO_RX_PACKET_TIMEOUT_MS (100u)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static unsigned char Rs232ReceiveByte(unsigned char *data);


/************************************************************************************//**
** \brief     Initializes the UART communication interface.
** \return    none.
**
****************************************************************************************/
static void BootComRs232Init(void)
{
  /* Enable the ASCLIN0 module. */
  IfxAsclin_enableModule(&MODULE_ASCLIN0);
  /* Disable the clock before configuring the GPIO pins. */
  IfxAsclin_setClockSource(&MODULE_ASCLIN0, IfxAsclin_ClockSource_noClock);
  /* Configure the ASCLIN0 GPIO pins P14.1 Rx and P14.0 Tx. */
  IfxAsclin_initRxPin(&IfxAsclin0_RXA_P14_1_IN, IfxPort_InputMode_pullUp,
                      IfxPort_PadDriver_cmosAutomotiveSpeed1);
  IfxAsclin_initTxPin(&IfxAsclin0_TX_P14_0_OUT, IfxPort_OutputMode_pushPull,
                      IfxPort_PadDriver_cmosAutomotiveSpeed1);
  /* Enter initialization mode. */
  IfxAsclin_setFrameMode(&MODULE_ASCLIN0, IfxAsclin_FrameMode_initialise);
  /* Temporarily enable the clock source for the baudrate configuration. */
  IfxAsclin_setClockSource(&MODULE_ASCLIN0, IfxAsclin_ClockSource_ascFastClock);
  /* Configure the baudrate generator prescaler. */
  IfxAsclin_setPrescaler(&MODULE_ASCLIN0, 1);
  /* Configure the communication speed, while using an oversampling of 16 bits and sample
   * three bits in the middle (7,8 and 9).
   */
  (void)IfxAsclin_setBitTiming(&MODULE_ASCLIN0, (float32)BOOT_COM_RS232_BAUDRATE,
                               IfxAsclin_OversamplingFactor_16,
                               IfxAsclin_SamplePointPosition_9,
                               IfxAsclin_SamplesPerBit_three);
  /* Disable the clock again for now. */
  IfxAsclin_setClockSource(&MODULE_ASCLIN0, IfxAsclin_ClockSource_noClock);
  /* Disable loopback mode. */
  IfxAsclin_enableLoopBackMode(&MODULE_ASCLIN0, FALSE);
  /* Configure shift direction. */
  IfxAsclin_setShiftDirection(&MODULE_ASCLIN0, IfxAsclin_ShiftDirection_lsbFirst);
  /* Disable idle delay. */
  IfxAsclin_setIdleDelay(&MODULE_ASCLIN0, IfxAsclin_IdleDelay_0);
  /* Configure 8,N,1 format. */
  IfxAsclin_enableParity(&MODULE_ASCLIN0, FALSE);
  IfxAsclin_setStopBit(&MODULE_ASCLIN0, IfxAsclin_StopBit_1);
  IfxAsclin_setDataLength(&MODULE_ASCLIN0, IfxAsclin_DataLength_8);
  /* Configure transmit FIFO. Use the "Combined Mode", 8-bit wide write and generate a
   * refill event when the filling level falls to 15 or below.
   */
  IfxAsclin_setTxFifoInletWidth(&MODULE_ASCLIN0, IfxAsclin_TxFifoInletWidth_1);
  IfxAsclin_setTxFifoInterruptLevel(&MODULE_ASCLIN0, IfxAsclin_TxFifoInterruptLevel_15);
  IfxAsclin_setTxFifoInterruptMode(&MODULE_ASCLIN0, IfxAsclin_FifoInterruptMode_combined);
  /* Configure the receive FIFO. Use the "Combined Mode", 8-bit wide read and generate
   * a drain event when the filling level rises to 1 or above.
   */
  IfxAsclin_setRxFifoOutletWidth(&MODULE_ASCLIN0, IfxAsclin_RxFifoOutletWidth_1);
  IfxAsclin_setRxFifoInterruptLevel(&MODULE_ASCLIN0, IfxAsclin_RxFifoInterruptLevel_1);
  IfxAsclin_setRxFifoInterruptMode(&MODULE_ASCLIN0, IfxAsclin_FifoInterruptMode_combined);
  /* Leave initialization mode and switch to ASC mode. */
  IfxAsclin_setFrameMode(&MODULE_ASCLIN0, IfxAsclin_FrameMode_asc);
  /* Enable the clock source. */
  IfxAsclin_setClockSource(&MODULE_ASCLIN0, IfxAsclin_ClockSource_ascFastClock);
  /* Disable and clear all event flags. */
  IfxAsclin_disableAllFlags(&MODULE_ASCLIN0);
  IfxAsclin_clearAllFlags(&MODULE_ASCLIN0);
  /* Enable the transmit and receive FIFOs. */
  IfxAsclin_enableRxFifoInlet(&MODULE_ASCLIN0, TRUE);
  IfxAsclin_enableTxFifoOutlet(&MODULE_ASCLIN0, TRUE);
  /* Flush the FIFOs. */
  IfxAsclin_flushRxFifo(&MODULE_ASCLIN0);
  IfxAsclin_flushTxFifo(&MODULE_ASCLIN0);
} /*** end of BootComRs232Init ***/


/************************************************************************************//**
** \brief     Receives the CONNECT request from the host, which indicates that the
**            bootloader should be activated and, if so, activates it.
** \return    none.
**
****************************************************************************************/
static void BootComRs232CheckActivationRequest(void)
{
  static unsigned char xcpCtoReqPacket[BOOT_COM_RS232_RX_MAX_DATA+1];
  static unsigned char xcpCtoRxLength;
  static unsigned char xcpCtoRxInProgress = 0;
  static unsigned long xcpCtoRxStartTime = 0;

  /* start of cto packet received? */
  if (xcpCtoRxInProgress == 0)
  {
    /* store the message length when received */
    if (Rs232ReceiveByte(&xcpCtoReqPacket[0]) == 1)
    {
      /* check that the length has a valid value. it should not be 0 */
      if ( (xcpCtoReqPacket[0] > 0) &&
           (xcpCtoReqPacket[0] <= BOOT_COM_RS232_RX_MAX_DATA) )
      {
        /* store the start time */
        xcpCtoRxStartTime = TimerGet();
        /* indicate that a cto packet is being received */
        xcpCtoRxInProgress = 1;
        /* reset packet data count */
        xcpCtoRxLength = 0;
      }
    }
  }
  else
  {
    /* store the next packet byte */
    if (Rs232ReceiveByte(&xcpCtoReqPacket[xcpCtoRxLength+1]) == 1)
    {
      /* increment the packet data count */
      xcpCtoRxLength++;

      /* check to see if the entire packet was received */
      if (xcpCtoRxLength == xcpCtoReqPacket[0])
      {
        /* done with cto packet reception */
        xcpCtoRxInProgress = 0;

        /* check if this was an XCP CONNECT command */
        if ((xcpCtoReqPacket[1] == 0xff) && (xcpCtoReqPacket[2] == 0x00))
        {
          /* connection request received so start the bootloader */
          BootActivate();
        }
      }
    }
    else
    {
      /* check packet reception timeout */
      if (TimerGet() > (xcpCtoRxStartTime + RS232_CTO_RX_PACKET_TIMEOUT_MS))
      {
        /* cancel cto packet reception due to timeout. note that this automatically
         * discards the already received packet bytes, allowing the host to retry.
         */
        xcpCtoRxInProgress = 0;
      }
    }
  }
} /*** end of BootComRs232CheckActivationRequest ***/


/************************************************************************************//**
** \brief     Receives a communication interface byte if one is present.
** \param     data Pointer to byte where the data is to be stored.
** \return    1 if a byte was received, 0 otherwise.
**
****************************************************************************************/
static unsigned char Rs232ReceiveByte(unsigned char *data)
{
  unsigned char result = 0;

  /* check if a new byte was received on the configured channel. */
  if (IfxAsclin_getRxFifoFillLevel(&MODULE_ASCLIN0) > 0)
  {
    /* retrieve and store the newly received byte, */
    (void)IfxAsclin_read8(&MODULE_ASCLIN0, data, 1);
    /* update the result */
    result = 1;
  }
  
  /* give the result back to the caller */
  return result;
} /*** end of Rs232ReceiveByte ***/
#endif /* BOOT_COM_RS232_ENABLE > 0 */


/*********************************** end of boot.c *************************************/
