/************************************************************************************//**
* \file         tbxmb_event.c
* \brief        Modbus event handler source file.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2023 by Feaser     www.feaser.com     All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* This file is part of MicroTBX-Modbus. MicroTBX-Modbus is free software: you can
* redistribute it and/or modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* MicroTBX-Modbus is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
* PARTICULAR PURPOSE. See the GNU General Public License for more details.
*
* You have received a copy of the GNU General Public License along with MicroTBX-Modbus.
* If not, see www.gnu.org/licenses/.
*
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "microtbx.h"                            /* MicroTBX module                    */
#include "microtbxmodbus.h"                      /* MicroTBX-Modbus module             */
#include "tbxmb_event_private.h"                 /* MicroTBX-Modbus event private      */
#include "tbxmb_osal_private.h"                  /* MicroTBX-Modbus OSAL private       */


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Minimal context for accessing the event poll and process functions. Think of
 *         it as the base type for all the other context (client/server/tp). That's the
 *         reason why these other context start with similar entries at exactly the same
 *         location. 
 */
typedef struct
{
  /* The following three entries must always be at the start and not change order. They
   * form the base that other context derive from.
   */
  void               * instancePtr;              /**< Reserved for C++ wrapper.        */
  tTbxMbEventPoll      pollFcn;                  /**< Event poll function.             */
  tTbxMbEventProcess   processFcn;               /**< Event process function.          */
} tTbxMbEventCtx;


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Array with context for which its poll function should be called during the
 *         event task.
 */
static tTbxMbEventCtx * pollerList[TBX_MB_EVENT_QUEUE_SIZE];

/** \brief Flag to track if the pollerList has been initialized or not. */
static uint8_t          pollerListInitialized = TBX_FALSE;


