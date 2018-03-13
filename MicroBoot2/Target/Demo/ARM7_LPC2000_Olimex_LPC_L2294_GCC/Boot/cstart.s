/************************************************************************************//**
* \file         Demo\ARM7_LPC2000_Olimex_LPC_L2294_GCC\Boot\cstart.s
* \brief        Bootloader C-startup assembly file.
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
* Macro definitions
****************************************************************************************/
/* stack Sizes */
.set  UND_STACK_SIZE, 0x00000004   /* stack for "undef" interrupts is 4 bytes          */
.set  ABT_STACK_SIZE, 0x00000004   /* stack for "abort" interrupts is 4 bytes          */
.set  FIQ_STACK_SIZE, 0x00000004   /* stack for "FIQ" interrupts  is 4 bytes           */
.set  IRQ_STACK_SIZE, 0X00000004   /* stack for "IRQ" normal interrupts is 4 bytes     */
.set  SVC_STACK_SIZE, 0x00000004   /* stack for "SVC" supervisor mode is 4 bytes       */

/* mode bits and Interrupt (I & F) flags in program status registers (PSRs) */
.set  MODE_USR, 0x10               /* Normal User Mode                                 */
.set  MODE_FIQ, 0x11               /* FIQ Processing Fast Interrupts Mode              */
.set  MODE_IRQ, 0x12               /* IRQ Processing Standard Interrupts Mode          */
.set  MODE_SVC, 0x13               /* Supervisor Processing Software Interrupts Mode   */
.set  MODE_ABT, 0x17               /* Abort Processing memory Faults Mode              */
.set  MODE_UND, 0x1B               /* Undefined Processing Undefined Instructions Mode */
.set  MODE_SYS, 0x1F               /* System Running Priviledged OS Tasks Mode         */
.set  I_BIT, 0x80                  /* when I bit is set, IRQ is disabled               */
.set  F_BIT, 0x40                  /* when F bit is set, FIQ is disabled               */

/* microcontroller registers */
.set  MEMMAP, 0xE01FC040           /* MEMMAP register                                  */

.text
.arm

.global	Reset_Handler
.global	SetupRAM
.global _startup
.func   _startup

_startup:
/****************************************************************************************
* Interrupt vector table
****************************************************************************************/
_vectors:       ldr     PC, Reset_Addr       /* point to Reset_Handler address         */   
                ldr     PC, Undef_Addr       /* point to UNDEF_ISR address             */
                ldr     PC, Undef_Addr        /* point to SWI_ISR address               */
                ldr     PC, PAbt_Addr        /* point to UNDEF_ISR address             */
                ldr     PC, DAbt_Addr        /* point to UNDEF_ISR address             */
                nop                          /* reserved for Philips ISP checksum      */
                ldr     PC, IRQ_Addr         /* point to IRQ_ISR address               */
                ldr     PC, FIQ_Addr         /* point to FIQ_ISR address               */

Reset_Addr:     .word   Reset_Handler        /* defined in this module below           */
Undef_Addr:     .word   UNDEF_ISR            /* defined in vectors.c                   */
PAbt_Addr:      .word   UNDEF_ISR            /* defined in vectors.c                   */
DAbt_Addr:      .word   UNDEF_ISR            /* defined in vectors.c                   */
FIQ_Addr:       .word   FIQ_ISR              /* defined in vectors.c                   */
IRQ_Addr:       .word   IRQ_ISR              /* defined in vectors.c                   */
                .word   0                    /* rounds vectors and ISR addresses to    */
                                             /* 64 bytes                               */


/************************************************************************************//**
** \brief     Reset interrupt service routine. Configures the stack for each mode,
**            disables the IRQ and FIQ interrupts, initializes RAM and jumps to
**            function main.
** \return    none.
**
****************************************************************************************/
Reset_Handler:  
          /* setup a stack and disable interrupts for each mode */
          ldr   r0, =_stack_end
          msr   CPSR_c, #MODE_UND|I_BIT|F_BIT /* Undefined Instruction Mode  */
          mov   sp, r0
          sub   r0, r0, #UND_STACK_SIZE
          msr   CPSR_c, #MODE_ABT|I_BIT|F_BIT /* Abort Mode */
          mov   sp, r0
          sub   r0, r0, #ABT_STACK_SIZE
          msr   CPSR_c, #MODE_FIQ|I_BIT|F_BIT /* FIQ Mode */
          mov   sp, r0
          sub   r0, r0, #FIQ_STACK_SIZE
          msr   CPSR_c, #MODE_IRQ|I_BIT|F_BIT /* IRQ Mode */
          mov   sp, r0
          sub   r0, r0, #IRQ_STACK_SIZE
          msr   CPSR_c, #MODE_SVC|I_BIT|F_BIT /* Supervisor Mode */
          mov   sp, r0
          sub   r0, r0, #SVC_STACK_SIZE
          msr   CPSR_c, #MODE_SYS|I_BIT|F_BIT /* User Mode */
          mov   sp, r0
          /* copy .data section from ROM to RAM and zero out .bss section */
          bl    SetupRAM
          /* start bootloader program by jumping to main() */
          b     main
/*** end of Reset_Handler ***/


/************************************************************************************//**
** \brief     Initializes RAM by copying .data section from ROM to RAM and zero-ing
**            out .bss section.
** \return    none.
**
****************************************************************************************/
SetupRAM:  
          /* copy .data section (Copy from ROM to RAM) */
          ldr     R1, =_etext
          ldr     R2, =_data
          ldr     R3, =_edata
1:        cmp     R2, R3
          ldrlo   R0, [R1], #4
          strlo   R0, [R2], #4
          blo     1b

          /* clear .bss section (Zero init)  */
          mov     R0, #0
          ldr     R1, =_bss_start
          ldr     R2, =_bss_end
2:        cmp     R1, R2
          strlo   R0, [R1], #4
          blo     2b
          /* return */
          bx      lr
/*** end of SetupRAM ***/
.endfunc 


.end
/*********************************** end of cstart.s ***********************************/
