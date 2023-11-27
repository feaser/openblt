/************************************************************************************//**
* \file         tbx_assert.h
* \brief        Run-time assertions header file.
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
#ifndef TBX_ASSERT_H
#define TBX_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************************
* Configuration macros
****************************************************************************************/
#ifndef TBX_CONF_ASSERTIONS_ENABLE
/** \brief Enable/disable run-time assertions. Note that it is possible to override this
 *         value by adding this macro definition to the configuration header file.
 */
#define TBX_CONF_ASSERTIONS_ENABLE               (0U)
#endif


/****************************************************************************************
* Macro definitions
****************************************************************************************/
#if (TBX_CONF_ASSERTIONS_ENABLE > 0U)
/** \brief Macro for run-time assertions. */
#define TBX_ASSERT(cond) { if(!(cond)) { TbxAssertTrigger(__FILE__, __LINE__); } }
#else
/** \brief Dummy macro for when assertions are disabled. */
#define TBX_ASSERT(cond) { ; }
#endif /* (TBX_CONF_ASSERTIONS_ENABLE > 0U) */


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Function type for a run-time assertion handler function. */
typedef void (* tTbxAssertHandler)(char     const * const file, 
                                   uint32_t               line);


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void TbxAssertSetHandler(tTbxAssertHandler assertHandler);

#if (TBX_CONF_ASSERTIONS_ENABLE > 0U)
void TbxAssertTrigger   (char     const * const file, 
                         uint32_t line);
#endif /* (TBX_CONF_ASSERTIONS_ENABLE > 0U) */


#ifdef __cplusplus
}
#endif

#endif /* TBX_ASSERT_H */
/*********************************** end of tbx_assert.h *******************************/
