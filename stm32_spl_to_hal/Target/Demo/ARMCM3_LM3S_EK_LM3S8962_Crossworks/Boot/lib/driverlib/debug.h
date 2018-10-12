//*****************************************************************************
//
// debug.h - Macros for assisting debug of the driver library.
//
// Copyright (c) 2006-2011 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 6852 of the Stellaris Peripheral Driver Library.
//
//*****************************************************************************

#ifndef __DEBUG_H__
#define __DEBUG_H__


#include "boot.h"

//*****************************************************************************
//
// Prototype for the function that is called when an invalid argument is passed
// to an API.  This is only used when doing a DEBUG build.
//
//*****************************************************************************
#ifndef NDEBUG
extern void AssertFailure(blt_char *file, blt_int32u line);
#endif

//*****************************************************************************
//
// The ASSERT macro, which does the actual assertion checking.  Typically, this
// will be for procedure arguments.
//
//*****************************************************************************
#ifdef NDEBUG
#define ASSERT(expr)
#else
#define ASSERT(expr) {                                          \
                         if(!(expr))                            \
                         {                                      \
                             AssertFailure(__FILE__, __LINE__); \
                         }                                      \
                     }
#endif

#endif // __DEBUG_H__
