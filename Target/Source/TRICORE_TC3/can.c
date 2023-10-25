/************************************************************************************//**
* \file         Source/TRICORE_TC3/can.c
* \brief        Bootloader CAN communication interface source file.
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
#if (BOOT_COM_CAN_ENABLE > 0)
#include "IfxCan.h"                              /* MSMCAN basic driver                */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout for transmitting a CAN message in milliseconds. */
#define CAN_MSG_TX_TIMEOUT_MS          (50u)

/* Map the configured CAN channel index to the TriCore's MCMCAN module and node. */
#if (BOOT_COM_CAN_CHANNEL_INDEX == 0)
#define MCMCAN_MODULE   (&MODULE_CAN0)
#define MCMCAN_NODE     (&MODULE_CAN0.N[IfxCan_NodeId_0])
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 1)
#define MCMCAN_MODULE   (&MODULE_CAN0)
#define MCMCAN_NODE     (&MODULE_CAN0.N[IfxCan_NodeId_1])
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 2)
#define MCMCAN_MODULE   (&MODULE_CAN0)
#define MCMCAN_NODE     (&MODULE_CAN0.N[IfxCan_NodeId_2])
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 3)
#define MCMCAN_MODULE   (&MODULE_CAN0)
#define MCMCAN_NODE     (&MODULE_CAN0.N[IfxCan_NodeId_3])
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 4)
#define MCMCAN_MODULE   (&MODULE_CAN1)
#define MCMCAN_NODE     (&MODULE_CAN1.N[IfxCan_NodeId_0])
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 5)
#define MCMCAN_MODULE   (&MODULE_CAN1)
#define MCMCAN_NODE     (&MODULE_CAN1.N[IfxCan_NodeId_1])
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 6)
#define MCMCAN_MODULE   (&MODULE_CAN1)
#define MCMCAN_NODE     (&MODULE_CAN1.N[IfxCan_NodeId_2])
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 7)
#define MCMCAN_MODULE   (&MODULE_CAN1)
#define MCMCAN_NODE     (&MODULE_CAN1.N[IfxCan_NodeId_3])
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 8)
#define MCMCAN_MODULE   (&MODULE_CAN2)
#define MCMCAN_NODE     (&MODULE_CAN2.N[IfxCan_NodeId_0])
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 9)
#define MCMCAN_MODULE   (&MODULE_CAN2)
#define MCMCAN_NODE     (&MODULE_CAN2.N[IfxCan_NodeId_1])
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 10)
#define MCMCAN_MODULE   (&MODULE_CAN2)
#define MCMCAN_NODE     (&MODULE_CAN2.N[IfxCan_NodeId_2])
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 11)
#define MCMCAN_MODULE   (&MODULE_CAN2)
#define MCMCAN_NODE     (&MODULE_CAN2.N[IfxCan_NodeId_3])
#endif

/** \brief Start address in message RAM for storing the message reception acceptance
 * filters for 11-bit standard identifiers.
 */
#define CAN_RX_FILTER_STD_MESSAGE_RAM_BASE_ADDR  (0x100)

/** \brief Start address in message RAM for storing the message reception acceptance
 * filters for 29-bit extended identifiers.
 */
#define CAN_RX_FILTER_EXT_MESSAGE_RAM_BASE_ADDR  (0x200)

/** \brief Start address in message RAM for storing the received messages. */
#define CAN_RX_BUFFERS_MESSAGE_RAM_BASE_ADDR     (0x300)

/** \brief Start address in message RAM for storing the transmit messages. */
#define CAN_TX_BUFFERS_MESSAGE_RAM_BASE_ADDR     (0x400)


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
IfxCan_Rxd_In * canRxPin = NULL_PTR;

/** \brief Specifies the CAN Tx pin. It is expected that the application overwrites this
 *         value with the correct one, before BootInit() is called.
 */
IfxCan_Txd_Out * canTxPin = NULL_PTR;


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

/** \brief Lookup table for converting the DLC value (0..8) into the data length code
 *         type that the driver library uses.
 */
