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
#if defined(PLATFORM_WIN32)
#include "pcanusb.h"                        /* Peak PCAN-USB interface                 */
#endif


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void XcpTpCanInit(void const * settings);
static void XcpTpCanTerminate(void);
static bool XcpTpCanConnect(void);
static void XcpTpCanDisconnect(void);
static bool XcpTpCanSendPacket(tXcpTransportPacket const * txPacket, 
                               tXcpTransportPacket * rxPacket, uint16_t timeout);

 
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
  /* Check parameters. */
  assert(settings != NULL);

  /* Only continue with valid parameters. */
  if (settings != NULL) /*lint !e774 */
  {
    /* ##Vg TODO Extract and set the pointer to the CAN interface to link. */
  }
  /* ##Vg TODO Implement CAN module initialization. */
} /*** end of XcpTpCanInit ***/


/************************************************************************************//**
** \brief     Terminates the transport layer.
**
****************************************************************************************/
static void XcpTpCanTerminate(void)
{
  /* ##Vg TODO Implement. */
} /*** end of XcpTpCanTerminate ***/


/************************************************************************************//**
** \brief     Connects to the transport layer.
** \return    True is connected, false otherwise.
**
****************************************************************************************/
static bool XcpTpCanConnect(void)
{
  bool result = false;

  /* ##Vg TODO Implement. */
  
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpTpCanConnect ***/


/************************************************************************************//**
** \brief     Disconnects from the transport layer.
**
****************************************************************************************/
static void XcpTpCanDisconnect(void)
{
  /* ##Vg TODO Disconnect from the CAN bus. */
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

  /* Check parameters. */
  assert(txPacket != NULL);
  assert(rxPacket != NULL);

  /* Only continue with valid parameters. */
  if ( (txPacket != NULL) && (rxPacket != NULL) ) /*lint !e774 */
  {
    /* ##Vg TODO Implement. */
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpTpCanSendPacket ***/


/*********************************** end of xcptpcan.c *********************************/
