/************************************************************************************//**
* \file         srecord.c
* \brief        Motorola S-record library header file.
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
* You should have received a copy of the GNU General Public License along with OpenBLT.
* If not, see <http://www.gnu.org/licenses/>.
*
* A special exception to the GPL is included to allow you to distribute a combined work 
* that includes OpenBLT without being obliged to provide the source code for any 
* proprietary components. The exception text is included at the bottom of the license
* file <license.html>.
* 
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include <assert.h>                                   /* assertion module              */
#include <sb_types.h>                                 /* C types                       */
#include <string.h>                                   /* for strcpy etc.               */
#include <ctype.h>                                    /* for toupper() etc.            */
#include "srecord.h"                                  /* S-record library              */


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Enumeration for the different S-record line types. */
typedef enum
{
  LINE_TYPE_S1,                                  /**< 16-bit address line              */
  LINE_TYPE_S2,                                  /**< 24-bit address line              */
  LINE_TYPE_S3,                                  /**< 32-bit address line              */
  LINE_TYPE_UNSUPPORTED                          /**< unsupported line                 */
} tSrecordLineType;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static tSrecordLineType SrecordGetLineType(const sb_char *line);
static sb_uint8         SrecordVerifyChecksum(const sb_char *line);
static sb_uint8         SrecordHexStringToByte(const sb_char *hexstring);
static sb_uint8         SrecordReadLine(sb_file srecordHandle, sb_char *line);


/************************************************************************************//**
** \brief     Checks if the specified srecordFile exists and contains s-records.
** \param     srecordFile The S-record file with full path if applicable.
** \return    SB_TRUE on the S-record is valid, SB_FALSE otherwise.
**
****************************************************************************************/
sb_uint8 SrecordIsValid(const sb_char *srecordFile)
{
  sb_file tempHandle;
  sb_char line[SRECORD_MAX_CHARS_PER_LINE];

  /* attempt to open the file */
  tempHandle = SrecordOpen(srecordFile);
  /* is the file available? */
  if (tempHandle == SB_NULL)
  {
    /* cannot open the file */
    return SB_FALSE;
  }
  /* all lines should be formatted as S-records. read the first one to check this */
  if (SrecordReadLine(tempHandle, line) == SB_FALSE)
  {
    /* could not read a line. file must be empty */
    SrecordClose(tempHandle);
    return SB_FALSE;
  }
  /* check if the line starts with the 'S' character, followed by a digit */
  if ( (toupper(line[0]) != 'S') || (isdigit(line[1]) == 0) )
  {
    SrecordClose(tempHandle);
    return SB_FALSE;
  }

  /* still here so it is a valid s-record */
  SrecordClose(tempHandle);
  return SB_TRUE;
} /*** end of SrecordIsValid ***/


/************************************************************************************//**
** \brief     Opens the S-record file for reading.
** \param     srecordFile The S-record file with full path if applicable.
** \return    The filehandle if successful, SB_NULL otherwise.
**
****************************************************************************************/
sb_file SrecordOpen(const sb_char *srecordFile)
{
  /* open the file for reading */
  return fopen(srecordFile, "r");
} /*** end of SrecordOpen ***/


/************************************************************************************//**
** \brief     Parse the S-record file to obtain information about its contents.
** \param     srecordHandle The S-record file handle. It is returned by SrecordOpen.
** \param     parseResults Pointer to where the parse results should be stored.
** \return    none.
**
****************************************************************************************/
void SrecordParse(sb_file srecordHandle, tSrecordParseResults *parseResults)
{
  tSrecordLineParseResults lineResults;

  /* start at the beginning of the file */
  rewind(srecordHandle);
  
  /* init data structure */
  parseResults->address_high = 0;
  parseResults->address_low = 0xffffffff;
  parseResults->data_bytes_total = 0;

  /* loop through all S-records with program data */
  while (SrecordParseNextDataLine(srecordHandle, &lineResults) == SB_TRUE)
  {
    /* update byte total */
    parseResults->data_bytes_total += lineResults.length;
    /* is this a new lowest address? */
    if (lineResults.address < parseResults->address_low)
    {
      parseResults->address_low = lineResults.address;
    }
    /* is this a new highest address? */
    if ((lineResults.address + lineResults.length - 1) > parseResults->address_high)
    {
      parseResults->address_high = (lineResults.address + lineResults.length - 1);
    }
  }
  /* reset to the beginning of the file again */
  rewind(srecordHandle);
} /*** end of SrecordParse ***/


