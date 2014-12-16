//*****************************************************************************
//
// usb_bulk_structs.c - Data structures defining this bulk USB device.
//
// Copyright (c) 2008-2013 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 1.1 of the DK-TM4C123G Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "driverlib/usbdrv.h"
#include "usblib/usblib.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdbulk.h"
#include "usb_bulk_structs.h"

//*****************************************************************************
//
// The languages supported by this device.
//
//*****************************************************************************
const uint8_t g_pui8LangDescriptor[] =
{
    4,
    USB_DTYPE_STRING,
    USBShort(USB_LANG_EN_US)
};

//*****************************************************************************
//
// The manufacturer string.
//
//*****************************************************************************
const uint8_t g_pui8ManufacturerString[] =
{
    (12 + 1) * 2,
    USB_DTYPE_STRING,
    'O', 0, 'p', 0, 'e', 0, 'n', 0, 'B', 0, 'L', 0, 'T', 0,
    ' ', 0, 'U', 0, 's', 0, 'e', 0, 'r', 0
};

//*****************************************************************************
//
// The product string.
//
//*****************************************************************************
const uint8_t g_pui8ProductString[] =
{
    (18 + 1) * 2,
    USB_DTYPE_STRING,
    'W', 0, 'i', 0, 'n', 0, 'U', 0, 'S', 0, 'B', 0, ' ', 0, 
    'B', 0, 'u', 0, 'l', 0, 'k', 0, ' ', 0,
    'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0
};

//*****************************************************************************
//
// The serial number string.
//
//*****************************************************************************
const uint8_t g_pui8SerialNumberString[] =
{
    (8 + 1) * 2,
    USB_DTYPE_STRING,
    '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0
};

//*****************************************************************************
//
// The data interface description string.
//
//*****************************************************************************
const uint8_t g_pui8DataInterfaceString[] =
{
    (21 + 1) * 2,
    USB_DTYPE_STRING,
    'W', 0, 'i', 0, 'n', 0, 'U', 0, 'S', 0, 'B', 0, ' ', 0, 
    'B', 0, 'u', 0, 'l', 0, 'k', 0, ' ', 0,
    'I', 0, 'n', 0, 't', 0, 'e', 0, 'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0
};

//*****************************************************************************
//
// The configuration description string.
//
//*****************************************************************************
const uint8_t g_pui8ConfigString[] =
{
    (23 + 1) * 2,
    USB_DTYPE_STRING,
    'B', 0, 'u', 0, 'l', 0, 'k', 0, ' ', 0, 'D', 0, 'a', 0, 't', 0,
    'a', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0, 'f', 0, 'i', 0, 'g', 0,
    'u', 0, 'r', 0, 'a', 0, 't', 0, 'i', 0, 'o', 0, 'n', 0
};

//*****************************************************************************
//
// The descriptor string table.
//
//*****************************************************************************
const uint8_t *const g_ppui8StringDescriptors[] =
{
    g_pui8LangDescriptor,
    g_pui8ManufacturerString,
    g_pui8ProductString,
    g_pui8SerialNumberString,
    g_pui8DataInterfaceString,
    g_pui8ConfigString
};

#define NUM_STRING_DESCRIPTORS (sizeof(g_ppui8StringDescriptors) /            \
                                sizeof(uint8_t *))

//*****************************************************************************
//
// The bulk device initialization and customization structures. In this case,
// we are using USBBuffers between the bulk device class driver and the
// application code. The function pointers and callback data values are set
// to insert a buffer in each of the data channels, transmit and receive.
//
// With the buffer in place, the bulk channel callback is set to the relevant
// channel function and the callback data is set to point to the channel
// instance data. The buffer, in turn, has its callback set to the application
// function and the callback data set to our bulk instance structure.
//
//*****************************************************************************
extern const tUSBBuffer g_sTxBuffer;
extern const tUSBBuffer g_sRxBuffer;

tUSBDBulkDevice g_sBulkDevice =
{
    0x1d50,
    0x60ac,
    500,
    USB_CONF_ATTR_SELF_PWR,
    USBBufferEventCallback,
    (void *)&g_sRxBuffer,
    USBBufferEventCallback,
    (void *)&g_sTxBuffer,
    g_ppui8StringDescriptors,
    NUM_STRING_DESCRIPTORS
};

//*****************************************************************************
//
// Receive buffer (from the USB perspective).
//
//*****************************************************************************
uint8_t g_pui8USBRxBuffer[BULK_BUFFER_SIZE];
uint8_t g_pui8RxBufferWorkspace[USB_BUFFER_WORKSPACE_SIZE];
const tUSBBuffer g_sRxBuffer =
{
    false,                           // This is a receive buffer.
    UsbBulkRxHandler,                // pfnCallback
    (void *)&g_sBulkDevice,          // Callback data is our device pointer.
    USBDBulkPacketRead,              // pfnTransfer
    USBDBulkRxPacketAvailable,       // pfnAvailable
    (void *)&g_sBulkDevice,          // pvHandle
    g_pui8USBRxBuffer,               // pi8Buffer
    BULK_BUFFER_SIZE,                // ui32BufferSize
    g_pui8RxBufferWorkspace          // pvWorkspace
};

//*****************************************************************************
//
// Transmit buffer (from the USB perspective).
//
//*****************************************************************************
uint8_t g_pui8USBTxBuffer[BULK_BUFFER_SIZE];
uint8_t g_pui8TxBufferWorkspace[USB_BUFFER_WORKSPACE_SIZE];
const tUSBBuffer g_sTxBuffer =
{
    true,                            // This is a transmit buffer.
    UsbBulkTxHandler,                // pfnCallback
    (void *)&g_sBulkDevice,          // Callback data is our device pointer.
    USBDBulkPacketWrite,             // pfnTransfer
    USBDBulkTxPacketAvailable,       // pfnAvailable
    (void *)&g_sBulkDevice,          // pvHandle
    g_pui8USBTxBuffer,               // pi8Buffer
    BULK_BUFFER_SIZE,                // ui32BufferSize
    g_pui8TxBufferWorkspace          // pvWorkspace
};
