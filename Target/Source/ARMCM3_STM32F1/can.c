/************************************************************************************//**
* \file         Source\ARMCM3_STM32F1\can.c
* \brief        Bootloader CAN communication interface source file.
* \ingroup      Target_ARMCM3_STM32F1
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
/** \brief Timeout for entering/leaving CAN initialization mode in milliseconds. */
#define CAN_INIT_TIMEOUT_MS            (250u)

/** \brief Timeout for transmitting a CAN message in milliseconds. */
#define CAN_MSG_TX_TIMEOUT_MS          (50u)


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief CAN transmission mailbox layout. */
typedef struct
{
  volatile blt_int32u TIR;
  volatile blt_int32u TDTR;
  volatile blt_int32u TDLR;
  volatile blt_int32u TDHR;
} tCanTxMailBox;

/** \brief CAN reception FIFO mailbox layout. */
typedef struct
{
  volatile blt_int32u RIR;
  volatile blt_int32u RDTR;
  volatile blt_int32u RDLR;
  volatile blt_int32u RDHR;
} tCanRxFIFOMailBox;

/** \brief CAN filter register layout. */
typedef struct
{
  volatile blt_int32u FR1;
  volatile blt_int32u FR2;
} tCanFilter;

/** \brief CAN controller register layout. */
typedef struct
{
  volatile blt_int32u MCR;
  volatile blt_int32u MSR;
  volatile blt_int32u TSR;
  volatile blt_int32u RF0R;
  volatile blt_int32u RF1R;
  volatile blt_int32u IER;
  volatile blt_int32u ESR;
  volatile blt_int32u BTR;
  blt_int32u          RESERVED0[88];
  tCanTxMailBox       sTxMailBox[3];
  tCanRxFIFOMailBox   sFIFOMailBox[2];
  blt_int32u          RESERVED1[12];
  volatile blt_int32u FMR;
  volatile blt_int32u FM1R;
  blt_int32u          RESERVED2;
  volatile blt_int32u FS1R;
  blt_int32u          RESERVED3;
  volatile blt_int32u FFA1R;
  blt_int32u          RESERVED4;
  volatile blt_int32u FA1R;
  blt_int32u          RESERVED5[8];
  tCanFilter          sFilterRegister[14];
} tCanRegs;


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Reset request bit. */
#define CAN_BIT_RESET    ((blt_int32u)0x00008000)
/** \brief Initialization request bit. */
#define CAN_BIT_INRQ     ((blt_int32u)0x00000001)
/** \brief Initialization acknowledge bit. */
#define CAN_BIT_INAK     ((blt_int32u)0x00000001)
/** \brief Sleep mode request bit. */
#define CAN_BIT_SLEEP    ((blt_int32u)0x00000002)
/** \brief Filter 0 selection bit. */
#define CAN_BIT_FILTER0  ((blt_int32u)0x00000001)
/** \brief Filter init mode bit. */
#define CAN_BIT_FINIT    ((blt_int32u)0x00000001)
/** \brief Transmit mailbox 0 empty bit. */
#define CAN_BIT_TME0     ((blt_int32u)0x04000000)
/** \brief Identifier extension bit. */
#define CAN_BIT_IDE      ((blt_int32u)0x00000004)
/** \brief Transmit mailbox request bit. */
#define CAN_BIT_TXRQ     ((blt_int32u)0x00000001)
/** \brief Release FIFO 0 mailbox bit. */
#define CAN_BIT_RFOM0    ((blt_int32u)0x00000020)
/** \brief Standard 11-bit identifier bit mask. */
#define CAN_BIT_STDID_MASK (0x7FFu << CAN_BIT_STDID_POS)
/** \brief Standard 11-bit identifier bits position. */
#define CAN_BIT_STDID_POS  (21u)
/** \brief Extended 29-bit identifier bit mask. */
#define CAN_BIT_EXTID_MASK (0x1FFFFFFFu << CAN_BIT_EXTID_POS)
/** \brief Extended 29-bit identifier bits position. */
#define CAN_BIT_EXTID_POS  (3u)


