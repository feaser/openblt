/************************************************************************************//**
* \file         Demo/ARMCM0_XMC1_XMC1400_Boot_Kit_Keil/Prog/header.h
* \brief        Generic header file.
* \ingroup      Prog_ARMCM0_XMC1_XMC1400_Boot_Kit_Keil
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
#ifndef HEADER_H
#define HEADER_H

/****************************************************************************************
* Include files
****************************************************************************************/
#include "../Boot/blt_conf.h"                    /* bootloader configuration           */
#include "xmc_scu.h"                             /* SCU driver header                  */
#include "xmc_gpio.h"                            /* GPIO driver header                 */
#include "xmc_uart.h"                            /* UART driver header                 */
#include "xmc_can.h"                             /* CAN driver header                  */
#include "boot.h"                                /* bootloader interface driver        */
#include "led.h"                                 /* LED driver                         */
#include "timer.h"                               /* Timer driver                       */


#endif /* HEADER_H */
/*********************************** end of header.h ***********************************/
