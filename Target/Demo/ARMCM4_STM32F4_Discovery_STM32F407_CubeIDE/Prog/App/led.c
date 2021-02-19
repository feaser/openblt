/************************************************************************************//**
* \file         Demo/ARMCM4_STM32F4_Olimex_STM32E407_CubeIDE/Prog/App/led.c
* \brief        LED driver source file.
* \ingroup      Prog_ARMCM4_STM32F4_Olimex_STM32E407_CubeIDE
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
#define LED_TOGGLE_MS  (500)

#define SET_LED3_ORANGE(x)	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, x);
#define SET_LED4_GREEN(x)	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, x);
#define SET_LED5_RED(x)		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, x);
#define SET_LED6_BLUE(x)	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, x);

/************************************************************************************//**
** \brief     Initializes the LED.
** \return    none.
**
****************************************************************************************/
void LedInit(void)
{
  /* Note that the initialization of the LED GPIO pin is done in HAL_MspInit(). All that
   * is left to do here is to make sure the LED is turned off after initialization.
   */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  SET_LED3_ORANGE(GPIO_PIN_SET);
  SET_LED4_GREEN(GPIO_PIN_SET);
  SET_LED5_RED(GPIO_PIN_SET);
  SET_LED6_BLUE(GPIO_PIN_SET);
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
    SET_LED3_ORANGE(GPIO_PIN_RESET);
    SET_LED4_GREEN(GPIO_PIN_SET);
    SET_LED5_RED(GPIO_PIN_RESET);
    SET_LED6_BLUE(GPIO_PIN_SET);
  }
  else
  {
    led_toggle_state = 0;
    /* turn the LED off */
    SET_LED3_ORANGE(GPIO_PIN_SET);
    SET_LED4_GREEN(GPIO_PIN_RESET);
    SET_LED5_RED(GPIO_PIN_SET);
    SET_LED6_BLUE(GPIO_PIN_RESET);
  }

  /* store toggle time to determine next toggle interval */
  timer_counter_last = timer_counter_now;
} /*** end of LedToggle ***/


/*********************************** end of led.c **************************************/
