/************************************************************************************//**
* \file         Source\file.h
* \brief        Bootloader file system interface header file.
* \ingroup      Core
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2013  by Feaser    http://www.feaser.com    All rights reserved
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
#ifndef FILE_H
#define FILE_H

#if (BOOT_FILE_SYS_ENABLE > 0)
/****************************************************************************************
* Include files
****************************************************************************************/
#include "ff.h"                                  /* FATFS file system library          */


/****************************************************************************************
* Defines
****************************************************************************************/
/** \brief Error code for not being able to open the firmware file. */
#define FILE_ERROR_CANNOT_OPEN_FIRMWARE_FILE       (1)
/** \brief Error code for not being able to read from the firmware file. */
#define FILE_ERROR_CANNOT_READ_FROM_FILE           (2)
/** \brief Error code because in incorrect checksum was found in the firmware file. */
#define FILE_ERROR_INVALID_CHECKSUM_IN_FILE        (3)
/** \brief Error code because the file pointers read pointer could not be rewinded. */
#define FILE_ERROR_REWINDING_FILE_READ_POINTER     (4)
/** \brief Error code because an error occurred during the memory erase operation. */
#define FILE_ERROR_CANNOT_ERASE_MEMORY             (5)
/** \brief Error code because an error occurred during the memory write operation. */
#define FILE_ERROR_CANNOT_PROGRAM_MEMORY           (6)
/** \brief Error code because the program's checksum could not be written to memory. */
#define FILE_ERROR_CANNOT_WRITE_CHECKSUM           (7)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void     FileInit(void);
void     FileTask(void);
blt_bool FileIsIdle(void);
blt_bool FileHandleFirmwareUpdateRequest(void);

#endif /* BOOT_FILE_SYS_ENABLE > 0 */


#endif /* FILE_H */
/*********************************** end of file.h *************************************/
