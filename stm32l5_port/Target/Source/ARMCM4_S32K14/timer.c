/************************************************************************************//**
* \file         Source/ARMCM4_S32K14/timer.c
* \brief        Bootloader timer driver source file.
* \ingroup      Target_ARMCM4_S32K14
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


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Local variable for storing the number of milliseconds that have elapsed since
 *         startup.
 */
static blt_int32u millisecond_counter;


/************************************************************************************//**
** \brief     Initializes the polling based millisecond timer driver.
** \return    none.
**
****************************************************************************************/
void TimerInit(void)
{
  /* Reset the timer configuration. */
  TimerReset();
  /* Configure the systick frequency as a 1 ms event generator. */
  S32_SysTick->RVR = BOOT_CPU_SYSTEM_SPEED_KHZ - 1;
  /* Reset the current counter value. */
  S32_SysTick->CVR = 0u;
  /* Select core clock as source and enable the timer. */
  S32_SysTick->CSR = S32_SysTick_CSR_ENABLE_MASK | S32_SysTick_CSR_CLKSOURCE_MASK;
  /* Reset the millisecond counter value. */
  millisecond_counter = 0;
} /*** end of TimerInit ***/


/************************************************************************************//**
** \brief     Reset the timer by placing the timer back into it's default reset
**            configuration.
** \return    none.
**
****************************************************************************************/
void TimerReset(void)
{
  /* Set the systick's status and control register back into the default reset value. */
  S32_SysTick->CSR = 0;
} /* end of TimerReset */


/************************************************************************************//**
** \brief     Updates the millisecond timer.
** \return    none.
**
****************************************************************************************/
void TimerUpdate(void)
{
  /* Check if the millisecond event occurred. */
  if ((S32_SysTick->CSR & S32_SysTick_CSR_COUNTFLAG_MASK) != 0)
  {
    /* Increment the millisecond counter. */
    millisecond_counter++;
  }
} /*** end of TimerUpdate ***/


/************************************************************************************//**
** \brief     Obtains the counter value of the millisecond timer.
** \return    Current value of the millisecond timer.
**
****************************************************************************************/
blt_int32u TimerGet(void)
{
  /* Updating timer here allows this function to be called in a loop with timeout
   * detection.
   */
  TimerUpdate();
  /* Read and return the amount of milliseconds that passed since initialization. */
  return millisecond_counter;
} /*** end of TimerGet ***/


/*********************************** end of timer.c ************************************/
