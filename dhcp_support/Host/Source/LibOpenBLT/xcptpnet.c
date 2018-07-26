/************************************************************************************//**
* \file         xcptpnet.c
* \brief        XCP TCP/IP transport layer source file.
* \ingroup      XcpTpNet
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
#include "session.h"                        /* Communication session module            */
#include "xcploader.h"                      /* XCP loader module                       */
#include "xcptpnet.h"                       /* XCP TCP/IP transport layer              */
#include "netaccess.h"                      /* TCP/IP network access module            */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void XcpTpNetInit(void const * settings);
static void XcpTpNetTerminate(void);
static bool XcpTpNetConnect(void);
static void XcpTpNetDisconnect(void);
static bool XcpTpNetSendPacket(tXcpTransportPacket const * txPacket,
                               tXcpTransportPacket * rxPacket, uint16_t timeout);

 
/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief XCP transport layer structure filled with TCP/IP specifics. */
static const tXcpTransport netTransport =
{
  XcpTpNetInit,
  XcpTpNetTerminate,
  XcpTpNetConnect,
  XcpTpNetDisconnect,
  XcpTpNetSendPacket
};


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief The settings to use in this transport layer. */
static tXcpTpNetSettings tpNetSettings;

/** \brief Command receive object (CRO) counter. This counter starts at 1 with each new
 *         connection and is sent with each command packet. The counter gets incremented
 *         for each command packet, allowing the server to determine the correct order
 *         for the received commands.
 */
static uint32_t tpNetCroCounter;


/***********************************************************************************//**
** \brief     Obtains a pointer to the transport layer structure, so that it can be 
**            linked to the XCP protocol module.
** \return    Pointer to transport layer structure.
**
****************************************************************************************/
tXcpTransport const * XcpTpNetGetTransport(void)
{
  return &netTransport;
} /*** end of XcpTpNetGetTransport ***/


/************************************************************************************//**
** \brief     Initializes the transport layer.
** \param     settings Pointer to settings structure.
** \return    None.
**
****************************************************************************************/
static void XcpTpNetInit(void const * settings)
{
  char * netAddress;

  /* Reset transport layer settings. */
  tpNetSettings.address = NULL;
  tpNetSettings.port = 0;

  /* Check parameters. */
  assert(settings != NULL);

  /* Only continue with valid parameters. */
  if (settings != NULL) /*lint !e774 */
  {
    /* Shallow copy the transport layer settings for layer usage. */
    tpNetSettings = *((tXcpTpNetSettings *)settings);
    /* The address is a pointer and it is not guaranteed that it stays valid so we need
     * to deep copy this one. note the +1 for '\0' in malloc.
     */
    assert(((tXcpTpNetSettings *)settings)->address != NULL);
    if (((tXcpTpNetSettings *)settings)->address != NULL) /*lint !e774 */
    {
      netAddress = malloc(strlen(((tXcpTpNetSettings *)settings)->address) + 1);
      assert(netAddress != NULL);
      if (netAddress != NULL) /*lint !e774 */
      {
        strcpy(netAddress, ((tXcpTpNetSettings *)settings)->address);
        tpNetSettings.address = netAddress;
      }
    }
  }
  /* Initialize the network access module. */
  NetAccessInit();
} /*** end of XcpTpNetInit ***/


/************************************************************************************//**
** \brief     Terminates the transport layer.
**
****************************************************************************************/
static void XcpTpNetTerminate(void)
{
  /* Terminate the network access module. */
  NetAccessTerminate();
  /* Release memory that was allocated for storing the network address. */
  if (tpNetSettings.address != NULL)
  {
    free((char *)tpNetSettings.address);
  }
  /* Reset transport layer settings. */
  tpNetSettings.address = NULL;
  tpNetSettings.port = 0;
} /*** end of XcpTpNetTerminate ***/