/****************************************************************************************
* Register definitions
****************************************************************************************/
/** \brief Macro for accessing CAN controller registers. */
#define CANx             ((tCanRegs *) (blt_int32u)0x40006400)


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

  /* loop through all possible time quanta configurations to find a match */
  for (cnt=0; cnt < sizeof(canTiming)/sizeof(canTiming[0]); cnt++)
  {
    if (((BOOT_CPU_SYSTEM_SPEED_KHZ/2) % (baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1))) == 0)
    {
      /* compute the prescaler that goes with this TQ configuration */
      *prescaler = (BOOT_CPU_SYSTEM_SPEED_KHZ/2)/(baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1));

      /* make sure the prescaler is valid */
      if ((*prescaler > 0) && (*prescaler <= 1024))
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
  blt_int16u prescaler;
  blt_int8u  tseg1, tseg2;
  blt_bool   result;
  blt_int32u timeout;
  blt_int32u rxMsgId = BOOT_COM_CAN_RX_MSG_ID;
  blt_int32u rxFilterId, rxFilterMask;

  /* the current implementation supports CAN1. throw an assertion error in case a
   * different CAN channel is configured.
   */
  ASSERT_CT(BOOT_COM_CAN_CHANNEL_INDEX == 0);
  /* obtain bittiming configuration information */
  result = CanGetSpeedConfig(BOOT_COM_CAN_BAUDRATE/1000, &prescaler, &tseg1, &tseg2);
  ASSERT_RT(result == BLT_TRUE);
  /* disable all can interrupt. this driver works in polling mode */
  CANx->IER = (blt_int32u)0;
  /* set request to reset the can controller */
  CANx->MCR |= CAN_BIT_RESET ;
  /* set timeout time to wait for can controller reset */
  timeout = TimerGet() + CAN_INIT_TIMEOUT_MS;
  /* wait for acknowledge that the can controller was reset */
  while ((CANx->MCR & CAN_BIT_RESET) != 0)
  {
    /* keep the watchdog happy */
    CopService();
    /* break loop upon timeout. this would indicate a hardware failure. */
    if (TimerGet() > timeout)
    {
      break;
    }
  }
  /* exit from sleep mode, which is the default mode after reset */
  CANx->MCR &= ~CAN_BIT_SLEEP;
  /* set request to enter initialisation mode */
  CANx->MCR |= CAN_BIT_INRQ ;
  /* set timeout time to wait for entering initialization mode */
  timeout = TimerGet() + CAN_INIT_TIMEOUT_MS;
  /* wait for acknowledge that initialization mode was entered */
  while ((CANx->MSR & CAN_BIT_INAK) == 0)
  {
    /* keep the watchdog happy */
    CopService();
    /* break loop upon timeout. this would indicate a hardware failure. */
    if (TimerGet() > timeout)
    {
      break;
    }
  }
  /* configure the bittming */
  CANx->BTR = (blt_int32u)((blt_int32u)(tseg1 - 1) << 16) | \
              (blt_int32u)((blt_int32u)(tseg2 - 1) << 20) | \
              (blt_int32u)(prescaler - 1);
  /* set request to leave initialisation mode */
  CANx->MCR &= ~CAN_BIT_INRQ;
  /* set timeout time to wait for exiting initialization mode */
  timeout = TimerGet() + CAN_INIT_TIMEOUT_MS;
  /* wait for acknowledge that initialization mode was exited */
  while ((CANx->MSR & CAN_BIT_INAK) != 0)
  {
    /* keep the watchdog happy */
    CopService();
    /* break loop upon timeout. this would indicate a hardware failure. */
    if (TimerGet() > timeout)
    {
      break;
    }
  }

  /* determine the reception filter mask and id values such that it only leaves one
   * CAN identifier through (BOOT_COM_CAN_RX_MSG_ID).
   */
  if ((rxMsgId & 0x80000000) == 0)
  {
    rxFilterId = rxMsgId << CAN_BIT_STDID_POS;
    rxFilterMask = (CAN_BIT_STDID_MASK) | CAN_BIT_IDE;
  }
  else
  {
    /* negate the ID-type bit */
    rxMsgId &= ~0x80000000;
    rxFilterId = (rxMsgId << CAN_BIT_EXTID_POS) | CAN_BIT_IDE;
    rxFilterMask = (CAN_BIT_EXTID_MASK) | CAN_BIT_IDE;
  }

  /* enter initialisation mode for the acceptance filter */
  CANx->FMR |= CAN_BIT_FINIT;
  /* deactivate filter 0 */
  CANx->FA1R &= ~CAN_BIT_FILTER0;
  /* 32-bit scale for the filter */
  CANx->FS1R |= CAN_BIT_FILTER0;
  /* Configure identifier mask mode for the filter */
  CANx->FM1R &= ~CAN_BIT_FILTER0;
  /* configure the acceptance filter to receive just one message */
  CANx->sFilterRegister[0].FR1 = rxFilterId;
  CANx->sFilterRegister[0].FR2 = rxFilterMask;
  /* select id/mask mode for the filter */
  CANx->FM1R &= ~CAN_BIT_FILTER0;
  /* FIFO 0 assignation for the filter */
  CANx->FFA1R &= ~CAN_BIT_FILTER0;
  /* filter activation */
  CANx->FA1R |= CAN_BIT_FILTER0;
  /* leave initialisation mode for the acceptance filter */
  CANx->FMR &= ~CAN_BIT_FINIT;
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

  /* make sure that transmit mailbox 0 is available */
  ASSERT_RT((CANx->TSR&CAN_BIT_TME0) == CAN_BIT_TME0);

  /* reset all CAN identifier related bits */
  CANx->sTxMailBox[0].TIR &= CAN_BIT_TXRQ;
  /* is it a 11-bit standard CAN identifier? */
  if ((txMsgId & 0x80000000) == 0)
  {
    /* store the 11-bit message identifier */
    CANx->sTxMailBox[0].TIR |= ((blt_int32u)txMsgId << 21);
  }
  /* it is a 29-bit extended CAN identifier */
  else
  {
    /* negate the ID-type bit */
    txMsgId &= ~0x80000000;
    /* store the 29-bit message identifier */
    CANx->sTxMailBox[0].TIR |= (((blt_int32u)txMsgId << 3) | CAN_BIT_IDE);
  }

  /* store the message date length code (DLC) */
  CANx->sTxMailBox[0].TDTR = len;
  /* store the message data bytes */
  CANx->sTxMailBox[0].TDLR = (((blt_int32u)data[3] << 24) | \
                              ((blt_int32u)data[2] << 16) | \
                              ((blt_int32u)data[1] <<  8) | \
                              ((blt_int32u)data[0]));
  CANx->sTxMailBox[0].TDHR = (((blt_int32u)data[7] << 24) | \
                              ((blt_int32u)data[6] << 16) | \
                              ((blt_int32u)data[5] <<  8) | \
                              ((blt_int32u)data[4]));
  /* request the start of message transmission */
  CANx->sTxMailBox[0].TIR |= CAN_BIT_TXRQ;
  /* set timeout time to wait for transmission completion */
  timeout = TimerGet() + CAN_MSG_TX_TIMEOUT_MS;
  /* wait for transmit completion */
  while ((CANx->TSR&CAN_BIT_TME0) == 0)
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
  blt_bool   result = BLT_FALSE;

  /* check if a new message was received */
  if ((CANx->RF0R&(blt_int32u)0x00000003) > 0)
  {
    /* read out the CAN identifier */
    if ((CANx->sFIFOMailBox[0].RIR & CAN_BIT_IDE) == 0)
    {
      /* read out the 11-bit standard CAN identifier */
      rxMsgId = (blt_int32u)0x000007FF & (CANx->sFIFOMailBox[0].RIR >> 21);
    }
    else
    {
      /* read out the 29-bit extended CAN identifier */
      rxMsgId = (blt_int32u)0x1FFFFFFF & (CANx->sFIFOMailBox[0].RIR >> 3);
      rxMsgId |= 0x80000000;
    }
    /* is this the packet identifier */
    if (rxMsgId == BOOT_COM_CAN_RX_MSG_ID)
    {
      result = BLT_TRUE;
      *len = ((blt_int8u)(CANx->sFIFOMailBox[0].RDTR)) & 0x0fu;
      /* store the received packet data */
      data[0] = (blt_int8u)0xFF & CANx->sFIFOMailBox[0].RDLR;
      data[1] = (blt_int8u)0xFF & (CANx->sFIFOMailBox[0].RDLR >> 8);
      data[2] = (blt_int8u)0xFF & (CANx->sFIFOMailBox[0].RDLR >> 16);
      data[3] = (blt_int8u)0xFF & (CANx->sFIFOMailBox[0].RDLR >> 24);
      data[4] = (blt_int8u)0xFF & CANx->sFIFOMailBox[0].RDHR;
      data[5] = (blt_int8u)0xFF & (CANx->sFIFOMailBox[0].RDHR >> 8);
      data[6] = (blt_int8u)0xFF & (CANx->sFIFOMailBox[0].RDHR >> 16);
      data[7] = (blt_int8u)0xFF & (CANx->sFIFOMailBox[0].RDHR >> 24);
    }
    /* release FIFO0 */
    CANx->RF0R |= CAN_BIT_RFOM0;
  }
  return result;
} /*** end of CanReceivePacket ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of can.c **************************************/
