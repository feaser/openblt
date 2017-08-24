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
/*lint -efile(537, pshpack1.h, poppack.h) */
/*lint -efile(451, pshpack1.h, poppack.h) */
#include "canlib.h"                         /* for Kvaser CANLIB API                   */


/***************************************************************************************
* Type definitions
****************************************************************************************/
/* Type definitions of the functions in the Kvaser CANLIB API that this CAN interface
 * uses.
 */
typedef void      (__stdcall * tLeafLightLibFuncInitializeLibrary)(void); 
typedef canStatus (__stdcall * tLeafLightLibFuncUnloadLibrary)(void); 
typedef CanHandle (__stdcall * tLeafLightLibFuncOpenChannel)(int32_t channel, int32_t flags);
typedef canStatus (__stdcall * tLeafLightLibFuncSetBusParams)(const CanHandle hnd, int32_t freq, uint32_t tseg1, uint32_t tseg2, uint32_t sjw, uint32_t noSamp, uint32_t syncmode);
typedef canStatus (__stdcall * tLeafLightLibFuncSetBusOutputControl)(const CanHandle hnd, const uint32_t drivertype);
typedef canStatus (__stdcall * tLeafLightLibFuncSetAcceptanceFilter)(const CanHandle hnd, uint32_t code, uint32_t mask, int32_t is_extended);
typedef canStatus (__stdcall * tLeafLightLibFuncIoCtl)(const CanHandle hnd, uint32_t func, void * buf, uint32_t buflen);
typedef canStatus (__stdcall * tLeafLightLibFuncBusOn)(const CanHandle hnd);
typedef canStatus (__stdcall * tLeafLightLibFuncWrite)(const CanHandle hnd, int32_t id, void * msg, uint32_t dlc, uint32_t flag);
typedef canStatus (__stdcall * tLeafLightLibFuncRead)(const CanHandle hnd, int32_t * id, void * msg, uint32_t * dlc, uint32_t * flag, uint32_t * time);
typedef canStatus (__stdcall * tLeafLightLibFuncReadStatus)(const CanHandle hnd, uint32_t * const flags);
typedef canStatus (__stdcall * tLeafLightLibFuncBusOff)(const CanHandle hnd);
typedef canStatus (__stdcall * tLeafLightLibFuncClose)(const CanHandle hnd);


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
/* CAN message reception thread. */
static DWORD WINAPI LeafLightReceptionThread(LPVOID pv);
/* Kvaser CANLIB library handling. */
static void LeafLightLibLoadDll(void);
static void LeafLightLibUnloadDll(void);
static void LeafLightLibFuncInitializeLibrary(void); 
static canStatus LeafLightLibFuncUnloadLibrary(void); 
static CanHandle LeafLightLibFuncOpenChannel(int32_t channel, int32_t flags);
static canStatus LeafLightLibFuncSetBusParams(const CanHandle hnd, int32_t freq, 
                                              uint32_t tseg1, uint32_t tseg2, 
                                              uint32_t sjw, uint32_t noSamp, 
                                              uint32_t syncmode);
static canStatus LeafLightLibFuncSetBusOutputControl(const CanHandle hnd, 
                                                     const uint32_t drivertype);
static canStatus LeafLightLibFuncSetAcceptanceFilter(const CanHandle hnd, uint32_t code,
                                                     uint32_t mask, int32_t is_extended);
static canStatus LeafLightLibFuncIoCtl(const CanHandle hnd, uint32_t func, void * buf, 
                                       uint32_t buflen);
static canStatus LeafLightLibFuncBusOn(const CanHandle hnd);
static canStatus LeafLightLibFuncWrite(const CanHandle hnd, int32_t id, void * msg, 
                                       uint32_t dlc, uint32_t flag);
static canStatus LeafLightLibFuncRead(const CanHandle hnd, int32_t * id, void * msg, 
                                      uint32_t * dlc, uint32_t * flag, uint32_t * time);
static canStatus LeafLightLibFuncReadStatus(const CanHandle hnd, uint32_t * const flags);
static canStatus LeafLightLibFuncBusOff(const CanHandle hnd);
static canStatus LeafLightLibFuncClose(const CanHandle hnd);


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

/** \brief Handle to the Kvaser CANLIB dynamic link library. */
static HINSTANCE leafLightDllHandle;

/** \brief Handle to the CAN channel. */
static CanHandle leafLightCanHandle;

/** \brief Function pointer to the Kvaser CANLIB canInitializeLibrary function. */
static tLeafLightLibFuncInitializeLibrary leafLightLibFuncInitializeLibraryPtr;

/** \brief Function pointer to the Kvaser CANLIB canUnloadLibrary function. */
static tLeafLightLibFuncUnloadLibrary leafLightLibFuncUnloadLibraryPtr;

