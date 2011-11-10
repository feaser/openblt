/****************************************************************************************
|  Description: bootloader timer driver source file
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
#include "boot.h"                                /* bootloader generic header          */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
static blt_int32u free_running_counter_last;
static blt_int32u millisecond_counter;


/****************************************************************************************
* Register definitions
****************************************************************************************/
#define T0MCR           (*((volatile blt_int32u *) 0xE0004014))
#define T0CCR           (*((volatile blt_int32u *) 0xE0004028))
#define T0PR            (*((volatile blt_int32u *) 0xE000400C))
#define T0PC            (*((volatile blt_int32u *) 0xE0004010))
#define T0TCR           (*((volatile blt_int32u *) 0xE0004004))
#define T0TC            (*((volatile blt_int32u *) 0xE0004008))


/****************************************************************************************
** NAME:           TimerInit
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Initializes the polling based millisecond timer driver.
**
****************************************************************************************/
void TimerInit(void)
{
  /* disable timer 0 interrupts */
  T0MCR = 0x00;
  T0CCR = 0x00;
  /* set prescaler so that free running counter runs at 1 kHz */
  T0PR = BOOT_CPU_SYSTEM_SPEED_KHZ - 1;
  /* start free running counter for timer 0 */
  T0TCR = 0x01;
  /* store the start value of the free running counter */
  free_running_counter_last = T0TC;
  /* reset the millisecond counter value */
  TimerSet(0);
} /*** end of TimerInit ***/


/****************************************************************************************
** NAME:           TimerReset
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Reset the timer by placing the timer back into it's default reset
**                 configuration.
**
****************************************************************************************/
void TimerReset(void)
{
  /* reset the timer */
  T0TCR = 0;
  /* reset the prescaler */
  T0PR = 0;
  /* reset the timer counter */
  T0TC = 0;
  /* reset the prescaled counter */
  T0PC = 0;
} /* TimerReset */


/****************************************************************************************
** NAME:           TimerUpdate
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Updates the millisecond timer.
**
****************************************************************************************/
void TimerUpdate(void)
{
  blt_int32u free_running_counter_now;

  /* update the millisecond counter */
  free_running_counter_now = T0TC;
  millisecond_counter += (free_running_counter_now - free_running_counter_last);
  /* store current free running counter value for next time around */
  free_running_counter_last = free_running_counter_now;
} /*** end of TimerUpdate ***/


/****************************************************************************************
** NAME:           TimerSet
** PARAMETER:      timer_value initialize value of the millisecond timer.
** RETURN VALUE:   none
** DESCRIPTION:    Sets the initial counter value of the millisecond timer.
**
****************************************************************************************/
void TimerSet(blt_int32u timer_value)
{
  /* set the millisecond counter value */
  millisecond_counter = timer_value;
} /*** end of TimerSet ***/


/****************************************************************************************
** NAME:           TimerGet
** PARAMETER:      none
** RETURN VALUE:   current value of the millisecond timer
** DESCRIPTION:    Obtains the counter value of the millisecond timer.
**
****************************************************************************************/
blt_int32u TimerGet(void)
{
  /* updating timer here allows this function to be called in a loop with timeout
   * detection.
   */
  TimerUpdate();
  
  /* read and return the amount of milliseconds that passed since initialization */
  return millisecond_counter;
} /*** end of TimerGet ***/


/*********************************** end of timer.c ************************************/
