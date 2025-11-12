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
#include <stdio.h>                          /* for standard I/O                        */
#include <stdlib.h>                         /* for standard library                    */
#include <string.h>                         /* for string library                      */
#include "candriver.h"                      /* Generic CAN driver module               */
#include "pcanusb.h"                        /* Peak PCAN-USB interface                 */
#include <windows.h>                        /* for Windows API                         */
#include "PCANBasic.h"                      /* for PCAN-Basic API                      */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Configurable to enabled/disable the automatic CAN bus off recovery feature.
 *         Testing shows that it is better to leave this disabled. If no connection
 *         with the target can be made, the PCAN-USB automatically re-initialized anyway.
 */
#define PCANUSB_BUSOFF_AUTORECOVERY_ENABLE       (0u)


/***************************************************************************************
* Type definitions
****************************************************************************************/
/* Type definitions of the functions in the PCAN-Basic API that this CAN interface uses.*/
typedef TPCANStatus (__stdcall * tPCanUsbLibFuncInitialize)(TPCANHandle, TPCANBaudrate, TPCANType, DWORD, WORD);
typedef TPCANStatus (__stdcall * tPCanUsbLibFuncInitializeFd)(TPCANHandle, TPCANBitrateFD);
typedef TPCANStatus (__stdcall * tPCanUsbLibFuncUninitialize)(TPCANHandle);
typedef TPCANStatus (__stdcall * tPCanUsbLibFuncGetStatus)(TPCANHandle); 
typedef TPCANStatus (__stdcall * tPCanUsbLibFuncGetValue)(TPCANHandle, TPCANParameter, void*, DWORD);
typedef TPCANStatus (__stdcall * tPCanUsbLibFuncSetValue)(TPCANHandle, TPCANParameter, void*, DWORD); 
typedef TPCANStatus (__stdcall * tPCanUsbLibFuncRead)(TPCANHandle, TPCANMsg*, TPCANTimestamp*); 
typedef TPCANStatus (__stdcall * tPCanUsbLibFuncReadFd)(TPCANHandle, TPCANMsgFD*, TPCANTimestampFD*);
typedef TPCANStatus (__stdcall * tPCanUsbLibFuncWrite)(TPCANHandle, TPCANMsg*);
typedef TPCANStatus (__stdcall * tPCanUsbLibFuncWriteFd)(TPCANHandle, TPCANMsgFD*);
typedef TPCANStatus (__stdcall * tPCanUsbLibFuncFilterMessages)(TPCANHandle, DWORD, DWORD, TPCANMode);


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Structure type for grouping together CAN peripheral parameters needed for
 *         performing bittiming calculations.
 */
typedef struct t_pcan_usb_periph_params
{
  uint32_t base_freq;                               /**< Source clock frequency [Hz]   */
  uint32_t prescaler_min;                           /**< Smallest supported prescaler  */
  uint32_t prescaler_max;                           /**< Largest supported prescaler   */
  uint16_t tseg1_min;                               /**< Smallest supported Tseg1      */
  uint16_t tseg1_max;                               /**< Largest supported Tseg1       */
  uint16_t tseg2_min;                               /**< Smallest supported Tseg2      */
  uint16_t tseg2_max;                               /**< Largest supported Tseg2       */
  uint16_t sjw_min;                                 /**< Smallest supported SJW        */
  uint16_t sjw_max;                                 /**< Largest supported SJW         */
} tPCanUsbPeriphParams;

/** \brief Structure type for grouping together CAN bittiming configuration element.   */
typedef struct t_pcan_usb_bittiming_cfg
{
  uint32_t prescaler;                               /**< CAN clock prescaler           */
  uint16_t tseg1;                                   /**< Time segment 1, excl. SYNC    */
  uint16_t tseg2;                                   /**< Time segment 2                */
  uint16_t sjw;                                     /**< Synchronization jump width    */
} tPCanUsbBittimingCfg;


/***************************************************************************************
* Function prototypes
****************************************************************************************/
/* CAN interface functions. */
static void PCanUsbInit(tCanSettings const * settings);
static void PCanUsbTerminate(void);
static bool PCanUsbConnect(void);
static void PCanUsbDisconnect(void);
static bool PCanUsbTransmit(tCanMsg const * msg);
static bool PCanUsbIsBusError(void);
static void PCanUsbRegisterEvents(tCanEvents const * events);
/* Utility functions. */
static bool PCanUsbCalculateBitTimingConfig(uint32_t baudrate,
                                            tPCanUsbPeriphParams const * periph_params,
                                            tPCanUsbBittimingCfg * bittiming_config);
/* CAN message reception thread. */
static DWORD WINAPI PCanUsbReceptionThread(LPVOID pv);
static void PCanUsbProcessCanClassicEvent(void);
static void PCanUsbProcessCanFdEvent(void);
/* PCAN-Basic library handling. */
static void PCanUsbLibLoadDll(void);
static void PCanUsbLibUnloadDll(void);
static TPCANStatus PCanUsbLibFuncInitialize(TPCANHandle Channel, TPCANBaudrate Btr0Btr1, 
                                            TPCANType HwType, DWORD IOPort, 
                                            WORD Interrupt);
static TPCANStatus PCanUsbLibFuncInitializeFd(TPCANHandle Channel,
                                              TPCANBitrateFD BitrateFD);
static TPCANStatus PCanUsbLibFuncUninitialize(TPCANHandle Channel);
static TPCANStatus PCanUsbLibFuncGetStatus(TPCANHandle Channel);
static TPCANStatus PCanUsbLibFuncGetValue(TPCANHandle Channel, TPCANParameter Parameter,
                                          void* Buffer, DWORD BufferLength);
static TPCANStatus PCanUsbLibFuncSetValue(TPCANHandle Channel, TPCANParameter Parameter,
                                          void * Buffer, DWORD BufferLength);
static TPCANStatus PCanUsbLibFuncRead(TPCANHandle Channel, TPCANMsg * MessageBuffer, 
                                      TPCANTimestamp * TimestampBuffer);
static TPCANStatus PCanUsbLibFuncReadFd(TPCANHandle Channel, TPCANMsgFD* MessageBuffer,
                                        TPCANTimestampFD* TimestampBuffer);
static TPCANStatus PCanUsbLibFuncWrite(TPCANHandle Channel, TPCANMsg * MessageBuffer);
static TPCANStatus PCanUsbLibFuncWriteFd(TPCANHandle Channel, TPCANMsgFD* MessageBuffer);
static TPCANStatus PCanUsbLibFuncFilterMessages(TPCANHandle Channel, DWORD FromID, 
                                                DWORD ToID, TPCANMode Mode);


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
  PCanUsbIsBusError,
  PCanUsbRegisterEvents
};

/** \brief PCAN-USB channel handle lookup table. The pCanUsbSettings.channel value can
 *         be used as the index.
 */
