/************************************************************************************//**
* \file         Demo/ARMCM33_STM32L5_Nucleo_L552ZE_Keil/Prog/boot.c
* \brief        Demo program bootloader interface source file.
* \ingroup      Prog_ARMCM33_STM32L5_Nucleo_L552ZE_Keil
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2021  by Feaser    http://www.feaser.com    All rights reserved
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
  /* perform software reset to activate the bootoader again */
  NVIC_SystemReset();
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

/** \brief Maximum bytes in a CTO packet on RS232, excluding the one extra for length and
 *         two extra for possibly configured checksum byte(s).
 */
#define RS232_CTO_RX_MAX_DATA          (129u)


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief UART handle to be used in API calls. */
static UART_HandleTypeDef rs232Handle;


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
  /* Configure UART peripheral. */
  rs232Handle.Instance = LPUART1;
  rs232Handle.Init.BaudRate = 57600;
  rs232Handle.Init.WordLength = UART_WORDLENGTH_8B;
  rs232Handle.Init.StopBits = UART_STOPBITS_1;
  rs232Handle.Init.Parity = UART_PARITY_NONE;
  rs232Handle.Init.Mode = UART_MODE_TX_RX;
  rs232Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  rs232Handle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  rs232Handle.Init.ClockPrescaler = UART_PRESCALER_DIV4;
  rs232Handle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  rs232Handle.FifoMode = UART_FIFOMODE_DISABLE;
  /* Initialize the UART peripheral. */
  HAL_UART_Init(&rs232Handle);
} /*** end of BootComRs232Init ***/


/************************************************************************************//**
** \brief     Receives the CONNECT request from the host, which indicates that the
**            bootloader should be activated and, if so, activates it.
** \return    none.
**
****************************************************************************************/
static void BootComRs232CheckActivationRequest(void)
{
  static unsigned char xcpCtoReqPacket[RS232_CTO_RX_MAX_DATA+3];
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
           (xcpCtoReqPacket[0] <= RS232_CTO_RX_MAX_DATA) )
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
  HAL_StatusTypeDef result;

  /* receive a byte in a non-blocking manner */
  result = HAL_UART_Receive(&rs232Handle, data, 1, 0);
  /* process the result */
  if (result == HAL_OK)
  {
    /* success */
    return 1;
  }
  /* error occurred */
  return 0;
} /*** end of Rs232ReceiveByte ***/
#endif /* BOOT_COM_RS232_ENABLE > 0 */


#if (BOOT_COM_CAN_ENABLE > 0)
/****************************************************************************************
*        C O N T R O L L E R   A R E A   N E T W O R K   I N T E R F A C E
****************************************************************************************/

/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Structure type for grouping together CAN peripheral parameters needed for
 *         performing bittiming calculations.
 */
typedef struct t_can_periph_params
{
  unsigned long base_freq;                        /**< Source clock frequency [Hz]     */
  unsigned short prescaler_min;                   /**< Smallest supported prescaler    */
  unsigned short prescaler_max;                   /**< Largest supported prescaler     */
  unsigned short tseg1_min;                       /**< Smallest supported Tseg1        */
  unsigned short tseg1_max;                       /**< Largest supported Tseg1         */
  unsigned short tseg2_min;                       /**< Smallest supported Tseg2        */
  unsigned short tseg2_max;                       /**< Largest supported Tseg2         */
} tCanPeriphParams;

/** \brief Structure type for grouping CAN bit timing configuration information. */
typedef struct
{
  unsigned short prescaler;                       /**< CAN clock prescaler             */
  unsigned short tseg1;                           /**< CAN time segment 1 (excl. SYNC) */
  unsigned short tseg2;                           /**< CAN time segment 2              */
  unsigned short sjw;                             /**< CAN synchronization jump width  */
} tCanBitTimingConfig;


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief CAN handle to be used in API calls. */
static FDCAN_HandleTypeDef canHandle;

