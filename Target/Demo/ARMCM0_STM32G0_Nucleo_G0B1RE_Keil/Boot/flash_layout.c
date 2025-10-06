/************************************************************************************//**
* \file         Demo/ARMCM0_STM32G0_Nucleo_G0B1RE_Keil/Boot/flash_layout.c
* \brief        Custom flash layout table source file.
* \ingroup      Boot_ARMCM0_STM32G0_Nucleo_G0B1RE_Keil
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2025  by Feaser    http://www.feaser.com    All rights reserved
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
  /* space is reserved for the bootloader configuration of the demo programs. it might
   * grow/shrink depending when the bootloader is reconfigured and or customized. make
   * sure to update the reserved space for the bootloader here in that case, as well as
   * the start address of the user program in its linker command script.
   */
  /* { 0x08000000, 0x00800,  0},           flash sector  0 - reserved for bootloader   */
  /* { 0x08000800, 0x00800,  1},           flash sector  1 - reserved for bootloader   */
  /* { 0x08001000, 0x00800,  2},           flash sector  2 - reserved for bootloader   */
  /* { 0x08001800, 0x00800,  3},           flash sector  3 - reserved for bootloader   */
  /* { 0x08002000, 0x00800,  4},           flash sector  4 - reserved for bootloader   */
  /* { 0x08002800, 0x00800,  5},           flash sector  5 - reserved for bootloader   */
  { 0x08003000, 0x00800,  6},           /* flash sector  6 - 2kb                       */
  { 0x08003800, 0x00800,  7},           /* flash sector  7 - 2kb                       */
  { 0x08004000, 0x00800,  8},           /* flash sector  8 - 2kb                       */
  { 0x08004800, 0x00800,  9},           /* flash sector  9 - 2kb                       */
  { 0x08005000, 0x00800, 10},           /* flash sector 10 - 2kb                       */
  { 0x08005800, 0x00800, 11},           /* flash sector 11 - 2kb                       */
  { 0x08006000, 0x00800, 12},           /* flash sector 12 - 2kb                       */
  { 0x08006800, 0x00800, 13},           /* flash sector 13 - 2kb                       */
  { 0x08007000, 0x00800, 14},           /* flash sector 14 - 2kb                       */
  { 0x08007800, 0x00800, 15},           /* flash sector 15 - 2kb                       */
  { 0x08008000, 0x08000, 16},           /* flash sector 16 - 32kb                       */
  { 0x08010000, 0x08000, 17},           /* flash sector 17 - 32kb                       */
  { 0x08018000, 0x08000, 18},           /* flash sector 18 - 32kb                       */
  { 0x08020000, 0x08000, 19},           /* flash sector 17 - 32kb                       */
  { 0x08028000, 0x08000, 20},           /* flash sector 18 - 32kb                       */
  { 0x08030000, 0x08000, 21},           /* flash sector 17 - 32kb                       */
  { 0x08038000, 0x08000, 22},           /* flash sector 18 - 32kb                       */
  { 0x08040000, 0x08000, 23},           /* flash sector 17 - 32kb                       */
  { 0x08048000, 0x08000, 24},           /* flash sector 18 - 32kb                       */
  { 0x08050000, 0x08000, 25},           /* flash sector 17 - 32kb                       */
  { 0x08058000, 0x08000, 26},           /* flash sector 18 - 32kb                       */
  { 0x08060000, 0x08000, 27},           /* flash sector 17 - 32kb                       */
  { 0x08068000, 0x08000, 28},           /* flash sector 18 - 32kb                       */
  { 0x08070000, 0x08000, 29},           /* flash sector 17 - 32kb                       */
  { 0x08078000, 0x08000, 30},           /* flash sector 18 - 32kb                       */
};


/*********************************** end of flash_layout.c *****************************/
