/************************************************************************************//**
* \file         xcploader.c
* \brief        XCP Loader module source file.
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
#include <assert.h>                         /* for assertions                          */
#include "xcploader.h"                      /* XCP loader module                       */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* XCP command codes as defined by the protocol currently supported by this module */
#define XCPLOADER_CMD_CONNECT         (0xFF)     /**< XCP connect command code.        */
#define XCPLOADER_CMD_DISCONNECT      (0xFE)     /**< XCP disconnect command code.     */
#define XCPLOADER_CMD_SET_MTA         (0xF6)     /**< XCP set mta command code.        */
#define XCPLOADER_CMD_UPLOAD          (0xF5)     /**< XCP upload command code.         */
#define XCPLOADER_CMD_PROGRAM_START   (0xD2)     /**< XCP program start command code.  */
#define XCPLOADER_CMD_PROGRAM_CLEAR   (0xD1)     /**< XCP program clear command code.  */
#define XCPLOADER_CMD_PROGRAM         (0xD0)     /**< XCP program command code.        */
#define XCPLOADER_CMD_PROGRAM_RESET   (0xCF)     /**< XCP program reset command code.  */
#define XCPLOADER_CMD_PROGRAM_MAX     (0xC9)     /**< XCP program max command code.    */

/* XCP response packet IDs as defined by the protocol. */
#define XCPLOADER_CMD_PID_RES         (0xFF)     /**< positive response                */

/** \brief Maximum timeout for the XCP connect command. */
#define XCPLOADER_CONNECT_TIMEOUT_MS  (20)

/** \brief Number of retries to connect to the XCP slave. */
#define XCPLOADER_CONNECT_RETRIES     (5)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static bool XcpLoaderSendCmdConnect(void);
static bool XcpLoaderSendCmdSetMta(uint32_t address);
static bool XcpLoaderSendCmdUpload(uint8_t *data, uint8_t length);
static bool XcpLoaderSendCmdProgramStart(void);
static bool XcpLoaderSendCmdProgramReset(void);
static bool XcpLoaderSendCmdProgram(uint8_t length, uint8_t *data);
static bool XcpLoaderSendCmdProgramMax(uint8_t *data);
static bool XcpLoaderSendCmdProgramClear(uint32_t length);
static void XcpLoaderSetOrderedLong(uint32_t value, uint8_t *data);


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Pointer to the XCP transport layer that is linked. */
static tXcpTransport const * transportPtr = NULL;

/** \brief The settings that should be used by the XCP loader. */
static tXcpSettings xcpSettings;

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


/************************************************************************************//**
** \brief     Initializes the loader module.
** \param     settings Pointer to settings structure.
** \param     transport Pointer to the transport layer to link.
** \param     tpsettings Pointer to transport layer settings structure.
** \return    None.
**
****************************************************************************************/
void XcpLoaderInit(tXcpSettings *settings, tXcpTransport const * const transport, void *tpsettings)
{
  /* verify parameters */
  assert(transport != NULL);
  assert(tpsettings != NULL);
  assert(settings != NULL);

  /* shallow copy the XCP settings for later usage */
  xcpSettings = *settings;
  /* link the XCP transport layer */
  transportPtr = transport;
  /* initialize the transport layer */
  transportPtr->Init(tpsettings);
  /* init locals */
  xcpConnected = false;
  xcpSlaveIsIntel = false;
  xcpMaxCto = 0;
  xcpMaxProgCto = 0;
  xcpMaxDto = 0;
} /*** end of XcpLoaderInit ***/


/************************************************************************************//**
** \brief     Uninitializes the loader module.
** \return    None.
**
****************************************************************************************/
void XcpLoaderDeinit(void)
{
  /* make sure the XCP transport layer is linked */
  assert(transportPtr != NULL);

  /* disconnect */
  XcpLoaderDisconnect();
  /* uninitialize the transport layer */
  transportPtr->Deinit();
  /* unlink the transport layer */
  transportPtr = NULL;
} /*** end of XcpLoaderDeinit ***/


