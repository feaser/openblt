/************************************************************************************//**
* \file         Source/ARMCM0_S32K11/mbrtu.c
* \brief        Bootloader Modbus RTU communication interface source file.
* \ingroup      Target_ARMCM0_S32K11
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
#if (BOOT_COM_MBRTU_ENABLE > 0)
#include "device_registers.h"                    /* device registers                   */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout for transmitting a byte in milliseconds. */
#define MBRTU_BYTE_TX_TIMEOUT_MS          (10u)

#if (BOOT_COM_MBRTU_CHANNEL_INDEX == 0)
/** \brief Set the peripheral LPUART0 base pointer. */
#define LPUARTx                        (LPUART0)
/** \brief Set the PCC index offset for LPUART0. */
#define PCC_LPUARTx_INDEX              (PCC_LPUART0_INDEX)
#elif (BOOT_COM_MBRTU_CHANNEL_INDEX == 1)
/** \brief Set the peripheral LPUART1 base pointer. */
#define LPUARTx                        (LPUART1)
/** \brief Set the PCC index offset for LPUART1. */
#define PCC_LPUARTx_INDEX              (PCC_LPUART1_INDEX)
#endif


/****************************************************************************************
* External data declarations
****************************************************************************************/
/** \brief The system clock frequency supplied to the SysTick timer and the processor
 *         core clock.
 */
extern blt_int32u SystemCoreClock;


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Stores the number of free running counter ticks that represents the 3.5
 *         character delay time (T3_5) for Modbus RTU.
 */
static blt_int16u mbRtuT3_5Ticks;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static blt_bool   MbRtuReceiveByte(blt_int8u *data);
static void       MbRtuTransmitByte(blt_int8u data, blt_bool end_of_packet);


