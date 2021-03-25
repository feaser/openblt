/************************************************************************************//**
* \file         Source/ARMCM33_STM32L5/flash.c
* \brief        Bootloader flash driver source file.
* \ingroup      Target_ARMCM33_STM32L5
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2021  by Feaser    http://www.feaser.com    All rights reserved
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


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Value for an invalid sector entry index into flashLayout[]. */
#define FLASH_INVALID_SECTOR_IDX        (0xff)
/** \brief Value for an invalid flash address. */
#define FLASH_INVALID_ADDRESS           (0xffffffff)
/** \brief Standard size of a flash block for writing. */
#define FLASH_WRITE_BLOCK_SIZE          (512)
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
/** \brief Offset into the user program's vector table where the checksum is located.
 *         For this target it is set to the end of the vector table. Note that the
 *         value can be overriden in blt_conf.h, because the size of the vector table
 *         could vary. When changing this value, don't forget to update the location
 *         of the checksum in the user program accordingly. Otherwise the checksum
 *         verification will always fail.
 */
#define BOOT_FLASH_VECTOR_TABLE_CS_OFFSET    (0x1F4)
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
  /* Note that the current layout assumes a single bank organization. */
  /* { 0x08000000, 0x01000,   0},          flash sector   0 - reserved for bootloader  */
  /* { 0x08001000, 0x01000,   1},          flash sector   1 - reserved for bootloader  */
  /* { 0x08002000, 0x01000,   2},          flash sector   2 - reserved for bootloader  */
  /* { 0x08003000, 0x01000,   3},          flash sector   3 - reserved for bootloader  */
  { 0x08004000, 0x01000,   4},           /* flash sector   4 - 4kb                     */
  { 0x08005000, 0x01000,   5},           /* flash sector   5 - 4kb                     */
  { 0x08006000, 0x01000,   6},           /* flash sector   6 - 4kb                     */
  { 0x08007000, 0x01000,   7},           /* flash sector   7 - 4kb                     */
  { 0x08008000, 0x01000,   8},           /* flash sector   8 - 4kb                     */
  { 0x08009000, 0x01000,   9},           /* flash sector   9 - 4kb                     */
  { 0x0800A000, 0x01000,  10},           /* flash sector  10 - 4kb                     */
  { 0x0800B000, 0x01000,  11},           /* flash sector  11 - 4kb                     */
  { 0x0800C000, 0x01000,  12},           /* flash sector  12 - 4kb                     */
  { 0x0800D000, 0x01000,  13},           /* flash sector  13 - 4kb                     */
  { 0x0800E000, 0x01000,  14},           /* flash sector  14 - 4kb                     */
  { 0x0800F000, 0x01000,  15},           /* flash sector  15 - 4kb                     */
#if (BOOT_NVM_SIZE_KB > 64)
  { 0x08010000, 0x01000,  16},           /* flash sector  16 - 4kb                     */
  { 0x08011000, 0x01000,  17},           /* flash sector  17 - 4kb                     */
  { 0x08012000, 0x01000,  18},           /* flash sector  18 - 4kb                     */
  { 0x08013000, 0x01000,  19},           /* flash sector  19 - 4kb                     */
  { 0x08014000, 0x01000,  20},           /* flash sector  20 - 4kb                     */
  { 0x08015000, 0x01000,  21},           /* flash sector  21 - 4kb                     */
  { 0x08016000, 0x01000,  22},           /* flash sector  22 - 4kb                     */
  { 0x08017000, 0x01000,  23},           /* flash sector  23 - 4kb                     */
  { 0x08018000, 0x01000,  24},           /* flash sector  24 - 4kb                     */
  { 0x08019000, 0x01000,  25},           /* flash sector  25 - 4kb                     */
  { 0x0801A000, 0x01000,  26},           /* flash sector  26 - 4kb                     */
  { 0x0801B000, 0x01000,  27},           /* flash sector  27 - 4kb                     */
  { 0x0801C000, 0x01000,  28},           /* flash sector  28 - 4kb                     */
  { 0x0801D000, 0x01000,  29},           /* flash sector  29 - 4kb                     */
  { 0x0801E000, 0x01000,  30},           /* flash sector  30 - 4kb                     */
  { 0x0801F000, 0x01000,  31},           /* flash sector  31 - 4kb                     */
