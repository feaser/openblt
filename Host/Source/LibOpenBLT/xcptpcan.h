/************************************************************************************//**
* \file         xcptpcan.h
* \brief        XCP CAN transport layer header file.
* \ingroup      XcpTpCan
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
/************************************************************************************//**
* \defgroup   XcpTpCan XCP CAN transport layer
* \brief      This module implements the XCP transport layer for CAN.
* \ingroup    XcpLoader
****************************************************************************************/
#ifndef XCPTPCAN_H
#define XCPTPCAN_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Layout of structure with settings specific to the XCP transport layer module
 *         for CAN.
 */
typedef struct t_xcp_tp_can_settings
{
  char const * device;           /**< Device name such as can0, peak_pcanusb, etc.     */
  uint32_t channel;              /**< Channel on the device to use.                    */
  uint32_t baudrate;             /**< Communication speed in bits/sec.                 */
  uint32_t transmitId;           /**< Transmit CAN identifier.                         */
  uint32_t receiveId;            /**< Receive CAN identifier.                          */
  bool useExtended;              /**< Boolean to configure 29-bit CAN identifiers.     */
} tXcpTpCanSettings;


/***************************************************************************************
* Function prototypes
****************************************************************************************/
tXcpTransport const * XcpTpCanGetTransport(void);

#ifdef __cplusplus
}
#endif

#endif /* XCPTPCAN_H */
/*********************************** end of xcptpcan.h *********************************/
