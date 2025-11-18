/************************************************************************************//**
* \file         Source/ARMCM33_STM32U5/can.c
* \brief        Bootloader CAN communication interface source file.
* \ingroup      Target_ARMCM33_STM32U5
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
#include "stm32u5xx.h"                           /* STM32 CPU and HAL header           */
#include "stm32u5xx_ll_rcc.h"                    /* STM32 LL RCC header                */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout for transmitting a CAN message in milliseconds. */
#define CAN_MSG_TX_TIMEOUT_MS          (50u)

/* map the configured CAN channel index to the STM32's CAN peripheral */
#if (BOOT_COM_CAN_CHANNEL_INDEX == 0)
/** \brief Set CAN base address to CAN1. */
#define CAN_CHANNEL   FDCAN1
#endif


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Structure type for grouping together CAN peripheral parameters needed for
 *         performing bittiming calculations.
 */
typedef struct t_can_periph_params
{
  blt_int32u base_freq;                           /**< Source clock frequency [Hz]     */
  blt_int16u prescaler_min;                       /**< Smallest supported prescaler    */
  blt_int16u prescaler_max;                       /**< Largest supported prescaler     */
  blt_int16u tseg1_min;                           /**< Smallest supported Tseg1        */
  blt_int16u tseg1_max;                           /**< Largest supported Tseg1         */
  blt_int16u tseg2_min;                           /**< Smallest supported Tseg2        */
  blt_int16u tseg2_max;                           /**< Largest supported Tseg2         */
} tCanPeriphParams;

/** \brief Structure type for grouping CAN bit timing configuration information. */
typedef struct
{
  blt_int16u prescaler;                           /**< CAN clock prescaler             */
  blt_int16u tseg1;                               /**< CAN time segment 1 (excl. SYNC) */
  blt_int16u tseg2;                               /**< CAN time segment 2              */
  blt_int16u sjw;                                 /**< CAN synchronization jump width  */
} tCanBitTimingConfig;


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief CAN handle to be used in API calls. */
static FDCAN_HandleTypeDef canHandle;

#if (BOOT_COM_CAN_FD_ENABLE > 0)
/** \brief Boolean flag to determine if the bitrate switch feature is used for CAN FD. */
static blt_bool canFdBitrateSwitchUsed;
#endif