static const IfxCan_DataLengthCode canDataLenLookup[] =
{
  IfxCan_DataLengthCode_0,
  IfxCan_DataLengthCode_1,
  IfxCan_DataLengthCode_2,
  IfxCan_DataLengthCode_3,
  IfxCan_DataLengthCode_4,
  IfxCan_DataLengthCode_5,
  IfxCan_DataLengthCode_6,
  IfxCan_DataLengthCode_7,
  IfxCan_DataLengthCode_8
};


/************************************************************************************//**
** \brief     Search algorithm to match the desired baudrate to a possible bus
**            timing configuration.
** \param     baud The desired baudrate in kbps. Valid values are 10..1000.
** \param     prescaler Pointer to where the value for the prescaler will be stored.
** \param     tseg1 Pointer to where the value for TSEG2 will be stored.
** \param     tseg2 Pointer to where the value for TSEG2 will be stored.
** \return    BLT_TRUE if the CAN bustiming register values were found, BLT_FALSE
**            otherwise.
**
****************************************************************************************/
static blt_bool CanGetSpeedConfig(blt_int16u baud, blt_int16u *prescaler,
                                  blt_int8u *tseg1, blt_int8u *tseg2)
{
  blt_int8u  cnt;
  blt_int32u canClockFreqkHz;

  /* determine the clock frequency that sources the CAN controller and is used for the
   * baudrate generation.
   */
  canClockFreqkHz = ((blt_int32u)IfxCan_getModuleFrequency()) / 1000;
  /* loop through all possible time quanta configurations to find a match */
  for (cnt=0; cnt < sizeof(canTiming)/sizeof(canTiming[0]); cnt++)
  {
    if ((canClockFreqkHz % (baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1))) == 0)
    {
      /* compute the prescaler that goes with this TQ configuration */
      *prescaler = canClockFreqkHz/(baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1));
      /* make sure the prescaler is valid */
      if ((*prescaler > 0) && (*prescaler <= 512))
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
  blt_int16u prescaler = 0;
  blt_int8u  tseg1 = 0, tseg2 = 0;
  blt_int32u rxMsgId = BOOT_COM_CAN_RX_MSG_ID;
  const IfxCan_ClockSelect clockSelectLookup[] =
  {
    IfxCan_ClockSelect_0,
    IfxCan_ClockSelect_1,
    IfxCan_ClockSelect_2,
    IfxCan_ClockSelect_3
  };

  /* this CAN driver supports CAN channels 0 to 11. */
  ASSERT_CT((BOOT_COM_CAN_CHANNEL_INDEX == 0)  ||
            (BOOT_COM_CAN_CHANNEL_INDEX == 1)  ||
            (BOOT_COM_CAN_CHANNEL_INDEX == 2)  ||
            (BOOT_COM_CAN_CHANNEL_INDEX == 3)  ||
            (BOOT_COM_CAN_CHANNEL_INDEX == 4)  ||
            (BOOT_COM_CAN_CHANNEL_INDEX == 5)  ||
            (BOOT_COM_CAN_CHANNEL_INDEX == 6)  ||
            (BOOT_COM_CAN_CHANNEL_INDEX == 7)  ||
            (BOOT_COM_CAN_CHANNEL_INDEX == 8)  ||
            (BOOT_COM_CAN_CHANNEL_INDEX == 9)  ||
            (BOOT_COM_CAN_CHANNEL_INDEX == 10) ||
            (BOOT_COM_CAN_CHANNEL_INDEX == 11));

  /* enable the CAN module. */
  IfxCan_enableModule(MCMCAN_MODULE);
  /* configure the CAN Tx and Rx GPIO pins. */
  IfxCan_Node_initTxPin(canTxPin, IfxPort_OutputMode_pushPull,
                        IfxPort_PadDriver_cmosAutomotiveSpeed1);
  IfxCan_Node_initRxPin(MCMCAN_NODE, canRxPin, IfxPort_InputMode_pullUp,
                        IfxPort_PadDriver_cmosAutomotiveSpeed1);
  /* enable both the synchronous and asynchronous clocks. The synchronous clock is used
   * as the source for the register and RAM interface of the CAN controller. The
   * asynchronous clock is used for the CAN baudrate generation.
   */
  IfxCan_setClockSource(MCMCAN_MODULE, clockSelectLookup[BOOT_COM_CAN_CHANNEL_INDEX % 4],
                        IfxCan_ClockSource_both);
  /* obtain bittiming configuration information. */
  if (CanGetSpeedConfig(BOOT_COM_CAN_BAUDRATE/1000, &prescaler, &tseg1, &tseg2) == BLT_FALSE)
  {
    /* Incorrect configuration. The specified baudrate is not supported for the given
     * clock configuration. Verify the following settings in blt_conf.h:
     *   - BOOT_COM_CAN_BAUDRATE
     *   - BOOT_CPU_XTAL_SPEED_KHZ
     *   - BOOT_CPU_SYSTEM_SPEED_KHZ
     */
    ASSERT_RT(BLT_FALSE);
  }
  /* enable configuration change for the selected node. */
  IfxCan_Node_enableConfigurationChange(MCMCAN_NODE);
  /* configure the bittiming values for the baudrate configuration .*/
  IfxCan_Node_setBitTimingValues(MCMCAN_NODE, 0, tseg1-1, tseg2-1, prescaler-1);
  /* transmit frame configuration. this driver currently supports classic can, so the
   * data field size is always max 8 bytes.
   */
  IfxCan_Node_setTxBufferDataFieldSize(MCMCAN_NODE, IfxCan_DataFieldSize_8);
  /* this driver uses one dedicated transmit buffer. this CAN module with 4 nodes has
   * 16kb message RAM. That's more than plenty, since this driver only uses one node.
   * basically just pick an address in RAM where you want to place the transmit message.
   * note that the entry of a transmit message with a datafield of 8 requires 4 32-bit
   * elements, so 16 (0x10) bytes.
   */
  IfxCan_Node_setTxBuffersStartAddress(MCMCAN_NODE, CAN_TX_BUFFERS_MESSAGE_RAM_BASE_ADDR);
  /* configure the number of dedicated transmit buffers to use. this driver only needs to
   * transmit one specific CAN message. therefore only one transmit buffer is needed.
   */
  IfxCan_Node_setDedicatedTxBuffersNumber(MCMCAN_NODE, 1);
  /* reception frame configuration. this driver currently supports classic can, so the
   * data field size is always max 8 bytes.
   */
  IfxCan_Node_setRxBufferDataFieldSize(MCMCAN_NODE, IfxCan_DataFieldSize_8);
  /* just pick an address in mesage RAM where you want to place the received message.
   * note that the entry of a reception message with a datafield of 8 requires 4 32-bit
   * elements, so 16 (0x10) bytes.
   */
  IfxCan_Node_setRxBuffersStartAddress(MCMCAN_NODE, CAN_RX_BUFFERS_MESSAGE_RAM_BASE_ADDR);
  /* set the message RAM base addresses for storing reception filters configuration. This
   * driver only uses one filter. Note that the entry of a reception filter requires a
   * 32-bit word, so 4 bytes.
   */
  IfxCan_Node_setStandardFilterListStartAddress(MCMCAN_NODE,
                                                CAN_RX_FILTER_STD_MESSAGE_RAM_BASE_ADDR);
  IfxCan_Node_setExtendedFilterListStartAddress(MCMCAN_NODE,
                                                CAN_RX_FILTER_EXT_MESSAGE_RAM_BASE_ADDR);
  /* does the message that this driver is supposed to receive have an 11-bit standard
   * identifier?
   */
  if ((rxMsgId & 0x80000000) == 0)
  {
    /* configure the number of reception acceptance filter accordingly. */
    IfxCan_Node_setStandardFilterListSize(MCMCAN_NODE, 1);
    IfxCan_Node_setExtendedFilterListSize(MCMCAN_NODE, 0);
    /* obtain message RAM pointer for the reception filter buffer 0. */
    Ifx_CAN_STDMSG * rxFilter0;
    rxFilter0 = IfxCan_Node_getStandardFilterElementAddress((blt_addr)MCMCAN_MODULE->RAM,
                                                            CAN_RX_FILTER_STD_MESSAGE_RAM_BASE_ADDR,
                                                            0);
    /* link the filter to the dedicated reception buffer 0. */
    IfxCan_Node_setStandardFilterRxBufferOffset(rxFilter0, IfxCan_RxBufferId_0);
    /* store the message identifier that the reception filter should accept. */
    IfxCan_Node_setStandardFilterId1(rxFilter0, rxMsgId);
    /* configure the filter to store an identifier matched newly received message
     * directly in the dedicated reception buffer 0.
     */
    IfxCan_Node_setStandardFilterConfiguration(rxFilter0, IfxCan_FilterElementConfiguration_storeInRxBuffer);
    /* note that there is no need to call IfxCan_Node_setStandardFilterType() because
     * this setting (SFT-bits) is ignored when the filter is configured to store the
     * received message directly in the dedicated reception buffer
     * (IfxCan_FilterElementConfiguration_storeInRxBuffer)
     */
  }
  /* the message to receive has a 29-bit extented identifier. */
  else
  {
    /* negate the ID-type bit */
    rxMsgId &= ~0x80000000;
    /* configure the number of reception acceptance filter accordingly. */
    IfxCan_Node_setStandardFilterListSize(MCMCAN_NODE, 0);
    IfxCan_Node_setExtendedFilterListSize(MCMCAN_NODE, 1);
    /* obtain message RAM pointer for the reception filter buffer 0. */
    Ifx_CAN_EXTMSG * rxFilter0;
    rxFilter0 = IfxCan_Node_getExtendedFilterElementAddress((blt_addr)MCMCAN_MODULE->RAM,
                                                            CAN_RX_FILTER_EXT_MESSAGE_RAM_BASE_ADDR,
                                                            0);
    /* link the filter to the dedicated reception buffer 0. */
    IfxCan_Node_setExtendedFilterRxBufferOffset(rxFilter0, IfxCan_RxBufferId_0);
    /* store the message identifier that the reception filter should accept. */
    IfxCan_Node_setExtendedFilterId1(rxFilter0, rxMsgId);
    /* configure the filter to store an identifier matched newly received message
     * directly in the dedicated reception buffer 0.
     */
    IfxCan_Node_setExtendedFilterConfiguration(rxFilter0, IfxCan_FilterElementConfiguration_storeInRxBuffer);
    /* note that there is no need to call IfxCan_Node_setExtendedFilterType() because
     * this setting (SFT-bits) is ignored when the filter is configured to store the
     * received message directly in the dedicated reception buffer
     * (IfxCan_FilterElementConfiguration_storeInRxBuffer)
     */
  }
  /* no need to forward other message identifiers to a reception FIFO. */
  IfxCan_Node_configureStandardFilterForNonMatchingFrames(MCMCAN_NODE,
                                                          IfxCan_NonMatchingFrame_reject);
  IfxCan_Node_configureExtendedFilterForNonMatchingFrames(MCMCAN_NODE,
                                                          IfxCan_NonMatchingFrame_reject);
  /* this driver is not interested in receiving remote frames either. */
  IfxCan_Node_rejectRemoteFramesWithStandardId(MCMCAN_NODE);
  IfxCan_Node_rejectRemoteFramesWithExtendedId(MCMCAN_NODE);
  /* this driver only needs to send and receive standard (classic) CAN frames. */
  IfxCan_Node_setFrameMode(MCMCAN_NODE, IfxCan_FrameMode_standard);
  /* disable configuration for the selected node. */
  IfxCan_Node_disableConfigurationChange(MCMCAN_NODE);
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
  blt_int32u txMsgId = BOOT_COM_CAN_TX_MSG_ID;
  blt_int32u timeout;
  Ifx_CAN_TXMSG * txBuf0;
  uint32 txMsgData[2];

  /* validate parameters. */
  ASSERT_RT((data != BLT_NULL) && (len <= 8));

  /* obtain message RAM pointer for the dedicated transmit buffer 0. */
  txBuf0 = IfxCan_Node_getTxBufferElementAddress(MCMCAN_NODE,
                                                 (blt_addr)MCMCAN_MODULE->RAM,
                                                 CAN_TX_BUFFERS_MESSAGE_RAM_BASE_ADDR,
                                                 IfxCan_TxBufferId_0);
  /* store the message identifier. */
  if ((txMsgId & 0x80000000) == 0)
  {
    /* set the 11-bit CAN identifier. */
    IfxCan_Node_setMsgId(txBuf0, txMsgId, IfxCan_MessageIdLength_standard);
  }
  else
  {
    /* negate the ID-type bit */
    txMsgId &= ~0x80000000;
    /* set the 29-bit CAN identifier. */
    IfxCan_Node_setMsgId(txBuf0, txMsgId, IfxCan_MessageIdLength_extended);
  }
  /* configure the message as a standard frame and not a remote frame. */
  IfxCan_Node_setRemoteTransmitReq(txBuf0, FALSE);
  /* set data length code. */
  IfxCan_Node_setDataLength(txBuf0, canDataLenLookup[len]);
  /* store the message data as two 32-bit values. */
  txMsgData[0]  = data[0];
  txMsgData[0] |= data[1] << 8;
  txMsgData[0] |= data[2] << 16;
  txMsgData[0] |= data[3] << 24;
  txMsgData[1]  = data[4];
  txMsgData[1] |= data[5] << 8;
  txMsgData[1] |= data[6] << 16;
  txMsgData[1] |= data[7] << 24;
  IfxCan_Node_writeTxBufData(txBuf0, canDataLenLookup[len], txMsgData);
  /* set CAN frame mode to standard for classic CAN. */
  IfxCan_Node_setFrameModeReq(txBuf0, IfxCan_FrameMode_standard);
  /* set transmit request to start the transmission. */
  IfxCan_Node_setTxBufferAddRequest(MCMCAN_NODE, IfxCan_TxBufferId_0);
  /* determine timeout time for the transmit completion. */
  timeout = TimerGet() + CAN_MSG_TX_TIMEOUT_MS;
  /* poll for completion of the transmit operation. */
  while (IfxCan_Node_isTxBufferTransmissionOccured(MCMCAN_NODE, IfxCan_TxBufferId_0) == FALSE)
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
  blt_bool result = BLT_FALSE;
  Ifx_CAN_RXMSG * rxBuf0;
  uint32 rxMsgData[2] = { 0, 0 };

  /* was the expected CAN message received in the dedicated reception buffer 0? */
  if (IfxCan_Node_isRxBufferNewDataUpdated(MCMCAN_NODE, IfxCan_RxBufferId_0) == TRUE)
  {
    /* obtain message RAM pointer for the dedicated reception buffer 0. */
    rxBuf0 = IfxCan_Node_getRxBufferElementAddress(MCMCAN_NODE,
                                                   (blt_addr)MCMCAN_MODULE->RAM,
                                                   CAN_RX_BUFFERS_MESSAGE_RAM_BASE_ADDR,
                                                   IfxCan_RxBufferId_0);
    /* note that the reception acceptance filter for this dedicated reception buffer 0
     * is already configured to only receive the one message with identfier
     * BOOT_COM_CAN_RX_MSG_ID and correct type (STD/EXT), so there is no need to double
     * check the identifier here. continue with reading out the data length.
     */
    *len = (blt_int8u)IfxCan_Node_getDataLengthCode(rxBuf0);
    /* only continue with the message reception if the length is valid. */
    if (*len <= 8)
    {
      /* retrieve and extract the data bytes. */
      IfxCan_Node_readData(rxBuf0, canDataLenLookup[*len], rxMsgData);
      data[0] = (blt_int8u)rxMsgData[0];
      data[1] = (blt_int8u)(rxMsgData[0] >> 8);
      data[2] = (blt_int8u)(rxMsgData[0] >> 16);
      data[3] = (blt_int8u)(rxMsgData[0] >> 24);
      data[4] = (blt_int8u)rxMsgData[1];
      data[5] = (blt_int8u)(rxMsgData[1] >> 8);
      data[6] = (blt_int8u)(rxMsgData[1] >> 16);
      data[7] = (blt_int8u)(rxMsgData[1] >> 24);
      /* update the result to indicate that a new packet was received. */
      result = BLT_TRUE;
    }
    /* clear newdata flag after reading. */
    IfxCan_Node_clearRxBufferNewDataFlag(MCMCAN_NODE, IfxCan_RxBufferId_0);
  }
  /* give the result back to the caller */
  return result;
} /*** end of CanReceivePacket ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of can.c **************************************/
