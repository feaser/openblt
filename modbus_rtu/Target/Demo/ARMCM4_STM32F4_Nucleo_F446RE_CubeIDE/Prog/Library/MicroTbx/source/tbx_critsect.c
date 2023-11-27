/************************************************************************************//**
* \file         tbx_critsect.c
* \brief        Critical sections source file.
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

/****************************************************************************************
* Include files
****************************************************************************************/
#include "microtbx.h"                            /* MicroTBX global header             */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Counter that gets incremented each time a critical section is entered and
 *         decremented each time a critical section is left.
 */
static volatile uint32_t      tbxCritSectNestingCounter = 0U;

/** \brief Holds the copy of the CPU status register from right before the interrupts
 *         were disabled. Needed to restore the interrupts enabled/disabled status upon
 *         exiting the critical section,
 */
static volatile tTbxPortCpuSR tbxCritSectCpuSR = 0U;


/************************************************************************************//**
** \brief     Enter a critical section. Critical sections are needed in an interrupt
**            driven software program to obtain mutual exclusive access shared resources
**            such as global data and certain peripherals. Note that each call to this
**            function should always be followed by a call to TbxCriticalSectionExit().
**            For example:
**              TbxCriticalSectionEnter();
**              ...access shared resource...
**              TbxCriticalSectionExit();
**
****************************************************************************************/
void TbxCriticalSectionEnter(void)
{
  tTbxPortCpuSR cpuSR;

  /* Disable the interrupts and store the CPU status register value in a local variable.
   * Note that it should not write directly to tbxCritSectCpuSR yet, because the
   * tbxCritSectCpuSR variable is a shared resource and should only be accessed with
   * interrupts disabled.
   */
  cpuSR = TbxPortInterruptsDisable();

  /* It this the first time we enter the critical section, as opposed to a nested
   * entry?
   */
  if (tbxCritSectNestingCounter == 0U)
  {
    /* Store the CPU status register value in tbxCritSectCpuSR, since it is now safe
     * to access it. It is needed to restore the interrupt status upon exiting the
     * critical section.
     */
    tbxCritSectCpuSR = cpuSR;
  }
  /* Increment the nesting counter. */
  tbxCritSectNestingCounter++;
} /*** end of TbxCriticalSectionEnter ***/


/************************************************************************************//**
** \brief     Exit a critical section. Critical sections are needed in an interrupt
**            driven software program to obtain mutual exclusive access shared resources
**            such as global data and certain peripherals. Note that each call to this
**            function should always be preceded by a call to TbxCriticalSectionEnter().
**            For example:
**              TbxCriticalSectionEnter();
**              ...access shared resource...
**              TbxCriticalSectionExit();
**
****************************************************************************************/
void TbxCriticalSectionExit(void)
{
  /* A call to this function must always be preceeded by a call to
   * TbxCriticalSectionEnter(). This means the tbxCritSectNestingCounter must be > 0.
   */
  TBX_ASSERT(tbxCritSectNestingCounter > 0U);

  /* Only continue if this function call was preceeded by a call to 
   * TbxCriticalSectionEnter().
   */
  if (tbxCritSectNestingCounter > 0U)
  {
    /* Decrement the nesting counter. */
    tbxCritSectNestingCounter--;

    /* Is this the final call meaning that it is time we exit the critical section by
     * actually restoring the interrupt status again?
     */
    if (tbxCritSectNestingCounter == 0U)
    {
      /* Restore the interrupt status to the state it was right before the interrupts
       * were all disabled upon the first time the critical section was entered.
      */
      TbxPortInterruptsRestore(tbxCritSectCpuSR);
    }
  }
} /*** end of TbxCriticalSectionExit ***/


/*********************************** end of tbx_critsect.c *****************************/
