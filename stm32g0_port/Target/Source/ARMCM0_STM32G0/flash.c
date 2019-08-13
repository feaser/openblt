/************************************************************************************//**
* \file         Source/ARMCM0_STM32G0/flash.c
* \brief        Bootloader flash driver source file.
* \ingroup      Target_ARMCM0_STM32G0
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2019  by Feaser    http://www.feaser.com    All rights reserved
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
#include "stm32g0xx.h"                           /* STM32 CPU and HAL header           */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Value for an invalid sector number. */
#define FLASH_INVALID_SECTOR            (0xff)
/** \brief Value for an invalid flash address. */
#define FLASH_INVALID_ADDRESS           (0xffffffff)
/** \brief Standard size of a flash block for writing. */
#define FLASH_WRITE_BLOCK_SIZE          (512)
/** \brief Total numbers of sectors in array flashLayout[]. */
#define FLASH_TOTAL_SECTORS             (sizeof(flashLayout)/sizeof(flashLayout[0]))
/** \brief Mask for low-order bits of flash address range */
#define FLASH_ADDRESS_MASK              (0x00ffffffu)
/** \brief Start address of the bootloader programmable flash. */
#define FLASH_START_ADDRESS             (flashLayout[0].sector_start & ~FLASH_ADDRESS_MASK)
/** \brief End address of the bootloader programmable flash. */
#define FLASH_END_ADDRESS               (flashLayout[FLASH_TOTAL_SECTORS-1].sector_start + \
                                         flashLayout[FLASH_TOTAL_SECTORS-1].sector_size - 1)
/** \brief Hardware erase unit size (sectors must be multiples of this) */
#define FLASH_ERASE_BLOCK_SIZE          (0x800)
/** \brief Flash page (erase unit) number */
#define FLASH_ERASE_BLOCK_NUM(adrs)     (((adrs) - FLASH_START_ADDRESS) / FLASH_ERASE_BLOCK_SIZE)

/** \brief Offset into the user program's vector table where the checksum is located. 
 *         For this target it is set to the end of the vector table. Note that the 
 *         value can be overriden in blt_conf.h, because the size of the vector table
 *         could vary. When changing this value, don't forget to update the location
 *         of the checksum in the user program accordingly. Otherwise the checksum
 *         verification will always fail.
 */
#ifndef BOOT_FLASH_VECTOR_TABLE_CS_OFFSET
#define BOOT_FLASH_VECTOR_TABLE_CS_OFFSET    (0xBC)
#endif


/****************************************************************************************
* Plausibility checks
****************************************************************************************/
#if (BOOT_FLASH_VECTOR_TABLE_CS_OFFSET >= FLASH_WRITE_BLOCK_SIZE)
#error "BOOT_FLASH_VECTOR_TABLE_CS_OFFSET is set too high. It must be located in the first writable block."
#endif

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
static blt_bool  FlashInitBlock(tFlashBlockInfo *block, blt_addr address);
static tFlashBlockInfo *FlashSwitchBlock(tFlashBlockInfo *block, blt_addr base_addr);
static blt_bool  FlashAddToBlock(tFlashBlockInfo *block, blt_addr address,
                                 blt_int8u *data, blt_int32u len);
static blt_bool  FlashWriteBlock(tFlashBlockInfo *block);
static blt_bool  FlashEraseSectors(blt_int8u first_sector, blt_int8u last_sector);
static blt_int8u FlashGetSector(blt_addr address);
static blt_addr  FlashGetSectorBaseAddr(blt_int8u sector);
static blt_addr  FlashGetSectorSize(blt_int8u sector);


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
/** \brief   Array with the layout of the flash memory.
 *  \details Also controls what part of the flash memory is reserved for the bootloader.
 *           If the bootloader size changes, the reserved sectors for the bootloader
 *           might need adjustment to make sure the bootloader doesn't get overwritten.
 *           The current flash layout does not reflect the minimum sector size of the
 *           physical flash (2 KiB), because this would make the table quite long and
 *           a waste of ROM. The minimum sector size is only really needed when erasing
 *           the flash. This can still be done in combination with macro
 *           FLASH_ERASE_BLOCK_SIZE.
 *           Note that the term sector here is used in a different meaning than in the
 *           controller's reference manual.
 *
 */