/************************************************************************************//**
** \brief     Connects to the transport layer.
** \return    True is connected, false otherwise.
**
****************************************************************************************/
static bool XcpTpNetConnect(void)
{
  bool result = false;

  /* Check transport layer settings. */
  assert(tpNetSettings.address != NULL);
  assert(tpNetSettings.port != 0);

  /* Initialize the CRO counter. */
  tpNetCroCounter = 1;

  /* Only continue if the transport layer settings are valid. */
  if ( (tpNetSettings.address != NULL) && (tpNetSettings.port != 0) ) /*lint !e774 */
  {
    /* Connect via the network access module. */
    result = NetAccessConnect(tpNetSettings.address, tpNetSettings.port);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpTpNetConnect ***/


/************************************************************************************//**
** \brief     Disconnects from the transport layer.
**
****************************************************************************************/
static void XcpTpNetDisconnect(void)
{
  /* Disconnect via the network access module. */
  NetAccessDisconnect();
} /*** end of XcpTpNetDisconnect ***/


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
static bool XcpTpNetSendPacket(tXcpTransportPacket const * txPacket,
                               tXcpTransportPacket * rxPacket, uint16_t timeout)
{
  bool result = false;
  uint16_t byteIdx;
  /* netBuffer is static to lower the stack load. +4 because the CRO counter value is
   * added at the start of the packet.
   */
  static uint8_t netBuffer[XCPLOADER_PACKET_SIZE_MAX + 4];

  /* Check parameters. */
  assert(txPacket != NULL);
  assert(rxPacket != NULL);

  /* Only continue with valid parameters. */
  if ( (txPacket != NULL) && (rxPacket != NULL) ) /*lint !e774 */
  {
    /* Set result value to okay and only change it from now on if an error occurred. */
    result = true;
    /* Prepare the XCP packet for transmission via TCP/IP. This is basically the same
     * as the XCP packet data but just the CRO counter of the packet is added to the
     * first four bytes.
     */
    netBuffer[0] = (uint8_t)tpNetCroCounter;
    netBuffer[1] = (uint8_t)(tpNetCroCounter >> 8);
    netBuffer[2] = (uint8_t)(tpNetCroCounter >> 16);
    netBuffer[3] = (uint8_t)(tpNetCroCounter >> 24);
    /* Increment the CRO counter for the next packet. */
    tpNetCroCounter++;
    /* Copy the actual packet data. */
    for (byteIdx=0; byteIdx<txPacket->len; byteIdx++)
    {
      netBuffer[byteIdx + 4] = txPacket->data[byteIdx];
    }
    /* Send the packet. */
    if (!NetAccessSend(netBuffer, txPacket->len + 4))
    {
      result = false;
    }

    /* Only continue if the packet was successfully sent. */
    uint32_t netRxLength = 0;
    if (result)
    {
      /* Reset the length of the received packet data. */
      rxPacket->len = 0;
      /* Set the maximum allowed length of the response packet. */
      netRxLength = sizeof(netBuffer)/sizeof(netBuffer[0]);
      /* Attempt to receive the response within the specified timeout. */
      if (!NetAccessReceive(netBuffer, &netRxLength, timeout))
      {
        result = false;
      }
    }
    /* Only continue if a response packet was received. */
    if (result)
    {
      /* Validate the response length. It must at least have a DTO counter (32-bits) and
       * one byte in the response data. It can also not be longer than the maximum
       * allowed size, based on the size of the buffer.
       */
      if ( (netRxLength < 5) ||
           (netRxLength > (sizeof(netBuffer)/sizeof(netBuffer[0]))) )
      {
        /* Invalid length. */
        result = false;
      }
    }
    /* Only continue if the response packet has a valid length. */
    if (result)
    {
      /* The first four bytes contain a DTO counter in which we are not really
       * interested.
       */
      rxPacket->len = (uint8_t)(netRxLength - 4);
      /* Copy the received packet data. */
      for (byteIdx=0; byteIdx<rxPacket->len; byteIdx++)
      {
        rxPacket->data[byteIdx] = netBuffer[byteIdx + 4];
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpTpNetSendPacket ***/


/*********************************** end of xcptpnet.c *********************************/
