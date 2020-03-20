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
  blt_int32u canClockFreqkHz;

  /* TODO ##Port This helper function assists with getting a compatible bittiming 
   * configuration, based on the specified 'baud' communication speed on the CAN bus in
   * kbps. This function needs two microcontroller specific values: (1) the speed of
   * the clock that sources the CAN peripheral and (2) the supported range of the
   * prescaler that for scaling down the CAN peripheral clock speed.
   */

  /* TODO ##Port Set the clock speed of the CAN peripheral in kHz. You can used the
   * macros BOOT_CPU_XTAL_SPEED_KHZ and BOOT_CPU_SYSTEM_SPEED_KHZ if applicable. 
   */
  canClockFreqkHz = BOOT_CPU_XTAL_SPEED_KHZ;

  /* loop through all possible time quanta configurations to find a match */
  for (cnt=0; cnt < sizeof(canTiming)/sizeof(canTiming[0]); cnt++)
  {
    if ((canClockFreqkHz % (baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1))) == 0)
    {
      /* compute the prescaler that goes with this TQ configuration */
      *prescaler = canClockFreqkHz/(baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1));
  
      /* TODO ##Port Update the prescaler range that is supported by the CAN peripheral
       * on the microcontroller. The example implementation is for a prescaler that can
       * be in the 1 - 1024 range.
       */
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
  blt_int16u prescaler = 0;
  blt_int8u  tseg1 = 0, tseg2 = 0;

  /* TODO ##Port Perform compile time assertion to check that the configured CAN channel
   * is actually supported by this driver. The example is for a driver where CAN
   * channels 0 - 1 are supported. 
   */
  ASSERT_CT((BOOT_COM_CAN_CHANNEL_INDEX == 0 || BOOT_COM_CAN_CHANNEL_INDEX == 1));

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

  /* TODO ##Port Perform the configuration and initialization of the CAN controller. Note
   * that the bittiming related values are already stored in 'prescaler, 'tseg1', and
   * 'tseg2'. There values are ready to be used. Typically, the following tasks need
   * to be performed:
   * (1) Place the CAN controller in initialization mode.
   * (2) Disable all CAN related interrupts as the bootloader runs in polling mode.
   * (3) Configure the bittiming based on: 'prescaler', 'tseg1' and 'tseg2'. It is okay
   *     to configure 1 time quanta for the synchronization jump width (SWJ).
   * (4) Configure one transmit message object. It will be used in CanTransmitPacket()
   *     to transmit a CAN message with identifier BOOT_COM_CAN_TX_MSG_ID. Note that if
   *     the 0x80000000 bit is set in this identifier, it means that it is a 29-bit CAN
   *     identifier instead of an 11-bit.
   * (5) Configure at least one reception message object and configure its reception
   *     acceptance filter such that only the CAN identifier BOOT_COM_CAN_RX_MSG_ID is 
   *     received. Note that if the 0x80000000 bit is set in this identifier, it means
   *     that it is a 29-bit CAN identifier instead of an 11-bit.
   * (6) Leave the initialization mode and place the CAN controller in operational mode.
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

  /* TODO ##Port Configure the transmit message object for transmitting a CAN message
   * with CAN identifier BOOT_COM_CAN_TX_MSG_ID. Note that if the 0x80000000 bit is set
   * in this identifier, it means that it is a 29-bit CAN identifier instead of an
   * 11-bit. Next, copy the message data to the transmit message object. The number
   * of data bytes is in 'len' and the actual data byte values are in array 'data'.
   * Once done, start the transmission of the message that was just stored in the
   * transmit message object.
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

  /* TODO ##Port Check for the reception of a new CAN message with identifier 
   * BOOT_COM_CAN_RX_MSG_ID. Note that if the 0x80000000 bit is set in this identifier, 
   * it means that it is a 29-bit CAN identifier instead of an 11-bit.
   * If a new message with this CAN identifier was received, store the data byte values
   * in array 'data' and store the number of data bytes in 'len'. Finally, set 'result'
   * to BLT_TRUE to indicate to the caller of this function that a new CAN message was
   * received and stored.
   */

  /* give the result back to the caller */
  return result;
} /*** end of CanReceivePacket ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of can.c **************************************/
