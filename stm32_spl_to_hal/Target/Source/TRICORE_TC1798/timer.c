/************************************************************************************//**
* \file         Source\TRICORE_TC1798\timer.c
* \brief        Bootloader timer driver source file.
* \ingroup      Target_TRICORE_TC1798
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2015  by Feaser    http://www.feaser.com    All rights reserved
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
#include "cpu_comp.h"                            /* compiler specific CPU definitions  */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Local variable for storing the number of milliseconds that have elapsed since
 *         startup.
 */
static blt_int32u millisecond_counter;

/** \brief Holds the timer tick count for 1 millisecond. */
static blt_int16u millisecond_ticks;


/************************************************************************************//**
** \brief     Initializes the polling based millisecond timer driver.
** \return    none.
**
****************************************************************************************/
void TimerInit(void)
{
  blt_int32u dummy;

  /* reset the timer configuration */
  TimerReset();
  /* obtain "E" access rights */
  CpuEnterInitMode();
  /* enable the GPT12 timer module */
  GPT120_CLC.reg  = 0x00000000;
  /* dummy read to avoid pipeline effects */
  dummy = GPT120_CLC.reg;
  /* release "E" access rights */
  CpuLeaveInitMode();
  /* core timer 3 is used for polling millisecond events. its configuration is:
   *   - timer 3 works in timer mode
   *   - external up/down control is disabled
   *   - prescaler factor is 128 (Ftimer3 = BOOT_CPU_SYSTEM_SPEED_KHZ / 128)
   *   - up/down control bit is reset
   *   - alternate output function T3OUT is disabled
   *   - timer 3 output toggle latch (T3OTL) is set to 0
   *   - timer 3 run bit is set
   */
  GPT120_T3CON.reg  = 0x00000845;
  /* reset the timer 3 register so that counter starts at 0 */
  GPT120_T3.reg = 0x00000000;
  /* calculate the number of timer ticks in 1 millisecond */
  millisecond_ticks = ((blt_int32u)BOOT_CPU_SYSTEM_SPEED_KHZ / 128);
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
  blt_int32u dummy;
  /* revert back to timer 3 configuration reset value */
  GPT120_T3CON.reg  = 0x00000000;
  /* revert back to timer 3 reset value */
  GPT120_T3.reg = 0x00000000;
  /* obtain "E" access rights */
  CpuEnterInitMode();
  /* disable the GPT12 timer module */
  GPT120_CLC.reg  = 0x00000003;
  /* dummy read to avoid pipeline effects */
  dummy = GPT120_CLC.reg;
  /* release "E" access rights */
  CpuLeaveInitMode();
} /* end of TimerReset */


/************************************************************************************//**
** \brief     Updates the millisecond timer.
** \return    none.
**
****************************************************************************************/
void TimerUpdate(void)
{
  /* check if the millisecond event occurred */
  if (GPT120_T3.reg >= millisecond_ticks)
  {
    GPT120_T3.reg = 0;
    /* reset timer 3 register for detecting the next millisecond */
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
