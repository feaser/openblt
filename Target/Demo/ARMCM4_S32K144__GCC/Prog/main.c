/************************************************************************************//**
* \file         Demo/_template/Prog/main.c
* \brief        Demo program application source file.
* \ingroup      Prog__template
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

/****************************************************************************************
* Include files
****************************************************************************************/
#include "header.h"                                    /* generic header               */

/* TODO ##Prog The PC host tools that ship with the OpenBLT bootloader (MicroBoot and
 * BootCommander) expect the firmware binary to be in the S-Record format. Make sure to
 * configure the project in your IDE or Makefile to generate an S-Record. This is
 * typically done in a post-build step. More information about the S-Record format can
 * be found here: https://en.wikipedia.org/wiki/SREC_(file_format).
 */

/* TODO ##Prog Note that the first part of flash memory is reserved for the bootloader. 
 * To find out exactly how much, have a look at the flashLayout[] array in the 
 * bootloader's flash.c file. The flash sectors reserved are typically commented out in 
 * flashLayout[]. This user program's linker descriptor file must be adjusted such that 
 * the start address is moved forward a number of bytes that equals how many bytes are 
 * reserved for the bootloader.
 */
 
/* TODO ##Prog The bootloader should handle the remapping of the interrupt vector table
 * base address such that this user program's vector table is used. This functionality
 * is implemented in the bootloader's function CpuStartUserProgram() in file cpu.c.
 * Some microcontroller manufacturer provided peripheral driver libraries, which might
 * be used in this user program, could unfortunately reset this and map the interrupt
 * vector table back to the start of flash. This would be where the bootloader's
 * interrupt vector table is located, which is incorrect. In this case the interrupt
 * vector table remapping needs to be explicitely implemented here again before the
 * interrupts are enabled.
 */
 
/* TODO ##Prog Some microcontrollers store the initial value for the stackpointer
 * CPU register in the interrupt vector table and the microcontroller automatically loads
 * this value into the stackpointer CPU register upon reset. Since this user program is
 * not directly started by a microcontroller reset, but by the bootloader, the CPU
 * stackpointer register will not be initialized properly. In this case the CPU
 * stackpointer register must be explicitly initialized by the startup code. The best
 * place to do this is at the start of the reset interrupt handler, which is typically
 * located in a file with assembly code.
 */  


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);


/************************************************************************************//**
** \brief     This is the entry point for the bootloader application and is called 
**            by the reset interrupt vector after the C-startup routines executed.
** \return    Program return code.
**
****************************************************************************************/
int main(void)
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

  /* program should never get here */
  return 0;
} /*** end of main ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller.
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  /* TODO ##Prog Initialize the microcontroller. This typically consists of configuring
   * the microcontroller's system clock and configuring the GPIO for the communication
   * peripheral(s) supported in this user program's boot.c. If the microcontroller does
   * not enable interrupts automatically after a reset, the interrupts should be enabled
   * here as well.
   */
  /* initialize the timer driver */
  TimerInit();
  /* initialize the led driver */
  LedInit();
} /*** end of Init ***/


/*********************************** end of main.c *************************************/
