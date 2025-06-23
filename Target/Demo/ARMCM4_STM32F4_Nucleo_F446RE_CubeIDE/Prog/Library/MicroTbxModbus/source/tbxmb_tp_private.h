/************************************************************************************//**
* \file         tbxmb_tp_private.h
* \brief        Modbus transport layer private header file.
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
#ifndef TBXMB_TP_PRIVATE_H
#define TBXMB_TP_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Maximum ADU overhead bytes before the actual PDU, Called "Additional address"
 *         in the Modbus protocol.
 */
#define TBX_MB_TP_ADU_HEAD_LEN_MAX     (7U)

/** \brief Maximum ADU overhead bytes after the actual PDU. Called "Error check" in the 
 *         Modbus protocol.
 */
#define TBX_MB_TP_ADU_TAIL_LEN_MAX     (2U)

/** \brief Maximum length of an "Application Data Unit" which packs communication layer
 *         data around a PDU.
 */
#define TBX_MB_TP_ADU_MAX_LEN          (TBX_MB_TP_ADU_HEAD_LEN_MAX + \
                                        TBX_MB_TP_PDU_MAX_LEN + \
                                        TBX_MB_TP_ADU_TAIL_LEN_MAX)


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Type for grouping all "Protocol Data Unit" data together. */
typedef struct
{
  uint8_t     code;                                    /**< PDU function code.         */
  uint8_t     data[TBX_MB_TP_PDU_DATA_LEN_MAX];        /**< PDU data bytes.            */
} tTbxMbTpPdu;


/** \brief Type for grouping all communication packet related data together. */
typedef struct 
{
  uint8_t     head[TBX_MB_TP_ADU_HEAD_LEN_MAX];        /**< ADU additional address.    */
  tTbxMbTpPdu pdu;                                     /**< Protocol data unit.        */
  uint8_t     tail[TBX_MB_TP_ADU_TAIL_LEN_MAX];        /**< ADU error check.           */
  uint8_t     dataLen;                                 /**< Number of PDU data bytes.  */
  uint8_t     node;                                    /**< Node identifier.           */
} tTbxMbTpPacket;


/** \brief Type for grouping all diagnostics related information together. */
typedef struct
{
  /** \brief Total number of received packets regardless of addressing or CRC. */
  uint16_t busMsgCnt; 
  /** \brief Total number of busMsgCnt that had an incorrect CRC issue. */
  uint16_t busCommErrCnt;
  /** \brief Total number of transmitted exception responses. */
  uint16_t busExcpErrCnt;
  /** \brief Total number of addresssed (uni/broad) reception packets with correct CRC.*/
  uint16_t srvMsgCnt;
  /** \brief Total number responses that could not be transmitted. */
  uint16_t srvNoRespCnt;
} tTbxMbTpDiagInfo;


/** \brief Transport layer interface function to start the transmission of the data 
 *         packet, stored in the transport layer context. Use getTxPacketFcn() to
 *         obtain access to the transmit packet.
 */
typedef uint8_t (* tTbxMbTpTransmit)            (tTbxMbTp      transport);


/** \brief Transport layer interface function to signal that the channel is done
 *         processing the newly received packet and no longer needs access to it.
 *         Should be called by a channel after it called getRxPacketFcn() while handling
 *         the TBX_MB_EVENT_ID_PDU_RECEIVED event.
 */
typedef void (* tTbxMbTpReceptionDone)          (tTbxMbTp      transport);


/** \brief Transport layer interface function to be called by a channel to obtain read
 *         access to the reception packet. Returns NULL is the packet is currently not
 *         accessible. Can be called when processing the TBX_MB_EVENT_ID_PDU_RECEIVED
 *         event.
 */
typedef tTbxMbTpPacket * (* tTbxMbTpGetRxPacket)(tTbxMbTp      transport);


/** \brief Transport layer interface function to be called by a channel to obtain write
 *         access to the transmission packet. Returns NULL is the packet is currently not
 *         accessible. Can by called to prepare the transmit packet before calling the
 *         transport layer's transmitFcn().
 */
typedef tTbxMbTpPacket * (* tTbxMbTpGetTxPacket)(tTbxMbTp      transport);


/** \brief   Modbus transport layer context that groups all transport layer specific
 *           data. It's what the tTbxMbTp opaque pointer points to. Each transport
 *           layer such as RTU, TCP and ASCII define there own context that derives from
 *           this one.
 */
typedef struct
{
  /* Event interface methods. The following three entries must always be at the start
   * and exactly match those in tTbxMbEventCtx. Think of it as the base that this struct
   * derives from. 
   */
  void                  * instancePtr;           /**< Reserved for C++ wrapper.        */
  tTbxMbEventPoll         pollFcn;               /**< Event poll function.             */
  tTbxMbEventProcess      processFcn;            /**< Event process function.          */
  /* The type member must always be the first one after the three entries that match
   * those in tTbxMbEventCtx.
   */
  uint8_t                 type;                  /**< Context type.                    */
  /* Public methods and members shared between all transport layers. These must always
   * follow the type member and be in exactly the same order for all transport layers.
   */
  void                  * channelCtx;            /**< Assigned channel context.        */
  uint8_t                 isClient;              /**< Info about the channel context.  */
  tTbxMbTpDiagInfo        diagInfo;              /**< Diagnostics information.         */
  tTbxMbTpTransmit        transmitFcn;           /**< Packet transmit function.        */
  tTbxMbTpReceptionDone   receptionDoneFcn;      /**< Rx packet processing done fcn.   */
  tTbxMbTpGetRxPacket     getRxPacketFcn;        /**< Obtain Rx packet access function.*/
  tTbxMbTpGetTxPacket     getTxPacketFcn;        /**< Obtain Rx packet access function.*/
} tTbxMbTpCtx;


#ifdef __cplusplus
}
#endif

#endif /* TBXMB_TP_PRIVATE_H */
/*********************************** end of tbxmb_tp_private.h *************************/
