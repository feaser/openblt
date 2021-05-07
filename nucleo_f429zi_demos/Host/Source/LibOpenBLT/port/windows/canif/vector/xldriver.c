/************************************************************************************//**
* \file         xldriver.c
* \brief        Vector XL driver interface source file.
* \ingroup      XLDRIVER
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2019  by Feaser    http://www.feaser.com    All rights reserved
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
#include "util.h"                           /* Utility module                          */
#include "xldriver.h"                       /* Vector XL driver interface              */
#include <windows.h>                        /* for Windows API                         */
#include <vxlapi.h>                         /* for XL CAN driver library               */

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Internal driver queue size in CAN events. */
#define VECTOR_XL_RX_QUEUE_SIZE        (4096u)


/***************************************************************************************
* Function prototypes
****************************************************************************************/
/* CAN interface functions. */
static void VectorXlInit(tCanSettings const * settings);
static void VectorXlTerminate(void);
static bool VectorXlConnect(void);
static void VectorXlDisconnect(void);
static bool VectorXlTransmit(tCanMsg const * msg);
static bool VectorXlIsBusError(void);
static void VectorXlRegisterEvents(tCanEvents const * events);
/* Baudrate conversion utility function. */
static uint32_t VectorXlConvertToRawBitrate(tCanBaudrate baudrate);
/* CAN event reception thread. */
static DWORD WINAPI VectorXlReceptionThread(LPVOID pv);


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief CAN interface structure filled with Vector XL driver specifics. */
static const tCanInterface pVectorXlInterface =
{
  VectorXlInit,
  VectorXlTerminate,
  VectorXlConnect,
  VectorXlDisconnect,
  VectorXlTransmit,
  VectorXlIsBusError,
  VectorXlRegisterEvents
};


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief The settings to use in this CAN interface. */
static tCanSettings vectorXlSettings;

/** \brief List with callback functions that this driver should use. */
static tCanEvents * vectorXlEventsList;

/** \brief Total number of event entries into the \ref vectorXlEventsList list. */
static uint32_t vectorXlEventsEntries;

/** \brief The handle to the CAN port needed for API functions. */
static XLportHandle vectorXlPortHandle;

/** \brief The mask for the configured CAN channel. */
static XLaccess vectorXLChannelMask;

/** \brief Boolean flag to track if the driver was opened or not. ***/
static bool vectorXlDriverOpened;

/** \brief Boolean flag to track if the port was opened or not. ***/
static bool vectorXlPortOpened;

/** \brief Boolean flag to track if the channel was activated or not. ***/
static bool vectorXlChannelActivated;

/** \brief Boolean flag to detect if a CAN bus error state was detected. ***/
static bool vectorXlBusErrorDetected;

/** \brief Handle for the event to terminate the reception thread. */
static HANDLE vectorXlTerminateEvent;

/** \brief Handle for a CAN related event. */
static HANDLE vectorXlCanEvent;

/** \brief Handle for the CAN reception thread. */
static HANDLE vectorXlRxThreadHandle;


/***********************************************************************************//**
** \brief     Obtains a pointer to the CAN interface structure, so that it can be linked
**            to the generic CAN driver module.
** \return    Pointer to CAN interface structure.
**
****************************************************************************************/
tCanInterface const * VectorXlGetInterface(void)
{
  return &pVectorXlInterface;
} /*** end of VectorXlGetInterface ***/


