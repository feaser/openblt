/************************************************************************************//**
* \file         Demo\ARMCM3_LM3S_EK_LM3S6965_GCC\Boot\hooks.c
* \brief        Bootloader callback source file.
* \ingroup      Boot_ARMCM3_LM3S_EK_LM3S6965_GCC
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2012  by Feaser    http://www.feaser.com    All rights reserved
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
* You should have received a copy of the GNU General Public License along with OpenBLT.
* If not, see <http://www.gnu.org/licenses/>.
*
* A special exception to the GPL is included to allow you to distribute a combined work 
* that includes OpenBLT without being obliged to provide the source code for any 
* proprietary components. The exception text is included at the bottom of the license
* file <license.html>.
* 
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */
#if (BOOT_FILE_LOGGING_ENABLE > 0)
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/uartlib.h"
#endif


/****************************************************************************************
*   B A C K D O O R   E N T R Y   H O O K   F U N C T I O N S
****************************************************************************************/

#if (BOOT_BACKDOOR_HOOKS_ENABLE > 0)
/************************************************************************************//**
** \brief     Initializes the backdoor entry option.
** \return    none.
**
****************************************************************************************/
void BackDoorInitHook(void)
{
} /*** end of BackDoorInitHook ***/


/************************************************************************************//**
** \brief     Checks if a backdoor entry is requested.
** \return    BLT_TRUE if the backdoor entry is requested, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool BackDoorEntryHook(void)
{
  /* default implementation always activates the bootloader after a reset */
  return BLT_TRUE;
} /*** end of BackDoorEntryHook ***/
#endif /* BOOT_BACKDOOR_HOOKS_ENABLE > 0 */


/****************************************************************************************
*   C P U   D R I V E R   H O O K   F U N C T I O N S
****************************************************************************************/

#if (BOOT_CPU_USER_PROGRAM_START_HOOK > 0)
/************************************************************************************//**
** \brief     Callback that gets called when the bootloader is about to exit and
**            hand over control to the user program. This is the last moment that
**            some final checking can be performed and if necessary prevent the
**            bootloader from activiting the user program.
** \return    BLT_TRUE if it is okay to start the user program, BLT_FALSE to keep
**            keep the bootloader active.
**
****************************************************************************************/
blt_bool CpuUserProgramStartHook(void)
{
  /* okay to start the user program */
  return BLT_TRUE;
} /*** end of CpuUserProgramStartHook ***/
#endif /* BOOT_CPU_USER_PROGRAM_START_HOOK > 0 */


/****************************************************************************************
*   N O N - V O L A T I L E   M E M O R Y   D R I V E R   H O O K   F U N C T I O N S
****************************************************************************************/

#if (BOOT_NVM_HOOKS_ENABLE > 0)
/************************************************************************************//**
** \brief     Callback that gets called at the start of the internal NVM driver
**            initialization routine. 
** \return    none.
**
****************************************************************************************/
void NvmInitHook(void)
{
} /*** end of NvmInitHook ***/


/************************************************************************************//**
** \brief     Callback that gets called at the start of the NVM driver write 
**            routine. It allows additional memory to be operated on. If the address
**            is not within the range of the additional memory, then 
**            BLT_NVM_NOT_IN_RANGE must be returned to indicate that the data hasn't
**            been written yet.
** \param     addr Start address.
** \param     len  Length in bytes.
** \param     data Pointer to the data buffer.
** \return    BLT_NVM_OKAY if successful, BLT_NVM_NOT_IN_RANGE if the address is
**            not within the supported memory range, or BLT_NVM_ERROR is the write
**            operation failed.
**
****************************************************************************************/
blt_int8u NvmWriteHook(blt_addr addr, blt_int32u len, blt_int8u *data)
{
  return BLT_NVM_NOT_IN_RANGE;
} /*** end of NvmWriteHook ***/


