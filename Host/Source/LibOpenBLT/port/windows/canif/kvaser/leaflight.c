/************************************************************************************//**
* \file         leaflight.c
* \brief        Kvaser Leaf Light v2 interface source file.
* \ingroup      Kvaser_LeafLight
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
#include "leaflight.h"                      /* Kvaser Leaf Light v2 interface          */
#include <windows.h>                        /* for Windows API                         */


/***************************************************************************************
* Function prototypes
****************************************************************************************/
/* CAN interface functions. */
static void LeafLightInit(tCanSettings const * settings);
static void LeafLightTerminate(void);
static bool LeafLightConnect(void);
static void LeafLightDisconnect(void);
static bool LeafLightTransmit(tCanMsg const * msg);
static bool LeafLightIsBusError(void);
static void LeafLightRegisterEvents(tCanEvents const * events);


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief CAN interface structure filled with Kvaser Leaf Light v2 specifics. */
static const tCanInterface leafLightInterface =
{
  LeafLightInit,
  LeafLightTerminate,
  LeafLightConnect,
  LeafLightDisconnect,
  LeafLightTransmit,
  LeafLightIsBusError,
  LeafLightRegisterEvents
};


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief The settings to use in this CAN interface. */
static tCanSettings leafLightSettings;

/** \brief List with callback functions that this driver should use. */
static tCanEvents * leafLightEventsList;

/** \brief Total number of event entries into the \ref leafLightEventsList list. */
static uint32_t leafLightEventsEntries;


/***********************************************************************************//**
** \brief     Obtains a pointer to the CAN interface structure, so that it can be linked
**            to the generic CAN driver module.
** \return    Pointer to CAN interface structure.
**
****************************************************************************************/
tCanInterface const * LeafLightGetInterface(void)
{
  return &leafLightInterface;
} /*** end of LeafLightGetInterface ***/


/************************************************************************************//**
** \brief     Initializes the CAN interface.
** \param     settings Pointer to the CAN interface settings.
**
****************************************************************************************/
static void LeafLightInit(tCanSettings const * settings)
{
  char * canDeviceName;

  /* Initialize locals. */
  leafLightEventsList = NULL;
  leafLightEventsEntries = 0;
  /* Reset CAN interface settings. */
  leafLightSettings.devicename = "";
  leafLightSettings.channel = 0;
  leafLightSettings.baudrate = CAN_BR500K;
  leafLightSettings.code = 0x00000000u;
  leafLightSettings.mask = 0x00000000u;

  /* Check parameters. */
  assert(settings != NULL);

  /* Only continue with valid parameters. */
  if (settings != NULL) /*lint !e774 */
  {
    /* Shallow copy the CAN interface settings for later usage. */
    leafLightSettings = *settings;
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
        leafLightSettings.devicename = canDeviceName;
      }
    }
    /* ##Vg TODO Perform initialization of Kvaser Leaf Light API. */
  }
} /*** end of LeafLightInit ***/


/************************************************************************************//**
** \brief     Terminates the CAN interface.
**
****************************************************************************************/
static void LeafLightTerminate(void)
{
  /* ##Vg TODO Perform termination of Kvaser Leaf Light API. */
  /* Release memory that was allocated for storing the device name. */
  if (leafLightSettings.devicename != NULL)
  {
    free((char *)leafLightSettings.devicename);
  }
  /* Reset CAN interface settings. */
  leafLightSettings.devicename = "";
  leafLightSettings.channel = 0;
  leafLightSettings.baudrate = CAN_BR500K;
  leafLightSettings.code = 0x00000000u;
  leafLightSettings.mask = 0x00000000u;
  /* Release memory that was allocated for CAN events and reset the entry count. */
  if ( (leafLightEventsList != NULL) && (leafLightEventsEntries != 0) )
  {
    free(leafLightEventsList);
    leafLightEventsEntries = 0;
  }
} /*** end of LeafLightTerminate ***/


/************************************************************************************//**
** \brief     Connects the CAN interface.
** \return    True if connected, false otherwise.
**
****************************************************************************************/
static bool LeafLightConnect(void)
{
  bool result = false;

  /* ##Vg TODO Process and verify settings, configure acceptance filter, connect. */

  /* Give the result back to the caller. */
  return result;
} /*** end of LeafLightConnect ***/


/************************************************************************************//**
** \brief     Disconnects the CAN interface.
**
****************************************************************************************/
static void LeafLightDisconnect(void)
{
  /* ##Vg TODO Disconnect. */
} /*** end of LeafLightDisconnect ***/


/************************************************************************************//**
** \brief     Submits a message for transmission on the CAN bus.
** \param     msg Pointer to CAN message structure.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool LeafLightTransmit(tCanMsg const * msg)
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
} /*** end of LeafLightTransmit ***/


/************************************************************************************//**
** \brief     Checks if a bus off or bus heavy situation occurred.
** \return    True if a bus error situation was detected, false otherwise.
**
****************************************************************************************/
static bool LeafLightIsBusError(void)
{
  bool result = false;

  /* ##Vg TODO Check and process status. */

  /* Give the result back to the caller. */
  return result;
} /*** end of LeafLightIsBusError ***/


/************************************************************************************//**
** \brief     Registers the event callback functions that should be called by the CAN
**            interface.
** \param     events Pointer to structure with event callback function pointers.
**
****************************************************************************************/
static void LeafLightRegisterEvents(tCanEvents const * events)
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
    leafLightEventsList = realloc(leafLightEventsList,
                                  (sizeof(tCanEvents) * (leafLightEventsEntries + 1)));
    /* Assert reallocation. */
    assert(leafLightEventsList != NULL);
    /* Only continue if reallocation was successful. */
    if (leafLightEventsList != NULL)
    {
      /* Increment events entry count. */
      leafLightEventsEntries++;
      /* Store the events in the new entry. */
      leafLightEventsList[leafLightEventsEntries - 1] = *events;
    }
    /* Reallocation failed. */
    else
    {
      /* Reset events entry count. */
      leafLightEventsEntries = 0;
    }
  }
} /*** end of LeafLightRegisterEvents ***/


/*********************************** end of leaflight.c ********************************/

