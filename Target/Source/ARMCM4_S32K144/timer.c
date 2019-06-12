/************************************************************************************//**
* \file         Source/_template/timer.c
* \brief        Bootloader timer driver source file.
* \ingroup      Target__template_timer
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2019  by Feaser    http://www.feaser.com    All rights reserved
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

/************************************************************************************//**
* \defgroup   Target__template_timer Timer driver of a port
* \brief      This module implements the timer memory driver of a microcontroller port. 
* \details    The timer driver implements a polling based 1 millisecond timer. It 
*             provides the time base for all timing related parts of the bootloader. The
*             bootloader calls the function TimerUpdate() continuously to check if the
*             next millisecond period passed.
* \ingroup    Target__template
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */


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

  /* TODO ##Port Configure a timer peripheral such that 1 millisecond events can be
   * detected. Note that the bootloader does not use interrupts, so this driver should
   * also not generate timer related interrupts. 
   */

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
  /* TODO ##Port Set the timer peripheral back into the default reset value. */
} /* end of TimerReset */


/************************************************************************************//**
** \brief     Updates the millisecond timer.
** \return    none.
**
****************************************************************************************/
void TimerUpdate(void)
{
  /* TODO ##Port Check with the timer peripheral if the 1 millisecond event occured. This
   * is typically done by looking at a flag bit this is set by the timer peripheral. An
   * alternative solution would use the timer peripheral's free running counter. Just
   * keep in mind that with the latter case, you would have to store the free running
   * counter value of the last time the millisecond event occured. This you can compare
   * it with the current value of the free running counter to determine if a millisecond
   * passed.
   */
  if (1 == 0)
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