#if (BOOT_COM_CAN_FD_ENABLE > 0)
/** \brief Boolean flag to determine if the bitrate switch feature is used for CAN FD. */
static unsigned char canFdBitrateSwitchUsed;
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
static unsigned char CanCalculateBitTimingConfig(unsigned long const baud,
                                                 tCanPeriphParams const * periph_params,
                                                 tCanBitTimingConfig * bittiming_config)
{
  unsigned char  result = 0U;
  unsigned short bitTq, bitTqMin, bitTqMax;
  unsigned short tseg1, tseg2;
  unsigned short prescaler;
  unsigned char  samplePoint;

  /* only continue with valid parameters. */
  if ( (baud >= 10000UL) && (baud <= 8000000UL) && (periph_params != NULL) &&
       (bittiming_config != NULL) )
  {
    /* calculate minimum and maximum possible time quanta per bit. remember that the
     * bittime in time quanta is 1 (sync) + tseg1 + tseg2.
     */
    bitTqMin = 1U + periph_params->tseg1_min + periph_params->tseg2_min;
    bitTqMax = 1U + periph_params->tseg1_max + periph_params->tseg2_max;

    /* loop through all the prescaler values from low to high to find one that results
     * in a time quanta per bit that is in the range bitTqMin .. bitTqMax. note that
     * looping through the prescalers low to high is important, because a lower prescaler
     * would result in a large number of time quanta per bit. This in turns gives you
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
          tseg1 = bitTq - tseg2 - 1;
          /* are these values within configurable range? */
          if ( (tseg1 >= periph_params->tseg1_min) &&
               (tseg1 <= periph_params->tseg1_max) &&
               (tseg2 >= periph_params->tseg2_min) &&
               (tseg2 <= periph_params->tseg2_max) )
          {
            /* calculate the actual sample point, given these Tseg values. */
            samplePoint = (unsigned char)(((1U + tseg1) * 100UL) / bitTq);
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
              result = 1U;
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
** \brief     Initializes the CAN communication interface.
** \details   It is up to the user to configure the desired CAN clock source. By default
**            it is HSE. Alternatives are PLL or PLLSAI1. To meet the clock tolerance
**            requirement of CAN 2.0B, an external crystal oscillator (HSE) is
**            recommended. A multiple of 8 MHz gives the best support for most commonly
**            used CAN baudrates, so that is recommended.
** \return    none.
**
****************************************************************************************/
static void BootComCanInit(void)
{
  unsigned long       rxMsgId = BOOT_COM_CAN_RX_MSG_ID;
  FDCAN_FilterTypeDef filterConfig;
  tCanPeriphParams    periphParams;
  tCanBitTimingConfig bittimingConfig = { 0 };
#if (BOOT_COM_CAN_FD_ENABLE > 0)
  tCanBitTimingConfig bittimingConfigBRS = { 0 };
#endif

  /* set the CAN controller channel, which is needed by HAL_FDCAN_MspInit(). */
  canHandle.Instance = FDCAN1;
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
    canFdBitrateSwitchUsed = 1U;
  }
  else
  {
    canFdBitrateSwitchUsed = 0U;
  }

  /* calculate the data bittiming configuration with enabled bitrate switch feature. */
  if (canFdBitrateSwitchUsed == 1U)
  {
    /* store CAN peripheral parameters related to the data bittiming configuration. */
    periphParams.base_freq = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_FDCAN);
    periphParams.prescaler_min = 1U;
    periphParams.prescaler_max = 32U;
    periphParams.tseg1_min = 1U;
    periphParams.tseg1_max = 32U;
    periphParams.tseg2_min = 1U;
    periphParams.tseg2_max = 16U;
    /* attempt to find a matching data bittiming configuration. */
    (void)CanCalculateBitTimingConfig(BOOT_COM_CAN_FD_BRS_BAUDRATE, &periphParams,
                                      &bittimingConfigBRS);
  }
#endif

  /* store CAN peripheral parameters related to the nominal bittiming configuration. */
  periphParams.base_freq = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_FDCAN);
  periphParams.prescaler_min = 1U;
  periphParams.prescaler_max = 512U;
  periphParams.tseg1_min = 1U;
  periphParams.tseg1_max = 256U;
  periphParams.tseg2_min = 1U;
  periphParams.tseg2_max = 128U;
  /* attempt to find a matching nominal bittiming configuration. */
  (void)CanCalculateBitTimingConfig(BOOT_COM_CAN_BAUDRATE, &periphParams,
                                    &bittimingConfig);

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
  if (canFdBitrateSwitchUsed == 0U)
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
  if (canFdBitrateSwitchUsed == 1U)
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
} /*** end of BootComCanInit ***/


/************************************************************************************//**
** \brief     Receives the CONNECT request from the host, which indicates that the
**            bootloader should be activated and, if so, activates it.
** \return    none.
**
****************************************************************************************/
static void BootComCanCheckActivationRequest(void)
{
  unsigned long rxMsgId = BOOT_COM_CAN_RX_MSG_ID;
  unsigned char packetIdMatches = 0;
  FDCAN_RxHeaderTypeDef rxMsgHeader;
#if (BOOT_COM_CAN_FD_ENABLE > 0)
  static const unsigned char dlc2len[] = { 0,  1,  2,  3,  4,  5,  6,  7,
                                           8, 12, 16, 20, 24, 32, 48, 64 };
  unsigned char rxMsgData[64];
#else
  unsigned char rxMsgData[8];
#endif
  unsigned char rxMsgLen;
  HAL_StatusTypeDef rxStatus = HAL_ERROR;

  /* poll for received CAN messages that await processing. */
  if (HAL_FDCAN_GetRxFifoFillLevel(&canHandle, FDCAN_RX_FIFO0) > 0)
  {
    /* attempt to read the newly received CAN message from its buffer. */
    rxStatus = HAL_FDCAN_GetRxMessage(&canHandle, FDCAN_RX_FIFO0, &rxMsgHeader,
                                      rxMsgData);
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
        packetIdMatches = 1;
      }
    }
    else
    {
      /* negate the ID-type bit */
      rxMsgId &= ~0x80000000;
      /* was an 29-bit CAN message received that matches? */
      if ( (rxMsgHeader.Identifier == rxMsgId) &&
           (rxMsgHeader.IdType == FDCAN_EXTENDED_ID) )
      {
        /* set flag that a packet with a matching CAN identifier was received. */
        packetIdMatches = 1;
      }
    }

    /* only continue if a packet with a matching CAN identifier was received. */
    if (packetIdMatches == 1)
    {
      /* obtain the CAN message length. */
#if (BOOT_COM_CAN_FD_ENABLE > 0)
      rxMsgLen = 0;
      if (rxMsgHeader.DataLength <= sizeof(dlc2len)/sizeof(dlc2len[0]))
      {
        rxMsgLen = dlc2len[rxMsgHeader.DataLength];
      }
#else
      rxMsgLen = (unsigned char)(rxMsgHeader.DataLength);
#endif
      /* check if this was an XCP CONNECT command */
      if ((rxMsgData[0] == 0xff) && (rxMsgLen == 2))
      {
        /* connection request received so start the bootloader */
        BootActivate();
      }
    }
  }
} /*** end of BootComCanCheckActivationRequest ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of boot.c *************************************/
