/************************************************************************************//**
* \file         Source/ARMCM0_S32K11/flash.c
* \brief        Bootloader flash driver source file.
* \ingroup      Target_ARMCM0_S32K11
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2020  by Feaser    http://www.feaser.com    All rights reserved
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
#include "device_registers.h"                    /* device registers                   */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Value for an invalid sector entry index into flashLayout[]. */
#define FLASH_INVALID_SECTOR_IDX        (0xff)
/** \brief Value for an invalid flash address. */
#define FLASH_INVALID_ADDRESS           (0xffffffff)
/** \brief Standard size of a flash block for writing. */
#define FLASH_WRITE_BLOCK_SIZE          (256)
/** \brief Standard size of a flash block for erasing. This is either 2 or 4 kb depending
 *         on the microcontroller derivative.
 */
#define FLASH_ERASE_BLOCK_SIZE          (FEATURE_FLS_PF_BLOCK_SECTOR_SIZE)
/** \brief Total numbers of sectors in array flashLayout[]. */
#define FLASH_TOTAL_SECTORS             (sizeof(flashLayout)/sizeof(flashLayout[0]))
/** \brief End address of the bootloader programmable flash. */
#define FLASH_END_ADDRESS               (flashLayout[FLASH_TOTAL_SECTORS-1].sector_start + \
                                         flashLayout[FLASH_TOTAL_SECTORS-1].sector_size - 1)
/** \brief FTFC program phrase command code. */
#define FLASH_FTFC_CMD_PROGRAM_PHRASE   (0x07U)
/** \brief FTFC erase sector command code. */
#define FLASH_FTFC_CMD_ERASE_SECTOR     (0x09U)

/** \brief Offset into the user program's vector table where the checksum is located. 
 *         Note that the value can be overriden in blt_conf.h, because the size of the
 *         vector table could vary. When changing this value, don't forget to update the
 *         location of the checksum in the user program accordingly. Otherwise the 
 *         checksum verification will always fail.
 */
#ifndef BOOT_FLASH_VECTOR_TABLE_CS_OFFSET
#define BOOT_FLASH_VECTOR_TABLE_CS_OFFSET    (0xC0)
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
static blt_bool  FlashEraseSectors(blt_int8u first_sector_idx, 
                                   blt_int8u last_sector_idx);
static blt_int8u FlashGetSectorIdx(blt_addr address);
START_FUNCTION_DECLARATION_RAMSECTION
static void      FlashCommandSequence(void)
END_FUNCTION_DECLARATION_RAMSECTION


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
 */