/************************************************************************************//**
** \brief     Connect to the XCP slave.
** \return    True is successful, false otherwise.
**
****************************************************************************************/
bool XcpLoaderConnect(void)
{
  uint8_t retryCnt;

  /* make sure the XCP transport layer is linked */
  assert(transportPtr != NULL);

  /* make sure that we are disconnected before connecting */
  XcpLoaderDisconnect();

  /* connect the transport layer */
  if (!transportPtr->Connect())
  {
    return false;
  }

  /* try to connect with a finite amount of retries */
  for (retryCnt=0; retryCnt<XCPLOADER_CONNECT_RETRIES; retryCnt++)
  {
    /* send the connect command */
    if (XcpLoaderSendCmdConnect())
    {
      /* update connection state */
      xcpConnected = true;
      /* connected so no need to retry */
      return true;
    }
  }

  /* still here so could not connect to XCP slave. disconnect the transport layer */
  transportPtr->Disconnect();
  return false;
} /*** end of XcpLoaderConnect ***/


/***********************************************************************************//**
** \brief     Disconnect from the XCP slave.
** \return    None.
**
****************************************************************************************/
void XcpLoaderDisconnect(void)
{
  /* make sure the XCP transport layer is linked */
  assert(transportPtr != NULL);

  /* only disconnect if actually connected */
  if (xcpConnected)
  {
    /* send reset command instead of the disconnect. this causes the user program on the
     * slave to automatically start again if present.
     */
    XcpLoaderSendCmdProgramReset();
    /* disconnect the transport layer */
    transportPtr->Disconnect();
    /* reset connection status */
    xcpConnected = false;
  }
} /*** end of XcpLoaderDisconnect ***/


/************************************************************************************//**
** \brief     Puts a connected slave in programming session.
** \return    True is successful, false otherwise.
**
****************************************************************************************/
bool XcpLoaderStartProgrammingSession(void)
{
  /* make sure the XCP transport layer is linked */
  assert(transportPtr != NULL);

  /* place the slave in programming mode */
  return XcpLoaderSendCmdProgramStart();
} /*** end of XcpLoaderStartProgrammingSession ***/


/************************************************************************************//**
** \brief     Stops the programming session by sending a program command with size 0 and
**            then resetting the slave.
** \return    True is successful, false otherwise.
**
****************************************************************************************/
bool XcpLoaderStopProgrammingSession(void)
{
  /* make sure the XCP transport layer is linked */
  assert(transportPtr != NULL);

  /* stop programming by sending the program command with size 0 */
  return XcpLoaderSendCmdProgram(0, NULL);
} /*** end of XcpLoaderStopProgrammingSession ***/


/************************************************************************************//**
** \brief     Erases non volatile memory on the slave.
** \param     addr Base memory address for the erase operation.
** \param     len Number of bytes to erase.
** \return    True is successful, false otherwise.
**
****************************************************************************************/
bool XcpLoaderClearMemory(uint32_t addr, uint32_t len)
{
  /* make sure the XCP transport layer is linked */
  assert(transportPtr != NULL);
  /* verify parameters */
  assert(len > 0);

  /* first set the MTA pointer */
  if (!XcpLoaderSendCmdSetMta(addr))
  {
    return false;
  }
  /* now perform the erase operation */
  return XcpLoaderSendCmdProgramClear(len);
} /*** end of XcpLoaderClearMemory ***/


/***********************************************************************************//**
** \brief     Reads data from the slave's memory.
** \param     addr Base memory address for the read operation
** \param     len Number of bytes to read.
** \param     data Destination buffer for storing the read data bytes.
** \return    True is successful, false otherwise.
**
****************************************************************************************/
bool XcpLoaderReadData(uint32_t addr, uint32_t len, uint8_t *data)
{
  uint8_t currentReadCnt;
  uint32_t bufferOffset = 0;

  /* make sure the XCP transport layer is linked */
  assert(transportPtr != NULL);
  /* verify parameters */
  assert(data != NULL);
  assert(len > 0);

  /* first set the MTA pointer */
  if (!XcpLoaderSendCmdSetMta(addr))
  {
    return false;
  }
  /* perform segmented upload of the data */
  while (len > 0)
  {
    /* set the current read length to make optimal use of the available packet data. */
    currentReadCnt = len % (xcpMaxDto - 1);
    if (currentReadCnt == 0)
    {
      currentReadCnt = (xcpMaxDto - 1);
    }
    /* upload some data */
    if (!XcpLoaderSendCmdUpload(&data[bufferOffset], currentReadCnt))
    {
      return false;
    }
    /* update loop variables */
    len -= currentReadCnt;
    bufferOffset += currentReadCnt;
  }
  /* still here so all data successfully read from the slave */
  return true;
} /*** end of XcpLoaderReadData ***/


