/*------------------------------------------------------------------------/
/  MMCv3/SDv1/SDv2 (in SDIO mode) control module
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2013, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------*/


/*
 * This file was modified from a sample available from the FatFs
 * web site. It was modified to work with a Infineon XMC4700 Relax Kit
 * evaluation board. It is based on the DAVE 4 SDMMC_BLOCK app, which was modified
 * to work with an existing timer and for operation in polling mode.
 *
 */
#include "diskio.h"
#include "boot.h"
#include "sdmmc_block.h"


static volatile
DSTATUS Stat = STA_NOINIT;	/* Disk status */

const
DRESULT FATFS_errorcodes[5] =   {
                                  RES_OK ,
                                  RES_ERROR,
                                  RES_WRPRT,
                                  RES_NOTRDY,
                                  RES_PARERR
                                };
const
DSTATUS FATFS_statuscodes[4] =  {
                                  (DSTATUS)0,
                                  (DSTATUS)STA_NOINIT,
                                  (DSTATUS)STA_NODISK,
                                  (DSTATUS)STA_PROTECT
                                };

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv		/* Physical drive number (0) */
)
{
  DSTATUS diskstatus;
  uint32_t status;

  /* Make sure the SDMMC block is initialized. Note that it has a protection
   * against mutiple initialization.
   */
  SDMMC_BLOCK_Init(&SDMMC_BLOCK_0);

  /* If drive number is greater than the maximum drives allowed  */
  if (pdrv > 0)
  {
    diskstatus = (DSTATUS)((uint8_t)STA_NODISK | (uint8_t)STA_NOINIT);
  }
  else
  {
    /* Call the Initialize function. */
    status = SDMMC_BLOCK_Initialize(&SDMMC_BLOCK_0);
    /* Fatfs to Device Abstraction Layer Error Code Mapping */
    diskstatus = FATFS_statuscodes[status];
  }

  return (diskstatus);
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv  /* Physical drive nmuber (0) */
)
{
  DSTATUS diskstatus;
  uint32_t status;

  /* If drive number is greater than the maximum drives allowed  */
  if (pdrv > 0)
  {
    diskstatus = (DSTATUS)((uint8_t)STA_NODISK | (uint8_t)STA_NOINIT);
  }
  else
  {
    /* Call the Initialize function.*/
    status = SDMMC_BLOCK_GetStatus(&SDMMC_BLOCK_0);
    /* Fatfs to Block Layer Error Code Mapping */
    diskstatus = FATFS_statuscodes[status];
  }
  return (diskstatus);
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,     /* Physical drive nmuber to identify the drive */
	BYTE *buff,    /* Data buffer to store read data */
	DWORD sector,  /* Sector address in LBA */
	UINT count     /* Number of sectors to read */
)
{
  DRESULT diskresult;
  uint32_t result;

  /* If drive number is greater than the maximum drives allowed  */
  if (pdrv > 0)
  {
    diskresult = RES_PARERR;
  }
  /* If sector count is less than 1. Minimum 1 sector is needed*/
  else if (count < (uint8_t)1)
  {
    diskresult = RES_PARERR;
  }
  /*Call the ReadBlk function.*/
  else
  {
    result = (uint32_t)SDMMC_BLOCK_ReadBlock(&SDMMC_BLOCK_0, (uint8_t *)buff,
                                             (uint32_t)sector, count);

    /* FatFs to Device Abstraction Layer Error Code Mapping */
    diskresult = FATFS_errorcodes[result];
  }
  return (diskresult);
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,         /* Physical drive nmuber to identify the drive */
	const BYTE *buff,  /* Data to be written */
	DWORD sector,      /* Sector address in LBA */
	UINT count         /* Number of sectors to write */
)
{
  DRESULT diskresult;
  uint32_t result;

  /* If drive number is greater than the maximum drives allowed  */
  if (pdrv > 0)
  {
    diskresult = RES_PARERR;
  }
  /* If sector count is less than 1. Minimum 1 sector is needed*/
  else if (count < (uint8_t)1)
  {
    diskresult = RES_PARERR;
  }
  /*Call the WriteBlk function.*/
  else
  {
    result = (uint32_t)SDMMC_BLOCK_WriteBlock(&SDMMC_BLOCK_0,(uint8_t *)buff,
                                                           (uint32_t)sector, count);
    /* FatFs to Device Abstraction Layer Error Code Mapping */
    diskresult = FATFS_errorcodes[result];
  }
  return (diskresult);
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,    /* Physical drive nmuber (0) */
	BYTE cmd,     /* Control code */
	void *buff    /* Buffer to send/receive data block */
)
{
  DRESULT diskresult;
  uint32_t result;
  if (pdrv > 0)
  {
    diskresult = RES_PARERR;
  }
  /*Call the Ioctl function.*/
  else
  {
    result = SDMMC_BLOCK_Ioctl(&SDMMC_BLOCK_0, cmd, buff);
    /* FatFs to Block Layer Error Code Mapping */
    diskresult = FATFS_errorcodes[result];
  }
  return (diskresult);
}



