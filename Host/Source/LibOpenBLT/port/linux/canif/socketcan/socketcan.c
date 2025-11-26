/************************************************************************************//**
* \file         socketcan.c
* \brief        Linux SocketCAN interface source file.
* \ingroup      SocketCan
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
#include <pthread.h>                        /* for posix threads                       */
#include <unistd.h>                         /* UNIX standard functions                 */
#include <fcntl.h>                          /* file control operations                 */
#include <sys/time.h>                       /* time definitions                        */
#include <sys/ioctl.h>                      /* I/O control operations                  */
#include <net/if.h>                         /* network interfaces                      */
#include <linux/can.h>                      /* CAN kernel definitions                  */
#include <linux/can/raw.h>                  /* CAN raw sockets                         */
#include <linux/can/error.h>                /* CAN errors                              */
#include "util.h"                           /* Utility module                          */
#include "candriver.h"                      /* Generic CAN driver module               */
#include "socketcan.h"                      /* SocketCAN interface                     */


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Groups data for thread control. */
typedef struct t_socket_can_thread_ctrl
{
  bool terminate;                           /**< flag to request thread termination.   */
  bool terminated;                          /**< handshake flag.                       */
} tSocketCanThreadCtrl;


/***************************************************************************************
* Function prototypes
****************************************************************************************/
/* CAN interface functions. */
static void SocketCanInit(tCanSettings const * settings);
static void SocketCanTerminate(void);
static bool SocketCanConnect(void);
static void SocketCanDisconnect(void);
static bool SocketCanTransmit(tCanMsg const * msg);
static bool SocketCanIsBusError(void);
static void SocketCanRegisterEvents(tCanEvents const * events);
/* Event thread functions. */
static bool  SocketCanStartEventThread(void);
static void  SocketCanStopEventThread(void);
static void *SocketCanEventThread(void *param);
/* Utility functions. */
static uint8_t SocketCanSanitizeFrameLen(uint8_t len);


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief CAN interface structure filled with SocketCAN specifics. */
static const tCanInterface socketCanInterface =
{
  SocketCanInit,
  SocketCanTerminate,
  SocketCanConnect,
  SocketCanDisconnect,
  SocketCanTransmit,
  SocketCanIsBusError,
  SocketCanRegisterEvents
};


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief The settings to use in this CAN interface. */
static tCanSettings socketCanSettings;

/** \brief Boolean flag to track if CAN classic or CAN FD should be used. */
static bool socketCanFdModeRequested;

/** \brief Boolean flag to track if the CAN FD bitrate switch feature should be used. */
static bool socketCanFdBrsRequested;

/** \brief List with callback functions that this driver should use. */
static volatile tCanEvents * socketCanEventsList;

/** \brief Total number of event entries into the \ref socketCanEventsList list. */
static volatile uint32_t socketCanEventsEntries;

/** \brief Flag to set in the event thread when either a bus off or bus heavy situation
 *
 */
static volatile bool socketCanErrorDetected;

/** \brief Event thread control. */
static volatile tSocketCanThreadCtrl eventThreadCtrl;

/** \brief The ID of the event thread. */
static pthread_t eventThreadId;

/** \brief CAN raw socket. */
static volatile int32_t canSocket;


/***********************************************************************************//**
** \brief     Obtains a pointer to the CAN interface structure, so that it can be linked
**            to the generic CAN driver module.
** \return    Pointer to CAN interface structure.
**
****************************************************************************************/
tCanInterface const * SocketCanGetInterface(void)
{
  return &socketCanInterface;
} /*** end of SocketCanGetInterface ***/


