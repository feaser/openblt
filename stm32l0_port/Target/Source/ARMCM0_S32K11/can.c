/************************************************************************************//**
* \file         Source/ARMCM0_S32K11/can.c
* \brief        Bootloader CAN communication interface source file.
* \ingroup      Target_ARMCM0_S32K11
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2020  by Feaser    http://www.feaser.com    All rights reserved
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
#include "device_registers.h"                    /* device registers                   */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout for entering/leaving CAN initialization mode in milliseconds. */
#define CAN_INIT_TIMEOUT_MS            (250U)
/** \brief Timeout for transmitting a CAN message in milliseconds. */
#define CAN_MSG_TX_TIMEOUT_MS          (50U)

#if (BOOT_COM_CAN_CHANNEL_INDEX == 0)
/** \brief Set the peripheral CAN0 base pointer. */
#define CANx                           (CAN0)
/** \brief Set the PCC index offset for CAN0. */
#define PCC_FlexCANx_INDEX             (PCC_FlexCAN0_INDEX)
/** \brief Set the number of message boxes supported by CAN0. */
#define CANx_MAX_MB_NUM                (FEATURE_CAN0_MAX_MB_NUM)
#endif

/** \brief The mailbox used for transmitting the XCP respond message. */
#define CAN_TX_MSGBOX_NUM              (8U)
/** \brief The mailbox used for receiving the XCP command message. */
#define CAN_RX_MSGBOX_NUM              (9U)


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Structure type for grouping CAN bus timing related information. */
typedef struct t_can_bus_timing
{
  blt_int8u timeQuanta;                               /**< Total number of time quanta */
  blt_int8u propSeg;                                  /**< CAN propagation segment     */
  blt_int8u phaseSeg1;                                /**< CAN phase segment 1         */
  blt_int8u phaseSeg2;                                /**< CAN phase segment 2         */
} tCanBusTiming;


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief CAN bit timing table for dynamically calculating the bittiming settings.
 *  \details According to the CAN protocol 1 bit-time can be made up of between 8..25
 *           time quanta (TQ). The total TQ in a bit is SYNC + TSEG1 + TSEG2 with SYNC
 *           always being 1. The sample point is (SYNC + TSEG1) / (SYNC + TSEG1 + TSEG2)
 *           * 100%. This array contains possible and valid time quanta configurations
 *           with a sample point between 68..78%. A visual representation of the TQ in
 *           a bit is:
 *             | SYNCSEG |      TIME1SEG       | TIME2SEG  |
 *           Or with an alternative representation:
 *             | SYNCSEG | PROPSEG | PHASE1SEG | PHASE2SEG |
 *           With the alternative representation TIME1SEG = PROPSEG + PHASE1SEG.
 *
 */
static const tCanBusTiming canTiming[] =
{
  /* Time-Quanta | PROPSEG | PSEG1 | PSEG2 | Sample-Point */
  /* ---------------------------------------------------- */
  {  8U, 3U, 2U, 2U },         /*1+3+2+1=8 |  3  |  2  |  2  | 75% */
  {  9U, 3U, 3U, 2U },              /*   9 |  3  |  3  |  2  | 78% */
  { 10U, 3U, 3U, 3U },              /*  10 |  3  |  3  |  3  | 70% */
  { 11U, 4U, 3U, 3U },              /*  11 |  4  |  3  |  3  | 73% */
  { 12U, 4U, 4U, 3U },              /*  12 |  4  |  4  |  3  | 75% */
  { 13U, 5U, 4U, 3U },              /*  13 |  5  |  4  |  3  | 77% */
  { 14U, 5U, 4U, 4U },              /*  14 |  5  |  4  |  4  | 71% */
  { 15U, 6U, 4U, 4U },              /*  15 |  6  |  4  |  4  | 73% */
  { 16U, 6U, 5U, 4U },              /*  16 |  6  |  5  |  4  | 75% */
  { 17U, 7U, 5U, 4U },              /*  17 |  7  |  5  |  4  | 76% */
  { 18U, 7U, 5U, 5U },              /*  18 |  7  |  5  |  5  | 72% */
  { 19U, 8U, 5U, 5U },              /*  19 |  8  |  5  |  5  | 74% */
  { 20U, 8U, 6U, 5U },              /*  20 |  8  |  6  |  5  | 75% */
  { 21U, 8U, 7U, 5U },              /*  21 |  8  |  7  |  5  | 76% */
  { 22U, 8U, 7U, 6U },              /*  22 |  8  |  7  |  6  | 73% */
  { 23U, 8U, 8U, 6U },              /*  23 |  8  |  8  |  6  | 74% */
  { 24U, 8U, 8U, 7U },              /*  24 |  8  |  8  |  7  | 71% */
  { 25U, 8U, 8U, 8U }               /*  25 |  8  |  8  |  8  | 68% */
};


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Dummy variable to store the CAN controller's free running timer value in.
 *         This is needed at the end of a CAN message reception to unlock the mailbox
 *         again. If this variable is declared locally within the function, it generates
 *         an unwanted compiler warning about assigning a value and not using it.
 *         For this reason this dummy variabled is declare here as a module global.
 */
