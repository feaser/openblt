/************************************************************************************//**
* \file         port/linux/usbbulk.c
* \brief        USB bulk driver source file.
* \ingroup      XcpTpUsb
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2017  by Feaser    http://www.feaser.com    All rights reserved
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
#include <assert.h>                         /* for assertions                          */
#include <stdint.h>                         /* for standard integer types              */
#include <stddef.h>                         /* for NULL declaration                    */
#include <stdbool.h>                        /* for boolean type                        */
#include "usbbulk.h"                        /* USB bulk driver                         */
#include <libusb-1.0/libusb.h>              /* LibUsb 1.0 driver                       */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Size of the internal endpoint read buffer. This should be the same as the size
 *         of the buffer size of the endpoint on the USB device itself.
 */
#define USBBULK_READ_DATA_BUFFER_SIZE    (64u)


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief Vendor ID of the OpenBLT bootloader as assigned by the OpenMoko project. */
static const uint16_t openBltVendorId = 0x1D50;

/** \brief Product ID of the OpenBLT bootloader as assigned by the OpenMoko project. */
static const uint16_t openBltProductId = 0x60AC;


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief LibUsb context. */
static libusb_context * libUsbCtx;

/** \brief LibUsb device handle. */
static libusb_device_handle * libUsbDevHandle;

/** \brief Internal endpoint read buffer. With LibUsb endpoint read operations should
 *         always be attempted with the size of the endpoint buffer on the USB device
 *         itself.
 */
static uint8_t readDataBuffer[USBBULK_READ_DATA_BUFFER_SIZE];

/** \brief Variable that holds the number of bytes that were read from the endpoint, but
 *         were not yet retrieved from this module via UsbBulkRead().
 */
static uint8_t readDataPending;

/** \brief Index into the endpoint read buffer (readDataBuffer[]) that point to the next
 *         byte value that should be read.
 */
static uint8_t readDataCurrentReadIdx;


/************************************************************************************//**
** \brief     Initializes the USB bulk driver.
**
****************************************************************************************/
void UsbBulkInit(void)
{
  /* Initialize locals. */
  libUsbCtx = NULL;
  libUsbDevHandle = NULL;
  readDataPending = 0;
  readDataCurrentReadIdx = 0;

  /* Initialize LibUsb. */
  if (libusb_init(&libUsbCtx) != (int)LIBUSB_SUCCESS)
  {
    /* Error occurred while initializing LibUsb. Invalidate the context. */
    libUsbCtx = NULL;
  }
  /* Only continue initialization with a valid context. */
  if (libUsbCtx != NULL)
  {
    /* Set the log message verbosity as advised by the documentation. */
#if LIBUSB_API_VERSION >= 0x01000106
    (void)libusb_set_option(libUsbCtx, LIBUSB_OPTION_LOG_LEVEL, (int)LIBUSB_LOG_LEVEL_WARNING);
#else
    libusb_set_debug(libUsbCtx, (int)LIBUSB_LOG_LEVEL_WARNING);
#endif
  }
} /*** end of UsbBulkInit ***/


/************************************************************************************//**
** \brief     Terminates the USB bulk driver.
**
****************************************************************************************/
void UsbBulkTerminate(void)
{
  /* Terminate LibUsb. */
  if (libUsbCtx != NULL)
  {
    libusb_exit(libUsbCtx);
    /* Invalidate the context. */
    libUsbCtx = NULL;
  }
} /*** end of UsbBulkTerminate ***/


