/************************************************************************************//**
* \file         tbx_checksum.c
* \brief        Checksum module source file.
* \details      The CRC algorithms are based on the examples presents in a how-to blog
*               article from the Barr Group. It can be found at:
*               barrgroup.com/Embedded-Systems/How-To/CRC-Calculation-C-Code.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2019 by Feaser     www.feaser.com     All rights reserved
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


/****************************************************************************************
* Configuration macros
****************************************************************************************/
#ifndef TBX_CONF_CHECKSUM_CRC16_POLYNOM
/** \brief Polynomial of the 16-bit CRC. Note that it is possible to override this value
 *         by adding this macro definition to the configuration header file.
 */
#define TBX_CONF_CHECKSUM_CRC16_POLYNOM          (0x1021U)
#endif

#ifndef TBX_CONF_CHECKSUM_CRC16_INITIAL
/** \brief Initial value of the 16-bit CRC calculation. Note that it is possible to
 *         override this value by adding this macro definition to the configuration
 *         header file.
 */
#define TBX_CONF_CHECKSUM_CRC16_INITIAL          (0xFFFFU)
#endif

#ifndef TBX_CONF_CHECKSUM_CRC32_POLYNOM
/** \brief Polynomial of the 32-bit CRC. Note that it is possible to override this value
 *         by adding this macro definition to the configuration header file.
 */
#define TBX_CONF_CHECKSUM_CRC32_POLYNOM          (0x04C11DB7UL)
#endif

#ifndef TBX_CONF_CHECKSUM_CRC32_INITIAL
/** \brief Initial value of the 32-bit CRC calculation. Note that it is possible to
 *         override this value by adding this macro definition to the configuration
 *         header file.
 */
#define TBX_CONF_CHECKSUM_CRC32_INITIAL          (0xFFFFFFFFUL)
#endif


/************************************************************************************//**
** \brief     Calculates a 16-bit CRC value over the specified data.
** \details   It uses the CRC16_CCITT_FALSE algorithm:
**            - Polynomal:        0x1021
**            - Initial value:    0xFFFF
**            - Final Xor value:  0x0000
**            - Input reflected:  no
**            - Result reflected: no
** \param     data Array with bytes over which the CRC16 should be calculated.
** \param     len Number of bytes in the data array.
** \return    The 16-bit CRC value.
**
****************************************************************************************/
uint16_t TbxChecksumCrc16Calculate(uint8_t const * data, 
                                   size_t          len)
{
  uint16_t result = 0;

  /* Verify parameters. */
  TBX_ASSERT((data != NULL) && (len > 0U));

  /* Only continue if the parameters are valid. */
  if ( (data != NULL) && (len > 0U) )
  {
    /* Set the initial value. */
    result = TBX_CONF_CHECKSUM_CRC16_INITIAL;

    /* Loop through all data bytes to perform modulo 2 division per byte. */
    for (size_t byteIdx = 0U; byteIdx < len; byteIdx++)
    {
      /* Introduce the next byte into the remainder. */
      result = result ^ ((uint16_t)data[byteIdx] << 8U);
      /* Loop through the bits to perform modulo 2 division per bit. */
      for (uint8_t bitIdx = 0U; bitIdx <= 7U; bitIdx++)
      {
        /* Attempt to divide the current bit. */
        if ((result & 0x8000U) != 0U)
        {
          result = ((uint16_t)(result << 1U)) ^ TBX_CONF_CHECKSUM_CRC16_POLYNOM;
        }
        else
        {
          result = (uint16_t)(result << 1U);
        }
      }
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxChecksumCrc16Calculate ***/


/************************************************************************************//**
** \brief     Calculates a 32-bit CRC value over the specified data.
** \details   It uses the CRC32_MPEG2 algorithm:
**            - Polynomal:        0x04C11DB7
**            - Initial value:    0xFFFFFFFF
**            - Final Xor value:  0x00000000
**            - Input reflected:  no
**            - Result reflected: no
** \param     data Array with bytes over which the CRC32 should be calculated.
** \param     len Number of bytes in the data array.
** \return    The 32-bit CRC value.
**
****************************************************************************************/
uint32_t TbxChecksumCrc32Calculate(uint8_t const * data, 
                                   size_t          len)
{
  uint32_t result = 0;

  /* Verify parameters. */
  TBX_ASSERT((data != NULL) && (len > 0U));

  /* Only continue if the parameters are valid. */
  if ( (data != NULL) && (len > 0U) )
  {
    /* Set the initial value. */
    result = TBX_CONF_CHECKSUM_CRC32_INITIAL;

    /* Loop through all data bytes to perform modulo 2 division per byte. */
    for (size_t byteIdx = 0U; byteIdx < len; byteIdx++)
    {
      /* Introduce the next byte into the remainder. */
      result = result ^ ((uint32_t)data[byteIdx] << 24U);
      /* Loop through the bits to perform modulo 2 division per bit. */
      for (uint8_t bitIdx = 0U; bitIdx <= 7U; bitIdx++)
      {
        /* Attempt to divide the current bit. */
        if ((result & 0x80000000UL) != 0U)
        {
          result = ((uint32_t)(result << 1U)) ^ TBX_CONF_CHECKSUM_CRC32_POLYNOM;
        }
        else
        {
          result = (uint32_t)(result << 1U);
        }
      }
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxChecksumCrc32Calculate ***/


/*********************************** end of tbx_checksum.c *****************************/
