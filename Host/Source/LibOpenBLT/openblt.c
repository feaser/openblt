/************************************************************************************//**
* \file         openblt.c
* \brief        OpenBLT host library source file.
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

/****************************************************************************************
* Include files
****************************************************************************************/
#include <assert.h>                         /* for assertions                          */
#include <stddef.h>                         /* for NULL declaration                    */
#include <stdbool.h>                        /* for boolean type                        */
#include "openblt.h"                        /* OpenBLT host library                    */
#include "util.h"                           /* Utility module                          */
#include "firmware.h"                       /* Firmware data module                    */
#include "srecparser.h"                     /* S-record parser                         */
#include "session.h"                        /* Communication session module            */
#include "xcploader.h"                      /* XCP loader module                       */
#include "xcptpuart.h"                      /* XCP UART transport layer                */
#ifndef NO_CAN_OR_USB
#include "xcptpcan.h"                       /* XCP CAN transport layer                 */
#include "xcptpusb.h"                       /* XCP USB transport layer                 */
#endif // NO_CAN_OR_USB
#include "xcptpnet.h"                       /* XCP TCP/IP transport layer              */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief The version number of the library as an integer. The number has two digits
 *         for major-, minor-, and patch-version. Version 1.05.12 would for example be
 *         10512.
 */
#define BLT_VERSION_NUMBER   (10309u)

/** \brief The version number of the library as a null-terminated string. */
#define BLT_VERSION_STRING   "1.03.09"


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief Constant null-terminated string with the version number of the library. */
char const bltVersionString[] = BLT_VERSION_STRING;


/****************************************************************************************
*             V E R S I O N   I N F O R M A T I O N
****************************************************************************************/
/************************************************************************************//**
** \brief     Obtains the version number of the library as an integer. The number has two
**            digits for major-, minor-, and patch-version. Version 1.05.12 would for
**            example return 10512.
** \return    Library version number as an integer.
**
****************************************************************************************/
LIBOPENBLT_EXPORT uint32_t BltVersionGetNumber(void)
{
  return BLT_VERSION_NUMBER;
} /*** end of BltVersionGetNumber ***/


/************************************************************************************//**
** \brief     Obtains the version number of the library as a null-terminated string. 
**            Version 1.05.12 would for example return "1.05.12".
** \return    Library version number as a null-terminated string.
**
****************************************************************************************/
LIBOPENBLT_EXPORT char const * BltVersionGetString(void)
{
  return BLT_VERSION_STRING;
} /*** end of BltVersionGetString ***/


