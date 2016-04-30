/************************************************************************************//**
* \file         xcpmaster.c
* \brief        XCP Master source file.
* \ingroup      SerialBoot
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2014  by Feaser    http://www.feaser.com    All rights reserved
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
#include <assert.h>                                   /* assertion module              */
#include <sb_types.h>                                 /* C types                       */
#include "xcpmaster.h"                                /* XCP master protocol module    */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* XCP command codes as defined by the protocol currently supported by this module */
#define XCP_MASTER_CMD_CONNECT         (0xFF)
#define XCP_MASTER_CMD_DISCONNECT      (0xFE)
#define XCP_MASTER_CMD_SET_MTA         (0xF6)
#define XCP_MASTER_CMD_UPLOAD          (0xF5)
#define XCP_MASTER_CMD_PROGRAM_START   (0xD2)
#define XCP_MASTER_CMD_PROGRAM_CLEAR   (0xD1)
#define XCP_MASTER_CMD_PROGRAM         (0xD0)
#define XCP_MASTER_CMD_PROGRAM_RESET   (0xCF)
#define XCP_MASTER_CMD_PROGRAM_MAX     (0xC9)

/* XCP response packet IDs as defined by the protocol */
#define XCP_MASTER_CMD_PID_RES         (0xFF) /* positive response */

/* timeout values */
#define XCP_MASTER_CONNECT_TIMEOUT_MS  (20)
#define XCP_MASTER_TIMEOUT_T1_MS       (1000)  /* standard command timeout */
#define XCP_MASTER_TIMEOUT_T2_MS       (2000)  /* build checksum timeout */
#define XCP_MASTER_TIMEOUT_T3_MS       (2000)  /* program start timeout */
#define XCP_MASTER_TIMEOUT_T4_MS       (10000) /* erase timeout */
#define XCP_MASTER_TIMEOUT_T5_MS       (1000)  /* write and reset timeout */
#define XCP_MASTER_TIMEOUT_T6_MS       (1000)  /* user specific connect connect timeout */
#define XCP_MASTER_TIMEOUT_T7_MS       (2000)  /* wait timer timeout */

/** \brief Number of retries to connect to the XCP slave. */
#define XCP_MASTER_CONNECT_RETRIES     (5)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static sb_uint8 XcpMasterSendCmdConnect(void);
static sb_uint8 XcpMasterSendCmdSetMta(sb_uint32 address);
static sb_uint8 XcpMasterSendCmdUpload(sb_uint8 data[], sb_uint8 length);
static sb_uint8 XcpMasterSendCmdProgramStart(void);
static sb_uint8 XcpMasterSendCmdProgramReset(void);
static sb_uint8 XcpMasterSendCmdProgram(sb_uint8 length, sb_uint8 data[]);
static sb_uint8 XcpMasterSendCmdProgramMax(sb_uint8 data[]);
static sb_uint8 XcpMasterSendCmdProgramClear(sb_uint32 length);
static void     XcpMasterSetOrderedLong(sb_uint32 value, sb_uint8 data[]);


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Store the byte ordering of the XCP slave. */
static sb_uint8 xcpSlaveIsIntel = SB_FALSE;

/** \brief The max number of bytes in the command transmit object (master->slave). */
static sb_uint8 xcpMaxCto;

/** \brief The max number of bytes in the command transmit object (master->slave) during
 *         a programming session.
 */
static sb_uint8 xcpMaxProgCto = 0;

/** \brief The max number of bytes in the data transmit object (slave->master). */
static sb_uint8 xcpMaxDto;

/** \brief Internal data buffer for storing the data of the XCP response packet. */
static tXcpTransportResponsePacket responsePacket;


/************************************************************************************//**
** \brief     Initializes the XCP master protocol layer.
** \param     device Serial communication device name. For example "COM4".
** \param     baudrate Communication speed in bits/sec.
** \return    SB_TRUE is successful, SB_FALSE otherwise.
**
****************************************************************************************/
sb_uint8 XcpMasterInit(sb_char *device, sb_uint32 baudrate)
{
  /* initialize the underlying transport layer that is used for the communication */
  return XcpTransportInit(device, baudrate);
} /*** end of XcpMasterInit ***/


