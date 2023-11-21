/************************************************************************************//**
* \file         Source/TRICORE_TC2/can.c
* \brief        Bootloader CAN communication interface source file.
* \ingroup      Target_TRICORE_TC2
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2023  by Feaser    http://www.feaser.com    All rights reserved
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
#if (BOOT_COM_CAN_ENABLE > 0)
#include "IfxMultican.h"                         /* MultiCAN basic driver              */
#include "IfxScuCcu.h"                           /* SCU basic driver                   */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout for CAN module initialization operations in milliseconds. */
#define CAN_INIT_TIMEOUT_MS            (250U)

/** \brief Timeout for transmitting a CAN message in milliseconds. */
#define CAN_MSG_TX_TIMEOUT_MS          (50u)

/** \brief CAN node message object for transmitting messages. */
#define CAN_MSG_TX_OBJECT_ID          ((IfxMultican_MsgObjId)0)

/** \brief CAN node message object for receiving messages. */
#define CAN_MSG_RX_OBJECT_ID          ((IfxMultican_MsgObjId)1)

/* map the configured CAN channel index to the TriCore's MultiCAN module and node. */
#if (BOOT_COM_CAN_CHANNEL_INDEX == 0)
#define MULTICAN_MODULE   (&MODULE_CAN)
#define MULTICAN_NODE     (&MODULE_CAN.N[IfxMultican_NodeId_0])
#define MULTICAN_LIST_ID  (1)
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 1)
#define MULTICAN_MODULE   (&MODULE_CAN)
#define MULTICAN_NODE     (&MODULE_CAN.N[IfxMultican_NodeId_1])
#define MULTICAN_LIST_ID  (2)
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 2)
#define MULTICAN_MODULE   (&MODULE_CAN)
#define MULTICAN_NODE     (&MODULE_CAN.N[IfxMultican_NodeId_2])
#define MULTICAN_LIST_ID  (3)
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 3)
#define MULTICAN_MODULE   (&MODULE_CAN)
#define MULTICAN_NODE     (&MODULE_CAN.N[IfxMultican_NodeId_3])
#define MULTICAN_LIST_ID  (4)
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 4)
#define MULTICAN_MODULE   (&MODULE_CANR)
#define MULTICAN_NODE     (&MODULE_CANR.N[IfxMultican_NodeId_0])
#define MULTICAN_LIST_ID  (1)
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 5)
#define MULTICAN_MODULE   (&MODULE_CANR)
#define MULTICAN_NODE     (&MODULE_CANR.N[IfxMultican_NodeId_1])
#define MULTICAN_LIST_ID  (2)
#endif


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Structure type for grouping CAN bus timing related information. */
typedef struct t_can_bus_timing
{
  blt_int8u tseg1;                                    /**< CAN time segment 1          */
  blt_int8u tseg2;                                    /**< CAN time segment 2          */
} tCanBusTiming;


/****************************************************************************************
* Global data declarations
****************************************************************************************/
/** \brief Specifies the CAN Rx pin. It is expected that the application overwrites this
 *         value with the correct one, before BootInit() is called.
 */
IfxMultican_Rxd_In * canRxPin = NULL_PTR;

/** \brief Specifies the CAN Tx pin. It is expected that the application overwrites this
 *         value with the correct one, before BootInit() is called.
 */
IfxMultican_Txd_Out * canTxPin = NULL_PTR;


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief CAN bittiming table for dynamically calculating the bittiming settings.
 *  \details According to the CAN protocol 1 bit-time can be made up of between 8..25
 *           time quanta (TQ). The total TQ in a bit is SYNC + TSEG1 + TSEG2 with SYNC
 *           always being 1. The sample point is (SYNC + TSEG1) / (SYNC + TSEG1 + SEG2) *
 *           100%. This array contains possible and valid time quanta configurations with
 *           a sample point between 68..78%.
 */
