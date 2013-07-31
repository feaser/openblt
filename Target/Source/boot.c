/************************************************************************************//**
* \file         Source\boot.c
* \brief        Bootloader core module source file.
* \ingroup      Core
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 3 of the License, or (at your option) any later
* version.
*
* OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE. See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with OpenBLT.
* If not, see <http://www.gnu.org/licenses/>.
*
* A special exception to the GPL is included to allow you to distribute a combined work 
* that includes OpenBLT without being obliged to provide the source code for any 
* proprietary components. The exception text is included at the bottom of the license
* file <license.html>.
* 
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */


/************************************************************************************//**
** \brief     Initializes the bootloader core. 
** \return    none
**
****************************************************************************************/
void BootInit(void)
{
  /* initialize the watchdog */
  CopInit();
  /* initialize the millisecond timer */
  TimerInit();
  /* initialize the non-volatile memory driver */
  NvmInit();
  #if (BOOT_FILE_SYS_ENABLE > 0)
  /* initialize the file system module */
  FileInit();
  #endif 
  #if (BOOT_COM_ENABLE > 0)
  /* initialize the communication module */
  ComInit();
  #endif
  /* initialize the backdoor entry */
  BackDoorInit();
} /*** end of BootInit ***/


/************************************************************************************//**
** \brief     Task function of the bootloader core that drives the program. 
** \return    none
**
****************************************************************************************/
void BootTask(void)
{
  /* service the watchdog */
  CopService();
  /* update the millisecond timer */
  TimerUpdate();
  #if (BOOT_FILE_SYS_ENABLE > 0)
  /* call worker task for updating firmware from locally attached file storage */
  FileTask();
  #endif /* BOOT_FILE_SYS_ENABLE > 0 */
  #if (BOOT_COM_ENABLE > 0)
  /* process possibly pending communication data */
  ComTask();
  #endif
  /* control the backdoor */
  BackDoorCheck();
} /*** end of BootTask ***/


/*********************************** end of boot.c *************************************/
