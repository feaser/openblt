/************************************************************************************//**
* \file         candriver.c
* \brief        Generic CAN driver source file.
* \ingroup      CanDriver
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

#ifndef NO_CAN_OR_USB

/****************************************************************************************
* Include files
****************************************************************************************/
#include <assert.h>                         /* for assertions                          */
#include <stdint.h>                         /* for standard integer types              */
#include <stddef.h>                         /* for NULL declaration                    */
#include <stdbool.h>                        /* for boolean type                        */
#include <string.h>                         /* for string library                      */
#include "candriver.h"                      /* Generic CAN driver module               */
#if defined(PLATFORM_LINUX)
#include "socketcan.h"                      /* SocketCAN interface                     */
#else
#include "pcanusb.h"                        /* Peak PCAN-USB interface                 */
#include "leaflight.h"                      /* Kvaser Leaf Light v2 interface          */
#include "canusb.h"                         /* Lawicel CANUSB interface                */
#include "xldriver.h"                       /* Vector XL driver interface              */
#include "vcidriver.h"                      /* Ixxat VCI driver interface              */
#endif


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Pointer to the CAN interface that is linked. */
static tCanInterface const * canIfPtr;

/** \brief Flag to store the connection status. */
static bool canConnected;


/************************************************************************************//**
** \brief     Initializes the CAN module. Typically called once at program startup.
** \param     settings Pointer to the CAN module settings.
**
****************************************************************************************/
void CanInit(tCanSettings const * settings)
{
  /* Initialize locals. */
  canIfPtr = NULL;
  canConnected = false;
  
  /* Check parameters. */
  assert(settings != NULL);
  
  /* Only continue with valid parameters. */
  if (settings != NULL)  /*lint !e774 */
  {
    /* Check device name. */
    assert(settings->devicename != NULL);

    /* Only continue with a valid device name. */
    if (settings->devicename != NULL) /*lint !e774 */
    {
      /* Determine the pointer to the correct CAN interface, based on the specified
       * device name.
       */
#if defined(PLATFORM_LINUX)
      /* On Linux, the device name is the name of the SocketCAN link, so always link
       * the SocketCAN interface to the CAN driver.
       */
      canIfPtr = SocketCanGetInterface();
#else
      if (strcmp(settings->devicename, "peak_pcanusb") == 0)
      {
        canIfPtr = PCanUsbGetInterface();
      }
      else if (strcmp(settings->devicename, "kvaser_leaflight") == 0)
      {
        canIfPtr = LeafLightGetInterface();
      }
      else if (strcmp(settings->devicename, "lawicel_canusb") == 0)
      {
        canIfPtr = CanUsbGetInterface();
      }
      else if (strcmp(settings->devicename, "vector_xldriver") == 0)
      {
        canIfPtr = VectorXlGetInterface();
      }
      else if (strcmp(settings->devicename, "ixxat_vcidriver") == 0)
      {
        canIfPtr = IxxatVciGetInterface();
      }
#endif
      /* Check validity of the interface. */
      assert(canIfPtr != NULL);

      /* Only continue with a valid interface. */
      if (canIfPtr != NULL) /*lint !e774 */
      {
        /* Initialize the CAN interface. */
        canIfPtr->Init(settings);
      }
    }
  }
} /*** end of CanInit ***/


/************************************************************************************//**
** \brief     Terminates the CAN module. Typically called once at program cleanup.
**
****************************************************************************************/
void CanTerminate(void)
{
  /* Make sure a valid CAN interface is linked. */
  assert(canIfPtr != NULL);
  
  /* Only continue with a valid CAN interface. */
  if (canIfPtr != NULL) /*lint !e774 */
  {
    /* Make sure to disconnect first. */
    CanDisconnect();
    /* Terminate the CAN interface. */
    canIfPtr->Terminate();
    /* Unlink the CAN interface. */
    canIfPtr = NULL;
  }
} /*** end of CanTerminate ***/


/************************************************************************************//**
** \brief     Connects the CAN module.
** \return    True if connected, false otherwise.
**
****************************************************************************************/
bool CanConnect(void)
{
  bool result = false;
  
  /* Make sure a valid CAN interface is linked. */
  assert(canIfPtr != NULL);
  
  /* Only continue with a valid CAN interface. */
  if (canIfPtr != NULL) /*lint !e774 */
  {
    /* Make sure that the CAN module is in the disconnected state. */
    CanDisconnect();
    /* Connect the CAN interface. */
    result = canIfPtr->Connect();
    /* Update the connection state. */
    canConnected = result;
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of CanConnect ***/


/************************************************************************************//**
** \brief     Disconnects the CAN module.
**
****************************************************************************************/
void CanDisconnect(void)
{
  /* Make sure a valid CAN interface is linked. */
  assert(canIfPtr != NULL);
  
  /* Only continue with a valid CAN interface. */
  if (canIfPtr != NULL) /*lint !e774 */
  {
    /* Only disconnect if actually connected. */
    if (canConnected)
    {
      /* Disconnect CAN interface. */
      canIfPtr->Disconnect();
      /* Update the connection state. */
      canConnected = false;
    }
  }
} /*** end of CanDisconnect ***/


/************************************************************************************//**
** \brief     Obtains the connection state of the CAN module.
** \return    True if connected, false otherwise.
**
****************************************************************************************/
bool CanIsConnected(void)
{
  /* Obtain and return the connection state to the caller. */
  return canConnected;
} /*** end of CanIsConnected ***/


/************************************************************************************//**
** \brief     Submits a message for transmission on the CAN bus.
** \param     msg Pointer to CAN message structure.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool CanTransmit(tCanMsg const * msg)
{
  bool result = false;
  
  /* Make sure a valid CAN interface is linked. */
  assert(canIfPtr != NULL);
  
  /* Only continue with a valid CAN interface. */
  if (canIfPtr != NULL) /*lint !e774 */
  {
    /* Only transmit the message if connected. */
    if (canConnected)
    {
      result = canIfPtr->Transmit(msg);
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of CanTransmit ***/


/************************************************************************************//**
** \brief     Checks if a bus off or bus heavy situation occurred.
** \return    True if a bus error situation was detected, false otherwise.
**
****************************************************************************************/
bool CanIsBusError(void)
{
  bool result = false;

  /* Make sure a valid CAN interface is linked. */
  assert(canIfPtr != NULL);
  
  /* Only continue with a valid CAN interface. */
  if (canIfPtr != NULL) /*lint !e774 */
  {
    /* Only check for bus error if connected. */
    if (canConnected)
    {
      result = canIfPtr->IsBusError();
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of CanIsBusError ***/


/************************************************************************************//**
** \brief     Registers the event callback functions that should be called by the CAN
**            module.
** \param     events Pointer to structure with event callback function pointers.
**
****************************************************************************************/
void CanRegisterEvents(tCanEvents const * events)
{
  /* Make sure a valid CAN interface is linked. */
  assert(canIfPtr != NULL);
  
  /* Only continue with a valid CAN interface. */
  if (canIfPtr != NULL) /*lint !e774 */
  {
    /* Register the events with the CAN interface. */
    canIfPtr->RegisterEvents(events);
  }
} /*** end of CanRegisterEvents ***/


#endif // NO_CAN_OR_USB
/*********************************** end of candriver.c ********************************/