/************************************************************************************//**
** \brief     Initializes the Modbus RTU communication interface.
** \attention It is the application's responsibility to initialize a timer peripheral
**            to have an upwards counting free running counter, which runs at 100 kHz.
** \return    none.
**
****************************************************************************************/
void MbRtuInit(void)
{
  blt_int16u startTimeTicks;
  blt_int16u deltaTimeTicks;
  blt_int16u currentTimeTicks;
  blt_int8u  rxDummy;
  blt_int32u sourceClockFreqHz;
  blt_int32u div2RegValue;
  blt_int32u ctrlRegValue;
  blt_int16u baudrateSbr0_12;
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

  /* Perform compile time assertion to check that the configured UART channel is actually
   * supported by this driver.
   */
  ASSERT_CT((BOOT_COM_MBRTU_CHANNEL_INDEX == 0) ||
            (BOOT_COM_MBRTU_CHANNEL_INDEX == 1));

  /* calculate the 3.5 character delay time in free running counter ticks. note that
   * the free running counter runs at 100 kHz, so one tick is 10 us. For baudrates >
   * 19200 bps, it can be fixed to 1750 us.
   */
  if (BOOT_COM_MBRTU_BAUDRATE > 19200)
  {
    /* set T3_5 time to a fixed value of 1750 us. */
    mbRtuT3_5Ticks = 175;
  }
  /* calculate the T3_5 time, because the baudrate is <= 19200 bps. */
  else
  {
    /* T3_5 [us * 10] = 3.5 * Tchar = 3.5 * 11 * 100000 / baudrate = 3850000 / baudrate.
     * make sure to do integer round up though. Make sure to add 1 to adjust for 10us
     * timer resolution inaccuracy.
     */
    mbRtuT3_5Ticks = (blt_int16u)(((3850000UL + (BOOT_COM_MBRTU_BAUDRATE - 1U)) /
                                    BOOT_COM_MBRTU_BAUDRATE) + 1);
  }

  /* Make sure the UART peripheral clock is disabled before configuring its source
   * clock.
   */
  PCC->PCCn[PCC_LPUARTx_INDEX] &= ~PCC_PCCn_CGC_MASK;
  /* Reset the currently selected clock. */
  PCC->PCCn[PCC_LPUARTx_INDEX] &= ~PCC_PCCn_PCS_MASK;
  /* Select option 3 as the UART peripheral source clock and enable the clock. Option 3
   * is the FIRCDIV2_CLK, which is available on all peripherals and configurations. The
   * FIRC clock also has a 3 times better accuracy than the SIRC clock.
   */
  PCC->PCCn[PCC_LPUARTx_INDEX] |= PCC_PCCn_PCS(3) | PCC_PCCn_CGC_MASK;
  /* Obtain the DIV2 divider value of the FIRC_CLK. */
  div2RegValue = (SCG->FIRCDIV & SCG_FIRCDIV_FIRCDIV2_MASK) >> SCG_FIRCDIV_FIRCDIV2_SHIFT;
  /* Check if the DIV2 register value for FIRC is 0. In this case FIRCDIV2_CLK is
   * currently disabled.
   */
  if (div2RegValue == 0U)
  {
    /* Configure the DIV2 for a default divide by 1 to make sure the FIRCDIV2_CLK is
     * actually enabled.
     */
    div2RegValue = 1U;
    SCG->FIRCDIV |= SCG_FIRCDIV_FIRCDIV2(div2RegValue);
  }
  /* Determine the FIRCDIV2_CLK frequency. The FIRC_CLK is trimmed to 48 MHz during
   * reset. Process the configued DIV2 divider factor to get the actual frequency
   * of FIRCDIV2_CLK, which was selected as the source clock for the UART peripheral.
   */
  sourceClockFreqHz = 48000000UL;
  sourceClockFreqHz /= div2DividerLookup[div2RegValue];
  /* Configure the baudrate from BOOT_COM_MBRTU_BAUDRATE, taking into account that an
   * oversampling ratio of 4 will be configured. Integer rounding is used to get the best
   * value for baudrateSbr0_12. Actual baudrate equals:
   *   sourceClockFreqHz / 4 / baudrateSbr0_12.
   */
  baudrateSbr0_12 = (((sourceClockFreqHz / BOOT_COM_MBRTU_BAUDRATE) +
                    (4UL - 2UL)) / 4UL) & LPUART_BAUD_SBR_MASK;
  /* OSR=3: Over sampling ratio = 3+1=4.
   * SBNS=x: One or two stop bits.
   * BOTHEDGE=1: receiver samples only on rising edge.
   * M10=0: Rx and Tx use 7 to 9 bit data characters.
   * RESYNCDIS=0: Resync during rec'd data word supported.
   * LBKDIE, RXEDGIE=0: interrupts disable.
   * TDMAE, RDMAE, TDMAE=0: DMA requests disabled.
   * MAEN1, MAEN2,  MATCFG=0: Match disabled.
   */
  LPUARTx->BAUD = LPUART_BAUD_SBR(baudrateSbr0_12) | LPUART_BAUD_BOTHEDGE(1) |
                  LPUART_BAUD_OSR(3) | LPUART_BAUD_SBNS(BOOT_COM_MBRTU_STOPBITS - 1U);
  /* Clear the error/interrupt flags */
  LPUARTx->STAT = FEATURE_LPUART_STAT_REG_FLAGS_MASK;
  /* Reset all features/interrupts by default */
  LPUARTx->CTRL = 0x00000000;
  /* Reset match addresses */
  LPUARTx->MATCH = 0x00000000;
#if FEATURE_LPUART_HAS_MODEM_SUPPORT
  /* Reset IrDA modem features */
  LPUARTx->MODIR = 0x00000000;
#endif
#if FEATURE_LPUART_FIFO_SIZE > 0U
  /* Reset FIFO feature */
  LPUARTx->FIFO = FEATURE_LPUART_FIFO_RESET_MASK;
  /* Enable the transmit and receive FIFOs. */
  LPUARTx->FIFO |= LPUART_FIFO_TXFE(1) | LPUART_FIFO_RXFE(1);
  /* Set the reception water mark to 0 and the transmitter water mark to 1. */
  LPUARTx->WATER = LPUART_WATER_TXWATER(1) | LPUART_WATER_RXWATER(0);
#endif
  /* Enable transmitter and receiver, no parity, 8 bit char:
   * RE=1: Receiver enabled.
   * TE=1: Transmitter enabled.
   * PE,PT=0: No hw parity generation or checking.
   * M7,M,R8T9,R9T8=0: 8-bit data characters.
   * DOZEEN=0: LPUART enabled in Doze mode.
   * ORIE,NEIE,FEIE,PEIE,TIE,TCIE,RIE,ILIE,MA1IE,MA2IE=0: no IRQ.
   * TxDIR=0: TxD pin is input if in single-wire mode.
   * TXINV=0: Transmit data not inverted.
   * RWU,WAKE=0: normal operation; rcvr not in standby.
   * IDLCFG=0: one idle character.
   * ILT=0: Idle char bit count starts after start bit.
   * SBK=0: Normal transmitter operation - no break char.
   * LOOPS,RSRC=0: no loop back.
   */
  ctrlRegValue = LPUART_CTRL_RE(1) | LPUART_CTRL_TE(1);
  /* odd parity enabled? */
  #if (BOOT_COM_MBRTU_PARITY == 1)
  ctrlRegValue |= LPUART_CTRL_M(1) | LPUART_CTRL_PE(1) | LPUART_CTRL_PT(1);
  #endif
  /* even parity enabled? */
  #if (BOOT_COM_MBRTU_PARITY == 2)
  ctrlRegValue |= LPUART_CTRL_M(1) | LPUART_CTRL_PE(1);
  #endif
  LPUARTx->CTRL = ctrlRegValue;

  /* enable the receiver output to be able to receive. */
  MbRtuDriverOutputControlHook(BLT_FALSE);

  /* wait for idle line detection. This is T3_5 time after reception of the last byte. */
  startTimeTicks = MbRtuFreeRunningCounterGet();
  do
  {
    /* service the watchdog. */
    CopService();
    /* get the current value of the free running counter. */
    currentTimeTicks = MbRtuFreeRunningCounterGet();
    /* check if a byte was received while waiting for the idle line. */
    if (MbRtuReceiveByte(&rxDummy) == BLT_TRUE)
    {
      /* restart the idle line detection. */
      startTimeTicks = currentTimeTicks;
    }
    /* calculate the number of ticks that elapsed since the start or since the last
     * byte reception. Note that this calculation works, even if the free running counter
     * overflowed.
     */
    deltaTimeTicks = currentTimeTicks - startTimeTicks;
  }
  while (deltaTimeTicks < mbRtuT3_5Ticks);
} /*** end of MbRtuInit ***/


