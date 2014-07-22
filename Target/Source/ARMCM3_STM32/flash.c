/************************************************************************************//**
* \file         Source\ARMCM3_STM32\flash.c
* \brief        Bootloader flash driver source file.
* \ingroup      Target_ARMCM3_STM32
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
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


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Value for an invalid flash sector. */
#define FLASH_INVALID_SECTOR            (0xff)
/** \brief Value for an invalid flash address. */
#define FLASH_INVALID_ADDRESS           (0xffffffff)
/** \brief Standard size of a flash block for writing. */
#define FLASH_WRITE_BLOCK_SIZE          (512)
/** \brief Total numbers of sectors in array flashLayout[]. */
#define FLASH_TOTAL_SECTORS             (sizeof(flashLayout)/sizeof(flashLayout[0]))
#if (BOOT_NVM_SIZE_KB > 128)
/** \brief Number of bytes to erase per erase operation. */
#define FLASH_ERASE_BLOCK_SIZE          (0x800)
#else
/** \brief Number of bytes to erase per erase operation. */
#define FLASH_ERASE_BLOCK_SIZE          (0x400)
#endif
/** \brief Macro for accessing the flash control registers. */
#define FLASH                           ((tFlashRegs *) (blt_int32u)0x40022000)
/** \brief Offset into the user program's vector table where the checksum is located. */
#define FLASH_VECTOR_TABLE_CS_OFFSET    (0x150)
#define FLASH_KEY1                      ((blt_int32u)0x45670123)
#define FLASH_KEY2                      ((blt_int32u)0xCDEF89AB)
#define FLASH_LOCK_BIT                  ((blt_int32u)0x00000080)
#define FLASH_EOP_BIT                   ((blt_int32u)0x00000020)
#define FLASH_PGERR_BIT                 ((blt_int32u)0x00000004)
#define FLASH_WRPRTERR_BIT              ((blt_int32u)0x00000010)
#define FLASH_BSY_BIT                   ((blt_int32u)0x00000001)
#define FLASH_PER_BIT                   ((blt_int32u)0x00000002)
#define FLASH_STRT_BIT                  ((blt_int32u)0x00000040)
#define FLASH_PG_BIT                    ((blt_int32u)0x00000001)


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

/** \brief Flash controller register layout type.  */
typedef struct
{
  volatile blt_int32u ACR;                       /**< flash access control register    */     
  volatile blt_int32u KEYR;                      /**< FPEC key register                */
  volatile blt_int32u OPTKEYR;                   /**< flash OPTKEY register            */
  volatile blt_int32u SR;                        /**< flash status register            */
  volatile blt_int32u CR;                        /**< flash control register           */
  volatile blt_int32u AR;                        /**< flash address register           */
  volatile blt_int32u RESERVED;
  volatile blt_int32u OBR;                       /**< option byte register             */
  volatile blt_int32u WRPR;                      /**< write protection register        */
} tFlashRegs;                                         


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static blt_bool  FlashInitBlock(tFlashBlockInfo *block, blt_addr address);
static tFlashBlockInfo *FlashSwitchBlock(tFlashBlockInfo *block, blt_addr base_addr);
static blt_bool  FlashAddToBlock(tFlashBlockInfo *block, blt_addr address, 
                                 blt_int8u *data, blt_int32u len);
static blt_bool  FlashWriteBlock(tFlashBlockInfo *block);
static blt_bool  FlashEraseSectors(blt_int8u first_sector, blt_int8u last_sector);
static void      FlashUnlock(void);
static void      FlashLock(void);
static blt_int8u FlashGetSector(blt_addr address);
static blt_addr  FlashGetSectorBaseAddr(blt_int8u sector);
static blt_addr  FlashGetSectorSize(blt_int8u sector);


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief   Array wit the layout of the flash memory.
 *  \details Also controls what part of the flash memory is reserved for the bootloader. 
 *           If the bootloader size changes, the reserved sectors for the bootloader 
 *           might need adjustment to make sure the bootloader doesn't get overwritten.
 *           The current flash layout does not reflect the minimum sector size of the
 *           physical flash (1 - 2kb), because this would make the table quit long and
 *           a waste of ROM. The minimum sector size is only really needed when erasing
 *           the flash. This can still be done in combination with macro 
 *           FLASH_ERASE_BLOCK_SIZE.
 */
