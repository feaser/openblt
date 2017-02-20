/************************************************************************************//**
* \file         srecparser.c
* \brief        S-record parser source file.
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
#include <stdio.h>                          /* for standard I/O library                */
#include <stdlib.h>                         /* for standard library                    */
#include <string.h>                         /* for string library                      */
#include <ctype.h>                          /* for character testing                   */
#include <assert.h>                         /* for assertions                          */
#include "srecparser.h"                     /* S-record parser                         */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Maximum number of characters that can be on a line in the firmware file. */
#define SRECORD_MAX_CHARS_PER_LINE        (512)

/** \brief Maximum number of data bytes that can be on a line in the firmware file
 *         (S-record).
 */
#define SRECORD_MAX_DATA_BYTES_PER_LINE   (SRECORD_MAX_CHARS_PER_LINE/2)


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Layout of a firmware segment node in the linked list. */
typedef struct t_segment_node
{
  /** \brief The firmware segment stored in this node. */
  tFirmwareSegment segment;
  /** \brief Pointer to the next node, or NULL if it is the last one. */
  struct t_segment_node *next;
} tSegmentNode;

/** \brief Enumeration for the different S-record line types. */
typedef enum
{
  LINE_TYPE_S1,                                  /**< 16-bit address line              */
  LINE_TYPE_S2,                                  /**< 24-bit address line              */
  LINE_TYPE_S3,                                  /**< 32-bit address line              */
  LINE_TYPE_UNSUPPORTED                          /**< unsupported line                 */
} tSrecordLineType;

/** \brief Structure type for grouping the parsing results of an S-record line. */
typedef struct
{
  uint8_t data[SRECORD_MAX_DATA_BYTES_PER_LINE]; /**< array for S1,S2 or S3 data bytes */
  uint32_t address;                              /**< address on S1,S2 or S3 line      */
  uint16_t length;                               /**< number of bytes written to array */
} tSrecordLineParseResults;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void SRecParserInit(void);
static void SRecParserDeinit(void);
static bool SRecParserLoadFromFile(char *firmwareFile);
static uint32_t SRecParserGetSegmentCount(void);
static const tFirmwareSegment *SRecParserGetSegment(uint32_t segmentIdx);
static void SRecParserSegmentListInit(void);
static void SRecParserSegmentListDeinit(void);
static tSegmentNode *SRecParserSegmentListCreateNode(void);
static uint32_t SRecParserSegmentListGetNodeCount(void);
static tSegmentNode *SRecParserSegmentListGetNode(uint32_t nodeIdx);
/* S-record utility functions */
static bool SrecordParseNextDataLine(FILE* srecordHandle, tSrecordLineParseResults *parseResults);
static tSrecordLineType SrecordGetLineType(const char *line);
static bool SrecordVerifyChecksum(const char *line);
static uint8_t SrecordHexStringToByte(const char *hexstring);
static bool SrecordReadLine(FILE *srecordHandle, char *line);



/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief XCP transport structure filled with CAN specifics. */
static const tFirmwareParser srecParser =
{
  SRecParserInit,
  SRecParserDeinit,
  SRecParserLoadFromFile,
  SRecParserGetSegmentCount,
  SRecParserGetSegment
};


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Linked list with firmware segments. */
static tSegmentNode *segmentList;


/***********************************************************************************//**
** \brief     Obtains a pointer to the parser structure, so that it can be linked to the
**            firmware loader module.
** \return    Pointer to firmware parser structure.
**
****************************************************************************************/
tFirmwareParser const * const SRecParserGetParser(void)
{
  return &srecParser;
} /*** end of SRecParserGetParser ***/


/************************************************************************************//**
** \brief     Initializes the parser.
** \return    None.
**
****************************************************************************************/
static void SRecParserInit(void)
{
  /* initialize the segment list */
  SRecParserSegmentListInit();
} /*** end of SRecParserInit ***/


/************************************************************************************//**
** \brief     Uninitializes the parser.
** \return    None.
**
****************************************************************************************/
static void SRecParserDeinit(void)
{
  /* uninitialize the segment list */
  SRecParserSegmentListDeinit();
} /*** end of SRecParserDeinit ***/


