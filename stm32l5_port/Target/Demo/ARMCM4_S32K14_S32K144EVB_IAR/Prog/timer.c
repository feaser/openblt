/************************************************************************************//**
* \file         Demo/ARMCM4_S32K14_S32K144EVB_IAR/Prog/timer.c
* \brief        Timer driver source file.
* \ingroup      Prog_ARMCM4_S32K14_S32K144EVB_IAR
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2020  by Feaser    http://www.feaser.com    All rights reserved
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


/************************************************************************************//**
** \brief     Initializes the timer.
** \return    none.
**
****************************************************************************************/
void TimerInit(void)
{
  /* Configure the systick frequency as a 1 ms event generator. */
  S32_SysTick->RVR = (SystemCoreClock / 1000U) - 1U;
  /* Reset the current counter value. */
  S32_SysTick->CVR = 0U;
  /* Select core clock as source and enable the timer. */
  S32_SysTick->CSR = S32_SysTick_CSR_ENABLE_MASK |
                     S32_SysTick_CSR_TICKINT_MASK |
                     S32_SysTick_CSR_CLKSOURCE_MASK;
  /* Reset the millisecond counter value. */
  millisecond_counter = 0U;
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
void SysTick_Handler(void)
{
  /* Increment the millisecond counter. */
  millisecond_counter++;
} /*** end of SysTick_Handler ***/


/*********************************** end of timer.c ************************************/
