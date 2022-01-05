/************************************************************************************//**
* \file         Source/ARMCM0_S32K11/uart.c
* \brief        Bootloader RS232 communication interface source file.
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
#if (BOOT_COM_RS232_ENABLE > 0)
#include "device_registers.h"                    /* device registers                   */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout time for the reception of a CTO packet. The timer is started upon
 *         reception of the first packet byte.
 */
#define RS232_CTO_RX_PACKET_TIMEOUT_MS (100U)
/** \brief Timeout for transmitting a byte in milliseconds. */
#define RS232_BYTE_TX_TIMEOUT_MS       (10U)

#if (BOOT_COM_RS232_CHANNEL_INDEX == 0)
/** \brief Set the peripheral LPUART0 base pointer. */
#define LPUARTx                        (LPUART0)
/** \brief Set the PCC index offset for LPUART0. */
#define PCC_LPUARTx_INDEX              (PCC_LPUART0_INDEX)
#elif (BOOT_COM_RS232_CHANNEL_INDEX == 1)
/** \brief Set the peripheral LPUART1 base pointer. */
#define LPUARTx                        (LPUART1)
/** \brief Set the PCC index offset for LPUART1. */
#define PCC_LPUARTx_INDEX              (PCC_LPUART1_INDEX)
#endif


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static blt_bool Rs232ReceiveByte(blt_int8u *data);
static void     Rs232TransmitByte(blt_int8u data);


/************************************************************************************//**
** \brief     Initializes the RS232 communication interface.
** \return    none.
**
****************************************************************************************/
void Rs232Init(void)
{
  blt_int32u       sourceClockFreqHz;
  blt_int32u       div2RegValue;
  blt_int16u       baudrateSbr0_12;
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
  ASSERT_CT((BOOT_COM_RS232_CHANNEL_INDEX == 0) ||
            (BOOT_COM_RS232_CHANNEL_INDEX == 1));

  /* Make sure the UART peripheral clock is disabled before configuring its source
   * clock.
   */
  PCC->PCCn[PCC_LPUARTx_INDEX] &= ~PCC_PCCn_CGC_MASK;
  /* Select option 2 as the UART peripheral source clock and enable the clock. Option 2
   * is the SIRCDIV2_CLK, which is available on all peripherals and configurations.
   */
  PCC->PCCn[PCC_LPUARTx_INDEX] |= PCC_PCCn_PCS(2) | PCC_PCCn_CGC_MASK;
  /* Obtain the DIV2 divider value of the SIRC_CLK. */
  div2RegValue = (SCG->SIRCDIV & SCG_SIRCDIV_SIRCDIV2_MASK) >> SCG_SIRCDIV_SIRCDIV2_SHIFT;
  /* Check if the DIV2 register value for SIRC is 0. In this case SIRCDIV2_CLK is
   * currently disabled.
   */
  if (div2RegValue == 0U)
  {
    /* Configure the DIV2 for a default divide by 1 to make sure the SIRCDIV2_CLK is
     * actually enabled.
     */
    div2RegValue = 1U;
    SCG->SIRCDIV |= SCG_SIRCDIV_SIRCDIV2(div2RegValue);
  }
  /* Determine the SIRC clock frequency. If SIRC high range is enabled, it is 8 MHz. If
   * SIRC low range is enabled, it is 2 MHz.
   */
  sourceClockFreqHz = 8000000U;
  if ((SCG->SIRCCFG & SCG_SIRCCFG_RANGE_MASK) == SCG_SIRCCFG_RANGE(0))
  {
    sourceClockFreqHz = 2000000U;
  }
  /* Now process the configured DIV2 divider factor to get the actual frequency of the
   * UART peripheral source clock.
   */
  sourceClockFreqHz /= div2DividerLookup[div2RegValue];
  /* Configure the baudrate from BOOT_COM_RS232_BAUDRATE, taking into account that an
   * oversampling of 8 will be configured. Default 8,n,1 format is used. Integer
   * rounding is used to get the best value for baudrateSbr0_12. Actual baudrate equals
   * sourceClockFreqHz / 8 / baudrateSbr0_12.
   */
  baudrateSbr0_12 = (((sourceClockFreqHz / BOOT_COM_RS232_BAUDRATE) + (8U - 1U)) / 8U) &
                    LPUART_BAUD_SBR_MASK;
  /* OSR=7: Over sampling ratio = 7+1=8.
   * SBNS=0: One stop bit.
   * BOTHEDGE=0: receiver samples only on rising edge.
   * M10=0: Rx and Tx use 7 to 9 bit data characters.
   * RESYNCDIS=0: Resync during rec'd data word supported.
   * LBKDIE, RXEDGIE=0: interrupts disable.
   * TDMAE, RDMAE, TDMAE=0: DMA requests disabled.
   * MAEN1, MAEN2,  MATCFG=0: Match disabled.
   */
  LPUARTx->BAUD = LPUART_BAUD_SBR(baudrateSbr0_12) | LPUART_BAUD_OSR(7);
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
  LPUARTx->CTRL = LPUART_CTRL_RE_MASK | LPUART_CTRL_TE_MASK;
} /*** end of Rs232Init ***/


