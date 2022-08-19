/************************************************************************************//**
* \file         Demo/ARMCM7_STM32H7_Nucleo_H743ZI_CubeIDE/Prog/App/timer.c
* \brief        Timer driver source file.
* \ingroup      Prog_ARMCM7_STM32H7_Nucleo_H743ZI_CubeIDE
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


/************************************************************************************//**
** \brief     Initializes the timer.
** \return    none.
**
****************************************************************************************/
void TimerInit(void)
{
  /* The HAL initialization already configured the Systick interrupt to generate an
   * interrupt every 1 millisecond. Nothing more needs to be done here.
   */
} /*** end of TimerInit ***/


/************************************************************************************//**
** \brief     Obtains the counter value of the millisecond timer.
** \return    Current value of the millisecond timer.
**
****************************************************************************************/
unsigned long TimerGet(void)
{
  /* Read and return the tick counter value. */
  return HAL_GetTick();
} /*** end of TimerGet ***/


/*********************************** end of timer.c ************************************/
