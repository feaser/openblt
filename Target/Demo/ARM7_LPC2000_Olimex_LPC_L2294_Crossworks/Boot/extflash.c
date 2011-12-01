/****************************************************************************************
|  Description: bootloader external flash driver source file
|    File Name: extflash.c
|
|----------------------------------------------------------------------------------------
|                          C O P Y R I G H T
|----------------------------------------------------------------------------------------
|   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
|
|----------------------------------------------------------------------------------------
|                            L I C E N S E
|----------------------------------------------------------------------------------------
| This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
| modify it under the terms of the GNU General Public License as published by the Free
| Software Foundation, either version 3 of the License, or (at your option) any later
| version.
|
| OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
| PURPOSE. See the GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License along with OpenBLT.
| If not, see <http://www.gnu.org/licenses/>.
|
| A special exception to the GPL is included to allow you to distribute a combined work 
| that includes OpenBLT without being obliged to provide the source code for any 
| proprietary components. The exception text is included at the bottom of the license
| file <license.html>.
| 
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */


#if (BOOT_NVM_HOOKS_ENABLE > 0)
/****************************************************************************************
* Macro definitions
****************************************************************************************/
#define FLASH_INVALID_SECTOR          (0xff)
#define FLASH_INVALID_ADDRESS         (0xffffffff)
#define FLASH_WRITE_BLOCK_SIZE        (512)
#define FLASH_TOTAL_SECTORS           (sizeof(flashLayout)/sizeof(flashLayout[0]))
/* C3 Intel flash commands */
#define FLASH_CMD_READ_ARRAY_MODE     (0xFF)
#define FLASH_CMD_READ_ID_MODE        (0x90)
#define FLASH_CMD_ERASE_MODE          (0x20)
#define FLASH_CMD_READ_STATUS_MODE    (0x70)
#define FLASH_CMD_CHANGE_LOCK_MODE    (0x60)
#define FLASH_CMD_UNLOCK_SECTOR       (0xD0)
#define FLASH_CMD_LOCK_SECTOR         (0x01)
#define FLASH_CMD_PROGRAM_MODE        (0x40)
#define FLASH_CMD_ERASE_CONFIRM       (0xD0)
#define FLASH_CMD_CLEAR_STATUS        (0x50)
/* C3 Intel flash info bits */
#define FLASH_LOCK_BIT                (0x01)
#define FLASH_STATUS_READY_BIT        (0x80)
/* C3 Intel flash error codes */
#define FLASH_ERR_LOCKED              (0x02)
#define FLASH_ERR_VPP_RANGE           (0x08)
#define FLASH_ERR_PROGRAM             (0x10)
#define FLASH_ERR_CMD_SEQ             (0x10)
#define FLASH_ERR_ERASE               (0x20)
/* flash operation timeout values */
#define FLASH_ERASE_TIMEOUT           ((blt_int32u)5000000)
#define FLASH_PROGRAM_TIMEOUT         ((blt_int32u)1000000)
/* supported Intel C3 flash device */
#define FLASH_DEV_MAN_ID              ((blt_int16u)0x0089)
#define FLASH_DEV_ID                  ((blt_int16u)0x88c3)
/* address offsets for reading device information */
#define FLASH_DEVINFO_MAN_ID          ((blt_int16u)0x0000)
#define FLASH_DEVINFO_DEV_ID          ((blt_int16u)0x0001)
#define FLASH_DEVINFO_LOCK_STATUS     ((blt_int16u)0x0002)
/* functions implemented in a macro for run-time and codesize optimization */
#define ExtFlashGetManID()          (ExtFlashGetDeviceInfo(flashLayout[0].sector_start, \
                                     FLASH_DEVINFO_MAN_ID))
#define ExtFlashGetDevID()          (ExtFlashGetDeviceInfo(flashLayout[0].sector_start, \
                                     FLASH_DEVINFO_DEV_ID))
#define ExtFlashGetLockStatus(base) (ExtFlashGetDeviceInfo(base, \
                                     FLASH_DEVINFO_LOCK_STATUS))