static const TPCANHandle pCanUsbChannelLookup[] =
{
  PCAN_USBBUS1,     /* PCAN-USB interface, channel 1.  */
  PCAN_USBBUS2,     /* PCAN-USB interface, channel 2.  */
  PCAN_USBBUS3,     /* PCAN-USB interface, channel 3.  */
  PCAN_USBBUS4,     /* PCAN-USB interface, channel 4.  */
  PCAN_USBBUS5,     /* PCAN-USB interface, channel 5.  */
  PCAN_USBBUS6,     /* PCAN-USB interface, channel 6.  */
  PCAN_USBBUS7,     /* PCAN-USB interface, channel 7.  */
  PCAN_USBBUS8,     /* PCAN-USB interface, channel 8.  */
  PCAN_USBBUS9,     /* PCAN-USB interface, channel 9.  */
  PCAN_USBBUS10,    /* PCAN-USB interface, channel 10. */ 
  PCAN_USBBUS11,    /* PCAN-USB interface, channel 11. */
  PCAN_USBBUS12,    /* PCAN-USB interface, channel 12. */
  PCAN_USBBUS13,    /* PCAN-USB interface, channel 13. */
  PCAN_USBBUS14,    /* PCAN-USB interface, channel 14. */
  PCAN_USBBUS15,    /* PCAN-USB interface, channel 15. */
  PCAN_USBBUS16     /* PCAN-USB interface, channel 16. */
};


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief The settings to use in this CAN interface. */
static tCanSettings pCanUsbSettings;

/** \brief Boolean flag to track if CAN classic or CAN FD should be used. */
static bool pCanUsbCanFdModeRequested;

/** \brief Boolean flag to track if the CAN FD bitrate switch feature should be used. */
static bool pCanUsbCanFdBrsRequested;

/** \brief List with callback functions that this driver should use. */
static tCanEvents * pCanUsbEventsList;

/** \brief Total number of event entries into the \ref pCanUsbEventsList list. */
static uint32_t pCanUsbEventsEntries;

/** \brief Handle to the PCAN-Basic dynamic link library. */
static HINSTANCE pCanUsbDllHandle;

/** \brief Function pointer to the PCAN-Basic Initialize function. */
static tPCanUsbLibFuncInitialize pCanUsbLibFuncInitializePtr;

/** \brief Function pointer to the PCAN-Basic Initialize FD function. */
static tPCanUsbLibFuncInitializeFd pCanUsbLibFuncInitializeFdPtr;

/** \brief Function pointer to the PCAN-Basic Uninitialize function. */
static tPCanUsbLibFuncUninitialize pCanUsbLibFuncUninitializePtr;

/** \brief Function pointer to the PCAN-Basic GetStatus function. */
static tPCanUsbLibFuncGetStatus pCanUsbLibFuncGetStatusPtr;

/** \brief Function pointer to the PCAN-Basic GetValue function. */
static tPCanUsbLibFuncGetValue pCanUsbLibFuncGetValuePtr;

/** \brief Function pointer to the PCAN-Basic SetValue function. */
static tPCanUsbLibFuncSetValue pCanUsbLibFuncSetValuePtr;

/** \brief Function pointer to the PCAN-Basic Read function. */
static tPCanUsbLibFuncRead pCanUsbLibFuncReadPtr;

/** \brief Function pointer to the PCAN-Basic Read FD function. */
static tPCanUsbLibFuncReadFd pCanUsbLibFuncReadFdPtr;

/** \brief Function pointer to the PCAN-Basic Write function. */
static tPCanUsbLibFuncWrite pCanUsbLibFuncWritePtr;

/** \brief Function pointer to the PCAN-Basic Write FD function. */
static tPCanUsbLibFuncWriteFd pCanUsbLibFuncWriteFdPtr;

/** \brief Function pointer to the PCAN-Basic FilterMessages function. */
static tPCanUsbLibFuncFilterMessages pCanUsbLibFuncFilterMessagesPtr;

/** \brief Handle for the event to terminate the reception thread. */
static HANDLE pCanUsbTerminateEvent;

/** \brief Handle for a CAN related event. */
static HANDLE pCanUsbCanEvent;

/** \brief Handle for the CAN reception thread. */
static HANDLE pCanUsbRxThreadHandle;


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
  pCanUsbTerminateEvent = NULL;
  pCanUsbCanEvent = NULL;
  pCanUsbRxThreadHandle = NULL;
  pCanUsbDllHandle = NULL;
  pCanUsbCanFdModeRequested = false;
  pCanUsbCanFdBrsRequested = false;
  /* Reset library function pointers. */
  pCanUsbLibFuncInitializePtr = NULL;
  pCanUsbLibFuncInitializeFdPtr = NULL;
  pCanUsbLibFuncUninitializePtr = NULL;
  pCanUsbLibFuncGetStatusPtr = NULL;
  pCanUsbLibFuncGetValuePtr = NULL;
  pCanUsbLibFuncSetValuePtr = NULL;
  pCanUsbLibFuncReadPtr = NULL;
  pCanUsbLibFuncReadFdPtr = NULL;
  pCanUsbLibFuncWritePtr = NULL;
  pCanUsbLibFuncWriteFdPtr = NULL;
  pCanUsbLibFuncFilterMessagesPtr = NULL;
  /* Reset CAN interface settings. */
  pCanUsbSettings.devicename = "";
  pCanUsbSettings.channel = 0;
  pCanUsbSettings.baudrate = CAN_BR500K;
  pCanUsbSettings.code = 0x00000000u;
  pCanUsbSettings.mask = 0x00000000u;
  pCanUsbSettings.brsbaudrate = CANFD_DISABLED;

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
    /* Perform initialization of PCAN-Basic API. */
    PCanUsbLibLoadDll();
  }
} /*** end of PCanUsbInit ***/


