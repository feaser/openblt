//****************************************************************************
//
// usbdcomp.c - USB composite device class driver.
//
// Copyright (c) 2010-2013 Texas Instruments Incorporated.  All rights reserved.
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
// This is part of revision 1.1 of the Tiva USB Library.
//
//****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/usbdrv.h"
#include "usblib/usblib.h"
#include "usblib/usblibpriv.h"
#include "usblib/usb-ids.h"
#include "usblib/usbcdc.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdcdc.h"
#include "usblib/device/usbdcomp.h"

//****************************************************************************
//
//! \addtogroup composite_device_class_api
//! @{
//
//****************************************************************************

//****************************************************************************
//
// Device Descriptor.  This is stored in RAM to allow several fields to be
// changed at runtime based on the client's requirements.
//
//****************************************************************************
static uint8_t g_pui8CompDeviceDescriptor[] =
{
    18,                     // Size of this structure.
    USB_DTYPE_DEVICE,       // Type of this structure.
    USBShort(0x110),        // USB version 1.1 (if we say 2.0, hosts assume
                            // high-speed - see USB 2.0 spec 9.2.6.6)
    USB_CLASS_MISC,         // USB Device Class (spec 5.1.1)
    USB_MISC_SUBCLASS_COMMON, // USB Device Sub-class (spec 5.1.1)
    USB_MISC_PROTOCOL_IAD,  // USB Device protocol (spec 5.1.1)
    64,                     // Maximum packet size for default pipe.
    USBShort(0),            // Vendor ID (filled in during USBDCompositeInit).
    USBShort(0),            // Product ID (filled in during USBDCompositeInit).
    USBShort(0x100),        // Device Version BCD.
    1,                      // Manufacturer string identifier.
    2,                      // Product string identifier.
    3,                      // Product serial number.
    1                       // Number of configurations.
};

//****************************************************************************
//
// Composite class device configuration descriptor.
//
// It is vital that the configuration descriptor bConfigurationValue field
// (byte 6) is 1 for the first configuration and increments by 1 for each
// additional configuration defined here.  This relationship is assumed in the
// device stack for simplicity even though the USB 2.0 specification imposes
// no such restriction on the bConfigurationValue values.
//
// Note that this structure is deliberately located in RAM since we need to
// be able to patch some values in it based on client requirements.
//
//****************************************************************************
static const uint8_t g_pui8CompConfigDescriptor[] =
{
    //
    // Configuration descriptor header.
    //
    9,                          // Size of the configuration descriptor.
    USB_DTYPE_CONFIGURATION,    // Type of this descriptor.
    USBShort(0),                // The total size of this full structure.
    0,                          // The number of interfaces in this
                                // configuration, this will be filled by
                                // the class as it discovers all classes
                                // supported.
    1,                          // The unique value for this configuration.
    0,                          // The string identifier that describes this
                                // configuration.
    USB_CONF_ATTR_BUS_PWR,      // .
    250,                        // The maximum power in 2mA increments.
};

//****************************************************************************
//
// Byte offsets used to access various fields in our index/interface/endpoint
// lookup table (tUSBDCompositeDevice.pui32DeviceWorkspace).  This workspace
// contains one 4 byte entry per device. The LSB is the device index, next byte
// is the number of the first interface not within this device, next byte is
// the number of the first IN endpoint not within this device and the final
// byte is the number of the first OUT endpoint not within this device.  Using
// this simple table we can reasonably quickly cross-reference index with
// interface and endpoint numbers.
//
//****************************************************************************
#define LOOKUP_INDEX_BYTE       0
#define LOOKUP_INTERFACE_BYTE   1
#define LOOKUP_IN_END_BYTE      2
#define LOOKUP_OUT_END_BYTE     3

//****************************************************************************
//
// A marker used to indicate an invalid index into the device table.
//
//****************************************************************************
#define INVALID_DEVICE_INDEX    0xFFFFFFFF

//****************************************************************************
//
// Various internal handlers needed by this class.
//
//****************************************************************************
static void HandleDisconnect(void *pvCompositeInstance);
static void InterfaceChange(void *pvCompositeInstance, uint8_t ui8InterfaceNum,
                            uint8_t ui8AlternateSetting);
static void ConfigChangeHandler(void *pvCompositeInstance, uint32_t ui32Value);
static void DataSent(void *pvCompositeInstance, uint32_t ui32Info);
static void DataReceived(void *pvCompositeInstance, uint32_t ui32Info);
static void HandleEndpoints(void *pvCompositeInstance, uint32_t ui32Status);
static void HandleRequests(void *pvCompositeInstance, tUSBRequest *psUSBRequest);
static void SuspendHandler(void *pvCompositeInstance);
static void ResumeHandler(void *pvCompositeInstance);
static void ResetHandler(void *pvCompositeInstance);
static void HandleDevice(void *pvCompositeInstance, uint32_t ui32Request,
                         void *pvRequestData);
static void GetDescriptor(void *pvCompositeInstance, tUSBRequest *psUSBRequest);

//****************************************************************************
//
// Configuration Descriptor.
//
//****************************************************************************
tConfigHeader *g_ppCompConfigDescriptors[1];

//****************************************************************************
//
// The device information structure for the USB Composite device.
//
//****************************************************************************
const tCustomHandlers g_sCompHandlers =
{
    //
    // GetDescriptor
    //
    GetDescriptor,

    //
    // RequestHandler
    //
    HandleRequests,

    //
    // InterfaceChange
    //
    InterfaceChange,

    //
    // ConfigChange
    //
    ConfigChangeHandler,

    //
    // DataReceived
    //
    DataReceived,

    //
    // DataSentCallback
    //
    DataSent,

    //
    // ResetHandler
    //
    ResetHandler,

    //
    // SuspendHandler
    //
    SuspendHandler,

    //
    // ResumeHandler
    //
    ResumeHandler,

    //
    // DisconnectHandler
    //
    HandleDisconnect,

    //
    // EndpointHandler
    //
    HandleEndpoints,

    //
    // DeviceHandler
    //
    HandleDevice,
};

