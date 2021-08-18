/************************************************************************************//**
* \file         xcploader.c
* \brief        XCP Loader module source file.
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
#include "xcpprotect.h"                     /* XCP protection module                   */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* XCP command codes as defined by the protocol currently supported by this module */
#define XCPLOADER_CMD_CONNECT         (0xFFu)    /**< XCP connect command code.        */
#define XCPLOADER_CMD_GET_STATUS      (0xFDu)    /**< XCP get status command code.     */
#define XCPLOADER_CMD_GET_SEED        (0xF8u)    /**< XCP get seed command code.       */
#define XCPLOADER_CMD_UNLOCK          (0xF7u)    /**< XCP unlock command code.         */
#define XCPLOADER_CMD_SET_MTA         (0xF6u)    /**< XCP set mta command code.        */
#define XCPLOADER_CMD_UPLOAD          (0xF5u)    /**< XCP upload command code.         */
#define XCPLOADER_CMD_PROGRAM_START   (0xD2u)    /**< XCP program start command code.  */
#define XCPLOADER_CMD_PROGRAM_CLEAR   (0xD1u)    /**< XCP program clear command code.  */
#define XCPLOADER_CMD_PROGRAM         (0xD0u)    /**< XCP program command code.        */
#define XCPLOADER_CMD_PROGRAM_RESET   (0xCFu)    /**< XCP program reset command code.  */
#define XCPLOADER_CMD_PROGRAM_MAX     (0xC9u)    /**< XCP program max command code.    */

/* XCP response packet IDs as defined by the protocol. */
#define XCPLOADER_CMD_PID_RES         (0xFFu)    /**< positive response                */

/** \brief Number of retries to connect to the XCP slave. */
#define XCPLOADER_CONNECT_RETRIES     (5u)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
/* Protocol functions for linking to the session module. */
static void XcpLoaderInit(void const * settings);
static void XcpLoaderTerminate(void);
static bool XcpLoaderStart(void);
static void XcpLoaderStop(void);
static bool XcpLoaderClearMemory(uint32_t address, uint32_t len);
static bool XcpLoaderWriteData(uint32_t address, uint32_t len, uint8_t const * data);  
static bool XcpLoaderReadData(uint32_t address, uint32_t len, uint8_t * data);
/* General module specific utility functions. */
static void XcpLoaderSetOrderedLong(uint32_t value, uint8_t *data);
static uint16_t XcpLoaderGetOrderedWord(uint8_t const * data);
/* XCP command functions. */
static bool XcpLoaderSendCmdConnect(void);
static bool XcpLoaderSendCmdGetStatus(uint8_t * session, uint8_t * protectedResources,
                                      uint16_t * configId);
static bool XcpLoaderSendCmdGetSeed(uint8_t resource, uint8_t mode, uint8_t * seed, uint8_t * seedLen);
static bool XcpLoaderSendCmdUnlock(uint8_t const * key, uint8_t keyLen,
                                   uint8_t * protectedResources);
static bool XcpLoaderSendCmdSetMta(uint32_t address);
static bool XcpLoaderSendCmdUpload(uint8_t * data, uint8_t length);
static bool XcpLoaderSendCmdProgramStart(void);
static bool XcpLoaderSendCmdProgramReset(void);
static bool XcpLoaderSendCmdProgram(uint8_t length, uint8_t const * data);
static bool XcpLoaderSendCmdProgramMax(uint8_t const * data);
static bool XcpLoaderSendCmdProgramClear(uint32_t length);


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief Protocol structure filled with XCP loader specifics. */
static const tSessionProtocol xcpLoader =
{
  .Init = XcpLoaderInit,
  .Terminate = XcpLoaderTerminate,
  .Start = XcpLoaderStart,
  .Stop = XcpLoaderStop,
  .ClearMemory = XcpLoaderClearMemory,
  .WriteData = XcpLoaderWriteData,
  .ReadData = XcpLoaderReadData
};


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief The settings that should be used by the XCP loader. */
static tXcpLoaderSettings xcpSettings;

/** \brief Flag to keep track of the connection status. */
static bool xcpConnected;

/** \brief Store the byte ordering of the XCP slave. */
static bool xcpSlaveIsIntel;

/** \brief The max number of bytes in the command transmit object (master->slave). */
static uint8_t xcpMaxCto;

/** \brief The max number of bytes in the command transmit object (master->slave) during
 *         a programming session.
 */
static uint8_t xcpMaxProgCto;

/** \brief The max number of bytes in the data transmit object (slave->master). */
static uint16_t xcpMaxDto;


/***********************************************************************************//**
** \brief     Obtains a pointer to the protocol structure, so that it can be linked to 
**            the communication session module.
** \return    Pointer to protocol structure.
**
****************************************************************************************/
tSessionProtocol const * XcpLoaderGetProtocol(void)
{
  return &xcpLoader;
} /*** end of XcpLoaderGetProtocol ***/


