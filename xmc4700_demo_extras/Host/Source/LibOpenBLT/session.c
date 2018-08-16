/************************************************************************************//**
* \file         session.c
* \brief        Communication session module source file.
* \ingroup      Session
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
#include "session.h"                        /* Communication session module            */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Pointer to the communication protocol that is linked. */
static tSessionProtocol const * protocolPtr;


/************************************************************************************//**
** \brief     Initializes the communication session module for the specified protocol.
** \param     protocol The session protocol module to link.
** \param     protocolSettings Pointer to structure with protocol specific settings.
**
****************************************************************************************/
void SessionInit(tSessionProtocol const * protocol, void const * protocolSettings)
{
  /* Check parameters. Note that the protocolSettings parameter is allowed to be NULL,
   * because not every protocol might need additional settings. 
   */
  assert(protocol != NULL);
  
  /* Link the protocol module. */
  protocolPtr = protocol;
  
  /* Initialize the protocol and pass on the settings pointer. */
  if (protocolPtr != NULL)
  {
    protocolPtr->Init(protocolSettings);
  }
} /*** end of SessionInit ***/


/************************************************************************************//**
** \brief     Terminates the communication session module.
**
****************************************************************************************/
void SessionTerminate(void)
{
  /* Terminate the linked protocol. */
  if (protocolPtr != NULL) 
  {
    protocolPtr->Terminate();
  }
  /* Unlink the protocol module. */
  protocolPtr = NULL;
} /*** end of SessionTerminate ***/


/************************************************************************************//**
** \brief     Starts the firmware update session. This is where the connection with the
**            target is made and the bootloader on the target is activated.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool SessionStart(void)
{
  bool result = false;
  
  /* Pass the request on to the linked protocol module. */
  if (protocolPtr != NULL)
  {
    result = protocolPtr->Start();
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of SessionStart ***/


/************************************************************************************//**
** \brief    Stops the firmware update. This is where the bootloader starts the user
**           program on the target if a valid one is present. After this the connection
**           with the target is severed.
**
****************************************************************************************/
void SessionStop(void)
{
  /* Pass the request on to the linked protocol module. */
  if (protocolPtr != NULL)
  {
    protocolPtr->Stop();
  }
} /*** end of SessionStop ***/


/************************************************************************************//**
** \brief     Requests the bootloader to erase the specified range of memory on the
**            target. The bootloader aligns this range to hardware specified erase 
**            blocks.
** \param     address The starting memory address for the erase operation.
** \param     len The total number of bytes to erase from memory.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool SessionClearMemory(uint32_t address, uint32_t len)
{
  bool result = false;
  
  /* Check parameters. */
  assert(len > 0);
  
  /* Only continue if the parameters are vald. */
  if (len > 0)
  {
    /* Pass the request on to the linked protocol module. */
    result = protocolPtr->ClearMemory(address, len);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of SessionClearMemory ***/


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
bool SessionWriteData(uint32_t address, uint32_t len, uint8_t const * data)
{
  bool result = false;

  /* Check parameters. */
  assert(data != NULL);
  assert(len > 0);
  
  /* Only continue if the parameters are valid. */
  if ( (data != NULL) && (len > 0) ) /*lint !e774 */
  {
    /* Pass the request on to the linked protocol module. */
    result = protocolPtr->WriteData(address, len, data);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of SessionWriteData ***/


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
bool SessionReadData(uint32_t address, uint32_t len, uint8_t * data)
{
  bool result = false;

  /* Check parameters. */
  assert(data != NULL);
  assert(len > 0);
  
  /* Only continue if the parameters are valid. */
  if ( (data != NULL) && (len > 0) ) /*lint !e774 */
  {
    /* Pass the request on to the linked protocol module. */
    result = protocolPtr->ReadData(address, len, data);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of SessionReadData ***/

/*********************************** end of session.c **********************************/
