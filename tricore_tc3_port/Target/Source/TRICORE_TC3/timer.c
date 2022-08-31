/************************************************************************************//**
* \file         Source/TRICORE_TC3/timer.c
* \brief        Bootloader timer driver source file.
* \ingroup      Target_TRICORE_TC3
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
#include "boot.h"                                /* bootloader generic header          */
#include "Stm/Std/IfxStm.h"                      /* STM driver                         */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Local variable for storing the number of milliseconds that have elapsed since
 *         startup.
 */
static blt_int32u millisecond_counter = 0;

/** \brief Buffer for storing the last value of the lower 32-bits of the free running
 *         counter.
 */
static blt_int32u free_running_counter_last = 0;

/** \brief Stores the number of counts of the free running counter that equals one
 *         millisecond. The system timer (STM) is clocked by the system PLL (fPLL0). The
 *         STM clock frequency (fSTM) is the fPLL0 divided by the STMDIV bits in register
 *         CCUCON0. After a system reset, STMDIV is 3.
 */
static blt_int32u counts_per_millisecond = BOOT_CPU_SYSTEM_SPEED_KHZ / 3U;


/************************************************************************************//**
** \brief     Initializes the polling based millisecond timer driver.
** \return    none.
**
****************************************************************************************/
void TimerInit(void)
{
  /* Recalculate the number of counts of the free running counter that equals one
   * millisecond, just in case the startup software (Ssw) changed the STMDIV bits in
   * register CCUCON0.
   */
  counts_per_millisecond = IfxStm_getFrequency(&MODULE_STM0) / 1000U;
  /* Initialize the last free running counter variable, which is used for delta
   * calculations.
   */
  free_running_counter_last = IfxStm_getLower(&MODULE_STM0);
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
  float32 reset_stm_freq;

  /* The startup software could have changed the STMDIV bits in register CCUCON0. By
   * default it is 3 after a system reset. Calculate the default fSTM frequency after a
   * reset.
   */
  reset_stm_freq = IfxScuCcu_getSourceFrequency(IfxScuCcu_Fsource_0) / 3U;
  /* Did the startup software change it? */
  if (reset_stm_freq != IfxStm_getFrequency(&MODULE_STM0))
  {
    /* Restore the default configuration that resembles the system reset state. */
    (void)IfxScuCcu_setStmFrequency(reset_stm_freq);
  }
  /* Note that this timer driver only used the free running counter for reading purposes.
   * The free running counter regsiter is read-only and therefore cannot be reset to 0.
   * Consequently, nothing more needs to be done here.
   */
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

  /* Get the current value of the lower 32-bits of the free running counter. */
  free_running_counter_now = IfxStm_getLower(&MODULE_STM0);
  /* Calculate the number of counts that passed since the detection of the last
   * millisecond event. Note that this calculation also works, in case the free running
   * counter overflowed, thanks to integer math.
   */
  delta_counts = free_running_counter_now - free_running_counter_last;

  /* Did one or more milliseconds pass since the last event? */
  if (delta_counts >= counts_per_millisecond)
  {
    /* Calculate how many milliseconds passed. */
    ms_counts = delta_counts / counts_per_millisecond;
    /* Update the millisecond counter. */
    millisecond_counter += ms_counts;
    /* Store the counter value of the last millisecond event, to detect the next one. */
    free_running_counter_last += (ms_counts * counts_per_millisecond);
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
