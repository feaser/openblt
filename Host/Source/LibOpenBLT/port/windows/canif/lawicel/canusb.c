/************************************************************************************//**
* \file         canusb.c
* \brief        Lawicel CANUSB interface source file.
* \ingroup      Lawicel_CanUsb
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2017  by Feaser    http://www.feaser.com    All rights reserved
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
#include <assert.h>                         /* for assertions                          */
#include <stdint.h>                         /* for standard integer types              */
#include <stddef.h>                         /* for NULL declaration                    */
#include <stdbool.h>                        /* for boolean type                        */
#include <stdlib.h>                         /* for standard library                    */
#include <string.h>                         /* for string library                      */
#include "candriver.h"                      /* Generic CAN driver module               */
#include "canusb.h"                         /* Lawicel CANUSB interface                */
#include <windows.h>                        /* for Windows API                         */
#include "lawicel_can.h"                    /* for Lawicel CANUSB API                  */


/***************************************************************************************
* Function prototypes
****************************************************************************************/
/* CAN interface functions. */
static void CanUsbInit(tCanSettings const * settings);
static void CanUsbTerminate(void);
static bool CanUsbConnect(void);
static void CanUsbDisconnect(void);
static bool CanUsbTransmit(tCanMsg const * msg);
static bool CanUsbIsBusError(void);
static void CanUsbRegisterEvents(tCanEvents const * events);
/* CAN message reception thread. */
static DWORD WINAPI CanUsbReceptionThread(LPVOID pv);


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief CAN interface structure filled with Lawicel CANUSB specifics. */
static const tCanInterface canUsbInterface =
{
  CanUsbInit,
  CanUsbTerminate,
  CanUsbConnect,
  CanUsbDisconnect,
  CanUsbTransmit,
  CanUsbIsBusError,
  CanUsbRegisterEvents
};


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief The settings to use in this CAN interface. */
static tCanSettings canUsbSettings;

/** \brief List with callback functions that this driver should use. */
static tCanEvents * canUsbEventsList;

/** \brief Total number of event entries into the \ref canUsbEventsList list. */
static uint32_t canUsbEventsEntries;

/** \brief Handle for the event to terminate the reception thread. */
static HANDLE canUsbTerminateEvent;

/** \brief Handle for the CAN reception thread. */
static HANDLE canUsbRxThreadHandle;


/***********************************************************************************//**
** \brief     Obtains a pointer to the CAN interface structure, so that it can be linked
**            to the generic CAN driver module.
** \return    Pointer to CAN interface structure.
**
****************************************************************************************/
tCanInterface const * CanUsbGetInterface(void)
{
  return &canUsbInterface;
} /*** end of CanUsbGetInterface ***/


/************************************************************************************//**
** \brief     Initializes the CAN interface.
** \param     settings Pointer to the CAN interface settings.
**
****************************************************************************************/
static void CanUsbInit(tCanSettings const * settings)
{
  char * canDeviceName;

  /* Initialize locals. */
  canUsbEventsList = NULL;
  canUsbEventsEntries = 0;
  canUsbTerminateEvent = NULL;
  canUsbRxThreadHandle = NULL;
  /* Reset CAN interface settings. */
  canUsbSettings.devicename = "";
  canUsbSettings.channel = 0;
  canUsbSettings.baudrate = CAN_BR500K;
  canUsbSettings.code = 0x00000000u;
  canUsbSettings.mask = 0x00000000u;

  /* Check parameters. */
  assert(settings != NULL);

  /* Only continue with valid parameters. */
  if (settings != NULL) /*lint !e774 */
  {
    /* Shallow copy the CAN interface settings for later usage. */
    canUsbSettings = *settings;
    /* The devicename is a pointer and it is not gauranteed that it stays valid so we need
     * to deep copy this one. note the +1 for '\0' in malloc.
     */
    assert(settings->devicename != NULL);
    if (settings->devicename != NULL) /*lint !e774 */
    {
      canDeviceName = malloc(strlen(settings->devicename) + 1);
      assert(canDeviceName != NULL);
      if (canDeviceName != NULL) /*lint !e774 */
      {
        strcpy(canDeviceName, settings->devicename);
        canUsbSettings.devicename = canDeviceName;
      }
    }
    /* ##Vg TODO Perform initialization of Lawicel CANUSB API. */
  }
} /*** end of CanUsbInit ***/


/************************************************************************************//**
** \brief     Terminates the CAN interface.
**
****************************************************************************************/
static void CanUsbTerminate(void)
{
  /* ##Vg TODO Perform termination of Lawicel CANUSB API. */
  /* Release memory that was allocated for storing the device name. */
  if (canUsbSettings.devicename != NULL)
  {
    free((char *)canUsbSettings.devicename);
  }
  /* Reset CAN interface settings. */
  canUsbSettings.devicename = "";
  canUsbSettings.channel = 0;
  canUsbSettings.baudrate = CAN_BR500K;
  canUsbSettings.code = 0x00000000u;
  canUsbSettings.mask = 0x00000000u;
  /* Release memory that was allocated for CAN events and reset the entry count. */
  if ( (canUsbEventsList != NULL) && (canUsbEventsEntries != 0) )
  {
    free(canUsbEventsList);
    canUsbEventsEntries = 0;
  }
} /*** end of CanUsbTerminate ***/


