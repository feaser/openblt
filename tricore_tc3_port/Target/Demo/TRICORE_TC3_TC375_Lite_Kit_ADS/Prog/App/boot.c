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


#if (BOOT_COM_CAN_ENABLE > 0)
/****************************************************************************************
*        C O N T R O L L E R   A R E A   N E T W O R K   I N T E R F A C E
****************************************************************************************/

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Map the MCMCAN module that the selected communication node belongs to. */
#define MCMCAN_MODULE   (&MODULE_CAN0)

/** \brief Map the communication node to use. */
#define MCMCAN_NODE     (&MODULE_CAN0.N[IfxCan_NodeId_0])

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
  unsigned char tseg1;                                /**< CAN time segment 1          */
  unsigned char tseg2;                                /**< CAN time segment 2          */
} tCanBusTiming;


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
{                       /*  TQ | TSEG1 | TSEG2 | SP  */
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
** \return    1 if the CAN bustiming register values were found, 0 otherwise.
**
****************************************************************************************/
static unsigned char CanGetSpeedConfig(unsigned short baud, unsigned short *prescaler,
                                       unsigned char *tseg1, unsigned char *tseg2)
{
  unsigned char cnt;
  unsigned long canClockFreqkHz;

  /* determine the clock frequency that sources the CAN controller and is used for the
   * baudrate generation.
   */
  canClockFreqkHz = ((unsigned long)IfxCan_getModuleFrequency()) / 1000;
  /* loop through all possible time quanta configurations to find a match */
  for (cnt=0; cnt < sizeof(canTiming)/sizeof(canTiming[0]); cnt++)
  {
    if ((canClockFreqkHz % (baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1))) == 0)
    {
      /* compute the prescaler that goes with this TQ configuration */
      *prescaler = (unsigned short)(canClockFreqkHz/(baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1)));
      /* make sure the prescaler is valid */
      if ((*prescaler > 0) && (*prescaler <= 512))
      {
        /* store the bustiming configuration */
        *tseg1 = canTiming[cnt].tseg1;
        *tseg2 = canTiming[cnt].tseg2;
        /* found a good bus timing configuration */
        return 1;
      }
    }
  }
  /* could not find a good bus timing configuration */
  return 0;
} /*** end of CanGetSpeedConfig ***/


/************************************************************************************//**
** \brief     Initializes the CAN communication interface.
** \return    none.
**
****************************************************************************************/
static void BootComCanInit(void)
{
  unsigned short prescaler = 0;
  unsigned char  tseg1 = 0, tseg2 = 0;
  unsigned long  rxMsgId = BOOT_COM_CAN_RX_MSG_ID;
  const IfxCan_ClockSelect clockSelectLookup[] =
  {
    IfxCan_ClockSelect_0,
    IfxCan_ClockSelect_1,
    IfxCan_ClockSelect_2,
    IfxCan_ClockSelect_3
  };

  /* Configure the STBY GPIO pin P20.6 as a digital output. */
  IfxPort_setPinModeOutput(&MODULE_P20, 6U, IfxPort_OutputMode_pushPull,
                                            IfxPort_OutputIdx_general);
  /* Switch the CAN transceiver to normal mode by setting the STBY GPIO pin logic low. */
  IfxPort_setPinLow(&MODULE_P20, 6U);
  /* Enable the MCM CAN module. */
  IfxCan_enableModule(MCMCAN_MODULE);
  /* Configure MCMCAN0 GPIO pins P20.8 Tx and P20.7 Rx (node 0). */
  IfxCan_Node_initTxPin(&IfxCan_TXD00_P20_8_OUT, IfxPort_OutputMode_pushPull,
                        IfxPort_PadDriver_cmosAutomotiveSpeed1);
  IfxCan_Node_initRxPin(MCMCAN_NODE, &IfxCan_RXD00B_P20_7_IN, IfxPort_InputMode_pullUp,
                        IfxPort_PadDriver_cmosAutomotiveSpeed1);
  /* enable both the synchronous and asynchronous clocks. The synchronous clock is used
   * as the source for the register and RAM interface of the CAN controller. The
   * asynchronous clock is used for the CAN baudrate generation.
   */
  IfxCan_setClockSource(MCMCAN_MODULE, clockSelectLookup[BOOT_COM_CAN_CHANNEL_INDEX % 4],
                        IfxCan_ClockSource_both);
  /* obtain bittiming configuration information. */
  (void)CanGetSpeedConfig(BOOT_COM_CAN_BAUDRATE/1000, &prescaler, &tseg1, &tseg2);
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
    rxFilter0 = IfxCan_Node_getStandardFilterElementAddress((unsigned long)MCMCAN_MODULE->RAM,
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
    rxFilter0 = IfxCan_Node_getExtendedFilterElementAddress((unsigned long)MCMCAN_MODULE->RAM,
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
} /*** end of BootComCanInit ***/


/************************************************************************************//**
** \brief     Receives the CONNECT request from the host, which indicates that the
**            bootloader should be activated and, if so, activates it.
** \return    none.
**
****************************************************************************************/
static void BootComCanCheckActivationRequest(void)
{
  /* lookup table for converting the DLC value (0..8) into the data length code type that
   * the driver library uses.
   */
  const IfxCan_DataLengthCode dataLenLookup[] =
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
  Ifx_CAN_RXMSG * rxBuf0;
  unsigned long rxMsgData[2];
  unsigned char rxByte0;
  unsigned char rxMsgLen;

  /* was the expected CAN message received in the dedicated reception buffer 0? */
  if (IfxCan_Node_isRxBufferNewDataUpdated(MCMCAN_NODE, IfxCan_RxBufferId_0) == TRUE)
  {
    /* obtain message RAM pointer for the dedicated reception buffer 0. */
    rxBuf0 = IfxCan_Node_getRxBufferElementAddress(MCMCAN_NODE,
                                                   (unsigned long)MCMCAN_MODULE->RAM,
                                                   CAN_RX_BUFFERS_MESSAGE_RAM_BASE_ADDR,
                                                   IfxCan_RxBufferId_0);
    /* note that the reception acceptance filter for this dedicated reception buffer 0
     * is already configured to only receive the one message with identfier
     * BOOT_COM_CAN_RX_MSG_ID and correct type (STD/EXT), so there is no need to double
     * check the identifier here. continue with reading out the data length.
     */
    rxMsgLen = (unsigned char)IfxCan_Node_getDataLengthCode(rxBuf0);
    /* only continue with the message reception if the length is valid. */
    if (rxMsgLen <= 8)
    {
      /* retrieve the data bytes. */
      IfxCan_Node_readData(rxBuf0, dataLenLookup[rxMsgLen], (uint32 *)rxMsgData);
      /* read out the first data byte. */
      rxByte0 = (unsigned char)(rxMsgData[0] & 0x000000FF);
      /* check if this was an XCP CONNECT command */
      if ((rxByte0 == 0xff) && (rxMsgLen == 2))
      {
        /* connection request received so start the bootloader */
        BootActivate();
      }
    }
    /* clear newdata flag after reading. */
    IfxCan_Node_clearRxBufferNewDataFlag(MCMCAN_NODE, IfxCan_RxBufferId_0);
  }
} /*** end of BootComCanCheckActivationRequest ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of boot.c *************************************/
