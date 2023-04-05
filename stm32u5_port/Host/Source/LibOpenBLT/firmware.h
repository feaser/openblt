/************************************************************************************//**
* \file         firmware.h
* \brief        Firmware data module header file.
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
/************************************************************************************//**
* \defgroup   Firmware Firmware Data Module
* \brief      Module with functionality to load, manipulate and store firmware data.
* \ingroup    Library
* \details
* The Firmwarwe Data module contains functionality to load, manipulate and store firmware
* data. It contains an interface for linking firmware file parsers that handle the
* loading and saving the firmware data from and to a file in the correct format. For 
* example the Motorola S-record format.
****************************************************************************************/
#ifndef FIRMWARE_H
#define FIRMWARE_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Groups information together of a firmware segment, such that it can be used
 *         as a node in a linked list. 
 */
typedef struct t_firmware_segment
{
  /** \brief Start memory address of the segment. */
  uint32_t base;  
  /** \brief Number of data bytes in the segment. */
  uint32_t length;                
  /** \brief Pointer to array with the segment's data bytes. */
  uint8_t *data;                  
  /** \brief Pointer to the previous node, or NULL if it is the first one. */
  struct t_firmware_segment * prev;
  /** \brief Pointer to the next node, or NULL if it is the last one. */
  struct t_firmware_segment * next;
} tFirmwareSegment;

/** \brief Firmware file parser. */
typedef struct t_firmware_parser
{
  /** \brief Extract the firmware segments from the firmware file and add them as nodes
   *         to the linked list.
   */
  bool (* LoadFromFile) (char const * firmwareFile, uint32_t addressOffset);
  /** \brief Write all the firmware segments from the linked list to the specified
   *         firmware file.
   */
  bool (* SaveToFile) (char const * firmwareFile);
} tFirmwareParser;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void FirmwareInit(tFirmwareParser const * parser);
void FirmwareTerminate(void);
bool FirmwareLoadFromFile(char const * firmwareFile, uint32_t addressOffset);
bool FirmwareSaveToFile(char const * firmwareFile);
uint32_t FirmwareGetSegmentCount(void);
tFirmwareSegment * FirmwareGetSegment(uint32_t segmentIdx);
bool FirmwareAddData(uint32_t address, uint32_t len, uint8_t const * data);
bool FirmwareRemoveData(uint32_t address, uint32_t len);
void FirmwareClearData(void);


#ifdef __cplusplus
}
#endif

#endif /* FIRMWARE_H */
/********************************* end of firmware.h ***********************************/

 
 