/************************************************************************************//**
** \brief     Task function that drives the entire Modbus stack. It processes internally
**            generated events. 
** \details   How to call this function depends on the selected operating system
**            abstraction layer (OSAL):
**            - In a traditional superloop application (tbxmb_superloop.c), call this
**              function continuously in the infinite program loop.
**            - When using an RTOS (e.g. tbxmb_freertos.c), create a new task during
**              application initialization and call this function from this task's 
**              infinite loop.
**
**            When using a traditional superloop application in combination with just
**            a Modbus client, you can omit the call to this task function. With this
**            combination, the communication with a Modbus server happens in a blocking
**            manner and the event task is automatically called internally, while
**            blocking. Convenient and easy, but not optimal from a run-time performance.
**            For this reason it is recommended to use an RTOS in combination with a
**            Modbus client.
**
****************************************************************************************/
void TbxMbEventTask(void)
{
  const  uint16_t  defaultWaitTimeoutMs = 5000U;
  static uint16_t  waitTimeoutMS = 5000U;
  tTbxMbEvent      newEvent = { 0 };

  /* Make sure the OSAL event module is initialized, just in case the application already
   * calls this task function, because it created a transport layer object.
   */
  TbxMbOsalEventInit();

  /* Only initialize the poller list once, */
  if (pollerListInitialized == TBX_FALSE)
  {
    /* Set flag not that the initialization is performed. */
    pollerListInitialized = TBX_TRUE;
    /* Reset all array entries to a NULL value. */
    for (size_t listIdx = 0U; listIdx < TBX_MB_EVENT_QUEUE_SIZE; listIdx++)
    {
      pollerList[listIdx] = NULL;
    }
  }

  /* Wait for a new event to be posted to the event queue. Note that that wait time only
   * applies in case an RTOS is configured for the OSAL. Otherwise (TBX_MB_OPT_OSAL_NONE)
   * this function returns immediately.
   */
  if (TbxMbOsalEventWait(&newEvent, waitTimeoutMS) == TBX_TRUE)
  {
    /* Check the opaque context pointer. */
    TBX_ASSERT(newEvent.context != NULL);
    /* Only continue with a valid opaque context pointer. */
    if (newEvent.context != NULL)
    {
      /* Filter on the event identifier. */
      switch (newEvent.id)
      {
        case TBX_MB_EVENT_ID_START_POLLING:
        {
          /* Obtain mutual exclusive access to the poller list. */
          TbxCriticalSectionEnter();
          /* Loop through the array to find a free spot. */
          uint8_t freeIdxFound = TBX_FALSE;
          for (size_t listIdx = 0U; listIdx < TBX_MB_EVENT_QUEUE_SIZE; listIdx++)
          {
            /* Is this index free? */
            if (pollerList[listIdx] == NULL)
            {
              /* Store the context at this index. */
              pollerList[listIdx] = newEvent.context;
              /* Set flag that we found a free index. */
              freeIdxFound = TBX_TRUE;
              /* No need to continue the loop. */
              break;
            }
          }
          /* Release mutual exclusive access to the poller list. */
          TbxCriticalSectionExit();
          /* Check that a free spot was found. If not, then TBX_MB_EVENT_QUEUE_SIZE is
           * configured too small.
           */
          TBX_ASSERT(freeIdxFound == TBX_TRUE);
        }
        break;
      
        case TBX_MB_EVENT_ID_STOP_POLLING:
        {
          /* Obtain mutual exclusive access to the poller list. */
          TbxCriticalSectionEnter();
          /* Loop through the array to find the spot with the same context. */
          for (size_t listIdx = 0U; listIdx < TBX_MB_EVENT_QUEUE_SIZE; listIdx++)
          {
            /* Is this the one we are looking for? */
            if (pollerList[listIdx] == newEvent.context)
            {
              /* Remove it from the array by setting its value back to NULL. */
              pollerList[listIdx] = NULL;
              /* No need to continue the loop. */
              break;
            }
          }
          /* Release mutual exclusive access to the poller list. */
          TbxCriticalSectionExit();
        }
        break;

        default:
        {
          /* Convert the opaque pointer to the event context structure. */
          tTbxMbEventCtx * eventCtx = (tTbxMbEventCtx *)newEvent.context;
          /* Pass the event on to the context's event processor. */
          if (eventCtx->processFcn != NULL)
          {
            eventCtx->processFcn(&newEvent);
          }
        }
        break;
      }
    }
  }

  /* Iterate over the event poller list. */
  size_t numPollerEntries = 0U;
  for (size_t listIdx = 0U; listIdx < TBX_MB_EVENT_QUEUE_SIZE; listIdx++)
  {
    /* Obtain mutual exclusive access to the poller list. */
    TbxCriticalSectionEnter();
    /* Create local copy of the array entry at this index. */
    tTbxMbEventCtx * eventPollCtx = pollerList[listIdx];
    /* Release mutual exclusive access to the poller list. */
    TbxCriticalSectionExit();
    /* Valid event context? */
    if (eventPollCtx != NULL)
    {
      /* Call its poll function if configured. */
      if (eventPollCtx->pollFcn != NULL)
      {
        eventPollCtx->pollFcn(eventPollCtx);
      }
      /* Increment number of detected event contexts in the pollier list. */
      numPollerEntries++;
    }
  }

  /* Set the event wait timeout for the next call to this task function. If the event
   * poller list is not empty, keep the wait time short to make sure the poll functions
   * get continuously called. Otherwise go back to the default wait time to not hog up
   * CPU time unnecessarily.
   */
  waitTimeoutMS = (numPollerEntries > 0U) ? 1U : defaultWaitTimeoutMs;
} /*** end of TbxMbEventTask ***/


/************************************************************************************//**
** \brief     Function that removes all entries from the event queue and the pollerlist,
**            which are related to the specified context. This function should be called
**            when free-ing a channel or transport layer.
** \param     context The context of the channel or transport layer, for which the event
**            queue and pollerlist should be purged.
**
****************************************************************************************/
void TbxMbEventPurge(void const * context)
{
  /* Verify parameters. */
  TBX_ASSERT(context != NULL);

  /* Only continue with valid parameters. */
  if (context != NULL)
  {
    /* Purge events from this context from the event queue. */
    TbxMbOsalEventPurge(context);

    /* Only need to purge the entries for this context from the poller list, it the list
     * was actually initialized.
     */
    if (pollerListInitialized == TBX_TRUE)
    {
      /* Obtain mutual exclusive access to the poller list. */
      TbxCriticalSectionEnter();
      /* Loop through the array to locate entires that need to be purged. */
      for (size_t listIdx = 0U; listIdx < TBX_MB_EVENT_QUEUE_SIZE; listIdx++)
      {
        /* Does this entrie equel the context to purge? */
        if (pollerList[listIdx] == context)
        {
          /* Remove it from the array by setting its value back to NULL. */
          pollerList[listIdx] = NULL;
        }
      }
      /* Release mutual exclusive access to the poller list. */
      TbxCriticalSectionExit();
    }
  }
} /*** end of TbxMbEventPurge ***/


/*********************************** end of tbxmb_event.c ******************************/