/************************************************************************************//**
** \brief     Search algorithm to match the desired baudrate to a possible bit
**            timing configuration, taking into account the given CAN peripheral
**            parameters.
** \param     baud The desired baudrate in bps. Valid values are 10 kbps ..8000 Mbps.
** \param     periph_params CAN peripheral parameters.
** \param     bittiming_config Found bit timing configuration.
** \return    BLT_TRUE if a matching CAN bittiming configuration was found, BLT_FALSE
**            otherwise.
**
****************************************************************************************/
static blt_bool CanCalculateBitTimingConfig(blt_int32u const baud,
                                            tCanPeriphParams const * periph_params,
                                            tCanBitTimingConfig * bittiming_config)
{
  blt_bool   result = BLT_FALSE;
  blt_int16u bitTq, bitTqMin, bitTqMax;
  blt_int16u tseg1, tseg2;
  blt_int16u prescaler;
  blt_int8u  samplePoint;

  /* only continue with valid parameters. */
  if ( (baud >= 10000UL) && (baud <= 8000000UL) && (periph_params != BLT_NULL) &&
       (bittiming_config != BLT_NULL) )
  {
    /* calculate minimum and maximum possible time quanta per bit. remember that the
     * bittime in time quanta is 1 (sync) + tseg1 + tseg2.
     */
    bitTqMin = 1U + periph_params->tseg1_min + periph_params->tseg2_min;
    bitTqMax = 1U + periph_params->tseg1_max + periph_params->tseg2_max;

    /* loop through all the prescaler values from low to high to find one that results
     * in a time quanta per bit that is in the range bitTqMin .. bitTqMax. note that
     * looping through the prescalers low to high is important, because a lower prescaler
     * would result in a large number of time quanta per bit. this in turns gives you
     * more flexibility for setting the a bit's sample point.
     */
    for (prescaler = periph_params->prescaler_min;
         prescaler <= periph_params->prescaler_max; prescaler++)
    {
      /* no need to continue if the configured peripheral clock, scaled down by this
       * prescaler value, is no longer high enough to get to the desired baudrate,
       * taking into account the minimum possible time quanta per bit.
       */
      if (periph_params->base_freq < (baud * bitTqMin))
      {
        break;
      }
      /* does this prescaler give a fixed (integer) number of time quanta? */
      if ( ((periph_params->base_freq % prescaler) == 0U) &&
           (((periph_params->base_freq / prescaler) % baud) == 0U) )
      {
        /* calculate how many time quanta per bit this prescaler would give. */
        bitTq = (periph_params->base_freq / prescaler) / baud;
        /* is this a configurable amount of time quanta? */
        if ( (bitTq >= bitTqMin) && (bitTq <= bitTqMax) )
        {
          /* if the sample point is at the very end of the bit time, the maximum
           * possible network length can be achieved. an earlier sample point reduces
           * the achievable network length, but increases robustness. as a reference,
           * a value of higher than 80% is not recommended for automotive applications
           * due to robustness reasons.
           *
           * for this reason try to get a sample point that is in the 65% - 80% range.
           * an efficient way of doing this is to calculate Tseg2, which should then
           * be 20% and round up by doing this calculation.
           *
           * example:
           *   baud      = 500 kbits/sec
           *   base_freq = 24 MHz
           *   prescaler = 1
           *   bitTq     = 48
           *
           *   for 80% sample point, Tseg2 should be 20% of 48 = 9.6 time quanta.
           *   rounded up to the next integer = 10. resulting sample point:
           *   ((48 - 10) / 48) * 100 = 79.17%
           */
          tseg2 = ((bitTq * 2U) + 9U) / 10U;
          /* calculate Tseg1 by deducting Tseg2 and 1 time quanta for the sync seq. */
          tseg1 = (bitTq - tseg2) - 1U;
          /* are these values within configurable range? */
          if ( (tseg1 >= periph_params->tseg1_min) &&
               (tseg1 <= periph_params->tseg1_max) &&
               (tseg2 >= periph_params->tseg2_min) &&
               (tseg2 <= periph_params->tseg2_max) )
          {
            /* calculate the actual sample point, given these Tseg values. */
            samplePoint = (blt_int8u)(((1U + tseg1) * 100UL) / bitTq);
            /* is this within the targeted 65% - 80% range? */
            if ( (samplePoint >= 65U) && (samplePoint <= 80U) )
            {
              /* store these bittiming settings. */
              bittiming_config->prescaler = prescaler;
              bittiming_config->tseg1 = tseg1;
              bittiming_config->tseg2 = tseg2;
              /* SJW depends highly on the baudrate tolerances of the other nodes on the
               * network. SJW 1 allows for only a small window of tolerance between node
               * baudrate. SJW = TSEG2 allows for a large winow, at the risk of a bit
               * being incorrectly sampled. a safe approach to to use TSEG2/2 but also
               * make sure SJW is > 0.
               */
              bittiming_config->sjw = bittiming_config->tseg2 / 2U;
              if (bittiming_config->sjw == 0U)
              {
                bittiming_config->sjw = 1U;
              }
              /* set the result to success. */
              result = BLT_TRUE;
              /* all done so no need to continue the loop. */
              break;
            }
          }
        }
      }
    }
  }

  /* give the result back to the caller. */
  return result;
} /*** end of CanCalculateBitTimingConfig ***/


