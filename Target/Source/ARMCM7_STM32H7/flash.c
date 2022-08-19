/************************************************************************************//**
* \file         Source/ARMCM7_STM32H7/flash.c
* \brief        Bootloader flash driver source file.
* \ingroup      Target_ARMCM7_STM32H7
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
#include "stm32h7xx.h"                           /* STM32 CPU and HAL header           */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Value for an invalid sector entry index into flashLayout[]. */
#define FLASH_INVALID_SECTOR_IDX        (0xff)
/** \brief Value for an invalid flash address. */
#define FLASH_INVALID_ADDRESS           (0xffffffff)
/** \brief Standard size of a flash block for writing. */
#ifdef  STM32H7A3xxQ
#define FLASH_WRITE_BLOCK_SIZE          (8192)
#else
#define FLASH_WRITE_BLOCK_SIZE          (1024)
#endif
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
#define BOOT_FLASH_VECTOR_TABLE_CS_OFFSET    (0x298)
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
  blt_int8u  bank_num;                           /**< bank number                      */
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
#if (BOOT_NVM_SIZE_KB < 1024)
  #error "BOOT_NVM_SIZE_KB < 1024 is currently not supported."
#elif (BOOT_NVM_SIZE_KB == 1024)
    /* { 0x08000000, 0x20000, 0, 1 },     flash sector idx 0 - reserved for bootloader */
    { 0x08020000, 0x20000, 1, 1},        /* flash sector idx  1 - 128kb                */
    { 0x08040000, 0x20000, 2, 1},        /* flash sector idx  2 - 128kb                */
    { 0x08060000, 0x20000, 3, 1},        /* flash sector idx  3 - 128kb                */
    /* Note that there is a gap here. */
    { 0x08100000, 0x20000, 0, 2},        /* flash sector idx  4 - 128kb                */
    { 0x08120000, 0x20000, 1, 2},        /* flash sector idx  5 - 128kb                */
    { 0x08140000, 0x20000, 2, 2},        /* flash sector idx  6 - 128kb                */
    { 0x08160000, 0x20000, 3, 2},        /* flash sector idx  7 - 128kb                */
