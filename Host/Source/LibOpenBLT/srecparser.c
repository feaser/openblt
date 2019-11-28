/************************************************************************************//**
* \file         srecparser.c
* \brief        Motorola S-record file parser source file.
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
#include <stdio.h>                          /* for standard I/O library                */
#include <string.h>                         /* for string library                      */
#include <ctype.h>                          /* for toupper() etc.                      */
#include "util.h"                           /* Utility module                          */
#include "firmware.h"                       /* Firmware data module                    */
#include "srecparser.h"                     /* S-record parser                         */


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Enumeration for the different supported S-record line types. */
typedef enum t_srec_parser_line_type
{
  SREC_PARSER_LINE_TYPE_S0,                      /**< Header record.                   */
  SREC_PARSER_LINE_TYPE_S1,                      /**< 16-bit address data record.      */
  SREC_PARSER_LINE_TYPE_S2,                      /**< 24-bit address data record.      */
  SREC_PARSER_LINE_TYPE_S3,                      /**< 32-bit address data record.      */
  SREC_PARSER_LINE_TYPE_S7,                      /**< 32-bit address termination.      */
  SREC_PARSER_LINE_TYPE_S8,                      /**< 24-bit address termination.      */
  SREC_PARSER_LINE_TYPE_S9,                      /**< 16-bit address termination.      */
  SREC_PARSER_LINE_TYPE_UNSUPPORTED              /**< Unsupported line.                */
} tSRecParserLineType;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static bool SRecParserLoadFromFile (char const * firmwareFile, uint32_t addressOffset);
static bool SRecParserVerifyFile(char const * firmwareFile);
static bool SRecParserSaveToFile (char const * firmwareFile);
static bool SRecParserExtractLineData(char const * line, uint32_t * address, 
                                      uint32_t * len, uint8_t * data);
static tSRecParserLineType SRecParserGetLineType(char const * line);
static bool SRecParserVerifyChecksum(char const * line);
static bool SRecParserConstructLine(char * line, tSRecParserLineType lineType, 
                                    uint32_t address,
                                    uint8_t const * data, uint8_t dataLen);
static uint8_t SRecParserHexStringToByte(char const * hexstring);


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief File parser structure filled with Motorola S-record parsing specifics. */
static const tFirmwareParser srecParser =
{
  .LoadFromFile = SRecParserLoadFromFile,
  .SaveToFile = SRecParserSaveToFile
};


/***********************************************************************************//**
** \brief     Obtains a pointer to the parser structure, so that it can be linked to the
**            firmware data module.
** \return    Pointer to firmware parser structure.
**
****************************************************************************************/
tFirmwareParser const * SRecParserGetParser(void)
{
  return &srecParser;
} /*** end of SRecParserGetParser ***/


