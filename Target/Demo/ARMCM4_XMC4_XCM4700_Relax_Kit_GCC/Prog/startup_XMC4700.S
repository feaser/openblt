/* File: startup_XMC4700.S
 * Purpose: startup file for Cortex-M4 devices. Should use with
 *   GCC for ARM Embedded Processors
 * Version: V1.3
 * Date: 08 Feb 2012
 *
 * Copyright (c) 2012, ARM Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the ARM Limited nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ARM LIMITED BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
    .syntax unified
    .arch armv7-m

    .section .stack
    .align 3
#ifdef __STACK_SIZE
    .equ    Stack_Size, __STACK_SIZE
#else
    .equ    Stack_Size, 0x400
#endif
    .globl    __StackTop
    .globl    __StackLimit
__StackLimit:
    .space    Stack_Size
    .size __StackLimit, . - __StackLimit
__StackTop:
    .size __StackTop, . - __StackTop

    .section .heap
    .align 3
#ifdef __HEAP_SIZE
    .equ    Heap_Size, __HEAP_SIZE
#else
    .equ    Heap_Size, 0xC00
#endif
    .globl    __HeapBase
    .globl    __HeapLimit
__HeapBase:
    .if    Heap_Size
    .space    Heap_Size
    .endif
    .size __HeapBase, . - __HeapBase
__HeapLimit:
    .size __HeapLimit, . - __HeapLimit

    .section .isr_vector
    .align 2
    .globl __isr_vector
__isr_vector:
    .long    __StackTop            /* Top of Stack */
    .long    Reset_Handler         /* Reset Handler */
    .long    NMI_Handler           /* NMI Handler */
    .long    HardFault_Handler     /* Hard Fault Handler */
    .long    MemManage_Handler     /* MPU Fault Handler */
    .long    BusFault_Handler      /* Bus Fault Handler */
    .long    UsageFault_Handler    /* Usage Fault Handler */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    SVC_Handler           /* SVCall Handler */
    .long    DebugMon_Handler      /* Debug Monitor Handler */
    .long    0                     /* Reserved */
    .long    PendSV_Handler        /* PendSV Handler */
    .long    SysTick_Handler       /* SysTick Handler */

                // External Interrupts
    .long    SCU_0_IRQHandler            // Handler name for SR SCU_0
    .long    ERU0_0_IRQHandler           // Handler name for SR ERU0_0
    .long    ERU0_1_IRQHandler           // Handler name for SR ERU0_1
    .long    ERU0_2_IRQHandler           // Handler name for SR ERU0_2
    .long    ERU0_3_IRQHandler           // Handler name for SR ERU0_3
    .long    ERU1_0_IRQHandler           // Handler name for SR ERU1_0
    .long    ERU1_1_IRQHandler           // Handler name for SR ERU1_1
    .long    ERU1_2_IRQHandler           // Handler name for SR ERU1_2
    .long    ERU1_3_IRQHandler           // Handler name for SR ERU1_3
    .long    0                           // Not Available
    .long    0                           // Not Available
    .long    0                           // Not Available
    .long    PMU0_0_IRQHandler           // Handler name for SR PMU0_0
    .long    0                           // Not Available
    .long    VADC0_C0_0_IRQHandler       // Handler name for SR VADC0_C0_0
    .long    VADC0_C0_1_IRQHandler       // Handler name for SR VADC0_C0_1
    .long    VADC0_C0_2_IRQHandler       // Handler name for SR VADC0_C0_1
    .long    VADC0_C0_3_IRQHandler       // Handler name for SR VADC0_C0_3
    .long    VADC0_G0_0_IRQHandler       // Handler name for SR VADC0_G0_0
    .long    VADC0_G0_1_IRQHandler       // Handler name for SR VADC0_G0_1
    .long    VADC0_G0_2_IRQHandler       // Handler name for SR VADC0_G0_2
    .long    VADC0_G0_3_IRQHandler       // Handler name for SR VADC0_G0_3
    .long    VADC0_G1_0_IRQHandler       // Handler name for SR VADC0_G1_0
    .long    VADC0_G1_1_IRQHandler       // Handler name for SR VADC0_G1_1
    .long    VADC0_G1_2_IRQHandler       // Handler name for SR VADC0_G1_2
    .long    VADC0_G1_3_IRQHandler       // Handler name for SR VADC0_G1_3
    .long    VADC0_G2_0_IRQHandler       // Handler name for SR VADC0_G2_0
    .long    VADC0_G2_1_IRQHandler       // Handler name for SR VADC0_G2_1
    .long    VADC0_G2_2_IRQHandler       // Handler name for SR VADC0_G2_2
    .long    VADC0_G2_3_IRQHandler       // Handler name for SR VADC0_G2_3
    .long    VADC0_G3_0_IRQHandler       // Handler name for SR VADC0_G3_0
    .long    VADC0_G3_1_IRQHandler       // Handler name for SR VADC0_G3_1
    .long    VADC0_G3_2_IRQHandler       // Handler name for SR VADC0_G3_2
    .long    VADC0_G3_3_IRQHandler       // Handler name for SR VADC0_G3_3
    .long    DSD0_0_IRQHandler           // Handler name for SR DSD0_0
    .long    DSD0_1_IRQHandler           // Handler name for SR DSD0_1
    .long    DSD0_2_IRQHandler           // Handler name for SR DSD0_2
    .long    DSD0_3_IRQHandler           // Handler name for SR DSD0_3
    .long    DSD0_4_IRQHandler           // Handler name for SR DSD0_4
    .long    DSD0_5_IRQHandler           // Handler name for SR DSD0_5
    .long    DSD0_6_IRQHandler           // Handler name for SR DSD0_6
    .long    DSD0_7_IRQHandler           // Handler name for SR DSD0_7
    .long    DAC0_0_IRQHandler           // Handler name for SR DAC0_0
    .long    DAC0_1_IRQHandler           // Handler name for SR DAC0_0
    .long    CCU40_0_IRQHandler          // Handler name for SR CCU40_0
    .long    CCU40_1_IRQHandler          // Handler name for SR CCU40_1
    .long    CCU40_2_IRQHandler          // Handler name for SR CCU40_2
    .long    CCU40_3_IRQHandler          // Handler name for SR CCU40_3
    .long    CCU41_0_IRQHandler          // Handler name for SR CCU41_0
    .long    CCU41_1_IRQHandler          // Handler name for SR CCU41_1
    .long    CCU41_2_IRQHandler          // Handler name for SR CCU41_2
    .long    CCU41_3_IRQHandler          // Handler name for SR CCU41_3
    .long    CCU42_0_IRQHandler          // Handler name for SR CCU42_0
    .long    CCU42_1_IRQHandler          // Handler name for SR CCU42_1
    .long    CCU42_2_IRQHandler          // Handler name for SR CCU42_2
    .long    CCU42_3_IRQHandler          // Handler name for SR CCU42_3
    .long    CCU43_0_IRQHandler          // Handler name for SR CCU43_0
    .long    CCU43_1_IRQHandler          // Handler name for SR CCU43_1
    .long    CCU43_2_IRQHandler          // Handler name for SR CCU43_2
    .long    CCU43_3_IRQHandler          // Handler name for SR CCU43_3
    .long    CCU80_0_IRQHandler          // Handler name for SR CCU80_0
    .long    CCU80_1_IRQHandler          // Handler name for SR CCU80_1
    .long    CCU80_2_IRQHandler          // Handler name for SR CCU80_2
    .long    CCU80_3_IRQHandler          // Handler name for SR CCU80_3
    .long    CCU81_0_IRQHandler          // Handler name for SR CCU81_0
    .long    CCU81_1_IRQHandler          // Handler name for SR CCU81_1
    .long    CCU81_2_IRQHandler          // Handler name for SR CCU81_2
    .long    CCU81_3_IRQHandler          // Handler name for SR CCU81_3
    .long    POSIF0_0_IRQHandler         // Handler name for SR POSIF0_0
    .long    POSIF0_1_IRQHandler         // Handler name for SR POSIF0_1
    .long    POSIF1_0_IRQHandler         // Handler name for SR POSIF1_0
    .long    POSIF1_1_IRQHandler         // Handler name for SR POSIF1_1
    .long    0                           // Not Available
    .long    0                           // Not Available
    .long    0                           // Not Available
    .long    0                           // Not Available
    .long    CAN0_0_IRQHandler           // Handler name for SR CAN0_0
    .long    CAN0_1_IRQHandler           // Handler name for SR CAN0_1
    .long    CAN0_2_IRQHandler           // Handler name for SR CAN0_2
    .long    CAN0_3_IRQHandler           // Handler name for SR CAN0_3
    .long    CAN0_4_IRQHandler           // Handler name for SR CAN0_4
    .long    CAN0_5_IRQHandler           // Handler name for SR CAN0_5
    .long    CAN0_6_IRQHandler           // Handler name for SR CAN0_6
    .long    CAN0_7_IRQHandler           // Handler name for SR CAN0_7
    .long    USIC0_0_IRQHandler          // Handler name for SR USIC0_0
    .long    USIC0_1_IRQHandler          // Handler name for SR USIC0_1
    .long    USIC0_2_IRQHandler          // Handler name for SR USIC0_2
    .long    USIC0_3_IRQHandler          // Handler name for SR USIC0_3
    .long    USIC0_4_IRQHandler          // Handler name for SR USIC0_4
    .long    USIC0_5_IRQHandler          // Handler name for SR USIC0_5
    .long    USIC1_0_IRQHandler          // Handler name for SR USIC1_0
    .long    USIC1_1_IRQHandler          // Handler name for SR USIC1_1
    .long    USIC1_2_IRQHandler          // Handler name for SR USIC1_2
    .long    USIC1_3_IRQHandler          // Handler name for SR USIC1_3
    .long    USIC1_4_IRQHandler          // Handler name for SR USIC1_4
    .long    USIC1_5_IRQHandler          // Handler name for SR USIC1_5
    .long    USIC2_0_IRQHandler          // Handler name for SR USIC2_0
    .long    USIC2_1_IRQHandler          // Handler name for SR USIC2_1
    .long    USIC2_2_IRQHandler          // Handler name for SR USIC2_2
    .long    USIC2_3_IRQHandler          // Handler name for SR USIC2_3
    .long    USIC2_4_IRQHandler          // Handler name for SR USIC2_4
    .long    USIC2_5_IRQHandler          // Handler name for SR USIC2_5
    .long    LEDTS0_0_IRQHandler         // Handler name for SR LEDTS0_0
    .long    0                           // Not Available
    .long    FCE0_0_IRQHandler           // Handler name for SR FCE0_0
    .long    GPDMA0_0_IRQHandler         // Handler name for SR GPDMA0_0
    .long    SDMMC0_0_IRQHandler         // Handler name for SR SDMMC0_0
    .long    USB0_0_IRQHandler           // Handler name for SR USB0_0
    .long    ETH0_0_IRQHandler           // Handler name for SR ETH0_0
    .long    0                           // Not Available
    .long    GPDMA1_0_IRQHandler         // Handler name for SR GPDMA1_0
    .long    0                           // Not Available                 
    .long    0x55AA11EE                  // Reserved for OpenBLT checksum

    .size    __isr_vector, . - __isr_vector

    .text
    .thumb
    .thumb_func
    .align 2
    .globl    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
