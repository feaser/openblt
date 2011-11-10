/****************************************************************************************
|  Description: bootloader core module source file
|    File Name: boot.c
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
** NAME:           BootInit
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Initializes the bootloader core.
**
****************************************************************************************/
void BootInit(void)
{
  /* initialize the watchdog */
  CopInit();
  /* initialize the backdoor entry */
  BackDoorInit();
  /* initialize the non-volatile memory driver */
  NvmInit();
  /* initialize the communication module */
  ComInit();
} /*** end of BootInit ***/


/****************************************************************************************
** NAME:           BootTask
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Task function of the bootloader core that drives the program.
**
****************************************************************************************/
void BootTask(void)
{
  /* service the watchdog */
  CopService();
  /* process possibly pending communication data */
  ComTask();
  /* control the backdoor */
  BackDoorCheck();
} /*** end of BootTask ***/


/*********************************** end of boot.c *************************************/
