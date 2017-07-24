/************************************************************************************//**
* \file         pcanusb.c
* \brief        Peak PCAN-USB interface source file.
* \ingroup      PCanUsb
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
#include "pcanusb.h"                        /* Peak PCAN-USB interface                 */
#include <windows.h>                        /* for Windows API                         */
#include "PCANBasic.h"                      /* for PCAN-Basic API                      */


/***************************************************************************************
* Function prototypes
****************************************************************************************/
static void PCanUsbInit(tCanSettings const * settings);
static void PCanUsbTerminate(void);
static bool PCanUsbConnect(void);
static void PCanUsbDisconnect(void);
static bool PCanUsbTransmit(tCanMsg const * msg);
static void PCanUsbRegisterEvents(tCanEvents const * events);


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief CAN interface structure filled with Peak PCAN-USB specifics. */
static const tCanInterface pCanUsbInterface =
{
  PCanUsbInit,
  PCanUsbTerminate,
  PCanUsbConnect,
  PCanUsbDisconnect,
  PCanUsbTransmit,
  PCanUsbRegisterEvents
};


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief The settings to use in this CAN interface. */
static tCanSettings pCanUsbSettings;

/** \brief List with callback functions that this driver should use. */
static tCanEvents * pCanUsbEventsList;

/** \brief Total number of event entries into the \ref pCanUsbEventsList list. */
static uint32_t pCanUsbEventsEntries;


/***********************************************************************************//**
** \brief     Obtains a pointer to the CAN interface structure, so that it can be linked
**            to the generic CAN driver module.
** \return    Pointer to CAN interface structure.
**
****************************************************************************************/
tCanInterface const * PCanUsbGetInterface(void)
{
  return &pCanUsbInterface;
} /*** end of PCanUsbGetInterface ***/


/************************************************************************************//**
** \brief     Initializes the CAN interface.
** \param     settings Pointer to the CAN interface settings.
**
****************************************************************************************/
static void PCanUsbInit(tCanSettings const * settings)
{
  char * canDeviceName;

  /* Initialize locals. */
  pCanUsbEventsList = NULL;
  pCanUsbEventsEntries = 0;
  /* Reset CAN interface settings. */
  pCanUsbSettings.devicename = "";
  pCanUsbSettings.channel = 0;
  pCanUsbSettings.baudrate = CAN_BR500K;
  pCanUsbSettings.code = 0x00000000u;
  pCanUsbSettings.mask = 0x00000000u;

  /* Check parameters. */
  assert(settings != NULL);

  /* Only continue with valid parameters. */
  if (settings != NULL) /*lint !e774 */
  {
    /* Shallow copy the CAN interface settings for later usage. */
    pCanUsbSettings = *settings;
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
        pCanUsbSettings.devicename = canDeviceName;
      }
    }
  }
  /* ##Vg TODO Perform initialization of PCAN-Basic API. */
} /*** end of PCanUsbInit ***/


/************************************************************************************//**
** \brief     Terminates the CAN interface.
**
****************************************************************************************/
static void PCanUsbTerminate(void)
{
  /* ##Vg TODO Perform termination of PCAN-Basic API. */
  /* Release memory that was allocated for storing the device name. */
  if (pCanUsbSettings.devicename != NULL)
  {
    free((char *)pCanUsbSettings.devicename);
  }
  /* Reset CAN interface settings. */
  pCanUsbSettings.devicename = "";
  pCanUsbSettings.channel = 0;
  pCanUsbSettings.baudrate = CAN_BR500K;
  pCanUsbSettings.code = 0x00000000u;
  pCanUsbSettings.mask = 0x00000000u;
  /* Release memory that was allocated for CAN events and reset the entry count. */
  if ( (pCanUsbEventsList != NULL) && (pCanUsbEventsEntries != 0) )
  {
    free(pCanUsbEventsList);
    pCanUsbEventsEntries = 0;
  }
} /*** end of PCanUsbTerminate ***/


/************************************************************************************//**
** \brief     Connects the CAN interface.
** \return    True if connected, false otherwise.
**
****************************************************************************************/
static bool PCanUsbConnect(void)
{
  bool result = false;

  /* ##Vg TODO Check CAN interface settings. */

  /* ##Vg TODO Connect to CAN interface, if settings are valid. */

  /* Give the result back to the caller. */
  return result;
} /*** end of PCanUsbConnect ***/


/************************************************************************************//**
** \brief     Disconnects the CAN interface.
**
****************************************************************************************/
static void PCanUsbDisconnect(void)
{
  /* ##Vg TODO Disconnect from the CAN interface. */
} /*** end of PCanUsbDisconnect ***/


/************************************************************************************//**
** \brief     Submits a message for transmission on the CAN bus.
** \param     msg Pointer to CAN message structure.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool PCanUsbTransmit(tCanMsg const * msg)
{
  bool result = false;

  /* Check parameters. */
  assert(msg != NULL);

  /* Only continue with valid parameters. */
  if (msg != NULL) /*lint !e774 */
  {
    /* ##Vg TODO Submit CAN message for transmission */
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of PCanUsbTransmit ***/


/************************************************************************************//**
** \brief     Registers the event callback functions that should be called by the CAN
**            interface.
** \param     events Pointer to structure with event callback function pointers.
**
****************************************************************************************/
static void PCanUsbRegisterEvents(tCanEvents const * events)
{
  /* Check parameters. */
  assert(events != NULL);

  /* ##Vg TODO Test with multiple event entries to check that realloc approach works. */

  /* Only continue with valid parameters. */
  if (events != NULL) /*lint !e774 */
  {
    /* Increase length of the list to make space for one more event entry. Note that
     * it is okay to call realloc with a NULL pointer. In this case it simply behaves
     * as malloc. 
     */
    pCanUsbEventsList = realloc(pCanUsbEventsList,
                                (sizeof(tCanEvents) * (pCanUsbEventsEntries + 1)));
    /* Assert reallocation. */
    assert(pCanUsbEventsList != NULL);
    /* Only continue if reallocation was successful. */
    if (pCanUsbEventsList != NULL)
    {
      /* Increment events entry count. */
      pCanUsbEventsEntries++;
      /* Store the events in the new entry. */
      pCanUsbEventsList[pCanUsbEventsEntries - 1] = *events;
    }
    /* Reallocation failed. */
    else
    {
      /* Reset events entry count. */
      pCanUsbEventsEntries = 0;
    }
  }
} /*** end of PCanUsbRegisterEvents ***/


/*********************************** end of pcanusb.c **********************************/

