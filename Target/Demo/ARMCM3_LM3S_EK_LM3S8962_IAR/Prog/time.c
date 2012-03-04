/****************************************************************************************
|  Description: Timer driver source file
|    File Name: time.c
|
|----------------------------------------------------------------------------------------
|                          C O P Y R I G H T
|----------------------------------------------------------------------------------------
|   Copyright (c) 2012  by Feaser    http://www.feaser.com    All rights reserved
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
** NAME:           TimeInit
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Initializes the timer.
**
****************************************************************************************/
void TimeInit(void)
{
  /* configure the SysTick timer for 1 ms period */
  SysTickPeriodSet((unsigned long)SysCtlClockGet() / 1000);
  SysTickEnable();
  SysTickIntEnable();
  /* reset the millisecond counter */
  TimeSet(0);
} /*** end of TimeInit ***/


/****************************************************************************************
** NAME:           TimeDeinit
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Stops and disables the timer.
**
****************************************************************************************/
void TimeDeinit(void)
{
  SysTickIntDisable();
  SysTickDisable();
} /*** end of TimeDeinit ***/


/****************************************************************************************
** NAME:           TimeSet
** PARAMETER:      timer_value initialize value of the millisecond timer.
** RETURN VALUE:   none
** DESCRIPTION:    Sets the initial counter value of the millisecond timer.
**
****************************************************************************************/
void TimeSet(unsigned long timer_value)
{
  /* set the millisecond counter */
  millisecond_counter = timer_value;
} /*** end of TimeSet ***/


/****************************************************************************************
** NAME:           TimeGet
** PARAMETER:      none
** RETURN VALUE:   current value of the millisecond timer
** DESCRIPTION:    Obtains the counter value of the millisecond timer.
**
****************************************************************************************/
unsigned long TimeGet(void)
{
  /* read and return the millisecond counter value */
  return millisecond_counter;
} /*** end of TimeGet ***/


/****************************************************************************************
** NAME:           TimeISRHandler
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Interrupt service routine of the timer.
**
****************************************************************************************/
void TimeISRHandler(void)
{
  /* increment the millisecond counter */
  millisecond_counter++;
} /*** end of TimeISRHandler ***/


/*********************************** end of time.c *************************************/
