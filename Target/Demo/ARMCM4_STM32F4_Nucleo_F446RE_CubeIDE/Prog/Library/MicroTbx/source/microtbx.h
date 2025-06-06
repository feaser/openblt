/************************************************************************************//**
* \file         microtbx.h
* \brief        MicroTBX global header file.
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
#ifndef MICROTBX_H
#define MICROTBX_H

/****************************************************************************************
* Include files
****************************************************************************************/
/* Note that it is possible to override the standard tbx_conf.h configuration header
 * file with a project specific one that is defined in the IDE/makefile. For example,
 * the following define could be configured: PROJ_TBX_CONF_H="my_tbx_config.h". This can
 * be handy if you use MicroTBX in several projects with a different configuration,
 * and enables you to have just one MicroTBX source base.
 */
#include <stdint.h>                         /* Standard integer types                  */
#include <stddef.h>                         /* Standard definitions                    */
#ifdef PROJ_TBX_CONF_H
#include PROJ_TBX_CONF_H                    /* Custom MicroTBX configuration file      */
#else
#include "tbx_conf.h"                       /* Standard MicroTBX configuration file    */
#endif /* PROJ_TBX_CONF_H */
#include "tbx_platform.h"                   /* Platform specifics                      */
#include "tbx_port.h"                       /* MicroTBX port specifics                 */
#include "tbx_assert.h"                     /* Run-time assertions                     */
#include "tbx_critsect.h"                   /* Critical sections                       */
#include "tbx_heap.h"                       /* Heap memory allocation                  */
#include "tbx_list.h"                       /* Linked lists                            */
#include "tbx_mempool.h"                    /* Pool based heap memory manager          */
#include "tbx_random.h"                     /* Random number generator                 */
#include "tbx_checksum.h"                   /* Checksum module                         */
#include "tbx_crypto.h"                     /* Cryptography module                     */


#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************************
* Version definitions
****************************************************************************************/
/** \brief Main version number of MicroTBX. */
#define TBX_VERSION_MAIN                         (1U)

/** \brief Minor version number of MicroTBX. */
#define TBX_VERSION_MINOR                        (4U)

/** \brief Patch number of MicroTBX. */
#define TBX_VERSION_PATCH                        (0U)


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Boolean true value. */
#define TBX_TRUE                                 (1U)

/** \brief Boolean false value. */
#define TBX_FALSE                                (0U)

/** \brief Generic okay value. */
#define TBX_OK                                   (1U)

/** \brief Generic error value. */
#define TBX_ERROR                                (0U)

/** \brief Generic on value. */
#define TBX_ON                                   (1U)

/** \brief Generic off value. */
#define TBX_OFF                                  (0U)

/** \brief Macro to flag a function parameter as unused, which allows the associated
 *         lint message and/or compiler warning to be suppressed.
 */
#define TBX_UNUSED_ARG(x)                        (void)(x)


#ifdef __cplusplus
}
#endif

#endif /* MICROTBX_H */
/*********************************** end of microtbx.h *********************************/
