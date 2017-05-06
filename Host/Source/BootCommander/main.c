/************************************************************************************//**
* \file         main.c
* \brief        BootCommander program source file.
* \ingroup      BootCommander
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
#include <stdio.h>                          /* Standard I/O functions.                 */
#include "openblt.h"                        /* OpenBLT library.                        */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* Program return codes. */
#define RESULT_OK                           (0u)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void DumpFirmwareData(void);


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
static const uint8_t testData1[8] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17 };
static const uint8_t testData2[8] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27 };
static const uint8_t testData3[8] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37 };
static const uint8_t testData4[8] = { 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47 };

static const uint8_t mergeData[64] = { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 
                                       0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
                                       0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 
                                       0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
                                       0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
                                       0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
                                       0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
                                       0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF };


/************************************************************************************//**
** \brief     This is the program entry point.
** \param     argc Number of program arguments.
** \param     argv Array with program arguments.
** \return    Program return code. 0 for success, error code otherwise.
**
****************************************************************************************/
int main(int argc, char *argv[])
{
  int result = RESULT_OK;
  
  (void)argc;
  (void)argv;
  
  /* Initialize the firmware data module using the S-record parser. */
  BltFirmwareInit(BLT_FIRMWARE_PARSER_SRECORD);

  /* Add firmware data. */
  (void)BltFirmwareAddData(0x1030, 8, testData4);
  (void)BltFirmwareAddData(0x1010, 8, testData2);
  (void)BltFirmwareAddData(0x1000, 8, testData1);
  (void)BltFirmwareAddData(0x1020, 8, testData3);
  
  /* Output firmware data. */
  printf("Original data:\n");
  DumpFirmwareData();
  
  /* Add some data. */
  (void)BltFirmwareAddData(0x1004, 16, mergeData); 
  
  /* Output firmware data. */
  printf("After change:\n");
  DumpFirmwareData();
  
  /* Terminate the firmware data module. */
  BltFirmwareTerminate();
  
  /* Give result back. */
  return result;
} /*** end of main ***/


/************************************************************************************//**
** \brief     Displays the contents of the firmware data on the standard output.
**
****************************************************************************************/
static void DumpFirmwareData(void)
{
  uint32_t i;
  uint8_t  * segmentData;
  uint32_t segmentBase;
  uint32_t segmentLen;
  uint32_t byteIdx;
  
  /* Loop through all segments with firmware data. */
  for (i = 0; i < BltFirmwareGetSegmentCount(); i++)
  {
    /* Extract segment information. */
    segmentData = BltFirmwareGetSegment(i, &segmentBase, &segmentLen);
    /* Process the segment is valid. */
    if (segmentData != NULL)
    {
      printf("Segment base = 0x%08X, len = %u", segmentBase, segmentLen);
      for (byteIdx = 0; byteIdx < segmentLen; byteIdx++)
      {
        if ((byteIdx % 8) == 0)
        {
          printf("\n  ");
        }
        printf("%02X ", segmentData[byteIdx]);
      }
      printf("\n");
      
    }
  }
} /*** end of DumpFirmwareData ***/


/*********************************** end of main.c *************************************/

 
