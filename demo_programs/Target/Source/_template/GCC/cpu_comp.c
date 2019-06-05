/************************************************************************************//**
* \file         Source/_template/GCC/cpu_comp.c
* \brief        Bootloader cpu module source file.
* \ingroup      Target__template_cpu_comp
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2019  by Feaser    http://www.feaser.com    All rights reserved
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

/************************************************************************************//**
* \defgroup   Target__template_cpu_comp Compiler specifics of a port
* \brief      This module implements the compiler specific parts of a microcontroller
*             port.
* \ingroup    Target__template
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */


/************************************************************************************//**
** \brief     Disable global interrupts.
** \return    none.
**
****************************************************************************************/
void CpuIrqDisable(void)
{
  /* TODO ##Port Disable the global interrupts. For safety and stability reasons, the
   * bootloader does not use interrupts. This function is called to disable the
   * generation of interrupt. It is called upon bootloader initialization by CpuInit().
   */
} /*** end of CpuIrqDisable ***/


/************************************************************************************//**
** \brief     Enable global interrupts.
** \return    none.
**
****************************************************************************************/
void CpuIrqEnable(void)
{
  /* TODO ##Port Enable the global interrupts. Not all, but some microcontrollers have
   * global interrupts enabled right after reset. In this case the bootloader should
   * enable the global interrupts again, right before the user program is started in
   * function CpuStartUserProgram(). 
   */
} /*** end of CpuIrqEnable ***/


/*********************************** end of cpu_comp.c *********************************/