//****************************************************************************
//
// Use the lookup table from the field pui32DeviceWorkspace in the
// tUSBDCompositeDevice structure to determine which device to call given a
// particular composite device interface number.
//
// The returned value is the index into psDevice->tCompositeEntry indicating
// the device which contains this interface or INVALID_DEVICE_INDEX if no
// device contains the passed interface number.
//
//****************************************************************************
static uint32_t
InterfaceToIndex(tUSBDCompositeDevice *psDevice, uint32_t ui32Interface)
{
    uint32_t ui32Loop;
    uint32_t ui32Lookup;

    //
    // Check each lookup entry in turn.
    //
    for(ui32Loop = 0; ui32Loop < psDevice->ui32NumDevices; ui32Loop++)
    {
        //
        // Get the look up value from the device.
        //
        ui32Lookup = psDevice->psDevices[ui32Loop].ui32DeviceWorkspace;
        ui32Lookup = (ui32Lookup >> (8 * LOOKUP_INTERFACE_BYTE)) & 0xff;

        //
        // If the desired interface number is lower than the value in the
        // current lookup table entry, we have found the desired device so
        // return its index.
        //
        if(ui32Interface < ui32Lookup)
        {
            return(ui32Loop);
        }
    }

    //
    // If we get here, an invalid interface number was passed so return a
    // marker to indicate this.
    //
    return(INVALID_DEVICE_INDEX);
}

//****************************************************************************
//
// Use the lookup table from the field pui32DeviceWorkspace in the
// tUSBDCompositeDevice structure to determine which device to call given a
// particular composite device endpoint number.
//
// The returned value is the index into psDevice->tCompositeEntry indicating
// the device which contains this endpoint or INVALID_DEVICE_INDEX if no
// device contains the passed endpoint number.
//
//****************************************************************************
static uint32_t
EndpointToIndex(tUSBDCompositeDevice *psDevice, uint32_t ui32Endpoint,
                bool bInEndpoint)
{
    uint32_t ui32Loop, ui32EndpointByte, ui32Lookup;

    //
    // Are we considering an IN or OUT endpoint?
    //
    ui32EndpointByte = bInEndpoint ? LOOKUP_IN_END_BYTE : LOOKUP_OUT_END_BYTE;

    //
    // Check each lookup entry in turn.
    //
    for(ui32Loop = 0; ui32Loop < psDevice->ui32NumDevices; ui32Loop++)
    {
        //
        // Get the look up byte from the device.
        //
        ui32Lookup = psDevice->psDevices[ui32Loop].ui32DeviceWorkspace;
        ui32Lookup = (ui32Lookup >> (ui32EndpointByte * 8)) & 0xff;

        //
        // If the desired endpoint number is lower than the value in the
        // current lookup table entry, we have found the desired device so
        // return its index.
        //
        if(ui32Endpoint < ui32Lookup)
        {
            return(ui32Loop);
        }
    }

    //
    // If we get here, an invalid endpoint number was passed so return a
    // marker to indicate this.
    //
    return(INVALID_DEVICE_INDEX);
}


//****************************************************************************
//
// This function will check if any device classes need a get descriptor
// handler called.
//
//****************************************************************************
static void
GetDescriptor(void *pvCompositeInstance, tUSBRequest *psUSBRequest)
{
    uint32_t ui32Idx;
    const tDeviceInfo *psDeviceInfo;
    tUSBDCompositeDevice *psCompDevice;

    //
    // Create the composite device pointer.
    //
    psCompDevice = (tUSBDCompositeDevice *)pvCompositeInstance;

    //
    // Determine which device this request is intended for.  We have to be
    // careful here to send this to the callback for the correct device
    // depending upon whether it is a request sent to the device, the interface
    // or the endpoint.
    //
    switch(psUSBRequest->bmRequestType & USB_RTYPE_RECIPIENT_M)
    {
        case USB_RTYPE_INTERFACE:
        {
            ui32Idx = InterfaceToIndex(psCompDevice,
                                       (psUSBRequest->wIndex & 0xFF));
            break;
        }

        case USB_RTYPE_ENDPOINT:
        {
            ui32Idx = EndpointToIndex(psCompDevice,
                            (psUSBRequest->wIndex & 0x0F),
                            (psUSBRequest->wIndex & 0x80) ? true : false);
            break;
        }

        //
        // Requests sent to the device or any other recipient can't be
        // handled here since we have no way of telling where they are
        // supposed to be handled.  As a result, we just stall them.
        //
        // If your composite device has some device-specific descriptors,
        // you should add code here to handle them.
        //
        case USB_RTYPE_DEVICE:
        case USB_RTYPE_OTHER:
        default:
        {
            ui32Idx = INVALID_DEVICE_INDEX;
            break;
        }
    }

    //
    // Did we find a device class to pass the request to?
    //
    if(ui32Idx != INVALID_DEVICE_INDEX)
    {
        //
        // Get a pointer to the individual device instance.
        //
        psDeviceInfo = psCompDevice->psDevices[ui32Idx].psDevInfo;

        //
        // Does this device have a GetDescriptor callback?
        //
        if(psDeviceInfo->psCallbacks->pfnGetDescriptor)
        {
            //
            // Remember this device index so that we can correctly route any
            // data notification callbacks to it.
            //
            psCompDevice->sPrivateData.ui32EP0Owner = ui32Idx;

            //
            // Call the device to retrieve the descriptor.
            //
            psDeviceInfo->psCallbacks->pfnGetDescriptor(
               psCompDevice->psDevices[ui32Idx].pvInstance, psUSBRequest);
        }
        else
        {
            //
            // Oops - we can't satisfy the request so stall EP0 to indicate
            // an error.
            //
            USBDCDStallEP0(USBBaseToIndex(
                              psCompDevice->sPrivateData.ui32USBBase));
        }
    }
    else
    {
        //
        // We are unable to satisfy the descriptor request so stall EP0 to
        // indicate an error.
        //
        USBDCDStallEP0(USBBaseToIndex(
                              psCompDevice->sPrivateData.ui32USBBase));
    }
}

