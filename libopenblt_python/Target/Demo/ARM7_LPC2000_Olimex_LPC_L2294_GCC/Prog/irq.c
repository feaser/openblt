/************************************************************************************//**
* \file         Demo\ARM7_LPC2000_Olimex_LPC_L2294_GCC\Prog\irq.c
* \brief        IRQ driver source file.
* \ingroup      Prog_ARM7_LPC2000_Olimex_LPC_L2294_GCC
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
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
/** \brief Interrupt status before disabling. Used for global interrupt en/disable. */
static unsigned long oldInterruptStatus;
/** \brief Interrupt nesting counter. Used for global interrupt en/disable. */
static unsigned char interruptNesting = 0;


/************************************************************************************//**
** \brief     Obtains current value of CPSR CPU register. Derived from a sample by R O 
**            Software that is Copyright 2004, R O SoftWare, and can be used for hobby 
**            or commercial purposes.
** \return    CPSR value.
**
****************************************************************************************/
static unsigned long IrqGetCPSR(void)
{
  unsigned long retval;
  asm volatile (" mrs  %0, cpsr" : "=r" (retval) : /* no inputs */  );
  return retval;
} /*** end of IrqGetCPSR ***/


/************************************************************************************//**
** \brief     Update value of CPSR CPU register. Derived from a sample by R O 
**            Software that is Copyright 2004, R O SoftWare, and can be used for hobby 
**            or commercial purposes.
** \param     val CPSR value.
** \return    none.
**
****************************************************************************************/
static void IrqSetCPSR(unsigned long val)
{
  asm volatile (" msr  cpsr, %0" : /* no outputs */ : "r" (val)  );
} /*** end of IrqSetCPSR ***/


/************************************************************************************//**
** \brief     Enables the generation IRQ interrupts. Typically called once during
**            software startup after completion of the initialization.
** \return    none.
**
****************************************************************************************/
void IrqInterruptEnable(void)
{
  unsigned _cpsr;

  _cpsr = IrqGetCPSR();
  IrqSetCPSR(_cpsr & ~0x00000080);
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
  unsigned long _cpsr;

  if (interruptNesting == 0)
  {
    _cpsr = IrqGetCPSR();
    IrqSetCPSR(_cpsr | 0x00000080);
    oldInterruptStatus = _cpsr;
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
  unsigned _cpsr;

  interruptNesting--;
  if (interruptNesting == 0)
  {
    _cpsr = IrqGetCPSR();
    IrqSetCPSR((_cpsr & ~0x00000080) | (oldInterruptStatus & 0x00000080));
  }
} /*** end of IrqInterruptRestore ***/


/*********************************** end of irq.c **************************************/
