//*****************************************************************************
//
// usbddfu-rt.c - USB Device Firmware Update runtime device class driver.
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
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_nvic.h"
#include "driverlib/debug.h"
#include "driverlib/usbdrv.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "usblib/usblib.h"
#include "usblib/usblibpriv.h"
#include "usblib/usbdfu.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbddfu-rt.h"
#include "usblib/usblibpriv.h"

//*****************************************************************************
//
//! \addtogroup dfu_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// DFU Device Descriptor.  This is a dummy structure since runtime DFU must be
// a part of a composite device and cannot be instantiated on its own.
//
//*****************************************************************************
const uint8_t g_pui8DFUDeviceDescriptor[] =
{
    18,                             // Size of this structure.
    USB_DTYPE_DEVICE,               // Type of this structure.
    USBShort(0x110),                // USB version 1.1 (if we say 2.0, hosts
                                    // assume
                                    // high-speed - see USB 2.0 spec 9.2.6.6)
    USB_CLASS_VEND_SPECIFIC,        // USB Device Class
    0,                              // USB Device Sub-class
    0,                              // USB Device protocol
    64,                             // Maximum packet size for default pipe.
    USBShort(0),                    // Vendor ID (VID).
    USBShort(0),                    // Product ID (PID).
    USBShort(0),                    // Device Release Number BCD.
    0,                              // Manufacturer string identifier.
    0,                              // Product string identifier.
    0,                              // Product serial number.
    1                               // Number of configurations.
};

//*****************************************************************************
//
// DFU device runtime configuration descriptor.  This is also a dummy structure
// since the primary device class configuration will be used when DFU is added
// to the composite device.
//
//*****************************************************************************
uint8_t g_pui8DFUConfigDescriptor[] =
{
    //
    // Configuration descriptor header.
    //
    9,                              // Size of the configuration descriptor.
    USB_DTYPE_CONFIGURATION,        // Type of this descriptor.
    USBShort(27),                   // The total size of this full structure.
    1,                              // The number of interfaces in this
                                    // configuration.
    1,                              // The unique value for this configuration.
    0,                              // The string identifier that describes
                                    // this configuration.
    USB_CONF_ATTR_SELF_PWR,         // Bus Powered, Self Powered, remote wake
                                    // up.
    250,                            // The maximum power in 2mA increments.
};

//*****************************************************************************
//
// The DFU runtime interface descriptor.
//
//*****************************************************************************
uint8_t g_pui8DFUInterface[DFUINTERFACE_SIZE] =
{
    //
    // Interface descriptor for runtime DFU operation.
    //
    9,                              // Length of this descriptor.
    USB_DTYPE_INTERFACE,            // This is an interface descriptor.
    0,                              // Interface number .
    0,                              // Alternate setting number.
    0,                              // Number of endpoints (only endpoint 0
                                    // used)
    USB_CLASS_APP_SPECIFIC,         // Application specific interface class
    USB_DFU_SUBCLASS,               // Device Firmware Upgrade subclass
    USB_DFU_RUNTIME_PROTOCOL,       // DFU runtime protocol
    0,                              // No string descriptor for this interface.
};

//*****************************************************************************
//
// The DFU functional descriptor.
//
//*****************************************************************************
uint8_t g_pui8DFUFunctionalDesc[DFUFUNCTIONALDESC_SIZE] =
{
    //
    // Device Firmware Upgrade functional descriptor.
    //
    9,                              // Length of this descriptor.
    USB_DFU_FUNC_DESCRIPTOR_TYPE,   // DFU Functional descriptor type
    (DFU_ATTR_CAN_DOWNLOAD |        // DFU attributes.
     DFU_ATTR_CAN_UPLOAD |
     DFU_ATTR_WILL_DETACH |
     DFU_ATTR_MANIFEST_TOLERANT),
    USBShort(0xFFFF),               // Detach timeout (set to maximum).
    USBShort(DFU_TRANSFER_SIZE),    // Transfer size 1KB.
    USBShort(0x0110)                // DFU Version 1.1
};