#elif (BOOT_NVM_SIZE_KB == 2048)
#ifdef  STM32H7A3xxQ
//	{0x8000000,0x2000,0,1},         /* flash sector idx 0 - 8kb     - reserved for bootloader    */
//	{0x8002000,0x2000,1,1},         /* flash sector idx 1 - 8kb     - reserved for bootloader    */
//	{0x8004000,0x2000,2,1},         /* flash sector idx 2 - 8kb     - reserved for bootloader    */
//	{0x8006000,0x2000,3,1},         /* flash sector idx 3 - 8kb     - reserved for bootloader    */
	{0x8008000,0x2000,4,1},         /* flash sector idx 4 - 8kb         */
	{0x800A000,0x2000,5,1},         /* flash sector idx 5 - 8kb         */
	{0x800C000,0x2000,6,1},         /* flash sector idx 6 - 8kb         */
	{0x800E000,0x2000,7,1},         /* flash sector idx 7 - 8kb         */
	{0x8010000,0x2000,8,1},         /* flash sector idx 8 - 8kb         */
	{0x8012000,0x2000,9,1},         /* flash sector idx 9 - 8kb         */
	{0x8014000,0x2000,10,1},        /* flash sector idx 10 - 8kb        */
	{0x8016000,0x2000,11,1},        /* flash sector idx 11 - 8kb        */
	{0x8018000,0x2000,12,1},        /* flash sector idx 12 - 8kb        */
	{0x801A000,0x2000,13,1},        /* flash sector idx 13 - 8kb        */
	{0x801C000,0x2000,14,1},        /* flash sector idx 14 - 8kb        */
	{0x801E000,0x2000,15,1},        /* flash sector idx 15 - 8kb        */
	{0x8020000,0x2000,16,1},        /* flash sector idx 16 - 8kb        */
	{0x8022000,0x2000,17,1},        /* flash sector idx 17 - 8kb        */
	{0x8024000,0x2000,18,1},        /* flash sector idx 18 - 8kb        */
	{0x8026000,0x2000,19,1},        /* flash sector idx 19 - 8kb        */
	{0x8028000,0x2000,20,1},        /* flash sector idx 20 - 8kb        */
	{0x802A000,0x2000,21,1},        /* flash sector idx 21 - 8kb        */
	{0x802C000,0x2000,22,1},        /* flash sector idx 22 - 8kb        */
	{0x802E000,0x2000,23,1},        /* flash sector idx 23 - 8kb        */
	{0x8030000,0x2000,24,1},        /* flash sector idx 24 - 8kb        */
	{0x8032000,0x2000,25,1},        /* flash sector idx 25 - 8kb        */
	{0x8034000,0x2000,26,1},        /* flash sector idx 26 - 8kb        */
	{0x8036000,0x2000,27,1},        /* flash sector idx 27 - 8kb        */
	{0x8038000,0x2000,28,1},        /* flash sector idx 28 - 8kb        */
	{0x803A000,0x2000,29,1},        /* flash sector idx 29 - 8kb        */
	{0x803C000,0x2000,30,1},        /* flash sector idx 30 - 8kb        */
	{0x803E000,0x2000,31,1},        /* flash sector idx 31 - 8kb        */
	{0x8040000,0x2000,32,1},        /* flash sector idx 32 - 8kb        */
	{0x8042000,0x2000,33,1},        /* flash sector idx 33 - 8kb        */
	{0x8044000,0x2000,34,1},        /* flash sector idx 34 - 8kb        */
	{0x8046000,0x2000,35,1},        /* flash sector idx 35 - 8kb        */
	{0x8048000,0x2000,36,1},        /* flash sector idx 36 - 8kb        */
	{0x804A000,0x2000,37,1},        /* flash sector idx 37 - 8kb        */
	{0x804C000,0x2000,38,1},        /* flash sector idx 38 - 8kb        */
	{0x804E000,0x2000,39,1},        /* flash sector idx 39 - 8kb        */
	{0x8050000,0x2000,40,1},        /* flash sector idx 40 - 8kb        */
	{0x8052000,0x2000,41,1},        /* flash sector idx 41 - 8kb        */
	{0x8054000,0x2000,42,1},        /* flash sector idx 42 - 8kb        */
	{0x8056000,0x2000,43,1},        /* flash sector idx 43 - 8kb        */
	{0x8058000,0x2000,44,1},        /* flash sector idx 44 - 8kb        */
	{0x805A000,0x2000,45,1},        /* flash sector idx 45 - 8kb        */
	{0x805C000,0x2000,46,1},        /* flash sector idx 46 - 8kb        */
	{0x805E000,0x2000,47,1},        /* flash sector idx 47 - 8kb        */
	{0x8060000,0x2000,48,1},        /* flash sector idx 48 - 8kb        */
	{0x8062000,0x2000,49,1},        /* flash sector idx 49 - 8kb        */
	{0x8064000,0x2000,50,1},        /* flash sector idx 50 - 8kb        */
	{0x8066000,0x2000,51,1},        /* flash sector idx 51 - 8kb        */
	{0x8068000,0x2000,52,1},        /* flash sector idx 52 - 8kb        */
	{0x806A000,0x2000,53,1},        /* flash sector idx 53 - 8kb        */
	{0x806C000,0x2000,54,1},        /* flash sector idx 54 - 8kb        */
	{0x806E000,0x2000,55,1},        /* flash sector idx 55 - 8kb        */
	{0x8070000,0x2000,56,1},        /* flash sector idx 56 - 8kb        */
	{0x8072000,0x2000,57,1},        /* flash sector idx 57 - 8kb        */
	{0x8074000,0x2000,58,1},        /* flash sector idx 58 - 8kb        */
	{0x8076000,0x2000,59,1},        /* flash sector idx 59 - 8kb        */
	{0x8078000,0x2000,60,1},        /* flash sector idx 60 - 8kb        */
	{0x807A000,0x2000,61,1},        /* flash sector idx 61 - 8kb        */
	{0x807C000,0x2000,62,1},        /* flash sector idx 62 - 8kb        */
	{0x807E000,0x2000,63,1},        /* flash sector idx 63 - 8kb        */
	{0x8080000,0x2000,64,1},        /* flash sector idx 64 - 8kb        */
	{0x8082000,0x2000,65,1},        /* flash sector idx 65 - 8kb        */
	{0x8084000,0x2000,66,1},        /* flash sector idx 66 - 8kb        */
	{0x8086000,0x2000,67,1},        /* flash sector idx 67 - 8kb        */
	{0x8088000,0x2000,68,1},        /* flash sector idx 68 - 8kb        */
	{0x808A000,0x2000,69,1},        /* flash sector idx 69 - 8kb        */
	{0x808C000,0x2000,70,1},        /* flash sector idx 70 - 8kb        */
	{0x808E000,0x2000,71,1},        /* flash sector idx 71 - 8kb        */
	{0x8090000,0x2000,72,1},        /* flash sector idx 72 - 8kb        */
	{0x8092000,0x2000,73,1},        /* flash sector idx 73 - 8kb        */
	{0x8094000,0x2000,74,1},        /* flash sector idx 74 - 8kb        */
	{0x8096000,0x2000,75,1},        /* flash sector idx 75 - 8kb        */
	{0x8098000,0x2000,76,1},        /* flash sector idx 76 - 8kb        */
	{0x809A000,0x2000,77,1},        /* flash sector idx 77 - 8kb        */
	{0x809C000,0x2000,78,1},        /* flash sector idx 78 - 8kb        */
	{0x809E000,0x2000,79,1},        /* flash sector idx 79 - 8kb        */
	{0x80A0000,0x2000,80,1},        /* flash sector idx 80 - 8kb        */
	{0x80A2000,0x2000,81,1},        /* flash sector idx 81 - 8kb        */
	{0x80A4000,0x2000,82,1},        /* flash sector idx 82 - 8kb        */
	{0x80A6000,0x2000,83,1},        /* flash sector idx 83 - 8kb        */
	{0x80A8000,0x2000,84,1},        /* flash sector idx 84 - 8kb        */
	{0x80AA000,0x2000,85,1},        /* flash sector idx 85 - 8kb        */
	{0x80AC000,0x2000,86,1},        /* flash sector idx 86 - 8kb        */
	{0x80AE000,0x2000,87,1},        /* flash sector idx 87 - 8kb        */
	{0x80B0000,0x2000,88,1},        /* flash sector idx 88 - 8kb        */
	{0x80B2000,0x2000,89,1},        /* flash sector idx 89 - 8kb        */
	{0x80B4000,0x2000,90,1},        /* flash sector idx 90 - 8kb        */
	{0x80B6000,0x2000,91,1},        /* flash sector idx 91 - 8kb        */
	{0x80B8000,0x2000,92,1},        /* flash sector idx 92 - 8kb        */
	{0x80BA000,0x2000,93,1},        /* flash sector idx 93 - 8kb        */
	{0x80BC000,0x2000,94,1},        /* flash sector idx 94 - 8kb        */
	{0x80BE000,0x2000,95,1},        /* flash sector idx 95 - 8kb        */
	{0x80C0000,0x2000,96,1},        /* flash sector idx 96 - 8kb        */
	{0x80C2000,0x2000,97,1},        /* flash sector idx 97 - 8kb        */
	{0x80C4000,0x2000,98,1},        /* flash sector idx 98 - 8kb        */
	{0x80C6000,0x2000,99,1},        /* flash sector idx 99 - 8kb        */
	{0x80C8000,0x2000,100,1},        /* flash sector idx 100 - 8kb        */
	{0x80CA000,0x2000,101,1},        /* flash sector idx 101 - 8kb        */
	{0x80CC000,0x2000,102,1},        /* flash sector idx 102 - 8kb        */
	{0x80CE000,0x2000,103,1},        /* flash sector idx 103 - 8kb        */
	{0x80D0000,0x2000,104,1},        /* flash sector idx 104 - 8kb        */
	{0x80D2000,0x2000,105,1},        /* flash sector idx 105 - 8kb        */
	{0x80D4000,0x2000,106,1},        /* flash sector idx 106 - 8kb        */
	{0x80D6000,0x2000,107,1},        /* flash sector idx 107 - 8kb        */
	{0x80D8000,0x2000,108,1},        /* flash sector idx 108 - 8kb        */
	{0x80DA000,0x2000,109,1},        /* flash sector idx 109 - 8kb        */
	{0x80DC000,0x2000,110,1},        /* flash sector idx 110 - 8kb        */
	{0x80DE000,0x2000,111,1},        /* flash sector idx 111 - 8kb        */
	{0x80E0000,0x2000,112,1},        /* flash sector idx 112 - 8kb        */
	{0x80E2000,0x2000,113,1},        /* flash sector idx 113 - 8kb        */
	{0x80E4000,0x2000,114,1},        /* flash sector idx 114 - 8kb        */
	{0x80E6000,0x2000,115,1},        /* flash sector idx 115 - 8kb        */
	{0x80E8000,0x2000,116,1},        /* flash sector idx 116 - 8kb        */
	{0x80EA000,0x2000,117,1},        /* flash sector idx 117 - 8kb        */
	{0x80EC000,0x2000,118,1},        /* flash sector idx 118 - 8kb        */
	{0x80EE000,0x2000,119,1},        /* flash sector idx 119 - 8kb        */
	{0x80F0000,0x2000,120,1},        /* flash sector idx 120 - 8kb        */
	{0x80F2000,0x2000,121,1},        /* flash sector idx 121 - 8kb        */
	{0x80F4000,0x2000,122,1},        /* flash sector idx 122 - 8kb        */
	{0x80F6000,0x2000,123,1},        /* flash sector idx 123 - 8kb        */
	{0x80F8000,0x2000,124,1},        /* flash sector idx 124 - 8kb        */
	{0x80FA000,0x2000,125,1},        /* flash sector idx 125 - 8kb        */
	{0x80FC000,0x2000,126,1},        /* flash sector idx 126 - 8kb        */
	{0x80FE000,0x2000,127,1},        /* flash sector idx 127 - 8kb        */
	{0x8100000,0x2000,0,2},        /* flash sector idx 0 - 8kb        */
	{0x8102000,0x2000,1,2},        /* flash sector idx 1 - 8kb        */
	{0x8104000,0x2000,2,2},        /* flash sector idx 2 - 8kb        */
	{0x8106000,0x2000,3,2},        /* flash sector idx 3 - 8kb        */
	{0x8108000,0x2000,4,2},        /* flash sector idx 4 - 8kb        */
	{0x810A000,0x2000,5,2},        /* flash sector idx 5 - 8kb        */
	{0x810C000,0x2000,6,2},        /* flash sector idx 6 - 8kb        */
	{0x810E000,0x2000,7,2},        /* flash sector idx 7 - 8kb        */
	{0x8110000,0x2000,8,2},        /* flash sector idx 8 - 8kb        */
	{0x8112000,0x2000,9,2},        /* flash sector idx 9 - 8kb        */
	{0x8114000,0x2000,10,2},        /* flash sector idx 10 - 8kb        */
	{0x8116000,0x2000,11,2},        /* flash sector idx 11 - 8kb        */
	{0x8118000,0x2000,12,2},        /* flash sector idx 12 - 8kb        */
	{0x811A000,0x2000,13,2},        /* flash sector idx 13 - 8kb        */
	{0x811C000,0x2000,14,2},        /* flash sector idx 14 - 8kb        */
	{0x811E000,0x2000,15,2},        /* flash sector idx 15 - 8kb        */
	{0x8120000,0x2000,16,2},        /* flash sector idx 16 - 8kb        */
	{0x8122000,0x2000,17,2},        /* flash sector idx 17 - 8kb        */
	{0x8124000,0x2000,18,2},        /* flash sector idx 18 - 8kb        */
	{0x8126000,0x2000,19,2},        /* flash sector idx 19 - 8kb        */
	{0x8128000,0x2000,20,2},        /* flash sector idx 20 - 8kb        */
	{0x812A000,0x2000,21,2},        /* flash sector idx 21 - 8kb        */
	{0x812C000,0x2000,22,2},        /* flash sector idx 22 - 8kb        */
	{0x812E000,0x2000,23,2},        /* flash sector idx 23 - 8kb        */
	{0x8130000,0x2000,24,2},        /* flash sector idx 24 - 8kb        */
	{0x8132000,0x2000,25,2},        /* flash sector idx 25 - 8kb        */
	{0x8134000,0x2000,26,2},        /* flash sector idx 26 - 8kb        */
	{0x8136000,0x2000,27,2},        /* flash sector idx 27 - 8kb        */
	{0x8138000,0x2000,28,2},        /* flash sector idx 28 - 8kb        */
	{0x813A000,0x2000,29,2},        /* flash sector idx 29 - 8kb        */
	{0x813C000,0x2000,30,2},        /* flash sector idx 30 - 8kb        */
	{0x813E000,0x2000,31,2},        /* flash sector idx 31 - 8kb        */
	{0x8140000,0x2000,32,2},        /* flash sector idx 32 - 8kb        */
	{0x8142000,0x2000,33,2},        /* flash sector idx 33 - 8kb        */
	{0x8144000,0x2000,34,2},        /* flash sector idx 34 - 8kb        */
	{0x8146000,0x2000,35,2},        /* flash sector idx 35 - 8kb        */
	{0x8148000,0x2000,36,2},        /* flash sector idx 36 - 8kb        */
	{0x814A000,0x2000,37,2},        /* flash sector idx 37 - 8kb        */
	{0x814C000,0x2000,38,2},        /* flash sector idx 38 - 8kb        */
	{0x814E000,0x2000,39,2},        /* flash sector idx 39 - 8kb        */
	{0x8150000,0x2000,40,2},        /* flash sector idx 40 - 8kb        */
	{0x8152000,0x2000,41,2},        /* flash sector idx 41 - 8kb        */
	{0x8154000,0x2000,42,2},        /* flash sector idx 42 - 8kb        */
	{0x8156000,0x2000,43,2},        /* flash sector idx 43 - 8kb        */
	{0x8158000,0x2000,44,2},        /* flash sector idx 44 - 8kb        */
	{0x815A000,0x2000,45,2},        /* flash sector idx 45 - 8kb        */
	{0x815C000,0x2000,46,2},        /* flash sector idx 46 - 8kb        */
	{0x815E000,0x2000,47,2},        /* flash sector idx 47 - 8kb        */
	{0x8160000,0x2000,48,2},        /* flash sector idx 48 - 8kb        */
	{0x8162000,0x2000,49,2},        /* flash sector idx 49 - 8kb        */
	{0x8164000,0x2000,50,2},        /* flash sector idx 50 - 8kb        */
	{0x8166000,0x2000,51,2},        /* flash sector idx 51 - 8kb        */
	{0x8168000,0x2000,52,2},        /* flash sector idx 52 - 8kb        */
	{0x816A000,0x2000,53,2},        /* flash sector idx 53 - 8kb        */
	{0x816C000,0x2000,54,2},        /* flash sector idx 54 - 8kb        */
	{0x816E000,0x2000,55,2},        /* flash sector idx 55 - 8kb        */
	{0x8170000,0x2000,56,2},        /* flash sector idx 56 - 8kb        */
	{0x8172000,0x2000,57,2},        /* flash sector idx 57 - 8kb        */
	{0x8174000,0x2000,58,2},        /* flash sector idx 58 - 8kb        */
	{0x8176000,0x2000,59,2},        /* flash sector idx 59 - 8kb        */
	{0x8178000,0x2000,60,2},        /* flash sector idx 60 - 8kb        */
	{0x817A000,0x2000,61,2},        /* flash sector idx 61 - 8kb        */
	{0x817C000,0x2000,62,2},        /* flash sector idx 62 - 8kb        */
	{0x817E000,0x2000,63,2},        /* flash sector idx 63 - 8kb        */
	{0x8180000,0x2000,64,2},        /* flash sector idx 64 - 8kb        */
	{0x8182000,0x2000,65,2},        /* flash sector idx 65 - 8kb        */
	{0x8184000,0x2000,66,2},        /* flash sector idx 66 - 8kb        */
	{0x8186000,0x2000,67,2},        /* flash sector idx 67 - 8kb        */
	{0x8188000,0x2000,68,2},        /* flash sector idx 68 - 8kb        */
	{0x818A000,0x2000,69,2},        /* flash sector idx 69 - 8kb        */
	{0x818C000,0x2000,70,2},        /* flash sector idx 70 - 8kb        */
	{0x818E000,0x2000,71,2},        /* flash sector idx 71 - 8kb        */
	{0x8190000,0x2000,72,2},        /* flash sector idx 72 - 8kb        */
	{0x8192000,0x2000,73,2},        /* flash sector idx 73 - 8kb        */
	{0x8194000,0x2000,74,2},        /* flash sector idx 74 - 8kb        */
	{0x8196000,0x2000,75,2},        /* flash sector idx 75 - 8kb        */
	{0x8198000,0x2000,76,2},        /* flash sector idx 76 - 8kb        */
	{0x819A000,0x2000,77,2},        /* flash sector idx 77 - 8kb        */
	{0x819C000,0x2000,78,2},        /* flash sector idx 78 - 8kb        */
	{0x819E000,0x2000,79,2},        /* flash sector idx 79 - 8kb        */
	{0x81A0000,0x2000,80,2},        /* flash sector idx 80 - 8kb        */
	{0x81A2000,0x2000,81,2},        /* flash sector idx 81 - 8kb        */
	{0x81A4000,0x2000,82,2},        /* flash sector idx 82 - 8kb        */
	{0x81A6000,0x2000,83,2},        /* flash sector idx 83 - 8kb        */
	{0x81A8000,0x2000,84,2},        /* flash sector idx 84 - 8kb        */
	{0x81AA000,0x2000,85,2},        /* flash sector idx 85 - 8kb        */
	{0x81AC000,0x2000,86,2},        /* flash sector idx 86 - 8kb        */
	{0x81AE000,0x2000,87,2},        /* flash sector idx 87 - 8kb        */
	{0x81B0000,0x2000,88,2},        /* flash sector idx 88 - 8kb        */
	{0x81B2000,0x2000,89,2},        /* flash sector idx 89 - 8kb        */
	{0x81B4000,0x2000,90,2},        /* flash sector idx 90 - 8kb        */
	{0x81B6000,0x2000,91,2},        /* flash sector idx 91 - 8kb        */
	{0x81B8000,0x2000,92,2},        /* flash sector idx 92 - 8kb        */
	{0x81BA000,0x2000,93,2},        /* flash sector idx 93 - 8kb        */
	{0x81BC000,0x2000,94,2},        /* flash sector idx 94 - 8kb        */
	{0x81BE000,0x2000,95,2},        /* flash sector idx 95 - 8kb        */
	{0x81C0000,0x2000,96,2},        /* flash sector idx 96 - 8kb        */
	{0x81C2000,0x2000,97,2},        /* flash sector idx 97 - 8kb        */
	{0x81C4000,0x2000,98,2},        /* flash sector idx 98 - 8kb        */
	{0x81C6000,0x2000,99,2},        /* flash sector idx 99 - 8kb        */
	{0x81C8000,0x2000,100,2},        /* flash sector idx 100 - 8kb        */
	{0x81CA000,0x2000,101,2},        /* flash sector idx 101 - 8kb        */
	{0x81CC000,0x2000,102,2},        /* flash sector idx 102 - 8kb        */
	{0x81CE000,0x2000,103,2},        /* flash sector idx 103 - 8kb        */
	{0x81D0000,0x2000,104,2},        /* flash sector idx 104 - 8kb        */
	{0x81D2000,0x2000,105,2},        /* flash sector idx 105 - 8kb        */
	{0x81D4000,0x2000,106,2},        /* flash sector idx 106 - 8kb        */
	{0x81D6000,0x2000,107,2},        /* flash sector idx 107 - 8kb        */
	{0x81D8000,0x2000,108,2},        /* flash sector idx 108 - 8kb        */
	{0x81DA000,0x2000,109,2},        /* flash sector idx 109 - 8kb        */
	{0x81DC000,0x2000,110,2},        /* flash sector idx 110 - 8kb        */
	{0x81DE000,0x2000,111,2},        /* flash sector idx 111 - 8kb        */
	{0x81E0000,0x2000,112,2},        /* flash sector idx 112 - 8kb        */
	{0x81E2000,0x2000,113,2},        /* flash sector idx 113 - 8kb        */
	{0x81E4000,0x2000,114,2},        /* flash sector idx 114 - 8kb        */
	{0x81E6000,0x2000,115,2},        /* flash sector idx 115 - 8kb        */
	{0x81E8000,0x2000,116,2},        /* flash sector idx 116 - 8kb        */
	{0x81EA000,0x2000,117,2},        /* flash sector idx 117 - 8kb        */
	{0x81EC000,0x2000,118,2},        /* flash sector idx 118 - 8kb        */
	{0x81EE000,0x2000,119,2},        /* flash sector idx 119 - 8kb        */
	{0x81F0000,0x2000,120,2},        /* flash sector idx 120 - 8kb        */
	{0x81F2000,0x2000,121,2},        /* flash sector idx 121 - 8kb        */
	{0x81F4000,0x2000,122,2},        /* flash sector idx 122 - 8kb        */
	{0x81F6000,0x2000,123,2},        /* flash sector idx 123 - 8kb        */
	{0x81F8000,0x2000,124,2},        /* flash sector idx 124 - 8kb        */
	{0x81FA000,0x2000,125,2},        /* flash sector idx 125 - 8kb        */
	{0x81FC000,0x2000,126,2},        /* flash sector idx 126 - 8kb        */
	{0x81FE000,0x2000,127,2},        /* flash sector idx 127 - 8kb        */