/************************************************************************************//**
** \brief     Initializes the CAN interface.
** \param     settings Pointer to the CAN interface settings.
**
****************************************************************************************/
static void VectorXlInit(tCanSettings const * settings)
{
  char           * canDeviceName;
  XLstatus         xlStatus = XL_ERROR;

  /* Initialize locals. */
  vectorXlEventsList = NULL;
  vectorXlEventsEntries = 0;
  vectorXlPortHandle = XL_INVALID_PORTHANDLE;
  vectorXLChannelMask = 0;
  vectorXlDriverOpened = false;
  vectorXlPortOpened = false;
  vectorXlChannelActivated = false;
  vectorXlBusErrorDetected = false;
  vectorXlTerminateEvent = NULL;
  vectorXlCanEvent = NULL;
  vectorXlRxThreadHandle = NULL;
  
  /* This module uses critical sections so initialize them. */
  UtilCriticalSectionInit();

  /* Reset CAN interface settings. */
  vectorXlSettings.devicename = "";
  vectorXlSettings.channel = 0;
  vectorXlSettings.baudrate = CAN_BR500K;
  vectorXlSettings.code = 0x00000000u;
  vectorXlSettings.mask = 0x00000000u;

  /* Check parameters. */
  assert(settings != NULL);

  /* Only continue with valid parameters. */
  if (settings != NULL) /*lint !e774 */
  {
    /* Shallow copy the CAN interface settings for later usage. */
    vectorXlSettings = *settings;
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
        vectorXlSettings.devicename = canDeviceName;
      }
    }

    /* Open the XL driver and dynamically load its functions. */
    xlStatus = xlOpenDriver();
    /* Assert that the driver was opened. If it could not be opened, that means that
     * either the XL driver was not installed on the system (vxlapi.dll/vxlapi64.dll)
     * or in incompatible vection is installed. Either way the user needs to be
     * informed that something is not properly configured on the PC system.
     */
    assert(xlStatus == XL_SUCCESS);
    /* Update flag to determine if the driver should be closed at a later point. */
    if (xlStatus == XL_SUCCESS)
    {
      vectorXlDriverOpened = true;
    }
  }
} /*** end of VectorXlInit ***/


/************************************************************************************//**
** \brief     Terminates the CAN interface.
**
****************************************************************************************/
static void VectorXlTerminate(void)
{
  /* Close the XL driver if necessary. */
  if (vectorXlDriverOpened)
  {
    (void)xlCloseDriver();
    vectorXlDriverOpened = false;
  }

  /* Release memory that was allocated for storing the device name. */
  if (vectorXlSettings.devicename != NULL)
  {
    free((char *)vectorXlSettings.devicename);
  }
  /* Reset CAN interface settings. */
  vectorXlSettings.devicename = "";
  vectorXlSettings.channel = 0;
  vectorXlSettings.baudrate = CAN_BR500K;
  vectorXlSettings.code = 0x00000000u;
  vectorXlSettings.mask = 0x00000000u;
  /* Release memory that was allocated for CAN events and reset the entry count. */
  if ( (vectorXlEventsList != NULL) && (vectorXlEventsEntries != 0) )
  {
    free(vectorXlEventsList);
    vectorXlEventsEntries = 0;
  }
  /* This module used critical sections so terminate them. */
  UtilCriticalSectionTerminate();
} /*** end of VectorXlTerminate ***/