/************************************************************************************//**
** \brief     Closes the S-record file.
** \param     srecordHandle The S-record file handle. It is returned by SrecordOpen.
** \return    none.
**
****************************************************************************************/
void SrecordClose(sb_file srecordHandle)
{
  /* close the file handle if valid */
  if (srecordHandle != SB_NULL)
  {
    fclose(srecordHandle);
  }
} /*** end of SrecordClose ***/


/************************************************************************************//**
** \brief     Reads the next S-record with program data, parses it and returns the 
**            results.
** \param     srecordHandle The S-record file handle. It is returned by SrecordOpen.
** \param     parseResults Pointer to where the parse results should be stored.
** \return    SB_TRUE is valid parse results were stored. SB_FALSE in case of end-of-
**            file.
**
****************************************************************************************/
sb_uint8 SrecordParseNextDataLine(sb_file srecordHandle, tSrecordLineParseResults *parseResults)
{
  sb_char line[SRECORD_MAX_CHARS_PER_LINE];
  sb_uint8 data_line_found = SB_FALSE;
  tSrecordLineType lineType;
  sb_uint16 bytes_on_line;
  sb_uint16 i;
  sb_char *linePtr;

  /* first set the length paramter to 0 */
  parseResults->length = 0;

  while (data_line_found == SB_FALSE)
  {
    /* read the next line from the file */
    if (SrecordReadLine(srecordHandle, line) == SB_FALSE)
    {
      /* end-of-file encountered */
      return SB_FALSE;
    }
    /* we now have a line. check if it is a S-record data line */
    lineType = SrecordGetLineType(line);
    if (lineType != LINE_TYPE_UNSUPPORTED)
    {
      /* check if the checksum on the line is correct */
      if (SrecordVerifyChecksum(line) == SB_TRUE)
      {
        /* found a valid line that can be parsed. loop will stop */
        data_line_found = SB_TRUE;
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
  return SB_TRUE;
} /*** end of SrecordParseNextDataLine ***/


/************************************************************************************//**
** \brief     Inspects a line from a Motorola S-Record file to determine its type.
** \param     line A line from the S-Record.
** \return    the S-Record line type.
**
****************************************************************************************/
static tSrecordLineType SrecordGetLineType(const sb_char *line)
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
static sb_uint8 SrecordVerifyChecksum(const sb_char *line)
{
  sb_uint16 bytes_on_line;
  sb_uint8  checksum = 0;
  
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
    return SB_FALSE;
  }
  /* still here so the checksum was correct */
  return SB_TRUE;
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
static sb_uint8 SrecordHexStringToByte(const sb_char *hexstring)
{
  sb_uint8 result = 0;
  sb_char  c;
  sb_uint8 counter;
  
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
static sb_uint8 SrecordReadLine(sb_file srecordHandle, sb_char *line)
{
  /* init the line as an empty line */
  line[0] = '\0';

  /* loop as long as we find a non-empty line or end-of-file */
  while (line[0] == '\0') 
  {
    if (fgets(line, SRECORD_MAX_CHARS_PER_LINE, srecordHandle) == SB_NULL)
    {
      /* no more lines available */
      return SB_FALSE;
    }
    /* replace the line termination with a string termination */
    line[strcspn(line, "\n")] = '\0';
  }
  /* still here so not EOF and not and empty line, so success */
  return SB_TRUE;
} /*** end of SrecordReadLine ***/


/*********************************** end of srecord.c **********************************/
