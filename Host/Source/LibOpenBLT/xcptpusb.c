/************************************************************************************//**
* \file         xcptpusb.c
* \brief        XCP USB transport layer source file.
* \ingroup      XcpTpUsb
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
#include "xcptpusb.h"                       /* XCP USB transport layer                 */
#include "util.h"                           /* Utility module                          */
#include "usbbulk.h"                        /* USB bulk driver                         */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void XcpTpUsbInit(void const * settings);
static void XcpTpUsbTerminate(void);
static bool XcpTpUsbConnect(void);
static void XcpTpUsbDisconnect(void);
static bool XcpTpUsbSendPacket(tXcpTransportPacket const * txPacket, 
                               tXcpTransportPacket * rxPacket, uint16_t timeout);

 
/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief XCP transport layer structure filled with USB specifics. */
static const tXcpTransport usbTransport =
{
  XcpTpUsbInit,
  XcpTpUsbTerminate,
  XcpTpUsbConnect,
  XcpTpUsbDisconnect,
  XcpTpUsbSendPacket
};


/***********************************************************************************//**
** \brief     Obtains a pointer to the transport layer structure, so that it can be 
**            linked to the XCP protocol module.
** \return    Pointer to transport layer structure.
**
****************************************************************************************/
tXcpTransport const * XcpTpUsbGetTransport(void)
{
  return &usbTransport;
} /*** end of XcpTpUsbGetTransport ***/


/************************************************************************************//**
** \brief     Initializes the transport layer.
** \param     settings Pointer to settings structure.
** \return    None.
**
****************************************************************************************/
static void XcpTpUsbInit(void const * settings)
{
  /* No settings needed for this transport layer. */
  (void)settings;

  /* Initialize the USB bulk driver. */
  UsbBulkInit();
} /*** end of XcpTpUsbInit ***/


/************************************************************************************//**
** \brief     Terminates the transport layer.
**
****************************************************************************************/
static void XcpTpUsbTerminate(void)
{
  /* Terminate the USB bulk driver. */
  UsbBulkTerminate();
} /*** end of XcpTpUsbTerminate ***/


/************************************************************************************//**
** \brief     Connects to the transport layer.
** \return    True is connected, false otherwise.
**
****************************************************************************************/
static bool XcpTpUsbConnect(void)
{
  bool result;
                      
  /* Connect to USB device via USB bulk driver. */
  result = UsbBulkOpen();
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpTpUsbConnect ***/


/************************************************************************************//**
** \brief     Disconnects from the transport layer.
**
****************************************************************************************/
static void XcpTpUsbDisconnect(void)
{
  /* Disconnect from USB device via USB bulk driver. */
  UsbBulkClose();
} /*** end of XcpTpUsbDisconnect ***/


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
static bool XcpTpUsbSendPacket(tXcpTransportPacket const * txPacket, 
                               tXcpTransportPacket * rxPacket, uint16_t timeout)
{
  bool result = false;
  uint16_t byteIdx;
  /* usbBuffer is static to lower the stack load. +1 because the first byte for an XCP 
   * packet on the USB transport layer contains the packet lenght. 
   */
  static uint8_t usbBuffer[XCPLOADER_PACKET_SIZE_MAX + 1]; 
  uint32_t responseTimeoutTime = 0;

  /* Check parameters. */
  assert(txPacket != NULL);
  assert(rxPacket != NULL);

  /* Only continue with valid parameters. */
  if ( (txPacket != NULL) && (rxPacket != NULL) ) /*lint !e774 */
  {
    /* Set result value to okay and only change it from now on if an error occurred. */
    result = true;
    /* Prepare the XCP packet for transmission on UART. This is basically the same as 
     * the XCP packet data but just the length of the packet is added to the first 
     * byte.
     */
    usbBuffer[0] = txPacket->len;
    for (byteIdx=0; byteIdx<txPacket->len; byteIdx++)
    {
      usbBuffer[byteIdx + 1] = txPacket->data[byteIdx];
    }
    /*  Transmit the packet via the USB bulk driver. */
    if (!UsbBulkWrite(usbBuffer, txPacket->len + 1))
    {
      result = false;
    }
    
    /* Only continue if the transmission was successful. */
    if (result)
    {
      /* Determine timeout time for the response packet. */
      responseTimeoutTime = UtilTimeGetSystemTimeMs() + timeout;
      /* Initialize packet reception length. */
      rxPacket->len = 0;
      /* Receive the first byte. This one contains the packet length and cannot be
       * zero. 
       */
      if (!UsbBulkRead(&rxPacket->len, 1, timeout))
      {
        result = false;
      }
      /* Check if a valid start of packet was received, in which case the first 
       * byte won't have a zero value.
       */
      if (rxPacket->len == 0)
      {
        /* No valid start of packet received, so a timeout occurred. */
        result = false;
      }
    }
      
    /* Only continue with reception if a valid packet length was received. */
    if (result)
    {
      /* Check if there is still time available before the initial timeout. */
      uint32_t currentTime = UtilTimeGetSystemTimeMs();
      if (currentTime >= responseTimeoutTime)
      {
        result = false;
      }
      else
      {
        /* Receive the actual packet data. */
        if (!UsbBulkRead(&rxPacket->data[0], rxPacket->len, responseTimeoutTime - currentTime))
        {
          result = false;
        }
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpTpUsbSendPacket ***/


#endif // NO_CAN_OR_USB
/*********************************** end of xcptpusb.c *********************************/
