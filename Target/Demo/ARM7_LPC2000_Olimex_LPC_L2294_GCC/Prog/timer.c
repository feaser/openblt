/****************************************************************************************
|  Description: Timer driver source file
|    File Name: timer.c
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
* Local data declarations
****************************************************************************************/
static unsigned long millisecond_counter;


/****************************************************************************************
* External functions
****************************************************************************************/
extern void TIMER0_ISR(void);


/****************************************************************************************
** NAME:           TimerInit
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Initializes the timer.
**
****************************************************************************************/
void TimerInit(void)
{
  /* configure timer0 as 1 ms software output compare */
  T0MR0 = BOOT_CPU_SYSTEM_SPEED_KHZ-1;
  /* enable interrupt and automatic reset upon compare */
  T0MCR = 0x01 | 0x02;
  /* enable the output compare */
  T0TCR = 0x01;
  /* set the interrupt service routine for the output compare event */
  VICVectAddr0 = (unsigned long)TIMER0_ISR;
  /* connect vectored IRQ slot 0 to Timer0's channel 4 */
  VICVectCntl0 = 0x20 | 4;
  /* enable the timer0 interrupt */
  VICIntEnable = 0x10;
  /* reset the millisecond counter */
  TimerSet(0);
} /*** end of TimerInit ***/


/****************************************************************************************
** NAME:           TimerUpdate
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Updates the millisecond timer. Should be called every millisecond by
**                 the timer interrupt service routine.
**
****************************************************************************************/
void TimerUpdate(void)
{
  /* increment the millisecond counter */
  millisecond_counter++;
} /*** end of TimerUpdate ***/


/****************************************************************************************
** NAME:           TimerSet
** PARAMETER:      timer_value initialize value of the millisecond timer.
** RETURN VALUE:   none
** DESCRIPTION:    Sets the initial counter value of the millisecond timer.
**
****************************************************************************************/
void TimerSet(unsigned long timer_value)
{
  /* set the millisecond counter */
  millisecond_counter = timer_value;
} /*** end of TimerSet ***/


/****************************************************************************************
** NAME:           TimerGet
** PARAMETER:      none
** RETURN VALUE:   current value of the millisecond timer
** DESCRIPTION:    Obtains the counter value of the millisecond timer.
**
****************************************************************************************/
unsigned long TimerGet(void)
{
  /* read and return the millisecond counter value */
  return millisecond_counter;
} /*** end of TimerGet ***/


/*********************************** end of timer.c ************************************/
