/************************************************************************************//**
* \file         Source/ARMCM4_XMC4/flash.c
* \brief        Bootloader flash driver source file.
* \ingroup      Target_ARMCM4_XMC4
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

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */
#include "xmc_flash.h"                           /* Flash driver header                */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Value for an invalid flash sector. */
#define FLASH_INVALID_SECTOR            (0xff)
/** \brief Value for an invalid flash address. */
#define FLASH_INVALID_ADDRESS           (0xffffffff)
/** \brief Standard size of a flash block for writing. It should be large enough so that
 *         the OpenBLT checksum fits in the first (boot) block)
 */
#define FLASH_WRITE_BLOCK_SIZE          (1024)
/** \brief Total numbers of sectors in array flashLayout[]. */
#define FLASH_TOTAL_SECTORS             (sizeof(flashLayout)/sizeof(flashLayout[0]))
/** \brief End address of the bootloader programmable flash. */
#define FLASH_END_ADDRESS               (flashLayout[FLASH_TOTAL_SECTORS-1].sector_start + \
                                         flashLayout[FLASH_TOTAL_SECTORS-1].sector_size - 1)
/** \brief Offset into the user program's vector table where the checksum is located.
 *         For this target it is set to the end of the vector table. Note that the
 *         value can be overriden in blt_conf.h, because the size of the vector table
 *         could vary. When changing this value, don't forget to update the location
 *         of the checksum in the user program accordingly. Otherwise the checksum
 *         verification will always fail.
 */
#ifndef BOOT_FLASH_VECTOR_TABLE_CS_OFFSET
#define BOOT_FLASH_VECTOR_TABLE_CS_OFFSET    (0x200)
#endif

/** \brief Minimum amount of bytes that can be programmed to flash at a time. It is
 *         hardware dependent.
 */
#define FLASH_WRITE_PAGE_SIZE           (256)

/** \brief Base address in the memory map for uncached flash. It is hardware dependent.
 */
#define FLASH_UNCACHED_BASE_ADDR        (0x0C000000U)

/** \brief Base address in the memory map for cached flash. It is hardware dependent.
 */
#define FLASH_CACHED_BASE_ADDR          (0x08000000U)

/** \brief Maximum time for a sector erase operation as specified by the XCM4xxx data-
 *         sheet with an added 20% margin.
 */
#define FLASH_ERASE_TIME_MAX_MS         (6600)

/** \brief Maximum time for a page program operation as specified by the XCM4xxx data-
 *         sheet with an added 20% margin.
 */
#define FLASH_PROGRAM_TIME_MAX_MS       (13)


/****************************************************************************************
* Plausibility checks
****************************************************************************************/
#if (BOOT_FLASH_VECTOR_TABLE_CS_OFFSET >= FLASH_WRITE_BLOCK_SIZE)
#error "BOOT_FLASH_VECTOR_TABLE_CS_OFFSET is set too high. It must be located in the first writable block."
#endif

#ifndef BOOT_FLASH_CUSTOM_LAYOUT_ENABLE
#define BOOT_FLASH_CUSTOM_LAYOUT_ENABLE (0u)
#endif

#if ((FLASH_WRITE_BLOCK_SIZE % FLASH_WRITE_PAGE_SIZE) != 0)
#error "FLASH_WRITE_BLOCK_SIZE must be a multiple of FLASH_WRITE_PAGE_SIZE."
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
static blt_bool  FlashInitBlock(tFlashBlockInfo *block, blt_addr address);
static tFlashBlockInfo *FlashSwitchBlock(tFlashBlockInfo *block, blt_addr base_addr);
static blt_bool  FlashAddToBlock(tFlashBlockInfo *block, blt_addr address,
                                 blt_int8u *data, blt_int32u len);
static blt_bool  FlashWriteBlock(tFlashBlockInfo *block);
static blt_bool  FlashEraseSectors(blt_int8u first_sector, blt_int8u last_sector);
static blt_int8u FlashGetSector(blt_addr address);
static blt_addr  FlashGetSectorBaseAddr(blt_int8u sector);
static blt_addr  FlashTranslateToNonCachedAddress(blt_addr address);


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
 *  \details Also controls what part of the flash memory is reserved for the bootloader.
 *           If the bootloader size changes, the reserved sectors for the bootloader
 *           might need adjustment to make sure the bootloader doesn't get overwritten.
 *           Note that the table contains uncached addresses, because flash program/
 *           erase operations need to be performed on uncached addresses. This flash
 *           driver automatically translated cached to uncached addresses, so there
 *           is no need for the user to adjust this when calling this driver's API.
 */