/****************************************************************************************
* Type definitions
****************************************************************************************/
/* flash sector descriptor type */
typedef struct 
{
  blt_addr   sector_start;                       /* sector start address               */
  blt_int32u sector_size;                        /* sector size in bytes               */
  blt_int8u  sector_num;                         /* sector number                      */
} tFlashSector;                                  /* flash sector description           */

/* programming is done per block of max FLASH_WRITE_BLOCK_SIZE. for this a flash block
 * manager is implemented in this driver. this flash block manager depends on this
 * flash block info structure. It holds the base address of the flash block and the
 * data that should be programmed into the flash block. 
 */
typedef struct
{
  blt_addr  base_addr;
  blt_int8u data[FLASH_WRITE_BLOCK_SIZE];
} tFlashBlockInfo;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static blt_bool   ExtFlashInitBlock(tFlashBlockInfo *block, blt_addr address);
static tFlashBlockInfo *ExtFlashSwitchBlock(tFlashBlockInfo *block, blt_addr base_addr);
static blt_bool   ExtFlashAddToBlock(tFlashBlockInfo *block, blt_addr address, 
                                     blt_int8u *data, blt_int16u len);
static blt_bool   ExtFlashWriteBlock(tFlashBlockInfo *block);
static blt_bool   ExtFlashEraseSector(blt_addr sector_base);
static blt_int16u ExtFlashGetDeviceInfo(blt_addr block_base, blt_int16u info);
static void       ExtFlashLockSector(blt_addr sector_base);
static void       ExtFlashUnlockSector(blt_addr sector_base);
static blt_int8u  ExtFlashGetSector(blt_addr address);


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/* The current layout supports the 2MB external C3 Intel flash:
 *   - manufacturer id = 0x0089
 *   - device id = 0x88c3 (16 Mbit bottom boot device)
 * Note that what Intel calls a block in the user manual, is called a sector in this
 * driver.
 */
static const tFlashSector flashLayout[] =
{
  { 0x80000000, 0x02000,  0},           /* flash sector  0 -  8 kbyte                  */
  { 0x80002000, 0x02000,  1},           /* flash sector  1 -  8 kbyte                  */
  { 0x80004000, 0x02000,  2},           /* flash sector  2 -  8 kbyte                  */
  { 0x80006000, 0x02000,  3},           /* flash sector  3 -  8 kbyte                  */
  { 0x80008000, 0x02000,  4},           /* flash sector  4 -  8 kbyte                  */
  { 0x8000A000, 0x02000,  5},           /* flash sector  5 -  8 kbyte                  */
  { 0x8000C000, 0x02000,  6},           /* flash sector  6 -  8 kbyte                  */
  { 0x8000E000, 0x02000,  7},           /* flash sector  7 -  8 kbyte                  */
  { 0x80010000, 0x10000,  8},           /* flash sector  8 - 64 kbyte                  */
  { 0x80020000, 0x10000,  9},           /* flash sector  9 - 64 kbyte                  */
  { 0x80030000, 0x10000, 10},           /* flash sector 10 - 64 kbyte                  */
  { 0x80040000, 0x10000, 11},           /* flash sector 11 - 64 kbyte                  */
  { 0x80050000, 0x10000, 12},           /* flash sector 12 - 64 kbyte                  */
  { 0x80060000, 0x10000, 13},           /* flash sector 13 - 64 kbyte                  */
  { 0x80070000, 0x10000, 14},           /* flash sector 14 - 64 kbyte                  */
  { 0x80080000, 0x10000, 15},           /* flash sector 15 - 64 kbyte                  */
  { 0x80090000, 0x10000, 16},           /* flash sector 16 - 64 kbyte                  */
  { 0x800A0000, 0x10000, 17},           /* flash sector 17 - 64 kbyte                  */
  { 0x800B0000, 0x10000, 18},           /* flash sector 18 - 64 kbyte                  */
  { 0x800C0000, 0x10000, 19},           /* flash sector 19 - 64 kbyte                  */
  { 0x800D0000, 0x10000, 20},           /* flash sector 20 - 64 kbyte                  */
  { 0x800E0000, 0x10000, 21},           /* flash sector 21 - 64 kbyte                  */
  { 0x800F0000, 0x10000, 22},           /* flash sector 22 - 64 kbyte                  */
  { 0x80100000, 0x10000, 23},           /* flash sector 23 - 64 kbyte                  */
  { 0x80110000, 0x10000, 24},           /* flash sector 24 - 64 kbyte                  */
  { 0x80120000, 0x10000, 25},           /* flash sector 25 - 64 kbyte                  */
  { 0x80130000, 0x10000, 26},           /* flash sector 26 - 64 kbyte                  */
  { 0x80140000, 0x10000, 27},           /* flash sector 27 - 64 kbyte                  */
  { 0x80150000, 0x10000, 28},           /* flash sector 28 - 64 kbyte                  */
  { 0x80160000, 0x10000, 29},           /* flash sector 29 - 64 kbyte                  */
  { 0x80170000, 0x10000, 30},           /* flash sector 30 - 64 kbyte                  */
  { 0x80180000, 0x10000, 31},           /* flash sector 31 - 64 kbyte                  */
  { 0x80190000, 0x10000, 32},           /* flash sector 32 - 64 kbyte                  */
  { 0x801A0000, 0x10000, 33},           /* flash sector 33 - 64 kbyte                  */
  { 0x801B0000, 0x10000, 34},           /* flash sector 34 - 64 kbyte                  */
  { 0x801C0000, 0x10000, 35},           /* flash sector 35 - 64 kbyte                  */
  { 0x801D0000, 0x10000, 36},           /* flash sector 36 - 64 kbyte                  */
  { 0x801E0000, 0x10000, 37},           /* flash sector 37 - 64 kbyte                  */
  { 0x801F0000, 0x10000, 38}            /* flash sector 38 - 64 kbyte                  */
};


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/* The smallest amount of flash that can be programmed is FLASH_WRITE_BLOCK_SIZE. A flash
 * block manager is implemented in this driver and stores info in this variable. Whenever
 * new data should be flashed, it is first added to a RAM buffer, which is part of this
 * variable. Whenever the RAM buffer, which has the size of a flash block, is full or 
 * data needs to be written to a different block, the contents of the RAM buffer are 
 * programmed to flash. The flash block manager requires some software overhead, yet
 * results is faster flash programming because data is first harvested, ideally until
 * there is enough to program an entire flash block, before the flash device is actually
 * operated on.
 */
