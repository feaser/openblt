/************************************************************************************//**
* \file         Source\ARM7_LPC2000\can.c
* \brief        Bootloader CAN communication interface source file.
* \ingroup      Target_ARM7_LPC2000
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
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
/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout for transmitting a CAN message in milliseconds. */
#define CAN_MSG_TX_TIMEOUT_MS          (50u)
/** \brief Transmit buffer 1 idle bit. */
#define CAN_TBS1        (0x00000004)
/** \brief Transmit buffer 1 complete bit. */
#define CAN_TCS1        (0x00000008)
/** \brief Receive buffer release bit. */
#define CAN_RRB         (0x04)
/** \brief Receive buffer status bit. */
#define CAN_RBS         (0x01)
/** \brief Transmission request bit. */
#define CAN_TR          (0x01)
/** \brief Select tx buffer 1 for transmit bit. */
#define CAN_STB1        (0x20)
/** \brief Frame format bit. 0 for 11-bit and 1 for 29-bit CAN identifiers. */
#define CAN_FF          (0x80000000)


/****************************************************************************************
* Register definitions
****************************************************************************************/
/** \brief CANAFMR CAN controller register. */
#define CANAFMR         (*((volatile blt_int8u  *) 0xE003C000))
/** \brief CAN1MOD CAN controller register. */
#define CAN1MOD         (*((volatile blt_int32u *) 0xE0044000))
/** \brief CAN1IER CAN controller register. */
#define CAN1IER         (*((volatile blt_int32u *) 0xE0044010))
/** \brief CAN1GSR CAN controller register. */
#define CAN1GSR         (*((volatile blt_int32u *) 0xE0044008))
/** \brief CAN1BTR CAN controller register. */
#define CAN1BTR         (*((volatile blt_int32u *) 0xE0044014))
/** \brief CAN1TFI1 CAN controller register. */
#define CAN1TFI1        (*((volatile blt_int32u *) 0xE0044030))
/** \brief CAN1TID1 CAN controller register. */
#define CAN1TID1        (*((volatile blt_int32u *) 0xE0044034))
/** \brief CAN1TDA1 CAN controller register. */
#define CAN1TDA1        (*((volatile blt_int32u *) 0xE0044038))
/** \brief CAN1TDB1 CAN controller register. */
#define CAN1TDB1        (*((volatile blt_int32u *) 0xE004403C))
/** \brief CAN1CMR CAN controller register. */
#define CAN1CMR         (*((volatile blt_int32u *) 0xE0044004))
/** \brief CAN1SR CAN controller register. */
#define CAN1SR          (*((volatile blt_int32u *) 0xE004401C))
/** \brief CAN1RFS CAN controller register. */
#define CAN1RFS         (*((volatile blt_int32u *) 0xE0044020))
/** \brief CAN1RID CAN controller register. */
#define CAN1RID         (*((volatile blt_int32u *) 0xE0044024))
/** \brief CAN1RDA CAN controller register. */
#define CAN1RDA         (*((volatile blt_int32u *) 0xE0044028))
/** \brief CAN1RDB CAN controller register. */
#define CAN1RDB         (*((volatile blt_int32u *) 0xE004402C))


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
** \param     baud The desired baudrate in kbps. Valid values are 10..1000.
** \param     btr  Pointer to where the value for register CANxBTR will be stored.
** \return    BLT_TRUE if the CAN bustiming register values were found, BLT_FALSE
**            otherwise.
**
****************************************************************************************/
static blt_bool CanGetSpeedConfig(blt_int16u baud, blt_int32u *btr)
{
  blt_int16u prescaler;
  blt_int8u  cnt;

  /* loop through all possible time quanta configurations to find a match */
  for (cnt=0; cnt < sizeof(canTiming)/sizeof(canTiming[0]); cnt++)
  {
    if ((BOOT_CPU_SYSTEM_SPEED_KHZ % (baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1))) == 0)
    {
      /* compute the prescaler that goes with this TQ configuration */
      prescaler = BOOT_CPU_SYSTEM_SPEED_KHZ/(baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1));

      /* make sure the prescaler is valid */
      if ((prescaler > 0) && (prescaler <= 1024))
      {
        /* store the prescaler and bustiming register value */
        *btr  = prescaler - 1;
        *btr |= ((canTiming[cnt].tseg2 - 1) << 20) | ((canTiming[cnt].tseg1 - 1) << 16);
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
  blt_bool   result;
  blt_int32u btr_reg_value=0;

  /* the current implementation supports CAN1, which has channel index 0. throw an
   * assertion error in case a different CAN channel is configured.
   */
  ASSERT_CT(BOOT_COM_CAN_CHANNEL_INDEX == 0);
  /* configure acceptance filter for bypass mode so it receives all messages */
  CANAFMR = 0x00000002L;
  /* take CAN controller offline and go into reset mode */
  CAN1MOD = 1;
  /* disable all interrupts. driver only needs to work in polling mode */
  CAN1IER = 0;
  /* reset CAN controller status */
  CAN1GSR = 0;
  /* configure the bittiming */
  result = CanGetSpeedConfig(BOOT_COM_CAN_BAUDRATE/1000, &btr_reg_value);
  /* check that a valid baudrate configuration was found */
  ASSERT_RT(result == BLT_TRUE);
  /* write the bittiming configuration to the register */
  CAN1BTR = btr_reg_value;
  /* enter normal operating mode and synchronize to the CAN bus */
  CAN1MOD = 0;
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
  blt_int32u timeout;

  /* check that transmit buffer 1 is ready to accept a new message */
  ASSERT_RT((CAN1SR & CAN_TBS1) != 0);
  /* write dlc and configure message as a standard message with 11-bit identifier */
  CAN1TFI1 = (len << 16);
  /* write the message identifier */
  CAN1TID1 = BOOT_COM_CAN_TX_MSG_ID;
  /* is it a 29-bit CAN identifier? */
  if ( (BOOT_COM_CAN_TX_MSG_ID & 0x80000000) != 0)
  {
    /* configure identifier as 29-bit extended. */
    CAN1TFI1 |= CAN_FF;
    /* Reset the mask bit. */
    CAN1TID1 &= ~0x80000000;
  }
  /* write the first set of 4 data bytes */
  CAN1TDA1 = (data[3] << 24) + (data[2] << 16) + (data[1] << 8) + data[0];
  /* write the second set of 4 data bytes */
  CAN1TDB1 = (data[7] << 24) + (data[6] << 16) + (data[5] << 8) + data[4];
  /* write transmission request for transmit buffer 1 */
  CAN1CMR = CAN_TR | CAN_STB1;
  /* set timeout time to wait for transmission completion */
  timeout = TimerGet() + CAN_MSG_TX_TIMEOUT_MS;
  /* wait for transmit completion */
  while ((CAN1SR & CAN_TCS1) == 0)
  {
    /* keep the watchdog happy */
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
  blt_int32u rxMsgId;
  blt_int8u rxMsgDlc;
  blt_bool result = BLT_FALSE;
  
  /* check if a new message was received */
  if ((CAN1SR & CAN_RBS) != 0)
  {
    /* read out the CAN message identifier */
    rxMsgId = CAN1RID;
    /* was is a 29-bit extended CAN identifier? */
    if ((CAN1RFS & CAN_FF) != 0)
    {
      /* set mask bit. */
      rxMsgId |= 0x80000000;
    }
    /* see if this is the message identifier that we are interested in */
    if (rxMsgId == BOOT_COM_CAN_RX_MSG_ID)
    {
      /* store the message data length */
      rxMsgDlc = ((blt_int8u)(CAN1RFS >> 16)) & 0x0Fu;
      if (rxMsgDlc > 8)
      {
        rxMsgDlc = 8;
      }
      *len = rxMsgDlc;
      /* store the message data */
      data[0] = (blt_int8u)CAN1RDA;
      data[1] = (blt_int8u)(CAN1RDA >> 8);
      data[2] = (blt_int8u)(CAN1RDA >> 16);
      data[3] = (blt_int8u)(CAN1RDA >> 24);
      data[4] = (blt_int8u)CAN1RDB;
      data[5] = (blt_int8u)(CAN1RDB >> 8);
      data[6] = (blt_int8u)(CAN1RDB >> 16);
      data[7] = (blt_int8u)(CAN1RDB >> 24);
      /* update the result. */
      result = BLT_TRUE;
    }
    /* release the receive buffer */
    CAN1CMR = CAN_RRB;
  }
  /* give the result back to the caller. */
  return result;
} /*** end of CanReceivePacket ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of can.c **************************************/
