/************************************************************************************//**
* \file         Source/ARMCM3_EFM32/timer.c
* \brief        Bootloader timer driver source file.
* \ingroup      Target_ARMCM3_EFM32
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
#include "boot.h"                                /* bootloader generic header          */
#include "efm32.h"


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Local variable for storing the number of milliseconds that have elapsed since
 *         startup.
 */
static blt_int32u millisecond_counter;

/** \brief Buffer for storing the last value of the free running counter. */
static blt_int32u free_running_counter_last;

/** \brief Stores the number of counts of the free running counter that equals one
 *         millisecond.
 */
static blt_int32u counts_per_millisecond;


/************************************************************************************//**
** \brief     Initializes the polling based millisecond timer driver.
** \return    none.
**
****************************************************************************************/
void TimerInit(void)
{
  /* Reset the timer configuration. */
  TimerReset();
  /* Configure the reload value such that the full 24-bit range is used. */
  SysTick->LOAD = (0xFFFFFFFFUL & SysTick_LOAD_RELOAD_Msk);
  /* Reset the current counter value. */
  SysTick->VAL = 0UL;
  /* Select core clock as source and enable the timer. */
  SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
  /* Calculate how many counts equal 1 millisecond. */
  counts_per_millisecond = SystemCoreClockGet() / 1000U;
  /* Initialize remaining locals. */
  millisecond_counter = 0;
  free_running_counter_last = (SysTick->VAL & SysTick_VAL_CURRENT_Msk);
} /*** end of TimerInit ***/


/************************************************************************************//**
** \brief     Reset the timer by placing the timer back into it's default reset
**            configuration.
** \return    none.
**
****************************************************************************************/
void TimerReset(void)
{
  /* set the systick's registers back into the default reset value */
  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;
  SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk;
} /*** end of TimerReset ***/


/************************************************************************************//**
** \brief     Updates the millisecond timer.
** \return    none.
**
****************************************************************************************/
void TimerUpdate(void)
{
  blt_int32u free_running_counter_now;
  blt_int32u delta_counts;
  blt_int32u ms_counts;

  /* Get the current value of the free running counter. */
  free_running_counter_now = (SysTick->VAL & SysTick_VAL_CURRENT_Msk);
  /* Calculate the number of counts that passed since the detection of the last
   * millisecond event. Keep in mind that the SysTick has a down-counting 24-bit free
   * running counter. Note that this calculation also works, in case the free running
   * counter overflowed, thanks to integer math.
   */
  delta_counts = free_running_counter_last - free_running_counter_now;
  delta_counts &= SysTick_VAL_CURRENT_Msk;

  /* Did one or more milliseconds pass since the last event? */
  if (delta_counts >= counts_per_millisecond)
  {
    /* Calculate how many milliseconds passed. */
    ms_counts = delta_counts / counts_per_millisecond;
    /* Update the millisecond counter. */
    millisecond_counter += ms_counts;
    /* Store the counter value of the last millisecond event, to detect the next one. */
    free_running_counter_last -= (ms_counts * counts_per_millisecond);
    free_running_counter_last &= SysTick_VAL_CURRENT_Msk;
  }
} /*** end of TimerUpdate ***/


/************************************************************************************//**
** \brief     Obtains the counter value of the millisecond timer.
** \return    Current value of the millisecond timer.
**
****************************************************************************************/
blt_int32u TimerGet(void)
{
  /* updating timer here allows this function to be called in a loop with timeout
   * detection.
   */
  TimerUpdate();
  /* read and return the amount of milliseconds that passed since initialization */
  return millisecond_counter;
} /*** end of TimerGet ***/


/*********************************** end of timer.c ************************************/