/** \brief Function pointer to the Kvaser CANLIB canOpenChannel function. */
static tLeafLightLibFuncOpenChannel leafLightLibFuncOpenChannelPtr;

/** \brief Function pointer to the Kvaser CANLIB canSetBusParams function. */
static tLeafLightLibFuncSetBusParams leafLightLibFuncSetBusParamsPtr;

/** \brief Function pointer to the Kvaser CANLIB canSetBusOutputControl function. */
static tLeafLightLibFuncSetBusOutputControl leafLightLibFuncSetBusOutputControlPtr;

/** \brief Function pointer to the Kvaser CANLIB canSetAcceptanceFilter function. */
static tLeafLightLibFuncSetAcceptanceFilter leafLightLibFuncSetAcceptanceFilterPtr;

/** \brief Function pointer to the Kvaser CANLIB canIoCtl function. */
static tLeafLightLibFuncIoCtl leafLightLibFuncIoCtlPtr;

/** \brief Function pointer to the Kvaser CANLIB canBusOn function. */
static tLeafLightLibFuncBusOn leafLightLibFuncBusOnPtr;

/** \brief Function pointer to the Kvaser CANLIB canWrite function. */
static tLeafLightLibFuncWrite leafLightLibFuncWritePtr;

/** \brief Function pointer to the Kvaser CANLIB canRead function. */
static tLeafLightLibFuncRead leafLightLibFuncReadPtr;

/** \brief Function pointer to the Kvaser CANLIB canReadStatus function. */
static tLeafLightLibFuncReadStatus leafLightLibFuncReadStatusPtr;

/** \brief Function pointer to the Kvaser CANLIB canBusOff function. */
static tLeafLightLibFuncBusOff leafLightLibFuncBusOffPtr;

/** \brief Function pointer to the Kvaser CANLIB canClose function. */
static tLeafLightLibFuncClose leafLightLibFuncClosePtr;

/** \brief Handle for the event to terminate the reception thread. */
static HANDLE leafLightTerminateEvent;

/** \brief Handle for a CAN related event. */
static HANDLE leafLightCanEvent;

/** \brief Handle for the CAN reception thread. */
static HANDLE leafLightRxThreadHandle;


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
  leafLightTerminateEvent = NULL;
  leafLightCanEvent = NULL;
  leafLightRxThreadHandle = NULL;
  leafLightDllHandle = NULL;
  leafLightCanHandle = -1;
  /* Reset library function pointers. */
  leafLightLibFuncInitializeLibraryPtr = NULL;
  leafLightLibFuncUnloadLibraryPtr = NULL;
  leafLightLibFuncOpenChannelPtr = NULL;
  leafLightLibFuncSetBusParamsPtr = NULL;
  leafLightLibFuncSetBusOutputControlPtr = NULL;
  leafLightLibFuncSetAcceptanceFilterPtr = NULL;
  leafLightLibFuncIoCtlPtr = NULL;
  leafLightLibFuncBusOnPtr = NULL;
  leafLightLibFuncWritePtr = NULL;
  leafLightLibFuncReadPtr = NULL;
  leafLightLibFuncReadStatusPtr = NULL;
  leafLightLibFuncBusOffPtr = NULL;
  leafLightLibFuncClosePtr = NULL;
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
    /* Perform initialization of Kvaser Leaf Light API. */
    LeafLightLibLoadDll();
    LeafLightLibFuncInitializeLibrary();
  }
} /*** end of LeafLightInit ***/


