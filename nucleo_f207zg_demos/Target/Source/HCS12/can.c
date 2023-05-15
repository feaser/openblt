/************************************************************************************//**
* \file         Source/HCS12/can.c
* \brief        Bootloader CAN communication interface source file.
* \ingroup      Target_HCS12
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2013  by Feaser    http://www.feaser.com    All rights reserved
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
* Type definitions
****************************************************************************************/
/** \brief Structure type with the layout of a CAN reception message slot. */
typedef volatile struct
{
  volatile blt_int8u  idr[4];              /**< identifier register 0..3               */
  volatile blt_int8u  dsr[8];              /**< data segment register 0..7             */
  volatile blt_int8u  dlr;                 /**< data length register                   */
  volatile blt_int8u  dummy;               /**< unused                                 */
  volatile blt_int16u tstamp;              /**< timestamp register                     */
} tCanRxMsgSlot;

/** \brief Structure type with the layout of a CAN transmit message slot. */
typedef volatile struct
{
  volatile blt_int8u  idr[4];              /**< identifier register 0..3               */
  volatile blt_int8u  dsr[8];              /**< data segment register 0..7             */
  volatile blt_int8u  dlr;                 /**< data length register                   */
  volatile blt_int8u  tbpr;                /**< transmit buffer priority register      */
  volatile blt_int16u tstamp;              /**< timestamp register                     */
} tCanTxMsgSlot;

/** \brief Structure type with the layout of the CAN related control registers. */
typedef volatile struct
{
  volatile blt_int8u cctl0;                /**< control register 0                     */
  volatile blt_int8u cctl1;                /**< control register 1                     */
  volatile blt_int8u cbtr0;                /**< bus timing register 0                  */
  volatile blt_int8u cbtr1;                /**< bus timing register 1                  */
  volatile blt_int8u crflg;                /**< receiver flag register                 */
  volatile blt_int8u crier;                /**< receiver interrupt enable register     */
  volatile blt_int8u ctflg;                /**< transmitter flag register              */
  volatile blt_int8u ctier;                /**< transmitter interrupt enable register  */
  volatile blt_int8u ctarq;                /**< transmitter message abort control      */
  volatile blt_int8u ctaak;                /**< transmitter message abort control      */
  volatile blt_int8u ctbsel;               /**< transmit buffer selection              */
  volatile blt_int8u cidac;                /**< identifier acceptance control register */
  volatile blt_int8u dummy1[2];            /**< reserved (2)                           */
  volatile blt_int8u crxerr;               /**< receive error counter                  */
  volatile blt_int8u ctxerr;               /**< transmit error counter                 */
  volatile blt_int8u cidar0;               /**< identifier acceptance register 0       */
  volatile blt_int8u cidar1;               /**< identifier acceptance register 1       */
  volatile blt_int8u cidar2;               /**< identifier acceptance register 2       */
  volatile blt_int8u cidar3;               /**< identifier acceptance register 3       */
  volatile blt_int8u cidmr0;               /**< identifier mask register 0             */
  volatile blt_int8u cidmr1;               /**< identifier mask register 1             */
  volatile blt_int8u cidmr2;               /**< identifier mask register 2             */
  volatile blt_int8u cidmr3;               /**< identifier mask register 3             */
  volatile blt_int8u cidar4;               /**< identifier acceptance register 4       */
  volatile blt_int8u cidar5;               /**< identifier acceptance register 5       */
  volatile blt_int8u cidar6;               /**< identifier acceptance register 6       */
  volatile blt_int8u cidar7;               /**< identifier acceptance register 7       */
  volatile blt_int8u cidmr4;               /**< identifier mask register 4             */
  volatile blt_int8u cidmr5;               /**< identifier mask register 5             */
  volatile blt_int8u cidmr6;               /**< identifier mask register 6             */
  volatile blt_int8u cidmr7;               /**< identifier mask register 7             */
  volatile tCanRxMsgSlot rxSlot;           /**< foreground receive message slot        */
  volatile tCanTxMsgSlot txSlot;           /**< foreground transmit message slot       */
} tCanRegs;


/** \brief Structure type with the layout of the CAN bus timing registers. */
typedef struct
{
  blt_int8u tseg1;                         /**< CAN time segment 1                     */
  blt_int8u tseg2;                         /**< CAN time segment 2                     */
} tCanBusTiming;


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout for entering/leaving CAN initialization mode in milliseconds. */
#define CAN_INIT_TIMEOUT_MS            (250u)
/** \brief Timeout for transmitting a CAN message in milliseconds. */
#define CAN_MSG_TX_TIMEOUT_MS          (50u)