//****************************************************************************
//
// This function will check if any device classes need an suspend handler
// called.
//
//****************************************************************************
static void
SuspendHandler(void *pvCompositeInstance)
{
    uint32_t ui32Idx;
    tUSBDCompositeDevice *psCompDevice;
    const tDeviceInfo *psDeviceInfo;
    void *pvDeviceInst;

    ASSERT(pvCompositeInstance != 0);

    //
    // Create the device instance pointer.
    //
    psCompDevice = (tUSBDCompositeDevice *)pvCompositeInstance;

    //
    // Inform the application that the device has resumed.
    //
    if(psCompDevice->pfnCallback)
    {
        psCompDevice->pfnCallback(pvCompositeInstance, USB_EVENT_SUSPEND,
                                       0, 0);
    }

    for(ui32Idx = 0; ui32Idx < psCompDevice->ui32NumDevices; ui32Idx++)
    {
        psDeviceInfo = psCompDevice->psDevices[ui32Idx].psDevInfo;
        pvDeviceInst = psCompDevice->psDevices[ui32Idx].pvInstance;

        if(psDeviceInfo->psCallbacks->pfnSuspendHandler)
        {
            psDeviceInfo->psCallbacks->pfnSuspendHandler(pvDeviceInst);
        }
    }
}

//****************************************************************************
//
// This function will check if any device classes need an resume handler
// called.
//
//****************************************************************************
static void
ResumeHandler(void *pvCompositeInstance)
{
    uint32_t ui32Idx;
    tUSBDCompositeDevice *psCompDevice;
    const tDeviceInfo *psDeviceInfo;
    void *pvDeviceInst;

    ASSERT(pvCompositeInstance != 0);

    //
    // Create the device instance pointer.
    //
    psCompDevice = (tUSBDCompositeDevice *)pvCompositeInstance;

    //
    // Inform the application that the device has resumed.
    //
    if(psCompDevice->pfnCallback)
    {
        psCompDevice->pfnCallback(pvCompositeInstance, USB_EVENT_RESUME,
                                       0, 0);
    }

    for(ui32Idx = 0; ui32Idx < psCompDevice->ui32NumDevices; ui32Idx++)
    {
        psDeviceInfo = psCompDevice->psDevices[ui32Idx].psDevInfo;
        pvDeviceInst = psCompDevice->psDevices[ui32Idx].pvInstance;

        if(psDeviceInfo->psCallbacks->pfnResumeHandler)
        {
            psDeviceInfo->psCallbacks->pfnResumeHandler(pvDeviceInst);
        }
    }
}

//****************************************************************************
//
// This function will check if any device classes need an reset handler
// called.
//
//****************************************************************************
static void
ResetHandler(void *pvCompositeInstance)
{
    uint32_t ui32Idx;
    tUSBDCompositeDevice *psCompDevice;
    const tDeviceInfo *psDeviceInfo;
    void *pvDeviceInst;

    ASSERT(pvCompositeInstance != 0);

    //
    // Create the device instance pointer.
    //
    psCompDevice = (tUSBDCompositeDevice *)pvCompositeInstance;

    //
    // Inform the application that the device has been connected.
    //
    if(psCompDevice->pfnCallback)
    {
        psCompDevice->pfnCallback(pvCompositeInstance,
                                       USB_EVENT_CONNECTED, 0, 0);
    }

    for(ui32Idx = 0; ui32Idx < psCompDevice->ui32NumDevices; ui32Idx++)
    {
        psDeviceInfo = psCompDevice->psDevices[ui32Idx].psDevInfo;
        pvDeviceInst = psCompDevice->psDevices[ui32Idx].pvInstance;

        if(psDeviceInfo->psCallbacks->pfnResetHandler)
        {
            psDeviceInfo->psCallbacks->pfnResetHandler(pvDeviceInst);
        }
    }
}

//****************************************************************************
//
// This function is called to handle data being set to the host so that the
// application callback can be called when the data has been transferred.
//
//****************************************************************************
static void
DataSent(void *pvCompositeInstance, uint32_t ui32Info)
{
    uint32_t ui32Idx;
    const tDeviceInfo *psDeviceInfo;
    tUSBDCompositeDevice *psCompDevice;

    //
    // Create the device instance pointer.
    //
    psCompDevice = (tUSBDCompositeDevice *)pvCompositeInstance;

    //
    // Pass this notification on to the device which last handled a
    // transaction on endpoint 0 (assuming we know who that was).
    //
    ui32Idx = psCompDevice->sPrivateData.ui32EP0Owner;

    if(ui32Idx != INVALID_DEVICE_INDEX)
    {
        psDeviceInfo = psCompDevice->psDevices[ui32Idx].psDevInfo;

        if(psDeviceInfo->psCallbacks->pfnDataSent)
        {
            psDeviceInfo->psCallbacks->pfnDataSent(
                psCompDevice->psDevices[ui32Idx].pvInstance, ui32Info);
        }
    }
}

