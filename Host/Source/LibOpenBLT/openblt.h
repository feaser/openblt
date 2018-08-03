/************************************************************************************//**
* \file         openblt.h
* \brief        OpenBLT host library header file.
* \ingroup      Library
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
* \defgroup   Library Library API
* \brief      OpenBLT Library API.
* \details
* The Library API contains the application programming interface for the OpenBLT libary.
* it defines the functions and definitions that an external program uses to access the
* library's functionality.
****************************************************************************************/
#ifndef OPENBLT_H
#define OPENBLT_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Include files
****************************************************************************************/
#include <stdint.h>                         /* for standard integer types              */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* CMake automatically defines macro openblt_shared_EXPORTS when building the shared
 * version of the library. When building under windows, this is used to set the export
 * macro, which is needed to confige the library functions.
 */
#if defined(_WIN32) || defined(_WIN64)
#if defined(openblt_shared_EXPORTS)
#define  LIBOPENBLT_EXPORT __declspec(dllexport)
#else
#define  LIBOPENBLT_EXPORT 
#endif /* openblt_shared_EXPORTS */
#else /* defined(_WIN32) || defined(_WIN64) */
#define LIBOPENBLT_EXPORT
#endif

/** \brief Function return value for when everything went okay. */
#define BLT_RESULT_OK                  (0u)

/** \brief Function return value for when a generic error occured. */
#define BLT_RESULT_ERROR_GENERIC       (1u)


/****************************************************************************************
*             V E R S I O N   I N F O R M A T I O N
****************************************************************************************/
/****************************************************************************************
* Function prototypes
****************************************************************************************/
LIBOPENBLT_EXPORT uint32_t BltVersionGetNumber(void);
LIBOPENBLT_EXPORT char const * BltVersionGetString(void);


/****************************************************************************************
*             S E S S I O N   /   T R A N S P O R T   L A Y E R S
****************************************************************************************/
/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief XCP protocol version 1.0. XCP is a universal measurement and calibration
 *         communication protocol. It contains functionality for reading, programming,
 *         and erasing (non-volatile) memory making it a good fit for bootloader
 *         purposes.
 */
#define BLT_SESSION_XCP_V10            ((uint32_t)0u)

/** \brief Transport layer for the XCP v1.0 protocol that uses RS-232 serial
 *         communication for data exchange.
 */
#define BLT_TRANSPORT_XCP_V10_RS232    ((uint32_t)0u)

/** \brief Transport layer for the XCP v1.0 protocol that uses Controller Area Network
 *         (CAN) for data exchange.
 */
#define BLT_TRANSPORT_XCP_V10_CAN      ((uint32_t)1u)

/** \brief Transport layer for the XCP v1.0 protocol that uses USB Bulk for data 
 *         exchange.
 */
#define BLT_TRANSPORT_XCP_V10_USB      ((uint32_t)2u)

/** \brief Transport layer for the XCP v1.0 protocol that uses TCP/IP for data
 *         exchange.
 */
#define BLT_TRANSPORT_XCP_V10_NET      ((uint32_t)3u)


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Structure layout of the XCP version 1.0 session settings. */
typedef struct t_blt_session_settings_xcp_v10
{
  uint16_t timeoutT1;            /**< Command response timeout in milliseconds.        */
  uint16_t timeoutT3;            /**< Start programming timeout in milliseconds.       */
  uint16_t timeoutT4;            /**< Erase memory timeout in milliseconds.            */
  uint16_t timeoutT5;            /**< Program memory and reset timeout in milliseconds.*/
  uint16_t timeoutT6;            /**< Connect response timeout in milliseconds.        */
  uint16_t timeoutT7;            /**< Busy wait timer timeout in milliseonds.          */
  char const * seedKeyFile;      /**< Seed/key algorithm library filename.             */
  uint8_t connectMode;           /**< Connection mode parameter in XCP connect command.*/
} tBltSessionSettingsXcpV10;

/** \brief Structure layout of the XCP version 1.0 RS232 transport layer settings. The
 *         portName field is platform dependent. On Linux based systems this should be
 *         the filename of the tty-device, such as "/dev/tty0". On Windows based systems
 *         it should be the name of the COM-port, such as "COM1".
 */
typedef struct t_blt_transport_settings_xcp_v10_rs232
{
  char const * portName;         /**< Communication port name such as /dev/tty0.       */
  uint32_t baudrate;             /**< Communication speed in bits/sec.                 */
} tBltTransportSettingsXcpV10Rs232;

/** \brief Structure layout of the XCP version 1.0 CAN transport layer settings. The
 *         deviceName field is platform dependent. On Linux based systems this should be
 *         the socketCAN interface name such as "can0". The terminal command "ip addr"
 *         can be issued to view a list of interfaces that are up and available. Under
 *         Linux it is assumed that the socketCAN interface is already configured on the
 *         system, before using the OpenBLT library. When baudrate is configured when
 *         bringing up the system, so the baudrate field in this structure is don't care
 *         when using the library on a Linux was system.
 *         On Windows based systems, the device name is a name that is pre-defined by
 *         this library for the supported CAN adapters. The device name should be one of
 *         the following: "peak_pcanusb", "kvaser_leaflight", or "lawicel_canusb".
 *         Field use extended is a boolean field. When set to 0, the specified transmitId
 *         and receiveId are assumed to be 11-bit standard CAN identifier. It the field
 *         is 1, these identifiers are assumed to be 29-bit extended CAN identifiers.
 */