/****************************************************************************************
*             S E S S I O N   /   T R A N S P O R T   L A Y E R S
****************************************************************************************/
/************************************************************************************//**
** \brief     Initializes the firmware update session for a specific communication
**            protocol and transport layer. This function is typically called once at
**            the start of the firmware update.
** \param     sessionType The communication protocol to use for this session. It should
**            be a BLT_SESSION_xxx value.
** \param     sessionSettings Pointer to a structure with communication protocol specific
**            settings.
** \param     transportType The transport layer to use for the specified communication
**            protocol. It should be a BLT_TRANSPORT_xxx value.
** \param     transportSettings Pointer to a structure with transport layer specific
**            settings.
**
****************************************************************************************/
LIBOPENBLT_EXPORT void BltSessionInit(uint32_t sessionType, 
                                      void const * sessionSettings,
                                      uint32_t transportType, 
                                      void const * transportSettings)
{
  /* Check parameters. Note that the settings-pointers are allowed to be NULL in case
   * no additional settings are needed for the specified session or transport type.
   */
  assert(sessionType == BLT_SESSION_XCP_V10);
  assert( (transportType == BLT_TRANSPORT_XCP_V10_RS232) || \
          (transportType == BLT_TRANSPORT_XCP_V10_CAN) || \
          (transportType == BLT_TRANSPORT_XCP_V10_USB) || \
          (transportType == BLT_TRANSPORT_XCP_V10_NET) );

  /* Initialize the correct session. */
  if (sessionType == BLT_SESSION_XCP_V10) /*lint !e774 */
  {
    /* Verify settingsSettings parameter because the XCP loader requires them. */
    assert(sessionSettings != NULL);
    /* Only continue if the settingsSettings parameter is valid. */
    if (sessionSettings != NULL) /*lint !e774 */
    {
      /* Cast session settings to the correct type. */
      tBltSessionSettingsXcpV10 * bltSessionSettingsXcpV10Ptr;
      bltSessionSettingsXcpV10Ptr = ((tBltSessionSettingsXcpV10 *)sessionSettings);
      /* Convert session settings to the format supported by the XCP loader module. */
      tXcpLoaderSettings xcpLoaderSettings;
      xcpLoaderSettings.timeoutT1 = bltSessionSettingsXcpV10Ptr->timeoutT1;
      xcpLoaderSettings.timeoutT3 = bltSessionSettingsXcpV10Ptr->timeoutT3;
      xcpLoaderSettings.timeoutT4 = bltSessionSettingsXcpV10Ptr->timeoutT4;
      xcpLoaderSettings.timeoutT5 = bltSessionSettingsXcpV10Ptr->timeoutT5;
      xcpLoaderSettings.timeoutT6 = bltSessionSettingsXcpV10Ptr->timeoutT6;
      xcpLoaderSettings.timeoutT7 = bltSessionSettingsXcpV10Ptr->timeoutT7;
      xcpLoaderSettings.seedKeyFile = bltSessionSettingsXcpV10Ptr->seedKeyFile;
      xcpLoaderSettings.connectMode = bltSessionSettingsXcpV10Ptr->connectMode;
      xcpLoaderSettings.transport = NULL;
      xcpLoaderSettings.transportSettings = NULL;
      /* Link the correct transport layer. */
      if (transportType == BLT_TRANSPORT_XCP_V10_RS232)
      {
        /* Verify transportSettings parameters because the XCP UART transport layer 
         * requires them.
         */
        assert(transportSettings != NULL);
        /* Only continue if the transportSettings parameter is valid. */
        if (transportSettings != NULL) /*lint !e774 */
        {
          /* Cast transport settings to the correct type. */
          tBltTransportSettingsXcpV10Rs232 * bltTransportSettingsXcpV10Rs232Ptr;
          bltTransportSettingsXcpV10Rs232Ptr = 
            (tBltTransportSettingsXcpV10Rs232 * )transportSettings;
          /* Convert transport settings to the format supported by the XCP UART transport
           * layer. It was made static to make sure it doesn't get out of scope when
           * used in xcpLoaderSettings.
           */
          static tXcpTpUartSettings xcpTpUartSettings;
          xcpTpUartSettings.baudrate = bltTransportSettingsXcpV10Rs232Ptr->baudrate;
          xcpTpUartSettings.portname = bltTransportSettingsXcpV10Rs232Ptr->portName;
          /* Store transport layer settings in the XCP loader settings. */
          xcpLoaderSettings.transportSettings = &xcpTpUartSettings;
          /* Link the transport layer to the XCP loader settings. */
          xcpLoaderSettings.transport = XcpTpUartGetTransport();
        }
      }
#ifndef NO_CAN_OR_USB
      else if (transportType == BLT_TRANSPORT_XCP_V10_CAN)
      {
        /* Verify transportSettings parameters because the XCP CAN transport layer 
         * requires them.
         */
        assert(transportSettings != NULL);
        /* Only continue if the transportSettings parameter is valid. */
        if (transportSettings != NULL) /*lint !e774 */
        {
          /* Cast transport settings to the correct type. */
          tBltTransportSettingsXcpV10Can * bltTransportSettingsXcpV10CanPtr;
          bltTransportSettingsXcpV10CanPtr =
            (tBltTransportSettingsXcpV10Can *)transportSettings;
          /* Convert transport settings to the format supported by the XCP CAN transport
            * layer. It was made static to make sure it doesn't get out of scope when
            * used in xcpLoaderSettings.
            */
          static tXcpTpCanSettings xcpTpCanSettings;
          xcpTpCanSettings.device = bltTransportSettingsXcpV10CanPtr->deviceName;
          xcpTpCanSettings.channel = bltTransportSettingsXcpV10CanPtr->deviceChannel;
          xcpTpCanSettings.baudrate = bltTransportSettingsXcpV10CanPtr->baudrate;
          xcpTpCanSettings.transmitId = bltTransportSettingsXcpV10CanPtr->transmitId;
          xcpTpCanSettings.receiveId = bltTransportSettingsXcpV10CanPtr->receiveId;
          xcpTpCanSettings.useExtended = (bltTransportSettingsXcpV10CanPtr->useExtended != 0);
          /* Store transport layer settings in the XCP loader settings. */
          xcpLoaderSettings.transportSettings = &xcpTpCanSettings;
          /* Link the transport layer to the XCP loader settings. */
          xcpLoaderSettings.transport = XcpTpCanGetTransport();
        }
      }
      else if (transportType == BLT_TRANSPORT_XCP_V10_USB)
      {
        /* Store transport layer settings in the XCP loader settings. */
        xcpLoaderSettings.transportSettings = NULL;
        /* Link the transport layer to the XCP loader settings. */
        xcpLoaderSettings.transport = XcpTpUsbGetTransport();
      }
#endif // NO_CAN_OR_USB
      else if (transportType == BLT_TRANSPORT_XCP_V10_NET)
      {
        /* Verify transportSettings parameters because the XCP NET transport layer
         * requires them.
         */
        assert(transportSettings != NULL);
        /* Only continue if the transportSettings parameter is valid. */
        if (transportSettings != NULL) /*lint !e774 */
        {
          /* Cast transport settings to the correct type. */
          tBltTransportSettingsXcpV10Net * bltTransportSettingsXcpV10NetPtr;
          bltTransportSettingsXcpV10NetPtr =
            (tBltTransportSettingsXcpV10Net * )transportSettings;
          /* Convert transport settings to the format supported by the XCP NET transport
           * layer. It was made static to make sure it doesn't get out of scope when
           * used in xcpLoaderSettings.
           */
          static tXcpTpNetSettings xcpTpNetSettings;
          xcpTpNetSettings.address = bltTransportSettingsXcpV10NetPtr->address;
          xcpTpNetSettings.port = bltTransportSettingsXcpV10NetPtr->port;
          /* Store transport layer settings in the XCP loader settings. */
          xcpLoaderSettings.transportSettings = &xcpTpNetSettings;
          /* Link the transport layer to the XCP loader settings. */
          xcpLoaderSettings.transport = XcpTpNetGetTransport();
        }
      }
      /* Perform actual session initialization. */
      SessionInit(XcpLoaderGetProtocol(), &xcpLoaderSettings);
    }
  }
} /*** end of BltSessionInit ***/