static tFlashBlockInfo blockInfo;


/****************************************************************************************
** NAME:           ExtFlashInit
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Initializes the flash driver. 
**
****************************************************************************************/
void ExtFlashInit(void)
{
  /* init the flash block info struct by setting the address to an invalid address */
  blockInfo.base_addr = FLASH_INVALID_ADDRESS;
  /* check the flash device identification */
  if ((ExtFlashGetManID() != FLASH_DEV_MAN_ID) || (ExtFlashGetDevID() != FLASH_DEV_ID))
  {
    ASSERT_RT(BLT_FALSE);
  }
} /*** end of ExtFlashInit ***/


/****************************************************************************************
** NAME:           ExtFlashWrite
** PARAMETER:      addr start address
**                 len  length in bytes
**                 data pointer to the data buffer.
** RETURN VALUE:   BLT_NVM_OKAY if successful, BLT_NVM_NOT_IN_RANGE if the address is
**                 not within the supported memory range, or BLT_NVM_ERROR is the write
**                 operation failed.
** DESCRIPTION:    Writes the data to flash.
**
****************************************************************************************/
blt_int8u ExtFlashWrite(blt_addr addr, blt_int32u len, blt_int8u *data)
{
  /* make sure the addresses are within the flash device */
  if ( (ExtFlashGetSector(addr) == FLASH_INVALID_SECTOR) || \
       (ExtFlashGetSector(addr+len-1) == FLASH_INVALID_SECTOR) )
  {
    return BLT_NVM_NOT_IN_RANGE;       
  }

  /* let the block manager handle it */
  if (ExtFlashAddToBlock(&blockInfo, addr, data, len) == BLT_FALSE)
  {
    return BLT_NVM_ERROR;
  }
  return BLT_NVM_OKAY;
} /*** end of FlashWrite ***/


