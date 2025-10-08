/************************************************************************************//**
* \file         kvcanlib.c
* \brief        Kvaser CANLIB SDK API interfacae source file.
* \ingroup      Kvaser_CanLib
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
#include "kvcanlib.h"                       /* Kvaser CANLIB SDK API                   */
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
typedef void      (__stdcall * tKvaserCanLibFuncInitializeLibrary)(void); 
typedef canStatus (__stdcall * tKvaserCanLibFuncUnloadLibrary)(void); 
typedef CanHandle (__stdcall * tKvaserCanLibFuncOpenChannel)(int32_t channel, int32_t flags);
typedef canStatus (__stdcall * tKvaserCanLibFuncSetBusParams)(const CanHandle hnd, int32_t freq, uint32_t tseg1, uint32_t tseg2, uint32_t sjw, uint32_t noSamp, uint32_t syncmode);
typedef canStatus (__stdcall * tKvaserCanLibFuncSetBusOutputControl)(const CanHandle hnd, const uint32_t drivertype);
typedef canStatus (__stdcall * tKvaserCanLibFuncSetAcceptanceFilter)(const CanHandle hnd, uint32_t code, uint32_t mask, int32_t is_extended);
typedef canStatus (__stdcall * tKvaserCanLibFuncIoCtl)(const CanHandle hnd, uint32_t func, void * buf, uint32_t buflen);
typedef canStatus (__stdcall * tKvaserCanLibFuncBusOn)(const CanHandle hnd);
typedef canStatus (__stdcall * tKvaserCanLibFuncWrite)(const CanHandle hnd, int32_t id, void * msg, uint32_t dlc, uint32_t flag);
typedef canStatus (__stdcall * tKvaserCanLibFuncRead)(const CanHandle hnd, int32_t * id, void * msg, uint32_t * dlc, uint32_t * flag, uint32_t * time);
typedef canStatus (__stdcall * tKvaserCanLibFuncReadStatus)(const CanHandle hnd, uint32_t * const flags);
typedef canStatus (__stdcall * tKvaserCanLibFuncBusOff)(const CanHandle hnd);
typedef canStatus (__stdcall * tKvaserCanLibFuncClose)(const CanHandle hnd);


/***************************************************************************************
* Function prototypes
****************************************************************************************/
/* CAN interface functions. */
static void KvaserCanLibInit(tCanSettings const * settings);
static void KvaserCanLibTerminate(void);
static bool KvaserCanLibConnect(void);
static void KvaserCanLibDisconnect(void);
static bool KvaserCanLibTransmit(tCanMsg const * msg);
static bool KvaserCanLibIsBusError(void);
static void KvaserCanLibRegisterEvents(tCanEvents const * events);
/* CAN message reception thread. */
static DWORD WINAPI KvaserCanLibReceptionThread(LPVOID pv);
/* Kvaser CANLIB library handling. */
static void KvaserCanLibLoadDll(void);
static void KvaserCanLibUnloadDll(void);
static void KvaserCanLibFuncInitializeLibrary(void); 
static canStatus KvaserCanLibFuncUnloadLibrary(void); 
static CanHandle KvaserCanLibFuncOpenChannel(int32_t channel, int32_t flags);
static canStatus KvaserCanLibFuncSetBusParams(const CanHandle hnd, int32_t freq, 
                                              uint32_t tseg1, uint32_t tseg2, 
                                              uint32_t sjw, uint32_t noSamp, 
                                              uint32_t syncmode);
static canStatus KvaserCanLibFuncSetBusOutputControl(const CanHandle hnd, 
                                                     const uint32_t drivertype);
static canStatus KvaserCanLibFuncSetAcceptanceFilter(const CanHandle hnd, uint32_t code,
                                                     uint32_t mask, int32_t is_extended);
static canStatus KvaserCanLibFuncIoCtl(const CanHandle hnd, uint32_t func, void * buf, 
                                       uint32_t buflen);
static canStatus KvaserCanLibFuncBusOn(const CanHandle hnd);
static canStatus KvaserCanLibFuncWrite(const CanHandle hnd, int32_t id, void * msg, 
                                       uint32_t dlc, uint32_t flag);
static canStatus KvaserCanLibFuncRead(const CanHandle hnd, int32_t * id, void * msg, 
                                      uint32_t * dlc, uint32_t * flag, uint32_t * time);
static canStatus KvaserCanLibFuncReadStatus(const CanHandle hnd, uint32_t * const flags);
static canStatus KvaserCanLibFuncBusOff(const CanHandle hnd);
static canStatus KvaserCanLibFuncClose(const CanHandle hnd);


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief CAN interface structure filled with Kvaser CANLIB SDK interfac driver
 *         specifics.
 */
static const tCanInterface kvaserCanLibInterface =
{
  KvaserCanLibInit,
  KvaserCanLibTerminate,
  KvaserCanLibConnect,
  KvaserCanLibDisconnect,
  KvaserCanLibTransmit,
  KvaserCanLibIsBusError,
  KvaserCanLibRegisterEvents
};


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief The settings to use in this CAN interface. */
static tCanSettings kvaserCanLibSettings;

