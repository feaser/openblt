/************************************************************************************//**
* \file         port/windows/usbbulk.c
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
#include <windows.h>                        /* for Windows API                         */
#include "usbbulk.h"                        /* USB bulk driver                         */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* USB bulk driver return values. */
#define UBL_ERROR            (0u)
#define UBL_OKAY             (1u)


/****************************************************************************************
* Type definitions
****************************************************************************************/
/* USB bulk driver interface functions. */
typedef uint8_t (__stdcall * tUsbBulkLibFuncOpen)(LPGUID guid);
typedef void    (__stdcall * tUsbBulkLibFuncClose)(void);
typedef uint8_t (__stdcall * tUsbBulkLibFuncTransmit)(uint8_t * data, uint16_t len);
typedef uint8_t (__stdcall * tUsbBulkLibFuncReceive)(uint8_t * data, uint16_t len, uint32_t timeout);


/***************************************************************************************
* Function prototypes
****************************************************************************************/
/* USB bulk driver handling. */
static void UsbBulkLibLoadDll(void);
static void UsbBulkLibUnloadDll(void);
static uint8_t UsbBulkLibFuncOpen(LPGUID guid);
static void UsbBulkLibFuncClose(void);
static uint8_t UsbBulkLibFuncTransmit(uint8_t * data, uint16_t len);
static uint8_t UsbBulkLibFuncReceive(uint8_t * data, uint16_t len, uint32_t timeout);


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Handle to the USB bulk driver dynamic link library. */
static HINSTANCE usbBulkLibDllHandle;

/** \brief Function pointer to the USB bulk driver library Open function. */
static tUsbBulkLibFuncOpen usbBulkLibFuncOpenPtr;

/** \brief Function pointer to the USB bulk driver library Close function. */
static tUsbBulkLibFuncClose usbBulkLibFuncClosePtr;

/** \brief Function pointer to the USB bulk driver library Transmit function. */
static tUsbBulkLibFuncTransmit usbBulkLibFuncTransmitPtr;

/** \brief Function pointer to the USB bulk driver library Receive function. */
static tUsbBulkLibFuncReceive usbBulkLibFuncReceivePtr;


/************************************************************************************//**
** \brief     Initializes the USB bulk driver.
**
****************************************************************************************/
void UsbBulkInit(void)
{
  /* Initialize locals. */
  usbBulkLibDllHandle = NULL;
  /* Reset library function pointers. */
  usbBulkLibFuncOpenPtr = NULL;
  usbBulkLibFuncClosePtr = NULL;
  usbBulkLibFuncTransmitPtr = NULL;
  usbBulkLibFuncReceivePtr = NULL;
  /* Perform initialization of USB bulk driver API. */
  UsbBulkLibLoadDll();
} /*** end of UsbBulkInit ***/


/************************************************************************************//**
** \brief     Terminates the USB bulk driver.
**
****************************************************************************************/
void UsbBulkTerminate(void)
{
  /* Perform termination of USB bulk driver API. */
  UsbBulkLibUnloadDll();
} /*** end of UsbBulkTerminate ***/


