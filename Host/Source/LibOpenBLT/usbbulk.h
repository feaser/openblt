/************************************************************************************//**
* \file         usbbulk.h
* \brief        USB bulk driver header file.
* \ingroup      XcpTpUsb
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
#ifndef USBBULK_H
#define USBBULK_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Function prototypes
****************************************************************************************/
void UsbBulkInit(void);
void UsbBulkTerminate(void);
bool UsbBulkOpen(void);
void UsbBulkClose(void);
bool UsbBulkWrite(uint8_t const * data, uint16_t length);
bool UsbBulkRead(uint8_t * data, uint16_t length, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* USBBULK_H */
/********************************* end of usbbulk.h ************************************/

