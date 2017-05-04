/************************************************************************************//**
* \file         firmware.c
* \brief        Firmware data module source file.
* \ingroup      Firmware
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
#include "firmware.h"                       /* Firmware data module                    */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Pointer to the firmware parser that is linked. */
static tFirmwareParser const * parserPtr;

/** \brief Linked list with firmware segments. */
static tFirmwareSegment * segmentList;


/* TODO Implement segment sort routine for the linked list. Should be called after adding
 *      and removing firmware data.
 */

/************************************************************************************//**
** \brief     Initializes the module.
** \param     parser The firmware file parser to link. It is okay to specify NULL if no
**            file parser is needed.
**
****************************************************************************************/
void FirmwareInit(tFirmwareParser const * parser)
{
  /* Link the firmware parser. */
  parserPtr = parser;
  /* Start with an empty segment list. */
  segmentList = NULL;
} /*** end of FirmwareInit ***/


/************************************************************************************//**
** \brief     Terminates the module.
**
****************************************************************************************/
void FirmwareTerminate(void)
{
  tFirmwareSegment * currentSegment;
  tFirmwareSegment * segmentToFree;
  
  /* Free al the segments in the segment list. */
  if (segmentList != NULL)
  {
    currentSegment = segmentList;
    do 
    {
      /* Store pointer to the node that should be released for later usage. */
      segmentToFree = currentSegment;
      /* Move to the next node before freeing it. */
      currentSegment = currentSegment->next;
      /* Sanity check. */
      assert(segmentToFree != NULL); 
      /* Only access the segment if it is not NULL. */
      if (segmentToFree != NULL) /*lint !e774 */
      {
        /* Free the segment. */
        if (segmentToFree->data != NULL)
        {
          free(segmentToFree->data);
        }
      }
    }
    while (currentSegment != NULL);
    /* Set the segment list to empty. */
    segmentList = NULL;
  }
  /* Unlink the firmware parser. */
  parserPtr = NULL;
} /*** end of FirmwareTerminate ***/


/************************************************************************************//**
** \brief     Uses the linked parser to load the firmware data from the specified file
**            into the linked list of segments.
** \param     firmwareFile Filename of the firmware file to load.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool FirmwareLoadFromFile(char const * firmwareFile)
{
  bool result = false;
  
  /* Verify parameters. */
  assert(firmwareFile != NULL);
  
  /* Only continue if parameters are valid. */
  if (firmwareFile != NULL) /*lint !e774 */
  {
    /* Check if a parser is linked. */
    if (parserPtr != NULL)
    {
      /* Check if a LoadFromFile method is linked. */
      if (parserPtr->LoadFromFile != NULL)
      {
        /* Request the parser to perform the load operation. */
        result = parserPtr->LoadFromFile(firmwareFile);
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of FirmwareLoadFromFile ***/


/************************************************************************************//**
** \brief     Uses the linked parser to save the dat stored in the segments of the linked
**            list to the  specified file.
** \param     firmwareFile Filename of the firmware file to write to.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool FirmwareSaveToFile(char const * firmwareFile)
{
  bool result = false;
  
  /* Verify parameters. */
  assert(firmwareFile != NULL);
  
  /* Only continue if parameters are valid. */
  if (firmwareFile != NULL) /*lint !e774 */
  {
    /* Check if a parser is linked. */
    if (parserPtr != NULL)
    {
      /* Check if a SaveToFile method is linked. */
      if (parserPtr->SaveToFile != NULL)
      {
        /* Request the parser to perform the save operation. */
        result = parserPtr->SaveToFile(firmwareFile);
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of FirmwareSaveToFile ***/


/************************************************************************************//**
** \brief     Obtains the total number of segments in the linked list with firmware data.
** \return    Total number of segments.
**
****************************************************************************************/
uint32_t FirmwareGetSegmentCount(void)
{
  tFirmwareSegment * currentSegment;
  uint32_t segmentCount = 0;
  
  /* Iterate over all segments to determine the total count. */
  if (segmentList != NULL)
  {
    currentSegment = segmentList;
    do 
    {
      segmentCount++;
      currentSegment = currentSegment->next;
    }
    while (currentSegment != NULL);
  }
  /* Give segment count back to the caller. */
  return segmentCount;
} /*** end of FirmwareGetSegmentCount ***/


/************************************************************************************//**
** \brief     Obtains the segment as the specified index from the linked list with
**            firmware data.
** \param     segmentIdx The segment index. It should be a value greater or equal to zero
**            and smaller than the value returned by \ref FirmwareGetSegmentCount.
** \return    The segment if successful, NULL otherwise.
**
****************************************************************************************/
tFirmwareSegment * FirmwareGetSegment(uint32_t segmentIdx)
{
  tFirmwareSegment * currentSegment = NULL;
  uint32_t currentIdx;
  
  /* Validate parameters. */
  assert(segmentIdx < FirmwareGetSegmentCount());
  
  /* Only continue if parameters are valid. */
  if (segmentIdx < FirmwareGetSegmentCount())
  {
    /* Iterate over the segments until the specified index is found. */
    if (segmentList != NULL)
    {
      currentSegment = segmentList;
      for (currentIdx = 0; currentIdx < segmentIdx; currentIdx++)
      {
        /* Move on to the next segment. */
        currentSegment = currentSegment->next;
        /* Make sure the segment is valid. */
        assert(currentSegment != NULL);
        if (currentSegment == NULL) /*lint !e774 */
        {
          /* The specified index tries to index a non-existing segment. Abort. */
          break;
        }
      }
    }
  }
  /* Give the requested segment back to the caller, if found. */
  return currentSegment;
} /*** end of FirmwareGetSegment ***/


/************************************************************************************//**
** \brief     Adds data to the segments that are currently present in the firmware data
**            module. If the data overlaps with already existing data, the existing data
**            gets overwritten. The size of a segment is automatically adjusted or a new
**            segment gets created, if necessary.
** \param     address Base address of the firmware data. 
** \param     len Number of bytes to add.
** \param     data Pointer to array with data bytes that should be added.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool FirmwareAddData(uint32_t address, uint32_t len, uint8_t const * data)
{
  bool result = false;


  /* Verify parameters. */
  assert(len > 0);
  assert(data != NULL);
  
  (void)address;

  /* Only continue if parameters are valid. */
  if ( (len > 0) && (data != NULL) ) /*lint !e774 */
  {
    /* TODO Implement. */
    result = true;
  }

  return result;
} /*** end of FirmwareAddData ***/


/************************************************************************************//**
** \brief     Removes data from the segments that are currently present in the firmware 
**            data module. The size of a segment is automatically adjusted or removed, if
**            necessary.
** \param     address Base address of the firmware data. 
** \param     len Number of bytes to remove.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool FirmwareRemoveData(uint32_t address, uint32_t len)
{
  bool result = false;
  
    /* Verify parameters. */
  assert(len > 0);
  
  (void)address;

  /* Only continue if parameters are valid. */
  if (len > 0)
  {
    /* TODO Implement. */
    result = true;
  }

  return result;
} /*** end of FirmwareRemoveData ***/


/*********************************** end of firmware.c *********************************/
