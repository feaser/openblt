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
#if (BOOT_COM_CAN_ENABLE > 0)
static void BootComCanInit(void);
static void BootComCanCheckActivationRequest(void);
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
#if (BOOT_COM_CAN_ENABLE > 0)
  BootComCanInit();
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
#if (BOOT_COM_CAN_ENABLE > 0)
  BootComCanCheckActivationRequest();
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
  static unsigned char xcpCtoReqPacket[BOOT_COM_RS232_RX_MAX_DATA+3];
  static unsigned char xcpCtoRxLength;
  static unsigned char xcpCtoRxInProgress = 0;
  static unsigned long xcpCtoRxStartTime = 0;
  #if (BOOT_COM_RS232_CS_TYPE == 1)
  unsigned char  csLen = 1;
  unsigned char  csByte;
  unsigned short csIdx;
  #else
  unsigned char  csLen = 0;
  #endif
  unsigned char  csCorrect = 1;

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
      if (xcpCtoRxLength == (xcpCtoReqPacket[0] + csLen))
      {
        #if (BOOT_COM_RS232_CS_TYPE == 1)
        /* calculate the byte checksum. */
        csByte = 0;
        for (csIdx = 0; csIdx < xcpCtoRxLength; csIdx++)
        {
          csByte += xcpCtoReqPacket[csIdx];
        }
        /* verify the checksum. */
        if (csByte != xcpCtoReqPacket[xcpCtoRxLength])
        {
          /* flag incorrect checksum. */
          csCorrect = 0;
          /* cancel the packet reception due to invalid checksum. */
          xcpCtoRxInProgress = 0;
        }
        #endif

        /* only continue with a valid checksum. */
        if (csCorrect != 0)
        {
          /* subtract the checksum from the packet length. */
          xcpCtoRxLength -= csLen;
          /* done with cto packet reception */
          xcpCtoRxInProgress = 0;
          /* check if this was an XCP CONNECT command */
          if ((xcpCtoReqPacket[1] == 0xff) && (xcpCtoRxLength == 2))
          {
            /* connection request received so start the bootloader */
            BootActivate();
          }
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


#if (BOOT_COM_CAN_ENABLE > 0)
/****************************************************************************************
*        C O N T R O L L E R   A R E A   N E T W O R K   I N T E R F A C E
****************************************************************************************/

/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief CAN node data structure. */
static IfxCan_Can_Node canNode;


/************************************************************************************//**
** \brief     Initializes the CAN communication interface.
** \return    none.
**
****************************************************************************************/
static void BootComCanInit(void)
{
  IfxCan_Can_Config canConfig;
  IfxCan_Can canModule;
  IfxCan_Can_NodeConfig canNodeConfig;
  IfxCan_Can_Pins canPins;
  IfxCan_Filter canFilter;
  uint32 canRxMsgId = BOOT_COM_CAN_RX_MSG_ID & (~0x80000000);
  boolean canRxMsgIdExtended = (BOOT_COM_CAN_RX_MSG_ID & 0x80000000) ? TRUE : FALSE;

  /* configure the STBY GPIO pin P20.6 as a digital output. */
  IfxPort_setPinModeOutput(&MODULE_P20, 6U, IfxPort_OutputMode_pushPull,
                                            IfxPort_OutputIdx_general);
  /* switch the CAN transceiver to normal mode by setting the STBY GPIO pin logic low. */
  IfxPort_setPinLow(&MODULE_P20, 6U);
  /* prepare CAN Rx and Tx pin configuration. */
  canPins.rxPin = &IfxCan_RXD00B_P20_7_IN;
  canPins.rxPinMode = IfxPort_InputMode_pullUp;
  canPins.txPin = &IfxCan_TXD00_P20_8_OUT;
  canPins.txPinMode = IfxPort_OutputMode_pushPull;
  canPins.padDriver = IfxPort_PadDriver_cmosAutomotiveSpeed2;
  /* initialize the CAN module configuration structure and the module itself to enable
   * it.
   */
  IfxCan_Can_initModuleConfig(&canConfig, &MODULE_CAN0);
  IfxCan_Can_initModule(&canModule, &canConfig);
  /* initialize the CAN node configuration structure. */
  IfxCan_Can_initNodeConfig(&canNodeConfig, &canModule);
  /* prepare CAN node configuration. note that this CAN driver just needs to receive
   * one specific message with either a 11-bit or 29-bit CAN identifier in dedicated
   * reception buffer 0, in a polling manner.
   */
  canNodeConfig.nodeId = IfxCan_NodeId_0;
  canNodeConfig.pins = &canPins;
  canNodeConfig.baudRate.baudrate = BOOT_COM_CAN_BAUDRATE;
  canNodeConfig.filterConfig.rejectRemoteFramesWithStandardId = 1;
  canNodeConfig.filterConfig.rejectRemoteFramesWithExtendedId = 1;
  canNodeConfig.filterConfig.standardFilterForNonMatchingFrames = IfxCan_NonMatchingFrame_reject;
  canNodeConfig.filterConfig.extendedFilterForNonMatchingFrames = IfxCan_NonMatchingFrame_reject;
  canNodeConfig.filterConfig.standardListSize = 1;
  canNodeConfig.filterConfig.extendedListSize = 1;
  /* configure the ID type of the message to receive. */
  if (canRxMsgIdExtended == FALSE)
  {
    canNodeConfig.filterConfig.messageIdLength = IfxCan_MessageIdLength_standard;
  }
  else
  {
    canNodeConfig.filterConfig.messageIdLength = IfxCan_MessageIdLength_extended;
  }
  /* initialize the CAN node. */
  IfxCan_Can_initNode(&canNode, &canNodeConfig);
  /* prepare reception acceptance filter 0 configuration for the dedicated reception
   * buffer 0.
   */
  canFilter.number = 0;
  canFilter.elementConfiguration = IfxCan_FilterElementConfiguration_storeInRxBuffer;
  canFilter.rxBufferOffset = IfxCan_RxBufferId_0;
  canFilter.id1 = canRxMsgId;
  /* does the reception message have an 11-bit standard identifier? */
  if (canRxMsgIdExtended == FALSE)
  {
    /* assign the filter to the node. */
    IfxCan_Can_setStandardFilter(&canNode, &canFilter);
  }
  /* it has a 29-bit extended identifier. */
  else
  {
    /* assign the filter to the node. */
    IfxCan_Can_setExtendedFilter(&canNode, &canFilter);
  }
} /*** end of BootComCanInit ***/


/************************************************************************************//**
** \brief     Receives the CONNECT request from the host, which indicates that the
**            bootloader should be activated and, if so, activates it.
** \return    none.
**
****************************************************************************************/
static void BootComCanCheckActivationRequest(void)
{
  IfxCan_Message rxMsg;
  uint32 rxMsgData[2];
  uint8 rxMsgLen;
  uint8 rxByte0;

  /* was the expected CAN message received in the dedicated reception buffer 0? */
  if (IfxCan_Can_isNewDataReceived(&canNode, IfxCan_RxBufferId_0) > 0)
  {
    /* initialize the message structure. */
    IfxCan_Can_initMessage(&rxMsg);
    /* read out the message and its data. note that the reception acceptance filter for
     * this dedicated reception buffer 0 is already configured to only receive the one
     * message with identfier BOOT_COM_CAN_RX_MSG_ID and correct type (STD/EXT), so there
     * is no need to double check the identifier here.
     */
    IfxCan_Can_readMessage(&canNode, &rxMsg, rxMsgData);
    /* read the message length. */
    rxMsgLen = (uint8)rxMsg.dataLengthCode;
    /* read out the first data byte. */
    rxByte0 = (unsigned char)(rxMsgData[0] & 0x000000FF);
    /* check if this was an XCP CONNECT command. */
    if ((rxByte0 == 0xff) && (rxMsgLen == 2))
    {
      /* connection request received so start the bootloader */
      BootActivate();
    }
  }
} /*** end of BootComCanCheckActivationRequest ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of boot.c *************************************/