/************************************************************************************//**
** \brief     Parses the data in the specified firmware file into firmware segments that
**            are stored internally in this module.
** \return    True is successful, false otherwise.
**
****************************************************************************************/
static bool SRecParserLoadFromFile(char *firmwareFile)
{
  FILE *fp;
  char line[SRECORD_MAX_CHARS_PER_LINE];
  tSrecordLineParseResults lineResults;
  tSegmentNode *node;
  uint32_t nodeIdx;
  bool matchFound;
  uint32_t byteIdx;
  uint32_t byteOffset;

  /* verify parameters */
  assert(firmwareFile != NULL);

  /* open the file for reading */
  fp = fopen(firmwareFile, "r");
  if (fp == NULL)
  {
    /* could not open the file */
    return false;
  }
  /* start at the beginning of the file */
  rewind(fp);

  /* -------------------------- file type validation --------------------------------- */
  /* validate S-record file. all lines should be formatted as S-records. read the first
   * one to check this.
   */
  if (!SrecordReadLine(fp, line))
  {
    /* could not read a line. file must be empty */
    fclose(fp);
    return false;
  }
  /* check if the line starts with the 'S' character, followed by a digit */
  if ( (toupper(line[0]) != 'S') || (isdigit(line[1]) == 0) )
  {
    fclose(fp);
    return false;
  }

  /* -------------------------- extract segment info --------------------------------- */
  /* start at the beginning of the file */
  rewind(fp);

  /* loop through all S-records with program data to obtain segment info */
  while (SrecordParseNextDataLine(fp, &lineResults))
  {
    /* reset flag that indicates that the line data was matched to an existing segment */
    matchFound = false;
    /* loop through all segment nodes */
    for (nodeIdx=0; nodeIdx < SRecParserSegmentListGetNodeCount(); nodeIdx++)
    {
      /* get node access */
      node = SRecParserSegmentListGetNode(nodeIdx);

      /* does the line data fit at the start of this node's segment? */
      if ((lineResults.address + lineResults.length) == node->segment.base)
      {
         /* update the node's segment */
         node->segment.base -= lineResults.length;
         node->segment.length += lineResults.length;
         /* match found so continue with the next line */
         matchFound = true;
         break;
      }
      /* does the line data fit at the end of this node's segment? */
      else if (lineResults.address == (node->segment.base + node->segment.length))
      {
         /* update the node's segment */
         node->segment.length += lineResults.length;
         /* match found so continue with the next line */
         matchFound = true;
         break;
      }
    }
    /* check if the line data was matched and added to an existing segment */
    if (!matchFound)
    {
      /* create a new segment */
      node = SRecParserSegmentListCreateNode();
      node->segment.base = lineResults.address;
      node->segment.length = lineResults.length;
    }
  }

  /* -------------------------- allocate data memory --------------------------------- */
  /* loop through all segment nodes */
  for (nodeIdx=0; nodeIdx < SRecParserSegmentListGetNodeCount(); nodeIdx++)
  {
    /* get node access */
    node = SRecParserSegmentListGetNode(nodeIdx);
    /* sanity check */
    assert(node->segment.length > 0);
    /* allocate data */
    node->segment.data = malloc(node->segment.length);
    /* check results */
    if (node->segment.data == NULL)
    {
      fclose(fp);
      return false;
    }
  }

  /* -------------------------- extract segment data --------------------------------- */
  /* start at the beginning of the file */
  rewind(fp);

  /* loop through all S-records with program data to obtain segment info */
  while (SrecordParseNextDataLine(fp, &lineResults))
  {
    /* loop through all segment nodes */
    for (nodeIdx=0; nodeIdx < SRecParserSegmentListGetNodeCount(); nodeIdx++)
    {
      /* get node access */
      node = SRecParserSegmentListGetNode(nodeIdx);
      /* does the line data belong in this segment? */
      if ( (lineResults.address >= node->segment.base) && \
           ((lineResults.address+lineResults.length) <= (node->segment.base+node->segment.length)) )
      {
        /* determine offset of the line data into the segment */
        byteOffset = lineResults.address - node->segment.base;
        /* store bytes in this segment */
        for (byteIdx=0; byteIdx<lineResults.length; byteIdx++)
        {
          node->segment.data[byteOffset + byteIdx] = lineResults.data[byteIdx];
        }
        /* line data stored, so continue with the next S-record */
        break;
      }
    }
  }

  /* close the file */
  fclose(fp);
  /* s-record successfully loaded and parsed */
  return true;
} /*** end of SRecParserLoadFromFile ***/


/************************************************************************************//**
** \brief     Returns the number of firmware segments that were loaded by this parser.
** \return    Number of firmware segments.
**
****************************************************************************************/
static uint32_t SRecParserGetSegmentCount(void)
{
  return SRecParserSegmentListGetNodeCount();
} /*** end of SRecParserGetSegmentCount ***/


