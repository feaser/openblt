/************************************************************************************//**
* \file         Demo/ARMCM0_S32K14_S32K118EVB_GCC/Boot/led.c
* \brief        LED driver source file.
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
#include "led.h"                                 /* module header                      */
#include "device_registers.h"                    /* device registers                   */


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
  /* LED GPIO pin configuration. PE8 = GPIO, MUX = ALT1. */
  PORTE->PCR[8] = PORT_PCR_MUX(1);
  /* Configure Port E pin 8 GPIO as digital output. */
  PTE->PDDR |= GPIO_PDDR_PDD(1 << 8U);
  /* Turn the LED off on Port E pin 8. */
  PTE->PCOR |= GPIO_PSOR_PTSO(1 << 8U);
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
      ledOn = BLT_TRUE;
      /* Turn the LED on. */
      PTE->PSOR |= GPIO_PSOR_PTSO(1 << 8U);
    }
    else
    {
      ledOn = BLT_FALSE;
      /* Turn the LED off. */
      PTE->PCOR |= GPIO_PSOR_PTSO(1 << 8U);
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
  /* Turn the LED off. */
  PTE->PCOR |= GPIO_PSOR_PTSO(1 << 8U);
} /*** end of LedBlinkExit ***/


/*********************************** end of led.c **************************************/
