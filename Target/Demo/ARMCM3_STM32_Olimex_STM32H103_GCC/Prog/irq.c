/************************************************************************************//**
* \file         Demo\ARMCM3_STM32_Olimex_STM32H103_GCC\Prog\irq.c
* \brief        IRQ driver source file.
* \ingroup      Prog_ARMCM3_STM32_Olimex_STM32H103_GCC
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

/****************************************************************************************
* Include files
****************************************************************************************/
#include "header.h"                                    /* generic header               */


/****************************************************************************************
* Local data definitions
****************************************************************************************/
/** \brief Interrupt nesting counter. Used for global interrupt en/disable. */
static unsigned char interruptNesting = 0;


/************************************************************************************//**
** \brief     Enables the generation IRQ interrupts. Typically called once during
**            software startup after completion of the initialization.
** \return    none.
**
****************************************************************************************/
void IrqInterruptEnable(void)
{
  __enable_irq();
} /*** end of IrqInterruptEnable ***/


/************************************************************************************//**
** \brief     Disables the generation IRQ interrupts and stores information on
**            whether or not the interrupts were already disabled before explicitly
**            disabling them with this function. Normally used as a pair together
**            with IrqInterruptRestore during a critical section.
** \return    none.
**
****************************************************************************************/
void IrqInterruptDisable(void)
{
  if (interruptNesting == 0)
  {
    __disable_irq();
  }
  interruptNesting++;
} /*** end of IrqInterruptDisable ***/


/************************************************************************************//**
** \brief     Restore the generation IRQ interrupts to the setting it had prior to
**            calling IrqInterruptDisable. Normally used as a pair together with
**            IrqInterruptDisable during a critical section.
** \return    none.
**
****************************************************************************************/
void IrqInterruptRestore(void)
{
  interruptNesting--;
  if (interruptNesting == 0)
  {
    __enable_irq();
  }
} /*** end of IrqInterruptRestore ***/


/*********************************** end of irq.c **************************************/