/****************************************************************************************
** NAME:           ExtFlashErase
** PARAMETER:      addr start address
**                 len  length in bytes
** RETURN VALUE:   BLT_NVM_OKAY if successful, BLT_NVM_NOT_IN_RANGE if the address is
**                 not within the supported memory range, or BLT_NVM_ERROR is the erase
**                 operation failed.
** DESCRIPTION:    Erases the flash memory. Note that this function also checks that no 
**                 data is erased outside the flash memory region.
**
****************************************************************************************/
blt_int8u ExtFlashErase(blt_addr addr, blt_int32u len)
{
  blt_int8u first_sector;
  blt_int8u last_sector;
  blt_int8u   sectorIdx;
  
  /* obtain the first and last sector number */
  first_sector = ExtFlashGetSector(addr);
  last_sector  = ExtFlashGetSector(addr+len-1);
  /* check them */
  if ( (first_sector == FLASH_INVALID_SECTOR) || (last_sector == FLASH_INVALID_SECTOR) )
  {
    return BLT_NVM_NOT_IN_RANGE;
  }
  /* erase the sectors one-by-one */
  for (sectorIdx = first_sector; sectorIdx <= last_sector; sectorIdx++)
  {
    /* keep the watchdog happy */
    CopService();
    /* erase the sector */
    if (ExtFlashEraseSector(flashLayout[sectorIdx].sector_start) == BLT_FALSE)
    {
      return BLT_NVM_ERROR;
    }
  }
  /* done so return the result of the operation */
  return BLT_NVM_OKAY;
} /*** end of ExtFlashErase ***/


/****************************************************************************************
** NAME:           ExtFlashDone
** PARAMETER:      none
** RETURN VALUE:   BLT_TRUE is succesful, BLT_FALSE otherwise.
** DESCRIPTION:    Finilizes the flash driver operations.
**
****************************************************************************************/
blt_bool ExtFlashDone(void)
{
  /* check if there is still data waiting to be programmed */
  if (blockInfo.base_addr != FLASH_INVALID_ADDRESS)
  {
    if (ExtFlashWriteBlock(&blockInfo) == BLT_FALSE)
    {
      return BLT_FALSE;
    }
  }
  /* still here so all is okay */  
  return BLT_TRUE;
} /*** end of ExtFlashDone ***/


/****************************************************************************************
** NAME:           ExtFlashInitBlock
** PARAMETER:      block   pointer to flash block info structure to operate on.
**                 address base address of the block data.
** RETURN VALUE:   BLT_TRUE is succesful, BLT_FALSE otherwise.
** DESCRIPTION:    Copies data currently in flash to the block->data and sets the 
**                 base address.
**
****************************************************************************************/
static blt_bool ExtFlashInitBlock(tFlashBlockInfo *block, blt_addr address)
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
} /*** end of ExtFlashInitBlock ***/


/****************************************************************************************
** NAME:           ExtFlashSwitchBlock
** PARAMETER:      block     pointer to flash block info structure to operate on.
**                 base_addr base address for the next block
** RETURN VALUE:   the pointer of the block info struct that is no being used, or a NULL
**                 pointer in case of error.
** DESCRIPTION:    Switches blocks by programming the current one and initializing the
**                 next.
**
****************************************************************************************/
static tFlashBlockInfo *ExtFlashSwitchBlock(tFlashBlockInfo *block, blt_addr base_addr)
{
  /* need to switch to a new block, so program the current one and init the next */
  if (ExtFlashWriteBlock(block) == BLT_FALSE)
  {
    return BLT_NULL;
  }
  /* initialize the new block when necessary */
  if (ExtFlashInitBlock(block, base_addr) == BLT_FALSE) 
  {
    return BLT_NULL;
  }
  /* still here to all is okay  */
  return block;
} /*** end of ExtFlashSwitchBlock ***/


