/************************************************************************************//**
* \file         firmware.c
* \brief        Firmware module source file.
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
#include <assert.h>                         /* for assertions                          */
#include "firmware.h"                       /* firmware module                         */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Pointer to the firmware parser that is linked. */
static tFirmwareParser const * parserPtr = NULL;


/************************************************************************************//**
** \brief     Initializes the firmware module.
** \param     parser Pointer to the firmware parser to link.
** \return    None.
**
****************************************************************************************/
void FirmwareInit(tFirmwareParser const * const parser)
{
  /* verify parameters */
  assert(parser != NULL);

  /* link the firmware parser */
  parserPtr = parser;
  /* initialize the firmware parser */
  parserPtr->Init();
} /*** end of FirmwareInit ***/


/************************************************************************************//**
** \brief     Uninitializes the firmware module.
** \return    None.
**
****************************************************************************************/
void FirmwareDeinit(void)
{
  /* make sure the parser is linked */
  assert(parserPtr != NULL);

  /* uninitialize the parser */
  parserPtr->Deinit();
  /* unlink the parser */
  parserPtr = NULL;
} /*** end of FirmwareDeinit ***/


/************************************************************************************//**
** \brief     Loads the firmware data from the specified firmware file, using the linked
**            parser.
** \return    True is successful, false otherwise.
**
****************************************************************************************/
bool FirmwareLoadFromFile(char *firmwareFile)
{
  /* make sure the parser is linked */
  assert(parserPtr != NULL);
  /* make sure the filename is valid */
  assert(firmwareFile != NULL);

  return parserPtr->LoadFromFile(firmwareFile);
} /*** end of FirmwareLoadFromFile ***/


/************************************************************************************//**
** \brief     Returns the number of firmware segments that were loaded by the parser.
** \return    Number of firmware segments.
**
****************************************************************************************/
uint32_t FirmwareGetSegmentCount(void)
{
  /* make sure the parser is linked */
  assert(parserPtr != NULL);

  return parserPtr->GetSegmentCount();
} /*** end of FirmwareGetSegmentCount ***/


/************************************************************************************//**
** \brief     Obtains a pointer to the firmware segment at the specified index.
** \return    Pointer to firmware segment if successful, NULL otherwise.
**
****************************************************************************************/
const tFirmwareSegment *FirmwareGetSegment(uint32_t segmentIdx)
{
  /* make sure the parser is linked */
  assert(parserPtr != NULL);

  return parserPtr->GetSegment(segmentIdx);
} /*** end of FirmwareGetSegment ***/


/*********************************** end of firmware.c *********************************/