/************************************************************************************//**
** \brief     Transmits a packet formatted for the communication interface.
** \param     data Pointer to byte array with data that it to be transmitted.
** \param     len  Number of bytes that are to be transmitted.
** \return    none.
**
****************************************************************************************/
void MbRtuTransmitPacket(blt_int8u *data, blt_int8u len)
{
  blt_int16u data_index;
  blt_int16u checksum;
  blt_bool endOfPacket = BLT_FALSE;
  /* Made static to lower stack load and +5 for Modbus RTU packet overhead. */
  static blt_int8u txPacket[BOOT_COM_MBRTU_TX_MAX_DATA + 5];

  /* On Modbus RTU, there must always be a T3_5 time separation between packet trans-
   * missions.
   *
   * This bootloader uses XCP packets embedded in Modbus RTU packets. The XCP
   * communication is always request / response based. That means that this packet is
   * a response packet and it will only be sent, after the reception of a request packet.
   *
   * A response packet is only deemed valid, after the T3_5 idle time. This module
   * implements the T3_5 end-of-packet time event detection. Consequently, it is already
   * guaranteed that there is T3_5 between subsequent packet transmissions. As such, no
   * further T3_5 wait time is needed here.
   */

  /* verify validity of the len-parameter */
  ASSERT_RT(len <= BOOT_COM_MBRTU_TX_MAX_DATA);

  /* construct the Modbus RTU packet. start by adding the slave address. */
  txPacket[0] = BOOT_COM_MBRTU_NODE_ID;
  /* add the user-defined function code for embedding XCP packets. */
  txPacket[1] = BOOT_COM_MBRTU_FCT_CODE_USER_XCP;
  /* add the XCP packet length. */
  txPacket[2] = len;

  /* copy the XCP packet data. */
  CpuMemCopy((blt_int32u)&txPacket[3], (blt_int32u)data, len);
  /* calculate the checksum for the packet, including slave address, function code and
   * extra XCP length.
   */
  checksum = MbRtuCrcCalculate(&txPacket[0], len + 3);
  /* add the checksum at the end of the packet */
  txPacket[len + 3] = (blt_int8u)(checksum & 0xff);
  txPacket[len + 4] = (blt_int8u)(checksum  >> 8);

  /* enable the driver output to be able to send. just make sure to wait a little around
   * the togglng of the DE/NRE pin.
   */
  MbRtuDelay(BOOT_COM_MBRTU_DRIVER_OUTPUT_ENABLE_DELAY_US);
  MbRtuDriverOutputControlHook(BLT_TRUE);
  MbRtuDelay(BOOT_COM_MBRTU_DRIVER_OUTPUT_ENABLE_DELAY_US);

  /* transmit all the packet bytes one-by-one */
  for (data_index = 0; data_index < (len + 5); data_index++)
  {
    /* keep the watchdog happy */
    CopService();
    /* last byte of the packet? */
    if (data_index == ((len + 5) - 1))
    {
      /* update the end of packet flag. */
      endOfPacket = BLT_TRUE;
    }
    /* write byte */
    MbRtuTransmitByte(txPacket[data_index], endOfPacket);
  }
  /* enable the receiver output to be able to receive again. just make sure to wait a
   * little around the togglng of the DE/NRE pin.
   */
  MbRtuDelay(BOOT_COM_MBRTU_DRIVER_OUTPUT_DISABLE_DELAY_US);
  MbRtuDriverOutputControlHook(BLT_FALSE);
  MbRtuDelay(BOOT_COM_MBRTU_DRIVER_OUTPUT_DISABLE_DELAY_US);
} /*** end of MbRtuTransmitPacket ***/


