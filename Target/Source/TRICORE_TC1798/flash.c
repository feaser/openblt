/************************************************************************************//**
* \file         Source\TRICORE_TC1798\flash.c
* \brief        Bootloader flash driver source file.
* \ingroup      Target_TRICORE_TC1798
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2015  by Feaser    http://www.feaser.com    All rights reserved
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
#include "cpu_comp.h"                            /* compiler specific CPU definitions  */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Value for an invalid flash sector. */
#define FLASH_INVALID_SECTOR    (0xffu)

/** \brief Value for an invalid flash address. */
#define FLASH_INVALID_ADDRESS   (0xffffffffu)

/** \brief Standard size of a flash block for writing. */
#define FLASH_WRITE_BLOCK_SIZE  (256u)

/** \brief Total numbers of sectors in array flashLayout[]. */
#define FLASH_TOTAL_SECTORS     (sizeof(flashLayout)/sizeof(flashLayout[0]))

/** \brief End address of the bootloader programmable flash. */
#define FLASH_END_ADDRESS       (flashLayout[FLASH_TOTAL_SECTORS-1].sector_start + \
                                 flashLayout[FLASH_TOTAL_SECTORS-1].sector_size - 1)

/** \brief The flash driver is setup to operate on the cached PFLASH addresses, whereas
 *         the actual PFLASH commands should operate on non-cached addresses. This
 *         macro defines the offset between cached (80xxxxxxh) and non-cached
 *        (A0xxxxxxh) addresses.
 */
#define FLASH_NON_CACHED_OFFSET (0x20000000u)

/** \brief Base address of the PFLASH0 module. */
#define FLASH_PFLASH0_BASE      (0x80000000u)

/** \brief Base address of the PFLASH1 module. */
#define FLASH_PFLASH1_BASE      (0x80800000u)

/** \brief Base address of the PFLASH0 flash status register. */
#define FLASH_PFLASH0_FSR_ADDR  (0xF8002010u)

/** \brief Base address of the PFLASH0 flash status register. */
#define FLASH_PFLASH1_FSR_ADDR  (0xF8004010u)

/** \brief Macro that returns the PFLASHx modules base address, giving any address
 *         in PFLASH.
 */
#define FLASH_GET_PFLASH_BASE(addr) ((addr < FLASH_PFLASH1_BASE) ? \
                                     FLASH_PFLASH0_BASE : FLASH_PFLASH1_BASE)

/** \brief Macro that converts are 32 bit address into a pointer to a 32-bit unsigned
 *         value and writes a value to this pointer.
 */
#define FLASH_WRITE_TO_U32_PTR_BY_ADDR(addr, val) ((*((volatile blt_int32u *) (addr+FLASH_NON_CACHED_OFFSET))) = val)

/** \brief The FLASHx_FSR register is the only register used in this driver. Its address
 *         depends on the PFLASH module that is being operated on. This macro gets the
 *         correct base address for the FSR register.
 */
#define FLASH_GET_FSR_REG_ADDR(addr) ((addr < FLASH_PFLASH1_BASE) ? \
                                      FLASH_PFLASH0_FSR_ADDR : FLASH_PFLASH1_FSR_ADDR)

/** \brief Offset in bytes from the bootblock's base address where the checksum is
 *         located.
 */
#define FLASH_CS_OFFSET               (0x04u)

/** \brief Offset in bytes from the bootblock's base address where the checksum area
 *        starts.
 */
#define FLASH_CS_RANGE_START_OFFSET   (0x08u)

/** \brief The total number of 32-bit words that are in the checksum address range. */
#define FLASH_CS_RANGE_TOTAL_WORDS    ((FLASH_WRITE_BLOCK_SIZE/4u) - \
                                       (FLASH_CS_RANGE_START_OFFSET/4u))

/** \brief Maximum time for a sector erase operation as specified by the Tricore data-
 *         sheet with an added margin of at least 20%.
 */
#define FLASH_ERASE_TIME_MAX_MS         (5100)

/** \brief Maximum time for a page program operation as specified by the Tricore data-
 *         sheet with an added margin of at least 20%.
 */
#define FLASH_PROGRAM_TIME_MAX_MS       (40)

                                       
/****************************************************************************************
* Plausibility checks
****************************************************************************************/
#ifndef BOOT_FLASH_CUSTOM_LAYOUT_ENABLE
#define BOOT_FLASH_CUSTOM_LAYOUT_ENABLE (0u)
#endif


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Flash sector descriptor type. */
typedef struct
{
  blt_addr   sector_start;                       /**< sector start address             */
  blt_int32u sector_size;                        /**< sector size in bytes             */
  blt_int8u  sector_num;                         /**< sector number                    */
} tFlashSector;

/** \brief    Structure type for grouping flash block information.
 *  \details  Programming is done per block of max FLASH_WRITE_BLOCK_SIZE. for this a
 *            flash block manager is implemented in this driver. this flash block manager
 *            depends on this flash block info structure. It holds the base address of
 *            the flash block and the data that should be programmed into the flash
 *            block. The .base_addr must be a multiple of FLASH_WRITE_BLOCK_SIZE.
 */
