/************************************************************************************//**
* \file         Demo\ARMCM3_STM32_Olimex_STM32P103_IAR\Prog\header.h
* \brief        Generic header file.
* \ingroup      Prog_ARMCM3_STM32_Olimex_STM32P103_IAR
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
#include "stm32f10x.h"                                 /* STM32 register definitions   */
#include "stm32f10x_conf.h"                            /* STM32 peripheral drivers     */
#include "boot.h"                                      /* bootloader interface driver  */
#include "irq.h"                                       /* IRQ driver                   */
#include "led.h"                                       /* LED driver                   */
#include "timer.h"                                     /* Timer driver                 */


#endif /* HEADER_H */
/*********************************** end of header.h ***********************************/
