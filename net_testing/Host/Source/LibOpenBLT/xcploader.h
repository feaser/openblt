/************************************************************************************//**
* \file         xcploader.h
* \brief        XCP Loader module header file.
* \ingroup      XcpLoader
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
/************************************************************************************//**
* \defgroup   XcpLoader XCP version 1.0 protocol
* \brief      This module implements the XCP communication protocol that can be linked 
*             to the Session module. 
* \ingroup    Session
* \details
* This XCP Loader module contains functionality according to the standardized XCP 
* protocol version 1.0. XCP is a universal measurement and calibration communication 
* protocol. Note that only those parts of the XCP master functionality are implemented 
* that are applicable to performing a firmware update on the slave. This means 
* functionality for reading, programming, and erasing (non-volatile) memory.
****************************************************************************************/
#ifndef XCPLOADER_H
#define XCPLOADER_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Total number of bytes in a master<->slave data packet. It should be at least
 *         equal or larger than that configured on the slave.
 */
#define XCPLOADER_PACKET_SIZE_MAX   (255u)
  
  
/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief XCP transport layer packet type. */
typedef struct t_xcp_transport_packet
{
  uint8_t data[XCPLOADER_PACKET_SIZE_MAX];       /**< Packet data.                     */
  uint8_t len;                                   /**< Packet length.                   */
} tXcpTransportPacket;

/** \brief XCP transport layer. */
typedef struct t_xcp_transport
{
  /** \brief Initialization of the XCP transport layer. */
  void (*Init) (void const * settings);
  /** \brief Termination the XCP transport layer. */
  void (*Terminate) (void);
  /** \brief Connects the XCP transport layer. */
  bool (*Connect) (void);
  /** \brief Disconnects the XCP transport layer. */
  void (*Disconnect) (void);
  /** \brief Sends an XCP packet and waits for the response to come back. */
  bool (*SendPacket) (tXcpTransportPacket const * txPacket, 
                      tXcpTransportPacket * rxPacket, uint16_t timeout);
} tXcpTransport;

/** \brief XCP protocol specific settings. */
typedef struct t_xcp_loader_settings
{
  /** \brief Command response timeout in milliseconds. */
  uint16_t timeoutT1;            
  /** \brief Start programming timeout in milliseconds. */
  uint16_t timeoutT3;            
  /** \brief Erase memory timeout in milliseconds. */
  uint16_t timeoutT4;            
  /** \brief Program memory and reset timeout in milliseconds. */
  uint16_t timeoutT5;            
  /** \brief Connect response timeout in milliseconds. */
  uint16_t timeoutT6;
  /** \brief Busy wait timer timeout in milliseconds. */
  uint16_t timeoutT7;     
  /** \brief Connection mode used in the XCP connect command. */
  uint8_t connectMode;
  /** \brief Seed/key algorithm library filename. */
  char const * seedKeyFile;
  /** \brief Pointer to the transport layer to use during protocol communications. */
  tXcpTransport const * transport;
  /** \brief Pointer to the settings for the transport layer. */
  void const * transportSettings;
} tXcpLoaderSettings;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
tSessionProtocol const * XcpLoaderGetProtocol(void);  

#ifdef __cplusplus
}
#endif

#endif /* XCPLOADER_H */
/*********************************** end of xcploader.h ********************************/
