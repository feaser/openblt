/************************************************************************************//**
* \file         Source\ARM7_LPC2000\GCC\cpu_comp.c
* \brief        Bootloader cpu module source file.
* \ingroup      Target_ARM7_LPC2000
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2016  by Feaser    http://www.feaser.com    All rights reserved
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
#include "boot.h"                                /* bootloader generic header          */


/************************************************************************************//**
** \brief     Obtains current value of CPSR CPU register. Derived from a sample by R O 
**            Software that is Copyright 2004, R O SoftWare, and can be used for hobby 
**            or commercial purposes.
** \return    CPSR value.
**
****************************************************************************************/
static blt_int32u IrqGetCPSR(void)
{
  blt_int32u retval;
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
static void IrqSetCPSR(blt_int32u val)
{
  asm volatile (" msr  cpsr, %0" : /* no outputs */ : "r" (val)  );
} /*** end of IrqSetCPSR ***/


/************************************************************************************//**
** \brief     Disable global interrupts.
** \return    none.
**
****************************************************************************************/
void CpuIrqDisable(void)
{
  blt_int32u _cpsr;

  _cpsr = IrqGetCPSR();
  IrqSetCPSR(_cpsr | 0x00000080);
} /*** end of CpuIrqDisable ***/


/************************************************************************************//**
** \brief     Enable global interrupts.
** \return    none.
**
****************************************************************************************/
void CpuIrqEnable(void)
{
  blt_int32u _cpsr;

  _cpsr = IrqGetCPSR();
  IrqSetCPSR(_cpsr & ~0x00000080);
} /*** end of CpuIrqEnable ***/

/*********************************** end of cpu_comp.c *********************************/
