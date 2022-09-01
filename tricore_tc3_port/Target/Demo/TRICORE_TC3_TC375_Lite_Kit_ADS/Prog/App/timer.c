/************************************************************************************//**
* \file         Demo/TRICORE_TC3_TC375_Lite_Kit_ADS/Prog/App/timer.c
* \brief        Timer driver source file.
* \ingroup      Prog_TRICORE_TC3_TC375_Lite_Kit_ADS
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2022  by Feaser    http://www.feaser.com    All rights reserved
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
/** \brief Configures the interrupt priority for the system timer (STM). */
#define TIMER_ISR_PRIORITY             40


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Local variable for storing the number of milliseconds that have elapsed since
 *         startup.
 */
static volatile unsigned long millisecond_counter;

/** \brief Stores the number of counts of the free running counter that equals one
 *         millisecond.
 */
static volatile unsigned long counts_per_millisecond;


/************************************************************************************//**
** \brief     Initializes the timer.
** \return    none.
**
****************************************************************************************/
void TimerInit(void)
{
  IfxStm_CompareConfig timer_config;

  /* Reset the millisecond counter value. */
  millisecond_counter = 0;
  /* Calculate how many STM free running counter counts equal one millisecond. */
  counts_per_millisecond = IfxStm_getTicksFromMilliseconds(&MODULE_STM0, 1);
  /* Reset the configuration data structure. */
  IfxStm_initCompareConfig(&timer_config);
  /* Set configuration data to generate an event in one millisecond from now. */
  timer_config.comparator = IfxStm_Comparator_0;
  timer_config.comparatorInterrupt = IfxStm_ComparatorInterrupt_ir0;
  timer_config.compareOffset = IfxStm_ComparatorOffset_0;
  timer_config.compareSize = IfxStm_ComparatorSize_32Bits;
  timer_config.triggerPriority = TIMER_ISR_PRIORITY;
  timer_config.typeOfService = IfxSrc_Tos_cpu0;
  timer_config.ticks = counts_per_millisecond;
  /* Initialize the system timer peripheral. */
  IfxStm_initCompare(&MODULE_STM0, &timer_config);
} /*** end of TimerInit ***/


/************************************************************************************//**
** \brief     Obtains the counter value of the millisecond timer.
** \return    Current value of the millisecond timer.
**
****************************************************************************************/
unsigned long TimerGet(void)
{
  /* Read and return the tick counter value. */
  return millisecond_counter;
} /*** end of TimerGet ***/


/************************************************************************************//**
** \brief     Interrupt service routine of the timer.
** \return    none.
**
****************************************************************************************/
IFX_INTERRUPT(TimerInterrupt, 0, TIMER_ISR_PRIORITY);
void TimerInterrupt(void)
{
  unsigned long free_running_counter_now;
  unsigned long compare_counter;
  unsigned long delta_counts;

  /* Increment the millisecond counter. */
  millisecond_counter++;
  /* Reschedule the system timer event for one millisecond from the last time it was
   * scheduled. Note that this is relative to the last event and not to the current
   * counter value. Otherwise you won't get an accurate one millisecond interval.
   */
  IfxStm_increaseCompare(&MODULE_STM0, IfxStm_Comparator_0, counts_per_millisecond);
  /* Get the current value of the lower 32-bits of the free running counter. */
  free_running_counter_now = IfxStm_getLower(&MODULE_STM0);
  /* Read out the current counter value of the compare register, which is also configured
   * as the lower 32-bits.
   */
  compare_counter = IfxStm_getCompare(&MODULE_STM0, IfxStm_Comparator_0);
  /* Calculate the difference in counts. */
  delta_counts = compare_counter - free_running_counter_now;
  /* It should be less or equal than a millisecond. If not, then a counter overrun
   * occured. This can happen during debugging or when a higher priority interrupt
   * took longer than a millisecond to run.
   */
  if (delta_counts > counts_per_millisecond)
  {
    /* Missed the event due to a counter overrun. It will now take way too long for the
     * timer event to trigger, because the lower 32-bit of the free running counter needs
     * to completely wrap around to reach the compare register value. Intervene by
     * rescheduling the next timer event to be one millisecond from now, as opposed to
     * one millisecond since the last event.
     */
    IfxStm_updateCompare(&MODULE_STM0, IfxStm_Comparator_0,
                         free_running_counter_now + counts_per_millisecond);
  }
  /* Clear the interrupt flag for the next compare match interrupt to trigger. */
  IfxStm_clearCompareFlag(&MODULE_STM0, IfxStm_Comparator_0);
} /*** end of TimerInterrupt ***/


/*********************************** end of timer.c ************************************/
