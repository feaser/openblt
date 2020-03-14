/************************************************************************************//**
* \file         Demo/ARMCM4_S32K14_S32K144EVB_GCC/Prog/boot.c
* \brief        Demo program bootloader interface source file.
* \ingroup      Prog_ARMCM4_S32K14_S32K144EVB_GCC
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
#include "header.h"                                    /* generic header               */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
#if (BOOT_COM_RS232_ENABLE > 0)
static void BootComRs232Init(void);
static void BootComRs232CheckActivationRequest(void);
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
} /*** end of BootComCheckActivationRequest ***/


/************************************************************************************//**
** \brief     Bootloader activation function.
** \return    none.
**
****************************************************************************************/
void BootActivate(void)
{
  /* Activate the bootloader by performing a software reset. */
  SystemSoftwareReset();
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
/** \brief Set the peripheral LPUART base pointer. */
#define LPUARTx                        (LPUART1)
/** \brief Set the PCC index offset for LPUART. */
#define PCC_LPUARTx_INDEX              (PCC_LPUART1_INDEX)


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
  unsigned long        sourceClockFreqHz;
  unsigned long        div2RegValue;
  unsigned short       baudrateSbr0_12;
  unsigned char  const div2DividerLookup[] =
  {
     0, /* 0b000. Output disabled. */
     1, /* 0b001. Divide by 1. */
     2, /* 0b010. Divide by 2. */
     4, /* 0b011. Divide by 4. */
     8, /* 0b100. Divide by 8. */
    16, /* 0b101. Divide by 16. */
    32, /* 0b110. Divide by 32. */
    64, /* 0b111. Divide by 64. */
  };

  /* Make sure the UART peripheral clock is disabled before configuring its source
   * clock.
   */
  PCC->PCCn[PCC_LPUARTx_INDEX] &= ~PCC_PCCn_CGC_MASK;
  /* Select option 2 as the UART peripheral source clock and enable the clock. Option 2
   * is the SIRCDIV2_CLK, which is available on all peripherals and configurations.
   */
  PCC->PCCn[PCC_LPUARTx_INDEX] |= PCC_PCCn_PCS(0b010) | PCC_PCCn_CGC_MASK;
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
    SCG->SIRCDIV = SCG_SIRCDIV_SIRCDIV2(div2RegValue);
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
} /*** end of BootComRs232Init ***/


/************************************************************************************//**
** \brief     Receives the CONNECT request from the host, which indicates that the
**            bootloader should be activated and, if so, activates it.
** \return    none.
**
****************************************************************************************/
static void BootComRs232CheckActivationRequest(void)
{
  static unsigned char xcpCtoReqPacket[BOOT_COM_RS232_RX_MAX_DATA+1];
  static unsigned char xcpCtoRxLength;
  static unsigned char xcpCtoRxInProgress = 0;
  static unsigned long xcpCtoRxStartTime = 0;

  /* start of cto packet received? */
  if (xcpCtoRxInProgress == 0)
  {
    /* store the message length when received */
    if (Rs232ReceiveByte(&xcpCtoReqPacket[0]) == 1)
    {
      /* check that the length has a valid value. it should not be 0 */
      if ( (xcpCtoReqPacket[0] > 0) &&
           (xcpCtoReqPacket[0] <= BOOT_COM_RS232_RX_MAX_DATA) )
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
      if (xcpCtoRxLength == xcpCtoReqPacket[0])
      {
        /* done with cto packet reception */
        xcpCtoRxInProgress = 0;

        /* check if this was an XCP CONNECT command */
        if ((xcpCtoReqPacket[1] == 0xff) && (xcpCtoReqPacket[2] == 0x00))
        {
          /* connection request received so start the bootloader */
          BootActivate();
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
  unsigned char result = 0;

  /* Check if a new byte was received by means of the RDRF-bit. */
  if (((LPUARTx->STAT & LPUART_STAT_RDRF_MASK) >> LPUART_STAT_RDRF_SHIFT) != 0U)
  {
    /* Retrieve and store the newly received byte. */
    *data = LPUARTx->DATA;
    /* Update the result. */
    result = 1;
  }
  
  /* Give the result back to the caller. */
  return result;
} /*** end of Rs232ReceiveByte ***/
#endif /* BOOT_COM_RS232_ENABLE > 0 */


/*********************************** end of boot.c *************************************/
