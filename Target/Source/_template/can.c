/************************************************************************************//**
* \file         Source/_template/can.c
* \brief        Bootloader CAN communication interface source file.
* \ingroup      Target__template_can
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2019  by Feaser    http://www.feaser.com    All rights reserved
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

/************************************************************************************//**
* \defgroup   Target__template_can CAN driver of a port
* \brief      This module implements the CAN driver of a microcontroller port. 
* \details    For the most parts, this driver is already implemented. The only parts that
*             need porting are the CAN initialization, CAN message reception and CAN
*             message transmission.
* \ingroup    Target__template
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */
#if (BOOT_COM_CAN_ENABLE > 0)
/* TODO ##Port Include microcontroller peripheral driver header files here. */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout for transmitting a CAN message in milliseconds. */
#define CAN_MSG_TX_TIMEOUT_MS          (50u)


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
** \return    none.
**
****************************************************************************************/
void CanInit(void)
{
  tCanPeriphParams    periphParams;
  tCanBitTimingConfig bittimingConfig = { 0 };
#if (BOOT_COM_CAN_FD_ENABLE > 0)
  tCanBitTimingConfig bittimingConfigBRS = { 0 };
#endif

  /* TODO ##Port Perform compile time assertion to check that the configured CAN channel
   * is actually supported by this driver. The example is for a driver where CAN
   * channels 0 - 1 are supported. 
   */
  ASSERT_CT((BOOT_COM_CAN_CHANNEL_INDEX == 0 || BOOT_COM_CAN_CHANNEL_INDEX == 1));

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
    /* TODO ##Port Update the periphParams element values to match that of your CAN
     * controller for configuring the CAN FD data bittiming. This is the faster rate
     * for communicating the actual data bytes of the CAN message.
     */
    /* store CAN peripheral parameters related to the data bittiming configuration. */
    periphParams.base_freq = BOOT_CPU_XTAL_SPEED_KHZ;
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

  /* TODO ##Port Update the periphParams element values to match that of your CAN
   * controller for configuring the CAN classic bittiming. Also referred to as the
   * nominal or arbitration bittiming.
   */
  /* store CAN peripheral parameters related to the nominal bittiming configuration. */
  periphParams.base_freq = BOOT_CPU_XTAL_SPEED_KHZ;
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

  /* TODO ##Port Perform the configuration and initialization of the CAN controller. 
   *
   * Note that the calculated bittiming related values (prescaler, tseg1, tseg2 and sjw)
   * are already stored in bittimingConfig (and bittimingConfigBRS for the case of CAN FD
   * with the bitrate switch feature enabled. There values are ready to be used. Also
   * note that the tseg1 element of these structures excludes the 1 timequanta of the
   * SYNC segment.
   *   
   * Typically, the following tasks need to be performed:
   * (1) Place the CAN controller in initialization mode.
   * (2) Disable all CAN related interrupts as the bootloader runs in polling mode.
   * (3) Configure the bittiming based on: 'prescaler', 'tseg1', 'tseg2' and 'sjw' as
   *     stored in the bittimingConfig variable. 
   * (4) In case the CAN FD bitrate switch feature is enabled, also configure the faster
   *     data bittiming based on: 'prescaler', 'tseg1', 'tseg2' and 'sjw' as
   *     stored in the bittimingConfigBRS variable. 
   * (5) Configure one transmit message object. It will be used in CanTransmitPacket()
   *     to transmit a CAN message with identifier BOOT_COM_CAN_TX_MSG_ID. Note that if
   *     the 0x80000000 bit is set in this identifier, it means that it is a 29-bit CAN
   *     identifier instead of an 11-bit.
   * (6) Configure at least one reception message object and configure its reception
   *     acceptance filter such that only the CAN identifier BOOT_COM_CAN_RX_MSG_ID is 
   *     received. Note that if the 0x80000000 bit is set in this identifier, it means
   *     that it is a 29-bit CAN identifier instead of an 11-bit.
   * (7) Leave the initialization mode and place the CAN controller in operational mode.
   */    
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
  
  /* TODO ##Port Configure the transmit message object for transmitting a CAN message
   * with CAN identifier BOOT_COM_CAN_TX_MSG_ID. Note that if the 0x80000000 bit is set
   * in this identifier, it means that it is a 29-bit CAN identifier instead of an
   * 11-bit. Next, copy the message data to the transmit message object. The number
   * of data bytes is in 'len' and the actual data byte values are in array 'data'.
   * When configuring the data length code of the message (DLC), use the lookup table
   * like this: dlc = len2dlc[len]. That way if works for both CAN classic and CAN FD.
   * Once done, start the transmission of the message that was just stored in the
   * transmit message object.
   * 
   * In case CAN FD is enabled, you might need to explicitly configure the frame format
   * to be CAN FD instead of CAN classic in the CAN controller's hardware transmit
   * buffer. You can use the macro BOOT_COM_CAN_FD_ENABLE value to check if CAN FD is
   * enabled.
   * If additionally the bitrate switch feature should be used, you have to typically set
   * a bit somewhere in the CAN controller's hardware transmit buffer to flag that the
   * bitrate switch feature (BRS) should be used when transmitting the message. You can
   * use the 'canFdBitrateSwitchUsed' variable to check if the bitrate switch feature
   * should be used.
   */

  /* TODO ##Port Wait for the message transmission to complete, with timeout though to
   * make sure this function doesn't hang in case of an error. This is typically achieved
   * by evaluating a transmit complete flag in a register of the transmit message object.
   */
  /* determine timeout time for the transmit completion. */
  timeout = TimerGet() + CAN_MSG_TX_TIMEOUT_MS;
  /* poll for completion of the transmit operation. */
  while (1 == 0)
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
  static const blt_int8u dlc2len[] = { 0,  1,  2,  3,  4,  5,  6,  7,
                                       8, 12, 16, 20, 24, 32, 48, 64 };

  /* TODO ##Port Check for the reception of a new CAN message with identifier 
   * BOOT_COM_CAN_RX_MSG_ID. Note that if the 0x80000000 bit is set in this identifier, 
   * it means that it is a 29-bit CAN identifier instead of an 11-bit.
   * If a new message with this CAN identifier was received, store the data byte values
   * in array 'data' and store the number of data bytes in 'len'. Make sure to use the 
   * lookup table when converting the messages's dlc value to 'len' for compatibility
   * with both CAN classic and CAN FD. E.g. *len = dlc2len[hwmsgbuf->dlc]. Finally, set
   * 'result' to BLT_TRUE to indicate to the caller of this function that a new CAN
   * message was received and stored.
   */

  /* give the result back to the caller */
  return result;
} /*** end of CanReceivePacket ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of can.c **************************************/