static const tFlashSector flashLayout[] =
{
  /* space is reserved for the bootloader configuration of the demo programs. it might
   * grow/shrink depending when the bootloader is reconfigured and or customized. make
   * sure to update the reserved space for the bootloader here in that case, as well as
   * the start address of the user program in its linker command script.
   */
  /* { 0x08000000, 0x00800,  0},           flash sector  0 - reserved for bootloader   */
  /* { 0x08000800, 0x00800,  1},           flash sector  1 - reserved for bootloader   */
  /* { 0x08001000, 0x00800,  2},           flash sector  2 - reserved for bootloader   */
  /* { 0x08001800, 0x00800,  3},           flash sector  3 - reserved for bootloader   */
  { 0x08002000, 0x00800,  4},           /* flash sector  4 - 2kb                       */
  { 0x08002800, 0x00800,  5},           /* flash sector  5 - 2kb                       */
  { 0x08003000, 0x00800,  6},           /* flash sector  6 - 2kb                       */
  { 0x08003800, 0x00800,  7},           /* flash sector  7 - 2kb                       */
#if (BOOT_NVM_SIZE_KB > 16)
  { 0x08004000, 0x00800,  8},           /* flash sector  8 - 2kb                       */
  { 0x08004800, 0x00800,  9},           /* flash sector  9 - 2kb                       */
  { 0x08005000, 0x00800, 10},           /* flash sector 10 - 2kb                       */
  { 0x08005800, 0x00800, 11},           /* flash sector 11 - 2kb                       */
  { 0x08006000, 0x00800, 12},           /* flash sector 12 - 2kb                       */
  { 0x08006800, 0x00800, 13},           /* flash sector 13 - 2kb                       */
  { 0x08007000, 0x00800, 14},           /* flash sector 14 - 2kb                       */
  { 0x08007800, 0x00800, 15},           /* flash sector 15 - 2kb                       */
#endif
#if (BOOT_NVM_SIZE_KB > 32)
  { 0x08008000, 0x08000, 16},           /* flash sector 16 - 32kb                       */
#endif
#if (BOOT_NVM_SIZE_KB > 64)
  { 0x08010000, 0x08000, 17},           /* flash sector 17 - 32kb                       */
  { 0x08018000, 0x08000, 18},           /* flash sector 18 - 32kb                       */
#endif
#if (BOOT_NVM_SIZE_KB > 128)
#error "BOOT_NVM_SIZE_KB > 128 is currently not supported."
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
  if ((FlashGetSector(addr)       == FLASH_INVALID_SECTOR) ||
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
  blt_bool  result = BLT_TRUE;
  blt_int8u first_sector = FLASH_INVALID_SECTOR;
  blt_int8u last_sector = FLASH_INVALID_SECTOR;

  /* validate the len parameter */
  if ((len - 1) > (FLASH_END_ADDRESS - addr))
  {
    result = BLT_FALSE;
  }
  
  /* only continue if all is okay so far */
  if (result == BLT_TRUE)
  {
    /* obtain the first and last sector numbers. */
    first_sector = FlashGetSector(addr);
    last_sector  = FlashGetSector(addr+len-1);
    /* check them */
    if ((first_sector == FLASH_INVALID_SECTOR) ||
        (last_sector  == FLASH_INVALID_SECTOR))
    {
      result = BLT_FALSE;
    }
  }

  /* only continue if all is okay so far */
  if (result == BLT_TRUE)
  {
    /* erase the sectors */
    result = FlashEraseSectors(first_sector, last_sector);
  }

  /* give the result back to the caller */
  return result;
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
  blt_bool   result = BLT_TRUE;
  blt_int32u signature_checksum = 0;

  /* for the STM32 target we defined the checksum as the Two's complement value of the
   * sum of the first 7 exception addresses.
   *
   * Layout of the vector table:
   *    0x08000000 Initial stack pointer
   *    0x08000004 Reset Handler
   *    0x08000008 NMI Handler
   *    0x0800000C Hard Fault Handler
   *    0x08000010 Reserved
   *    0x08000014 Reserved
   *    0x08000018 Reserved
   *
   *    signature_checksum = Two's complement of (SUM(exception address values))
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
  if (bootBlockInfo.base_addr != FLASH_INVALID_ADDRESS)
  {
#if (BOOT_FLASH_CRYPTO_HOOKS_ENABLE > 0)
    /* perform decryption of the bootblock, before calculating the checksum and writing it
     * to flash memory.
     */
    if (FlashCryptoDecryptDataHook(bootBlockInfo.data, FLASH_WRITE_BLOCK_SIZE) == BLT_FALSE)
    {
      result = BLT_FALSE;
    }
#endif

    /* only continue if all is okay so far */
    if (result == BLT_TRUE)
    {
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
      signature_checksum += 1; /* two's complement */

      /* write the checksum */
      result = FlashWrite(flashLayout[0].sector_start+BOOT_FLASH_VECTOR_TABLE_CS_OFFSET,
                          sizeof(blt_addr), (blt_int8u *)&signature_checksum);
    }
  }
  
  /* give the result back to the caller */
  return result;
} /*** end of FlashWriteChecksum ***/


