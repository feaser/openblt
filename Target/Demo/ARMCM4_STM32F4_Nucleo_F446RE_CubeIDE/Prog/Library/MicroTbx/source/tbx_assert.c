/************************************************************************************//**
* \file         tbx_assert.c
* \brief        Run-time assertions source file.
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
* Local data declarations
****************************************************************************************/
#if (TBX_CONF_ASSERTIONS_ENABLE > 0U)
/** \brief Pointer to the application provided assertion handler function that should be
 *         used, whenever a run-time assertion is triggered.
 */
static tTbxAssertHandler tbxAssertHandlerPtr = NULL;
#endif /* (TBX_ASSERTIONS_ENABLE > 0U) */


/************************************************************************************//**
** \brief     Sets the application specific assertion handler.
** \param     assertHandler Pointer to the application specific assertion handler to use
**            instead of the default internal handler.
**
****************************************************************************************/
void TbxAssertSetHandler(tTbxAssertHandler assertHandler)
{
#if (TBX_CONF_ASSERTIONS_ENABLE > 0U)
  /* Verify parameter. */
  TBX_ASSERT(assertHandler != NULL);

  /* Only continue if the parameters are valid. */
  if (assertHandler != NULL)
  {
    /* Store the pointer to the application specific assertion handler. */
    tbxAssertHandlerPtr = assertHandler;
  }
#else
  TBX_UNUSED_ARG(assertHandler);
#endif /* (TBX_CONF_ASSERTIONS_ENABLE > 0U) */
} /*** end of TbxAssertSetHandler ***/


#if (TBX_CONF_ASSERTIONS_ENABLE > 0U)
/* cppcheck-suppress [misra-c2012-8.7,unmatchedSuppression] 
 * MISRA exception: External linkage for API functions.
 */
/************************************************************************************//**
** \brief     Triggers the run-time assertion. The default implementation is to enter an
**            infinite loop, which halts the program and can be used for debugging 
**            purposes. Inspecting the values of the file and line parameters gives a 
**            clear indication where the run-time assertion occurred. Note that an
**            alternative application specific assertion handler can be configured with
**            function TbxAssertSetHandler().
** \param     file The filename of the source file where the assertion occurred in.
** \param     line The line number inside the file where the assertion occurred.
**
****************************************************************************************/
void TbxAssertTrigger(char     const * const file, 
                      uint32_t               line)
{
  /* Check if there is an application specific assertion handler configured. */
  if (tbxAssertHandlerPtr != NULL)
  {
    /* Call the application specific assertion handler. */
    tbxAssertHandlerPtr(file, line);
  }
  /* Use the default internal assertion handler which simple enters an infinite loop. */
  else
  {
    for (;;)
    {
      ;
    }
  }
} /*** end of TbxAssertTrigger ***/
#endif /* (TBX_ASSERTIONS_ENABLE > 0U) */


/*********************************** end of tbx_assert.c *******************************/