//****************************************************************************
//
// This function is called to handle data being received back from the host so
// that the application callback can be called when the new data is ready.
//
//****************************************************************************
static void
DataReceived(void *pvCompositeInstance, uint32_t ui32Info)
{
    uint32_t ui32Idx;
    const tDeviceInfo *psDeviceInfo;
    tUSBDCompositeDevice *psCompDevice;

    //
    // Create the device instance pointer.
    //
    psCompDevice = (tUSBDCompositeDevice *)pvCompositeInstance;

    //
    // Pass this notification on to the device which last handled a
    // transaction on endpoint 0 (assuming we know who that was).
    //
    ui32Idx = psCompDevice->sPrivateData.ui32EP0Owner;

    if(ui32Idx != INVALID_DEVICE_INDEX)
    {
        psDeviceInfo = psCompDevice->psDevices[ui32Idx].psDevInfo;

        if(psDeviceInfo->psCallbacks->pfnDataReceived)
        {
            psDeviceInfo->psCallbacks->pfnDataReceived(
                psCompDevice->psDevices[ui32Idx].pvInstance, ui32Info);
        }
    }
}

//****************************************************************************
//
// This function will check if any device classes need an endpoint handler
// called.
//
//****************************************************************************
static void
HandleEndpoints(void *pvCompositeInstance, uint32_t ui32Status)
{
    uint32_t ui32Idx;
    const tDeviceInfo *psDeviceInfo;
    tUSBDCompositeDevice *psCompDevice;

    ASSERT(pvCompositeInstance != 0);

    //
    // Create the device instance pointer.
    //
    psCompDevice = (tUSBDCompositeDevice *)pvCompositeInstance;

    //
    // Call each of the endpoint handlers.  This may seem odd since we should
    // only call the handler whose endpoint needs service.  Unfortunately, if
    // the device class driver is using uDMA, we have no way of knowing which
    // handler to call (since ui32Status will be 0).  Since the handlers are
    // set up to ignore any callback that is not for them, this is safe.
    //
    for(ui32Idx = 0; ui32Idx < psCompDevice->ui32NumDevices; ui32Idx++)
    {
        psDeviceInfo = psCompDevice->psDevices[ui32Idx].psDevInfo;

        if(psDeviceInfo->psCallbacks->pfnEndpointHandler)
        {
            psDeviceInfo->psCallbacks->pfnEndpointHandler(
                psCompDevice->psDevices[ui32Idx].pvInstance, ui32Status);
        }
    }
}

//*****************************************************************************
//
// Device instance specific handler.
//
//*****************************************************************************
static void
HandleDevice(void *pvCompositeInstance, uint32_t ui32Request,
             void *pvRequestData)
{
    uint32_t ui32Idx;
    tUSBDCompositeDevice *psCompDevice;
    const tDeviceInfo *psDeviceInfo;

    ASSERT(pvCompositeInstance != 0);

    //
    // Create the device instance pointer.
    //
    psCompDevice = (tUSBDCompositeDevice *)pvCompositeInstance;

    for(ui32Idx = 0; ui32Idx < psCompDevice->ui32NumDevices; ui32Idx++)
    {
        psDeviceInfo = psCompDevice->psDevices[ui32Idx].psDevInfo;

        if(psDeviceInfo->psCallbacks->pfnDeviceHandler)
        {
            psDeviceInfo->psCallbacks->pfnDeviceHandler(
                psCompDevice->psDevices[ui32Idx].pvInstance, ui32Request,
                pvRequestData);
        }
    }
}

//****************************************************************************
//
// This function is called by the USB device stack whenever the device is
// disconnected from the host.
//
//****************************************************************************
static void
HandleDisconnect(void *pvCompositeInstance)
{
    uint32_t ui32Idx;
    const tDeviceInfo *psDeviceInfo;
    tUSBDCompositeDevice *psCompDevice;

    ASSERT(pvCompositeInstance != 0);

    //
    // Create the device instance pointer.
    //
    psCompDevice = (tUSBDCompositeDevice *)pvCompositeInstance;

    //
    // Inform the application that the device has been disconnected.
    //
    if(psCompDevice->pfnCallback)
    {
        psCompDevice->pfnCallback(pvCompositeInstance,
                                       USB_EVENT_DISCONNECTED, 0, 0);
    }

    for(ui32Idx = 0; ui32Idx < psCompDevice->ui32NumDevices; ui32Idx++)
    {
        psDeviceInfo = psCompDevice->psDevices[ui32Idx].psDevInfo;

        if(psDeviceInfo->psCallbacks->pfnDisconnectHandler)
        {
            psDeviceInfo->psCallbacks->pfnDisconnectHandler(
                psCompDevice->psDevices[ui32Idx].pvInstance);
        }
    }
}

//****************************************************************************
//
// This function is called by the USB device stack whenever the device
// interface changes.  It will be passed on to the device classes if they have
// a handler for this function.
//
//****************************************************************************
static void
InterfaceChange(void *pvCompositeInstance, uint8_t ui8InterfaceNum,
                uint8_t ui8AlternateSetting)
{
    uint32_t ui32Idx;
    const tDeviceInfo *psDeviceInfo;
    tUSBDCompositeDevice *psCompDevice;

    ASSERT(pvCompositeInstance != 0);

    //
    // Create the device instance pointer.
    //
    psCompDevice = (tUSBDCompositeDevice *)pvCompositeInstance;
    for(ui32Idx = 0; ui32Idx < psCompDevice->ui32NumDevices; ui32Idx++)
    {
        psDeviceInfo = psCompDevice->psDevices[ui32Idx].psDevInfo;

        if(psDeviceInfo->psCallbacks->pfnInterfaceChange)
        {
            psDeviceInfo->psCallbacks->pfnInterfaceChange(
                psCompDevice->psDevices[ui32Idx].pvInstance,
                ui8InterfaceNum, ui8AlternateSetting);
        }
    }
}

