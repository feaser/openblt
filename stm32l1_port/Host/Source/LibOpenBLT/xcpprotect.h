/************************************************************************************//**
* \file         xcpprotect.h
* \brief        XCP Protection module header file.
* \ingroup      XcpLoader
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
#ifndef XCPPROTECT_H
#define XCPPROTECT_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* XCP supported resources. */
#define XCPPROTECT_RESOURCE_PGM        (0x10u)   /**< ProGraMing resource.             */
#define XCPPROTECT_RESOURCE_STIM       (0x08u)   /**< data STIMulation resource.       */
#define XCPPROTECT_RESOURCE_DAQ        (0x04u)   /**< Data AcQuisition resource.       */
#define XCPPROTECT_RESOURCE_CALPAG     (0x01u)   /**< CALibration and PAGing resource. */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void XcpProtectInit(char const * seedKeyFile);
void XcpProtectTerminate(void);
bool XCPProtectComputeKeyFromSeed(uint8_t resource, uint8_t seedLen,
                                  uint8_t const * seedPtr, uint8_t * keyLenPtr,
                                  uint8_t * keyPtr);
bool XcpProtectGetPrivileges(uint8_t * resourcePtr);


#ifdef __cplusplus
}
#endif

#endif /* XCPPROTECT_H */
/*********************************** end of xcpprotect.h *******************************/