/************************************************************************************//**
** \brief     Connects the CAN interface.
** \return    True if connected, false otherwise.
**
****************************************************************************************/
static bool VectorXlConnect(void)
{
  bool           result = true;
  XLstatus       xlStatus;
  XLdriverConfig xlDrvConfig;
  char           xlAppName[XL_MAX_LENGTH + 1] = "";
  XLaccess       xlPermissionMask = 0;
  uint32_t       xlBitrate;

  /* Invalidate handles. */
  vectorXlTerminateEvent = NULL;
  vectorXlCanEvent = NULL;
  vectorXlRxThreadHandle = NULL;
  /* Reset the bus error detected flag. */
  vectorXlBusErrorDetected = false;

  /* Obtain the hardware configuration. */
  xlStatus = xlGetDriverConfig(&xlDrvConfig);
  /* Evaluate the result. */
  if (xlStatus != XL_SUCCESS)
  {
    result = false;
  }

  /* Check if the requested channel is actually available. */
  if (result)
  {
    if (vectorXlSettings.channel >= xlDrvConfig.channelCount)
    {
      result = false;
    }
    else
    {
      /* Does the requested channel actually support CAN? */
      if ((xlDrvConfig.channel[vectorXlSettings.channel].channelBusCapabilities &
          XL_BUS_ACTIVE_CAP_CAN) == 0)
      {
        result = false;
      }
    }
  }

  /* Open the port. */
  if (result)
  {
    /* Invalidate the port handle and reset the port opened flag. */
    vectorXlPortHandle = XL_INVALID_PORTHANDLE;
    vectorXlPortOpened = false;
    /* Initialize the access and permission masks. */
    vectorXLChannelMask = xlDrvConfig.channel[vectorXlSettings.channel].channelMask;
    xlPermissionMask = vectorXLChannelMask;
    /* Attempt to open the port. */
    xlStatus = xlOpenPort(&vectorXlPortHandle, xlAppName, vectorXLChannelMask, 
                          &xlPermissionMask, VECTOR_XL_RX_QUEUE_SIZE, 
                          XL_INTERFACE_VERSION, XL_BUS_TYPE_CAN);
    /* Evaluate the result. */
    if ((xlStatus != XL_SUCCESS) || (vectorXlPortHandle == XL_INVALID_PORTHANDLE))
    {
      result = false;
    }
    else
    {
      /* Update flag to determine if the port should be closed at a later point. */
      vectorXlPortOpened = true;
    }
  }

  /* Configure the baudrate if initialization access is available. Otherwise skip this
   * part, assuming that whomever does have this access already configured the correct
   * baudrate.
   */
  if (result)
  {
    if (vectorXLChannelMask == xlPermissionMask)
    {
      /* Determine the requested bitrate in bits/second. */
      xlBitrate = VectorXlConvertToRawBitrate(vectorXlSettings.baudrate);
      /* Attempt to configure the communication speed. */
      xlStatus = xlCanSetChannelBitrate(vectorXlPortHandle, vectorXLChannelMask,
                                        xlBitrate);
      /* Evaluate the result. */
      if (xlStatus != XL_SUCCESS)
      {
        result = false;
      }
    }
  }

  /* Configure normal output mode. */
  if (result)
  {
    xlStatus = xlCanSetChannelOutput(vectorXlPortHandle, vectorXLChannelMask, 
                                     XL_OUTPUT_MODE_NORMAL);
    /* Evaluate the result. */
    if (xlStatus != XL_SUCCESS)
    {
      result = false;
    }
  }

  /* Suppress error frame reception and allow chipstate info events. */
  if (result)
  {
    xlStatus = xlCanSetReceiveMode(vectorXlPortHandle, 1, 0);
    /* Evaluate the result. */
    if (xlStatus != XL_SUCCESS)
    {
      result = false;
    }
  }

  /* Create the terminate event handle used in the reception thread. */
  if (result)
  {
    vectorXlTerminateEvent = CreateEvent(NULL, TRUE, FALSE, "");
    if (vectorXlTerminateEvent == NULL)
    {
      result = false;
    }
  }
    
  /* Obtain handle for notification events. */
  if (result)
  {
    xlStatus = xlSetNotification(vectorXlPortHandle, &vectorXlCanEvent, 1);
    /* Evaluate the result. */
    if (xlStatus != XL_SUCCESS)
    {
      result = false;
    }
  }

  /* Configure the acceptance mask. */
  if (result)
  {
    /* Use bit logic to determine if the filter should accept standard 11-bit and/or
      * extended 29-bit identifiers:
      *   acceptStdId = ((mask & code & CAN_MSG_EXT_ID_MASK) == 0)
      *   acceptExtId = ((mask & code & CAN_MSG_EXT_ID_MASK) != 0) ||
      *                 ((mask & CAN_MSG_EXT_ID_MASK) == 0)
      */
    bool acceptStdID = ((vectorXlSettings.mask & vectorXlSettings.code & CAN_MSG_EXT_ID_MASK) == 0);
    bool acceptExtID = ((vectorXlSettings.mask & vectorXlSettings.code & CAN_MSG_EXT_ID_MASK) != 0) ||
                       ((vectorXlSettings.mask & CAN_MSG_EXT_ID_MASK) == 0);

    /* Start by closing the acceptance filters for all 11-bit and 29-bit identifiers. */
    (void)xlCanSetChannelAcceptance(vectorXlPortHandle, vectorXLChannelMask, 
                                    0x00000fffu, 0x00000fffu, XL_CAN_STD);
    (void)xlCanSetChannelAcceptance(vectorXlPortHandle, vectorXLChannelMask, 
                                    0xffffffffu, 0xffffffffu, XL_CAN_EXT);

    /* Configure acceptance filter for standard 11-bit identifiers. */
    if (acceptStdID)
    {
      xlStatus = xlCanSetChannelAcceptance(vectorXlPortHandle, vectorXLChannelMask, 
                                           vectorXlSettings.code & 0x1fffffffu, 
                                           vectorXlSettings.mask & 0x1fffffffu, 
                                           XL_CAN_STD);
      /* Evaluate the result. */
      if (xlStatus != XL_SUCCESS)
      {
        result = false;
      }
    }
    /* Configure acceptance filter for extended 29-bit identifiers. */
    if ((acceptExtID) && (result))
    {
      xlStatus = xlCanSetChannelAcceptance(vectorXlPortHandle, vectorXLChannelMask, 
                                           vectorXlSettings.code & 0x1fffffffu, 
                                           vectorXlSettings.mask & 0x1fffffffu, 
                                           XL_CAN_EXT);
      /* Evaluate the result. */
      if (xlStatus != XL_SUCCESS)
      {
        result = false;
      }
    }
  }

  /* Activate the channel. */
  if (result)
  {
    /* Reset the channel activated flag. */
    vectorXlChannelActivated = false;
    /* Attempt to activate the channel. */
    xlStatus = xlActivateChannel(vectorXlPortHandle, vectorXLChannelMask, 
                                 XL_BUS_TYPE_CAN, XL_ACTIVATE_RESET_CLOCK);
    /* Evaluate the result. */
    if (xlStatus != XL_SUCCESS)
    {
      result = false;
    }
    else
    {
      /* Update the channel activated flag. */
      vectorXlChannelActivated = true;
    }
  }

  /* Start the reception thread. */
  if (result)
  {
    vectorXlRxThreadHandle = CreateThread(NULL, 0, VectorXlReceptionThread,
                                          NULL, 0, NULL);
    if (vectorXlRxThreadHandle == NULL)
    {
      result = false;
    }
  }

  /* Cleanup in case this function was not successful. */
  if (!result)
  {
    /* Deactivate the channel if it was activated. */
    if (vectorXlChannelActivated)
    {
      (void)xlDeactivateChannel(vectorXlPortHandle, vectorXLChannelMask);
      vectorXlChannelActivated = false;
    }
    /* Invalidate the notification event if it was set. */
    if (vectorXlCanEvent != NULL)
    {
      vectorXlCanEvent = NULL;
    }
    /* Close the termination event handle if it was created. */
    if (vectorXlTerminateEvent != NULL)
    {
      /* Close the event handle. */
      (void)CloseHandle(vectorXlTerminateEvent);
      vectorXlTerminateEvent = NULL;
    }
    /* Close the port if it was opened. */
    if (vectorXlPortOpened)
    {
      (void)xlClosePort(vectorXlPortHandle);
      vectorXlPortOpened = false;
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of VectorXlConnect ***/


/************************************************************************************//**
** \brief     Disconnects the CAN interface.
**
****************************************************************************************/
static void VectorXlDisconnect(void)
{
   /* Stop the reception thread. */
  if (vectorXlRxThreadHandle != NULL)
  {
    /* Trigger event to request the reception thread to stop. */
    (void)SetEvent(vectorXlTerminateEvent);
    /* Wait for the thread to signal termination. */
    (void)WaitForSingleObject(vectorXlRxThreadHandle, INFINITE);
    /* Close the thread handle. */
    (void)CloseHandle(vectorXlRxThreadHandle);
    vectorXlRxThreadHandle = NULL;
  }

  /* Close the termination event handle if it was created. */
  if (vectorXlTerminateEvent != NULL)
  {
    /* Close the event handle. */
    (void)CloseHandle(vectorXlTerminateEvent);
    vectorXlTerminateEvent = NULL;
  }
  /* Invalidate the notification event if it was set. */
  if (vectorXlCanEvent != NULL)
  {
    vectorXlCanEvent = NULL;
  }
  /* Deactivate the channel if necessary. */
  if (vectorXlChannelActivated)
  {
    (void)xlDeactivateChannel(vectorXlPortHandle, vectorXLChannelMask);
    vectorXlChannelActivated = false;
  }

  /* Close the XL port if necessary. */
  if (vectorXlPortOpened)
  {
    (void)xlClosePort(vectorXlPortHandle);
    vectorXlPortOpened = false;
  }
} /*** end of VectorXlDisconnect ***/


/************************************************************************************//**
** \brief     Submits a message for transmission on the CAN bus.
** \param     msg Pointer to CAN message structure.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool VectorXlTransmit(tCanMsg const * msg)
{
  bool               result = false;
  tCanEvents const * pEvents;
  XLstatus           xlStatus = XL_ERROR;
  XLevent            xlEvent[1];
  unsigned int       txMsgCount = 1;
  uint8_t            byteIdx;

  /* Check parameters. */
  assert(msg != NULL);

  /* Only continue with valid parameters. */
  if (msg != NULL) /*lint !e774 */
  {
    /* Convert the CAN message to the XLevent format. */
    memset(xlEvent, 0, sizeof(xlEvent));
    xlEvent[0].tag = XL_TRANSMIT_MSG;
    xlEvent[0].tagData.msg.flags = 0;
    if ((msg->id & CAN_MSG_EXT_ID_MASK) == 0)
    {
      xlEvent[0].tagData.msg.id = msg->id & 0x7ffu;
    }
    else
    {
      xlEvent[0].tagData.msg.id = msg->id & 0x1fffffffu;
      xlEvent[0].tagData.msg.id |= XL_CAN_EXT_MSG_ID;
    }
    xlEvent[0].tagData.msg.dlc = msg->dlc;
    if (xlEvent[0].tagData.msg.dlc > CAN_MSG_MAX_LEN)
    {
      xlEvent[0].tagData.msg.dlc = CAN_MSG_MAX_LEN;
    }
    for (byteIdx = 0; byteIdx < msg->dlc; byteIdx++)
    {
      xlEvent[0].tagData.msg.data[byteIdx] = msg->data[byteIdx];
    }

    /* Attempt to submit the message for transmission. */
    xlStatus = xlCanTransmit(vectorXlPortHandle, vectorXLChannelMask, &txMsgCount, 
                             xlEvent);
    /* Check the result. */
    if (xlStatus == XL_SUCCESS)
    {
      /* Update result value to success. */
      result = true;
      /* Trigger transmit complete event(s). */
      pEvents = vectorXlEventsList;
      for (uint32_t idx = 0; idx < vectorXlEventsEntries; idx++)
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
} /*** end of VectorXlTransmit ***/


/************************************************************************************//**
** \brief     Checks if a bus off or bus heavy situation occurred.
** \return    True if a bus error situation was detected, false otherwise.
**
****************************************************************************************/
static bool VectorXlIsBusError(void)
{
  bool result = false;

  /* To obtain the bus off and error passive state info, a chip state message must
   * be requested, which is then received and processed by the reception thread.
   */
  (void)xlCanRequestChipState(vectorXlPortHandle, vectorXLChannelMask);

  /* Enter critical section. */
  UtilCriticalSectionEnter();
  
  /* Was a bus error chip state detected by the reception thread? */
  if (vectorXlBusErrorDetected)
  {
    /* Reset the flag to be able to detect the next bus error event. */
    vectorXlBusErrorDetected = false;
    /* Update the result. */
    result = true;
  }
  
  /* Exit critical section. */
  UtilCriticalSectionExit();

  /* Give the result back to the caller. */
  return result;
} /*** end of VectorXlIsBusError ***/


/************************************************************************************//**
** \brief     Registers the event callback functions that should be called by the CAN
**            interface.
** \param     events Pointer to structure with event callback function pointers.
**
****************************************************************************************/
static void VectorXlRegisterEvents(tCanEvents const * events)
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
    vectorXlEventsList = realloc(vectorXlEventsList,
                                 (sizeof(tCanEvents) * (vectorXlEventsEntries + 1)));
    /* Assert reallocation. */
    assert(vectorXlEventsList != NULL);
    /* Only continue if reallocation was successful. */
    if (vectorXlEventsList != NULL)
    {
      /* Increment events entry count. */
      vectorXlEventsEntries++;
      /* Store the events in the new entry. */
      vectorXlEventsList[vectorXlEventsEntries - 1] = *events;
    }
    /* Reallocation failed. */
    else
    {
      /* Reset events entry count. */
      vectorXlEventsEntries = 0;
    }
  }
} /*** end of VectorXlRegisterEvents ***/


/************************************************************************************//**
** \brief     Converts the baudrate enumerated type value to a bitrate in bits/second.
** \param     baudrate Baudrate enumarated type.
** \return    Bitrate in bits/second.
**
****************************************************************************************/
static uint32_t VectorXlConvertToRawBitrate(tCanBaudrate baudrate)
{
  uint32_t result = 500000;

  switch (baudrate)
  {
    case CAN_BR10K:
      result = 10000;
      break;
    case CAN_BR20K:
      result = 20000;
      break;
    case CAN_BR50K:
      result = 50000;
      break;
    case CAN_BR100K:
      result = 100000;
      break;
    case CAN_BR125K:
      result = 125000;
      break;
    case CAN_BR250K:
      result = 250000;
      break;
    case CAN_BR500K:
      result = 500000;
      break;
    case CAN_BR800K:
      result = 800000;
      break;
    case CAN_BR1M:
      result = 1000000;
      break;
    default:
      /* Default to a common used baudrate as a fallback. */
      result = 500000;
      break;
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of VectorXlConvertToRawBitrate ***/


/************************************************************************************//**
** \brief     CAN event reception thread.
** \param     pv Pointer to thread parameters.
** \return    Thread exit code.
**
****************************************************************************************/
static DWORD WINAPI VectorXlReceptionThread(LPVOID pv)
{
  DWORD waitResult;
  HANDLE handles[] = 
  { 
    vectorXlCanEvent, 
    vectorXlTerminateEvent 
  };
  bool running = true;
  XLstatus xlStatus;
  unsigned int xlEventCount;
  XLevent xlEvent;
  tCanMsg rxMsg;
  uint8_t byteIdx;
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
        /* Empty out the event queue. */
        xlStatus = XL_SUCCESS;
        while (xlStatus == XL_SUCCESS)
        {
          /* Set the number of events to request to read. */
          xlEventCount = 1;
          xlStatus = xlReceive(vectorXlPortHandle, &xlEventCount, &xlEvent);      
          if (xlStatus != XL_ERR_QUEUE_IS_EMPTY ) 
          {
            /* Was is a message reception event? */
            if (xlEvent.tag == XL_RECEIVE_MSG)
            {
              /* Only process actual newly received messages and ignore things such as
               * a confirmation of a message transmission.
               */
              if (xlEvent.tagData.msg.flags == 0)
              {
                /* Read out the message identifier. */
                if ((xlEvent.tagData.msg.id & XL_CAN_EXT_MSG_ID) == 0)
                {
                  rxMsg.id = xlEvent.tagData.msg.id & 0x7ffu;
                }
                else
                {
                  rxMsg.id = xlEvent.tagData.msg.id & 0x1fffffffu;
                  rxMsg.id |= CAN_MSG_EXT_ID_MASK;
                }
                /* Read out the message length. */
                if (xlEvent.tagData.msg.dlc > CAN_MSG_MAX_LEN)
                {
                  rxMsg.dlc = CAN_MSG_MAX_LEN;
                }
                else
                {
                  rxMsg.dlc = (uint8_t)xlEvent.tagData.msg.dlc;
                }
                /* Read out the message data. */
                for (byteIdx = 0; byteIdx < rxMsg.dlc; byteIdx++)
                {
                  rxMsg.data[byteIdx] = xlEvent.tagData.msg.data[byteIdx];
                }
                /* Trigger message reception event(s). */
                pEvents = vectorXlEventsList;
                for (uint32_t idx = 0; idx < vectorXlEventsEntries; idx++)
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
            /* Was is a chip state event? */
            else if (xlEvent.tag == XL_CHIP_STATE)
            {
              /* Is a bus off or bus have state reported? */
              if ( ((xlEvent.tagData.chipState.busStatus & XL_CHIPSTAT_BUSOFF) != 0) ||
                   ((xlEvent.tagData.chipState.busStatus & XL_CHIPSTAT_ERROR_PASSIVE) != 0))
              {
                /* Enter critical section. */
                UtilCriticalSectionEnter();
                /* Set bus error flag. Note that this flag is automatically reset by
                 * VectorXlIsBusError.
                 */
                vectorXlBusErrorDetected = true;
                /* Exit critical section. */
                UtilCriticalSectionExit();
              }
            }
          }  
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

} /*** end of VectorXlReceptionThread ***/


/*********************************** end of xldriver.c *********************************/

