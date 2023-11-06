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
#include <SetupAPI.h>                       /* for Windows setup and device installer  */
#include <winusb.h>                         /* for Windows interface to winusb.dll     */
#include <strsafe.h>                        /* for safer C string library              */
#include "usbbulk.h"                        /* USB bulk driver                         */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* USB bulk library return values. */
#define UBL_OKAY             (0U)
#define UBL_ERROR            (1U)
#define UBL_TIMEOUT          (2U)


/***************************************************************************************
* Function prototypes
****************************************************************************************/
/* USB bulk library interface. */
static uint8_t UblOpen(LPCGUID guid);
static void    UblClose(void);
static uint8_t UblTransmit(uint8_t* data, uint16_t len);
static uint8_t UblReceive(uint8_t* data, uint16_t len, uint32_t timeout);


/************************************************************************************//**
** \brief     Initializes the USB bulk driver.
**
****************************************************************************************/
void UsbBulkInit(void)
{
  /* Nothing to do here so just leave it empty. */
} /*** end of UsbBulkInit ***/


/************************************************************************************//**
** \brief     Terminates the USB bulk driver.
**
****************************************************************************************/
void UsbBulkTerminate(void)
{
  /* Nothing to do here so just leave it empty. */
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
  if (UblOpen(&deviceGuidOpenBLT) == UBL_OKAY)
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
  UblClose();
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
    if (UblTransmit((uint8_t *)data, length) == UBL_OKAY)
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
    if (UblReceive(data, length, timeout) == UBL_OKAY)
    {
      result = true;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of UsbBulkRead ***/


/****************************************************************************************
*   U S B   B U L K   L I B R A R Y   F U N C T I O N S
****************************************************************************************/

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Max length of the device path. */
#define MAX_DEVPATH_LENGTH    (128)

/** \brief Identifier value of an invalid USB PIPE. */
#define INVALID_PIPE_ID       (255)


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Type for grouping together all USB bulk device related data. */
typedef struct
{
  HANDLE hDev;
  WINUSB_INTERFACE_HANDLE hWinUSBDev;
  UCHAR  pipeBulkIn;
  UCHAR  pipeBulkOut;
  HANDLE evReader;
} tBulkUsbDev;


/****************************************************************************************
* Local data declarations
****************************************************************************************/
static tBulkUsbDev bulkUsbDev;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static HANDLE UblOpenDevice(LPCGUID InterfaceGuid);
static BOOL   UblGetDevicePath(LPCGUID InterfaceGuid, PCHAR DevicePath, size_t BufLen);


/************************************************************************************//**
** \brief     Opens and configures the connection with the USB bulk device.
** \param     guid Pointer to GUID of the USB bulk device as found in the driver's
**            INF-file.
** \return    UBL_OKAY if successful, UBL_ERROR otherwise.
**
****************************************************************************************/
static uint8_t UblOpen(LPCGUID guid)
{
  USB_INTERFACE_DESCRIPTOR ifaceDescriptor;
  WINUSB_PIPE_INFORMATION pipeInfo;
  uint32_t i;

  /* Init device members. */
  bulkUsbDev.hDev = NULL;
  bulkUsbDev.hWinUSBDev = NULL;
  bulkUsbDev.evReader = NULL;
  bulkUsbDev.pipeBulkIn = INVALID_PIPE_ID;
  bulkUsbDev.pipeBulkOut = INVALID_PIPE_ID;
  /* Open the usb device. */
  bulkUsbDev.hDev = UblOpenDevice(guid);
  if (bulkUsbDev.hDev == NULL)
  {
    UblClose();
    return UBL_ERROR;
  }
  /* Init the winusb device. */
  if (WinUsb_Initialize(bulkUsbDev.hDev, &bulkUsbDev.hWinUSBDev) == FALSE)
  {
    UblClose();
    return UBL_ERROR;
  }
  /* Obtain interface settings. */
  if (WinUsb_QueryInterfaceSettings(bulkUsbDev.hWinUSBDev, 0, &ifaceDescriptor) == FALSE)
  {
    UblClose();
    return UBL_ERROR;
  }
  /* Iterate through endpoint to find bulk IN and OUT. */
  for (i = 0; i < ifaceDescriptor.bNumEndpoints; i++)
  {
    if (WinUsb_QueryPipe(bulkUsbDev.hWinUSBDev, 0, (UCHAR)i, &pipeInfo) == FALSE)
    {
      UblClose();
      return UBL_ERROR;
    }

    /* Is this the bulk IN endpoint? */
    if ((pipeInfo.PipeType == UsbdPipeTypeBulk) && \
      (USB_ENDPOINT_DIRECTION_IN(pipeInfo.PipeId)))
    {
      bulkUsbDev.pipeBulkIn = pipeInfo.PipeId;
    }
    /* Is this the bulk OUT endpoint? */
    else if ((pipeInfo.PipeType == UsbdPipeTypeBulk) && \
      (USB_ENDPOINT_DIRECTION_OUT(pipeInfo.PipeId)))
    {
      bulkUsbDev.pipeBulkOut = pipeInfo.PipeId;
    }
  }
  /* Verify that the bulk IN and OUT pipes were found. */
  if ((bulkUsbDev.pipeBulkIn == INVALID_PIPE_ID) || \
    (bulkUsbDev.pipeBulkOut == INVALID_PIPE_ID))
  {
    UblClose();
    return UBL_ERROR;
  }
  /* Create the reader event handle. */
  bulkUsbDev.evReader = CreateEvent(NULL, TRUE, FALSE, NULL);
  if (bulkUsbDev.evReader == NULL)
  {
    UblClose();
    return UBL_ERROR;
  }

  /* Still here so all is okay. */
  return UBL_OKAY;
} /*** end of UblOpen ***/


/************************************************************************************//**
** \brief     Closes the connection with the USB bulk device and frees all the related
**            handles.
**
****************************************************************************************/
static void UblClose(void)
{
  /* close reader event handle */
  if (bulkUsbDev.evReader != NULL)
  {
    (void)CloseHandle(bulkUsbDev.evReader);
    bulkUsbDev.evReader = NULL;
  }
  /* close the winusb device */
  if (bulkUsbDev.hWinUSBDev != NULL)
  {
    (void)WinUsb_Free(bulkUsbDev.hWinUSBDev);
    bulkUsbDev.hWinUSBDev = NULL;
  }
  /* reset bulk OUT and IN pipes */
  bulkUsbDev.pipeBulkOut = INVALID_PIPE_ID;
  bulkUsbDev.pipeBulkIn = INVALID_PIPE_ID;
  /* close the usb device handle  */
  if (bulkUsbDev.hDev != NULL)
  {
    (void)CloseHandle(bulkUsbDev.hDev);
    bulkUsbDev.hDev = NULL;
  }
} /*** end of UblClose ***/


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
static uint8_t UblTransmit(uint8_t * data, uint16_t len)
{
  DWORD bytesWritten = 0;

  /* Submit the transmit request. */
  if (WinUsb_WritePipe(bulkUsbDev.hWinUSBDev, bulkUsbDev.pipeBulkOut, data, len,
                       &bytesWritten, NULL) == 0)
  {
    /* Could not write the data. */
    return UBL_ERROR;
  }
  if (bytesWritten != len)
  {
    /* Not all data was written. */
    return UBL_ERROR;
  }
  /* Still here so all is okay. */
  return UBL_OKAY;
} /*** end of UblTransmit ***/


/************************************************************************************//**
** \brief     Starts the asynchronous reception of the data from the bulk IN pipe. This
**            function makes use of the overlapped functionality, which means the calling
**            thread is placed into sleep mode until the reception is complete.
** \param     data Pointer to byte array where the data will be stored.
** \param     len Number of bytes to receive.
** \param     timeout Max time in milliseconds for the read to complete.
** \return    UBL_OKAY if all bytes were received, UBL_TIMEOUT if a timeout occured,
**            UBL_ERROR otherwise.
**
****************************************************************************************/
static uint8_t UblReceive(uint8_t * data, uint16_t len, uint32_t timeout)
{
  OVERLAPPED osReader = { 0 };            /* overlapped structure for read operations  */
  HANDLE     hArray[1];                   /* event handle array                        */
  DWORD 	   dwRead;                      /* bytes actually read                       */
  DWORD      dwRes;                       /* result from WaitForSingleObject           */
  BOOL       bRes;                        /* result from WinUsb_ReadPipe               */
  DWORD      bytesRead = 0;
  DWORD      totalBytesRead = 0;
  DWORD      dwStartTime;

  /* Get the current time and store it for timeout feature. */
  dwStartTime = GetTickCount();

  /* Create an overlapped structure for read events. */
  osReader.hEvent = bulkUsbDev.evReader;
  /* Build array with events to detect. */
  hArray[0] = osReader.hEvent;

  while (((GetTickCount() - dwStartTime) < timeout) && (totalBytesRead < len))
  {
    /* Start the read operation. */
    bRes = WinUsb_ReadPipe(bulkUsbDev.hWinUSBDev, bulkUsbDev.pipeBulkIn, data, len, 
                           &bytesRead, &osReader);

    /* Upon positive return means the read was completed. */
    if (bRes)
    {
      /* Update the total. */
      totalBytesRead += bytesRead;
      /* Check number of bytes read. */
      if (totalBytesRead == len)
      {
        /* All bytes read from bulk IN pipe. */
        return UBL_OKAY;
      }
      else
      {
        /* No I/O operation is pending, yet still not received all the bytes so
         * restart the read operation.
         */
        continue;
      }
    }
    /* Still here so this is either because of an error or because the read is delayed
     * because of an overlapped operation this is in progress.
     */
    if (GetLastError() != ERROR_IO_PENDING)
    {
      /* Not delayed, but here due to an error. */
      return UBL_ERROR;
    }
    /* Read is delayed so wait for it to complete or a timeout to occur. */
    dwRes = WaitForMultipleObjects(1, hArray, FALSE, timeout);

    /* Process the result. */
    switch (dwRes)
    {
    case WAIT_OBJECT_0: /*lint !e835 */
      /* Read operation returned so check the results. */
      if (!GetOverlappedResult(bulkUsbDev.hDev, &osReader, &dwRead, FALSE))
      {
        if (GetLastError() != ERROR_OPERATION_ABORTED)
        {
          /* Read completed but with error. */
          return UBL_ERROR;
        }
      }
      else
      {
        /* See if all the data was now read. */
        totalBytesRead += dwRead;
        if (totalBytesRead == len)
        {
          return UBL_OKAY;
        }
      }
      break;

    case WAIT_TIMEOUT:
      /* Timeout occurred so abort the pipe operation. */
      (void)WinUsb_AbortPipe(bulkUsbDev.hWinUSBDev, bulkUsbDev.pipeBulkIn);
      return UBL_TIMEOUT;

    default:
      /* Unexpected so abort the pipe operation. */
      (void)WinUsb_AbortPipe(bulkUsbDev.hWinUSBDev, bulkUsbDev.pipeBulkIn);
      return UBL_ERROR;
    }
  }
  /* Still here so read incomplete due to timeout. */
  return UBL_ERROR;
} /*** end of UblReceive ***/


/************************************************************************************//**
** \brief     Opens the USB device and obtains its handle, which is needed to obtain a 
**            handle to the WinUSB device.
** \param     InterfaceGuid InterfaceGuid GUID of the device (not its class though).
** \return    The handle to the USB device or NULL.
**
****************************************************************************************/
static HANDLE UblOpenDevice(LPCGUID InterfaceGuid)
{
  HANDLE hDev = NULL;
  char devicePath[MAX_DEVPATH_LENGTH] = { 0 };
  BOOL retVal;

  retVal = UblGetDevicePath(InterfaceGuid, devicePath, sizeof(devicePath));
  if (retVal)
  {
    hDev = CreateFile(devicePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE |
                      FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL |
                      FILE_FLAG_OVERLAPPED, NULL);
  }
  return hDev;
} /*** end of UblOpenDevice ***/


/************************************************************************************//**
** \brief     Attempts to obtain the path to the WinUSB device, based on its GUID.
** \param     InterfaceGuid InterfaceGuid GUID of the device (not its class though).
** \param     DevicePath Pointer to where the path should be stored.
** \param     BufLen Maximum length of the path.
** \return    TRUE if the device path was obtained, FALSE otherwise.
**
****************************************************************************************/
static BOOL UblGetDevicePath(LPCGUID InterfaceGuid, PCHAR DevicePath, size_t BufLen)
{
  BOOL bResult;
  HDEVINFO deviceInfo;
  SP_DEVICE_INTERFACE_DATA interfaceData;
  PSP_DEVICE_INTERFACE_DETAIL_DATA detailData;
  ULONG length;
  ULONG requiredLength = 0;
  HRESULT hr;

  deviceInfo = SetupDiGetClassDevs(InterfaceGuid, NULL, NULL, DIGCF_PRESENT | 
                                   DIGCF_DEVICEINTERFACE);

  interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
  (void)SetupDiEnumDeviceInterfaces(deviceInfo, NULL, InterfaceGuid, 0, 
                                    &interfaceData);

  (void)SetupDiGetDeviceInterfaceDetail(deviceInfo, &interfaceData, NULL, 0, &requiredLength,
                                        NULL);
  if (requiredLength == 0)
  {
    (void)SetupDiDestroyDeviceInfoList(deviceInfo);
    return FALSE;
  }
  detailData = LocalAlloc(LMEM_FIXED, requiredLength);
  if (detailData == NULL)
  {
    (void)SetupDiDestroyDeviceInfoList(deviceInfo);
    return FALSE;
  }
  detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
  length = requiredLength;
  bResult = SetupDiGetDeviceInterfaceDetail(deviceInfo, &interfaceData, detailData,
                                            length, &requiredLength, NULL);

  if (bResult == FALSE)
  {
    LocalFree(detailData);
    return FALSE;
  }

  hr = StringCchCopy(DevicePath, BufLen, detailData->DevicePath);
  if (FAILED(hr))
  {
    (void)SetupDiDestroyDeviceInfoList(deviceInfo);
    LocalFree(detailData);
  }

  LocalFree(detailData);

  return bResult;
} /*** end of UblGetDevicePath ***/


/*********************************** end of usbbulk.c **********************************/

