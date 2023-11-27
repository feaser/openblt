/************************************************************************************//**
* \file         tbxmb_event_private.h
* \brief        Modbus event handler private header file.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2023 by Feaser     www.feaser.com     All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* This file is part of MicroTBX-Modbus. MicroTBX-Modbus is free software: you can
* redistribute it and/or modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* MicroTBX-Modbus is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
* PARTICULAR PURPOSE. See the GNU General Public License for more details.
*
* You have received a copy of the GNU General Public License along with MicroTBX-Modbus.
* If not, see www.gnu.org/licenses/.
*
* \endinternal
****************************************************************************************/
#ifndef TBXMB_EVENT_PRIVATE_H
#define TBXMB_EVENT_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Enumerated type with all supported events. */
typedef enum
{
  /* Start calling the context's polling function each time TbxMbEventTask() runs. */
  TBX_MB_EVENT_ID_START_POLLING = 0U,
  /* Stop calling the context's polling function. */
  TBX_MB_EVENT_ID_STOP_POLLING,
  /* Transport layer received a new protocol data unit (PDU). */
  TBX_MB_EVENT_ID_PDU_RECEIVED,
  /* Transport layer completed transmission of a protocol data unit (PDU). */
  TBX_MB_EVENT_ID_PDU_TRANSMITTED,
  /* Extra entry to obtain the number of elements. */
  TBX_MB_EVENT_NUM_ID
} tTbxMbEventId;


/** \brief Groups all event related information. */
typedef struct
{
  tTbxMbEventId   id;                            /**< Event identifier.                */
  void          * context;                       /**< Opaque event context.            */
} tTbxMbEvent;


#ifdef __cplusplus
}
#endif

#endif /* TBXMB_EVENT_PRIVATE_H */
/*********************************** end of tbxmb_event_private.h **********************/