/************************************************************************************//**
** \brief     Programs data to the slave's non volatile memory. Note that it must be
**            erased first.
** \param     addr Base memory address for the program operation
** \param     len Number of bytes to program.
** \param     data Source buffer with the to be programmed bytes.
** \return    True is successful, false otherwise.
**
****************************************************************************************/
bool XcpLoaderProgramData(uint32_t addr, uint32_t len, uint8_t *data)
{
  uint8_t currentWriteCnt;
  uint32_t bufferOffset = 0;

  /* make sure the XCP transport layer is linked */
  assert(transportPtr != NULL);
  /* verify parameters */
  assert(data != NULL);
  assert(len > 0);

  /* first set the MTA pointer */
  if (!XcpLoaderSendCmdSetMta(addr))
  {
    return false;
  }
  /* perform segmented programming of the data */
  while (len > 0)
  {
    /* set the current read length to make optimal use of the available packet data. */
    currentWriteCnt = len % (xcpMaxProgCto - 1);
    if (currentWriteCnt == 0)
    {
      currentWriteCnt = (xcpMaxProgCto - 1);
    }
    /* prepare the packed data for the program command */
    if (currentWriteCnt < (xcpMaxProgCto - 1))
    {
      /* program data */
      if (!XcpLoaderSendCmdProgram(currentWriteCnt, &data[bufferOffset]))
      {
        return false;
      }
    }
    else
    {
      /* program max data */
      if (!XcpLoaderSendCmdProgramMax(&data[bufferOffset]))
      {
        return false;
      }
    }
    /* update loop variables */
    len -= currentWriteCnt;
    bufferOffset += currentWriteCnt;
  }
  /* still here so all data successfully programmed */
  return true;
} /*** end of XcpLoaderProgramData ***/


/************************************************************************************//**
** \brief     Sends the XCP Connect command.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderSendCmdConnect(void)
{
  tXcpPacket cmdPacket;
  tXcpPacket resPacket;

  /* prepare the command packet */
  cmdPacket.data[0] = XCPLOADER_CMD_CONNECT;
  cmdPacket.data[1] = 0; /* normal mode */
  cmdPacket.len = 2;

  /* send the packet */
  if (!transportPtr->SendPacket(&cmdPacket, &resPacket, XCPLOADER_CONNECT_TIMEOUT_MS))
  {
    /* could not send packet or receive response within the specified timeout */
    return false;
  }

  /* still here so a response was received. check if the reponse was valid */
  if ( (resPacket.len == 0) || (resPacket.data[0] != XCPLOADER_CMD_PID_RES) )
  {
    /* not a valid or positive response */
    return false;
  }

  /* process response data */
  if ((resPacket.data[2] & 0x01) == 0)
  {
    /* store slave's byte ordering information */
    xcpSlaveIsIntel = true;
  }
  /* store max number of bytes the slave allows for master->slave packets. */
  xcpMaxCto = resPacket.data[3];
  xcpMaxProgCto = xcpMaxCto;
  /* store max number of bytes the slave allows for slave->master packets. */
  if (xcpSlaveIsIntel)
  {
    xcpMaxDto = resPacket.data[4] + (resPacket.data[5] << 8);
  }
  else
  {
    xcpMaxDto = resPacket.data[5] + (resPacket.data[4] << 8);
  }

  /* double check size configuration */
  assert(XCPLOADER_PACKET_SIZE_MAX >= xcpMaxCto);
  assert(XCPLOADER_PACKET_SIZE_MAX >= xcpMaxDto);

  /* still here so all went well */
  return true;
} /*** end of XcpLoaderSendCmdConnect ***/


/************************************************************************************//**
** \brief     Sends the XCP Set MTA command.
** \param     address New MTA address for the slave.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderSendCmdSetMta(uint32_t address)
{
  tXcpPacket cmdPacket;
  tXcpPacket resPacket;

  /* prepare the command packet */
  cmdPacket.data[0] = XCPLOADER_CMD_SET_MTA;
  cmdPacket.data[1] = 0; /* reserved */
  cmdPacket.data[2] = 0; /* reserved */
  cmdPacket.data[3] = 0; /* address extension not supported */
  /* set the address taking into account byte ordering */
  XcpLoaderSetOrderedLong(address, &cmdPacket.data[4]);
  cmdPacket.len = 8;

  /* send the packet */
  if (!transportPtr->SendPacket(&cmdPacket, &resPacket, xcpSettings.timeoutT1))
  {
    /* could not send packet or receive response within the specified timeout */
    return false;
  }

  /* still here so a response was received. check if the reponse was valid */
  if ( (resPacket.len == 0) || (resPacket.data[0] != XCPLOADER_CMD_PID_RES) )
  {
    /* not a valid or positive response */
    return false;
  }

  /* still here so all went well */
  return true;
} /*** end of XcpLoaderSendCmdSetMta ***/


