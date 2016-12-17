/************************************************************************************//**
* \file         Source\backdoor.c
* \brief        Bootloader backdoor entry source file.
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
* You have received a copy of the GNU General Public License along with OpenBLT. It
* should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
*
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
#if (BOOT_BACKDOOR_HOOKS_ENABLE == 0)
#ifndef BOOT_BACKDOOR_ENTRY_TIMEOUT_MS
/** \brief Sets the time in milliseconds that the backdoor is open, but allow an
 *         override for this time. note that this time should be at least 2.5 times
 *         as long as the time that is configured in Microboot's XCP settings for the
 *         connect command response. This is the last entry on XCP Timeouts tab. By
 *         default the connect command response is configured as 20ms by Microboot,
 *         except for TCP/IP where it is 300ms due to accomodate for worldwide
 *         network latency. The default value was chosen safely for compatibility
 *         reasons with all supported communication interfaces. It could be made
 *         shorter your bootloader. To change this value, simply add the macro
 *         BOOT_BACKDOOR_ENTRY_TIMEOUT_MS to blt_conf.h with your desired backdoor 
 *         open time in milliseconds.
 */
#if (BOOT_COM_NET_ENABLE == 1)
#define BOOT_BACKDOOR_ENTRY_TIMEOUT_MS  (750)
#else
#define BOOT_BACKDOOR_ENTRY_TIMEOUT_MS  (500)
#endif
#endif
#endif

/****************************************************************************************
* Hook functions
****************************************************************************************/
#if (BOOT_BACKDOOR_HOOKS_ENABLE > 0)
extern void BackDoorInitHook(void);
extern blt_bool BackDoorEntryHook(void);
#endif


/****************************************************************************************
* Local data declarations
****************************************************************************************/
#if (BOOT_BACKDOOR_HOOKS_ENABLE == 0)
/** \brief To determine if the backdoor is open or closed. */
static blt_bool backdoorOpen;
/** \brief To determine how long the backdoor has been open in milliseconds. */
static blt_int32u backdoorOpenTime;
#endif


/************************************************************************************//**
** \brief     Initializes the backdoor entry option.
** \return    none
**
****************************************************************************************/
void BackDoorInit(void)
{
#if (BOOT_BACKDOOR_HOOKS_ENABLE > 0)
  /* initialize application's backdoor functionality */
  BackDoorInitHook();

  /* attempt to start the user program when no backdoor entry is requested */
  if (BackDoorEntryHook() == BLT_FALSE)
  {
    /* this function does not return if a valid user program is present */
    CpuStartUserProgram();
  }
#if (BOOT_FILE_SYS_ENABLE > 0)
  else
  {
    /* the backdoor is open so we should check if a update from locally  attached storage
     * is requested and, if so, start it.
     */
    FileHandleFirmwareUpdateRequest();
  }
#endif
#else
  /* open the backdoor after a reset */
  backdoorOpen = BLT_TRUE;
  backdoorOpenTime = TimerGet();
#endif
  /* perform the first check that open/closes the backdoor */
  BackDoorCheck();
} /*** end of BackDoorInit ***/


/************************************************************************************//**
** \brief     The default backdoor entry feature keeps the bootloader active for a
**            predetermined time after reset, allowing the host application to
**            establish a connection and start a programming sequence. This function
**            controls the opening/closing of the backdoor.
** \return    none
**
****************************************************************************************/
void BackDoorCheck(void)
{
#if (BOOT_BACKDOOR_HOOKS_ENABLE == 0)
#if (BOOT_COM_ENABLE > 0)
  /* check if a connection with the host was already established. in this case the
   * backdoor stays open anyway, so no need to check if it needs to be closed.
   */
  if (ComIsConnected() == BLT_TRUE)
  {
    return;
  }
#endif
#if (BOOT_FILE_SYS_ENABLE > 0)
  /* check if the file module is busy, indicating that a firmware update through the
   * locally attached storage is in progress. in this case the backdoor stays open
   * anyway, so no need to check if it needs to be closed.
   */
  if (FileIsIdle() == BLT_FALSE)
  {
    return;
  }
#endif

  /* when the backdoor is still open, check if it's time to close it */
  if (backdoorOpen == BLT_TRUE)
  {
    /* check if the backdoor entry time window elapsed */
    if (TimerGet() >= (BOOT_BACKDOOR_ENTRY_TIMEOUT_MS + backdoorOpenTime))
    {
      /* close the backdoor */
      backdoorOpen = BLT_FALSE;
#if (BOOT_FILE_SYS_ENABLE > 0)
      /* during the timed backdoor no remote update request was detected. now do one
       * last check to see if a firmware update from locally attached storage is
       * pending.
       */
      if (FileHandleFirmwareUpdateRequest() == BLT_FALSE)
#endif
      {
        /* no firmware update requests detected, so attempt to start the user program.
         * this function does not return if a valid user program is present.
         */
        CpuStartUserProgram();
      }
    }
  }
#endif
} /*** end of BackDoorCheck ***/


/*********************************** end of backdoor.c *********************************/
