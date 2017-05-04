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
#include "firmware.h"                       /* Firmware data module                    */
#include "srecparser.h"                     /* S-record parser                         */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
bool SRecParserLoadFromFile (char const * firmwareFile);
bool SRecParserSaveToFile (char const * firmwareFile);


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
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool SRecParserLoadFromFile (char const * firmwareFile)
{
  bool result = false;
  
  /* Check parameters. */
  assert(firmwareFile != NULL);
  
  /* Only continue if the parameters are valid. */
  if (firmwareFile != NULL) /*lint !e774 */
  {
    /* TODO Implement. Use function FirmwareAddData to add the firmware data. */
    result = true;
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of SRecParserLoadFromFile ***/


/************************************************************************************//**
** \brief     Writes firmware data to the specified file in the correct file format.
** \param     firmwareFile Filename of the firmware file to write to.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool SRecParserSaveToFile (char const * firmwareFile)
{
  bool result = false;
  
  /* Check parameters. */
  assert(firmwareFile != NULL);
  
  /* Only continue if the parameters are valid. */
  if (firmwareFile != NULL) /*lint !e774 */
  {
    /* TODO Implement. Use functions FirmwareGetSegment and FirmwareGetSegmentCount to
     *      access the firmware data that should be written. 
     */
    result = true;
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of SRecParserSaveToFile ***/


/*********************************** end of srecparser.c *******************************/

 
 