/************************************************************************************//**
** \brief     Uninitializes the XCP master protocol layer.
** \return    none.
**
****************************************************************************************/
void XcpMasterDeinit(void)
{
  XcpTransportClose();
} /*** end of XcpMasterDeinit ***/


/************************************************************************************//**
** \brief     Connect to the XCP slave.
** \return    SB_TRUE is successfull, SB_FALSE otherwise.
**
****************************************************************************************/
sb_uint8 XcpMasterConnect(void)
{
  sb_uint8 cnt;
  
  /* try to connect with a finite amount of retries */
  for (cnt=0; cnt<XCP_MASTER_CONNECT_RETRIES; cnt++)
  {
    /* send the connect command */
    if (XcpMasterSendCmdConnect() == SB_TRUE)
    {
      /* connected so no need to retry */
      return SB_TRUE;
    }
  }
  /* still here so could not connect */
  return SB_FALSE;
} /*** end of XcpMasterConnect ***/


/************************************************************************************//**
** \brief     Disconnect the slave.
** \return    SB_TRUE is successfull, SB_FALSE otherwise.
**
****************************************************************************************/
sb_uint8 XcpMasterDisconnect(void)
{
  /* send reset command instead of the disconnect. this causes the user program on the
   * slave to automatically start again if present.
   */
  return XcpMasterSendCmdProgramReset();
} /*** end of XcpMasterDisconnect ***/

/************************************************************************************//**
** \brief     Puts a connected slave in programming session.
** \return    SB_TRUE is successfull, SB_FALSE otherwise.
**
****************************************************************************************/
sb_uint8 XcpMasterStartProgrammingSession(void)
{
  /* place the slave in programming mode */
  return XcpMasterSendCmdProgramStart();
} /*** end of XcpMasterStartProgrammingSession ***/


/************************************************************************************//**
** \brief     Stops the programming session by sending a program command with size 0 and
**            then resetting the slave.
** \return    SB_TRUE is successfull, SB_FALSE otherwise.
**
****************************************************************************************/
sb_uint8 XcpMasterStopProgrammingSession(void)
{
  /* stop programming by sending the program command with size 0 */
  if (XcpMasterSendCmdProgram(0, SB_NULL) == SB_FALSE)
  {
    return SB_FALSE;
  }
  /* request a reset of the slave */
  return XcpMasterSendCmdProgramReset();
} /*** end of XcpMasterStopProgrammingSession ***/


/************************************************************************************//**
** \brief     Erases non volatile memory on the slave.
** \param     addr Base memory address for the erase operation.
** \param     len Number of bytes to erase.
** \return    SB_TRUE is successfull, SB_FALSE otherwise.
**
****************************************************************************************/
sb_uint8 XcpMasterClearMemory(sb_uint32 addr, sb_uint32 len)
{
  /* first set the MTA pointer */
  if (XcpMasterSendCmdSetMta(addr) == SB_FALSE)
  {
    return SB_FALSE;
  }
  /* now perform the erase operation */
  return XcpMasterSendCmdProgramClear(len);
} /*** end of XcpMasterClearMemory ***/


/************************************************************************************//**
** \brief     Reads data from the slave's memory.
** \param     addr Base memory address for the read operation
** \param     len Number of bytes to read.
** \param     data Destination buffer for storing the read data bytes.
** \return    SB_TRUE is successfull, SB_FALSE otherwise.
**
****************************************************************************************/
sb_uint8 XcpMasterReadData(sb_uint32 addr, sb_uint32 len, sb_uint8 data[])
{
  sb_uint8 currentReadCnt;
  sb_uint32 bufferOffset = 0;

  /* first set the MTA pointer */
  if (XcpMasterSendCmdSetMta(addr) == SB_FALSE)
  {
    return SB_FALSE;
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
    if (XcpMasterSendCmdUpload(&data[bufferOffset], currentReadCnt) == SB_FALSE)
    {
      return SB_FALSE;
    }
    /* update loop variables */
    len -= currentReadCnt;
    bufferOffset += currentReadCnt;
  }
  /* still here so all data successfully read from the slave */
  return SB_TRUE;
} /*** end of XcpMasterReadData ***/


