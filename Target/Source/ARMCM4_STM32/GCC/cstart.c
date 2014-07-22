/************************************************************************************//**
* \file         Source\ARMCM4_STM32\GCC\cstart.c
* \brief        Bootloader C startup source file.
* \ingroup      Target_ARMCM4_STM32
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
* You should have received a copy of the GNU General Public License along with OpenBLT.
* If not, see <http://www.gnu.org/licenses/>.
*
* A special exception to the GPL is included to allow you to distribute a combined work 
* that includes OpenBLT without being obliged to provide the source code for any 
* proprietary components. The exception text is included at the bottom of the license
* file <license.html>.
* 
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                    /* bootloader generic header      */


/****************************************************************************************
* External function protoypes
****************************************************************************************/
extern int  main(void);


/****************************************************************************************
* External data declarations
****************************************************************************************/
/* these externals are declared by the linker */
extern blt_int32u _etext;
extern blt_int32u _data;
extern blt_int32u _edata;
extern blt_int32u _bss;
extern blt_int32u _ebss;
extern blt_int32u _estack;


/************************************************************************************//**
** \brief     Reset interrupt service routine. Configures the stack, initializes 
**            RAM and jumps to function main.
** \return    none.
**
****************************************************************************************/
void reset_handler(void)
{
  blt_int32u *pSrc, *pDest;

  /* disable interrupts first */
  __asm("    cpsid     i");
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