#endif
#if (BOOT_NVM_SIZE_KB > 128)
  { 0x08020000, 0x01000,  32},           /* flash sector  32 - 4kb                     */
  { 0x08021000, 0x01000,  33},           /* flash sector  33 - 4kb                     */
  { 0x08022000, 0x01000,  34},           /* flash sector  34 - 4kb                     */
  { 0x08023000, 0x01000,  35},           /* flash sector  35 - 4kb                     */
  { 0x08024000, 0x01000,  36},           /* flash sector  36 - 4kb                     */
  { 0x08025000, 0x01000,  37},           /* flash sector  37 - 4kb                     */
  { 0x08026000, 0x01000,  38},           /* flash sector  38 - 4kb                     */
  { 0x08027000, 0x01000,  39},           /* flash sector  39 - 4kb                     */
  { 0x08028000, 0x01000,  40},           /* flash sector  40 - 4kb                     */
  { 0x08029000, 0x01000,  41},           /* flash sector  41 - 4kb                     */
  { 0x0802A000, 0x01000,  42},           /* flash sector  42 - 4kb                     */
  { 0x0802B000, 0x01000,  43},           /* flash sector  43 - 4kb                     */
  { 0x0802C000, 0x01000,  44},           /* flash sector  44 - 4kb                     */
  { 0x0802D000, 0x01000,  45},           /* flash sector  45 - 4kb                     */
  { 0x0802E000, 0x01000,  46},           /* flash sector  46 - 4kb                     */
  { 0x0802F000, 0x01000,  47},           /* flash sector  47 - 4kb                     */
  { 0x08030000, 0x01000,  48},           /* flash sector  48 - 4kb                     */
  { 0x08031000, 0x01000,  49},           /* flash sector  49 - 4kb                     */
  { 0x08032000, 0x01000,  50},           /* flash sector  50 - 4kb                     */
  { 0x08033000, 0x01000,  51},           /* flash sector  51 - 4kb                     */
  { 0x08034000, 0x01000,  52},           /* flash sector  52 - 4kb                     */
  { 0x08035000, 0x01000,  53},           /* flash sector  53 - 4kb                     */
  { 0x08036000, 0x01000,  54},           /* flash sector  54 - 4kb                     */
  { 0x08037000, 0x01000,  55},           /* flash sector  55 - 4kb                     */
  { 0x08038000, 0x01000,  56},           /* flash sector  56 - 4kb                     */
  { 0x08039000, 0x01000,  57},           /* flash sector  57 - 4kb                     */
  { 0x0803A000, 0x01000,  58},           /* flash sector  58 - 4kb                     */
  { 0x0803B000, 0x01000,  59},           /* flash sector  59 - 4kb                     */
  { 0x0803C000, 0x01000,  60},           /* flash sector  60 - 4kb                     */
  { 0x0803D000, 0x01000,  61},           /* flash sector  61 - 4kb                     */
  { 0x0803E000, 0x01000,  62},           /* flash sector  62 - 4kb                     */
  { 0x0803F000, 0x01000,  63},           /* flash sector  63 - 4kb                     */
