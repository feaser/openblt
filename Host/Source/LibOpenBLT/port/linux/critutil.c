/************************************************************************************//**
* \file         port/linux/critutil.c
* \brief        Critical section utility source file.
* \ingroup      Utility
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
#include <stdint.h>                         /* for standard integer types              */
#include <stdbool.h>                        /* for boolean type                        */
#include <pthread.h>                        /* for posix threads                       */
#include "util.h"                           /* Utility module                          */


/****************************************************************************************
* Local data declarations
****************************************************************************************/


/************************************************************************************//**
** \brief     Enters a critical section. The functions UtilCriticalSectionEnter and 
**            UtilCriticalSectionExit should always be used in a pair.
**
****************************************************************************************/
void UtilCriticalSectionEnter(void)
{
} /*** end of UtilCriticalSectionEnter ***/


/************************************************************************************//**
** \brief     Leaves a critical section. The functions UtilCriticalSectionEnter and 
**            UtilCriticalSectionExit should always be used in a pair.
**
****************************************************************************************/
void UtilCriticalSectionExit(void)
{
} /*** end of UtilCriticalSectionExit ***/


/*********************************** end of critutil.c *********************************/

