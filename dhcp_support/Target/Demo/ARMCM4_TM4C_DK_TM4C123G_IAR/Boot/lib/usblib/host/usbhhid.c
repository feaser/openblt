//*****************************************************************************
//
// usbhhid.c - This file contains the host HID driver.
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
// This is part of revision 1.1 of the Tiva USB Library.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_types.h"
#include "driverlib/usbdrv.h"
#include "usblib/usblib.h"
#include "usblib/usblibpriv.h"
#include "usblib/usbhid.h"
#include "usblib/host/usbhost.h"
#include "usblib/host/usbhostpriv.h"
#include "usblib/host/usbhhid.h"

static void * HIDDriverOpen(tUSBHostDevice *psDevice);
static void HIDDriverClose(void *pvInstance);

//*****************************************************************************
//
//! \addtogroup usblib_host_class
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// If the user has not explicitly stated the maximum number of HID devices to
// support, we assume that we need to support up to the maximum number of USB
// devices that the build is configured for.
//
//*****************************************************************************
#ifndef MAX_HID_DEVICES
#define MAX_HID_DEVICES MAX_USB_DEVICES
#endif

//*****************************************************************************
//
// This is the structure that holds all of the data for a given instance of
// a HID device.
//
//*****************************************************************************
struct tHIDInstance
{
    //
    // Save the device instance.
    //
    tUSBHostDevice *psDevice;

    //
    // Used to save the callback.
    //
    tUSBCallback pfnCallback;

    //
    // Callback data provided by caller.
    //
    void *pvCBData;

    //
    // Used to remember what type of device was registered.
    //
    tHIDSubClassProtocol iDeviceType;

    //
    // Interrupt IN pipe.
    //
    uint32_t ui32IntInPipe;
};

//*****************************************************************************
//
// The instance data storage for attached hid devices.
//
//*****************************************************************************
static tHIDInstance g_psHIDDevice[MAX_HID_DEVICES];

//*****************************************************************************
//
//! This constant global structure defines the HID Class Driver that is
//! provided with the USB library.
//
//*****************************************************************************
const tUSBHostClassDriver g_sUSBHIDClassDriver =
{
    USB_CLASS_HID,
    HIDDriverOpen,
    HIDDriverClose,
    0
};

//*****************************************************************************
//
//! This function is used to open an instance of a HID device.
//!
//! \param iDeviceType is the type of device that should be loaded for this
//! instance of the HID device.
//! \param pfnCallback is the function that will be called whenever changes
//! are detected for this device.
//! \param pvCBData is the data that will be returned in when the
//! \e pfnCallback function is called.
//!
//! This function creates an instance of an specific type of HID device.  The
//! \e iDeviceType parameter is one subclass/protocol values of the types
//! specified in enumerated types tHIDSubClassProtocol.  Only devices that
//! enumerate with this type will be called back via the \e pfnCallback
//! function.  The \e pfnCallback parameter is the callback function for any
//! events that occur for this device type.  The \e pfnCallback function must
//! point to a valid function of type \e tUSBCallback for this call to complete
//! successfully.  To release this device instance the caller of USBHHIDOpen()
//! should call USBHHIDClose() and pass in the value returned from the
//! USBHHIDOpen() call.
//!
//! \return This function returns and instance value that should be used with
//! any other APIs that require an instance value.  If a value of 0 is returned
//! then the device instance could not be created.
//
//*****************************************************************************
tHIDInstance *
USBHHIDOpen(tHIDSubClassProtocol iDeviceType, tUSBCallback pfnCallback,
            void *pvCBData)
{
    uint32_t ui32Loop;

    //
    // Find a free device instance structure.
    //
    for(ui32Loop = 0; ui32Loop < MAX_HID_DEVICES; ui32Loop++)
    {
        if(g_psHIDDevice[ui32Loop].iDeviceType == eUSBHHIDClassNone)
        {
            //
            // Save the instance data for this device.
            //
            g_psHIDDevice[ui32Loop].pfnCallback = pfnCallback;
            g_psHIDDevice[ui32Loop].iDeviceType = iDeviceType;
            g_psHIDDevice[ui32Loop].pvCBData = pvCBData;

            //
            // Return the device instance pointer.
            //
            return(&g_psHIDDevice[ui32Loop]);
        }
    }

    //
    // If we get here, there are no space device slots so return NULL to
    // indicate a problem.
    //
    return(0);
}