/************************************************************************************//**
** \brief     Initializes the protocol module.
** \param     settings Pointer to the structure with protocol settings.
**
****************************************************************************************/
static void XcpLoaderInit(void const * settings)
{
  char * seedNKeyFileName;

  /* Initialize locals. */
  xcpConnected = false;
  xcpSlaveIsIntel = false;
  xcpMaxCto = 0;
  xcpMaxProgCto = 0;
  xcpMaxDto = 0;

  /* Reset the XCP session layer settings. */
  xcpSettings.timeoutT1 = 1000;
  xcpSettings.timeoutT3 = 2000;
  xcpSettings.timeoutT4 = 10000;
  xcpSettings.timeoutT5 = 1000;
  xcpSettings.timeoutT6 = 50;
  xcpSettings.timeoutT7 = 2000;
  xcpSettings.connectMode = 0;
  xcpSettings.seedKeyFile = NULL;
  xcpSettings.transport = NULL;
  xcpSettings.transportSettings = NULL;

  /* Check parameter. */
  assert(settings != NULL);
  /* Only continue with valid parameter. */
  if (settings != NULL) /*lint !e774 */
  {
    /* shallow copy the XCP settings for later usage */
    xcpSettings = *(tXcpLoaderSettings *)settings;

    /* The seedKeyFile is a pointer and it is not guaranteed that it stays valid so we
     * need to deep copy this one. note the +1 for '\0' in malloc. Note that it is okay
     * for this value to be NULL.
     */
    if (((tXcpLoaderSettings *)settings)->seedKeyFile != NULL) /*lint !e774 */
    {
      seedNKeyFileName = malloc(strlen(((tXcpLoaderSettings *)settings)->seedKeyFile) + 1);
      assert(seedNKeyFileName != NULL);
      if (seedNKeyFileName != NULL) /*lint !e774 */
      {
        strcpy(seedNKeyFileName, ((tXcpLoaderSettings *)settings)->seedKeyFile);
        xcpSettings.seedKeyFile = seedNKeyFileName;
      }
    }
    /* Check that a valid transport layer was specified. */
    assert(xcpSettings.transport != NULL);
    /* Only access the transport layer if it is valid. */
    if (xcpSettings.transport != NULL) /*lint !e774 */
    {
      /* Initialize the transport layer. */
      xcpSettings.transport->Init(xcpSettings.transportSettings);
    }
    /* Invalidate the transportSettings as it is probably no longer valid outside this
     * function scope and should also not be used anymore.
     */
    xcpSettings.transportSettings = NULL;
  }
  /* Initialize the XCP protection module. */
  XcpProtectInit(xcpSettings.seedKeyFile);
} /*** end of XcpLoaderInit ***/


/************************************************************************************//**
** \brief     Terminates the protocol module.
**
****************************************************************************************/
static void XcpLoaderTerminate(void)
{
  /* Make sure a valid transport layer is linked. */
  assert(xcpSettings.transport != NULL);
  
  /* Terminate the XCP protection module. */
  XcpProtectTerminate();
  /* Only continue with a valid transport layer. */
  if (xcpSettings.transport != NULL) /*lint !e774 */
  {
    /* Terminate the transport layer. */
    xcpSettings.transport->Terminate();
    /* Unlink the transport layer. */
    xcpSettings.transport = NULL;
  }
  /* Release memory that was allocated for storing the seedKeyFile. */
  if (xcpSettings.seedKeyFile != NULL)
  {
    free((char *)xcpSettings.seedKeyFile);
  }
  /* Reset the XCP session layer settings. */
  xcpSettings.timeoutT1 = 1000;
  xcpSettings.timeoutT3 = 2000;
  xcpSettings.timeoutT4 = 10000;
  xcpSettings.timeoutT5 = 1000;
  xcpSettings.timeoutT6 = 50;
  xcpSettings.timeoutT7 = 2000;
  xcpSettings.connectMode = 0;
  xcpSettings.seedKeyFile = NULL;
  xcpSettings.transport = NULL;
  xcpSettings.transportSettings = NULL;
} /*** end of XcpLoaderTerminate ***/


