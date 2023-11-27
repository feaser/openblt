/************************************************************************************//**
* \file         port/ARM_CORTEXM/GCC/tbx_comp.s
* \brief        GCC compiler specifics of the port source file.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2019 by Feaser     www.feaser.com     All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
*
* SPDX-License-Identifier: MIT
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* \endinternal
****************************************************************************************/
  .syntax unified
  .thumb


.global TbxPortInterruptsDisable
.global TbxPortInterruptsRestore


/************************************************************************************//**
** \brief     Stores the current state of the CPU status register and then disables the
**            generation of global interrupts. The status register contains information
**            about the interrupts being disable/enabled before they get disabled. This
**            is needed to later on restore the state. 
**            Prototype: 
**              tTbxPortCpuSR TbxPortInterruptsDisable(void);
** \return    The current value of the CPU status register.
**
****************************************************************************************/
  .section .text,"ax"
  .weak TbxPortInterruptsDisable
  .type TbxPortInterruptsDisable, %function
TbxPortInterruptsDisable:
  /* Store state of the currently enabled/disabled interrupts in register 0. On the 
   * ARM Cortex this is stored in CPU register PRIMASK. Note that register 0 is used by
   * GCC for storing the return value of a function.
   */
  mrs   r0, primask
  /* Disable the interrupts by setting all bits in the CPU register PRIMASK to a value
   * of 1, which means the actication of the exception is prevented.
   */
  cpsid i
  /* Return from this function by branching back to the location stored in the link
   * register.
   */
  bx    lr


/************************************************************************************//**
** \brief     Restores the interrupts enabled/disabled state to the state it was when
**            function TbxPortInterruptsDisable() was previously called. It does this
**            by writing the value of the CPU status register that was returned by
**            TbxPortInterruptsDisable(). 
**            Prototype: 
**              void TbxPortInterruptsRestore(tTbxPortCpuSR prevCpuSr);
** \param     prevCpuSr The previous value of the CPU status register from right before
**            the interrupts where disabled. This value is returned by function
**            TbxPortInterruptsDisable().
**
****************************************************************************************/
  .section .text,"ax"
  .weak TbxPortInterruptsRestore
  .type TbxPortInterruptsRestore, %function
TbxPortInterruptsRestore:
  /* Restore the state of the interrupts by storing the value of register 0 in the CPU
   * register PRIMASK. Note that register 0 is used by GCC for passing the first function
   * parameter.
   */
  msr   primask, r0
  /* Return from this function by branching back to the location stored in the link
   * register.
   */
  bx    lr


/*********************************** end of tbx_comp.s *********************************/

