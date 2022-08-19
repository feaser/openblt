/************************************************************************************//**
* \file         Demo/ARMCM7_STM32H7_Nucleo_H743ZI_CubeIDE/Prog/App/app.c
* \brief        User program application source file.
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
** \brief     Initializes the user program application. Should be called once during
**            software program initialization.
** \return    none.
**
****************************************************************************************/
void AppInit(void)
{
  /* Initialize the timer driver. */
  TimerInit();
  /* Initialize the led driver. */
  LedInit();
  /* initialize the bootloader interface */
  BootComInit();
} /*** end of AppInit ***/


/************************************************************************************//**
** \brief     Task function of the user program application. Should be called
**            continuously in the program loop.
** \return    none.
**
****************************************************************************************/
void AppTask(void)
{
  /* Toggle LED with a fixed frequency. */
  LedToggle();
  /* check for bootloader activation request */
  BootComCheckActivationRequest();
} /*** end of AppTask ***/


/*********************************** end of app.c **************************************/
