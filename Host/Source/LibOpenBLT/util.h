/************************************************************************************//**
* \file         util.h
* \brief        Utility module header file.
* \ingroup      Utility
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
* \defgroup   Utility Generic Utilities
* \brief      Generic utility functions and definitions.
* \ingroup    Library
* \details
* The Utility module contains generic functions and definitions that can be handy for
* use internally in the library and also externally by another application that makes
* use of the library.
****************************************************************************************/
#ifndef UTIL_H
#define UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Function prototypes
****************************************************************************************/
uint16_t UtilChecksumCrc16Calculate(uint8_t const * data, uint32_t len);
uint32_t UtilChecksumCrc32Calculate(uint8_t const * data, uint32_t len);
bool UtilFileExtractFilename(char const * fullFilename, char * filenameBuffer);
uint32_t UtilTimeGetSystemTimeMs(void);
void UtilTimeDelayMs(uint16_t delay);
void UtilCriticalSectionInit(void);
void UtilCriticalSectionTerminate(void);
void UtilCriticalSectionEnter(void);
void UtilCriticalSectionExit(void);
bool UtilCryptoAes256Encrypt(uint8_t * data, uint32_t len, uint8_t const * key);
bool UtilCryptoAes256Decrypt(uint8_t * data, uint32_t len, uint8_t const * key);


#ifdef __cplusplus
}
#endif

#endif /* UTIL_H */
/********************************* end of util.h ***************************************/

 
 
