/************************************************************************************//**
* \file         xcptpuart.c
* \brief        XCP transport layer module for UART source file.
* \ingroup      SerialBoot
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
#include <stddef.h>                         /* for NULL declaration                    */
#include <stdlib.h>                         /* for standard library                    */
#include <string.h>                         /* for string library                      */
#include <assert.h>                         /* for assertions                          */
#include "xcptpuart.h"                      /* XCP transport layer for UART            */
#include "timeutil.h"                       /* for time utilities module               */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Maximum amount of data bytes that this module supports for XCP packets. */
#define XCP_TP_UART_MAX_DATA_LEN   (256)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void XcpTpUartInit(void *settings);
static void XcpTpUartDeinit(void);
static bool XcpTpUartConnect(void);
static void XcpTpUartDisconnect(void);
static bool XcpTpUartSendPacket(tXcpPacket *txPacket, tXcpPacket *rxPacket, uint16_t timeout);


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief XCP transport structure filled with UART specifics. */
static const tXcpTransport uartTransport =
{
  XcpTpUartInit,
  XcpTpUartDeinit,
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
** \brief     Obtains a pointer to the XCP UART transport structure, so that it can
**            be linked to the XCP loader module.
** \return    Pointer to XCP UART transport structure.
**
****************************************************************************************/
tXcpTransport const * const XcpTpUartGetTransport(void)
{
  return &uartTransport;
} /*** end of XcpTpUartGetTransport ***/


/************************************************************************************//**
** \brief     Initializes the transport layer.
** \param     settings Pointer to settings structure.
** \return    None.
**
****************************************************************************************/
static void XcpTpUartInit(void *settings)
{
  /* verify parameters */
  assert(settings != NULL);

  /* shallow copy the transport layer settings for layer usage */
  tpUartSettings = *((tXcpTpUartSettings *)settings);
  /* the portname is a pointer and it is not gauranteed that it stays valid so we need
   * to deep copy this one. note the +1 for '\0' in malloc.
   */
  tpUartSettings.portname = malloc(strlen(((tXcpTpUartSettings *)settings)->portname) + 1);
  strcpy(tpUartSettings.portname, ((tXcpTpUartSettings *)settings)->portname);
} /*** end of XcpTpUartInit ***/


/************************************************************************************//**
** \brief     Uninitializes the transport layer.
** \return    None.
**
****************************************************************************************/
static void XcpTpUartDeinit(void)
{
  /* release memory that was allocated for storing the port name */
  if (tpUartSettings.portname != NULL)
  {
    free(tpUartSettings.portname);
  }
} /*** end of XcpTpUartDeinit ***/


/************************************************************************************//**
** \brief     Connects to the transport layer.
** \return    True is connected, false otherwise.
**
****************************************************************************************/
static bool XcpTpUartConnect(void)
{
  /* connect to the serial port */
  return SerialPortOpen(tpUartSettings.portname, tpUartSettings.baudrate);
} /*** end of XcpTpUartConnect ***/


/************************************************************************************//**
** \brief     Disconnects from the transport layer.
** \return    None.
**
****************************************************************************************/
static void XcpTpUartDisconnect(void)
{
  /* disconnect from the serial port */
  SerialPortClose();
} /*** end of XcpTpUartDisconnect ***/


/************************************************************************************//**
** \brief     Transmits an XCP packet on the transport layer and attempts to receive the
**            response packet within the specified timeout.
** \return    True is successful and a response packet was received, false otherwise.
**
****************************************************************************************/
static bool XcpTpUartSendPacket(tXcpPacket *txPacket, tXcpPacket *rxPacket, uint16_t timeout)
{
  static uint8_t uartBuffer[XCP_TP_UART_MAX_DATA_LEN + 1]; /* static to lower stack load */
  uint8_t byteIdx;
  uint32_t responseTimeoutTime;
  bool packetReceptionComplete;

  /* verify parameters */
  assert(txPacket != NULL);
  assert(txPacket->len <= XCP_TP_UART_MAX_DATA_LEN);
  assert(rxPacket != NULL);
  assert(timeout > 0);


  /* prepare the XCP packet for transmission on UART. this is basically the same as the
   * xcp packet data but just the length of the packet is added to the first byte.
   */
  uartBuffer[0] = txPacket->len;
  for (byteIdx=0; byteIdx<txPacket->len; byteIdx++)
  {
    uartBuffer[byteIdx + 1] = txPacket->data[byteIdx];
  }

  /* transmit the packet */
  if (!SerialPortWrite(uartBuffer, txPacket->len + 1))
  {
    return false;
  }

  /* determine timeout time for the response packet */
  responseTimeoutTime = TimeUtilGetSystemTimeMs() + timeout;

  /* initialize packet reception length */
  uartBuffer[0] = 0;
  /* poll with timeout detection to receive the first byte. this one contains the
   * packet length and cannot be zero.
   */
  while (TimeUtilGetSystemTimeMs() < responseTimeoutTime)
  {
    if (SerialPortRead(&uartBuffer[0], 1))
    {
      /* length received. validate it before accepting it */
      if (uartBuffer[0] > 0)
      {
        /* start of packet received. stop this loop to continue with the
         * reception of the packet.
         */
         break;
      }
    }
  }
  /* check if a valid start of packet was received */
  if (uartBuffer[0] == 0)
  {
    /* no valid start of packet received, so a timeout occurred. */
    return false;
  }

  /* continue with reception of the packet */
  packetReceptionComplete = false;
  byteIdx = 1;
  /* poll with timeout detection to receive the full packet */
  while (TimeUtilGetSystemTimeMs() < responseTimeoutTime)
  {
    /* check if the next byte was received */
    if (SerialPortRead(&uartBuffer[byteIdx], 1))
    {
      /* check if the packet reception is now complete */
      if (byteIdx == uartBuffer[0])
      {
        /* set flag and stop the loop */
        packetReceptionComplete = true;
        break;
      }
      /* increment indexer to the next byte */
      byteIdx++;
    }
  }

  /* check if a timeout occurred */
  if (!packetReceptionComplete)
  {
    return false;
  }

  /* still here so a full packet was received. copy its contents except the length info
   * which is stored in the first byte
   */
  for (byteIdx=0; byteIdx<uartBuffer[0]; byteIdx++)
  {
    rxPacket->data[byteIdx] = uartBuffer[byteIdx + 1];
  }
  rxPacket->len = uartBuffer[0];

  return true;
} /*** end of XcpTpUartSendPacket ***/


/*********************************** end of xcptpuart.c ********************************/