static const tFlashSector flashLayout[] =
{
  /* the space reserved for the bootloader might need updating after changing the
   * configuration. enough space should be reserved so that the entire ROM code of
   * the bootloader fits in it. this is needed to protect the bootloader from being
   * overwritten during a firmware update.
   */
  /* { 0x0c000000, 0x04000,  0},           flash sector  0 - reserved for bootloader   */
  /* { 0x0c004000, 0x04000,  1},           flash sector  1 - reserved for bootloader   */
  /* { 0x0c008000, 0x04000,  2},           flash sector  2 - reserved for bootloader   */
  { 0x0c00c000, 0x04000,  3},           /* flash sector  3 -  16kb                     */
  { 0x0c010000, 0x04000,  4},           /* flash sector  4 -  16kb                     */
  { 0x0c014000, 0x04000,  5},           /* flash sector  5 -  16kb                     */
  { 0x0c018000, 0x04000,  6},           /* flash sector  6 -  16kb                     */
  { 0x0c01c000, 0x04000,  7},           /* flash sector  7 -  16kb                     */
  { 0x0c020000, 0x20000,  8},           /* flash sector  8 -  128kb                    */
#if (BOOT_NVM_SIZE_KB > 256)
  { 0x0c040000, 0x40000,  9},           /* flash sector  9 -  256kb                    */
#endif
#if (BOOT_NVM_SIZE_KB > 512)
  { 0x0c080000, 0x40000, 10},           /* flash sector 10 -  256kb                    */
  { 0x0c0C0000, 0x40000, 11},           /* flash sector 11 -  256kb                    */
#endif
#if (BOOT_NVM_SIZE_KB > 1024)
  { 0x0c100000, 0x40000, 12},           /* flash sector 12 -  256kb                    */
  { 0x0c140000, 0x40000, 13},           /* flash sector 13 -  256kb                    */
  { 0x0c180000, 0x40000, 14},           /* flash sector 14 -  256kb                    */
  { 0x0c1C0000, 0x40000, 15},           /* flash sector 15 -  256kb                    */
#endif
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

  /* automatically translate cached memory addresses to non-cached */
  addr = FlashTranslateToNonCachedAddress(addr);

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

  /* automatically translate cached memory addresses to non-cached */
  addr = FlashTranslateToNonCachedAddress(addr);

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

  /* for the XMC4 target we defined the checksum as the One's complement value of the
   * sum of the first 7 exception addresses.
   *
   * Layout of the vector table:
   *    0x0c000000 Initial stack pointer
   *    0x0c000004 Reset Handler
   *    0x0c000008 NMI Handler
   *    0x0c00000C Hard Fault Handler
   *    0x0c000010 MPU Fault Handler
   *    0x0c000014 Bus Fault Handler
   *    0x0c000018 Usage Fault Handler
   *
   *    signature_checksum = One's complement of (SUM(exception address values))
   *
   *    the bootloader writes this 32-bit checksum value right after the vector table
   *    of the user program. note that this means one extra dummy entry must be added
   *    at the end of the user program's vector table to reserve storage space for the
   *    checksum.
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

  /* compute the checksum. note that the user program's vectors are not yet written
   * to flash but are present in the bootblock data structure at this point.
   */
  signature_checksum += *((blt_int32u *)(&bootBlockInfo.data[0+0x00]));
  signature_checksum += *((blt_int32u *)(&bootBlockInfo.data[0+0x04]));
  signature_checksum += *((blt_int32u *)(&bootBlockInfo.data[0+0x08]));
  signature_checksum += *((blt_int32u *)(&bootBlockInfo.data[0+0x0C]));
  signature_checksum += *((blt_int32u *)(&bootBlockInfo.data[0+0x10]));
  signature_checksum += *((blt_int32u *)(&bootBlockInfo.data[0+0x14]));
  signature_checksum += *((blt_int32u *)(&bootBlockInfo.data[0+0x18]));
  signature_checksum  = ~signature_checksum; /* one's complement */

  /* write the checksum */
  return FlashWrite(flashLayout[0].sector_start+BOOT_FLASH_VECTOR_TABLE_CS_OFFSET,
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

  /* verify the checksum based on how it was written by CpuWriteChecksum() */
  signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start));
  signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+0x04));
  signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+0x08));
  signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+0x0C));
  signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+0x10));
  signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+0x14));
  signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+0x18));
  signature_checksum = ~signature_checksum; /* one's complement */

  /* read the checksum value from flash that was writtin by the bootloader at the end
   * of the last firmware update
   */
  signature_checksum_rom = *((blt_int32u *)(flashLayout[0].sector_start+BOOT_FLASH_VECTOR_TABLE_CS_OFFSET));

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
  blt_int32u page_cnt;
  blt_addr   page_addr;
  blt_int8u *page_data;
  blt_int32u status;
  blt_int32u timeoutTime;
  blt_int32u byteIdx;

  /* check that address is actually within flash */
  if (FlashGetSector(block->base_addr) == FLASH_INVALID_SECTOR)
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

  /* program all pages in the block one by one */
  for (page_cnt=0; page_cnt< (FLASH_WRITE_BLOCK_SIZE/FLASH_WRITE_PAGE_SIZE); page_cnt++)
  {
    /* keep the watchdog happy */
    CopService();
    /* set page base address and pointer to page data */
    page_addr = block->base_addr + (page_cnt * FLASH_WRITE_PAGE_SIZE);
    page_data = &(block->data[page_cnt * FLASH_WRITE_PAGE_SIZE]);
    /* determine timeout time of the operation */
    timeoutTime = TimerGet() + FLASH_PROGRAM_TIME_MAX_MS;
    /* start erase operation */
    XMC_FLASH_ProgramPage((uint32_t *)page_addr, (uint32_t *)page_data);
    /* wait for the flash operation to complete */
    while (XMC_FLASH_IsBusy() > 0)
    {
      /* check for operation timeout */
      if (TimerGet() > timeoutTime)
      {
        /* timeout occurred. cannot continue */
        return BLT_FALSE;
      }
      /* keep the watchdog happy */
      CopService();
    }
    /* check the result */
    status = XMC_FLASH_GetStatus();
    /* reset the program finished flag */
    status &= ~XMC_FLASH_STATUS_PROGRAMMING_STATE;
    if (status != XMC_FLASH_STATUS_OK)
    {
      /* error occurred during flash erase, abort */
      return BLT_FALSE;

    }
    /* verify that the data was correctly programmed */
    for (byteIdx=0; byteIdx < FLASH_WRITE_PAGE_SIZE; byteIdx++)
    {
      if (*((volatile blt_int8u *)(page_addr+byteIdx)) != page_data[byteIdx])
      {
        /* program verification failed. abort */
        return BLT_FALSE;
      }
    }
  }

  /* still here so all is okay */
  return BLT_TRUE;
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
  blt_int8u sector_cnt;
  blt_addr sectorBaseAddr;
  blt_int32u status;
  blt_int32u timeoutTime;

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

  /* erase all sectors one by one */
  for (sector_cnt=first_sector; sector_cnt<= last_sector; sector_cnt++)
  {
    /* keep the watchdog happy */
    CopService();

    /* submit the sector erase request by specifying its start address */
    sectorBaseAddr = FlashGetSectorBaseAddr(sector_cnt);
    if (sectorBaseAddr == FLASH_INVALID_ADDRESS)
    {
      /* not a valid sector address so abort */
      return BLT_FALSE;
    }
    /* determine timeout time of the operation */
    timeoutTime = TimerGet() + FLASH_ERASE_TIME_MAX_MS;
    /* start erase operation */
    XMC_FLASH_EraseSector((uint32_t *)sectorBaseAddr);
    /* wait for the flash operation to complete */
    while (XMC_FLASH_IsBusy() > 0)
    {
      /* check for operation timeout */
      if (TimerGet() > timeoutTime)
      {
        /* timeout occurred. cannot continue */
        return BLT_FALSE;
      }
      /* keep the watchdog happy */
      CopService();
    }
    /* check the result */
    status = XMC_FLASH_GetStatus();
    /* reset the erase finished flag */
    status &= ~XMC_FLASH_STATUS_ERASE_STATE;
    if (status != XMC_FLASH_STATUS_OK)
    {
      /* error occurred during flash erase, abort */
      return BLT_FALSE;

    }
  }

  /* still here so all went okay */
  return BLT_TRUE;
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
** \brief     Obtains the base address of the specified sector.
** \param     sector Sector to get the base address of.
** \return    Base Base address of the sector if found, FLASH_INVALID_ADDRESS otherwise.
**
****************************************************************************************/
static blt_addr FlashGetSectorBaseAddr(blt_int8u sector)
{
  blt_int8u sectorIdx;
  blt_addr baseAddr;

  /* initialize base address to invalid */
  baseAddr = FLASH_INVALID_ADDRESS;

  /* search through the sectors to find the right one */
  for (sectorIdx = 0; sectorIdx < FLASH_TOTAL_SECTORS; sectorIdx++)
  {
    /* keep the watchdog happy */
    CopService();
    /* is this the sector that was specified? */
    if (flashLayout[sectorIdx].sector_num == sector)
    {
      /* read out its base address and stop the loop */
      baseAddr = flashLayout[sectorIdx].sector_start;
      break;
    }
  }
  /* return the results */
  return baseAddr;
} /*** end of FlashGetSectorBaseAddr ***/


