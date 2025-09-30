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


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Structure type for grouping together CAN peripheral parameters needed for
 *         performing bittiming calculations.
 */
typedef struct t_vector_xl_periph_params
{
  uint32_t base_freq;                               /**< Source clock frequency [Hz]   */
  uint32_t prescaler_min;                           /**< Smallest supported prescaler  */
  uint32_t prescaler_max;                           /**< Largest supported prescaler   */
  uint16_t tseg1_min;                               /**< Smallest supported Tseg1      */
  uint16_t tseg1_max;                               /**< Largest supported Tseg1       */
  uint16_t tseg2_min;                               /**< Smallest supported Tseg2      */
  uint16_t tseg2_max;                               /**< Largest supported Tseg2       */
} tVectorXlPeriphParams;

/** \brief Structure type for grouping together CAN bittiming configuration element.   */
typedef struct t_vector_xl_bittiming_cfg
{
  uint32_t prescaler;                               /**< CAN clock prescaler           */
  uint16_t tseg1;                                   /**< Time segment 1, excl. SYNC    */
  uint16_t tseg2;                                   /**< Time segment 2                */
  uint16_t sjw;                                     /**< Synchronization jump width    */
} tVectorXlBittimingCfg;


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
/* Utility functions. */
static bool VectorXlCalculateBitTimingConfig(uint32_t baudrate, tVectorXlPeriphParams const* periph_params, tVectorXlBittimingCfg* bittiming_config);
/* CAN event reception thread. */
static DWORD WINAPI VectorXlReceptionThread(LPVOID pv);
static void VectorXlProcessCanClassicEvent(void);
static void VectorXlProcessCanFdEvent(void);


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

/** \brief Boolean flag to track if CAN classic or CAN FD should be used. */
static bool vectorXlCanFdModeRequested;

