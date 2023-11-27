/************************************************************************************//**
* \file         tbxmb_superloop.c
* \brief        Modbus OSAL implementation for a traditional superloop.
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
* Macro definitions
****************************************************************************************/
/** \brief Unique context type to identify a context as being a semaphore. */
#define TBX_MB_OSAL_SEM_CONTEXT_TYPE   (76U)


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Data type that groups semaphore related information. It's what the 
 *         tTbxMbOsalSem opaque pointer points to.
 */
typedef struct
{
  uint8_t type;                        /**< Context type.                              */
  uint8_t count;                       /**< Semaphore count. 0 = taken, 1 = available. */
} tTbxMbOsalSemCtx;


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Ring buffer based First-In-First-Out (FIFO) queue for storing events. */
static struct 
{
  tTbxMbEvent entries[TBX_MB_EVENT_QUEUE_SIZE];       /**< Preallocated event storage. */
  uint16_t    count;                                  /**< Number of stored entries.   */
  uint16_t    readIdx;                                /**< Read index into entries[].  */
  uint16_t    writeIdx;                               /**< Write index into entries[]. */
} eventQueue;


/************************************************************************************//**
** \brief     Initialization function for the OSAL module. 
** \attention This function has a built-in protection to make sure it only runs once.
**
****************************************************************************************/
void TbxMbOsalEventInit(void)
{
  static uint8_t osalInitialized = TBX_FALSE;

  /* Only run this function once, */
  if (osalInitialized == TBX_FALSE)
  {
    osalInitialized = TBX_TRUE;
    /* Initialize the queue. */
    eventQueue.count = 0U;
    eventQueue.readIdx = 0U;
    eventQueue.writeIdx = 0U;
  }
} /*** end of TbxMbOsalEventInit ***/


/************************************************************************************//**
** \brief     Signals the occurrence of an event.
** \param     event Pointer to the event to signal.
** \param     fromIsr TBX_TRUE when calling this function from an interrupt service
**            routine, TBX_FALSE otherwise.
**
****************************************************************************************/
void TbxMbOsalEventPost(tTbxMbEvent const * event, 
                        uint8_t             fromIsr)
{
  TBX_UNUSED_ARG(fromIsr);

  /* Verify parameters. */
  TBX_ASSERT(event != NULL);

  /* Only continue with valid parameters. */
  if (event != NULL)
  {
    TbxCriticalSectionEnter();
    /* Make sure there is still space in the queue. If not, then the event queue size is
     * set too small. In this case increase the event queue size using configuration
     * macro TBX_MB_EVENT_QUEUE_SIZE.
     */
    TBX_ASSERT(eventQueue.count < TBX_MB_EVENT_QUEUE_SIZE);

    /* Only continue with enough space. */
    if (eventQueue.count < TBX_MB_EVENT_QUEUE_SIZE)
    {
      /* Store the new event in the queue at the current write index. */
      eventQueue.entries[eventQueue.writeIdx] = *event;
      /* Update the total count. */
      eventQueue.count++;
      /* Increment the write index to point to the next entry. */
      eventQueue.writeIdx++;
      /* Time to wrap around to the start? */
      if (eventQueue.writeIdx == TBX_MB_EVENT_QUEUE_SIZE)
      {
        eventQueue.writeIdx = 0U;
      }
    }
    TbxCriticalSectionExit();
  }
} /*** end of TbxMbOsalEventPost ***/