//*****************************************************************************
//
//! This function is used to release an instance of a HID device.
//!
//! \param psHIDInstance is the instance value for a HID device to release.
//!
//! This function releases an instance of a HID device that was created by a
//! call to USBHHIDOpen().  This call is required to allow other HID devices
//! to be enumerated after another HID device has been disconnected.  The
//! \e psHIDInstance parameter should hold the value that was returned from
//! the previous call to USBHHIDOpen().
//!
//! \return None.
//
//*****************************************************************************
void
USBHHIDClose(tHIDInstance *psHIDInstance)
{
    //
    // Disable any more notifications from the HID layer.
    //
    psHIDInstance->pfnCallback = 0;

    //
    // Mark this device slot as free.
    //
    psHIDInstance->iDeviceType = eUSBHHIDClassNone;
}

//*****************************************************************************
//
// This function handles callbacks for the interrupt IN endpoint.
//
//*****************************************************************************
static void
HIDIntINCallback(uint32_t ui32Pipe, uint32_t ui32Event)
{
    int32_t i32Dev;

    switch (ui32Event)
    {
        //
        // Handles a request to schedule a new request on the interrupt IN
        // pipe.
        //
        case USB_EVENT_SCHEDULER:
        {
            USBHCDPipeSchedule(ui32Pipe, 0, 1);
            break;
        }
        //
        // Called when new data is available on the interrupt IN pipe.
        //
        case USB_EVENT_RX_AVAILABLE:
        {
            //
            // Determine which device this notification is intended for.
            //
            for(i32Dev = 0; i32Dev < MAX_HID_DEVICES; i32Dev++)
            {
                //
                // Does this device own the pipe we have been passed?
                //
                if(g_psHIDDevice[i32Dev].ui32IntInPipe == ui32Pipe)
                {
                    //
                    // Yes - send the report data to the USB host HID device
                    // class driver.
                    //
                    g_psHIDDevice[i32Dev].pfnCallback(
                                    g_psHIDDevice[i32Dev].pvCBData,
                                    USB_EVENT_RX_AVAILABLE, ui32Pipe, 0);
                }
            }

            break;
        }
    }
}

