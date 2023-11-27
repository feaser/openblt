/************************************************************************************//**
* \file         tbx_heap.c
* \brief        Heap memory allocation source file.
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
#ifndef TBX_CONF_HEAP_SIZE
/** \brief Configure the size of the heap in bytes. Note that it is possible to override
 *         this value by adding this macro definition to the configuration header file.
 */
#define TBX_CONF_HEAP_SIZE                       (1024U)
#endif


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Keeps track of how many bytes have already been allocated on the heap. */
static size_t tbxHeapAllocated = 0U;


/************************************************************************************//**
** \brief     Allocates the desired number of bytes on the heap. It can be used instead
**            of the compiler specific malloc() function. Note that free-ing of allocated
**            memory is not supported to prevent memory fragmentation.
** \param     size The number of bytes to allocate on the heap.
** \return    Pointer to the start of the newly allocated heap memory if successful,
**            NULL otherwise.
**
****************************************************************************************/
void * TbxHeapAllocate(size_t size)
{
  /* cppcheck-suppress [unassignedVariable,unmatchedSuppression] 
   * The actual heap buffer. Whenever memory needs to be dynamically allocated, it will
   * be taken from this buffer. As such, it is okay to not be initialized and therefore
   * the warning about no value being assigned to this variable can be ignored.
   */
  static uint8_t   tbxHeapBuffer[TBX_CONF_HEAP_SIZE];
  void           * result = NULL;

  /* Verify parameter. */
  TBX_ASSERT(size > 0U);

  /* Only continue if the parameters are valid. */
  if (size > 0U)
  {
    /* Align the desired size to the address size to make it work on all targets. */
    size_t sizeWanted = (size + (sizeof(void *) - 1U)) & ~(sizeof(void *) - 1U);
    /* Obtain mutual exclusive access to tbxHeapAllocated. */
    TbxCriticalSectionEnter();
      /* Determine the number of still available bytes in the heap buffer. */
    size_t sizeAvailable = TBX_CONF_HEAP_SIZE - tbxHeapAllocated;
    /* Is there enough space left on the heap for this allocation request? */
    if (sizeAvailable >= sizeWanted)
    {
      /* Set the address for the newly allocated memory. */
      result = &tbxHeapBuffer[tbxHeapAllocated];
      /* Perform the actual allocation by incrementing the counter. */
      tbxHeapAllocated += sizeWanted;
    }
    /* Release mutual exclusive access to tbxHeapAllocated. */
    TbxCriticalSectionExit();
  }

  /* Return the address of the allocated memory to the caller. */
  return result;
} /*** end of TbxHeapAllocate ***/


/************************************************************************************//**
** \brief     Obtains the current amount of bytes that are still available on the heap.
** \return    Number of free bytes on the heap.
**
****************************************************************************************/
size_t TbxHeapGetFree(void)
{
  size_t result;

  /* Obtain mutual exclusive access to tbxHeapAllocated. */
  TbxCriticalSectionEnter();
  /* Determine the number of still available bytes in the heap buffer. */
  result = TBX_CONF_HEAP_SIZE - tbxHeapAllocated;
  /* Release mutual exclusive access to tbxHeapAllocated. */
  TbxCriticalSectionExit();

  /* Give the result back to the caller. */
  return result;
} /*** end of TbxHeapGetFree ***/


/*********************************** end of tbx_heap.c *********************************/