#endif
#if (BOOT_NVM_SIZE_KB > 256)
  { 0x08040000, 0x01000,  64},           /* flash sector  64 - 4kb                     */
  { 0x08041000, 0x01000,  65},           /* flash sector  65 - 4kb                     */
  { 0x08042000, 0x01000,  66},           /* flash sector  66 - 4kb                     */
  { 0x08043000, 0x01000,  67},           /* flash sector  67 - 4kb                     */
  { 0x08044000, 0x01000,  68},           /* flash sector  68 - 4kb                     */
  { 0x08045000, 0x01000,  69},           /* flash sector  69 - 4kb                     */
  { 0x08046000, 0x01000,  70},           /* flash sector  70 - 4kb                     */
  { 0x08047000, 0x01000,  71},           /* flash sector  71 - 4kb                     */
  { 0x08048000, 0x01000,  72},           /* flash sector  72 - 4kb                     */
  { 0x08049000, 0x01000,  73},           /* flash sector  73 - 4kb                     */
  { 0x0804A000, 0x01000,  74},           /* flash sector  74 - 4kb                     */
  { 0x0804B000, 0x01000,  75},           /* flash sector  75 - 4kb                     */
  { 0x0804C000, 0x01000,  76},           /* flash sector  76 - 4kb                     */
  { 0x0804D000, 0x01000,  77},           /* flash sector  77 - 4kb                     */
  { 0x0804E000, 0x01000,  78},           /* flash sector  78 - 4kb                     */
  { 0x0804F000, 0x01000,  79},           /* flash sector  79 - 4kb                     */
  { 0x08050000, 0x01000,  80},           /* flash sector  80 - 4kb                     */
  { 0x08051000, 0x01000,  81},           /* flash sector  81 - 4kb                     */
  { 0x08052000, 0x01000,  82},           /* flash sector  82 - 4kb                     */
  { 0x08053000, 0x01000,  83},           /* flash sector  83 - 4kb                     */
  { 0x08054000, 0x01000,  84},           /* flash sector  84 - 4kb                     */
  { 0x08055000, 0x01000,  85},           /* flash sector  85 - 4kb                     */
  { 0x08056000, 0x01000,  86},           /* flash sector  86 - 4kb                     */
  { 0x08057000, 0x01000,  87},           /* flash sector  87 - 4kb                     */
  { 0x08058000, 0x01000,  88},           /* flash sector  88 - 4kb                     */
  { 0x08059000, 0x01000,  89},           /* flash sector  89 - 4kb                     */
  { 0x0805A000, 0x01000,  90},           /* flash sector  90 - 4kb                     */
  { 0x0805B000, 0x01000,  91},           /* flash sector  91 - 4kb                     */
  { 0x0805C000, 0x01000,  92},           /* flash sector  92 - 4kb                     */
  { 0x0805D000, 0x01000,  93},           /* flash sector  93 - 4kb                     */
  { 0x0805E000, 0x01000,  94},           /* flash sector  94 - 4kb                     */
  { 0x0805F000, 0x01000,  95},           /* flash sector  95 - 4kb                     */
  { 0x08060000, 0x01000,  96},           /* flash sector  96 - 4kb                     */
  { 0x08061000, 0x01000,  97},           /* flash sector  97 - 4kb                     */
  { 0x08062000, 0x01000,  98},           /* flash sector  98 - 4kb                     */
  { 0x08063000, 0x01000,  99},           /* flash sector  99 - 4kb                     */
  { 0x08064000, 0x01000, 100},           /* flash sector 100 - 4kb                     */
  { 0x08065000, 0x01000, 101},           /* flash sector 101 - 4kb                     */
  { 0x08066000, 0x01000, 102},           /* flash sector 102 - 4kb                     */
  { 0x08067000, 0x01000, 103},           /* flash sector 103 - 4kb                     */
  { 0x08068000, 0x01000, 104},           /* flash sector 104 - 4kb                     */
  { 0x08069000, 0x01000, 105},           /* flash sector 105 - 4kb                     */
  { 0x0806A000, 0x01000, 106},           /* flash sector 106 - 4kb                     */
  { 0x0806B000, 0x01000, 107},           /* flash sector 107 - 4kb                     */
  { 0x0806C000, 0x01000, 108},           /* flash sector 108 - 4kb                     */
  { 0x0806D000, 0x01000, 109},           /* flash sector 109 - 4kb                     */
  { 0x0806E000, 0x01000, 110},           /* flash sector 110 - 4kb                     */
  { 0x0806F000, 0x01000, 111},           /* flash sector 111 - 4kb                     */
  { 0x08070000, 0x01000, 112},           /* flash sector 112 - 4kb                     */
  { 0x08071000, 0x01000, 113},           /* flash sector 113 - 4kb                     */
  { 0x08072000, 0x01000, 114},           /* flash sector 114 - 4kb                     */
  { 0x08073000, 0x01000, 115},           /* flash sector 115 - 4kb                     */
  { 0x08074000, 0x01000, 116},           /* flash sector 116 - 4kb                     */
  { 0x08075000, 0x01000, 117},           /* flash sector 117 - 4kb                     */
  { 0x08076000, 0x01000, 118},           /* flash sector 118 - 4kb                     */
  { 0x08077000, 0x01000, 119},           /* flash sector 119 - 4kb                     */
  { 0x08078000, 0x01000, 120},           /* flash sector 120 - 4kb                     */
  { 0x08079000, 0x01000, 121},           /* flash sector 121 - 4kb                     */
  { 0x0807A000, 0x01000, 122},           /* flash sector 122 - 4kb                     */
  { 0x0807B000, 0x01000, 123},           /* flash sector 123 - 4kb                     */
  { 0x0807C000, 0x01000, 124},           /* flash sector 124 - 4kb                     */
  { 0x0807D000, 0x01000, 125},           /* flash sector 125 - 4kb                     */
  { 0x0807E000, 0x01000, 126},           /* flash sector 126 - 4kb                     */
  { 0x0807F000, 0x01000, 127},           /* flash sector 127 - 4kb                     */