#if (BOOT_COM_CAN_CHANNEL_INDEX == 0)
/** \brief Set CAN base address to CAN0. */
#define CAN_REGS_BASE_ADDRESS  (0x0140)
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 1)
/** \brief Set CAN base address to CAN1. */
#define CAN_REGS_BASE_ADDRESS  (0x0180)
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 2)
/** \brief Set CAN base address to CAN2. */
#define CAN_REGS_BASE_ADDRESS  (0x01c0)
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 3)
/** \brief Set CAN base address to CAN3. */
#define CAN_REGS_BASE_ADDRESS  (0x0200)
#elif (BOOT_COM_CAN_CHANNEL_INDEX == 4)
/** \brief Set CAN base address to CAN4. */
#define CAN_REGS_BASE_ADDRESS  (0x0280)
#endif
/** \brief Macro for accessing the CAN related control registers. */
#define CAN                    ((volatile tCanRegs *)CAN_REGS_BASE_ADDRESS)
/** \brief Configures a CAN message id for 29-bit (extended). */
#define EXTIDMASK_BIT     (0x80000000)
/* macros for conveniently converting standard and extended message identifiers to the
 * format specified by the MSCAN message slot.
 */
#define CONVERT_STD_ID_TO_REG0(id) ((blt_int8u)(((blt_int16u)id & 0x07f8) >> 3))
#define CONVERT_STD_ID_TO_REG1(id) ((blt_int8u)(id & 0x07) << 5)
#define CONVERT_STD_ID_TO_REG2(id) (0)
#define CONVERT_STD_ID_TO_REG3(id) (0)
#define CONVERT_EXT_ID_TO_REG0(id) ((blt_int8u)(id >> 21))
#define CONVERT_EXT_ID_TO_REG1(id) ((((blt_int8u)(id >> 15)) & 0x07) |  \
                                   (((blt_int8u)(id >> 13)) & 0xe0) | (IDE_BIT))
#define CONVERT_EXT_ID_TO_REG2(id) ((blt_int8u)(((blt_int16u)id & 0x7f80) >> 7))
#define CONVERT_EXT_ID_TO_REG3(id) ((blt_int8u)(id & 0x7f) << 1)


