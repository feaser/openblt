/************************************************************************************//**
* \file         Demo/ARMCM0_XMC1_XMC1400_Boot_Kit_GCC/Boot/led.c
* \brief        LED driver source file.
* \ingroup      Boot_ARMCM0_XMC1_XMC1400_Boot_Kit_GCC
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2017  by Feaser    http://www.feaser.com    All rights reserved
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
#include "led.h"                                 /* module header                      */
#include "xmc_gpio.h"                            /* GPIO module                        */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Holds the desired LED blink interval time. */
static blt_int16u ledBlinkIntervalMs;


/************************************************************************************//**
** \brief     Initializes the LED blink driver.
** \param     interval_ms Specifies the desired LED blink interval time in milliseconds.
** \return    none.
**
****************************************************************************************/
void LedBlinkInit(blt_int16u interval_ms)
{
  /* initialize and turn of the LED */
  XMC_GPIO_SetMode(P4_0, XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
  XMC_GPIO_SetOutputLevel(P4_0, XMC_GPIO_OUTPUT_LEVEL_HIGH);
  /* store the interval time between LED toggles */
  ledBlinkIntervalMs = interval_ms;
} /*** end of LedBlinkInit ***/


/************************************************************************************//**
** \brief     Task function for blinking the LED as a fixed timer interval.
** \return    none.
**
****************************************************************************************/
void LedBlinkTask(void)
{
  static blt_bool ledOn = BLT_FALSE;
  static blt_int32u nextBlinkEvent = 0;

  /* check for blink event */
  if (TimerGet() >= nextBlinkEvent)
  {
    /* toggle the LED state */
    if (ledOn == BLT_FALSE)
    {
      /* turn the LED on */
      ledOn = BLT_TRUE;
      XMC_GPIO_SetOutputLevel(P4_0, XMC_GPIO_OUTPUT_LEVEL_LOW);
    }
    else
    {
      /* turn the LED off */
      ledOn = BLT_FALSE;
      XMC_GPIO_SetOutputLevel(P4_0, XMC_GPIO_OUTPUT_LEVEL_HIGH);
    }
    /* schedule the next blink event */
    nextBlinkEvent = TimerGet() + ledBlinkIntervalMs;
  }
} /*** end of LedBlinkTask ***/


/************************************************************************************//**
** \brief     Cleans up the LED blink driver. This is intended to be used upon program
**            exit.
** \return    none.
**
****************************************************************************************/
void LedBlinkExit(void)
{
  /* turn the LED off */
  XMC_GPIO_SetOutputLevel(P4_0, XMC_GPIO_OUTPUT_LEVEL_HIGH);
} /*** end of LedBlinkExit ***/


/*********************************** end of led.c **************************************/