/************************************************************************************//**
** \brief     Callback that gets called at the start of the NVM driver erase 
**            routine. It allows additional memory to be operated on. If the address
**            is not within the range of the additional memory, then
**            BLT_NVM_NOT_IN_RANGE must be returned to indicate that the memory
**            hasn't been erased yet.
** \param     addr Start address.
** \param     len  Length in bytes.
** \return    BLT_NVM_OKAY if successful, BLT_NVM_NOT_IN_RANGE if the address is
**            not within the supported memory range, or BLT_NVM_ERROR is the erase
**            operation failed.
**
****************************************************************************************/
blt_int8u NvmEraseHook(blt_addr addr, blt_int32u len)
{
  return BLT_NVM_NOT_IN_RANGE;
} /*** end of NvmEraseHook ***/


/************************************************************************************//**
** \brief     Callback that gets called at the end of the NVM programming session.
** \return    BLT_TRUE is successful, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool NvmDoneHook(void)
{
  return BLT_TRUE;
} /*** end of NvmDoneHook ***/
#endif /* BOOT_NVM_HOOKS_ENABLE > 0 */


/****************************************************************************************
*   W A T C H D O G   D R I V E R   H O O K   F U N C T I O N S
****************************************************************************************/

#if (BOOT_COP_HOOKS_ENABLE > 0)
/************************************************************************************//**
** \brief     Callback that gets called at the end of the internal COP driver
**            initialization routine. It can be used to configure and enable the
**            watchdog.
** \return    none.
**
****************************************************************************************/
void CopInitHook(void)
{
} /*** end of CopInitHook ***/


/************************************************************************************//**
** \brief     Callback that gets called at the end of the internal COP driver
**            service routine. This gets called upon initialization and during
**            potential long lasting loops and routine. It can be used to service
**            the watchdog to prevent a watchdog reset.
** \return    none.
**
****************************************************************************************/
void CopServiceHook(void)
{
} /*** end of CopServiceHook ***/
#endif /* BOOT_COP_HOOKS_ENABLE > 0 */


/****************************************************************************************
*   F I L E   S Y S T E M   I N T E R F A C E   H O O K   F U N C T I O N S
****************************************************************************************/

#if (BOOT_FILE_SYS_ENABLE > 0)

/****************************************************************************************
* Constant data declarations
****************************************************************************************/
/** \brief Firmware filename. */
static const blt_char firmwareFilename[] = "/demoprog_ek_lm3s6965.srec";


/****************************************************************************************
* Local data declarations
****************************************************************************************/
#if (BOOT_FILE_LOGGING_ENABLE > 0)
/** \brief Data structure for grouping log-file related information. */
static struct 
{
  FIL      handle;                  /**< FatFS handle to the log-file.                 */
  blt_bool canUse;                  /**< Flag to indicate if the log-file can be used. */
} logfile;
#endif


/************************************************************************************//**
** \brief     Callback that gets called to check whether a firmware update from 
**            local file storage should be started. This could for example be when
**            a switch is pressed, when a certain file is found on the local file 
**            storage, etc.
** \return    BLT_TRUE if a firmware update is requested, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool FileIsFirmwareUpdateRequestedHook(void)
{
  FILINFO fileInfoObject = { 0 }; /* needs to be zeroed according to f_stat docs */;

  /* Current example implementation looks for a predetermined firmware file on the 
   * SD-card. If the SD-card is accessible and the firmware file was found the firmware
   * update is started. When successfully completed, the firmware file is deleted.
   * During the firmware update, progress information is written to a file called
   * bootlog.txt and additionally outputted on UART @57600 bps for debugging purposes.
   */
  /* check if firmware file is present and SD-card is accessible */
  if (f_stat(firmwareFilename, &fileInfoObject) == FR_OK) 
  {
    /* check if the filesize is valid and that it is not a directory */
    if ( (fileInfoObject.fsize > 0) && (!(fileInfoObject.fattrib & AM_DIR)) )
    {
      /* all conditions are met to start a firmware update from local file storage */
      return BLT_TRUE;
    }
  }
  /* still here so no firmware update request is pending */  
  return BLT_FALSE;
} /*** end of FileIsFirmwareUpdateRequestedHook ***/


