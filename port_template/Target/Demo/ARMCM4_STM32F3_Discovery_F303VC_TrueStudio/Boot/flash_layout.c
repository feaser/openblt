/************************************************************************************//**
* \file         Demo/ARMCM4_STM32F3_Discovery_F303VC_TrueStudio/Boot/flash_layout.c
* \brief        Custom flash layout table source file.
* \ingroup      Boot_ARMCM4_STM32F3_Discovery_F303VC_TrueStudio
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2016  by Feaser    http://www.feaser.com    All rights reserved
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
  /* { 0x08000000, 0x00800 },           flash sector  0 - 2kb (reserved for bootloader)*/
  /* { 0x08000800, 0x00800 },           flash sector  1 - 2kb (reserved for bootloader)*/
  /* { 0x08001000, 0x00800 },           flash sector  2 - 2kb (reserved for bootloader)*/
  /* { 0x08001800, 0x00800 },           flash sector  3 - 2kb (reserved for bootloader)*/
  /*{ 0x08002000, 0x00800 },            flash sector  4 - 2kb (reserved for bootloader)*/
  /*{ 0x08002800, 0x00800 },            flash sector  5 - 2kb (reserved for bootloader)*/
  /*{ 0x08003000, 0x00800 },            flash sector  6 - 2kb (reserved for bootloader)*/
  /*{ 0x08003800, 0x00800 },            flash sector  7 - 2kb (reserved for bootloader)*/
  { 0x08004000, 0x00800 },              /* flash sector  8 - 2kb                       */
  { 0x08004800, 0x00800 },              /* flash sector  9 - 2kb                       */
  { 0x08005000, 0x00800 },              /* flash sector 10 - 2kb                       */
  { 0x08005800, 0x00800 },              /* flash sector 11 - 2kb                       */
  { 0x08006000, 0x00800 },              /* flash sector 12 - 2kb                       */
  { 0x08006800, 0x00800 },              /* flash sector 13 - 2kb                       */
  { 0x08007000, 0x00800 },              /* flash sector 14 - 2kb                       */
  { 0x08007800, 0x00800 },              /* flash sector 15 - 2kb                       */
  { 0x08008000, 0x08000 },              /* flash sectors 16 to 31 - 32kb               */
  { 0x08010000, 0x10000 },              /* flash sectors 32 to 63 - 64kb               */
  { 0x08020000, 0x20000 },              /* flash sectors 64 to 127 - 128kb             */
};


/*********************************** end of flash_layout.c *****************************/