static volatile blt_int32u dummyTimerVal;


/************************************************************************************//**
** \brief     Search algorithm to match the desired baudrate to a possible bus
**            timing configuration.
** \param     baud The desired baudrate in kbps. Valid values are 10..1000.
** \param     prescaler Pointer to where the value for the prescaler will be stored.
** \param     busTimingCfg Pointer to where the bus timing values will be stored.
** \return    BLT_TRUE if the CAN bustiming register values were found, BLT_FALSE
**            otherwise.
**
****************************************************************************************/
static blt_bool CanGetSpeedConfig(blt_int16u baud, blt_int16u * prescaler,
                                  tCanBusTiming * busTimingCfg)
{
  blt_int8u  cnt;
  blt_int32u canClockFreqkHz;
  blt_int32u div2RegValue;
  blt_int8u  const div2DividerLookup[] =
  {
     0U, /* 0b000. Output disabled. */
     1U, /* 0b001. Divide by 1. */
     2U, /* 0b010. Divide by 2. */
     4U, /* 0b011. Divide by 4. */
     8U, /* 0b100. Divide by 8. */
    16U, /* 0b101. Divide by 16. */
    32U, /* 0b110. Divide by 32. */
    64U, /* 0b111. Divide by 64. */
  };

  /* Obtain the DIV2 divider value of the SOSC_CLK. */
  div2RegValue = (SCG->SOSCDIV & SCG_SOSCDIV_SOSCDIV2_MASK) >> SCG_SOSCDIV_SOSCDIV2_SHIFT;
  /* Check if the DIV2 register value for SOSC is 0. In this case SOSCDIV2_CLK is
   * currently disabled.
   */
  if (div2RegValue == 0U)
  {
    /* Configure the DIV2 for a default divide by 1 to make sure the SOSCDIV2_CLK is
     * actually enabled.
     */
    div2RegValue = 1U;
    SCG->SOSCDIV = SCG_SOSCDIV_SOSCDIV2(div2RegValue);
  }
  /* Determine the SOSC clock frequency. */
  canClockFreqkHz = BOOT_CPU_XTAL_SPEED_KHZ;
  /* Now process the configured DIV2 divider factor to get the actual frequency of the
   * CAN peripheral source clock.
   */
  canClockFreqkHz /= div2DividerLookup[div2RegValue];

  /* Loop through all possible time quanta configurations to find a match. */
  for (cnt=0; cnt < sizeof(canTiming)/sizeof(canTiming[0]); cnt++)
  {
    if ((canClockFreqkHz % (baud * canTiming[cnt].timeQuanta)) == 0U)
    {
      /* Compute the prescaler that goes with this TQ configuration. */
      *prescaler = canClockFreqkHz/(baud * canTiming[cnt].timeQuanta);
  
      /* Make sure the prescaler is valid. */
      if ((*prescaler > 0U) && (*prescaler <= 256U))
      {
        /* Store the bustiming configuration. */
        *busTimingCfg = canTiming[cnt];
        /* Found a good bus timing configuration. */
        return BLT_TRUE;
      }
    }
  }
  /* Could not find a good bus timing configuration. */
  return BLT_FALSE;
} /*** end of CanGetSpeedConfig ***/