/************************************************************************************//**
** \brief     Initializes the CAN interface. Note that this module assumes that the CAN
**            device was already properly configured and brought online on the Linux
**            system. Terminal command "ip addr" can be used to verify this.
** \param     settings Pointer to the CAN interface settings.
**
****************************************************************************************/
static void SocketCanInit(tCanSettings const * settings)
{
  char * canDeviceName;

  /* Initialize locals. */
  socketCanEventsList = NULL;
  socketCanEventsEntries = 0;
  socketCanErrorDetected = false;
  socketCanFdModeRequested = false;
  socketCanFdBrsRequested = false;
  /* Reset CAN interface settings. */
  socketCanSettings.devicename = "";
  socketCanSettings.channel = 0;
  socketCanSettings.baudrate = CAN_BR500K;
  socketCanSettings.code = 0x00000000u;
  socketCanSettings.mask = 0x00000000u;
  socketCanSettings.brsbaudrate = CANFD_DISABLED;

  /* Initialize the critical section module. */
  UtilCriticalSectionInit();

  /* Check parameters. */
  assert(settings != NULL);

  /* Only continue with valid parameters. */
  if (settings != NULL) /*lint !e774 */
  {
    /* Shallow copy the CAN interface settings for later usage. */
    socketCanSettings = *settings;
    /* The devicename is a pointer and it is not gauranteed that it stays valid so we
     * need to deep copy this one. note the +1 for '\0' in malloc.
     */
    assert(settings->devicename != NULL);
    if (settings->devicename != NULL) /*lint !e774 */
    {
      canDeviceName = malloc(strlen(settings->devicename) + 1);
      assert(canDeviceName != NULL);
      if (canDeviceName != NULL) /*lint !e774 */
      {
        strcpy(canDeviceName, settings->devicename);
        socketCanSettings.devicename = canDeviceName;
      }
    }
  }
} /*** end of SocketCanInit ***/


/************************************************************************************//**
** \brief     Terminates the CAN interface.
**
****************************************************************************************/
static void SocketCanTerminate(void)
{
  /* Release memory that was allocated for storing the device name. */
  if (socketCanSettings.devicename != NULL)
  {
    free((char *)socketCanSettings.devicename);
  }
  /* Reset CAN interface settings. */
  socketCanSettings.devicename = "";
  socketCanSettings.channel = 0;
  socketCanSettings.baudrate = CAN_BR500K;
  socketCanSettings.code = 0x00000000u;
  socketCanSettings.mask = 0x00000000u;
  socketCanSettings.brsbaudrate = CANFD_DISABLED;
  /* Release memory that was allocated for CAN events and reset the entry count. */
  if ( (socketCanEventsList != NULL) && (socketCanEventsEntries != 0) )
  {
    free((void *)socketCanEventsList);
    socketCanEventsEntries = 0;
  }
  /* Terminate the critical section module. */
  UtilCriticalSectionTerminate();
} /*** end of SocketCanTerminate ***/