/** \brief List with callback functions that this driver should use. */
static tCanEvents * kvaserCanLibEventsList;

/** \brief Total number of event entries into the \ref kvaserCanLibEventsList list. */
static uint32_t kvaserCanLibEventsEntries;

/** \brief Handle to the Kvaser CANLIB dynamic link library. */
static HINSTANCE kvaserCanLibDllHandle;

/** \brief Handle to the CAN channel. */
static CanHandle kvaserCanLibCanHandle;

/** \brief Handle to the CAN channel for usage in the CAN reception thread. */
static CanHandle kvaserCanLibRxCanHandle;

/** \brief Function pointer to the Kvaser CANLIB canInitializeLibrary function. */
static tKvaserCanLibFuncInitializeLibrary kvaserCanLibFuncInitializeLibraryPtr;

/** \brief Function pointer to the Kvaser CANLIB canUnloadLibrary function. */
static tKvaserCanLibFuncUnloadLibrary kvaserCanLibFuncUnloadLibraryPtr;

/** \brief Function pointer to the Kvaser CANLIB canOpenChannel function. */
static tKvaserCanLibFuncOpenChannel kvaserCanLibFuncOpenChannelPtr;

/** \brief Function pointer to the Kvaser CANLIB canSetBusParams function. */
static tKvaserCanLibFuncSetBusParams kvaserCanLibFuncSetBusParamsPtr;

/** \brief Function pointer to the Kvaser CANLIB canSetBusOutputControl function. */
static tKvaserCanLibFuncSetBusOutputControl kvaserCanLibFuncSetBusOutputControlPtr;

/** \brief Function pointer to the Kvaser CANLIB canSetAcceptanceFilter function. */
static tKvaserCanLibFuncSetAcceptanceFilter kvaserCanLibFuncSetAcceptanceFilterPtr;

/** \brief Function pointer to the Kvaser CANLIB canIoCtl function. */
static tKvaserCanLibFuncIoCtl kvaserCanLibFuncIoCtlPtr;

/** \brief Function pointer to the Kvaser CANLIB canBusOn function. */
static tKvaserCanLibFuncBusOn kvaserCanLibFuncBusOnPtr;

/** \brief Function pointer to the Kvaser CANLIB canWrite function. */
static tKvaserCanLibFuncWrite kvaserCanLibFuncWritePtr;

/** \brief Function pointer to the Kvaser CANLIB canRead function. */
static tKvaserCanLibFuncRead kvaserCanLibFuncReadPtr;

/** \brief Function pointer to the Kvaser CANLIB canReadStatus function. */
static tKvaserCanLibFuncReadStatus kvaserCanLibFuncReadStatusPtr;

/** \brief Function pointer to the Kvaser CANLIB canBusOff function. */
static tKvaserCanLibFuncBusOff kvaserCanLibFuncBusOffPtr;

/** \brief Function pointer to the Kvaser CANLIB canClose function. */
static tKvaserCanLibFuncClose kvaserCanLibFuncClosePtr;

/** \brief Handle for the event to terminate the reception thread. */
static HANDLE kvaserCanLibTerminateEvent;

/** \brief Handle for a CAN related event. */
static HANDLE kvaserCanLibCanEvent;

/** \brief Handle for the CAN reception thread. */
static HANDLE kvaserCanLibRxThreadHandle;


/***********************************************************************************//**
** \brief     Obtains a pointer to the CAN interface structure, so that it can be linked
**            to the generic CAN driver module.
** \return    Pointer to CAN interface structure.
**
****************************************************************************************/
tCanInterface const * KvaserCanLibGetInterface(void)
{
  return &kvaserCanLibInterface;
} /*** end of KvaserCanLibGetInterface ***/


