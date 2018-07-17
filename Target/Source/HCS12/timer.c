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
/** \brief Number of free running counter ticks in one millisecond. */
#define TIMER_COUNTS_PER_MS      (BOOT_CPU_SYSTEM_SPEED_KHZ)


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Local variable for storing the number of milliseconds that have elapsed since
 *         startup.
 */
static blt_int32u millisecond_counter;


/****************************************************************************************
* Register definitions
****************************************************************************************/
/** \brief TSCR1 - timer enable bit. */
#define TEN_BIT     (0x80)
/** \brief TIOS  - channel 0 ic/oc configuration bit. */
#define IOS0_BIT    (0x01)
/** \brief TFLG1 - channel 0 ic/oc event flag bit. */
#define C0F_BIT     (0x01)


/************************************************************************************//**
** \brief     Initializes the polling based millisecond timer driver.
** \return    none.
**
****************************************************************************************/
void TimerInit(void)
{
  /* reset the timer configuration. note that this also sets the default prescaler
   * to 1, so the free running counter runs at the same speed as the system
   * (BOOT_CPU_SYSTEM_SPEED_KHZ).
   */
  TimerReset();
  /* configure timer channel 0 as a 1 millisecond software timer */
  TIMER->tios |= IOS0_BIT;
  /* make sure timer 0 interrupt flag is cleared */
  TIMER->tflg1 = C0F_BIT;
  /* generate output compare event in 1 milliseconds from now */
  TIMER->tc[0] = TIMER->tcnt + TIMER_COUNTS_PER_MS;
  /* enable the timer subsystem */
  TIMER->tscr1 |= TEN_BIT;
  /* reset the millisecond counter value */
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
  /* check if the millisecond event occurred */
  if ((TIMER->tflg1 & C0F_BIT) == C0F_BIT)
  {
    /* make sure timer 0 interrupt flag is cleared */
    TIMER->tflg1 = C0F_BIT;
    /* generate output compare event in 1 milliseconds from now */
    TIMER->tc[0] += TIMER_COUNTS_PER_MS;
    /* increment the millisecond counter */
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
  /* updating timer here allows this function to be called in a loop with timeout
   * detection.
   */
  TimerUpdate();
  /* read and return the amount of milliseconds that passed since initialization */
  return millisecond_counter;
} /*** end of TimerGet ***/


/*********************************** end of timer.c ************************************/