/************************************************************************************//**
** \brief     Verifies the checksum, which indicates that a valid user program is
**            present and can be started.
** \return    BLT_TRUE if successful, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool FlashVerifyChecksum(void)
{
  blt_bool   result = BLT_TRUE;
  blt_int32u signature_checksum = 0;

  /* verify the checksum based on how it was written by FlashWriteChecksum() */
  signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start));
  signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+0x04));
  signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+0x08));
  signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+0x0C));
  signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+0x10));
  signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+0x14));
  signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+0x18));
  /* add the checksum value that was written by FlashWriteChecksum(). Since this was a
   * Two complement's value, the resulting value should equal 0.
   */ 
  signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+BOOT_FLASH_VECTOR_TABLE_CS_OFFSET));
  /* sum should add up to an unsigned 32-bit value of 0 */
  if (signature_checksum != 0)
  {
    /* checksum not okay */
    result = BLT_FALSE;
  }
  
  /* give the result back to the caller */
  return result;
} /*** end of FlashVerifyChecksum ***/


/************************************************************************************//**
** \brief     Finalizes the flash driver operations. There could still be data in
**            the currently active block that needs to be flashed.
** \return    BLT_TRUE if successful, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool FlashDone(void)
{
  blt_bool result = BLT_TRUE;

  /* check if there is still data waiting to be programmed in the boot block */
  if (bootBlockInfo.base_addr != FLASH_INVALID_ADDRESS)
  {
    if (FlashWriteBlock(&bootBlockInfo) == BLT_FALSE)
    {
      /* update the result value to flag the error */
      result = BLT_FALSE;
    }
  }

  /* only continue if all is okay so far */
  if (result == BLT_TRUE)
  {
    /* check if there is still data waiting to be programmed */
    if (blockInfo.base_addr != FLASH_INVALID_ADDRESS)
    {
      if (FlashWriteBlock(&blockInfo) == BLT_FALSE)
      {
        /* update the result value to flag the error */
        result = BLT_FALSE;
      }
    }
  }

  /* give the result back to the caller */
  return result;
} /*** end of FlashDone ***/


/************************************************************************************//**
** \brief     Obtains the base address of the flash memory available to the user program.
**            This is basically the first address in the flashLayout table.
** \return    Base address.
**
****************************************************************************************/
blt_addr FlashGetUserProgBaseAddress(void)
{
  blt_addr result;
  
  result = flashLayout[0].sector_start;
  
  /* give the result back to the caller */
  return result;
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
  blt_bool result = BLT_TRUE;

  /* check address alignment */
  if ((address % FLASH_WRITE_BLOCK_SIZE) != 0)
  {
    /* update the result value to flag the error */
    result = BLT_FALSE;
  }
  
  /* only continue if all is okay so far */
  if (result == BLT_TRUE)
  {
    /* make sure that we are initializing a new block and not the same one */
    if (block->base_addr != address)
    {
      /* set the base address and copies the current data from flash */
      block->base_addr = address;
      CpuMemCopy((blt_addr)block->data, address, FLASH_WRITE_BLOCK_SIZE);
    }
  }

  /* give the result back to the caller */
  return result;
} /*** end of FlashInitBlock ***/


/************************************************************************************//**
** \brief     Switches blocks by programming the current one and initializing the
**            next.
** \param     block   Pointer to flash block info structure to operate on.
** \param     base_addr Base address of the next block.
** \return    The pointer of the block info struct that is now being used, or a NULL
**            pointer in case of error.
**
****************************************************************************************/
static tFlashBlockInfo *FlashSwitchBlock(tFlashBlockInfo *block, blt_addr base_addr)
{
  tFlashBlockInfo * result = BLT_NULL;

  /* check if a switch needs to be made away from the boot block. in this case the boot
   * block shouldn't be written yet, because this is done at the end of the programming
   * session by FlashDone(), this is right after the checksum was written.
   */
  if (block == &bootBlockInfo)
  {
    /* switch from the boot block to the generic block info structure */
    block = &blockInfo;
    result = block;
  }
  /* check if a switch back into the bootblock is needed. in this case the generic block
   * doesn't need to be written here yet.
   */
  else if (base_addr == flashLayout[0].sector_start)
  {
    /* switch from the generic block to the boot block info structure */
    block = &bootBlockInfo;
    base_addr = flashLayout[0].sector_start;
    result = block;
  }
  else
  {
    /* need to switch to a new block, so program the current one and init the next */
    if (FlashWriteBlock(block) == BLT_TRUE)
    {
      /* block write succeeded; update result accordingly */
      result = block;
    }
  }

  /* only continue if all is okay sofar */
  if (result != BLT_NULL)
  {
    /* initialize the new block when necessary */
    if (FlashInitBlock(block, base_addr) == BLT_FALSE)
    {
      /* invalidate the result value to flag the error */
      result = BLT_NULL;
    }
  }

  /* Give the result back to the caller. */
  return result;
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
  blt_bool   result = BLT_TRUE;
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
      result = BLT_FALSE;
    }
  }

  /* only continue if all is okay so far */
  if (result == BLT_TRUE)
  {
    /* check if the new data fits in the current block */
    if (block->base_addr != current_base_addr)
    {
      /* need to switch to a new block, so program the current one and init the next */
      block = FlashSwitchBlock(block, current_base_addr);
      if (block == BLT_NULL)
      {
        result = BLT_FALSE;
      }
    }
  }

  /* only continue if all is okay so far */
  if (result == BLT_TRUE)
  {
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
          /* flag error and stop looping */
          result = BLT_FALSE;
          break;
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
  }

  /* give the result back to the caller */
  return result;
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
  blt_bool   result = BLT_TRUE;
  blt_addr   prog_addr;
  blt_int64u prog_data;
  blt_int32u word_cnt;

  /* check that the address is actually within flash */
  if (FlashGetSector(block->base_addr) == FLASH_INVALID_SECTOR)
  {
    result = BLT_FALSE;
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
      result = BLT_FALSE;
   }
  }