/************************************************************************************//**
** \brief     Initializes the CAN interface.
** \param     settings Pointer to the CAN interface settings.
**
****************************************************************************************/
static void KvaserCanLibInit(tCanSettings const * settings)
{
  char * canDeviceName;

  /* Initialize locals. */
  kvaserCanLibEventsList = NULL;
  kvaserCanLibEventsEntries = 0;
  kvaserCanLibTerminateEvent = NULL;
  kvaserCanLibCanEvent = NULL;
  kvaserCanLibRxThreadHandle = NULL;
  kvaserCanLibDllHandle = NULL;
  kvaserCanLibCanHandle = -1;
  kvaserCanLibRxCanHandle = -1;
  /* Reset library function pointers. */
  kvaserCanLibFuncInitializeLibraryPtr = NULL;
  kvaserCanLibFuncUnloadLibraryPtr = NULL;
  kvaserCanLibFuncOpenChannelPtr = NULL;
  kvaserCanLibFuncSetBusParamsPtr = NULL;
  kvaserCanLibFuncSetBusOutputControlPtr = NULL;
  kvaserCanLibFuncSetAcceptanceFilterPtr = NULL;
  kvaserCanLibFuncIoCtlPtr = NULL;
  kvaserCanLibFuncBusOnPtr = NULL;
  kvaserCanLibFuncWritePtr = NULL;
  kvaserCanLibFuncReadPtr = NULL;
  kvaserCanLibFuncReadStatusPtr = NULL;
  kvaserCanLibFuncBusOffPtr = NULL;
  kvaserCanLibFuncClosePtr = NULL;
  /* Reset CAN interface settings. */
  kvaserCanLibSettings.devicename = "";
  kvaserCanLibSettings.channel = 0;
  kvaserCanLibSettings.baudrate = CAN_BR500K;
  kvaserCanLibSettings.code = 0x00000000u;
  kvaserCanLibSettings.mask = 0x00000000u;
  kvaserCanLibSettings.brsbaudrate = CANFD_DISABLED;

  /* Check parameters. */
  assert(settings != NULL);

  /* Only continue with valid parameters. */
  if (settings != NULL) /*lint !e774 */
  {
    /* Shallow copy the CAN interface settings for later usage. */
    kvaserCanLibSettings = *settings;
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
        kvaserCanLibSettings.devicename = canDeviceName;
      }
    }
    /* Perform initialization of Kvaser CANLIB SDK API. */
    KvaserCanLibLoadDll();
    KvaserCanLibFuncInitializeLibrary();
  }
} /*** end of KvaserCanLibInit ***/


/************************************************************************************//**
** \brief     Terminates the CAN interface.
**
****************************************************************************************/
static void KvaserCanLibTerminate(void)
{
  /* Perform termination of Kvaser CANLIB SDK API. */
  (void)KvaserCanLibFuncUnloadLibrary();
  KvaserCanLibUnloadDll();
  /* Release memory that was allocated for storing the device name. */
  if (kvaserCanLibSettings.devicename != NULL)
  {
    free((char *)kvaserCanLibSettings.devicename);
  }
  /* Reset CAN interface settings. */
  kvaserCanLibSettings.devicename = "";
  kvaserCanLibSettings.channel = 0;
  kvaserCanLibSettings.baudrate = CAN_BR500K;
  kvaserCanLibSettings.code = 0x00000000u;
  kvaserCanLibSettings.mask = 0x00000000u;
  kvaserCanLibSettings.brsbaudrate = CANFD_DISABLED;
  /* Release memory that was allocated for CAN events and reset the entry count. */
  if ( (kvaserCanLibEventsList != NULL) && (kvaserCanLibEventsEntries != 0) )
  {
    free(kvaserCanLibEventsList);
    kvaserCanLibEventsEntries = 0;
  }
} /*** end of KvaserCanLibTerminate ***/