#endif
#if (BOOT_NVM_SIZE_KB > 512)
#error "BOOT_NVM_SIZE_KB > 512 is currently not supported."
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

  /* TODO ##Port Calculate and write the signature checksum such that it appears at the
   * address configured with macro BOOT_FLASH_VECTOR_TABLE_CS_OFFSET. Use the 
   * FlashWrite() function for the actual write operation. For a typical microcontroller,
   * the bootBlock holds the program code that includes the user program's interrupt
   * vector table and after which the 32-bit for the signature checksum is reserved.
   * 
   * Note that this means one extra dummy entry must be added at the end of the user 
   * program's vector table to reserve storage space for the signature checksum value,
   * which is then overwritten by this function.
   *
   * The example here calculates a signature checksum by summing up the first 32-bit
   * values in the bootBlock (so the first 7 interrupt vectors) and then taking the
   * Two's complement of this sum. You can modify this to anything you like as long as
   * the signature checksum is based on program code present in the bootBlock.
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

  /* TODO ##Port Implement code here that basically does the reverse of
   * FlashWriteChecksum(). Just make sure to read the values directory from flash memory
   * and NOT from the bootBlock. 
   * The example implementation reads the first 7 32-bit from the user program flash
   * memory and sums them up. The signature checksum written by FlashWriteChecksum() was
   * the Two complement's value. This means that if you add the previously written
   * signature checksum value to the sum of the first 7 32-bit values, the result is
   * a value of 0 in case the signature checksum is valid.
   */

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
  blt_bool   result = BLT_TRUE;
  blt_addr   prog_addr;
  blt_int32u prog_data;
  blt_int32u word_cnt;

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
    /* TODO ##Port Program the data contents in 'block' to flash memory here and read the
     * programmed data values back directory from flash memory to verify that the flash
     * program operation was successful. The example implementation assumes that flash
     * data can be written 32-bits at a time.
     */

    /* program all words in the block one by one */
    for (word_cnt=0; word_cnt<(FLASH_WRITE_BLOCK_SIZE/sizeof(blt_int32u)); word_cnt++)
    {
      prog_addr = block->base_addr + (word_cnt * sizeof(blt_int32u));
      prog_data = *(volatile blt_int32u *)(&block->data[word_cnt * sizeof(blt_int32u)]);
      /* keep the watchdog happy */
      CopService();
      /* TODO ##Port Program 32-bit 'prog_data' data value to memory address 'prog_addr'.
       * In case an error occured, set result to BLT_FALSE and break the loop. 
       */
      if (1 == 0)
      {
        result = BLT_FALSE;
        break;
      }
      /* verify that the written data is actually there */
      if (*(volatile blt_int32u *)prog_addr != prog_data)
      {
        /* TODO ##Port Uncomment the following two lines again. It was commented out so
         * that a dry run with the flash driver is possible without it reporting errors.
         */
        /*result = BLT_FALSE;*/
        /*break;*/
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
      
      /* TODO ##Port Perform the flash erase operation of a sector that starts at
       * 'sectorBaseAddr' and has a length of 'sectorSize' bytes. In case an error
       * occured, set result to BLT_FALSE and break the loop.
       */
      if(1 == 0)
      {
        /* could not perform erase operation */
        result = BLT_FALSE;
        /* error detected so don't bother continuing with the loop */
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


/*********************************** end of flash.c ************************************/