/************************************************************************************//**
** \brief     Starts the firmware update session. This is where the connection with the
**            target is made and the bootloader on the target is activated.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderStart(void)
{
  bool result = false;
  uint8_t retryCnt;
  uint8_t protectedResources = 0;

  /* Make sure a valid transport layer is linked. */
  assert(xcpSettings.transport != NULL);
  
  /* Only continue with a valid transport layer. */
  if (xcpSettings.transport != NULL) /*lint !e774 */
  {
    /* Make sure the session is stopped before starting a new one. */
    XcpLoaderStop();
    /* Init the result value to okay and only set it to error when a problem occurred. */
    result = true;
    /* Connect the transport layer. */
    if (!xcpSettings.transport->Connect())
    {
      /* Could not connect the transport layer. */
      result = false;
    }
    /* Connect to the target with a finite amount of retries. */
    if (result)
    {
      for (retryCnt=0; retryCnt<XCPLOADER_CONNECT_RETRIES; retryCnt++)
      {
        /* Send the connect command. */
        if (XcpLoaderSendCmdConnect())
        {
          /* Update connection state. */
          xcpConnected = true;
          /* Connected so no need to retry. */
          break;
        }
      }
      /* Check if a connection with the target could be made within the finite amount
       * of retries.
       */
      if (!xcpConnected)
      {
        /* Disconnect the transport layer again. */
        xcpSettings.transport->Disconnect();
        /* Update the result. */
        result = false;
      }
    }
    /* Obtain the current resource protection status. */
    if (result)
    {
      if (!XcpLoaderSendCmdGetStatus(NULL, &protectedResources, NULL))
      {
        result = false;
      }
    }
    /* Check if the programming resource needs to be unlocked. */
    if ( (protectedResources & XCPPROTECT_RESOURCE_PGM) != 0)
    {
      uint8_t availableResources = 0;
      uint8_t seed[256] = { 0 };
      uint8_t *seedPtr = &seed[0];
      uint8_t seedTotalLen = 0;
      uint8_t seedRemainingLen = 0;
      uint8_t key[256] = { 0 };
      uint8_t keyTotalLen = 0;
      uint8_t keyRemainingLen = 0;
      uint8_t keyCurrentLen = 0;
      uint8_t *keyPtr = &key[0];

      /* Make sure the XCP protection module contains an unlock algorithm for the
       * programming resource.
       */
      if (result)
      {
        if (!XcpProtectGetPrivileges(&availableResources))
        {
          /* Could not obtain the supported resource privileges from the XCP protection
           * module.
           */
          result = false;
        }
        else if ((availableResources & XCPPROTECT_RESOURCE_PGM) == 0)
        {
          /* No unlock algorithm available for the programming resource in the XCP
           * protection module.
           */
          result = false;
        }
      }
      /* Request (first part of) the seed for unlocking the programming resources. */
      if (result)
      {
        if (!XcpLoaderSendCmdGetSeed(XCPPROTECT_RESOURCE_PGM, 0, seedPtr, &seedRemainingLen))
        {
          result = false;
        }
        else
        {
          /* store the total seed length */
          seedTotalLen = seedRemainingLen;
        }
      }
      /* Check if more parts of the seed need to be requested. */
      if (result)
      {
        while (seedRemainingLen > (xcpMaxDto - 2))
        {
          /* update the seed pointer for the next part */
          seedPtr += (xcpMaxDto - 2);
          if (!XcpLoaderSendCmdGetSeed(XCPPROTECT_RESOURCE_PGM, 1, seedPtr, &seedRemainingLen))
          {
            result = false;
            break;
          }
        }
      }
      /* Only continue with resource unlock operation if not already unlocked, which
       * is indicated by a seed length of 0.
       */
      if ( (result) && (seedTotalLen > 0) )
      {
        /* Compute the key using the XCP protection module. */
        if (!XCPProtectComputeKeyFromSeed(XCPPROTECT_RESOURCE_PGM, seedTotalLen, seed,
                                          &keyTotalLen, key))
        {
          result = false;
        }
        /* Unlock the resource now that the key is available. */
        if (result)
        {
          uint8_t currentlyProtectedResources = 0;

          /* Initialize remaining length */
          keyRemainingLen = keyTotalLen;

          /* Send the key to unlock the resource */
          while (keyRemainingLen > 0)
          {
            /* Determine how many key bytes are about to be sent. */
            keyCurrentLen = keyRemainingLen;
            if (keyCurrentLen > (xcpMaxCto - 2))
            {
              keyCurrentLen = (xcpMaxCto - 2);
            }
            /* Send the (possible partial) unlock command. */
            if (!XcpLoaderSendCmdUnlock(keyPtr, keyRemainingLen, &currentlyProtectedResources))
            {
              result = false;
              break;
            }
            /* Update key pointer and the remaining length */
            keyRemainingLen -= keyCurrentLen;
            keyPtr += keyCurrentLen;
            /* Check if the key was now completely sent. */
            if (keyRemainingLen == 0)
            {
              /* Double-check that the programming resource is now unlocked. */
              if ((currentlyProtectedResources & XCPPROTECT_RESOURCE_PGM) != 0)
              {
                /* Programming resource unlock operation failed. */
                result = false;
              }
            }
          }
        }
      }
    }
    /* Place the target in programming mode if connected. */
    if (result)
    {
      if (!XcpLoaderSendCmdProgramStart())
      {
        result = false;
      }
    }
  }   
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpLoaderStart ***/


/************************************************************************************//**
** \brief     Stops the firmware update. This is where the bootloader starts the user
**            program on the target if a valid one is present. After this the connection
**            with the target is severed.
**
****************************************************************************************/
static void XcpLoaderStop(void)
{
  /* Make sure a valid transport layer is linked. */
  assert(xcpSettings.transport != NULL);
  
  /* Only continue with a valid transport layer and if actually connected. */
  if ( (xcpSettings.transport != NULL) && (xcpConnected) ) /*lint !e774 */
  {
    /* End the programming session by sending the program command with size 0. */
    if (XcpLoaderSendCmdProgram(0, NULL))
    {
      /* Disconnect the target. Here the reset command is used instead of the disconnect
       * command, because the bootloader should start the user program on the target.
       */
      (void)XcpLoaderSendCmdProgramReset();
    }
    /* Disconnect the transport layer. */
    xcpSettings.transport->Disconnect();
    /* Reset connection status. */
    xcpConnected = false;
  }
} /*** end of XcpLoaderStop ***/


