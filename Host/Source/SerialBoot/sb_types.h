/************************************************************************************//**
* \file         sb_types.h
* \brief        Serial Boot type definitions header file.
* \ingroup      SerialBoot
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
#ifndef SB_TYPES_H
#define SB_TYPES_H

/****************************************************************************************
* Include files
****************************************************************************************/
#include <stdio.h>                                    /* standard I/O library          */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Generic boolean true value. */
#define SB_TRUE       (1u)

/** \brief Ceneric boolean false value. */
#define SB_FALSE      (0u)

/** \brief NULL pointer value. */
#define SB_NULL       ((void *)0)


/****************************************************************************************
* Type definitions
****************************************************************************************/
typedef signed char       sb_char;
typedef signed char       sb_int8;
typedef signed short      sb_int16;
typedef signed int        sb_int32;
typedef unsigned char     sb_uint8;
typedef unsigned short    sb_uint16;
typedef unsigned int      sb_uint32;
typedef FILE *            sb_file;



#endif /* SB_TYPES_H */
/*********************************** end of sb_types.h *********************************/