#endif

  /* only continue if all is okay so far */
  if (result == BLT_TRUE)
  {
    /* unlock the flash peripheral to enable the flash control register access. */
    HAL_FLASH_Unlock();

    /* program all words in the block one by one */
    for (word_cnt=0; word_cnt<(FLASH_WRITE_BLOCK_SIZE/sizeof(prog_data)); word_cnt++)
    {
      prog_addr = block->base_addr + (word_cnt * sizeof(prog_data));
      prog_data = *(volatile blt_int64u *)(&block->data[word_cnt * sizeof(prog_data)]);
      /* keep the watchdog happy */
      CopService();
      /* program the word */
      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, prog_addr, prog_data) != HAL_OK)
      {
        result = BLT_FALSE;
        break;
      }
      /* verify that the written data is actually there */
      if (*(volatile blt_int64u *)prog_addr != prog_data)
      {
        result = BLT_FALSE;
        break;
      }
    }

    /* lock the flash peripheral to disable the flash control register access. */
    HAL_FLASH_Lock();
  }

  /* Give the result back to the caller. */
  return result;
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
  blt_bool   result = BLT_TRUE;
  blt_int16u nr_of_blocks;
  blt_int16u block_cnt;
  blt_addr   start_addr;
  blt_addr   end_addr;
  blt_int32u pageError = 0;
  FLASH_EraseInitTypeDef eraseInitStruct;

  /* validate the sector numbers */
  if (first_sector > last_sector)
  {
    result = BLT_FALSE;
  }

  /* only continue if all is okay so far */
  if (result == BLT_TRUE)
  {
    if ((first_sector < flashLayout[0].sector_num) || \
        (last_sector  > flashLayout[FLASH_TOTAL_SECTORS-1].sector_num))
    {
      result = BLT_FALSE;
    }
  }

  /* only continue if all is okay so far */
  if (result == BLT_TRUE)
  {
    /* determine how many blocks need to be erased */
    start_addr = FlashGetSectorBaseAddr(first_sector);
    end_addr   = FlashGetSectorBaseAddr(last_sector) + 
                 FlashGetSectorSize(last_sector) - 1;
    nr_of_blocks = (end_addr - start_addr + 1) / FLASH_ERASE_BLOCK_SIZE;
    /* prepare the erase initialization structure. */
    eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInitStruct.Page      = FLASH_ERASE_BLOCK_NUM(start_addr);
    eraseInitStruct.NbPages   = 1;
    /* unlock the flash array */
    HAL_FLASH_Unlock();
    /* erase all blocks one by one */
    for (block_cnt=0; block_cnt<nr_of_blocks; block_cnt++)
    {
      /* keep the watchdog happy */
      CopService();
      /* erase block */
      if (HAL_FLASHEx_Erase(&eraseInitStruct, (uint32_t *)&pageError) != HAL_OK)
      {
        /* could not perform erase operation. update result and stop loop. */
        result = BLT_FALSE;
        /* error detected so don't bother continuing with the loop */
        break;
      }
      /* update the page number for the next sector. */
      eraseInitStruct.Page += 1;
    }
    /* lock the flash array again */
    HAL_FLASH_Lock();
  }

  /* give the result back to the caller */
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
static blt_addr FlashGetSectorSize(blt_int8u sector)
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


/*********************************** end of flash.c ************************************/
