/************************************************************************************//**
* \file         Demo/ARMCM4_STM32F4_Nucleo_F446RE_CubeIDE/Boot/App/flash_layout.c
* \brief        Custom flash layout table source file.
* \ingroup      Boot_ARMCM4_STM32F4_Nucleo_F446RE_CubeIDE
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

/** \brief   Array wit the layout of the flash memory.
 *  \details Also controls what part of the flash memory is reserved for the bootloader.
 *           If the bootloader size changes, the reserved sectors for the bootloader
 *           might need adjustment to make sure the bootloader doesn't get overwritten.
 */
static const tFlashSector flashLayout[] =
{
  /* space is reserved for a bootloader configuration with all supported communication
   * interfaces enabled. when for example only UART is needed, then the space required
   * for the bootloader can be made a lot smaller here.
   */
  /* { 0x08000000, 0x04000,  0},           flash sector  0 - reserved for bootloader   */
  { 0x08004000, 0x04000,  1},           /* flash sector  1 -  16kb                     */
  { 0x08008000, 0x04000,  2},           /* flash sector  2 -  16kb                     */
  { 0x0800c000, 0x04000,  3},           /* flash sector  3 -  16kb                     */
  { 0x08010000, 0x10000,  4},           /* flash sector  4 -  64kb                     */
  { 0x08020000, 0x20000,  5},           /* flash sector  5 - 128kb                     */
  { 0x08040000, 0x20000,  6},           /* flash sector  6 - 128kb                     */
  { 0x08060000, 0x20000,  7},           /* flash sector  7 - 128kb                     */
};


/*********************************** end of flash_layout.c *****************************/