/************************************************************************************//**
** \brief     Opens the connection with the USB device.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool UsbBulkOpen(void)
{
  /* This must be the same GUID as the one declared in the *.inf driver file. */
  static GUID deviceGuidOpenBLT = 
  {
    0x807999C3, 0xE4E0, 0x40EA, 0x81, 0x88, 0x48, 0xE8, 0x52, 0xB5, 0x4F, 0x2B
  };
  bool result = false;

  /* Open the connection with the USB device. */
  if (UsbBulkLibFuncOpen(&deviceGuidOpenBLT) == UBL_OKAY)
  {
    result = true;
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
  /* Close the connection with the USB device. */
  UsbBulkLibFuncClose();
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
    /* Write data to the USB device. */
    if (UsbBulkLibFuncTransmit((uint8_t *)data, length) == UBL_OKAY)
    {
      result = true;
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

  (void)timeout;

  /* Check parameters. */
  assert(data != NULL);
  assert(length > 0);

  /* Only continue with valid parameters. */
  if ((data != NULL) && (length > 0)) /*lint !e774 */
  {
    /* Read data from the USB device. */
    if (UsbBulkLibFuncReceive(data, length, timeout) == UBL_OKAY)
    {
      result = true;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of UsbBulkRead ***/


/************************************************************************************//**
** \brief     Loads the USB bulk driver DLL and initializes the API function pointers.
**
****************************************************************************************/
static void UsbBulkLibLoadDll(void)
{
  /* Start out by resetting the API function pointers. */
  usbBulkLibFuncOpenPtr = NULL;
  usbBulkLibFuncClosePtr = NULL;
  usbBulkLibFuncTransmitPtr = NULL;
  usbBulkLibFuncReceivePtr = NULL;

  /* Attempt to load the library and obtain a handle to it. */
  usbBulkLibDllHandle = LoadLibrary("UsbBulkLib");

  /* Assert libary handle. */
  assert(usbBulkLibDllHandle != NULL);

  /* Only continue if the library was successfully loaded */
  if (usbBulkLibDllHandle != NULL) /*lint !e774 */
  {
    /* Set UblOpen function pointer. */
    usbBulkLibFuncOpenPtr = (tUsbBulkLibFuncOpen)GetProcAddress(usbBulkLibDllHandle, "UblOpen");
    /* Set UblClose function pointer. */
    usbBulkLibFuncClosePtr = (tUsbBulkLibFuncClose)GetProcAddress(usbBulkLibDllHandle, "UblClose");
    /* Set UblTransmit function pointer. */
    usbBulkLibFuncTransmitPtr = (tUsbBulkLibFuncTransmit)GetProcAddress(usbBulkLibDllHandle, "UblTransmit");
    /* Set UblReceive function pointer. */
    usbBulkLibFuncReceivePtr = (tUsbBulkLibFuncReceive)GetProcAddress(usbBulkLibDllHandle, "UblReceive");
  }
} /*** end of UsbBulkLibLoadDll ***/


/************************************************************************************//**
** \brief     Unloads the USB bulk driver DLL and resets the API function pointers.
**
****************************************************************************************/
static void UsbBulkLibUnloadDll(void)
{
  /* Reset the API function pointers. */
  usbBulkLibFuncOpenPtr = NULL;
  usbBulkLibFuncClosePtr = NULL;
  usbBulkLibFuncTransmitPtr = NULL;
  usbBulkLibFuncReceivePtr = NULL;

  /* Unload the library and invalidate its handle. */
  if (usbBulkLibDllHandle != NULL) 
  {
    (void)FreeLibrary(usbBulkLibDllHandle);
    usbBulkLibDllHandle = NULL;
  }
} /*** end of UsbBulkLibUnloadDll ***/


/************************************************************************************//**
** \brief     Opens and configures the connection with the USB bulk device.
** \param     guid Pointer to GUID of the USB bulk device as found in the driver's 
**            INF-file.
** \return    UBL_OKAY if successful, UBL_ERROR otherwise.
**
****************************************************************************************/
static uint8_t UsbBulkLibFuncOpen(LPGUID guid)
{
  uint8_t result = UBL_ERROR;

  /* Check function pointer and library handle. */
  assert(usbBulkLibFuncOpenPtr != NULL);
  assert(usbBulkLibDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((usbBulkLibFuncOpenPtr != NULL) && (usbBulkLibDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = usbBulkLibFuncOpenPtr(guid);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of UsbBulkLibFuncOpen ***/


/************************************************************************************//**
** \brief     Closes the connection with the USB bulk device and frees all the related
**            handles.
**
****************************************************************************************/
static void UsbBulkLibFuncClose(void)
{
  /* Check function pointer and library handle. */
  assert(usbBulkLibFuncClosePtr != NULL);
  assert(usbBulkLibDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((usbBulkLibFuncClosePtr != NULL) && (usbBulkLibDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    usbBulkLibFuncClosePtr();
  }
} /*** end of UsbBulkLibFuncClose ***/


/************************************************************************************//**
** \brief     Starts transmission of the data on the bulk OUT pipe. Because USB bulk 
**            transmissions are quick, this function does not use the overlapped
**            functionality, which means the caller is blocked until the tranmission
**            completed.
** \param     data Pointer to byte array with transmit data.
** \param     len Number of bytes to transmit.
** \return    UBL_OKAY if successful, UBL_ERROR otherwise.
**
****************************************************************************************/
static uint8_t UsbBulkLibFuncTransmit(uint8_t * data, uint16_t len)
{
  uint8_t result = UBL_ERROR;

  /* Check function pointer and library handle. */
  assert(usbBulkLibFuncTransmitPtr != NULL);
  assert(usbBulkLibDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((usbBulkLibFuncTransmitPtr != NULL) && (usbBulkLibDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = usbBulkLibFuncTransmitPtr(data, len);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of UsbBulkLibFuncTransmit ***/


/************************************************************************************//**
** \brief     Starts the asynchronous reception of the data from the bulk IN pipe. This
**            function makes use of the overlapped functionality, which means the calling
**            thread is placed into sleep mode until the reception is complete.
** \param     data Pointer to byte array where the data will be stored.
** \param     len Number of bytes to receive.
** \param     timeout Maximum time in milliseconds for the read to complete.
** \return    UBL_OKAY if successful, UBL_TIMEOUT if failure due to timeout or 
**            UBL_ERROR otherwise.
**
****************************************************************************************/
static uint8_t UsbBulkLibFuncReceive(uint8_t * data, uint16_t len, uint32_t timeout)
{
  uint8_t result = UBL_ERROR;

  /* Check function pointer and library handle. */
  assert(usbBulkLibFuncReceivePtr != NULL);
  assert(usbBulkLibDllHandle != NULL);

  /* Only continue with valid function pointer and library handle. */
  if ((usbBulkLibFuncReceivePtr != NULL) && (usbBulkLibDllHandle != NULL)) /*lint !e774 */
  {
    /* Call library function. */
    result = usbBulkLibFuncReceivePtr(data, len, timeout);
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of UsbBulkLibFuncReceive ***/


/*********************************** end of usbbulk.c **********************************/