/************************************************************************************//**
** \brief     Opens the connection with the USB device.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool UsbBulkOpen(void)
{
  bool result = false;

  /* Reset read buffer management variables. */
  readDataPending = 0;
  readDataCurrentReadIdx = 0;

  /* Only continue with a valid context. */
  if (libUsbCtx != NULL)
  {
    /* Set the result to okay and only update it in case of an error from here on. */
    result = true;
    /* Attempt to open the USB device. */
    libUsbDevHandle = libusb_open_device_with_vid_pid(libUsbCtx, openBltVendorId,
                                                      openBltProductId);
    /* Check if the USB device could be opened. */
    if (libUsbDevHandle == NULL)
    {
      /* Update result. */
      result = false;
    }
    /* Before the USB device can be claimed, it cannot be attached to a kernel driver. */
    if (result)
    {
      if (libusb_kernel_driver_active(libUsbDevHandle, 0) == 1)
      {
        /* Kernel driver active, so detach it. */
        (void)libusb_detach_kernel_driver(libUsbDevHandle, 0);
      }
    }
    /* Attempt to claim the interface. */
    if (result)
    {
      if (libusb_claim_interface(libUsbDevHandle, 0) != (int)LIBUSB_SUCCESS)
      {
        result = false;
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of UsbBulkOpen ***/


/************************************************************************************//**
** \brief     Closes the connection with the USB device.
**
****************************************************************************************/
void UsbBulkClose(void)
{
  /* Release the interface and close the connection with the USB device. */
  if (libUsbDevHandle != NULL)
  {
    (void)libusb_release_interface(libUsbDevHandle, 0);
    libusb_close(libUsbDevHandle);
    /* Invalidate the handler. */
    libUsbDevHandle = NULL;
  }
} /*** end of UsbBulkClose ***/


/************************************************************************************//**
** \brief     Writes data to the USB device.
** \param     data Pointer to byte array with data to write.
** \param     length Number of bytes in the data array.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool UsbBulkWrite(uint8_t const * data, uint16_t length)
{
  bool result = false;
  
  /* Check parameters. */
  assert(data != NULL);
  assert(length > 0);
  
  /* Only continue with valid parameters. */
  if ((data != NULL) && (length > 0)) /*lint !e774 */
  {
    /* Only continue with a valid context and device handle. */
    if ( (libUsbDevHandle != NULL) && (libUsbCtx != NULL) )
    {
      /* Attempt to write the data to the endpoint. */
      int bytesWritten = 0;
      int transferResult = libusb_bulk_transfer(libUsbDevHandle, 0x01, (uint8_t *)data,
                                                (int)length, &bytesWritten, 0);
      /* Check the transfer result. */
      if (transferResult == (int)LIBUSB_SUCCESS)
      {
        /* Check if all data was written. */
        if (bytesWritten == length)
        {
          /* Data successfully written the endpoint. */
          result = true;
        }
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of UsbBulkWrite ***/


/************************************************************************************//**
** \brief     Reads data from the USB device.
** \param     data Pointer to byte array where received data should be stored.
** \param     length Number of bytes to read from the USB device.
** \param     timeout Timeout in milliseconds for the read operation.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool UsbBulkRead(uint8_t * data, uint16_t length, uint32_t timeout)
{
  bool result = false;

  /* Check parameters. */
  assert(data != NULL);
  assert(length > 0);
  assert(length <= USBBULK_READ_DATA_BUFFER_SIZE);

  /* Only continue with valid parameters. */
  if ((data != NULL) && (length > 0) &&
      (length <= USBBULK_READ_DATA_BUFFER_SIZE) ) /*lint !e774 */
  {
    /* Only continue with a valid context and device handle. */
    if ( (libUsbDevHandle != NULL) && (libUsbCtx != NULL) )
    {
      uint16_t totalBytesRead = 0;
      uint8_t * currentWriteDataPtr = data;

      /* First empty out whatever is still pending in the read buffer. */
      uint16_t bytesPendingInBuffer = length;
      if (readDataPending < length)
      {
        bytesPendingInBuffer = readDataPending;
      }
      uint16_t bytesToReadFromDevice = length - bytesPendingInBuffer;
      /* Copy data. */
      for (uint16_t idx = 0; idx < bytesPendingInBuffer; idx++)
      {
        /* Sanity check for the current read indexer/ */
        assert(readDataCurrentReadIdx < USBBULK_READ_DATA_BUFFER_SIZE);
        /* Only copy data if current read indexer is not out of bounds. */
        if (readDataCurrentReadIdx < USBBULK_READ_DATA_BUFFER_SIZE)
        {
          *currentWriteDataPtr = readDataBuffer[readDataCurrentReadIdx];
        }
        /* Update total bytes read counter. */
        totalBytesRead++;
        /* Increment the write data pointer. */
        currentWriteDataPtr++;
        /* Increment current read indexer. */
        readDataCurrentReadIdx++;
        /* Sanity check for data pending counter. */
        assert(readDataPending > 0);
        /* Decrement data pending counter. */
        if (readDataPending > 0)
        {
          readDataPending--;
        }
      }

      /* If more data is still to be read, then perform a read operation of the full
       * endpoint size.
       */
      if (bytesToReadFromDevice > 0)
      {
        /* Sanity check. */
        assert(readDataPending == 0);

        /* Attempt to read data from the endpoint. Should be done in steps of the
         * endpoint buffer size on the target. This is 64 for OpenBLT so exactly the size
         * of our read buffer.
         */
        int bytesRead = 0;
        int transferResult = libusb_bulk_transfer(libUsbDevHandle, 0x81, readDataBuffer,
                                                  USBBULK_READ_DATA_BUFFER_SIZE,
                                                  &bytesRead, timeout);
        if (transferResult == (int)LIBUSB_SUCCESS)
        {
          /* Update read buffer management. */
          readDataPending = (uint8_t)bytesRead;
          readDataCurrentReadIdx = 0;
        }
      }

      /* Copy remaining data from the newly read data buffer contents as long as they
       * are present.
       */
      uint16_t bytesToCopy = bytesToReadFromDevice;
      if (readDataPending < bytesToCopy)
      {
        bytesToCopy = readDataPending;
      }
      /* Copy data. */
      for (uint16_t idx = 0; idx < bytesToCopy; idx++)
      {
        /* Sanity check for the current read indexer/ */
        assert(readDataCurrentReadIdx < USBBULK_READ_DATA_BUFFER_SIZE);
        /* Only copy data if current read indexer is not out of bounds. */
        if (readDataCurrentReadIdx < USBBULK_READ_DATA_BUFFER_SIZE)
        {
          *currentWriteDataPtr = readDataBuffer[readDataCurrentReadIdx];
        }
        /* Update total bytes read counter. */
        totalBytesRead++;
        /* Increment the write data pointer. */
        currentWriteDataPtr++;
        /* Increment current read indexer. */
        readDataCurrentReadIdx++;
        /* Sanity check for data pending counter. */
        assert(readDataPending > 0);
        /* Decrement data pending counter. */
        if (readDataPending > 0)
        {
          readDataPending--;
        }
      }

      /* As a final step, check that all requested data bytes were read. */
      if (totalBytesRead == length)
      {
        result = true;
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of UsbBulkRead ***/


/*********************************** end of usbbulk.c **********************************/