/************************************************************************************//**
** \brief     Receives a communication interface packet if one is present.
** \param     data Pointer to byte array where the data is to be stored.
** \param     len Pointer where the length of the packet is to be stored.
** \return    BLT_TRUE if a packet was received, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool MbRtuReceivePacket(blt_int8u *data, blt_int8u *len)
{
  blt_bool result = BLT_FALSE;
  blt_int8u rxByte;
  blt_int16u currentTimeTicks;
  blt_int16u deltaTimeTicks;
  blt_int16u checksumCalculated;
  blt_int16u checksumReceived;
  /* Made static to lower stack load and +5 for Modbus RTU packet overhead. */
  static blt_int8u rxPacket[BOOT_COM_MBRTU_RX_MAX_DATA + 5];
  static blt_int8u rxLength = 0;
  static blt_bool packetRxInProgress = BLT_FALSE;
  static blt_int16u lastRxByteTimeTicks = 0;

  /* get the current value of the free running counter. */
  currentTimeTicks = MbRtuFreeRunningCounterGet();

  /* check for a newly received byte. */
  if (MbRtuReceiveByte(&rxByte) == BLT_TRUE)
  {
    /* store the time at which the byte was received. */
    lastRxByteTimeTicks = currentTimeTicks;
    /* is this the potential start of a new packet? */
    if (packetRxInProgress == BLT_FALSE)
    {
      /* initialize the reception of a new packet. */
      rxLength = 0;
      packetRxInProgress = BLT_TRUE;
    }
    /* store the newly received byte in the buffer, with buffer overrun protection. */
    if (rxLength < (sizeof(rxPacket)/sizeof(rxPacket[0])))
    {
      rxPacket[rxLength] = rxByte;
      rxLength++;
    }
    /* buffer overrun occurred. received packet was longer than supported so discard
     * the packet to try and sync to the next one.
     */
    else
    {
      /* discard the partially received packet. */
      packetRxInProgress = BLT_FALSE;
    }
  }

  /* only attempt to detect the end of packet, when a reception is in progress. */
  if (packetRxInProgress == BLT_TRUE)
  {
    /* calculate the number of ticks that elapsed since the last byte reception. note
     * that this calculation works, even if the free running counter overflowed.
     */
    deltaTimeTicks = currentTimeTicks - lastRxByteTimeTicks;
    /* packet reception is assumed complete after T3_5 of not receiving new data. */
    if (deltaTimeTicks >= mbRtuT3_5Ticks)
    {
      /* a Modbus RTU packet consists of at least the address field, function code and
       * 16-bit CRC. Validate the packet length based on this info.
       */
      if (rxLength >= 4)
      {
        /* calculate the packet checksum. */
        checksumCalculated = MbRtuCrcCalculate(&rxPacket[0], rxLength - 2);
        /* extract the checksum received with the packet. */
        checksumReceived = rxPacket[rxLength - 2] | (rxPacket[rxLength - 1] << 8);
        /* only continue with packet processing if the checksums match. */
        if (checksumCalculated == checksumReceived)
        {
          /* we are only interested in Modbus RTU packets that are addressed to us and
           * have an XCP packet embedded.
           */
          if ( (rxPacket[0] == BOOT_COM_MBRTU_NODE_ID) &&
               (rxPacket[1] == BOOT_COM_MBRTU_FCT_CODE_USER_XCP) )
          {
            /* An XCP packet embedded in a Modbus RTU packet has an extra XCP packet
             * length value. Use it to double-check that the packet length is valid.
             */
            if (rxPacket[2] == (rxLength - 5))
            {
              /* copy the packet's XCP data. */
              CpuMemCopy((blt_int32u)data, (blt_int32u)&rxPacket[3], rxLength - 5);
              /* set the packet's XCP length. */
              *len = rxLength - 5;
              /* update the result to success to indicate that this XCP packet is ready
               * for processing.
               */
              result = BLT_TRUE;
            }
          }
        }
      }
      /* reset the packet reception in progress flag, to be able to receive the next. */
      packetRxInProgress = BLT_FALSE;
    }
  }

  /* give the result back to the caller. */
  return result;
} /*** end of MbRtuReceivePacket ***/