/************************************************************************************//**
** \brief     Wait for an event to occur.
** \param     event Pointer where the occurred event is written to.
** \param     timeoutMs Maximum time in milliseconds to block while waiting for an
**            event.
** \return    TBX_TRUE if an event occurred, TBX_FALSE otherwise (typically a timeout).
**
****************************************************************************************/
uint8_t TbxMbOsalEventWait(tTbxMbEvent * event,
                           uint16_t      timeoutMs)
{
  uint8_t result = TBX_FALSE;

  TBX_UNUSED_ARG(timeoutMs);

  /* Verify parameters. */
  TBX_ASSERT(event != NULL);

  /* Only continue with valid parameters. */
  if (event != NULL)
  {
    TbxCriticalSectionEnter();
    /* Is there an event available in the queue? */
    if (eventQueue.count > 0U)
    {
      /* Retrieve the event from the queue at the read index (oldest).  */
      *event = eventQueue.entries[eventQueue.readIdx];
      /* Update the total count. */
      eventQueue.count--;
      /* Increment the read index to point to the next entry. */
      eventQueue.readIdx++;
      /* Time to wrap around to the start? */
      if (eventQueue.readIdx == TBX_MB_EVENT_QUEUE_SIZE)
      {
        eventQueue.readIdx = 0U;
      }
      /* Update the result. */
      result = TBX_TRUE;
    }
    TbxCriticalSectionExit();
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbOsalEventWait ***/


/************************************************************************************//**
** \brief     Creates a new binary semaphore object with an initial count of 0, meaning
**            that it's taken.
** \return    Handle to the newly created binary semaphore object if successful, NULL
**            otherwise.
**
****************************************************************************************/
tTbxMbOsalSem TbxMbOsalSemCreate(void)
{
  tTbxMbOsalSem result = NULL;

  /* Allocate memory for the new semaphore context. */
  tTbxMbOsalSemCtx * newSemCtx = TbxMemPoolAllocate(sizeof(tTbxMbOsalSemCtx));
  /* Automatically increase the memory pool, if it was too small. */
  if (newSemCtx == NULL)
  {
    /* No need to check the return value, because if it failed, the following
     * allocation fails too, which is verified later on.
     */
    (void)TbxMemPoolCreate(1U, sizeof(tTbxMbOsalSemCtx));
    newSemCtx = TbxMemPoolAllocate(sizeof(tTbxMbOsalSemCtx));      
  }
  /* Verify memory allocation of the semaphore context. */
  TBX_ASSERT(newSemCtx != NULL);
  /* Only continue if the memory allocation succeeded. */
  if (newSemCtx != NULL)
  {
    /* Initialize the semaphore in a taken state. */
    newSemCtx->type = TBX_MB_OSAL_SEM_CONTEXT_TYPE;
    newSemCtx->count = 0U;
    /* Update the result. */
    result = newSemCtx;
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbOsalSemCreate ***/


/************************************************************************************//**
** \brief     Releases a binary semaphore object, previously created with
**            TbxMbOsalSemCreate().
** \param     sem Handle to the binary semaphore object to release.
**
****************************************************************************************/
void TbxMbOsalSemFree(tTbxMbOsalSem sem)
{
  /* Verify parameters. */
  TBX_ASSERT(sem != NULL);

  /* Only continue with valid parameters. */
  if (sem != NULL)
  {
    /* Convert the semaphore pointer to the context structure. */
    tTbxMbOsalSemCtx * semCtx = (tTbxMbOsalSemCtx *)sem;
    /* Sanity check on the context type. */
    TBX_ASSERT(semCtx->type == TBX_MB_OSAL_SEM_CONTEXT_TYPE);
    /* Give the semaphore context back to the memory pool. */
    TbxMemPoolRelease(semCtx);
  }
} /*** end of TbxMbOsalSemFree ***/


/************************************************************************************//**
** \brief     Give the semaphore, setting its count to 1, meaning that it's available.
** \param     sem Handle to the binary semaphore object.
** \param     fromIsr TBX_TRUE when calling this function from an interrupt service
**            routine, TBX_FALSE otherwise.
**
****************************************************************************************/
void TbxMbOsalSemGive(tTbxMbOsalSem sem,
                      uint8_t       fromIsr)
{
  TBX_UNUSED_ARG(fromIsr);

  /* Verify parameters. */
  TBX_ASSERT(sem != NULL);

  /* Only continue with valid parameters. */
  if (sem != NULL)
  {
    /* Convert the semaphore pointer to the context structure. */
    tTbxMbOsalSemCtx * semCtx = (tTbxMbOsalSemCtx *)sem;
    /* Sanity check on the context type. */
    TBX_ASSERT(semCtx->type == TBX_MB_OSAL_SEM_CONTEXT_TYPE);
    /* Give the semaphore by setting its count to 1. */
    TbxCriticalSectionEnter();
    semCtx->count = 1U;
    TbxCriticalSectionExit();
  }
} /*** end of TbxMbOsalSemGive ***/


/************************************************************************************//**
** \brief     Take the semaphore when available (count > 0) or wait a finite amount of
**            time for it to become available. The take operation decrements to count.
** \param     sem Handle to the binary semaphore object.
** \param     timeoutMs Maximum time in milliseconds to block while waiting for the 
**            semaphore to become available.
** \return    TBX_TRUE if the semaphore could be taken, TBX_FALSE otherwise (typically a
**            timeout).
**
****************************************************************************************/
uint8_t TbxMbOsalSemTake(tTbxMbOsalSem sem,
                         uint16_t      timeoutMs)
{
  uint8_t result = TBX_FALSE;

  /* Verify parameters. */
  TBX_ASSERT(sem != NULL);

  /* Only continue with valid parameters. */
  if (sem != NULL)
  {
    /* Convert the semaphore pointer to the context structure. */
    tTbxMbOsalSemCtx * semCtx = (tTbxMbOsalSemCtx *)sem;
    /* Sanity check on the context type. */
    TBX_ASSERT(semCtx->type == TBX_MB_OSAL_SEM_CONTEXT_TYPE);
    /* Is the semaphore currently available? */
    TbxCriticalSectionEnter();
    if (semCtx->count > 0U)
    {
      /* Take the semaphore and update the result for success. */
      semCtx->count = 0U;
      result = TBX_TRUE;
    }
    /* Semaphore not yet available. Wait for it with the hope that it becomes available
     * before the specified timeout. 
     */
    else
    {
      /* Keep track of when the last millisecond was detected. */
      uint16_t volatile lastMsTickTime = TbxMbPortTimerCount();
      /* Initialize variable with the actual number of milliseconds to wait. */
      uint16_t volatile waitTimeMs = timeoutMs;
      /* Enter wait loop. */
      while (waitTimeMs > 0U)
      {
        /* Temporarily leave the critical section. */
        TbxCriticalSectionExit();
        /* Run the event task to make sure that whatever is supposed to give the
         * semaphore can actually do so.
         */
        TbxMbEventTask();
        /* Get the number of ticks that elapsed since the last millisecond detection. 
         * Note that this calculation works, even if the 20 kHz timer counter
         * overflowed.
         */
        uint16_t deltaTicks = TbxMbPortTimerCount() - lastMsTickTime;
        /* Determine how many milliseconds passed since the last one was detected. */
        uint16_t deltaMs = deltaTicks / 20U;
        /* Did one or more milliseconds pass? */
        if (deltaMs > 0U)
        {
          /* Update the last millisecond detection tick time. Needed for the detection
           * of the next millisecond. Note that this calculation works, even if the
           * lastMsTickTime variable overflows.
           */
          lastMsTickTime += (deltaMs * 20U);
          /* Subtract the elapsed milliseconds from the remaining wait time, with
           * underflow protection. Note that the wait loop automatically stops when
           * waitTimeMs becomes zero.
           */
          if (waitTimeMs >= deltaMs)
          {
            waitTimeMs -= deltaMs;
          }
          else
          {
            waitTimeMs = 0U;
          }
        }
        /* Re-enter the critical section. */
        TbxCriticalSectionEnter();
        /* Did the semaphore become available? */
        if (semCtx->count > 0U)
        {
          /* Take the semaphore, update the result for success, and leave the loop. */
          semCtx->count = 0U;
          result = TBX_TRUE;
          break;
        }
      }
    }
    TbxCriticalSectionExit();
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbOsalSemTake ****/


/*********************************** end of tbxmb_superloop.c **************************/
