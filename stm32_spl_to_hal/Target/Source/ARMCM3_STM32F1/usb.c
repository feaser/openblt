/************************************************************************************//**
* \file         Source/ARMCM3_STM32F1/usb.c
* \brief        Bootloader USB communication interface source file.
* \ingroup      Target_ARMCM3_STM32F1
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

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */
#if (BOOT_COM_USB_ENABLE > 0)
#include "usb.h"                                 /* USB driver module                  */


/****************************************************************************************
* Configuration macros
****************************************************************************************/
/* For a USB bootloader, the backdoor needs to stay open long enough for the USB device
 * to enumerate on the host PC. Therefore the default backdoor open time needs to be
 * extended. Note that this won't be long enough for a first time USB driver install
 * on the host PC. In this case the bootloader should be started with the backup backdoor
 * that uses, for example, a digital input to force the bootloader to stay active. This
 * can be implemented in CpuUserProgramStartHook(). Feel free to shorten/lengthen this
 * time for finetuning. Note that adding this configuration macro to blt_conf.h overrides
 * the value here.
 */
#ifndef BOOT_COM_USB_BACKDOOR_EXTENSION_MS
#define BOOT_COM_USB_BACKDOOR_EXTENSION_MS   (2000)
#endif


/************************************************************************************//**
** \brief     Initializes the USB communication interface.
** \return    none.
**
****************************************************************************************/
void UsbInit(void)
{
  /* TODO ##Vg Implement UsbInit(). */

  /* extend the time that the backdoor is open in case the default timed backdoor
   * mechanism is used.
   */
#if (BOOT_BACKDOOR_HOOKS_ENABLE == 0)
  if (BackDoorGetExtension() < BOOT_COM_USB_BACKDOOR_EXTENSION_MS)
  {
    BackDoorSetExtension(BOOT_COM_USB_BACKDOOR_EXTENSION_MS);
  }
#endif /* BOOT_BACKDOOR_HOOKS_ENABLE == 0 */
} /*** end of UsbInit ***/


/************************************************************************************//**
** \brief     Releases the USB communication interface.
** \return    none.
**
****************************************************************************************/
void UsbFree(void)
{
  /* TODO ##Vg Implement UsbFree(). */
} /*** end of UsbFree ***/


/************************************************************************************//**
** \brief     Transmits a packet formatted for the communication interface.
** \param     data Pointer to byte array with data that it to be transmitted.
** \param     len  Number of bytes that are to be transmitted.
** \return    none.
**
****************************************************************************************/
void UsbTransmitPacket(blt_int8u *data, blt_int8u len)
{
  /* TODO ##Vg Implement UsbTransmitPacket(). */
} /*** end of UsbTransmitPacket ***/


/************************************************************************************//**
** \brief     Receives a communication interface packet if one is present.
** \param     data Pointer to byte array where the data is to be stored.
** \param     len Pointer where the length of the packet is to be stored.
** \return    BLT_TRUE if a packet was received, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool UsbReceivePacket(blt_int8u *data, blt_int8u *len)
{
  /* TODO ##Vg Implement UsbReceivePacket(). */

  return BLT_FALSE;
} /*** end of UsbReceivePacket ***/
#endif /* BOOT_COM_USB_ENABLE > 0 */


/*********************************** end of usb.c **************************************/