/************************************************************************************//**
** \brief     The XMC4xxx has its PFLASH accessible in the memory map in two regions.
**            One is the non-cached region starting at FLASH_UNCACHED_BASE_ADDR and the
**            other is the cached region starting at FLASH_CACHED_BASE_ADDR. Flash
**            erase and programming operations need to operate on addresses in the
**            non-cached region. It is possible that the caller of this driver's API
**            functions, specifies memory addresses in the cached region. This function
**            automatically translates the memory address from cached to non-cached.
** \param     address Address to translate.
** \return    Translated address.
**
****************************************************************************************/
static blt_addr FlashTranslateToNonCachedAddress(blt_addr address)
{
  blt_addr translatedAddr;

  /* initialize local */
  translatedAddr = address;

  /* determine is this address is in the cached region by looking at the address' MSB */
  if ( ((address >> 24) & 0x000000ffu) == ((FLASH_CACHED_BASE_ADDR >> 24) & 0x000000ffu) )
  {
    /* translate address by adding offset to the non-cached region */
    translatedAddr += (FLASH_UNCACHED_BASE_ADDR - FLASH_CACHED_BASE_ADDR);
  }

  /* give back the translated address */
  return translatedAddr;
} /*** end of FlashTranslateToNonCachedAddress ***/


/*********************************** end of flash.c ************************************/