/************************************************************************************//**
** \brief     Connects the CAN interface. Note that the channel and baudrate settings
**            are ignored for the SocketCAN, because these are expected to be configured
**            when the CAN device was brought online on the Linux system.
** \return    True if connected, false otherwise.
**
****************************************************************************************/
static bool SocketCanConnect(void)
{
  bool result = false;
  struct sockaddr_can addr;
  struct ifreq ifr;
  int32_t flags;
  struct can_filter rxFilter;
  can_err_mask_t errMask;

  /* Reset CAN FD related flags. */
  socketCanFdModeRequested = false;
  socketCanFdBrsRequested = false;

  /* Set flags to determine if and which CAN FD features are requested. */
  if (socketCanSettings.brsbaudrate != CANFD_DISABLED)
  {
    /* CAN FD mode requested. */
    socketCanFdModeRequested = true;
    /* When the bitrate switch feature is requested, the bitrate switch baudrate value
     * should be higher than the nominal baudrate.
     */
    if (CanConvertFdBaudrate(socketCanSettings.brsbaudrate) > CanConvertBaudrate(socketCanSettings.baudrate))
    {
      /* CAN FD bitrate switch requested. */
      socketCanFdBrsRequested = true;
    }
  }

  /* Reset the error flag. */
  socketCanErrorDetected = false;

  /* Check settings. */
  assert(socketCanSettings.devicename != NULL);

  /* Only continue with valid settings. */
  if (socketCanSettings.devicename != NULL) /*lint !e774 */
  {
    /* Set positive result at this point and negate upon error detected. */
    result = true;

    /* Create an ifreq structure for passing data in and out of ioctl. */
    strncpy(ifr.ifr_name, socketCanSettings.devicename, IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';

    /* Get open socket descriptor */
    if ((canSocket = socket(PF_CAN, (int)SOCK_RAW, CAN_RAW)) < 0)
    {
      result = false;
    }

    if (result)
    {
      /* CAN FD mode requested? */
      if (socketCanFdModeRequested)
      {
        /* Determine if the CAN device is configured for CAN classic or CAN FD mode. Do so
         * by reading the MTU size of the CAN device. For CAN classic it will be CAN_MTU.
         * For CAN FD it will be CANFD_MTU. Assume that CAN FD is not supported by default.
         */
        result = false;
        /* Attempt to read the MTU value from the CAN device. */
        if (ioctl(canSocket, SIOCGIFMTU, &ifr) >= 0)
        {
          /* Is the CAN device configured for CAN FD? */
          if (ifr.ifr_mtu == (int)CANFD_MTU)
          {
            /* CAN device configured for CAN FD. Attempt to switch the socket into CAN FD
             * mode.
             */
            int enable_canfd = 1;
            if (setsockopt(canSocket, SOL_CAN_RAW, CAN_RAW_FD_FRAMES,
                           &enable_canfd, sizeof(enable_canfd)) == 0)
            {
              /* Successfully switched to CAN FD mode. Update the result accordingly. */
              result = true;
            }
          }
        }
      }
    }

    if (result)
    {
      /* Obtain interface index. */
      if (ioctl(canSocket, SIOCGIFINDEX, &ifr) < 0)
      {
        close(canSocket);
        result = false;
      }
    }

    if (result)
    {
      /* Configure socket to work in non-blocking mode. */
      flags = fcntl(canSocket, F_GETFL, 0);
      if (flags == -1)
      {
        flags = 0;
      }
      if (fcntl(canSocket, F_SETFL, flags | O_NONBLOCK) == -1)
      {
        close(canSocket);
        result = false;
      }
    }

    if (result)
    {
      /* Set the address info. */
      addr.can_family = AF_CAN;
      addr.can_ifindex = ifr.ifr_ifindex;

      /* Bind the socket. */
      if (bind(canSocket, (struct sockaddr *)&addr, sizeof(addr)) < 0) /*lint !e740 */
      {
        close(canSocket);
        result = false;
      }
    }

    /* Configure reception acceptance filter, if it is not supposed to be fully open. */
    if ( (result) && (socketCanSettings.mask != 0x00000000u) )
    {
      /* Configure reception acceptance filter. The CAN_MSG_EXT_ID_MASK-bit from
       * the CAN driver is currently the same as the CAN_EFF_FLAG-bit from SocketCAN.
       * However, we don't know if this gets changed in the future in SocketCAN so
       * process this bit manually.
       */
      rxFilter.can_id = socketCanSettings.code;
      if ((rxFilter.can_id & CAN_MSG_EXT_ID_MASK) != 0)
      {
        rxFilter.can_id &= ~CAN_MSG_EXT_ID_MASK;
        rxFilter.can_id |= CAN_EFF_FLAG;
      }
      rxFilter.can_mask = socketCanSettings.mask;
      if ((rxFilter.can_mask & CAN_MSG_EXT_ID_MASK) != 0)
      {
        rxFilter.can_mask &= ~CAN_MSG_EXT_ID_MASK;
        rxFilter.can_mask |= CAN_EFF_FLAG;
      }
      if (setsockopt(canSocket, SOL_CAN_RAW, CAN_RAW_FILTER, &rxFilter, sizeof(rxFilter)) != 0)
      {
        close(canSocket);
        result = false;
      }
    }

    /* Configure reception of bus off and other CAN controller status related events. */
    if (result)
    {
      errMask = (CAN_ERR_BUSOFF | CAN_ERR_CRTL);
      if (setsockopt(canSocket, SOL_CAN_RAW, CAN_RAW_ERR_FILTER, &errMask, sizeof(errMask)) != 0)
      {
        close(canSocket);
        result = false;
      }
    }

    if (result)
    {
      /* Start the event thread. */
      if (!SocketCanStartEventThread())
      {
        close(canSocket);
        result = false;
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of SocketCanConnect ***/


/************************************************************************************//**
** \brief     Disconnects the CAN interface.
**
****************************************************************************************/
static void SocketCanDisconnect(void)
{
  /* Stop the event thread. */
  SocketCanStopEventThread();
  /* Close the socket. */
  close(canSocket);
  /* Reset the error flag. */
  socketCanErrorDetected = false;
} /*** end of SocketCanDisconnect ***/


/************************************************************************************//**
** \brief     Submits a message for transmission on the CAN bus.
** \param     msg Pointer to CAN message structure.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool SocketCanTransmit(tCanMsg const * msg)
{
  bool result = false;
  struct canfd_frame canTxFrame;
  tCanEvents volatile const * pEvents;
  uint8_t frameLenMax;
  size_t frameSizeMax;

  /* Check parameters. */
  assert(msg != NULL);

  /* Only continue with valid parameters. */
  if (msg != NULL) /*lint !e774 */
  {
    /* Construct the message frame. */
    canTxFrame.can_id = msg->id;
    /* Reset the flags. */
    canTxFrame.flags = 0;
    if ((msg->id & CAN_MSG_EXT_ID_MASK) != 0)
    {
      canTxFrame.can_id &= ~CAN_MSG_EXT_ID_MASK;
      canTxFrame.can_id |= CAN_EFF_FLAG;
    }
    /* CAN classic mode? */
    if (!socketCanFdModeRequested)
    {
      /* Set frame length and size max values for CAN classic. */
      frameLenMax = CAN_MAX_DLEN;
      frameSizeMax = CAN_MTU;
    }
    /* CAN FD mode. */
    else
    {
      /* Set frame length and size max values for CAN classic. */
      frameLenMax = CANFD_MAX_DLEN;
      frameSizeMax = CANFD_MTU;
      /* Bitrate switch feature enabled? */
      if (socketCanFdBrsRequested)
      {
        canTxFrame.flags |= CANFD_BRS;
      }
    }
    /* Determine data length with out-of-bounds correction. */
    uint8_t dataLen = ((msg->len <= frameLenMax) ? msg->len : frameLenMax);
    /* Set the data length of the message. */
    canTxFrame.len = SocketCanSanitizeFrameLen(dataLen);
    for (uint8_t idx = 0; idx < dataLen; idx++)
    {
      canTxFrame.data[idx] = msg->data[idx];
    }

    /* Submit the frame for transmission. */
    if (write(canSocket, &canTxFrame, frameSizeMax) == (ssize_t)frameSizeMax)
    {
      /* Update result value to success. */
      result = true;
      /* Trigger transmit complete event(s). */
      pEvents = socketCanEventsList;
      for (uint32_t idx = 0; idx < socketCanEventsEntries; idx++)
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
} /*** end of SocketCanTransmit ***/


/************************************************************************************//**
** \brief     Checks if a bus off or bus heavy situation occurred.
** \return    True if a bus error situation was detected, false otherwise.
**
****************************************************************************************/
static bool SocketCanIsBusError(void)
{
  bool result= false;

  /* Read flag to detect bus off or bus heavy. */
  UtilCriticalSectionEnter();
  if (socketCanErrorDetected)
  {
    /* Update result. */
    result = true;
    /* Reset the error flag. */
    socketCanErrorDetected = false;
  }
  UtilCriticalSectionExit();

  /* Give the result back to the caller. */
  return result;
} /*** end of SocketCanIsBusError ***/


/************************************************************************************//**
** \brief     Registers the event callback functions that should be called by the CAN
**            interface.
** \param     events Pointer to structure with event callback function pointers.
**
****************************************************************************************/
static void SocketCanRegisterEvents(tCanEvents const * events)
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
    socketCanEventsList = realloc((void *)socketCanEventsList,
                                  (sizeof(tCanEvents) * (socketCanEventsEntries + 1)));
    /* Assert reallocation. */
    assert(socketCanEventsList != NULL);
    /* Only continue if reallocation was successful. */
    if (socketCanEventsList != NULL)
    {
      /* Increment events entry count. */
      socketCanEventsEntries++;
      /* Store the events in the new entry. */
      socketCanEventsList[socketCanEventsEntries - 1] = *events;
    }
    /* Reallocation failed. */
    else
    {
      /* Reset events entry count. */
      socketCanEventsEntries = 0;
    }
  }
} /*** end of SocketCanRegisterEvents ***/


/************************************************************************************//**
** \brief     Starts the event thread.
** \return    True if the thread was successfully started, false otherwise.
**
****************************************************************************************/
static bool SocketCanStartEventThread(void)
{
  bool result = false;

  /* Prepare thread parameters. */
  UtilCriticalSectionEnter();
  eventThreadCtrl.terminate = false;
  eventThreadCtrl.terminated = false;
  UtilCriticalSectionExit();
  /* Create and start the event thread */
  if (pthread_create(&eventThreadId, NULL, &SocketCanEventThread, NULL) == 0) /*lint !e546 */
  {
    result = true;
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of SocketCanStartEventThread ***/


/************************************************************************************//**
** \brief     Stops the event thread. It sets the termination request and then waits for
**            the termination handshake.
** \return    None.
**
****************************************************************************************/
static void SocketCanStopEventThread(void)
{
  bool terminatedCpy = false;

  /* Set the terminate request. */
  UtilCriticalSectionEnter();
  eventThreadCtrl.terminate = true;
  UtilCriticalSectionExit();
  /* Wait for thread to terminate. */
  while (!terminatedCpy)
  {
    UtilCriticalSectionEnter();
    terminatedCpy = eventThreadCtrl.terminated;
    UtilCriticalSectionExit();
    /* Wait a little without starving the CPU. */
    UtilTimeDelayMs(10);
  }
} /*** end of SocketCanStopEventThread ***/


/************************************************************************************//**
** \brief     Event thread that handles the asynchronous reception of data from the CAN
**            interface.
** \param     arg Pointer to thread parameters.
** \return    Thread return value. Not used in this case, so always set to NULL.
**
****************************************************************************************/
static void *SocketCanEventThread(void *param)
{
  /* TODO ##Vg Update this for CAN FD support. */
  bool terminateRequest = false;
  struct canfd_frame canRxFrame;
  tCanMsg rxMsg;
  ssize_t frameSize = 0;
  tCanEvents volatile const * pEvents;

  /* Unused parameter. */
  (void)param;

  /* Run thread as long as it is not requested to terminate. */
  while (!terminateRequest)
  {
    /* Update termination request. */
    UtilCriticalSectionEnter();
    terminateRequest = eventThreadCtrl.terminate;
    UtilCriticalSectionExit();

    /* Check if CAN frames were received. */
    do
    {
      /* Attempt to read the next frame from the queue. */
      frameSize = read(canSocket, &canRxFrame, CANFD_MTU);
      /* CAN FD or CAN classic frames are the only valid ones. */
      if ( (frameSize == (ssize_t)CANFD_MTU) || (frameSize == (ssize_t)CAN_MTU) )
      {
        /* Ignore remote frames */
        if (canRxFrame.can_id & CAN_RTR_FLAG)
        {
          continue;
        }
        /* Does the message contain error information? */
        else if (canRxFrame.can_id & CAN_ERR_FLAG)
        {
          /* Was it a bus off event? */
          if ((canRxFrame.can_id & CAN_ERR_BUSOFF) != 0)
          {
            /* Set the error flag. */
            UtilCriticalSectionEnter();
            socketCanErrorDetected = true;
            UtilCriticalSectionExit();
          }
          /* Was it a CAN controller event? */
          else if ((canRxFrame.can_id & CAN_ERR_CRTL) != 0)
          {
            /* Is the controller in error passive mode (bus heavy)? */
            if ((canRxFrame.data[1] & (CAN_ERR_CRTL_RX_PASSIVE | CAN_ERR_CRTL_TX_PASSIVE)) != 0)
            {
              /* Set the error flag. */
              UtilCriticalSectionEnter();
              socketCanErrorDetected = true;
              UtilCriticalSectionExit();
            }
          }
        }
        /* It was a regular CAN message with either 11- or 29-bit identifier. */
        else
        {
          /* Copy it to the CAN message object */
          rxMsg.id = canRxFrame.can_id;
          if (rxMsg.id & CAN_EFF_FLAG)
          {
            rxMsg.id &= ~CAN_EFF_FLAG;
            rxMsg.id |= CAN_MSG_EXT_ID_MASK;
          }
          rxMsg.len = canRxFrame.len;
          for (uint8_t idx = 0; idx < rxMsg.len; idx++)
          {
            rxMsg.data[idx] = canRxFrame.data[idx];
          }

          /* Trigger message reception event(s). */
          pEvents = socketCanEventsList;
          for (uint32_t idx = 0; idx < socketCanEventsEntries; idx++)
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
    while ((frameSize == (ssize_t)CANFD_MTU) || (frameSize == (ssize_t)CAN_MTU));
    /* Wait a little to not starve the CPU, but not too long to prevent interference with
     * data throughput.
     */
    (void)usleep(10u);
  }

  /* handshake */
  UtilCriticalSectionEnter();
  eventThreadCtrl.terminated = true;
  UtilCriticalSectionExit();

  /* exit the thread */
  return NULL;
} /*** end of SocketCanEventThread ***/


/************************************************************************************//**
** \brief     Helper function to sanitize the CAN frame length, specifically for CAN FD.
**            On CAN FD, the frame lengths can be: 0..8, 12, 16, 20, 24, 32, 48, 64.
**            This means that if a frame length of 14 is specified, it should be rounded
**            up to the next supported frame length value, 16 in this case.
** \param     len Unsanitized frame length. 0..64.
** \return    Sanitized frame length in the range 0..8, 12, 16, 20, 24, 32, 48, 64.
**
****************************************************************************************/
static uint8_t SocketCanSanitizeFrameLen(uint8_t len)
{
  uint8_t result;
  uint8_t frame_len;
  uint8_t frame_dlc;
  static const uint8_t len2dlc[] =
  {  0,  1,  2,  3,  4,  5,  6,  7,  8,    /*  0 -  8 */
     9,  9,  9,  9,                        /*  9 - 12 */
    10, 10, 10, 10,                        /* 13 - 16 */
    11, 11, 11, 11,                        /* 17 - 20 */
    12, 12, 12, 12,                        /* 21 - 24 */
    13, 13, 13, 13, 13, 13, 13, 13,        /* 25 - 32 */
    14, 14, 14, 14, 14, 14, 14, 14,        /* 33 - 40 */
    14, 14, 14, 14, 14, 14, 14, 14,        /* 41 - 48 */
    15, 15, 15, 15, 15, 15, 15, 15,        /* 49 - 56 */
    15, 15, 15, 15, 15, 15, 15, 15         /* 57 - 64 */
  };
  static const uint8_t dlc2len[] =
  {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64
  };

  /* Make sure the specified len parameter is valid. If not, correct it. */
  frame_len = (len > CANFD_MAX_DLEN) ? CANFD_MAX_DLEN : len;
  /* Convert the lenght value to the CAN FD dlc value (0..15). */
  frame_dlc = len2dlc[frame_len];
  /* Convert the CAN FD dlc value to its representive frame length value. */
  result = dlc2len[frame_dlc];

  /* Give the result back to the caller. */
  return result;
} /*** end of SocketCanSanitizeFrameLen ***/


/*********************************** end of socketcan.c ********************************/