//*****************************************************************************
//
//! This function is used to open an instance of the HID driver.
//!
//! \param psDevice is a pointer to the device information structure.
//!
//! This function will attempt to open an instance of the HID driver based on
//! the information contained in the psDevice structure.  This call can fail if
//! there are not sufficient resources to open the device.  The function will
//! return a value that should be passed back into USBHIDClose() when the
//! driver is no longer needed.
//!
//! \return The function will return a pointer to a HID driver instance.
//
//*****************************************************************************
static void *
HIDDriverOpen(tUSBHostDevice *psDevice)
{
    int32_t i32Idx, i32Dev;
    tEndpointDescriptor *psEndpointDescriptor;
    tInterfaceDescriptor *psInterface;

    //
    // Get the interface descriptor.
    //
    psInterface = USBDescGetInterface(psDevice->psConfigDescriptor, 0, 0);

    //
    // Search the currently open instances for one that supports the protocol
    // of this device.
    //
    for(i32Dev = 0; i32Dev < MAX_HID_DEVICES; i32Dev++)
    {
        if(g_psHIDDevice[i32Dev].iDeviceType ==
           psInterface->bInterfaceProtocol)
        {
            //
            // Save the device pointer.
            //
            g_psHIDDevice[i32Dev].psDevice = psDevice;

            for(i32Idx = 0; i32Idx < 3; i32Idx++)
            {
                //
                // Get the first endpoint descriptor.
                //
                psEndpointDescriptor = USBDescGetInterfaceEndpoint(psInterface,
                                                                   i32Idx,
                                                                   256);

                //
                // If no more endpoints then break out.
                //
                if(psEndpointDescriptor == 0)
                {
                    break;
                }

                //
                // Interrupt
                //
                if((psEndpointDescriptor->bmAttributes & USB_EP_ATTR_TYPE_M) ==
                   USB_EP_ATTR_INT)
                {
                    //
                    // Interrupt IN.
                    //
                    if(psEndpointDescriptor->bEndpointAddress & USB_EP_DESC_IN)
                    {
                        g_psHIDDevice[i32Dev].ui32IntInPipe =
                                USBHCDPipeAlloc(0, USBHCD_PIPE_INTR_IN,
                                                psDevice, HIDIntINCallback);
                        USBHCDPipeConfig(g_psHIDDevice[i32Dev].ui32IntInPipe,
                                    psEndpointDescriptor->wMaxPacketSize,
                                    psEndpointDescriptor->bInterval,
                                    (psEndpointDescriptor->bEndpointAddress &
                                     USB_EP_DESC_NUM_M));
                    }
                }
            }

            //
            // If there is a callback function call it to inform the application that
            // the device has been enumerated.
            //
            if(g_psHIDDevice[i32Dev].pfnCallback != 0)
            {
                g_psHIDDevice[i32Dev].pfnCallback(
                                    g_psHIDDevice[i32Dev].pvCBData,
                                    USB_EVENT_CONNECTED,
                                    (uint32_t)&g_psHIDDevice[i32Dev], 0);
            }

            //
            // Save the device pointer.
            //
            g_psHIDDevice[i32Dev].psDevice = psDevice;

            return (&g_psHIDDevice[i32Dev]);
        }
    }

    //
    // If we get here, no user has registered an interest in this particular
    // HID device so we return an error.
    //
    return(0);
}

//*****************************************************************************
//
//! This function is used to release an instance of the HID driver.
//!
//! \param pvInstance is an instance pointer that needs to be released.
//!
//! This function will free up any resources in use by the HID driver instance
//! that is passed in.  The \e pvInstance pointer should be a valid value that
//! was returned from a call to USBHIDOpen().
//!
//! \return None.
//
//*****************************************************************************
static void
HIDDriverClose(void *pvInstance)
{
    tHIDInstance *psInst;

    //
    // Get our instance pointer.
    //
    psInst = (tHIDInstance *)pvInstance;

    //
    // Reset the device pointer.
    //
    psInst->psDevice = 0;

    //
    // Free the Interrupt IN pipe.
    //
    if(psInst->ui32IntInPipe != 0)
    {
        USBHCDPipeFree(psInst->ui32IntInPipe);
    }

    //
    // If the callback exists, call it with a DISCONNECTED event.
    //
    if(psInst->pfnCallback != 0)
    {
        psInst->pfnCallback(psInst->pvCBData, USB_EVENT_DISCONNECTED,
                            (uint32_t)pvInstance, 0);
    }
}

//*****************************************************************************
//
//! This function is used to set the idle timeout for a HID device.
//!
//! \param psHIDInstance is the value that was returned from the call to
//! USBHHIDOpen().
//! \param ui8Duration is the duration of the timeout in milliseconds.
//! \param ui8ReportID is the report identifier to set the timeout on.
//!
//! This function will send the Set Idle command to a HID device to set the
//! idle timeout for a given report.  The length of the timeout is specified
//! by the \e ui8Duration parameter and the report the timeout for is in the
//! \e ui8ReportID value.
//!
//! \return Always returns 0.
//
//*****************************************************************************
uint32_t
USBHHIDSetIdle(tHIDInstance *psHIDInstance, uint8_t ui8Duration,
               uint8_t ui8ReportID)
{
    tUSBRequest sSetupPacket;

    //
    // This is a Class specific interface OUT request.
    //
    sSetupPacket.bmRequestType = USB_RTYPE_DIR_OUT | USB_RTYPE_CLASS |
                                 USB_RTYPE_INTERFACE;

    //
    // Request a Device Descriptor.
    //
    sSetupPacket.bRequest = USBREQ_SET_IDLE;
    sSetupPacket.wValue = (ui8Duration << 8) | ui8ReportID;

    //
    // Set this on interface 1.
    //
    sSetupPacket.wIndex = 0;

    //
    // This is always 0 for this request.
    //
    sSetupPacket.wLength = 0;

    //
    // Put the setup packet in the buffer.
    //
    return(USBHCDControlTransfer(0, &sSetupPacket, psHIDInstance->psDevice,
                                 0, 0, MAX_PACKET_SIZE_EP0));
}

