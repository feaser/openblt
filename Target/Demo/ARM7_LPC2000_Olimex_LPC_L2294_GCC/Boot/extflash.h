/************************************************************************************//**
* \file         Demo\ARM7_LPC2000_Olimex_LPC_L2294_GCC\Boot\extflash.h
* \brief        Bootloader external flash driver header file.
* \ingroup      Boot_ARM7_LPC2000_Olimex_LPC_L2294_GCC
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
* You have received a copy of the GNU General Public License along with OpenBLT. It 
* should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
* 
* \endinternal
****************************************************************************************/
#ifndef EXTFLASH_H
#define EXTFLASH_H

#if (BOOT_NVM_HOOKS_ENABLE > 0)
/****************************************************************************************
* Function prototypes
****************************************************************************************/
void      ExtFlashInit(void);
blt_int8u ExtFlashWrite(blt_addr addr, blt_int32u len, blt_int8u *data);
blt_int8u ExtFlashErase(blt_addr addr, blt_int32u len);
blt_bool  ExtFlashDone(void);
#endif

#endif /* EXTFLASH_H */
/*********************************** end of extflash.h *********************************/
