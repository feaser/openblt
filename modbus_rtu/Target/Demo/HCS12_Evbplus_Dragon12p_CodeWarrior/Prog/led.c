/************************************************************************************//**
* \file         Demo/HCS12_Evbplus_Dragon12p_CodeWarrior/Prog/led.c
* \brief        LED driver source file.
* \ingroup      Prog_HCS12_Evbplus_Dragon12p_CodeWarrior
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2013  by Feaser    http://www.feaser.com    All rights reserved
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
#define LED_TOGGLE_MS  (500)


/************************************************************************************//**
** \brief     Initializes the LED. 
** \return    none.
**
****************************************************************************************/
void LedInit(void)
{
  /* enable LEDs connected to PORTB */
  DDRJ_DDRJ1 = 1;
  PTJ_PTJ1 = 0;
  /* configure PB0 as a digital output */
  DDRB_BIT0 = 1;
  /* turn off the LED by default */
  PORTB_BIT0 = 0;
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

  /* check if toggle interval time passed */
  timer_counter_now = TimerGet();
  if ( (timer_counter_now - timer_counter_last) < LED_TOGGLE_MS)
  {
    /* not yet time to toggle */
    return;
  }
  
  /* determine toggle action */
  if (led_toggle_state == 0)
  {
    led_toggle_state = 1;
    /* turn the LED on */
    PORTB_BIT0 = 1;

  }
  else
  {
    led_toggle_state = 0;
    /* turn the LED off */
    PORTB_BIT0 = 0;
  }

  /* store toggle time to determine next toggle interval */
  timer_counter_last = timer_counter_now;
} /*** end of LedToggle ***/


/*********************************** end of led.c **************************************/