/****************************************************************************************
* Register definitions
****************************************************************************************/
/** \brief Initialization mode request bit. */
#define INITRQ_BIT        (0x01)
/** \brief Initialization mode handshake bit. */
#define INITAK_BIT        (0x01)
/** \brief CAN controller enable bit. */
#define CANE_BIT          (0x80)
/** \brief Filter mode bit 0. */
#define IDAM0_BIT         (0x10)
/** \brief Filter mode bit 1. */
#define IDAM1_BIT         (0x20)
/** \brief Transmit buffer 0 select bit. */
#define TX0_BIT           (0x01)
/** \brief Transmit buffer 0 empty bit. */
#define TXE0_BIT          (0x01)
/** \brief 29-bit extended id bit. */
#define IDE_BIT           (0x08)
/** \brief Receive buffer full flag bit. */
#define RXF_BIT           (0x01)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static blt_bool CanGetSpeedConfig(blt_int16u baud, blt_int8u *btr0, blt_int8u *btr1);


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/**
* \brief     Array with possible time quanta configurations.
* \details   According to the CAN protocol 1 bit-time can be made up of between 8..25
*            time quanta (TQ). The total TQ in a bit is SYNC + TSEG1 + TSEG2 with SYNC
*            always being 1. The sample point is (SYNC + TSEG1) / (SYNC + TSEG1 + SEG2)
*            * 100%. This array contains possible and valid time quanta configurations
*            with a sample point between 68..78%.
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
** \brief     Initializes the CAN controller and synchronizes it to the CAN bus.
** \return    none.
**
****************************************************************************************/
void CanInit(void)
{
  blt_int8u btrRegValues[2];
  blt_bool  result;
  blt_int32u accept_code;
  blt_int32u accept_mask;
  blt_int32u timeout;

  /* the current implementation supports CAN0..4. throw an assertion error in case a
   * different CAN channel is configured.
   */
  ASSERT_CT((BOOT_COM_CAN_CHANNEL_INDEX >= 0) && (BOOT_COM_CAN_CHANNEL_INDEX <= 4));

  /* enter initialization mode. note that this automatically disables CAN interrupts */
  CAN->cctl0 = INITRQ_BIT;
  /* set timeout time for entering init mode */
  timeout = TimerGet() + CAN_INIT_TIMEOUT_MS;
  /* wait for initialization mode entry handshake from the hardware */
  while ((CAN->cctl1 & INITAK_BIT) == 0)
  {
    /* keep the watchdog happy */
    CopService();
    /* break loop upon timeout. this would indicate a hardware failure. */
    if (TimerGet() > timeout)
    {
      break;
    }
  }

  /* enable the CAN controller, disable wake up and listen modes and set the
   * crystal oscillator as the clock source.
   */
  CAN->cctl1 = CANE_BIT;

  /* configure baudrate */
  result = CanGetSpeedConfig(BOOT_COM_CAN_BAUDRATE/1000, &btrRegValues[0], &btrRegValues[1]);
  ASSERT_RT(result == BLT_TRUE);

  /* configure the baudrate */
  CAN->cbtr0 = btrRegValues[0];
  CAN->cbtr1 = btrRegValues[1];

  /* enable 2 32-bit acceptance filters. both will be configured for the same code and
   * mask. the only difference is that filter 0 will be setup to receive extended 29-bit
   * identifiers and filter 0 to receive standard 11-bit identifiers.
   */
  CAN->cidac &= ~(IDAM1_BIT | IDAM0_BIT);

  /* set the acceptance filter code and mask to a value that only BOOT_COM_CAN_RX_MSG_ID
   * is received.
   */
  accept_code = BOOT_COM_CAN_RX_MSG_ID;
  accept_mask = 0;

  /* configure acceptance filter 0 for 29-bit extended identifiers */
  CAN->cidar0 = CONVERT_EXT_ID_TO_REG0(accept_code);
  CAN->cidar1 = CONVERT_EXT_ID_TO_REG1(accept_code);
  CAN->cidar2 = CONVERT_EXT_ID_TO_REG2(accept_code);
  CAN->cidar3 = CONVERT_EXT_ID_TO_REG3(accept_code);
  CAN->cidmr0 = CONVERT_EXT_ID_TO_REG0(accept_mask);
  CAN->cidmr1 = (CONVERT_EXT_ID_TO_REG1(accept_mask) | 0x10) & ~IDE_BIT;
  CAN->cidmr2 = CONVERT_EXT_ID_TO_REG2(accept_mask);
  CAN->cidmr3 = CONVERT_EXT_ID_TO_REG3(accept_mask);

  /* configure acceptance filter 1 for 11-bit standard identifiers */
  CAN->cidar4 = CONVERT_STD_ID_TO_REG0(accept_code);
  CAN->cidar5 = CONVERT_STD_ID_TO_REG1(accept_code);
  CAN->cidar6 = CONVERT_STD_ID_TO_REG2(accept_code);
  CAN->cidar7 = CONVERT_STD_ID_TO_REG3(accept_code);
  CAN->cidmr4 = CONVERT_STD_ID_TO_REG0(accept_mask);
  CAN->cidmr5 = CONVERT_STD_ID_TO_REG1(accept_mask) | (0x04 | 0x02 | 0x01);
  CAN->cidmr6 = CONVERT_STD_ID_TO_REG2(accept_mask);
  CAN->cidmr7 = CONVERT_STD_ID_TO_REG3(accept_mask);

  /* leave initialization mode and synchronize to the CAN bus */
  CAN->cctl0 &= ~INITRQ_BIT;
  /* set timeout time for leaving init mode */
  timeout = TimerGet() + CAN_INIT_TIMEOUT_MS;
  /* wait for CAN bus synchronization handshake from the hardware */
  while ((CAN->cctl1 & INITAK_BIT) != 0)
  {
    /* keep the watchdog happy */
    CopService();
    /* break loop upon timeout. this would indicate a hardware failure. */
    if (TimerGet() > timeout)
    {
      break;
    }
  }

  /* bring transmit buffer 0 in the foreground as this is the only one used by this
   * driver.
   */
  CAN->ctbsel = TX0_BIT;
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
  blt_int8u byte_idx;
  blt_int32u txMsgId;
  blt_int32u timeout;

  /* double check that the transmit slot is really available */
  ASSERT_RT((CAN->ctflg & TXE0_BIT) != 0);

  /* is this a message with an 11-bit identifier? */
  if ((BOOT_COM_CAN_TX_MSG_ID & EXTIDMASK_BIT) == 0)
  {
    /* store the identifier */
    txMsgId = BOOT_COM_CAN_TX_MSG_ID;
    txMsgId &= ~EXTIDMASK_BIT;
    CAN->txSlot.idr[0] = CONVERT_STD_ID_TO_REG0(txMsgId);
    CAN->txSlot.idr[1] = CONVERT_STD_ID_TO_REG1(txMsgId);
    CAN->txSlot.idr[2] = CONVERT_STD_ID_TO_REG2(txMsgId);
    CAN->txSlot.idr[3] = CONVERT_STD_ID_TO_REG3(txMsgId);
  }
  else
  {
    /* store the identifier */
    txMsgId = BOOT_COM_CAN_TX_MSG_ID;
    txMsgId &= ~EXTIDMASK_BIT;
    CAN->txSlot.idr[0] = CONVERT_EXT_ID_TO_REG0(txMsgId);
    CAN->txSlot.idr[1] = CONVERT_EXT_ID_TO_REG1(txMsgId);
    CAN->txSlot.idr[2] = CONVERT_EXT_ID_TO_REG2(txMsgId);
    CAN->txSlot.idr[3] = CONVERT_EXT_ID_TO_REG3(txMsgId);
  }

  /* store the data length code */
  CAN->txSlot.dlr = len;

  /* store the message data */
  for (byte_idx=0; byte_idx<len; byte_idx++)
  {
    CAN->txSlot.dsr[byte_idx] = data[byte_idx];
  }

  /* start the transmission by clearing the buffer empty flag. must be done
   * by writing a 1 value.
   */
  CAN->ctflg = TXE0_BIT;

  /* set timeout time to wait for transmission completion */
  timeout = TimerGet() + CAN_MSG_TX_TIMEOUT_MS;

  /* wait for transmit completion */
  while ((CAN->ctflg & TXE0_BIT) == 0)
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
  blt_int8u  rxMsgLen;
  blt_int8u  byte_idx;
  blt_bool   result = BLT_FALSE;

  /* check if a new message was received */
  if ((CAN->crflg & RXF_BIT) == RXF_BIT)
  {
    /* check IDE-bit to determine if it is a 11-bit or 29-bit identifier */
    if ((CAN->rxSlot.idr[1] & IDE_BIT) == 0)
    {
      /* 11-bit id */
      rxMsgId = (*(blt_int16u *)(&CAN->rxSlot.idr[0])) >> 5;
    }
    else
    {
      /* 29-bit id */
      rxMsgId = (blt_int32u)(((*(blt_int32u *)(&CAN->rxSlot.idr[0])) & 0x0007ffff) >> 1) |
                (blt_int32u)(((*(blt_int32u *)(&CAN->rxSlot.idr[0])) & 0xffe00000) >> 3);
      rxMsgId |= EXTIDMASK_BIT;
    }
    /* is this the packet identifier? */
    if (rxMsgId == BOOT_COM_CAN_RX_MSG_ID)
    {
      result = BLT_TRUE;
      /* store the dlc */
      rxMsgLen = CAN->rxSlot.dlr & 0xf;
      /* copy message data */
      for (byte_idx=0; byte_idx<rxMsgLen; byte_idx++)
      {
        data[byte_idx] = CAN->rxSlot.dsr[byte_idx];
      }
      *len = rxMsgLen;
    }
    /* release the receive object by clearing the rx flag */
    CAN->crflg &= RXF_BIT;
  }
  return result;
} /*** end of CanReceivePacket ***/


