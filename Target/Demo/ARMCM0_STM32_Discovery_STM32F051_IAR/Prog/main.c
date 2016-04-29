/************************************************************************************//**
* \file         Demo\ARMCM0_STM32_Discovery_STM32F051_IAR\Prog\main.c
* \brief        Demo program application source file.
* \ingroup      Prog_ARMCM0_STM32_Discovery_STM32F051_IAR
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2016  by Feaser    http://www.feaser.com    All rights reserved
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
#include "header.h"                                    /* generic header               */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);


/************************************************************************************//**
** \brief     This is the entry point for the bootloader application and is called
**            by the reset interrupt vector after the C-startup routines executed.
** \return    none.
**
****************************************************************************************/
void main(void)
{
  /* initialize the microcontroller */
  Init();
  /* initialize the bootloader interface */
  BootComInit();

  /* start the infinite program loop */
  while (1)
  {
    /* toggle LED with a fixed frequency */
    LedToggle();
    /* check for bootloader activation request */
    BootComCheckActivationRequest();
  }
} /*** end of main ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller.
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  /* init the led driver */
  LedInit();
  /* init the timer driver */
  TimerInit();
} /*** end of Init ***/


/*********************************** end of main.c *************************************/
