/************************************************************************************//**
* \file         tbx_platform.c
* \brief        Platform specifics module source file.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2023 by Feaser     www.feaser.com     All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
*
* SPDX-License-Identifier: MIT
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "microtbx.h"                            /* MicroTBX global header             */


/************************************************************************************//**
** \brief     Utility function to determine if the targets memory storage organization
**            is little endian (Intel) or big endian (Motorola).
** \return    TBX_TRUE for little endian, TBX_FALSE for big endian.
**
****************************************************************************************/
uint8_t TbxPlatformLittleEndian(void) 
{
  uint8_t                   result = TBX_FALSE;
  volatile uint32_t         endianTestVal = 0x01234567UL;
  volatile uint8_t  const * firstBytePtr;

  /* Initialize the pointer to the first byte of the test value. */
  firstBytePtr = (volatile uint8_t const *)&endianTestVal;

  /* Is the target little endian? In this case the LSB is stored first in memory. */
  if (firstBytePtr[0] == 0x67U)
  {
    result = TBX_TRUE;
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxPlatformLittleEndian ***/


/*********************************** end of tbx_platform.c *****************************/
