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
#include "xcploader.h"                      /* XCP loader module                       */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void XcpLoaderInit(void const * settings);
static void XcpLoaderTerminate(void);
static bool XcpLoaderStart(void);
static void XcpLoaderStop(void);
static bool XcpLoaderClearMemory(uint32_t address, uint32_t len);
static bool XcpLoaderWriteData(uint32_t address, uint32_t len, uint8_t const * data);  
static bool XcpLoaderReadData(uint32_t address, uint32_t len, uint8_t * data);


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
  /* Check parameters. */
  assert(settings != NULL);
  
  /* Only continue with valid parameters. */
  if (settings != NULL) /*lint !e774 */
  {
    /* TODO Implement. */
  }
} /*** end of XcpLoaderInit ***/


/************************************************************************************//**
** \brief     Terminates the protocol module.
**
****************************************************************************************/
static void XcpLoaderTerminate(void)
{
  /* TODO Implement. */
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
   
   /* TODO Implement. */
   
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
   /* TODO Implement. */
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
  
  /* Only continue if the parameters are vald. */
  if (len > 0)
  {
    /* TODO Implement. */
    (void)address;
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

  /* Check parameters. */
  assert(data != NULL);
  assert(len > 0);
  
  /* Only continue if the parameters are valid. */
  if ( (data != NULL) && (len > 0) ) /*lint !e774 */
  {
    /* TODO Implement. */
    (void)address;
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

  /* Check parameters. */
  assert(data != NULL);
  assert(len > 0);
  
  /* Only continue if the parameters are valid. */
  if ( (data != NULL) && (len > 0) ) /*lint !e774 */
  {
    /* TODO Implement. */
    (void)address;
    data[0] = 0;
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XcpLoaderReadData ***/


/*********************************** end of xcploader.c ********************************/
