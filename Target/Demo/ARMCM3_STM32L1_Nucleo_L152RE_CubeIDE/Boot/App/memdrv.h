/************************************************************************************//**
* \file         Demo/ARMCM3_STM32L1_Nucleo_L152RE_CubeIDE/Boot/App/memdrv.h
* \brief        STM32L152RE EEPROM driver header file.
* \ingroup      Boot_ARMCM3_STM32L1_Nucleo_L152RE_CubeIDE
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2024  by Feaser    http://www.feaser.com    All rights reserved
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
#ifndef MEMDRV_H
#define MEMDRV_H

/****************************************************************************************
* Function prototypes
****************************************************************************************/
void       MemDrvInit(void);
blt_bool   MemDrvWrite(blt_addr address, blt_int32u len, blt_int8u const * data);
blt_bool   MemDrvErase(blt_addr address, blt_int32u len);
blt_int32u MemDrvGetSize(void);
blt_addr   MemDrvGetBaseAddress(void);
blt_bool   MemDrvDone(void);


#endif /* MEMDRV_H */
/*********************************** end of memdrv.h ***********************************/
