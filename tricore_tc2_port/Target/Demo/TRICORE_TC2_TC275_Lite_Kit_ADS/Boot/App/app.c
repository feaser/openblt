/************************************************************************************//**
* \file         Demo/TRICORE_TC2_TC275_Lite_Kit_ADS/Boot/App/app.c
* \brief        Bootloader application source file.
* \ingroup      Boot_TRICORE_TC2_TC275_Lite_Kit_ADS
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2023  by Feaser    http://www.feaser.com    All rights reserved
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
#include "IfxPort.h"                             /* GPIO driver                        */
#include "IfxAsclin.h"                           /* ASCLIN basic driver                */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);


/************************************************************************************//**
** \brief     Initializes the bootloader application. Should be called once during
**            software program initialization.
** \return    none.
**
****************************************************************************************/
void AppInit(void)
{
  /* Initialize the microcontroller. */
  Init();
  /* Initialize the bootloader */
  BootInit();
} /*** end of AppInit ***/


/************************************************************************************//**
** \brief     Task function of the bootloader application. Should be called continuously
**            in the program loop.
** \return    none.
**
****************************************************************************************/
void AppTask(void)
{
  /* Run the bootloader task. */
  BootTask();
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
  /* Configure the pushbutton GPIO pin P00.7. */
  IfxPort_setPinMode(&MODULE_P00, 7U, IfxPort_Mode_inputPullUp);
#if (BOOT_COM_RS232_ENABLE > 0)
  /* Enable the ASCLIN0 module. */
  IfxAsclin_enableModule(&MODULE_ASCLIN0);
  /* Disable the clock before configuring the GPIO pins. */
  IfxAsclin_setClockSource(&MODULE_ASCLIN0, IfxAsclin_ClockSource_noClock);
  /* Configure the ASCLIN0 GPIO pins P14.1 Rx and P14.0 Tx. */
  IfxAsclin_initRxPin(&IfxAsclin0_RXA_P14_1_IN, IfxPort_InputMode_pullUp,
                      IfxPort_PadDriver_cmosAutomotiveSpeed1);
  IfxAsclin_initTxPin(&IfxAsclin0_TX_P14_0_OUT, IfxPort_OutputMode_pushPull,
                      IfxPort_PadDriver_cmosAutomotiveSpeed1);
#endif
} /*** end of Init ***/


/*********************************** end of app.c **************************************/
