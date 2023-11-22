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
* This file is part of OpenBLT. It is released under a commercial license. Refer to
* the license.pdf file for the exact licensing terms. It should be located in
* '.\Doc\license.pdf'. Contact Feaser if you did not receive the license.pdf file.
* 
* In a nutshell, you are allowed to modify and use this software in your closed source
* proprietary application, provided that the following conditions are met:
* 1. The software is not released or distributed in source (human-readable) form.
* 2. These licensing terms and the copyright notice above are not changed.
* 
* This software has been carefully tested, but is not guaranteed for any particular
* purpose. Feaser does not offer any warranties and does not guarantee the accuracy,
* adequacy, or completeness of the software and is not responsible for any errors or
* omissions or the results obtained from use of the software.
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
