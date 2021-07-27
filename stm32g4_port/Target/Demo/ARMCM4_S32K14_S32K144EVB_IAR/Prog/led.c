/************************************************************************************//**
* \file         Demo/ARMCM4_S32K14_S32K144EVB_IAR/Prog/led.c
* \brief        LED driver source file.
* \ingroup      Prog_ARMCM4_S32K14_S32K144EVB_IAR
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
* Macro definitions
****************************************************************************************/
/** \brief Toggle interval time in milliseconds. */
#define LED_TOGGLE_MS  (500U)


/************************************************************************************//**
** \brief     Initializes the LED. 
** \return    none.
**
****************************************************************************************/
void LedInit(void)
{
  /* LED GPIO pin configuration. PD0 = GPIO, MUX = ALT1. */
  PORTD->PCR[0] |= PORT_PCR_MUX(1);
  /* configure Port D pin 0 GPIO as digital output */
  PTD->PDDR |= GPIO_PDDR_PDD(0x00000001);
  /* turn the LED off on Port D pin 0 */
  PTD->PSOR |= GPIO_PSOR_PTSO(0x00000001);
} /*** end of LedInit ***/


/************************************************************************************//**
** \brief     Toggles the LED at a fixed time interval.
** \return    none.
**
****************************************************************************************/
void LedToggle(void)
{
  static unsigned char led_toggle_state = 0;
  static unsigned long timer_counter_last = 0;
  unsigned long timer_counter_now;

  /* Check if toggle interval time passed. */
  timer_counter_now = TimerGet();
  if ( (timer_counter_now - timer_counter_last) < LED_TOGGLE_MS)
  {
    /* Not yet time to toggle. */
    return;
  }

  /* Determine toggle action. */
  if (led_toggle_state == 0)
  {
    led_toggle_state = 1;
    /* Turn the LED on. */
    PTD->PCOR |= GPIO_PCOR_PTCO(0x00000001);
  }
  else
  {
    led_toggle_state = 0;
    /* Turn the LED off. */
    PTD->PSOR |= GPIO_PSOR_PTSO(0x00000001);
  }

  /* Store toggle time to determine next toggle interval. */
  timer_counter_last = timer_counter_now;
} /*** end of LedToggle ***/


/*********************************** end of led.c **************************************/