/************************************************************************************//**
** \brief     Obtains a pointer to the firmware segment at the specified index.
** \return    Pointer to firmware segment.
**
****************************************************************************************/
static const tFirmwareSegment *SRecParserGetSegment(uint32_t segmentIdx)
{
  /* validate the parameter */
  assert(segmentIdx < SRecParserSegmentListGetNodeCount());

  return &(SRecParserSegmentListGetNode(segmentIdx)->segment);
} /*** end of SRecParserGetSegment ***/


/************************************************************************************//**
** \brief     Initializes the linked list with firmware segments.
** \return    None.
**
****************************************************************************************/
static void SRecParserSegmentListInit(void)
{
  segmentList = NULL;
} /*** end of SRecParserSegmentListInit ***/


/************************************************************************************//**
** \brief     Uninitializes the linked list with firmware segments.
** \return    None.
**
****************************************************************************************/
static void SRecParserSegmentListDeinit(void)
{
  tSegmentNode *currentNode;
  tSegmentNode *nodeToFree;

  /* free all nodes */
  if (segmentList != NULL)
  {
    currentNode = segmentList;
    do
    {
      /* store pointer to the node that should be released for later usage */
      nodeToFree = currentNode;
      /* move to the next node before freeing it */
      currentNode = currentNode->next;
      /* sanity check */
      assert(nodeToFree != NULL);
      /* free the node */
      if (nodeToFree->segment.data != NULL)
      {
        free(nodeToFree->segment.data);
      }
      free(nodeToFree);
    }
    while(currentNode != NULL);
  }
} /*** end of SRecParserSegmentListDeinit ***/


/************************************************************************************//**
** \brief     Creates a new node in the linked list with firmware segments.
** \return    Pointer to the new node.
**
****************************************************************************************/
static tSegmentNode *SRecParserSegmentListCreateNode(void)
{
  tSegmentNode *newNode;
  tSegmentNode *currentNode;

  /* allocate memory for the node */
  newNode = malloc(sizeof(tSegmentNode));
  assert(newNode != NULL);

  /* initialize the node */
  newNode->next = NULL;
  newNode->segment.base = 0x00000000;
  newNode->segment.length = 0;
  newNode->segment.data = NULL;

  /* add the first node if the list is empty */
  if (segmentList == NULL)
  {
    segmentList = newNode;
  }
  /* add the node to the end of the list */
  else
  {
    /* find last entry in to list */
    currentNode = segmentList;
    while(currentNode->next != NULL)
    {
      currentNode = currentNode->next;
    }
    /* add the now */
    currentNode->next = newNode;
  }

  return newNode;
} /*** end of SRecParserSegmentListCreateNode ***/


/************************************************************************************//**
** \brief     Obtains the number of nodes in the linked list with firmware segments.
** \return    Number of nodes.
**
****************************************************************************************/
static uint32_t SRecParserSegmentListGetNodeCount(void)
{
  tSegmentNode *currentNode;
  uint32_t nodeCount = 0;

  /* iterate over all nodes to determine to total count */
  if (segmentList != NULL)
  {
    currentNode = segmentList;
    do
    {
      nodeCount++;
      currentNode = currentNode->next;
    }
    while(currentNode != NULL);
  }

  return nodeCount;
} /*** end of SRecParserSegmentListGetNodeCount ***/


/************************************************************************************//**
** \brief     Obtains the node at the specified index from the linked list with firmware
**            segments.
** \return    Pointer to the node.
**
****************************************************************************************/
static tSegmentNode *SRecParserSegmentListGetNode(uint32_t nodeIdx)
{
  tSegmentNode *currentNode = NULL;
  uint32_t currentIdx = 0;

  /* validate the parameter */
  assert(nodeIdx < SRecParserSegmentListGetNodeCount());

  /* iterate until the specified index is found */
  currentNode = segmentList;
  for (currentIdx=0; currentIdx<nodeIdx; currentIdx++)
  {
    currentNode = currentNode->next;
  }

  return currentNode;
} /*** end of SRecParserSegmentListGetNode ***/


