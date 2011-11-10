/****************************************************************************************
|  Description: IRQ driver source file
|    File Name: irq.c
|
|----------------------------------------------------------------------------------------
|                          C O P Y R I G H T
|----------------------------------------------------------------------------------------
|   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
|
|----------------------------------------------------------------------------------------
|                            L I C E N S E
|----------------------------------------------------------------------------------------
| This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
| modify it under the terms of the GNU General Public License as published by the Free
| Software Foundation, either version 3 of the License, or (at your option) any later
| version.
|
| OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
| PURPOSE. See the GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License along with OpenBLT.
| If not, see <http://www.gnu.org/licenses/>.
|
| A special exception to the GPL is included to allow you to distribute a combined work 
| that includes OpenBLT without being obliged to provide the source code for any 
| proprietary components. The exception text is included at the bottom of the license
| file <license.html>.
| 
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "header.h"                                    /* generic header               */


/****************************************************************************************
* Local data definitions
****************************************************************************************/
static unsigned long oldInterruptStatus;       /* used for global interrupt en/disable */
static unsigned char interruptNesting = 0;     /* used for global interrupt en/disable */


/****************************************************************************************
** NAME:           IrqGetCPSR
** PARAMETER:      none
** RETURN VALUE:   CPSR value
** DESCRIPTION:    Obtains current value of CPSR CPU register.
** NOTE:           Derived from a sample by R O Software that is Copyright 2004,
**                 R O SoftWare, and can be used for hobby or commercial purposes.
**
****************************************************************************************/
static unsigned long IrqGetCPSR(void)
{
  unsigned long retval;
  asm volatile (" mrs  %0, cpsr" : "=r" (retval) : /* no inputs */  );
  return retval;
} /*** end of IrqGetCPSR ***/


/****************************************************************************************
** NAME:           IrqSetCPSR
** PARAMETER:      CPSR value
** RETURN VALUE:   none
** DESCRIPTION:    Update value of CPSR CPU register.
** NOTE:           Derived from a sample by R O Software that is Copyright 2004,
**                 R O SoftWare, and can be used for hobby or commercial purposes.
**
****************************************************************************************/
static void IrqSetCPSR(unsigned long val)
{
  asm volatile (" msr  cpsr, %0" : /* no outputs */ : "r" (val)  );
} /*** end of IrqSetCPSR ***/


/****************************************************************************************
** NAME:           IrqInterruptEnable
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Enables the generation IRQ interrupts. Typically called once during
**                 software startup after completion of the initialization.
**
****************************************************************************************/
void IrqInterruptEnable(void)
{
  unsigned _cpsr;

  _cpsr = IrqGetCPSR();
  IrqSetCPSR(_cpsr & ~0x00000080);
} /*** end of IrqInterruptEnable ***/


/****************************************************************************************
** NAME:           HwInterruptDisable
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Disables the generation IRQ interrupts and stores information on
**                 whether or not the interrupts were already disabled before explicitly
**                 disabling them with this function. Normally used as a pair together
**                 with IrqInterruptRestore during a critical section.
**
****************************************************************************************/
void IrqInterruptDisable(void)
{
  unsigned long _cpsr;

  if (interruptNesting == 0)
  {
    _cpsr = IrqGetCPSR();
    IrqSetCPSR(_cpsr | 0x00000080);
    oldInterruptStatus = _cpsr;
  }
  interruptNesting++;
} /*** end of IrqInterruptDisable ***/


/****************************************************************************************
** NAME:           IrqInterruptRestore
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Restore the generation IRQ interrupts to the setting it had prior to
**                 calling IrqInterruptDisable. Normally used as a pair together with
**                 IrqInterruptDisable during a critical section.
**
****************************************************************************************/
void IrqInterruptRestore(void)
{
  unsigned _cpsr;

  interruptNesting--;
  if (interruptNesting == 0)
  {
    _cpsr = IrqGetCPSR();
    IrqSetCPSR((_cpsr & ~0x00000080) | (oldInterruptStatus & 0x00000080));
  }
} /*** end of IrqInterruptRestore ***/


/*********************************** end of irq.c **************************************/
