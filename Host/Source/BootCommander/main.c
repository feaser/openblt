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

  /* Load firmware data from an S-record. */
  (void)BltFirmwareLoadFromFile("/home/voorburg/Desktop/TestSrecords/demoprog.srec");
  
  /* Display the firmware data on the standard output. */
  DumpFirmwareData();
  
  /* Write the firmware data to an S-record. */
  (void)BltFirmwareSaveToFile("/home/voorburg/Desktop/TestSrecords/demoprog2.srec");

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
        if ((byteIdx % 16) == 0)
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

 
