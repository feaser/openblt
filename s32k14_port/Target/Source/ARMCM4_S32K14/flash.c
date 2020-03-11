/************************************************************************************//**
* \file         Source/ARMCM4_S32K14/flash.c
* \brief        Bootloader flash driver source file.
* \ingroup      Target_ARMCM4_S32K14
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


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Value for an invalid sector entry index into flashLayout[]. */
#define FLASH_INVALID_SECTOR_IDX        (0xff)
/** \brief Value for an invalid flash address. */
#define FLASH_INVALID_ADDRESS           (0xffffffff)
/** \brief Standard size of a flash block for writing. */
#define FLASH_WRITE_BLOCK_SIZE          (1024)
/** \brief Total numbers of sectors in array flashLayout[]. */
#define FLASH_TOTAL_SECTORS             (sizeof(flashLayout)/sizeof(flashLayout[0]))
/** \brief End address of the bootloader programmable flash. */
#define FLASH_END_ADDRESS               (flashLayout[FLASH_TOTAL_SECTORS-1].sector_start + \
                                         flashLayout[FLASH_TOTAL_SECTORS-1].sector_size - 1)
/** \brief Offset into the user program's vector table where the checksum is located. 
 *         For this target it is set to the second to last entry (254) in the vector 
 *         table. Note that the value can be overriden in blt_conf.h, because the size of
 *         the vector table could vary. When changing this value, don't forget to update 
 *         the location of the checksum in the user program accordingly. Otherwise the 
 *         checksum verification will always fail.
 */
#ifndef BOOT_FLASH_VECTOR_TABLE_CS_OFFSET
#define BOOT_FLASH_VECTOR_TABLE_CS_OFFSET    (0x3F8)
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
  blt_int16u sector_num;                         /**< sector number                    */
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
  /* Update the contents of this array with the erase sector sizes as defined in the
   * microcontroller's reference manual. The flash sector erase sizes are hardware
   * specific and must therefore match, otherwise erase operations cannot be performed
   * properly.
   * Besides controlling the flash erase size, this array also controls which sectors
   * are reserved for the bootloader and will therefore never be erased. Note the for the
   * S32K14x, the flash sector erase size is 2kb when the total flash size is less than
   * 512kb. If the total flash size if greater or equal than 512kb, then the flash sector
   * erase size is 4kb.
   */
