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
#include <string.h>                         /* for string library                      */
#include "firmware.h"                       /* Firmware data module                    */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Pointer to the firmware parser that is linked. */
static tFirmwareParser const * parserPtr;

/** \brief Linked list with firmware segments. */
static tFirmwareSegment * segmentList;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void FirmwareCreateSegment(uint32_t address, uint32_t len, uint8_t const * data);
static void FirmwareDeleteSegment(tFirmwareSegment const * segment);
static void FirmwareTrimSegment(tFirmwareSegment const * segment, uint32_t address, 
                                uint32_t len);
static void FirmwareSortSegments(void);
static void FirmwareMergeSegments(void);
static uint32_t FirmwareGetFirstAddress(void);
static uint32_t FirmwareGetLastAddress(void);


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
  /* Clear all data and segments from the linked list. */
  FirmwareClearData();
  /* Unlink the firmware parser. */
  parserPtr = NULL;
} /*** end of FirmwareTerminate ***/


/************************************************************************************//**
** \brief     Uses the linked parser to load the firmware data from the specified file
**            into the linked list of segments.
** \param     firmwareFile Filename of the firmware file to load.
** \param     addressOffset Optional memory address offset to add when loading the 
**            firmware data from the file.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool FirmwareLoadFromFile(char const * firmwareFile, uint32_t addressOffset)
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
        result = parserPtr->LoadFromFile(firmwareFile, addressOffset);
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
    /* The to be added data could span several existing segments, eithe partially or 
     * completely. If is therefore faster to first remove the same range, and then
     * add the data as one new segment.
     */
    if (FirmwareRemoveData(address, len))
    {
      /* Next add the new data as a new segment. */
      FirmwareCreateSegment(address, len, data);
      /* Merge the segments after adding a new one. Note that this automatically sorts 
       * the segments as well.
       */
      FirmwareMergeSegments();
      /* Data successfully added. */
      result = true;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of FirmwareAddData ***/


/************************************************************************************//**
** \brief     Removes data from the segments that are currently present in the firmware 
**            data module. The size of a segment is automatically adjusted or removed, if
**            necessary. Note that it is safe to assume in this function that the
**            segments are already ordered in the linked list by ascending base memory 
**            address.
** \param     address Base address of the firmware data. 
** \param     len Number of bytes to remove.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool FirmwareRemoveData(uint32_t address, uint32_t len)
{
  bool result = false;
  tFirmwareSegment * startSegment = NULL;
  tFirmwareSegment * endSegment = NULL;
  uint32_t startSegmentIdx;
  uint32_t endSegmentIdx;
  tFirmwareSegment * currentSegment;
  
  /* Verify parameters. */
  assert(len > 0);
  
  /* Only continue if parameters are valid. */
  if (len > 0)
  {
    /* In case there are no segments yet in the list, there is nothing extra to do. */
    if (segmentList == NULL)
    {
      result = true;
    }
    /* At least one segment in the list. Check if the to be erased data falls outside the
     * range of the segments that are currently in the linked list. In this case there
     * is nothing extra to do.
     */
    else if ( ((address + len - 1u) < FirmwareGetFirstAddress()) ||
              (address > FirmwareGetLastAddress()) )
    {
      result = true;
    }
    /* At least one segment in the list and the to be erased data overlaps one or more
     * segments. 
     */
    else
    {
      /* Find the segment with the lowest index that contains data to remove. */
      currentSegment = segmentList;
      startSegmentIdx = 0;
      do 
      {
        /* Is there to be removed data in this segment? */
        if (address < (currentSegment->base + currentSegment->length))
        {
          /* Start segment found. Store it and stop looping. */
          startSegment = currentSegment;
          break;
        }
        /* Continue with the next segment. */
        currentSegment = currentSegment->next;
        startSegmentIdx++;
      }
      while (currentSegment != NULL);
      
      /* Find the segment with the highest index that contains data to remove. */
      currentSegment = FirmwareGetSegment(FirmwareGetSegmentCount() - 1u);
      endSegmentIdx = FirmwareGetSegmentCount() - 1u;
      do 
      {
        /* Is there to be removed data in this segment? */
        if (currentSegment->base < (address + len))
        {
          /* End segment found. Store it and stop looping. */
          endSegment = currentSegment;
          break;
        }
        /* Continue with the previous segment. */
        currentSegment = currentSegment->prev;
        endSegmentIdx--;
      }
      while (currentSegment != NULL);
      
      /* Sanity check. The start- and endSegments must be valid now. */
      assert((startSegment != NULL) && (endSegment != NULL));
      /* Only continue if segment pointers are valid. */
      if ((startSegment != NULL) && (endSegment != NULL)) /*lint !e774 */
      {
        /* Check if the endSegmentIdx is smaller than the startSegmentIdx. This indicates
         * that the to be erased data falls in a gap between 2 segments and that is
         * nothing to remove.
         */
        if (endSegmentIdx < startSegmentIdx)
        {
          /* Nothing to remove, so we are all done. */
          result = true;
        }
        else
        {
          /* Remove all segments in between the start- and endSegments, if any */
          if (startSegment != endSegment)
          {
            /* Delete the segments until the endSegment is reached. */
            while (startSegment->next != endSegment)
            {
              /* Note that this automatically updates the next-member to the segment after
              * the one that got deleted. 
              */
              FirmwareDeleteSegment(startSegment->next);
            }
          }
          /* Segments between the start- and endSegment are now removed. Now trim the
          * start- and endSegments.
          */
          FirmwareTrimSegment(startSegment, address, len); 
          /* Only trim endSegment if it is different from the startSegment. */
          if (startSegment != endSegment)
          {
            FirmwareTrimSegment(endSegment, address, len);
          }
          /* Resort the segments because new segments might have been added during
           * the trim operation.
           */
          FirmwareSortSegments();
          /* Segment removal completed successfully. */
          result = true;
        }
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of FirmwareRemoveData ***/


/************************************************************************************//**
** \brief     Clears all data and segments that are currently present in the linked list.
**
****************************************************************************************/
void FirmwareClearData(void)
{
  tFirmwareSegment * currentSegment;
  tFirmwareSegment * segmentToFree;
  
  /* Free al the segments in the segment list. */
  if (segmentList != NULL)
  {
    currentSegment = segmentList;
    do 
    {
      /* Store pointer to the segment that should be released for later usage. */
      segmentToFree = currentSegment;
      /* Move to the next segment before freeing it. */
      currentSegment = currentSegment->next;
      /* Sanity check. */
      assert(segmentToFree != NULL); 
      /* Only access the segment if it is not NULL. */
      if (segmentToFree != NULL) /*lint !e774 */
      {
        /* Free the segment data. */
        if (segmentToFree->data != NULL)
        {
          free(segmentToFree->data);
        }
        /* Free the segment. */
        free(segmentToFree);
      }
    }
    while (currentSegment != NULL);
    /* Set the segment list to empty. */
    segmentList = NULL;
  }
} /*** end of FirmwareClearData ***/


/************************************************************************************//**
** \brief     Creates and adds a new segment to the linked list. It allocates memory for
**            the segment data and copies the data to it.
** \param     address Base address of the firmware data. 
** \param     len Number of bytes to add to the new segment.
** \param     data Pointer to the byte array with data for the segment.
**
****************************************************************************************/
static void FirmwareCreateSegment(uint32_t address, uint32_t len, uint8_t const * data)
{
  /*lint -esym(593, newSegment) newSegment pointer is freed when the segment is removed
   *                             from the linked list.
   */ 
  tFirmwareSegment * newSegment; 
  tFirmwareSegment * currentSegment;
  
  /* Verify parameters. */
  assert(len > 0);
  assert(data != NULL);
  
  /* Only continue if parameters are valid. */
  if ( (len > 0) && (data != NULL) ) /*lint !e774 */
  {
    /* Allocate memory for the new segment. */
    newSegment = malloc(sizeof(tFirmwareSegment));
    /* Verify allocation result. */
    assert(newSegment != NULL);
    /* Only continue if allocation was successful. */
    if (newSegment != NULL) /*lint !e774 */
    {
      /* Allocate memory for the segment data. */
      newSegment->data = malloc(len);
      /* Verify allocation result. */
      assert(newSegment->data != NULL);
      /* Only continue if allocation was successful. */
      if (newSegment->data != NULL) /*lint !e774 */
      {
        /* Copy the data to the segment. */
        memcpy(newSegment->data, data, len);
        /* Set other segment fields. */
        newSegment->base = address;
        newSegment->length = len;
        newSegment->next = NULL;
        /* Add the new segment as the first segment if the linked list is empty. */
        if (segmentList == NULL)
        {
          newSegment->prev = NULL;
          segmentList = newSegment;
        }
        /* Add the segment to the end of the list. */
        else
        {
          /* Find the last entry in the list. */
          currentSegment = segmentList;
          while (currentSegment->next != NULL)
          {
            currentSegment = currentSegment->next;
          }
          /* Add the new segment. */
          newSegment->prev = currentSegment;
          currentSegment->next = newSegment;
        }
      }
    }
  }
} /*** end of FirmwareCreateSegment ***/


/************************************************************************************//**
** \brief     Deletes the specified segment from the linked list and handles
**            the release of the segment's allocated memory.
** \param     segment Pointer to the segment.
**
****************************************************************************************/
static void FirmwareDeleteSegment(tFirmwareSegment const * segment)
{
  tFirmwareSegment * currentSegment = NULL;

  /* Validate parameters. */
  assert(segment != NULL);
  
  /* Only continue if parameters are valid and the linked list is not empty. */
  if ( (segment != NULL) && (segmentList != NULL) ) /*lint !e774 */
  {
    /* Iterate over the segments in the linked list, just to verify that it is a valid
     * segment pointer.
     */
    currentSegment = segmentList;
    do 
    {
      /* Is this the segment that is requested to be deleted? */
      if (currentSegment == segment)
      {
        /* Is the segment the first one in the linked list? */
        if (currentSegment->prev == NULL)
        {
          /* Make the next segment the start of the linked list, if present. */
          if (currentSegment->next != NULL)
          {
            currentSegment->next->prev = NULL;
            segmentList = currentSegment->next;
          }
          /* The to be deleted segment was the only segment in the list. So set the 
           * list to empty now.
           */
          else
          {
            segmentList = NULL;
          }
        }
        /* Is the segment the last one in the linked list? Note that we already know that
         * it is not the first one due to the previous check. This automatically means
         * there are at least 2 segments in the list, so there is no need to check its
         * previous entry pointer for NULL.
         */
        else if (currentSegment->next == NULL)
        {
          /* Make the previous segment the end of the linked list. */
          currentSegment->prev->next = NULL;
        }
        /* It is a segment somewhere in the middle of the list. Note that we already know
         * that it is not the first or the last segment. This means that there are at
         * least three or more segments in the list, so there is no need to check the
         * next and previous entry pointers for NULL.
         */
        else
        {
          /* Update links of adjacent segments as to remove ourselves. */
          currentSegment->prev->next = currentSegment->next;
          currentSegment->next->prev = currentSegment->prev;
        }
        /* The segment was removed from the list. Now its allocated memory should be
         * released.
         */
        if (currentSegment->data != NULL)
        {
          free(currentSegment->data);
        }
        free(currentSegment);
        /* No need to continue looping as we are done already. */
        break;
      }
      /* Move on to the next segment. */
      currentSegment = currentSegment->next;
    }
    while (currentSegment != NULL);
  }
} /*** end of FirmwareDeleteSegment ***/


/************************************************************************************//**
** \brief     Removes the specified data range (address to address + len) from the 
**            segment. If if overlaps the entire segment, the segment will be deleted.
**            Otherwise, the segment will be trimmed and, if needed, split into multiple
**            segments.
** \param     segment Pointer to the segment to trim.
** \param     address Start address of the data that should be removed.
** \param     len Total number of data bytes that should be removed.
**
****************************************************************************************/
static void FirmwareTrimSegment(tFirmwareSegment const * segment, uint32_t address, 
                                uint32_t len)
{
  tFirmwareSegment * currentSegment = NULL;
  bool segmentValid = false;
  uint32_t newLength1, newLength2;
  uint32_t newBase1, newBase2;
  uint8_t *newData1, *newData2; 

  /* Validate parameters. */
  assert(segment != NULL);
  assert(len > 0);
  
  /* Only continue if parameters are valid and the linked list is not empty. */
  if ( (segment != NULL) && (len > 0) && (segmentList != NULL) ) /*lint !e774 */
  {
    /* Iterate over the segments in the linked list, just to verify that it is a valid
     * segment pointer.
     */
    currentSegment = segmentList;
    do 
    {
      /* Is this the segment that is requested to be trimmed? */
      if (currentSegment == segment)
      {
        /* Set flag that the specified segment is valid. */
        segmentValid = true;
        break;
      }
      /* Move on to the next segment. */
      currentSegment = currentSegment->next;
    }
    while (currentSegment != NULL);

    /* Only continue if the segment was successfully validated. */
    if (segmentValid)
    {
      /* Does the data range to trim cover the entire segment? */
      if ((address <= segment->base) &&
        ((address + len) >= (segment->base + segment->length)))
      {
        /* Delete the entire segment. */
        FirmwareDeleteSegment(segment);
      }
      /* Does the data range cover the start of the segment, but does not go all the way
      * to the end?
      */
      else if (address <= segment->base)
      {
        /* Create a new segment with the data that should remain in the segment. */
        newBase1 = address + len;
        newLength1 = (segment->base + segment->length) - (address + len);
        newData1 = &(segment->data[newBase1 - segment->base]);
        FirmwareCreateSegment(newBase1, newLength1, newData1);
        /* The original segment can now be deleted. */
        FirmwareDeleteSegment(segment);
      }
      /* does the data range cover the end of the segment, but does not go all the way to
      * the start?
      */
      else if ((address + len) >= (segment->base + segment->length))
      {
        /* Create a new segment with the data that should remain in the segment. */
        newBase1 = segment->base;
        newLength1 = address - segment->base;
        newData1 = segment->data;
        FirmwareCreateSegment(newBase1, newLength1, newData1);
        /* The original segment can now be deleted. */
        FirmwareDeleteSegment(segment);
      }
      /* The data range covers a part in the middle of the segment. A split is needed. */
      else
      {
        /* Create a new segment with the data that should remain in the segment that is
        * currently before the to be removed range and create a new segment with the data
        * that should remain in the segment that is currently after the to be removed
        * range.
        */
        newBase1 = segment->base;
        newLength1 = address - segment->base;
        newData1 = segment->data;
        newBase2 = address + len;
        newLength2 = (segment->base + segment->length) - (address + len);
        newData2 = &(segment->data[newBase2 - segment->base]);
        FirmwareCreateSegment(newBase1, newLength1, newData1);
        FirmwareCreateSegment(newBase2, newLength2, newData2);
        /* The original segment can now be deleted. */
        FirmwareDeleteSegment(segment);
      }
    }
  }
} /*** end of FirmwareTrimSegment ***/


/************************************************************************************//**
** \brief     Helper function to sort the segments in the linked list in order of
**            ascending base address. It uses a bubble sort algorithm.
**
****************************************************************************************/
static void FirmwareSortSegments(void)
{
  tFirmwareSegment * currentSegment;
  uint32_t i;
  uint32_t tempBase;  
  uint32_t tempLength;                
  uint8_t *tempData;                  
  
  /* Only continue if the list is not empty and has at least 2 segments. */
  if (FirmwareGetSegmentCount() > 1)
  {
    /* Repeat bubbling for the total amount of segments - 1. */
    for (i = 1; i < FirmwareGetSegmentCount(); i++)
    {
      /* Beging at the start of the list. */
      currentSegment = segmentList;
      while ( (currentSegment != NULL) && (currentSegment->next != NULL) )
      {
        /* Is the first one's base address higher then the next one's? */
        if (currentSegment->base > currentSegment->next->base) 
        {
          /* Swap the contents of these segments, excluding the next and prev fields. */
          tempBase = currentSegment->next->base;
          tempLength = currentSegment->next->length;                
          tempData = currentSegment->next->data;
          currentSegment->next->base = currentSegment->base;
          currentSegment->next->length = currentSegment->length;
          currentSegment->next->data = currentSegment->data;
          currentSegment->base = tempBase;
          currentSegment->length = tempLength;
          currentSegment->data = tempData;
        }
        /* Continue with the next pair. */
        currentSegment = currentSegment->next;
      }
    }
  }
} /*** end of FirmwareSortSegments ***/


/************************************************************************************//**
** \brief     Helper function to merge the segments in the linked list. When the firmware
**            data in two adjacent segments also holds an adjacent range, then the
**            firmware data from both segments are combined into one new one. Note that
**            this function only works properly if the segments are already ordered. For
**            this reasonse, the segments are explicitely sorted at the start.
**
****************************************************************************************/
static void FirmwareMergeSegments(void)
{
  tFirmwareSegment * currentSegment;
  tFirmwareSegment * removeSegment;

  /* Only continue if the list is not empty and has at least 2 segments. */
  if (FirmwareGetSegmentCount() > 1)
  {
    /* This function only works properly if the segments are correctly ordered. Perform
     * sorting here to make sure this is the case.
     */
    FirmwareSortSegments();
    /* Begin as the start of the linked list. */
    currentSegment = segmentList;
    /* Iterate through the segments until the end of the list. */
    while ( (currentSegment != NULL) && (currentSegment->next != NULL) )
    {
      /* Are these segments adjacent? */
      if ((currentSegment->base + currentSegment->length) == currentSegment->next->base)
      {
        /* Increase the size of the allocated data array such that it can hold the data
         * of both segments.
         */
        currentSegment->data = realloc(currentSegment->data, currentSegment->length + 
                                       currentSegment->next->length);
        /* Assert reallocation. */
        assert(currentSegment->data != NULL);
        /* Only continue if reallocation was successful. */
        if (currentSegment->data != NULL)
        {
          /* Append the data from the next segment. */
          memcpy(&(currentSegment->data[currentSegment->length]), 
                 currentSegment->next->data, currentSegment->next->length);
          currentSegment->length += currentSegment->next->length;
          /* Store the segment pointer that should be removed. */
          removeSegment = currentSegment->next;
          /* Remove the segment now that is has been merged with the previous one. */
          FirmwareDeleteSegment(removeSegment);
          /* After removing the current segment's higher address sibling we have a new 
           * sibling. This one could also be adjacent and in need of merging. So do not 
           * update the currentSegment pointer to the next one. Instead repeat the loop
           * iteration for the same currentSegment pointer. We just need to check that
           * the new sibling is actually there and not the end of the list. This would
           * cause a problem when accessing currentSegment->next in the while-loop 
           * condition. If there is no sibling, then we can simply stop the loop because
           * merging is all done then.
           */
          if (currentSegment->next == NULL)
          {
            /* End of the linked list reached. Merging is done. */
            break; 
          }
        }
        else
        {
          /* Merge skipped due to memory allocation issue. Just continue with the next
           * segment then.
           */
          currentSegment = currentSegment->next;
        }
      }
      else
      {
        /* Continue with the next one. */
        currentSegment = currentSegment->next;
      }
    }
  }
} /*** end of FirmwareMergeSegments ***/


/************************************************************************************//**
** \brief     Helper function to obtain the first memory address of the firmware data
**            that is present in the linked list with segments.
** \return    The first memory address.
**
****************************************************************************************/
static uint32_t FirmwareGetFirstAddress(void)
{
  uint32_t result = 0;
  tFirmwareSegment * firstSegment;
  
  /* Obtain first segment in the linked list. */
  firstSegment = FirmwareGetSegment(0u);
  /* Sanity check. Make sure a valid segment was found. */
  assert(firstSegment != NULL);
  /* Only continue if a valid segment was found. */
  if (firstSegment != NULL) /*lint !e774 */
  {
    /* Set the first address in this segment as the result. */
    result = firstSegment->base;
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of FirmwareGetFirstAddress ***/


/************************************************************************************//**
** \brief     Helper function to obtain the last memory address of the firmware data
**            that is present in the linked list with segments.
** \return    The last memory address.
**
****************************************************************************************/
static uint32_t FirmwareGetLastAddress(void)
{
  uint32_t result = 0;
  tFirmwareSegment * lastSegment;

  /* Obtain last segment in the linked list. */
  lastSegment = FirmwareGetSegment(FirmwareGetSegmentCount() - 1u);
  /* Sanity check. Make sure a valid segment was found. */
  assert(lastSegment != NULL);
  /* Only continue if a valid segment was found. */
  if (lastSegment != NULL) /*lint !e774 */
  {
    /* Calculate the last address in this segment. */
    result = lastSegment->base + lastSegment->length - 1u;
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of FirmwareGetLastAddress ***/


/*********************************** end of firmware.c *********************************/