typedef struct
{
  blt_addr  base_addr;
  blt_int8u data[FLASH_WRITE_BLOCK_SIZE];
} tFlashBlockInfo;


/****************************************************************************************
* Hook functions
****************************************************************************************/
#if (BOOT_FLASH_CRYPTO_HOOKS_ENABLE > 0)
extern blt_bool FlashCryptoDecryptDataHook(blt_int8u * data, blt_int32u size);
#endif


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static blt_bool   FlashInitBlock(tFlashBlockInfo *block, blt_addr address);
static tFlashBlockInfo *FlashSwitchBlock(tFlashBlockInfo *block, blt_addr base_addr);
static blt_bool   FlashAddToBlock(tFlashBlockInfo *block, blt_addr address,
                                  blt_int8u *data, blt_int32u len);
static blt_bool   FlashWriteBlock(tFlashBlockInfo *block);
static blt_bool   FlashEraseSectors(blt_int8u first_sector, blt_int8u last_sector);
static blt_int8u  FlashGetSector(blt_addr address);
static blt_addr   FlashGetSectorBaseAddr(blt_int8u sector);
static blt_int32u FlashGetSectorSize(blt_int8u sector);
static blt_bool   FlashTricoreProgramPage(blt_addr start_addr, blt_int8u *data);
static blt_bool   FlashTricoreEraseSector(blt_addr start_addr);


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief   If desired, it is possible to set BOOT_FLASH_CUSTOM_LAYOUT_ENABLE to > 0
 *           in blt_conf.h and then implement your own version of the flashLayout[] table
 *           in a source-file with the name flash_layout.c. This way you customize the
 *           flash memory size reserved for the bootloader, without having to modify
 *           the flashLayout[] table in this file directly. This file will then include
 *           flash_layout.c so there is no need to compile it additionally with your
 *           project.
 */
#if (BOOT_FLASH_CUSTOM_LAYOUT_ENABLE == 0)
/** \brief   Array wit the layout of the flash memory.
 *  \details The current implementation assumes that the bootloader is in the 2Mbyte
 *           PFLASH0 and supports flash operations only on the 2Mbyte PFLASH1. The reason
 *           for this is that a flash module cannot be in read mode and command mode at
 *           the same time. A future improvement is one where the actual flash command
 *           code is copied and run from RAM, to bypass this restriction.
 */
static const tFlashSector flashLayout[] =
{
  { 0x80800000, 0x04000,  0},           /* flash sector  0 -  16kb                     */
  { 0x80804000, 0x04000,  1},           /* flash sector  1 -  16kb                     */
  { 0x80808000, 0x04000,  2},           /* flash sector  2 -  16kb                     */
  { 0x8080C000, 0x04000,  3},           /* flash sector  3 -  16kb                     */
  { 0x80810000, 0x04000,  4},           /* flash sector  4 -  16kb                     */
  { 0x80814000, 0x04000,  5},           /* flash sector  5 -  16kb                     */
  { 0x80818000, 0x04000,  6},           /* flash sector  6 -  16kb                     */
  { 0x8081C000, 0x04000,  7},           /* flash sector  7 -  16kb                     */
  { 0x80820000, 0x20000,  8},           /* flash sector  8 - 128kb                     */
  { 0x80840000, 0x40000,  9},           /* flash sector  9 - 256kb                     */
  { 0x80880000, 0x40000, 10},           /* flash sector 10 - 256kb                     */
  { 0x808C0000, 0x40000, 11},           /* flash sector 11 - 256kb                     */
  { 0x80900000, 0x40000, 12},           /* flash sector 12 - 256kb                     */
  { 0x80940000, 0x40000, 13},           /* flash sector 13 - 256kb                     */
  { 0x80980000, 0x40000, 14},           /* flash sector 14 - 256kb                     */
  { 0x809C0000, 0x40000, 15},           /* flash sector 15 - 256kb                     */
#if (BOOT_NVM_SIZE_KB > 2048)
#error "BOOT_NVM_SIZE_KB > 2048 is currently not supported."
#endif
};
#else
#include "flash_layout.c"
#endif /* BOOT_FLASH_CUSTOM_LAYOUT_ENABLE == 0 */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief   Local variable with information about the flash block that is currently
 *           being operated on.
 *  \details The smallest amount of flash that can be programmed is
 *           FLASH_WRITE_BLOCK_SIZE. A flash block manager is implemented in this driver
 *           and stores info in this variable. Whenever new data should be flashed, it
 *           is first added to a RAM buffer, which is part of this variable. Whenever
 *           the RAM buffer, which has the size of a flash block, is full or  data needs
 *           to be written to a different block, the contents of the RAM buffer are
 *           programmed to flash. The flash block manager requires some software
 *           overhead, yet results is faster flash programming because data is first
 *           harvested, ideally until there is enough to program an entire flash block,
 *           before the flash device is actually operated on.
 */
static tFlashBlockInfo blockInfo;