//****************************************************************************
//
// This function is called by the USB device stack whenever the device
// configuration changes. It will be passed on to the device classes if they
// have a handler for this function.
//
//****************************************************************************
static void
ConfigChangeHandler(void *pvCompositeInstance, uint32_t ui32Value)
{
    uint32_t ui32Idx;
    const tDeviceInfo *psDeviceInfo;
    tUSBDCompositeDevice *psCompDevice;

    ASSERT(pvCompositeInstance != 0);

    //
    // Create the device instance pointer.
    //
    psCompDevice = (tUSBDCompositeDevice *)pvCompositeInstance;

    for(ui32Idx = 0; ui32Idx < psCompDevice->ui32NumDevices; ui32Idx++)
    {
        psDeviceInfo = psCompDevice->psDevices[ui32Idx].psDevInfo;

        if(psDeviceInfo->psCallbacks->pfnConfigChange)
        {
            psDeviceInfo->psCallbacks->pfnConfigChange(
                psCompDevice->psDevices[ui32Idx].pvInstance, ui32Value);
        }
    }
}

//****************************************************************************
//
// This function is called by the USB device stack whenever a non-standard
// request is received.
//
// \param pvCompositeInstance
// \param psUSBRequest points to the request received.
//
// This call  will be passed on to the device classes if they have a handler
// for this function.
//
// \return None.
//
//****************************************************************************
static void
HandleRequests(void *pvCompositeInstance, tUSBRequest *psUSBRequest)
{
    uint32_t ui32Idx;
    const tDeviceInfo *psDeviceInfo;
    tUSBDCompositeDevice *psCompDevice;

    //
    // Create the device instance pointer.
    //
    psCompDevice = (tUSBDCompositeDevice *)pvCompositeInstance;

    //
    // Determine which device this request is intended for.  We have to be
    // careful here to send this to the callback for the correct device
    // depending upon whether it is a request sent to the device, the interface
    // or the endpoint.
    //
    switch(psUSBRequest->bmRequestType & USB_RTYPE_RECIPIENT_M)
    {
        case USB_RTYPE_INTERFACE:
        {
            ui32Idx = InterfaceToIndex(psCompDevice,
                                       (psUSBRequest->wIndex & 0xFF));
            break;
        }

        case USB_RTYPE_ENDPOINT:
        {
            ui32Idx = EndpointToIndex(psCompDevice,
                             (psUSBRequest->wIndex & 0x0F),
                             (psUSBRequest->wIndex & 0x80) ? true : false);
            break;
        }

        //
        // Requests sent to the device or any other recipient can't be
        // handled here since we have no way of telling where they are
        // supposed to be handled.  As a result, we just stall them.
        //
        // If your composite device has some device-specific requests that need
        // to be handled at the device (rather than interface or endpoint)
        // level, you should add code here to handle them.
        //
        case USB_RTYPE_DEVICE:
        case USB_RTYPE_OTHER:
        default:
        {
            ui32Idx = INVALID_DEVICE_INDEX;
            break;
        }
    }

    //
    // Did we find a device class to pass the request to?
    //
    if(ui32Idx != INVALID_DEVICE_INDEX)
    {
        //
        // Get a pointer to the individual device instance.
        //
        psDeviceInfo = psCompDevice->psDevices[ui32Idx].psDevInfo;

        //
        // Does this device have a RequestHandler callback?
        //
        if(psDeviceInfo->psCallbacks->pfnRequestHandler)
        {
            //
            // Remember this device index so that we can correctly route any
            // data notification callbacks to it.
            //
            psCompDevice->sPrivateData.ui32EP0Owner = ui32Idx;

            //
            // Yes - call the device to retrieve the descriptor.
            //
            psDeviceInfo->psCallbacks->pfnRequestHandler(
                    psCompDevice->psDevices[ui32Idx].pvInstance,
                    psUSBRequest);
        }
        else
        {
            //
            // Oops - we can't satisfy the request so stall EP0 to indicate
            // an error.
            //
            USBDCDStallEP0(USBBaseToIndex(
                              psCompDevice->sPrivateData.ui32USBBase));
        }
    }
    else
    {
        //
        // We are unable to satisfy the descriptor request so stall EP0 to
        // indicate an error.
        //
        USBDCDStallEP0(USBBaseToIndex(
                              psCompDevice->sPrivateData.ui32USBBase));
    }
}

//****************************************************************************
//
// This function handles sending interface number changes to device instances.
//
//****************************************************************************
static void
CompositeIfaceChange(tCompositeEntry *psCompDevice, uint8_t ui8Old,
                     uint8_t ui8New)
{
    uint8_t pui8Interfaces[2];

    if(psCompDevice->psDevInfo->psCallbacks->pfnDeviceHandler)
    {
        //
        // Create the data to pass to the device handler.
        //
        pui8Interfaces[0] = ui8Old;
        pui8Interfaces[1] = ui8New;

        //
        // Call the device handler to inform the class of the interface number
        // change.
        //
        psCompDevice->psDevInfo->psCallbacks->pfnDeviceHandler(
            psCompDevice->pvInstance, USB_EVENT_COMP_IFACE_CHANGE,
            (void *)pui8Interfaces);
    }
}