//*****************************************************************************
//
//! This function can be used to retrieve the report descriptor for a given
//! device instance.
//!
//! \param psHIDInstance is the value that was returned from the call to
//! USBHHIDOpen().
//! \param pui8Buffer is the memory buffer to use to store the report
//! descriptor.
//! \param ui32Size is the size in bytes of the buffer pointed to by
//! \e pui8Buffer.
//!
//! This function is used to return a report descriptor from a HID device
//! instance so that it can determine how to interpret reports that are
//! returned from the device indicated by the \e psHIDInstance parameter.
//! This call is blocking and will return the number of bytes read into the
//! \e pui8Buffer.
//!
//! \return Returns the number of bytes read into the \e pui8Buffer.
//
//*****************************************************************************
uint32_t
USBHHIDGetReportDescriptor(tHIDInstance *psHIDInstance, uint8_t *pui8Buffer,
                           uint32_t ui32Size)
{
    tUSBRequest sSetupPacket;
    uint32_t ui32Bytes;

    //
    // This is a Standard Device IN request.
    //
    sSetupPacket.bmRequestType = USB_RTYPE_DIR_IN | USB_RTYPE_STANDARD |
                                 USB_RTYPE_INTERFACE;

    //
    // Request a Report Descriptor.
    //
    sSetupPacket.bRequest = USBREQ_GET_DESCRIPTOR;
    sSetupPacket.wValue = USB_HID_DTYPE_REPORT << 8;

    //
    // Index is always 0 for device requests.
    //
    sSetupPacket.wIndex = 0;

    //
    // All devices must have at least an 8 byte max packet size so just ask
    // for 8 bytes to start with.
    //
    sSetupPacket.wLength = ui32Size;

    //
    // Now get the full descriptor now that the actual maximum packet size
    // is known.
    //
    ui32Bytes = USBHCDControlTransfer(0, &sSetupPacket,
                psHIDInstance->psDevice, pui8Buffer, ui32Size,
                psHIDInstance->psDevice->sDeviceDescriptor.bMaxPacketSize0);

    return(ui32Bytes);
}

//*****************************************************************************
//
//! This function is used to set or clear the boot protocol state of a device.
//!
//! \param psHIDInstance is the value that was returned from the call to
//! USBHHIDOpen().
//! \param ui32BootProtocol is either zero or non-zero to indicate which
//! protocol to use for the device.
//!
//! A USB host device can use this function to set the protocol for a connected
//! HID device.  This is commonly used to set keyboards and mice into their
//! simplified boot protocol modes to fix the report structure to a know
//! state.
//!
//! \return This function returns 0.
//
//*****************************************************************************
uint32_t
USBHHIDSetProtocol(tHIDInstance *psHIDInstance, uint32_t ui32BootProtocol)
{
    tUSBRequest sSetupPacket;

    //
    // This is a Standard Device IN request.
    //
    sSetupPacket.bmRequestType = USB_RTYPE_DIR_OUT | USB_RTYPE_CLASS |
                                 USB_RTYPE_INTERFACE;

    //
    // Request a Report Descriptor.
    //
    sSetupPacket.bRequest = USBREQ_SET_PROTOCOL;

    if(ui32BootProtocol)
    {
        //
        // Boot Protocol.
        //
        sSetupPacket.wValue = 0;
    }
    else
    {
        //
        // Report Protocol.
        //
        sSetupPacket.wValue = 1;
    }

    //
    // Index is always 0 for device requests.
    //
    sSetupPacket.wIndex = 0;

    //
    // Always 0.
    //
    sSetupPacket.wLength = 0;

    //
    // Now get the full descriptor now that the actual maximum packet size
    // is known.
    //
    USBHCDControlTransfer(0, &sSetupPacket, psHIDInstance->psDevice, 0, 0,
                psHIDInstance->psDevice->sDeviceDescriptor.bMaxPacketSize0);

    return(0);
}