/************************************************************************************//**
** \brief     Receives a communication interface byte if one is present.
** \param     data Pointer to byte where the data is to be stored.
** \return    BLT_TRUE if a byte was received, BLT_FALSE otherwise.
**
****************************************************************************************/
static blt_bool MbRtuReceiveByte(blt_int8u *data)
{
  blt_bool result = BLT_FALSE;

  /* check if a new byte was received by means of the RDRF-bit. */
  if (((LPUARTx->STAT & LPUART_STAT_RDRF_MASK) >> LPUART_STAT_RDRF_SHIFT) != 0U)
  {
    /* update the result */
    result = BLT_TRUE;
    /* check for a frame error. the frame error check is important because it can detect
     * a missing stopbit. on an RS485 bus without bias resistors, the A-B differential
     * voltage is 0. for an RS485 transceiver this is neither a 0 nor a 1 bit, so
     * undefined. most RS485 transceivers feature a reception failsafe function to drive
     * the Rx output (going to the UART Rx) to a defined state of logic 1. in case the
     * used RS485 transceiver doesn't have such a feature, it typically leaves the Rx
     * output in a logic 0 state. this means that after the stop bit of the last packet
     * byte, the UART Rx input sees a logic 0, and assumes it is a start bit. the
     * remaining data bits will always be 0 and, most importantly no stop bit is
     * present, causing a framing error. Long story short: if you don't check for the
     * framing error flag, you might receive an extra byte with value 0, which is not
     * actually transmitted on the RS485 bus. you can catch and ignore this byte by doing
     * a frame error check.
     */
    if (((LPUARTx->STAT & LPUART_STAT_FE_MASK) >> LPUART_STAT_FE_SHIFT) != 0U)
    {
      /* clear the error flag by writing a 1 to its bit location. */
      LPUARTx->STAT |= LPUART_STAT_FE_MASK;
      /* ignore the byte because of a detected frame error. */
      result = BLT_FALSE;
    }
    #if (BOOT_COM_MBRTU_PARITY > 0)
    /* check for a parity error. */
    if (((LPUARTx->STAT & LPUART_STAT_PF_MASK) >> LPUART_STAT_PF_SHIFT) != 0U)
    {
      /* clear the error flag by writing a 1 to its bit location. */
      LPUARTx->STAT |= LPUART_STAT_PF_MASK;
      /* ignore the byte because of a detected parity error. */
      result = BLT_FALSE;
    }
    #endif
    /* retrieve and store the newly received byte. */
    *data = LPUARTx->DATA;
  }
  /* give the result back to the caller */
  return result;
} /*** end of MbRtuReceiveByte ***/


