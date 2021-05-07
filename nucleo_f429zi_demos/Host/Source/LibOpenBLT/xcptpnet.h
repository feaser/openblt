/************************************************************************************//**
* \file         xcptpnet.h
* \brief        XCP TCP/IP transport layer header file.
* \ingroup      XcpTpNet
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
* \defgroup   XcpTpNet XCP TCP/IP transport layer
* \brief      This module implements the XCP transport layer for TCP/IP.
* \ingroup    XcpLoader
****************************************************************************************/
#ifndef XCPTPNET_H
#define XCPTPNET_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Layout of structure with settings specific to the XCP transport layer module
 *         for TCP/IP.
 */
typedef struct t_xcp_tp_net_settings
{
  char const * address;          /**< Target IP-address or hostname on the network.    */
  uint16_t port;                 /**< TCP port to use.                                 */
} tXcpTpNetSettings;


/***************************************************************************************
* Function prototypes
****************************************************************************************/
tXcpTransport const * XcpTpNetGetTransport(void);

#ifdef __cplusplus
}
#endif

#endif /* XCPTPNET_H */
/*********************************** end of xcptpnet.h *********************************/