/************************************************************************************//**
** \brief     Connects the CAN interface.
** \return    True if connected, false otherwise.
**
****************************************************************************************/
static bool CanUsbConnect(void)
{
  bool result = false;
  bool baudrateSupported = true;

  /* Note that the device name itself is not needed anymore at this point, it was only
   * needed by the CAN driver to link the correct interface (this one). The channel is
   * also don't care as the adapter only has one channel. Check settings.
   */
  assert(baudrateSupported);

  /* Invalidate handles. */
  canUsbTerminateEvent = NULL;
  canUsbRxThreadHandle = NULL;

  /* Only continue with valid settings. */
  if (baudrateSupported)
  {
    /* Init result code to success and only negate it on detection of error. */
    result = true;

    /* ##Vg TODO Process and verify settings, configure acceptance filter, connect. */
    result = false; /* Temporary. */

    /* Create the terminate event handle used in the reception thread. */
    if (result)
    {
      canUsbTerminateEvent = CreateEvent(NULL, TRUE, FALSE, "");
      if (canUsbTerminateEvent == NULL)
      {
        result = false;
      }
    }
    /* Start the reception thread as the last step. */
    if (result)
    {
      canUsbRxThreadHandle = CreateThread(NULL, 0, CanUsbReceptionThread,
                                           NULL, 0, NULL);
      if (canUsbRxThreadHandle == NULL)
      {
        result = false;
      }
    }
  }
  
  /* Clean-up in case an error occurred. */
  if (!result)
  {
    if (canUsbTerminateEvent != NULL)
    {
      /* Close the event handle. */
      (void)CloseHandle(canUsbTerminateEvent);
      canUsbTerminateEvent = NULL;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of CanUsbConnect ***/


/************************************************************************************//**
** \brief     Disconnects the CAN interface.
**
****************************************************************************************/
static void CanUsbDisconnect(void)
{
   /* Stop the reception thread. */
  if (canUsbRxThreadHandle != NULL)
  {
    /* Trigger event to request the reception thread to stop. */
    (void)SetEvent(canUsbTerminateEvent);
    /* Wait for the thread to signal termination. */
    (void)WaitForSingleObject(canUsbRxThreadHandle, INFINITE);
    /* Close the thread handle. */
    (void)CloseHandle(canUsbRxThreadHandle);
    canUsbRxThreadHandle = NULL;
  }
  /* Close the terminate event handle. */
  if (canUsbTerminateEvent != NULL)
  {
    (void)CloseHandle(canUsbTerminateEvent);
    canUsbTerminateEvent = NULL;
  }
  /* ##Vg TODO Go off the bus and close the channel. */
} /*** end of CanUsbDisconnect ***/


/************************************************************************************//**
** \brief     Submits a message for transmission on the CAN bus.
** \param     msg Pointer to CAN message structure.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool CanUsbTransmit(tCanMsg const * msg)
{
  bool result = false;

  /* Check parameters. */
  assert(msg != NULL);

  /* Only continue with valid parameters. */
  if (msg != NULL) /*lint !e774 */
  {
    /* ##Vg TODO Transmit and trigger event(s). */
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of CanUsbTransmit ***/


/************************************************************************************//**
** \brief     Checks if a bus off or bus heavy situation occurred.
** \return    True if a bus error situation was detected, false otherwise.
**
****************************************************************************************/
static bool CanUsbIsBusError(void)
{
  bool result = false;

  /* ##Vg TODO Check and process status. */

  /* Give the result back to the caller. */
  return result;
} /*** end of CanUsbIsBusError ***/


/************************************************************************************//**
** \brief     Registers the event callback functions that should be called by the CAN
**            interface.
** \param     events Pointer to structure with event callback function pointers.
**
****************************************************************************************/
static void CanUsbRegisterEvents(tCanEvents const * events)
{
  /* Check parameters. */
  assert(events != NULL);

  /* Only continue with valid parameters. */
  if (events != NULL) /*lint !e774 */
  {
    /* Increase length of the list to make space for one more event entry. Note that
     * it is okay to call realloc with a NULL pointer. In this case it simply behaves
     * as malloc. 
     */
    canUsbEventsList = realloc(canUsbEventsList,
                                  (sizeof(tCanEvents) * (canUsbEventsEntries + 1)));
    /* Assert reallocation. */
    assert(canUsbEventsList != NULL);
    /* Only continue if reallocation was successful. */
    if (canUsbEventsList != NULL)
    {
      /* Increment events entry count. */
      canUsbEventsEntries++;
      /* Store the events in the new entry. */
      canUsbEventsList[canUsbEventsEntries - 1] = *events;
    }
    /* Reallocation failed. */
    else
    {
      /* Reset events entry count. */
      canUsbEventsEntries = 0;
    }
  }
} /*** end of CanUsbRegisterEvents ***/


/************************************************************************************//**
** \brief     CAN message reception thread.
** \param     pv Pointer to thread parameters.
** \return    Thread exit code.
**
****************************************************************************************/
static DWORD WINAPI CanUsbReceptionThread(LPVOID pv)
{
  DWORD waitResult;
  bool running = true;

  /* Parameter not used. */
  (void)pv;

  /* Enter thread's infinite loop. */
  while (running)
  {
    waitResult = WaitForSingleObject(canUsbTerminateEvent, 10);
    switch (waitResult) 
    {
      /* Termination event. */
      case WAIT_OBJECT_0 + 0: /*lint !e835 */
        /* Stop thread. */
        running = false;
        break;
      default:
        break;
    }
  }
  /* Exit thread. */
  return 0;
} /*** end of CanUsbReceptionThread ***/


/*********************************** end of canusb.c ***********************************/

