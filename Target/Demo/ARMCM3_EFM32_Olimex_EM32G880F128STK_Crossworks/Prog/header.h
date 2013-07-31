/************************************************************************************//**
* \file         Demo\ARMCM3_EFM32_Olimex_EM32G880F128STK_Crossworks\Prog\header.h
* \brief        Generic header file.
* \ingroup      Prog_ARMCM3_EFM32_Olimex_EM32G880F128STK_Crossworks
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2012  by Feaser    http://www.feaser.com    All rights reserved
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
#ifndef HEADER_H
#define HEADER_H

/****************************************************************************************
* Include files
****************************************************************************************/
#include "../Boot/config.h"                            /* bootloader configuration     */
#include "efm32.h"                                     /* EFM32 registers              */
#include "efm32_chip.h"                                /* EFM32 chip initialization    */
#include "efm32_cmu.h"                                 /* EFM32 clock management       */
#include "efm32_gpio.h"                                /* EFM32 GPIO management        */
#include "efm32_leuart.h"                              /* EFM32 LEUART management      */
#include "boot.h"                                      /* bootloader interface driver  */
#include "irq.h"                                       /* IRQ driver                   */
#include "led.h"                                       /* LED driver                   */
#include "timer.h"                                     /* Timer driver                 */


#endif /* HEADER_H */
/*********************************** end of header.h ***********************************/
