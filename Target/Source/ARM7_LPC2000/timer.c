/************************************************************************************//**
* \file         Source\ARM7_LPC2000\timer.c
* \brief        Bootloader timer driver source file.
* \ingroup      Target_ARM7_LPC2000
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
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
* Local data declarations
****************************************************************************************/
/** \brief Local variable that holds the last value of the free running counter. */
static blt_int32u free_running_counter_last;
/** \brief Local variable for storing the number of milliseconds that have elapsed since
 *         startup.
 */
static blt_int32u millisecond_counter;


/****************************************************************************************
* Register definitions
****************************************************************************************/
/** \brief T0MCR timer register. */
#define T0MCR           (*((volatile blt_int32u *) 0xE0004014))
/** \brief T0CCR timer register. */
#define T0CCR           (*((volatile blt_int32u *) 0xE0004028))
/** \brief T0PR timer register. */
#define T0PR            (*((volatile blt_int32u *) 0xE000400C))
/** \brief T0PC timer register. */
#define T0PC            (*((volatile blt_int32u *) 0xE0004010))
/** \brief T0TCR timer register. */
#define T0TCR           (*((volatile blt_int32u *) 0xE0004004))
/** \brief T0TC timer register. */
#define T0TC            (*((volatile blt_int32u *) 0xE0004008))


/************************************************************************************//**
** \brief     Initializes the polling based millisecond timer driver.
** \return    none.
**
****************************************************************************************/
void TimerInit(void)
{
  /* disable timer 0 interrupts */
  T0MCR = 0x00;
  T0CCR = 0x00;
  /* set prescaler so that free running counter runs at 1 kHz */
  T0PR = BOOT_CPU_SYSTEM_SPEED_KHZ - 1;
  /* start free running counter for timer 0 */
  T0TCR = 0x01;
  /* store the start value of the free running counter */
  free_running_counter_last = T0TC;
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
  /* reset the timer */
  T0TCR = 0;
  /* reset the prescaler */
  T0PR = 0;
  /* reset the timer counter */
  T0TC = 0;
  /* reset the prescaled counter */
  T0PC = 0;
} /* TimerReset */


/************************************************************************************//**
** \brief     Updates the millisecond timer.
** \return    none.
**
****************************************************************************************/
void TimerUpdate(void)
{
  blt_int32u free_running_counter_now;

  /* update the millisecond counter */
  free_running_counter_now = T0TC;
  millisecond_counter += (free_running_counter_now - free_running_counter_last);
  /* store current free running counter value for next time around */
  free_running_counter_last = free_running_counter_now;
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