static const tFlashSector flashLayout[] =
{
  /* space is reserved for a bootloader configuration with all supported communication
   * interfaces enabled. when for example only UART is needed, than the space required
   * for the bootloader can be made a lot smaller here.
   */
  /* { 0x08000000, 0x02000,  0},           flash sector  0 - reserved for bootloader   */
  /* { 0x08002000, 0x02000,  1},           flash sector  1 - reserved for bootloader   */
  /* { 0x08004000, 0x02000,  2},           flash sector  2 - reserved for bootloader   */
  { 0x08006000, 0x02000,  3},           /* flash sector  3 - 8kb                       */
#if (BOOT_NVM_SIZE_KB > 32)
  { 0x08008000, 0x02000,  4},           /* flash sector  4 - 8kb                       */
  { 0x0800A000, 0x02000,  5},           /* flash sector  5 - 8kb                       */
  { 0x0800C000, 0x02000,  6},           /* flash sector  6 - 8kb                       */
  { 0x0800E000, 0x02000,  7},           /* flash sector  7 - 8kb                       */
#endif
#if (BOOT_NVM_SIZE_KB > 64)
  { 0x08010000, 0x02000,  8},           /* flash sector  8 - 8kb                       */
  { 0x08012000, 0x02000,  9},           /* flash sector  9 - 8kb                       */
  { 0x08014000, 0x02000, 10},           /* flash sector 10 - 8kb                       */
  { 0x08016000, 0x02000, 11},           /* flash sector 11 - 8kb                       */
  { 0x08018000, 0x02000, 12},           /* flash sector 12 - 8kb                       */
  { 0x0801A000, 0x02000, 13},           /* flash sector 13 - 8kb                       */
  { 0x0801C000, 0x02000, 14},           /* flash sector 14 - 8kb                       */
  { 0x0801E000, 0x02000, 15},           /* flash sector 15 - 8kb                       */
#endif
#if (BOOT_NVM_SIZE_KB > 128)
  { 0x08020000, 0x08000, 16},           /* flash sector 16 - 32kb                      */
  { 0x08028000, 0x08000, 17},           /* flash sector 17 - 32kb                      */
  { 0x08030000, 0x08000, 18},           /* flash sector 18 - 32kb                      */
  { 0x08038000, 0x08000, 19},           /* flash sector 19 - 32kb                      */
#endif
#if (BOOT_NVM_SIZE_KB > 256)
  { 0x08040000, 0x08000, 20},           /* flash sector 20 - 32kb                      */
  { 0x08048000, 0x08000, 21},           /* flash sector 21 - 32kb                      */
  { 0x08050000, 0x08000, 22},           /* flash sector 22 - 32kb                      */
  { 0x08058000, 0x08000, 23},           /* flash sector 23 - 32kb                      */
  { 0x08060000, 0x08000, 24},           /* flash sector 24 - 32kb                      */
  { 0x08068000, 0x08000, 25},           /* flash sector 25 - 32kb                      */
  { 0x08070000, 0x08000, 26},           /* flash sector 26 - 32kb                      */
  { 0x08078000, 0x08000, 27},           /* flash sector 27 - 32kb                      */
#endif
#if (BOOT_NVM_SIZE_KB > 512)
#error "BOOT_NVM_SIZE_KB > 512 is currently not supported."
#endif
};


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

  /* make sure the addresses are within the flash device */
  if ( (FlashGetSector(addr) == FLASH_INVALID_SECTOR) || \
       (FlashGetSector(addr+len-1) == FLASH_INVALID_SECTOR) )
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
  
  /* obtain the first and last sector number */
  first_sector = FlashGetSector(addr);
  last_sector  = FlashGetSector(addr+len-1);
  /* check them */
  if ( (first_sector == FLASH_INVALID_SECTOR) || (last_sector == FLASH_INVALID_SECTOR) )
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
   if (bootBlockInfo.base_addr == FLASH_INVALID_ADDRESS)
   {
    return BLT_TRUE;
   }

  /* compute the checksum. note that the user program's vectors are not yet written
   * to flash but are present in the bootblock data structure at this point.
   */
  signature_checksum += *((blt_int32u*)(&bootBlockInfo.data[0+0x00]));
  signature_checksum += *((blt_int32u*)(&bootBlockInfo.data[0+0x04]));
  signature_checksum += *((blt_int32u*)(&bootBlockInfo.data[0+0x08]));
  signature_checksum += *((blt_int32u*)(&bootBlockInfo.data[0+0x0C]));
  signature_checksum += *((blt_int32u*)(&bootBlockInfo.data[0+0x10]));
  signature_checksum += *((blt_int32u*)(&bootBlockInfo.data[0+0x14]));
  signature_checksum += *((blt_int32u*)(&bootBlockInfo.data[0+0x18]));
  signature_checksum  = ~signature_checksum; /* one's complement */
  signature_checksum += 1; /* two's complement */

  /* write the checksum */
  return FlashWrite(flashLayout[0].sector_start+FLASH_VECTOR_TABLE_CS_OFFSET, 
                    sizeof(blt_addr), (blt_int8u*)&signature_checksum);
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
  
  /* verify the checksum based on how it was written by CpuWriteChecksum() */
  signature_checksum += *((blt_int32u*)(flashLayout[0].sector_start));
  signature_checksum += *((blt_int32u*)(flashLayout[0].sector_start+0x04));
  signature_checksum += *((blt_int32u*)(flashLayout[0].sector_start+0x08));
  signature_checksum += *((blt_int32u*)(flashLayout[0].sector_start+0x0C));
  signature_checksum += *((blt_int32u*)(flashLayout[0].sector_start+0x10));
  signature_checksum += *((blt_int32u*)(flashLayout[0].sector_start+0x14));
  signature_checksum += *((blt_int32u*)(flashLayout[0].sector_start+0x18));
  signature_checksum += *((blt_int32u*)(flashLayout[0].sector_start+FLASH_VECTOR_TABLE_CS_OFFSET));
  /* sum should add up to an unsigned 32-bit value of 0 */
  if (signature_checksum == 0)
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
  blt_bool   result = BLT_TRUE;
  blt_addr   prog_addr;
  blt_int32u prog_data;
  blt_int32u word_cnt;

  /* check that address is actually within flash */
  sector_num = FlashGetSector(block->base_addr);
  if (sector_num == FLASH_INVALID_SECTOR)
  {
    return BLT_FALSE;
  }
  /* unlock the flash array */
  FlashUnlock();
  /* check that the flash peripheral is not busy */
  if ((FLASH->SR & FLASH_BSY_BIT) == FLASH_BSY_BIT)
  {
    /* lock the flash array again */
    FlashLock();
    /* could not perform erase operation */
    return BLT_FALSE;
  }
  /* set the program bit to indicate that we are about to program data */
  FLASH->CR |= FLASH_PG_BIT;
  /* program all words in the block one by one */
  for (word_cnt=0; word_cnt<(FLASH_WRITE_BLOCK_SIZE/sizeof(blt_int32u)); word_cnt++)
  {
    prog_addr = block->base_addr + (word_cnt * sizeof(blt_int32u));
    prog_data = *(volatile blt_int32u*)(&block->data[word_cnt * sizeof(blt_int32u)]);
    /* program the first half word */
    *(volatile blt_int16u*)prog_addr = (blt_int16u)prog_data;
    /* wait for the program operation to complete */
    while ((FLASH->SR & FLASH_BSY_BIT) == FLASH_BSY_BIT)
    {
      /* keep the watchdog happy */
      CopService();
    }
    /* program the second half word */
    *(volatile blt_int16u*)(prog_addr+2) = (blt_int16u)(prog_data >> 16);
    /* wait for the program operation to complete */
    while ((FLASH->SR & FLASH_BSY_BIT) == FLASH_BSY_BIT)
    {
      /* keep the watchdog happy */
      CopService();
    }
    /* verify that the written data is actually there */
    if (*(volatile blt_int32u*)prog_addr != prog_data)
    {
      result = BLT_FALSE;
      break;
    }
  }
  /* reset the program bit to indicate that we are done programming data */
  FLASH->CR &= ~FLASH_PG_BIT;
  /* lock the flash array */
  FlashLock();
  /* still here so all is okay */
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
  blt_int16u nr_of_blocks;
  blt_int16u block_cnt;
  blt_addr   start_addr;
  blt_addr   end_addr;

  /* validate the sector numbers */
  if (first_sector > last_sector)
  {
    return BLT_FALSE;
  }
  if ( (first_sector < flashLayout[0].sector_num) || \
       (last_sector > flashLayout[FLASH_TOTAL_SECTORS-1].sector_num) )
  {
    return BLT_FALSE;
  }
  /* unlock the flash array */
  FlashUnlock();
  /* check that the flash peripheral is not busy */
  if ((FLASH->SR & FLASH_BSY_BIT) == FLASH_BSY_BIT)
  {
    /* lock the flash array again */
    FlashLock();
    /* could not perform erase operation */
    return BLT_FALSE;
  }
  /* set the page erase bit to indicate that we are about to erase a block */
  FLASH->CR |= FLASH_PER_BIT;

  /* determine how many blocks need to be erased */
  start_addr = FlashGetSectorBaseAddr(first_sector);
  end_addr = FlashGetSectorBaseAddr(last_sector) + FlashGetSectorSize(last_sector) - 1;
  nr_of_blocks = (end_addr - start_addr + 1) / FLASH_ERASE_BLOCK_SIZE;
  
  /* erase all blocks one by one */
  for (block_cnt=0; block_cnt<nr_of_blocks; block_cnt++)
  {
    /* store an address of the block that is to be erased to select the block */
    FLASH->AR = start_addr + (block_cnt * FLASH_ERASE_BLOCK_SIZE);
    /* start the block erase operation */
    FLASH->CR |= FLASH_STRT_BIT;
    /* wait for the erase operation to complete */
    while ((FLASH->SR & FLASH_BSY_BIT) == FLASH_BSY_BIT)
    {
      /* keep the watchdog happy */
      CopService();
    }
  }
  /* reset the page erase bit because we're all done erasing */
  FLASH->CR &= ~FLASH_PER_BIT;
  /* lock the flash array */
  FlashLock();
  /* still here so all went okay */
  return BLT_TRUE;
} /*** end of FlashEraseSectors ***/


/************************************************************************************//**
** \brief     Unlocks the flash array so that erase and program operations can be
**            performed.
** \return    none.
**
****************************************************************************************/
static void FlashUnlock(void)
{
  /* authorize the FPEC to access bank 1 */
  FLASH->KEYR = FLASH_KEY1;
  FLASH->KEYR = FLASH_KEY2;
  /* clear all possibly pending status flags */
  FLASH->SR = (FLASH_EOP_BIT | FLASH_PGERR_BIT | FLASH_WRPRTERR_BIT);
} /*** end of FlashUnlock ***/


/************************************************************************************//**
** \brief     Locks the flash array so that erase and program operations can no
**            longer be performed.
** \return    none.
**
****************************************************************************************/
static void FlashLock(void)
{
  /* set the lock bit to lock the FPEC */
  FLASH->CR |= FLASH_LOCK_BIT;
} /*** end of FlashLock ***/


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
    if ( (address >= flashLayout[sectorIdx].sector_start) && \
         (address < (flashLayout[sectorIdx].sector_start + \
                  flashLayout[sectorIdx].sector_size)) )
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
