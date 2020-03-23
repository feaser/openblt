/************************************************************************************//**
* \file         Demo/ARMCM3_EFM32_Olimex_EM32G880F128STK_Crossworks/Prog/main.c
* \brief        Demo program application source file.
* \ingroup      Prog_ARMCM3_EFM32_Olimex_EM32G880F128STK_Crossworks
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2012  by Feaser    http://www.feaser.com    All rights reserved
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
static void Init(void);


/************************************************************************************//**
** \brief     This is the entry point for the bootloader application and is called 
**            by the reset interrupt vector after the C-startup routines executed.
** \return    Program return code.
**
****************************************************************************************/
int main(void)
{
  /* initialize the microcontroller */
  Init();
  /* initialize the bootloader interface */
  BootComInit();

  /* start the infinite program loop */
  while (1)
  {
    /* toggle LED with a fixed frequency */
    LedToggle();
    /* check for bootloader activation request */
    BootComCheckActivationRequest();
  }

  /* program should never get here */
  return 0;
} /*** end of main ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller.
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  /* initialize the system and its clocks */
  SystemInit();
  /* handle chip errate workarounds */
  CHIP_Init();
  /* enable the low frequency crystal oscillator */
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
  /* turn on clocking of all the modules */
  CMU->HFCORECLKEN0 |= 0x0F;
  CMU->HFPERCLKEN0 |= 0xFFFF;
  /* disable clocking of the modules that are not in use */
  CMU_ClockEnable(cmuClock_AES, false);
  CMU_ClockEnable(cmuClock_DMA, false);
  CMU_ClockEnable(cmuClock_EBI, false);
  CMU_ClockEnable(cmuClock_PRS, false);
  CMU_ClockEnable(cmuClock_USART0, false);
  CMU_ClockEnable(cmuClock_USART1, false);
  CMU_ClockEnable(cmuClock_USART2, false);
  CMU_ClockEnable(cmuClock_UART0, false);
  CMU_ClockEnable(cmuClock_ACMP0, false);
  CMU_ClockEnable(cmuClock_ACMP1, false);
  CMU_ClockEnable(cmuClock_DAC0, false);
  CMU_ClockEnable(cmuClock_ADC0, false);
  CMU_ClockEnable(cmuClock_I2C0, false);
  CMU_ClockEnable(cmuClock_VCMP, false);
#if (BOOT_COM_RS232_ENABLE > 0)
  /* enable power to U2 (RS232_PWR_E) */
  GPIO_PinModeSet(gpioPortB, 9, gpioModePushPullDrive, 1);
  /* set port B outputs to drive up to 20 mA */
  GPIO_DriveModeSet(gpioPortB, gpioDriveModeHigh);
#endif
  /* init the led driver */
  LedInit();
  /* init the timer driver */
  TimerInit();
} /*** end of Init ***/


/*********************************** end of main.c *************************************/
