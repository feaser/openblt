/************************************************************************************//**
* \file         Demo/ARMCM3_STM32L1_Nucleo_L152RE_CubeIDE/Boot/App/flash_layout.c
* \brief        Custom flash layout table source file.
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
  /* { 0x08000000, 0x00800 },              flash sector   0 - reserved for bootloader  */
  /* { 0x08000800, 0x00800 },              flash sector   1 - reserved for bootloader  */
  /* { 0x08001000, 0x00800 },              flash sector   2 - reserved for bootloader  */
  /* { 0x08001800, 0x00800 },              flash sector   3 - reserved for bootloader  */
  /* { 0x08002000, 0x00800 },              flash sector   4 - reserved for bootloader  */
  { 0x08002800, 0x00800 },              /* flash sector   5 - 2kb                      */
  { 0x08003000, 0x00800 },              /* flash sector   6 - 2kb                      */
  { 0x08003800, 0x00800 },              /* flash sector   7 - 2kb                      */
  { 0x08004000, 0x00800 },              /* flash sector   8 - 2kb                      */
  { 0x08004800, 0x00800 },              /* flash sector   9 - 2kb                      */
  { 0x08005000, 0x00800 },              /* flash sector  10 - 2kb                      */
  { 0x08005800, 0x00800 },              /* flash sector  11 - 2kb                      */
  { 0x08006000, 0x00800 },              /* flash sector  12 - 2kb                      */
  { 0x08006800, 0x00800 },              /* flash sector  13 - 2kb                      */
  { 0x08007000, 0x00800 },              /* flash sector  14 - 2kb                      */
  { 0x08007800, 0x00800 },              /* flash sector  15 - 2kb                      */
  { 0x08008000, 0x08000 },              /* flash sector  16-31  - 32kb                 */
  { 0x08010000, 0x08000 },              /* flash sector  32-47  - 32kb                 */
  { 0x08018000, 0x08000 },              /* flash sector  48-63  - 32kb                 */
  { 0x08020000, 0x08000 },              /* flash sector  64-79  - 32kb                 */
  { 0x08028000, 0x08000 },              /* flash sector  80-95  - 32kb                 */
  { 0x08030000, 0x08000 },              /* flash sector  96-111 - 32kb                 */
  { 0x08038000, 0x08000 },              /* flash sector 112-127 - 32kb                 */
  { 0x08040000, 0x08000 },              /* flash sector 128-143 - 32kb                 */
  { 0x08048000, 0x08000 },              /* flash sector 144-159 - 32kb                 */
  { 0x08050000, 0x08000 },              /* flash sector 160-175 - 32kb                 */
  { 0x08058000, 0x08000 },              /* flash sector 176-191 - 32kb                 */
  { 0x08060000, 0x08000 },              /* flash sector 192-207 - 32kb                 */
  { 0x08068000, 0x08000 },              /* flash sector 208-223 - 32kb                 */
  { 0x08070000, 0x08000 },              /* flash sector 224-239 - 32kb                 */
  { 0x08078000, 0x08000 }               /* flash sector 240-255 - 32kb                 */
};

/*********************************** end of flash_layout.c *****************************/
