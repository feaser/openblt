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
* Type definitions
****************************************************************************************/
/* Type definitions of the functions in the Lawicel CANUSB API that this CAN interface 
 * uses.
 */
typedef CANHANDLE (__stdcall * tCanUsbLibFuncOpen)(LPCSTR szID, LPCSTR szBitrate, uint32_t acceptance_code, uint32_t acceptance_mask, uint32_t flags);
typedef int32_t   (__stdcall * tCanUsbLibFuncClose)(CANHANDLE h);
typedef int32_t   (__stdcall * tCanUsbLibFuncWrite)(CANHANDLE h, CANMsg *msg);
typedef int32_t   (__stdcall * tCanUsbLibFuncStatus)(CANHANDLE h);
typedef int32_t   (__stdcall * tCanUsbLibFuncSetReceiveCallBack)(CANHANDLE h, LPFNDLL_RECEIVE_CALLBACK fn);


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
static bool CanUsbOpenChannel(void);
static bool CanUsbCloseChannel(void);
/* Lawicel CANUSB library handling. */
static void CanUsbLibLoadDll(void);
static void CanUsbLibUnloadDll(void);
static void __stdcall CanUsbLibReceiveCallback(CANMsg const * pMsg);
static CANHANDLE CanUsbLibFuncOpen(LPCSTR szID, LPCSTR szBitrate, 
                                   uint32_t acceptance_code, uint32_t acceptance_mask,
                                   uint32_t flags);
static int32_t CanUsbLibFuncClose(CANHANDLE h);
static int32_t CanUsbLibFuncWrite(CANHANDLE h, CANMsg * msg);
static int32_t CanUsbLibFuncStatus(CANHANDLE h);
static int32_t CanUsbLibFuncSetReceiveCallBack(CANHANDLE h, LPFNDLL_RECEIVE_CALLBACK fn);


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

/** \brief Handle to the Lawicel CANUSB dynamic link library. */
static HINSTANCE canUsbDllHandle;

/** \brief Handle to the CAN channel. */
static CANHANDLE canUsbCanHandle;

/** \brief Function pointer to the Lawicel CANUSB canusb_Open function. */
static tCanUsbLibFuncOpen canUsbLibFuncOpenPtr;

/** \brief Function pointer to the Lawicel CANUSB canusb_Close function. */
static tCanUsbLibFuncClose canUsbLibFuncClosePtr;

/** \brief Function pointer to the Lawicel CANUSB canusb_Write function. */
static tCanUsbLibFuncWrite canUsbLibFuncWritePtr;

/** \brief Function pointer to the Lawicel CANUSB canusb_Status function. */
static tCanUsbLibFuncStatus canUsbLibFuncStatusPtr;

/** \brief Function pointer to the Lawicel CANUSB canusb_setReceiveCallBack function. */
static tCanUsbLibFuncSetReceiveCallBack canUsbLibFuncSetReceiveCallBackPtr;


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
  canUsbDllHandle = NULL;
  canUsbCanHandle = 0;
  /* Reset library function pointers. */
  canUsbLibFuncOpenPtr = NULL;
  canUsbLibFuncClosePtr = NULL;
  canUsbLibFuncWritePtr = NULL;
  canUsbLibFuncStatusPtr = NULL;
  canUsbLibFuncSetReceiveCallBackPtr = NULL;
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
    /* Perform initialization of Lawicel CANUSB API. */
    CanUsbLibLoadDll();
    /* Open the CAN channel. */
    (void)CanUsbOpenChannel();
  }
} /*** end of CanUsbInit ***/