static const tFlashSector flashLayout[] =
{
  /* Update the contents of this array with the erase sector sizes as defined in the
   * microcontroller's reference manual. The flash sector erase sizes are hardware
   * specific and must therefore match, otherwise erase operations cannot be performed
   * properly.
   * Besides controlling the flash erase size, this array also controls which sectors
   * are reserved for the bootloader and will therefore never be erased. Note the for the
   * S32K11x, the flash sector erase size is 2kb. It was decided to create entries that 
   * are equal or a multiple of 2kb to simplify the flash layout array and to keep it 
   * from getting unnecessarily large.
   */
  /*{ 0x00000000, 0x00800,   0},           flash sector  0 - reserved for bootloader   */
  /*{ 0x00000800, 0x01000,   1},           flash sector  1 - reserved for bootloader   */
  /*{ 0x00001000, 0x00800,   2},           flash sector  2 - reserved for bootloader   */
  /*{ 0x00001800, 0x00800,   3},           flash sector  3 - reserved for bootloader   */
  { 0x00002000, 0x00800,   4},          /* flash sector  4 -  2kb                      */
  { 0x00002800, 0x00800,   5},          /* flash sector  5 -  2kb                      */
  { 0x00003000, 0x00800,   6},          /* flash sector  6 -  2kb                      */
  { 0x00003800, 0x00800,   7},          /* flash sector  7 -  2kb                      */
  { 0x00004000, 0x00800,   8},          /* flash sector  8 -  2kb                      */
  { 0x00004800, 0x00800,   9},          /* flash sector  9 -  2kb                      */
  { 0x00005000, 0x00800,  10},          /* flash sector 10 -  2kb                      */
  { 0x00005800, 0x00800,  11},          /* flash sector 11 -  2kb                      */
  { 0x00006000, 0x00800,  12},          /* flash sector 12 -  2kb                      */
  { 0x00006800, 0x00800,  13},          /* flash sector 13 -  2kb                      */
  { 0x00007000, 0x00800,  14},          /* flash sector 14 -  2kb                      */
  { 0x00007800, 0x00800,  15},          /* flash sector 15 -  2kb                      */
  { 0x00008000, 0x08000,  16},          /* flash sector 16 - 32kb                      */
  { 0x00010000, 0x08000,  17},          /* flash sector 17 - 32kb                      */
  { 0x00018000, 0x08000,  18},          /* flash sector 18 - 32kb                      */
  #if (BOOT_NVM_SIZE_KB > 128)
  { 0x00020000, 0x08000,  19},          /* flash sector 19 - 32kb                      */
  { 0x00028000, 0x08000,  20},          /* flash sector 20 - 32kb                      */
  { 0x00030000, 0x08000,  21},          /* flash sector 21 - 32kb                      */
  { 0x00038000, 0x08000,  22},          /* flash sector 22 - 32kb                      */
  #endif
  #if (BOOT_NVM_SIZE_KB > 256)
  #error "BOOT_NVM_SIZE_KB > 256 is currently not supported."
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
  blt_bool result = BLT_TRUE;
  blt_addr base_addr;

  /* validate the len parameter */
  if ((len - 1) > (FLASH_END_ADDRESS - addr))
  {
    result = BLT_FALSE;
  }
  
  /* only continue if all is okay so far */
  if (result == BLT_TRUE)
  {
    /* make sure the addresses are within the flash device */
    if ((FlashGetSectorIdx(addr) == FLASH_INVALID_SECTOR_IDX) || \
        (FlashGetSectorIdx(addr+len-1) == FLASH_INVALID_SECTOR_IDX))
    {
      result = BLT_FALSE;
    }
  }
  
  /* only continue if all is okay so far */
  if (result == BLT_TRUE)
  {
    /* if this is the bootblock, then let the boot block manager handle it */
    base_addr = (addr/FLASH_WRITE_BLOCK_SIZE)*FLASH_WRITE_BLOCK_SIZE;
    if (base_addr == flashLayout[0].sector_start)
    {
      /* let the boot block manager handle it */
      result = FlashAddToBlock(&bootBlockInfo, addr, data, len);
    }
    else
    {
      /* let the block manager handle it */
      result = FlashAddToBlock(&blockInfo, addr, data, len);
    }
  }

  /* give the result back to the caller */
  return result;
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
  blt_int8u first_sector_idx;
  blt_int8u last_sector_idx;

  /* validate the len parameter */
  if ((len - 1) > (FLASH_END_ADDRESS - addr))
  {
    result = BLT_FALSE;
  }
  
  /* only continue if all is okay so far */
  if (result == BLT_TRUE)
  {
    /* obtain the first and last sector entry indices to the flashLayout[] array. */
    first_sector_idx = FlashGetSectorIdx(addr);
    last_sector_idx  = FlashGetSectorIdx(addr+len-1);
    /* check them */
    if ((first_sector_idx == FLASH_INVALID_SECTOR_IDX) ||
        (last_sector_idx == FLASH_INVALID_SECTOR_IDX))
    {
      result = BLT_FALSE;
    }
  }

  /* only continue if all is okay so far */
  if (result == BLT_TRUE)
  {
    /* erase the sectors */
    result = FlashEraseSectors(first_sector_idx, last_sector_idx);
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

  /* verify the checksum based on how it was written by FlashWriteChecksum(). */
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
  /* no switching between the generic block and the bootblock needed. it is a switch
   * within a generic block. the current block needs to be first programmed before a 
   * switch to the new one can be make.
   */   
  else
  {
    /* start by initializing the result to success */
    result = block;
    /* need to switch to a new block, so program the current one and init the next */
    if (FlashWriteBlock(block) == BLT_FALSE)
    {
      /* invalidate the result value to flag the error */
      result = BLT_NULL;
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
  blt_bool           result = BLT_TRUE;
  blt_addr           prog_addr;
  blt_int8u        * prog_data;
  blt_int8u  const * flash_data;
  blt_int32u         phrase_cnt;
  blt_int8u  const   phrase_size = FEATURE_FLS_PF_BLOCK_WRITE_UNIT_SIZE;
  blt_int8u          phrase_byte_idx;

  /* check that the address is actually within flash */
  if (FlashGetSectorIdx(block->base_addr) == FLASH_INVALID_SECTOR_IDX)
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
    /* program all phrases in the block one by one */
    for (phrase_cnt=0; phrase_cnt<(FLASH_WRITE_BLOCK_SIZE/phrase_size); phrase_cnt++)
    {
      prog_addr = block->base_addr + (phrase_cnt * phrase_size);
      prog_data = &block->data[phrase_cnt * phrase_size];
      /* keep the watchdog happy */
      CopService();
      /* check CCIF to verify that the previous command is completed. */
      if ((FTFC->FSTAT & FTFC_FSTAT_CCIF_MASK) == FTFC_FSTAT_CCIF(0))
      {
        /* FTFC module should not be busy anymore. flag error and abort. */
        result = BLT_FALSE;
        break;
      }
      /* clear the old errors that might still be set from a previous operation. */
      FTFC->FSTAT = FTFC_FSTAT_FPVIOL_MASK | FTFC_FSTAT_ACCERR_MASK | FTFC_FSTAT_RDCOLERR_MASK;
      /* prepare the program phrase command.
       * FTFC->FCCOB[3] = FCCOB0
       */
      FTFC->FCCOB[3] = FLASH_FTFC_CMD_PROGRAM_PHRASE;
      /* set the program base address.
       * FTFC->FCCOB[2] = FCCOB1
       * FTFC->FCCOB[1] = FCCOB2
       * FTFC->FCCOB[0] = FCCOB3
       */
      FTFC->FCCOB[2] = (blt_int8u)(((blt_addr)(prog_addr >> 16U)) & 0xFFU);
      FTFC->FCCOB[1] = (blt_int8u)(((blt_addr)(prog_addr >>  8U)) & 0xFFU);
      FTFC->FCCOB[0] = (blt_int8u)(prog_addr & 0xFFU);
      /* set the phrase bytes that should be programmed.
       * FTFC->FCCOB[7]  = FCCOB4
       * FTFC->FCCOB[6]  = FCCOB5
       * FTFC->FCCOB[5]  = FCCOB6
       * FTFC->FCCOB[4]  = FCCOB7
       * FTFC->FCCOB[11] = FCCOB8
       * FTFC->FCCOB[10] = FCCOB9
       * FTFC->FCCOB[9]  = FCCOBA
       * FTFC->FCCOB[8]  = FCCOBB
       */
      FTFC->FCCOB[4]  = prog_data[0];
      FTFC->FCCOB[5]  = prog_data[1];
      FTFC->FCCOB[6]  = prog_data[2];
      FTFC->FCCOB[7]  = prog_data[3];
      FTFC->FCCOB[8]  = prog_data[4];
      FTFC->FCCOB[9]  = prog_data[5];
      FTFC->FCCOB[10] = prog_data[6];
      FTFC->FCCOB[11] = prog_data[7];
      /* Execute the command. Note that it needs to run from RAM. */
      FlashCommandSequence();
      /* Check the results. */
      if ((FTFC->FSTAT & (FTFC_FSTAT_MGSTAT0_MASK | FTFC_FSTAT_FPVIOL_MASK |
          FTFC_FSTAT_ACCERR_MASK | FTFC_FSTAT_RDCOLERR_MASK)) != 0U)
      {
        /* could not perform program operation */
        result = BLT_FALSE;
        /* error detected so don't bother continuing with the loop */
        break;
      }
      /* verify that the written data is actually there. */
      flash_data = ((blt_int8u const *)prog_addr);
      for (phrase_byte_idx = 0; phrase_byte_idx < phrase_size; phrase_byte_idx++)
      {
        /* check that the byte in flash has the same value as what was programmed. */
        if (flash_data[phrase_byte_idx] != prog_data[phrase_byte_idx])
        {
          /* verification of programmed data failed. */
          result = BLT_FALSE;
          /* error detected so don't bother continuing with the loop */
          break;

        }
      }
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of FlashWriteBlock ***/


/************************************************************************************//**
** \brief     Erases the flash sectors from indices first_sector_idx up until
**            last_sector_idx into the flashLayout[] array.
** \param     first_sector_idx First flash sector number index into flashLayout[].
** \param     last_sector_idx  Last flash sector number index into flashLayout[].
** \return    BLT_TRUE if successful, BLT_FALSE otherwise.
**
****************************************************************************************/
static blt_bool FlashEraseSectors(blt_int8u first_sector_idx, blt_int8u last_sector_idx)
{
  blt_bool   result = BLT_TRUE;
  blt_int8u  sectorIdx;
  blt_addr   sectorBaseAddr;
  blt_int32u sectorSize;
  blt_int8u  blockIdx;
  blt_addr   blockBaseAddr;
  blt_int8u  totalBlocks;

  /* validate the sector numbers */
  if (first_sector_idx > last_sector_idx)
  {
    result = BLT_FALSE;
  }

  /* only continue if all is okay so far */
  if (result == BLT_TRUE)
  {
    if (last_sector_idx > (FLASH_TOTAL_SECTORS-1))
    {
      result = BLT_FALSE;
    }
  }

  /* only continue if all is okay so far */
  if (result == BLT_TRUE)
  {
    /* erase the sectors one by one */
    for (sectorIdx = first_sector_idx; sectorIdx <= last_sector_idx; sectorIdx++)
    {
      /* service the watchdog */
      CopService();
      /* get information about the sector */
      sectorBaseAddr = flashLayout[sectorIdx].sector_start;
      sectorSize = flashLayout[sectorIdx].sector_size;
      /* validate the sector information */
      if ( (sectorBaseAddr == FLASH_INVALID_ADDRESS) || (sectorSize == 0) )
      {
        /* invalid sector information. flag error and abort erase operation */
        result = BLT_FALSE;
        break;
      }
      
      /* each sector could contain more than just one block. make sure the base address
       * of the sector is block aligned.
       */
      if ((sectorBaseAddr % FLASH_ERASE_BLOCK_SIZE) != 0)
      {
        /* sector base address not aligned to the start of a block. flag error and abort
         * erase operation
         */
        result = BLT_FALSE;
        break;
      }

      /* make sure the sector size is an exact multiple of the block size. */
      if ((sectorSize % FLASH_ERASE_BLOCK_SIZE) != 0)
      {
        /* sector base address not aligned to the start of a block. flag error and abort
         * erase operation
         */
        result = BLT_FALSE;
        break;
      }

      /* erase the sector one block at a time. */
      totalBlocks = sectorSize / FLASH_ERASE_BLOCK_SIZE;
      for (blockIdx = 0; blockIdx < totalBlocks; blockIdx++)
      {
        /* service the watchdog */
        CopService();
        /* store the block base address. */
        blockBaseAddr = sectorBaseAddr + (blockIdx * FLASH_ERASE_BLOCK_SIZE);
        /* check CCIF to verify that the previous command is completed. */
        if ((FTFC->FSTAT & FTFC_FSTAT_CCIF_MASK) == FTFC_FSTAT_CCIF(0))
        {
          /* FTFC module should not be busy anymore. flag error and abort. */
          result = BLT_FALSE;
          break;
        }
        /* clear the old errors that might still be set from a previous operation. */
        FTFC->FSTAT = FTFC_FSTAT_FPVIOL_MASK | FTFC_FSTAT_ACCERR_MASK | FTFC_FSTAT_RDCOLERR_MASK;
        /* prepare the sector erase command.
         * FTFC->FCCOB[3] = FCCOB0
         */
        FTFC->FCCOB[3] = FLASH_FTFC_CMD_ERASE_SECTOR;
        /* set the erase sector base address. note that in this function that means the
         * block base address.
         * FTFC->FCCOB[2] = FCCOB1
         * FTFC->FCCOB[1] = FCCOB2
         * FTFC->FCCOB[0] = FCCOB3
         */
        FTFC->FCCOB[2] = (blt_int8u)(((blt_addr)(blockBaseAddr >> 16U)) & 0xFFU);
        FTFC->FCCOB[1] = (blt_int8u)(((blt_addr)(blockBaseAddr >>  8U)) & 0xFFU);
        FTFC->FCCOB[0] = (blt_int8u)(blockBaseAddr & 0xFFU);
        /* Execute the command. Note that it needs to run from RAM. */
        FlashCommandSequence();
        /* Check the results. */
        if ((FTFC->FSTAT & (FTFC_FSTAT_MGSTAT0_MASK | FTFC_FSTAT_FPVIOL_MASK |
            FTFC_FSTAT_ACCERR_MASK | FTFC_FSTAT_RDCOLERR_MASK)) != 0U)
        {
          /* could not perform erase operation */
          result = BLT_FALSE;
          /* error detected so don't bother continuing with the loop */
          break;
        }
      }

      /* Only continue with the next sector if all is okay so far. */
      if (result == BLT_FALSE)
      {
        break;
      }
    }
  }

  /* give the result back to the caller */
  return result;
} /*** end of FlashEraseSectors ***/


/************************************************************************************//**
** \brief     Determines the index into the flashLayout[] array of the flash sector that
**            the specified address is in.
** \param     address Address in the flash sector.
** \return    Flash sector index in flashLayout[] or FLASH_INVALID_SECTOR_IDX.
**
****************************************************************************************/
static blt_int8u FlashGetSectorIdx(blt_addr address)
{
  blt_int8u result = FLASH_INVALID_SECTOR_IDX;
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
      /* update the result value and stop looping */
      result = sectorIdx;
      break;
    }
  }

  /* give the result back to the caller */
  return result;
} /*** end of FlashGetSectorIdx ***/


/************************************************************************************//**
** \brief     Use the FTFC module to run the flash command sequence. It is assumed that
**            that command and its necessary parameters were already written to the
**            correct FTFC registers.
** \attention This function needs to run from RAM. It is configured such that the C
**            start-up code automatically copies it from ROM to RAM in function
**            init_data_bss(), which is called by the reset handler.
** \return    None.
**
****************************************************************************************/
START_FUNCTION_DEFINITION_RAMSECTION
static void FlashCommandSequence(void)
{
  /* Clear CCIF to launch command. This is done by writing a 1 to the bit. */
  FTFC->FSTAT |= FTFC_FSTAT_CCIF_MASK;

  /* Wait for operation to complete.
   * From S32K Reference Manual:
   * While executing from a particular PFLASH read partition , FTFC commands (except
   * parallel boot) cannot run over that PFLASH read partition.
   *
   * The S32K series up to 512kB only have 1 partition, meaning we cannot return from
   * this (ram based) function until the operation completes. We don't have to worry
   * about a potentially endless loop, as if an error occurs during the command, the
   * operation will return and set an error flag, which can be evaluated after this
   * function call.  If an operation hangs we have a processor hardware error, and have
   * more to worry about than a hanging while loop.
   */
  while ((FTFC->FSTAT & FTFC_FSTAT_CCIF_MASK) == 0U)
  {
    /* Ideally, the watchdog is serviced in this function. But function CopService() is
     * located in the flash partition and can therefore not be accessed. This does mean
     * that the watchdog timeout period should be configured to be longer that the worst
     * case execution time of the flash phrase program / sector erase commands.
     */
    ;
  }
} /*** end of FlashCommandSequence ***/
END_FUNCTION_DEFINITION_RAMSECTION


/*********************************** end of flash.c ************************************/