/************************************************************************************//**
** \brief     Programs data to the slave's non volatile memory. Note that it must be
**            erased first.
** \param     addr Base memory address for the program operation
** \param     len Number of bytes to program.
** \param     data Source buffer with the to be programmed bytes.
** \return    SB_TRUE is successfull, SB_FALSE otherwise.
**
****************************************************************************************/
sb_uint8 XcpMasterProgramData(sb_uint32 addr, sb_uint32 len, sb_uint8 data[])
{
  sb_uint8 currentWriteCnt;
  sb_uint32 bufferOffset = 0;

  /* first set the MTA pointer */
  if (XcpMasterSendCmdSetMta(addr) == SB_FALSE)
  {
    return SB_FALSE;
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
      if (XcpMasterSendCmdProgram(currentWriteCnt, &data[bufferOffset]) == SB_FALSE)
      {
        return SB_FALSE;
      }
    }
    else
    {
      /* program max data */
      if (XcpMasterSendCmdProgramMax(&data[bufferOffset]) == SB_FALSE)
      {
        return SB_FALSE;
      }
    }
    /* update loop variables */
    len -= currentWriteCnt;
    bufferOffset += currentWriteCnt;
  }
  /* still here so all data successfully programmed */
  return SB_TRUE;
} /*** end of XcpMasterProgramData ***/


/************************************************************************************//**
** \brief     Sends the XCP Connect command.
** \return    SB_TRUE is successfull, SB_FALSE otherwise.
**
****************************************************************************************/
static sb_uint8 XcpMasterSendCmdConnect(void)
{
  sb_uint8 packetData[2];
  tXcpTransportResponsePacket *responsePacketPtr;
  
  /* prepare the command packet */
  packetData[0] = XCP_MASTER_CMD_CONNECT;
  packetData[1] = 0; /* normal mode */
  
  /* send the packet */
  if (XcpTransportSendPacket(packetData, 2, XCP_MASTER_CONNECT_TIMEOUT_MS) == SB_FALSE)
  {
    /* cound not set packet or receive response within the specified timeout */
    return SB_FALSE;
  }
  /* still here so a response was received */
  responsePacketPtr = XcpTransportReadResponsePacket();
  
  /* check if the reponse was valid */
  if ( (responsePacketPtr->len == 0) || (responsePacketPtr->data[0] != XCP_MASTER_CMD_PID_RES) )
  {
    /* not a valid or positive response */
    return SB_FALSE;
  }
  
  /* process response data */
  if ((responsePacketPtr->data[2] & 0x01) == 0)
  {
    /* store slave's byte ordering information */
    xcpSlaveIsIntel = SB_TRUE;
  }
  /* store max number of bytes the slave allows for master->slave packets. */
  xcpMaxCto = responsePacketPtr->data[3];
  xcpMaxProgCto = xcpMaxCto;
  /* store max number of bytes the slave allows for slave->master packets. */
  if (xcpSlaveIsIntel == SB_TRUE)
  {
    xcpMaxDto = responsePacketPtr->data[4] + (responsePacketPtr->data[5] << 8);
  }
  else
  {
    xcpMaxDto = responsePacketPtr->data[5] + (responsePacketPtr->data[4] << 8);
  }
  
  /* double check size configuration of the master */
  assert(XCP_MASTER_TX_MAX_DATA >= xcpMaxCto);
  assert(XCP_MASTER_RX_MAX_DATA >= xcpMaxDto);
  
  /* still here so all went well */  
  return SB_TRUE;
} /*** end of XcpMasterSendCmdConnect ***/