/** \brief   Local variable with information about the flash boot block.
 *  \details The first block of the user program holds the vector table, which on the
 *           STM32 is also the where the checksum is written to. Is it likely that
 *           the vector table is first flashed and then, at the end of the programming
 *           sequence, the checksum. This means that this flash block need to be written
 *           to twice. Normally this is not a problem with flash memory, as long as you
 *           write the same values to those bytes that are not supposed to be changed
 *           and the locations where you do write to are still in the erased 0xFF state.
 *           Unfortunately, writing twice to flash this way, does not work reliably on
 *           all micros. This is why we need to have an extra block, the bootblock,
 *           placed under the management of the block manager. This way is it possible
 *           to implement functionality so that the bootblock is only written to once
 *           at the end of the programming sequence.
 */
static tFlashBlockInfo bootBlockInfo;


/************************************************************************************//**
** \brief     Initializes the flash driver.
** \return    none.
**
****************************************************************************************/
void FlashInit(void)
{
  /* init the flash block info structs by setting the address to an invalid address */
  blockInfo.base_addr = FLASH_INVALID_ADDRESS;
  bootBlockInfo.base_addr = FLASH_INVALID_ADDRESS;
} /*** end of FlashInit ***/


/************************************************************************************//**
** \brief     Reinitializes the flash driver.
** \return    none.
**
****************************************************************************************/
void FlashReinit(void)
{
  /* init the flash block info structs by setting the address to an invalid address */
  blockInfo.base_addr = FLASH_INVALID_ADDRESS;
  bootBlockInfo.base_addr = FLASH_INVALID_ADDRESS;
} /*** end of FlashReinit ***/


/************************************************************************************//**
** \brief     Writes the data to flash through a flash block manager. Note that this
**            function also checks that no data is programmed outside the flash
**            memory region, so the bootloader can never be overwritten.
** \param     addr Start address.
** \param     len  Length in bytes.
** \param     data Pointer to the data buffer.
** \return    BLT_TRUE if successful, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool FlashWrite(blt_addr addr, blt_int32u len, blt_int8u *data)
{
  blt_addr base_addr;

  /* validate the len parameter */
  if ((len - 1) > (FLASH_END_ADDRESS - addr))
  {
    return BLT_FALSE;
  }

  /* make sure the addresses are within the flash device */
  if ((FlashGetSector(addr) == FLASH_INVALID_SECTOR) || \
      (FlashGetSector(addr+len-1) == FLASH_INVALID_SECTOR))
  {
    return BLT_FALSE;
  }

  /* if this is the bootblock, then let the boot block manager handle it */
  base_addr = (addr/FLASH_WRITE_BLOCK_SIZE)*FLASH_WRITE_BLOCK_SIZE;
  if (base_addr == flashLayout[0].sector_start)
  {
    /* let the boot block manager handle it */
    return FlashAddToBlock(&bootBlockInfo, addr, data, len);
  }
  /* let the block manager handle it */
  return FlashAddToBlock(&blockInfo, addr, data, len);
} /*** end of FlashWrite ***/


/************************************************************************************//**
** \brief     Erases the flash memory. Note that this function also checks that no
**            data is erased outside the flash memory region, so the bootloader can
**            never be erased.
** \param     addr Start address.
** \param     len  Length in bytes.
** \return    BLT_TRUE if successful, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool FlashErase(blt_addr addr, blt_int32u len)
{
  blt_int8u first_sector;
  blt_int8u last_sector;

  /* validate the len parameter */
  if ((len - 1) > (FLASH_END_ADDRESS - addr))
  {
    return BLT_FALSE;
  }

  /* obtain the first and last sector number */
  first_sector = FlashGetSector(addr);
  last_sector  = FlashGetSector(addr+len-1);
  /* check them */
  if ((first_sector == FLASH_INVALID_SECTOR) || (last_sector == FLASH_INVALID_SECTOR))
  {
    return BLT_FALSE;
  }
  /* erase the sectors */
  return FlashEraseSectors(first_sector, last_sector);
} /*** end of FlashErase ***/


/************************************************************************************//**
** \brief     Writes a checksum of the user program to non-volatile memory. This is
**            performed once the entire user program has been programmed. Through
**            the checksum, the bootloader can check if the programming session
**            was completed, which indicates that a valid user programming is
**            present and can be started.
** \return    BLT_TRUE if successful, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool FlashWriteChecksum(void)
{
  blt_int32u signature_checksum = 0;
  blt_int8u wordIdx;

  /* for the Tricore TC1798 target, the bootlblock is FLASH_WRITE_BLOCK_SIZE in size.
   * the actual 32-bit checksum value in this bootblock is located at:
   * <bootblock_base_addr> + 4.
   * for this reason the checksum is defined as the one's complement value of the sum
   * of everything else in the bootblock, so starting at:
   * <bootblock_base_addr> + 8 and ending at:
   * <bootblock_base_addr> + FLASH_WRITE_BLOCK_SIZE - 1;
   *
   * note that the user program need to be modified to reserve 32-bit at
   * <bootblock_base_addr> + 4, because the bootload will write the checksum value
   * here. refer to the port specific documentation for additional details.
   *
   * keep in mind that this checksum is just used as a user program signature, i.e. as
   * a flag to figure out if a user program is present or not. the checksum is not
   * calculated over the full user program size. such a checksum routine is typically
   * application/customer specific and therefore not part of the standard bootloader.
   * it can however be easily implemented by adding the following macro to blt_conf.h:
   * #define BOOT_NVM_CHECKSUM_HOOKS_ENABLE (1).
   * You can then implement your own checksum write/verify routines in the hook
   * functions NvmWriteChecksumHook() and NvmVerifyChecksumHook().
   */

  /* first check that the bootblock contains valid data. if not, this means the
   * bootblock is not part of the reprogramming this time and therefore no
   * new checksum needs to be written
   */
  if (bootBlockInfo.base_addr == FLASH_INVALID_ADDRESS)
  {
    return BLT_TRUE;
  }

