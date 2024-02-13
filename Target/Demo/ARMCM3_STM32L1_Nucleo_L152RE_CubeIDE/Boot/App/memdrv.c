/************************************************************************************//**
* \file         Demo/ARMCM3_STM32L1_Nucleo_L152RE_CubeIDE/Boot/App/memdrv.c
* \brief        STM32L152RE EEPROM driver source file.
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

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */
#include "memdrv.h"                              /* memory device driver header        */
#include "stm32l1xx.h"                           /* STM32 CPU and HAL header           */


/* This source file implements the driver framework for an additional memory device. It
 * is interfaced to the bootloader's non-volatile memory (NVM) module, through
 * the NvmXxxHook() functions located in "hooks.c". The configuration macro
 * BOOT_NVM_HOOKS_ENABLE was set to 1 to activate these hook-functions.
 *
 * In a nutshell, you just need to implement the low-level memory erase and program
 * functionality in this source-file:
 *
 *   - MemDrvInit()
 *   - MemDrvWrite()
 *   - MemDrvErase()
 *   - MemDrvGetSize()
 *   - MemDrvGetBaseAddress()
 *   - MemDrvDone()
 *
 * Afterwards, the bootloader can reprogram data in this additional memory device, during
 * a firmware update.
 *
 * The specific example implementation contains an EEPROM driver for the STM32L152RE's
 * internal EEPROM.
 */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief   Total size in bytes of the memory device.
 *  \details The STM32L152RE features 16 kbyte of internal EEPROM. Note that the
 *           STM32L152RE is a so called Caterogy 5 device, according to section 1.5
 *           "Product category definition" of its reference manual. From an internal
 *           EEPROM device memory map perspective this means:
 *
 *              --------------------------------------------------------
 *             | Data EEPROM bank 1 | 0x08080000 - 0x08081FFF | 8 kbyte |
 *             |--------------------|-------------------------|---------|
 *             | Data EEPROM bank 2 | 0x08082000 - 0x08083FFF | 8 kbyte |
 *              --------------------------------------------------------
 */
#define MEM_DRV_DEVICE_TOTAL_SIZE      (16U * 1024U)

/** \brief   Base address of the memory device in the memory map.
 *  \details Some serial memory devices (e.g. I2C or SPI EEPROM) are not directly
 *           accessible through the memory map, but through the communication peripheral.
 *           In this case you would still need to set a base memory map address.
 *
 *           This is because the firmware file selected for the firmware update,
 *           typically an S-record, only supports data mapped to memory addresses. This
 *           means that you must have somehow placed the data destined for this memory
 *           device at a specific memory address in the firmware file.
 *
 *           For these types of serial memory devices, you can typically just make up
 *           your own base address, as long as it doesn't overlap with something else
 *           in the memory map. When communicating with the memory device, the first
 *           data typically appears at address 0. Consequently, you simply need to
 *           subtract MEM_DRV_DEVICE_BASE_ADDRESS from the 'address' parameter inside
 *           the MemDrvWrite() and MemDrvErase() functions.
 */
#define MEM_DRV_DEVICE_BASE_ADDR       (0x08080000UL)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static blt_bool MemDrvEraseWord(blt_addr address);
static blt_bool MemDrvProgramByte(blt_addr address, blt_int8u data);


/************************************************************************************//**
** \brief     Initializes the memory device driver.
** \return    none.
**
****************************************************************************************/
void MemDrvInit(void)
{
  /* Nothing needs to be done here for this memory device driver. */
} /*** end of MemDrvInit ***/


