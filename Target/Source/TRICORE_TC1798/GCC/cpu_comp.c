/************************************************************************************//**
* \file         Source\TRICORE_TC1798\GCC\cpu_comp.c
* \brief        Bootloader compiler specific cpu module source file.
* \ingroup      Target_TRICORE_TC1798
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2015  by Feaser    http://www.feaser.com    All rights reserved
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
#include "cpu_comp.h"                            /* compiler specific CPU definitions  */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
#define CPU_INIT_MODE_TIMEOUT_MS       (250u)


/****************************************************************************************
* Local function prototypes
****************************************************************************************/
static void CpuWriteWDTCON0(blt_int32u uwValue);


/************************************************************************************//**
** \brief     Disable global interrupts.
** \return    none.
**
****************************************************************************************/
void CpuIrqDisable(void)
{
  _disable();
} /*** end of CpuIrqDisable ***/


/************************************************************************************//**
** \brief     Enable global interrupts.
** \return    none.
**
****************************************************************************************/
void CpuIrqEnable(void)
{
  _enable();
} /*** end of CpuIrqEnable ***/


/************************************************************************************//**
** \brief     This macro clears the EndInit bit, which controls access to system critical
**            registers. Clearing the EndInit bit unlocks all EndInit protectedd
**            registers. Modifications of the EndInit bit are monitored by the watchdog
**            timer such that after clearing the EndInit, the watchdog timer enters a
**            defined time-out mode; EndInit must be set again before the time-out
**            expires.
** \return    none.
**
****************************************************************************************/
void CpuEnterInitMode(void)
{
  blt_int32u timeout;
  
  /* request clearing of the EndInit bit */
  CpuWriteWDTCON0(WDT_CON0.reg & ~0x00000001);
  /* set timeout to wait for hardware handshake */
  timeout = TimerGet() + CPU_INIT_MODE_TIMEOUT_MS;
  /* wait for hardware handshake */
  while (WDT_CON0.bits.ENDINIT != 0)
  {
    /* keep the watchdog happy */
    CopService();
    /* break loop if timeout occurred */
    if (TimerGet() > timeout)
    {
      break;
    }
  }
} /*** end of CpuEnterInitMode ***/


/************************************************************************************//**
** \brief     This macro sets the EndInit bit, which controls access to system critical
**            registers. Setting the EndInit bit locks all EndInit protected registers.
** \return    none.
**
****************************************************************************************/
void CpuLeaveInitMode(void)
{
  /* set the EndInit bit */
  CpuWriteWDTCON0(WDT_CON0.reg | 0x00000001);
} /*** end of CpuLeaveInitMode ***/


/************************************************************************************//**
** \brief     Write a new value to the WDTCON0 register.
** \param     value New value for the WDTCON0 register.
** \return    none.
**
****************************************************************************************/
static void CpuWriteWDTCON0(blt_int32u value)
{
  blt_int32u dummy;

  /* load current value of the WDTCON0 register */
  dummy = WDT_CON0.reg;
  /* set HWPW1 = 1111b */
  dummy |= 0x000000F0;
  /* set HWPW0 = WDTDR */
  if (WDT_CON1.bits.DR)
  {
    dummy |= 0x00000008;
  }
  else
  {
    dummy &= ~0x00000008;
  }
  /* set HWPW0 = WDTIR */
  if (WDT_CON1.bits.IR)
  {
    dummy |= 0x00000004;
  }
  else
  {
    dummy &= ~0x00000004;
  }
  /* set WDTLCK = 0 */
  dummy &= ~0x00000002;
  /* unlock access */
  WDT_CON0.reg = dummy;
  /* set HWPW1 = 1111b and WDTLCK = 1 */
  value |=  0x000000F2;
  /* set HWPW0 = 00b */
  value &= ~0x0000000C;
  /* write access and lock */
  WDT_CON0.reg = value;
} /*** end of CpuWriteWDTCON0 ***/


/*********************************** end of cpu_comp.c *********************************/
