/************************************************************************************//**
* \file         Source/events.h
* \brief        Bootloader events module header file.
* \ingroup      Core
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2026  by Feaser    http://www.feaser.com    All rights reserved
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
#ifndef EVENTS_H
#define EVENTS_H

#if (BOOT_EVENTS_ENABLE > 0)
/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Enumeration for the event identifiers. */
typedef enum
{
  EVENT_ID_ON_ENTRY = 0U,              /**< Bootloader initialization is done.         */
  EVENT_ID_ON_EXIT,                    /**< About to exit and start the user program.  */
  EVENT_ID_ON_SUPPRESS,                /**< User program start is suppressed.          */
  EVENT_ID_ON_START,                   /**< Firmware update starts.                    */
  EVENT_ID_ON_SUCCESS,                 /**< Firmware update successfully completed.    */
  EVENT_ID_ON_ERROR,                   /**< Firmware update aborted due to error.      */
  EVENT_ID_ON_ERASE,                   /**< Erasing non-volatile memory.               */
  EVENT_ID_ON_WRITE,                   /**< Writing non-volatile memory.               */
  NUM_EVENT_IDS                        /**< Total number of event identifiers.         */
} tEventsId;

/** \brief Enumeration for the EVENT_ID_ON_START firmware update types. */
typedef enum
{
  EVENT_START_TYPE_NORMAL = 0U,        /**< Normal firmware update.                    */
  EVENT_START_TYPE_ROLLBACK_CREATE,    /**< Create a new rollback image.               */
  EVENT_START_TYPE_ROLLBACK_RESTORE,   /**< Restore from the rollback image.           */  
  NUM_EVENT_START_TYPES                /**< Total number of firmware update types.     */
} tEventsStartType;

/** \brief Enumeration for the EVENT_ID_ON_ERROR error identifiers. */
typedef enum
{
  EVENT_ERROR_ID_ERASE = 0U,           /**< Could not erase NVM.                       */
  EVENT_ERROR_ID_WRITE,                /**< Could not write NVM.                       */
  EVENT_ERROR_ID_COM_TIMEOUT,          /**< No new communication command received.     */
  EVENT_ERROR_ID_XCP_REQUEST,          /**< Error while processing XCP request.        */
  EVENT_ERROR_ID_XCP_UNAUTHORIZED,     /**< Programming resource locked (seed/key).    */
  EVENT_ERROR_ID_INFO_TABLE_CHECK,     /**< Firmware info table check not passed.      */
  EVENT_ERROR_ID_INFO_TABLE_MISSING,   /**< Firmware info not in firmware file.        */
  EVENT_ERROR_ID_FILE_OPEN,            /**< Cannot open firmware file.                 */
  EVENT_ERROR_ID_FILE_READ,            /**< Cannot read from firmware file.            */
  EVENT_ERROR_ID_FILE_REWIND_READ_PTR, /**< Cannot rewind firmware file read pointer.  */
  EVENT_ERROR_ID_FILE_CHECKSUM,        /**< Invalid checksum detected in firmware file.*/
  EVENT_ERROR_ID_ROLLBACK_OPERATION,   /**< Rollback manager could not copy firmware.  */
  NUM_EVENT_ERROR_IDS                  /**< Total number of event error identifiers.   */
} tEventsErrorId;

/** \brief Structure type for grouping the EVENT_ID_ON_START related information.      */
typedef struct
{
  tEventsStartType type;               /**< Firmware update type.                      */
  blt_int8u        node_id;            /**< Node ID. Only applicable with gateway.     */             
  const blt_char * filename;           /**< Firmware filename (update from FatFS only) */
} tEventsInfoStart;

/** \brief Structure type for grouping the EVENT_ID_ON_ERROR related information.      */
typedef struct
{
  tEventsErrorId error_id;             /**< Error identifier.                          */
} tEventsInfoError;

/** \brief Structure type for grouping the EVENT_ID_ON_ERASE related information. */
typedef struct
{
  blt_addr       base_addr;            /**< Base address of the NVM erase operation.   */
  blt_int32u     num_bytes;            /**< Number of bytes requested to erase.        */
} tEventsInfoErase;

/** \brief Structure type for grouping the EVENT_ID_ON_WRITE related information. */
typedef struct
{
  blt_addr       base_addr;            /**< Base address of the NVM write operation.   */
  blt_int32u     num_bytes;            /**< Number of bytes requested to write.        */
  blt_int8u      progress;             /**< Firmware update write progress (percentage)*/
} tEventsInfoWrite;


/****************************************************************************************
* Hook functions
****************************************************************************************/
extern void EventsHook(tEventsId id, void const * info);


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void EventsInit(void);
void EventsProcess(tEventsId id, void * info);


#endif /* BOOT_EVENTS_ENABLE > 0 */


#endif /* EVENTS_H */
/*********************************** end of events.h ***********************************/
