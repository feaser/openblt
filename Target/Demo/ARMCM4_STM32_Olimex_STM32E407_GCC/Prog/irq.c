/****************************************************************************************
|  Description: IRQ driver source file
|    File Name: irq.c
|
|----------------------------------------------------------------------------------------
|                          C O P Y R I G H T
|----------------------------------------------------------------------------------------
|   Copyright (c) 2013  by Feaser    http://www.feaser.com    All rights reserved
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
static unsigned char interruptNesting = 0;     /* used for global interrupt en/disable */


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
  __enable_irq();
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
  if (interruptNesting == 0)
  {
    __disable_irq();
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
  interruptNesting--;
  if (interruptNesting == 0)
  {
    __enable_irq();
  }
} /*** end of IrqInterruptRestore ***/


/*********************************** end of irq.c **************************************/
