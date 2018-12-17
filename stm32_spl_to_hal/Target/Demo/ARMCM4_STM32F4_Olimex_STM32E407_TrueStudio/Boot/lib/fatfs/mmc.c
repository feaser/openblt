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


static volatile
DSTATUS Stat = STA_NOINIT;	/* Disk status */


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
  DSTATUS diskstatus = STA_NODISK;
  
  /* TODO ##Vg Implement disk_initialize(). */
  
  return (diskstatus);
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv  /* Physical drive nmuber (0) */
)
{
  DSTATUS diskstatus = STA_NODISK;

  /* TODO ##Vg Implement disk_status(). */

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
  DRESULT diskresult = RES_PARERR;

  /* TODO ##Vg Implement disk_read(). */

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
  DRESULT diskresult = RES_PARERR;

  /* TODO ##Vg Implement disk_write(). */

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
  DRESULT diskresult = RES_PARERR;

  /* TODO ##Vg Implement disk_ioctl(). */

  return (diskresult);
}