static const tCanBusTiming canTiming[] =
{
  /*  TQ | TSEG1 | TSEG2 | SP  */
  /* ------------------------- */
  {  5, 2 },          /*   8 |   5   |   2   | 75% */
  {  6, 2 },          /*   9 |   6   |   2   | 78% */
  {  6, 3 },          /*  10 |   6   |   3   | 70% */
  {  7, 3 },          /*  11 |   7   |   3   | 73% */
  {  8, 3 },          /*  12 |   8   |   3   | 75% */
  {  9, 3 },          /*  13 |   9   |   3   | 77% */
  {  9, 4 },          /*  14 |   9   |   4   | 71% */
  { 10, 4 },          /*  15 |  10   |   4   | 73% */
  { 11, 4 },          /*  16 |  11   |   4   | 75% */
  { 12, 4 },          /*  17 |  12   |   4   | 76% */
  { 12, 5 },          /*  18 |  12   |   5   | 72% */
  { 13, 5 },          /*  19 |  13   |   5   | 74% */
  { 14, 5 },          /*  20 |  14   |   5   | 75% */
  { 15, 5 },          /*  21 |  15   |   5   | 76% */
  { 15, 6 },          /*  22 |  15   |   6   | 73% */
  { 16, 6 },          /*  23 |  16   |   6   | 74% */
  { 16, 7 },          /*  24 |  16   |   7   | 71% */
  { 16, 8 }           /*  25 |  16   |   8   | 68% */
};


/************************************************************************************//**
** \brief     Search algorithm to match the desired baudrate to a possible bus
**            timing configuration.
** \param     baud The desired baudrate in kbps. Valid values are 10..1000.
** \param     prescaler Pointer to where the value for the prescaler will be stored.
** \param     tseg1 Pointer to where the value for TSEG2 will be stored.
** \param     tseg2 Pointer to where the value for TSEG2 will be stored.
** \param     div8 BLT_TRUE if the extra divide-by-8 should be applied to the prescaler,
**            BTL_FALSE otherwise.
** \return    BLT_TRUE if the CAN bustiming register values were found, BLT_FALSE
**            otherwise.
**
****************************************************************************************/
static blt_bool CanGetSpeedConfig(blt_int16u baud, blt_int16u *prescaler,
                                  blt_int8u *tseg1, blt_int8u *tseg2, blt_bool * div8)
{
  blt_int8u  cnt;
  blt_int32u canClockFreqkHz;

  /* determine the clock frequency that sources the CAN controller and is used for the
   * baudrate generation. CanInit() selects as the Fosc0 source clock with a prescaler
   * of 1 for Fcan.
   */
  canClockFreqkHz = ((blt_int32u)IfxScuCcu_getOsc0Frequency()) / 1000;
  *div8 = BLT_FALSE;
  /* loop through all possible time quanta configurations to find a match */
  for (cnt=0; cnt < sizeof(canTiming)/sizeof(canTiming[0]); cnt++)
  {
    if ((canClockFreqkHz % (baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1))) == 0)
    {
      /* compute the prescaler that goes with this TQ configuration */
      *prescaler = canClockFreqkHz/(baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1));
      /* make sure the prescaler is valid */
      if ((*prescaler > 0) && (*prescaler <= 64))
      {
        /* store the bustiming configuration */
        *tseg1 = canTiming[cnt].tseg1;
        *tseg2 = canTiming[cnt].tseg2;
        /* found a good bus timing configuration */
        return BLT_TRUE;
      }
    }
  }

  /* still here so no fitting bittiming settings were found. try again but this time
   * with the extra divide-by-8 factor applied to the prescaler.
   */
  canClockFreqkHz = ((blt_int32u)IfxScuCcu_getOsc0Frequency()) / (1000 * 8);
  *div8 = BLT_TRUE;
  /* loop through all possible time quanta configurations to find a match */
  for (cnt=0; cnt < sizeof(canTiming)/sizeof(canTiming[0]); cnt++)
  {
    if ((canClockFreqkHz % (baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1))) == 0)
    {
      /* compute the prescaler that goes with this TQ configuration */
      *prescaler = canClockFreqkHz/(baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1));
      /* make sure the prescaler is valid */
      if ((*prescaler > 0) && (*prescaler <= 64))
      {
        /* store the bustiming configuration */
        *tseg1 = canTiming[cnt].tseg1;
        *tseg2 = canTiming[cnt].tseg2;
        /* found a good bus timing configuration */
        return BLT_TRUE;
      }
    }
  }

  /* could not find a good bus timing configuration */
  return BLT_FALSE;
} /*** end of CanGetSpeedConfig ***/


