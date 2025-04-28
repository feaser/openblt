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
/** \brief Event task interface function to detect events in a polling manner. */
typedef void (* tTbxMbEventPoll)   (void        * context);


/** \brief Event processor interface function for processing events. */
typedef void (* tTbxMbEventProcess)(tTbxMbEvent * event);


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
  static tTbxList * pollerList = NULL;
  static uint8_t    pollerListInitialized = TBX_FALSE;
  const  uint16_t   defaultWaitTimeoutMs = 5000U;
  static uint16_t   waitTimeoutMS = 5000U;
  tTbxMbEvent       newEvent = { 0 };

  /* Only initialize the event poller once, */
  if (pollerListInitialized == TBX_FALSE)
  {
    pollerListInitialized = TBX_TRUE;
    /* Ceate the queue for storing context of which the pollFcn should be called. */
    pollerList = TbxListCreate();
    /* Verify that the queue creation succeeded. If this assertion fails, increase the
     * heap size using configuration macro TBX_CONF_HEAP_SIZE.
     */
    TBX_ASSERT(pollerList != NULL);
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
          /* Poller list entries are allocated from a memory pool. This means that there
           * is no need to worry about heap fragmentation. Just make sure to cap the
           * maximum number of entries to prevent heap exhaustion.
           */
          TBX_ASSERT(TbxListGetSize(pollerList) <= TBX_MB_EVENT_QUEUE_SIZE);
          /* Only continue if the current poller list size is not yet maxed out. */
          if (TbxListGetSize(pollerList) <= TBX_MB_EVENT_QUEUE_SIZE)
          {
            /* From a design perspective, it is allowed and possible that a poller list
             * entry is requested to be added, which actually is already present in the
             * poller list. There is no need to add a duplicate entry to the poller list.
             * It causes unnecessary run-time overhead by running the pollFcn multiple
             * times and, more importantly, would cause a problem further down in this
             * function, when iterating over the poller list. TbxListGetNextItem() takes
             * the list item as a reference and not the internal list node. So if you
             * have two identical list entries (let's call it item_a) and then a third
             * different list entry (item_b), the call to TbxListGetNextItem(item_a) will
             * always return return the second item_a. Consequently, iterating over the
             * list with either an if- or while-loop, will never give access to item_b. 
             * Long story short: Make sure to not add a duplcate entry into the list. 
             * The quick and easy way to do this is by first removing the item. If the
             * item is not yet in the list, attempting to remove it simply does nothing. 
             * If it was in the list, it gets removed first and then newly added to 
             * preven the duplicate entry. Exactly what you want here:
             */
            TbxListRemoveItem(pollerList, newEvent.context);
            /* Add the context at the end of the event poller list. */
            uint8_t insertResult = TbxListInsertItemBack(pollerList, newEvent.context);
            /* Check that the item could be added to the queue. If not, then the heaps size
             * is configured too small. In this case increase the heap size using
            * configuration macro TBX_CONF_HEAP_SIZE. 
            */
            TBX_ASSERT(insertResult == TBX_OK);
          }
        }
        break;
      
        case TBX_MB_EVENT_ID_STOP_POLLING:
        {
          /* Remove the context from the event poller list. */
          TbxListRemoveItem(pollerList, newEvent.context);
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
  size_t listSize = TbxListGetSize(pollerList);
  void * listItem = TbxListGetFirstItem(pollerList);
  for (size_t listItemIdx = 0U; listItemIdx < listSize; listItemIdx++)
  {
    /* Only continue with a valid list item. */
    if (listItem != NULL)
    {
      /* Convert the opaque pointer to the event context structure. */
      tTbxMbEventCtx * eventPollCtx = (tTbxMbEventCtx *)listItem;
      /* Call its poll function if configured. */
      if (eventPollCtx->pollFcn != NULL)
      {
        eventPollCtx->pollFcn(listItem);
      }
      /* Move on to the next item in the list. */
      listItem = TbxListGetNextItem(pollerList, listItem);
    }
  }

  /* Set the event wait timeout for the next call to this task function. If the event
   * poller list is not empty, keep the wait time short to make sure the poll functions
   * get continuously called. Otherwise go back to the default wait time to not hog up
   * CPU time unnecessarily.
   */
  waitTimeoutMS = (listSize > 0U) ? 1U : defaultWaitTimeoutMs;
} /*** end of TbxMbEventTask ***/


/*********************************** end of tbxmb_event.c ******************************/