//*****************************************************************************
//
// The DFU runtime configuration descriptor is defined as two sections.
// These sections are:
//
// 1.  The 9 byte configuration descriptor.
// 2.  The interface descriptor + DFU functional descriptor.
//
//*****************************************************************************
const tConfigSection g_sDFUConfigSection =
{
    sizeof(g_pui8DFUConfigDescriptor),
    g_pui8DFUConfigDescriptor
};

const tConfigSection g_sDFUInterfaceSection =
{
    sizeof(g_pui8DFUInterface),
    g_pui8DFUInterface
};

const tConfigSection g_sDFUFunctionalDescSection =
{
    sizeof(g_pui8DFUFunctionalDesc),
    g_pui8DFUFunctionalDesc
};

//*****************************************************************************
//
// This array lists all the sections that must be concatenated to make a
// single, complete DFU runtime configuration descriptor.
//
//*****************************************************************************
const tConfigSection *g_psDFUSections[] =
{
    &g_sDFUConfigSection,
    &g_sDFUInterfaceSection,
    &g_sDFUFunctionalDescSection
};

#define NUM_DFU_SECTIONS        (sizeof(g_psDFUSections) /                    \
                                 sizeof(g_psDFUSections[0]))

//*****************************************************************************
//
// The header for the single configuration we support.  This is the root of
// the data structure that defines all the bits and pieces that are pulled
// together to generate the configuration descriptor.
//
//*****************************************************************************
tConfigHeader g_sDFUConfigHeader =
{
    NUM_DFU_SECTIONS,
    g_psDFUSections
};

//*****************************************************************************
//
// Configuration Descriptor.
//
//*****************************************************************************
const tConfigHeader * const g_ppsDFUConfigDescriptors[] =
{
    &g_sDFUConfigHeader
};

//*****************************************************************************
//
// Forward references for device handler callbacks
//
//*****************************************************************************
static void HandleGetDescriptor(void *pvDFUInstance, tUSBRequest *psUSBRequest);
static void HandleRequest(void *pvDFUInstance, tUSBRequest *psUSBRequest);
static void HandleDevice(void *pvDFUInstance, uint32_t ui32Request,
                         void *pvRequestData);

//*****************************************************************************
//
// The device information structure for the USB DFU devices.
//
//*****************************************************************************
static const tCustomHandlers g_sDFUHandlers =
{
    //
    // GetDescriptor
    //
    HandleGetDescriptor,

    //
    // RequestHandler
    //
    HandleRequest,

    //
    // InterfaceChange
    //
    0,

    //
    // ConfigChange
    //
    0,

    //
    // DataReceived
    //
    0,

    //
    // DataSentCallback
    //
    0,

    //
    // ResetHandler
    //
    0,

    //
    // SuspendHandler
    //
    0,

    //
    //

    //
    // ResumeHandler
    //
    0,

    //
    // DisconnectHandler
    //
    0,

    //
    // EndpointHandler
    //
    0,

    //
    // Device handler.
    //
    HandleDevice,
};