#if (BOOT_FLASH_CRYPTO_HOOKS_ENABLE > 0)
  /* perform decryption of the bootblock, before calculating the checksum and writing it
   * to flash memory.
   */
  if (FlashCryptoDecryptDataHook(bootBlockInfo.data, FLASH_WRITE_BLOCK_SIZE) == BLT_FALSE)
  {
    return BLT_FALSE;
  }
#endif

  /* compute the checksum. note that the data in the checksum range is not yet written
   * to flash but is present in the bootblock data structure at this point.
   */
  for (wordIdx = 0; wordIdx < FLASH_CS_RANGE_TOTAL_WORDS; wordIdx++)
  {
    signature_checksum += *((blt_int32u *)(&bootBlockInfo.data[(wordIdx*4)+FLASH_CS_RANGE_START_OFFSET]));
  }
  signature_checksum  = ~signature_checksum; /* one's complement */

  /* write the checksum */
  return FlashWrite(flashLayout[0].sector_start+FLASH_CS_OFFSET,
                    sizeof(blt_addr), (blt_int8u *)&signature_checksum);
} /*** end of FlashWriteChecksum ***/


/************************************************************************************//**
** \brief     Verifies the checksum, which indicates that a valid user program is
**            present and can be started.
** \return    BLT_TRUE if successful, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool FlashVerifyChecksum(void)
{
  blt_int32u signature_checksum = 0;
  blt_int32u signature_checksum_rom;
  blt_int8u wordIdx;

  /* compute the checksum by reading it from flash */
  for (wordIdx = 0; wordIdx < FLASH_CS_RANGE_TOTAL_WORDS; wordIdx++)
  {
    signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start + (wordIdx*4) + FLASH_CS_RANGE_START_OFFSET));
  }
  signature_checksum  = ~signature_checksum; /* one's complement */

  /* read the checksum value from flash that was writtin by the bootloader at the end
   * of the last firmware update
   */
  signature_checksum_rom = *((blt_int32u *)(flashLayout[0].sector_start + FLASH_CS_OFFSET));
  /* verify that they are both the same */
  if (signature_checksum == signature_checksum_rom)
  {
    /* checksum okay */
    return BLT_TRUE;
  }
  /* checksum incorrect */
  return BLT_FALSE;
} /*** end of FlashVerifyChecksum ***/


/************************************************************************************//**
** \brief     Finalizes the flash driver operations. There could still be data in
**            the currently active block that needs to be flashed.
** \return    BLT_TRUE if successful, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool FlashDone(void)
{
  /* check if there is still data waiting to be programmed in the boot block */
  if (bootBlockInfo.base_addr != FLASH_INVALID_ADDRESS)
  {
    if (FlashWriteBlock(&bootBlockInfo) == BLT_FALSE)
    {
      return BLT_FALSE;
    }
  }

  /* check if there is still data waiting to be programmed */
  if (blockInfo.base_addr != FLASH_INVALID_ADDRESS)
  {
    if (FlashWriteBlock(&blockInfo) == BLT_FALSE)
    {
      return BLT_FALSE;
    }
  }
  /* still here so all is okay */
  return BLT_TRUE;
} /*** end of FlashDone ***/


/************************************************************************************//**
** \brief     Obtains the base address of the flash memory available to the user program.
**            This is basically the first address in the flashLayout table.
** \return    Base address.
**
****************************************************************************************/
blt_addr FlashGetUserProgBaseAddress(void)
{
  return flashLayout[0].sector_start;
} /*** end of FlashGetUserProgBaseAddress ***/


/************************************************************************************//**
** \brief     Copies data currently in flash to the block->data and sets the
**            base address.
** \param     block   Pointer to flash block info structure to operate on.
** \param     address Base address of the block data.
** \return    BLT_TRUE if successful, BLT_FALSE otherwise.
**
****************************************************************************************/
static blt_bool FlashInitBlock(tFlashBlockInfo *block, blt_addr address)
{
  /* check address alignment */
  if ((address % FLASH_WRITE_BLOCK_SIZE) != 0)
  {
    return BLT_FALSE;
  }
  /* make sure that we are initializing a new block and not the same one */
  if (block->base_addr == address)
  {
    /* block already initialized, so nothing to do */
    return BLT_TRUE;
  }
  /* set the base address and copies the current data from flash */
  block->base_addr = address;
  CpuMemCopy((blt_addr)block->data, address, FLASH_WRITE_BLOCK_SIZE);
  return BLT_TRUE;
} /*** end of FlashInitBlock ***/


