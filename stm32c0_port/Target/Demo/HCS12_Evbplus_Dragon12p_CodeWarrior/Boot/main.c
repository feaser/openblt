/************************************************************************************//**
* \file         Demo/HCS12_Evbplus_Dragon12p_CodeWarrior/Boot/main.c
* \brief        Bootloader application source file.
* \ingroup      Boot_HCS12_Evbplus_Dragon12p_CodeWarrior
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
#include "boot.h"                                /* bootloader generic header          */
#include "derivative.h"                          /* MCU registers                      */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);


/************************************************************************************//**
** \brief     This is the entry point for the bootloader application and is called 
**            by the reset interrupt vector after the C-startup routines executed.
** \return    none.
**
****************************************************************************************/
void main(void)
{
  /* initialize the microcontroller */
  Init();
  
  /* initialize the bootloader */
  BootInit();
  
  /* start the infinite program loop */
  while (1)
  {
    /* run the bootloader task */
    BootTask();
  }
} /*** end of main ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller. 
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  blt_int8u  synrCnt;
  blt_int8u  refdvCnt;
  blt_int32u systemSpeed;
  blt_bool   found = BLT_FALSE;

  /* initialize the system clock to BOOT_CPU_SYSTEM_SPEED_KHZ by configuring the PLL 
   * subsystem. first default to oscillator clock source.
   */
  CLKSEL &= ~0x80;
  /* search for the synthesizer and reference divider values. the equation to use is:
   * PLLCLK = EXTCLK * ( (synrCnt + 1) / (refdvCnt + 1) ), with synrCnt can be from
   * 0..63 and refdvCnt can be from 0..15
   */
  for (refdvCnt = 0; refdvCnt <= 15; refdvCnt++)
  {
    for (synrCnt = 0; synrCnt <= 63; synrCnt++)
    {
      /* calculate the system speed with these SYNR and REFDV settings */
      systemSpeed = ((blt_int32u)BOOT_CPU_XTAL_SPEED_KHZ * (synrCnt+1)) / (refdvCnt+1);
      /* was a match found? */
      if (systemSpeed == BOOT_CPU_SYSTEM_SPEED_KHZ)
      {
        /* flag success */
        found = BLT_TRUE;
        /* break loop */
        break;
      }
    }
    if (found == BLT_TRUE)
    {
      /* break this loop as well if a match was already found */
      break;
    }
  }
  /* flag error if no match was found */
  ASSERT_RT(found == BLT_TRUE);
  /* set the synthesizer and reference divider values */
  SYNR = synrCnt;
  REFDV = refdvCnt;
  /* wait for PLL to lock */
  while((CRGFLG & 0x08) == 0)
  {
    ;
  }
  /* select PLL as clock source */
  CLKSEL |= 0x80;
} /*** end of Init ***/


/*********************************** end of main.c *************************************/