/************************************************************************************//**
** \brief     Connects the CAN interface.
** \return    True if connected, false otherwise.
**
****************************************************************************************/
static bool KvaserCanLibConnect(void)
{
  bool result = false;
  bool baudrateSupported = true;
  int32_t frequency = 0;
  uint32_t tseg1 = 0;
  uint32_t tseg2 = 0;
  uint32_t sjw = 0;
  uint32_t noSamp = 0;

  /* This CAN driver does not support CAN FD mode. Cannot connect if CAN FD
   * mode was requested in the settings.
   */
  if (kvaserCanLibSettings.brsbaudrate != CANFD_DISABLED)
  {
    return false;
  }

  /* Convert the baudrate to a value supported by the PCAN-Basic API. */
  switch (kvaserCanLibSettings.baudrate)
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
  kvaserCanLibCanHandle = -1;
  kvaserCanLibRxCanHandle = -1;
  kvaserCanLibTerminateEvent = NULL;
  kvaserCanLibCanEvent = NULL;
  kvaserCanLibRxThreadHandle = NULL;

  /* Only continue with valid settings. */
  if (baudrateSupported)
  {
    /* Init result code to success and only negate it on detection of error. */
    result = true;
    /* Open the CAN channel with support for both 11- and 29-bit CAN identifiers, and 
     *  obtain its handle. 
     */
    kvaserCanLibCanHandle = KvaserCanLibFuncOpenChannel(0, canOPEN_REQUIRE_INIT_ACCESS |
                                                     canOPEN_REQUIRE_EXTENDED);
    /* Validate the handle. */
    if (kvaserCanLibCanHandle < 0)
    {
      result = false;
    }
    /* Configure the baudrate. */
    if (result)
    {
      if (KvaserCanLibFuncSetBusParams(kvaserCanLibCanHandle, frequency, tseg1, tseg2, 
                                       sjw, noSamp, 0) != canOK)
      {
        result = false;
      }
    }
    /* Set output control to the default normal mode. */
    if (result)
    {
      if (KvaserCanLibFuncSetBusOutputControl(kvaserCanLibCanHandle, 
                                              canDRIVER_NORMAL) != canOK)
      {
        result = false;
      }
    }
    /* Configure reception acceptance filter. */
    if ( (result) && (kvaserCanLibSettings.mask != 0x00000000u) )
    {
      /* Start out by closing the acceptance filters first. */
      if (KvaserCanLibFuncSetAcceptanceFilter(kvaserCanLibCanHandle, 0x00000000u, 
                                              0x00000000u, 0) != canOK)
      {
        result = false;
      }
      if (result)
      {
        if (KvaserCanLibFuncSetAcceptanceFilter(kvaserCanLibCanHandle, 0x00000000u, 
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
        bool acceptStdID = ((kvaserCanLibSettings.mask & kvaserCanLibSettings.code & CAN_MSG_EXT_ID_MASK) == 0);
        bool acceptExtID = ((kvaserCanLibSettings.mask & kvaserCanLibSettings.code & CAN_MSG_EXT_ID_MASK) != 0) ||
          ((kvaserCanLibSettings.mask & CAN_MSG_EXT_ID_MASK) == 0);
        /* Configure acceptance filter for standard 11-bit identifiers. */
        if (acceptStdID)
        {
          if (KvaserCanLibFuncSetAcceptanceFilter(kvaserCanLibCanHandle,
            kvaserCanLibSettings.code & 0x1fffffffu,
            kvaserCanLibSettings.mask & 0x1fffffffu,
            0) != canOK)
          {
            result = false;
          }
        }
        /* Configure acceptance filter for extended 29-bit identifiers. */
        if ((acceptExtID) && (result))
        {
          if (KvaserCanLibFuncSetAcceptanceFilter(kvaserCanLibCanHandle,
            kvaserCanLibSettings.code & 0x1fffffffu,
            kvaserCanLibSettings.mask & 0x1fffffffu,
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
      if (KvaserCanLibFuncBusOn(kvaserCanLibCanHandle) != canOK)
      {
        result = false;
      }
    }
    /* Open the CAN channel and obtain its handle, which will only be used in the
     * reception thread. CAN channel handles are not thread safe and therefore a
     * second handle is needed. Note that no init access is needed for this
     * handle.
     */
    kvaserCanLibRxCanHandle = KvaserCanLibFuncOpenChannel(0, canOPEN_NO_INIT_ACCESS);
    /* Validate the handle. */
    if (kvaserCanLibRxCanHandle < 0)
    {
      result = false;
    }
    /* Go on the bus. */
    if (result)
    {
      if (KvaserCanLibFuncBusOn(kvaserCanLibRxCanHandle) != canOK)
      {
        result = false;
      }
    }
    /* Obtain the handle for CAN events. */
    if (result)
    {
      kvaserCanLibCanEvent = NULL;
      if (KvaserCanLibFuncIoCtl(kvaserCanLibRxCanHandle, canIOCTL_GET_EVENTHANDLE,
                                &kvaserCanLibCanEvent, sizeof(kvaserCanLibCanEvent)) != canOK)
      {
        result = false;
      }
      /* Validate the handle. */
      if (kvaserCanLibCanEvent == NULL)
      {
        result = false;
      }
    }
    /* Create the terminate event handle used in the reception thread. */
    if (result)
    {
      kvaserCanLibTerminateEvent = CreateEvent(NULL, TRUE, FALSE, "");
      if (kvaserCanLibTerminateEvent == NULL)
      {
        result = false;
      }
    }
    /* Start the reception thread as the last step. */
    if (result)
    {
      kvaserCanLibRxThreadHandle = CreateThread(NULL, 0, KvaserCanLibReceptionThread,
                                           NULL, 0, NULL);
      if (kvaserCanLibRxThreadHandle == NULL)
      {
        result = false;
      }
    }
  }

  /* Clean-up in case an error occurred. */
  if (!result)
  {
    if (kvaserCanLibCanHandle >= 0)
    {
      /* Go off the bus and close the channel. */
      (void)KvaserCanLibFuncBusOff(kvaserCanLibCanHandle);
      (void)KvaserCanLibFuncClose(kvaserCanLibCanHandle);
      kvaserCanLibCanHandle = -1;
    }
    if (kvaserCanLibRxCanHandle >= 0)
    {
      /* Go off the bus and close the channel. */
      (void)KvaserCanLibFuncBusOff(kvaserCanLibRxCanHandle);
      (void)KvaserCanLibFuncClose(kvaserCanLibRxCanHandle);
      kvaserCanLibRxCanHandle = -1;
    }
    if (kvaserCanLibTerminateEvent != NULL)
    {
      /* Close the event handle. */
      (void)CloseHandle(kvaserCanLibTerminateEvent);
      kvaserCanLibTerminateEvent = NULL;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of KvaserCanLibConnect ***/


/************************************************************************************//**
** \brief     Disconnects the CAN interface.
**
****************************************************************************************/
static void KvaserCanLibDisconnect(void)
{
   /* Stop the reception thread. */
  if (kvaserCanLibRxThreadHandle != NULL)
  {
    /* Trigger event to request the reception thread to stop. */
    (void)SetEvent(kvaserCanLibTerminateEvent);
    /* Wait for the thread to signal termination. */
    (void)WaitForSingleObject(kvaserCanLibRxThreadHandle, INFINITE);
    /* Close the thread handle. */
    (void)CloseHandle(kvaserCanLibRxThreadHandle);
    kvaserCanLibRxThreadHandle = NULL;
  }
  /* Close the terminate event handle. */
  if (kvaserCanLibTerminateEvent != NULL)
  {
    (void)CloseHandle(kvaserCanLibTerminateEvent);
    kvaserCanLibTerminateEvent = NULL;
  }
  /* Go off the bus and close the channel. */
  if (kvaserCanLibCanHandle >= 0)
  {
    (void)KvaserCanLibFuncBusOff(kvaserCanLibCanHandle);
    (void)KvaserCanLibFuncClose(kvaserCanLibCanHandle);
    kvaserCanLibCanHandle = -1;
  }
  if (kvaserCanLibRxCanHandle >= 0)
  {
    (void)KvaserCanLibFuncBusOff(kvaserCanLibRxCanHandle);
    (void)KvaserCanLibFuncClose(kvaserCanLibRxCanHandle);
    kvaserCanLibRxCanHandle = -1;
  }
} /*** end of KvaserCanLibDisconnect ***/


/************************************************************************************//**
** \brief     Submits a message for transmission on the CAN bus.
** \param     msg Pointer to CAN message structure.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool KvaserCanLibTransmit(tCanMsg const * msg)
{
  bool result = false;
  tCanEvents const * pEvents;
  int32_t txId;
  uint32_t txFlags;
  uint8_t txData[CAN_MSG_MAX_LEN];

  /* Check parameters. */
  assert(msg != NULL);

  /* Only continue with valid parameters and handle. */
  if ( (msg != NULL) && (kvaserCanLibCanHandle >= 0) ) /*lint !e774 */
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
    for (uint8_t idx = 0; idx < msg->len; idx++)
    {
      txData[idx] = msg->data[idx];
    }
    /* Submit CAN message for transmission. */
    if (KvaserCanLibFuncWrite(kvaserCanLibCanHandle, txId, txData, msg->len, txFlags) == canOK)
    {
      /* Update result value to success. */
      result = true;
      /* Trigger transmit complete event(s). */
      pEvents = kvaserCanLibEventsList;
      for (uint32_t idx = 0; idx < kvaserCanLibEventsEntries; idx++)
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
} /*** end of KvaserCanLibTransmit ***/


/************************************************************************************//**
** \brief     Checks if a bus off or bus heavy situation occurred.
** \return    True if a bus error situation was detected, false otherwise.
**
****************************************************************************************/
static bool KvaserCanLibIsBusError(void)
{
  bool result = false;
  uint32_t statusFlags;

  /* Only continue with valid handle. */
  if (kvaserCanLibCanHandle >= 0)
  {
    /* Read the status flags. */
    if (KvaserCanLibFuncReadStatus(kvaserCanLibCanHandle, &statusFlags) == canOK)
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
} /*** end of KvaserCanLibIsBusError ***/


/************************************************************************************//**
** \brief     Registers the event callback functions that should be called by the CAN
**            interface.
** \param     events Pointer to structure with event callback function pointers.
**
****************************************************************************************/
static void KvaserCanLibRegisterEvents(tCanEvents const * events)
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
    kvaserCanLibEventsList = realloc(kvaserCanLibEventsList,
                                  (sizeof(tCanEvents) * (kvaserCanLibEventsEntries + 1)));
    /* Assert reallocation. */
    assert(kvaserCanLibEventsList != NULL);
    /* Only continue if reallocation was successful. */
    if (kvaserCanLibEventsList != NULL)
    {
      /* Increment events entry count. */
      kvaserCanLibEventsEntries++;
      /* Store the events in the new entry. */
      kvaserCanLibEventsList[kvaserCanLibEventsEntries - 1] = *events;
    }
    /* Reallocation failed. */
    else
    {
      /* Reset events entry count. */
      kvaserCanLibEventsEntries = 0;
    }
  }
} /*** end of KvaserCanLibRegisterEvents ***/


/************************************************************************************//**
** \brief     CAN message reception thread.
** \param     pv Pointer to thread parameters.
** \return    Thread exit code.
**
****************************************************************************************/
static DWORD WINAPI KvaserCanLibReceptionThread(LPVOID pv)
{
  DWORD waitResult;
  HANDLE handles[] = 
  { 
    kvaserCanLibCanEvent, 
    kvaserCanLibTerminateEvent 
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
        if (kvaserCanLibRxCanHandle >= 0)
        {
          /* Empty out the event queue. */
          do
          {
            rxStatus = KvaserCanLibFuncRead(kvaserCanLibRxCanHandle, &rxId, &rxMsg.data[0], 
                                            &rxDlc, &rxFlags, &rxTime);
            /* Only process the result if a message was read. */
            if (rxStatus == canOK)
            {
              /* Only process STD and EXT messages. */
              if ((rxFlags & (canMSG_STD | canMSG_EXT)) != 0)
              {
                /* Convert the CAN message to the format of the CAN driver module. Note
                 * that the data was already copied by KvaserCanLibFuncRead().
                 */
                rxMsg.id = (uint32_t)rxId;
                if ((rxFlags & canMSG_EXT) != 0)
                {
                  rxMsg.id |= CAN_MSG_EXT_ID_MASK;
                }
                rxMsg.len = (uint8_t)rxDlc;

                /* Trigger message reception event(s). */
                pEvents = kvaserCanLibEventsList;
                for (uint32_t idx = 0; idx < kvaserCanLibEventsEntries; idx++)
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
} /*** end of KvaserCanLibReceptionThread ***/


/************************************************************************************//**
** \brief     Loads the Kvaser CANLIB DLL and initializes the API function pointers.
**
****************************************************************************************/
static void KvaserCanLibLoadDll(void)
{
  /* Reset the channel handles. */
  kvaserCanLibCanHandle = -1;
  kvaserCanLibRxCanHandle = -1;
  /* Start out by resetting the API function pointers. */
  kvaserCanLibFuncInitializeLibraryPtr = NULL;
  kvaserCanLibFuncUnloadLibraryPtr = NULL;
  kvaserCanLibFuncOpenChannelPtr = NULL;
  kvaserCanLibFuncSetBusParamsPtr = NULL;
  kvaserCanLibFuncSetBusOutputControlPtr = NULL;
  kvaserCanLibFuncSetAcceptanceFilterPtr = NULL;
  kvaserCanLibFuncIoCtlPtr = NULL;
  kvaserCanLibFuncBusOnPtr = NULL;
  kvaserCanLibFuncWritePtr = NULL;
  kvaserCanLibFuncReadPtr = NULL;
  kvaserCanLibFuncReadStatusPtr = NULL;
  kvaserCanLibFuncBusOffPtr = NULL;
  kvaserCanLibFuncClosePtr = NULL;

  /* Attempt to load the library and obtain a handle to it. */
  kvaserCanLibDllHandle = LoadLibrary("canlib32");

  /* Assert libary handle. */
  assert(kvaserCanLibDllHandle != NULL);

  /* Only continue if the library was successfully loaded */
  if (kvaserCanLibDllHandle != NULL) /*lint !e774 */
  {
    /* Set canInitializeLibrary function pointer. */
    kvaserCanLibFuncInitializeLibraryPtr = (tKvaserCanLibFuncInitializeLibrary)GetProcAddress(kvaserCanLibDllHandle, "canInitializeLibrary");
    /* Set canUnloadLibrary function pointer. */
    kvaserCanLibFuncUnloadLibraryPtr = (tKvaserCanLibFuncUnloadLibrary)GetProcAddress(kvaserCanLibDllHandle, "canUnloadLibrary");
    /* Set canOpenChannel function pointer. */
    kvaserCanLibFuncOpenChannelPtr = (tKvaserCanLibFuncOpenChannel)GetProcAddress(kvaserCanLibDllHandle, "canOpenChannel");
    /* Set canSetBusParams function pointer. */
    kvaserCanLibFuncSetBusParamsPtr = (tKvaserCanLibFuncSetBusParams)GetProcAddress(kvaserCanLibDllHandle, "canSetBusParams");
    /* Set canSetBusOutputControl function pointer. */
    kvaserCanLibFuncSetBusOutputControlPtr = (tKvaserCanLibFuncSetBusOutputControl)GetProcAddress(kvaserCanLibDllHandle, "canSetBusOutputControl");
    /* Set canAccept function pointer. */
    kvaserCanLibFuncSetAcceptanceFilterPtr = (tKvaserCanLibFuncSetAcceptanceFilter)GetProcAddress(kvaserCanLibDllHandle, "canSetAcceptanceFilter");
    /* Set canIoCtl function pointer. */
    kvaserCanLibFuncIoCtlPtr = (tKvaserCanLibFuncIoCtl)GetProcAddress(kvaserCanLibDllHandle, "canIoCtl");
    /* Set canBusOn function pointer. */
    kvaserCanLibFuncBusOnPtr = (tKvaserCanLibFuncBusOn)GetProcAddress(kvaserCanLibDllHandle, "canBusOn");
    /* Set canWrite function pointer. */
    kvaserCanLibFuncWritePtr = (tKvaserCanLibFuncWrite)GetProcAddress(kvaserCanLibDllHandle, "canWrite");
    /* Set canRead function pointer. */
    kvaserCanLibFuncReadPtr = (tKvaserCanLibFuncRead)GetProcAddress(kvaserCanLibDllHandle, "canRead");
    /* Set canReadStatus function pointer. */
    kvaserCanLibFuncReadStatusPtr = (tKvaserCanLibFuncReadStatus)GetProcAddress(kvaserCanLibDllHandle, "canReadStatus");
    /* Set canBusOff function pointer. */
    kvaserCanLibFuncBusOffPtr = (tKvaserCanLibFuncBusOff)GetProcAddress(kvaserCanLibDllHandle, "canBusOff");
    /* Set canClose function pointer. */
    kvaserCanLibFuncClosePtr = (tKvaserCanLibFuncClose)GetProcAddress(kvaserCanLibDllHandle, "canClose");
  }
} /*** end of KvaserCanLibLoadDll ***/


/************************************************************************************//**
** \brief     Unloads the Kvaser CANLIB DLL and resets the API function pointers.
**
****************************************************************************************/
static void KvaserCanLibUnloadDll(void)
{
  /* Reset the API function pointers. */
  kvaserCanLibFuncInitializeLibraryPtr = NULL;
  kvaserCanLibFuncUnloadLibraryPtr = NULL;
  kvaserCanLibFuncOpenChannelPtr = NULL;
  kvaserCanLibFuncSetBusParamsPtr = NULL;
  kvaserCanLibFuncSetBusOutputControlPtr = NULL;
  kvaserCanLibFuncSetAcceptanceFilterPtr = NULL;
  kvaserCanLibFuncIoCtlPtr = NULL;
  kvaserCanLibFuncBusOnPtr = NULL;
  kvaserCanLibFuncWritePtr = NULL;
  kvaserCanLibFuncReadPtr = NULL;
  kvaserCanLibFuncReadStatusPtr = NULL;
  kvaserCanLibFuncBusOffPtr = NULL;
  kvaserCanLibFuncClosePtr = NULL;
  /* Reset the channel handles. */
  kvaserCanLibCanHandle = -1;
  kvaserCanLibRxCanHandle = -1;
  /* Unload the library and invalidate its handle. */
  if (kvaserCanLibDllHandle != NULL) 
  {
    (void)FreeLibrary(kvaserCanLibDllHandle);
    kvaserCanLibDllHandle = NULL;
  }
} /*** end of KvaserCanLibUnloadDll **/


/************************************************************************************//**
** \brief     This function must be called before any other functions is used. It will 
**            initialize the driver.
**
****************************************************************************************/
static void KvaserCanLibFuncInitializeLibrary(void)
{
  /* Check function pointer and library handle. */
  assert(kvaserCanLibFuncInitializeLibraryPtr != NULL);
  assert(kvaserCanLibDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((kvaserCanLibFuncInitializeLibraryPtr != NULL) && (kvaserCanLibDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    kvaserCanLibFuncInitializeLibraryPtr();
  }
} /*** end of KvaserCanLibFuncInitializeLibrary ***/


/************************************************************************************//**
** \brief     Frees allocated memory, unload the DLLs canlib32.dll has loaded and de-
**            initializes data structures.
** \return    canOK if successful, canERR_xxx otherwise.
**
****************************************************************************************/
static canStatus KvaserCanLibFuncUnloadLibrary(void)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(kvaserCanLibFuncUnloadLibraryPtr != NULL);
  assert(kvaserCanLibDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((kvaserCanLibFuncUnloadLibraryPtr != NULL) && (kvaserCanLibDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = kvaserCanLibFuncUnloadLibraryPtr();
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of KvaserCanLibFuncUnloadLibrary ***/


/************************************************************************************//**
** \brief     Opens a CAN channel and returns a handle which is used in subsequent calls.
** \param     channel The number of the channel. Channel numbering is hardware dependent.
** \param     flags A combination of canOPEN_xxx flags.
** \return    Handle (positive) to the channel if successful, canERR_xxx (negative) 
**            otherwise.
**
****************************************************************************************/
static CanHandle KvaserCanLibFuncOpenChannel(int32_t channel, int32_t flags)
{
  canHandle result = (canHandle)canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(kvaserCanLibFuncOpenChannelPtr != NULL);
  assert(kvaserCanLibDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((kvaserCanLibFuncOpenChannelPtr != NULL) && (kvaserCanLibDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = kvaserCanLibFuncOpenChannelPtr(channel, flags);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of KvaserCanLibFuncOpenChannel ***/


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
static canStatus KvaserCanLibFuncSetBusParams(const CanHandle hnd, int32_t freq, 
                                              uint32_t tseg1, uint32_t tseg2, 
                                              uint32_t sjw, uint32_t noSamp, 
                                              uint32_t syncmode)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(kvaserCanLibFuncSetBusParamsPtr != NULL);
  assert(kvaserCanLibDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((kvaserCanLibFuncSetBusParamsPtr != NULL) && (kvaserCanLibDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = kvaserCanLibFuncSetBusParamsPtr(hnd, freq, tseg1, tseg2, sjw, noSamp, syncmode);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of KvaserCanLibFuncSetBusParams ***/


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
static canStatus KvaserCanLibFuncSetBusOutputControl(const CanHandle hnd, 
                                                     const uint32_t drivertype)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(kvaserCanLibFuncSetBusOutputControlPtr != NULL);
  assert(kvaserCanLibDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((kvaserCanLibFuncSetBusOutputControlPtr != NULL) && (kvaserCanLibDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = kvaserCanLibFuncSetBusOutputControlPtr(hnd, drivertype);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of KvaserCanLibFuncSetBusOutputControl ***/


/************************************************************************************//**
** \brief     This routine sets the message acceptance filters on a CAN channel.
** \param     hnd A handle to an open circuit.
** \param     code The acceptance code to set.
** \param     mask The acceptance mask to set
** \param     is_extended Select 29-bit CAN identifiers.
** \return    canOK if successful, canERR_xxx otherwise.
**
****************************************************************************************/
static canStatus KvaserCanLibFuncSetAcceptanceFilter(const CanHandle hnd, uint32_t code,
                                                     uint32_t mask, int32_t is_extended)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(kvaserCanLibFuncSetAcceptanceFilterPtr != NULL);
  assert(kvaserCanLibDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((kvaserCanLibFuncSetAcceptanceFilterPtr != NULL) && (kvaserCanLibDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = kvaserCanLibFuncSetAcceptanceFilterPtr(hnd, code, mask, is_extended);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of KvaserCanLibFuncSetAcceptanceFilter ***/


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
static canStatus KvaserCanLibFuncIoCtl(const CanHandle hnd, uint32_t func, void * buf, 
                                       uint32_t buflen)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(kvaserCanLibFuncIoCtlPtr != NULL);
  assert(kvaserCanLibDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((kvaserCanLibFuncIoCtlPtr != NULL) && (kvaserCanLibDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = kvaserCanLibFuncIoCtlPtr(hnd, func, buf, buflen);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of KvaserCanLibFuncIoCtl ***/


/************************************************************************************//**
** \brief     Takes the specified channel on-bus.
** \param     hnd An open handle to a CAN channel.
** \return    canOK if successful, canERR_xxx otherwise.
**
****************************************************************************************/
static canStatus KvaserCanLibFuncBusOn(const CanHandle hnd)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(kvaserCanLibFuncBusOnPtr != NULL);
  assert(kvaserCanLibDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((kvaserCanLibFuncBusOnPtr != NULL) && (kvaserCanLibDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = kvaserCanLibFuncBusOnPtr(hnd);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of KvaserCanLibFuncBusOn ***/


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
static canStatus KvaserCanLibFuncWrite(const CanHandle hnd, int32_t id, void * msg, 
                                       uint32_t dlc, uint32_t flag)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(kvaserCanLibFuncWritePtr != NULL);
  assert(kvaserCanLibDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((kvaserCanLibFuncWritePtr != NULL) && (kvaserCanLibDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = kvaserCanLibFuncWritePtr(hnd, id, msg, dlc, flag);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of KvaserCanLibFuncWrite ***/


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
static canStatus KvaserCanLibFuncRead(const CanHandle hnd, int32_t * id, void * msg, 
                                      uint32_t * dlc, uint32_t * flag, uint32_t * time)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(kvaserCanLibFuncReadPtr != NULL);
  assert(kvaserCanLibDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((kvaserCanLibFuncReadPtr != NULL) && (kvaserCanLibDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = kvaserCanLibFuncReadPtr(hnd, id, msg, dlc, flag, time);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of KvaserCanLibFuncRead ***/


/************************************************************************************//**
** \brief     Returns the status for the specified circuit. flags points to a longword
**            which receives a combination of the canSTAT_xxx flags.
** \param     hnd A handle to an open circuit.
** \param     flags Pointer to a DWORD which receives the status flags; this is a
**            combination of any of the canSTAT_xxx. 
** \return    canOK if successful, canERR_xxx otherwise.
**
****************************************************************************************/
static canStatus KvaserCanLibFuncReadStatus(const CanHandle hnd, uint32_t * const flags)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(kvaserCanLibFuncReadStatusPtr != NULL);
  assert(kvaserCanLibDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((kvaserCanLibFuncReadStatusPtr != NULL) && (kvaserCanLibDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = kvaserCanLibFuncReadStatusPtr(hnd, flags);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of KvaserCanLibFuncReadStatus ***/


/************************************************************************************//**
** \brief     Takes the specified channel off-bus.
** \param     hnd A handle to an open circuit.
** \return    canOK if successful, canERR_xxx otherwise.
**
****************************************************************************************/
static canStatus KvaserCanLibFuncBusOff(const CanHandle hnd)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(kvaserCanLibFuncBusOffPtr != NULL);
  assert(kvaserCanLibDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((kvaserCanLibFuncBusOffPtr != NULL) && (kvaserCanLibDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = kvaserCanLibFuncBusOffPtr(hnd);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of KvaserCanLibFuncBusOff ***/


/************************************************************************************//**
** \brief     Closes the channel associated with the handle. If no other threads are
**            using the CAN circuit, it is taken off bus. The handle can not be used for
**            further references to the channel, so any variable containing it should be
**            zeroed.
** \param     hnd A handle to an open circuit.
** \return    canOK if successful, canERR_xxx otherwise.
**
****************************************************************************************/
static canStatus KvaserCanLibFuncClose(const CanHandle hnd)
{
  canStatus result = canERR_NOTINITIALIZED;
  
  /* Check function pointer and library handle. */
  assert(kvaserCanLibFuncClosePtr != NULL);
  assert(kvaserCanLibDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((kvaserCanLibFuncClosePtr != NULL) && (kvaserCanLibDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = kvaserCanLibFuncClosePtr(hnd);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of KvaserCanLibFuncClose ***/


/*********************************** end of kvcanlib.c *********************************/

