/****************************************************************************************
|  Description: demo program interrupt vectors source file
|    File Name: vectors.c
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
* Function prototypes
****************************************************************************************/
void __attribute__ ((interrupt("IRQ")))   TIMER0_ISR(void);
void __attribute__ ((interrupt("SWI")))   SWI_ISR(void);
void __attribute__ ((interrupt("FIQ")))   FIQ_ISR(void);
void __attribute__ ((interrupt("UNDEF"))) UNDEF_ISR(void);


/****************************************************************************************
** NAME:           TIMER0_ISR
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Timer0 exception routine.
**
****************************************************************************************/
void TIMER0_ISR(void)
{
  /* clear the interrupt flag */
  T0IR = 0x01;
  /* acknowledge interrupt */
  VICVectAddr = 0;
  /* process time tick */
  TimerUpdate();
} /*** end of TIMER0_ISR ***/


/****************************************************************************************
** NAME:           SWI_ISR
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    SWI exception routine.
**
****************************************************************************************/
void SWI_ISR(void)
{
  /* unexpected interrupt so halt program */
  for (;;) { ; }
} /*** end of SWI_ISR ***/


/****************************************************************************************
** NAME:           FIQ_ISR
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    FIQ exception routine.
**
****************************************************************************************/
void FIQ_ISR(void)
{
  /* unexpected interrupt so halt program */
  for (;;) { ; }
} /*** end of FIQ_ISR ***/


/****************************************************************************************
** NAME:           UNDEF_ISR
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    UNDEF exception routine.
**
****************************************************************************************/
void UNDEF_ISR(void)
{
  /* unexpected interrupt so halt program */
  for (;;) { ; }
} /*** end of UNDEF_ISR ***/


/*********************************** end of vectors.c **********************************/