/************************************************************************************//**
** \brief     Sends the XCP Set MTA command.
** \param     address New MTA address for the slave.
** \return    SB_TRUE is successfull, SB_FALSE otherwise.
**
****************************************************************************************/
static sb_uint8 XcpMasterSendCmdSetMta(sb_uint32 address)
{
  sb_uint8 packetData[8];
  tXcpTransportResponsePacket *responsePacketPtr;
  
  /* prepare the command packet */
  packetData[0] = XCP_MASTER_CMD_SET_MTA;
  packetData[1] = 0; /* reserved */
  packetData[2] = 0; /* reserved */
  packetData[3] = 0; /* address extension not supported */
  
  /* set the address taking into account byte ordering */
  XcpMasterSetOrderedLong(address, &packetData[4]);
  
  /* send the packet */
  if (XcpTransportSendPacket(packetData, 8, XCP_MASTER_TIMEOUT_T1_MS) == SB_FALSE)
  {
    /* cound not set packet or receive response within the specified timeout */
    return SB_FALSE;
  }
  /* still here so a response was received */
  responsePacketPtr = XcpTransportReadResponsePacket();
  
  /* check if the reponse was valid */
  if ( (responsePacketPtr->len == 0) || (responsePacketPtr->data[0] != XCP_MASTER_CMD_PID_RES) )
  {
    /* not a valid or positive response */
    return SB_FALSE;
  }
  
  /* still here so all went well */  
  return SB_TRUE;
} /*** end of XcpMasterSendCmdSetMta ***/


/************************************************************************************//**
** \brief     Sends the XCP UPLOAD command.
** \param     data Destination data buffer.
** \param     length Number of bytes to upload.
** \return    SB_TRUE is successfull, SB_FALSE otherwise.
**
****************************************************************************************/
static sb_uint8 XcpMasterSendCmdUpload(sb_uint8 data[], sb_uint8 length)
{
  sb_uint8 packetData[2];
  tXcpTransportResponsePacket *responsePacketPtr;
  sb_uint8 data_index;
  
  /* cannot request more data then the max rx data - 1 */
  assert(length < XCP_MASTER_RX_MAX_DATA);
  
  /* prepare the command packet */
  packetData[0] = XCP_MASTER_CMD_UPLOAD;
  packetData[1] = length;

  /* send the packet */
  if (XcpTransportSendPacket(packetData, 2, XCP_MASTER_TIMEOUT_T1_MS) == SB_FALSE)
  {
    /* cound not set packet or receive response within the specified timeout */
    return SB_FALSE;
  }
  /* still here so a response was received */
  responsePacketPtr = XcpTransportReadResponsePacket();
  
  /* check if the reponse was valid */
  if ( (responsePacketPtr->len == 0) || (responsePacketPtr->data[0] != XCP_MASTER_CMD_PID_RES) )
  {
    /* not a valid or positive response */
    return SB_FALSE;
  }
  
  /* now store the uploaded data */
  for (data_index=0; data_index<length; data_index++)
  {
    data[data_index] = responsePacketPtr->data[data_index+1];
  }
  
  /* still here so all went well */  
  return SB_TRUE;
} /*** end of XcpMasterSendCmdUpload ***/


/************************************************************************************//**
** \brief     Sends the XCP PROGRAM START command.
** \return    SB_TRUE is successfull, SB_FALSE otherwise.
**
****************************************************************************************/
static sb_uint8 XcpMasterSendCmdProgramStart(void)
{
  sb_uint8 packetData[1];
  tXcpTransportResponsePacket *responsePacketPtr;

  /* prepare the command packet */
  packetData[0] = XCP_MASTER_CMD_PROGRAM_START;

  /* send the packet */
  if (XcpTransportSendPacket(packetData, 1, XCP_MASTER_TIMEOUT_T3_MS) == SB_FALSE)
  {
    /* cound not set packet or receive response within the specified timeout */
    return SB_FALSE;
  }
  /* still here so a response was received */
  responsePacketPtr = XcpTransportReadResponsePacket();
  
  /* check if the reponse was valid */
  if ( (responsePacketPtr->len == 0) || (responsePacketPtr->data[0] != XCP_MASTER_CMD_PID_RES) )
  {
    /* not a valid or positive response */
    return SB_FALSE;
  }
  
  /* store max number of bytes the slave allows for master->slave packets during the
   * programming session
   */
  xcpMaxProgCto = responsePacketPtr->data[3];
  
  /* still here so all went well */  
  return SB_TRUE;
} /*** end of XcpMasterSendCmdProgramStart ***/


