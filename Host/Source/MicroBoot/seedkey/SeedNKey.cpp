/************************************************************************************//**
* \file         SeedNKey.cpp
* \brief        XCP Seed/Key algorithms
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2014  by Feaser    http://www.feaser.com    All rights reserved
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
#ifdef  __cplusplus
extern "C"
{
#endif
/****************************************************************************************
* Type definitions
****************************************************************************************/
typedef unsigned char BYTE;
typedef unsigned long DWORD;


/****************************************************************************************
* Defines
****************************************************************************************/
/* XCP dll function type info */
#define XCP_DLL_EXPORT __declspec(dllexport) __cdecl

/* XCP supported resources */
#define kXcpResPGM             0x10                   /* ProGraMing                    */
#define kXcpResSTIM            0x08                   /* data STIMulation              */
#define kXcpResDAQ             0x04                   /* Data AcQuisition              */
#define kXcpResCALPAG          0x01                   /* CALibration and PAGing        */


/****************************************************************************************
** NAME:               XCP_ComputeKeyFromSeed
** PARAMETER:          resource : resource for which the unlock key is requested
**                     seedLen  : length of the seed
**                     seedPtr  : pointer to the seed data
**                     keyLenPtr: pointer where to store the key length
**                     keyPtr   : pointer where to store the key data
** RETURN VALUE:       0: success, 1: error
** DESCRIPTION:        Computes the key for the requested resource.
**
****************************************************************************************/

/************************************************************************************//**
** \brief     Computes the key for the requested resource.
** \param     resource resource for which the unlock key is requested
** \param     seedLen  length of the seed
** \param     seedPtr  pointer to the seed data
** \param     keyLenPtr pointer where to store the key length
** \param     keyPtr pointer where to store the key data
** \return    0 on success, otherwise 1.
**
****************************************************************************************/
DWORD XCP_DLL_EXPORT XCP_ComputeKeyFromSeed( BYTE  resource, BYTE  seedLen, BYTE *seedPtr, 
                                             BYTE *keyLenPtr, BYTE *keyPtr)
{
  BYTE i;
  
  /* Feaser XCP driver example key algorithm for PGM simply
   * decrements the value of each seed by 1
   */
  if ( resource == kXcpResPGM )
  {
    /* compute the key */
    for ( i=0; i<seedLen; i++)
    {
      keyPtr[i] = seedPtr[i] - 1;
    }

    /* set the key length */
    *keyLenPtr = seedLen;
  
    /* done */
    return 0;
  }

  /* still here, so the resource was unsupported */
  return 1;
} /*** end of XCP_ComputeKeyFromSeed ***/
  

/************************************************************************************//**
** \brief     Computes the key for the requested resource.
** \param     resourcePtr pointer where to store the supported resources for the key 
**                        computation.
** \return    0 on success, otherwise 1.
**
****************************************************************************************/
DWORD XCP_DLL_EXPORT XCP_GetAvailablePrivileges( BYTE *resourcePtr)
{
  /* this dll supports key computation algorithm for the PGM resource */  
  *resourcePtr = (kXcpResPGM);

  return 0;
} /*** end of XCP_GetAvailablePrivileges ***/


#ifdef  __cplusplus
}
#endif

/*********************************** end of SeedNKey.cpp *******************************/
