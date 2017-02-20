/************************************************************************************//**
* \file         xcploader.h
* \brief        XCP Loader module header file.
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
#ifndef XCPLOADER_H
#define XCPLOADER_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Include files
****************************************************************************************/
#include <stdint.h>                         /* for standard integer types              */
#include <stdbool.h>                        /* for boolean type                        */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Total number of bytes in a master<->slave data packet. It should be at least
 *         equal or larger than that configured on the slave.
 */
#define XCPLOADER_PACKET_SIZE_MAX   (255)


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief XCP protocol specific settings. */
typedef struct t_xcp_settings
{
  uint16_t timeoutT1;            /**< Command response timeout in milliseconds.        */
  uint16_t timeoutT3;            /**< Start programming timeout in milliseconds.       */
  uint16_t timeoutT4;            /**< Erase memory timeout in milliseonds.             */
  uint16_t timeoutT5;            /**< Program memory and reset timeout in milliseonds. */
  uint16_t timeoutT7;            /**< Busy wait timer timeout in milliseonds.          */
} tXcpSettings;


/** \brief XCP packet type. */
typedef struct t_xcp_packet
{
  uint8_t data[XCPLOADER_PACKET_SIZE_MAX];       /**< Packet data.                     */
  uint8_t len;                                   /**< Packet length.                   */
} tXcpPacket;


/** \brief XCP transport layer. */
typedef struct t_xcp_transport
{
  /** \brief Initialization of the XCP transpor layer. */
  void (*Init) (void *settings);
  /** \brief Uninitializes the XCP transpor layer. */
  void (*Deinit) (void);
  /** \brief Connects the XCP transpor layer. */
  bool (*Connect) (void);
  /** \brief Disconnects the XCP transpor layer. */
  void (*Disconnect) (void);
  /** \brief Sends an XCP packet and waits for the response to come back. */
  bool (*SendPacket) (tXcpPacket *txPacket, tXcpPacket *rxPacket, uint16_t timeout);
} tXcpTransport;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void XcpLoaderInit(tXcpSettings *settings, tXcpTransport const * const transport, void *tpsettings);
void XcpLoaderDeinit(void);
bool XcpLoaderConnect(void);
void XcpLoaderDisconnect(void);
bool XcpLoaderStartProgrammingSession(void);
bool XcpLoaderStopProgrammingSession(void);
bool XcpLoaderClearMemory(uint32_t addr, uint32_t len);
bool XcpLoaderReadData(uint32_t addr, uint32_t len, uint8_t *data);
bool XcpLoaderProgramData(uint32_t addr, uint32_t len, uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif /* XCPLOADER_H */
/********************************* end of xcploader.h **********************************/

