/************************************************************************************//**
* \file         Source/ARMCM4_STM32F4/timer.c
* \brief        Bootloader timer driver source file.
* \ingroup      Target_ARMCM4_STM32F4
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
#include "stm32f4xx.h"                           /* STM32 CPU and HAL header           */


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

  /* Configure the systick frequency as a 1 ms event generator. */
  SysTick->LOAD = BOOT_CPU_SYSTEM_SPEED_KHZ - 1;
  /* Reset the current counter value. */
  SysTick->VAL = 0;
  /* Select core clock as source and enable the timer. */
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
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
  /* Set the systick's registers back into the default reset value. */
  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;
} /* end of TimerReset */


/************************************************************************************//**
** \brief     Updates the millisecond timer.
** \return    none.
**
****************************************************************************************/
void TimerUpdate(void)
{
  /* Check if the millisecond event occurred. */
  if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0)
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


/************************************************************************************//**
** \brief     Override for the HAL driver's GetTick() functionality. This is needed
**            because the bootloader doesn't use interrupts, but the HAL's tick
**            functionality assumes that it does. This will cause the HAL_Delay()
**            function to not work properly. As a result of this override, the HAL's
**            tick functionality works in polling mode.
** \return    Current value of the millisecond timer.
**
****************************************************************************************/
uint32_t HAL_GetTick(void)
{
  /* Link to the bootloader's 1ms timer. */
  return TimerGet();
} /*** end of HAL_GetTick ***/


/************************************************************************************//**
** \brief     This function handles the SysTick interrupt. The HAL driver is initialized
**            before this timer driver. The HAL driver configures the SysTick for
**            interrupt driven mode, which is afterwards disabled by the timer driver
**            initialization. It is theoretically possible that the SysTick interrupt
**            still fires before the timer driver disables it. Therefore the handler
**            is implemented here. If not, then the default handler from cstart.s is
**            used, which hangs the system.
** \return    none.
**
****************************************************************************************/
__weak void SysTick_Handler(void)
{
  /* Nothing to do here. */
} /*** end of SysTick_Handler ***/


/*********************************** end of timer.c ************************************/
