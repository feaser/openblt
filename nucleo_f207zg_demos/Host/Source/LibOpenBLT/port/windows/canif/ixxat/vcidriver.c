/************************************************************************************//**
* \file         vcidriver.c
* \brief        Ixxat VCI driver interface source file.
* \ingroup      VCIDRIVER
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2021  by Feaser    http://www.feaser.com    All rights reserved
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
#include <assert.h>                     /* for assertions                              */
#include <stdint.h>                     /* for standard integer types                  */
#include <stddef.h>                     /* for NULL declaration                        */
#include <stdbool.h>                    /* for boolean type                            */
#include <stdlib.h>                     /* for standard library                        */
#include <string.h>                     /* for string library                          */
#include "candriver.h"                  /* Generic CAN driver module                   */
#include "vcidriver.h"                  /* Ixxat VCI driver interface                  */
#include <windows.h>                    /* for Windows API                             */
#include "vcinpl.h"                     /* VCI native programming library (VCINPL.DLL) */


/***************************************************************************************
* Function prototypes
****************************************************************************************/
/* CAN interface functions. */
static void IxxatVciInit(tCanSettings const * settings);
static void IxxatVciTerminate(void);
static bool IxxatVciConnect(void);
static void IxxatVciDisconnect(void);
static bool IxxatVciTransmit(tCanMsg const * msg);
static bool IxxatVciIsBusError(void);
static void IxxatVciRegisterEvents(tCanEvents const * events);
/* CAN message reception thread. */
static DWORD WINAPI IxxatVciReceptionThread(LPVOID pv);
/* Utility functions. */
static bool IxxatVciConvertBaudrate(UINT8 * bBtr0, UINT8 * bBtr1);
/* Ixxat VCI library handling. */
static void IxxatVciLibLoadDll(void);
static void IxxatVciLibUnloadDll(void);
static HRESULT IxxatVciLibFuncVciEnumDeviceOpen(PHANDLE hEnum);
static HRESULT IxxatVciLibFuncVciEnumDeviceClose(HANDLE hEnum);
static HRESULT IxxatVciLibFuncVciEnumDeviceNext(HANDLE hEnum, PVCIDEVICEINFO pInfo);
static HRESULT IxxatVciLibFuncVciDeviceOpen(REFVCIID rVciid, PHANDLE phDevice);
static HRESULT IxxatVciLibFuncVciDeviceClose(HANDLE hDevice);
static HRESULT IxxatVciLibFuncCanControlOpen(HANDLE hDevice, UINT32  dwCanNo, PHANDLE phCanCtl);
static HRESULT IxxatVciLibFuncCanControlReset(HANDLE hCanCtl);
static HRESULT IxxatVciLibFuncCanControlGetCaps(HANDLE hCanCtl, PCANCAPABILITIES pCanCaps);
static HRESULT IxxatVciLibFuncCanControlInitialize(HANDLE hCanCtl, UINT8 bMode, UINT8 bBtr0, UINT8 bBtr1);
static HRESULT IxxatVciLibFuncCanControlSetAccFilter(HANDLE hCanCtl, BOOL fExtend, UINT32 dwCode, UINT32 dwMask);
static HRESULT IxxatVciLibFuncCanControlStart(HANDLE hCanCtl, BOOL fStart);
static HRESULT IxxatVciLibFuncCanControlClose(HANDLE hCanCtl);
static HRESULT IxxatVciLibFuncCanChannelOpen(HANDLE hDevice, UINT32 dwCanNo, BOOL fExclusive, PHANDLE phCanChn);
static HRESULT IxxatVciLibFuncCanChannelGetStatus(HANDLE hCanChn, PCANCHANSTATUS pStatus);
static HRESULT IxxatVciLibFuncCanChannelInitialize(HANDLE hCanChn, UINT16 wRxFifoSize, UINT16 wRxThreshold, UINT16 wTxFifoSize, UINT16 wTxThreshold);
static HRESULT IxxatVciLibFuncCanChannelActivate(HANDLE hCanChn,BOOL fEnable);
static HRESULT IxxatVciLibFuncCanChannelPostMessage(HANDLE hCanChn, PCANMSG pCanMsg);
static HRESULT IxxatVciLibFuncCanChannelReadMessage(HANDLE hCanChn, UINT32 dwTimeout, PCANMSG pCanMsg);
static HRESULT IxxatVciLibFuncCanChannelClose(HANDLE hCanChn);


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief CAN interface structure filled with Ixxat VCI driver specifics. */
static const tCanInterface ixxatVciInterface =
{
  IxxatVciInit,
  IxxatVciTerminate,
  IxxatVciConnect,
  IxxatVciDisconnect,
  IxxatVciTransmit,
  IxxatVciIsBusError,
  IxxatVciRegisterEvents
};


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief The settings to use in this CAN interface. */
static tCanSettings ixxatVciSettings;

/** \brief List with callback functions that this driver should use. */
static tCanEvents * ixxatVciEventsList;

/** \brief Total number of event entries into the \ref pCanUsbEventsList list. */
static uint32_t ixxatVciEventsEntries;

/** \brief Handle to the Ixxat VCI dynamic link library. */
static HINSTANCE ixxatVciDllHandle;

/** \brief Handle for the event to terminate the reception thread. */
static HANDLE ixxatVciTerminateEvent;

/** \brief Handle for the CAN reception thread. */
static HANDLE ixxatVciRxThreadHandle;

/** \brief Handle for the Ixxat device. */
static HANDLE ixxatVciDeviceHandle;

/** \brief Handle for the Ixxat control. */
static HANDLE ixxatCanControlHandle;

/** \brief Handle for the Ixxat channel. */
static HANDLE ixxatCanChannelHandle;

/** \brief Function pointer to the Ixxat vciEnumDeviceOpen function. */
static PF_vciEnumDeviceOpen ixxatVciLibFuncVciEnumDeviceOpenPtr;

/** \brief Function pointer to the Ixxat vciEnumDeviceClose function. */
static PF_vciEnumDeviceClose ixxatVciLibFuncVciEnumDeviceClosePtr;

/** \brief Function pointer to the Ixxat vciEnumDeviceNext function. */
static PF_vciEnumDeviceNext ixxatVciLibFuncVciEnumDeviceNextPtr;

/** \brief Function pointer to the Ixxat vciDeviceOpen function. */
static PF_vciDeviceOpen ixxatVciLibFuncVciDeviceOpenPtr;

/** \brief Function pointer to the Ixxat vciDeviceClose function. */
static PF_vciDeviceClose ixxatVciLibFuncVciDeviceClosePtr;

/** \brief Function pointer to the Ixxat canControlOpen function. */
static PF_canControlOpen ixxatVciLibFuncCanControlOpenPtr;

/** \brief Function pointer to the Ixxat canControlReset function. */
static PF_canControlReset ixxatVciLibFuncCanControlResetPtr;

/** \brief Function pointer to the Ixxat canControlGetCaps function. */
static PF_canControlGetCaps ixxatVciLibFuncCanControlGetCapsPtr;

/** \brief Function pointer to the Ixxat canControlInitialize function. */
static PF_canControlInitialize ixxatVciLibFuncCanControlInitializePtr;

/** \brief Function pointer to the Ixxat canControlSetAccFilter function. */
static PF_canControlSetAccFilter ixxatVciLibFuncCanControlSetAccFilterPtr;

/** \brief Function pointer to the Ixxat canControlStart function. */
static PF_canControlStart ixxatVciLibFuncCanControlStartPtr;

