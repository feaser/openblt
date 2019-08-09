/************************************************************************************//**
* \file         Demo/HCS12_DevKit_S12G128_CodeWarrior/Prog/header.h
* \brief        Generic header file.
* \ingroup      Prog_HCS12_DevKit_S12G128_CodeWarrior
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2018  by Feaser    http://www.feaser.com    All rights reserved
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
* Defines
****************************************************************************************/
/** \brief   Configuration switch to enable programming and debugging with a BDM 
 *           interface.
 *  \details To configure the program for downloading with the OpenBLT bootloader, set 
 *           this value to 0. This is typically done for release versions. If support
 *           for programming and debugging with a BDM debugger interface is desired 
 *           during development, then set this value to 1.
 */
#define BDM_DEBUGGING_ENABLED  (0)


/****************************************************************************************
* Include files
****************************************************************************************/
#include <hidef.h>                                     /* common defines and macros    */
#include "../Boot/blt_conf.h"                          /* bootloader configuration     */
#include "boot.h"                                      /* bootloader interface driver  */
#include "led.h"                                       /* LED driver                   */
#include "timer.h"                                     /* Timer driver                 */
#include "derivative.h"                                /* MCU registers                */


#endif /* HEADER_H */
/*********************************** end of header.h ***********************************/