/************************************************************************************//**
** \brief     Parses the specified firmware file to extract firmware data and adds this 
**            data to the firmware data that is currently managed by the firmware data
**            module.
** \param     firmwareFile Filename of the firmware file to load.
** \param     addressOffset Optional memory address offset to add when loading the 
**            firmware data from the file.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool SRecParserLoadFromFile (char const * firmwareFile, uint32_t addressOffset)
{
  bool result = false;
  FILE *fp;
  /* The bytes count entry on the S-record line is max 255 bytes. This include the 
   * address and the checksum. This would result in 255 * 2 = 510 characters. Another
   * 4 characters are needed for the bytes count and line type characters. Then another
   * two for possible line termination (new line + cariage return). This brings the total
   * characters to 516. Note that this array was made static to lower the stack load.
   */
  static char line[516];
  /* The bytes count entry on the S-record line is max 255 bytes. This includes the 
   * address and checksum. This means the worst case max data bytes per line is 255 - 3.
   * Note that this array was made static to lower the stack load.
   */
  static uint8_t data[252];
  uint32_t address;
  uint32_t len;
  
  /* Check parameters. */
  assert(firmwareFile != NULL);
  
  /* Only continue if the parameters are valid. */
  if (firmwareFile != NULL) /*lint !e774 */
  {
    /* First verify that the file is actually a valid S-record with correct line
     * checksums and that it actually contains program data. Only continue if this
     * verification was successful.
     */
    if (SRecParserVerifyFile(firmwareFile))
    {
      /* Open the file for reading. */
      fp = fopen(firmwareFile, "r");
      /* Only continue if the filepointer is valid. */
      if (fp != NULL)
      {
        /* Start at the beginning of the file. */
        rewind(fp);
        /* Assume that everyting goes okay and then only set a negative result value upon
         * detection of a problem.
         */
        result = true;
        /* Read the entire file, one line at a time. */
        while (fgets(line, sizeof(line)/sizeof(line[0]), fp) != NULL )
        {
          /* Replace the line termination with a string termination. */
          line[strcspn(line, "\n\r")] = '\0'; 
          /* Extra data from the S-record line. */
          if (SRecParserExtractLineData(line, &address, &len, data))
          {
            /* Only add data if there is actually something to add. */
            if (len > 0)
            {
              /* Add the extracted data to the firmware data module and add the memory
               * address that was specified by the caller.
               */
              if (!FirmwareAddData(address + addressOffset, len, data))
              {
                /* Error detected. Flag it and abort. */
                result = false;
                break;
              }
            }
          }
        }      
        /* Close the file now that we are done with it. */
        fclose(fp);
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of SRecParserLoadFromFile ***/


/************************************************************************************//**
** \brief     Parses the specified firmware file to verify that the file is a valid
**            S-record file. 
** \param     firmwareFile Filename of the firmware file to verify.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool SRecParserVerifyFile(char const * firmwareFile)
{
  bool result = false;
  FILE *fp;
  /* The bytes count entry on the S-record line is max 255 bytes. This include the 
   * address and the checksum. This would result in 255 * 2 = 510 characters. Another
   * 4 characters are needed for the bytes count and line type characters. Then another
   * two for possible line termination (new line + cariage return). This brings the total
   * characters to 516. Note that this array was made static to lower the stack load.
   */
  static char line[516];
  tSRecParserLineType lineType;
  bool programDataDetected = false;
  
  /* Check parameters. */
  assert(firmwareFile != NULL);
  
  /* Only continue if the parameters are valid. */
  if (firmwareFile != NULL) /*lint !e774 */
  {
    /* Open the file for reading. */
    fp = fopen(firmwareFile, "r");
    /* Only continue if the filepointer is valid. */
    if (fp != NULL)
    {
      /* Start at the beginning of the file. */
      rewind(fp);
      /* Assume that everyting goes okay and then only set a negative result value upon
       * detection of a problem.
       */
      result = true;
      /* Read the entire file, one line at a time. */
      while (fgets(line, sizeof(line)/sizeof(line[0]), fp) != NULL )
      {
        /* Replace the line termination with a string termination. */
        line[strcspn(line, "\n\r")] = '\0'; 
        /* Determine the line type. */
        lineType = SRecParserGetLineType(line);
        /* We are only interested in S-record line thatt contains program data. */
        if ( (lineType == SREC_PARSER_LINE_TYPE_S1) || 
             (lineType == SREC_PARSER_LINE_TYPE_S2) ||
             (lineType == SREC_PARSER_LINE_TYPE_S3) )
        {
          /* The file contains program data, so update the flag for this. */
          programDataDetected = true;
          /* Verify the checksum of the line. */
          if (!SRecParserVerifyChecksum(line))
          {
            /* Invalid S-record file. Update the result value and stop looping. */
            result = false;
            break;
          }
        }
      }      
      /* Close the file now that we are done with it. */
      fclose(fp);
      /* Update the result in case no program data was encountered. */
      if (!programDataDetected)
      {
        result = false;
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of SRecParserVerifyFile ***/


/************************************************************************************//**
** \brief     Writes firmware data to the specified file in the correct file format.
** \param     firmwareFile Filename of the firmware file to write to.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool SRecParserSaveToFile (char const * firmwareFile)
{
  bool result = false;
  FILE *fp;
  /* The bytes count entry on the S-record line is max 255 bytes. This include the 
   * address and the checksum. This would result in 255 * 2 = 510 characters. Another
   * 4 characters are needed for the bytes count and line type characters. Then another
   * two for possible line termination (new line + cariage return). This brings the total
   * characters to 516. Note that this array was made static to lower the stack load.
   */
  static char line[516];
  /* The bytes count entry on the S-record line is max 255 bytes. This includes the 
   * address and checksum. This means the worst case max data bytes per line is 255 - 3.
   * Note that this array was made static to lower the stack load.
   */
  static uint8_t data[252];
  tFirmwareSegment * segment;
  uint32_t progDataLowestAddress = 0x00000000;
  uint32_t progDataHighestAddress = 0xffffffff;
  tSRecParserLineType dataLineType = SREC_PARSER_LINE_TYPE_S1;
  tSRecParserLineType terminationLineType = SREC_PARSER_LINE_TYPE_S9;
  uint32_t segmentIdx;
  uint32_t currentAddress;
  uint8_t currentByteCnt;
  uint8_t const * currentDataPtr;
  uint32_t segmentBytesLeft;
  const uint8_t maxDataBytesPerLine = 32;
 
  /* Check parameters. */
  assert(firmwareFile != NULL);
  
  /* Only continue if the parameters are valid and if there is something to save. */
  if ( (firmwareFile != NULL) && (FirmwareGetSegmentCount() > 0) ) /*lint !e774 */
  {
    /* Open the file for writing. */
    fp = fopen(firmwareFile, "w");
    /* Only continue if the filepointer is valid. */
    if (fp != NULL)
    {
      /* Init result value to okay at this point and only set it to error in case a 
       * problem was detected.
       */
      result = true;

      /* Determine the lowest memory address used in the program data. This address needs
       * to be specified in the termination record at the end of the S-record file.
       */
      segment = FirmwareGetSegment(0);
      /* Sanity check. */
      assert(segment != NULL);
      if (segment == NULL) /*lint !e774 */
      {
        /* Flag error. */
        result = false;
      }
      else
      {
        /* Store the lowest memory address. */
        progDataLowestAddress = segment->base;
      }

      /* Determine the highest memory address used in the program data. This address 
       * is needed to determine the number of bits needed for the address in the 
       * S-record data lines (16, 24 or 32), which determines the type of S-record for
       * the data lines (S1, S2, S3) and the termination line (S7, S8 or S9).
       */
      if (result) /*lint !e774 */
      {
        segment = FirmwareGetSegment(FirmwareGetSegmentCount() - 1u);
        /* Sanity check. */
        assert(segment != NULL);
        if (segment == NULL) /*lint !e774 */
        {
          /* Flag error. */
          result = false;
        }
        else
        {
          progDataHighestAddress = segment->base + segment->length - 1u;
          /* Does the address have more than 24 bits? */
          if (progDataHighestAddress > 0xffffff)
          {
            dataLineType = SREC_PARSER_LINE_TYPE_S3;
            terminationLineType = SREC_PARSER_LINE_TYPE_S7;
          }
          /* Does the address have more than 16 bits? */
          else if (progDataHighestAddress > 0xffff)
          {
            dataLineType = SREC_PARSER_LINE_TYPE_S2;
            terminationLineType = SREC_PARSER_LINE_TYPE_S8;
          }
        }
      }
      
      /* Extract just the filename and copy it to the data buffer. */
      if (result) /*lint !e774 */
      {
        if (!UtilFileExtractFilename(firmwareFile, (char *)data))
        {
          /* Could not extract filename. */
          result = false;
        }
      }
      
      /* Construct and add the S0-record with the filename. */
      if (result)
      {
        /* Construct the S0-record. */
        result = SRecParserConstructLine(line, SREC_PARSER_LINE_TYPE_S0, 0x0000, data, 
                                         (uint8_t)strlen((char *)data));
        if (result)
        {
          /* Add the S0-record. */
          if (fprintf(fp, "%s\n", line) < 0)
          {
            /* Could not write line to the file. */
            result = false;
          }
        }
      }
      
      /* Write all the program data records. Process one segment at a time. */
      if (result)
      {
        /* Loop through all segments. */
        for (segmentIdx = 0; segmentIdx < FirmwareGetSegmentCount(); segmentIdx++)
        {
          /* Obtain the segment. */
          segment = FirmwareGetSegment(segmentIdx);
          /* Sanity check. */
          assert(segment != NULL);
          if (segment == 0) /*lint !e774 */
          {
            /* Flag error and abort loop. */
            result = false;
            break;
          }
          /* Initialize base address, byte count and data pointer. */
          currentAddress = segment->base;
          currentDataPtr = segment->data;
          segmentBytesLeft = segment->length;
          /* Process al bytes in the segment. */
          while (segmentBytesLeft > 0)
          {
            /* Determine the number of bytes that can be written. */
            currentByteCnt = maxDataBytesPerLine;
            if (segmentBytesLeft < maxDataBytesPerLine)
            {
              currentByteCnt = (uint8_t)segmentBytesLeft;
            }
            /* Construct the data record. */
            result = SRecParserConstructLine(line, dataLineType, currentAddress, 
                                             currentDataPtr, currentByteCnt);
            if (!result)
            {
              /* Error detected. No need to continue the loop. */
              break;
            }
            /* Add the data record. */
            if (fprintf(fp, "%s\n", line) < 0)
            {
              /* Could not write line to the file. Abort loop. */
              result = false;
              break;
            }
            /* Update loop variables. */
            currentAddress += currentByteCnt;
            currentDataPtr += currentByteCnt;
            segmentBytesLeft -= currentByteCnt;
          }
        }
      }
      
      /* Write the termination record. */
      if (result)
      {
        /* Construct the termination record. */
        result = SRecParserConstructLine(line, terminationLineType, 
                                         progDataLowestAddress, data, 0u);
        if (result)
        {
          /* Add the termination record. */
          if (fprintf(fp, "%s\n", line) < 0)
          {
            /* Could not write line to the file. */
            result = false;
          }
        }
      }
      /* Close the file now that we are done with it. */
      fclose(fp);
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of SRecParserSaveToFile ***/


/************************************************************************************//**
** \brief     Checks if the specified S-record line is of the type that contains program
**            data. If it does, then the program data and base address are extracted and
**            stored at the function parameter pointers.
** \param     line Pointer to the line from an S-record file.
** \param     address Pointer where the start address of the program data is stored.
** \param     len Pointer for storing the number of extracted program data bytes.
** \param     data Pointer to byte array where the extracted program data bytes are 
**            stored.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool SRecParserExtractLineData(char const * line, uint32_t * address, 
                                      uint32_t * len, uint8_t * data)
{
  bool result = false;
  tSRecParserLineType lineType;
  uint8_t dataByteCount = 0;
  uint8_t bytesOnLine;
  uint8_t idx;
  
  /* Verify parameters. */
  assert(line != NULL);
  assert(address != NULL);
  assert(len != NULL);
  assert(data != NULL);
  
  /* Only continue with valid parameters. */
  if ( (line != NULL) && (address != NULL) && (len != NULL) && 
       (data != NULL) ) /*lint !e774 */
  {
    /* Initialize parameter result values. */
    *len = 0;
    *address = 0;
  
    /* Determine the line type. */
    lineType = SRecParserGetLineType(line);
    /* Only continue if the S-record line contains program data. */
    if ( (lineType == SREC_PARSER_LINE_TYPE_S1) || 
         (lineType == SREC_PARSER_LINE_TYPE_S2) ||
         (lineType == SREC_PARSER_LINE_TYPE_S3) )
    {
      /* Verify the checksum of the line. */
      if (SRecParserVerifyChecksum(line))
      {
        /* Adjust pointer to point to byte count value. */
        line += 2u;
        /* Read out the number of byte values that follow on the line. */
        bytesOnLine = SRecParserHexStringToByte(line);
        /* Process S1 line type. */
        if (lineType == SREC_PARSER_LINE_TYPE_S1)
        {
          /* Read out the 16-bit address. */
          line += 2u;
          *address = (uint32_t)SRecParserHexStringToByte(line) << 8u;
          line += 2u;
          *address += (uint32_t)SRecParserHexStringToByte(line);
          /* Determine how many data bytes are on the line. -2 bytes for address and 
            * -1 byte for the checksum.
            */
          dataByteCount = bytesOnLine - 3u; 
        }
        /* Process S2 line type. */
        else if (lineType == SREC_PARSER_LINE_TYPE_S2)
        {
          /* Read out the 24-bit address. */
          line += 2u;
          *address = (uint32_t)SRecParserHexStringToByte(line) << 16u;
          line += 2u;
          *address += (uint32_t)SRecParserHexStringToByte(line) << 8u;
          line += 2u;
          *address += (uint32_t)SRecParserHexStringToByte(line);
          /* Determine how many data bytes are on the line. -3 bytes for address and 
            * -1 byte for the checksum.
            */
          dataByteCount = bytesOnLine - 4u;
        }
        /* Process S3 line type. */
        else
        {
          /* Read out the 32-bit address. */
          line += 2u;
          *address = (uint32_t)SRecParserHexStringToByte(line) << 24u;
          line += 2u;
          *address += (uint32_t)SRecParserHexStringToByte(line) << 16u;
          line += 2u;
          *address += (uint32_t)SRecParserHexStringToByte(line) << 8u;
          line += 2u;
          *address += (uint32_t)SRecParserHexStringToByte(line);
          /* Determine how many data bytes are on the line. -4 bytes for address and 
            * -1 byte for the checksum.
            */
          dataByteCount = bytesOnLine - 5u;
        }
        /* Adjust pointer to point to the first data byte after the address. */
        line += 2u;
        /* Read and store data bytes. */
        for (idx = 0; idx < dataByteCount; idx++)
        {
          data[idx] = SRecParserHexStringToByte(line);
          line += 2u;
        }
        /* Store the number of data bytes that were copied to the array. */
        *len = dataByteCount;
        /* Data extraction complete. */
        result = true;
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of SRecParserExtractLineData ***/


/************************************************************************************//**
** \brief     Inspects a line from a Motorola S-Record file to determine its type.
** \param     line A line from the S-Record.
** \return    The S-Record line type.
**
****************************************************************************************/
static tSRecParserLineType SRecParserGetLineType(char const * line)
{
  tSRecParserLineType result = SREC_PARSER_LINE_TYPE_UNSUPPORTED;
  
  /* Verify parameters. */
  assert(line != NULL);
  
  /* Only continue with valid parameters. */
  if (line != NULL) /*lint !e774 */
  {
    /* Check if the line starts with the 'S' character, followed by a digit */
    if ( (toupper((int32_t)(line[0])) == 'S') && (isdigit((int32_t)(line[1]))) )
    {
      /* Check the digit that follows the 'S' character. Currently only line types that
      * contain program data are needed.
      */
      switch (line[1])
      {
        case '0':
          result = SREC_PARSER_LINE_TYPE_S0;
          break;
        case '1':
          result = SREC_PARSER_LINE_TYPE_S1;
          break;
        case '2':
          result = SREC_PARSER_LINE_TYPE_S2;
          break;
        case '3':
          result = SREC_PARSER_LINE_TYPE_S3;
          break;
        case '7':
          result = SREC_PARSER_LINE_TYPE_S7;
          break;
        case '8':
          result = SREC_PARSER_LINE_TYPE_S8;
          break;
        case '9':
          result = SREC_PARSER_LINE_TYPE_S9;
          break;
        default:
          result = SREC_PARSER_LINE_TYPE_UNSUPPORTED;
          break;
      }
    }
  }
  
  /* Give the result back to the caller. */
  return result;
} /*** end of SRecParserGetLineType ***/


/************************************************************************************//**
** \brief     Inspects an S1, S2 or S3 line from a Motorola S-Record file to
**            determine if the checksum at the end is corrrect.
** \param     line An S1, S2 or S3 line from the S-Record.
** \return    True if the checksum is correct, false otherwise.
**
****************************************************************************************/
static bool SRecParserVerifyChecksum(char const * line)
{
  bool result = false;
  uint8_t bytes_on_line;
  uint8_t checksum = 0;
  
  /* Verify parameters. */
  assert(line != NULL);
  /* Only continue with with valid parameters. */
  if (line != NULL) /*lint !e774 */
  {
    /* Adjust pointer to point to byte count value. */
    line += 2u;
    /* Read out the number of byte values that follow on the line. */
    bytes_on_line = SRecParserHexStringToByte(line);
    /* An S-record will always have at least a 16-bit address and a checksum value. */
    if (bytes_on_line >= 3u)
    {
      /* Byte count is part of checksum. */
      checksum += bytes_on_line;
      /* Adjust pointer to the first byte of the address. */
      line += 2u;
      /* Add byte values of address and data, but not the final checksum. */
      do
      {
        /* Add the next byte value to the checksum. */
        checksum += SRecParserHexStringToByte(line);
        /* Update counter. */
        bytes_on_line--;
        /* Point to next hex string in the line. */
        line += 2u;
      }
      while (bytes_on_line > 1u);
      /* The checksum is calculated by summing up the values of the byte count, address 
       * and databytes and then taking the 1-complement of the sum's least signigicant 
       * byte.
       */
      checksum = ~checksum;
      /* Finally verify the calculated checksum with the one at the end of the line. */
      if (checksum == SRecParserHexStringToByte(line))
      {
        /* Checksum correct. */
        result = true;
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of SRecParserVerifyChecksum ***/


/************************************************************************************//**
** \brief     Creates a NUL terminated S-record line, given the specified line type,
**            address and data bytes. The checksum at the end of the line is also
**            calculated and added.
** \param     line Pointer to character array where the string will be stored.
** \param     lineType The type of S-record line to construct.
** \param     address The address to embed into the line after the byte count.
** \param     data Point to byte array with data bytes to add to the line.
** \param     dataLen The number of data bytes present in the data-array.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool SRecParserConstructLine(char * line, tSRecParserLineType lineType, 
                                    uint32_t address, uint8_t const * data, 
                                    uint8_t dataLen)
{
  bool result = false;
  uint8_t addressBits;
  uint8_t byteVal;
  char hexByteStr[3];
  uint8_t cnt;
  uint8_t checksumVal = 0;
  
  /* Verify parameters. */
  assert(line != NULL);
  if (dataLen > 0)
  {
    assert(data != NULL);
  }
  
  /* Only continue with valid parameters. */
  if (line != NULL) /*lint !e774 */
  {
    /* Set the result value to successful and only change this upon detection of an
     * error.
     */
    result = true;
    /* Start of with an empty string. */
    line[0] = '\0';
    /* Add the record type based on the lineType and store the number of bits in the 
     * address.
     */
    switch (lineType)
    {
      case SREC_PARSER_LINE_TYPE_S0:
        strcat(line, "S0");
        addressBits = 16;
        break;
      case SREC_PARSER_LINE_TYPE_S1:
        strcat(line, "S1");
        addressBits = 16;
        break;
      case SREC_PARSER_LINE_TYPE_S2:
        strcat(line, "S2");
        addressBits = 24;
        break;
      case SREC_PARSER_LINE_TYPE_S3:
        strcat(line, "S3");
        addressBits = 32;
        break;
      case SREC_PARSER_LINE_TYPE_S7:
        strcat(line, "S7");
        addressBits = 32;
        break;
      case SREC_PARSER_LINE_TYPE_S8:
        strcat(line, "S8");
        addressBits = 24;
        break;
      case SREC_PARSER_LINE_TYPE_S9:
        strcat(line, "S9");
        addressBits = 16;
        break;
      case SREC_PARSER_LINE_TYPE_UNSUPPORTED:
      default:
        /* Invalid line type specified. Should not happed. */
        addressBits = 16;
        assert(false); /*lint !e506 */
        break;
    }
    
    /* Next, add the number of bytes that will follow on the line. This is the size in
     * bytes of the address, the actual number of data bytes and the checksum.
     */
    byteVal = (addressBits / 8u) + dataLen + 1u;
    checksumVal += byteVal;
    if (sprintf(hexByteStr, "%02X", byteVal) != 2)
    {
      /* Error occurred. */
      result = false;
    }
    else
    {
      /* Append it to the line. */
      strcat(line, hexByteStr);
    }
      
    /* Add the address. */
    for (cnt = (addressBits / 8u); cnt > 0; cnt--)
    {
      byteVal = (uint8_t)(address >> ( (cnt - 1u) * 8u));
      checksumVal += byteVal;
      if (sprintf(hexByteStr, "%02X", byteVal) != 2)
      {
        /* Error occurred. */
        result = false;
      }
      else
      {
        /* Append it to the line. */
        strcat(line, hexByteStr);
      }
    }
    
    /* Add the data bytes. */
    for (cnt = 0; cnt < dataLen; cnt++)
    {
      byteVal = 0;
      if (data != NULL)
      {
        byteVal = data[cnt];
      }
      checksumVal += byteVal;
      if (sprintf(hexByteStr, "%02X", byteVal) != 2)
      {
        /* Error occurred. */
        result = false;
      }
      else
      {
        /* Append it to the line. */
        strcat(line, hexByteStr);
      }
    }
    
    /* Calculate and add the checksum. */
    checksumVal = ~checksumVal;
    byteVal = checksumVal;
    if (sprintf(hexByteStr, "%02X", byteVal) != 2)
    {
      /* Error occurred. */
      result = false;
    }
    else
    {
      /* Append it to the line. */
      strcat(line, hexByteStr);
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of SRecParserConstructLine ***/


/************************************************************************************//**
** \brief     Helper function to convert a sequence of 2 characters that represent
**            a hexadecimal value to the actual byte value.
**              Example: SRecParserHexStringToByte("2f")  --> returns 47.
** \param     hexstring String beginning with 2 characters that represent a hexa-
**            decimal value.
** \return    The resulting byte value.
**
****************************************************************************************/
static uint8_t SRecParserHexStringToByte(char const * hexstring)
{
  uint8_t result = 0;
  int32_t c;
  uint8_t counter;

  /* Verify parameters. */
  assert(hexstring != NULL);
  /* Only continue with valid parameters. */
  if (hexstring != NULL) /*lint !e774 */
  {
    /* A hexadecimal character is 2 characters long (i.e 0x4F minus the 0x part). */
    for (counter = 0; counter < 2u; counter++)
    {
      /* Read out the character */
      c = toupper((int32_t)(hexstring[counter]));
      /* Check that the character is 0..9 or A..F */
      if (isxdigit(c))
      {
        /* Convert character to 4-bit value (check ASCII table for more info) */
        c -= '0';
        if (c > 9)
        {
          c -= 7;
        }
        /* Add it to the result. */
        result = (uint8_t)((result << 4u) + c);
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of SRecParserHexStringToByte ***/


/*********************************** end of srecparser.c *******************************/

 
 
