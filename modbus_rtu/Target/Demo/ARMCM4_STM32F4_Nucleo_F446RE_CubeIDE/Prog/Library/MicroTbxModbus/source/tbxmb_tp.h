/************************************************************************************//**
* \file         tbxmb_tp.h
* \brief        Modbus transport layer header file.
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
#ifndef TBXMB_TP_H
#define TBXMB_TP_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Node address value for broadcast purposes. */
#define TBX_MB_TP_NODE_ADDR_BROADCAST       (0U)

/** \brief Minimum value of a valid node address. */
#define TBX_MB_TP_NODE_ADDR_MIN             (1U)

/** \brief Maximum value of a valid node address. */
#define TBX_MB_TP_NODE_ADDR_MAX             (247U)

/** \brief Maximum size of the "Function code" at the start of a PDU. */
#define TBX_MB_TP_PDU_CODE_LEN_MAX          (1U)

/** \brief Maximum number of data bytes inside a PDU. This excludes the function code. */
#define TBX_MB_TP_PDU_DATA_LEN_MAX          (252U)

/** \brief Maximum length of an "Protocol Data Unit". */
#define TBX_MB_TP_PDU_MAX_LEN               (TBX_MB_TP_PDU_CODE_LEN_MAX + \
                                             TBX_MB_TP_PDU_DATA_LEN_MAX) 


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Handle to a Modbus transport layer object, in the format of an opaque
 *         pointer.
 */
typedef void * tTbxMbTp;


#ifdef __cplusplus
}
#endif

#endif /* TBXMB_TP_H */
/*********************************** end of tbxmb_tp.h *********************************/