//****************************************************************************
//
// This function handles sending endpoint number changes to device instances.
//
//****************************************************************************
static void
CompositeEPChange(tCompositeEntry *psCompDevice, uint8_t ui8Old,
                     uint8_t ui8New)
{
    uint8_t pui8Interfaces[2];

    if(psCompDevice->psDevInfo->psCallbacks->pfnDeviceHandler)
    {
        //
        // Create the data to pass to the device handler.
        //
        pui8Interfaces[0] = ui8Old;
        pui8Interfaces[1] = ui8New;

        ui8New--;

        //
        // Call the device handler to inform the class of the interface number
        // change.
        //
        psCompDevice->psDevInfo->psCallbacks->pfnDeviceHandler(
            psCompDevice->pvInstance, USB_EVENT_COMP_EP_CHANGE,
            (void *)pui8Interfaces);
    }
}

//****************************************************************************
//
// This function merges the configuration descriptors into a single multiple
// instance device.
//
//****************************************************************************
uint32_t
BuildCompositeDescriptor(tUSBDCompositeDevice *psCompDevice)
{
    uint32_t ui32Idx, ui32Offset, ui32CPIdx, ui32FixINT, ui32Dev;
    uint16_t ui16TotalLength, ui16Bytes;
    uint8_t ui8Interface, ui8INEndpoint, ui8OUTEndpoint;
    uint8_t *pui8Data, *pui8Config;
    const tConfigHeader *psConfigHeader;
    tDescriptorHeader *psHeader;
    const uint8_t *pui8Descriptor;
    tInterfaceDescriptor *psInterface;
    tEndpointDescriptor *psEndpoint;
    const tDeviceInfo *psDevice;

    //
    // Save the number of devices to look through.
    //
    ui32Dev = 0;
    ui32Idx = 0;
    ui8Interface = 0;
    ui8INEndpoint = 1;
    ui8OUTEndpoint = 1;
    ui32Offset = 0;
    ui32FixINT = 0;

    //
    // This puts the first section pointer in the first entry in the list
    // of sections.
    //
    psCompDevice->sPrivateData.ppsCompSections[0] =
        &psCompDevice->sPrivateData.psCompSections[0];

    //
    // Put the pointer to this instances configuration descriptor into the
    // front of the list.
    //
    psCompDevice->sPrivateData.ppsCompSections[0]->pui8Data =
        (uint8_t *)&psCompDevice->sPrivateData.sConfigDescriptor;

    psCompDevice->sPrivateData.ppsCompSections[0]->ui16Size =
        psCompDevice->sPrivateData.sConfigDescriptor.bLength;

    //
    // The configuration descriptor is 9 bytes so initialize the total length
    // to 9 bytes.
    //
    ui16TotalLength = 9;

    //
    // Copy the section pointer into the section array for the composite
    // device.  This is awkward but is required given the definition
    // of the structures.
    //
    psCompDevice->sPrivateData.ppsCompSections[1] =
        &psCompDevice->sPrivateData.psCompSections[1];

    //
    // Copy the pointer to the application supplied space into the section
    // list.
    //
    psCompDevice->sPrivateData.ppsCompSections[1]->ui16Size = 0;
    psCompDevice->sPrivateData.ppsCompSections[1]->pui8Data =
        psCompDevice->sPrivateData.pui8Data;

    //
    // Create a local pointer to the data that is used to copy data from
    // the other devices into the composite descriptor.
    //
    pui8Data = psCompDevice->sPrivateData.pui8Data;

    //
    // Consider each device in turn.
    //
    while(ui32Dev < psCompDevice->ui32NumDevices)
    {
        //
        // Save the current starting address of this descriptor.
        //
        pui8Config = pui8Data + ui32Offset;

        //
        // Create a local pointer to the configuration header.
        //
        psDevice = psCompDevice->psDevices[ui32Dev].psDevInfo;
        psConfigHeader = psDevice->ppsConfigDescriptors[0];

        //
        // Loop through each of the sections in this device's configuration
        // descriptor.
        //
        for(ui32Idx = 0; ui32Idx < psConfigHeader->ui8NumSections; ui32Idx++)
        {
            //
            // Initialize the local offset in this descriptor.  We include
            // a special case here to ignore the initial 9 byte configuration
            // descriptor since this has already been handled.
            //
            if(ui32Idx)
            {
                //
                // This is not the first section so we handle everything in
                // it.
                //
                ui16Bytes = 0;
            }
            else
            {
                //
                // This is the first section for this device so skip the 9
                // byte configuration descriptor since we've already handled
                // this.
                //
                ui16Bytes = 9;

                //
                // If this section includes only the configuration descriptor,
                // skip it entirely.
                //
                if(psConfigHeader->psSections[ui32Idx]->ui16Size <= ui16Bytes)
                {
                    continue;
                }
            }

            //
            // Get a pointer to the configuration descriptor.
            //
            pui8Descriptor = psConfigHeader->psSections[ui32Idx]->pui8Data;

            //
            // Bounds check the allocated space and return if there is not
            // enough space.
            //
            if(ui32Offset > psCompDevice->sPrivateData.ui32DataSize)
            {
                return(1);
            }

            //
            // Copy the descriptor from the device into the descriptor list.
            //
            for(ui32CPIdx = 0;
                ui32CPIdx < psConfigHeader->psSections[ui32Idx]->ui16Size;
                ui32CPIdx++)
            {
                pui8Data[ui32CPIdx + ui32Offset] = pui8Descriptor[ui32CPIdx];
            }

            //
            // Read out the descriptors in this section.
            //
            while(ui16Bytes < psConfigHeader->psSections[ui32Idx]->ui16Size)
            {
                //
                // Create a descriptor header pointer.
                //
                psHeader = (tDescriptorHeader *)&pui8Data[ui32Offset +
                                                          ui16Bytes];

                //
                // Check for interface descriptors and modify the numbering to
                // match the composite device.
                //
                if(psHeader->bDescriptorType == USB_DTYPE_INTERFACE)
                {
                    psInterface = (tInterfaceDescriptor *)psHeader;

                    //
                    // See if this is an alternate setting or the initial
                    // setting.
                    //
                    if(psInterface->bAlternateSetting != 0)
                    {
                        //
                        // If this is an alternate setting then use the
                        // previous interface number because the current one
                        // has already been incremented.
                        //
                        psInterface->bInterfaceNumber = ui8Interface - 1;
                    }
                    else
                    {
                        //
                        // Notify the class that it's interface number has
                        // changed.
                        //
                        CompositeIfaceChange(
                                         &psCompDevice->psDevices[ui32Dev],
                                         psInterface->bInterfaceNumber,
                                         ui8Interface);
                        //
                        // This was the non-alternate setting so save the
                        // value and move to the next interface number.
                        //
                        psInterface->bInterfaceNumber = ui8Interface;

                        //
                        // No strings allowed on interface descriptors for
                        // composite devices.
                        //
                        psInterface->iInterface = 0;

                        ui8Interface++;
                    }
                }
                //
                // Check for endpoint descriptors and modify the numbering to
                // match the composite device.
                //
                else if(psHeader->bDescriptorType == USB_DTYPE_ENDPOINT)
                {
                    psEndpoint = (tEndpointDescriptor *)psHeader;

                    //
                    // Check if this is an IN or OUT endpoint.
                    //
                    if(psEndpoint->bEndpointAddress & USB_RTYPE_DIR_IN)
                    {
                        //
                        // Check if this is the special Fixed Interrupt class
                        // and this is the interrupt endpoint.
                        //
                        if(((psEndpoint->bmAttributes & USB_EP_ATTR_TYPE_M) ==
                            USB_EP_ATTR_INT) &&
                           (psCompDevice->ui16PID == USB_PID_COMP_SERIAL))
                        {
                            //
                            // Check if the Fixed Interrupt endpoint has been
                            // set yet.
                            //
                            if(ui32FixINT == 0)
                            {
                                //
                                // Allocate the fixed interrupt endpoint and
                                // save its number.
                                //
                                ui32FixINT = ui8INEndpoint++;
                            }

                            CompositeEPChange(
                                        &psCompDevice->psDevices[ui32Dev],
                                        psEndpoint->bEndpointAddress,
                                        ui32FixINT);

                            psEndpoint->bEndpointAddress = ui32FixINT |
                                                           USB_RTYPE_DIR_IN;
                        }
                        else
                        {
                            //
                            // Notify the class that it's interface number has
                            // changed.
                            //
                            CompositeEPChange(
                                        &psCompDevice->psDevices[ui32Dev],
                                        psEndpoint->bEndpointAddress,
                                        ui8INEndpoint);

                            psEndpoint->bEndpointAddress = ui8INEndpoint++ |
                                                           USB_RTYPE_DIR_IN;
                        }
                    }
                    else
                    {
                        //
                        // Notify the class that it's interface number has
                        // changed.
                        //
                        CompositeEPChange(&psCompDevice->psDevices[ui32Dev],
                                          psEndpoint->bEndpointAddress,
                                          ui8OUTEndpoint);
                        psEndpoint->bEndpointAddress = ui8OUTEndpoint++;
                    }
                }

                //
                // Move on to the next descriptor.
                //
                ui16Bytes += psHeader->bLength;
            }

            ui32Offset += psConfigHeader->psSections[ui32Idx]->ui16Size;

            ui16TotalLength += ui16Bytes;
        }

        //
        // Allow the device class to make adjustments to the configuration
        // descriptor.
        //
        psCompDevice->psDevices[ui32Dev].psDevInfo->psCallbacks->pfnDeviceHandler(
                psCompDevice->psDevices[ui32Dev].pvInstance,
                USB_EVENT_COMP_CONFIG, (void *)pui8Config);

        //
        // Add an entry into the device workspace array to allow us to quickly
        // map interface and endpoint numbers to device instances later.
        //
        psCompDevice->psDevices[ui32Dev].ui32DeviceWorkspace =
            (ui32Dev << (LOOKUP_INDEX_BYTE * 8)) |
            (ui8Interface << (LOOKUP_INTERFACE_BYTE * 8)) |
            (ui8OUTEndpoint << (LOOKUP_OUT_END_BYTE * 8)) |
            (ui8INEndpoint << (LOOKUP_IN_END_BYTE * 8));

        //
        // Move on to the next device.
        //
        ui32Dev++;
    }

    //
    // Modify the configuration descriptor to match the number of interfaces
    // and the new total size.
    //
    psCompDevice->sPrivateData.sCompConfigHeader.ui8NumSections = 2;
    psCompDevice->sPrivateData.ppsCompSections[1]->ui16Size = ui32Offset;
    psCompDevice->sPrivateData.sConfigDescriptor.bNumInterfaces =
       ui8Interface;
    psCompDevice->sPrivateData.sConfigDescriptor.wTotalLength =
       ui16TotalLength;


    return(0);
}