typedef struct t_blt_transport_settings_xcp_v10_can
{
  char const * deviceName;       /**< Device name such as can0, peak_pcanusb etc.      */
  uint32_t deviceChannel;        /**< Channel on the device to use.                    */
  uint32_t baudrate;             /**< Communication speed in bits/sec.                 */
  uint32_t transmitId;           /**< Transmit CAN identifier.                         */
  uint32_t receiveId;            /**< Receive CAN identifier.                          */
  uint32_t useExtended;          /**< Boolean to configure 29-bit CAN identifiers.     */
} tBltTransportSettingsXcpV10Can;

/** \brief Structure layout of the XCP version 1.0 NET transport layer settings. The
 *         address field can be set to either the IP address or the hostname, such as
 *         "192.168.178.23" or "mymicro.mydomain.com". The port should be set to the 
 *         TCP port number that the bootloader target listens on.
 */
typedef struct t_blt_transport_settings_xcp_v10_net
{
  char const * address;          /**< Target IP-address or hostname on the network.    */
  uint16_t port;                 /**< TCP port to use.                                 */
} tBltTransportSettingsXcpV10Net;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
LIBOPENBLT_EXPORT void BltSessionInit(uint32_t sessionType, 
                                      void const * sessionSettings,
                                      uint32_t transportType, 
                                      void const * transportSettings);
LIBOPENBLT_EXPORT void BltSessionTerminate(void);
LIBOPENBLT_EXPORT uint32_t BltSessionStart(void);
LIBOPENBLT_EXPORT void BltSessionStop(void);
LIBOPENBLT_EXPORT uint32_t BltSessionClearMemory(uint32_t address, uint32_t len);
LIBOPENBLT_EXPORT uint32_t BltSessionWriteData(uint32_t address, uint32_t len, 
                                               uint8_t const * data);
LIBOPENBLT_EXPORT uint32_t BltSessionReadData(uint32_t address, uint32_t len, 
                                              uint8_t * data);


/****************************************************************************************
*             F I R M W A R E   D A T A
****************************************************************************************/
/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief The S-record parser enables writing and reading firmware data to and from
 *         file formatted as Motorola S-record. This is a widely known file format and
 *         pretty much all microcontroller compiler toolchains included functionality to
 *         output or convert the firmware's data as an S-record.
 */
#define BLT_FIRMWARE_PARSER_SRECORD    ((uint32_t)0u)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
LIBOPENBLT_EXPORT void BltFirmwareInit(uint32_t parserType);
LIBOPENBLT_EXPORT void BltFirmwareTerminate(void);
LIBOPENBLT_EXPORT uint32_t BltFirmwareLoadFromFile(char const * firmwareFile, 
                                                   uint32_t addressOffset);
LIBOPENBLT_EXPORT uint32_t BltFirmwareSaveToFile(char const * firmwareFile);
LIBOPENBLT_EXPORT uint32_t BltFirmwareGetSegmentCount(void);
LIBOPENBLT_EXPORT uint8_t * BltFirmwareGetSegment(uint32_t idx, uint32_t * address, 
                                                  uint32_t * len);
LIBOPENBLT_EXPORT uint32_t BltFirmwareAddData(uint32_t address, uint32_t len, 
                                              uint8_t const * data);
LIBOPENBLT_EXPORT uint32_t BltFirmwareRemoveData(uint32_t address, uint32_t len);
LIBOPENBLT_EXPORT void BltFirmwareClearData(void);


/****************************************************************************************
*             G E N E R I C   U T I L I T I E S
****************************************************************************************/
/****************************************************************************************
* Function prototypes
****************************************************************************************/
LIBOPENBLT_EXPORT uint16_t BltUtilCrc16Calculate(uint8_t const * data, uint32_t len);
LIBOPENBLT_EXPORT uint32_t BltUtilCrc32Calculate(uint8_t const * data, uint32_t len);
LIBOPENBLT_EXPORT uint32_t BltUtilTimeGetSystemTime(void);
LIBOPENBLT_EXPORT void BltUtilTimeDelayMs(uint16_t delay);
LIBOPENBLT_EXPORT uint32_t BltUtilCryptoAes256Encrypt(uint8_t * data, uint32_t len,
                                                      uint8_t const * key);
LIBOPENBLT_EXPORT uint32_t BltUtilCryptoAes256Decrypt(uint8_t * data, uint32_t len,
                                                      uint8_t const * key);


#ifdef __cplusplus
}
#endif

#endif /* OPENBLT_H */
/********************************* end of openblt.h ************************************/

 