//*****************************************************************************
//
//! This function is used to retrieve a report from a HID device.
//!
//! \param psHIDInstance is the value that was returned from the call to
//! USBHHIDOpen().
//! \param ui32Interface is the interface to retrieve the report from.
//! \param pui8Data is the memory buffer to use to store the report.
//! \param ui32Size is the size in bytes of the buffer pointed to by
//! \e pui8Buffer.
//!
//! This function is used to retrieve a report from a USB pipe.  It is usually
//! called when the USB HID layer has detected a new data available in a USB
//! pipe.  The USB HID host device code will receive a
//! \b USB_EVENT_RX_AVAILABLE event when data is available, allowing the
//! callback function to retrieve the data.
//!
//! \return Returns the number of bytes read from report.
//
//*****************************************************************************
uint32_t
USBHHIDGetReport(tHIDInstance *psHIDInstance, uint32_t ui32Interface,
                 uint8_t *pui8Data, uint32_t ui32Size)
{
    //
    // Read the Data out.
    //
    ui32Size = USBHCDPipeReadNonBlocking(psHIDInstance->ui32IntInPipe,
                                         pui8Data, ui32Size);

    //
    // Return the number of bytes read from the interrupt in pipe.
    //
    return(ui32Size);
}

//*****************************************************************************
//
//! This function is used to send a report to a HID device.
//!
//! \param psHIDInstance is the value that was returned from the call to
//! USBHHIDOpen().
//! \param ui32Interface is the interface to send the report to.
//! \param pui8Data is the memory buffer to use to store the report.
//! \param ui32Size is the size in bytes of the buffer pointed to by
//! \e pui8Buffer.
//!
//! This function is used to send a report to a USB HID device.  It can be
//! only be called from outside the callback context as this function will not
//! return from the call until the data has been sent successfully.
//!
//! \return Returns the number of bytes sent to the device.
//
//*****************************************************************************
uint32_t
USBHHIDSetReport(tHIDInstance *psHIDInstance, uint32_t ui32Interface,
                 uint8_t *pui8Data, uint32_t ui32Size)
{
    tUSBRequest sSetupPacket;

    //
    // This is a class specific OUT request.
    //
    sSetupPacket.bmRequestType = USB_RTYPE_DIR_OUT | USB_RTYPE_CLASS |
                                 USB_RTYPE_INTERFACE;

    //
    // Request a Report Descriptor.
    //
    sSetupPacket.bRequest = USBREQ_SET_REPORT;
    sSetupPacket.wValue = USB_HID_REPORT_OUTPUT << 8;

    //
    // Index is always 0 for device requests.
    //
    sSetupPacket.wIndex = (uint16_t)ui32Interface;

    //
    // Always 0.
    //
    sSetupPacket.wLength = ui32Size;

    //
    // Now get the full descriptor now that the actual maximum packet size
    // is known.
    //
    USBHCDControlTransfer(0, &sSetupPacket, psHIDInstance->psDevice,
            pui8Data, ui32Size,
            psHIDInstance->psDevice->sDeviceDescriptor.bMaxPacketSize0);

    return(ui32Size);
}

//*****************************************************************************
//
//! @}
//
//*****************************************************************************
