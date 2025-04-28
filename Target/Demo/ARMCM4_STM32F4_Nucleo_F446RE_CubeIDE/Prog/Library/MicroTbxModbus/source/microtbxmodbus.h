/************************************************************************************//**
* \file         microtbxmodbus.h
* \brief        MicroTBX-Modbus header file.
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
#ifndef MICROTBXMODBUS_H
#define MICROTBXMODBUS_H

/****************************************************************************************
* Include files
****************************************************************************************/
#include <stdint.h>                              /* Standard integer types             */
#include <stddef.h>                              /* Standard definitions               */
#include "tbxmb_common.h"                        /* MicroTBX-Modbus common definitions */
#include "tbxmb_tp.h"                            /* MicroTBX-Modbus transport layer    */
#include "tbxmb_uart.h"                          /* MicroTBX-Modbus UART               */
#include "tbxmb_rtu.h"                           /* MicroTBX-Modbus RTU                */
#include "tbxmb_event.h"                         /* MicroTBX-Modbus event handling     */
#include "tbxmb_server.h"                        /* MicroTBX-Modbus server             */
#include "tbxmb_client.h"                        /* MicroTBX-Modbus client             */
#include "tbxmb_port.h"                          /* MicroTBX-Modbus hardware port      */


#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************************
* Version definitions
****************************************************************************************/
/** \brief Main version number of MicroTBX-Modbus. */
#define TBX_MB_VERSION_MAIN                  (0U)

/** \brief Minor version number of MicroTBX-Modbus. */
#define TBX_MB_VERSION_MINOR                 (9U)

/** \brief Patch number of MicroTBX-Modbus. */
#define TBX_MB_VERSION_PATCH                 (4U)


#ifdef __cplusplus
}
#endif

#endif /* MICROTBXMODBUS_H */
/*********************************** end of microtbxmodbus.h ***************************/
