/************************************************************************************//**
* \file         xcptpcan.c
* \brief        XCP CAN transport layer source file.
* \ingroup      XcpTpCan
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
#include <stdlib.h>                         /* for standard library                    */
#include <string.h>                         /* for string library                      */
#include "session.h"                        /* Communication session module            */
#include "xcploader.h"                      /* XCP loader module                       */
#include "xcptpcan.h"                       /* XCP CAN transport layer                 */
#include "util.h"                           /* Utility module                          */
#include "candriver.h"                      /* Generic CAN driver module               */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
/* Transport layer module functions. */
static void XcpTpCanInit(void const * settings);
static void XcpTpCanTerminate(void);
static bool XcpTpCanConnect(void);
static void XcpTpCanDisconnect(void);
static bool XcpTpCanSendPacket(tXcpTransportPacket const * txPacket, 
                               tXcpTransportPacket * rxPacket, uint16_t timeout);
/* CAN event functions. */
static void XcpTpCanEventMessageTransmitted(tCanMsg const * msg);
static void XcpTpCanEventMessageReceived(tCanMsg const * msg);

 
/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief XCP transport layer structure filled with CAN specifics. */
static const tXcpTransport canTransport =
{
  XcpTpCanInit,
  XcpTpCanTerminate,
  XcpTpCanConnect,
  XcpTpCanDisconnect,
  XcpTpCanSendPacket
};

/** \brief CAN driver event functions. */
static const tCanEvents canEvents =
{
  XcpTpCanEventMessageTransmitted,
  XcpTpCanEventMessageReceived
};


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief The settings to use in this transport layer. */
static tXcpTpCanSettings tpCanSettings;

/** \brief Flag to indicate that a response packet was received via CAN. Made volatile
 *         because it is shared with an event callback function that could be called from
 *         a different thread.
 */
static volatile bool tpCanResponseMessageReceived;

/** \brief Buffer for storing the CAN message with response packet data. Made volatile
 *         because it is shared with an event callback function that could be called from
 *         a different thread.
 */
static volatile tCanMsg tpCanResponseMessage;


/***********************************************************************************//**
** \brief     Obtains a pointer to the transport layer structure, so that it can be 
**            linked to the XCP protocol module.
** \return    Pointer to transport layer structure.
**
****************************************************************************************/
tXcpTransport const * XcpTpCanGetTransport(void)
{
  return &canTransport;
} /*** end of XcpTpCanGetTransport ***/


/************************************************************************************//**
** \brief     Initializes the transport layer.
** \param     settings Pointer to settings structure.
** \return    None.
**
****************************************************************************************/
static void XcpTpCanInit(void const * settings)
{
  char * canDeviceName;
  tCanSettings canSettings;

  /* Reset transport layer settings. */
  tpCanSettings.device = NULL;
  tpCanSettings.channel = 0;
  tpCanSettings.baudrate = 500000;
  tpCanSettings.transmitId = 0x667;
  tpCanSettings.receiveId = 0x7e1;
  tpCanSettings.useExtended = false;

  /* This module uses critical sections so initialize them. */
  UtilCriticalSectionInit();

  /* Check parameters. */
  assert(settings != NULL);

  /* Only continue with valid parameters. */
  if (settings != NULL) /*lint !e774 */
  {
    /* Shallow copy the transport layer settings for layer usage. */
    tpCanSettings = *((tXcpTpCanSettings *)settings);
    /* The device name is a pointer and it is not gauranteed that it stays valid so we need
     * to deep copy this one. note the +1 for '\0' in malloc.
     */
    assert(((tXcpTpCanSettings *)settings)->device != NULL);
    if (((tXcpTpCanSettings *)settings)->device != NULL) /*lint !e774 */
    {
      canDeviceName = malloc(strlen(((tXcpTpCanSettings *)settings)->device) + 1);
      assert(canDeviceName != NULL);
      if (canDeviceName != NULL) /*lint !e774 */
      {
        strcpy(canDeviceName, ((tXcpTpCanSettings *)settings)->device);
        tpCanSettings.device = canDeviceName;
      }
    }
  }

  /* Convert the transport layer settings to CAN driver settings. */
  canSettings.devicename = tpCanSettings.device;
  canSettings.channel = tpCanSettings.channel;
  switch (tpCanSettings.baudrate)
  {
    case 1000000:
      canSettings.baudrate = CAN_BR1M;
      break;
    case 800000:
      canSettings.baudrate = CAN_BR800K;
      break;
    case 500000:
      canSettings.baudrate = CAN_BR500K;
      break;
    case 250000:
      canSettings.baudrate = CAN_BR250K;
      break;
    case 125000:
      canSettings.baudrate = CAN_BR125K;
      break;
    case 100000:
      canSettings.baudrate = CAN_BR100K;
      break;
    case 50000:
      canSettings.baudrate = CAN_BR50K;
      break;
    case 20000:
      canSettings.baudrate = CAN_BR20K;
      break;
    case 10000:
      canSettings.baudrate = CAN_BR10K;
      break;
    default:
      /* Default to 500 kbits/sec in case an unsupported baudrate was specified. */
      canSettings.baudrate = CAN_BR500K;
      break;
  }
  /* Configure the reception acceptance filter to receive only one CAN identifier. */
  canSettings.code = tpCanSettings.receiveId;
  if (tpCanSettings.useExtended)
  {
    canSettings.code |= CAN_MSG_EXT_ID_MASK;
  }
  canSettings.mask = 0x9fffffff;
  /* Initialize the CAN driver. */
  CanInit(&canSettings);
  /* Register CAN event functions. */
  CanRegisterEvents(&canEvents);
} /*** end of XcpTpCanInit ***/