#else
 /* { 0x08000000, 0x20000, 0, 1 },     flash sector idx 0 - reserved for bootloader */
    { 0x08020000, 0x20000, 1, 1},        /* flash sector idx  1 - 128kb                */
    { 0x08040000, 0x20000, 2, 1},        /* flash sector idx  2 - 128kb                */
    { 0x08060000, 0x20000, 3, 1},        /* flash sector idx  3 - 128kb                */
    { 0x08080000, 0x20000, 4, 1},        /* flash sector idx  4 - 128kb                */
    { 0x080A0000, 0x20000, 5, 1},        /* flash sector idx  5 - 128kb                */
    { 0x080C0000, 0x20000, 6, 1},        /* flash sector idx  6 - 128kb                */
    { 0x080E0000, 0x20000, 7, 1},        /* flash sector idx  7 - 128kb                */
    { 0x08100000, 0x20000, 0, 2},        /* flash sector idx  8 - 128kb                */
    { 0x08120000, 0x20000, 1, 2},        /* flash sector idx  9 - 128kb                */
    { 0x08140000, 0x20000, 2, 2},        /* flash sector idx 10 - 128kb                */
    { 0x08160000, 0x20000, 3, 2},        /* flash sector idx 11 - 128kb                */
    { 0x08180000, 0x20000, 4, 2},        /* flash sector idx 12 - 128kb                */
    { 0x081A0000, 0x20000, 5, 2},        /* flash sector idx 13 - 128kb                */
    { 0x081C0000, 0x20000, 6, 2},        /* flash sector idx 14 - 128kb                */
    { 0x081E0000, 0x20000, 7, 2},        /* flash sector idx 15 - 128kb                */
