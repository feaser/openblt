/************************************************************************************//**
* \file         Demo/ARMCM4_XMC4_XMC4700_Relax_Kit_CMake/Boot/hooks.c
* \brief        Bootloader callback source file.
* \ingroup      Boot_ARMCM4_XMC4_XMC4700_Relax_Kit_CMake
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2026  by Feaser    http://www.feaser.com    All rights reserved
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
#include "led.h"                                 /* LED driver header                  */
#include "SEGGER_RTT.h"                          /* Segger real-time transfer          */
#include "xmc_gpio.h"                            /* GPIO module                        */


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
  /* additional and optional backdoor entry through BUTTON1 on the board. to
   * force the bootloader to stay active after reset, keep it pressed during reset.
   */
  if (XMC_GPIO_GetInput(P15_13) == 0)
  {
    /* pushbutton pressed, so do not start the user program and keep the
     * bootloader active instead.
     */
    return BLT_FALSE;
  }

  /* clean up the LED driver */
  LedBlinkExit();

  /*  okay to start the user program.*/
  return BLT_TRUE;
} /*** end of CpuUserProgramStartHook ***/
#endif /* BOOT_CPU_USER_PROGRAM_START_HOOK > 0 */


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
  /* this function is called upon initialization. might as well use it to initialize
   * the LED driver. It is kind of a visual watchdog anyways.
   */
  LedBlinkInit(100);
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
  /* run the LED blink task. this is a better place to do it than in the main() program
   * loop. certain operations such as flash erase can take a long time, which would cause
   * a blink interval to be skipped. this function is also called during such operations,
   * so no blink intervals will be skipped when calling the LED blink task here.
   */
  LedBlinkTask();
} /*** end of CopServiceHook ***/
#endif /* BOOT_COP_HOOKS_ENABLE > 0 */


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


#if (BOOT_NVM_CHECKSUM_HOOKS_ENABLE > 0)
/************************************************************************************//**
** \brief     Verifies the checksum, which indicates that a valid user program is
**            present and can be started.
** \return    BLT_TRUE if successful, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool NvmVerifyChecksumHook(void)
{
  return BLT_TRUE;
} /*** end of NvmVerifyChecksum ***/


/************************************************************************************//**
** \brief     Writes a checksum of the user program to non-volatile memory. This is
**            performed once the entire user program has been programmed. Through
**            the checksum, the bootloader can check if a valid user programming is
**            present and can be started.
** \return    BLT_TRUE if successful, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool NvmWriteChecksumHook(void)
{
  return BLT_TRUE;
}
#endif /* BOOT_NVM_CHECKSUM_HOOKS_ENABLE > 0 */


/****************************************************************************************
*   U S B   C O M M U N I C A T I O N   I N T E R F A C E   H O O K   F U N C T I O N S
****************************************************************************************/

#if (BOOT_COM_USB_ENABLE > 0)
/************************************************************************************//**
** \brief     Callback that gets called whenever the USB device should be connected
**            to the USB bus.
** \param     connect BLT_TRUE to connect and BLT_FALSE to disconnect.
** \return    none.
**
****************************************************************************************/
void UsbConnectHook(blt_bool connect)
{
  /* Note that this is handled automatically by the OTG peripheral. */
} /*** end of UsbConnect ***/


/************************************************************************************//**
** \brief     Callback that gets called whenever the USB host requests the device
**            to enter a low power mode.
** \return    none.
**
****************************************************************************************/
void UsbEnterLowPowerModeHook(void)
{
  /* support to enter a low power mode can be implemented here */
} /*** end of UsbEnterLowPowerMode ***/


/************************************************************************************//**
** \brief     Callback that gets called whenever the USB host requests the device to
**            exit low power mode.
** \return    none.
**
****************************************************************************************/
void UsbLeaveLowPowerModeHook(void)
{
  /* support to leave a low power mode can be implemented here */
} /*** end of UsbLeaveLowPowerMode ***/
#endif /* BOOT_COM_USB_ENABLE > 0 */


/****************************************************************************************
*   F I L E   S Y S T E M   I N T E R F A C E   H O O K   F U N C T I O N S
****************************************************************************************/

#if (BOOT_FILE_SYS_ENABLE > 0)
/****************************************************************************************
* Constant data declarations
****************************************************************************************/
/** \brief Firmware filename. */
static const blt_char firmwareFilename[] = "/demoprog_xmc4700.srec";


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
#endif /* BOOT_FILE_SYS_ENABLE > 0 */


/****************************************************************************************
*   E V E N T S   M O D U L E   H O O K   F U N C T I O N S
****************************************************************************************/