/** \brief Function pointer to the Ixxat canControlClose function. */
static PF_canControlClose ixxatVciLibFuncCanControlClosePtr;

/** \brief Function pointer to the Ixxat canChannelOpen function. */
static PF_canChannelOpen ixxatVciLibFuncCanChannelOpenPtr;

/** \brief Function pointer to the Ixxat canChannelGetStatus function. */
static PF_canChannelGetStatus ixxatVciLibFuncCanChannelGetStatusPtr;

/** \brief Function pointer to the Ixxat canChannelInitialize function. */
static PF_canChannelInitialize ixxatVciLibFuncCanChannelInitializePtr;

/** \brief Function pointer to the Ixxat canChannelActivate function. */
static PF_canChannelActivate ixxatVciLibFuncCanChannelActivatePtr;

/** \brief Function pointer to the Ixxat canChannelPostMessage function. */
static PF_canChannelPostMessage ixxatVciLibFuncCanChannelPostMessagePtr;

/** \brief Function pointer to the Ixxat canChannelReadMessage function. */
static PF_canChannelReadMessage ixxatVciLibFuncCanChannelReadMessagePtr;

/** \brief Function pointer to the Ixxat canChannelClose function. */
static PF_canChannelClose ixxatVciLibFuncCanChannelClosePtr;


/***********************************************************************************//**
** \brief     Obtains a pointer to the CAN interface structure, so that it can be linked
**            to the generic CAN driver module.
** \return    Pointer to CAN interface structure.
**
****************************************************************************************/
tCanInterface const * IxxatVciGetInterface(void)
{
  return &ixxatVciInterface;
} /*** end of IxxatVciGetInterface ***/


/************************************************************************************//**
** \brief     Initializes the CAN interface.
** \param     settings Pointer to the CAN interface settings.
**
****************************************************************************************/
static void IxxatVciInit(tCanSettings const * settings)
{
  char * canDeviceName;

  /* Initialize locals. */
  ixxatVciEventsList = NULL;
  ixxatVciEventsEntries = 0;
  ixxatVciTerminateEvent = NULL;
  ixxatVciRxThreadHandle = NULL;
  ixxatVciDllHandle = NULL;
  ixxatVciDeviceHandle = NULL;
  ixxatCanControlHandle = NULL;
  ixxatCanChannelHandle = NULL;

  /* Reset library function pointers. */
  ixxatVciLibFuncVciEnumDeviceOpenPtr = NULL;
  ixxatVciLibFuncVciEnumDeviceClosePtr = NULL;
  ixxatVciLibFuncVciEnumDeviceNextPtr = NULL;
  ixxatVciLibFuncVciDeviceOpenPtr = NULL;
  ixxatVciLibFuncVciDeviceClosePtr = NULL;
  ixxatVciLibFuncCanControlOpenPtr = NULL;
  ixxatVciLibFuncCanControlResetPtr = NULL;
  ixxatVciLibFuncCanControlGetCapsPtr = NULL;
  ixxatVciLibFuncCanControlInitializePtr = NULL;
  ixxatVciLibFuncCanControlSetAccFilterPtr = NULL;
  ixxatVciLibFuncCanControlStartPtr = NULL;
  ixxatVciLibFuncCanControlClosePtr = NULL;
  ixxatVciLibFuncCanChannelOpenPtr = NULL;
  ixxatVciLibFuncCanChannelGetStatusPtr = NULL;
  ixxatVciLibFuncCanChannelInitializePtr = NULL;
  ixxatVciLibFuncCanChannelActivatePtr = NULL;
  ixxatVciLibFuncCanChannelPostMessagePtr = NULL;
  ixxatVciLibFuncCanChannelReadMessagePtr = NULL;
  ixxatVciLibFuncCanChannelClosePtr = NULL;

  /* Reset CAN interface settings. */
  ixxatVciSettings.devicename = "";
  ixxatVciSettings.channel = 0;
  ixxatVciSettings.baudrate = CAN_BR500K;
  ixxatVciSettings.code = 0x00000000u;
  ixxatVciSettings.mask = 0x00000000u;

  /* Check parameters. */
  assert(settings != NULL);

  /* Only continue with valid parameters. */
  if (settings != NULL) /*lint !e774 */
  {
    /* Shallow copy the CAN interface settings for later usage. */
    ixxatVciSettings = *settings;
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
        ixxatVciSettings.devicename = canDeviceName;
      }
    }
    /* Perform initialization of the Ixxat VCI driver API. */
    IxxatVciLibLoadDll();
  }
} /*** end of IxxatVciInit ***/


/************************************************************************************//**
** \brief     Terminates the CAN interface.
**
****************************************************************************************/
static void IxxatVciTerminate(void)
{
  /* Perform termination of Iccat VCI driver API. */
  IxxatVciLibUnloadDll();

  /* Release memory that was allocated for storing the device name. */
  if (ixxatVciSettings.devicename != NULL)
  {
    free((char *)ixxatVciSettings.devicename);
  }
  /* Reset CAN interface settings. */
  ixxatVciSettings.devicename = "";
  ixxatVciSettings.channel = 0;
  ixxatVciSettings.baudrate = CAN_BR500K;
  ixxatVciSettings.code = 0x00000000u;
  ixxatVciSettings.mask = 0x00000000u;
  /* Release memory that was allocated for CAN events and reset the entry count. */
  if ( (ixxatVciEventsList != NULL) && (ixxatVciEventsEntries != 0) )
  {
    free(ixxatVciEventsList);
    ixxatVciEventsEntries = 0;
  }
} /*** end of IxxatVciTerminate ***/