/************************************************************************************//**
** \brief     Sends the XCP UPLOAD command.
** \param     data Destination data buffer.
** \param     length Number of bytes to upload.
** \return    SB_TRUE is successfull, SB_FALSE otherwise.
**
****************************************************************************************/
static bool XcpLoaderSendCmdUpload(uint8_t *data, uint8_t length)
{
  tXcpPacket cmdPacket;
  tXcpPacket resPacket;
  uint8_t data_index;

  /* cannot request more data then the max rx data - 1 */
  assert(length < XCPLOADER_PACKET_SIZE_MAX);
  assert(data != NULL);

  /* prepare the command packet */
  cmdPacket.data[0] = XCPLOADER_CMD_UPLOAD;
  cmdPacket.data[1] = length;
  cmdPacket.len = 2;

  /* send the packet */
  if (!transportPtr->SendPacket(&cmdPacket, &resPacket, xcpSettings.timeoutT1))
  {
    /* could not send packet or receive response within the specified timeout */
    return false;
  }

  /* still here so a response was received. check if the reponse was valid */
  if ( (resPacket.len == 0) || (resPacket.data[0] != XCPLOADER_CMD_PID_RES) )
  {
    /* not a valid or positive response */
    return false;
  }

  /* now store the uploaded data */
  for (data_index=0; data_index<length; data_index++)
  {
    data[data_index] = resPacket.data[data_index+1];
  }

  /* still here so all went well */
  return true;
} /*** end of XcpLoaderSendCmdUpload ***/


/************************************************************************************//**
** \brief     Sends the XCP PROGRAM START command.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderSendCmdProgramStart(void)
{
  tXcpPacket cmdPacket;
  tXcpPacket resPacket;

  /* prepare the command packet */
  cmdPacket.data[0] = XCPLOADER_CMD_PROGRAM_START;
  cmdPacket.len = 1;

  /* send the packet */
  if (!transportPtr->SendPacket(&cmdPacket, &resPacket, xcpSettings.timeoutT3))
  {
    /* could not send packet or receive response within the specified timeout */
    return false;
  }

  /* still here so a response was received. check if the reponse was valid */
  if ( (resPacket.len == 0) || (resPacket.data[0] != XCPLOADER_CMD_PID_RES) )
  {
    /* not a valid or positive response */
    return false;
  }

  /* store max number of bytes the slave allows for master->slave packets during the
   * programming session
   */
  xcpMaxProgCto = resPacket.data[3];

  /* still here so all went well */
  return true;
} /*** end of XcpLoaderSendCmdProgramStart ***/


/************************************************************************************//**
** \brief     Sends the XCP PROGRAM RESET command. Note that this command is a bit
**            different as in it does not require a response.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderSendCmdProgramReset(void)
{
  tXcpPacket cmdPacket;
  tXcpPacket resPacket;

  /* prepare the command packet */
  cmdPacket.data[0] = XCPLOADER_CMD_PROGRAM_RESET;
  cmdPacket.len = 1;

  /* send the packet, assume the sending itself is ok and check if a response was
   * received.
   */
  if (!transportPtr->SendPacket(&cmdPacket, &resPacket, xcpSettings.timeoutT5))
  {
    /* probably no response received within the specified timeout, but that is allowed
     * for the reset command.
     */
    return true;
  }

  /* still here so a response was received. check if the reponse was valid */
  if ( (resPacket.len == 0) || (resPacket.data[0] != XCPLOADER_CMD_PID_RES) )
  {
    /* not a valid or positive response */
    return false;
  }

  /* still here so all went well */
  return true;
} /*** end of XcpLoaderSendCmdProgramReset ***/