/** \brief Boolean flag to track if the CAN FD bitrate switch feature should be used. */
static bool vectorXlCanFdBrsRequested;

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
  vectorXlCanFdModeRequested = false;
  vectorXlCanFdBrsRequested = false;
  
  /* This module uses critical sections so initialize them. */
  UtilCriticalSectionInit();

  /* Reset CAN interface settings. */
  vectorXlSettings.devicename = "";
  vectorXlSettings.channel = 0;
  vectorXlSettings.baudrate = CAN_BR500K;
  vectorXlSettings.code = 0x00000000u;
  vectorXlSettings.mask = 0x00000000u;
  vectorXlSettings.brsbaudrate = CANFD_DISABLED;

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
  vectorXlSettings.brsbaudrate = CANFD_DISABLED;
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
  bool                  result = true;
  XLstatus              xlStatus;
  XLdriverConfig        xlDrvConfig;
  char                  xlAppName[XL_MAX_LENGTH + 1] = "";
  XLaccess              xlPermissionMask = 0;
  uint32_t              xlBitrate;
  uint32_t              xlInterfaceVersion = XL_INTERFACE_VERSION;
  XLcanFdConf           xlCanFdConf = {0};
  tVectorXlPeriphParams xlPeriphParams = {0};
  tVectorXlBittimingCfg xlBittimingCfg = {0};

  /* Reset CAN FD related flags. */
  vectorXlCanFdModeRequested = false;
  vectorXlCanFdBrsRequested = false;

  /* Set flags to determine if and which CAN FD features are requested. */
  if (vectorXlSettings.brsbaudrate != CANFD_DISABLED)
  {
    /* CAN FD mode requested. */
    vectorXlCanFdModeRequested = true;
    /* When the bitrate switch feature is requested, the bitrate switch baudrate value
     * should be higher than the nominal baudrate.
     */
    if (CanConvertFdBaudrate(vectorXlSettings.brsbaudrate) > CanConvertBaudrate(vectorXlSettings.baudrate))
    {
      /* CAN FD bitrate switch requested. */
      vectorXlCanFdBrsRequested = true;
    }
  }

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

  /* Check if the requested CAN FD features are actually supported by the hardware. */
  if (result)
  {
    /* CAN FD mode requested? */
    if (vectorXlCanFdModeRequested)
    {
      /* Use API version 4 for CAN FD. */
      xlInterfaceVersion = XL_INTERFACE_VERSION_V4;
      /* Does the requested channel actually support CAN FD? */
      if ((xlDrvConfig.channel[vectorXlSettings.channel].channelCapabilities &
        XL_CHANNEL_FLAG_CANFD_ISO_SUPPORT) == 0)
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
                          xlInterfaceVersion, XL_BUS_TYPE_CAN);
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
      /* CAN classic mode? */
      if (!vectorXlCanFdModeRequested)
      {
        /* Determine the requested bitrate in bits/second. */
        xlBitrate = CanConvertBaudrate(vectorXlSettings.baudrate);
        /* Attempt to configure the communication speed. */
        xlStatus = xlCanSetChannelBitrate(vectorXlPortHandle, vectorXLChannelMask,
                                          xlBitrate);
        /* Evaluate the result. */
        if (xlStatus != XL_SUCCESS)
        {
          result = false;
        }
      }
      /* CAN FD mode. */
      else
      {
        /* Initialize the parameters for the nominal (arbitration) bittiming settings.
         * Note that xlGetDriverConfig() does not give the CAN clock frequency. The
         * XL drive documentation states that software shall assume an 80 MHz CAN clock,
         * although the hardware may internally use a different clock.
         */
        xlPeriphParams.base_freq = 80000000U;
        xlPeriphParams.prescaler_min = 1U;
        xlPeriphParams.prescaler_max = 254U; // Assumed.
        xlPeriphParams.tseg1_min = 1U;
        xlPeriphParams.tseg1_max = 254U;
        xlPeriphParams.tseg2_min = 1U;
        xlPeriphParams.tseg2_max = 254U;
        /* Determine the requested nominal (arbitration) bitrate in bits/second. */
        xlBitrate = CanConvertBaudrate(vectorXlSettings.baudrate);
        /* Attempt to calculate the nominal (arbitration) bittiming settings. */
        if (!VectorXlCalculateBitTimingConfig(xlBitrate, &xlPeriphParams, &xlBittimingCfg))
        {
          result = false;
        }
        else
        {
          /* Store the nominal (arbitration) bittiming settings. */
          xlCanFdConf.arbitrationBitRate = xlBitrate;
          xlCanFdConf.tseg1Abr = xlBittimingCfg.tseg1;
          xlCanFdConf.tseg2Abr = xlBittimingCfg.tseg2;
          xlCanFdConf.sjwAbr = (xlBittimingCfg.sjw > 128U) ? 128U : xlBittimingCfg.sjw;
        }

        /* Initialize the parameters for the data bittiming settings.
         * Note that xlGetDriverConfig() does not give the CAN clock frequency. The
         * XL drive documentation states that software shall assume an 80 MHz CAN clock,
         * although the hardware may internally use a different clock.
         */
        xlPeriphParams.base_freq = 80000000U;
        xlPeriphParams.prescaler_min = 1U;
        xlPeriphParams.prescaler_max = 254U; // Assumed.
        xlPeriphParams.tseg1_min = 1U;
        xlPeriphParams.tseg1_max = 126U;
        xlPeriphParams.tseg2_min = 1U;
        xlPeriphParams.tseg2_max = 126U;
        /* Determine the requested data bitrate in bits/second. */
        xlBitrate = CanConvertFdBaudrate(vectorXlSettings.brsbaudrate);
        /* Attempt to calculate the data bittiming settings. */
        if (!VectorXlCalculateBitTimingConfig(xlBitrate, &xlPeriphParams, &xlBittimingCfg))
        {
          result = false;
        }
        else
        {
          /* Store the data bittiming settings. */
          xlCanFdConf.dataBitRate = xlBitrate;
          xlCanFdConf.tseg1Dbr = xlBittimingCfg.tseg1;
          xlCanFdConf.tseg2Dbr = xlBittimingCfg.tseg2;
          xlCanFdConf.sjwDbr = (xlBittimingCfg.sjw > 64) ? 64U : xlBittimingCfg.sjw;
        }

        /* Bittiming calculation successful? */
        if (result)
        {
          xlStatus = xlCanFdSetConfiguration(vectorXlPortHandle, vectorXLChannelMask,
                                             &xlCanFdConf);
          /* Evaluate the result. */
          if (xlStatus != XL_SUCCESS)
          {
            result = false;
          }
        }
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
  unsigned int       txMsgCount = 1;
  uint8_t            byteIdx;

  /* Check parameters. */
  assert(msg != NULL);

  /* Only continue with valid parameters. */
  if (msg != NULL) /*lint !e774 */
  {
    /* CAN classic mode? */
    if (!vectorXlCanFdModeRequested)
    {
      XLevent xlEvent[1];

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
      xlEvent[0].tagData.msg.dlc = msg->len;
      if (xlEvent[0].tagData.msg.dlc > CAN_MSG_MAX_LEN)
      {
        xlEvent[0].tagData.msg.dlc = CAN_MSG_MAX_LEN;
      }
      for (byteIdx = 0; byteIdx < msg->len; byteIdx++)
      {
        xlEvent[0].tagData.msg.data[byteIdx] = msg->data[byteIdx];
      }

      /* Attempt to submit the message for transmission. */
      xlStatus = xlCanTransmit(vectorXlPortHandle, vectorXLChannelMask, &txMsgCount,
                               xlEvent);
    }
    /* CAN FD mode */
    else
    {
      XLcanTxEvent         xlcanTxEvent[1];
      uint8_t              dataLen;
      static const uint8_t len2dlc[] = { 0,  1,  2,  3,  4,  5,  6,  7,  8, /*  0 -  8 */
                                         9,  9,  9,  9,                     /*  9 - 12 */
                                         10, 10, 10, 10,                    /* 13 - 16 */
                                         11, 11, 11, 11,                    /* 17 - 20 */
                                         12, 12, 12, 12,                    /* 21 - 24 */
                                         13, 13, 13, 13, 13, 13, 13, 13,    /* 25 - 32 */
                                         14, 14, 14, 14, 14, 14, 14, 14,    /* 33 - 40 */
                                         14, 14, 14, 14, 14, 14, 14, 14,    /* 41 - 48 */
                                         15, 15, 15, 15, 15, 15, 15, 15,    /* 49 - 56 */
                                         15, 15, 15, 15, 15, 15, 15, 15 };  /* 57 - 64 */


      /* Convert the CAN message to the XLevent format. */
      memset(xlcanTxEvent, 0, sizeof(xlcanTxEvent));
      xlcanTxEvent[0].tag = XL_CAN_EV_TAG_TX_MSG;
      xlcanTxEvent[0].tagData.canMsg.msgFlags = XL_CAN_TXMSG_FLAG_EDL;
      if (vectorXlCanFdBrsRequested)
      {
        xlcanTxEvent[0].tagData.canMsg.msgFlags |= XL_CAN_TXMSG_FLAG_BRS;
      }
      /* Set the message identifier. */
      if ((msg->id & CAN_MSG_EXT_ID_MASK) == 0)
      {
        xlcanTxEvent[0].tagData.canMsg.canId = msg->id & 0x7ffu;
      }
      else
      {
        xlcanTxEvent[0].tagData.canMsg.canId = msg->id & 0x1fffffffu;
        xlcanTxEvent[0].tagData.canMsg.canId |= XL_CAN_EXT_MSG_ID;
      }
      /* Determine data length with out-of-bounds correction. */
      dataLen = ((msg->len <= XL_CAN_MAX_DATA_LEN) ? msg->len : XL_CAN_MAX_DATA_LEN);
      xlcanTxEvent[0].tagData.canMsg.dlc = len2dlc[dataLen];
      for (byteIdx = 0; byteIdx < dataLen; byteIdx++)
      {
        xlcanTxEvent[0].tagData.canMsg.data[byteIdx] = msg->data[byteIdx];
      }
      /* Attempt to submit the message for transmission. */
      xlStatus = xlCanTransmitEx(vectorXlPortHandle, vectorXLChannelMask, 1,
                                 &txMsgCount, xlcanTxEvent);
    }

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
** \brief     Search algorithm to match the desired baudrate to a possible bit
**            timing configuration, taking into account the given CAN peripheral
**            parameters.
** \param     baudrate Desired CAN communication speed in bits/sec.
** \param     periph_params CAN peripheral parameters.
** \param     bittiming_config Found bit timing configuration.
** \return    True if a matching CAN bittiming configuration was found, false otherwise.
**
****************************************************************************************/
static bool VectorXlCalculateBitTimingConfig(uint32_t baudrate,
                                             tVectorXlPeriphParams const * periph_params,
                                             tVectorXlBittimingCfg * bittiming_config)
{
  bool result = false;

  /* Verify parameters. */
  assert((baudrate > 0) && (periph_params != NULL) && (bittiming_config != NULL));

  /* Only continue with valid parameters. */
  if ((baudrate > 0) && (periph_params != NULL) && (bittiming_config != NULL))
  {
    uint16_t bitTq, bitTqMin, bitTqMax;
    uint16_t tseg1, tseg2;
    uint32_t prescaler;
    uint8_t  samplePoint;

    /* Calculate minimum and maximum possible time quanta per bit. Remember that the
     * bittime in time quanta is 1 (sync) + tseg1 + tseg2.
     */
    bitTqMin = 1U + periph_params->tseg1_min + periph_params->tseg2_min;
    bitTqMax = 1U + periph_params->tseg1_max + periph_params->tseg2_max;

    /* Loop through all the prescaler values from low to high to find one that results
     * in a time quanta per bit that is in the range bitTqMin.. bitTqMax. Note that
     * looping through the prescalers low to high is important, because a lower prescaler
     * would result in a large number of time quanta per bit. This in turns gives you
     * more flexibility for setting the a bit's sample point.
     */
    for (prescaler = periph_params->prescaler_min;
      prescaler <= periph_params->prescaler_max; prescaler++)
    {
      /* No need to continue if the configured peripheral clock, scaled down by this
       * prescaler value, is no longer high enough to get to the desired baudrate,
       * taking into account the minimum possible time quanta per bit.
       */
      if (periph_params->base_freq < (baudrate * bitTqMin))
      {
        break;
      }
      /* Does this prescaler give a fixed (integer) number of time quanta? */
      if (((periph_params->base_freq % prescaler) == 0U) &&
        (((periph_params->base_freq / prescaler) % baudrate) == 0U))
      {
        /* Calculate how many time quanta per bit this prescaler would give. */
        bitTq = (uint16_t)((periph_params->base_freq / prescaler) / baudrate);
        /* Is this a configurable amount of time quanta? */
        if ((bitTq >= bitTqMin) && (bitTq <= bitTqMax))
        {
          /* If the sample point is at the very end of the bit time, the maximum
           * possible network length can be achieved. an earlier sample point reduces
           * the achievable network length, but increases robustness. As a reference,
           * a value of higher than 80% is not recommended for automotive applications
           * due to robustness reasons.
           *
           * For this reason try to get a sample point that is in the 65% - 80% range.
           * An efficient way of doing this is to calculate Tseg2, which should then
           * be 20% and round up by doing this calculation.
           *
           * Example:
           *   baud      = 500 kbits/sec
           *   base_freq = 24 MHz
           *   prescaler = 1
           *   bitTq     = 48
           *
           *   For 80% sample point, Tseg2 should be 20% of 48 = 9.6 time quanta.
           *   Rounded up to the next integer = 10. Resulting sample point:
           *   ((48 - 10) / 48) * 100 = 79.17%
           */
          tseg2 = (uint16_t)(((bitTq * 2U) + 9U) / 10U);
          /* Calculate Tseg1 by deducting Tseg2 and 1 time quanta for the sync seq. */
          tseg1 = (uint16_t)((bitTq - tseg2) - 1U);
          /* Are these values within configurable range? */
          if ((tseg1 >= periph_params->tseg1_min) &&
            (tseg1 <= periph_params->tseg1_max) &&
            (tseg2 >= periph_params->tseg2_min) &&
            (tseg2 <= periph_params->tseg2_max))
          {
            /* Calculate the actual sample point, given these Tseg values. */
            samplePoint = (uint8_t)(((1U + tseg1) * 100UL) / bitTq);
            /* Is this within the targeted 65% - 80% range? */
            if ((samplePoint >= 65U) && (samplePoint <= 80U))
            {
              /* Store these bittiming settings. */
              bittiming_config->prescaler = prescaler;
              bittiming_config->tseg1 = tseg1;
              bittiming_config->tseg2 = tseg2;
              /* SJW depends highly on the baudrate tolerances of the other nodes on the
               * network. SJW 1 allows for only a small window of tolerance between node
               * baudrate. SJW = TSEG2 allows for a large winow, at the risk of a bit
               * being incorrectly sampled. A safe approach to to use TSEG2/2 but also
               * make sure SJW is > 0.
               */
              bittiming_config->sjw = tseg2 / 2U;
              if (bittiming_config->sjw == 0U)
              {
                bittiming_config->sjw = 1U;
              }
              /* Set the result to success. */
              result = true;
              /* All done so no need to continue the loop. */
              break;
            }
          }
        }
      }
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of VectorXlCalculateBitTimingConfig ***/

  
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
        /* CAN classic mode? */
        if (!vectorXlCanFdModeRequested)
        {
          VectorXlProcessCanClassicEvent();
        }
        /* CAN FD mode */
        else
        {
          VectorXlProcessCanFdEvent();
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


/************************************************************************************//**
** \brief     CAN classic event handler. Should be called when a CAN classic related
**            notication event triggered.
**
****************************************************************************************/
static void VectorXlProcessCanClassicEvent(void)
{
  XLstatus           xlStatus;
  unsigned int       xlEventCount;
  XLevent            xlEvent;
  tCanMsg            rxMsg;
  uint8_t            byteIdx;
  tCanEvents const * pEvents;

  /* Empty out the event queue. */
  xlStatus = XL_SUCCESS;
  while (xlStatus == XL_SUCCESS)
  {
    /* Set the number of events to request to read. */
    xlEventCount = 1;
    xlStatus = xlReceive(vectorXlPortHandle, &xlEventCount, &xlEvent);
    if (xlStatus != XL_ERR_QUEUE_IS_EMPTY)
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
            rxMsg.len = CAN_MSG_MAX_LEN;
          }
          else
          {
            rxMsg.len = (uint8_t)xlEvent.tagData.msg.dlc;
          }
          /* Read out the message data. */
          for (byteIdx = 0; byteIdx < rxMsg.len; byteIdx++)
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
        if (((xlEvent.tagData.chipState.busStatus & XL_CHIPSTAT_BUSOFF) != 0) ||
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
} /*** end of VectorXlProcessCanClassicEvent ***/


/************************************************************************************//**
** \brief     CAN FD event handler. Should be called when a CAN FD related notication
**            event triggered.
**
****************************************************************************************/
static void VectorXlProcessCanFdEvent(void)
{
  XLstatus               xlStatus;
  XLcanRxEvent           xlCanRxEvent;
  tCanMsg                rxMsg;
  uint8_t                byteIdx;
  tCanEvents     const * pEvents;
  static uint8_t const   dlc2len[] = { 0,  1,  2,  3,  4,  5,  6,  7,
                                       8, 12, 16, 20, 24, 32, 48, 64 };

  /* Empty out the event queue. */
  xlStatus = XL_SUCCESS;
  while (xlStatus == XL_SUCCESS)
  {
    /* Set the number of events to request to read. */
    xlStatus = xlCanReceive(vectorXlPortHandle, &xlCanRxEvent);
    if (xlStatus != XL_ERR_QUEUE_IS_EMPTY)
    {
      /* Was is a message reception event? */
      if (xlCanRxEvent.tag == XL_CAN_EV_TAG_RX_OK)
      {
        /* A newly received CAN FD message should have the extended data length (EDL)
         * flag set.
         */
        if ((xlCanRxEvent.tagData.canRxOkMsg.msgFlags & XL_CAN_RXMSG_FLAG_EDL) != 0)
        {
          /* Read out the message identifier. */
          if ((xlCanRxEvent.tagData.canRxOkMsg.canId & XL_CAN_EXT_MSG_ID) == 0)
          {
            rxMsg.id = xlCanRxEvent.tagData.canRxOkMsg.canId & 0x7ffu;
          }
          else
          {
            rxMsg.id = xlCanRxEvent.tagData.canRxOkMsg.canId & 0x1fffffffu;
            rxMsg.id |= CAN_MSG_EXT_ID_MASK;
          }
          /* Read out the message length. */
          if (xlCanRxEvent.tagData.canRxOkMsg.dlc >= ( sizeof(dlc2len)/sizeof(dlc2len[0])))
          {
            rxMsg.len = CAN_MSG_MAX_LEN;
          }
          else
          {
            rxMsg.len = dlc2len[xlCanRxEvent.tagData.canRxOkMsg.dlc];
          }
          /* Read out the message data. */
          for (byteIdx = 0; byteIdx < rxMsg.len; byteIdx++)
          {
            rxMsg.data[byteIdx] = xlCanRxEvent.tagData.canRxOkMsg.data[byteIdx];
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
      else if (xlCanRxEvent.tag == XL_CAN_EV_TAG_CHIP_STATE)
      {
        /* Is a bus off or bus have state reported? */
        if (((xlCanRxEvent.tagData.canChipState.busStatus & XL_CHIPSTAT_BUSOFF) != 0) ||
          ((xlCanRxEvent.tagData.canChipState.busStatus & XL_CHIPSTAT_ERROR_PASSIVE) != 0))
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
} /*** end of VectorXlProcessCanFdEvent ***/


/*********************************** end of xldriver.c *********************************/

