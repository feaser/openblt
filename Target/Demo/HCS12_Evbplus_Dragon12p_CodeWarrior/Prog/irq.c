/************************************************************************************//**
* \file         Demo/HCS12_Evbplus_Dragon12p_CodeWarrior/Prog/irq.c
* \brief        IRQ driver source file.
* \ingroup      Prog_HCS12_Evbplus_Dragon12p_CodeWarrior
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2013  by Feaser    http://www.feaser.com    All rights reserved
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
* Local data definitions
****************************************************************************************/
/** \brief IRQ nesting counter . */
static volatile unsigned long irqNesting=0;
/** \brief Copy of CCR register. */
static volatile unsigned char irqCCRregSave;


/************************************************************************************//**
** \brief     Enables the generation IRQ interrupts. Typically called once during
**            software startup after completion of the initialization.
** \return    none.
**
****************************************************************************************/
void IrqInterruptEnable(void)
{
  /* enable the global interrupts */
  asm("cli");
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
	if (irqNesting == 0) 
	{
  	asm 
    {
      tpa
      sei
      staa  irqCCRregSave
    }
	}
	irqNesting++;
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
	irqNesting--;
	if (irqNesting == 0)
	{
	  asm
    {
      ldaa  irqCCRregSave
      tap
    }
	}
} /*** end of IrqInterruptRestore ***/


/*********************************** end of irq.c **************************************/
