/*****************************************************************************
 * Copyright (c) 2001, 2002 Rowley Associates Limited.                       *
 *                                                                           *
 * This file may be distributed under the terms of the License Agreement     *
 * provided with this software.                                              *
 *                                                                           *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE   *
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. *
 *****************************************************************************/

/*****************************************************************************
 *                           Preprocessor Definitions
 *                           ------------------------
 * APP_ENTRY_POINT
 *
 *   Defines the application entry point function, if undefined this setting
 *   defaults to "main".
 *
 * INITIALIZE_STACKS
 *
 *   If defined, the contents of the stacks will be initialized to a the 
 *   value 0xCC.
 *
 * INITIALIZE_SECONDARY_SECTIONS
 *
 *   If defined, the .text2, .data2 and .bss2 sections will be initialized.
 *
 * SUPERVISOR_START
 *
 *   If defined, the application will start up in supervisor mode. If 
 *   undefined the application will start up in system mode.
 *
 * FULL_LIBRARY
 *
 *  If defined then 
 *    - argc, argv are setup by the debug_getargs.
 *    - the exit symbol is defined and executes on return from main.
 *    - the exit symbol calls destructors, atexit functions and then debug_exit.
 *  
 *  If not defined then
 *    - argc and argv are zero.
 *    - the exit symbol is defined, executes on return from main and loops
 *
 *****************************************************************************/

  .section .vectors, "ax"
  .code 32
  .align 0
  .global _vectors
  .global reset_handler

/*****************************************************************************
 * Exception Vectors                                                         *
 *****************************************************************************/