/************************************************************************************//**
** \brief     Terminates the transport layer.
**
****************************************************************************************/
static void XcpTpCanTerminate(void)
{
  /* Terminate the CAN driver. */
  CanTerminate();
  /* Release memory that was allocated for storing the device name. */
  if (tpCanSettings.device != NULL)
  {
    free((char *)tpCanSettings.device);
  }
  /* Reset transport layer settings. */
  tpCanSettings.device = NULL;
  tpCanSettings.channel = 0;
  tpCanSettings.baudrate = 500000;
  tpCanSettings.transmitId = 0x667;
  tpCanSettings.receiveId = 0x7e1;
  tpCanSettings.useExtended = false;
  /* This module used critical sections so terminate them. */
  UtilCriticalSectionTerminate();
} /*** end of XcpTpCanTerminate ***/


/************************************************************************************//**
** \brief     Connects to the transport layer.
** \return    True is connected, false otherwise.
**
****************************************************************************************/
static bool XcpTpCanConnect(void)
{
  bool result = false;

  /* Connect to the CAN driver. */
  if (CanConnect())
  {
    result = true;
  }
  
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpTpCanConnect ***/


/************************************************************************************//**
** \brief     Disconnects from the transport layer.
**
****************************************************************************************/
static void XcpTpCanDisconnect(void)
{
  /* Disconnect from the CAN driver. */
  CanDisconnect();
} /*** end of XcpTpCanDisconnect ***/


/************************************************************************************//**
** \brief     Transmits an XCP packet on the transport layer and attempts to receive the
**            response packet within the specified timeout.
** \param     txPacket Pointer to the packet to transmit.
** \param     rxPacket Pointer where the received packet info is stored.
** \param     timeout Maximum time in milliseconds to wait for the reception of the
**            response packet.
** \return    True is successful and a response packet was received, false otherwise.
**
****************************************************************************************/
static bool XcpTpCanSendPacket(tXcpTransportPacket const * txPacket, 
                               tXcpTransportPacket * rxPacket, uint16_t timeout)
{
  bool result = false;
  tCanMsg canMsg;
  uint32_t responseTimeoutTime = 0;
  
  /* Check parameters. */
  assert(txPacket != NULL);
  assert(rxPacket != NULL);

  /* Only continue with valid parameters. */
  if ( (txPacket != NULL) && (rxPacket != NULL) ) /*lint !e774 */
  {
    /* Only continue if data length fits in a CAN message and the CAN bus is not in error
     * state. 
     */
    if ( ((txPacket->len <= CAN_MSG_MAX_LEN)) && (!CanIsBusError()) )
    {
      /* Set result value to okay and only change it from now on if an error occurred. */
      result = true;
      /* Store the packet data into a CAN message. */
      canMsg.id = tpCanSettings.transmitId;
      if (tpCanSettings.useExtended)
      {
        canMsg.id |= CAN_MSG_EXT_ID_MASK;
      }
      canMsg.dlc = txPacket->len;
      for (uint8_t idx = 0; idx < canMsg.dlc; idx++)
      {
        canMsg.data[idx] = txPacket->data[idx];
      }
      /* Enter critical section. */
      UtilCriticalSectionEnter();
      /* Reset packet received flag before transmitting the packet, to be able to detect
       * its response packet.
       */
      tpCanResponseMessageReceived = false;
      /* Exit critical section. */
      UtilCriticalSectionExit();
      /* Submit the packet for transmission on the CAN bus. */
      if (!CanTransmit(&canMsg))
      {
        result = false;
      }
      /* Only continue if the transmission was successful. */
      if (result)
      {
        /* Determine timeout time for the response packet. */
        responseTimeoutTime = UtilTimeGetSystemTimeMs() + timeout;
        /* Poll with timeout detection to receive the response packet as a CAN message
         * on the CAN bus.
         */
        while (UtilTimeGetSystemTimeMs() < responseTimeoutTime)
        {
          /* Enter critical section. */
          UtilCriticalSectionEnter();
          /* Response received? */
          if (tpCanResponseMessageReceived)
          {
            /* Copy the response packet. */
            rxPacket->len = tpCanResponseMessage.dlc;
            for (uint8_t idx = 0; idx < rxPacket->len; idx++)
            {
              rxPacket->data[idx] = tpCanResponseMessage.data[idx];
            }
            /* Exit critical section. */
            UtilCriticalSectionExit();
            /* Response packet receive so no need to continue loop. */
            break;
          }
          /* Exit critical section. */
          UtilCriticalSectionExit();
        }
        /* Enter critical section. */
        UtilCriticalSectionEnter();
        /* Check if a timeout occurred and no response was received. */
        if (!tpCanResponseMessageReceived)
        {
          result = false;
        }
        /* Exit critical section. */
        UtilCriticalSectionExit();
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpTpCanSendPacket ***/


/************************************************************************************//**
** \brief     CAN driver event callback function that gets called each time a CAN
**            message was transmitted.
** \param     msg Pointer to the transmitted CAN message.
**
****************************************************************************************/
static void XcpTpCanEventMessageTransmitted(tCanMsg const * msg)
{
  (void)msg;

  /* Nothing needs to be done here for now. Added for possible future expansions. */
} /*** end of XcpTpCanEventMessageTransmitted ***/


/************************************************************************************//**
** \brief     CAN driver event callback function that gets called each time a CAN
**            message was received.
** \param     msg Pointer to the received CAN message.
**
****************************************************************************************/
static void XcpTpCanEventMessageReceived(tCanMsg const * msg)
{
  /* Determine CAN identifier for receiving XCP commands via CAN. */
  uint32_t tpCanRxId = tpCanSettings.receiveId;
  if (tpCanSettings.useExtended)
  {
    tpCanRxId |= CAN_MSG_EXT_ID_MASK;
  }

  /* Check if the identifier matches the one for XCP on CAN. */
  if (msg->id == tpCanRxId)
  {
    /* Enter critical section. */
    UtilCriticalSectionEnter();
    /* Copy to the packet response message buffer. */
    tpCanResponseMessage.id = msg->id;
    tpCanResponseMessage.dlc = msg->dlc;
    for (uint8_t idx = 0; idx < msg->dlc; idx++)
    {
      tpCanResponseMessage.data[idx] = msg->data[idx];
    }
    /* Set packet received flag. */
    tpCanResponseMessageReceived = true;
    /* Exit critical section. */
    UtilCriticalSectionExit();
  }
} /*** end of XcpTpCanEventMessageReceived ***/


#endif // NO_CAN_OR_USB
/*********************************** end of xcptpcan.c *********************************/