/****************************************************************************************
** NAME:           ExtFlashAddToBlock
** PARAMETER:      block   pointer to flash block info structure to operate on.
**                 address flash destination address
**                 data    pointer to the byte array with data
**                 len     number of bytes to add to the block
** RETURN VALUE:   BLT_TRUE if successful, BLT_FALSE otherwise.
** DESCRIPTION:    Programming is done per block. This function adds data to the block
**                 that is currently collecting data to be written to flash. If the
**                 address is outside of the current block, the current block is written
**                 to flash an a new block is initialized.
**
****************************************************************************************/
static blt_bool ExtFlashAddToBlock(tFlashBlockInfo *block, blt_addr address, 
                                   blt_int8u *data, blt_int16u len)
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
    if (ExtFlashInitBlock(block, current_base_addr) == BLT_FALSE)
    {
      return BLT_FALSE;
    }
  }
  /* check if the new data fits in the current block */
  if (block->base_addr != current_base_addr)
  {
    /* need to switch to a new block, so program the current one and init the next */
    block = ExtFlashSwitchBlock(block, current_base_addr);
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
      block = ExtFlashSwitchBlock(block, current_base_addr+FLASH_WRITE_BLOCK_SIZE);
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
} /*** end of ExtFlashAddToBlock ***/


/****************************************************************************************
** NAME:           ExtFlashWriteBlock
** PARAMETER:      block pointer to flash block info structure to operate on.
** RETURN VALUE:   BLT_TRUE if successful, BLT_FALSE otherwise.
** DESCRIPTION:    Programs FLASH_WRITE_BLOCK_SIZE bytes to flash from the block->data
**                 array. 
**
****************************************************************************************/
static blt_bool ExtFlashWriteBlock(tFlashBlockInfo *block)
{
  volatile blt_int16u *pAddr;
  volatile blt_int16u *pData;
  blt_bool             result = BLT_TRUE;
  volatile blt_int32u  timeout = 0;

  /* unlock the sector */
  ExtFlashUnlockSector(block->base_addr);
  /* init pointer to valid address in the flash block */
  pAddr = (blt_int16u *)block->base_addr;
  /* init pointer to start of block data */
  pData = (blt_int16u *)block->data;
  /* program all block data 16-bits at a time */
  while ((blt_addr)pAddr < (block->base_addr+FLASH_WRITE_BLOCK_SIZE))
  {
    /* keep the watchdog happy */
    CopService();
    /* issue program setup command */
    *pAddr =  FLASH_CMD_PROGRAM_MODE;
    /* write 16-bit data that is to be programmed to start programming operation */
    *pAddr = *pData;
    /* check status register for completion */
    *pAddr = FLASH_CMD_READ_STATUS_MODE;
    /* wait for completion or timeout */
    while( ((*pAddr & FLASH_STATUS_READY_BIT) == 0) && (timeout < FLASH_PROGRAM_TIMEOUT) )
    {
      timeout++;
    }
    /* check for possible errors */
    if ( (timeout >= FLASH_ERASE_TIMEOUT) || \
         ((*pAddr & (FLASH_ERR_LOCKED | FLASH_ERR_VPP_RANGE | FLASH_ERR_PROGRAM))!= 0) )
    {
      result = BLT_FALSE;
    }
    /* clear the status register */
    *pAddr = FLASH_CMD_CLEAR_STATUS;
    /* increment address and data pointers */
    pAddr++;
    pData++;
  }
  /* lock the sector. this also switches back to read array mode */
  ExtFlashLockSector(block->base_addr);
  /* inform the caller about the result */
  return result;
} /*** end of ExtFlashWriteBlock ***/


/****************************************************************************************
** NAME:           ExtFlashEraseSector
** PARAMETER:      sector_base base address of the sector to erase.
** RETURN VALUE:   BLT_TRUE is erasure was successful, BLT_FALSE otherwise.
** DESCRIPTION:    Erases the flash sector.
**
****************************************************************************************/
static blt_bool ExtFlashEraseSector(blt_addr sector_base)
{
  volatile blt_int16u *pAddr;
  blt_bool             result = BLT_TRUE;
  volatile blt_int32u  timeout = 0;

  /* unlock the sector */
  ExtFlashUnlockSector(sector_base);
  /* init pointer to valid address in the flash sector */
  pAddr = (blt_int16u *)sector_base;
  /* issue erase setup command */
  *pAddr =  FLASH_CMD_ERASE_MODE;
  /* issue erase confirm command */
  *pAddr =  FLASH_CMD_ERASE_CONFIRM;
  /* check status register for completion */
  *pAddr = FLASH_CMD_READ_STATUS_MODE;
  /* wait for completion or timeout */
  while( ((*pAddr & FLASH_STATUS_READY_BIT) == 0) && (timeout < FLASH_ERASE_TIMEOUT) )
  {
    timeout++;
  }
  /* check for possible errors */
  if ( (timeout >= FLASH_ERASE_TIMEOUT) || \
       ((*pAddr & (FLASH_ERR_LOCKED | FLASH_ERR_VPP_RANGE | FLASH_ERR_ERASE))!= 0) )
  {
    result = BLT_FALSE;
  }
  /* clear the status register */
  *pAddr = FLASH_CMD_CLEAR_STATUS;
  /* lock the sector. this also switches back to read array mode */
  ExtFlashLockSector(sector_base);
  /* inform the caller about the result */
  return result;
} /*** end of ExtFlashEraseSector ***/