/************************************************************************************//**
** \brief     Initializes the CAN controller and synchronizes it to the CAN bus.
** \return    none.
**
****************************************************************************************/
void CanInit(void)
{
  blt_int16u      endInitCpuPassword;
  blt_int32u      timeout;
  blt_int8u       idx;
  blt_int16u      prescaler = 0;
  blt_int8u       tseg1 = 0, tseg2 = 0, sjw;
  blt_bool        div8 = BLT_FALSE;
  Ifx_CAN_N_BTR   nbtr;
  Ifx_CAN_MO    * msgObjPtr;
  boolean         extendedFrame;

  /* this CAN driver supports CAN channels 0 to 5. */
  ASSERT_CT((BOOT_COM_CAN_CHANNEL_INDEX == 0)  ||
            (BOOT_COM_CAN_CHANNEL_INDEX == 1)  ||
            (BOOT_COM_CAN_CHANNEL_INDEX == 2)  ||
            (BOOT_COM_CAN_CHANNEL_INDEX == 3)  ||
            (BOOT_COM_CAN_CHANNEL_INDEX == 4)  ||
            (BOOT_COM_CAN_CHANNEL_INDEX == 5));

  /* obtain bittiming configuration information. */
  if (CanGetSpeedConfig(BOOT_COM_CAN_BAUDRATE/1000, &prescaler, &tseg1, &tseg2,
                        &div8) == BLT_FALSE)
  {
    /* Incorrect configuration. The specified baudrate is not supported for the given
     * clock configuration. Verify the following settings in blt_conf.h:
     *   - BOOT_COM_CAN_BAUDRATE
     *   - BOOT_CPU_XTAL_SPEED_KHZ
     *   - BOOT_CPU_SYSTEM_SPEED_KHZ
     */
    ASSERT_RT(BLT_FALSE);
  }

  /* ---------------------- module init ---------------------------------------------- */
  /* get the current password of the WatchDog module and disable EndInit protection. */
  endInitCpuPassword = IfxScuWdt_getCpuWatchdogPassword();
  IfxScuWdt_clearCpuEndinit(endInitCpuPassword);
  /* set timeout time for enabling the CAN module. */
  timeout = TimerGet() + CAN_INIT_TIMEOUT_MS;
  /* enable the CAN module that the CAN channel belongs to and disregard sleep mode
   * request.
   */
  IfxMultican_enableModule(MULTICAN_MODULE);
  IfxMultican_disableSleepMode(MULTICAN_MODULE);
  /* wait until module is enabled with a timeout. */
  while (IfxMultican_isModuleEnabled(MULTICAN_MODULE) == FALSE)
  {
    /* keep the watchdog happy. */
    CopService();
    /* break loop upon timeout. this would indicate a hardware failure. */
    if (TimerGet() > timeout)
    {
      break;
    }
  }
  /* first disable the clock as two writes to the CLKSEL are always necessary. */
  IfxMultican_setInputClock(MULTICAN_MODULE, IfxMultican_ClockSelect_noClock);
  /* select the oscillator clock as the source because it results in an accurate baudrate
   * configuration to meet the CAN 2.0B timing requirements.
   */
  IfxMultican_setInputClock(MULTICAN_MODULE, IfxMultican_ClockSelect_fosc0);
  /* select normal divider mode with a step of 1023. this configures a prescaler
   * of 1 between the incoming CAN module clock (Fosc0) and the Fcan clock frequency.
   * Fcan is used by the CAN module for bittiming purposes. essentially, this configures:
   * Fcan = Fosc0.
   */
  IfxMultican_setFractionalDividerStepValue(MULTICAN_MODULE, 1023);
  IfxMultican_setFractionalDividerMode(MULTICAN_MODULE, 1);
  /* re-enable EndInit protection. */
  IfxScuWdt_setCpuEndinit(endInitCpuPassword);
  /* set timeout time for panel operations to complete. */
  timeout = TimerGet() + CAN_INIT_TIMEOUT_MS;
  /* wait for any pending panel operations to complete. */
  while (MULTICAN_MODULE->PANCTR.B.BUSY != 0)
  {
    /* keep the watchdog happy. */
    CopService();
    /* break loop upon timeout. this would indicate a hardware failure. */
    if (TimerGet() > timeout)
    {
      break;
    }
  }
  /* deinitialise all message object pending bits. */
  for (idx = 0; idx < 8; idx++)
  {
      IfxMultican_clearPendingMessageNotification(MULTICAN_MODULE, idx);
  }
  /* the position is simply given by the message pending number MPN. */
  IfxMultican_clearMessagePendingSeletor(MULTICAN_MODULE);
  /* all message objects contribute to the calculation of the message index. */
  IfxMultican_setMessageIndexMask(MULTICAN_MODULE, 0xFFFFFFFF);

  /* ---------------------- node init ------------------------------------------------ */
  /* configure CAN channel node control. */
  IfxMultican_Node_resetControlRegister(MULTICAN_NODE);
  IfxMultican_Node_setTransferInterrupt(MULTICAN_NODE, FALSE);
  IfxMultican_Node_setLastErrorCodeInterrupt(MULTICAN_NODE, FALSE);
  IfxMultican_Node_setAlertInterrupt(MULTICAN_NODE, FALSE);
  IfxMultican_Node_setAnalyzerMode(MULTICAN_NODE, FALSE);
  IfxMultican_Node_setLoopBackMode(MULTICAN_NODE, FALSE);
  IfxMultican_Node_activate(MULTICAN_NODE);
  /* configure the interrupt source to service request CAN channel node. */
  IfxMultican_Node_resetInterruptPointers(MULTICAN_NODE);
  IfxMultican_Node_setTransferInterruptPointer(MULTICAN_NODE, IfxMultican_SrcId_0);
  IfxMultican_Node_setLastErrorCodeInterruptPointer(MULTICAN_NODE, IfxMultican_SrcId_0);
  IfxMultican_Node_setAlertInterruptPointer(MULTICAN_NODE, IfxMultican_SrcId_0);
  IfxMultican_Node_setFrameCounterInterruptPointer(MULTICAN_NODE, IfxMultican_SrcId_0);
  IfxMultican_Node_setTimerEventInterruptPointer(MULTICAN_NODE, IfxMultican_SrcId_0);
  /* configure the CAN channel node error counters. */
  IfxMultican_Node_resetErrorCounters(MULTICAN_NODE);
  IfxMultican_Node_setReceiveErrorCounter(MULTICAN_NODE, 0);
  IfxMultican_Node_setTransmitErrorCounter(MULTICAN_NODE, 0);
  IfxMultican_Node_setErrorWarningLevel(MULTICAN_NODE, 96);
  /* configure the CAN Tx and Rx GPIO pins. */
  IfxMultican_Node_initTxPin(MULTICAN_NODE, canTxPin, IfxPort_OutputMode_pushPull,
                             IfxPort_PadDriver_cmosAutomotiveSpeed1);
  IfxMultican_Node_initRxPin(MULTICAN_NODE, canRxPin, IfxPort_InputMode_pullUp,
                             IfxPort_PadDriver_cmosAutomotiveSpeed1);
  /* configure the bittiming settings. */
  sjw = (tseg2 > 4U) ? 4U : tseg2;
  nbtr.U       = 0;
  nbtr.B.BRP   = prescaler - 1;
  nbtr.B.DIV8  = (div8 == BLT_TRUE) ? 1 : 0;
  nbtr.B.TSEG1 = tseg1 - 1;
  nbtr.B.TSEG2 = tseg2 - 1;
  nbtr.B.SJW   = sjw - 1;
  MULTICAN_NODE->BTR.U = nbtr.U;
  /* configure the framecounter. */
  IfxMultican_Node_setFrameCounterMode(MULTICAN_NODE,
                                       IfxMultican_FrameCounterMode_timeStampMode);
  IfxMultican_Node_setFrameCounterInterrupt(MULTICAN_NODE, FALSE);
  /* disable configuration for the CAN channel node. */
  IfxMultican_Node_disableConfigurationChange(MULTICAN_NODE);

  /* ---------------------- transmission message object init ------------------------- */
  /* initialize pointer to the hardware message object used for message transmission. */
  msgObjPtr = IfxMultican_MsgObj_getPointer(MULTICAN_MODULE, CAN_MSG_TX_OBJECT_ID);
  /* set STD/EXT frame type flag. */
  extendedFrame = ((BOOT_COM_CAN_TX_MSG_ID & 0x80000000) == 0) ? FALSE : TRUE;
  /* reset message object status flags. */
  IfxMultican_MsgObj_clearStatusFlag(msgObjPtr,
                                     IfxMultican_MsgObjStatusFlag_messageValid);
  IfxMultican_MsgObj_clearStatusFlag(msgObjPtr,
                                     IfxMultican_MsgObjStatusFlag_receivePending);
  IfxMultican_MsgObj_clearStatusFlag(msgObjPtr,
                                     IfxMultican_MsgObjStatusFlag_transmitPending);
  IfxMultican_MsgObj_clearStatusFlag(msgObjPtr,
                                     IfxMultican_MsgObjStatusFlag_receiveUpdating);
  IfxMultican_MsgObj_clearStatusFlag(msgObjPtr,
                                     IfxMultican_MsgObjStatusFlag_newData);
  IfxMultican_MsgObj_clearStatusFlag(msgObjPtr,
                                     IfxMultican_MsgObjStatusFlag_messageLost);
  IfxMultican_MsgObj_clearStatusFlag(msgObjPtr,
                                     IfxMultican_MsgObjStatusFlag_receiveTransmitSelected);
  IfxMultican_MsgObj_clearStatusFlag(msgObjPtr,
                                     IfxMultican_MsgObjStatusFlag_transmitRequest);
  /* append message object to the end of the node's list. */
  IfxMultican_setListCommand(MULTICAN_MODULE, 0x2, MULTICAN_LIST_ID,
                             CAN_MSG_TX_OBJECT_ID);
  /* configure the message object for transmitting messages. */
  IfxMultican_MsgObj_setStatusFlag(msgObjPtr,
                                   IfxMultican_MsgObjStatusFlag_transmitEnable0);
  IfxMultican_MsgObj_setStatusFlag(msgObjPtr,
                                   IfxMultican_MsgObjStatusFlag_transmitEnable1);
  IfxMultican_MsgObj_setStatusFlag(msgObjPtr,
                                   IfxMultican_MsgObjStatusFlag_receiveTransmitSelected);
  IfxMultican_MsgObj_setStatusFlag(msgObjPtr,
                                   IfxMultican_MsgObjStatusFlag_messageValid);
  IfxMultican_MsgObj_setStatusFlag(msgObjPtr,
                                   IfxMultican_MsgObjStatusFlag_messageDirection);
  IfxMultican_MsgObj_setMessageMode(msgObjPtr, IfxMultican_MsgObjMode_standard);
  IfxMultican_MsgObj_setDataLengthCode(msgObjPtr, 8);
  IfxMultican_MsgObj_setSingleTransmitTrial(msgObjPtr, FALSE);
  IfxMultican_MsgObj_setSingleDataTransfer(msgObjPtr, FALSE);
  IfxMultican_MsgObj_clearFifoGatewayPointers(msgObjPtr);
  IfxMultican_MsgObj_setAcceptanceMask(msgObjPtr, 0x7FFFFFFFUL, extendedFrame);
  IfxMultican_MsgObj_setMatchingId(msgObjPtr, FALSE);
  IfxMultican_MsgObj_setMessageId(msgObjPtr, (BOOT_COM_CAN_TX_MSG_ID & ~0x80000000),
                                  extendedFrame);
  IfxMultican_MsgObj_setIdentifierExtension(msgObjPtr, extendedFrame);
  IfxMultican_MsgObj_setPriorityClass(msgObjPtr, IfxMultican_Priority_CAN_ID);
  IfxMultican_MsgObj_clearDataRegisters(msgObjPtr);
  IfxMultican_MsgObj_setReceiveInterruptNodePointer(msgObjPtr, IfxMultican_SrcId_0);
  IfxMultican_MsgObj_setTransmitInterruptNodePointer(msgObjPtr, IfxMultican_SrcId_0);
  IfxMultican_MsgObj_setMessagePendingNumber(msgObjPtr, CAN_MSG_TX_OBJECT_ID);

  /* ---------------------- reception message object init ---------------------------- */
  /* initialize pointer to the hardware message object used for message reception. */
  msgObjPtr = IfxMultican_MsgObj_getPointer(MULTICAN_MODULE, CAN_MSG_RX_OBJECT_ID);
  /* set STD/EXT frame type flag. */
  extendedFrame = ((BOOT_COM_CAN_RX_MSG_ID & 0x80000000) == 0) ? FALSE : TRUE;
  /* reset message object status flags. */
  IfxMultican_MsgObj_clearStatusFlag(msgObjPtr,
                                     IfxMultican_MsgObjStatusFlag_messageValid);
  IfxMultican_MsgObj_clearStatusFlag(msgObjPtr,
                                     IfxMultican_MsgObjStatusFlag_receivePending);
  IfxMultican_MsgObj_clearStatusFlag(msgObjPtr,
                                     IfxMultican_MsgObjStatusFlag_transmitPending);
  IfxMultican_MsgObj_clearStatusFlag(msgObjPtr,
                                     IfxMultican_MsgObjStatusFlag_receiveUpdating);
  IfxMultican_MsgObj_clearStatusFlag(msgObjPtr,
                                     IfxMultican_MsgObjStatusFlag_newData);
  IfxMultican_MsgObj_clearStatusFlag(msgObjPtr,
                                     IfxMultican_MsgObjStatusFlag_messageLost);
  IfxMultican_MsgObj_clearStatusFlag(msgObjPtr,
                                     IfxMultican_MsgObjStatusFlag_receiveTransmitSelected);
  IfxMultican_MsgObj_clearStatusFlag(msgObjPtr,
                                     IfxMultican_MsgObjStatusFlag_transmitRequest);
  /* append message object to the end of the node's list. */
  IfxMultican_setListCommand(MULTICAN_MODULE, 0x2, MULTICAN_LIST_ID,
                             CAN_MSG_RX_OBJECT_ID);
  /* configure the message object for receiving messages. */
  IfxMultican_MsgObj_setStatusFlag(msgObjPtr,
                                   IfxMultican_MsgObjStatusFlag_receiveEnable);
  IfxMultican_MsgObj_clearStatusFlag(msgObjPtr,
                                     IfxMultican_MsgObjStatusFlag_messageDirection);
  IfxMultican_MsgObj_setMessageMode(msgObjPtr, IfxMultican_MsgObjMode_standard);
  IfxMultican_MsgObj_setSingleTransmitTrial(msgObjPtr, FALSE);
  IfxMultican_MsgObj_setSingleDataTransfer(msgObjPtr, FALSE);
  IfxMultican_MsgObj_clearFifoGatewayPointers(msgObjPtr);
  IfxMultican_MsgObj_setAcceptanceMask(msgObjPtr, 0x7FFFFFFFUL, extendedFrame);
  IfxMultican_MsgObj_setMatchingId(msgObjPtr, FALSE);
  IfxMultican_MsgObj_setMessageId(msgObjPtr, (BOOT_COM_CAN_RX_MSG_ID & ~0x80000000),
                                  extendedFrame);
  IfxMultican_MsgObj_setIdentifierExtension(msgObjPtr, extendedFrame);
  IfxMultican_MsgObj_setPriorityClass(msgObjPtr, IfxMultican_Priority_CAN_ID);
  IfxMultican_MsgObj_clearDataRegisters(msgObjPtr);
  IfxMultican_MsgObj_setReceiveInterruptNodePointer(msgObjPtr, IfxMultican_SrcId_0);
  IfxMultican_MsgObj_setTransmitInterruptNodePointer(msgObjPtr, IfxMultican_SrcId_0);
  IfxMultican_MsgObj_setMessagePendingNumber(msgObjPtr, CAN_MSG_RX_OBJECT_ID);
  IfxMultican_MsgObj_setStatusFlag(msgObjPtr,
                                   IfxMultican_MsgObjStatusFlag_receiveTransmitSelected);
  IfxMultican_MsgObj_setStatusFlag(msgObjPtr, IfxMultican_MsgObjStatusFlag_messageValid);
} /*** end of CanInit ***/