/* Initialize the stackpointer. this is done automatically after a reset event, but
 * this program is started by the bootloader and not a reset event. */
    ldr    r1, =__StackTop
    mov    sp, r1

/*     Loop to copy data from read only memory to RAM. The ranges
 *      of copy from/to are specified by following symbols evaluated in
 *      linker script.
 *      __etext: End of code section, i.e., begin of data sections to copy from.
 *      __data_start__/__data_end__: RAM address range that data should be
 *      copied to. Both must be aligned to 4 bytes boundary.  */

    ldr    r1, =__etext
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__

#if 1
/* Here are two copies of loop implemenations. First one favors code size
 * and the second one favors performance. Default uses the first one.
 * Change to "#if 0" to use the second one */
.flash_to_ram_loop:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .flash_to_ram_loop
#else
    subs    r3, r2
    ble    .flash_to_ram_loop_end
.flash_to_ram_loop:
    subs    r3, #4
    ldr    r0, [r1, r3]
    str    r0, [r2, r3]
    bgt    .flash_to_ram_loop
.flash_to_ram_loop_end:
#endif

#ifndef __NO_SYSTEM_INIT
    ldr    r0, =SystemInit
    blx    r0
#endif

    ldr    r0, =_start
    bx    r0
    .pool
    .size Reset_Handler, . - Reset_Handler