/************************************************************************************//**
** \brief     Terminates the firmware update session. This function is typically called
**            once at the end of the firmware update.
**
****************************************************************************************/
LIBOPENBLT_EXPORT void BltSessionTerminate(void)
{
  /* Terminate the session. */
  SessionTerminate();
} /*** end of BltSessionTerminate ***/


/************************************************************************************//**
** \brief     Starts the firmware update session. This is were the library attempts to
**            activate and connect with the bootloader running on the target, through
**            the transport layer that was specified during the session's initialization.
** \return    BLT_RESULT_OK if successful, BLT_RESULT_ERROR_xxx otherwise.
**
****************************************************************************************/
LIBOPENBLT_EXPORT uint32_t BltSessionStart(void)
{
  uint32_t result = BLT_RESULT_ERROR_GENERIC;

  /* Start the session. */
  if (SessionStart())
  {
    result = BLT_RESULT_OK;
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of BltSessionStart ***/


/************************************************************************************//**
** \brief     Stops the firmware update session. This is there the library disconnects
**            the transport layer as well.
**
****************************************************************************************/
LIBOPENBLT_EXPORT void BltSessionStop(void)
{
  /* Stop the session. */
  SessionStop();
} /*** end of BltSessionStop ***/


/************************************************************************************//**
** \brief     Requests the target to erase the specified range of memory on the target.
**            Note that the target automatically aligns this to the erasable memory
**            block sizes. This typically results in more memory being erased than the
**            range that was specified here. Refer to the target implementation for
**            details.
** \param     address The starting memory address for the erase operation.
** \param     len The total number of bytes to erase from memory.
** \return    BLT_RESULT_OK if successful, BLT_RESULT_ERROR_xxx otherwise.
**
****************************************************************************************/
LIBOPENBLT_EXPORT uint32_t BltSessionClearMemory(uint32_t address, uint32_t len)
{
  uint32_t result = BLT_RESULT_ERROR_GENERIC;
  
  /* Check parameters. */
  assert(len > 0);
  
  /* Only continue if the parameters are valid. */
  if (len > 0) 
  {
    /* Pass the request on to the session module. */
    if (SessionClearMemory(address, len))
    {
      result = BLT_RESULT_OK;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of BltSessionClearMemory ***/


/************************************************************************************//**
** \brief     Requests the target to program the specified data to memory. Note that it
**            is the responsibility of the application to make sure the memory range was
**            erased beforehand.
** \param     address The starting memory address for the write operation.
** \param     len The number of bytes in the data buffer that should be written.
** \param     data Pointer to the byte array with data to write.
** \return    BLT_RESULT_OK if successful, BLT_RESULT_ERROR_xxx otherwise.
**
****************************************************************************************/
LIBOPENBLT_EXPORT uint32_t BltSessionWriteData(uint32_t address, uint32_t len, 
                                               uint8_t const * data)
{
  uint32_t result = BLT_RESULT_ERROR_GENERIC;

  /* Check parameters. */
  assert(data != NULL);
  assert(len > 0);
  
  /* Only continue if the parameters are valid. */ 
  if ( (data != NULL) && (len > 0) ) /*lint !e774 */
  {
    /* Pass the request on to the session module. */
    if (SessionWriteData(address, len, data))
    {
      result = BLT_RESULT_OK;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of BltSessionWriteData ***/


/************************************************************************************//**
** \brief     Requests the target to upload the specified range from memory and store its
**            contents in the specified data buffer.
** \param     address The starting memory address for the read operation.
** \param     len The number of bytes to upload from the target and store in the data 
**            buffer.
** \param     data Pointer to the byte array where the uploaded data should be stored.
** \return    BLT_RESULT_OK if successful, BLT_RESULT_ERROR_xxx otherwise.
**
****************************************************************************************/
LIBOPENBLT_EXPORT uint32_t BltSessionReadData(uint32_t address, uint32_t len, 
                                              uint8_t * data)
{
  uint32_t result = BLT_RESULT_ERROR_GENERIC;

  /* Check parameters. */
  assert(data != NULL);
  assert(len > 0);

  /* Only continue if the parameters are valid. */
  if ( (data != NULL) && (len > 0) ) /*lint !e774 */
  {
    /* Pass the request on to the session module. */
    if (SessionReadData(address, len, data))
    {
      result = BLT_RESULT_OK;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of BltSessionReadData ***/


/****************************************************************************************
*             F I R M W A R E   D A T A
****************************************************************************************/
/************************************************************************************//**
** \brief     Initializes the firmware data module for a specified firmware file parser.
** \param     parserType The firmware file parser to use in this module. It should be a
**            BLT_FIRMWARE_PARSER_xxx value.
**
****************************************************************************************/
LIBOPENBLT_EXPORT void BltFirmwareInit(uint32_t parserType)
{
  tFirmwareParser const * firmwareParser = NULL;

  /* Verify parameters. */
  assert(parserType == BLT_FIRMWARE_PARSER_SRECORD);
  
  /* Set the parser pointer. */
  if (parserType == BLT_FIRMWARE_PARSER_SRECORD) /*lint !e774 */
  {
    firmwareParser = SRecParserGetParser();
  }
  /* Initialize the firmware data module by linking the firmware file parser. */
  FirmwareInit(firmwareParser);
} /*** end of BltFirmwareInit ***/


/************************************************************************************//**
** \brief     Terminates the firmware data module. Typically called at the end of the 
**            program when the firmware data module is no longer needed.
**
****************************************************************************************/
LIBOPENBLT_EXPORT void BltFirmwareTerminate(void)
{
  /* Terminate the firmware data module. */
  FirmwareTerminate();
} /*** end of BltFirmwareTerminate ***/


/************************************************************************************//**
** \brief     Loads firmware data from the specified file using the firmware file parser
**            that was specified during the initialization of this module.
** \param     firmwareFile Filename of the firmware file to load.
** \param     addressOffset Optional memory address offset to add when loading the 
**            firmware data from the file. This is typically only useful when loading
**            firmware data from a binary formatted firmware file.
** \return    BLT_RESULT_OK if successful, BLT_RESULT_ERROR_xxx otherwise.
**
****************************************************************************************/
LIBOPENBLT_EXPORT uint32_t BltFirmwareLoadFromFile(char const * firmwareFile, 
                                                   uint32_t addressOffset)
{
  uint32_t result = BLT_RESULT_ERROR_GENERIC;
  
  /* Verify parameters. */
  assert(firmwareFile != NULL);
  
  /* Only continue if parameters are valid. */
  if (firmwareFile != NULL) /*lint !e774 */
  {
    /* Pass the request on to the firmware data module. */
    if (FirmwareLoadFromFile(firmwareFile, addressOffset))
    {
      result = BLT_RESULT_OK;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of BltFirmwareLoadFromFile ***/


/************************************************************************************//**
** \brief     Writes firmware data to the specified file using the firmware file parser
**            that was specified during the initialization of this module.
** \param     firmwareFile Filename of the firmware file to write to.
** \return    BLT_RESULT_OK if successful, BLT_RESULT_ERROR_xxx otherwise.
**
****************************************************************************************/
LIBOPENBLT_EXPORT uint32_t BltFirmwareSaveToFile(char const * firmwareFile)
{
  uint32_t result = BLT_RESULT_ERROR_GENERIC;
  
  /* Verify parameters. */
  assert(firmwareFile != NULL);
  
  /* Only continue if parameters are valid. */
  if (firmwareFile != NULL) /*lint !e774 */
  {
    /* Pass the request on to the firmware data module. */
    if (FirmwareSaveToFile(firmwareFile))
    {
      result = BLT_RESULT_OK;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of BltFirmwareSaveToFile ***/


/************************************************************************************//**
** \brief     Obtains the number of firmware data segments that are currently present
**            in the firmware data module.
** \return    The total number of segments.
**
****************************************************************************************/
LIBOPENBLT_EXPORT uint32_t BltFirmwareGetSegmentCount(void)
{
  /* Pass the request on to the firmware data module. */
  return FirmwareGetSegmentCount();
} /*** end of BltFirmwareGetSegmentCount ***/


/************************************************************************************//**
** \brief     Obtains the contents of the firmware data segment that was specified by the
**            index parameter. 
** \param     idx The segment index. It should be a value greater or equal to zero and
**            smaller than the value returned by \ref BltFirmwareGetSegmentCount.
** \param     address Pointer to where the segment's base address will be written to.
** \param     len Pointer to where the segment's length will be written to.
** \return    Pointer to the segment data if successful, NULL otherwise.
**
****************************************************************************************/
LIBOPENBLT_EXPORT uint8_t * BltFirmwareGetSegment(uint32_t idx, uint32_t * address, 
                                                  uint32_t * len)
{
  uint8_t * result = NULL;
  tFirmwareSegment *segmentPtr;

  /* Verify parameters. Note that it is okay for the data parameter to be NULL. */
  assert(idx < FirmwareGetSegmentCount());
  assert(address != NULL);
  assert(len != NULL);
  
  /* Only continue if parameters are valid. */ 
  if ((address != NULL) && (len != NULL) &&
      (idx < FirmwareGetSegmentCount()) ) /*lint !e774 */
  {
    /* Pass the request on to the firmware data module. */
    segmentPtr = FirmwareGetSegment(idx);
    /* Process the result. */
    if (segmentPtr != NULL)
    {
      *address = segmentPtr->base;
      *len = segmentPtr->length;
      result = segmentPtr->data;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of BltFirmwareGetSegment ***/


/************************************************************************************//**
** \brief     Adds data to the segments that are currently present in the firmware data
**            module. If the data overlaps with already existing data, the existing data
**            gets overwritten. The size of a segment is automatically adjusted or a new
**            segment gets created, if necessary.
** \param     address Base address of the firmware data. 
** \param     len Number of bytes to add.
** \param     data Pointer to array with data bytes that should be added.
** \return    BLT_RESULT_OK if successful, BLT_RESULT_ERROR_xxx otherwise.
**
****************************************************************************************/
LIBOPENBLT_EXPORT uint32_t BltFirmwareAddData(uint32_t address, uint32_t len, 
                                              uint8_t const * data)
{
  uint32_t result = BLT_RESULT_ERROR_GENERIC;
  
  /* Verify parameters. */
  assert(len > 0);
  assert(data != NULL);
  
  /* Only continue if parameters are valid. */
  if ( (len > 0) && (data != NULL) ) /*lint !e774 */
  {
    /* Pass the request on to the firmware data module. */
    if (FirmwareAddData(address, len, data))
    {
      result = BLT_RESULT_OK;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of BltFirmwareAddData ***/


/************************************************************************************//**
** \brief     Removes data from the segments that are currently present in the firmware 
**            data module. The size of a segment is automatically adjusted or removed, if
**            necessary.
** \param     address Base address of the firmware data. 
** \param     len Number of bytes to remove.
** \return    BLT_RESULT_OK if successful, BLT_RESULT_ERROR_xxx otherwise.
**
****************************************************************************************/
LIBOPENBLT_EXPORT uint32_t BltFirmwareRemoveData(uint32_t address, uint32_t len)
{
  uint32_t result = BLT_RESULT_ERROR_GENERIC;

  /* Verify parameters. */
  assert(len > 0);
  
  /* Only continue if parameters are valid. */
  if (len > 0)
  {
    /* Pass the request on to the firmware data module. */
    if (FirmwareRemoveData(address, len))
    {
      result = BLT_RESULT_OK;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of BltFirmwareRemoveData ***/


/************************************************************************************//**
** \brief     Clears all data and segments that are currently present in the firmware
**            data module.
**
****************************************************************************************/
LIBOPENBLT_EXPORT void BltFirmwareClearData(void)
{
  /* Pass the request on to the firmware data module. */
  FirmwareClearData();
} /*** end of BltFirmwareClearData ***/


/****************************************************************************************
*             G E N E R I C   U T I L I T I E S
****************************************************************************************/
/************************************************************************************//**
** \brief     Calculates a 16-bit CRC value over the specified data.
** \param     data Array with bytes over which the CRC16 should be calculated.
** \param     len Number of bytes in the data array.
** \return    The 16-bit CRC value.
**
****************************************************************************************/
LIBOPENBLT_EXPORT uint16_t BltUtilCrc16Calculate(uint8_t const * data, uint32_t len)
{
  uint16_t result = 0;
  
  /* Check parameters. */
  assert(data != NULL);
  assert(len > 0);

  /* Only continue if parameters are valid. */
  if ( (data != NULL) && (len > 0) ) /*lint !e774 */
  {
    /* Perform checksum calculation. */
    result = UtilChecksumCrc16Calculate(data, len);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of BltUtilCrc16Calculate ***/


/************************************************************************************//**
** \brief     Calculates a 32-bit CRC value over the specified data.
** \param     data Array with bytes over which the CRC32 should be calculated.
** \param     len Number of bytes in the data array.
** \return    The 32-bit CRC value.
**
****************************************************************************************/
LIBOPENBLT_EXPORT uint32_t BltUtilCrc32Calculate(uint8_t const * data, uint32_t len)
{
  uint32_t result = 0;
  
  /* Check parameters. */
  assert(data != NULL);
  assert(len > 0);

  /* Only continue if parameters are valid. */
  if ( (data != NULL) && (len > 0) ) /*lint !e774 */
  {
    /* Perform checksum calculation. */
    result = UtilChecksumCrc32Calculate(data, len);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of BltUtilCrc32Calculate ***/


/************************************************************************************//**
** \brief     Get the system time in milliseconds.
** \return    Time in milliseconds.
**
****************************************************************************************/
LIBOPENBLT_EXPORT uint32_t BltUtilTimeGetSystemTime(void)
{
  uint32_t result;
  
  /* Pass the request on to the utility module. */
  result = UtilTimeGetSystemTimeMs();
  /* Give the result back to the caller. */
  return result;
} /*** end of BltUtilTimeGetSystemTime ***/

/************************************************************************************//**
** \brief     Performs a delay of the specified amount of milliseconds.
** \param     delay Delay time in milliseconds.
**
****************************************************************************************/
LIBOPENBLT_EXPORT void BltUtilTimeDelayMs(uint16_t delay)
{
  /* Pass the request on to the utility module. */
  UtilTimeDelayMs(delay);
} /*** end of BltUtilTimeDelayMs ***/


/************************************************************************************//**
** \brief     Encrypts the len-bytes in the specified data-array, using the specified
**            256-bit (32 bytes) key. The results are written back into the same array.
** \param     data Pointer to the byte array with data to encrypt. The encrypted bytes
**            are stored in the same array.
** \param     len The number of bytes in the data-array to encrypt. It must be a multiple
**            of 16, as this is the AES256 minimal block size.
** \param     key The 256-bit encryption key as a array of 32 bytes.
** \return    BLT_RESULT_OK if successful, BLT_RESULT_ERROR_xxx otherwise.
**
****************************************************************************************/
LIBOPENBLT_EXPORT uint32_t BltUtilCryptoAes256Encrypt(uint8_t * data, uint32_t len,
                                                      uint8_t const * key)
{
  uint32_t result = BLT_RESULT_ERROR_GENERIC;

  /* Check parameters */
  assert(data != NULL);
  assert(key != NULL);

  /* Only continue with valid parameters. Also add a block size check for 'len'. */
  if ( (data != NULL) && (key != NULL) && ((len % 16u) == 0) ) /*lint !e774 */
  {
    /* Pass the request on to the utility module. */
    if (UtilCryptoAes256Encrypt(data, len, key))
    {
      result = BLT_RESULT_OK;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of BltUtilCryptoAes256Encrypt ***/


/************************************************************************************//**
** \brief     Decrypts the len-bytes in the specified data-array, using the specified 256-
**            bit (32 bytes) key. The results are written back into the same array.
** \param     data Pointer to the byte array with data to decrypt. The decrypted bytes
**            are stored in the same array.
** \param     len The number of bytes in the data-array to decrypt. It must be a multiple
**            of 16, as this is the AES256 minimal block size.
** \param     key The 256-bit decryption key as a array of 32 bytes.
** \return    BLT_RESULT_OK if successful, BLT_RESULT_ERROR_xxx otherwise.
**
****************************************************************************************/
LIBOPENBLT_EXPORT uint32_t BltUtilCryptoAes256Decrypt(uint8_t * data, uint32_t len,
                                                      uint8_t const * key)
{
  uint32_t result = BLT_RESULT_ERROR_GENERIC;

  /* Check parameters */
  assert(data != NULL);
  assert(key != NULL);

  /* Only continue with valid parameters. Also add a block size check for 'len'. */
  if ( (data != NULL) && (key != NULL) && ((len % 16u) == 0) ) /*lint !e774 */
  {
    /* Pass the request on to the utility module. */
    if (UtilCryptoAes256Decrypt(data, len, key))
    {
      result = BLT_RESULT_OK;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of BltUtilCryptoAes256Decrypt ***/


/*********************************** end of openblt.c **********************************/

 
