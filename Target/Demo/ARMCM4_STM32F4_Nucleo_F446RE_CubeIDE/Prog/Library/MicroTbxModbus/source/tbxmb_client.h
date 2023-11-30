/************************************************************************************//**
* \file         tbxmb_client.h
* \brief        Modbus client header file.
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
#ifndef TBXMB_CLIENT_H
#define TBXMB_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Handle to a Modbus client channel object, in the format of an opaque
 *         pointer.
 */
typedef void * tTbxMbClient;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
tTbxMbClient TbxMbClientCreate          (tTbxMbTp             transport,
                                         uint16_t             responseTimeout,
                                         uint16_t             turnaroundDelay);

void         TbxMbClientFree            (tTbxMbClient         channel);

uint8_t      TbxMbClientReadCoils       (tTbxMbClient         channel,
                                         uint8_t              node,
                                         uint16_t             addr,
                                         uint16_t             num,
                                         uint8_t            * coils);

uint8_t      TbxMbClientReadInputs      (tTbxMbClient         channel,
                                         uint8_t              node,
                                         uint16_t             addr,
                                         uint16_t             num,
                                         uint8_t            * inputs);

uint8_t      TbxMbClientReadInputRegs   (tTbxMbClient         channel,
                                         uint8_t              node,
                                         uint16_t             addr,
                                         uint8_t              num,
                                         uint16_t           * inputRegs);

uint8_t      TbxMbClientReadHoldingRegs (tTbxMbClient         channel,
                                         uint8_t              node,
                                         uint16_t             addr,
                                         uint8_t              num,
                                         uint16_t           * holdingRegs);

uint8_t      TbxMbClientWriteCoils      (tTbxMbClient         channel,
                                         uint8_t              node,
                                         uint16_t             addr,
                                         uint16_t             num,
                                         uint8_t      const * coils);

uint8_t      TbxMbClientWriteHoldingRegs(tTbxMbClient         channel,
                                         uint8_t              node,
                                         uint16_t             addr,
                                         uint8_t              num,
                                         uint16_t     const * holdingRegs);

uint8_t      TbxMbClientDiagnostics     (tTbxMbClient         channel,
                                         uint8_t              node,
                                         uint16_t             subcode,
                                         uint16_t           * count);

uint8_t      TbxMbClientCustomFunction  (tTbxMbClient         channel,
                                         uint8_t              node,
                                         uint8_t      const * txPdu,
                                         uint8_t            * rxPdu,
                                         uint8_t            * len);


#ifdef __cplusplus
}
#endif

#endif /* TBXMB_CLIENT_H */
/*********************************** end of tbxmb_client.h *****************************/