/************************************************************************************//**
** \brief     Initializes the CAN controller and synchronizes it to the CAN bus.
** \details   It is up to the user to configure the desired CAN clock source. By default
**            it is HSE. Alternatives are PLL1Q or PLL2P. To meet the clock tolerance
**            requirement of CAN 2.0B, an external crystal oscillator (HSE) is
**            recommended. A multiple of 8 MHz gives the best support for most commonly
**            used CAN baudrates, so that is recommended.
** \return    none.
**
****************************************************************************************/
void CanInit(void)
{
  blt_int32u          rxMsgId = BOOT_COM_CAN_RX_MSG_ID;
  FDCAN_FilterTypeDef filterConfig;
  tCanPeriphParams    periphParams;
  tCanBitTimingConfig bittimingConfig = { 0 };
#if (BOOT_COM_CAN_FD_ENABLE > 0)
  tCanBitTimingConfig bittimingConfigBRS = { 0 };
#endif

  /* the current implementation supports CAN1. throw an assertion error in case a
   * different CAN channel is configured.
   */
  ASSERT_CT(BOOT_COM_CAN_CHANNEL_INDEX == 0);

  /* set the CAN controller channel, which is needed by HAL_FDCAN_MspInit(). */
  canHandle.Instance = CAN_CHANNEL;
  /* call HAL_FDCAN_MspInit() here already, even though this is done automatically later
   * on in the call to HAL_FDCAN_Init(). HAL_FDCAN_MspInit() configures the source clock,
   * which should be done first, otherwise the call to LL_RCC_GetFDCANClockFreq() to
   * obtain the FDCAN clock frequency returns an incorrect value. the FDCAN clock
   * frequency is needed to properly calculate the bittiming, which is turn needs to be
   * done before calling HAL_FDCAN_Init(). explicitly calling HAL_FDCAN_MspInit() here
   * one extra time works around this chicken and egg type problem. when CubeMX is not
   * used in a project and therefore HAL_FDCAN_MspInit() is not generated, it still works
   * because the weak HAL_FDCAN_MspInit() is then called.
   */
  HAL_FDCAN_MspInit(&canHandle);

#if (BOOT_COM_CAN_FD_ENABLE > 0)
  /* determine if the bitrate switch feature should be used. note that you cannot use
   * BOOT_COM_CAN_FD_BRS_BAUDRATE in a preprocessor compile-time check, because this
   * macro might be set to a function call, instead of just a baudrate value.
   */
  if (BOOT_COM_CAN_FD_BRS_BAUDRATE > 0)
  {
    canFdBitrateSwitchUsed = BLT_TRUE;
  }
  else
  {
    canFdBitrateSwitchUsed = BLT_FALSE;
  }

  /* calculate the data bittiming configuration with enabled bitrate switch feature. */
  if (canFdBitrateSwitchUsed == BLT_TRUE)
  {
    /* store CAN peripheral parameters related to the data bittiming configuration. */
    periphParams.base_freq = LL_RCC_GetFDCANClockFreq(LL_RCC_FDCAN_CLKSOURCE);
    periphParams.prescaler_min = 1U;
    periphParams.prescaler_max = 32U;
    periphParams.tseg1_min = 1U;
    periphParams.tseg1_max = 32U;
    periphParams.tseg2_min = 1U;
    periphParams.tseg2_max = 16U;
    /* attempt to find a matching data bittiming configuration. */
    if (CanCalculateBitTimingConfig(BOOT_COM_CAN_FD_BRS_BAUDRATE, &periphParams,
                                    &bittimingConfigBRS) == BLT_FALSE)
    {
      /* Incorrect configuration. The specified baudrate is not supported for the given
       * clock configuration. Verify the following settings in blt_conf.h:
       *   - BOOT_COM_CAN_FD_BRS_BAUDRATE
       *   - BOOT_CPU_XTAL_SPEED_KHZ
       *   - BOOT_CPU_SYSTEM_SPEED_KHZ
       */
      ASSERT_RT(BLT_FALSE);
    }
  }
#endif

  /* store CAN peripheral parameters related to the nominal bittiming configuration. */
  periphParams.base_freq = LL_RCC_GetFDCANClockFreq(LL_RCC_FDCAN_CLKSOURCE);
  periphParams.prescaler_min = 1U;
  periphParams.prescaler_max = 512U;
  periphParams.tseg1_min = 1U;
  periphParams.tseg1_max = 256U;
  periphParams.tseg2_min = 1U;
  periphParams.tseg2_max = 128U;
  /* attempt to find a matching nominal bittiming configuration. */
  if (CanCalculateBitTimingConfig(BOOT_COM_CAN_BAUDRATE, &periphParams,
                                  &bittimingConfig) == BLT_FALSE)
  {
    /* Incorrect configuration. The specified baudrate is not supported for the given
     * clock configuration. Verify the following settings in blt_conf.h:
     *   - BOOT_COM_CAN_BAUDRATE
     *   - BOOT_CPU_XTAL_SPEED_KHZ
     *   - BOOT_CPU_SYSTEM_SPEED_KHZ
     */
    ASSERT_RT(BLT_FALSE);
  }

  /* set the CAN controller configuration. */
  canHandle.Init.ClockDivider = FDCAN_CLOCK_DIV1;
  canHandle.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  canHandle.Init.Mode = FDCAN_MODE_NORMAL;
  canHandle.Init.AutoRetransmission = ENABLE;
  canHandle.Init.TransmitPause = ENABLE;
  canHandle.Init.ProtocolException = DISABLE;
  canHandle.Init.NominalPrescaler = bittimingConfig.prescaler;
  canHandle.Init.NominalTimeSeg1 = bittimingConfig.tseg1;
  canHandle.Init.NominalTimeSeg2 = bittimingConfig.tseg2;
  canHandle.Init.NominalSyncJumpWidth = bittimingConfig.sjw;
  canHandle.Init.DataPrescaler = 1;
  canHandle.Init.DataSyncJumpWidth = 1;
  canHandle.Init.DataTimeSeg1 = 1;
  canHandle.Init.DataTimeSeg2 = 1;
#if (BOOT_COM_CAN_FD_ENABLE > 0)
  if (canFdBitrateSwitchUsed == BLT_FALSE)
  {
    canHandle.Init.FrameFormat = FDCAN_FRAME_FD_NO_BRS;
  }
  else
  {
    canHandle.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
    canHandle.Init.DataPrescaler = bittimingConfigBRS.prescaler;
    canHandle.Init.DataTimeSeg1 = bittimingConfigBRS.tseg1;
    canHandle.Init.DataTimeSeg2 = bittimingConfigBRS.tseg2;
    canHandle.Init.DataSyncJumpWidth = bittimingConfigBRS.sjw;
  }
#endif
  /* does the message to be received have a standard 11-bit CAN identifier? */
  if ((rxMsgId & 0x80000000) == 0)
  {
    canHandle.Init.StdFiltersNbr = 1;
    canHandle.Init.ExtFiltersNbr = 0;
  }
  else
  {
    canHandle.Init.StdFiltersNbr = 0;
    canHandle.Init.ExtFiltersNbr = 1;
  }
  canHandle.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  /* initialize the CAN controller. this only fails if the CAN controller hardware is
   * faulty. no need to evaluate the return value as there is nothing we can do about
   * a faulty CAN controller.
   */
  (void)HAL_FDCAN_Init(&canHandle);

  /* configure the reception filter. note that the implementation of this function
   * always returns HAL_OK as long as the CAN controller is initialized, so no need to
   * evaluate the return value.
   */
  if ((rxMsgId & 0x80000000) == 0)
  {
    filterConfig.IdType = FDCAN_STANDARD_ID;
  }
  else
  {
    filterConfig.IdType = FDCAN_EXTENDED_ID;
    /* negate the ID-type bit */
    rxMsgId &= ~0x80000000;
  }
  filterConfig.FilterIndex = 0;
  filterConfig.FilterType = FDCAN_FILTER_DUAL;
  filterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  filterConfig.FilterID1 = rxMsgId;
  filterConfig.FilterID2 = rxMsgId;
  (void)HAL_FDCAN_ConfigFilter(&canHandle, &filterConfig);

  /* configure global filter to reject all non-matching frames. */
  HAL_FDCAN_ConfigGlobalFilter(&canHandle, FDCAN_REJECT, FDCAN_REJECT, 
                               FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
  
#if (BOOT_COM_CAN_FD_ENABLE > 0)
  if (canFdBitrateSwitchUsed == BLT_TRUE)
  {
    /* configure and enable the transmit delay compensation, which is required if the
     * bitrate switch feature is enabled. recommended settings:
     * - offset: DataTimeSeg1 * DataPrescaler
     * - filter: 0
     */
    (void)HAL_FDCAN_ConfigTxDelayCompensation(&canHandle, bittimingConfigBRS.tseg1 *
                                              bittimingConfigBRS.prescaler, 0U);
  }
#endif

  /* start the CAN peripheral. no need to evaluate the return value as there is nothing
   * we can do about a faulty CAN controller. */
  (void)HAL_FDCAN_Start(&canHandle);
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
  blt_int32u             txMsgId = BOOT_COM_CAN_TX_MSG_ID;
  FDCAN_TxHeaderTypeDef  txMsgHeader;
  blt_int32u             timeout;
  HAL_StatusTypeDef      status;
  blt_int32u             txMsgBuffer;
#if (BOOT_COM_CAN_FD_ENABLE > 0)
  static const blt_int8u len2dlc[] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,   /*  0 -  8 */
                                       9,  9,  9,  9,                       /*  9 - 12 */
                                       10, 10, 10, 10,                      /* 13 - 16 */
                                       11, 11, 11, 11,                      /* 17 - 20 */
                                       12, 12, 12, 12,                      /* 21 - 24 */
                                       13, 13, 13, 13, 13, 13, 13, 13,      /* 25 - 32 */
                                       14, 14, 14, 14, 14, 14, 14, 14,      /* 33 - 40 */
                                       14, 14, 14, 14, 14, 14, 14, 14,      /* 41 - 48 */
                                       15, 15, 15, 15, 15, 15, 15, 15,      /* 49 - 56 */
                                       15, 15, 15, 15, 15, 15, 15, 15 };    /* 57 - 64 */
#endif

  /* validate the specified length. */
  ASSERT_RT(len <= BOOT_COM_CAN_TX_MAX_DATA);

  /* configure the message that should be transmitted. */
  if ((txMsgId & 0x80000000) == 0)
  {
    /* set the 11-bit CAN identifier. */
    txMsgHeader.Identifier = txMsgId;
    txMsgHeader.IdType = FDCAN_STANDARD_ID;
  }
  else
  {
    /* negate the ID-type bit. */
    txMsgId &= ~0x80000000;
    /* set the 29-bit CAN identifier. */
    txMsgHeader.Identifier = txMsgId;
    txMsgHeader.IdType = FDCAN_EXTENDED_ID;
  }

  txMsgHeader.TxFrameType = FDCAN_DATA_FRAME;
  txMsgHeader.FDFormat = FDCAN_CLASSIC_CAN;
  txMsgHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txMsgHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  txMsgHeader.MessageMarker = 0x52;
  txMsgHeader.BitRateSwitch = FDCAN_BRS_OFF;
  txMsgHeader.DataLength = len;
#if (BOOT_COM_CAN_FD_ENABLE > 0)
  txMsgHeader.FDFormat = FDCAN_FD_CAN;
  txMsgHeader.DataLength = len2dlc[len];
  if (canFdBitrateSwitchUsed == BLT_TRUE)
  {
    txMsgHeader.BitRateSwitch = FDCAN_BRS_ON;
  }
#endif

  /* add the message to the transmit FIFO to request its transmission. */
  status = HAL_FDCAN_AddMessageToTxFifoQ(&canHandle, &txMsgHeader, data);
  /* read out which FIFO buffer was used for the last transmit request. */
  txMsgBuffer = HAL_FDCAN_GetLatestTxFifoQRequestBuffer(&canHandle);

  /* only continue with polling for transmit completion if the message transmit request
   * could be submitted to a valid FIFO buffer.
   */
  if ((status == HAL_OK) && (txMsgBuffer != 0))
  {
    /* determine timeout time for the transmit completion. */
    timeout = TimerGet() + CAN_MSG_TX_TIMEOUT_MS;
    /* poll for completion of the transmit operation. */
    while (HAL_FDCAN_IsTxBufferMessagePending(&canHandle, txMsgBuffer) != 0)
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
  blt_bool               result = BLT_FALSE;
  blt_int32u             rxMsgId = BOOT_COM_CAN_RX_MSG_ID;
  FDCAN_RxHeaderTypeDef  rxMsgHeader;
  HAL_StatusTypeDef      rxStatus = HAL_ERROR;
#if (BOOT_COM_CAN_FD_ENABLE > 0)
  static const blt_int8u dlc2len[] = { 0,  1,  2,  3,  4,  5,  6,  7,
                                       8, 12, 16, 20, 24, 32, 48, 64 };
#endif

  /* check if the expected CAN message was received? */
  if (HAL_FDCAN_GetRxFifoFillLevel(&canHandle, FDCAN_RX_FIFO0) > 0)
  {
    /* attempt to read the newly received CAN message from its buffer. */
    rxStatus = HAL_FDCAN_GetRxMessage(&canHandle, FDCAN_RX_FIFO0, &rxMsgHeader, data);
  }

  /* only continue processing the CAN message if something was received. */
  if (rxStatus == HAL_OK)
  {
    /* check if this message has the configured CAN packet identifier. */
    if ((rxMsgId & 0x80000000) == 0)
    {
      /* was an 11-bit CAN message received that matches? */
      if ( (rxMsgHeader.Identifier == rxMsgId) &&
           (rxMsgHeader.IdType == FDCAN_STANDARD_ID) )
      {
        /* set flag that a packet with a matching CAN identifier was received. */
        result = BLT_TRUE;
      }
    }
    else
    {
      /* negate the ID-type bit. */
      rxMsgId &= ~0x80000000;
      /* was an 29-bit CAN message received that matches? */
      if ( (rxMsgHeader.Identifier == rxMsgId) &&
           (rxMsgHeader.IdType == FDCAN_EXTENDED_ID) )
      {
        /* set flag that a packet with a matching CAN identifier was received. */
        result = BLT_TRUE;
      }
    }
    /* store the data length. */
    if (result == BLT_TRUE)
    {
#if (BOOT_COM_CAN_FD_ENABLE > 0)
      *len = 0;
      if (rxMsgHeader.DataLength <= sizeof(dlc2len)/sizeof(dlc2len[0]))
      {
        *len = dlc2len[rxMsgHeader.DataLength];
      }
#else
      *len = (blt_int8u)(rxMsgHeader.DataLength);
#endif
      /* validate the received length. */
      ASSERT_RT(*len <= BOOT_COM_CAN_RX_MAX_DATA);
    }
  }

  /* give the result back to the caller. */
  return result;
} /*** end of CanReceivePacket ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of can.c **************************************/
