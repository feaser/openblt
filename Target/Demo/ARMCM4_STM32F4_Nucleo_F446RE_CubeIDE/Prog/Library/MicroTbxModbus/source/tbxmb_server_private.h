/************************************************************************************//**
* \file         tbxmb_server_private.h
* \brief        Modbus server private header file.
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
#ifndef TBXMB_SERVER_PRIVATE_H
#define TBXMB_SERVER_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Modbus server channel layer context that groups all channel specific data. 
 *         It's what the tTbxMbServer opaque pointer points to.
 */
typedef struct
{
  /* Event interface methods. The following three entries must always be at the start
   * and exactly match those in tTbxMbEventCtx. Think of it as the base that this struct
   * derives from. 
   */
  void                        * instancePtr;        /**< Reserved for C++ wrapper.     */
  tTbxMbEventPoll               pollFcn;            /**< Event poll function.          */
  tTbxMbEventProcess            processFcn;         /**< Event process function.       */
  /* The type member must always be the first one after the three entries that match
   * those in tTbxMbEventCtx.
   */
  uint8_t                       type;               /**< Context type.                 */
  /* Private server channel specific methods and members. */
  tTbxMbTpCtx                 * tpCtx;              /**< Assigned transport layer ctx. */
  tTbxMbServerReadInput         readInputFcn;       /**< Read discrete input callback. */
  tTbxMbServerReadCoil          readCoilFcn;        /**< Read coil callback.           */
  tTbxMbServerWriteCoil         writeCoilFcn;       /**< Write coil callback.          */
  tTbxMbServerReadInputReg      readInputRegFcn;    /**< Read input register callback. */
  tTbxMbServerReadHoldingReg    readHoldingRegFcn;  /**< Read holding register cb.     */
  tTbxMbServerWriteHoldingReg   writeHoldingRegFcn; /**< Write holding register cb.    */
  tTbxMbServerCustomFunction    customFunctionFcn;  /**< Custom function code callback.*/  
} tTbxMbServerCtx;


#ifdef __cplusplus
}
#endif

#endif /* TBXMB_SERVER_PRIVATE_H */
/*********************************** end of tbxmb_server_private.h *********************/