/************************************************************************************//**
** \brief     Transmits a communication interface byte.
** \param     data Value of byte that is to be transmitted.
** \param     end_of_packet BLT_TRUE if this is the last byte of the packet, BLT_FALSE
**            otherwise.
** \return    none.
**
****************************************************************************************/
static void MbRtuTransmitByte(blt_int8u data, blt_bool end_of_packet)
{
  blt_int32u timeout;

  /* write the byte value in 'data' to the transmit register of the UART peripheral such
   * that the transmission of the byte value is started.
   */
  LPUARTx->DATA = data;
  /* set timeout time to wait for transmit completion. */
  timeout = TimerGet() + MBRTU_BYTE_TX_TIMEOUT_MS;

  /* not the last byte of the packet? */
  if (end_of_packet == BLT_FALSE)
  {
    /* wait for tx holding register to be empty */
    while (((LPUARTx->STAT & LPUART_STAT_TDRE_MASK) >> LPUART_STAT_TDRE_SHIFT) == 0U)
    {
      /* keep the watchdog happy. */
      CopService();
      /* break loop upon timeout. this would indicate a hardware failure. */
      if (TimerGet() > timeout)
      {
        break;
      }
    }
  }
  /* this is the last byte of a packet. */
  else
  {
    /* wait for tx complete flag to be set. this is needed for the last byte, otherwise
     * the transceiver's transmit output gets disabled with 
     * MbRtuDriverOutputControlHook() before the byte reception completes. 
     */
    while (((LPUARTx->STAT & LPUART_STAT_TC_MASK) >> LPUART_STAT_TC_SHIFT) == 0U)
    {
      /* keep the watchdog happy. */
      CopService();
      /* break loop upon timeout. this would indicate a hardware failure. */
      if (TimerGet() > timeout)
      {
        break;
      }
    }
  }
} /*** end of MbRtuTransmitByte ***/


/************************************************************************************//**
** \brief     Obtains the counter value of the 100 kHz free running counter. Note that
**            each count represent 10 us. The Modbus RTU communication module uses this
**            free running counter for Modbus RTU packet timing related purposes. The
**            already available 1 ms timer does not have the needed resolution for this
**            purpose.
** \return    Current value of the free running counter.
**
****************************************************************************************/
blt_int16u MbRtuFreeRunningCounterGet(void)
{
  static blt_bool initialized = BLT_FALSE;
  static blt_int32u counts_per_ten_us = 0;
  static blt_int32u free_running_counter_last = 0;
  static blt_int16u ten_us_tick_counter = 0;
  blt_int32u free_running_counter_now;
  blt_int32u delta_counts;
  blt_int32u ten_us_ticks;

  /* perform one-time initialization.*/
  if (initialized == BLT_FALSE)
  {
    /* set the flag to make sure that this part only runs once. */
    initialized = BLT_TRUE;
    /* calculate how many counts of the SysTick's free running counter equals 10us,
     * because one count of a 100 kHz counter equals that. Note that the timer module
     * already initialized and enable the SysTick.
     */
    counts_per_ten_us = SystemCoreClock / 100000U;
    /* initialize current value of the timer's free running counter. */
    free_running_counter_last = (S32_SysTick->CVR & S32_SysTick_CVR_CURRENT_MASK);
  }

  /* get the current value of the free running counter. */
  free_running_counter_now = (S32_SysTick->CVR & S32_SysTick_CVR_CURRENT_MASK);
  /* Calculate the number of counts that passed since the detection of the last
   * millisecond event. Keep in mind that the SysTick has a down-counting 24-bit free
   * running counter. Note that this calculation also works, in case the free running
   * counter overflowed, thanks to integer math.
   */
  delta_counts = free_running_counter_last - free_running_counter_now;
  delta_counts &= S32_SysTick_CVR_CURRENT_MASK;

  /* did one or more ten microsecond counts pass since the last event? */
  if (delta_counts >= counts_per_ten_us)
  {
    /* calculate how many ten microsecond counts passed. */
    ten_us_ticks = delta_counts / counts_per_ten_us;
    /* update the counter. */
    ten_us_tick_counter += ten_us_ticks;
    /* store the counter value of the last event to detect the next one. */
    free_running_counter_last -= (ten_us_ticks * counts_per_ten_us);
    free_running_counter_last &= S32_SysTick_CVR_CURRENT_MASK;
  }

  /* return the current value of the 100 kHz free running counter to the caller. */
  return ten_us_tick_counter;
} /*** end of MbRtuFreeRunningCounterGet ***/
#endif /* BOOT_COM_MBRTU_ENABLE > 0 */

/*********************************** end of mbrtu.c ************************************/
