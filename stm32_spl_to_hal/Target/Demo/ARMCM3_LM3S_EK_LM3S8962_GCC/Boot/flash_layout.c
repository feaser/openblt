/************************************************************************************//**
* \file         Demo/ARMCM3_LM3S_EK_LM3S8962_GCC/Boot/flash_layout.c
* \brief        Custom flash layout table source file.
* \ingroup      Boot_ARMCM3_LM3S_EK_LM3S8962_GCC
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
  /* { 0x00000000, 0x02000,  0},           flash sector  0 - reserved for bootloader   */
  /* { 0x00002000, 0x02000,  1},           flash sector  1 - reserved for bootloader   */
  { 0x00004000, 0x02000,  2},           /* flash sector  2 - 8kb                       */
  { 0x00006000, 0x02000,  3},           /* flash sector  3 - 8kb                       */
  { 0x00008000, 0x02000,  4},           /* flash sector  4 - 8kb                       */
  { 0x0000A000, 0x02000,  5},           /* flash sector  5 - 8kb                       */
  { 0x0000C000, 0x02000,  6},           /* flash sector  6 - 8kb                       */
  { 0x0000E000, 0x02000,  7},           /* flash sector  7 - 8kb                       */
  { 0x00010000, 0x02000,  8},           /* flash sector  8 - 8kb                       */
  { 0x00012000, 0x02000,  9},           /* flash sector  9 - 8kb                       */
  { 0x00014000, 0x02000, 10},           /* flash sector 10 - 8kb                       */
  { 0x00016000, 0x02000, 11},           /* flash sector 11 - 8kb                       */
  { 0x00018000, 0x02000, 12},           /* flash sector 12 - 8kb                       */
  { 0x0001A000, 0x02000, 13},           /* flash sector 13 - 8kb                       */
  { 0x0001C000, 0x02000, 14},           /* flash sector 14 - 8kb                       */
  { 0x0001E000, 0x02000, 15},           /* flash sector 15 - 8kb                       */
  { 0x00020000, 0x08000, 16},           /* flash sector 16 - 32kb                      */
  { 0x00028000, 0x08000, 17},           /* flash sector 17 - 32kb                      */
  { 0x00030000, 0x08000, 18},           /* flash sector 18 - 32kb                      */
  { 0x00038000, 0x08000, 19},           /* flash sector 19 - 32kb                      */
};

/*********************************** end of flash_layout.c *****************************/
