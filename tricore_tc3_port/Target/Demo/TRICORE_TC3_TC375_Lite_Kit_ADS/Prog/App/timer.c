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
  /* Increment the millisecond counter. */
  millisecond_counter++;
  /* Reschedule the system timer event for one millisecond from now. */
  IfxStm_clearCompareFlag(&MODULE_STM0, IfxStm_Comparator_0);
  IfxStm_increaseCompare(&MODULE_STM0, IfxStm_Comparator_0, counts_per_millisecond);
} /*** end of TimerInterrupt ***/


/*********************************** end of timer.c ************************************/