/************************************************************************************//**
** \brief     Sends the XCP PROGRAM command.
** \param     length Number of bytes in the data array to program.
** \param     data Array with data bytes to program.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderSendCmdProgram(uint8_t length, uint8_t *data)
{
  tXcpPacket cmdPacket;
  tXcpPacket resPacket;
  uint8_t cnt;

  /* verify that this number of bytes actually first in this command */
  assert(length <= (xcpMaxProgCto-2) && (xcpMaxProgCto <= XCPLOADER_PACKET_SIZE_MAX));
  if (length > 0)
  {
    assert(data != NULL);
  }

  /* prepare the command packet */
  cmdPacket.data[0] = XCPLOADER_CMD_PROGRAM;
  cmdPacket.data[1] = length;
  for (cnt=0; cnt<length; cnt++)
  {
    cmdPacket.data[cnt+2] = data[cnt];
  }
  cmdPacket.len = length + 2;

  /* send the packet */
  if (!transportPtr->SendPacket(&cmdPacket, &resPacket, xcpSettings.timeoutT5))
  {
    /* could not send packet or receive response within the specified timeout */
    return false;
  }

  /* still here so a response was received. check if the reponse was valid */
  if ( (resPacket.len == 0) || (resPacket.data[0] != XCPLOADER_CMD_PID_RES) )
  {
    /* not a valid or positive response */
    return false;
  }

  /* still here so all went well */
  return true;
} /*** end of XcpLoaderSendCmdProgram ***/


/************************************************************************************//**
** \brief     Sends the XCP PROGRAM MAX command.
** \param     data Array with data bytes to program.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderSendCmdProgramMax(uint8_t *data)
{
  tXcpPacket cmdPacket;
  tXcpPacket resPacket;
  uint8_t cnt;

  /* verify that this number of bytes actually first in this command */
  assert(xcpMaxProgCto <= XCPLOADER_PACKET_SIZE_MAX);
  assert(data != NULL);

  /* prepare the command packet */
  cmdPacket.data[0] = XCPLOADER_CMD_PROGRAM_MAX;
  for (cnt=0; cnt<(xcpMaxProgCto-1); cnt++)
  {
    cmdPacket.data[cnt+1] = data[cnt];
  }
  cmdPacket.len = xcpMaxProgCto;

  /* send the packet */
  if (!transportPtr->SendPacket(&cmdPacket, &resPacket, xcpSettings.timeoutT5))
  {
    /* could not send packet or receive response within the specified timeout */
    return false;
  }

  /* still here so a response was received. check if the reponse was valid */
  if ( (resPacket.len == 0) || (resPacket.data[0] != XCPLOADER_CMD_PID_RES) )
  {
    /* not a valid or positive response */
    return false;
  }

  /* still here so all went well */
  return true;
} /*** end of XcpLoaderSendCmdProgramMax ***/


/************************************************************************************//**
** \brief     Sends the XCP PROGRAM CLEAR command.
** \param     length Number of elements to clear starting at the MTA address.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool XcpLoaderSendCmdProgramClear(uint32_t length)
{
  tXcpPacket cmdPacket;
  tXcpPacket resPacket;

  /* prepare the command packet */
  cmdPacket.data[0] = XCPLOADER_CMD_PROGRAM_CLEAR;
  cmdPacket.data[1] = 0; /* use absolute mode */
  cmdPacket.data[2] = 0; /* reserved */
  cmdPacket.data[3] = 0; /* reserved */
  /* set the erase length taking into account byte ordering */
  XcpLoaderSetOrderedLong(length, &cmdPacket.data[4]);
  cmdPacket.len = 8;

  /* send the packet */
  if (!transportPtr->SendPacket(&cmdPacket, &resPacket, xcpSettings.timeoutT4))
  {
    /* could not send packet or receive response within the specified timeout */
    return false;
  }

  /* still here so a response was received. check if the reponse was valid */
  if ( (resPacket.len == 0) || (resPacket.data[0] != XCPLOADER_CMD_PID_RES) )
  {
    /* not a valid or positive response */
    return false;
  }

  /* still here so all went well */
  return true;
} /*** end of XcpLoaderSendCmdProgramClear ***/


/************************************************************************************//**
** \brief     Stores a 32-bit value into a byte buffer taking into account Intel
**            or Motorola byte ordering.
** \param     value The 32-bit value to store in the buffer.
** \param     data Array to the buffer for storage.
** \return    None.
**
****************************************************************************************/
static void XcpLoaderSetOrderedLong(uint32_t value, uint8_t *data)
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
} /*** end of XcpLoaderSetOrderedLong ***/


/*********************************** end of xcploader.c ********************************/

