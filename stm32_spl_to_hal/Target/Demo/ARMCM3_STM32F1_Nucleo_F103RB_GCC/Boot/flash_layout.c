/************************************************************************************//**
* \file         Demo/ARMCM3_STM32F1_Nucleo_F103RB_GCC/Boot/flash_layout.c
* \brief        Custom flash layout table source file.
* \ingroup      Boot_ARMCM3_STM32F1_Nucleo_F103RB_GCC
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

/** \brief   Array wit the layout of the flash memory.
 *  \details Also controls what part of the flash memory is reserved for the bootloader.
 *           If the bootloader size changes, the reserved sectors for the bootloader
 *           might need adjustment to make sure the bootloader doesn't get overwritten.
 */
static const tFlashSector flashLayout[] =
{
  /* space is reserved for a bootloader configuration with all supported communication
   * interfaces enabled. when for example only UART is needed, than the space required
   * for the bootloader can be made a lot smaller here.
   */
  /* { 0x08000000, 0x02000 },           flash sector  0 - reserved for bootloader   */
  { 0x08002000, 0x02000 },           /* flash sector  1 - 8kb                       */
  { 0x08004000, 0x02000 },           /* flash sector  2 - 8kb                       */
  { 0x08006000, 0x02000 },           /* flash sector  3 - 8kb                       */
  { 0x08008000, 0x02000 },           /* flash sector  4 - 8kb                       */
  { 0x0800A000, 0x02000 },           /* flash sector  5 - 8kb                       */
  { 0x0800C000, 0x02000 },           /* flash sector  6 - 8kb                       */
  { 0x0800E000, 0x02000 },           /* flash sector  7 - 8kb                       */
  { 0x08010000, 0x02000 },           /* flash sector  8 - 8kb                       */
  { 0x08012000, 0x02000 },           /* flash sector  9 - 8kb                       */
  { 0x08014000, 0x02000 },           /* flash sector 10 - 8kb                       */
  { 0x08016000, 0x02000 },           /* flash sector 11 - 8kb                       */
  { 0x08018000, 0x02000 },           /* flash sector 12 - 8kb                       */
  { 0x0801A000, 0x02000 },           /* flash sector 13 - 8kb                       */
  { 0x0801C000, 0x02000 },           /* flash sector 14 - 8kb                       */
  { 0x0801E000, 0x02000 },           /* flash sector 15 - 8kb                       */
};


/*********************************** end of flash_layout.c *****************************/
