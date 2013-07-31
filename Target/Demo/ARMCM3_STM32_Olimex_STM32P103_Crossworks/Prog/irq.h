/************************************************************************************//**
* \file         Demo\ARMCM3_STM32_Olimex_STM32P103_Crossworks\Prog\irq.h
* \brief        IRQ driver header file.
* \ingroup      Prog_ARMCM3_STM32_Olimex_STM32P103_Crossworks
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
#ifndef IRQ_H
#define IRQ_H

/****************************************************************************************
* Function prototypes
****************************************************************************************/
void IrqInterruptEnable(void);
void IrqInterruptDisable(void);
void IrqInterruptRestore(void);


#endif /* IRQ_H */
/*********************************** end of irq.h **************************************/
