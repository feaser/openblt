/************************************************************************************//**
* \file         Demo/ARMCM4_TM4C_DK_TM4C123G_IAR/Prog/header.h
* \brief        Generic header file.
* \ingroup      Prog_ARMCM4_TM4C_DK_TM4C123G_IAR
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2014  by Feaser    http://www.feaser.com    All rights reserved
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
#include "../Boot/blt_conf.h"                          /* bootloader configuration     */
#include "boot.h"                                      /* bootloader interface driver  */
#include "led.h"                                       /* LED driver                   */
#include "time.h"                                      /* Timer driver                 */
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/fpu.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"



#endif /* HEADER_H */
/*********************************** end of header.h ***********************************/