#if (BOOT_NVM_SIZE_KB < 512)
  #if (BOOT_NVM_SIZE_KB < 256)
  #error "BOOT_NVM_SIZE_KB < 256 is currently not supported."
  #endif
  /*{ 0x00000000, 0x00800,   0},           flash sector  0 - reserved for bootloader   */
  /*{ 0x00000800, 0x00800,   1},           flash sector  1 - reserved for bootloader   */
  /*{ 0x00001000, 0x00800,   2},           flash sector  2 - reserved for bootloader   */
  /*{ 0x00001800, 0x00800,   3},           flash sector  3 - reserved for bootloader   */
  /*{ 0x00002000, 0x00800,   4},           flash sector  4 - reserved for bootloader   */
  /*{ 0x00002800, 0x00800,   5},           flash sector  5 - reserved for bootloader   */
  /*{ 0x00003000, 0x00800,   6},           flash sector  6 - reserved for bootloader   */
  /*{ 0x00003800, 0x00800,   7},           flash sector  7 - reserved for bootloader   */
  { 0x00004000, 0x00800,   8},          /* flash sector  8 - 2kb                       */
  { 0x00004800, 0x00800,   9},          /* flash sector  9 - 2kb                       */
  { 0x00005000, 0x00800,  10},          /* flash sector 10 - 2kb                       */
  { 0x00005800, 0x00800,  11},          /* flash sector 11 - 2kb                       */
  { 0x00006000, 0x00800,  12},          /* flash sector 12 - 2kb                       */
  { 0x00006800, 0x00800,  13},          /* flash sector 13 - 2kb                       */
  { 0x00007000, 0x00800,  14},          /* flash sector 14 - 2kb                       */
  { 0x00007800, 0x00800,  15},          /* flash sector 15 - 2kb                       */
  { 0x00008000, 0x00800,  16},          /* flash sector 16 - 2kb                       */
  { 0x00008800, 0x00800,  17},          /* flash sector 17 - 2kb                       */
  { 0x00009000, 0x00800,  18},          /* flash sector 18 - 2kb                       */
  { 0x00009800, 0x00800,  19},          /* flash sector 19 - 2kb                       */
  { 0x0000A000, 0x00800,  20},          /* flash sector 20 - 2kb                       */
  { 0x0000A800, 0x00800,  21},          /* flash sector 21 - 2kb                       */
  { 0x0000B000, 0x00800,  22},          /* flash sector 22 - 2kb                       */
  { 0x0000B800, 0x00800,  23},          /* flash sector 23 - 2kb                       */
  { 0x0000C000, 0x00800,  24},          /* flash sector 24 - 2kb                       */
  { 0x0000C800, 0x00800,  25},          /* flash sector 25 - 2kb                       */
  { 0x0000D000, 0x00800,  26},          /* flash sector 26 - 2kb                       */
  { 0x0000D800, 0x00800,  27},          /* flash sector 27 - 2kb                       */
  { 0x0000E000, 0x00800,  28},          /* flash sector 28 - 2kb                       */
  { 0x0000E800, 0x00800,  29},          /* flash sector 29 - 2kb                       */
  { 0x0000F000, 0x00800,  30},          /* flash sector 30 - 2kb                       */
  { 0x0000F800, 0x00800,  31},          /* flash sector 31 - 2kb                       */
  { 0x00010000, 0x00800,  32},          /* flash sector 32 - 2kb                       */
  { 0x00010800, 0x00800,  33},          /* flash sector 33 - 2kb                       */
  { 0x00011000, 0x00800,  34},          /* flash sector 34 - 2kb                       */
  { 0x00011800, 0x00800,  35},          /* flash sector 35 - 2kb                       */
  { 0x00012000, 0x00800,  36},          /* flash sector 36 - 2kb                       */
  { 0x00012800, 0x00800,  37},          /* flash sector 37 - 2kb                       */
  { 0x00013000, 0x00800,  38},          /* flash sector 38 - 2kb                       */
  { 0x00013800, 0x00800,  39},          /* flash sector 39 - 2kb                       */
  { 0x00014000, 0x00800,  40},          /* flash sector 40 - 2kb                       */
  { 0x00014800, 0x00800,  41},          /* flash sector 41 - 2kb                       */
  { 0x00015000, 0x00800,  42},          /* flash sector 42 - 2kb                       */
  { 0x00015800, 0x00800,  43},          /* flash sector 43 - 2kb                       */
  { 0x00016000, 0x00800,  44},          /* flash sector 44 - 2kb                       */
  { 0x00016800, 0x00800,  45},          /* flash sector 45 - 2kb                       */
  { 0x00017000, 0x00800,  46},          /* flash sector 46 - 2kb                       */
  { 0x00017800, 0x00800,  47},          /* flash sector 47 - 2kb                       */
  { 0x00018000, 0x00800,  48},          /* flash sector 48 - 2kb                       */
  { 0x00018800, 0x00800,  49},          /* flash sector 49 - 2kb                       */
  { 0x00019000, 0x00800,  50},          /* flash sector 50 - 2kb                       */
  { 0x00019800, 0x00800,  51},          /* flash sector 51 - 2kb                       */
  { 0x0001A000, 0x00800,  52},          /* flash sector 52 - 2kb                       */
  { 0x0001A800, 0x00800,  52},          /* flash sector 53 - 2kb                       */
  { 0x0001B000, 0x00800,  54},          /* flash sector 54 - 2kb                       */
  { 0x0001B800, 0x00800,  55},          /* flash sector 55 - 2kb                       */
  { 0x0001C000, 0x00800,  56},          /* flash sector 56 - 2kb                       */
  { 0x0001C800, 0x00800,  57},          /* flash sector 57 - 2kb                       */
  { 0x0001D000, 0x00800,  58},          /* flash sector 58 - 2kb                       */
  { 0x0001D800, 0x00800,  59},          /* flash sector 59 - 2kb                       */
  { 0x0001E000, 0x00800,  60},          /* flash sector 60 - 2kb                       */
  { 0x0001E800, 0x00800,  61},          /* flash sector 61 - 2kb                       */
  { 0x0001F000, 0x00800,  62},          /* flash sector 62 - 2kb                       */
  { 0x0001F800, 0x00800,  63},          /* flash sector 63 - 2kb                       */
  { 0x00020000, 0x00800,  64},          /* flash sector 64 - 2kb                       */
  { 0x00020800, 0x00800,  65},          /* flash sector 65 - 2kb                       */
  { 0x00021000, 0x00800,  66},          /* flash sector 66 - 2kb                       */
  { 0x00021800, 0x00800,  67},          /* flash sector 67 - 2kb                       */
  { 0x00022000, 0x00800,  68},          /* flash sector 68 - 2kb                       */
  { 0x00022800, 0x00800,  69},          /* flash sector 69 - 2kb                       */
  { 0x00023000, 0x00800,  70},          /* flash sector 70 - 2kb                       */
  { 0x00023800, 0x00800,  71},          /* flash sector 71 - 2kb                       */
  { 0x00024000, 0x00800,  72},          /* flash sector 72 - 2kb                       */
  { 0x00024800, 0x00800,  73},          /* flash sector 73 - 2kb                       */
  { 0x00025000, 0x00800,  74},          /* flash sector 74 - 2kb                       */
  { 0x00025800, 0x00800,  75},          /* flash sector 75 - 2kb                       */
  { 0x00026000, 0x00800,  76},          /* flash sector 76 - 2kb                       */
  { 0x00026800, 0x00800,  77},          /* flash sector 77 - 2kb                       */
  { 0x00027000, 0x00800,  78},          /* flash sector 78 - 2kb                       */
  { 0x00027800, 0x00800,  79},          /* flash sector 79 - 2kb                       */
  { 0x00028000, 0x00800,  80},          /* flash sector 80 - 2kb                       */
  { 0x00028800, 0x00800,  81},          /* flash sector 81 - 2kb                       */
  { 0x00029000, 0x00800,  82},          /* flash sector 82 - 2kb                       */
  { 0x00029800, 0x00800,  83},          /* flash sector 83 - 2kb                       */
  { 0x0002A000, 0x00800,  84},          /* flash sector 84 - 2kb                       */
  { 0x0002A800, 0x00800,  85},          /* flash sector 85 - 2kb                       */
  { 0x0002B000, 0x00800,  86},          /* flash sector 86 - 2kb                       */
  { 0x0002B800, 0x00800,  87},          /* flash sector 87 - 2kb                       */
  { 0x0002C000, 0x00800,  88},          /* flash sector 88 - 2kb                       */
  { 0x0002C800, 0x00800,  89},          /* flash sector 89 - 2kb                       */
  { 0x0002D000, 0x00800,  90},          /* flash sector 90 - 2kb                       */
  { 0x0002D800, 0x00800,  91},          /* flash sector 91 - 2kb                       */
  { 0x0002E000, 0x00800,  92},          /* flash sector 92 - 2kb                       */
  { 0x0002E800, 0x00800,  93},          /* flash sector 93 - 2kb                       */
  { 0x0002F000, 0x00800,  94},          /* flash sector 94 - 2kb                       */
  { 0x0002F800, 0x00800,  95},          /* flash sector 95 - 2kb                       */
  { 0x00030000, 0x00800,  96},          /* flash sector 96 - 2kb                       */
  { 0x00030800, 0x00800,  97},          /* flash sector 97 - 2kb                       */
  { 0x00031000, 0x00800,  98},          /* flash sector 98 - 2kb                       */
  { 0x00031800, 0x00800,  99},          /* flash sector 99 - 2kb                       */
  { 0x00032000, 0x00800, 100},          /* flash sector 100 - 2kb                      */
  { 0x00032800, 0x00800, 101},          /* flash sector 101 - 2kb                      */
  { 0x00033000, 0x00800, 102},          /* flash sector 102 - 2kb                      */
  { 0x00033800, 0x00800, 103},          /* flash sector 103 - 2kb                      */
  { 0x00034000, 0x00800, 104},          /* flash sector 104 - 2kb                      */
  { 0x00034800, 0x00800, 105},          /* flash sector 105 - 2kb                      */
  { 0x00035000, 0x00800, 106},          /* flash sector 106 - 2kb                      */
  { 0x00035800, 0x00800, 107},          /* flash sector 107 - 2kb                      */
  { 0x00036000, 0x00800, 108},          /* flash sector 108 - 2kb                      */
  { 0x00036800, 0x00800, 109},          /* flash sector 109 - 2kb                      */
  { 0x00037000, 0x00800, 110},          /* flash sector 110 - 2kb                      */
  { 0x00037800, 0x00800, 111},          /* flash sector 111 - 2kb                      */
  { 0x00038000, 0x00800, 112},          /* flash sector 112 - 2kb                      */
  { 0x00038800, 0x00800, 113},          /* flash sector 113 - 2kb                      */
  { 0x00039000, 0x00800, 114},          /* flash sector 114 - 2kb                      */
  { 0x00039800, 0x00800, 115},          /* flash sector 115 - 2kb                      */
  { 0x0003A000, 0x00800, 116},          /* flash sector 116 - 2kb                      */
  { 0x0003A800, 0x00800, 117},          /* flash sector 117 - 2kb                      */
  { 0x0003B000, 0x00800, 118},          /* flash sector 118 - 2kb                      */
  { 0x0003B800, 0x00800, 119},          /* flash sector 119 - 2kb                      */
  { 0x0003C000, 0x00800, 120},          /* flash sector 120 - 2kb                      */
  { 0x0003C800, 0x00800, 121},          /* flash sector 121 - 2kb                      */
  { 0x0003D000, 0x00800, 122},          /* flash sector 122 - 2kb                      */
  { 0x0003D800, 0x00800, 123},          /* flash sector 123 - 2kb                      */
  { 0x0003E000, 0x00800, 124},          /* flash sector 124 - 2kb                      */
  { 0x0003E800, 0x00800, 125},          /* flash sector 125 - 2kb                      */
  { 0x0003F000, 0x00800, 126},          /* flash sector 126 - 2kb                      */
  { 0x0003F800, 0x00800, 127},          /* flash sector 127 - 2kb                      */