/************************************************************************************//**
** \brief     Switches blocks by programming the current one and initializing the
**            next.
** \param     block   Pointer to flash block info structure to operate on.
** \param     base_addr Base address of the next block.
** \return    The pointer of the block info struct that is no being used, or a NULL
**            pointer in case of error.
**
****************************************************************************************/
static tFlashBlockInfo *FlashSwitchBlock(tFlashBlockInfo *block, blt_addr base_addr)
{
  /* check if a switch needs to be made away from the boot block. in this case the boot
   * block shouldn't be written yet, because this is done at the end of the programming
   * session by FlashDone(), this is right after the checksum was written.
   */
  if (block == &bootBlockInfo)
  {
    /* switch from the boot block to the generic block info structure */
    block = &blockInfo;
  }
  /* check if a switch back into the bootblock is needed. in this case the generic block
   * doesn't need to be written here yet.
   */
  else if (base_addr == flashLayout[0].sector_start)
  {
    /* switch from the generic block to the boot block info structure */
    block = &bootBlockInfo;
    base_addr = flashLayout[0].sector_start;
  }
  else
  {
    /* need to switch to a new block, so program the current one and init the next */
    if (FlashWriteBlock(block) == BLT_FALSE)
    {
      return BLT_NULL;
    }
  }

  /* initialize tne new block when necessary */
  if (FlashInitBlock(block, base_addr) == BLT_FALSE)
  {
    return BLT_NULL;
  }

  /* still here to all is okay  */
  return block;
} /*** end of FlashSwitchBlock ***/


/************************************************************************************//**
** \brief     Programming is done per block. This function adds data to the block
**            that is currently collecting data to be written to flash. If the
**            address is outside of the current block, the current block is written
**            to flash an a new block is initialized.
** \param     block   Pointer to flash block info structure to operate on.
** \param     address Flash destination address.
** \param     data    Pointer to the byte array with data.
** \param     len     Number of bytes to add to the block.
** \return    BLT_TRUE if successful, BLT_FALSE otherwise.
**
****************************************************************************************/
static blt_bool FlashAddToBlock(tFlashBlockInfo *block, blt_addr address,
                                blt_int8u *data, blt_int32u len)
{
  blt_addr   current_base_addr;
  blt_int8u  *dst;
  blt_int8u  *src;

  /* determine the current base address */
  current_base_addr = (address/FLASH_WRITE_BLOCK_SIZE)*FLASH_WRITE_BLOCK_SIZE;

  /* make sure the blockInfo is not uninitialized */
  if (block->base_addr == FLASH_INVALID_ADDRESS)
  {
    /* initialize the blockInfo struct for the current block */
    if (FlashInitBlock(block, current_base_addr) == BLT_FALSE)
    {
      return BLT_FALSE;
    }
  }

  /* check if the new data fits in the current block */
  if (block->base_addr != current_base_addr)
  {
    /* need to switch to a new block, so program the current one and init the next */
    block = FlashSwitchBlock(block, current_base_addr);
    if (block == BLT_NULL)
    {
      return BLT_FALSE;
    }
  }

  /* add the data to the current block, but check for block overflow */
  dst = &(block->data[address - block->base_addr]);
  src = data;
  do
  {
    /* keep the watchdog happy */
    CopService();
    /* buffer overflow? */
    if ((blt_addr)(dst-&(block->data[0])) >= FLASH_WRITE_BLOCK_SIZE)
    {
      /* need to switch to a new block, so program the current one and init the next */
      block = FlashSwitchBlock(block, current_base_addr+FLASH_WRITE_BLOCK_SIZE);
      if (block == BLT_NULL)
      {
        return BLT_FALSE;
      }
      /* reset destination pointer */
      dst = &(block->data[0]);
    }
    /* write the data to the buffer */
    *dst = *src;
    /* update pointers */
    dst++;
    src++;
    /* decrement byte counter */
    len--;
  }
  while (len > 0);
  /* still here so all is good */
  return BLT_TRUE;
} /*** end of FlashAddToBlock ***/


/************************************************************************************//**
** \brief     Programs FLASH_WRITE_BLOCK_SIZE bytes to flash from the block->data
**            array.
** \param     block   Pointer to flash block info structure to operate on.
** \return    BLT_TRUE if successful, BLT_FALSE otherwise.
**
****************************************************************************************/
static blt_bool FlashWriteBlock(tFlashBlockInfo *block)
{
  blt_int8u  sector_num;

  /* check that address is actually within flash */
  sector_num = FlashGetSector(block->base_addr);
  if (sector_num == FLASH_INVALID_SECTOR)
  {
    return BLT_FALSE;
  }

#if (BOOT_FLASH_CRYPTO_HOOKS_ENABLE > 0)
  #if (BOOT_NVM_CHECKSUM_HOOKS_ENABLE == 0)
  /* note that the bootblock is already decrypted in FlashWriteChecksum(), if the
   * internal checksum mechanism is used. Therefore don't decrypt it again.
   */
  if (block != &bootBlockInfo)
  #endif
  {
    /* perform decryption of the program data before writing it to flash memory. */
    if (FlashCryptoDecryptDataHook(block->data, FLASH_WRITE_BLOCK_SIZE) == BLT_FALSE)
    {
      return BLT_FALSE;
    }
  }
#endif

  /* the FLASH_WRITE_BLOCK_SIZE is configured to exactly match the size of a page in
   * PFLASH. so here simply need to program one page in PFLASH.
   */
  return FlashTricoreProgramPage(block->base_addr, block->data);
} /*** end of FlashWriteBlock ***/


