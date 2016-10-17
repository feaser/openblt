/*****************************************************************************
 * Copyright (c) 2009 Rowley Associates Limited.                             *
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
 * USE_PROCESS_STACK
 *
 *   If defined, thread mode will be configured to use the process stack if
 *   the size of the process stack is greater than zero bytes in length.
 *
 * INITIALIZE_STACK
 *
 *   If defined, the contents of the stack will be initialized to a the
 *   value 0xCC.
 *
 * INITIALIZE_SECONDARY_SECTIONS
 *
 *   If defined, the .data2, .text2, .rodata2 and .bss2 sections will be initialized.
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
 *****************************************************************************/

#ifndef APP_ENTRY_POINT
#define APP_ENTRY_POINT main
#endif

#ifndef ARGSSPACE
#define ARGSSPACE 128
#endif

  .extern APP_ENTRY_POINT
  .global exit
  .global reset_handler

  .section .init, "ax"
  .code 16
  .align 2

 
  .thumb_func
/****************************************************************************************
** NAME:           reset_handler
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Reset interrupt service routine. Configures the stack, initializes 
**                 RAM and jumps to function main.
**
****************************************************************************************/
reset_handler:
  ldr r1, =__stack_end__
#ifdef __ARM_EABI__
  mov r2, #0x7
  bic r1, r2
#endif
  mov sp, r1
#ifdef INITIALIZE_STACK
  mov r2, #0xCC
  ldr r0, =__stack_start__
  bl memory_set
#endif

#ifdef USE_PROCESS_STACK
  /* Set up process stack if size > 0 */
  ldr r1, =__stack_process_end__
  ldr r0, =__stack_process_start__
  sub r2, r1, r0
  beq 1f
#ifdef __ARM_EABI__
  mov r2, #0x7
  bic r1, r2
#endif
  msr psp, r1
  mov r2, #2
  msr control, r2
#ifdef INITIALIZE_STACK
  mov r2, #0xCC
  bl memory_set
#endif
1:
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
  blt 1f
  mov r2, #0
  str r2, [r0]
  add r0, r0, #4
  str r1, [r0]
1:

  /* Call constructors */
  ldr r0, =__ctors_start__
  ldr r1, =__ctors_end__
ctor_loop2:
  cmp r0, r1
  beq ctor_end2
  ldr r2, [r0]
  add r0, #4
  push {r0-r1}  
  blx r2
  pop {r0-r1}
  b ctor_loop2
ctor_end2:

  /* Setup initial call frame */
  mov r0, #0
  mov lr, r0
  mov r12, sp

start2:
  /* Jump to application entry point */
#ifdef FULL_LIBRARY
  mov r0, #ARGSSPACE
  ldr r1, =args
  ldr r2, =debug_getargs  
  blx r2
  ldr r1, =args
#else
  mov r0, #0
  mov r1, #0
#endif
  ldr r2, =APP_ENTRY_POINT
  blx r2

  .thumb_func
exit:
#ifdef FULL_LIBRARY  
  mov r5, r0 // save the exit parameter/return result

  /* Call destructors */
  ldr r0, =__dtors_start__
  ldr r1, =__dtors_end__
dtor_loop:
  cmp r0, r1
  beq dtor_end
  ldr r2, [r0]
  add r0, #4
  push {r0-r1}
  blx r2
  pop {r0-r1}
  b dtor_loop
dtor_end:

  /* Call atexit functions */
  ldr r2, =_execute_at_exit_fns  
  blx r2

  /* Call debug_exit with return result/exit parameter */
  mov r0, r5
  ldr r2, =debug_exit  
  blx r2
#endif

  /* Returned from application entry point, loop forever. */
exit_loop:
  b exit_loop

  .thumb_func
memory_copy:
  cmp r0, r1
  beq 2f
  sub r2, r2, r1
  beq 2f
1:
  ldrb r3, [r0]
  add r0, r0, #1
  strb r3, [r1]
  add r1, r1, #1
  sub r2, r2, #1
  bne 1b
2:
  bx lr

  .thumb_func
memory_set:
  cmp r0, r1
  beq 1f
  strb r2, [r0]
  add r0, r0, #1
  b memory_set
1:
  bx lr


#ifdef FULL_LIBRARY
  .bss
args:
  .space ARGSSPACE
#endif

  /* Setup attibutes of stack and heap sections so they don't take up room in the elf file */
  .section .stack, "wa", %nobits
  .section .stack_process, "wa", %nobits
  .section .heap, "wa", %nobits