/************************************************************************************//**
** \brief     Reads the next S-record with program data, parses it and returns the
**            results.
** \param     srecordHandle The S-record file handle. It is returned by SrecordOpen.
** \param     parseResults Pointer to where the parse results should be stored.
** \return    SB_TRUE is valid parse results were stored. SB_FALSE in case of end-of-
**            file.
**
****************************************************************************************/
static bool SrecordParseNextDataLine(FILE* srecordHandle, tSrecordLineParseResults *parseResults)
{
  char line[SRECORD_MAX_CHARS_PER_LINE];
  bool data_line_found = false;
  tSrecordLineType lineType;
  uint16_t bytes_on_line;
  uint16_t i;
  char *linePtr;

  /* first set the length paramter to 0 */
  parseResults->length = 0;

  while (!data_line_found)
  {
    /* read the next line from the file */
    if (!SrecordReadLine(srecordHandle, line))
    {
      /* end-of-file encountered */
      return false;
    }
    /* we now have a line. check if it is a S-record data line */
    lineType = SrecordGetLineType(line);
    if (lineType != LINE_TYPE_UNSUPPORTED)
    {
      /* check if the checksum on the line is correct */
      if (SrecordVerifyChecksum(line))
      {
        /* found a valid line that can be parsed. loop will stop */
        data_line_found = true;
        break;
      }
    }
  }

  /* still here so we have a valid S-record data line. start parsing */
  linePtr = &line[0];
  /* all good so far, now read out the address and databytes for the line */
  switch (lineType)
  {
    /* ---------------------------- S1 line type ------------------------------------- */
    case LINE_TYPE_S1:
      /* adjust pointer to point to byte count value */
      linePtr += 2;
      /* read out the number of byte values that follow on the line */
      bytes_on_line = SrecordHexStringToByte(linePtr);
      /* read out the 16-bit address */
      linePtr += 2;
      parseResults->address = SrecordHexStringToByte(linePtr) << 8;
      linePtr += 2;
      parseResults->address += SrecordHexStringToByte(linePtr);
      /* adjust pointer to point to the first data byte after the address */
      linePtr += 2;
      /* determine how many data bytes are on the line */
      parseResults->length = bytes_on_line - 3; /* -2 bytes address, -1 byte checksum */
      /* read and store data bytes if requested */
      for (i=0; i<parseResults->length; i++)
      {
        parseResults->data[i] = SrecordHexStringToByte(linePtr);
        linePtr += 2;
      }
      break;

    /* ---------------------------- S2 line type ------------------------------------- */
    case LINE_TYPE_S2:
      /* adjust pointer to point to byte count value */
      linePtr += 2;
      /* read out the number of byte values that follow on the line */
      bytes_on_line = SrecordHexStringToByte(linePtr);
      /* read out the 32-bit address */
      linePtr += 2;
      parseResults->address = SrecordHexStringToByte(linePtr) << 16;
      linePtr += 2;
      parseResults->address += SrecordHexStringToByte(linePtr) << 8;
      linePtr += 2;
      parseResults->address += SrecordHexStringToByte(linePtr);
      /* adjust pointer to point to the first data byte after the address */
      linePtr += 2;
      /* determine how many data bytes are on the line */
      parseResults->length = bytes_on_line - 4; /* -3 bytes address, -1 byte checksum */
      /* read and store data bytes if requested */
      for (i=0; i<parseResults->length; i++)
      {
        parseResults->data[i] = SrecordHexStringToByte(linePtr);
        linePtr += 2;
      }
      break;

    /* ---------------------------- S3 line type ------------------------------------- */
    case LINE_TYPE_S3:
      /* adjust pointer to point to byte count value */
      linePtr += 2;
      /* read out the number of byte values that follow on the line */
      bytes_on_line = SrecordHexStringToByte(linePtr);
      /* read out the 32-bit address */
      linePtr += 2;
      parseResults->address = SrecordHexStringToByte(linePtr) << 24;
      linePtr += 2;
      parseResults->address += SrecordHexStringToByte(linePtr) << 16;
      linePtr += 2;
      parseResults->address += SrecordHexStringToByte(linePtr) << 8;
      linePtr += 2;
      parseResults->address += SrecordHexStringToByte(linePtr);
      /* adjust pointer to point to the first data byte after the address */
      linePtr += 2;
      /* determine how many data bytes are on the line */
      parseResults->length = bytes_on_line - 5; /* -4 bytes address, -1 byte checksum */
      /* read and store data bytes if requested */
      for (i=0; i<parseResults->length; i++)
      {
        parseResults->data[i] = SrecordHexStringToByte(linePtr);
        linePtr += 2;
      }
      break;

    default:
      /* will not happen */
      break;
  }

  /* parsing all done */
  return true;
} /*** end of SrecordParseNextDataLine ***/