/************************************************************************************//**
** \brief     Erases the flash sectors from first_sector up until last_sector.
** \param     first_sector First flash sector number.
** \param     last_sector  Last flash sector number.
** \return    BLT_TRUE if successful, BLT_FALSE otherwise.
**
****************************************************************************************/
static blt_bool FlashEraseSectors(blt_int8u first_sector, blt_int8u last_sector)
{
  blt_int8u current_sector;
  blt_bool result = BLT_TRUE;

  /* validate the sector numbers */
  if (first_sector > last_sector)
  {
    return BLT_FALSE;
  }
  if ((first_sector < flashLayout[0].sector_num) || \
      (last_sector > flashLayout[FLASH_TOTAL_SECTORS-1].sector_num))
  {
    return BLT_FALSE;
  }
  /* the table flashLayout[] is implemented such that it exactly matches the sectors
   * in PFLASH. this means that here we simply need to loop through the sectors one-
   * by-one and erase them.
   */
  for (current_sector = first_sector; current_sector <= last_sector; current_sector++)
  {
    if (FlashTricoreEraseSector(FlashGetSectorBaseAddr(current_sector)) == BLT_FALSE)
    {
      /* flag error and stop the loop */
      result = BLT_FALSE;
      break;
    }
    /* keep the watchdog happy */
    CopService();
  }
  /* return the result */
  return result;
} /*** end of FlashEraseSectors ***/


/************************************************************************************//**
** \brief     Determines the flash sector the address is in.
** \param     address Address in the flash sector.
** \return    Flash sector number or FLASH_INVALID_SECTOR.
**
****************************************************************************************/
static blt_int8u FlashGetSector(blt_addr address)
{
  blt_int8u sectorIdx;

  /* search through the sectors to find the right one */
  for (sectorIdx = 0; sectorIdx < FLASH_TOTAL_SECTORS; sectorIdx++)
  {
    /* keep the watchdog happy */
    CopService();
    /* is the address in this sector? */
    if ((address >= flashLayout[sectorIdx].sector_start) && \
        (address < (flashLayout[sectorIdx].sector_start + \
                    flashLayout[sectorIdx].sector_size)))
    {
      /* return the sector number */
      return flashLayout[sectorIdx].sector_num;
    }
  }
  /* still here so no valid sector found */
  return FLASH_INVALID_SECTOR;
} /*** end of FlashGetSector ***/


/************************************************************************************//**
** \brief     Determines the flash sector base address.
** \param     sector Sector to get the base address of.
** \return    Flash sector base address or FLASH_INVALID_ADDRESS.
**
****************************************************************************************/
static blt_addr FlashGetSectorBaseAddr(blt_int8u sector)
{
  blt_int8u sectorIdx;

  /* search through the sectors to find the right one */
  for (sectorIdx = 0; sectorIdx < FLASH_TOTAL_SECTORS; sectorIdx++)
  {
    /* keep the watchdog happy */
    CopService();
    if (flashLayout[sectorIdx].sector_num == sector)
    {
      return flashLayout[sectorIdx].sector_start;
    }
  }
  /* still here so no valid sector found */
  return FLASH_INVALID_ADDRESS;
} /*** end of FlashGetSectorBaseAddr ***/


/************************************************************************************//**
** \brief     Determines the flash sector size.
** \param     sector Sector to get the size of.
** \return    Flash sector size or 0.
**
****************************************************************************************/
static blt_int32u FlashGetSectorSize(blt_int8u sector)
{
  blt_int8u sectorIdx;

  /* search through the sectors to find the right one */
  for (sectorIdx = 0; sectorIdx < FLASH_TOTAL_SECTORS; sectorIdx++)
  {
    /* keep the watchdog happy */
    CopService();
    if (flashLayout[sectorIdx].sector_num == sector)
    {
      return flashLayout[sectorIdx].sector_size;
    }
  }
  /* still here so no valid sector found */
  return 0;
} /*** end of FlashGetSectorSize ***/