/************************************************************************************//**
** \brief     Terminates the CAN interface.
**
****************************************************************************************/
static void CanUsbTerminate(void)
{
  /* Close the CAN channel. */
  (void)CanUsbCloseChannel();
  /* Perform termination of Lawicel CANUSB API. */
  CanUsbLibUnloadDll();
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

  /* Only continue with an opened CAN channel. */
  if (canUsbCanHandle != 0)
  {
    /* Init result code to success and only negate it on detection of error. */
    result = true;
    /* Set the reception callback function. */
    if (CanUsbLibFuncSetReceiveCallBack(canUsbCanHandle, 
                               (LPFNDLL_RECEIVE_CALLBACK)CanUsbLibReceiveCallback) <= 0)
    {
      result = false;
    }
  }
  
  /* Clean-up in case an error occurred. */
  if (!result)
  {
    if (canUsbCanHandle != 0)
    {
      /* Reset the reception callback handler and close the channel. */
      (void)CanUsbLibFuncSetReceiveCallBack(canUsbCanHandle, NULL);
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
  /* Reset the reception callback handler and close the channel. */
  if (canUsbCanHandle != 0)
  {
    (void)CanUsbLibFuncSetReceiveCallBack(canUsbCanHandle, NULL);
  }
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
  CANMsg txMsg;
  tCanEvents const * pEvents;

  /* Check parameters. */
  assert(msg != NULL);

  /* Only continue with valid parameters and handle. */
  if ( (msg != NULL) && (canUsbCanHandle != 0) ) /*lint !e774 */
  {
    /* Convert the message to a type supported by the PCAN-Basic API. */
    txMsg.id  = msg->id & 0x1fffffffu;
    txMsg.flags = 0;
    if ((msg->id & CAN_MSG_EXT_ID_MASK) != 0)
    {
      txMsg.flags |= CANMSG_EXTENDED;
    }
    txMsg.len = msg->dlc;
    for (uint8_t idx = 0; idx < msg->dlc; idx++)
    {
      txMsg.data[idx] = msg->data[idx];
    }
    /* Submit CAN message for transmission. */
    if (CanUsbLibFuncWrite(canUsbCanHandle, &txMsg) > 0)
    {
      /* Update result value to success. */
      result = true;
      /* Trigger transmit complete event(s). */
      pEvents = canUsbEventsList;
      for (uint32_t idx = 0; idx < canUsbEventsEntries; idx++)
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
} /*** end of CanUsbTransmit ***/


/************************************************************************************//**
** \brief     Checks if a bus off or bus heavy situation occurred.
** \return    True if a bus error situation was detected, false otherwise.
**
****************************************************************************************/
static bool CanUsbIsBusError(void)
{
  bool result = false;
  int32_t status;

  /* Only continue with valid handle. */
  if (canUsbCanHandle != 0)
  {
    /* Check and process status. */
    status = CanUsbLibFuncStatus(canUsbCanHandle);
    if ((status & CANSTATUS_BUS_ERROR) != 0)
    {
      result = true;
      /* Due to poor time performance of the Lawicel CANUSB API when opening and closing
       * the CAN channel, this is not done in CanUsbConnect/Disconnect, but in 
       * CanUsbInit/Terminate as a performance optimization workaround. Usually, when a
       * bus error is detected here, the caller will do a Disconnect/Connect sequence to 
       * resolve the error, but this won't have any effect due to the before mentioned 
       * workaround. For this reason a CAN channel close/open was explicitely added here.
       */
      (void)CanUsbCloseChannel();
      (void)CanUsbOpenChannel();
    }
  }
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
** \brief     Opens the CAN channel. Note that the opening of the CAN channel takes a
**            long time in the Lawicel CANUSB API, therefore this is not done in 
**            CanUsbConnect() for this CAN interface.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool CanUsbOpenChannel(void)
{
  bool result = false;
  bool baudrateSupported = true;
  char bitrateStr[16] = "";

  /* Reset the channel handle. */
  canUsbCanHandle = 0;

  /* Convert the baudrate to a value supported by the Lawical CANUSB API. */
  switch (canUsbSettings.baudrate)
  {
    case CAN_BR10K:
      strcat(bitrateStr, "10");
      break;
    case CAN_BR20K:
      strcat(bitrateStr, "20");
      break;
    case CAN_BR50K:
      strcat(bitrateStr, "50");
      break;
    case CAN_BR100K:
      strcat(bitrateStr, "100");
      break;
    case CAN_BR125K:
      strcat(bitrateStr, "0x03:0x1c");
      break;
    case CAN_BR250K:
      strcat(bitrateStr, "250");
      break;
    case CAN_BR500K:
      strcat(bitrateStr, "500");
      break;
    case CAN_BR800K:
      strcat(bitrateStr, "800");
      break;
    case CAN_BR1M:
      strcat(bitrateStr, "1000");
      break;
    default:
      baudrateSupported = false;
      break;
  }
  /* Validate baudrate. */
  assert(baudrateSupported);

  /* Only continue with a valid baudrate. */
  if (baudrateSupported)
  {
    /* Configure reception acceptance filter. Default to an open filter, which will be
     * used in case both 11-bit standard and 29-bit extended CAN identiers should be
     * received. It is not possible to optimize the filter for this scenario, because
     * the SJA1000's acceptance filter on the CANUSB is pre-configured for dual filter
     * mode.
     */
    uint8_t regACR0 = 0x00u;
    uint8_t regAMR0 = 0xffu;
    uint8_t regACR1 = 0x00u;
    uint8_t regAMR1 = 0xffu;
    uint8_t regACR2 = 0x00u;
    uint8_t regAMR2 = 0xffu;
    uint8_t regACR3 = 0x00u;
    uint8_t regAMR3 = 0xffu;
    /* Use bit logic to determine if the filter should accept standard 11-bit and/or
      * extended 29-bit identifiers:
      *   acceptStdId = ((mask & code & CAN_MSG_EXT_ID_MASK) == 0)
      *   acceptExtId = ((mask & code & CAN_MSG_EXT_ID_MASK) != 0) || 
      *                 ((mask & CAN_MSG_EXT_ID_MASK) == 0)
      */
    bool acceptStdID = ((canUsbSettings.mask & canUsbSettings.code & CAN_MSG_EXT_ID_MASK) == 0);
    bool acceptExtID = ((canUsbSettings.mask & canUsbSettings.code & CAN_MSG_EXT_ID_MASK) != 0) ||
                        ((canUsbSettings.mask & CAN_MSG_EXT_ID_MASK) == 0);
    /* Configure acceptance filter for standard 11-bit identifiers. */
    if (acceptStdID)
    {
      /* Set 11-bit code and mask, taking into account that the meaning of the mask bits
       * on the SJA1000 is inverted: 1=don't care and 0=care. 
       */
      uint16_t code11bit = canUsbSettings.code & 0x7ffu;
      uint16_t mask11bit = ~(canUsbSettings.mask & 0x7ffu);
      /* Convert to ACRx and AMRx register values. */
      regACR0 = (uint8_t)(code11bit >> 3);
      regAMR0 = (uint8_t)(mask11bit >> 3);
      regACR1 = ((uint8_t)(code11bit << 5)) | 0x1fu;
      regAMR1 = ((uint8_t)(mask11bit << 5)) | 0x1fu;
      regACR2 = (uint8_t)(code11bit >> 3);
      regAMR2 = (uint8_t)(mask11bit >> 3);
      regACR3 = ((uint8_t)(code11bit << 5)) | 0x1fu;
      regAMR3 = ((uint8_t)(mask11bit << 5)) | 0x1fu;
    }
    /* Configure acceptance filter for extended 29-bit identifiers. */
    else if (acceptExtID)
    {
      /* Set 29-bit code and mask, taking into account that the meaning of the mask bits
       * on the SJA1000 is inverted: 1=don't care and 0=care. Also note that only the 16
       * most significant bits of the CAN identifier can be filters due to hardware
       * restrictions of the SJA1000.
       */
      uint16_t code29bit = (uint16_t)((canUsbSettings.code & 0x1fffffffu) >> 13);
      uint16_t mask29bit = ~((uint16_t)((canUsbSettings.mask & 0x1fffffffu) >> 13));
      /* Convert to ACRx and AMRx register values. */
      regACR0 = (uint8_t)(code29bit >> 8);
      regAMR0 = (uint8_t)(mask29bit >> 8);
      regACR1 = (uint8_t)code29bit;
      regAMR1 = (uint8_t)mask29bit;
      regACR2 = (uint8_t)(code29bit >> 8);
      regAMR2 = (uint8_t)(mask29bit >> 8);
      regACR3 = (uint8_t)code29bit;
      regAMR3 = (uint8_t)mask29bit;
    }
    /* Convert to 32-bit code and mask values. */
    uint32_t acceptCode = ((uint32_t)regACR3 << 24) & 0xff000000u;
    acceptCode |= ((uint32_t)regACR2 << 16)         & 0x00ff0000u;
    acceptCode |= ((uint32_t)regACR1 << 8)          & 0x0000ff00u;
    acceptCode |= ((uint32_t)regACR0)               & 0x000000ffu;
    uint32_t acceptMask = ((uint32_t)regAMR3 << 24) & 0xff000000u;
    acceptMask |= ((uint32_t)regAMR2 << 16)         & 0x00ff0000u;
    acceptMask |= ((uint32_t)regAMR1 << 8)          & 0x0000ff00u;
    acceptMask |= ((uint32_t)regAMR0)               & 0x000000ffu;
    /* Open the CAN channel, configure the reception acceptance filter and obtains the
     * channel handle.
     */
    canUsbCanHandle = CanUsbLibFuncOpen(NULL, bitrateStr, acceptCode, acceptMask, 0);
    if (canUsbCanHandle != 0)
    {
      result = true;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of CanUsbOpenChannel ***/


/************************************************************************************//**
** \brief     Closes the CAN channel. Note that the closing of the CAN channel takes a
**            long time in the Lawicel CANUSB API, therefore this is not done in 
**            CanUsbDisconnect() for this CAN interface.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool CanUsbCloseChannel(void)
{
  bool result = false;

  /* Close the CAN channel. */
  if (canUsbCanHandle != 0)
  {
    if (CanUsbLibFuncClose(canUsbCanHandle) >= 0)
    {
      result = true;
    }
    canUsbCanHandle = 0;
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of CanUsbCloseChannel ***/


/************************************************************************************//**
** \brief     Loads the Lawicel CANUSBDRV DLL and initializes the API function pointers.
**
****************************************************************************************/
static void CanUsbLibLoadDll(void)
{
  /* Reset the channel handle. */
  canUsbCanHandle = 0;
  /* Start out by resetting the API function pointers. */
  canUsbLibFuncOpenPtr = NULL;
  canUsbLibFuncClosePtr = NULL;
  canUsbLibFuncWritePtr = NULL;
  canUsbLibFuncStatusPtr = NULL;
  canUsbLibFuncSetReceiveCallBackPtr = NULL;

  /* Attempt to load the library and obtain a handle to it. */
  canUsbDllHandle = LoadLibrary("canusbdrv");

  /* Assert libary handle. */
  assert(canUsbDllHandle != NULL);

  /* Only continue if the library was successfully loaded */
  if (canUsbDllHandle != NULL) /*lint !e774 */
  {
    /* Set canusb_Open function pointer. */
    canUsbLibFuncOpenPtr = (tCanUsbLibFuncOpen)GetProcAddress(canUsbDllHandle, "canusb_Open");
    /* Set canusb_Close function pointer. */
    canUsbLibFuncClosePtr = (tCanUsbLibFuncClose)GetProcAddress(canUsbDllHandle, "canusb_Close");
    /* Set canusb_Write function pointer. */
    canUsbLibFuncWritePtr = (tCanUsbLibFuncWrite)GetProcAddress(canUsbDllHandle, "canusb_Write");
    /* Set canusb_Status function pointer. */
    canUsbLibFuncStatusPtr = (tCanUsbLibFuncStatus)GetProcAddress(canUsbDllHandle, "canusb_Status");
    /* Set canusb_setReceiveCallBack function pointer. */
    canUsbLibFuncSetReceiveCallBackPtr = (tCanUsbLibFuncSetReceiveCallBack)GetProcAddress(canUsbDllHandle, "canusb_setReceiveCallBack");
  }
} /*** end of CanUsbLibLoadDll ***/


/************************************************************************************//**
** \brief     Unloads the Lawicel CANUSBDRV DLL and resets the API function pointers.
**
****************************************************************************************/
static void CanUsbLibUnloadDll(void)
{
  /* Reset the API function pointers. */
  canUsbLibFuncOpenPtr = NULL;
  canUsbLibFuncClosePtr = NULL;
  canUsbLibFuncWritePtr = NULL;
  canUsbLibFuncStatusPtr = NULL;
  canUsbLibFuncSetReceiveCallBackPtr = NULL;
  /* Reset the channel handle. */
  canUsbCanHandle = 0;
  /* Unload the library and invalidate its handle. */
  if (canUsbDllHandle != NULL) 
  {
    (void)FreeLibrary(canUsbDllHandle);
    canUsbDllHandle = NULL;
  }
} /*** end of CanUsbLibUnloadDll **/


/************************************************************************************//**
** \brief     Callback function that gets called by the Lawicel CANUSB API each time a
**            CAN message was received.
** \param     pMsg Pointer to the received CAN message.
**
****************************************************************************************/
static void __stdcall CanUsbLibReceiveCallback(CANMsg const * pMsg)
{
  tCanMsg rxMsg;
  tCanEvents const * pEvents;

  /* Only continue with a valid message. */
  if (pMsg != NULL)
  {
    /* Ignore remote frames. */
    if ((pMsg->flags & CANMSG_RTR) == 0)
    {
      rxMsg.id = pMsg->id;
      if ((pMsg->flags & CANMSG_EXTENDED) != 0)
      {
        rxMsg.id |= CAN_MSG_EXT_ID_MASK;
      }
      rxMsg.dlc = pMsg->len;
      for (uint8_t idx = 0; idx < rxMsg.dlc; idx++)
      {
        rxMsg.data[idx] = pMsg->data[idx];
      }
      /* Trigger message reception event(s). */
      pEvents = canUsbEventsList;
      for (uint32_t idx = 0; idx < canUsbEventsEntries; idx++)
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
} /*** end of CanUsbLibReceiveCallback ***/


/************************************************************************************//**
** \brief     Open a channel to a physical CAN interface.
** \param     szID Serial number for adapter or NULL to open the first found.
** \param     szBitrate "10", "20", "50", "100", "250", "500", "800" or "1000" (kbps) or
**            as a btr pair. btr0:btr1 pair ex. "0x03:0x1c" can be used to set a custom
**            baudrate.
** \param     acceptance_code Set to CANUSB_ACCEPTANCE_CODE_ALL to get all messages or
**            another code to filter messages.
** \param     acceptance_mask Set to CANUSB_ACCEPTANCE_MASK_ALL to get all messages or
**            another code to filter messages.
** \param     flags Optional flags CANUSB_FLAG_xxx.
** \return    Handle to device if open was successful or zero on failure.
**
****************************************************************************************/
static CANHANDLE CanUsbLibFuncOpen(LPCSTR szID, LPCSTR szBitrate,
                                   uint32_t acceptance_code, uint32_t acceptance_mask,
                                   uint32_t flags)
{
  CANHANDLE result = 0;

  /* Check function pointer and library handle. */
  assert(canUsbLibFuncOpenPtr != NULL);
  assert(canUsbDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((canUsbLibFuncOpenPtr != NULL) && (canUsbDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = canUsbLibFuncOpenPtr(szID, szBitrate, acceptance_code, acceptance_mask, 
                                  flags);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of CanUsbLibFuncOpen ***/


/************************************************************************************//**
** \brief     Close channel with handle h.
** \param     h Handle to the opened device.
** \return    > 0 if successful, ERROR_CANUSB_xxx (<= 0) otherwise.
**
****************************************************************************************/
static int32_t CanUsbLibFuncClose(CANHANDLE h)
{
  int32_t result = ERROR_CANUSB_GENERAL;

  /* Check function pointer and library handle. */
  assert(canUsbLibFuncClosePtr != NULL);
  assert(canUsbDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((canUsbLibFuncClosePtr != NULL) && (canUsbDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = canUsbLibFuncClosePtr(h);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of CanUsbLibFuncClose ***/


/************************************************************************************//**
** \brief     Write message to channel with handle h.
** \param     h Handle to the opened device.
** \param     msg CAN message to send.
** \return    > 0 if successful, ERROR_CANUSB_xxx (<= 0) otherwise.
**
****************************************************************************************/
static int32_t CanUsbLibFuncWrite(CANHANDLE h, CANMsg * msg)
{
  int32_t result = ERROR_CANUSB_GENERAL;

  /* Check function pointer and library handle. */
  assert(canUsbLibFuncWritePtr != NULL);
  assert(canUsbDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((canUsbLibFuncWritePtr != NULL) && (canUsbDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = canUsbLibFuncWritePtr(h, msg);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of CanUsbLibFuncWrite ***/


/************************************************************************************//**
** \brief     Get Adapter status for channel with handle h.
** \param     h Handle to the opened device.
** \return    CANSTATUS_xxx if status info is set, 0 otherwise.
**
****************************************************************************************/
static int32_t CanUsbLibFuncStatus(CANHANDLE h)
{
  int32_t result = 0;

  /* Check function pointer and library handle. */
  assert(canUsbLibFuncStatusPtr != NULL);
  assert(canUsbDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((canUsbLibFuncStatusPtr != NULL) && (canUsbDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = canUsbLibFuncStatusPtr(h);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of CanUsbLibFuncStatus ***/


/************************************************************************************//**
** \brief     With this method one can define a function that will receive all incoming
**            messages.
** \param     h Handle to the opened device.
** \param     fn Pointer to the callback function to set. NULL removes it again.
** \return    > 0 if successful, ERROR_CANUSB_xxx (<= 0) otherwise.
**
****************************************************************************************/
static int32_t CanUsbLibFuncSetReceiveCallBack(CANHANDLE h, LPFNDLL_RECEIVE_CALLBACK fn)
{
  int32_t result = ERROR_CANUSB_GENERAL;

  /* Check function pointer and library handle. */
  assert(canUsbLibFuncSetReceiveCallBackPtr != NULL);
  assert(canUsbDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((canUsbLibFuncSetReceiveCallBackPtr != NULL) && (canUsbDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = canUsbLibFuncSetReceiveCallBackPtr(h, fn);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of CanUsbLibFuncSetReceiveCallBack ***/


/*********************************** end of canusb.c ***********************************/