/************************************************************************************//**
** \brief     Connects the CAN interface.
** \return    True if connected, false otherwise.
**
****************************************************************************************/
static bool IxxatVciConnect(void)
{
  bool result = true;
  HANDLE hEnum = NULL;
  CANCAPABILITIES sCanCaps = { 0 };
  VCIDEVICEINFO sInfo = { 0 }; /*lint !e708 Union part is not used. */

  /* Invalidate handles. */
  ixxatVciTerminateEvent = NULL;
  ixxatVciRxThreadHandle = NULL;
  ixxatVciDeviceHandle = NULL;
  ixxatCanControlHandle = NULL;
  ixxatCanChannelHandle = NULL;

  /* Open the device list.*/
  if (IxxatVciLibFuncVciEnumDeviceOpen(&hEnum) == VCI_OK)
  {
    /* This driver assumes that there is only one Ixxat CAN device connected. Therfore,
     * retrieve information about the first device within the device list.
     */
    if (IxxatVciLibFuncVciEnumDeviceNext(hEnum, &sInfo) != VCI_OK)
    {
      /* No Ixxat device connected to the system, so we cannot connect to it. */
      result = false;
    }

    /* Close the device list again, since it is no longer needed. */
    (void)IxxatVciLibFuncVciEnumDeviceClose(hEnum);
  }

  /* Open the device and store its handle. */
  if (result)
  {
    if (IxxatVciLibFuncVciDeviceOpen(&sInfo.VciObjectId, &ixxatVciDeviceHandle) != VCI_OK)
    {
      /* Could not open the device. */
      result = false;
    }
  }

  /* Obtain access to the CAN controller itself. */
  if (result)
  {
    if (IxxatVciLibFuncCanControlOpen(ixxatVciDeviceHandle, ixxatVciSettings.channel, 
                                      &ixxatCanControlHandle) != VCI_OK)
    {
      /* Could not obtain access to the CAN controller. */
      result = false;
    }
  }

  /* Reset the CAN controller to clear possible errors and clear the message filters. */
  if (result)
  {
    (void)IxxatVciLibFuncCanControlReset(ixxatCanControlHandle);
  }

  /* Request the capabilities of the CAN controller. */
  if (result)
  {
    if (IxxatVciLibFuncCanControlGetCaps(ixxatCanControlHandle, &sCanCaps) != VCI_OK)
    {
      /* Could not access CAN controller capabilities. */
      result = false;
    }
  }

  /* Initialize the CAN controller for the requested baudrate. */
  if (result)
  {
    /* First convert the baudrate to the values needed by the API. */
    uint8_t bBtr0, bBtr1;
    if (!IxxatVciConvertBaudrate(&bBtr0, &bBtr1))
    {
      result = false;
    }
    /* Valid BTR0/1 settings found, okay to continue. */
    else
    {
      uint8_t bOpMode = 0;

      /* Acceptance filter configured to exclusively receive either 11- or 29-bit IDs? */
      if ((ixxatVciSettings.mask & CAN_MSG_EXT_ID_MASK) != 0)
      {
        /* Should only 11-bit IDs be received? */
        if ((ixxatVciSettings.code & CAN_MSG_EXT_ID_MASK) == 0)
        {
          bOpMode |= CAN_OPMODE_STANDARD;
        }
        /* Only 29-bit IDs should be received. */
        else
        {
          bOpMode |= CAN_OPMODE_EXTENDED;
        }
      }
      /* Acceptance filter configured to receive both 11- and 29-bit IDs. */
      else
      {
        bOpMode |= (CAN_OPMODE_STANDARD | CAN_OPMODE_EXTENDED);

        /* Does the CAN controller actually support this? */
        if ((sCanCaps.dwFeatures & CAN_FEATURE_STDANDEXT) == 0)
        {
          /* Nope, CAN controller does not support this. Configure to only receive 11-bit
           * identifiers, since this is more common.
           */
          bOpMode &= ~CAN_OPMODE_EXTENDED;
        }
      }

      /* Initialize the CAN controller's baudrate and operating mode. */
      if (IxxatVciLibFuncCanControlInitialize(ixxatCanControlHandle, bOpMode, bBtr0, bBtr1) != VCI_OK)
      {
        /* Error initializing the CAN controller. */
        result = false;
      }
    }
  }

  /* Configure the reception acceptance filter. */
  if (result)
  {
    uint32_t dwCode, dwMask;

    /* Acceptance filter configured to exclusively receive either 11- or 29-bit IDs? 
     * Note that there is no need to check the return values of the API functions
     * here, because by default all CAN messages will be received after a CAN
     * controller reset. That always works as a fallback.
     */
    if ((ixxatVciSettings.mask & CAN_MSG_EXT_ID_MASK) != 0)
    {
      /* Should only 11-bit IDs be received? */
      if ((ixxatVciSettings.code & CAN_MSG_EXT_ID_MASK) == 0)
      {
        /* Configure acceptance code and mask for 11-bit IDs (non-RTR). */
        dwCode = ixxatVciSettings.code << 1;
        dwMask = ((ixxatVciSettings.mask & ~(CAN_MSG_EXT_ID_MASK)) << 1) | 0x00000001;

        (void)IxxatVciLibFuncCanControlSetAccFilter(ixxatCanControlHandle, FALSE, dwCode, dwMask);
        /* Disable the reception of 29-bit IDs. */
        (void)IxxatVciLibFuncCanControlSetAccFilter(ixxatCanControlHandle, TRUE, CAN_ACC_CODE_NONE, CAN_ACC_MASK_NONE);
      }
      /* Only 29-bit IDs should be received. */
      else
      {
        /* Configure acceptance code and mask for 29-bit IDs (non-RTR). */
        dwCode = (ixxatVciSettings.code & ~(CAN_MSG_EXT_ID_MASK)) << 1;
        dwMask = ((ixxatVciSettings.mask & ~(CAN_MSG_EXT_ID_MASK)) << 1) | 0x00000001;
        (void)IxxatVciLibFuncCanControlSetAccFilter(ixxatCanControlHandle, TRUE, dwCode, dwMask);
        /* Disable the reception of 11-bit IDs. */
        (void)IxxatVciLibFuncCanControlSetAccFilter(ixxatCanControlHandle, FALSE, CAN_ACC_CODE_NONE, CAN_ACC_MASK_NONE);
      }
    }
    /* Acceptance filter configured to receive both 11- and 29-bit IDs. */
    else
    {
       /* Configure acceptance code and mask for 11- and 29-bit IDs (non-RTR). */
      dwCode = (ixxatVciSettings.code & ~(CAN_MSG_EXT_ID_MASK)) << 1;
      dwMask = (ixxatVciSettings.mask << 1) | 0x00000001;
      (void)IxxatVciLibFuncCanControlSetAccFilter(ixxatCanControlHandle, FALSE, dwCode, dwMask);
      (void)IxxatVciLibFuncCanControlSetAccFilter(ixxatCanControlHandle, TRUE, dwCode, dwMask);
    }
  }

  /* Start the CAN controller. */
  if (result)
  {
    if (IxxatVciLibFuncCanControlStart(ixxatCanControlHandle, TRUE) != VCI_OK)
    {
      /* Could not synchronize CAN controller to the bus. */
      result = false;
    }
  }

  /* Open a CAN message channel. */
  if (result)
  {
    if (IxxatVciLibFuncCanChannelOpen(ixxatVciDeviceHandle, ixxatVciSettings.channel, FALSE,
        &ixxatCanChannelHandle) != VCI_OK)
    {
      /* Unable to open a CAN message channel. */
      result = false;
    }
  }

  /* Initialize the CAN message channel. */
  if (result)
  {
    if (IxxatVciLibFuncCanChannelInitialize(ixxatCanChannelHandle, 128, 1, 128, 1) != VCI_OK)
    {
      /* Unable the initialize the CAN message channel. */
      result = false;
    }
  }

  /* Activate the CAN message channel. */
  if (result)
  {
    if (IxxatVciLibFuncCanChannelActivate(ixxatCanChannelHandle, TRUE) != VCI_OK)
    {
      /* Unable to activate the CAN message channel. */
      result = false;
    }
  }
  
  /* Create the terminate event handle used in the reception thread. */
  if (result)
  {
    ixxatVciTerminateEvent = CreateEvent(NULL, TRUE, FALSE, "");
    if (ixxatVciTerminateEvent == NULL)
    {
      result = false;
    }
  }

  /* Start the reception thread as the last step. */
  if (result)
  {
    ixxatVciRxThreadHandle = CreateThread(NULL, 0, IxxatVciReceptionThread,
      NULL, 0, NULL);
    if (ixxatVciRxThreadHandle == NULL)
    {
      result = false;
    }
  }

  /* Clean-up in case an error occurred. */
  if (!result)
  {
    /* Close the event handle. */
    if (ixxatVciTerminateEvent != NULL)
    {
      (void)CloseHandle(ixxatVciTerminateEvent);
      ixxatVciTerminateEvent = NULL;
    }

    /* Close the CAN message channel. */
    if (ixxatCanChannelHandle != NULL)
    {
      (void)IxxatVciLibFuncCanChannelClose(ixxatCanChannelHandle);
      ixxatCanChannelHandle = NULL;
    }

    /* Close the CAN controller. */
    if (ixxatCanControlHandle != NULL)
    {
      (void)IxxatVciLibFuncCanControlClose(ixxatCanControlHandle);
      ixxatCanControlHandle = NULL;
    }

    /* Close the device handle. */
    if (ixxatVciDeviceHandle != NULL)
    {
      (void)IxxatVciLibFuncVciDeviceClose(ixxatVciDeviceHandle);
      ixxatVciDeviceHandle = NULL;
    }
  }
  
  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciConnect ***/


/************************************************************************************//**
** \brief     Disconnects the CAN interface.
**
****************************************************************************************/
static void IxxatVciDisconnect(void)
{
  /* Stop the reception thread. */
  if (ixxatVciRxThreadHandle != NULL)
  {
    /* Trigger event to request the reception thread to stop. */
    (void)SetEvent(ixxatVciTerminateEvent);
    /* Wait for the thread to signal termination. */
    (void)WaitForSingleObject(ixxatVciRxThreadHandle, INFINITE);
    /* Close the thread handle. */
    (void)CloseHandle(ixxatVciRxThreadHandle);
    ixxatVciRxThreadHandle = NULL;
  }

  /* Close the terminate event handle. */
  if (ixxatVciTerminateEvent != NULL)
  {
    (void)CloseHandle(ixxatVciTerminateEvent);
    ixxatVciTerminateEvent = NULL;
  }

  /* Close the CAN message channel. */
  if (ixxatCanChannelHandle != NULL)
  {
    (void)IxxatVciLibFuncCanChannelClose(ixxatCanChannelHandle);
    ixxatCanChannelHandle = NULL;
  }

  /* Close the CAN controller. */
  if (ixxatCanControlHandle != NULL)
  {
    (void)IxxatVciLibFuncCanControlClose(ixxatCanControlHandle);
    ixxatCanControlHandle = NULL;
  }

  /* Close the device handle. */
  if (ixxatVciDeviceHandle != NULL)
  {
    (void)IxxatVciLibFuncVciDeviceClose(ixxatVciDeviceHandle);
    ixxatVciDeviceHandle = NULL;
  }
} /*** end of IxxatVciDisconnect ***/


/************************************************************************************//**
** \brief     Submits a message for transmission on the CAN bus.
** \param     msg Pointer to CAN message structure.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool IxxatVciTransmit(tCanMsg const * msg)
{
  bool result = false;
  tCanEvents const * pEvents;
  CANMSG  sCanMsg = { 0 };
   
  /* Check parameters. */
  assert(msg != NULL);

  /* Only continue with valid parameters. */
  if (msg != NULL) /*lint !e774 */
  {
    /* Convert the message to a type supported by the Ixxat VCI driver API. */
    if ((msg->id & CAN_MSG_EXT_ID_MASK) == 0)
    {
      sCanMsg.dwMsgId = msg->id & 0x7ffu;
    }
    else
    {
      sCanMsg.dwMsgId = msg->id & 0x1fffffffu;
      sCanMsg.uMsgInfo.Bits.ext = 1;
    }
    sCanMsg.uMsgInfo.Bytes.bType = CAN_MSGTYPE_DATA;
    sCanMsg.uMsgInfo.Bits.dlc = ((msg->dlc <= CAN_MSG_MAX_LEN) ? msg->dlc : CAN_MSG_MAX_LEN);
    for (uint8_t idx = 0; idx < sCanMsg.uMsgInfo.Bits.dlc; idx++)
    {
      sCanMsg.abData[idx] = msg->data[idx];
    }
    
    /* Submit CAN message for transmission. */
    if (IxxatVciLibFuncCanChannelPostMessage(ixxatCanChannelHandle, &sCanMsg) == VCI_OK)
    {
      /* Update result value to success. */
      result = true;
      /* Trigger transmit complete event(s). */
      pEvents = ixxatVciEventsList;
      for (uint32_t idx = 0; idx < ixxatVciEventsEntries; idx++)
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
} /*** end of IxxatVciTransmit ***/


/************************************************************************************//**
** \brief     Checks if a bus off or bus heavy situation occurred.
** \return    True if a bus error situation was detected, false otherwise.
**
****************************************************************************************/
static bool IxxatVciIsBusError(void)
{
  bool result = false;
  CANCHANSTATUS sChanStatus;

  /* Attempt to obtain the channel status. */
  if (IxxatVciLibFuncCanChannelGetStatus(ixxatCanChannelHandle, &sChanStatus) == VCI_OK)
  {
    /* Was a bus off event or bus heavy situation detected? */
    if ((sChanStatus.sLineStatus.dwStatus & (CAN_STATUS_BUSOFF | CAN_STATUS_ERRLIM)) != 0)
    {
      result = true;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciIsBusError ***/


/************************************************************************************//**
** \brief     Registers the event callback functions that should be called by the CAN
**            interface.
** \param     events Pointer to structure with event callback function pointers.
**
****************************************************************************************/
static void IxxatVciRegisterEvents(tCanEvents const * events)
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
    ixxatVciEventsList = realloc(ixxatVciEventsList,
                                (sizeof(tCanEvents) * (ixxatVciEventsEntries + 1)));
    /* Assert reallocation. */
    assert(ixxatVciEventsList != NULL);
    /* Only continue if reallocation was successful. */
    if (ixxatVciEventsList != NULL)
    {
      /* Increment events entry count. */
      ixxatVciEventsEntries++;
      /* Store the events in the new entry. */
      ixxatVciEventsList[ixxatVciEventsEntries - 1] = *events;
    }
    /* Reallocation failed. */
    else
    {
      /* Reset events entry count. */
      ixxatVciEventsEntries = 0;
    }
  }
} /*** end of IxxatVciRegisterEvents ***/


/************************************************************************************//**
** \brief     CAN message reception thread.
** \param     pv Pointer to thread parameters.
** \return    Thread exit code.
**
****************************************************************************************/
static DWORD WINAPI IxxatVciReceptionThread(LPVOID pv)
{
  DWORD waitResult;
  bool running = true;
  CANMSG sCanMsg;
  tCanMsg rxMsg;
  tCanEvents const* pEvents;

  /* Parameter not used. */
  (void)pv;

  /* Enter thread's infinite loop. */
  while (running)
  {
    if (IxxatVciLibFuncCanChannelReadMessage(ixxatCanChannelHandle, 5, &sCanMsg) == VCI_OK)
    {
      /* Only process data frames. */
      if (sCanMsg.uMsgInfo.Bytes.bType == CAN_MSGTYPE_DATA)
      {
        /* Convert CAN mesage from Ixxat VCI API format to the one of the CAN driver
         * module.
         */
        rxMsg.id = sCanMsg.dwMsgId;
        if (sCanMsg.uMsgInfo.Bits.ext)
        {
          rxMsg.id |= CAN_MSG_EXT_ID_MASK;
        }
        rxMsg.dlc = sCanMsg.uMsgInfo.Bits.dlc;
        for (uint8_t idx = 0; idx < rxMsg.dlc; idx++)
        {
          rxMsg.data[idx] = sCanMsg.abData[idx];
        }

        /* Trigger message reception event(s). */
        pEvents = ixxatVciEventsList;
        for (uint32_t idx = 0; idx < ixxatVciEventsEntries; idx++)
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

    /* Check if the terminate event is set, without waiting for it. */
    waitResult = WaitForSingleObject(ixxatVciTerminateEvent, 0);
    if (waitResult == WAIT_OBJECT_0) /*lint !e835 Cannot change the WIN32 API macro */
    {
      /* Stop thread. */
      running = false;
    }
  }
  /* Exit thread. */
  return 0;
} /*** end of IxxatVciReceptionThread ***/


/************************************************************************************//**
** \brief     Converts the baudrate enum value to the Ixxat API's BTR0 and BTR1 values.
** \param     bBtr0 Storage for the BTR0 setting.
** \param     bBtr1 Storage for the BTR1 setting.
** \return    True if the baudrate could be converted, false otherwise.
**
****************************************************************************************/
static bool IxxatVciConvertBaudrate(UINT8* bBtr0, UINT8* bBtr1)
{
  bool result = true;

  /* Convert the baudrate to a BTR0/1 values supported by the Ixxat API. */
  switch(ixxatVciSettings.baudrate)
  {
  case CAN_BR10K:
    *bBtr0 = 0x31;
    *bBtr1 = 0x1C;
    break;
  case CAN_BR20K:
    *bBtr0 = 0x18;
    *bBtr1 = 0x1C;
    break;
  case CAN_BR50K:
    *bBtr0 = 0x09;
    *bBtr1 = 0x1C;
    break;
  case CAN_BR100K:
    *bBtr0 = 0x04;
    *bBtr1 = 0x1C;
    break;
  case CAN_BR125K:
    *bBtr0 = 0x03;
    *bBtr1 = 0x1C;
    break;
  case CAN_BR250K:
    *bBtr0 = 0x01;
    *bBtr1 = 0x1C;
    break;
  case CAN_BR500K:
    *bBtr0 = 0x00;
    *bBtr1 = 0x1C;
    break;
  case CAN_BR800K:
    *bBtr0 = 0x00;
    *bBtr1 = 0x16;
    break;
  case CAN_BR1M:
    *bBtr0 = 0x00;
    *bBtr1 = 0x14;
    break;
  default:
    result = false;
    break;
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciConvertBaudrate ***/


/************************************************************************************//**
** \brief     Loads the Ixxat VCI DLL and initializes the API function pointers.
**
****************************************************************************************/
static void IxxatVciLibLoadDll(void)
{
  /* Start out by resetting the API function pointers. */
  ixxatVciLibFuncVciEnumDeviceOpenPtr = NULL;
  ixxatVciLibFuncVciEnumDeviceClosePtr = NULL;
  ixxatVciLibFuncVciEnumDeviceNextPtr = NULL;
  ixxatVciLibFuncVciDeviceOpenPtr = NULL;
  ixxatVciLibFuncVciDeviceClosePtr = NULL;
  ixxatVciLibFuncCanControlOpenPtr = NULL;
  ixxatVciLibFuncCanControlResetPtr = NULL;
  ixxatVciLibFuncCanControlGetCapsPtr = NULL;
  ixxatVciLibFuncCanControlInitializePtr = NULL;
  ixxatVciLibFuncCanControlSetAccFilterPtr = NULL;
  ixxatVciLibFuncCanControlStartPtr = NULL;
  ixxatVciLibFuncCanControlClosePtr = NULL;
  ixxatVciLibFuncCanChannelOpenPtr = NULL;
  ixxatVciLibFuncCanChannelGetStatusPtr = NULL;
  ixxatVciLibFuncCanChannelInitializePtr = NULL;
  ixxatVciLibFuncCanChannelActivatePtr = NULL;
  ixxatVciLibFuncCanChannelPostMessagePtr = NULL;
  ixxatVciLibFuncCanChannelReadMessagePtr = NULL;
  ixxatVciLibFuncCanChannelClosePtr = NULL;

  /* Attempt to load the library and obtain a handle to it. */
  ixxatVciDllHandle = LoadLibrary("vcinpl");

  /* Assert libary handle. */
  assert(ixxatVciDllHandle != NULL);

  /* Only continue if the library was successfully loaded */
  if (ixxatVciDllHandle != NULL) /*lint !e774 */
  {
    /* Set vciEnumDeviceOpen function pointer. */
    ixxatVciLibFuncVciEnumDeviceOpenPtr = (PF_vciEnumDeviceOpen)GetProcAddress(ixxatVciDllHandle, "vciEnumDeviceOpen");
    /* Set vciEnumDeviceClose function pointer. */
    ixxatVciLibFuncVciEnumDeviceClosePtr = (PF_vciEnumDeviceClose)GetProcAddress(ixxatVciDllHandle, "vciEnumDeviceClose");
    /* Set vciEnumDeviceNext function pointer. */
    ixxatVciLibFuncVciEnumDeviceNextPtr = (PF_vciEnumDeviceNext)GetProcAddress(ixxatVciDllHandle, "vciEnumDeviceNext");
    /* Set vciDeviceOpen function pointer. */
    ixxatVciLibFuncVciDeviceOpenPtr = (PF_vciDeviceOpen)GetProcAddress(ixxatVciDllHandle, "vciDeviceOpen");
    /* Set vciDeviceClose function pointer. */
    ixxatVciLibFuncVciDeviceClosePtr = (PF_vciDeviceClose)GetProcAddress(ixxatVciDllHandle, "vciDeviceClose");
    /* Set canControlOpen function pointer. */
    ixxatVciLibFuncCanControlOpenPtr = (PF_canControlOpen)GetProcAddress(ixxatVciDllHandle, "canControlOpen");
    /* Set canControlReset function pointer. */
    ixxatVciLibFuncCanControlResetPtr = (PF_canControlReset)GetProcAddress(ixxatVciDllHandle, "canControlReset");
    /* Set canControlGetCaps function pointer. */
    ixxatVciLibFuncCanControlGetCapsPtr = (PF_canControlGetCaps)GetProcAddress(ixxatVciDllHandle, "canControlGetCaps");
    /* Set canControlInitialize function pointer. */
    ixxatVciLibFuncCanControlInitializePtr = (PF_canControlInitialize)GetProcAddress(ixxatVciDllHandle, "canControlInitialize");
    /* Set canControlSetAccFilter function pointer. */
    ixxatVciLibFuncCanControlSetAccFilterPtr = (PF_canControlSetAccFilter)GetProcAddress(ixxatVciDllHandle, "canControlSetAccFilter");
    /* Set canControlStart function pointer. */
    ixxatVciLibFuncCanControlStartPtr = (PF_canControlStart)GetProcAddress(ixxatVciDllHandle, "canControlStart");
    /* Set canControlClose function pointer. */
    ixxatVciLibFuncCanControlClosePtr = (PF_canControlClose)GetProcAddress(ixxatVciDllHandle, "canControlClose");
    /* Set canChannelOpen function pointer. */
    ixxatVciLibFuncCanChannelOpenPtr = (PF_canChannelOpen)GetProcAddress(ixxatVciDllHandle, "canChannelOpen");
    /* Set canChannelGetStatus function pointer. */
    ixxatVciLibFuncCanChannelGetStatusPtr = (PF_canChannelGetStatus)GetProcAddress(ixxatVciDllHandle, "canChannelGetStatus");
    /* Set canChannelInitialize function pointer. */
    ixxatVciLibFuncCanChannelInitializePtr = (PF_canChannelInitialize)GetProcAddress(ixxatVciDllHandle, "canChannelInitialize");
    /* Set canChannelActivate function pointer. */
    ixxatVciLibFuncCanChannelActivatePtr = (PF_canChannelActivate)GetProcAddress(ixxatVciDllHandle, "canChannelActivate");
    /* Set canChannelPostMessage function pointer. */
    ixxatVciLibFuncCanChannelPostMessagePtr = (PF_canChannelPostMessage)GetProcAddress(ixxatVciDllHandle, "canChannelPostMessage");
    /* Set canChannelReadMessage function pointer. */
    ixxatVciLibFuncCanChannelReadMessagePtr = (PF_canChannelReadMessage)GetProcAddress(ixxatVciDllHandle, "canChannelReadMessage");
    /* Set canChannelClose function pointer. */
    ixxatVciLibFuncCanChannelClosePtr = (PF_canChannelClose)GetProcAddress(ixxatVciDllHandle, "canChannelClose");
  }
} /*** end of IxxatVciLibLoadDll ***/


/************************************************************************************//**
** \brief     Unloads the Ixxat VCI DLL and resets the API function pointers.
**
****************************************************************************************/
static void IxxatVciLibUnloadDll(void)
{
  /* Reset the API function pointers. */
  ixxatVciLibFuncVciEnumDeviceOpenPtr = NULL;
  ixxatVciLibFuncVciEnumDeviceClosePtr = NULL;
  ixxatVciLibFuncVciEnumDeviceNextPtr = NULL;

  /* Unload the library and invalidate its handle. */
  if (ixxatVciDllHandle != NULL)
  {
    (void)FreeLibrary(ixxatVciDllHandle);
    ixxatVciDllHandle = NULL;
  }
} /*** end of IxxatVciLibUnloadDll ***/


/************************************************************************************//**
** \brief     Opens the list of all fieldbus adapters registered with the VCI.
** \param     hEnum Address of a variable of type HANDLE. If run successfully, the
**            function returns the handle of the opened device list in this variable.
**            In the case of an error, the variable is set to ZERO.
** \return    VCI_OK if successful.
**
****************************************************************************************/
static HRESULT IxxatVciLibFuncVciEnumDeviceOpen(PHANDLE hEnum)
{
  HRESULT result = VCI_E_UNEXPECTED; /*lint !e648 Cannot change Ixxat VCI API macro */

  /* Check function pointer and library handle. */
  assert(ixxatVciLibFuncVciEnumDeviceOpenPtr != NULL);
  assert(ixxatVciDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((ixxatVciLibFuncVciEnumDeviceOpenPtr != NULL) && (ixxatVciDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = ixxatVciLibFuncVciEnumDeviceOpenPtr(hEnum);
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciLibFuncVciEnumDeviceOpen ***/


/************************************************************************************//**
** \brief     Closes the device list opened with the function vciEnumDeviceOpen.
** \param     hEnum Handle of the device list to be closed.
** \return    VCI_OK if successful.
**
****************************************************************************************/
static HRESULT IxxatVciLibFuncVciEnumDeviceClose(HANDLE hEnum)
{
  HRESULT result = VCI_E_UNEXPECTED; /*lint !e648 Cannot change Ixxat VCI API macro */

  /* Check function pointer and library handle. */
  assert(ixxatVciLibFuncVciEnumDeviceClosePtr != NULL);
  assert(ixxatVciDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((ixxatVciLibFuncVciEnumDeviceClosePtr != NULL) && (ixxatVciDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = ixxatVciLibFuncVciEnumDeviceClosePtr(hEnum);
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciLibFuncVciEnumDeviceClose ***/


/************************************************************************************//**
** \brief     Determines the description of a fieldbus adapter of the device list and
**            increases the internal list index so that a subsequent call of the function
**            supplies the description to the next adapter.
** \param     hEnum Handle to the opened device list.
** \param     pInfo Address of a data structure of type VCIDEVICEINFO. If run
**            successfully, the function saves information on the adapter in the
**            memory area specified here.
** \return    VCI_OK if successful, VCI_E_NO_MORE_ITEMS if the list does not contain
**            any more entries, otherwise an error occurred.
**
****************************************************************************************/
static HRESULT IxxatVciLibFuncVciEnumDeviceNext(HANDLE hEnum, PVCIDEVICEINFO pInfo)
{
  HRESULT result = VCI_E_UNEXPECTED; /*lint !e648 Cannot change Ixxat VCI API macro */

  /* Check function pointer and library handle. */
  assert(ixxatVciLibFuncVciEnumDeviceNextPtr != NULL);
  assert(ixxatVciDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((ixxatVciLibFuncVciEnumDeviceNextPtr != NULL) && (ixxatVciDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = ixxatVciLibFuncVciEnumDeviceNextPtr(hEnum, pInfo);
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciLibFuncVciEnumDeviceNext ***/


/************************************************************************************//**
** \brief     Opens the fieldbus adapter with the specified device ID.
** \param     rVciid Device ID of the adapter to be opened.
** \param     phDevice Address of a variable of type HANDLE. If run successfully, the
**            function returns the handle of the opened adapter in this variable. In
**            the event of an error, the variable is set to ZERO.
** \return    VCI_OK if successful.
**
****************************************************************************************/
static HRESULT IxxatVciLibFuncVciDeviceOpen(REFVCIID rVciid, PHANDLE  phDevice)
{
  HRESULT result = VCI_E_UNEXPECTED; /*lint !e648 Cannot change Ixxat VCI API macro */

  /* Check function pointer and library handle. */
  assert(ixxatVciLibFuncVciDeviceOpenPtr != NULL);
  assert(ixxatVciDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((ixxatVciLibFuncVciDeviceOpenPtr != NULL) && (ixxatVciDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = ixxatVciLibFuncVciDeviceOpenPtr(rVciid, phDevice);
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciLibFuncVciDeviceOpen ***/


/************************************************************************************//**
** \brief     Closes an opened fieldbus adapter.
** \param     hDevice Handle of the adapter to be closed. The specified handle must
**            come from a call of one of the functions vciEnumDeviceOpen or
**            vciDeviceOpenDlg.
** \return    VCI_OK if successful.
**
****************************************************************************************/
static HRESULT IxxatVciLibFuncVciDeviceClose(HANDLE hDevice)
{
  HRESULT result = VCI_E_UNEXPECTED; /*lint !e648 Cannot change Ixxat VCI API macro */

  /* Check function pointer and library handle. */
  assert(ixxatVciLibFuncVciDeviceClosePtr != NULL);
  assert(ixxatVciDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((ixxatVciLibFuncVciDeviceClosePtr != NULL) && (ixxatVciDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = ixxatVciLibFuncVciDeviceClosePtr(hDevice);
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciLibFuncVciDeviceClose ***/


/************************************************************************************//**
** \brief     Opens the control unit of a CAN connection on a fieldbus adapter.
** \param     hDevice Handle of the fieldbus adapter.
** \param     dwCanNo Number of the CAN connection of the control unit to be opened. The
**            value 0 selects the first connection, the value 1 the second connection and
**            so on.
** \param     phCanCtl Pointer to a variable of type HANDLE. If run successfully, the
**            function returns the handle of the opened CAN controller in this variable.
**            In the event of an error, the variable is set to ZERO.
** \return    VCI_OK if successful.
**
****************************************************************************************/
static HRESULT IxxatVciLibFuncCanControlOpen(HANDLE hDevice, UINT32 dwCanNo, PHANDLE phCanCtl)
{
  HRESULT result = VCI_E_UNEXPECTED; /*lint !e648 Cannot change Ixxat VCI API macro */

  /* Check function pointer and library handle. */
  assert(ixxatVciLibFuncCanControlOpenPtr != NULL);
  assert(ixxatVciDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((ixxatVciLibFuncCanControlOpenPtr != NULL) && (ixxatVciDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = ixxatVciLibFuncCanControlOpenPtr(hDevice, dwCanNo, phCanCtl);
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciLibFuncCanControlOpen ***/


/************************************************************************************//**
** \brief     Resets the controller hardware and resets the message filters of a CAN 
**            connection.
** \param     hCanCtl Handle of the opened CAN controller.
** \return    VCI_OK if successful.
**
****************************************************************************************/
static HRESULT IxxatVciLibFuncCanControlReset(HANDLE hCanCtl)
{
  HRESULT result = VCI_E_UNEXPECTED; /*lint !e648 Cannot change Ixxat VCI API macro */

  /* Check function pointer and library handle. */
  assert(ixxatVciLibFuncCanControlResetPtr != NULL);
  assert(ixxatVciDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((ixxatVciLibFuncCanControlResetPtr != NULL) && (ixxatVciDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = ixxatVciLibFuncCanControlResetPtr(hCanCtl);
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciLibFuncCanControlReset ***/


/************************************************************************************//**
** \brief     Determines the features of a CAN connection.
** \param     hCanCtl Handle of the opened CAN controller.
** \param     pCanCaps Pointer to a structure of type CANCAPABILITIES. If run
**            successfully, the function saves the features of the CAN connection in the
**            memory area specified here.
** \return    VCI_OK if successful.
**
****************************************************************************************/
static HRESULT IxxatVciLibFuncCanControlGetCaps(HANDLE hCanCtl, PCANCAPABILITIES pCanCaps)
{
  HRESULT result = VCI_E_UNEXPECTED; /*lint !e648 Cannot change Ixxat VCI API macro */

  /* Check function pointer and library handle. */
  assert(ixxatVciLibFuncCanControlGetCapsPtr != NULL);
  assert(ixxatVciDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((ixxatVciLibFuncCanControlGetCapsPtr != NULL) && (ixxatVciDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = ixxatVciLibFuncCanControlGetCapsPtr(hCanCtl, pCanCaps);
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciLibFuncCanControlGetCaps ***/


/************************************************************************************//**
** \brief     Sets the operating mode and bit rate of a CAN connection.
** \param     hCanCtl Handle of the opened CAN controller.
** \param     bMode Operating mode of the CAN controller.
** \param     bBtr0 Value for the bus timing register 0 of the CAN controller. The value
**            corresponds to the BTR0 register of the Philips SJA 1000 CAN controller
**            with a cycle frequency of 16 MHz.
** \param     bBtr1 Value for the bus timing register 1 of the CAN controller. The value
**            corresponds to the BTR1 register of the Philips SJA 1000 CAN controller
**            with a cycle frequency of 16 MHz.
** \return    VCI_OK if successful.
**
****************************************************************************************/
static HRESULT IxxatVciLibFuncCanControlInitialize(HANDLE hCanCtl, UINT8 bMode, UINT8 bBtr0, UINT8 bBtr1)
{
  HRESULT result = VCI_E_UNEXPECTED; /*lint !e648 Cannot change Ixxat VCI API macro */

  /* Check function pointer and library handle. */
  assert(ixxatVciLibFuncCanControlInitializePtr != NULL);
  assert(ixxatVciDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((ixxatVciLibFuncCanControlInitializePtr != NULL) && (ixxatVciDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = ixxatVciLibFuncCanControlInitializePtr(hCanCtl, bMode, bBtr0, bBtr1);
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciLibFuncCanControlInitialize ***/


/************************************************************************************//**
** \brief     Sets the 11- or 29-bit acceptance filter of a CAN connection.
** \param     hCanCtl Handle of the opened CAN controller.
** \param     fExtend Selection of the acceptance filter. The 11-bit acceptance filter is
**            selected with the value FALSE and the 29-bit acceptance filter with the
**            value TRUE.
** \param     dwCode Bit sample of the identifier(s) to be accepted including RTR-bit.
** \param     dwMask Bit sample of the relevant bits in dwCode. If a bit has the value 0
**            in dwMask, the corresponding bit in dwCode is not used for the comparison.
**            If a bit has the value 1, it is relevant for the comparison.
** \return    VCI_OK if successful.
**
****************************************************************************************/
static HRESULT IxxatVciLibFuncCanControlSetAccFilter(HANDLE hCanCtl, BOOL fExtend, UINT32 dwCode, UINT32 dwMask)
{
  HRESULT result = VCI_E_UNEXPECTED; /*lint !e648 Cannot change Ixxat VCI API macro */

  /* Check function pointer and library handle. */
  assert(ixxatVciLibFuncCanControlSetAccFilterPtr != NULL);
  assert(ixxatVciDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((ixxatVciLibFuncCanControlSetAccFilterPtr != NULL) && (ixxatVciDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = ixxatVciLibFuncCanControlSetAccFilterPtr(hCanCtl, fExtend, dwCode, dwMask);
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciLibFuncCanControlSetAccFilter ***/


/************************************************************************************//**
** \brief     Starts or stops the controller of a CAN connection.
** \param     hCanCtl Handle of the opened CAN controller.
** \param     fStart The value TRUE starts and the value FALSE stops the CAN controller.
** \return    VCI_OK if successful.
**
****************************************************************************************/
static HRESULT IxxatVciLibFuncCanControlStart(HANDLE hCanCtl, BOOL fStart)
{
  HRESULT result = VCI_E_UNEXPECTED; /*lint !e648 Cannot change Ixxat VCI API macro */

  /* Check function pointer and library handle. */
  assert(ixxatVciLibFuncCanControlStartPtr != NULL);
  assert(ixxatVciDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((ixxatVciLibFuncCanControlStartPtr != NULL) && (ixxatVciDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = ixxatVciLibFuncCanControlStartPtr(hCanCtl, fStart);
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciLibFuncCanControlStart ***/


/************************************************************************************//**
** \brief     Closes an opened CAN controller.
** \param     hCanCtl Handle of the CAN controller to be closed. The specified handle
**            must come from a call of the function canControlOpen.
** \return    VCI_OK if successful.
**
****************************************************************************************/
static HRESULT IxxatVciLibFuncCanControlClose(HANDLE hCanCtl)
{
  HRESULT result = VCI_E_UNEXPECTED; /*lint !e648 Cannot change Ixxat VCI API macro */

  /* Check function pointer and library handle. */
  assert(ixxatVciLibFuncCanControlClosePtr != NULL);
  assert(ixxatVciDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((ixxatVciLibFuncCanControlClosePtr != NULL) && (ixxatVciDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = ixxatVciLibFuncCanControlClosePtr(hCanCtl);
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciLibFuncCanControlClose ***/


/************************************************************************************//**
** \brief     Opens or creates a message channel for a CAN connection of a fieldbus
**            adapter.
** \param     hDevice Handle of the fieldbus adapter.
** \param     dwCanNo Number of the CAN connection for which a message channel is to
**            be opened. The value 0 selects the first connection, the value 1 the
**            second connection and so on.
** \param     fExclusive Defines whether the connection is used exclusively for the
**            channel to be opened. If the value TRUE is specified here, the CAN
**            connection is used exclusively for the new message channel. With the value
**            FALSE, more than one message channel can be opened for the CAN connection.
** \param     phCanChn Pointer to a variable of type HANDLE. If run successfully, the
**            function returns the handle of the opened CAN message channel in this
**            variable. In the event of an of an error, the variable is set to ZERO.
** \return    VCI_OK if successful.
**
****************************************************************************************/
static HRESULT IxxatVciLibFuncCanChannelOpen(HANDLE hDevice, UINT32 dwCanNo, BOOL fExclusive, PHANDLE phCanChn)
{
  HRESULT result = VCI_E_UNEXPECTED; /*lint !e648 Cannot change Ixxat VCI API macro */

  /* Check function pointer and library handle. */
  assert(ixxatVciLibFuncCanChannelOpenPtr != NULL);
  assert(ixxatVciDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((ixxatVciLibFuncCanChannelOpenPtr != NULL) && (ixxatVciDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = ixxatVciLibFuncCanChannelOpenPtr(hDevice, dwCanNo, fExclusive, phCanChn);
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciLibFuncCanChannelOpen ***/


/************************************************************************************//**
** \brief     Determines the current status of a message channel as well as the current
**            settings and the current status of the controller that is connected to the
**            channel.
** \param     hCanChn Handle of the opened message channel.
** \param     pStatus Pointer to a structure of type CANCHANSTATUS. If run successfully,
**            the function saves the current status of the channel and controller in the
**            memory area specified here.
** \return    VCI_OK if successful.
**
****************************************************************************************/
static HRESULT IxxatVciLibFuncCanChannelGetStatus(HANDLE hCanChn, PCANCHANSTATUS pStatus)
{
  HRESULT result = VCI_E_UNEXPECTED; /*lint !e648 Cannot change Ixxat VCI API macro */

  /* Check function pointer and library handle. */
  assert(ixxatVciLibFuncCanChannelGetStatusPtr != NULL);
  assert(ixxatVciDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((ixxatVciLibFuncCanChannelGetStatusPtr != NULL) && (ixxatVciDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = ixxatVciLibFuncCanChannelGetStatusPtr(hCanChn, pStatus);
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciLibFuncCanChannelGetStatus ***/


/************************************************************************************//**
** \brief     Initializes the receive and transmit buffers of a message channel.
** \param     hCanChn Handle of the opened message channel.
** \param     wRxFifoSize Size of the receive buffer in number of CAN messages.
** \param     wRxThreshold Threshold value for the receive event. The event is triggered
**            when the number of messages in the receive buffer reaches or exceeds
**            the number specified here.
** \param     wTxFifoSize Size of the transmit buffer in number of CAN messages.
** \param     wTxThreshold Threshold value for the transmit event. The event is triggered
**            when the number of free entries in the transmit buffer reaches or exceeds
**            the number specified here.
** \return    VCI_OK if successful.
**
****************************************************************************************/
static HRESULT IxxatVciLibFuncCanChannelInitialize(HANDLE hCanChn, UINT16 wRxFifoSize, UINT16 wRxThreshold, UINT16 wTxFifoSize, UINT16 wTxThreshold)
{
  HRESULT result = VCI_E_UNEXPECTED; /*lint !e648 Cannot change Ixxat VCI API macro */

  /* Check function pointer and library handle. */
  assert(ixxatVciLibFuncCanChannelInitializePtr != NULL);
  assert(ixxatVciDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((ixxatVciLibFuncCanChannelInitializePtr != NULL) && (ixxatVciDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = ixxatVciLibFuncCanChannelInitializePtr(hCanChn, wRxFifoSize, wRxThreshold, wTxFifoSize, wTxThreshold);
  }

  /* Give the result back to the caller. */
  return result;

} /*** end of IxxatVciLibFuncCanChannelInitialize ***/


/************************************************************************************//**
** \brief     Activates or deactivates a message channel.
** \param     hCanChn Handle of the opened message channel.
** \param     fEnable With the value TRUE, the function activates the message flow
**            between the CAN controller and the message channel, with the value FALSE
**            the function deactivates the message flow.
** \return    VCI_OK if successful.
**
****************************************************************************************/
static HRESULT IxxatVciLibFuncCanChannelActivate(HANDLE hCanChn, BOOL fEnable)
{
  HRESULT result = VCI_E_UNEXPECTED; /*lint !e648 Cannot change Ixxat VCI API macro */

  /* Check function pointer and library handle. */
  assert(ixxatVciLibFuncCanChannelActivatePtr != NULL);
  assert(ixxatVciDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((ixxatVciLibFuncCanChannelActivatePtr != NULL) && (ixxatVciDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = ixxatVciLibFuncCanChannelActivatePtr(hCanChn, fEnable);
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciLibFuncCanChannelActivate ***/


/************************************************************************************//**
** \brief     Writes a CAN message in the transmit buffer of the specified message channel.
** \param     hCanChn Handle of the opened message channel.
** \param     pCanMsg Pointer to an initialized structure of type CANMSG with the CAN
**            message to be transmitted.
** \return    VCI_OK if successful.
**
****************************************************************************************/
static HRESULT IxxatVciLibFuncCanChannelPostMessage(HANDLE hCanChn, PCANMSG pCanMsg)
{
  HRESULT result = VCI_E_UNEXPECTED; /*lint !e648 Cannot change Ixxat VCI API macro */

  /* Check function pointer and library handle. */
  assert(ixxatVciLibFuncCanChannelPostMessagePtr != NULL);
  assert(ixxatVciDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((ixxatVciLibFuncCanChannelPostMessagePtr != NULL) && (ixxatVciDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = ixxatVciLibFuncCanChannelPostMessagePtr(hCanChn, pCanMsg);
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciLibFuncCanChannelPostMessage ***/


/************************************************************************************//**
** \brief     Retrieves the next CAN message from the receive FIFO of the specified CAN
**            channel. The function waits for a message to be received from the CAN bus.
** \param     hCanChn Handle of the opened message channel.
** \param     dwTimeout Maximum waiting time in milliseconds. The function returns to the
**            caller with the error code VCI_E_TIMEOUT if no message is read or received
**            within the specified time. With the value INFINITE (0xFFFFFFFF), the
**            function waits until a message has been read.
** \param     pCanMsg Pointer to a CANMSG structure where the function stores the
**            retrieved CAN message. If this parameter is set to NULL, the function
**            simply removes the next CAN message from the FIFO.
** \return    VCI_OK if successful, VCI_E_RXQUEUE_EMPTY if there currently is no CAN
**            message available, VCI_E_TIMEOUT if the time-out interval elapsed without
**            a new CAN message available. Another value indicates a generic error. 
**
****************************************************************************************/
static HRESULT IxxatVciLibFuncCanChannelReadMessage(HANDLE hCanChn, UINT32 dwTimeout, PCANMSG pCanMsg)
{
  HRESULT result = VCI_E_UNEXPECTED; /*lint !e648 Cannot change Ixxat VCI API macro */

  /* Check function pointer and library handle. */
  assert(ixxatVciLibFuncCanChannelReadMessagePtr != NULL);
  assert(ixxatVciDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((ixxatVciLibFuncCanChannelReadMessagePtr != NULL) && (ixxatVciDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = ixxatVciLibFuncCanChannelReadMessagePtr(hCanChn, dwTimeout, pCanMsg);
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciLibFuncCanChannelReadMessage ***/


/************************************************************************************//**
** \brief     Closes an opened message channel.
** \param     hCanChn Handle of the message channel to be closed. The specified handle
**            must come from a call of the function canChannelOpen.
** \return    VCI_OK if successful.
**
****************************************************************************************/
static HRESULT IxxatVciLibFuncCanChannelClose(HANDLE hCanChn)
{
  HRESULT result = VCI_E_UNEXPECTED; /*lint !e648 Cannot change Ixxat VCI API macro */

  /* Check function pointer and library handle. */
  assert(ixxatVciLibFuncCanChannelClosePtr != NULL);
  assert(ixxatVciDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((ixxatVciLibFuncCanChannelClosePtr != NULL) && (ixxatVciDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = ixxatVciLibFuncCanChannelClosePtr(hCanChn);
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of IxxatVciLibFuncCanChannelClose ***/


/*********************************** end of vcidriver.c ********************************/
