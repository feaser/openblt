/************************************************************************************//**
* \file         xcptpmbrtu.h
* \brief        XCP Modbus RTU transport layer header file.
* \ingroup      XcpTpMbRtu
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2022  by Feaser    http://www.feaser.com    All rights reserved
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
* \defgroup   XcpTpMbRtu XCP Modbus RTU transport layer
* \brief      This module implements the XCP transport layer for Modbus RTU.
* \ingroup    XcpLoader
****************************************************************************************/
#ifndef XCPTPMBRTU_H
#define XCPTPMBRTU_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Layout of structure with settings specific to the XCP transport layer module
 *         for Modbus RTU.
 */
typedef struct t_xcp_tp_mbrtu_settings
{
  char const * portname;               /**< Interface port name, i.e. /dev/ttyUSB0.    */
  uint32_t baudrate;                   /**< Communication speed in bits/sec.           */
  uint8_t parity;                      /**< Parity (0 for none, 1 for odd, 2 for even. */
  uint8_t stopbits;                    /**< Stopbits (1 for one, 2 for two stopbits.   */
  uint8_t destinationAddr;             /**< Destination address (receiver node ID).    */
} tXcpTpMbRtuSettings;


/***************************************************************************************
* Function prototypes
****************************************************************************************/
tXcpTransport const * XcpTpMbRtuGetTransport(void);

#ifdef __cplusplus
}
#endif

#endif /* XCPTPMBRTU_H */
/*********************************** end of xcptpmbrtu.h *******************************/