/************************************************************************************//**
** \brief     Write a certain amount of bytes to the memory device, starting at the
**            specified base address.
** \param     address The base address for the write operation.
** \param     len The number of bytes to write.
** \param     data Pointer to byte array where the data is read from.
** \return    BLT_TRUE if successful, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool MemDrvWrite(blt_addr address, blt_int32u len, blt_int8u const * data)
{
  blt_bool   result = BLT_FALSE;
  blt_int32u byteIdx;

  /* Although the EEPROM device has a minimum erase size of one word (32-bits / 4 bytes),
   * programming can actually be done one byte at a time.
   */

  /* Only continue with valid parameters. */
  if ( (len > 0U) &&
       (address >= MEM_DRV_DEVICE_BASE_ADDR) &&
       ((address + len) <= (MEM_DRV_DEVICE_BASE_ADDR + MEM_DRV_DEVICE_TOTAL_SIZE)) &&
       (data != BLT_NULL) )
  {
    /* Update the result for success and negate upon problem detection. */
    result = BLT_TRUE;
    /* Loop over all the data bytes. */
    for (byteIdx = 0U; byteIdx < len; byteIdx++)
    {
      /* Attempt to program this byte. */
      if (MemDrvProgramByte(address + byteIdx, data[byteIdx]) == BLT_FALSE)
      {
        /* Flag the error and stop the loop. */
        result = BLT_FALSE;
        break;
      }
      /* Service the watchdog. */
      CopService();
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of MemDrvWrite ***/


/************************************************************************************//**
** \brief     Erase a certain amount of bytes from the memory device, starting at the
**            specified base address.
** \param     address The base address for the erase operation.
** \param     len The number of bytes to erase.
** \return    BLT_TRUE if successful, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool MemDrvErase(blt_addr address, blt_int32u len)
{
  blt_bool     result = BLT_FALSE;
  blt_int8u    bytenums_to_restore_start;
  blt_int8u    bytenums_to_restore_end = 0;
  blt_addr     address_restore_start;
  blt_addr     address_restore_end;
  blt_int8u    bytes_restore_start[sizeof(blt_int32u)];
  blt_int8u    bytes_restore_end[sizeof(blt_int32u)];
  blt_addr     adjusted_base_address;
  blt_int32u   adjusted_length;
  blt_int8u  * byte_ptr;
  blt_int8u    byte_idx;
  blt_int16u   nr_of_erase_words;
  blt_int16u   word_idx;
  blt_addr     word_base_address;

  /* The EEPROM device has a minimum erase size of one word (32-bits / 4 bytes). This
   * means that when erasing a number of bytes that is not divisable by 4, it will
   * erase more bytes. To remedy this, the extra erased bytes should be backed up
   * before erasing the word and then restored after erasing the word. The erase
   * address does not have to be 32-bit aligned.
   *
   * Just keep in mind that this could lead to a problem. For example when trying to
   * erase the last 2 bytes of the EEPROM. This then means it will want to erase 2 more
   * bytes (to make one word), which are outside of its memory range.
   *
   * A good test case for these challenges is: Erasing 1 byte in the second-to-last
   * address of 0x08083FFE:
   *
   *  ----------------------------------
   *     0x08083FF8    |  0x08083FFC    |
   *  -----------------|----------------|
   *     0 | 1 | 2 | 3 |  0 | 1 | 2 | 3 |
   *  ----------------------------------
   *                              ^
   * One solution is to align the erase base address to the previous 32-bit boundary.
   * This is 0x08083FFC in this example. Additionally, round up the erase length to
   * the next multiple of 4. This would be 4 in this example. Before the actual erase
   * operation, backup the byte values 0, 1 and 3 of the word at 0x08083FFC, which
   * need to be reprogrammed after the erase operation.
   */

  /* Only continue with valid parameters. */
  if ( (len > 0U) &&
       (address >= MEM_DRV_DEVICE_BASE_ADDR) &&
       ((address + len) <= (MEM_DRV_DEVICE_BASE_ADDR + MEM_DRV_DEVICE_TOTAL_SIZE)) )
  {
    /* Update the result for success and negate upon problem detection. */
    result = BLT_TRUE;

    /* Calculate the base address of the previous 32-bit boundary, the erase length
     * rounded up to the next multiple of 4 bytes, the number of bytes to restore at
     * the start and at the end.
     */
    adjusted_base_address = (address / sizeof(blt_int32u)) * sizeof(blt_int32u);
    bytenums_to_restore_start = address - adjusted_base_address;
    adjusted_length = len + bytenums_to_restore_start;
    if ((adjusted_length % sizeof(blt_int32u)) != 0)
    {
      bytenums_to_restore_end += sizeof(blt_int32u) -
                                 (adjusted_length % sizeof(blt_int32u));
      adjusted_length += bytenums_to_restore_end;
    }
    /* Calculate the address where the bytes at the start and at the end need to be
     * restored.
     */
    address_restore_start = adjusted_base_address;
    address_restore_end = address + len;

    /* Sanity checks. */
    ASSERT_RT((bytenums_to_restore_start <= sizeof(blt_int32u)));
    ASSERT_RT((bytenums_to_restore_end   <= sizeof(blt_int32u)));
    ASSERT_RT((adjusted_length % sizeof(blt_int32u)) == 0U);

    /* Backup the bytes at the start and at the end, if any. */
    byte_ptr = (blt_int8u *)address_restore_start;
    for (byte_idx = 0U; byte_idx < bytenums_to_restore_start; byte_idx++)
    {
      bytes_restore_start[byte_idx] = byte_ptr[byte_idx];
    }
    byte_ptr = (blt_int8u *)address_restore_end;
    for (byte_idx = 0U; byte_idx < bytenums_to_restore_end; byte_idx++)
    {
      bytes_restore_end[byte_idx] = byte_ptr[byte_idx];
    }

    /* Erase all the words one-by-one. */
    nr_of_erase_words = adjusted_length / sizeof(blt_int32u);
    for (word_idx = 0U; word_idx < nr_of_erase_words; word_idx++)
    {
      word_base_address = adjusted_base_address + (word_idx * sizeof(blt_int32u));
      if (MemDrvEraseWord(word_base_address) == BLT_FALSE)
      {
        /* Flag the error and stop the loop. */
        result = BLT_FALSE;
        break;
      }
      /* Service the watchdog. */
      CopService();
    }

    /* Only continue if all is okay so far. */
    if (result == BLT_TRUE)
    {
      /* Restore the bytes at the start, if any. */
      for (byte_idx = 0U; byte_idx < bytenums_to_restore_start; byte_idx++)
      {
        if (MemDrvProgramByte(address_restore_start + byte_idx,
                              bytes_restore_start[byte_idx]) == BLT_FALSE)
        {
          /* Flag the error and stop the loop. */
          result = BLT_FALSE;
          break;
        }
      }
    }

    /* Only continue if all is okay so far. */
    if (result == BLT_TRUE)
    {
      /* Restore the bytes at the end, if any. */
      for (byte_idx = 0U; byte_idx < bytenums_to_restore_end; byte_idx++)
      {
        if (MemDrvProgramByte(address_restore_end + byte_idx,
                              bytes_restore_end[byte_idx]) == BLT_FALSE)
        {
          /* Flag the error and stop the loop. */
          result = BLT_FALSE;
          break;
        }
      }
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of MemDrvErase ***/


/************************************************************************************//**
** \brief     Getter to read the total size of the memory device.
** \return    Total size in bytes of the memory device.
**
****************************************************************************************/
blt_int32u MemDrvGetSize(void)
{
  return MEM_DRV_DEVICE_TOTAL_SIZE;
} /*** end of MemDrvGetSize ***/


/************************************************************************************//**
** \brief     Getter to read the base address of the memory device in the memory map.
** \return    Base address of the memory device.
**
****************************************************************************************/
blt_addr MemDrvGetBaseAddress(void)
{
  return MEM_DRV_DEVICE_BASE_ADDR;
} /*** end of MemDrvGetBaseAddress ***/


/************************************************************************************//**
** \brief     Finalizes the memory device driver operations.
** \details   This function is called at the end of a firmware update. You can use it to
**            finalize the memory device erase/program operations, if applicable.
**            For example if data is first buffered in RAM blocks, before actually
**            writing to the device, there might still be data pending to be written to
**            the device at the end of the firmware update. This can be done  in this
**            function.
** \return    BLT_TRUE is succesful, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool MemDrvDone(void)
{
  blt_bool result = BLT_TRUE;

  /* Nothing needs to be done here for this memory device driver. */

  /* Give the result back to the caller. */
  return result;
} /*** end of MemDrvDone ***/


/************************************************************************************//**
** \brief     Erase a 32-bit value from the specified address.
** \details   The EEPROM device supports erasing a word value any address inside the
**            array. That means that there is no need for the address to be 32-bit
**            aligned.
** \param     address The base address for the erase operation.
** \return    BLT_TRUE if successful, BLT_FALSE otherwise.
**
****************************************************************************************/
static blt_bool MemDrvEraseWord(blt_addr address)
{
  blt_bool result = BLT_TRUE;

  /* Only perform the actual erase operation if the word is not already erased. */
  if (*(blt_int32u volatile *)address != 0U)
  {
    /* Unlock to enable the EEPROM control register access. */
    HAL_FLASHEx_DATAEEPROM_Unlock();
    /* Perform the word erase operation. */
    if (HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEERASEDATA_WORD, address) != HAL_OK)
    {
      /* Flag the error. */
      result = BLT_FALSE;
    }
    /* Lock to disable the EEPROM control register access. */
    HAL_FLASHEx_DATAEEPROM_Lock();

    /* Read the erased value back as a final verification, if all is okay sofar. */
    if (result == BLT_TRUE)
    {
      if (*(blt_int32u volatile *)address != 0U)
      {
        /* Flag the error. */
        result = BLT_FALSE;
      }
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of MemDrvEraseWord ***/


/************************************************************************************//**
** \brief     Programs an 8-bit value at the specified address.
** \details   The EEPROM device supports programming a byte value at any address inside
**            the array. That means that there is no need for the address to be 32-bit
**            aligned.
** \param     address The base address for the program operation.
** \param     data Word value to program.
** \return    BLT_TRUE if successful, BLT_FALSE otherwise.
**
****************************************************************************************/
static blt_bool MemDrvProgramByte(blt_addr address, blt_int8u data)
{
  blt_bool result = BLT_TRUE;

  /* Only perform the actual program operation if the word is not already the same. */
  if (*(blt_int8u volatile *)address != data)
  {
    /* Unlock to enable the EEPROM control register access. */
    HAL_FLASHEx_DATAEEPROM_Unlock();
    /* Perform the word program operation. Use the 'fast' option, as it is expected
     * that the data is already in the erased state.
     */
    if (HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_FASTBYTE, address, data)
        != HAL_OK)
    {
      /* Flag the error. */
      result = BLT_FALSE;
    }
    /* Lock to disable the EEPROM control register access. */
    HAL_FLASHEx_DATAEEPROM_Lock();

    /* Read the programmed value back as a final verification, if all is okay sofar. */
    if (result == BLT_TRUE)
    {
      if (*(blt_int8u volatile *)address != data)
      {
        /* Flag the error. */
        result = BLT_FALSE;
      }
    }
  }

  /* Give the result back to the caller. */
  return result;

} /*** end of MemDrvProgramByte ***/

/*********************************** end of memdrv.c ***********************************/
