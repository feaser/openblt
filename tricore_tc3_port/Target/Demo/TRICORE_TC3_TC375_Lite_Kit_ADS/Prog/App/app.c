/************************************************************************************//**
* \file         Demo/TRICORE_TC3_TC375_Lite_Kit_ADS/Prog/App/app.c
* \brief        User program application source file.
* \ingroup      Prog_TRICORE_TC3_TC375_Lite_Kit_ADS
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2022  by Feaser    http://www.feaser.com    All rights reserved
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
* Global constant declarations
****************************************************************************************/
#pragma section farrom "cssig"
/** \brief   Signature checksum placeholder. Should be linked to the address as
 *           configured with macro BOOT_FLASH_VECTOR_TABLE_CS_OFFSET.
 */
const unsigned long bltChecksumSignature = 0x55AA11EE;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);


/************************************************************************************//**
** \brief     Initializes the user program application. Should be called once during
**            software program initialization.
** \return    none.
**
****************************************************************************************/
void AppInit(void)
{
  /* Initialize the microcontroller. */
  Init();
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


/************************************************************************************//**
** \brief     Initializes the microcontroller.
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  /* Configure the LED GPIO pin P00.5. */
  IfxPort_setPinModeOutput(&MODULE_P00, 5U, IfxPort_OutputMode_pushPull,
                                            IfxPort_OutputIdx_general);
} /*** end of Init ***/


/*********************************** end of app.c **************************************/
