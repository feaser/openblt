/************************************************************************************//**
* \file         tbxmb_client_private.h
* \brief        Modbus client private header file.
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
#ifndef TBXMB_CLIENT_PRIVATE_H
#define TBXMB_CLIENT_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Modbus client channel interface function to detect events in a polling
 *         manner.
 */
typedef void (* tTbxMbClientPoll)   (void        * context);


/** \brief Modbus client channel  interface function for processing events. */
typedef void (* tTbxMbClientProcess)(tTbxMbEvent * event);


/** \brief Modbus client channel layer context that groups all channel specific data. 
 *         It's what the tTbxMbClient opaque pointer points to.
 */
typedef struct
{
  /* Event interface methods. The following three entries must always be at the start
   * and exactly match those in tTbxMbEventCtx. Think of it as the base that this struct
   * derives from. 
   */
  void               * instancePtr;              /**< Reserved for C++ wrapper.        */
  tTbxMbClientPoll     pollFcn;                  /**< Event poll function.             */
  tTbxMbClientProcess  processFcn;               /**< Event process function.          */
  /* Private members. */
  uint8_t              type;                     /**< Context type.                    */
  tTbxMbTpCtx        * tpCtx;                    /**< Assigned transport layer context.*/
  uint16_t             responseTimeout;          /**< Maximum response wait time (ms). */
  uint16_t             turnaroundDelay;          /**< Delay (ms) after broadcast PDU.  */
  tTbxMbOsalSem        transceiveSem;            /**< PDU transmit/receive semaphore.  */
} tTbxMbClientCtx;


#ifdef __cplusplus
}
#endif

#endif /* TBXMB_CLIENT_PRIVATE_H */
/*********************************** end of tbxmb_client_private.h *********************/