//****************************************************************************
//
//! This function should be called once for the composite class device to
//! initialize basic operation and prepare for enumeration.
//!
//! \param ui32Index is the index of the USB controller to initialize for
//! composite device operation.
//! \param psDevice points to a structure containing parameters customizing
//! the operation of the composite device.
//! \param ui32Size is the size in bytes of the data pointed to by the
//! \e pui8Data parameter.
//! \param pui8Data is the data area that the composite class can use to build
//! up descriptors.
//!
//! In order for an application to initialize the USB composite device class,
//! it must first call this function with the a valid composite device class
//! structure in the \e psDevice parameter.  This allows this function to
//! initialize the USB controller and device code to be prepared to enumerate
//! and function as a USB composite device.  The \e ui32Size and \e pui8Data
//! parameters should be large enough to hold all of the class instances
//! passed in via the \e psDevice structure.  This is typically the full size
//! of the configuration descriptor for a device minus its configuration
//! header(9 bytes).
//!
//! This function returns a void pointer that must be passed in to all other
//! APIs used by the composite class.
//!
//! See the documentation on the tUSBDCompositeDevice structure for more
//! information on how to properly fill the structure members.
//!
//! \return This function returns 0 on failure or a non-zero void pointer on
//! success.
//
//****************************************************************************
void *
USBDCompositeInit(uint32_t ui32Index, tUSBDCompositeDevice *psDevice,
                  uint32_t ui32Size, uint8_t *pui8Data)
{
    tCompositeInstance *psInst;
    int32_t i32Idx;
    uint8_t *pui8Temp;

    //
    // Check parameter validity.
    //
    ASSERT(ui32Index == 0);
    ASSERT(psDevice);
    ASSERT(psDevice->ppui8StringDescriptors);

    //
    // Initialize the work space in the passed instance structure.
    //
    psInst = &psDevice->sPrivateData;
    psInst->ui32DataSize = ui32Size;
    psInst->pui8Data = pui8Data;

    //
    // Save the base address of the USB controller.
    //
    psInst->ui32USBBase = USBIndexToBase(ui32Index);

    //
    // No device is currently transferring data on EP0.
    //
    psInst->ui32EP0Owner = INVALID_DEVICE_INDEX;

    //
    // Initialize the device information structure.
    //
    psInst->sDevInfo.psCallbacks = &g_sCompHandlers;
    psInst->sDevInfo.pui8DeviceDescriptor = g_pui8CompDeviceDescriptor;
    psInst->sDevInfo.ppsConfigDescriptors =
                    (const tConfigHeader * const *)g_ppCompConfigDescriptors;
    psInst->sDevInfo.ppui8StringDescriptors = 0;
    psInst->sDevInfo.ui32NumStringDescriptors = 0;

    //
    // Initialize the device info structure for the composite device.
    //
    USBDCDDeviceInfoInit(0, &psInst->sDevInfo);

    g_ppCompConfigDescriptors[0] = &psInst->sCompConfigHeader;
    g_ppCompConfigDescriptors[0]->ui8NumSections = 0;
    g_ppCompConfigDescriptors[0]->psSections =
      (const tConfigSection * const *)psDevice->sPrivateData.ppsCompSections;

    //
    // Create a byte pointer to use with the copy.
    //
    pui8Temp = (uint8_t *)&psInst->sConfigDescriptor;

    //
    // Copy the default configuration descriptor into the instance data.
    //
    for(i32Idx = 0; i32Idx < g_pui8CompConfigDescriptor[0]; i32Idx++)
    {
        pui8Temp[i32Idx] = g_pui8CompConfigDescriptor[i32Idx];
    }

    //
    // Create a byte pointer to use with the copy.
    //
    pui8Temp = (uint8_t *)&psInst->sDeviceDescriptor;

    //
    // Copy the default configuration descriptor into the instance data.
    //
    for(i32Idx = 0; i32Idx < g_pui8CompDeviceDescriptor[0]; i32Idx++)
    {
        pui8Temp[i32Idx] = g_pui8CompDeviceDescriptor[i32Idx];
    }

    //
    // Fix up the device descriptor with the client-supplied values.
    //
    psInst->sDeviceDescriptor.idVendor = psDevice->ui16VID;
    psInst->sDeviceDescriptor.idProduct = psDevice->ui16PID;

    //
    // Fix up the configuration descriptor with client-supplied values.
    //
    psInst->sConfigDescriptor.bmAttributes = psDevice->ui8PwrAttributes;
    psInst->sConfigDescriptor.bMaxPower =
        (uint8_t)(psDevice->ui16MaxPowermA>>1);

    psInst->sDevInfo.pui8DeviceDescriptor =
                                (const uint8_t *)&psInst->sDeviceDescriptor;

    //
    // Plug in the client's string table to the device information
    // structure.
    //
    psInst->sDevInfo.ppui8StringDescriptors =
                                            psDevice->ppui8StringDescriptors;
    psInst->sDevInfo.ui32NumStringDescriptors =
                                            psDevice->ui32NumStringDescriptors;

    //
    // Enable Clocking to the USB controller so that changes to the USB
    // controller can be made in the BuildCompositeDescriptor() function.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);

    //
    // Create the combined descriptors.
    //
    if(BuildCompositeDescriptor(psDevice))
    {
        return(0);
    }

    //
    // All is well so now pass the descriptors to the lower layer and put
    // the bulk device on the bus.
    //
    USBDCDInit(ui32Index, &psInst->sDevInfo, (void *)psDevice);

    //
    // Return the pointer to the instance indicating that everything went
    // well.
    //
    return((void *)psDevice);
}

//****************************************************************************
//
//! Shuts down the composite device.
//!
//! \param pvCompositeInstance is the pointer to the device instance structure
//! as returned by USBDCompositeInit().
//!
//! This function terminates composite device interface for the instance
//! not me supplied. Following this call, the \e pvCompositeInstance instance
//! should not be used in any other calls.
//!
//! \return None.
//
//****************************************************************************
void
USBDCompositeTerm(void *pvCompositeInstance)
{
    ASSERT(pvCompositeInstance != 0);

}

//****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//****************************************************************************

