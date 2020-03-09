/************************************************************************************//**
* \file         Demo/ARMCM3_EFM32_Olimex_EM32G880F128STK_GCC/Prog/cstart.c
* \brief        Demo program C startup source file.
* \ingroup      Prog_ARMCM3_EFM32_Olimex_EM32G880F128STK_GCC
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
* You have received a copy of the GNU General Public License along with OpenBLT. It 
* should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
* 
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "header.h"                                  /* generic header                 */


/****************************************************************************************
* External function protoypes
****************************************************************************************/
extern int main(void);


/****************************************************************************************
* External data declarations
****************************************************************************************/
/* these externals are declared by the linker */
extern unsigned long _etext;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;
extern unsigned long _estack;


/************************************************************************************//**
** \brief     Reset interrupt service routine. Configures the stack, initializes 
**            RAM and jumps to function main.
** \return    none.
**
****************************************************************************************/
void reset_handler(void)
{
  unsigned long *pSrc, *pDest;

  /* initialize stack pointer */
  __asm("    ldr r1, =_estack\n"
        "    mov sp, r1");
  /* copy the data segment initializers from flash to SRAM */
  pSrc = &_etext;
  for(pDest = &_data; pDest < &_edata; )
  {
    *pDest++ = *pSrc++;
  }
  /* zero fill the bss segment. this is done with inline assembly since this will 
   * clear the value of pDest if it is not kept in a register.
   */
  __asm("    ldr     r0, =_bss\n"
        "    ldr     r1, =_ebss\n"
        "    mov     r2, #0\n"
        "    .thumb_func\n"
        "zero_loop:\n"
        "        cmp     r0, r1\n"
        "        it      lt\n"
        "        strlt   r2, [r0], #4\n"
        "        blt     zero_loop");
  /* start the software application by calling its entry point */
  main();
} /*** end of reset_handler ***/


/************************************ end of cstart.c **********************************/