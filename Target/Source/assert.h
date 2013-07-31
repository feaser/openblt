/************************************************************************************//**
* \file         Source\assert.h
* \brief        Bootloader assertion module header file.
* \ingroup      Core
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
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
* You should have received a copy of the GNU General Public License along with OpenBLT.
* If not, see <http://www.gnu.org/licenses/>.
*
* A special exception to the GPL is included to allow you to distribute a combined work 
* that includes OpenBLT without being obliged to provide the source code for any 
* proprietary components. The exception text is included at the bottom of the license
* file <license.html>.
* 
* \endinternal
****************************************************************************************/
#ifndef ASSERT_H
#define ASSERT_H

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* declare assertion macro's. ASSERT_CT is for compile time assertions and ASSERT_RT is
 * for runtime assertions.
 */
#ifdef NDEBUG
  #define ASSERT_CT(cond) ((void)0)
  #define ASSERT_RT(cond) ((void)0)
#else 
  #define ASSERT_CONCAT_(a, b) a##b
  #define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
  /** \brief      Macro for assertions that can be performed at compile time. */
  #define ASSERT_CT(cond) enum { ASSERT_CONCAT(assert_error_on_line_, __LINE__) = 1/(!!(cond)) }  
  /** \brief Macro for assertions that can only be performed at run time. */
  #define ASSERT_RT(cond) \
    if (cond) \
      { ; } \
    else \
      AssertFailure(__FILE__, __LINE__)
#endif /* NDEBUG */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
#ifndef NDEBUG
void AssertFailure(blt_char *file, blt_int32u line);
#endif

#endif /* ASSERT_H */
/*********************************** end of assert.h ***********************************/