_vectors:
  ldr pc, [pc, #reset_handler_address - . - 8]  /* reset */
  ldr pc, [pc, #undef_handler_address - . - 8]  /* undefined instruction */
  ldr pc, [pc, #undef_handler_address - . - 8]  /* swi handler */
  ldr pc, [pc, #pabort_handler_address - . - 8] /* abort prefetch */
  ldr pc, [pc, #dabort_handler_address - . - 8] /* abort data */
  .word 0xB9205F88                              /* boot loader checksum */
  ldr pc, [pc, #-0xFF0]                         /* irq handler */
  ldr pc, [pc, #fiq_handler_address - . - 8]    /* fiq handler */

reset_handler_address:
  .word Reset_Handler
undef_handler_address:
  .word undef_handler
pabort_handler_address:
  .word pabort_handler
dabort_handler_address:
  .word dabort_handler
fiq_handler_address:
  .word fiq_handler


  .section .init, "ax"
  .code 32
  .align 4

#ifndef APP_ENTRY_POINT
#define APP_ENTRY_POINT main
#endif

#ifndef ARGSSPACE
#define ARGSSPACE 128
#endif
                
  .weak Reset_Handler
  .global __start  
  .global __gccmain
  .extern APP_ENTRY_POINT
  .global exit

/************************************************************************************//**
** \brief     Reset interrupt service routine. Configures the stack for each mode,
**            disables the IRQ and FIQ interrupts, initializes RAM and jumps to
**            function main.
** \return    none.
**
****************************************************************************************/
Reset_Handler:
__start:                        
  mrs r0, cpsr
  bic r0, r0, #0x1F

  /* Setup stacks */ 
  orr r1, r0, #0x1B /* Undefined mode */
  msr cpsr_cxsf, r1
  ldr sp, =__stack_und_end__
#ifdef __ARM_EABI__
  bic sp, sp, #0x7
#endif
  
  orr r1, r0, #0x17 /* Abort mode */
  msr cpsr_cxsf, r1
  ldr sp, =__stack_abt_end__
#ifdef __ARM_EABI__
  bic sp, sp, #0x7
#endif

  orr r1, r0, #0x12 /* IRQ mode */
  msr cpsr_cxsf, r1
  ldr sp, =__stack_irq_end__
#ifdef __ARM_EABI__
  bic sp, sp, #0x7
#endif

  orr r1, r0, #0x11 /* FIQ mode */
  msr cpsr_cxsf, r1
  ldr sp, =__stack_fiq_end__
#ifdef __ARM_EABI__
  bic sp, sp, #0x7
#endif

  orr r1, r0, #0x13 /* Supervisor mode */
  msr cpsr_cxsf, r1
  ldr sp, =__stack_svc_end__
#ifdef __ARM_EABI__
  bic sp, sp, #0x7
#endif

#ifdef SUPERVISOR_START
  /* Start application in supervisor mode */
  ldr r1, =__stack_end__ /* Setup user/system mode stack */ 
#ifdef __ARM_EABI__
  bic r1, r1, #0x7
#endif
  mov r2, sp
  stmfd r2!, {r1}
  ldmfd r2, {sp}^
#else
  /* Start application in system mode */
  orr r1, r0, #0x1F /* System mode */
  msr cpsr_cxsf, r1
  ldr sp, =__stack_end__
#ifdef __ARM_EABI__
  bic sp, sp, #0x7
#endif
#endif

#ifdef INITIALIZE_STACKS  
  mov r2, #0xCC
  ldr r0, =__stack_und_start__
  ldr r1, =__stack_und_end__
  bl memory_set
  ldr r0, =__stack_abt_start__
  ldr r1, =__stack_abt_end__
  bl memory_set
  ldr r0, =__stack_irq_start__
  ldr r1, =__stack_irq_end__
  bl memory_set
  ldr r0, =__stack_fiq_start__
  ldr r1, =__stack_fiq_end__
  bl memory_set
  ldr r0, =__stack_svc_start__
  ldr r1, =__stack_svc_end__
  bl memory_set  
  ldr r0, =__stack_start__
  ldr r1, =__stack_end__
  bl memory_set  
#endif

  /* Copy initialised memory sections into RAM (if necessary). */
  ldr r0, =__data_load_start__
  ldr r1, =__data_start__
  ldr r2, =__data_end__
  bl memory_copy
  ldr r0, =__text_load_start__
  ldr r1, =__text_start__
  ldr r2, =__text_end__
  bl memory_copy
  ldr r0, =__fast_load_start__
  ldr r1, =__fast_start__
  ldr r2, =__fast_end__
  bl memory_copy
  ldr r0, =__ctors_load_start__
  ldr r1, =__ctors_start__
  ldr r2, =__ctors_end__
  bl memory_copy
  ldr r0, =__dtors_load_start__
  ldr r1, =__dtors_start__
  ldr r2, =__dtors_end__
  bl memory_copy
  ldr r0, =__rodata_load_start__
  ldr r1, =__rodata_start__
  ldr r2, =__rodata_end__
  bl memory_copy
#ifdef INITIALIZE_SECONDARY_SECTIONS
  ldr r0, =__data2_load_start__
  ldr r1, =__data2_start__
  ldr r2, =__data2_end__
  bl memory_copy
  ldr r0, =__text2_load_start__
  ldr r1, =__text2_start__
  ldr r2, =__text2_end__
  bl memory_copy
  ldr r0, =__rodata2_load_start__
  ldr r1, =__rodata2_start__
  ldr r2, =__rodata2_end__
  bl memory_copy
#endif /* #ifdef INITIALIZE_SECONDARY_SECTIONS */
  
  /* Zero the bss. */
  ldr r0, =__bss_start__
  ldr r1, =__bss_end__
  mov r2, #0
  bl memory_set
#ifdef INITIALIZE_SECONDARY_SECTIONS
  ldr r0, =__bss2_start__
  ldr r1, =__bss2_end__
  mov r2, #0
  bl memory_set
#endif /* #ifdef INITIALIZE_SECONDARY_SECTIONS */

  /* Initialise the heap */
  ldr r0, = __heap_start__
  ldr r1, = __heap_end__
  sub r1, r1, r0
  cmp r1, #8
  movge r2, #0
  strge r2, [r0], #+4
  strge r1, [r0]

  /* Call constructors */
  ldr r0, =__ctors_start__
  ldr r1, =__ctors_end__
ctor_loop:
  cmp r0, r1
  beq ctor_end
  ldr r2, [r0], #+4
  stmfd sp!, {r0-r1}
  mov lr, pc
#ifdef __ARM_ARCH_3__
  mov pc, r2
#else    
  bx r2
#endif
  ldmfd sp!, {r0-r1}
  b ctor_loop
ctor_end:

  .type start, function
start:
  /* Jump to application entry point */
#ifdef FULL_LIBRARY
  mov r0, #ARGSSPACE
  ldr r1, =args
  ldr r2, =debug_getargs
  mov lr, pc
#ifdef __ARM_ARCH_3__
  mov pc, r2
#else    
  bx r2
#endif 
  ldr r1, =args
#else
  mov r0, #0
  mov r1, #0
#endif
  ldr r2, =APP_ENTRY_POINT
  mov lr, pc
#ifdef __ARM_ARCH_3__
  mov pc, r2
#else    
  bx r2
#endif

exit:
#ifdef FULL_LIBRARY  
  mov r5, r0 // save the exit parameter/return result
  
  /* Call destructors */
  ldr r0, =__dtors_start__
  ldr r1, =__dtors_end__
dtor_loop:
  cmp r0, r1
  beq dtor_end
  ldr r2, [r0], #+4
  stmfd sp!, {r0-r1}
  mov lr, pc
#ifdef __ARM_ARCH_3__
  mov pc, r2
#else    
  bx r2
#endif
  ldmfd sp!, {r0-r1}
  b dtor_loop
dtor_end:

  /* Call atexit functions */
  ldr r2, =_execute_at_exit_fns
  mov lr, pc
#ifdef __ARM_ARCH_3__
  mov pc, r2
#else    
  bx r2
#endif 

  /* Call debug_exit with return result/exit parameter */
  mov r0, r5
  ldr r2, =debug_exit
  mov lr, pc
#ifdef __ARM_ARCH_3__
  mov pc, r2
#else    
  bx r2
#endif 

#endif

  /* Returned from application entry point/debug_exit, loop forever. */
exit_loop:
  b exit_loop




memory_copy:
  cmp r0, r1
  moveq pc, lr  
  subs r2, r2, r1
  moveq pc, lr  

  /* if either pointer or length is not word aligned then byte copy */
  orr r3, r0, r1
  orr r3, r3, r2
  tst r3, #0x3
  bne 2f
  /* word copy */
1:
  ldr r3, [r0], #+4
  str r3, [r1], #+4
  subs r2, r2, #4
  bne 1b
  mov pc, lr
  /* byte copy */
2:
  ldrb r3, [r0], #+1
  strb r3, [r1], #+1
  subs r2, r2, #1
  bne 2b
  mov pc, lr

memory_set:
  cmp r0, r1
  moveq pc, lr
  strb r2, [r0], #1
  b memory_set


  .section .init, "ax"
  .code 32
  .align 0

/******************************************************************************
 *                                                                            *
 * Default exception handlers                                                 *
 *                                                                            *
 ******************************************************************************/
/******************************************************************************
 *                                                                            *
 * Default exception handlers                                                 *
 * These are declared weak symbols so they can be redefined in user code.     * 
 *                                                                            *
 ******************************************************************************/
undef_handler:
  b undef_handler
  
pabort_handler:
  b pabort_handler
  
dabort_handler:
  b dabort_handler
  
fiq_handler:
  b fiq_handler

irq_handler:
  b irq_handler

  .weak undef_handler, pabort_handler, dabort_handler, fiq_handler, irq_handler
                                                    

#ifdef FULL_LIBRARY
  .bss
args:
  .space ARGSSPACE
#endif

  /* Setup attibutes of stack and heap sections so they don't take up unnecessary room in the elf file */
  .section .stack, "wa", %nobits
  .section .stack_abt, "wa", %nobits
  .section .stack_irq, "wa", %nobits
  .section .stack_fiq, "wa", %nobits
  .section .stack_svc, "wa", %nobits
  .section .stack_und, "wa", %nobits
  .section .heap, "wa", %nobits

