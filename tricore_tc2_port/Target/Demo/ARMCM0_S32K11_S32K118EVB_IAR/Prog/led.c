/************************************************************************************//**
* \file         Demo/ARMCM0_S32K14_S32K118EVB_IAR/Prog/led.c
* \brief        LED driver source file.
* \ingroup      Prog_ARMCM0_S32K14_S32K118EVB_IAR
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
  /* LED GPIO pin configuration. PE8 = GPIO, MUX = ALT1. */
  PORTE->PCR[8] = PORT_PCR_MUX(1);
  /* Configure Port E pin 8 GPIO as digital output. */
  PTE->PDDR |= GPIO_PDDR_PDD(1 << 8U);
  /* Turn the LED off on Port E pin 8. */
  PTE->PCOR |= GPIO_PSOR_PTSO(1 << 8U);
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
    PTE->PSOR |= GPIO_PSOR_PTSO(1 << 8U);
  }
  else
  {
    led_toggle_state = 0;
    /* Turn the LED off. */
    PTE->PCOR |= GPIO_PSOR_PTSO(1 << 8U);
  }

  /* Store toggle time to determine next toggle interval. */
  timer_counter_last = timer_counter_now;
} /*** end of LedToggle ***/


/*********************************** end of led.c **************************************/
