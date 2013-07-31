/************************************************************************************//**
* \file         Source\ARMCM3_STM32\timer.c
* \brief        Bootloader timer driver source file.
* \ingroup      Target_ARMCM3_STM32
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
* You should have received a copy of the GNU General Public License along with OpenBLT.
* If not, see <http://www.gnu.org/licenses/>.
*
* A special exception to the GPL is included to allow you to distribute a combined work 
* that includes OpenBLT without being obliged to provide the source code for any 
* proprietary components. The exception text is included at the bottom of the license
* file <license.html>.
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
/** \brief Systick registers. */
typedef struct
{
  volatile blt_int32u CTRL;                   /**< SysTick Control and Status Register */
  volatile blt_int32u LOAD;                   /**< SysTick Reload Value Register       */
  volatile blt_int32u VAL;                    /**< SysTick Current Value Register      */
} tSysTickRegs;


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief CLKSOURCE bit of the system tick. */
#define SYSTICK_BIT_CLKSOURCE    ((blt_int32u)0x00000004)
/** \brief ENABLE bit of the system tick. */
#define SYSTICK_BIT_ENABLE       ((blt_int32u)0x00000001)
/** \brief COUNTERFLAG bit of the system tick. */
#define SYSTICK_BIT_COUNTERFLAG  ((blt_int32u)0x00010000)


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
/** \brief Macro to access the system tick registers. */
#define SYSTICK          ((tSysTickRegs *) (blt_int32u)0xE000E010)


/************************************************************************************//**
** \brief     Initializes the polling based millisecond timer driver.
** \return    none.
**
****************************************************************************************/
void TimerInit(void)
{
  /* reset the timer configuration */
  TimerReset();
  /* configure the systick frequency as a 1 ms event generator */
  SYSTICK->LOAD = BOOT_CPU_SYSTEM_SPEED_KHZ - 1;
  /* reset the current counter value */
  SYSTICK->VAL = 0;
  /* select core clock as source and enable the timer */
  SYSTICK->CTRL = SYSTICK_BIT_CLKSOURCE | SYSTICK_BIT_ENABLE;
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
  /* set the systick's status and control register back into the default reset value */
  SYSTICK->CTRL = 0;
} /* end of TimerReset */


/************************************************************************************//**
** \brief     Updates the millisecond timer.
** \return    none.
**
****************************************************************************************/
void TimerUpdate(void)
{
  /* check if the millisecond event occurred */
  if ((SYSTICK->CTRL & SYSTICK_BIT_COUNTERFLAG) != 0)
  {
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