/************************************************************************************//**
** \brief     Transmits a packet formatted for the communication interface.
** \param     data Pointer to byte array with data that it to be transmitted.
** \param     len  Number of bytes that are to be transmitted.
** \return    none.
**
****************************************************************************************/
void CanTransmitPacket(blt_int8u *data, blt_int8u len)
{
  blt_int32u            timeout;
  IfxMultican_Message   txMsg;
  Ifx_CAN_MO          * txMsgObjPtr;

  /* initialization of the transmit message. note that the configuration of the STD/EXT
   * CAN identifier type was already made when configuring the transmist hardware
   * message object during this CAN driver's initialization.
   */
  txMsg.data[0]  = data[0];
  txMsg.data[0] |= data[1] << 8;
  txMsg.data[0] |= data[2] << 16;
  txMsg.data[0] |= data[3] << 24;
  txMsg.data[1]  = data[4];
  txMsg.data[1] |= data[5] << 8;
  txMsg.data[1] |= data[6] << 16;
  txMsg.data[1] |= data[7] << 24;
  txMsg.id = (BOOT_COM_CAN_TX_MSG_ID & ~0x80000000);
  txMsg.lengthCode = len;
  txMsg.fastBitRate = FALSE;
  /* initialize pointer to the hardware message object used for message transmission. */
  txMsgObjPtr = IfxMultican_MsgObj_getPointer(MULTICAN_MODULE, CAN_MSG_TX_OBJECT_ID);
  /* submit the CAN message for transmission using the hardware message object. */
  (void)IfxMultican_MsgObj_sendMessage(txMsgObjPtr, &txMsg);
  /* determine timeout time for the transmit completion. */
  timeout = TimerGet() + CAN_MSG_TX_TIMEOUT_MS;
  /* poll for completion of the transmit operation. */
  while (IfxMultican_MsgObj_isTransmitRequested(txMsgObjPtr) == TRUE)
  {
    /* service the watchdog. */
    CopService();
    /* break loop upon timeout. this would indicate a hardware failure or no other
     * nodes connected to the bus.
     */
    if (TimerGet() > timeout)
    {
      break;
    }
  }
} /*** end of CanTransmitPacket ***/


