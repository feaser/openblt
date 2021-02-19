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
 * web site. It was modified to work with a Olimex STM32-E407 board.
 */
#include "diskio.h"
#include "boot.h"
#include "sd_diskio.h"


static volatile
DSTATUS Stat = STA_NOINIT;	/* Disk status */

static volatile
BYTE DiskInitialized = 0;


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
  DSTATUS stat = RES_OK;

  if (DiskInitialized == 0)
  {
    DiskInitialized = 1;
    stat = SD_initialize(0);
  }
  return stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv  /* Physical drive nmuber (0) */
)
{
  DSTATUS stat;

  stat = SD_status(0);
  return stat;
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
  DRESULT res;

  res = SD_read(0, buff, sector, count);
  return res;
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
  DRESULT res;

  res = SD_write(0, buff, sector, count);
  return res;
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
  DRESULT res;

  res = SD_ioctl(0, cmd, buff);
  return res;
}