//*****************************************************************************
//
// Device instance specific handler.  This callback received notifications of
// events related to handling interface, endpoint and string identifiers when
// a device is part of a composite device.  In this case, the only resource we
// need which may be renumbered is the DFU runtime interface.
//
//*****************************************************************************
static void
HandleDevice(void *pvDFUInstance, uint32_t ui32Request, void *pvRequestData)
{
    tDFUInstance *psInst;
    uint8_t *pui8Data;

    //
    // Get a pointer to the DFU device instance data pointer
    //
    psInst = &((tUSBDDFUDevice *)pvDFUInstance)->sPrivateData;

    //
    // Get a byte pointer to the data.
    //
    pui8Data = (uint8_t *)pvRequestData;

    //
    // Which request event have we been passed?
    //
    switch(ui32Request)
    {
        //
        // This was an interface change event.
        //
        case USB_EVENT_COMP_IFACE_CHANGE:
        {
            //
            // Save the change to the interface number.
            //
            psInst->ui8Interface = pui8Data[1];
            break;
        }

        //
        // We are not interested in any other event.
        //
        default:
        {
            break;
        }
    }
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever a request for a
// non-standard descriptor is received.
//
// \param pvDFUInstance is the instance data for this request.
// \param psUSBRequest points to the request received.
//
// This call parses the provided request structure and determines which
// descriptor is being requested.  Assuming the descriptor can be found, it is
// scheduled for transmission via endpoint zero.  If the descriptor cannot be
// found, the endpoint is stalled to indicate an error to the host.
//
//*****************************************************************************
static void
HandleGetDescriptor(void *pvDFUInstance, tUSBRequest *psUSBRequest)
{
    uint32_t ui32Size;

    ASSERT(pvDFUInstance != 0);

    //
    // Which type of class descriptor are we being asked for?  We only support
    // 1 type - the DFU functional descriptor.
    //
    if(((psUSBRequest->wValue >> 8) == USB_DFU_FUNC_DESCRIPTOR_TYPE) &&
       ((psUSBRequest->wValue & 0xFF) == 0))
    {
        //
        // If there is more data to send than the host requested then just
        // send the requested amount of data.
        //
        if((uint16_t)g_pui8DFUFunctionalDesc[0] > psUSBRequest->wLength)
        {
            ui32Size = (uint32_t)psUSBRequest->wLength;
        }
        else
        {
            ui32Size = (uint32_t)g_pui8DFUFunctionalDesc[0];
        }

        //
        // Send the data via endpoint 0.
        //
        USBDCDSendDataEP0(0, g_pui8DFUFunctionalDesc, ui32Size);
    }
    else
    {
        //
        // This was an unknown or invalid request so stall.
        //
        USBDCDStallEP0(0);
    }
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever a non-standard
// request is received.
//
// \param pvDFUInstance is the instance data for this HID device.
// \param psUSBRequest points to the request received.
//
// This call parses the provided request structure.  Assuming the request is
// understood, it is handled and any required response generated.  If the
// request cannot be handled by this device class, endpoint zero is stalled to
// indicate an error to the host.
//
//*****************************************************************************
static void
HandleRequest(void *pvDFUInstance, tUSBRequest *psUSBRequest)
{
    tDFUInstance *psInst;
    tUSBDDFUDevice *psDevice;

    ASSERT(pvDFUInstance != 0);

    //
    // Get a pointer to the DFU device structure
    //
    psDevice = pvDFUInstance;

    //
    // Get a pointer to the DFU device instance data pointer
    //
    psInst = &psDevice->sPrivateData;

    //
    // Make sure the request was for this interface.
    //
    if(psUSBRequest->wIndex != psInst->ui8Interface)
    {
        return;
    }

    //
    // Determine the type of request.
    //
    switch(psUSBRequest->bRequest)
    {
        //
        // We have been asked to detach.  In this case, we call back to the
        // application telling it to tidy up and re-enter the boot loader.  We
        // rely upon it doing this on our behalf since this must be done from a
        // non-interrupt context and this call is most likely in interrupt
        // context.
        //
        case USBD_DFU_REQUEST_DETACH:
        {
            //
            // Tell the application it's time to reenter the boot loader.
            //
            psDevice->pfnCallback(psDevice->pvCBData, USBD_DFU_EVENT_DETACH,
                                  0, (void *)0);
            break;
        }

        //
        // This request was not recognized so stall.
        //
        default:
        {
            USBDCDStallEP0(0);
            break;
        }
    }
}

//*****************************************************************************
//
//! Initializes DFU device operation for a given USB controller.
//!
//! \param ui32Index is the index of the USB controller which is to be
//! initialized for DFU runtime device operation.
//! \param psDFUDevice points to a structure containing parameters customizing
//! the operation of the DFU device.
//! \param psCompEntry is the composite device entry to initialize when
//! creating a composite device.
//!
//! The \e psCompEntry should point to the composite device entry to
//! initialize. This is part of the array that is passed to the
//! USBDCompositeInit() function.
//!
//! \return Returns zero on failure or a non-zero instance value that should be
//! used with the remaining USB DFU APIs.
//
//*****************************************************************************
void *
USBDDFUCompositeInit(uint32_t ui32Index, tUSBDDFUDevice *psDFUDevice,
                     tCompositeEntry *psCompEntry)
{
    tDFUInstance *psInst;

    //
    // Check parameter validity.
    //
    ASSERT(ui32Index == 0);
    ASSERT(psDFUDevice);
    ASSERT(psCompEntry != 0);

    //
    // Get a pointer to the DFU device instance data pointer
    //
    psInst = &psDFUDevice->sPrivateData;

    //
    // Initialize the composite entry that is used by the composite device
    // class.
    //
    if(psCompEntry != 0)
    {
        psCompEntry->psDevInfo = &psInst->sDevInfo;
        psCompEntry->pvInstance = (void *)psDFUDevice;
    }

    //
    // Initialize the device information structure.
    //
    psInst->sDevInfo.psCallbacks = &g_sDFUHandlers;
    psInst->sDevInfo.pui8DeviceDescriptor = g_pui8DFUDeviceDescriptor;
    psInst->sDevInfo.ppsConfigDescriptors = g_ppsDFUConfigDescriptors;
    psInst->sDevInfo.ppui8StringDescriptors = 0;
    psInst->sDevInfo.ui32NumStringDescriptors = 0;

    psInst->ui32USBBase = USB0_BASE;
    psInst->bConnected = false;
    psInst->ui8Interface = 0;

    //
    // Initialize the device info structure for the DFU device.
    //
    USBDCDDeviceInfoInit(0, &psInst->sDevInfo);

    //
    // Return the pointer to the instance indicating that everything went well.
    //
    return((void *)psDFUDevice);
}

//*****************************************************************************
//
//! Shuts down the DFU device.
//!
//! \param pvDFUInstance is the pointer to the device instance structure as
//! returned by USBDDFUCompositeInit().
//!
//! This function terminates DFU operation for the instance supplied and
//! removes the device from the USB bus.
//!
//! Following this call, the \e pvDFUInstance instance should not me used in
//! any other calls.
//!
//! \return None.
//
//*****************************************************************************
void
USBDDFUCompositeTerm(void *pvDFUInstance)
{
    tDFUInstance *psInst;

    ASSERT(pvDFUInstance);

    //
    // Get a pointer to our instance data.
    //
    psInst = &((tUSBDDFUDevice *)pvDFUInstance)->sPrivateData;

    //
    // Terminate the requested instance.
    //
    USBDCDTerm(0);

    psInst->ui32USBBase = 0;
}

//*****************************************************************************
//
//! Removes the current USB device from the bus and transfers control to the
//! DFU boot loader.
//!
//! This function should be called from the application's main loop (i.e. not
//! in interrupt context) following a callback to the USB DFU callback function
//! notifying the application of a DETACH request from the host.  The function
//! will prepare the system to switch to DFU mode and transfer control to the
//! boot loader in preparation for a firmware upgrade from the host.
//!
//! The application must ensure that it has completed all necessary shutdown
//! activities (saved any required data, etc.) before making this call since
//! the function will not return.
//!
//! \return This function does not return.
//
//*****************************************************************************
void
USBDDFUUpdateBegin(void)
{
    //
    // Terminate the USB device and take us off the bus.
    //
    USBDCDTerm(0);

    //
    // Disable all interrupts.
    //
    MAP_IntMasterDisable();

    //
    // We must make sure we turn off SysTick and its interrupt
    // before entering the boot loader!
    //
    MAP_SysTickIntDisable();
    MAP_SysTickDisable();

    //
    // Disable all processor interrupts.  Instead of disabling them
    // one at a time, a direct write to NVIC is done to disable all
    // peripheral interrupts.
    //
    HWREG(NVIC_DIS0) = 0xffffffff;
    HWREG(NVIC_DIS1) = 0xffffffff;

    //
    // Reset the USB peripheral
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);
    MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_USB0);
    MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_USB0);

    //
    // Wait for about a second.
    //
    MAP_SysCtlDelay(MAP_SysCtlClockGet() / 3);

    //
    // Re-enable interrupts at the NVIC level.
    //
    MAP_IntMasterEnable();

    //
    // Return control to the boot loader.  This is a call to the SVC
    // handler in the boot loader.
    //
    (*((void (*)(void))(*(uint32_t *)0x2c)))();

    //
    // Should never get here, but just in case.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
