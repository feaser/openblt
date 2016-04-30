/************************************************************************************//**
* \file         Demo\ARM7_LPC2000_Olimex_LPC_L2294_GCC\Prog\timer.c
* \brief        Timer driver source file.
* \ingroup      Prog_ARM7_LPC2000_Olimex_LPC_L2294_GCC
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
#include "header.h"                                    /* generic header               */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Local variable for storing the number of milliseconds that have elapsed since
 *         startup.
 */
static unsigned long millisecond_counter;


/****************************************************************************************
* External functions
****************************************************************************************/
extern void TIMER0_ISR(void);


/************************************************************************************//**
** \brief     Initializes the timer.
** \return    none.
**
****************************************************************************************/
void TimerInit(void)
{
  /* configure timer0 as 1 ms software output compare */
  T0MR0 = BOOT_CPU_SYSTEM_SPEED_KHZ-1;
  /* enable interrupt and automatic reset upon compare */
  T0MCR = 0x01 | 0x02;
  /* enable the output compare */
  T0TCR = 0x01;
  /* set the interrupt service routine for the output compare event */
  VICVectAddr0 = (unsigned long)TIMER0_ISR;
  /* connect vectored IRQ slot 0 to Timer0's channel 4 */
  VICVectCntl0 = 0x20 | 4;
  /* enable the timer0 interrupt */
  VICIntEnable = 0x10;
  /* reset the millisecond counter */
  TimerSet(0);
} /*** end of TimerInit ***/


/************************************************************************************//**
** \brief     Updates the millisecond timer. Should be called every millisecond by
**            the timer interrupt service routine.
** \return    none.
**
****************************************************************************************/
void TimerUpdate(void)
{
  /* increment the millisecond counter */
  millisecond_counter++;
} /*** end of TimerUpdate ***/


/************************************************************************************//**
** \brief     Sets the initial counter value of the millisecond timer.
** \param     timer_value initialize value of the millisecond timer.
** \return    none.
**
****************************************************************************************/
void TimerSet(unsigned long timer_value)
{
  /* set the millisecond counter */
  millisecond_counter = timer_value;
} /*** end of TimerSet ***/


/************************************************************************************//**
** \brief     Obtains the counter value of the millisecond timer.
** \return    Current value of the millisecond timer.
**
****************************************************************************************/
unsigned long TimerGet(void)
{
  /* read and return the millisecond counter value */
  return millisecond_counter;
} /*** end of TimerGet ***/


/*********************************** end of timer.c ************************************/
