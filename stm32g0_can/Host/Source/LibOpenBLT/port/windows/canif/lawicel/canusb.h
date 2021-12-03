/************************************************************************************//**
* \file         canusb.h
* \brief        Lawicel CANUSB interface header file.
* \ingroup      Lawicel_CanUsb
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
* \defgroup   Lawicel_CanUsb Lawicel CANUSB interface
* \brief      This module implements the CAN interface for the Lawicel CANUSB.
* \details    When using the Lawicel CANUSB interface, the 32-bit driver for the CANUSB 
*             DLL API should be installed: 
*             http://www.can232.com/download/canusb_setup_win32_v_2_2.zip
* \ingroup    CanDriver
****************************************************************************************/
#ifndef CANUSB_H
#define CANUSB_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************
* Function prototypes
****************************************************************************************/
tCanInterface const * CanUsbGetInterface(void);

#ifdef __cplusplus
}
#endif

#endif /* CANUSB_H */
/*********************************** end of canusb.h ***********************************/