/************************************************************************************//**
** \brief     Requests the bootloader to erase the specified range of memory on the
**            target. The bootloader aligns this range to hardware specified erase 
**            blocks.
** \param     address The starting memory address for the erase operation.
** \param     len The total number of bytes to erase from memory.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderClearMemory(uint32_t address, uint32_t len)
{
  bool result = false;
  
  /* Check parameters. */
  assert(len > 0);
  /* Make sure a valid transport layer is linked. */
  assert(xcpSettings.transport != NULL);
  
  /* Only continue if a transport layer is linked and the parameters are valid. */
  if ( (len > 0) && (xcpSettings.transport != NULL) && (xcpConnected) )
  {
    /* Init the result value to okay and only set it to error when a problem occurred. */
    result = true;
    /* First set the MTA pointer. */
    if (!XcpLoaderSendCmdSetMta(address))
    {
      result = false;
    }
    /* Now perform the erase operation. */
    if (result)
    {
      if (!XcpLoaderSendCmdProgramClear(len))
      {
        result = false;
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpLoaderClearMemory ***/


/************************************************************************************//**
** \brief     Requests the bootloader to program the specified data to memory. In case of
**            non-volatile memory, the application needs to make sure the memory range
**            was erased beforehand.
** \param     address The starting memory address for the write operation.
** \param     len The number of bytes in the data buffer that should be written.
** \param     data Pointer to the byte array with data to write.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderWriteData(uint32_t address, uint32_t len, uint8_t const * data)
{
  bool result = false;
  uint8_t currentWriteCnt;
  uint32_t bufferOffset = 0;

  /* Check parameters. */
  assert(data != NULL);
  assert(len > 0);
  /* Make sure a valid transport layer is linked. */
  assert(xcpSettings.transport != NULL);
  
  /* Only continue if the parameters are valid. */
  if ( (data != NULL) && (len > 0) && (xcpSettings.transport != NULL) && /*lint !e774 */
       (xcpConnected) ) 
  {
    /* Init the result value to okay and only set it to error when a problem occurred. */
    result = true;
    /* First set the MTA pointer. */
    if (!XcpLoaderSendCmdSetMta(address))
    {
      result = false;
    }

    /* Perform segmented programming of the data. */
    if (result)
    {
      while (len > 0)
      {
        /* Set the current write length to make optimal use of the available packet 
         * data. 
         */
        currentWriteCnt = len % (xcpMaxProgCto - 1u);
        if (currentWriteCnt == 0)
        {
          currentWriteCnt = (xcpMaxProgCto - 1u);
        }
        /* Prepare the packed data for the program command. */
        if (currentWriteCnt < (xcpMaxProgCto - 1u))
        {
          /* Program data. */
          if (!XcpLoaderSendCmdProgram(currentWriteCnt, &data[bufferOffset]))
          {
            result = false;
            break;
          }
        }
        else
        {
          /* Program max data. */
          if (!XcpLoaderSendCmdProgramMax(&data[bufferOffset]))
          {
            result = false;
            break;
          }
        }
        /* Update loop variables. */
        len -= currentWriteCnt;
        bufferOffset += currentWriteCnt;
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpLoaderWriteData ***/


/************************************************************************************//**
** \brief     Request the bootloader to upload the specified range of memory. The data is
**            stored in the data byte array to which the pointer was specified.
** \param     address The starting memory address for the read operation.
** \param     len The number of bytes to upload from the target and store in the data 
**            buffer.
** \param     data Pointer to the byte array where the uploaded data should be stored.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderReadData(uint32_t address, uint32_t len, uint8_t * data)
{
  bool result = false;
  uint8_t currentReadCnt;
  uint32_t bufferOffset = 0;

  /* Check parameters. */
  assert(data != NULL);
  assert(len > 0);
  /* Make sure a valid transport layer is linked. */
  assert(xcpSettings.transport != NULL);
  
  /* Only continue if the parameters are valid. */
  if ( (data != NULL) && (len > 0) && (xcpSettings.transport != NULL) && /*lint !e774 */
       (xcpConnected) ) 
  {
    /* Init the result value to okay and only set it to error when a problem occurred. */
    result = true;
    /* First set the MTA pointer. */
    if (!XcpLoaderSendCmdSetMta(address))
    {
      result = false;
    }
    /* Perform segmented upload of the data. */
    if (result)
    {
      while (len > 0)
      {
        /* Set the current read length to make optimal use of the available packet 
         * data. 
         */
        currentReadCnt = len % (uint8_t)(xcpMaxDto - 1u);
        if (currentReadCnt == 0)
        {
          currentReadCnt = (uint8_t)(xcpMaxDto - 1u);
        }
        /* Upload some data */
        if (!XcpLoaderSendCmdUpload(&data[bufferOffset], currentReadCnt))
        {
          result = false;
          break;
        }
        /* Update loop variables. */
        len -= currentReadCnt;
        bufferOffset += currentReadCnt;
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpLoaderReadData ***/


/************************************************************************************//**
** \brief     Stores a 32-bit value into a byte buffer taking into account Intel
**            or Motorola byte ordering.
** \param     value The 32-bit value to store in the buffer.
** \param     data Array to the buffer for storage.
**
****************************************************************************************/
static void XcpLoaderSetOrderedLong(uint32_t value, uint8_t *data)
{
  /* Check parameters. */
  assert(data != NULL);
  
  /* Only continue with valid parameters. */
  if (data != NULL) /*lint !e774 */
  {
    if (xcpSlaveIsIntel)
    {
      data[3] = (uint8_t)(value >> 24);
      data[2] = (uint8_t)(value >> 16);
      data[1] = (uint8_t)(value >>  8);
      data[0] = (uint8_t)value;
    }
    else
    {
      data[0] = (uint8_t)(value >> 24);
      data[1] = (uint8_t)(value >> 16);
      data[2] = (uint8_t)(value >>  8);
      data[3] = (uint8_t)value;
    }
  }
} /*** end of XcpLoaderSetOrderedLong ***/


/************************************************************************************//**
** \brief     Obtains a 16-bit value from a byte buffer taking into account Intel
**            or Motorola byte ordering.
** \param     data Array to the buffer with the word value stored as bytes.
** \return    The 16-bit value.
**
****************************************************************************************/
static uint16_t XcpLoaderGetOrderedWord(uint8_t const * data)
{
  uint16_t result = 0;

  /* Check parameters. */
  assert(data != NULL);

  /* Only continue with valid parameters. */
  if (data != NULL) /*lint !e774 */
  {
    if (xcpSlaveIsIntel)
    {
      result |= (uint16_t)data[0];
      result |= (uint16_t)(data[1] << 8);
    }
    else
    {
      result |= (uint16_t)data[1];
      result |= (uint16_t)(data[0] << 8);
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpLoaderGetOrderedWord ***/


/************************************************************************************//**
** \brief     Sends the XCP Connect command.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderSendCmdConnect(void)
{
  bool result = false;
  tXcpTransportPacket cmdPacket;
  tXcpTransportPacket resPacket;

  /* Make sure a valid transport layer is linked. */
  assert(xcpSettings.transport != NULL);
  
  /* Only continue with a valid transport layer. */
  if (xcpSettings.transport != NULL) /*lint !e774 */
  {
    /* Init the result value to okay and only set it to error when a problem occurred. */
    result = true;
    /* Prepare the command packet. */
    cmdPacket.data[0] = XCPLOADER_CMD_CONNECT;
    cmdPacket.data[1] = xcpSettings.connectMode;
    cmdPacket.len = 2;
    /* Send the packet. */
    if (!xcpSettings.transport->SendPacket(&cmdPacket, &resPacket, 
                                           xcpSettings.timeoutT6))
    {
      /* Could not send packet or receive response within the specified timeout. */
      result = false;
    }
    /* Only continue if a response was received. */
    if (result)
    {
      /* Check if the response was valid. */
      if ( (resPacket.len != 8) || (resPacket.data[0] != XCPLOADER_CMD_PID_RES) )
      {
        /* Not a valid or positive response. */
        result = false;
      }
    }
    /* Process the response data. */
    if (result)
    {
      /* Store slave's byte ordering information. */
      if ((resPacket.data[2] & 0x01) == 0)
      {
        xcpSlaveIsIntel = true;
      }
      else
      {
        xcpSlaveIsIntel = false;
      }
      /* Store max number of bytes the slave allows for master->slave packets. */
      xcpMaxCto = resPacket.data[3];
      xcpMaxProgCto = xcpMaxCto;
      /* Store max number of bytes the slave allows for slave->master packets. */
      if (xcpSlaveIsIntel)
      {
        xcpMaxDto = resPacket.data[4] + (resPacket.data[5] << 8);
      }
      else
      {
        xcpMaxDto = resPacket.data[5] + (resPacket.data[4] << 8);
      }
      /* Double check size configuration. CTO length can be adjusted. DTO not. */
      if (xcpMaxCto > XCPLOADER_PACKET_SIZE_MAX) /*lint !e685 */
      {
        xcpMaxCto = XCPLOADER_PACKET_SIZE_MAX;
      }
      if (xcpMaxDto > XCPLOADER_PACKET_SIZE_MAX)
      {
        result = false;
      }
      if ( (xcpMaxCto == 0) || (xcpMaxDto == 0) )
      {
        result = false;
      }
    }
  }  
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpLoaderSendCmdConnect ***/


/************************************************************************************//**
** \brief     Sends the XCP Get Status command. Note that it is okay to specify a NULL
**            value for the parameters if you are not interested in a particular one.
** \param     session Current session status.
** \param     protectedResources Current resource protection status.
** \param     configId Session configuration identifier.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderSendCmdGetStatus(uint8_t * session, uint8_t * protectedResources,
                                      uint16_t * configId)
{
  bool result = false;
  tXcpTransportPacket cmdPacket;
  tXcpTransportPacket resPacket;

  /* Make sure a valid transport layer is linked. */
  assert(xcpSettings.transport != NULL);

  /* Only continue with a valid transport layer. */
  if (xcpSettings.transport != NULL) /*lint !e774 */
  {
    /* Init the result value to okay and only set it to error when a problem occurred. */
    result = true;
    /* Prepare the command packet. */
    cmdPacket.data[0] = XCPLOADER_CMD_GET_STATUS;
    cmdPacket.len = 1;
    /* Send the packet. */
    if (!xcpSettings.transport->SendPacket(&cmdPacket, &resPacket,
                                           xcpSettings.timeoutT1))
    {
      /* Could not send packet or receive response within the specified timeout. */
      result = false;
    }
    /* Only continue if a response was received. */
    if (result)
    {
      /* Check if the response was valid. */
      if ( (resPacket.len != 6) || (resPacket.data[0] != XCPLOADER_CMD_PID_RES) )
      {
        /* Not a valid or positive response. */
        result = false;
      }
    }
    /* Extract and store the received status information. */
    if (result)
    {
      /* Store the current session status. */
      if (session != NULL)
      {
        *session = resPacket.data[1];
      }
      /* Store the current resource protection status. */
      if (protectedResources != NULL)
      {
        *protectedResources = resPacket.data[2];
      }
      /* Store the session configuration id. */
      if (configId != NULL)
      {
        *configId = XcpLoaderGetOrderedWord(&resPacket.data[4]);
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpLoaderSendCmdGetStatus ***/


/************************************************************************************//**
** \brief     Sends the XCP Get Seed command.
** \param     resource The resource to unlock (XCPPROTECT_RESOURCE_xxx).
** \param     mode 0 for the first part of the seed, 1 for the remaining part.
** \param     seed Pointer to byte array where the received seed is stored.
** \param     seedLen Length of the seed in bytes.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderSendCmdGetSeed(uint8_t resource, uint8_t mode, uint8_t * seed, 
                                    uint8_t * seedLen)
{
  bool result = false;
  uint8_t currentSeedLen;
  tXcpTransportPacket cmdPacket;
  tXcpTransportPacket resPacket;

  /* Make sure a valid transport layer is linked and that the parameters are valid. */
  assert(xcpSettings.transport != NULL);
  assert(seed != NULL);
  assert(seedLen != NULL);

  /* Only continue with a valid transport layer and parameters. */
  if ( (xcpSettings.transport != NULL) && (seed != NULL) &&
       (seedLen != NULL) ) /*lint !e774 */
  {
    /* Only continue with a valid resource value. */
    if ( (resource == XCPPROTECT_RESOURCE_PGM) ||
         (resource == XCPPROTECT_RESOURCE_STIM)||
         (resource == XCPPROTECT_RESOURCE_DAQ) ||
         (resource == XCPPROTECT_RESOURCE_CALPAG))
    {
      /* Init the result value to okay and only set it to error when a problem
       * occurred.
       */
      result = true;
      /* Prepare the command packet. */
      cmdPacket.data[0] = XCPLOADER_CMD_GET_SEED;
      cmdPacket.data[1] = mode;
      cmdPacket.data[2] = resource;
      cmdPacket.len = 3;
      /* Send the packet. */
      if (!xcpSettings.transport->SendPacket(&cmdPacket, &resPacket,
                                             xcpSettings.timeoutT1))
      {
        /* Could not send packet or receive response within the specified timeout. */
        result = false;
      }
      /* Only continue if a response was received. */
      if (result)
      {
        /* Check if the response was valid. */
        if ( (resPacket.len <= 2) || (resPacket.len > xcpMaxDto) ||
             (resPacket.data[0] != XCPLOADER_CMD_PID_RES) )
        {
          /* Not a valid or positive response. */
          result = false;
        }
      }
      /* Extract and store the seed. */
      if (result)
      {
        /* Store the seed length. */
        *seedLen = resPacket.data[1];
        /* Determine the number of seed bytes in the current response */
        currentSeedLen = *seedLen;
        if (currentSeedLen > ((uint8_t)xcpMaxDto - 2))
        {
          currentSeedLen = ((uint8_t)xcpMaxDto - 2);
        }
        /* Store the seed bytes. */
        for (uint8_t idx = 0; idx < currentSeedLen; idx++)
        {
          seed[idx] = resPacket.data[idx + 2];
        }
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpLoaderSendCmdGetSeed ***/


/************************************************************************************//**
** \brief     Sends the XCP Unlock command.
** \param     key Pointer to a byte array containing the key.
** \param     keyLen The length of the key in bytes.
** \param     protectedResources Current resource protection status.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderSendCmdUnlock(uint8_t const * key, uint8_t keyLen,
                                   uint8_t * protectedResources)
{
  bool result = false;
  uint8_t keyCurrentLen;
  tXcpTransportPacket cmdPacket;
  tXcpTransportPacket resPacket;

  /* Make sure a valid transport layer is linked and that the parameters are valid. */
  assert(xcpSettings.transport != NULL);
  assert(key != NULL);
  assert(keyLen > 0);
  assert(protectedResources != NULL);

  /* Only continue with a valid transport layer and parameters. */
  if ( (xcpSettings.transport != NULL) && (key != NULL) && (keyLen > 0) &&
       (protectedResources != NULL) ) /*lint !e774 */
  {
    /* Init the result value to okay and only set it to error when a problem occurred. */
    result = true;
    /* Prepare the command packet. */
    cmdPacket.data[0] = XCPLOADER_CMD_UNLOCK;
    cmdPacket.data[1] = keyLen;
    /* Determine number of key bytes for the packet. */
    keyCurrentLen = keyLen;
    if (keyCurrentLen > (xcpMaxCto - 2))
    {
      keyCurrentLen = xcpMaxCto - 2;
    }
    /* Copy key bytes. */
    for (uint8_t idx = 0; idx < keyCurrentLen; idx++)
    {
      cmdPacket.data[idx + 2] = key[idx];
    }
    cmdPacket.len = keyCurrentLen + 2;
    /* Send the packet. */
    if (!xcpSettings.transport->SendPacket(&cmdPacket, &resPacket,
                                           xcpSettings.timeoutT1))
    {
      /* Could not send packet or receive response within the specified timeout. */
      result = false;
    }
    /* Only continue if a response was received. */
    if (result)
    {
      /* Check if the response was valid. */
      if ( (resPacket.len != 2) || (resPacket.data[0] != XCPLOADER_CMD_PID_RES) )
      {
        /* Not a valid or positive response. */
        result = false;
      }
    }
    /* Store the current resource protection status. */
    if (result)
    {
      *protectedResources = resPacket.data[1];
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpLoaderSendCmdUnlock ***/


/************************************************************************************//**
** \brief     Sends the XCP Set MTA command.
** \param     address New MTA address for the slave.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderSendCmdSetMta(uint32_t address)
{
  bool result = false;
  tXcpTransportPacket cmdPacket;
  tXcpTransportPacket resPacket;

  /* Make sure a valid transport layer is linked. */
  assert(xcpSettings.transport != NULL);
  
  /* Only continue with a valid transport layer. */
  if (xcpSettings.transport != NULL) /*lint !e774 */
  {
    /* Init the result value to okay and only set it to error when a problem occurred. */
    result = true;
    /* Prepare the command packet. */
    cmdPacket.data[0] = XCPLOADER_CMD_SET_MTA;
    cmdPacket.data[1] = 0; /* Reserved. */
    cmdPacket.data[2] = 0; /* Reserved. */
    cmdPacket.data[3] = 0; /* Address extension not supported. */
    /* Set the address taking into account byte ordering. */
    XcpLoaderSetOrderedLong(address, &cmdPacket.data[4]);
    cmdPacket.len = 8;
    /* Send the packet. */
    if (!xcpSettings.transport->SendPacket(&cmdPacket, &resPacket, 
                                           xcpSettings.timeoutT1))
    {
      /* Could not send packet or receive response within the specified timeout. */
      result = false;
    }
    /* Only continue if a response was received. */
    if (result)
    {
      /* Check if the response was valid. */
      if ( (resPacket.len != 1) || (resPacket.data[0] != XCPLOADER_CMD_PID_RES) )
      {
        /* Not a valid or positive response. */
        result = false;
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpLoaderSendCmdSetMta ***/


/************************************************************************************//**
** \brief     Sends the XCP UPLOAD command.
** \param     data Destination data buffer.
** \param     length Number of bytes to upload.
** \return    SB_TRUE is successfull, SB_FALSE otherwise.
**
****************************************************************************************/
static bool XcpLoaderSendCmdUpload(uint8_t * data, uint8_t length)
{
  bool result = false;
  tXcpTransportPacket cmdPacket;
  tXcpTransportPacket resPacket;
  uint8_t dataIdx;

  /* Cannot request more data then the max rx data - 1. */
  assert(length < xcpMaxDto);
  assert(data != NULL);
  /* Make sure a valid transport layer is linked. */
  assert(xcpSettings.transport != NULL);
  
  /* Only continue with a valid transport layer and valid parameters. */
  if ( (length < xcpMaxDto) && (data != NULL) && 
       (xcpSettings.transport != NULL) ) /*lint !e774 */
  {
    /* Init the result value to okay and only set it to error when a problem occurred. */
    result = true;
    /* Prepare the command packet. */
    cmdPacket.data[0] = XCPLOADER_CMD_UPLOAD;
    cmdPacket.data[1] = length;
    cmdPacket.len = 2;
    /* Send the packet. */
    if (!xcpSettings.transport->SendPacket(&cmdPacket, &resPacket, 
                                           xcpSettings.timeoutT1))
    {
      /* Could not send packet or receive response within the specified timeout. */
      result = false;
    }
    /* Only continue if a response was received. */
    if (result)
    {
      /* Check if the response was valid. */
      if ( (resPacket.len == 0) || (resPacket.data[0] != XCPLOADER_CMD_PID_RES) )
      {
        /* Not a valid or positive response. */
        result = false;
      }
    }
    
    /* Now store the uploaded data. */
    if (result)
    {
      for (dataIdx = 0; dataIdx < length; dataIdx++)
      {
        data[dataIdx] = resPacket.data[dataIdx + 1];
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpLoaderSendCmdUpload ***/


/************************************************************************************//**
** \brief     Sends the XCP PROGRAM START command.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderSendCmdProgramStart(void)
{
  bool result = false;
  tXcpTransportPacket cmdPacket;
  tXcpTransportPacket resPacket;

  /* Make sure a valid transport layer is linked. */
  assert(xcpSettings.transport != NULL);
  
  /* Only continue with a valid transport layer. */
  if (xcpSettings.transport != NULL) /*lint !e774 */
  {
    /* Init the result value to okay and only set it to error when a problem occurred. */
    result = true;
    /* Prepare the command packet. */
    cmdPacket.data[0] = XCPLOADER_CMD_PROGRAM_START;
    cmdPacket.len = 1;
    /* Send the packet. */
    if (!xcpSettings.transport->SendPacket(&cmdPacket, &resPacket, 
                                           xcpSettings.timeoutT3))
    {
      /* Could not send packet or receive response within the specified timeout. */
      result = false;
    }
    /* Only continue if a response was received. */
    if (result)
    {
      /* Check if the response was valid. */
      if ( (resPacket.len != 7) || (resPacket.data[0] != XCPLOADER_CMD_PID_RES) )
      {
        /* Not a valid or positive response. */
        result = false;
      }
    }
    
    /* Store max number of bytes the slave allows for master->slave packets during the
     * programming session.
     */
    xcpMaxProgCto = resPacket.data[3];
    if (xcpMaxProgCto > XCPLOADER_PACKET_SIZE_MAX) /*lint !e685 */
    {
      xcpMaxProgCto = XCPLOADER_PACKET_SIZE_MAX;
    }
  }  
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpLoaderSendCmdProgramStart ***/


/************************************************************************************//**
** \brief     Sends the XCP PROGRAM RESET command. Note that this command is a bit
**            different as in it does not require a response.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderSendCmdProgramReset(void)
{
  bool result = false;
  tXcpTransportPacket cmdPacket;
  tXcpTransportPacket resPacket;

  /* Make sure a valid transport layer is linked. */
  assert(xcpSettings.transport != NULL);
  
  /* Only continue with a valid transport layer. */
  if (xcpSettings.transport != NULL) /*lint !e774 */
  {
    /* Init the result value to okay and only set it to error when a problem occurred. */
    result = true;
    /* Prepare the command packet. */
    cmdPacket.data[0] = XCPLOADER_CMD_PROGRAM_RESET;
    cmdPacket.len = 1;
    /* Send the packet. Note that it is okay if no response is received as this is 
     * allowed for the program reset command. Just make sure to only process the 
     * response if one was actually received.
     */
    if (xcpSettings.transport->SendPacket(&cmdPacket, &resPacket, 
                                           xcpSettings.timeoutT5))
    {
      /* Still here so a response was received. check if the reponse was valid. */
      if ( (resPacket.len != 1) || (resPacket.data[0] != XCPLOADER_CMD_PID_RES) )
      {
        /* Not a valid or positive response. */
        result = false;
      }
    }
  }  
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpLoaderSendCmdProgramReset ***/


/************************************************************************************//**
** \brief     Sends the XCP PROGRAM command.
** \param     length Number of bytes in the data array to program.
** \param     data Array with data bytes to program.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderSendCmdProgram(uint8_t length, uint8_t const * data)
{
  bool result = false;
  tXcpTransportPacket cmdPacket;
  tXcpTransportPacket resPacket;
  uint8_t cnt;

  /* Verify that this number of bytes actually fits in this command. */
  assert((length <= (xcpMaxProgCto-2)) && 
         (xcpMaxProgCto <= XCPLOADER_PACKET_SIZE_MAX)); /*lint !e685 */
  if (length > 0)
  {
    assert(data != NULL);
  }
  /* Make sure a valid transport layer is linked. */
  assert(xcpSettings.transport != NULL);
  
  /* Only continue with a valid transport layer and valid parameters. */
  if ( (length <= (xcpMaxProgCto-2)) && 
       (xcpMaxProgCto <= XCPLOADER_PACKET_SIZE_MAX) && /*lint !e685 */
       (xcpSettings.transport != NULL) )/*lint !e774 */ 
  {
    /* Init the result value to okay and only set it to error when a problem occurred. */
    result = true;
    /* Prepare the command packet */
    cmdPacket.data[0] = XCPLOADER_CMD_PROGRAM;
    cmdPacket.data[1] = length;
    /* Only access data if it is not a NULL pointer. */
    if (data != NULL)
    {
      for (cnt=0; cnt<length; cnt++)
      {
        cmdPacket.data[cnt+2] = data[cnt];
      }
    }
    cmdPacket.len = length + 2;
    /* Send the packet. */
    if (!xcpSettings.transport->SendPacket(&cmdPacket, &resPacket, 
                                           xcpSettings.timeoutT5))
    {
      /* Could not send packet or receive response within the specified timeout. */
      result = false;
    }
    /* Only continue if a response was received. */
    if (result)
    {
      /* Check if the response was valid. */
      if ( (resPacket.len != 1) || (resPacket.data[0] != XCPLOADER_CMD_PID_RES) )
      {
        /* Not a valid or positive response. */
        result = false;
      }
    }
  }  
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpLoaderSendCmdProgram ***/


/************************************************************************************//**
** \brief     Sends the XCP PROGRAM MAX command.
** \param     data Array with data bytes to program.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderSendCmdProgramMax(uint8_t const * data)
{
  bool result = false;
  tXcpTransportPacket cmdPacket;
  tXcpTransportPacket resPacket;
  uint8_t cnt;

  /* verify that this number of bytes actually fits in this command */
  assert(xcpMaxProgCto <= XCPLOADER_PACKET_SIZE_MAX); /*lint !e685 */
  assert(data != NULL);
  /* Only continue with a valid transport layer and valid parameters. */
  if ( (xcpMaxProgCto <= XCPLOADER_PACKET_SIZE_MAX) && (data != NULL) && /*lint !e685 */
       (xcpSettings.transport != NULL) )/*lint !e774 */
  {
    /* Init the result value to okay and only set it to error when a problem occurred. */
    result = true;
    /* Prepare the command packet. */
    cmdPacket.data[0] = XCPLOADER_CMD_PROGRAM_MAX;
    for (cnt = 0; cnt < (xcpMaxProgCto-1); cnt++)
    {
      cmdPacket.data[cnt+1] = data[cnt];
    }
    cmdPacket.len = xcpMaxProgCto;
    /* Send the packet. */
    if (!xcpSettings.transport->SendPacket(&cmdPacket, &resPacket, 
                                           xcpSettings.timeoutT5))
    {
      /* Could not send packet or receive response within the specified timeout. */
      result = false;
    }
    /* Only continue if a response was received. */
    if (result)
    {
      /* Check if the response was valid. */
      if ( (resPacket.len != 1) || (resPacket.data[0] != XCPLOADER_CMD_PID_RES) )
      {
        /* Not a valid or positive response. */
        result = false;
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpLoaderSendCmdProgramMax ***/


/************************************************************************************//**
** \brief     Sends the XCP PROGRAM CLEAR command.
** \param     length Number of elements to clear starting at the MTA address.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderSendCmdProgramClear(uint32_t length)
{
  bool result = false;
  tXcpTransportPacket cmdPacket;
  tXcpTransportPacket resPacket;

  /* Only continue with a valid transport layer. */
  if (xcpSettings.transport != NULL) /*lint !e774 */
  {
    /* Init the result value to okay and only set it to error when a problem occurred. */
    result = true;
    /*Pprepare the command packet. */
    cmdPacket.data[0] = XCPLOADER_CMD_PROGRAM_CLEAR;
    cmdPacket.data[1] = 0; /* Use absolute mode. */
    cmdPacket.data[2] = 0; /* Reserved. */
    cmdPacket.data[3] = 0; /* Reserved. */
    /* Set the erase length taking into account byte ordering. */
    XcpLoaderSetOrderedLong(length, &cmdPacket.data[4]);
    cmdPacket.len = 8;
    /* Send the packet. */
    if (!xcpSettings.transport->SendPacket(&cmdPacket, &resPacket, 
                                           xcpSettings.timeoutT4))
    {
      /* Could not send packet or receive response within the specified timeout. */
      result = false;
    }
    /* Only continue if a response was received. */
    if (result)
    {
      /* Check if the response was valid. */
      if ( (resPacket.len != 1) || (resPacket.data[0] != XCPLOADER_CMD_PID_RES) )
      {
        /* Not a valid or positive response. */
        result = false;
      }
    }
  }  
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpLoaderSendCmdProgramClear ***/


/*********************************** end of xcploader.c ********************************/
