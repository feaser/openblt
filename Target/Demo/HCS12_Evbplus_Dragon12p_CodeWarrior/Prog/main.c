/************************************************************************************//**
* \file         Demo/HCS12_Evbplus_Dragon12p_CodeWarrior/Prog/main.c
* \brief        Demo program application source file.
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
* Function prototypes
****************************************************************************************/
static void Init(void);
static void SysClockInit(void);


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
  /* initialize the bootloader interface */
  BootComInit();

  /* start the infinite program loop */
  while (1)
  {
    /* toggle LED with a fixed frequency */
    LedToggle();
    /* check for bootloader activation request */
    BootComCheckActivationRequest();
  }
} /*** end of main ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller.
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  /* initialize the system clock */
  SysClockInit();
  /* init the led driver */
  LedInit();
  /* init the timer driver */
  TimerInit();
  /* enable interrupts */
  asm("cli");
} /*** end of Init ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller.
** \return    none.
**
****************************************************************************************/
static void SysClockInit(void)
{
#if (BDM_DEBUGGING_ENABLED == 1)
  /* normally, OpenBLT configures the system clock speed before starting the user 
   * program. when BDM programming/debugging is used instead of OpenBLT, the
   * system clock speed configuration still needs to be done here.
   */
  unsigned char synrCnt;
  unsigned char refdvCnt;
  unsigned long systemSpeed;
  unsigned char found = 0;

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
      systemSpeed = ((unsigned long)BOOT_CPU_XTAL_SPEED_KHZ * (synrCnt+1)) / (refdvCnt+1);
      /* was a match found? */
      if (systemSpeed == BOOT_CPU_SYSTEM_SPEED_KHZ)
      {
        /* flag success */
        found = 1;
        /* break loop */
        break;
      }
    }
    if (found == 1)
    {
      /* break this loop as well if a match was already found */
      break;
    }
  }
  /* flag error if no match was found */
  if (found == 0)
  {
    while (1==1)
    {
      ; 
    }
  }
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
#endif /* (BDM_DEBUGGING_ENABLED == 1) */
} /*** end of SysClockInit ***/


/*********************************** end of main.c *************************************/