/************************************************************************************//**
** \brief     Places the CAN controller in freeze mode. Note that the CAN controller
**            can only be placed in freeze mode, if it is actually enabled.
** \return    none.
**
****************************************************************************************/
static void CanFreezeModeEnter(void)
{
  blt_int32u timeout;

  /* This function should only be called with the module enabled. */
  ASSERT_RT((CANx->MCR & CAN_MCR_MDIS_MASK) == 0U);

  /* Request to enter freeze mode. */
  CANx->MCR = (CANx->MCR & ~CAN_MCR_FRZ_MASK)  | CAN_MCR_FRZ(1U);
  CANx->MCR = (CANx->MCR & ~CAN_MCR_HALT_MASK) | CAN_MCR_HALT(1U);
  /* Set timeout time for entering freeze mode. */
  timeout = TimerGet() + CAN_INIT_TIMEOUT_MS;
  /* Wait for freeze mode acknowledgement. */
  while (((CANx->MCR & CAN_MCR_FRZACK_MASK)) == 0U)
  {
    /* Keep the watchdog happy. */
    CopService();
    /* Break loop upon timeout. This would indicate a hardware failure. */
    if (TimerGet() > timeout)
    {
      break;
    }
  }
} /*** end of CanFreezeModeEnter ***/


/************************************************************************************//**
** \brief     Leaves the CAN controller's freeze mode. Note that this operation can
**            only be done, if it is actually enabled.
** \return    none.
**
****************************************************************************************/
static void CanFreezeModeExit(void)
{
  blt_int32u timeout;

  /* This function should only be called with the module enabled. */
  ASSERT_RT((CANx->MCR & CAN_MCR_MDIS_MASK) == 0U);

  /* Request to leave freeze mode. */
  CANx->MCR = (CANx->MCR & ~CAN_MCR_FRZ_MASK)  | CAN_MCR_FRZ(0U);
  CANx->MCR = (CANx->MCR & ~CAN_MCR_HALT_MASK) | CAN_MCR_HALT(0U);
  /* Set timeout time for leaving freeze mode. */
  timeout = TimerGet() + CAN_INIT_TIMEOUT_MS;
  /* Wait for non freeze mode acknowledgement. */
  while (((CANx->MCR & CAN_MCR_FRZACK_MASK)) != 0U)
  {
    /* Keep the watchdog happy. */
    CopService();
    /* Break loop upon timeout. This would indicate a hardware failure. */
    if (TimerGet() > timeout)
    {
      break;
    }
  }
} /*** end of CanFreezeModeExit ***/


/************************************************************************************//**
** \brief     Places the CAN controller in disabled mode.
** \return    none.
**
****************************************************************************************/
static void CanDisabledModeEnter(void)
{
  blt_int32u timeout;

  /* Only continue if the CAN controller is currently enabled. */
  if ((CANx->MCR & CAN_MCR_MDIS_MASK) == 0U)
  {
    /* Request disabled mode. */
    CANx->MCR = (CANx->MCR & ~CAN_MCR_MDIS_MASK) | CAN_MCR_MDIS(1U);
    /* Set timeout time for entering disabled mode. */
    timeout = TimerGet() + CAN_INIT_TIMEOUT_MS;
    /* Wait for disabled mode acknowledgement. */
    while (((CANx->MCR & CAN_MCR_LPMACK_MASK)) == 0U)
    {
      /* Keep the watchdog happy. */
      CopService();
      /* Break loop upon timeout. This would indicate a hardware failure. */
      if (TimerGet() > timeout)
      {
        break;
      }
    }
  }
} /*** end of CanDisabledModeEnter ***/

/************************************************************************************//**
** \brief     Places the CAN controller in enabled mode.
** \return    none.
**
****************************************************************************************/
static void CanDisabledModeExit(void)
{
  blt_int32u timeout;

  /* Only continue if the CAN controller is currently disabled. */
  if ((CANx->MCR & CAN_MCR_MDIS_MASK) != 0U)
  {
    /* Request enabled mode. */
    CANx->MCR = (CANx->MCR & ~CAN_MCR_MDIS_MASK) | CAN_MCR_MDIS(0U);
    /* Set timeout time for leaving disabled mode. */
    timeout = TimerGet() + CAN_INIT_TIMEOUT_MS;
    /* Wait for disabled mode acknowledgement. */
    while (((CANx->MCR & CAN_MCR_LPMACK_MASK)) != 0U)
    {
      /* Keep the watchdog happy. */
      CopService();
      /* Break loop upon timeout. This would indicate a hardware failure. */
      if (TimerGet() > timeout)
      {
        break;
      }
    }
  }
} /*** end of CanDisabledModeExit ***/


