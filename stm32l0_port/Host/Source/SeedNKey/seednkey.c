/************************************************************************************//**
* \file         seednkey.c
* \brief        XCP Seed and Key shared library source file.
* \ingroup      SeedNKey
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

/****************************************************************************************
* Include files
****************************************************************************************/
#include <assert.h>                         /* for assertions                          */
#include <stddef.h>                         /* for NULL declaration                    */
#include "seednkey.h"                       /* XCP seed and key                        */


/************************************************************************************//**
** \brief     Computes the key for the requested resource.
** \param     resource resource for which the unlock key is requested
** \param     seedLen  length of the seed
** \param     seedPtr  pointer to the seed data
** \param     keyLenPtr pointer where to store the key length
** \param     keyPtr pointer where to store the key data
** \return    XCP_RESULT_OK on success, otherwise XCP_RESULT_ERROR.
**
****************************************************************************************/
LIBOPENBLT_EXPORT uint32_t XCP_ComputeKeyFromSeed(uint8_t resource, uint8_t seedLen, 
                                                  uint8_t const * seedPtr, 
                                                  uint8_t * keyLenPtr,
                                                  uint8_t * keyPtr)
{
  uint32_t result = XCP_RESULT_ERROR;
  uint8_t idx;
  
  /* Check parameters. */
  assert(seedLen > 0);
  assert(seedPtr != NULL);
  assert(keyLenPtr != NULL);
  assert(keyPtr != NULL);
  
  /* Only continue if the parameters are valid. */
  if ( (seedLen > 0) && (seedPtr != NULL) && (keyLenPtr != NULL) && 
       (keyPtr != NULL) ) /*lint !e774 */
  {
    /* This example implementation of the key algorithm for PGM simply decrements the value
     * of each seed byte by 1. This coincides with the default implementation of the hook-
     * function XcpVerifyKeyHook() in the OpenBLT demo bootloaders.
     */
    if (resource == XCP_RESOURCE_PGM)
    {
      /* Compute the key. */
      for (idx = 0; idx < seedLen; idx++)
      {
        keyPtr[idx] = seedPtr[idx] - 1;
      }
      /* Set the key length. */
      *keyLenPtr = seedLen;
      /* Update the result value. */
      result = XCP_RESULT_OK;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XCP_ComputeKeyFromSeed ***/

                                                  
/************************************************************************************//**
** \brief     Obtains a bitmask of the resources for which an key algorithm is available.
** \param     resourcePtr pointer where to store the supported resources for the key 
**                        computation.
** \return    XCP_RESULT_OK on success, otherwise XCP_RESULT_ERROR.
**
****************************************************************************************/
LIBOPENBLT_EXPORT uint32_t XCP_GetAvailablePrivileges(uint8_t * resourcePtr)
{
  uint32_t result = XCP_RESULT_ERROR;
 
  /* Check parameters. */
  assert(resourcePtr != NULL);
  
  /* Only continue if the parameter is valid. */
  if (resourcePtr != NULL) /*lint !e774 */
  {
    /* This example implementation supports a key computation algorithm for the PGM 
     * resource.
     */  
    *resourcePtr = XCP_RESOURCE_PGM;
    /* Update the result value. */
    result = XCP_RESULT_OK;
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of XCP_GetAvailablePrivileges ***/


/*********************************** end of seednkey.c *********************************/

 