/****************************************************************************************
** NAME:           ExtFlashLockSector
** PARAMETER:      sector_base base address of the sector to lock.
** RETURN VALUE:   none
** DESCRIPTION:    Locks the flash sector.
**
****************************************************************************************/
static void ExtFlashLockSector(blt_addr sector_base)
{
  volatile blt_int16u *pAddr;

  /* no need to lock a sector that is already locked */
  if ((ExtFlashGetLockStatus(sector_base) & FLASH_LOCK_BIT) != 0)
  {
    return;
  }
  /* init pointer to valid address in the flash sector */
  pAddr = (blt_int16u *)sector_base;
  /* switch to change lock mode */
  *pAddr = FLASH_CMD_CHANGE_LOCK_MODE;
  /* unlock the sector */
  *pAddr = FLASH_CMD_LOCK_SECTOR;
  /* check that the sector is now actually locked */
  ASSERT_RT((ExtFlashGetLockStatus(sector_base) & FLASH_LOCK_BIT) != 0);
} /*** end of ExtFlashLockSector ***/


/****************************************************************************************
** NAME:           ExtFlashUnlockSector
** PARAMETER:      sector_base base address of the sector to unlock.
** RETURN VALUE:   none
** DESCRIPTION:    Unlocks the flash sector.
**
****************************************************************************************/
static void ExtFlashUnlockSector(blt_addr sector_base)
{
  volatile blt_int16u *pAddr;

  /* no need to unlock a sector that is already unlocked */
  if ((ExtFlashGetLockStatus(sector_base) & FLASH_LOCK_BIT) == 0)
  {
    return;
  }
  /* init pointer to valid address in the flash sector */
  pAddr = (blt_int16u *)sector_base;
  /* switch to change lock mode */
  *pAddr = FLASH_CMD_CHANGE_LOCK_MODE;
  /* unlock the sector */
  *pAddr = FLASH_CMD_UNLOCK_SECTOR;
  /* check that the sector is now actually unlocked */
  ASSERT_RT((ExtFlashGetLockStatus(sector_base) & FLASH_LOCK_BIT) == 0);
} /*** end of ExtFlashUnlockSector ***/


/****************************************************************************************
** NAME:           ExtFlashGetDeviceInfo
** PARAMETER:      sector_base base address of the sector to get the info from.
**                 info identifier to the type of info to obtain.
** RETURN VALUE:   device info.
** DESCRIPTION:    Obtains device information from the flash device.
**
****************************************************************************************/
static blt_int16u ExtFlashGetDeviceInfo(blt_addr sector_base, blt_int16u info)
{
  volatile blt_int16u *pAddr;
  blt_int16u           readData;

  /* init pointer to any valid address in the flash device */
  pAddr = (blt_int16u *)sector_base + info;
  /* switch to read identifier mode */
  *pAddr = FLASH_CMD_READ_ID_MODE;
  /* read the info */
  readData = *pAddr;
  /* switch back to reading mode */
  *pAddr = FLASH_CMD_READ_ARRAY_MODE;
  /* return the result */
  return readData;
} /*** end of ExtFlashGetDeviceInfo ***/


/****************************************************************************************
** NAME:           ExtFlashGetSector
** PARAMETER:      address address in the flash sector
** RETURN VALUE:   flash sector number or FLASH_INVALID_SECTOR
** DESCRIPTION:    Determines the flash sector the address is in.
**
****************************************************************************************/
static blt_int8u ExtFlashGetSector(blt_addr address)
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
} /*** end of ExtFlashGetSector ***/
#endif

/*********************************** end of extflash.c *********************************/