/************************************************************************************//**
** \brief     Programs FLASH_WRITE_BLOCK_SIZE bytes into flash starting at the page's base
**            address.
** \param     start_addr Starting address of the page where the bytes should be
**            programmed. Should be aligned to FLASH_WRITE_BLOCK_SIZE.
** \param     data Pointer to a byte array with the data to be programmed. The array
**            should have FLASH_WRITE_BLOCK_SIZE bytes.
** \return    BLT_TRUE is the page was programmed successfully, BLT_FALSE otherwise.
**
****************************************************************************************/
static blt_bool FlashTricoreProgramPage(blt_addr start_addr, blt_int8u *data)
{
  blt_addr baseAddr;
  blt_int32u *dataPtr;
  blt_int8u *readPtr;
  blt_int32u idx;
  FLASHn_FSR_t *pflashFSR;
  blt_int32u timeout;

  /* check address alignment to a page in PFLASH */
  if ((start_addr % FLASH_WRITE_BLOCK_SIZE) != 0)
  {
    return BLT_FALSE;
  }
  /* determine base address of the PFLASH module */
  baseAddr = FLASH_GET_PFLASH_BASE(start_addr);
  /* set pointer for the PFLASH module's FSR register */
  pflashFSR = (FLASHn_FSR_t *)FLASH_GET_FSR_REG_ADDR(start_addr);
  /* use "clear status" command to clear flags */
  FLASH_WRITE_TO_U32_PTR_BY_ADDR(baseAddr + 0x5554u, 0x000000F5u);
  /* execute "enter page mode" command to activate the PFLASH assembly buffer */
  FLASH_WRITE_TO_U32_PTR_BY_ADDR(baseAddr + 0x5554u, 0x00000050u);
  /* perform DSYNC */
  CpuSetDSYNC();
  /* set timeout time for hardware handshake */
  timeout = TimerGet() + FLASH_PROGRAM_TIME_MAX_MS;
  /* wait until FSR.xFPAGE = '1' */
  while (pflashFSR->bits.PFPAGE != 1)
  {
    /* fail if FSR.SQER = '1' */
    if (pflashFSR->bits.SQER == 1)
    {
      return BLT_FALSE;
    }
    /* fail if FSR.PROER = '1' */
    if (pflashFSR->bits.PROER == 1)
    {
      return BLT_FALSE;
    }
    /* keep the watchdog happy */
    CopService();
    /* fail in case of timeout */
    if (TimerGet() > timeout)
    {
      return BLT_FALSE;
    }
  }
  /* load FLASH_WRITE_BLOCK_SIZE bytes of program data into the assembly buffer */
  dataPtr = (blt_int32u *)data;
  for (idx = 0; idx <(FLASH_WRITE_BLOCK_SIZE/8u); idx++)
  {
    /* write first 32-bit value */
    FLASH_WRITE_TO_U32_PTR_BY_ADDR(baseAddr + 0x55F0U, *dataPtr);
    dataPtr++;
    /* write second 32-bit value */
    FLASH_WRITE_TO_U32_PTR_BY_ADDR(baseAddr + 0x55F4U, *dataPtr);
    dataPtr++;
  }
  /* launch the "write page" command */
  FLASH_WRITE_TO_U32_PTR_BY_ADDR(baseAddr + 0x5554u, 0x000000AAu);
  FLASH_WRITE_TO_U32_PTR_BY_ADDR(baseAddr + 0xAAA8u, 0x00000055u);
  FLASH_WRITE_TO_U32_PTR_BY_ADDR(baseAddr + 0x5554u, 0x000000A0u);
  FLASH_WRITE_TO_U32_PTR_BY_ADDR(start_addr, 0x000000AAu);
  /* perform DSYNC */
  CpuSetDSYNC();
  /* set timeout time for hardware handshake */
  timeout = TimerGet() + FLASH_PROGRAM_TIME_MAX_MS;
  /* wait until FSR.PROG = '1' */
  while (pflashFSR->bits.PROG != 1)
  {
    /* fail if FSR.SQER = '1' */
    if (pflashFSR->bits.SQER == 1)
    {
      return BLT_FALSE;
    }
    /* fail if FSR.PROER = '1' */
    if (pflashFSR->bits.PROER == 1)
    {
      return BLT_FALSE;
    }
    /* keep the watchdog happy */
    CopService();
    /* fail in case of timeout */
    if (TimerGet() > timeout)
    {
      return BLT_FALSE;
    }
  }
  /* set timeout time for hardware handshake */
  timeout = TimerGet() + FLASH_PROGRAM_TIME_MAX_MS;
  /* wait until FSR.xBUSY = '0' */
  while (pflashFSR->bits.PBUSY == 1)
  {
    /* check flag FSR.xFOPER for ‘1’ as abort criterion to protect against hardware
     * failures causing BUSY to stay '1'
     */
    if (pflashFSR->bits.PFOPER == 1)
    {
      return BLT_FALSE;
    }
    /* keep the watchdog happy */
    CopService();
    /* fail in case of timeout */
    if (TimerGet() > timeout)
    {
      return BLT_FALSE;
    }
  }
  /* check FSR.VER flag */
  if (pflashFSR->bits.VER != 0)
  {
    return BLT_FALSE;
  }
  /* fail if FSR.xFOPER = '1' */
  if (pflashFSR->bits.PFOPER != 0)
  {
    return BLT_FALSE;
  }
  /* evaluate FSR.xDBER */
  if (pflashFSR->bits.PFDBER != 0)
  {
    return BLT_FALSE;
  }
  /* use "clear status" command to clear flags */
  FLASH_WRITE_TO_U32_PTR_BY_ADDR(baseAddr + 0x5554u, 0x000000F5u);
  /* perform verification by checking the written values. do this on a byte-per-byte
   * basis to also check the code for byte swapping mistakes.
   */
  readPtr = (blt_int8u *)start_addr;
  for (idx = 0; idx <FLASH_WRITE_BLOCK_SIZE; idx++)
  {
    /* verify 32-bits at a time */
    if (*readPtr != data[idx])
    {
      return BLT_FALSE;
    }
    /* increment pointer */
    readPtr++;
  }
  /* still here so programming was completed successfully */
  return BLT_TRUE;
} /*** end of FlashTricoreProgramPage ***/