/************************************************************************************//**
** \brief     Initializes the CAN controller and synchronizes it to the CAN bus.
** \return    none.
**
****************************************************************************************/
void CanInit(void)
{
  blt_int16u    prescaler = 0;
  tCanBusTiming timingCfg = { 0 };
  blt_int8u     rjw;
  blt_int16u    idx;
  blt_int32u    timeout;
  blt_int32u    rxMsgId  = BOOT_COM_CAN_RX_MSG_ID;

  /* Perform compile time assertion to check that the configured CAN channel is actually
   * supported by this driver.
   */
  ASSERT_CT(BOOT_COM_CAN_CHANNEL_INDEX == 0);

  /* Verify the correct configuration of the transmit and receive mailboxes. */
  ASSERT_CT(CAN_TX_MSGBOX_NUM < CANx_MAX_MB_NUM);
  ASSERT_CT(CAN_RX_MSGBOX_NUM < CANx_MAX_MB_NUM);

  /* Enable the CAN peripheral clock. */
  PCC->PCCn[PCC_FlexCANx_INDEX] |= PCC_PCCn_CGC_MASK;

  /* The source clock needs to be configured first. For this the CAN controller must be
   * in disabled mode, but that can only be entered after first entering freeze mode,
   * which in turn can only be in enabled mode. So first enable the module, then goto
   * freeze mode and finally enter disabled mode.
   */
  CanDisabledModeExit();
  CanFreezeModeEnter();
  CanDisabledModeEnter();
  /* Configure SOSCDIV2 as the source clock. This assumes that an external oscillator
   * is available, which is typically the case to meet the clock tolerance requirements
   * of the CAN 2.0B secification.
   */
  CANx->CTRL1 &= ~CAN_CTRL1_CLKSRC_MASK;
  /* Leave disabled mode. */
  CanDisabledModeExit();
  /* Make sure freeze mode is active to be able to initialize the CAN controller. */
  CanFreezeModeEnter();

  /* Obtain bittiming configuration information. */
  if (CanGetSpeedConfig(BOOT_COM_CAN_BAUDRATE/1000, &prescaler, &timingCfg) == BLT_FALSE)
  {
    /* Incorrect configuration. The specified baudrate is not supported for the given
     * clock configuration. Verify the following settings in blt_conf.h:
     *   - BOOT_COM_CAN_BAUDRATE
     *   - BOOT_CPU_XTAL_SPEED_KHZ
     *   - BOOT_CPU_SYSTEM_SPEED_KHZ
     */
    ASSERT_RT(BLT_FALSE);
  }

  /* Reset the current bittiming configuration. */
  CANx->CTRL1 &= ~(CAN_CTRL1_PRESDIV_MASK | CAN_CTRL1_PROPSEG_MASK |
                   CAN_CTRL1_PSEG1_MASK | CAN_CTRL1_PSEG2_MASK | CAN_CTRL1_RJW_MASK |
                   CAN_CTRL1_SMP_MASK);
  /* Configure the baudrate prescaler. */
  CANx->CTRL1 |= CAN_CTRL1_PRESDIV(prescaler - 1U);
  /* Configure the propagation segment. */
  CANx->CTRL1 |= CAN_CTRL1_PROPSEG(timingCfg.propSeg - 1U);
  /* Configure the phase segments. */
  CANx->CTRL1 |= CAN_CTRL1_PSEG1(timingCfg.phaseSeg1 - 1U);
  CANx->CTRL1 |= CAN_CTRL1_PSEG2(timingCfg.phaseSeg2 - 1U);
  /* The resynchronization jump width (RJW) can be 1 - 4 TQ, yet should never be larger
   * than pseg1. Configure the longest possible value for RJW.
   */
  rjw = (timingCfg.phaseSeg1 < 4) ? timingCfg.phaseSeg1 : 4;
  CANx->CTRL1 |= CAN_CTRL1_RJW(rjw - 1U);
  /* All the entries in canTiming[] have a PSEG1 >= 2, so three samples can be used to
   * determine the value of the received bit, instead of the default one.
   */
  CANx->CTRL1 |= CAN_CTRL1_SMP(1U);

  /* Clear the message box RAM. Each message box covers 4 words (1 word = 32-bits. */
  for (idx = 0; idx < (CANx_MAX_MB_NUM * 4U); idx++)
  {
    CANx->RAMn[idx] = 0U;
  }
  /* Clear the reception mask register for each message box. */
  for (idx = 0; idx < CANx_MAX_MB_NUM; idx++)
  {
    CANx->RXIMR[idx] = 0U;
  }
  /* Configure the maximum number of message boxes. */
  CANx->MCR = (CANx->MCR & ~CAN_MCR_MAXMB_MASK) | CAN_MCR_MAXMB(CANx_MAX_MB_NUM - 1U);
  /* Disable the self reception feature. */
  CANx->MCR = (CANx->MCR & ~CAN_MCR_SRXDIS_MASK) | CAN_MCR_SRXDIS(1U);

  /* Enable individual reception masking. This disables the legacy support for the
   * global reception mask and the mailbox 14/15 individual reception mask.
   */
  CANx->MCR = (CANx->MCR & ~CAN_MCR_IRMQ_MASK) | CAN_MCR_IRMQ(1U);
  /* Disable the reception FIFO. This driver only needs to receive one CAN message
   * identifier. It is sufficient to use just one dedicated mailbox for this.
   */
  CANx->MCR &= ~CAN_MCR_RFEN_MASK;
  /* Configure the mask of the invididual message reception mailbox to check all ID bits
   * and also the IDE bit.
   */
  CANx->RXIMR[CAN_RX_MSGBOX_NUM] = 0x40000000U | 0x1FFFFFFFU;
  /* Configure the reception mailbox to receive just the CAN message configured with
   * BOOT_COM_CAN_RX_MSG_ID.
   *   EDL, BRS, ESI=0: CANFD not used.
   *   CODE=0b0100: mailbox set to active and empty.
   *   IDE=0: 11-bit CAN identifier.
   *   SRR, RTR, TIME STAMP=0: not applicable.
   */
  CANx->RAMn[(CAN_RX_MSGBOX_NUM * 4U) + 0U] = 0x04000000;
  /* Store the message identifier to receive in the mailbox RAM. */
  if ((rxMsgId & 0x80000000U) != 0U)
  {
    /* It is a 29-bit extended CAN identifier. */
    rxMsgId &= ~0x80000000U;
    /* Set the IDE bit to configure the message for a 29-bit identifier. */
    CANx->RAMn[(CAN_RX_MSGBOX_NUM * 4U) + 0U] |= CAN_WMBn_CS_IDE_MASK;
    /* Store the 29-bit CAN identifier. */
    CANx->RAMn[(CAN_RX_MSGBOX_NUM * 4U) + 1U] = CAN_WMBn_ID_ID(rxMsgId);
  }
  else
  {
    /* Store the 11-bit CAN identifier. */
   CANx->RAMn[(CAN_RX_MSGBOX_NUM * 4U) + 1U] = CAN_WMBn_ID_ID(rxMsgId << 18U);
  }

  /* Disable all message box interrupts. */
  CANx->IMASK1 = 0U;
  /* Clear all mesasge box interrupt flags. */
  CANx->IFLAG1 = CAN_IMASK1_BUF31TO0M_MASK;
  /* Clear all error interrupt flags */
  CANx->ESR1 = CAN_ESR1_ERRINT_MASK | CAN_ESR1_BOFFINT_MASK | CAN_ESR1_RWRNINT_MASK |
               CAN_ESR1_TWRNINT_MASK | CAN_ESR1_BOFFDONEINT_MASK |
               CAN_ESR1_ERRINT_FAST_MASK | CAN_ESR1_ERROVR_MASK;

  /* Switch to normal user mode. */
  CANx->MCR &= ~CAN_MCR_SUPV_MASK;
  CANx->CTRL1 &= ~(CAN_CTRL1_LOM_MASK | CAN_CTRL1_LPB_MASK);
  /* Exit freeze mode. */
  CanFreezeModeExit();
  /* Set timeout time for entering normal user mode. */
  timeout = TimerGet() + CAN_INIT_TIMEOUT_MS;
  /* Wait for normal user mode acknowledgement. */
  while (((CANx->MCR & CAN_MCR_NOTRDY_MASK)) != 0U)
  {
    /* Keep the watchdog happy. */
    CopService();
    /* Break loop upon timeout. This would indicate a hardware failure. */
    if (TimerGet() > timeout)
    {
      break;
    }
  }
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
  blt_int32u   timeout;
  blt_bool     isExtId = BLT_FALSE;
  blt_int32u   txMsgId = BOOT_COM_CAN_TX_MSG_ID;
  blt_int8u  * pMsgBoxData;
  blt_int8u    byteIdx;

  /* Prepare information about the message identifier. */
  if ((txMsgId & 0x80000000U) != 0U)
  {
    /* It is a 29-bit extended CAN identifier. */
    txMsgId &= ~0x80000000U;
    isExtId = BLT_TRUE;
  }

  /* Clear the mailbox interrupt flag by writing a 1 to the corresponding box. */
  CANx->IFLAG1 = (1U << CAN_TX_MSGBOX_NUM);

  /* Prepare the mailbox RAM for a basic CAN message.
   *  EDL,BRS,ESI=0: CANFD not used.
   */
  CANx->RAMn[(CAN_TX_MSGBOX_NUM * 4U) + 0U] &= ~0xE0000000U;
  /* Configure SRR, IDE, RTR bits for a standard 11-bit transmit frame. */
  CANx->RAMn[(CAN_TX_MSGBOX_NUM * 4U) + 0U] &= ~(CAN_WMBn_CS_IDE_MASK |
                                                 CAN_WMBn_CS_RTR_MASK);
  CANx->RAMn[(CAN_TX_MSGBOX_NUM * 4U) + 0U] |= CAN_WMBn_CS_SRR_MASK;
  /* Configure the DLC. */
  CANx->RAMn[(CAN_TX_MSGBOX_NUM * 4U) + 0U] &= ~CAN_WMBn_CS_DLC_MASK;
  CANx->RAMn[(CAN_TX_MSGBOX_NUM * 4U) + 0U] |= CAN_WMBn_CS_DLC(len);
  /* Write the data bytes of the CAN message to the mailbox RAM. */
  pMsgBoxData = (blt_int8u * )(&CANx->RAMn[(CAN_TX_MSGBOX_NUM * 4U) + 2U]);
  for (byteIdx = 0; byteIdx < len; byteIdx++)
  {
    pMsgBoxData[((byteIdx) & ~3U) + (3U - ((byteIdx) & 3U))] = data[byteIdx];
  }
  /* Store the CAN message identifier in the mailbox RAM. */
  if (isExtId == BLT_FALSE)
  {
     /* Store the 11-bit CAN identifier. */
    CANx->RAMn[(CAN_TX_MSGBOX_NUM * 4U) + 1U] = CAN_WMBn_ID_ID(txMsgId << 18U);
  }
  else
  {
    /* Set the IDE bit to configure the message for a 29-bit identifier. */
    CANx->RAMn[(CAN_TX_MSGBOX_NUM * 4U) + 0U] |= CAN_WMBn_CS_IDE_MASK;
    /* Store the 29-bit CAN identifier. */
    CANx->RAMn[(CAN_TX_MSGBOX_NUM * 4U) + 1U] = CAN_WMBn_ID_ID(txMsgId);
  }
  /* Activate the mailbox to start the transmission by writing 0x0C to the CODE field. */
  CANx->RAMn[(CAN_TX_MSGBOX_NUM * 4U) + 0U] |= (0x0CU << 24U) & 0x0F000000U;

  /* Determine timeout time for the transmit completion. */
  timeout = TimerGet() + CAN_MSG_TX_TIMEOUT_MS;
  /* Poll for completion of the transmit operation. */
  while ((CANx->IFLAG1 & (1U << CAN_TX_MSGBOX_NUM)) == 0U)
  {
    /* Service the watchdog. */
    CopService();
    /* Break loop upon timeout. this would indicate a hardware failure or no other
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
  blt_bool    result = BLT_FALSE;
  blt_int8u * pMsgBoxData;
  blt_int8u   byteIdx;

  /* Check if a message was received in the individual mailbox configured to receive
   * the BOOT_COM_CAN_RX_MSG_ID message.
   */
  if ((CANx->IFLAG1 & (1U << CAN_RX_MSGBOX_NUM)) != 0U)
  {
    /* Note that there is no need to verify the identifier of the CAN message because the
     * mailbox is configured to only receive the BOOT_COM_CAN_TX_MSG_ID message. Start
     * by reading out the DLC of the newly received CAN message.
     */
    *len = (CANx->RAMn[(CAN_RX_MSGBOX_NUM * 4U) + 0U] & CAN_WMBn_CS_DLC_MASK) >> CAN_WMBn_CS_DLC_SHIFT;
    /* Read the data bytes of the CAN message from the mailbox RAM. */
    pMsgBoxData = (blt_int8u *)(&CANx->RAMn[(CAN_RX_MSGBOX_NUM * 4U) + 2U]);
    for (byteIdx = 0; byteIdx < *len; byteIdx++)
    {
      data[byteIdx] = pMsgBoxData[((byteIdx) & ~3U) + (3U - ((byteIdx) & 3U))];
    }
    /* Clear the mailbox interrupt flag by writing a 1 to the corresponding box. */
    CANx->IFLAG1 = (1U << CAN_RX_MSGBOX_NUM);
    /* Read the free running timer to unlock the mailbox. */
    dummyTimerVal = CANx->TIMER;
    /* Update the result. */
    result = BLT_TRUE;
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of CanReceivePacket ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of can.c **************************************/
