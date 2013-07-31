/************************************************************************************//**
* \file         ARM7_LPC2000\GCC\cstart.s
* \brief        Bootloader C-startup assembly file.
* \ingroup      Target_ARM7_LPC2000
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

.extern ComSetConnectEntryState
.global EntryFromProg
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
                ldr     PC, SWI_Addr         /* point to SWI_ISR address               */
                ldr     PC, PAbt_Addr        /* point to UNDEF_ISR address             */
                ldr     PC, DAbt_Addr        /* point to UNDEF_ISR address             */
                nop                          /* reserved for Philips ISP checksum      */
                ldr     PC, IRQ_Addr         /* point to IRQ_ISR address               */
                ldr     PC, FIQ_Addr         /* point to FIQ_ISR address               */

Reset_Addr:     .word   Reset_Handler        /* defined in this module below           */
Undef_Addr:     .word   UNDEF_ISR            /* defined in vectors.c                   */
SWI_Addr:       .word   Reset_Handler_SWI    /* defined in this module below           */
PAbt_Addr:      .word   UNDEF_ISR            /* defined in vectors.c                   */
DAbt_Addr:      .word   UNDEF_ISR            /* defined in vectors.c                   */
FIQ_Addr:       .word   FIQ_ISR              /* defined in vectors.c                   */
IRQ_Addr:       .word   IRQ_ISR              /* defined in vectors.c                   */
                .word   0                    /* rounds vectors and ISR addresses to    */
                                             /* 64 bytes                               */


/************************************************************************************//**
** \brief     Called by the user program to activate the bootloader. Do not place
**            any assembly code between this function and the end of the vector
**            table. This guarantees that this function is located at address
**            0x00000040. The user program can call this function from C in the 
**            following way:
**                         void ActivateBootloader(void)
**                         {
**                           void (*pEntryFromProgFnc)(void);
**
**                           pEntryFromProgFnc = (void*)0x00000040;
**                           pEntryFromProgFnc();
**                         }
** \return    none.
**
****************************************************************************************/
EntryFromProg:  
          /* remap interrupt vector table back to ROM to make sure the bootloader
           * vectors are used:
           *   MEMMAP = 0x01;
           */
          ldr   r0, =MEMMAP
          mov   r1, #1
          str   r1, [r0, #0]
          /* trigger SWI to entry supervisor mode and run Reset_Handler_SWI */
          swi   0
 /*** end of EntryFromProg ***/


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
** \brief     Reset handler for a software reset after the user program activated
**            the bootloader. Configures the stack for each mode, disables the IRQ 
**            and FIQ interrupts, initializes RAM. Most importantly, before jumping
**            to function main to start the bootloader program, the COM interface
**            is configured to start in a connected state. Here is why:
**            At the start of a new programming session, the host sends the XCP 
**            CONNECT command. Upon reception, the user program activates the 
**            bootloader by jumping to function EntryFromProg(), which triggers the 
**            SWI instruction that gets the program to this point. When the 
**            bootloader is started, it now needs to send the response to the XCP
**            CONNECT command, because the host is waiting for this before it can
**            continue.
** \return    none.
**
****************************************************************************************/
Reset_Handler_SWI:  
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
          /* this part makes the difference with the normal Reset_Handler */
          bl    ComSetConnectEntryState
          /* start bootloader program by jumping to main() */
          b     main
/*** end of Reset_Handler_SWI ***/


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