/************************************************************************************//**
** \brief     Sends the XCP PROGRAM RESET command. Note that this command is a bit 
**            different as in it does not require a response.
** \return    SB_TRUE is successfull, SB_FALSE otherwise.
**
****************************************************************************************/
static sb_uint8 XcpMasterSendCmdProgramReset(void)
{
  sb_uint8 packetData[1];
  tXcpTransportResponsePacket *responsePacketPtr;

  /* prepare the command packet */
  packetData[0] = XCP_MASTER_CMD_PROGRAM_RESET;

  /* send the packet, assume the sending itself is ok and check if a response was
   * received.
   */
  if (XcpTransportSendPacket(packetData, 1, XCP_MASTER_TIMEOUT_T5_MS) == SB_FALSE)
  {
    /* probably no response received within the specified timeout, but that is allowed
     * for the reset command.
     */
    return SB_TRUE;
  }
  /* still here so a response was received */
  responsePacketPtr = XcpTransportReadResponsePacket();
  
  /* check if the reponse was valid */
  if ( (responsePacketPtr->len == 0) || (responsePacketPtr->data[0] != XCP_MASTER_CMD_PID_RES) )
  {
    /* not a valid or positive response */
    return SB_FALSE;
  }
  
  /* still here so all went well */  
  return SB_TRUE;
} /*** end of XcpMasterSendCmdProgramReset ***/


/************************************************************************************//**
** \brief     Sends the XCP PROGRAM command.
** \param     length Number of bytes in the data array to program.
** \param     data Array with data bytes to program.
** \return    SB_TRUE is successfull, SB_FALSE otherwise.
**
****************************************************************************************/
static sb_uint8 XcpMasterSendCmdProgram(sb_uint8 length, sb_uint8 data[])
{
  sb_uint8 packetData[XCP_MASTER_TX_MAX_DATA];
  tXcpTransportResponsePacket *responsePacketPtr;
  sb_uint8 cnt;
  
  /* verify that this number of bytes actually first in this command */
  assert(length <= (xcpMaxProgCto-2) && (xcpMaxProgCto <= XCP_MASTER_TX_MAX_DATA));
  
  /* prepare the command packet */
  packetData[0] = XCP_MASTER_CMD_PROGRAM;
  packetData[1] = length;
  for (cnt=0; cnt<length; cnt++)
  {
    packetData[cnt+2] = data[cnt];
  }

  /* send the packet */
  if (XcpTransportSendPacket(packetData, length+2, XCP_MASTER_TIMEOUT_T5_MS) == SB_FALSE)
  {
    /* cound not set packet or receive response within the specified timeout */
    return SB_FALSE;
  }
  /* still here so a response was received */
  responsePacketPtr = XcpTransportReadResponsePacket();
  
  /* check if the reponse was valid */
  if ( (responsePacketPtr->len == 0) || (responsePacketPtr->data[0] != XCP_MASTER_CMD_PID_RES) )
  {
    /* not a valid or positive response */
    return SB_FALSE;
  }
  
  /* still here so all went well */  
  return SB_TRUE;
} /*** end of XcpMasterSendCmdProgram ***/