/************************************************************************************//**
** \brief     Inspects a line from a Motorola S-Record file to determine its type.
** \param     line A line from the S-Record.
** \return    the S-Record line type.
**
****************************************************************************************/
static tSrecordLineType SrecordGetLineType(const char *line)
{
  /* check if the line starts with the 'S' character, followed by a digit */
  if ( (toupper(line[0]) != 'S') || (isdigit(line[1]) == 0) )
  {
    /* not a valid S-Record line type */
    return LINE_TYPE_UNSUPPORTED;
  }
  /* determine the line type */
  if (line[1] == '1')
  {
    return LINE_TYPE_S1;
  }
  if (line[1] == '2')
  {
    return LINE_TYPE_S2;
  }
  if (line[1] == '3')
  {
    return LINE_TYPE_S3;
  }
  /* still here so not a supported line type found */
  return LINE_TYPE_UNSUPPORTED;
} /*** end of SrecordGetLineType ***/


/************************************************************************************//**
** \brief     Inspects an S1, S2 or S3 line from a Motorola S-Record file to
**            determine if the checksum at the end is corrrect.
** \param     line An S1, S2 or S3 line from the S-Record.
** \return    SB_TRUE if the checksum is correct, SB_FALSE otherwise.
**
****************************************************************************************/
static bool SrecordVerifyChecksum(const char *line)
{
  uint16_t bytes_on_line;
  uint8_t  checksum = 0;

  /* adjust pointer to point to byte count value */
  line += 2;
  /* read out the number of byte values that follow on the line */
  bytes_on_line = SrecordHexStringToByte(line);
  /* byte count is part of checksum */
  checksum += bytes_on_line;
  /* adjust pointer to the first byte of the address */
  line += 2;
  /* add byte values of address and data, but not the final checksum */
  do
  {
    /* add the next byte value to the checksum */
    checksum += SrecordHexStringToByte(line);
    /* update counter */
    bytes_on_line--;
    /* point to next hex string in the line */
    line += 2;
  }
  while (bytes_on_line > 1);
  /* the checksum is calculated by summing up the values of the byte count, address and
   * databytes and then taking the 1-complement of the sum's least signigicant byte */
  checksum = ~checksum;
  /* finally verify the calculated checksum with the one at the end of the line */
  if (checksum != SrecordHexStringToByte(line))
  {
    /* checksum incorrect */
    return false;
  }
  /* still here so the checksum was correct */
  return true;
} /*** end of SrecordVerifyChecksum ***/


/************************************************************************************//**
** \brief     Helper function to convert a sequence of 2 characters that represent
**            a hexadecimal value to the actual byte value.
**              Example: SrecordHexStringToByte("2f")  --> returns 47.
** \param     hexstring String beginning with 2 characters that represent a hexa-
**                      decimal value.
** \return    The resulting byte value.
**
****************************************************************************************/
static uint8_t SrecordHexStringToByte(const char *hexstring)
{
  uint8_t result = 0;
  char  c;
  uint8_t counter;

  /* a hexadecimal character is 2 characters long (i.e 0x4F minus the 0x part) */
  for (counter=0; counter < 2; counter++)
  {
    /* read out the character */
    c = toupper(hexstring[counter]);
    /* check that the character is 0..9 or A..F */
    if ( (c < '0') || (c > 'F') || ( (c > '9') && (c < 'A') ) )
    {
      /* character not valid */
      return 0;
    }
    /* convert character to 4-bit value (check ASCII table for more info) */
    c -= '0';
    if (c > 9)
    {
      c -= 7;
    }
    /* add it to the result */
    result = (result << 4) + c;
  }
  /* return the results */
  return result;
} /*** end of SrecordHexStringToByte ***/


/************************************************************************************//**
** \brief     Reads the next line from the S-record file handle.
** \param     srecordHandle The S-record file handle. It is returned by SrecordOpen.
** \param     line Destination buffer for the line characters. Should be of size
**            SRECORD_MAX_CHARS_PER_LINE.
** \return    SB_TRUE if successful, SB_FALSE otherwise.
**
****************************************************************************************/
static bool SrecordReadLine(FILE *srecordHandle, char *line)
{
  /* init the line as an empty line */
  line[0] = '\0';

  /* loop as long as we find a non-empty line or end-of-file */
  while (line[0] == '\0')
  {
    if (fgets(line, SRECORD_MAX_CHARS_PER_LINE, srecordHandle) == NULL)
    {
      /* no more lines available */
      return false;
    }
    /* replace the line termination with a string termination */
    line[strcspn(line, "\n")] = '\0';
  }
  /* still here so not EOF and not and empty line, so success */
  return true;
} /*** end of SrecordReadLine ***/


/*********************************** end of srecparser.c *******************************/