/************************************************************************************//**
** \brief     Transmits a packet formatted for the communication interface.
** \param     data Pointer to byte array with data that it to be transmitted.
** \param     len  Number of bytes that are to be transmitted.
** \return    none.
**
****************************************************************************************/
void Rs232TransmitPacket(blt_int8u *data, blt_int8u len)
{
  blt_int16u data_index;

  /* Verify validity of the len-paramenter. */
  ASSERT_RT(len <= BOOT_COM_RS232_TX_MAX_DATA);

  /* First transmit the length of the packet. */
  Rs232TransmitByte(len);

  /* Transmit all the packet bytes one-by-one. */
  for (data_index = 0U; data_index < len; data_index++)
  {
    /* Keep the watchdog happy. */
    CopService();
    /* Write byte. */
    Rs232TransmitByte(data[data_index]);
  }
} /*** end of Rs232TransmitPacket ***/


/************************************************************************************//**
** \brief     Receives a communication interface packet if one is present.
** \param     data Pointer to byte array where the data is to be stored.
** \param     len Pointer where the length of the packet is to be stored.
** \return    BLT_TRUE if a packet was received, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool Rs232ReceivePacket(blt_int8u *data, blt_int8u *len)
{
  static blt_int8u xcpCtoReqPacket[BOOT_COM_RS232_RX_MAX_DATA+1U];  /* One extra for length. */
  static blt_int8u xcpCtoRxLength;
  static blt_bool  xcpCtoRxInProgress = BLT_FALSE;
  static blt_int32u xcpCtoRxStartTime = 0U;

  /* Start of cto packet received? */
  if (xcpCtoRxInProgress == BLT_FALSE)
  {
    /* Store the message length when received. */
    if (Rs232ReceiveByte(&xcpCtoReqPacket[0]) == BLT_TRUE)
    {
      if ( (xcpCtoReqPacket[0] > 0U) &&
           (xcpCtoReqPacket[0] <= BOOT_COM_RS232_RX_MAX_DATA) )
      {
        /* Store the start time. */
        xcpCtoRxStartTime = TimerGet();
        /* Reset packet data count. */
        xcpCtoRxLength = 0U;
        /* Indicate that a cto packet is being received. */
        xcpCtoRxInProgress = BLT_TRUE;
      }
    }
  }
  else
  {
    /* Store the next packet byte. */
    if (Rs232ReceiveByte(&xcpCtoReqPacket[xcpCtoRxLength+1U]) == BLT_TRUE)
    {
      /* Increment the packet data count. */
      xcpCtoRxLength++;

      /* Check to see if the entire packet was received. */
      if (xcpCtoRxLength == xcpCtoReqPacket[0])
      {
        /* Copy the packet data. */
        CpuMemCopy((blt_int32u)data, (blt_int32u)&xcpCtoReqPacket[1], xcpCtoRxLength);
        /* Done with cto packet reception. */
        xcpCtoRxInProgress = BLT_FALSE;
        /* Set the packet length. */
        *len = xcpCtoRxLength;
        /* Packet reception complete. */
        return BLT_TRUE;
      }
    }
    else
    {
      /* Check packet reception timeout. */
      if (TimerGet() > (xcpCtoRxStartTime + RS232_CTO_RX_PACKET_TIMEOUT_MS))
      {
        /* Cancel cto packet reception due to timeout. Note that that automaticaly
         * discards the already received packet bytes, allowing the host to retry.
         */
        xcpCtoRxInProgress = BLT_FALSE;
      }
    }
  }
  /* Packet reception not yet complete. */
  return BLT_FALSE;
} /*** end of Rs232ReceivePacket ***/


/************************************************************************************//**
** \brief     Receives a communication interface byte if one is present.
** \param     data Pointer to byte where the data is to be stored.
** \return    BLT_TRUE if a byte was received, BLT_FALSE otherwise.
**
****************************************************************************************/
static blt_bool Rs232ReceiveByte(blt_int8u *data)
{
  blt_bool result = BLT_FALSE;

  /* Check if a new byte was received by means of the RDRF-bit. */
  if (((LPUARTx->STAT & LPUART_STAT_RDRF_MASK) >> LPUART_STAT_RDRF_SHIFT) != 0U)
  {
    /* Retrieve and store the newly received byte. */
    *data = LPUARTx->DATA;
    /* Update the result. */
    result = BLT_TRUE;
  }
  
  /* Give the result back to the caller. */
  return result;
} /*** end of Rs232ReceiveByte ***/


/************************************************************************************//**
** \brief     Transmits a communication interface byte.
** \param     data Value of byte that is to be transmitted.
** \return    none.
**
****************************************************************************************/
static void Rs232TransmitByte(blt_int8u data)
{
  blt_int32u timeout;

  /* Write the byte value in 'data' to the transmit register of the UART peripheral such
   * that the transmission of the byte value is started.
   */
  LPUARTx->DATA = data;
  /* Set timeout time to wait for transmit completion. */
  timeout = TimerGet() + RS232_BYTE_TX_TIMEOUT_MS;
  /* Wait for tx holding register to be empty. */
  while (((LPUARTx->STAT & LPUART_STAT_TDRE_MASK) >> LPUART_STAT_TDRE_SHIFT) == 0U)
  {
    /* Keep the watchdog happy. */
    CopService();
    /* Break loop upon timeout. this would indicate a hardware failure. */
    if (TimerGet() > timeout)
    {
      break;
    }
  }
} /*** end of Rs232TransmitByte ***/
#endif /* BOOT_COM_RS232_ENABLE > 0 */


/*********************************** end of rs232.c ************************************/
