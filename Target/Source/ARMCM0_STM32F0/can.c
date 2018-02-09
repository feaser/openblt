/************************************************************************************//**
* \file         Source\ARMCM0_STM32F0\can.c
* \brief        Bootloader CAN communication interface source file.
* \ingroup      Target_ARMCM0_STM32F0
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
#include "stm32f0xx.h"                           /* STM32 registers and drivers        */


#if (BOOT_COM_CAN_ENABLE > 0)
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
** \param     baud The desired baudrate in bps. Valid values are 10..1000kpbs.
** \param     prescaler Pointer to where the value for the prescaler will be stored.
** \param     bitTiming Pointer to where the values for TSEG1 and TSEG2 will be stored.
** \return    TRUE if the CAN bustiming register values were found, FALSE otherwise.
**
****************************************************************************************/
static blt_bool CanGetSpeedConfig(blt_int32u baud, blt_int16u *prescaler, tCanBusTiming *bitTiming)

{
  blt_int8u cnt;
  blt_int32u canClockFreqHz;
  RCC_ClocksTypeDef rccClocks;

  /* get the speed of the CAN peripheral clock (fAPB) */
  RCC_GetClocksFreq(&rccClocks);
  canClockFreqHz = rccClocks.PCLK_Frequency;

  /* loop through all possible time quanta configurations to find a match */
  for (cnt=0; cnt < sizeof(canTiming)/sizeof(canTiming[0]); cnt++)
  {
    if ((canClockFreqHz % (baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1))) == 0)
    {
      /* compute the prescaler that goes with this TQ configuration */
      *prescaler = canClockFreqHz/(baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1));

      /* make sure the prescaler is valid */
      if ((*prescaler > 0) && (*prescaler <= 1024))
      {
        /* store the bittiming configuration */
        bitTiming->tseg1 = canTiming[cnt].tseg1;
        bitTiming->tseg2 = canTiming[cnt].tseg2;
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
  CAN_InitTypeDef CAN_InitStructure;
  CAN_FilterInitTypeDef CAN_FilterInitStructure;
  blt_int16u prescaler = 1;
  tCanBusTiming bitTiming = { 1, 1 };
  blt_bool   result;

  /* the current implementation supports CAN1. throw an assertion error in case a
   * different CAN channel is configured.
   */
  ASSERT_CT(BOOT_COM_CAN_CHANNEL_INDEX == 0);
  /* obtain bittiming configuration information */
  result = CanGetSpeedConfig(BOOT_COM_CAN_BAUDRATE, &prescaler, &bitTiming);
  ASSERT_RT(result == BLT_TRUE);

  /* CAN register init */
  CAN_DeInit(CAN);
  CAN_StructInit(&CAN_InitStructure);
  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = DISABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
  CAN_InitStructure.CAN_BS1 = bitTiming.tseg1 - 1;
  CAN_InitStructure.CAN_BS2 = bitTiming.tseg2 - 1;
  CAN_InitStructure.CAN_Prescaler = prescaler;
  CAN_Init(CAN, &CAN_InitStructure);
  /* CAN filter init - receive all messages */
  CAN_FilterInitStructure.CAN_FilterNumber = 0;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);
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
  CanTxMsg txMsg;
  uint8_t byteIdx;
  uint8_t txMailbox;

  /* prepare message */
  if ((BOOT_COM_CAN_TX_MSG_ID & 0x80000000) == 0)
  {
    /* 11-bit standard CAN identifier. */
    txMsg.IDE = CAN_Id_Standard;
    txMsg.StdId = BOOT_COM_CAN_TX_MSG_ID;
  }
  else
  {
    /* 29-bit extended CAN identifier. */
    txMsg.IDE = CAN_Id_Extended;
    txMsg.ExtId = BOOT_COM_CAN_TX_MSG_ID;
    txMsg.ExtId &= ~0x80000000;
  }
  txMsg.RTR = CAN_RTR_DATA;
  txMsg.DLC = len;
  for (byteIdx=0; byteIdx<len; byteIdx++)
  {
    txMsg.Data[byteIdx] = data[byteIdx];
  }
  txMailbox = CAN_Transmit(CAN, &txMsg);
  /* wait for transmit completion */
  while (CAN_TransmitStatus(CAN, txMailbox) == CAN_TxStatus_Pending)
  {
    /* keep the watchdog happy */
    CopService();
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
  CanRxMsg rxMsg;
  blt_int8u byteIdx;
  blt_bool result = BLT_FALSE;
  blt_bool canIdMatched = BLT_FALSE;

  /* check if a new message was received */
  if (CAN_MessagePending(CAN, CAN_FIFO0) > 0)
  {
    /* receive the message */
    CAN_Receive(CAN, CAN_FIFO0, &rxMsg);

    /* check if the message identifier matches the bootloader reception message */
    if ( (rxMsg.IDE == CAN_Id_Standard) &&
         (rxMsg.StdId == BOOT_COM_CAN_RX_MSG_ID) )
    {
      canIdMatched = BLT_TRUE;
    }
    if ( (rxMsg.IDE == CAN_Id_Extended) &&
         ((rxMsg.ExtId | 0x80000000) == BOOT_COM_CAN_RX_MSG_ID) )
    {
      canIdMatched = BLT_TRUE;
    }
    /* is the identifier a match to the bootloader reception message identifier? */
    if (canIdMatched == BLT_TRUE)
    {
      *len = rxMsg.DLC;
      for (byteIdx=0; byteIdx<rxMsg.DLC; byteIdx++)
      {
        data[byteIdx] = rxMsg.Data[byteIdx];
      }
      result = BLT_TRUE;
    }
  }
  return result;
} /*** end of CanReceivePacket ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of can.c **************************************/
