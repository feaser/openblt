/************************************************************************************//**
* \file         Source/HCS12/timer.c
* \brief        Bootloader timer driver source file.
* \ingroup      Target_HCS12
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
#include "boot.h"                                /* bootloader generic header          */


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Structure type with the layout of the timer related control registers. */
typedef struct
{
  volatile blt_int8u  tios;             /**< input capture/output compare select       */
  volatile blt_int8u  cforc;            /**< compare force register                    */
  volatile blt_int8u  oc7m;             /**< output compare 7 mask register            */
  volatile blt_int8u  oc7d;             /**< output compare 7 data register            */
  volatile blt_int16u tcnt;             /**< timer counter register                    */
  volatile blt_int8u  tscr1;            /**< system control register 1                 */
  volatile blt_int8u  ttov;             /**< toggle overflow register                  */
  volatile blt_int8u  tctl1;            /**< timer control register 1                  */
  volatile blt_int8u  tctl2;            /**< timer control register 2                  */
  volatile blt_int8u  tctl3;            /**< timer control register 3                  */
  volatile blt_int8u  tctl4;            /**< timer control register 4                  */
  volatile blt_int8u  tie;              /**< interrupt enable register                 */
  volatile blt_int8u  tscr2;            /**< system control register 2                 */
  volatile blt_int8u  tflg1;            /**< timer interrupt flag 1                    */
  volatile blt_int8u  tflg2;            /**< timer interrupt flag 2                    */
  volatile blt_int16u tc[8];            /**< input capture/output compare register n   */
} tTimerRegs;                           /**< timer related registers                   */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Base address for the timer related control registers. */
#define TIMER_REGS_BASE_ADDRESS  (0x0040)
/** \brief Macro for accessing the flash related control registers. */
#define TIMER                    ((volatile tTimerRegs *)TIMER_REGS_BASE_ADDRESS)


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Local variable for storing the number of milliseconds that have elapsed since
 *         startup.
 */
static blt_int32u millisecond_counter;

/** \brief Buffer for storing the last value of the free running counter. */
static blt_int16u free_running_counter_last;

/** \brief Stores the number of counts of the free running counter that equals one
 *         millisecond.
 */
static blt_int16u counts_per_millisecond;


/****************************************************************************************
* Register definitions
****************************************************************************************/
/** \brief TSCR1 - timer enable bit. */
#define TEN_BIT     (0x80)
/** \brief TSCR2 - timer prescaler select bit 0. */
#define PR0_BIT     (0x01)
/** \brief TSCR2 - timer prescaler select bit 1. */
#define PR1_BIT     (0x02)
/** \brief TSCR2 - timer prescaler select bit 2. */
#define PR2_BIT     (0x04)


/************************************************************************************//**
** \brief     Initializes the polling based millisecond timer driver.
** \return    none.
**
****************************************************************************************/
void TimerInit(void)
{
  /* make sure the timer system is in its reset state. */
  TimerReset();
  /* S12 derivatives support system speeds up to 32 MHz. Typical speeds are a multiple
   * of 4 MHz. Scale the timer's free running counter down a bit to get a better
   * time range between free running counter overflows. Use a fixed prescaler of 4.
   */
  TIMER->tscr2 &= ~(PR2_BIT | PR1_BIT | PR0_BIT);
  TIMER->tscr2 |= PR1_BIT;
  /* calculate how many counts equal 1 millisecond. */
  counts_per_millisecond = BOOT_CPU_SYSTEM_SPEED_KHZ/4;
  /* enable the timer subsystem to start the free running counter. */
  TIMER->tscr1 |= TEN_BIT;
  /* initialize remaining locals. */
  millisecond_counter = 0;
  free_running_counter_last = TIMER->tcnt;
} /*** end of TimerInit ***/


/************************************************************************************//**
** \brief     Reset the timer by placing the timer back into it's default reset
**            configuration.
** \return    none.
**
****************************************************************************************/
void TimerReset(void)
{
  /* bring the timer subsystem back into its reset state */
  TIMER->tie = 0;
  TIMER->tscr1 = 0;
  TIMER->tscr2 = 0;
  TIMER->tios = 0;
  TIMER->ttov = 0;
  TIMER->tctl1 = 0;
  TIMER->tctl2 = 0;
  TIMER->tctl3 = 0;
  TIMER->tctl4 = 0;
} /* end of TimerReset */


/************************************************************************************//**
** \brief     Updates the millisecond timer.
** \return    none.
**
****************************************************************************************/
void TimerUpdate(void)
{
  blt_int16u free_running_counter_now;
  blt_int16u delta_counts;
  blt_int16u ms_counts;

  /* get the current value of the free running counter. */
  free_running_counter_now = TIMER->tcnt;
  /* calculate the number of counts that passed since the detection of the last
   * millisecond event. Note that this calculation also works, in case the free running
   * counter overflowed, thanks to integer math.
   */
  delta_counts = free_running_counter_now - free_running_counter_last;

  /* did one or more milliseconds pass since the last event? */
  if (delta_counts >= counts_per_millisecond)
  {
    /* calculate how many milliseconds passed. */
    ms_counts = delta_counts / counts_per_millisecond;
    /* update the millisecond counter. */
    millisecond_counter += ms_counts;
    /* store the counter value of the last millisecond event, to detect the next one. */
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
  /* updating timer here allows this function to be called in a loop with timeout
   * detection.
   */
  TimerUpdate();
  /* read and return the amount of milliseconds that passed since initialization */
  return millisecond_counter;
} /*** end of TimerGet ***/


/*********************************** end of timer.c ************************************/