/************************************************************************************//**
** \brief     Terminates the CAN interface.
**
****************************************************************************************/
static void PCanUsbTerminate(void)
{
  /* Perform termination of PCAN-Basic API. */
  PCanUsbLibUnloadDll();
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
  pCanUsbSettings.brsbaudrate = CANFD_DISABLED;
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
  bool channelSupported;
  bool libInitialized = false;
  uint32_t bitrate;
  uint32_t iFeatures;
  tPCanUsbPeriphParams periphParams = { 0 };
  tPCanUsbBittimingCfg bittimingCfg = { 0 };
  TPCANBaudrate btr0btr1 = 0;
  TPCANBitrateFD bitrateFdStr;
  TPCANBitrateFD partialStr;
#if (PCANUSB_BUSOFF_AUTORECOVERY_ENABLE > 0)
  uint32_t iBuffer;
#endif

  /* Reset CAN FD related flags. */
  pCanUsbCanFdModeRequested = false;
  pCanUsbCanFdBrsRequested = false;

  /* Set flags to determine if and which CAN FD features are requested. */
  if (pCanUsbSettings.brsbaudrate != CANFD_DISABLED)
  {
    /* CAN FD mode requested. */
    pCanUsbCanFdModeRequested = true;
    /* When the bitrate switch feature is requested, the bitrate switch baudrate value
     * should be higher than the nominal baudrate.
     */
    if (CanConvertFdBaudrate(pCanUsbSettings.brsbaudrate) > CanConvertBaudrate(pCanUsbSettings.baudrate))
    {
      /* CAN FD bitrate switch requested. */
      pCanUsbCanFdBrsRequested = true;
    }
  }

  /* Validate channel index. For this CAN interface, the channel index specifies the
   * PCAN-USB device, in case multiple are connected.
   */
  channelSupported = (pCanUsbSettings.channel < (sizeof(pCanUsbChannelLookup) / 
                                                 sizeof(pCanUsbChannelLookup[0])));

  /* Note that the device name itself is not needed anymore at this point, it was only
   * needed by the CAN driver to link the correct interface (this one). Check settings.
   */
  assert(channelSupported);

  /* Invalidate handles. */
  pCanUsbTerminateEvent = NULL;
  pCanUsbCanEvent = NULL;
  pCanUsbRxThreadHandle = NULL;

  /* Only continue with valid settings. */
  if (channelSupported)
  {
    /* Init result code to success and only negate it on detection of error. */
    result = true;

    /* CAN FD mode requested? */
    if (pCanUsbCanFdModeRequested)
    {
      /* Query the hardware to check if CAN FD mode is actually supported. */
      iFeatures = 0;
      if (PCanUsbLibFuncGetValue(pCanUsbChannelLookup[pCanUsbSettings.channel],
                                 PCAN_CHANNEL_FEATURES, &iFeatures,
                                 sizeof(iFeatures)) != PCAN_ERROR_OK)
      {
        /* Could not retrieve the CAN channel features. */
        result = false;
      }
      else
      {
        /* Is the CAN channel FD capable? */
        if ((iFeatures & FEATURE_FD_CAPABLE) == 0)
        {
          /* Not FD capable, yet this was requested. Flag the error. */
          result = false;
        }
      }
    }

    /* Connect to the CAN hardware interface. */
    if (result)
    {
      /* CAN FD mode requested? */
      if (pCanUsbCanFdModeRequested)
      {
        /* Allocate memory to construct the bitrate FD string and initialize it to zeros. */
        bitrateFdStr = malloc(256);
        assert(bitrateFdStr != NULL);
        /* Allocate memory to construct the partial string and initialize it to zeros. */
        partialStr = malloc(32);
        assert(partialStr != NULL);
        /* Check allocation results. */
        if ((bitrateFdStr == NULL) || (partialStr == NULL))
        {
          result = false;
        }

        /* Only continue if the allocation was successful. */
        if (result)
        {
          /* Zero the strings. */
          memset(bitrateFdStr, '\0', 256); /*lint !e668 bitrateFdStr is never NULL pointers at this point. */
          memset(partialStr, '\0', 32);    /*lint !e668 partialStr   is never NULL pointers at this point. */

          /* Initialize the parameters for the nominal (arbitration) bittiming settings. */
          periphParams.base_freq = 80000000U;
          periphParams.prescaler_min = 1U;
          periphParams.prescaler_max = 1024U;
          periphParams.tseg1_min = 1U;
          periphParams.tseg1_max = 256U;
          periphParams.tseg2_min = 1U;
          periphParams.tseg2_max = 128U;
          periphParams.sjw_min = 1U;
          periphParams.sjw_max = 128U;
          /* Determine the requested nominal (arbitration) bitrate in bits/second. */
          bitrate = CanConvertBaudrate(pCanUsbSettings.baudrate);
          /* Attempt to calculate the nominal (arbitration) bittiming settings. */
          if (!PCanUsbCalculateBitTimingConfig(bitrate, &periphParams, &bittimingCfg))
          {
            result = false;
          }
          else
          {
            /* Construct the bitrate FD string for the nominal bitrate. */
            sprintf(partialStr, "f_clock=%u", periphParams.base_freq);
            strcat(bitrateFdStr, partialStr);
            sprintf(partialStr, ",nom_brp=%u", bittimingCfg.prescaler);
            strcat(bitrateFdStr, partialStr);
            sprintf(partialStr, ",nom_tseg1=%u", bittimingCfg.tseg1);
            strcat(bitrateFdStr, partialStr);
            sprintf(partialStr, ",nom_tseg2=%u", bittimingCfg.tseg2);
            strcat(bitrateFdStr, partialStr);
            sprintf(partialStr, ",nom_sjw=%u", bittimingCfg.sjw);
            strcat(bitrateFdStr, partialStr);
          }
        }

        /* Continue with data bittiming settings with BRS enabled and all still okay. */
        if ((result) && (pCanUsbCanFdBrsRequested))
        {
          /* Initialize the parameters for the data bittiming settings. */
          periphParams.base_freq = 80000000U;
          periphParams.prescaler_min = 1U;
          periphParams.prescaler_max = 1024U;
          periphParams.tseg1_min = 1U;
          periphParams.tseg1_max = 32U;
          periphParams.tseg2_min = 1U;
          periphParams.tseg2_max = 16U;
          periphParams.sjw_min = 1U;
          periphParams.sjw_max = 16U;
          /* Determine the requested nominal (arbitration) bitrate in bits/second. */
          bitrate = CanConvertFdBaudrate(pCanUsbSettings.brsbaudrate);
          /* Attempt to calculate the data bittiming settings. */
          if (!PCanUsbCalculateBitTimingConfig(bitrate, &periphParams, &bittimingCfg))
          {
            result = false;
          }
          else
          {
            /* Append the bitrate FD string for the data bitrate. */
            sprintf(partialStr, ",data_brp=%u", bittimingCfg.prescaler); /*lint !e668 */
            strcat(bitrateFdStr, partialStr); /*lint !e668 */
            sprintf(partialStr, ",data_tseg1=%u", bittimingCfg.tseg1);
            strcat(bitrateFdStr, partialStr);
            sprintf(partialStr, ",data_tseg2=%u", bittimingCfg.tseg2);
            strcat(bitrateFdStr, partialStr);
            sprintf(partialStr, ",data_sjw=%u", bittimingCfg.sjw);
            strcat(bitrateFdStr, partialStr);
          }
        }
        /* Connect to the CAN hardware interface if all is still okay. */
        if (result)
        {
          if (PCanUsbLibFuncInitializeFd(pCanUsbChannelLookup[pCanUsbSettings.channel],
                                         bitrateFdStr) != PCAN_ERROR_OK)
          {
            result = false;
          }
        }
        /* Release the memory allocated for the strings. */
        free(partialStr);
        free(bitrateFdStr);
      }
      /* CAN Classic mode requested. */
      else
      {
        /* Initialize for SJA1000 CAN controller. */
        periphParams.base_freq = 8000000U;
        periphParams.prescaler_min = 1U;
        periphParams.prescaler_max = 64U; 
        periphParams.tseg1_min = 1U;
        periphParams.tseg1_max = 16U;
        periphParams.tseg2_min = 1U;
        periphParams.tseg2_max = 8U;
        periphParams.sjw_min = 1U;
        periphParams.sjw_max = 1U;
        /* Determine the requested nominal (arbitration) bitrate in bits/second. */
        bitrate = CanConvertBaudrate(pCanUsbSettings.baudrate);
        /* Attempt to calculate the nominal (arbitration) bittiming settings. */
        if (!PCanUsbCalculateBitTimingConfig(bitrate, &periphParams, &bittimingCfg))
        {
          result = false;
        }
        else
        {
          /* Convert to BTR0BTR1 value. */
          btr0btr1 |= (TPCANBaudrate)((bittimingCfg.prescaler - 1U) << 8U);
          btr0btr1 |= (TPCANBaudrate)((bittimingCfg.sjw       - 1U) << (6U + 8U));
          btr0btr1 |= (TPCANBaudrate)( bittimingCfg.tseg1     - 1U);
          btr0btr1 |= (TPCANBaudrate)((bittimingCfg.tseg2     - 1U) << 4U);
          /* Connect to the CAN hardware interface. */
          if (PCanUsbLibFuncInitialize(pCanUsbChannelLookup[pCanUsbSettings.channel],
                                       btr0btr1, 0, 0, 0) != PCAN_ERROR_OK)
          {
            result = false;
          }
        }
      }
      /* Update library initialized flag. */
      if (result)
      {
        libInitialized = true;
      }
    }

    /* Create the terminate event handle used in the reception thread. */
    if (result)
    {
      pCanUsbTerminateEvent = CreateEvent(NULL, TRUE, FALSE, "");
      if (pCanUsbTerminateEvent == NULL)
      {
        result = false;
      }
    }
    
    /* Create the CAN event handle used in the reception thread. */
    if (result)
    {
      pCanUsbCanEvent = CreateEvent(NULL, FALSE, FALSE, "");
      if (pCanUsbCanEvent == NULL)
      {
        result = false;
      }
    }

#if (PCANUSB_BUSOFF_AUTORECOVERY_ENABLE > 0)
    /* Enable the bus off auto reset. */
    if (result)
    {
      iBuffer = PCAN_PARAMETER_ON;
      if (PCanUsbLibFuncSetValue(pCanUsbChannelLookup[pCanUsbSettings.channel],
                                 PCAN_BUSOFF_AUTORESET, &iBuffer,
                                 sizeof(iBuffer)) != PCAN_ERROR_OK)
      {
        result = false;
      }
    }
#endif

    /* Configure reception acceptance filter, if it is not supposed to be fully open. */
    if ( (result) && (pCanUsbSettings.mask != 0x00000000u) )
    {
      /* Configuration of the reception acceptance filter in the PCAN-Basic library does
       * not use a mask and code approach. Instead it has an API function for setting 
       * the filter by specifing an identifier range. This means the mask and code pair
       * should be used to calculate the lowest and highest identifier that should pass
       * the filter. It is not 100% accurate. In some cases more identifiers can pass
       * the filter than desired, but there is no other option:
       *   Lowest CAN ID  = (code &  mask) & 0x1fffffff
       *   Highest CAN ID = (code | ~mask) & 0x1fffffff
       */
      uint32_t lowestCanID  = (pCanUsbSettings.code &  pCanUsbSettings.mask)  & 0x1fffffffu;
      uint32_t highestCanID = (pCanUsbSettings.code | ~pCanUsbSettings.mask)  & 0x1fffffffu;
      /* Use bit logic to determine if the filter should accept standard 11-bit and/or
       * extended 29-bit identifiers:
       *   acceptStdId = ((mask & code & CAN_MSG_EXT_ID_MASK) == 0)
       *   acceptExtId = ((mask & code & CAN_MSG_EXT_ID_MASK) != 0) || 
       *                 ((mask & CAN_MSG_EXT_ID_MASK) == 0)
       */
      bool acceptStdID = ((pCanUsbSettings.mask & pCanUsbSettings.code & CAN_MSG_EXT_ID_MASK) == 0);
      bool acceptExtID = ((pCanUsbSettings.mask & pCanUsbSettings.code & CAN_MSG_EXT_ID_MASK) != 0) ||
                         ((pCanUsbSettings.mask & CAN_MSG_EXT_ID_MASK) == 0);
      /* Configure acceptance filter for standard 11-bit identifiers. */
      if (acceptStdID)
      {
        if (PCanUsbLibFuncFilterMessages(pCanUsbChannelLookup[pCanUsbSettings.channel],
                                         lowestCanID, highestCanID,
                                         PCAN_MODE_STANDARD) != PCAN_ERROR_OK)
        {
          result = false;
        }
      }
      /* Configure acceptance filter for extended 29-bit identifiers. */
      if ( (acceptExtID) && (result) )
      {
        if (PCanUsbLibFuncFilterMessages(pCanUsbChannelLookup[pCanUsbSettings.channel],
                                         lowestCanID, highestCanID,
                                         PCAN_MODE_EXTENDED) != PCAN_ERROR_OK)
        {
          result = false;
        }
      }
    }

    /* Register the event reception handle. */
    if (result)
    {
      if (PCanUsbLibFuncSetValue(pCanUsbChannelLookup[pCanUsbSettings.channel],
                                 PCAN_RECEIVE_EVENT, &pCanUsbCanEvent,
                                 sizeof(pCanUsbCanEvent)) != PCAN_ERROR_OK)
      {
        result = false;
      }
    }

    /* Start the reception thread as the last step. */
    if (result)
    {
      pCanUsbRxThreadHandle = CreateThread(NULL, 0, PCanUsbReceptionThread,
                                           NULL, 0, NULL);
      if (pCanUsbRxThreadHandle == NULL)
      {
        result = false;
      }
    }
  }

  /* Clean-up in case an error occurred. */
  if (!result)
  {
    if (libInitialized)
    { 
      /* Uninitialize the library. */
      (void)PCanUsbLibFuncUninitialize(pCanUsbChannelLookup[pCanUsbSettings.channel]);
    }
    if (pCanUsbTerminateEvent != NULL)
    {
      /* Close the event handle. */
      (void)CloseHandle(pCanUsbTerminateEvent);
      pCanUsbTerminateEvent = NULL;
    }
    if (pCanUsbCanEvent != NULL)
    {
      /* Close the event handle. */
      (void)CloseHandle(pCanUsbCanEvent);
      pCanUsbCanEvent = NULL;
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of PCanUsbConnect ***/


/************************************************************************************//**
** \brief     Disconnects the CAN interface.
**
****************************************************************************************/
static void PCanUsbDisconnect(void)
{
   /* Stop the reception thread. */
  if (pCanUsbRxThreadHandle != NULL)
  {
    /* Trigger event to request the reception thread to stop. */
    (void)SetEvent(pCanUsbTerminateEvent);
    /* Wait for the thread to signal termination. */
    (void)WaitForSingleObject(pCanUsbRxThreadHandle, INFINITE);
    /* Close the thread handle. */
    (void)CloseHandle(pCanUsbRxThreadHandle);
    pCanUsbRxThreadHandle = NULL;
  }

  /* Close the terminate event handle. */
  if (pCanUsbTerminateEvent != NULL)
  {
    (void)CloseHandle(pCanUsbTerminateEvent);
    pCanUsbTerminateEvent = NULL;
  }
  /* Close the CAN event handle. */
  if (pCanUsbCanEvent != NULL)
  {
    (void)CloseHandle(pCanUsbCanEvent);
    pCanUsbCanEvent = NULL;
  }
  /* Disconnect from the CAN interface. */
  (void)PCanUsbLibFuncUninitialize(pCanUsbChannelLookup[pCanUsbSettings.channel]);
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
  TPCANMsg msgBuf;
  TPCANMsgFD msgBufFd;
  uint8_t dataLen;
  tCanEvents const * pEvents;
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
 
  /* Check parameters. */
  assert(msg != NULL);

  /* Only continue with valid parameters. */
  if (msg != NULL) /*lint !e774 */
  {
    /* CAN classic mode? */
    if (!pCanUsbCanFdModeRequested)
    {
      /* Convert the message to a type supported by the PCAN-Basic API. */
      if ((msg->id & CAN_MSG_EXT_ID_MASK) == 0)
      {
        msgBuf.ID = msg->id & 0x7ffu;
        msgBuf.MSGTYPE = PCAN_MESSAGE_STANDARD;
      }
      else
      {
        msgBuf.ID = msg->id & 0x1fffffffu;
        msgBuf.MSGTYPE = PCAN_MESSAGE_EXTENDED;
      }
      /* Determine data length with out-of-bounds correction. */
      dataLen = ((msg->len <= 8U) ? msg->len : 8U);
      msgBuf.LEN = dataLen;
      for (uint8_t idx = 0; idx < dataLen; idx++)
      {
        msgBuf.DATA[idx] = msg->data[idx];
      }
      /* Submit CAN message for transmission. */
      if (PCanUsbLibFuncWrite(pCanUsbChannelLookup[pCanUsbSettings.channel],
                              &msgBuf) == PCAN_ERROR_OK)
      {
        /* Update result value to success. */
        result = true;
      }
    }
    else
    {
      /* Convert the message to a type supported by the PCAN-Basic API. */
      if ((msg->id & CAN_MSG_EXT_ID_MASK) == 0)
      {
        msgBufFd.ID = msg->id & 0x7ffu;
        msgBufFd.MSGTYPE = PCAN_MESSAGE_STANDARD;
      }
      else
      {
        msgBufFd.ID = msg->id & 0x1fffffffu;
        msgBufFd.MSGTYPE = PCAN_MESSAGE_EXTENDED;
      }
      /* Flag the message as a CAN FD message. */
      msgBufFd.MSGTYPE |= PCAN_MESSAGE_FD;
      /* Bitrate switch feature enabled? */
      if (pCanUsbCanFdBrsRequested)
      {
        msgBufFd.MSGTYPE |= PCAN_MESSAGE_BRS;
      }
      /* Determine data length with out-of-bounds correction. */
      dataLen = ((msg->len <= CAN_MSG_MAX_LEN) ? msg->len : CAN_MSG_MAX_LEN);
      msgBufFd.DLC = len2dlc[dataLen];
      for (uint8_t idx = 0; idx < dataLen; idx++)
      {
        msgBufFd.DATA[idx] = msg->data[idx];
      }
      /* Submit CAN message for transmission. */
      if (PCanUsbLibFuncWriteFd(pCanUsbChannelLookup[pCanUsbSettings.channel],
                                &msgBufFd) == PCAN_ERROR_OK)
      {
        /* Update result value to success. */
        result = true;
      }
    }

    /* Message successfully submitted for transmission? */
    if (result)
    {
      /* Trigger transmit complete event(s). */
      pEvents = pCanUsbEventsList;
      for (uint32_t idx = 0; idx < pCanUsbEventsEntries; idx++)
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
} /*** end of PCanUsbTransmit ***/


/************************************************************************************//**
** \brief     Checks if a bus off or bus heavy situation occurred.
** \return    True if a bus error situation was detected, false otherwise.
**
****************************************************************************************/
static bool PCanUsbIsBusError(void)
{
  bool result = false;
  TPCANStatus status;

  /* Obtain the status information. */
  status = PCanUsbLibFuncGetStatus(pCanUsbChannelLookup[pCanUsbSettings.channel]);

  /* Process the status to detect bus off or bus heavy. */
  if ((status == PCAN_ERROR_BUSOFF) || (status == PCAN_ERROR_BUSHEAVY))
  {
    result = true;
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of PCanUsbIsBusError ***/


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
static bool PCanUsbCalculateBitTimingConfig(uint32_t baudrate,
                                            tPCanUsbPeriphParams const * periph_params,
                                            tPCanUsbBittimingCfg * bittiming_config)
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
               * make sure SJW is within its min and max ranges.
               */
              bittiming_config->sjw = tseg2 / 2U;
              bittiming_config->sjw = max(bittiming_config->sjw, periph_params->sjw_min);
              bittiming_config->sjw = min(bittiming_config->sjw, periph_params->sjw_max);
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
** \brief     CAN message reception thread.
** \param     pv Pointer to thread parameters.
** \return    Thread exit code.
**
****************************************************************************************/
static DWORD WINAPI PCanUsbReceptionThread(LPVOID pv)
{
  DWORD waitResult;
  HANDLE handles[] = 
  { 
    pCanUsbCanEvent, 
    pCanUsbTerminateEvent 
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
        if (!pCanUsbCanFdModeRequested)
        {
          PCanUsbProcessCanClassicEvent();
        }
        /* CAN FD mode */
        else
        {
          PCanUsbProcessCanFdEvent();
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
} /*** end of PCanUsbReceptionThread ***/


/************************************************************************************//**
** \brief     CAN classic event handler. Should be called when a CAN classic related
**            notication event triggered.
**
****************************************************************************************/
static void PCanUsbProcessCanClassicEvent(void)
{
  TPCANMsg rxLibMsg;
  tCanMsg rxMsg;
  tCanEvents const * pEvents;

  /* Empty out the queue with received events. */
  while (PCanUsbLibFuncRead(pCanUsbChannelLookup[pCanUsbSettings.channel],
                            &rxLibMsg, NULL) == PCAN_ERROR_OK)
  {
    /* Only process events that contain a CAN message with a 11-bit or 29-bit
     * identifier.
     */
    if ((rxLibMsg.MSGTYPE == PCAN_MESSAGE_STANDARD) ||
        (rxLibMsg.MSGTYPE == PCAN_MESSAGE_EXTENDED))
    {
      /* Convert CAN mesage from PCAN-Basic format to the one of the CAN driver
       * module.
       */
      rxMsg.id = rxLibMsg.ID;
      if (rxLibMsg.MSGTYPE == PCAN_MESSAGE_EXTENDED)
      {
        rxMsg.id |= CAN_MSG_EXT_ID_MASK;
      }
      rxMsg.len = rxLibMsg.LEN;
      for (uint8_t idx = 0; idx < rxMsg.len; idx++)
      {
        rxMsg.data[idx] = rxLibMsg.DATA[idx];
      }

      /* Trigger message reception event(s). */
      pEvents = pCanUsbEventsList;
      for (uint32_t idx = 0; idx < pCanUsbEventsEntries; idx++)
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
} /*** end of PCanUsbProcessCanClassicEvent ***/


/************************************************************************************//**
** \brief     CAN FD event handler. Should be called when a CAN FD related notication
**            event triggered.
**
****************************************************************************************/
static void PCanUsbProcessCanFdEvent(void)
{
  TPCANMsgFD rxLibMsg;
  tCanMsg rxMsg;
  tCanEvents const * pEvents;
  static uint8_t const dlc2len[] = { 0,  1,  2,  3,  4,  5,  6,  7,
                                     8, 12, 16, 20, 24, 32, 48, 64 };

  /* Empty out the queue with received events. */
  while (PCanUsbLibFuncReadFd(pCanUsbChannelLookup[pCanUsbSettings.channel],
                              &rxLibMsg, NULL) == PCAN_ERROR_OK)
  {
    /* Only interested in the STD and EXT flags, so reset the FD flags. */
    rxLibMsg.MSGTYPE &= ~(PCAN_MESSAGE_FD | PCAN_MESSAGE_BRS);
    /* Only process events that contain a CAN message with a 11-bit or 29-bit
     * identifier.
     */
    if ((rxLibMsg.MSGTYPE == PCAN_MESSAGE_STANDARD) ||
        (rxLibMsg.MSGTYPE == PCAN_MESSAGE_EXTENDED))
    {
      /* Convert CAN mesage from PCAN-Basic format to the one of the CAN driver
       * module.
       */
      rxMsg.id = rxLibMsg.ID;
      if (rxLibMsg.MSGTYPE == PCAN_MESSAGE_EXTENDED)
      {
        rxMsg.id |= CAN_MSG_EXT_ID_MASK;
      }
      rxMsg.len = dlc2len[rxLibMsg.DLC];
      for (uint8_t idx = 0; idx < rxMsg.len; idx++)
      {
        rxMsg.data[idx] = rxLibMsg.DATA[idx];
      }

      /* Trigger message reception event(s). */
      pEvents = pCanUsbEventsList;
      for (uint32_t idx = 0; idx < pCanUsbEventsEntries; idx++)
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
} /*** end of PCanUsbProcessCanFdEvent ***/


/************************************************************************************//**
** \brief     Loads the PCAN-Basic DLL and initializes the API function pointers.
**
****************************************************************************************/
static void PCanUsbLibLoadDll(void)
{
  /* Start out by resetting the API function pointers. */
  pCanUsbLibFuncInitializePtr = NULL;
  pCanUsbLibFuncInitializeFdPtr = NULL;
  pCanUsbLibFuncUninitializePtr = NULL;
  pCanUsbLibFuncGetStatusPtr = NULL;
  pCanUsbLibFuncGetValuePtr = NULL;
  pCanUsbLibFuncSetValuePtr = NULL;
  pCanUsbLibFuncReadPtr = NULL;
  pCanUsbLibFuncReadFdPtr = NULL;
  pCanUsbLibFuncWritePtr = NULL;
  pCanUsbLibFuncWriteFdPtr = NULL;
  pCanUsbLibFuncFilterMessagesPtr = NULL;

  /* Attempt to load the library and obtain a handle to it. */
  pCanUsbDllHandle = LoadLibrary("PCANBasic");

  /* Assert libary handle. */
  assert(pCanUsbDllHandle != NULL);

  /* Only continue if the library was successfully loaded */
  if (pCanUsbDllHandle != NULL) /*lint !e774 */
  {
    /* Set CAN_Initialize function pointer. */
    pCanUsbLibFuncInitializePtr = (tPCanUsbLibFuncInitialize)GetProcAddress(pCanUsbDllHandle, "CAN_Initialize");
    /* Set CAN_InitializeFD function pointer. */
    pCanUsbLibFuncInitializeFdPtr = (tPCanUsbLibFuncInitializeFd)GetProcAddress(pCanUsbDllHandle, "CAN_InitializeFD");
    /* Set CAN_Uninitialize function pointer. */
    pCanUsbLibFuncUninitializePtr = (tPCanUsbLibFuncUninitialize)GetProcAddress(pCanUsbDllHandle, "CAN_Uninitialize");
    /* Set CAN_GetStatus function pointer. */
    pCanUsbLibFuncGetStatusPtr = (tPCanUsbLibFuncGetStatus)GetProcAddress(pCanUsbDllHandle, "CAN_GetStatus");
    /* Set CAN_GetValue function pointer. */
    pCanUsbLibFuncGetValuePtr = (tPCanUsbLibFuncGetValue)GetProcAddress(pCanUsbDllHandle, "CAN_GetValue");
    /* Set CAN_SetValue function pointer. */
    pCanUsbLibFuncSetValuePtr = (tPCanUsbLibFuncSetValue)GetProcAddress(pCanUsbDllHandle, "CAN_SetValue");
    /* Set CAN_Read function pointer. */
    pCanUsbLibFuncReadPtr = (tPCanUsbLibFuncRead)GetProcAddress(pCanUsbDllHandle, "CAN_Read");
    /* Set CAN_ReadFD function pointer. */
    pCanUsbLibFuncReadFdPtr = (tPCanUsbLibFuncReadFd)GetProcAddress(pCanUsbDllHandle, "CAN_ReadFD");
    /* Set CAN_Write function pointer. */
    pCanUsbLibFuncWritePtr = (tPCanUsbLibFuncWrite)GetProcAddress(pCanUsbDllHandle, "CAN_Write");
    /* Set CAN_WriteFD function pointer. */
    pCanUsbLibFuncWriteFdPtr = (tPCanUsbLibFuncWriteFd)GetProcAddress(pCanUsbDllHandle, "CAN_WriteFD");
    /* Set CAN_FilterMessages function pointer. */
    pCanUsbLibFuncFilterMessagesPtr = (tPCanUsbLibFuncFilterMessages)GetProcAddress(pCanUsbDllHandle, "CAN_FilterMessages");
  }
} /*** end of PCanUsbLibLoadDll ***/


/************************************************************************************//**
** \brief     Unloads the PCAN-Basic DLL and resets the API function pointers.
**
****************************************************************************************/
static void PCanUsbLibUnloadDll(void)
{
  /* Reset the API function pointers. */
  pCanUsbLibFuncInitializePtr = NULL;
  pCanUsbLibFuncInitializeFdPtr = NULL;
  pCanUsbLibFuncUninitializePtr = NULL;
  pCanUsbLibFuncGetStatusPtr = NULL;
  pCanUsbLibFuncGetValuePtr = NULL;
  pCanUsbLibFuncSetValuePtr = NULL;
  pCanUsbLibFuncReadPtr = NULL;
  pCanUsbLibFuncReadFdPtr = NULL;
  pCanUsbLibFuncWritePtr = NULL;
  pCanUsbLibFuncWriteFdPtr = NULL;
  pCanUsbLibFuncFilterMessagesPtr = NULL;

  /* Unload the library and invalidate its handle. */
  if (pCanUsbDllHandle != NULL) 
  {
    (void)FreeLibrary(pCanUsbDllHandle);
    pCanUsbDllHandle = NULL;
  }
} /*** end of PCanUsbLibUnloadDll ***/


/************************************************************************************//**
** \brief     Initializes a PCAN Channel.
** \param     Channel The handle of a PCAN Channel.
** \param     Btr0Btr1 The speed for the communication (BTR0BTR1 code).
** \param     HwType The type of the Non-Plug-and-Play hardware and its operation mode.
** \param     IOPort The I/O address for the parallel port of the Non-Plug-and-Play 
**            hardware.
** \param     Interrupt The Interrupt number of the parallel port of the Non-Plug-
**            and-Play hardware. 
** \return    The return value is a TPCANStatus code. PCAN_ERROR_OK is returned on 
**            success.
**
****************************************************************************************/
static TPCANStatus PCanUsbLibFuncInitialize(TPCANHandle Channel, TPCANBaudrate Btr0Btr1, 
                                            TPCANType HwType, DWORD IOPort, 
                                            WORD Interrupt)
{
  /* set result to error. */
  TPCANStatus result = PCAN_ERROR_INITIALIZE;

  /* Check function pointer and library handle. */
  assert(pCanUsbLibFuncInitializePtr != NULL);
  assert(pCanUsbDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((pCanUsbLibFuncInitializePtr != NULL) && (pCanUsbDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = pCanUsbLibFuncInitializePtr(Channel, Btr0Btr1, HwType, IOPort, Interrupt);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of PCanUsbLibFuncInitialize ***/


/************************************************************************************//**
** \brief     Initializes a FD capable PCAN Channel.
** \param     Channel The handle of a PCAN Channel.
** \param     BitrateFD The speed for the communication (FD bit rate string). See
**            PCAN_BR_* values.
**            - Parameter and values must be separated by '='
**            - Couples of Parameter/value must be separated by ','
**            - Following Parameter must be filled out: f_clock, data_brp, data_sjw, 
**              data_tseg1, data_tseg2, nom_brp, nom_sjw, nom_tseg1, nom_tseg2.
**            - Following Parameters are optional (not used yet): data_ssp_offset,
**              nom_sam
** \return    The return value is a TPCANStatus code. PCAN_ERROR_OK is returned on
**            success.
**
****************************************************************************************/
static TPCANStatus PCanUsbLibFuncInitializeFd(TPCANHandle Channel,
                                              TPCANBitrateFD BitrateFD) /*lint -esym(593, BitrateFD) */
{
  /* set result to error. */
  TPCANStatus result = PCAN_ERROR_INITIALIZE;

  /* Check function pointer and library handle. */
  assert(pCanUsbLibFuncInitializeFdPtr != NULL);
  assert(pCanUsbDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((pCanUsbLibFuncInitializeFdPtr != NULL) && (pCanUsbDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = pCanUsbLibFuncInitializeFdPtr(Channel, BitrateFD);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of PCanUsbLibFuncInitializeFd ***/


/************************************************************************************//**
** \brief     Uninitializes a PCAN Channel.
** \param     Channel The handle of a PCAN Channel.
** \return    The return value is a TPCANStatus code. PCAN_ERROR_OK is returned on 
**            success.
**
****************************************************************************************/
static TPCANStatus PCanUsbLibFuncUninitialize(TPCANHandle Channel)
{
  /* set result to error. */
  TPCANStatus result = PCAN_ERROR_INITIALIZE;

  /* Check function pointer and library handle. */
  assert(pCanUsbLibFuncUninitializePtr != NULL);
  assert(pCanUsbDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((pCanUsbLibFuncUninitializePtr != NULL) && (pCanUsbDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = pCanUsbLibFuncUninitializePtr(Channel);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of PCanUsbLibFuncUninitialize ***/


/************************************************************************************//**
** \brief     Gets the current BUS status of a PCAN Channel.
** \param     Channel The handle of a PCAN Channel.
** \return    The return value is a TPCANStatus code. PCAN_ERROR_OK is returned on 
**            success.
**
****************************************************************************************/
static TPCANStatus PCanUsbLibFuncGetStatus(TPCANHandle Channel)
{
  /* set result to error. */
  TPCANStatus result = PCAN_ERROR_INITIALIZE;

  /* Check function pointer and library handle. */
  assert(pCanUsbLibFuncGetStatusPtr != NULL);
  assert(pCanUsbDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((pCanUsbLibFuncGetStatusPtr != NULL) && (pCanUsbDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = pCanUsbLibFuncGetStatusPtr(Channel);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of PCanUsbLibFuncGetStatus ***/


/************************************************************************************//**
** \brief     Retrieves a PCAN Channel value.
** \param     Channel The handle of a PCAN Channel.
** \param     Parameter The TPCANParameter parameter to get.
** \param     Buffer Buffer for the parameter value
** \param     BufferLength Size in bytes of the buffer.
** \return    The return value is a TPCANStatus code. PCAN_ERROR_OK is returned on
**            success.
**
****************************************************************************************/
static TPCANStatus PCanUsbLibFuncGetValue(TPCANHandle Channel, TPCANParameter Parameter,
                                          void* Buffer, DWORD BufferLength)
{
  /* set result to error. */
  TPCANStatus result = PCAN_ERROR_INITIALIZE;

  /* Check function pointer and library handle. */
  assert(pCanUsbLibFuncGetValuePtr != NULL);
  assert(pCanUsbDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((pCanUsbLibFuncGetValuePtr != NULL) && (pCanUsbDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = pCanUsbLibFuncGetValuePtr(Channel, Parameter, Buffer, BufferLength);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of PCanUsbLibFuncGetValue ***/


/************************************************************************************//**
** \brief     Sets a configuration or information value within a PCAN Channel.
** \param     Channel The handle of a PCAN Channel.
** \param     Parameter The code of the value to be set .
** \param     Buffer The buffer containing the value to be set.
** \param     BufferLength The length in bytes of the given buffer.
** \return    The return value is a TPCANStatus code. PCAN_ERROR_OK is returned on 
**            success.
**
****************************************************************************************/
static TPCANStatus PCanUsbLibFuncSetValue(TPCANHandle Channel, TPCANParameter Parameter,
                                          void * Buffer, DWORD BufferLength)
{
  /* set result to error. */
  TPCANStatus result = PCAN_ERROR_INITIALIZE;

  /* Check function pointer and library handle. */
  assert(pCanUsbLibFuncSetValuePtr != NULL);
  assert(pCanUsbDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((pCanUsbLibFuncSetValuePtr != NULL) && (pCanUsbDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = pCanUsbLibFuncSetValuePtr(Channel, Parameter, Buffer, BufferLength);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of PCanUsbLibFuncSetValue ***/


/************************************************************************************//**
** \brief     Reads a CAN message from the receive queue of a PCAN Channel.
** \param     Channel The handle of a PCAN Channel.
** \param     MessageBuffer A TPCANMsg buffer to store the CAN message. 
** \param     TimestampBuffer A TPCANTimestamp buffer to get the reception time of the 
**            message. 
** \return    The return value is a TPCANStatus code. PCAN_ERROR_OK is returned on 
**            success.
**
****************************************************************************************/
static TPCANStatus PCanUsbLibFuncRead(TPCANHandle Channel, TPCANMsg * MessageBuffer, 
                                      TPCANTimestamp * TimestampBuffer)
{
  /* set result to error. */
  TPCANStatus result = PCAN_ERROR_INITIALIZE;

  /* Check function pointer and library handle. */
  assert(pCanUsbLibFuncReadPtr != NULL);
  assert(pCanUsbDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((pCanUsbLibFuncReadPtr != NULL) && (pCanUsbDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = pCanUsbLibFuncReadPtr(Channel, MessageBuffer, TimestampBuffer);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of PCanUsbLibFuncRead ***/


/************************************************************************************//**
** \brief     Reads a CAN message from the receive queue of a FD capable PCAN Channel.
** \param     Channel The handle of a PCAN Channel.
** \param     MessageBuffer A TPCANMsgFD buffer to store the CAN message.
** \param     TimestampBuffer A TPCANTimestampFD buffer to get the reception time of the
**            message.
** \return    The return value is a TPCANStatus code. PCAN_ERROR_OK is returned on
**            success.
**
****************************************************************************************/
static TPCANStatus PCanUsbLibFuncReadFd(TPCANHandle Channel, TPCANMsgFD* MessageBuffer,
                                        TPCANTimestampFD* TimestampBuffer)
{
  /* set result to error. */
  TPCANStatus result = PCAN_ERROR_INITIALIZE;

  /* Check function pointer and library handle. */
  assert(pCanUsbLibFuncReadFdPtr != NULL);
  assert(pCanUsbDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((pCanUsbLibFuncReadFdPtr != NULL) && (pCanUsbDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = pCanUsbLibFuncReadFdPtr(Channel, MessageBuffer, TimestampBuffer);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of PCanUsbLibFuncReadFd ***/


/************************************************************************************//**
** \brief     Transmits a CAN message.
** \param     Channel The handle of a PCAN Channel.
** \param     MessageBuffer A TPCANMsg buffer containing the CAN message to be sent. 
** \return    The return value is a TPCANStatus code. PCAN_ERROR_OK is returned on 
**            success.
**
****************************************************************************************/
static TPCANStatus PCanUsbLibFuncWrite(TPCANHandle Channel, TPCANMsg * MessageBuffer)
{
  /* set result to error. */
  TPCANStatus result = PCAN_ERROR_INITIALIZE;

  /* Check function pointer and library handle. */
  assert(pCanUsbLibFuncWritePtr != NULL);
  assert(pCanUsbDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((pCanUsbLibFuncWritePtr != NULL) && (pCanUsbDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = pCanUsbLibFuncWritePtr(Channel, MessageBuffer);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of PCanUsbLibFuncWrite ***/


/************************************************************************************//**
** \brief      Transmits a CAN message over a FD capable PCAN Channel
** \param     Channel The handle of a PCAN Channel.
** \param     MessageBuffer A TPCANMsgFD buffer containing the CAN message to be sent.
** \return    The return value is a TPCANStatus code. PCAN_ERROR_OK is returned on
**            success.
**
****************************************************************************************/
static TPCANStatus PCanUsbLibFuncWriteFd(TPCANHandle Channel, TPCANMsgFD* MessageBuffer)
{
  /* set result to error. */
  TPCANStatus result = PCAN_ERROR_INITIALIZE;

  /* Check function pointer and library handle. */
  assert(pCanUsbLibFuncWriteFdPtr != NULL);
  assert(pCanUsbDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((pCanUsbLibFuncWriteFdPtr != NULL) && (pCanUsbDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = pCanUsbLibFuncWriteFdPtr(Channel, MessageBuffer);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of PCanUsbLibFuncWriteFd ***/


/************************************************************************************//**
** \brief     Configures the reception filter.
** \param     Channel The handle of a PCAN Channel.
** \param     FromID The lowest CAN ID wanted to be received.
** \param     ToID The highest CAN ID wanted to be received. 
** \param     Mode The type of the filter being set.
** \return    The return value is a TPCANStatus code. PCAN_ERROR_OK is returned on 
**            success.
**
****************************************************************************************/
static TPCANStatus PCanUsbLibFuncFilterMessages(TPCANHandle Channel, DWORD FromID,
                                                DWORD ToID, TPCANMode Mode)
{
  /* set result to error. */
  TPCANStatus result = PCAN_ERROR_INITIALIZE;

  /* Check function pointer and library handle. */
  assert(pCanUsbLibFuncFilterMessagesPtr != NULL);
  assert(pCanUsbDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((pCanUsbLibFuncFilterMessagesPtr != NULL) && (pCanUsbDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = pCanUsbLibFuncFilterMessagesPtr(Channel, FromID, ToID, Mode);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of PCanUsbLibFuncFilterMessages ***/


/*********************************** end of pcanusb.c **********************************/

