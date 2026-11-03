/************************************************************************************//**
* \file         Source/events.c
* \brief        Bootloader events module source file.
* \ingroup      Core
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2026  by Feaser    http://www.feaser.com    All rights reserved
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


#if (BOOT_EVENTS_ENABLE > 0)
/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Holds the size of the to-be-programmed firmware. Determined by adding up how
 *         many bytes are erased. Can be used for calculating the programming progress
 *         as a percentage.
 */
static blt_int32u firmwareSize;

/** \brief Holds then number of firmware bytes that were already written. */
static blt_int32u bytesWritten;

/** \brief Boolean flag to track if an error was detected during the firmware update. */
static blt_bool errorDetected;


/***********************************************************************************//**
** \brief     Initializes the events module.
** \return    none
**
****************************************************************************************/
void EventsInit(void)
{
  /* Initialize locals. */
  firmwareSize = 0U;
  bytesWritten = 0U;
  errorDetected = BLT_FALSE;
} /*** end of EventsInit ***/


/***********************************************************************************//**
** \brief     Processes the occurrence of a new event. Should only be called from the
**            bootloader core. Cast the opaque info pointer to the correct structure
**            type (tEventsInfoXxx) to access the event related information.
** \param     id The identifier of the event that occurred.
** \param     info Opaque pointer to event identifier related information. Can be 
**            BLT_NULL depending on the event identifer. For example when no additional
**            information is needed for processing the event.
** \return    none
**
****************************************************************************************/
void EventsProcess(tEventsId id, void * info)
{
  blt_bool  invokeHookFct = BLT_TRUE;
  blt_int8u progress;

  /* Filter on the events identifier for those events that need additional processing
   * before invoking the hook-function.
   */
  switch (id)
  {
    case EVENT_ID_ON_WRITE:
      ASSERT_RT(info != BLT_NULL);
      /* Calculate and store the write progress as a percentage. Use integer rounding,
       * yet protect against divide by zero or other unexpected values.
       */
      progress = 0U;
      if ( (firmwareSize > 0U) && (bytesWritten <= firmwareSize) )
      {
        progress = (blt_int8u)(((bytesWritten*100U) + (firmwareSize/2U)) / firmwareSize);
      }
      ((tEventsInfoWrite *)info)->progress = progress;
      /* Increment the firmware bytes that will afterwards be written. */
      bytesWritten += ((tEventsInfoWrite const *)info)->num_bytes;
      break;

    case EVENT_ID_ON_ERASE:
      ASSERT_RT(info != BLT_NULL);
      /* Increment the firmware size. */
      firmwareSize += ((tEventsInfoErase const *)info)->num_bytes;
      break;

    case EVENT_ID_ON_START:
      /* Reset the firmware size, bytes written, and error flag. */
      firmwareSize = 0U;
      bytesWritten = 0U;
      errorDetected = BLT_FALSE;
      break;

    case EVENT_ID_ON_SUCCESS:
      /* Only pass this event on if all data was actually written and no error was
       * detected
       */
      if ( (errorDetected == BLT_TRUE)    || 
           (bytesWritten != firmwareSize) || 
           (bytesWritten == 0U) )
      {
        invokeHookFct = BLT_FALSE;
      }           
      break;

    case EVENT_ID_ON_ERROR:
      /* Set the flag to remember that an error was encountered. */
      errorDetected = BLT_TRUE;
      break;

    default:
      break;  
  }
  
  /* Okay to invoke the hook function? */
  if (invokeHookFct == BLT_TRUE)
  {
    /* Pass the event on to the application for further processing. */
    EventsHook(id, info);
  }
#if (ADDON_ROLLBACK_MOD_ENABLE > 0)
  if (id == EVENT_ID_ON_ERROR)
  {
    /* Inform the rollback module about the error event. */
    RollbackProcessError(((tEventsInfoError const *)info)->error_id);
  }
#endif
} /*** end of EventsProcess ***/
#endif /* BOOT_EVENTS_ENABLE > 0 */


/*********************************** end of events.c ***********************************/