#if (BOOT_EVENTS_ENABLE > 0)
/***********************************************************************************//**
** \brief     Callback that gets called when a firmware update related event gets
**            triggered. Implement your event handling here. For example for updating
**            a user interface or for logging purposes.
**            Cast the opaque info pointer to the correct structure type (tEventsInfoXxx)
**            to access additional event related information.
** \param     id The identifier of the event that occurred.
** \param     info Opaque pointer to event identifier related information. Can be 
**            BLT_NULL depending on the event identifer. For example when no additional
**            information available for the event.
** \return    none
**
****************************************************************************************/
void EventsHook(tEventsId id, void const *info)
{
  blt_addr                 base_addr;
  blt_int32u               num_bytes;
  blt_int8u                progress;
  blt_char         const * filename;
  blt_int8u                node_id;
  tEventsErrorId           error_id;
  tEventsStartType         update_type;
  static blt_bool          update_from_file = BLT_FALSE;
  static blt_char  const * update_types[] = { "Normal firmware update", 
                                              "Create new rollback image",
                                              "Restore from rollback image"
                                            };

  /* Filter on the events identifier. */
  switch (id)
  {
    case EVENT_ID_ON_WRITE:
      base_addr = ((tEventsInfoWrite const *)info)->base_addr;
      num_bytes = ((tEventsInfoWrite const *)info)->num_bytes;
      progress  = ((tEventsInfoWrite const *)info)->progress;
      SEGGER_RTT_printf(0, "Programming %3u bytes at %08Xh [%3u%%].\n",
                        num_bytes, base_addr, progress);
      break;

    case EVENT_ID_ON_ERASE:
      base_addr = ((tEventsInfoErase const *)info)->base_addr;
      num_bytes = ((tEventsInfoErase const *)info)->num_bytes;
      SEGGER_RTT_printf(0, "Erasing %lu bytes from %08Xh.\n", num_bytes, base_addr);
      break;

    case EVENT_ID_ON_ENTRY:
      SEGGER_RTT_WriteString(0, "Bootloader initialized.\n");
      break;

    case EVENT_ID_ON_EXIT:
      SEGGER_RTT_WriteString(0, "Starting user program.\n");
      break;

    case EVENT_ID_ON_START:
      update_from_file = BLT_FALSE;
      update_type = ((tEventsInfoStart const *)info)->type;
      filename = ((tEventsInfoStart const *)info)->filename;
      node_id = ((tEventsInfoStart const *)info)->node_id;
      SEGGER_RTT_WriteString(0, "Firmware update started");
      if (filename != BLT_NULL)
      {
        SEGGER_RTT_printf(0, " (%s)", filename);
        update_from_file = BLT_TRUE;
      }
      else
      {
        SEGGER_RTT_printf(0, " (%d)", node_id);
      }
      SEGGER_RTT_printf(0, " [%s].\n", update_types[update_type]);
      break;

    case EVENT_ID_ON_SUCCESS:
      SEGGER_RTT_WriteString(0, "Successfully updated the firmware.\n");
      if (update_from_file == BLT_TRUE)
      {
#if (BOOT_FILE_SYS_ENABLE > 0)
        /* now delete the firmware file from the disk since the update was successful */
        f_unlink(firmwareFilename);
#endif        
      }
      break;

    case EVENT_ID_ON_SUPPRESS:
      SEGGER_RTT_WriteString(0, "User program start bypassed.\n");
      break;

    case EVENT_ID_ON_ERROR:
      error_id = ((tEventsInfoError const *)info)->error_id;
      SEGGER_RTT_printf(0, "Error with id #%u detected. Aborting.\n", error_id);
      break;

    default:
      break;  
  }
} /*** end of EventsHook ***/
#endif /* BOOT_EVENTS_ENABLE > 0 */


/****************************************************************************************
*   S E E D / K E Y   S E C U R I T Y   H O O K   F U N C T I O N S
****************************************************************************************/

#if (BOOT_XCP_SEED_KEY_ENABLE > 0)
/************************************************************************************//**
** \brief     Provides a seed to the XCP master that will be used for the key
**            generation when the master attempts to unlock the specified resource.
**            Called by the GET_SEED command.
** \param     resource  Resource that the seed if requested for (XCP_RES_XXX).
** \param     seed      Pointer to byte buffer wher the seed will be stored.
** \return    Length of the seed in bytes.
**
****************************************************************************************/
blt_int8u XcpGetSeedHook(blt_int8u resource, blt_int8u *seed)
{
  /* request seed for unlocking ProGraMming resource */
  if ((resource & XCP_RES_PGM) != 0)
  {
    seed[0] = 0x55;
  }

  /* return seed length */
  return 1;
} /*** end of XcpGetSeedHook ***/


/************************************************************************************//**
** \brief     Called by the UNLOCK command and checks if the key to unlock the
**            specified resource was correct. If so, then the resource protection
**            will be removed.
** \param     resource  resource to unlock (XCP_RES_XXX).
** \param     key       pointer to the byte buffer holding the key.
** \param     len       length of the key in bytes.
** \return    1 if the key was correct, 0 otherwise.
**
****************************************************************************************/
blt_int8u XcpVerifyKeyHook(blt_int8u resource, blt_int8u *key, blt_int8u len)
{
  /* suppress compiler warning for unused parameter */
  len = len;

  /* the example key algorithm in "libseednkey.dll" works as follows:
   *  - PGM will be unlocked if key = seed - 1
   */

  /* check key for unlocking ProGraMming resource */
  if ((resource == XCP_RES_PGM) && (key[0] == (0x55-1)))
  {
    /* correct key received for unlocking PGM resource */
    return 1;
  }

  /* still here so key incorrect */
  return 0;
} /*** end of XcpVerifyKeyHook ***/
#endif /* BOOT_XCP_SEED_KEY_ENABLE > 0 */


/*********************************** end of hooks.c ************************************/