/************************************************************************************//**
** \brief     Erases a sector in PFLASH. The sector is identified using its start
**            address.
** \param     start_addr Starting address of the sector that is to be erased.
** \return    BLT_TRUE is the sector was successfully erased, BLT_FALSE otherwise.
**
****************************************************************************************/
static blt_bool FlashTricoreEraseSector(blt_addr start_addr)
{
  blt_addr baseAddr;
  FLASHn_FSR_t *pflashFSR;
  blt_int32u sectorWords;
  blt_int8u sectorNum;
  blt_int32u *readPtr;
  blt_int32u idx;
  blt_int32u timeout;

  /* determine base address of the PFLASH module */
  baseAddr = FLASH_GET_PFLASH_BASE(start_addr);
  /* set pointer for the PFLASH module's FSR register */
  pflashFSR = (FLASHn_FSR_t *)FLASH_GET_FSR_REG_ADDR(start_addr);
  /* use "clear status" command to clear flags */
  FLASH_WRITE_TO_U32_PTR_BY_ADDR(baseAddr + 0x5554u, 0x000000F5u);
  /* issue "erase sector" command */
  FLASH_WRITE_TO_U32_PTR_BY_ADDR(baseAddr + 0x5554u, 0x000000AAu);
  FLASH_WRITE_TO_U32_PTR_BY_ADDR(baseAddr + 0xAAA8u, 0x00000055u);
  FLASH_WRITE_TO_U32_PTR_BY_ADDR(baseAddr + 0x5554u, 0x00000080u);
  FLASH_WRITE_TO_U32_PTR_BY_ADDR(baseAddr + 0x5554u, 0x000000AAu);
  FLASH_WRITE_TO_U32_PTR_BY_ADDR(baseAddr + 0xAAA8u, 0x00000055u);
  FLASH_WRITE_TO_U32_PTR_BY_ADDR(start_addr, 0x00000030u);
  /* perform DSYNC */
  CpuSetDSYNC();
  /* set timeout time for hardware handshake */
  timeout = TimerGet() + FLASH_ERASE_TIME_MAX_MS;
  /* wait until FSR.ERASE = '1' */
  while (pflashFSR->bits.ERASE != 1)
  {
    /* fail if FSR.SQER = '1' */
    if (pflashFSR->bits.SQER == 1)
    {
      return BLT_FALSE;
    }
    /* fail if FSR.PROER = '1' */
    if (pflashFSR->bits.PROER == 1)
    {
      return BLT_FALSE;
    }
    /* keep the watchdog happy */
    CopService();
    /* fail in case of timeout */
    if (TimerGet() > timeout)
    {
      return BLT_FALSE;
    }
  }
  /* set timeout time for hardware handshake */
  timeout = TimerGet() + FLASH_ERASE_TIME_MAX_MS;
  /* wait until FSR.xBUSY = '0' */
  while (pflashFSR->bits.PBUSY == 1)
  {
    /* check flag FSR.xFOPER for ‘1’ as abort criterion to protect against hardware
     * failures causing BUSY to stay '1'
     */
    if (pflashFSR->bits.PFOPER == 1)
    {
      return BLT_FALSE;
    }
    /* keep the watchdog happy */
    CopService();
    /* fail in case of timeout */
    if (TimerGet() > timeout)
    {
      return BLT_FALSE;
    }
  }
  /* check FSR.VER flag */
  if (pflashFSR->bits.VER != 0)
  {
    return BLT_FALSE;
  }
  /* fail if FSR.xFOPER = '1' */
  if (pflashFSR->bits.PFOPER != 0)
  {
    return BLT_FALSE;
  }
  /* use "clear status" command to clear flags */
  FLASH_WRITE_TO_U32_PTR_BY_ADDR(baseAddr + 0x5554u, 0x000000F5u);

  /* perform erase verification */
  sectorNum = FlashGetSector(start_addr);
  if (sectorNum == FLASH_INVALID_SECTOR)
  {
    /* should not happen */
    return BLT_FALSE;
  }
  /* get sector size in words and set the read pointer to the first word in the sector */
  sectorWords = FlashGetSectorSize(sectorNum) / sizeof(blt_int32u);
  if (sectorWords == 0)
  {
    /* should not happen */
    return BLT_FALSE;
  }
  readPtr = (blt_int32u *)(FlashGetSectorBaseAddr(sectorNum));
  /* loop through all words in the sector and check that they are in the erase state.
   * note that this is a 0 value for PFLASH.
   */
  for (idx=0; idx<sectorWords; idx++)
  {
    if (*readPtr != 0)
    {
      /* not in the erased state */
      return BLT_FALSE;
    }
    /* continue with the next word */
    readPtr++;
    /* keep the watchdog happy. this is a fast loop so no need to do it every iteration */
    if ((idx % 100) == 0)
    {
      CopService();
    }
  }
  /* still here so erase operation was completed successfully */
  return BLT_TRUE;
} /*** end of FlashTricoreEraseSector ***/


/*********************************** end of flash.c ************************************/