/************************************************************************************//**
** \brief     Callback to obtain the filename of the firmware file that should be
**            used during the firmware update from the local file storage. This 
**            hook function is called at the beginning of the firmware update from
**            local storage sequence. 
** \return    valid firmware filename with full path or BLT_NULL.
**
****************************************************************************************/
const blt_char *FileGetFirmwareFilenameHook(void)
{
  return firmwareFilename;
} /*** end of FileGetFirmwareFilenameHook ***/


#if (BOOT_FILE_STARTED_HOOK_ENABLE > 0)
/************************************************************************************//**
** \brief     Callback that gets called to inform the application that a firmware
**            update from local storage just started. 
** \return    none.
**
****************************************************************************************/
void FileFirmwareUpdateStartedHook(void)
{
  #if (BOOT_FILE_LOGGING_ENABLE > 0)
  /* create/overwrite the logfile */
  logfile.canUse = BLT_FALSE;
  if (f_open(&logfile.handle, "/bootlog.txt", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
  {
    logfile.canUse = BLT_TRUE;
  }
  #endif
} /*** end of FileFirmwareUpdateStartedHook ***/
#endif /* BOOT_FILE_STARTED_HOOK_ENABLE > 0 */


#if (BOOT_FILE_COMPLETED_HOOK_ENABLE > 0)
/************************************************************************************//**
** \brief     Callback that gets called to inform the application that a firmware
**            update was successfully completed.
** \return    none.
**
****************************************************************************************/
void FileFirmwareUpdateCompletedHook(void)
{
  #if (BOOT_FILE_LOGGING_ENABLE > 0)
  /* close the log file */
  if (logfile.canUse == BLT_TRUE)
  {
    f_close(&logfile.handle);
  }
  /* wait for all logging related transmission to complete */
  while (UARTBusy(UART0_BASE) == true);
  #endif
  /* now delete the firmware file from the disk since the update was successful */
  f_unlink(firmwareFilename);
} /*** end of FileFirmwareUpdateCompletedHook ***/
#endif /* BOOT_FILE_COMPLETED_HOOK_ENABLE > 0 */


#if (BOOT_FILE_ERROR_HOOK_ENABLE > 0)
/************************************************************************************//**
** \brief     Callback that gets called in case an error occurred during a firmware
**            update. Refer to <file.h> for a list of available error codes.
** \return    none.
**
****************************************************************************************/
void FileFirmwareUpdateErrorHook(blt_int8u error_code)
{
  #if (BOOT_FILE_LOGGING_ENABLE > 0)
  /* error detected which stops the firmware update, so close the log file */
  if (logfile.canUse == BLT_TRUE)
  {
    f_close(&logfile.handle);
  }
  #endif
} /*** end of FileFirmwareUpdateErrorHook ***/
#endif /* BOOT_FILE_ERROR_HOOK_ENABLE > 0 */


#if (BOOT_FILE_LOGGING_ENABLE > 0)
/************************************************************************************//**
** \brief     Callback that gets called each time new log information becomes 
**            available during a firmware update.
** \param     info_string Pointer to a character array with the log entry info.
** \return    none.
**
****************************************************************************************/
void FileFirmwareUpdateLogHook(blt_char *info_string)
{
  /* write the string to the log file */
  if (logfile.canUse == BLT_TRUE)
  {
    if (f_puts(info_string, &logfile.handle) < 0)
    {
      logfile.canUse = BLT_FALSE;
      f_close(&logfile.handle);
    }
  }
  /* echo all characters in the string on UART */
  while(*info_string != '\0')
  {
    /* write character to transmit holding register */
    UARTCharPutNonBlocking(UART0_BASE, *info_string);
    /* wait for tx holding register to be empty */
    while(UARTSpaceAvail(UART0_BASE) == false);
    /* point to the next character in the string */
    info_string++;
  }
} /*** end of FileFirmwareUpdateLogHook ***/
#endif /* BOOT_FILE_LOGGING_ENABLE > 0 */


#endif /* BOOT_FILE_SYS_ENABLE > 0 */


/*********************************** end of hooks.c ************************************/
