/************************************************************************************//**
* \file         xcptpuart.c
* \brief        XCP UART transport layer source file.
* \ingroup      XcpTpUart
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
#include "xcptpuart.h"                      /* XCP UART transport layer                */
#include "util.h"                           /* Utility module                          */
#include "serialport.h"                     /* Serial port module                      */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void XcpTpUartInit(void const * settings);
static void XcpTpUartTerminate(void);
static bool XcpTpUartConnect(void);
static void XcpTpUartDisconnect(void);
static bool XcpTpUartSendPacket(tXcpTransportPacket const * txPacket, 
                                tXcpTransportPacket * rxPacket, uint16_t timeout);

 
/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief XCP transport layer structure filled with UART specifics. */
static const tXcpTransport uartTransport =
{
  XcpTpUartInit,
  XcpTpUartTerminate,
  XcpTpUartConnect,
  XcpTpUartDisconnect,
  XcpTpUartSendPacket
};


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief The settings to use in this transport layer. */
static tXcpTpUartSettings tpUartSettings;


/***********************************************************************************//**
** \brief     Obtains a pointer to the transport layer structure, so that it can be 
**            linked to the XCP protocol module.
** \return    Pointer to transport layer structure.
**
****************************************************************************************/
tXcpTransport const * XcpTpUartGetTransport(void)
{
  return &uartTransport;
} /*** end of XcpTpUartGetTransport ***/


/************************************************************************************//**
** \brief     Initializes the transport layer.
** \param     settings Pointer to settings structure.
** \return    None.
**
****************************************************************************************/
static void XcpTpUartInit(void const * settings)
{
  char * uartPortName;

  /* Reset transport layer settings. */
  tpUartSettings.portname = NULL;
  tpUartSettings.baudrate = 0;
  
  /* Check parameters. */
  assert(settings != NULL);

  /* Only continue with valid parameters. */
  if (settings != NULL) /*lint !e774 */
  {
    /* Shallow copy the transport layer settings for layer usage. */
    tpUartSettings = *((tXcpTpUartSettings *)settings);
    /* The portname is a pointer and it is not guaranteed that it stays valid so we need
     * to deep copy this one. note the +1 for '\0' in malloc.
     */
    assert(((tXcpTpUartSettings *)settings)->portname != NULL);
    if (((tXcpTpUartSettings *)settings)->portname != NULL) /*lint !e774 */
    {
      uartPortName = malloc(strlen(((tXcpTpUartSettings *)settings)->portname) + 1);
      assert(uartPortName != NULL);
      if (uartPortName != NULL) /*lint !e774 */
      {
        strcpy(uartPortName, ((tXcpTpUartSettings *)settings)->portname);
        tpUartSettings.portname = uartPortName;
      }
    }
  }
  /* Initialize the serial port. */
  SerialPortInit();
} /*** end of XcpTpUartInit ***/


/************************************************************************************//**
** \brief     Terminates the transport layer.
**
****************************************************************************************/
static void XcpTpUartTerminate(void)
{
  /* Terminate the serial port. */
  SerialPortTerminate();
  /* Release memory that was allocated for storing the port name. */
  if (tpUartSettings.portname != NULL)
  {
    free((char *)tpUartSettings.portname);
  }
  /* Reset transport layer settings. */
  tpUartSettings.portname = NULL;
  tpUartSettings.baudrate = 0;
} /*** end of XcpTpUartTerminate ***/


/************************************************************************************//**
** \brief     Connects to the transport layer.
** \return    True is connected, false otherwise.
**
****************************************************************************************/
static bool XcpTpUartConnect(void)
{
  bool result = false;
  bool baudrateSupported;
  tSerialPortBaudrate baudrate;
  
  /* Check if the specified baudrate is supported by the serial port driver. */
  baudrateSupported = (tpUartSettings.baudrate == 9600) || 
                      (tpUartSettings.baudrate == 19200) ||
                      (tpUartSettings.baudrate == 38400) ||
                      (tpUartSettings.baudrate == 57600) ||
                      (tpUartSettings.baudrate == 115200);
                      
  /* Check transport layer settings. */
  assert(tpUartSettings.portname != NULL);
  assert(baudrateSupported);
                      
  /* Only continue if the transport layer settings are valid. */
  if ( (tpUartSettings.portname != NULL) && (baudrateSupported) ) /*lint !e774 */
  {
    /* Convert the numeric baudrate to the one supported by the serial port driver. */
    switch (tpUartSettings.baudrate) 
    {
      case 115200:
        baudrate = SERIALPORT_BR115200;
        break;
      case 57600:
        baudrate = SERIALPORT_BR57600;
        break;
      case 38400:
        baudrate = SERIALPORT_BR38400;
        break;
      case 19200:
        baudrate = SERIALPORT_BR19200;
        break;
      case 9600:
      default:
        baudrate = SERIALPORT_BR9600;
        break;
    }
    /* Connect to the serial port. */
    result = SerialPortOpen(tpUartSettings.portname, baudrate);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpTpUartConnect ***/


/************************************************************************************//**
** \brief     Disconnects from the transport layer.
**
****************************************************************************************/
static void XcpTpUartDisconnect(void)
{
  /* Disconnect from the serial port. */
  SerialPortClose();
} /*** end of XcpTpUartDisconnect ***/


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
static bool XcpTpUartSendPacket(tXcpTransportPacket const * txPacket, 
                                tXcpTransportPacket * rxPacket, uint16_t timeout)
{
  bool result = false;
  uint16_t byteIdx;
  /* uartBuffer is static to lower the stack load. +1 because the first byte for an XCP 
   * packet on the UART transport layer contains the packet length.
   */
  static uint8_t uartBuffer[XCPLOADER_PACKET_SIZE_MAX + 1]; 
  uint32_t responseTimeoutTime = 0;
  bool packetReceptionComplete;

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
    uartBuffer[0] = txPacket->len;
    for (byteIdx=0; byteIdx<txPacket->len; byteIdx++)
    {
      uartBuffer[byteIdx + 1] = txPacket->data[byteIdx];
    }
    /* Transmit the packet. */
    if (!SerialPortWrite(uartBuffer, txPacket->len + 1))
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
      /* Poll with timeout detection to receive the first byte. This one contains the
       * packet length and cannot be zero.
       */
      while (UtilTimeGetSystemTimeMs() < responseTimeoutTime)
      {
        if (SerialPortRead(&rxPacket->len, 1))
        {
          /* Length received. Validate it before accepting it */
          if (rxPacket->len > 0)
          {
            /* Start of packet received. Stop this loop to continue with the
             * reception of the packet.
             */
            break;
          }
        }
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
      
    /* Only continue with reception if a valid pacekt lenght was received. */
    if (result)
    {
      /* Continue with reception of the packet. */
      packetReceptionComplete = false;
      byteIdx = 0;
      /* Poll with timeout detection to receive the full packet. */
      while (UtilTimeGetSystemTimeMs() < responseTimeoutTime)
      {
        /* Check if the next byte was received. */
        if (SerialPortRead(&rxPacket->data[byteIdx], 1))
        {
          /* Check if the packet reception is now complete. */
          if ((byteIdx + 1) == rxPacket->len)
          {
            /* Set flag and stop the loop. */
            packetReceptionComplete = true;
            break;
          }
          /* Increment indexer to the next byte. */
          byteIdx++;
        }
      }
      /* Check if a timeout occurred. */
      if (!packetReceptionComplete)
      {
        result = false;
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpTpUartSendPacket ***/


/*********************************** end of xcptpuart.c ********************************/