/* Our weak _start alternative if we don't use the library _start
 * The zero init section must be cleared, otherwise the librtary is
 * doing that */
    .align 1
    .thumb_func
    .weak _start
    .type _start, %function
_start:

    /* Zero fill the bss segment. */
    ldr   r1, = __bss_start__
    ldr   r2, = __bss_end__
    movs  r3, #0
    b  .fill_zero_bss
.loop_zero_bss:
    str  r3, [r1], #4

.fill_zero_bss:
    cmp  r1, r2
    bcc  .loop_zero_bss

    /* Jump to our main */
    bl main
    b .
    .size    _start, . - _start

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro    def_irq_handler    handler_name
    .align 1
    .thumb_func
    .weak    \handler_name
    .type    \handler_name, %function
\handler_name :
    b    .
    .size    \handler_name, . - \handler_name
    .endm

    def_irq_handler    NMI_Handler
    def_irq_handler    HardFault_Handler
    def_irq_handler    MemManage_Handler
    def_irq_handler    BusFault_Handler
    def_irq_handler    UsageFault_Handler
    def_irq_handler    SVC_Handler
    def_irq_handler    DebugMon_Handler
    def_irq_handler    PendSV_Handler
    def_irq_handler    SysTick_Handler
    def_irq_handler    Default_Handler

                // External Interrupts
    def_irq_handler    SCU_0_IRQHandler
    def_irq_handler    ERU0_0_IRQHandler
    def_irq_handler    ERU0_1_IRQHandler
    def_irq_handler    ERU0_2_IRQHandler
    def_irq_handler    ERU0_3_IRQHandler
    def_irq_handler    ERU1_0_IRQHandler
    def_irq_handler    ERU1_1_IRQHandler
    def_irq_handler    ERU1_2_IRQHandler
    def_irq_handler    ERU1_3_IRQHandler
    def_irq_handler    PMU0_0_IRQHandler
    def_irq_handler    VADC0_C0_0_IRQHandler
    def_irq_handler    VADC0_C0_1_IRQHandler
    def_irq_handler    VADC0_C0_2_IRQHandler
    def_irq_handler    VADC0_C0_3_IRQHandler
    def_irq_handler    VADC0_G0_0_IRQHandler
    def_irq_handler    VADC0_G0_1_IRQHandler
    def_irq_handler    VADC0_G0_2_IRQHandler
    def_irq_handler    VADC0_G0_3_IRQHandler
    def_irq_handler    VADC0_G1_0_IRQHandler
    def_irq_handler    VADC0_G1_1_IRQHandler
    def_irq_handler    VADC0_G1_2_IRQHandler
    def_irq_handler    VADC0_G1_3_IRQHandler
    def_irq_handler    VADC0_G2_0_IRQHandler
    def_irq_handler    VADC0_G2_1_IRQHandler
    def_irq_handler    VADC0_G2_2_IRQHandler
    def_irq_handler    VADC0_G2_3_IRQHandler
    def_irq_handler    VADC0_G3_0_IRQHandler
    def_irq_handler    VADC0_G3_1_IRQHandler
    def_irq_handler    VADC0_G3_2_IRQHandler
    def_irq_handler    VADC0_G3_3_IRQHandler
    def_irq_handler    DSD0_0_IRQHandler
    def_irq_handler    DSD0_1_IRQHandler
    def_irq_handler    DSD0_2_IRQHandler
    def_irq_handler    DSD0_3_IRQHandler
    def_irq_handler    DSD0_4_IRQHandler
    def_irq_handler    DSD0_5_IRQHandler
    def_irq_handler    DSD0_6_IRQHandler
    def_irq_handler    DSD0_7_IRQHandler
    def_irq_handler    DAC0_0_IRQHandler
    def_irq_handler    DAC0_1_IRQHandler
    def_irq_handler    CCU40_0_IRQHandler
    def_irq_handler    CCU40_1_IRQHandler
    def_irq_handler    CCU40_2_IRQHandler
    def_irq_handler    CCU40_3_IRQHandler
    def_irq_handler    CCU41_0_IRQHandler
    def_irq_handler    CCU41_1_IRQHandler
    def_irq_handler    CCU41_2_IRQHandler
    def_irq_handler    CCU41_3_IRQHandler
    def_irq_handler    CCU42_0_IRQHandler
    def_irq_handler    CCU42_1_IRQHandler
    def_irq_handler    CCU42_2_IRQHandler
    def_irq_handler    CCU42_3_IRQHandler
    def_irq_handler    CCU43_0_IRQHandler
    def_irq_handler    CCU43_1_IRQHandler
    def_irq_handler    CCU43_2_IRQHandler
    def_irq_handler    CCU43_3_IRQHandler
    def_irq_handler    CCU80_0_IRQHandler
    def_irq_handler    CCU80_1_IRQHandler
    def_irq_handler    CCU80_2_IRQHandler
    def_irq_handler    CCU80_3_IRQHandler
    def_irq_handler    CCU81_0_IRQHandler
    def_irq_handler    CCU81_1_IRQHandler
    def_irq_handler    CCU81_2_IRQHandler
    def_irq_handler    CCU81_3_IRQHandler
    def_irq_handler    POSIF0_0_IRQHandler
    def_irq_handler    POSIF0_1_IRQHandler
    def_irq_handler    POSIF1_0_IRQHandler
    def_irq_handler    POSIF1_1_IRQHandler
    def_irq_handler    CAN0_0_IRQHandler
    def_irq_handler    CAN0_1_IRQHandler
    def_irq_handler    CAN0_2_IRQHandler
    def_irq_handler    CAN0_3_IRQHandler
    def_irq_handler    CAN0_4_IRQHandler
    def_irq_handler    CAN0_5_IRQHandler
    def_irq_handler    CAN0_6_IRQHandler
    def_irq_handler    CAN0_7_IRQHandler
    def_irq_handler    USIC0_0_IRQHandler
    def_irq_handler    USIC0_1_IRQHandler
    def_irq_handler    USIC0_2_IRQHandler
    def_irq_handler    USIC0_3_IRQHandler
    def_irq_handler    USIC0_4_IRQHandler
    def_irq_handler    USIC0_5_IRQHandler
    def_irq_handler    USIC1_0_IRQHandler
    def_irq_handler    USIC1_1_IRQHandler
    def_irq_handler    USIC1_2_IRQHandler
    def_irq_handler    USIC1_3_IRQHandler
    def_irq_handler    USIC1_4_IRQHandler
    def_irq_handler    USIC1_5_IRQHandler
    def_irq_handler    USIC2_0_IRQHandler
    def_irq_handler    USIC2_1_IRQHandler
    def_irq_handler    USIC2_2_IRQHandler
    def_irq_handler    USIC2_3_IRQHandler
    def_irq_handler    USIC2_4_IRQHandler
    def_irq_handler    USIC2_5_IRQHandler
    def_irq_handler    LEDTS0_0_IRQHandler
    def_irq_handler    FCE0_0_IRQHandler
    def_irq_handler    GPDMA0_0_IRQHandler
    def_irq_handler    SDMMC0_0_IRQHandler
    def_irq_handler    USB0_0_IRQHandler
    def_irq_handler    ETH0_0_IRQHandler
    def_irq_handler    GPDMA1_0_IRQHandler

    .end