/************************************************************************************//**
** \brief     Search algorithm to match the desired baudrate to a possible bus timing
**            configuration.
** \param     baud The desired baudrate in kbps. Valid values are 10..1000.
** \param     btr0 Pointer to where the value for register CANxBTR0 will be stored.
** \param     btr1 Pointer to where the value for register CANxBTR1 will be stored.
** \return    BLT_TRUE if the CAN bustiming register values were found, BLT_FALSE
**            otherwise.
**
****************************************************************************************/
static blt_bool CanGetSpeedConfig(blt_int16u baud, blt_int8u *btr0, blt_int8u *btr1)
{
  blt_int8u prescaler;
  blt_int8u cnt;

  /* loop through all possible time quanta configurations to find a match */
  for (cnt=0; cnt < sizeof(canTiming)/sizeof(canTiming[0]); cnt++)
  {
    if ((BOOT_CPU_XTAL_SPEED_KHZ % (baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1))) == 0)
    {
      /* compute the prescaler that goes with this TQ configuration */
      prescaler = (blt_int8u)(BOOT_CPU_XTAL_SPEED_KHZ/(baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1)));

      /* make sure the prescaler is valid */
      if ((prescaler > 0) && (prescaler <= 64))
      {
        /* store the MSCAN bustiming register values */
        *btr0 = prescaler - 1;
        *btr1 = ((canTiming[cnt].tseg2 - 1) << 4) | (canTiming[cnt].tseg1 - 1);
        /* found a good bus timing configuration */
        return BLT_TRUE;
      }
    }
    /* service the watchdog */
    CopService();
  }
  /* could not find a good bus timing configuration */
  return BLT_FALSE;
} /*** end of CanGetSpeedConfig ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of can.c **************************************/
