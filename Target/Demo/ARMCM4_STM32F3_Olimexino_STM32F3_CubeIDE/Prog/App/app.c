/************************************************************************************//**
* \file         Demo/ARMCM4_STM32F3_Olimexino_STM32F3_CubeIDE/Prog/App/app.c
* \brief        User program application source file.
* \ingroup      Prog_ARMCM4_STM32F3_Olimexino_STM32F3_CubeIDE
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

/****************************************************************************************
* Include files
****************************************************************************************/
#include "header.h"                                    /* generic header               */


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Layout of the firmware info table type. Note that this is what you need to
 *         typecast the pointers to in the bootloader's InfoTableCheckHook() function.
 *         Feel free to add more or less information to this info table as you see fit.
 */
typedef struct t_firmware_info_table
{
  unsigned long tableId;         /**< fixed value for identification as an info table. */
  unsigned long productId;       /**< product identification. E.g. 1234 = Airpump.     */
  unsigned long firmwareVersion; /**< firmware version. E.g. 10429 = v1.4.29           */
} tFirmwareInfoTable;


/****************************************************************************************
* Global constant declarations
****************************************************************************************/
/** \brief     The firmware info table.
 *  \details   Note that this constant table is linked to the section
 *             ".infoTable". This section is defined in the linker script and configured
 *             such that this info table appears at the same location as configured with
 *             the BOOT_INFO_TABLE_ADDR configuration macro and has the same length as
 *             configured with the BOOT_INFO_TABLE_LEN configuration macro.
 *
 *             Instead of adding this table directly in the code of the firmware, an
 *             alternative approach would be to patch it into the firmware file during a
 *             postbuild operation. A free application such as "srec_cat" can be
 *             used. It's also possible to build your own patch tool based on LibOpenBLT,
 *             since this library includes firmware file reading and editing
 *             functionality.
 *  \attention When using the bootloader's default internal signature checksum
 *             verification mechanism, this info table should not be located in the area
 *             that the bootloader's flash driver calls the "boot block", unless the
 *             info table itself is located before the actual location configured for the
 *             signature checksum value. The "boot block" is defined as the first
 *             FLASH_WRITE_BLOCK_SIZE bytes of the user program's flash.
 *
 *             If the info table were to be located in the "boot block" and after the
 *             actual signature checksum location, a miniscule chance is present that a
 *             power loss during the write of this "boot block" results in a valid
 *             signature checksum value, yet only partially written (and therefore
 *             invalid) info table.
 *
 *             Note that this restriction does not apply when using the Improved Checksum
 *             add-on module, instead of the default internal signature checksum. More
 *             info regarding the Improved Checksum add-on module can be found here:
 *             - https://www.feaser.com/en/addons.php#checksum
 */
const tFirmwareInfoTable firmwareInfoTable __attribute__ ((section (".infoTable"))) =
{
  .tableId = 0x9A4B8107UL,
  .productId = 1234UL,
  .firmwareVersion = 10429UL
};


/************************************************************************************//**
** \brief     Initializes the user program application. Should be called once during
**            software program initialization.
** \return    none.
**
****************************************************************************************/
void AppInit(void)
{
  /* Initialize the timer driver. */
  TimerInit();
  /* Initialize the led driver. */
  LedInit();
  /* initialize the bootloader interface */
  BootComInit();
} /*** end of AppInit ***/


/************************************************************************************//**
** \brief     Task function of the user program application. Should be called
**            continuously in the program loop.
** \return    none.
**
****************************************************************************************/
void AppTask(void)
{
  /* Toggle LED with a fixed frequency. */
  LedToggle();
  /* check for bootloader activation request */
  BootComCheckActivationRequest();
} /*** end of AppTask ***/


/*********************************** end of app.c **************************************/
