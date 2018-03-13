/************************************************************************************//**
* \file         Demo\ARM7_LPC2000_Olimex_LPC_L2294_GCC\Boot\vectors.c
* \brief        Bootloader interrupt vectors source file.
* \ingroup      Boot_ARM7_LPC2000_Olimex_LPC_L2294_GCC
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
#include "boot.h"                                /* bootloader generic header          */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void __attribute__((interrupt("FIQ")))   FIQ_ISR(void);
void __attribute__((interrupt("IRQ")))   IRQ_ISR(void);
void __attribute__((interrupt("UNDEF"))) UNDEF_ISR(void);


/************************************************************************************//**
** \brief     FIQ exception routine.
** \return    none.
**
****************************************************************************************/
void FIQ_ISR(void)
{
  /* unexpected interrupt so trigger assertion */
  ASSERT_RT(BLT_FALSE);
} /*** end of FIQ_ISR ***/


/************************************************************************************//**
** \brief     IRQ exception routine.
** \return    none.
**
****************************************************************************************/
void IRQ_ISR(void)
{
  /* unexpected interrupt so trigger assertion */
  ASSERT_RT(BLT_FALSE);
} /*** end of IRQ_ISR ***/


/************************************************************************************//**
** \brief     UNDEF exception routine.
** \return    none.
**
****************************************************************************************/
void UNDEF_ISR(void)
{
  /* unexpected interrupt so trigger assertion */
  ASSERT_RT(BLT_FALSE);
} /*** end of UNDEF_ISR ***/


/*********************************** end of vectors.c **********************************/
