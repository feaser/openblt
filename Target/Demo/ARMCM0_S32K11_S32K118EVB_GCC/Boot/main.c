/************************************************************************************//**
* \file         Demo/ARMCM0_S32K14_S32K118EVB_GCC/Boot/main.c
* \brief        Bootloader application source file.
* \ingroup      Boot_ARMCM0_S32K14_S32K118EVB_GCC
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
#include "device_registers.h"                    /* device registers                   */
#include "system_S32K118.h"                      /* device sconfiguration              */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);
static void SystemClockConfig(void);


/************************************************************************************//**
** \brief     This is the entry point for the bootloader application and is called
**            by the reset interrupt vector after the C-startup routines executed.
** \return    Program return code.
**
****************************************************************************************/
int main(void)
{
  /* Initialize the microcontroller. */
  Init();
  /* Initialize the bootloader. */
  BootInit();

  /* Start the infinite program loop. */
  while (1)
  {
    /* Run the bootloader task. */
    BootTask();
  }

  /* Program should never get here. */
  return 0;
} /*** end of main ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller.
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  /* Configure the system clock. */
  SystemClockConfig();
  /* Enable the peripheral clock for the ports that are used. */
  PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC_MASK;
  PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;
  PCC->PCCn[PCC_PORTE_INDEX] |= PCC_PCCn_CGC_MASK;

  /* Configure SW2 (PD3) GPIO pin for (optional) backdoor entry input. */
  /* Input GPIO pin configuration. PD3 = GPIO, MUX = ALT1. */
  PORTD->PCR[3] |= PORT_PCR_MUX(1);
  /* Disable pull device, as SW2 already has a pull down resistor on the board. */
  PORTD->PCR[3] &= ~PORT_PCR_PE(1);
  /* Configure and enable Port D pin 3 GPIO as digital input */
  PTD->PDDR &= ~GPIO_PDDR_PDD(1 << 3U);
  PTD->PIDR &= ~GPIO_PIDR_PID(1 << 3U);
#if (BOOT_COM_RS232_ENABLE > 0)
  /* UART RX GPIO pin configuration. PB0 = UART0 RX, MUX = ALT2. */
  PORTB->PCR[0] |= PORT_PCR_MUX(2);
  /* UART TX GPIO pin configuration. PB1 = UART0 TX, MUX = ALT2. */
  PORTB->PCR[1] |= PORT_PCR_MUX(2);
#endif
#if (BOOT_COM_CAN_ENABLE > 0)
  /* CAN RX GPIO pin configuration. PE4 = CAN0 RX, MUX = ALT5. */
  PORTE->PCR[4] |= PORT_PCR_MUX(5);
  /* CAN TX GPIO pin configuration. PE5 = CAN0 TX, MUX = ALT5. */
  PORTE->PCR[5] |= PORT_PCR_MUX(5);
#endif
} /*** end of Init ***/


/************************************************************************************//**
** \brief     System Clock Configuration. This code was derived from a S32 Design Studio
**            example program. It enables the SOCS (40 MHz external crystal), FIRC (48
**            MHz internal) and SIRC (8 MHz internal). The FIRC is used as a source for
**            the system clock and configures the normal RUN mode for the following clock
**            settings:
**            - CORE_CLK     =  48    MHz
**            - SYS_CLK      =  48    MHz
**            - BUS_CLK      =  48    MHz
**            - FLASH_CLK    =  24    MHz
**            - SOSCDIV1_CLK =  40    MHz
**            - SOSCDIV2_CLK =  40    MHz
**            - FIRCDIV1_CLK =  48    MHz
**            - FIRCDIV2_CLK =  48    MHz
**            - SIRCDIV1_CLK =   8    MHz
**            - SIRCDIV2_CLK =   8    MHz
** \return    none.
**
****************************************************************************************/
static void SystemClockConfig(void)
{
  /* --------- SOSC Initialization (40 MHz) ------------------------------------------ */
  /* SOSCDIV1 & SOSCDIV2 =1: divide by 1. */
  SCG->SOSCDIV = SCG_SOSCDIV_SOSCDIV1(1) | SCG_SOSCDIV_SOSCDIV2(1);
  /* Range=3: High freq (SOSC betw 8MHz - 40MHz).
   * HGO=0:   Config xtal osc for low power.
   * EREFS=1: Input is external XTAL.
   */
  SCG->SOSCCFG = SCG_SOSCCFG_RANGE(3) | SCG_SOSCCFG_EREFS_MASK;
  /* Ensure SOSCCSR unlocked. */
  while (SCG->SOSCCSR & SCG_SOSCCSR_LK_MASK)
  {
    ;
  }

  /* LK=0:          SOSCCSR can be written.
   * SOSCCMRE=0:    OSC CLK monitor IRQ if enabled.
   * SOSCCM=0:      OSC CLK monitor disabled.
   * SOSCERCLKEN=0: Sys OSC 3V ERCLK output clk disabled.
   * SOSCLPEN=0:    Sys OSC disabled in VLP modes.
   * SOSCSTEN=0:    Sys OSC disabled in Stop modes.
   * SOSCEN=1:      Enable oscillator.
   */
  SCG->SOSCCSR = SCG_SOSCCSR_SOSCEN_MASK;
  /* Wait for system OSC clock to become valid. */
  while (!(SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK))
  {
    ;
  }

  /* --------- FIRC Initialization --------------------------------------------------- */
  /* Fast IRC is enabled and trimmed to 48 MHz in reset (default). Enable FIRCDIV2_CLK
   * and FIRCDIV1_CLK, divide by 1 = 48 MHz.
   */
  SCG->FIRCDIV = SCG_FIRCDIV_FIRCDIV1(1) | SCG_FIRCDIV_FIRCDIV2(1);

  /* --------- SIRC Initialization --------------------------------------------------- */
  /* Slow IRC is enabled with high range (8 MHz) in reset. Enable SIRCDIV2_CLK and
   * SIRCDIV1_CLK, divide by 1 = 8MHz asynchronous clock source.
   */
  SCG->SIRCDIV = SCG_SIRCDIV_SIRCDIV1(1) | SCG_SIRCDIV_SIRCDIV2(1);

  /* --------- Change to normal RUN mode with 48MHz FIRC ----------------------------- */
  /* Select FIRC as clock source.
   * DIVCORE=0, div. by 1: Core clock = 48 MHz.
   * DIVBUS=0, div. by 1: bus clock = 48 MHz.
   * DIVSLOW=1, div. by 2: SCG slow, flash clock= 24 MHz
   */
  SCG->RCCR = SCG_RCCR_SCS(3) | SCG_RCCR_DIVCORE(0) | SCG_RCCR_DIVBUS(0) |
              SCG_RCCR_DIVSLOW(1);

  /* Wait until system clock source is FIRC. */
  while (((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT ) != 3U)
  {
    ;
  }
   /* Evaluate the clock register settings and calculates the current core clock. This
    * function must be called when the clock manager component is not used.
    */
   SystemCoreClockUpdate();
} /*** end of SystemClockConfig ***/


/*********************************** end of main.c *************************************/
