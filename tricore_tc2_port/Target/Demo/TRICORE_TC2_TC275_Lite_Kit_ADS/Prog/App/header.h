/************************************************************************************//**
* \file         Demo/TRICORE_TC2_TC275_Lite_Kit_ADS/Prog/App/header.h
* \brief        Generic header file.
* \ingroup      Prog_TRICORE_TC2_TC275_Lite_Kit_ADS
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
#ifndef HEADER_H
#define HEADER_H

/****************************************************************************************
* Include files
****************************************************************************************/
#include "../../Boot/App/blt_conf.h"                   /* bootloader configuration     */
#include "app.h"                                       /* application header           */
#include "boot.h"                                      /* bootloader interface driver  */
#include "led.h"                                       /* LED driver                   */
#include "timer.h"                                     /* Timer driver                 */
#include "IfxCpu.h"                                    /* CPU driver                   */
#include "IfxPort.h"                                   /* GPIO driver                  */
#include "IfxAsclin.h"                                 /* ASCLIN basic driver          */
#include "IfxMultican_Can.h"                           /* MultiCAN detailed driver     */
#include "IfxStm.h"                                    /* System timer driver          */


#endif /* HEADER_H */
/*********************************** end of header.h ***********************************/