#elif (BOOT_NVM_SIZE_KB >= 512)
  /*{ 0x00000000, 0x01000,   0},           flash sector  0 - reserved for bootloader   */
  /*{ 0x00001000, 0x01000,   1},           flash sector  1 - reserved for bootloader   */
  /*{ 0x00002000, 0x01000,   2},           flash sector  2 - reserved for bootloader   */
  /*{ 0x00003000, 0x01000,   3},           flash sector  3 - reserved for bootloader   */
  { 0x00004000, 0x01000,   4},          /* flash sector  4 - 4kb                       */
  { 0x00005000, 0x01000,   5},          /* flash sector  5 - 4kb                       */
  { 0x00006000, 0x01000,   6},          /* flash sector  6 - 4kb                       */
  { 0x00007000, 0x01000,   7},          /* flash sector  7 - 4kb                       */
  { 0x00008000, 0x01000,   8},          /* flash sector  8 - 4kb                       */
  { 0x00009000, 0x01000,   9},          /* flash sector  9 - 4kb                       */
  { 0x0000A000, 0x01000,  10},          /* flash sector 10 - 4kb                       */
  { 0x0000B000, 0x01000,  11},          /* flash sector 11 - 4kb                       */
  { 0x0000C000, 0x01000,  12},          /* flash sector 12 - 4kb                       */
  { 0x0000D000, 0x01000,  13},          /* flash sector 13 - 4kb                       */
  { 0x0000E000, 0x01000,  14},          /* flash sector 14 - 4kb                       */
  { 0x0000F000, 0x01000,  15},          /* flash sector 15 - 4kb                       */
  { 0x00010000, 0x01000,  16},          /* flash sector 16 - 4kb                       */
  { 0x00011000, 0x01000,  17},          /* flash sector 17 - 4kb                       */
  { 0x00012000, 0x01000,  18},          /* flash sector 18 - 4kb                       */
  { 0x00013000, 0x01000,  19},          /* flash sector 19 - 4kb                       */
  { 0x00014000, 0x01000,  20},          /* flash sector 20 - 4kb                       */
  { 0x00015000, 0x01000,  21},          /* flash sector 21 - 4kb                       */
  { 0x00016000, 0x01000,  22},          /* flash sector 22 - 4kb                       */
  { 0x00017000, 0x01000,  23},          /* flash sector 23 - 4kb                       */
  { 0x00018000, 0x01000,  24},          /* flash sector 24 - 4kb                       */
  { 0x00019000, 0x01000,  25},          /* flash sector 25 - 4kb                       */
  { 0x0001A000, 0x01000,  26},          /* flash sector 26 - 4kb                       */
  { 0x0001B000, 0x01000,  27},          /* flash sector 27 - 4kb                       */
  { 0x0001C000, 0x01000,  28},          /* flash sector 28 - 4kb                       */
  { 0x0001D000, 0x01000,  29},          /* flash sector 29 - 4kb                       */
  { 0x0001E000, 0x01000,  30},          /* flash sector 30 - 4kb                       */
  { 0x0001F000, 0x01000,  31},          /* flash sector 31 - 4kb                       */
  { 0x00020000, 0x01000,  32},          /* flash sector 32 - 4kb                       */
  { 0x00021000, 0x01000,  33},          /* flash sector 33 - 4kb                       */
  { 0x00022000, 0x01000,  34},          /* flash sector 34 - 4kb                       */
  { 0x00023000, 0x01000,  35},          /* flash sector 35 - 4kb                       */
  { 0x00024000, 0x01000,  36},          /* flash sector 36 - 4kb                       */
  { 0x00025000, 0x01000,  37},          /* flash sector 37 - 4kb                       */
  { 0x00026000, 0x01000,  38},          /* flash sector 38 - 4kb                       */
  { 0x00027000, 0x01000,  39},          /* flash sector 39 - 4kb                       */
  { 0x00028000, 0x01000,  40},          /* flash sector 40 - 4kb                       */
  { 0x00029000, 0x01000,  41},          /* flash sector 41 - 4kb                       */
  { 0x0002A000, 0x01000,  42},          /* flash sector 42 - 4kb                       */
  { 0x0002B000, 0x01000,  43},          /* flash sector 43 - 4kb                       */
  { 0x0002C000, 0x01000,  44},          /* flash sector 44 - 4kb                       */
  { 0x0002D000, 0x01000,  45},          /* flash sector 45 - 4kb                       */
  { 0x0002E000, 0x01000,  46},          /* flash sector 46 - 4kb                       */
  { 0x0002F000, 0x01000,  47},          /* flash sector 47 - 4kb                       */
  { 0x00030000, 0x01000,  48},          /* flash sector 48 - 4kb                       */
  { 0x00031000, 0x01000,  49},          /* flash sector 49 - 4kb                       */
  { 0x00032000, 0x01000,  50},          /* flash sector 50 - 4kb                       */
  { 0x00033000, 0x01000,  51},          /* flash sector 51 - 4kb                       */
  { 0x00034000, 0x01000,  52},          /* flash sector 52 - 4kb                       */
  { 0x00035000, 0x01000,  53},          /* flash sector 53 - 4kb                       */
  { 0x00036000, 0x01000,  54},          /* flash sector 54 - 4kb                       */
  { 0x00037000, 0x01000,  55},          /* flash sector 55 - 4kb                       */
  { 0x00038000, 0x01000,  56},          /* flash sector 56 - 4kb                       */
  { 0x00039000, 0x01000,  57},          /* flash sector 57 - 4kb                       */
  { 0x0003A000, 0x01000,  58},          /* flash sector 58 - 4kb                       */
  { 0x0003B000, 0x01000,  59},          /* flash sector 59 - 4kb                       */
  { 0x0003C000, 0x01000,  60},          /* flash sector 60 - 4kb                       */
  { 0x0003D000, 0x01000,  61},          /* flash sector 61 - 4kb                       */
  { 0x0003E000, 0x01000,  62},          /* flash sector 62 - 4kb                       */
  { 0x0003F000, 0x01000,  63},          /* flash sector 63 - 4kb                       */
  { 0x00040000, 0x01000,  64},          /* flash sector 64 - 4kb                       */
  { 0x00041000, 0x01000,  65},          /* flash sector 65 - 4kb                       */
  { 0x00042000, 0x01000,  66},          /* flash sector 66 - 4kb                       */
  { 0x00043000, 0x01000,  67},          /* flash sector 67 - 4kb                       */
  { 0x00044000, 0x01000,  68},          /* flash sector 68 - 4kb                       */
  { 0x00045000, 0x01000,  69},          /* flash sector 69 - 4kb                       */
  { 0x00046000, 0x01000,  70},          /* flash sector 70 - 4kb                       */
  { 0x00047000, 0x01000,  71},          /* flash sector 71 - 4kb                       */
  { 0x00048000, 0x01000,  72},          /* flash sector 72 - 4kb                       */
  { 0x00049000, 0x01000,  73},          /* flash sector 73 - 4kb                       */
  { 0x0004A000, 0x01000,  74},          /* flash sector 74 - 4kb                       */
  { 0x0004B000, 0x01000,  75},          /* flash sector 75 - 4kb                       */
  { 0x0004C000, 0x01000,  76},          /* flash sector 76 - 4kb                       */
  { 0x0004D000, 0x01000,  77},          /* flash sector 77 - 4kb                       */
  { 0x0004E000, 0x01000,  78},          /* flash sector 78 - 4kb                       */
  { 0x0004F000, 0x01000,  79},          /* flash sector 79 - 4kb                       */
  { 0x00050000, 0x01000,  80},          /* flash sector 80 - 4kb                       */
  { 0x00051000, 0x01000,  81},          /* flash sector 81 - 4kb                       */
  { 0x00052000, 0x01000,  82},          /* flash sector 82 - 4kb                       */
  { 0x00053000, 0x01000,  83},          /* flash sector 83 - 4kb                       */
  { 0x00054000, 0x01000,  84},          /* flash sector 84 - 4kb                       */
  { 0x00055000, 0x01000,  85},          /* flash sector 85 - 4kb                       */
  { 0x00056000, 0x01000,  86},          /* flash sector 86 - 4kb                       */
  { 0x00057000, 0x01000,  87},          /* flash sector 87 - 4kb                       */
  { 0x00058000, 0x01000,  88},          /* flash sector 88 - 4kb                       */
  { 0x00059000, 0x01000,  89},          /* flash sector 89 - 4kb                       */
  { 0x0005A000, 0x01000,  90},          /* flash sector 90 - 4kb                       */
  { 0x0005B000, 0x01000,  91},          /* flash sector 91 - 4kb                       */
  { 0x0005C000, 0x01000,  92},          /* flash sector 92 - 4kb                       */
  { 0x0005D000, 0x01000,  93},          /* flash sector 93 - 4kb                       */
  { 0x0005E000, 0x01000,  94},          /* flash sector 94 - 4kb                       */
  { 0x0005F000, 0x01000,  95},          /* flash sector 95 - 4kb                       */
  { 0x00060000, 0x01000,  96},          /* flash sector 96 - 4kb                       */
  { 0x00061000, 0x01000,  97},          /* flash sector 97 - 4kb                       */
  { 0x00062000, 0x01000,  98},          /* flash sector 98 - 4kb                       */
  { 0x00063000, 0x01000,  99},          /* flash sector 99 - 4kb                       */
  { 0x00064000, 0x01000, 100},          /* flash sector 100 - 4kb                      */
  { 0x00065000, 0x01000, 101},          /* flash sector 101 - 4kb                      */
  { 0x00066000, 0x01000, 102},          /* flash sector 102 - 4kb                      */
  { 0x00067000, 0x01000, 103},          /* flash sector 103 - 4kb                      */
  { 0x00068000, 0x01000, 104},          /* flash sector 104 - 4kb                      */
  { 0x00069000, 0x01000, 105},          /* flash sector 105 - 4kb                      */
  { 0x0006A000, 0x01000, 106},          /* flash sector 106 - 4kb                      */
  { 0x0006B000, 0x01000, 107},          /* flash sector 107 - 4kb                      */
  { 0x0006C000, 0x01000, 108},          /* flash sector 108 - 4kb                      */
  { 0x0006D000, 0x01000, 109},          /* flash sector 109 - 4kb                      */
  { 0x0006E000, 0x01000, 110},          /* flash sector 110 - 4kb                      */
  { 0x0006F000, 0x01000, 111},          /* flash sector 111 - 4kb                      */
  { 0x00070000, 0x01000, 112},          /* flash sector 112 - 4kb                      */
  { 0x00071000, 0x01000, 113},          /* flash sector 113 - 4kb                      */
  { 0x00072000, 0x01000, 114},          /* flash sector 114 - 4kb                      */
  { 0x00073000, 0x01000, 115},          /* flash sector 115 - 4kb                      */
  { 0x00074000, 0x01000, 116},          /* flash sector 116 - 4kb                      */
  { 0x00075000, 0x01000, 117},          /* flash sector 117 - 4kb                      */
  { 0x00076000, 0x01000, 118},          /* flash sector 118 - 4kb                      */
  { 0x00077000, 0x01000, 119},          /* flash sector 119 - 4kb                      */
  { 0x00078000, 0x01000, 120},          /* flash sector 120 - 4kb                      */
  { 0x00079000, 0x01000, 121},          /* flash sector 121 - 4kb                      */
  { 0x0007A000, 0x01000, 122},          /* flash sector 122 - 4kb                      */
  { 0x0007B000, 0x01000, 123},          /* flash sector 123 - 4kb                      */
  { 0x0007C000, 0x01000, 124},          /* flash sector 124 - 4kb                      */
  { 0x0007D000, 0x01000, 125},          /* flash sector 125 - 4kb                      */
  { 0x0007E000, 0x01000, 126},          /* flash sector 126 - 4kb                      */
  { 0x0007F000, 0x01000, 127},          /* flash sector 127 - 4kb                      */
  #if (BOOT_NVM_SIZE_KB > 512)
  { 0x00080000, 0x01000, 128},          /* flash sector 128 - 4kb                      */
  { 0x00081000, 0x01000, 129},          /* flash sector 129 - 4kb                      */
  { 0x00082000, 0x01000, 130},          /* flash sector 130 - 4kb                      */
  { 0x00083000, 0x01000, 131},          /* flash sector 131 - 4kb                      */
  { 0x00084000, 0x01000, 132},          /* flash sector 132 - 4kb                      */
  { 0x00085000, 0x01000, 133},          /* flash sector 133 - 4kb                      */
  { 0x00086000, 0x01000, 134},          /* flash sector 134 - 4kb                      */
  { 0x00087000, 0x01000, 135},          /* flash sector 135 - 4kb                      */
  { 0x00088000, 0x01000, 136},          /* flash sector 136 - 4kb                      */
  { 0x00089000, 0x01000, 137},          /* flash sector 137 - 4kb                      */
  { 0x0008A000, 0x01000, 138},          /* flash sector 138 - 4kb                      */
  { 0x0008B000, 0x01000, 139},          /* flash sector 139 - 4kb                      */
  { 0x0008C000, 0x01000, 140},          /* flash sector 140 - 4kb                      */
  { 0x0008D000, 0x01000, 141},          /* flash sector 141 - 4kb                      */
  { 0x0008E000, 0x01000, 142},          /* flash sector 142 - 4kb                      */
  { 0x0008F000, 0x01000, 143},          /* flash sector 143 - 4kb                      */
  { 0x00090000, 0x01000, 144},          /* flash sector 144 - 4kb                      */
  { 0x00091000, 0x01000, 145},          /* flash sector 145 - 4kb                      */
  { 0x00092000, 0x01000, 146},          /* flash sector 146 - 4kb                      */
  { 0x00093000, 0x01000, 147},          /* flash sector 147 - 4kb                      */
  { 0x00094000, 0x01000, 148},          /* flash sector 148 - 4kb                      */
  { 0x00095000, 0x01000, 149},          /* flash sector 149 - 4kb                      */
  { 0x00096000, 0x01000, 150},          /* flash sector 150 - 4kb                      */
  { 0x00097000, 0x01000, 151},          /* flash sector 151 - 4kb                      */
  { 0x00098000, 0x01000, 152},          /* flash sector 152 - 4kb                      */
  { 0x00099000, 0x01000, 153},          /* flash sector 153 - 4kb                      */
  { 0x0009A000, 0x01000, 154},          /* flash sector 154 - 4kb                      */
  { 0x0009B000, 0x01000, 155},          /* flash sector 155 - 4kb                      */
  { 0x0009C000, 0x01000, 156},          /* flash sector 156 - 4kb                      */
  { 0x0009D000, 0x01000, 157},          /* flash sector 157 - 4kb                      */
  { 0x0009E000, 0x01000, 158},          /* flash sector 158 - 4kb                      */
  { 0x0009F000, 0x01000, 159},          /* flash sector 159 - 4kb                      */
  { 0x000A0000, 0x01000, 160},          /* flash sector 160 - 4kb                      */
  { 0x000A1000, 0x01000, 161},          /* flash sector 161 - 4kb                      */
  { 0x000A2000, 0x01000, 162},          /* flash sector 162 - 4kb                      */
  { 0x000A3000, 0x01000, 163},          /* flash sector 163 - 4kb                      */
  { 0x000A4000, 0x01000, 164},          /* flash sector 164 - 4kb                      */
  { 0x000A5000, 0x01000, 165},          /* flash sector 165 - 4kb                      */
  { 0x000A6000, 0x01000, 166},          /* flash sector 166 - 4kb                      */
  { 0x000A7000, 0x01000, 167},          /* flash sector 167 - 4kb                      */
  { 0x000A8000, 0x01000, 168},          /* flash sector 168 - 4kb                      */
  { 0x000A9000, 0x01000, 169},          /* flash sector 169 - 4kb                      */
  { 0x000AA000, 0x01000, 170},          /* flash sector 170 - 4kb                      */
  { 0x000AB000, 0x01000, 171},          /* flash sector 171 - 4kb                      */
  { 0x000AC000, 0x01000, 172},          /* flash sector 172 - 4kb                      */
  { 0x000AD000, 0x01000, 173},          /* flash sector 173 - 4kb                      */
  { 0x000AE000, 0x01000, 174},          /* flash sector 174 - 4kb                      */
  { 0x000AF000, 0x01000, 175},          /* flash sector 175 - 4kb                      */
  { 0x000B0000, 0x01000, 176},          /* flash sector 176 - 4kb                      */
  { 0x000B1000, 0x01000, 177},          /* flash sector 177 - 4kb                      */
  { 0x000B2000, 0x01000, 178},          /* flash sector 178 - 4kb                      */
  { 0x000B3000, 0x01000, 179},          /* flash sector 179 - 4kb                      */
  { 0x000B4000, 0x01000, 180},          /* flash sector 180 - 4kb                      */
  { 0x000B5000, 0x01000, 181},          /* flash sector 181 - 4kb                      */
  { 0x000B6000, 0x01000, 182},          /* flash sector 182 - 4kb                      */
  { 0x000B7000, 0x01000, 183},          /* flash sector 183 - 4kb                      */
  { 0x000B8000, 0x01000, 184},          /* flash sector 184 - 4kb                      */
  { 0x000B9000, 0x01000, 185},          /* flash sector 185 - 4kb                      */
  { 0x000BA000, 0x01000, 186},          /* flash sector 186 - 4kb                      */
  { 0x000BB000, 0x01000, 187},          /* flash sector 187 - 4kb                      */
  { 0x000BC000, 0x01000, 188},          /* flash sector 188 - 4kb                      */
  { 0x000BD000, 0x01000, 189},          /* flash sector 189 - 4kb                      */
  { 0x000BE000, 0x01000, 190},          /* flash sector 190 - 4kb                      */
  { 0x000BF000, 0x01000, 191},          /* flash sector 191 - 4kb                      */
  { 0x000C0000, 0x01000, 192},          /* flash sector 192 - 4kb                      */
  { 0x000C1000, 0x01000, 193},          /* flash sector 193 - 4kb                      */
  { 0x000C2000, 0x01000, 194},          /* flash sector 194 - 4kb                      */
  { 0x000C3000, 0x01000, 195},          /* flash sector 195 - 4kb                      */
  { 0x000C4000, 0x01000, 196},          /* flash sector 196 - 4kb                      */
  { 0x000C5000, 0x01000, 197},          /* flash sector 197 - 4kb                      */
  { 0x000C6000, 0x01000, 198},          /* flash sector 198 - 4kb                      */
  { 0x000C7000, 0x01000, 199},          /* flash sector 199 - 4kb                      */
  { 0x000C8000, 0x01000, 200},          /* flash sector 200 - 4kb                      */
  { 0x000C9000, 0x01000, 201},          /* flash sector 201 - 4kb                      */
  { 0x000CA000, 0x01000, 202},          /* flash sector 202 - 4kb                      */
  { 0x000CB000, 0x01000, 203},          /* flash sector 203 - 4kb                      */
  { 0x000CC000, 0x01000, 204},          /* flash sector 204 - 4kb                      */
  { 0x000CD000, 0x01000, 205},          /* flash sector 205 - 4kb                      */
  { 0x000CE000, 0x01000, 206},          /* flash sector 206 - 4kb                      */
  { 0x000CF000, 0x01000, 207},          /* flash sector 207 - 4kb                      */
  { 0x000D0000, 0x01000, 208},          /* flash sector 208 - 4kb                      */
  { 0x000D1000, 0x01000, 209},          /* flash sector 209 - 4kb                      */
  { 0x000D2000, 0x01000, 210},          /* flash sector 210 - 4kb                      */
  { 0x000D3000, 0x01000, 211},          /* flash sector 211 - 4kb                      */
  { 0x000D4000, 0x01000, 212},          /* flash sector 212 - 4kb                      */
  { 0x000D5000, 0x01000, 213},          /* flash sector 213 - 4kb                      */
  { 0x000D6000, 0x01000, 214},          /* flash sector 214 - 4kb                      */
  { 0x000D7000, 0x01000, 215},          /* flash sector 215 - 4kb                      */
  { 0x000D8000, 0x01000, 216},          /* flash sector 216 - 4kb                      */
  { 0x000D9000, 0x01000, 217},          /* flash sector 217 - 4kb                      */
  { 0x000DA000, 0x01000, 218},          /* flash sector 218 - 4kb                      */
  { 0x000DB000, 0x01000, 219},          /* flash sector 219 - 4kb                      */
  { 0x000DC000, 0x01000, 220},          /* flash sector 220 - 4kb                      */
  { 0x000DD000, 0x01000, 221},          /* flash sector 221 - 4kb                      */
  { 0x000DE000, 0x01000, 222},          /* flash sector 222 - 4kb                      */
  { 0x000DF000, 0x01000, 223},          /* flash sector 223 - 4kb                      */
  { 0x000E0000, 0x01000, 224},          /* flash sector 224 - 4kb                      */
  { 0x000E1000, 0x01000, 225},          /* flash sector 225 - 4kb                      */
  { 0x000E2000, 0x01000, 226},          /* flash sector 226 - 4kb                      */
  { 0x000E3000, 0x01000, 227},          /* flash sector 227 - 4kb                      */
  { 0x000E4000, 0x01000, 228},          /* flash sector 228 - 4kb                      */
  { 0x000E5000, 0x01000, 229},          /* flash sector 229 - 4kb                      */
  { 0x000E6000, 0x01000, 230},          /* flash sector 230 - 4kb                      */
  { 0x000E7000, 0x01000, 231},          /* flash sector 231 - 4kb                      */
  { 0x000E8000, 0x01000, 232},          /* flash sector 232 - 4kb                      */
  { 0x000E9000, 0x01000, 233},          /* flash sector 233 - 4kb                      */
  { 0x000EA000, 0x01000, 234},          /* flash sector 234 - 4kb                      */
  { 0x000EB000, 0x01000, 235},          /* flash sector 235 - 4kb                      */
  { 0x000EC000, 0x01000, 236},          /* flash sector 236 - 4kb                      */
  { 0x000ED000, 0x01000, 237},          /* flash sector 237 - 4kb                      */
  { 0x000EE000, 0x01000, 238},          /* flash sector 238 - 4kb                      */
  { 0x000EF000, 0x01000, 239},          /* flash sector 239 - 4kb                      */
  { 0x000F0000, 0x01000, 240},          /* flash sector 240 - 4kb                      */
  { 0x000F1000, 0x01000, 241},          /* flash sector 241 - 4kb                      */
  { 0x000F2000, 0x01000, 242},          /* flash sector 242 - 4kb                      */
  { 0x000F3000, 0x01000, 243},          /* flash sector 243 - 4kb                      */
  { 0x000F4000, 0x01000, 244},          /* flash sector 244 - 4kb                      */
  { 0x000F5000, 0x01000, 245},          /* flash sector 245 - 4kb                      */
  { 0x000F6000, 0x01000, 246},          /* flash sector 246 - 4kb                      */
  { 0x000F7000, 0x01000, 247},          /* flash sector 247 - 4kb                      */
  { 0x000F8000, 0x01000, 248},          /* flash sector 248 - 4kb                      */
  { 0x000F9000, 0x01000, 249},          /* flash sector 249 - 4kb                      */
  { 0x000FA000, 0x01000, 250},          /* flash sector 250 - 4kb                      */
  { 0x000FB000, 0x01000, 251},          /* flash sector 251 - 4kb                      */
  { 0x000FC000, 0x01000, 252},          /* flash sector 252 - 4kb                      */
  { 0x000FD000, 0x01000, 253},          /* flash sector 253 - 4kb                      */
  { 0x000FE000, 0x01000, 254},          /* flash sector 254 - 4kb                      */
  { 0x000FF000, 0x01000, 255},          /* flash sector 255 - 4kb                      */
  #endif
  #if (BOOT_NVM_SIZE_KB > 1024)
  { 0x00100000, 0x01000, 256},          /* flash sector 256 - 4kb                      */
  { 0x00101000, 0x01000, 257},          /* flash sector 257 - 4kb                      */
  { 0x00102000, 0x01000, 258},          /* flash sector 258 - 4kb                      */
  { 0x00103000, 0x01000, 259},          /* flash sector 259 - 4kb                      */
  { 0x00104000, 0x01000, 260},          /* flash sector 260 - 4kb                      */
  { 0x00105000, 0x01000, 261},          /* flash sector 261 - 4kb                      */
  { 0x00106000, 0x01000, 262},          /* flash sector 262 - 4kb                      */
  { 0x00107000, 0x01000, 263},          /* flash sector 263 - 4kb                      */
  { 0x00108000, 0x01000, 264},          /* flash sector 264 - 4kb                      */
  { 0x00109000, 0x01000, 265},          /* flash sector 265 - 4kb                      */
  { 0x0010A000, 0x01000, 266},          /* flash sector 266 - 4kb                      */
  { 0x0010B000, 0x01000, 267},          /* flash sector 267 - 4kb                      */
  { 0x0010C000, 0x01000, 268},          /* flash sector 268 - 4kb                      */
  { 0x0010D000, 0x01000, 269},          /* flash sector 269 - 4kb                      */
  { 0x0010E000, 0x01000, 270},          /* flash sector 270 - 4kb                      */
  { 0x0010F000, 0x01000, 271},          /* flash sector 271 - 4kb                      */
  { 0x00110000, 0x01000, 272},          /* flash sector 272 - 4kb                      */
  { 0x00111000, 0x01000, 273},          /* flash sector 273 - 4kb                      */
  { 0x00112000, 0x01000, 274},          /* flash sector 274 - 4kb                      */
  { 0x00113000, 0x01000, 275},          /* flash sector 275 - 4kb                      */
  { 0x00114000, 0x01000, 276},          /* flash sector 276 - 4kb                      */
  { 0x00115000, 0x01000, 277},          /* flash sector 277 - 4kb                      */
  { 0x00116000, 0x01000, 278},          /* flash sector 278 - 4kb                      */
  { 0x00117000, 0x01000, 279},          /* flash sector 279 - 4kb                      */
  { 0x00118000, 0x01000, 280},          /* flash sector 280 - 4kb                      */
  { 0x00119000, 0x01000, 281},          /* flash sector 281 - 4kb                      */
  { 0x0011A000, 0x01000, 282},          /* flash sector 282 - 4kb                      */
  { 0x0011B000, 0x01000, 283},          /* flash sector 283 - 4kb                      */
  { 0x0011C000, 0x01000, 284},          /* flash sector 284 - 4kb                      */
  { 0x0011D000, 0x01000, 285},          /* flash sector 285 - 4kb                      */
  { 0x0011E000, 0x01000, 286},          /* flash sector 286 - 4kb                      */
  { 0x0011F000, 0x01000, 287},          /* flash sector 287 - 4kb                      */
  { 0x00120000, 0x01000, 288},          /* flash sector 288 - 4kb                      */
  { 0x00121000, 0x01000, 289},          /* flash sector 289 - 4kb                      */
  { 0x00122000, 0x01000, 290},          /* flash sector 290 - 4kb                      */
  { 0x00123000, 0x01000, 291},          /* flash sector 291 - 4kb                      */
  { 0x00124000, 0x01000, 292},          /* flash sector 292 - 4kb                      */
  { 0x00125000, 0x01000, 293},          /* flash sector 293 - 4kb                      */
  { 0x00126000, 0x01000, 294},          /* flash sector 294 - 4kb                      */
  { 0x00127000, 0x01000, 295},          /* flash sector 295 - 4kb                      */
  { 0x00128000, 0x01000, 296},          /* flash sector 296 - 4kb                      */
  { 0x00129000, 0x01000, 297},          /* flash sector 297 - 4kb                      */
  { 0x0012A000, 0x01000, 298},          /* flash sector 298 - 4kb                      */
  { 0x0012B000, 0x01000, 299},          /* flash sector 299 - 4kb                      */
  { 0x0012C000, 0x01000, 300},          /* flash sector 300 - 4kb                      */
  { 0x0012D000, 0x01000, 301},          /* flash sector 301 - 4kb                      */
  { 0x0012E000, 0x01000, 302},          /* flash sector 302 - 4kb                      */
  { 0x0012F000, 0x01000, 303},          /* flash sector 303 - 4kb                      */
  { 0x00130000, 0x01000, 304},          /* flash sector 304 - 4kb                      */
  { 0x00131000, 0x01000, 305},          /* flash sector 305 - 4kb                      */
  { 0x00132000, 0x01000, 306},          /* flash sector 306 - 4kb                      */
  { 0x00133000, 0x01000, 307},          /* flash sector 307 - 4kb                      */
  { 0x00134000, 0x01000, 308},          /* flash sector 308 - 4kb                      */
  { 0x00135000, 0x01000, 309},          /* flash sector 309 - 4kb                      */
  { 0x00136000, 0x01000, 310},          /* flash sector 310 - 4kb                      */
  { 0x00137000, 0x01000, 311},          /* flash sector 311 - 4kb                      */
  { 0x00138000, 0x01000, 312},          /* flash sector 312 - 4kb                      */
  { 0x00139000, 0x01000, 313},          /* flash sector 313 - 4kb                      */
  { 0x0013A000, 0x01000, 314},          /* flash sector 314 - 4kb                      */
  { 0x0013B000, 0x01000, 315},          /* flash sector 315 - 4kb                      */
  { 0x0013C000, 0x01000, 316},          /* flash sector 316 - 4kb                      */
  { 0x0013D000, 0x01000, 317},          /* flash sector 317 - 4kb                      */
  { 0x0013E000, 0x01000, 318},          /* flash sector 318 - 4kb                      */
  { 0x0013F000, 0x01000, 319},          /* flash sector 319 - 4kb                      */
  { 0x00140000, 0x01000, 320},          /* flash sector 320 - 4kb                      */
  { 0x00141000, 0x01000, 321},          /* flash sector 321 - 4kb                      */
  { 0x00142000, 0x01000, 322},          /* flash sector 322 - 4kb                      */
  { 0x00143000, 0x01000, 323},          /* flash sector 323 - 4kb                      */
  { 0x00144000, 0x01000, 324},          /* flash sector 324 - 4kb                      */
  { 0x00145000, 0x01000, 325},          /* flash sector 325 - 4kb                      */
  { 0x00146000, 0x01000, 326},          /* flash sector 326 - 4kb                      */
  { 0x00147000, 0x01000, 327},          /* flash sector 327 - 4kb                      */
  { 0x00148000, 0x01000, 328},          /* flash sector 328 - 4kb                      */
  { 0x00149000, 0x01000, 329},          /* flash sector 329 - 4kb                      */
  { 0x0014A000, 0x01000, 330},          /* flash sector 330 - 4kb                      */
  { 0x0014B000, 0x01000, 331},          /* flash sector 331 - 4kb                      */
  { 0x0014C000, 0x01000, 332},          /* flash sector 332 - 4kb                      */
  { 0x0014D000, 0x01000, 333},          /* flash sector 333 - 4kb                      */
  { 0x0014E000, 0x01000, 334},          /* flash sector 334 - 4kb                      */
  { 0x0014F000, 0x01000, 335},          /* flash sector 335 - 4kb                      */
  { 0x00150000, 0x01000, 336},          /* flash sector 336 - 4kb                      */
  { 0x00151000, 0x01000, 337},          /* flash sector 337 - 4kb                      */
  { 0x00152000, 0x01000, 338},          /* flash sector 338 - 4kb                      */
  { 0x00153000, 0x01000, 339},          /* flash sector 339 - 4kb                      */
  { 0x00154000, 0x01000, 340},          /* flash sector 340 - 4kb                      */
  { 0x00155000, 0x01000, 341},          /* flash sector 341 - 4kb                      */
  { 0x00156000, 0x01000, 342},          /* flash sector 342 - 4kb                      */
  { 0x00157000, 0x01000, 343},          /* flash sector 343 - 4kb                      */
  { 0x00158000, 0x01000, 344},          /* flash sector 344 - 4kb                      */
  { 0x00159000, 0x01000, 345},          /* flash sector 345 - 4kb                      */
  { 0x0015A000, 0x01000, 346},          /* flash sector 346 - 4kb                      */
  { 0x0015B000, 0x01000, 347},          /* flash sector 347 - 4kb                      */
  { 0x0015C000, 0x01000, 348},          /* flash sector 348 - 4kb                      */
  { 0x0015D000, 0x01000, 349},          /* flash sector 349 - 4kb                      */
  { 0x0015E000, 0x01000, 350},          /* flash sector 350 - 4kb                      */
  { 0x0015F000, 0x01000, 351},          /* flash sector 351 - 4kb                      */
  { 0x00160000, 0x01000, 352},          /* flash sector 352 - 4kb                      */
  { 0x00161000, 0x01000, 353},          /* flash sector 353 - 4kb                      */
  { 0x00162000, 0x01000, 354},          /* flash sector 354 - 4kb                      */
  { 0x00163000, 0x01000, 355},          /* flash sector 355 - 4kb                      */
  { 0x00164000, 0x01000, 356},          /* flash sector 356 - 4kb                      */
  { 0x00165000, 0x01000, 357},          /* flash sector 357 - 4kb                      */
  { 0x00166000, 0x01000, 358},          /* flash sector 358 - 4kb                      */
  { 0x00167000, 0x01000, 359},          /* flash sector 359 - 4kb                      */
  { 0x00168000, 0x01000, 360},          /* flash sector 360 - 4kb                      */
  { 0x00169000, 0x01000, 361},          /* flash sector 361 - 4kb                      */
  { 0x0016A000, 0x01000, 362},          /* flash sector 362 - 4kb                      */
  { 0x0016B000, 0x01000, 363},          /* flash sector 363 - 4kb                      */
  { 0x0016C000, 0x01000, 364},          /* flash sector 364 - 4kb                      */
  { 0x0016D000, 0x01000, 365},          /* flash sector 365 - 4kb                      */
  { 0x0016E000, 0x01000, 366},          /* flash sector 366 - 4kb                      */
  { 0x0016F000, 0x01000, 367},          /* flash sector 367 - 4kb                      */
  { 0x00170000, 0x01000, 368},          /* flash sector 368 - 4kb                      */
  { 0x00171000, 0x01000, 369},          /* flash sector 369 - 4kb                      */
  { 0x00172000, 0x01000, 370},          /* flash sector 370 - 4kb                      */
  { 0x00173000, 0x01000, 371},          /* flash sector 371 - 4kb                      */
  { 0x00174000, 0x01000, 372},          /* flash sector 372 - 4kb                      */
  { 0x00175000, 0x01000, 373},          /* flash sector 373 - 4kb                      */
  { 0x00176000, 0x01000, 374},          /* flash sector 374 - 4kb                      */
  { 0x00177000, 0x01000, 375},          /* flash sector 375 - 4kb                      */
  { 0x00178000, 0x01000, 376},          /* flash sector 376 - 4kb                      */
  { 0x00179000, 0x01000, 377},          /* flash sector 377 - 4kb                      */
  { 0x0017A000, 0x01000, 378},          /* flash sector 378 - 4kb                      */
  { 0x0017B000, 0x01000, 379},          /* flash sector 379 - 4kb                      */
  { 0x0017C000, 0x01000, 380},          /* flash sector 380 - 4kb                      */
  { 0x0017D000, 0x01000, 381},          /* flash sector 381 - 4kb                      */
  { 0x0017E000, 0x01000, 382},          /* flash sector 382 - 4kb                      */
  { 0x0017F000, 0x01000, 383},          /* flash sector 383 - 4kb                      */
  #endif
  #if (BOOT_NVM_SIZE_KB > 1536)
  #error "BOOT_NVM_SIZE_KB > 1536 is currently not supported."
  #endif
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