#endif

#else
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
  blt_int8u first_sector_idx = FLASH_INVALID_SECTOR_IDX;
  blt_int8u last_sector_idx = FLASH_INVALID_SECTOR_IDX;

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

  /* for the STM32 target we defined the checksum as the Two's complement value of the
   * sum of the first 7 exception addresses.
   *
   * Layout of the vector table:
   *    0x08000000 Initial stack pointer
   *    0x08000004 Reset Handler
   *    0x08000008 NMI Handler
   *    0x0800000C Hard Fault Handler
   *    0x08000010 MPU Fault Handler
   *    0x08000014 Bus Fault Handler
   *    0x08000018 Usage Fault Handler
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
  blt_bool              result = BLT_TRUE;
  blt_addr              prog_addr;
  blt_int8u  volatile * prog_data;
  blt_int32u            word_cnt;
#ifdef  STM32H7A3xxQ
  blt_int8u  const      word_size = 16U;
#else
  blt_int8u  const      word_size = 32U;
#endif
  blt_int8u             byte_idx;

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
    /* unlock the flash array */
    HAL_FLASH_Unlock();

    /* program all words (32 bytes) in the block one by one */
    for (word_cnt=0; word_cnt<(FLASH_WRITE_BLOCK_SIZE/word_size); word_cnt++)
    {
      prog_addr = block->base_addr + (word_cnt * word_size);
      prog_data = (volatile blt_int8u *)(&block->data[word_cnt * word_size]);
      /* keep the watchdog happy */
      CopService();
      /* program the word */
      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, prog_addr,
          (uint32_t)prog_data) != HAL_OK)
      {
        result = BLT_FALSE;
        break;
      }
      /* verify that the written data is actually there */
      for (byte_idx = 0; byte_idx < word_size; byte_idx++)
      {
        /* compare the byte value located in flash with the one that was supposed to
         * be programmed there
         */
        if (((blt_int8u *)prog_addr)[byte_idx] != prog_data[byte_idx])
        {
          /* byte value in flash is not as expected, so a programming error must have
           * happened
           */
          result = BLT_FALSE;
          break;
        }
      }
      /* check if an error was flagged during the verification step */
      if (result == BLT_FALSE)
      {
        /* no point in continuing with the programming loop */
        break;
      }
    }
    /* lock the flash array again */
    HAL_FLASH_Lock();
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
  FLASH_EraseInitTypeDef eraseInitStruct;
  blt_int32u eraseSectorError = 0;

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
    /* unlock the flash array */
    HAL_FLASH_Unlock();

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
      
      /* intialize the sector erase info structure */
      eraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
#ifndef STM32H7A3xxQ
      eraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
#endif
      eraseInitStruct.Banks = flashLayout[sectorIdx].bank_num;
      eraseInitStruct.Sector = flashLayout[sectorIdx].sector_num;
      eraseInitStruct.NbSectors = 1;

      /* submit the sector erase request */
      if (HAL_FLASHEx_Erase(&eraseInitStruct, (uint32_t *)&eraseSectorError) != HAL_OK)
      {
        /* could not perform erase operation */
        result = BLT_FALSE;
        /* error detected so don't bother continuing with the loop */
        break;
      }

    }
    /* lock the flash array again */
    HAL_FLASH_Lock();
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
