/************************************************************************************//**
* \file         Source/TRICORE_TC2/Tasking/ram_func.h
* \brief        RAM function macros header file.
* \ingroup      Target_TRICORE_TC2
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2023  by Feaser    http://www.feaser.com    All rights reserved
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
#ifndef RAM_FUNC_H
#define RAM_FUNC_H

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Macro used at the start of a function implementation to inform the linker that
 *         the function should be copied from flash to RAM by the startup code and also
 *         run from RAM.
 */
#define BLT_RAM_FUNC_BEGIN _Pragma("section code cpu0_psram") 

/** \brief Macro used at the end of a function implementation to inform the linker that
 *         the RAM function is now ended.
 */
#define BLT_RAM_FUNC_END   _Pragma("section code restore") 


#endif /* RAM_FUNC_H */
/*********************************** end of ram_func.h *********************************/