/************************************************************************************//**
** \brief     Sends the XCP PROGRAM MAX command.
** \param     data Array with data bytes to program.
** \return    SB_TRUE is successfull, SB_FALSE otherwise.
**
****************************************************************************************/
static sb_uint8 XcpMasterSendCmdProgramMax(sb_uint8 data[])
{
  sb_uint8 packetData[XCP_MASTER_TX_MAX_DATA];
  tXcpTransportResponsePacket *responsePacketPtr;
  sb_uint8 cnt;
  
  /* verify that this number of bytes actually first in this command */
  assert(xcpMaxProgCto <= XCP_MASTER_TX_MAX_DATA);
  
  /* prepare the command packet */
  packetData[0] = XCP_MASTER_CMD_PROGRAM_MAX;
  for (cnt=0; cnt<(xcpMaxProgCto-1); cnt++)
  {
    packetData[cnt+1] = data[cnt];
  }

  /* send the packet */
  if (XcpTransportSendPacket(packetData, xcpMaxProgCto, XCP_MASTER_TIMEOUT_T5_MS) == SB_FALSE)
  {
    /* cound not set packet or receive response within the specified timeout */
    return SB_FALSE;
  }
  /* still here so a response was received */
  responsePacketPtr = XcpTransportReadResponsePacket();
  
  /* check if the reponse was valid */
  if ( (responsePacketPtr->len == 0) || (responsePacketPtr->data[0] != XCP_MASTER_CMD_PID_RES) )
  {
    /* not a valid or positive response */
    return SB_FALSE;
  }
  
  /* still here so all went well */  
  return SB_TRUE;
} /*** end of XcpMasterSendCmdProgramMax ***/


/************************************************************************************//**
** \brief     Sends the XCP PROGRAM CLEAR command.
** \return    SB_TRUE is successfull, SB_FALSE otherwise.
**
****************************************************************************************/
static sb_uint8 XcpMasterSendCmdProgramClear(sb_uint32 length)
{
  sb_uint8 packetData[8];
  tXcpTransportResponsePacket *responsePacketPtr;

  /* prepare the command packet */
  packetData[0] = XCP_MASTER_CMD_PROGRAM_CLEAR;
  packetData[1] = 0; /* use absolute mode */
  packetData[2] = 0; /* reserved */
  packetData[3] = 0; /* reserved */

  /* set the erase length taking into account byte ordering */
  XcpMasterSetOrderedLong(length, &packetData[4]);


  /* send the packet */
  if (XcpTransportSendPacket(packetData, 8, XCP_MASTER_TIMEOUT_T4_MS) == SB_FALSE)
  {
    /* cound not set packet or receive response within the specified timeout */
    return SB_FALSE;
  }
  /* still here so a response was received */
  responsePacketPtr = XcpTransportReadResponsePacket();
  
  /* check if the reponse was valid */
  if ( (responsePacketPtr->len == 0) || (responsePacketPtr->data[0] != XCP_MASTER_CMD_PID_RES) )
  {
    /* not a valid or positive response */
    return SB_FALSE;
  }
  
  /* still here so all went well */  
  return SB_TRUE;
} /*** end of XcpMasterSendCmdProgramClear ***/


/************************************************************************************//**
** \brief     Stores a 32-bit value into a byte buffer taking into account Intel
**            or Motorola byte ordering.
** \param     value The 32-bit value to store in the buffer.
** \param     data Array to the buffer for storage.
** \return    none.
**
****************************************************************************************/
static void XcpMasterSetOrderedLong(sb_uint32 value, sb_uint8 data[])
{
  if (xcpSlaveIsIntel == SB_TRUE)
  {
    data[3] = (sb_uint8)(value >> 24);
    data[2] = (sb_uint8)(value >> 16);
    data[1] = (sb_uint8)(value >>  8);
    data[0] = (sb_uint8)value;
  }
  else
  {
    data[0] = (sb_uint8)(value >> 24);
    data[1] = (sb_uint8)(value >> 16);
    data[2] = (sb_uint8)(value >>  8);
    data[3] = (sb_uint8)value;
  }
} /*** end of XcpMasterSetOrderedLong ***/


/*********************************** end of xcpmaster.c ********************************/
