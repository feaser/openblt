/************************************************************************************//**
* \file         firmware.h
* \brief        Firmware module header file.
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
#ifndef FIRMWARE_H
#define FIRMWARE_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Include files
****************************************************************************************/
#include <stdint.h>                         /* for standard integer types              */
#include <stdbool.h>                        /* for boolean type                        */


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Groups information together of a firmware segments. */
typedef struct t_firmware_segment
{
  uint32_t base;                  /**< Start memory address of the segment.            */
  uint32_t length;                /**< Number of data bytes in the segment.            */
  uint8_t *data;                  /**< Pointer to array with the segment's data bytes. */
} tFirmwareSegment;

/** \brief Firmware file parser. */
typedef struct t_firmware_parser
{
  /** \brief Initialization of the file parser. */
  void (*Init) (void);
  /** \brief Uninitializes the file parser. */
  void (*Deinit) (void);
  /** \brief Extract the firmware segments from the firmware file. */
  bool (*LoadFromFile) (char *firmwareFile);
  /** \brief Obtains the number of segments. */
  uint32_t (*GetSegmentCount) (void);
  /** \brief Obtains a segment. */
  const tFirmwareSegment * (*GetSegment) (uint32_t segmentIdx);
} tFirmwareParser;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void FirmwareInit(tFirmwareParser const * const parser);
void FirmwareDeinit(void);
bool FirmwareLoadFromFile(char *firmwareFile);
uint32_t FirmwareGetSegmentCount(void);
const tFirmwareSegment *FirmwareGetSegment(uint32_t segmentIdx);

#ifdef __cplusplus
}
#endif

#endif /* FIRMWARE_H */
/********************************* end of firmware.h ***********************************/