/************************************************************************************//**
** \brief     Terminates the CAN interface.
**
****************************************************************************************/
static void LeafLightTerminate(void)
{
  /* Perform termination of Kvaser Leaf Light API. */
  (void)LeafLightLibFuncUnloadLibrary();
  LeafLightLibUnloadDll();
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
  bool baudrateSupported = true;
  int32_t frequency = 0;
  uint32_t tseg1 = 0;
  uint32_t tseg2 = 0;
  uint32_t sjw = 0;
  uint32_t noSamp = 0;

  /* Convert the baudrate to a value supported by the PCAN-Basic API. */
  switch (leafLightSettings.baudrate)
  {
    case CAN_BR10K:
      frequency = canBITRATE_10K;
      break;
    case CAN_BR20K:
      frequency = 20000;
      tseg1 = 5;
      tseg2 = 2;
      sjw = 2;
      noSamp = 1;
      break;
    case CAN_BR50K:
      frequency = canBITRATE_50K;
      break;
    case CAN_BR100K:
      frequency = canBITRATE_100K;
      break;
    case CAN_BR125K:
      frequency = canBITRATE_125K;
      break;
    case CAN_BR250K:
      frequency = canBITRATE_250K;
      break;
    case CAN_BR500K:
      frequency = canBITRATE_500K;
      break;
    case CAN_BR800K:
      frequency = 800000;
      tseg1 = 6;
      tseg2 = 3;
      sjw = 2;
      noSamp = 1;
      break;
    case CAN_BR1M:
      frequency = canBITRATE_1M;
      break;
    default:
      baudrateSupported = false;
      break;
  }

  /* Note that the device name itself is not needed anymore at this point, it was only
   * needed by the CAN driver to link the correct interface (this one). The channel is
   * also don't care as the adapter only has one channel. Check settings.
   */
  assert(baudrateSupported);

  /* Invalidate handles. */
  leafLightCanHandle = -1;
  leafLightTerminateEvent = NULL;
  leafLightCanEvent = NULL;
  leafLightRxThreadHandle = NULL;

  /* Only continue with valid settings. */
  if (baudrateSupported)
  {
    /* Init result code to success and only negate it on detection of error. */
    result = true;
    /* Open the CAN channel with support for both 11- and 29-bit CAN identifiers, and 
     *  obtain its handle. 
     */
    leafLightCanHandle = LeafLightLibFuncOpenChannel(0, canOPEN_REQUIRE_INIT_ACCESS |
                                                     canOPEN_REQUIRE_EXTENDED);
    /* Validate the handle. */
    if (leafLightCanHandle < 0)
    {
      result = false;
    }
    /* Configure the baudrate. */
    if (result)
    {
      if (LeafLightLibFuncSetBusParams(leafLightCanHandle, frequency, tseg1, tseg2, 
                                       sjw, noSamp, 0) != canOK)
      {
        result = false;
      }
    }
    /* Set output control to the default normal mode. */
    if (result)
    {
      if (LeafLightLibFuncSetBusOutputControl(leafLightCanHandle, 
                                              canDRIVER_NORMAL) != canOK)
      {
        result = false;
      }
    }
    /* Configure reception acceptance filter. */
    if ( (result) && (leafLightSettings.mask != 0x00000000u) )
    {
      /* Start out by closing the acceptance filters first. */
      if (LeafLightLibFuncSetAcceptanceFilter(leafLightCanHandle, 0x00000000u, 
                                              0x00000000u, 0) != canOK)
      {
        result = false;
      }
      if (result)
      {
        if (LeafLightLibFuncSetAcceptanceFilter(leafLightCanHandle, 0x00000000u, 
                                                0x00000000u, 1) != canOK)
        {
          result = false;
        }
      }
      if (result)
      {
        /* Use bit logic to determine if the filter should accept standard 11-bit and/or
         * extended 29-bit identifiers:
         *   acceptStdId = ((mask & code & CAN_MSG_EXT_ID_MASK) == 0)
         *   acceptExtId = ((mask & code & CAN_MSG_EXT_ID_MASK) != 0) ||
         *                 ((mask & CAN_MSG_EXT_ID_MASK) == 0)
         */
        bool acceptStdID = ((leafLightSettings.mask & leafLightSettings.code & CAN_MSG_EXT_ID_MASK) == 0);
        bool acceptExtID = ((leafLightSettings.mask & leafLightSettings.code & CAN_MSG_EXT_ID_MASK) != 0) ||
          ((leafLightSettings.mask & CAN_MSG_EXT_ID_MASK) == 0);
        /* Configure acceptance filter for standard 11-bit identifiers. */
        if (acceptStdID)
        {
          if (LeafLightLibFuncSetAcceptanceFilter(leafLightCanHandle,
            leafLightSettings.code & 0x1fffffffu,
            leafLightSettings.mask & 0x1fffffffu,
            0) != canOK)
          {
            result = false;
          }
        }
        /* Configure acceptance filter for extended 29-bit identifiers. */
        if ((acceptExtID) && (result))
        {
          if (LeafLightLibFuncSetAcceptanceFilter(leafLightCanHandle,
            leafLightSettings.code & 0x1fffffffu,
            leafLightSettings.mask & 0x1fffffffu,
            1) != canOK)
          {
            result = false;
          }
        }
      }
    }
    /* Go on the bus. */
    if (result)
    {
      if (LeafLightLibFuncBusOn(leafLightCanHandle) != canOK)
      {
        result = false;
      }
    }
    /* Obtain the handle for CAN events. */
    if (result)
    {
      leafLightCanEvent = NULL;
      if (LeafLightLibFuncIoCtl(leafLightCanHandle, canIOCTL_GET_EVENTHANDLE,
                                &leafLightCanEvent, sizeof(leafLightCanEvent)) != canOK)
      {
        result = false;
      }
      /* Validate the handle. */
      if (leafLightCanEvent == NULL)
      {
        result = false;
      }
    }
    /* Create the terminate event handle used in the reception thread. */
    if (result)
    {
      leafLightTerminateEvent = CreateEvent(NULL, TRUE, FALSE, "");
      if (leafLightTerminateEvent == NULL)
      {
        result = false;
      }
    }
    /* Start the reception thread as the last step. */
    if (result)
    {
      leafLightRxThreadHandle = CreateThread(NULL, 0, LeafLightReceptionThread,
                                           NULL, 0, NULL);
      if (leafLightRxThreadHandle == NULL)
      {
        result = false;
      }
    }
  }

  /* Clean-up in case an error occurred. */
  if (!result)
  {
    if (leafLightCanHandle >= 0)
    {
      /* Go off the bus and close the channel. */
      (void)LeafLightLibFuncBusOff(leafLightCanHandle);
      (void)LeafLightLibFuncClose(leafLightCanHandle);
      leafLightCanHandle = -1;
    }
    if (leafLightTerminateEvent != NULL)
    {
      /* Close the event handle. */
      (void)CloseHandle(leafLightTerminateEvent);
      leafLightTerminateEvent = NULL;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of LeafLightConnect ***/


/************************************************************************************//**
** \brief     Disconnects the CAN interface.
**
****************************************************************************************/
static void LeafLightDisconnect(void)
{
   /* Stop the reception thread. */
  if (leafLightRxThreadHandle != NULL)
  {
    /* Trigger event to request the reception thread to stop. */
    (void)SetEvent(leafLightTerminateEvent);
    /* Wait for the thread to signal termination. */
    (void)WaitForSingleObject(leafLightRxThreadHandle, INFINITE);
    /* Close the thread handle. */
    (void)CloseHandle(leafLightRxThreadHandle);
    leafLightRxThreadHandle = NULL;
  }
  /* Close the terminate event handle. */
  if (leafLightTerminateEvent != NULL)
  {
    (void)CloseHandle(leafLightTerminateEvent);
    leafLightTerminateEvent = NULL;
  }
  /* Go off the bus and close the channel. */
  if (leafLightCanHandle >= 0)
  {
    (void)LeafLightLibFuncBusOff(leafLightCanHandle);
    (void)LeafLightLibFuncClose(leafLightCanHandle);
    leafLightCanHandle = -1;
  }
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
  tCanEvents const * pEvents;
  int32_t txId;
  uint32_t txFlags;
  uint8_t txData[CAN_MSG_MAX_LEN];

  /* Check parameters. */
  assert(msg != NULL);

  /* Only continue with valid parameters and handle. */
  if ( (msg != NULL) && (leafLightCanHandle >= 0) ) /*lint !e774 */
  {
    /* Prepare message. */
    if ((msg->id & CAN_MSG_EXT_ID_MASK) == 0)
    {
      txId = msg->id & 0x7ffu;
      txFlags = canMSG_STD;
    }
    else
    {
      txId = msg->id & 0x1fffffffu;
      txFlags = canMSG_EXT;
    }
    for (uint8_t idx = 0; idx < msg->dlc; idx++)
    {
      txData[idx] = msg->data[idx];
    }
    /* Submit CAN message for transmission. */
    if (LeafLightLibFuncWrite(leafLightCanHandle, txId, txData, msg->dlc, txFlags) == canOK)
    {
      /* Update result value to success. */
      result = true;
      /* Trigger transmit complete event(s). */
      pEvents = leafLightEventsList;
      for (uint32_t idx = 0; idx < leafLightEventsEntries; idx++)
      {
        if (pEvents != NULL)
        {
          if (pEvents->MsgTxed != NULL)
          {
            pEvents->MsgTxed(msg);
          }
          /* Move on to the next entry in the list. */
          pEvents++;
        }
      }
    }
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
  uint32_t statusFlags;

  /* Only continue with valid handle. */
  if (leafLightCanHandle >= 0)
  {
    /* Read the status flags. */
    if (LeafLightLibFuncReadStatus(leafLightCanHandle, &statusFlags) == canOK)
    {
      /* Check for bus off or error passive bits. */
      if ((statusFlags & (canSTAT_BUS_OFF | canSTAT_ERROR_PASSIVE)) != 0)
      {
        result = true;
      }
    }
  }
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


/************************************************************************************//**
** \brief     CAN message reception thread.
** \param     pv Pointer to thread parameters.
** \return    Thread exit code.
**
****************************************************************************************/
static DWORD WINAPI LeafLightReceptionThread(LPVOID pv)
{
  DWORD waitResult;
  HANDLE handles[] = 
  { 
    leafLightCanEvent, 
    leafLightTerminateEvent 
  };
  bool running = true;
  int32_t rxId;
  uint32_t rxDlc;
  uint32_t rxTime;
  uint32_t rxFlags;
  tCanMsg rxMsg;
  canStatus rxStatus;
  tCanEvents const * pEvents;

  /* Parameter not used. */
  (void)pv;

  /* Enter thread's infinite loop. */
  while (running)
  {
    waitResult = WaitForMultipleObjects(sizeof(handles)/sizeof(handles[0]), handles,
                                        FALSE, INFINITE);
    switch (waitResult) 
    {
      /* CAN reception event. */
      case WAIT_OBJECT_0 + 0: /*lint !e835 */
        /* Only read out the events when the handle is valid. */
        if (leafLightCanHandle >= 0)
        {
          /* Empty out the event queue. */
          do
          {
            rxStatus = LeafLightLibFuncRead(leafLightCanHandle, &rxId, &rxMsg.data[0], 
                                            &rxDlc, &rxFlags, &rxTime);
            /* Only process the result if a message was read. */
            if (rxStatus == canOK)
            {
              /* Only process STD and EXT messages. */
              if ((rxFlags & (canMSG_STD | canMSG_EXT)) != 0)
              {
                /* Convert the CAN message to the format of the CAN driver module. Note
                 * that the data was already copied by LeafLightLibFuncRead().
                 */
                rxMsg.id = (uint32_t)rxId;
                if ((rxFlags & canMSG_EXT) != 0)
                {
                  rxMsg.id |= CAN_MSG_EXT_ID_MASK;
                }
                rxMsg.dlc = (uint8_t)rxDlc;

                /* Trigger message reception event(s). */
                pEvents = leafLightEventsList;
                for (uint32_t idx = 0; idx < leafLightEventsEntries; idx++)
                {
                  if (pEvents != NULL)
                  {
                    if (pEvents->MsgRxed != NULL)
                    {
                      pEvents->MsgRxed(&rxMsg);
                    }
                    /* Move on to the next entry in the list. */
                    pEvents++;
                  }
                }
              }
            }
          } 
          while (rxStatus != canERR_NOMSG);
        }
        break;

      /* Termination event. */
      default:
      case WAIT_OBJECT_0 + 1: /*lint !e835 */
        /* Stop thread. */
        running = false;
        break;
    }
  }
  /* Exit thread. */
  return 0;
} /*** end of LeafLightReceptionThread ***/


/************************************************************************************//**
** \brief     Loads the Kvaser CANLIB DLL and initializes the API function pointers.
**
****************************************************************************************/
static void LeafLightLibLoadDll(void)
{
  /* Reset the channel handle. */
  leafLightCanHandle = -1;
  /* Start out by resetting the API function pointers. */
  leafLightLibFuncInitializeLibraryPtr = NULL;
  leafLightLibFuncUnloadLibraryPtr = NULL;
  leafLightLibFuncOpenChannelPtr = NULL;
  leafLightLibFuncSetBusParamsPtr = NULL;
  leafLightLibFuncSetBusOutputControlPtr = NULL;
  leafLightLibFuncSetAcceptanceFilterPtr = NULL;
  leafLightLibFuncIoCtlPtr = NULL;
  leafLightLibFuncBusOnPtr = NULL;
  leafLightLibFuncWritePtr = NULL;
  leafLightLibFuncReadPtr = NULL;
  leafLightLibFuncReadStatusPtr = NULL;
  leafLightLibFuncBusOffPtr = NULL;
  leafLightLibFuncClosePtr = NULL;

  /* Attempt to load the library and obtain a handle to it. */
  leafLightDllHandle = LoadLibrary("canlib32");

  /* Assert libary handle. */
  assert(leafLightDllHandle != NULL);

  /* Only continue if the library was successfully loaded */
  if (leafLightDllHandle != NULL) /*lint !e774 */
  {
    /* Set canInitializeLibrary function pointer. */
    leafLightLibFuncInitializeLibraryPtr = (tLeafLightLibFuncInitializeLibrary)GetProcAddress(leafLightDllHandle, "canInitializeLibrary");
    /* Set canUnloadLibrary function pointer. */
    leafLightLibFuncUnloadLibraryPtr = (tLeafLightLibFuncUnloadLibrary)GetProcAddress(leafLightDllHandle, "canUnloadLibrary");
    /* Set canOpenChannel function pointer. */
    leafLightLibFuncOpenChannelPtr = (tLeafLightLibFuncOpenChannel)GetProcAddress(leafLightDllHandle, "canOpenChannel");
    /* Set canSetBusParams function pointer. */
    leafLightLibFuncSetBusParamsPtr = (tLeafLightLibFuncSetBusParams)GetProcAddress(leafLightDllHandle, "canSetBusParams");
    /* Set canSetBusOutputControl function pointer. */
    leafLightLibFuncSetBusOutputControlPtr = (tLeafLightLibFuncSetBusOutputControl)GetProcAddress(leafLightDllHandle, "canSetBusOutputControl");
    /* Set canAccept function pointer. */
    leafLightLibFuncSetAcceptanceFilterPtr = (tLeafLightLibFuncSetAcceptanceFilter)GetProcAddress(leafLightDllHandle, "canSetAcceptanceFilter");
    /* Set canIoCtl function pointer. */
    leafLightLibFuncIoCtlPtr = (tLeafLightLibFuncIoCtl)GetProcAddress(leafLightDllHandle, "canIoCtl");
    /* Set canBusOn function pointer. */
    leafLightLibFuncBusOnPtr = (tLeafLightLibFuncBusOn)GetProcAddress(leafLightDllHandle, "canBusOn");
    /* Set canWrite function pointer. */
    leafLightLibFuncWritePtr = (tLeafLightLibFuncWrite)GetProcAddress(leafLightDllHandle, "canWrite");
    /* Set canRead function pointer. */
    leafLightLibFuncReadPtr = (tLeafLightLibFuncRead)GetProcAddress(leafLightDllHandle, "canRead");
    /* Set canReadStatus function pointer. */
    leafLightLibFuncReadStatusPtr = (tLeafLightLibFuncReadStatus)GetProcAddress(leafLightDllHandle, "canReadStatus");
    /* Set canBusOff function pointer. */
    leafLightLibFuncBusOffPtr = (tLeafLightLibFuncBusOff)GetProcAddress(leafLightDllHandle, "canBusOff");
    /* Set canClose function pointer. */
    leafLightLibFuncClosePtr = (tLeafLightLibFuncClose)GetProcAddress(leafLightDllHandle, "canClose");
  }
} /*** end of LeafLightLibLoadDll ***/


/************************************************************************************//**
** \brief     Unloads the Kvaser CANLIB DLL and resets the API function pointers.
**
****************************************************************************************/
static void LeafLightLibUnloadDll(void)
{
  /* Reset the API function pointers. */
  leafLightLibFuncInitializeLibraryPtr = NULL;
  leafLightLibFuncUnloadLibraryPtr = NULL;
  leafLightLibFuncOpenChannelPtr = NULL;
  leafLightLibFuncSetBusParamsPtr = NULL;
  leafLightLibFuncSetBusOutputControlPtr = NULL;
  leafLightLibFuncSetAcceptanceFilterPtr = NULL;
  leafLightLibFuncIoCtlPtr = NULL;
  leafLightLibFuncBusOnPtr = NULL;
  leafLightLibFuncWritePtr = NULL;
  leafLightLibFuncReadPtr = NULL;
  leafLightLibFuncReadStatusPtr = NULL;
  leafLightLibFuncBusOffPtr = NULL;
  leafLightLibFuncClosePtr = NULL;
  /* Reset the channel handle. */
  leafLightCanHandle = -1;
  /* Unload the library and invalidate its handle. */
  if (leafLightDllHandle != NULL) 
  {
    (void)FreeLibrary(leafLightDllHandle);
    leafLightDllHandle = NULL;
  }
} /*** end of LeafLightLibUnloadDll **/


/************************************************************************************//**
** \brief     This function must be called before any other functions is used. It will 
**            initialize the driver.
**
****************************************************************************************/
static void LeafLightLibFuncInitializeLibrary(void)
{
  /* Check function pointer and library handle. */
  assert(leafLightLibFuncInitializeLibraryPtr != NULL);
  assert(leafLightDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((leafLightLibFuncInitializeLibraryPtr != NULL) && (leafLightDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    leafLightLibFuncInitializeLibraryPtr();
  }
} /*** end of LeafLightLibFuncInitializeLibrary ***/


/************************************************************************************//**
** \brief     Frees allocated memory, unload the DLLs canlib32.dll has loaded and de-
**            initializes data structures.
** \return    canOK if successful, canERR_xxx otherwise.
**
****************************************************************************************/
static canStatus LeafLightLibFuncUnloadLibrary(void)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(leafLightLibFuncUnloadLibraryPtr != NULL);
  assert(leafLightDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((leafLightLibFuncUnloadLibraryPtr != NULL) && (leafLightDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = leafLightLibFuncUnloadLibraryPtr();
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of LeafLightLibFuncUnloadLibrary ***/


/************************************************************************************//**
** \brief     Opens a CAN channel and returns a handle which is used in subsequent calls.
** \param     channel The number of the channel. Channel numbering is hardware dependent.
** \param     flags A combination of canOPEN_xxx flags.
** \return    Handle (positive) to the channel if successful, canERR_xxx (negative) 
**            otherwise.
**
****************************************************************************************/
static CanHandle LeafLightLibFuncOpenChannel(int32_t channel, int32_t flags)
{
  canHandle result = (canHandle)canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(leafLightLibFuncOpenChannelPtr != NULL);
  assert(leafLightDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((leafLightLibFuncOpenChannelPtr != NULL) && (leafLightDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = leafLightLibFuncOpenChannelPtr(channel, flags);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of LeafLightLibFuncOpenChannel ***/


/************************************************************************************//**
** \brief     This function sets the nominal bus timing parameters for the specified CAN
**            controller. The library provides default values for tseg1, tseg2, sjw and
**            noSamp when freq is specified to one of the pre-defined constants, 
**            canBITRATE_xxx for classic CAN and canFD_BITRATE_xxx for CAN FD.
** \param     hnd An open handle to a CAN controller.
** \param     freq Bit rate (measured in bits per second); or one of the predefined 
**            constants (canBITRATE_xxx for classic CAN and canFD_BITRATE_xxx for 
**            CAN FD).
** \param     tseg1 Time segment 1, that is, the number of quanta from (but not 
**            including) the Sync Segment to the sampling point.
** \param     tseg2 Time segment 2, that is, the number of quanta from the sampling point
**            to the end of the bit.
** \param     sjw The Synchronization Jump Width.
** \param     noSamp The number of sampling points; can be 1 or 3.
** \param     syncmode Unsupported and ignored.
** \return    canOK if successful, canERR_xxx otherwise.
**
****************************************************************************************/
static canStatus LeafLightLibFuncSetBusParams(const CanHandle hnd, int32_t freq, 
                                              uint32_t tseg1, uint32_t tseg2, 
                                              uint32_t sjw, uint32_t noSamp, 
                                              uint32_t syncmode)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(leafLightLibFuncSetBusParamsPtr != NULL);
  assert(leafLightDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((leafLightLibFuncSetBusParamsPtr != NULL) && (leafLightDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = leafLightLibFuncSetBusParamsPtr(hnd, freq, tseg1, tseg2, sjw, noSamp, syncmode);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of LeafLightLibFuncSetBusParams ***/


/************************************************************************************//**
** \brief     This function sets the driver type for a CAN controller. This corresponds 
**            loosely to the bus output control register in the CAN controller, hence the
**            name of this function. CANLIB does not allow for direct manipulation of the
**            bus output control register; instead, symbolic constants are used to select
**            the desired driver type.
** \param     hnd A handle to an open circuit. 
** \param     drivertype Can driver type (canDRIVER_xxx).
** \return    canOK if successful, canERR_xxx otherwise.
**
****************************************************************************************/
static canStatus LeafLightLibFuncSetBusOutputControl(const CanHandle hnd, 
                                                     const uint32_t drivertype)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(leafLightLibFuncSetBusOutputControlPtr != NULL);
  assert(leafLightDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((leafLightLibFuncSetBusOutputControlPtr != NULL) && (leafLightDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = leafLightLibFuncSetBusOutputControlPtr(hnd, drivertype);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of LeafLightLibFuncSetBusOutputControl ***/


/************************************************************************************//**
** \brief     This routine sets the message acceptance filters on a CAN channel.
** \param     hnd A handle to an open circuit.
** \param     code The acceptance code to set.
** \param     mask The acceptance mask to set
** \param     is_extended Select 29-bit CAN identifiers.
** \return    canOK if successful, canERR_xxx otherwise.
**
****************************************************************************************/
static canStatus LeafLightLibFuncSetAcceptanceFilter(const CanHandle hnd, uint32_t code,
                                                     uint32_t mask, int32_t is_extended)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(leafLightLibFuncSetAcceptanceFilterPtr != NULL);
  assert(leafLightDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((leafLightLibFuncSetAcceptanceFilterPtr != NULL) && (leafLightDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = leafLightLibFuncSetAcceptanceFilterPtr(hnd, code, mask, is_extended);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of LeafLightLibFuncSetAcceptanceFilter ***/


/************************************************************************************//**
** \brief     This API call performs several different functions; these are described 
**            below. The functions are handle-specific unless otherwise noted; this means
**            that they affect only the handle you pass to canIoCtl(), whereas other open
**            handles will remain unaffected. The contents of buf after the call is
**            dependent on the function code you specified.
** \param     hnd A handle to an open circuit.
** \param     func A canIOCTL_xxx function code.
** \param     buf Pointer to a buffer containing function-dependent data; or a NULL
**            pointer for certain function codes. The buffer can be used for both input
**            and output depending on the function code. See canIOCTL_xxx.
** \param     buflen The length of the buffer.
** \return    canOK if successful, canERR_xxx otherwise.
**
****************************************************************************************/
static canStatus LeafLightLibFuncIoCtl(const CanHandle hnd, uint32_t func, void * buf, 
                                       uint32_t buflen)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(leafLightLibFuncIoCtlPtr != NULL);
  assert(leafLightDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((leafLightLibFuncIoCtlPtr != NULL) && (leafLightDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = leafLightLibFuncIoCtlPtr(hnd, func, buf, buflen);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of LeafLightLibFuncIoCtl ***/


/************************************************************************************//**
** \brief     Takes the specified channel on-bus.
** \param     hnd An open handle to a CAN channel.
** \return    canOK if successful, canERR_xxx otherwise.
**
****************************************************************************************/
static canStatus LeafLightLibFuncBusOn(const CanHandle hnd)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(leafLightLibFuncBusOnPtr != NULL);
  assert(leafLightDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((leafLightLibFuncBusOnPtr != NULL) && (leafLightDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = leafLightLibFuncBusOnPtr(hnd);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of LeafLightLibFuncBusOn ***/


/************************************************************************************//**
** \brief     This function sends a CAN message. The call returns immediately after 
**            queuing the message to the driver.
** \param     hnd A handle to an open CAN circuit.
** \param     id The identifier of the CAN message to send.
** \param     msg A pointer to the message data, or NULL.
** \param     dlc The length of the message in bytes.
** \param     flag A combination of message flags, canMSG_xxx (including canFDMSG_xxx if
**            the CAN FD protocol is enabled). Use this parameter to send extended
**            (29-bit) frames and/or remote frames. Use canMSG_EXT and/or canMSG_RTR for
**            this purpose. 
** \return    canOK if successful, canERR_xxx otherwise.
**
****************************************************************************************/
static canStatus LeafLightLibFuncWrite(const CanHandle hnd, int32_t id, void * msg, 
                                       uint32_t dlc, uint32_t flag)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(leafLightLibFuncWritePtr != NULL);
  assert(leafLightDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((leafLightLibFuncWritePtr != NULL) && (leafLightDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = leafLightLibFuncWritePtr(hnd, id, msg, dlc, flag);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of LeafLightLibFuncWrite ***/


/************************************************************************************//**
** \brief     Reads a message from the receive buffer. If no message is available, the 
**            function returns immediately with return code canERR_NOMSG.
** \param     hnd A handle to an open circuit.
** \param     id Pointer to a buffer which receives the CAN identifier. This buffer will
**            only get the identifier. To determine whether this identifier was standard
**            (11-bit) or extended (29-bit), and/or whether it was remote or not, or if
**            it was an error frame, examine the contents of the flag argument.
** \param     msg Pointer to the buffer which receives the message data. This buffer must
**            be large enough (i.e. 8 bytes.) Only the message data is copied; the rest
**            of the buffer is left as-is. 
** \param     dlc Pointer to a buffer which receives the message length.
** \param     flag Pointer to a buffer which receives the message flags, which is a
**            combination of the canMSG_xxx and canMSGERR_xxx values.
** \param     time Pointer to a buffer which receives the message time stamp.
** \return    canOK if successful, canERR_xxx otherwise.
**
****************************************************************************************/
static canStatus LeafLightLibFuncRead(const CanHandle hnd, int32_t * id, void * msg, 
                                      uint32_t * dlc, uint32_t * flag, uint32_t * time)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(leafLightLibFuncReadPtr != NULL);
  assert(leafLightDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((leafLightLibFuncReadPtr != NULL) && (leafLightDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = leafLightLibFuncReadPtr(hnd, id, msg, dlc, flag, time);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of LeafLightLibFuncRead ***/


/************************************************************************************//**
** \brief     Returns the status for the specified circuit. flags points to a longword
**            which receives a combination of the canSTAT_xxx flags.
** \param     hnd A handle to an open circuit.
** \param     flags Pointer to a DWORD which receives the status flags; this is a
**            combination of any of the canSTAT_xxx. 
** \return    canOK if successful, canERR_xxx otherwise.
**
****************************************************************************************/
static canStatus LeafLightLibFuncReadStatus(const CanHandle hnd, uint32_t * const flags)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(leafLightLibFuncReadStatusPtr != NULL);
  assert(leafLightDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((leafLightLibFuncReadStatusPtr != NULL) && (leafLightDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = leafLightLibFuncReadStatusPtr(hnd, flags);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of LeafLightLibFuncReadStatus ***/


/************************************************************************************//**
** \brief     Takes the specified channel off-bus.
** \param     hnd A handle to an open circuit.
** \return    canOK if successful, canERR_xxx otherwise.
**
****************************************************************************************/
static canStatus LeafLightLibFuncBusOff(const CanHandle hnd)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(leafLightLibFuncBusOffPtr != NULL);
  assert(leafLightDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((leafLightLibFuncBusOffPtr != NULL) && (leafLightDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = leafLightLibFuncBusOffPtr(hnd);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of LeafLightLibFuncBusOff ***/


/************************************************************************************//**
** \brief     Closes the channel associated with the handle. If no other threads are
**            using the CAN circuit, it is taken off bus. The handle can not be used for
**            further references to the channel, so any variable containing it should be
**            zeroed.
** \param     hnd A handle to an open circuit.
** \return    canOK if successful, canERR_xxx otherwise.
**
****************************************************************************************/
static canStatus LeafLightLibFuncClose(const CanHandle hnd)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(leafLightLibFuncClosePtr != NULL);
  assert(leafLightDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((leafLightLibFuncClosePtr != NULL) && (leafLightDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = leafLightLibFuncClosePtr(hnd);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of LeafLightLibFuncClose ***/


/*********************************** end of leaflight.c ********************************/