/************************************************************************************//**
** \brief     Receives a communication interface packet if one is present.
** \param     data Pointer to byte array where the data is to be stored.
** \param     len Pointer where the length of the packet is to be stored.
** \return    BLT_TRUE is a packet was received, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool CanReceivePacket(blt_int8u *data, blt_int8u *len)
{
  blt_bool              result = BLT_FALSE;
  IfxMultican_Message   rxMsg;
  Ifx_CAN_MO          * rxMsgObjPtr;

  /* initialize pointer to the hardware message object used for message reception. */
  rxMsgObjPtr = IfxMultican_MsgObj_getPointer(MULTICAN_MODULE, CAN_MSG_RX_OBJECT_ID);

  /* clear pending flag. */
  IfxMultican_MsgObj_clearRxPending(rxMsgObjPtr);

  /* new CAN message with identifier BOOT_COM_CAN_RX_MSG_ID received? */
  if (IfxMultican_MsgObj_readMessage(rxMsgObjPtr, &rxMsg) == IfxMultican_Status_newData)
  {
    /* store the data length. */
    *len = (blt_int8u)rxMsg.lengthCode;
    /* store the data. */
    data[0] = (blt_int8u) rxMsg.data[0];
    data[1] = (blt_int8u)(rxMsg.data[0] >> 8);
    data[2] = (blt_int8u)(rxMsg.data[0] >> 16);
    data[3] = (blt_int8u)(rxMsg.data[0] >> 24);
    data[4] = (blt_int8u) rxMsg.data[1];
    data[5] = (blt_int8u)(rxMsg.data[1] >> 8);
    data[6] = (blt_int8u)(rxMsg.data[1] >> 16);
    data[7] = (blt_int8u)(rxMsg.data[1] >> 24);
    /* update the result to indicate that a new packet was received. */
    result = BLT_TRUE;
  }
  /* give the result back to the caller. */
  return result;
} /*** end of CanReceivePacket ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of can.c **************************************/
