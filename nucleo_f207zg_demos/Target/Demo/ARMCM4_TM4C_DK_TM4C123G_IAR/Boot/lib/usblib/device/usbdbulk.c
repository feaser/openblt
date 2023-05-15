//*****************************************************************************
//
// usbdbulk.c - USB bulk device class driver.
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
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/usbdrv.h"
#include "usblib/usblib.h"
#include "usblib/usblibpriv.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdbulk.h"
#include "usblib/device/usbdcomp.h"
#include "usblib/usblibpriv.h"

//*****************************************************************************
//
//! \addtogroup bulk_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// The subset of endpoint status flags that we consider to be reception
// errors.  These are passed to the client via USB_EVENT_ERROR if seen.
//
//*****************************************************************************
#define USB_RX_ERROR_FLAGS      (USBERR_DEV_RX_DATA_ERROR |                   \
                                 USBERR_DEV_RX_OVERRUN |                      \
                                 USBERR_DEV_RX_FIFO_FULL)

//*****************************************************************************
//
// Flags that may appear in ui16DeferredOpFlags to indicate some operation that
// has been requested but could not be processed at the time it was received.
// Each deferred operation is defined as the bit number that should be set in
// tBulkInstance->ui16DeferredOpFlags to indicate that the operation is
// pending.
//
//*****************************************************************************
#define BULK_DO_PACKET_RX       5

//*****************************************************************************
//
// Endpoints to use for each of the required endpoints in the driver.'
//
//*****************************************************************************
#define DATA_IN_ENDPOINT        USB_EP_1
#define DATA_OUT_ENDPOINT       USB_EP_1

//*****************************************************************************
//
// Maximum packet size for the bulk endpoints used for bulk data
// transmission and reception and the associated FIFO sizes to set aside
// for each endpoint.
//
//*****************************************************************************
#define DATA_IN_EP_FIFO_SIZE    USB_FIFO_SZ_64
#define DATA_OUT_EP_FIFO_SIZE   USB_FIFO_SZ_64

#define DATA_IN_EP_MAX_SIZE     USBFIFOSizeToBytes(DATA_IN_EP_FIFO_SIZE)
#define DATA_OUT_EP_MAX_SIZE    USBFIFOSizeToBytes(DATA_OUT_EP_FIFO_SIZE)

//*****************************************************************************
//
// Device Descriptor.  This is stored in RAM to allow several fields to be
// changed at runtime based on the client's requirements.
//
//*****************************************************************************
uint8_t g_pui8BulkDeviceDescriptor[] =
{
    18,                         // Size of this structure.
    USB_DTYPE_DEVICE,           // Type of this structure.
    USBShort(0x110),            // USB version 1.1 (if we say 2.0, hosts assume
                                // high-speed - see USB 2.0 spec 9.2.6.6)
    USB_CLASS_VEND_SPECIFIC,    // USB Device Class
    0,                          // USB Device Sub-class
    0,                          // USB Device protocol
    64,                         // Maximum packet size for default pipe.
    USBShort(0),                // Vendor ID (VID).
    USBShort(0),                // Product ID (PID).
    USBShort(0x100),            // Device Version BCD.
    1,                          // Manufacturer string identifier.
    2,                          // Product string identifier.
    3,                          // Product serial number.
    1                           // Number of configurations.
};

//*****************************************************************************
//
// Bulk device configuration descriptor.
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
//*****************************************************************************
uint8_t g_pui8BulkDescriptor[] =
{
    //
    // Configuration descriptor header.
    //
    9,                          // Size of the configuration descriptor.
    USB_DTYPE_CONFIGURATION,    // Type of this descriptor.
    USBShort(32),               // The total size of this full structure.
    1,                          // The number of interfaces in this
                                // configuration.
    1,                          // The unique value for this configuration.
    5,                          // The string identifier that describes this
                                // configuration.
    USB_CONF_ATTR_SELF_PWR,     // Bus Powered, Self Powered, remote wake up.
    250,                        // The maximum power in 2mA increments.
};

//*****************************************************************************
//
// The remainder of the configuration descriptor is stored in flash since we
// don't need to modify anything in it at runtime.
//
//*****************************************************************************
const uint8_t g_pui8BulkInterface[BULKINTERFACE_SIZE] =
{
    //
    // Vendor-specific Interface Descriptor.
    //
    9,                              // Size of the interface descriptor.
    USB_DTYPE_INTERFACE,            // Type of this descriptor.
    0,                              // The index for this interface.
    0,                              // The alternate setting for this
                                    // interface.
    2,                              // The number of endpoints used by this
                                    // interface.
    USB_CLASS_VEND_SPECIFIC,        // The interface class
    0,                              // The interface sub-class.
    0,                              // The interface protocol for the sub-class
                                    // specified above.
    4,                              // The string index for this interface.

    //
    // Endpoint Descriptor
    //
    7,                              // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,             // Descriptor type is an endpoint.
    USB_EP_DESC_IN | USBEPToIndex(DATA_IN_ENDPOINT),
    USB_EP_ATTR_BULK,               // Endpoint is a bulk endpoint.
    USBShort(DATA_IN_EP_MAX_SIZE),  // The maximum packet size.
    0,                              // The polling interval for this endpoint.

    //
    // Endpoint Descriptor
    //
    7,                               // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,              // Descriptor type is an endpoint.
    USB_EP_DESC_OUT | USBEPToIndex(DATA_OUT_ENDPOINT),
    USB_EP_ATTR_BULK,                // Endpoint is a bulk endpoint.
    USBShort(DATA_OUT_EP_MAX_SIZE),  // The maximum packet size.
    0,                               // The polling interval for this endpoint.
};

//*****************************************************************************
//
// The bulk configuration descriptor is defined as two sections, one
// containing just the 9 byte USB configuration descriptor and the other
// containing everything else that is sent to the host along with it.
//
//*****************************************************************************
const tConfigSection g_sBulkConfigSection =
{
    sizeof(g_pui8BulkDescriptor),
    g_pui8BulkDescriptor
};

const tConfigSection g_sBulkInterfaceSection =
{
    sizeof(g_pui8BulkInterface),
    g_pui8BulkInterface
};

//*****************************************************************************
//
// This array lists all the sections that must be concatenated to make a
// single, complete bulk device configuration descriptor.
//
//*****************************************************************************
const tConfigSection *g_psBulkSections[] =
{
    &g_sBulkConfigSection,
    &g_sBulkInterfaceSection
};

#define NUM_BULK_SECTIONS       (sizeof(g_psBulkSections) /                   \
                                 sizeof(g_psBulkSections[0]))

//*****************************************************************************
//
// The header for the single configuration we support.  This is the root of
// the data structure that defines all the bits and pieces that are pulled
// together to generate the configuration descriptor.
//
//*****************************************************************************
const tConfigHeader g_sBulkConfigHeader =
{
    NUM_BULK_SECTIONS,
    g_psBulkSections
};

//*****************************************************************************
//
// Configuration Descriptor.
//
//*****************************************************************************
const tConfigHeader * const g_ppBulkConfigDescriptors[] =
{
    &g_sBulkConfigHeader
};

//*****************************************************************************
//
// Forward references for device handler callbacks
//
//*****************************************************************************
static void HandleConfigChange(void *pvBulkDevice, uint32_t ui32Info);
static void HandleDisconnect(void *pvBulkDevice);
static void HandleEndpoints(void *pvBulkDevice, uint32_t ui32Status);
static void HandleSuspend(void *pvBulkDevice);
static void HandleResume(void *pvBulkDevice);
static void HandleDevice(void *pvBulkDevice, uint32_t ui32Request,
                         void *pvRequestData);

//*****************************************************************************
//
// Device event handler callbacks.
//
//*****************************************************************************
const tCustomHandlers g_sBulkHandlers =
{
    //
    // GetDescriptor
    //
    0,

    //
    // RequestHandler
    //
    0,

    //
    // InterfaceChange
    //
    0,

    //
    // ConfigChange
    //
    HandleConfigChange,

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
    HandleSuspend,

    //
    // ResumeHandler
    //
    HandleResume,

    //
    // DisconnectHandler
    //
    HandleDisconnect,

    //
    // EndpointHandler
    //
    HandleEndpoints,

    //
    // Device handler
    //
    HandleDevice
};

//*****************************************************************************
//
// Set or clear deferred operation flags in an "atomic" manner.
//
// \param pui16DeferredOp points to the flags variable which is to be modified.
// \param ui16Bit indicates which bit number is to be set or cleared.
// \param bSet indicates the state that the flag must be set to.  If \b true,
// the flag is set, if \b false, the flag is cleared.
//
// This function safely sets or clears a bit in a flag variable.  The operation
// makes use of bitbanding to ensure that the operation is atomic (no read-
// modify-write is required).
//
// \return None.
//
//*****************************************************************************
static void
SetDeferredOpFlag(volatile uint16_t *pui16DeferredOp, uint16_t ui16Bit,
                  bool bSet)
{
    //
    // Set the flag bit to 1 or 0 using a bitband access.
    //
    HWREGBITH(pui16DeferredOp, ui16Bit) = bSet ? 1 : 0;
}

//*****************************************************************************
//
// Receives notifications related to data received from the host.
//
// \param psBulkDevice is the device instance whose endpoint is to be
// processed.
// \param ui32Status is the USB interrupt status that caused this function to
// be called.
//
// This function is called from HandleEndpoints for all interrupts signaling
// the arrival of data on the bulk OUT endpoint (in other words, whenever the
// host has sent us a packet of data).  We inform the client that a packet
// is available and, on return, check to see if the packet has been read.  If
// not, we schedule another notification to the client for a later time.
//
// \return Returns \b true on success or \b false on failure.
//
//*****************************************************************************
static bool
ProcessDataFromHost(tUSBDBulkDevice *psBulkDevice, uint32_t ui32Status)
{
    uint32_t ui32EPStatus;
    uint32_t ui32Size;
    tBulkInstance *psInst;

    //
    // Get a pointer to the bulk device instance data pointer
    //
    psInst = &psBulkDevice->sPrivateData;

    //
    // Get the endpoint status to see why we were called.
    //
    ui32EPStatus = MAP_USBEndpointStatus(USB0_BASE, psInst->ui8OUTEndpoint);

    //
    // Clear the status bits.
    //
    MAP_USBDevEndpointStatusClear(USB0_BASE, psInst->ui8OUTEndpoint,
                                  ui32EPStatus);

    //
    // Has a packet been received?
    //
    if(ui32EPStatus & USB_DEV_RX_PKT_RDY)
    {
        //
        // Set the flag we use to indicate that a packet read is pending.  This
        // will be cleared if the packet is read.  If the client does not read
        // the packet in the context of the USB_EVENT_RX_AVAILABLE callback,
        // the event will be signaled later during tick processing.
        //
        SetDeferredOpFlag(&psInst->ui16DeferredOpFlags, BULK_DO_PACKET_RX,
                          true);

        //
        // How big is the packet we have just received?
        //
        ui32Size = MAP_USBEndpointDataAvail(psInst->ui32USBBase,
                                            psInst->ui8OUTEndpoint);

        //
        // The receive channel is not blocked so let the caller know
        // that a packet is waiting.  The parameters are set to indicate
        // that the packet has not been read from the hardware FIFO yet.
        //
        psBulkDevice->pfnRxCallback(psBulkDevice->pvRxCBData,
                                    USB_EVENT_RX_AVAILABLE,
                                    ui32Size, (void *)0);
    }
    else
    {
        //
        // No packet was received.  Some error must have been reported.  Check
        // and pass this on to the client if necessary.
        //
        if(ui32EPStatus & USB_RX_ERROR_FLAGS)
        {
            //
            // This is an error we report to the client so allow the callback
            // to handle it.
            //
            psBulkDevice->pfnRxCallback(psBulkDevice->pvRxCBData,
                                        USB_EVENT_ERROR,
                                        (ui32EPStatus & USB_RX_ERROR_FLAGS),
                                        (void *)0);
        }
        return(false);
    }

    return(true);
}

//*****************************************************************************
//
// Receives notifications related to data sent to the host.
//
// \param psBulkDevice is the device instance whose endpoint is to be
// processed.
// \param ui32Status is the USB interrupt status that caused this function to
// be called.
//
// This function is called from HandleEndpoints for all interrupts originating
// from the bulk IN endpoint (in other words, whenever data has been
// transmitted to the USB host).  We examine the cause of the interrupt and,
// if due to completion of a transmission, notify the client.
//
// \return Returns \b true on success or \b false on failure.
//
//*****************************************************************************
static bool
ProcessDataToHost(tUSBDBulkDevice *psBulkDevice, uint32_t ui32Status)
{
    tBulkInstance *psInst;
    uint32_t ui32EPStatus, ui32Size;

    //
    // Get a pointer to the bulk device instance data pointer
    //
    psInst = &psBulkDevice->sPrivateData;

    //
    // Get the endpoint status to see why we were called.
    //
    ui32EPStatus = MAP_USBEndpointStatus(psInst->ui32USBBase,
                                         psInst->ui8INEndpoint);

    //
    // Clear the status bits.
    //
    MAP_USBDevEndpointStatusClear(psInst->ui32USBBase, psInst->ui8INEndpoint,
                                  ui32EPStatus);

    //
    // Our last transmission completed.  Clear our state back to idle and
    // see if we need to send any more data.
    //
    psInst->iBulkTxState = eBulkStateIdle;

    //
    // Notify the client that the last transmission completed.
    //
    ui32Size = psInst->ui16LastTxSize;
    psInst->ui16LastTxSize = 0;
    psBulkDevice->pfnTxCallback(psBulkDevice->pvTxCBData,
                                USB_EVENT_TX_COMPLETE, ui32Size, (void *)0);

    return(true);
}

//*****************************************************************************
//
// Called by the USB stack for any activity involving one of our endpoints
// other than EP0.  This function is a fan out that merely directs the call to
// the correct handler depending upon the endpoint and transaction direction
// signaled in ui32Status.
//
//*****************************************************************************
static void
HandleEndpoints(void *pvBulkDevice, uint32_t ui32Status)
{
    tUSBDBulkDevice *psBulkDevice;
    tBulkInstance *psInst;

    ASSERT(pvBulkDevice != 0);

    //
    // The bulk device structure pointer.
    //
    psBulkDevice = (tUSBDBulkDevice *)pvBulkDevice;

    //
    // Get a pointer to the bulk device instance data pointer
    //
    psInst = &psBulkDevice->sPrivateData;

    //
    // Handler for the bulk OUT data endpoint.
    //
    if(ui32Status & (0x10000 << USBEPToIndex(psInst->ui8OUTEndpoint)))
    {
        //
        // Data is being sent to us from the host.
        //
        ProcessDataFromHost(psBulkDevice, ui32Status);
    }

    //
    // Handler for the bulk IN data endpoint.
    //
    if(ui32Status & (1 << USBEPToIndex(psInst->ui8INEndpoint)))
    {
        ProcessDataToHost(psBulkDevice, ui32Status);
    }
}

//*****************************************************************************
//
// Called by the USB stack whenever a configuration change occurs.
//
//*****************************************************************************
static void
HandleConfigChange(void *pvBulkDevice, uint32_t ui32Info)
{
    tBulkInstance *psInst;
    tUSBDBulkDevice *psBulkDevice;

    ASSERT(pvBulkDevice != 0);

    //
    // The bulk device structure pointer.
    //
    psBulkDevice = (tUSBDBulkDevice *)pvBulkDevice;

    //
    // Get a pointer to the bulk device instance data pointer
    //
    psInst = &psBulkDevice->sPrivateData;

    //
    // Set all our endpoints to idle state.
    //
    psInst->iBulkRxState = eBulkStateIdle;
    psInst->iBulkTxState = eBulkStateIdle;

    //
    // If we have a control callback, let the client know we are open for
    // business.
    //
    if(psBulkDevice->pfnRxCallback)
    {
        //
        // Pass the connected event to the client.
        //
        psBulkDevice->pfnRxCallback(psBulkDevice->pvRxCBData,
                                    USB_EVENT_CONNECTED, 0, (void *)0);
    }

    //
    // Remember that we are connected.
    //
    psInst->bConnected = true;
}

//*****************************************************************************
//
// Device instance specific handler.
//
//*****************************************************************************
static void
HandleDevice(void *pvBulkDevice, uint32_t ui32Request, void *pvRequestData)
{
    tBulkInstance *psInst;
    uint8_t *pui8Data;
    tUSBDBulkDevice *psBulkDevice;

    //
    // The bulk device structure pointer.
    //
    psBulkDevice = (tUSBDBulkDevice *)pvBulkDevice;

    //
    // Get a pointer to the bulk device instance data pointer
    //
    psInst = &psBulkDevice->sPrivateData;

    //
    // Create the 8-bit array used by the events supported by the USB Bulk
    // class.
    //
    pui8Data = (uint8_t *)pvRequestData;

    switch(ui32Request)
    {
        //
        // This was an interface change event.
        //
        case USB_EVENT_COMP_IFACE_CHANGE:
        {
            psInst->ui8Interface = pui8Data[1];
            break;
        }

        //
        // This was an endpoint change event.
        //
        case USB_EVENT_COMP_EP_CHANGE:
        {
            //
            // Determine if this is an IN or OUT endpoint that has changed.
            //
            if(pui8Data[0] & USB_EP_DESC_IN)
            {
                psInst->ui8INEndpoint = IndexToUSBEP((pui8Data[1] & 0x7f));
            }
            else
            {
                //
                // Extract the new endpoint number.
                //
                psInst->ui8OUTEndpoint = IndexToUSBEP(pui8Data[1] & 0x7f);
            }
            break;
        }

        default:
        {
            break;
        }
    }
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the device is
// disconnected from the host.
//
//*****************************************************************************
static void
HandleDisconnect(void *pvBulkDevice)
{
    tUSBDBulkDevice *psBulkDevice;
    tBulkInstance *psInst;

    ASSERT(pvBulkDevice != 0);

    //
    // The bulk device structure pointer.
    //
    psBulkDevice = (tUSBDBulkDevice *)pvBulkDevice;

    //
    // Get a pointer to the bulk device instance data pointer
    //
    psInst = &psBulkDevice->sPrivateData;

    //
    // If we are not currently connected so let the client know we are open
    // for business.
    //
    if(psInst->bConnected)
    {
        //
        // Pass the disconnected event to the client.
        //
        psBulkDevice->pfnRxCallback(psBulkDevice->pvRxCBData,
                                    USB_EVENT_DISCONNECTED, 0, (void *)0);
    }

    //
    // Remember that we are no longer connected.
    //
    psInst->bConnected = false;
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the bus is put into
// suspend state.
//
//*****************************************************************************
static void
HandleSuspend(void *pvBulkDevice)
{
    const tUSBDBulkDevice *psBulkDevice;

    ASSERT(pvBulkDevice != 0);

    //
    // The bulk device structure pointer.
    //
    psBulkDevice = (const tUSBDBulkDevice *)pvBulkDevice;

    //
    // Pass the event on to the client.
    //
    psBulkDevice->pfnRxCallback(psBulkDevice->pvRxCBData, USB_EVENT_SUSPEND, 0,
                                (void *)0);
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the bus is taken
// out of suspend state.
//
//*****************************************************************************
static void
HandleResume(void *pvBulkDevice)
{
    const tUSBDBulkDevice *psBulkDevice;

    ASSERT(pvBulkDevice != 0);

    //
    // The bulk device structure pointer.
    //
    psBulkDevice = (const tUSBDBulkDevice *)pvBulkDevice;

    //
    // Pass the event on to the client.
    //
    psBulkDevice->pfnRxCallback(psBulkDevice->pvRxCBData, USB_EVENT_RESUME, 0,
                                (void *)0);
}

//*****************************************************************************
//
// This function is called periodically and provides us with a time reference
// and method of implementing delayed or time-dependent operations.
//
// \param ui32Index is the index of the USB controller for which this tick
// is being generated.
// \param ui32TimemS is the elapsed time in milliseconds since the last call
// to this function.
//
// \return None.
//
//*****************************************************************************
static void
BulkTickHandler(void *pvBulkDevice, uint32_t ui32TimemS)
{
    tBulkInstance *psInst;
    uint32_t ui32Size;
    tUSBDBulkDevice *psBulkDevice;

    ASSERT(pvBulkDevice != 0);

    //
    // The bulk device structure pointer.
    //
    psBulkDevice = (tUSBDBulkDevice *)pvBulkDevice;

    //
    // Get a pointer to the bulk device instance data pointer
    //
    psInst = &psBulkDevice->sPrivateData;

    //
    // Do we have a deferred receive waiting
    //
    if(psInst->ui16DeferredOpFlags & (1 << BULK_DO_PACKET_RX))
    {
        //
        // Yes - how big is the waiting packet?
        //
        ui32Size = MAP_USBEndpointDataAvail(psInst->ui32USBBase,
                                            psInst->ui8OUTEndpoint);

        //
        // Tell the client that there is a packet waiting for it.
        //
        psBulkDevice->pfnRxCallback(psBulkDevice->pvRxCBData,
                                    USB_EVENT_RX_AVAILABLE, ui32Size,
                                    (void *)0);
    }

    return;
}

//*****************************************************************************
//
//! Initializes bulk device operation for a given USB controller.
//!
//! \param ui32Index is the index of the USB controller which is to be
//! initialized for bulk device operation.
//! \param psBulkDevice points to a structure containing parameters customizing
//! the operation of the bulk device.
//!
//! An application wishing to make use of a USB bulk communication channel
//! must call this function to initialize the USB controller and attach the
//! device to the USB bus.  This function performs all required USB
//! initialization.
//!
//! On successful completion, this function will return the \e psBulkDevice
//! pointer passed to it.  This must be passed on all future calls to the
//! device driver related to this device.
//!
//! The USBDBulk interface offers packet-based transmit and receive operation.
//! If the application would rather use block based communication with
//! transmit and receive buffers, USB buffers may be used above the bulk
//! transmit and receive channels to offer this functionality.
//!
//! Transmit Operation:
//!
//! Calls to USBDBulkPacketWrite must send no more than 64 bytes of data at a
//! time and may only be made when no other transmission is currently
//! outstanding.
//!
//! Once a packet of data has been acknowledged by the USB host, a
//! \b USB_EVENT_TX_COMPLETE event is sent to the application callback to
//! inform it that another packet may be transmitted.
//!
//! Receive Operation:
//!
//! An incoming USB data packet will result in a call to the application
//! callback with event \b USBD_EVENT_RX_AVAILABLE.  The application must then
//! call USBDBulkPacketRead(), passing a buffer capable of holding 64 bytes, to
//! retrieve the data and acknowledge reception to the USB host.
//!
//! \note The application must not make any calls to the low level USB Device
//! API if interacting with USB via the USB bulk device class API.  Doing so
//! will cause unpredictable (though almost certainly unpleasant) behavior.
//!
//! \return Returns NULL on failure or void pointer that should be used with
//! the remaining USB bulk class APSs.
//
//*****************************************************************************
void *
USBDBulkInit(uint32_t ui32Index, tUSBDBulkDevice *psBulkDevice)
{
    void *pvBulkDevice;
    tDeviceDescriptor *psDevDesc;
    tConfigDescriptor *psConfigDesc;

    //
    // Check parameter validity.
    //
    ASSERT(ui32Index == 0);
    ASSERT(psBulkDevice);

    pvBulkDevice = USBDBulkCompositeInit(ui32Index, psBulkDevice, 0);

    if(pvBulkDevice)
    {
        //
        // Fix up the device descriptor with the client-supplied values.
        //
        psDevDesc = (tDeviceDescriptor *)g_pui8BulkDeviceDescriptor;
        psDevDesc->idVendor = psBulkDevice->ui16VID;
        psDevDesc->idProduct = psBulkDevice->ui16PID;

        //
        // Fix up the configuration descriptor with client-supplied values.
        //
        psConfigDesc = (tConfigDescriptor *)g_pui8BulkDescriptor;
        psConfigDesc->bmAttributes = psBulkDevice->ui8PwrAttributes;
        psConfigDesc->bMaxPower = (uint8_t)(psBulkDevice->ui16MaxPowermA / 2);

        //
        // All is well so now pass the descriptors to the lower layer and put
        // the bulk device on the bus.
        //
        USBDCDInit(ui32Index, &psBulkDevice->sPrivateData.sDevInfo,
                   (void *)psBulkDevice);
    }

    //
    // Return the pointer to the instance indicating that everything went well.
    //
    return(pvBulkDevice);
}

//*****************************************************************************
//
//! Initializes bulk device operation for a given USB controller.
//!
//! \param ui32Index is the index of the USB controller which is to be
//! initialized for bulk device operation.
//! \param psBulkDevice points to a structure containing parameters customizing
//! the operation of the bulk device.
//! \param psCompEntry is the composite device entry to initialize when
//! creating a composite device.
//!
//! This call is very similar to USBDBulkInit() except that it is used for
//! initializing an instance of the bulk device for use in a composite device.
//! When this bulk device is part of a composite device, then the
//! \e psCompEntry should point to the composite device entry to initialize.
//! This is part of the array that is passed to the USBDCompositeInit()
//! function.
//!
//! \return Returns zero on failure or a non-zero value that should be
//! used with the remaining USB Bulk APIs.
//
//*****************************************************************************
void *
USBDBulkCompositeInit(uint32_t ui32Index, tUSBDBulkDevice *psBulkDevice,
                      tCompositeEntry *psCompEntry)
{
    tBulkInstance *psInst;

    //
    // Check parameter validity.
    //
    ASSERT(ui32Index == 0);
    ASSERT(psBulkDevice);
    ASSERT(psBulkDevice->ppui8StringDescriptors);
    ASSERT(psBulkDevice->pfnRxCallback);
    ASSERT(psBulkDevice->pfnTxCallback);

    //
    // Initialize the workspace in the passed instance structure.
    //
    psInst = &psBulkDevice->sPrivateData;

    //
    // Initialize the composite entry that is used by the composite device
    // class.
    //
    if(psCompEntry != 0)
    {
        psCompEntry->psDevInfo = &psInst->sDevInfo;
        psCompEntry->pvInstance = (void *)psBulkDevice;
    }

    //
    // Initialize the device information structure.
    //
    psInst->sDevInfo.psCallbacks = &g_sBulkHandlers;
    psInst->sDevInfo.pui8DeviceDescriptor = g_pui8BulkDeviceDescriptor;
    psInst->sDevInfo.ppsConfigDescriptors = g_ppBulkConfigDescriptors;
    psInst->sDevInfo.ppui8StringDescriptors = 0;
    psInst->sDevInfo.ui32NumStringDescriptors = 0;

    //
    // Set the basic state information for the class.
    //
    psInst->ui32USBBase = USB0_BASE;
    psInst->iBulkRxState = eBulkStateUnconfigured;
    psInst->iBulkTxState = eBulkStateUnconfigured;
    psInst->ui16DeferredOpFlags = 0;
    psInst->bConnected = false;

    //
    // Initialize the device info structure for the Bulk device.
    //
    USBDCDDeviceInfoInit(0, &psInst->sDevInfo);

    //
    // Set the default endpoint and interface assignments.
    //
    psInst->ui8INEndpoint = DATA_IN_ENDPOINT;
    psInst->ui8OUTEndpoint = DATA_OUT_ENDPOINT;
    psInst->ui8Interface = 0;

    //
    // Plug in the client's string stable to the device information
    // structure.
    //
    psInst->sDevInfo.ppui8StringDescriptors =
                                        psBulkDevice->ppui8StringDescriptors;
    psInst->sDevInfo.ui32NumStringDescriptors =
                                        psBulkDevice->ui32NumStringDescriptors;

    //
    // Initialize the USB tick module, this will prevent it from being
    // initialized later in the call to USBDCDInit();
    //
    InternalUSBTickInit();

    //
    // Register our tick handler (this must be done after USBDCDInit).
    //
    InternalUSBRegisterTickHandler(BulkTickHandler, (void *)psBulkDevice);

    //
    // Return the pointer to the instance indicating that everything went well.
    //
    return((void *)psBulkDevice);
}

//*****************************************************************************
//
//! Shut down the bulk device.
//!
//! \param pvBulkDevice is the pointer to the device instance structure as
//! returned by USBDBulkInit().
//!
//! This function terminates device operation for the instance supplied and
//! removes the device from the USB bus.  This function should not be called
//! if the bulk device is part of a composite device and instead the
//! USBDCompositeTerm() function should be called for the full composite
//! device.
//!
//! Following this call, the \e pvBulkDevice instance should not me used in any
//! other calls.
//!
//! \return None.
//
//*****************************************************************************
void
USBDBulkTerm(void *pvBulkDevice)
{
    tBulkInstance *psInst;

    ASSERT(pvBulkDevice);

    //
    // Get a pointer to the bulk device instance data pointer
    //
    psInst = &((tUSBDBulkDevice *)pvBulkDevice)->sPrivateData;

    //
    // Terminate the requested instance.
    //
    USBDCDTerm(USBBaseToIndex(psInst->ui32USBBase));

    psInst->ui32USBBase = 0;

    return;
}

//*****************************************************************************
//
//! Sets the client-specific pointer parameter for the receive channel
//! callback.
//!
//! \param pvBulkDevice is the pointer to the device instance structure as
//! returned by USBDBulkInit().
//! \param pvCBData is the pointer that client wishes to be provided on each
//! event sent to the receive channel callback function.
//!
//! The client uses this function to change the callback pointer passed in
//! the first parameter on all callbacks to the \e pfnRxCallback function
//! passed on USBDBulkInit().
//!
//! If a client wants to make runtime changes in the callback pointer, it must
//! ensure that the \e pvBulkDevice structure passed to USBDBulkInit() resides
//! in RAM.  If this structure is in flash, callback pointer changes are not
//! possible.
//!
//! \return Returns the previous callback pointer that was being used for
//! this instance's receive callback.
//
//*****************************************************************************
void *
USBDBulkSetRxCBData(void *pvBulkDevice, void *pvCBData)
{
    void *pvOldValue;

    ASSERT(pvBulkDevice);

    //
    // Set the callback data for the receive channel after remembering the
    // previous value.
    //
    pvOldValue = ((tUSBDBulkDevice *)pvBulkDevice)->pvRxCBData;
    ((tUSBDBulkDevice *)pvBulkDevice)->pvRxCBData = pvCBData;

    //
    // Return the previous callback pointer.
    //
    return(pvOldValue);
}

//*****************************************************************************
//
//! Sets the client-specific pointer parameter for the transmit callback.
//!
//! \param pvBulkDevice is the pointer to the device instance structure as
//! returned by USBDBulkInit().
//! \param pvCBData is the pointer that client wishes to be provided on each
//! event sent to the transmit channel callback function.
//!
//! The client uses this function to change the callback pointer passed in
//! the first parameter on all callbacks to the \e pfnTxCallback function
//! passed on USBDBulkInit().
//!
//! If a client wants to make runtime changes in the callback pointer, it must
//! ensure that the \e pvBulkDevice structure passed to USBDBulkInit() resides
//! in RAM.  If this structure is in flash, callback pointer changes are not
//! possible.
//!
//! \return Returns the previous callback pointer that was being used for
//! this instance's transmit callback.
//
//*****************************************************************************
void *
USBDBulkSetTxCBData(void *pvBulkDevice, void *pvCBData)
{
    void *pvOldValue;

    ASSERT(pvBulkDevice);

    //
    // Set the callback pointer for the transmit channel after remembering the
    // previous value.
    //
    pvOldValue = ((tUSBDBulkDevice *)pvBulkDevice)->pvTxCBData;
    ((tUSBDBulkDevice *)pvBulkDevice)->pvTxCBData = pvCBData;

    //
    // Return the previous callback pointer value.
    //
    return(pvOldValue);
}

//*****************************************************************************
//
//! Transmits a packet of data to the USB host via the bulk data interface.
//!
//! \param pvBulkDevice is the pointer to the device instance structure as
//! returned by USBDBulkInit().
//! \param pi8Data points to the first byte of data which is to be transmitted.
//! \param ui32Length is the number of bytes of data to transmit.
//! \param bLast indicates whether more data is to be written before a packet
//! should be scheduled for transmission.  If \b true, the client will make
//! a further call to this function.  If \b false, no further call will be
//! made and the driver should schedule transmission of a short packet.
//!
//! This function schedules the supplied data for transmission to the USB
//! host in a single USB packet.  If no transmission is currently ongoing,
//! the data is immediately copied to the relevant USB endpoint FIFO for
//! transmission.  Whenever a USB packet is acknowledged by the host, a
//! \b USB_EVENT_TX_COMPLETE event will be sent to the transmit channel
//! callback indicating that more data can now be transmitted.
//!
//! The maximum value for \e ui32Length is 64 bytes (the maximum USB packet
//! size for the bulk endpoints in use by the device).  Attempts to send more
//! data than this will result in a return code of 0 indicating that the data
//! cannot be sent.
//!
//! The \e bLast parameter allows a client to make multiple calls to this
//! function before scheduling transmission of the packet to the host.  This
//! can be helpful if, for example, constructing a packet on the fly or
//! writing a packet which spans the wrap point in a ring buffer.
//!
//! \return Returns the number of bytes actually sent.  At this level, this
//! will either be the number of bytes passed (if less than or equal to the
//! maximum packet size for the USB endpoint in use and no outstanding
//! transmission ongoing) or 0 to indicate a failure.
//
//*****************************************************************************
uint32_t
USBDBulkPacketWrite(void *pvBulkDevice, uint8_t *pi8Data, uint32_t ui32Length,
                    bool bLast)
{
    tBulkInstance *psInst;
    int32_t i32Retcode;

    ASSERT(pvBulkDevice);

    //
    // Get a pointer to the bulk device instance data pointer
    //
    psInst = &((tUSBDBulkDevice *)pvBulkDevice)->sPrivateData;

    //
    // Can we send the data provided?
    //
    if((ui32Length > DATA_IN_EP_MAX_SIZE) ||
       (psInst->iBulkTxState != eBulkStateIdle))
    {
        //
        // Either the packet was too big or we are in the middle of sending
        // another packet.  Return 0 to indicate that we can't send this data.
        //
        return(0);
    }

    //
    // Copy the data into the USB endpoint FIFO.
    //
    i32Retcode = MAP_USBEndpointDataPut(psInst->ui32USBBase,
                                        psInst->ui8INEndpoint,
                                        pi8Data, ui32Length);

    //
    // Did we copy the data successfully?
    //
    if(i32Retcode != -1)
    {
        //
        // Remember how many bytes we sent.
        //
        psInst->ui16LastTxSize += (uint16_t)ui32Length;

        //
        // If this is the last call for this packet, schedule transmission.
        //
        if(bLast)
        {
            //
            // Send the packet to the host if we have received all the data we
            // can expect for this packet.
            //
            psInst->iBulkTxState = eBulkStateWaitData;
            i32Retcode = MAP_USBEndpointDataSend(psInst->ui32USBBase,
                                                 psInst->ui8INEndpoint,
                                                 USB_TRANS_IN);
        }
    }

    //
    // Did an error occur while trying to send the data?
    //
    if(i32Retcode != -1)
    {
        //
        // No - tell the caller we sent all the bytes provided.
        //
        return(ui32Length);
    }
    else
    {
        //
        // Yes - tell the caller we could not send the data.
        //
        return(0);
    }
}

//*****************************************************************************
//
//! Reads a packet of data received from the USB host via the bulk data
//! interface.
//!
//! \param pvBulkDevice is the pointer to the device instance structure as
//! returned by USBDBulkInit().
//! \param pi8Data points to a buffer into which the received data will be
//! written.
//! \param ui32Length is the size of the buffer pointed to by pi8Data.
//! \param bLast indicates whether the client will make a further call to
//! read additional data from the packet.
//!
//! This function reads up to \e ui32Length bytes of data received from the USB
//! host into the supplied application buffer.  If the driver detects that the
//! entire packet has been read, it is acknowledged to the host.
//!
//! The \e bLast parameter is ignored in this implementation since the end of
//! a packet can be determined without relying upon the client to provide
//! this information.
//!
//! \return Returns the number of bytes of data read.
//
//*****************************************************************************
uint32_t
USBDBulkPacketRead(void *pvBulkDevice, uint8_t *pi8Data, uint32_t ui32Length,
                   bool bLast)
{
    uint32_t ui32EPStatus, ui32Count, ui32Pkt;
    tBulkInstance *psInst;
    int32_t i32Retcode;

    ASSERT(pvBulkDevice);

    //
    // Get our instance data pointer
    //
    psInst = &((tUSBDBulkDevice *)pvBulkDevice)->sPrivateData;

    //
    // Does the relevant endpoint FIFO have a packet waiting for us?
    //
    ui32EPStatus = MAP_USBEndpointStatus(psInst->ui32USBBase,
                                         psInst->ui8OUTEndpoint);

    if(ui32EPStatus & USB_DEV_RX_PKT_RDY)
    {
        //
        // How many bytes are available for us to receive?
        //
        ui32Pkt = MAP_USBEndpointDataAvail(psInst->ui32USBBase,
                                           psInst->ui8OUTEndpoint);

        //
        // Get as much data as we can.
        //
        ui32Count = ui32Length;
        i32Retcode = MAP_USBEndpointDataGet(psInst->ui32USBBase,
                                            psInst->ui8OUTEndpoint,
                                            pi8Data, &ui32Count);

        //
        // Did we read the last of the packet data?
        //
        if(ui32Count == ui32Pkt)
        {
            //
            // Clear the endpoint status so that we know no packet is
            // waiting.
            //
            MAP_USBDevEndpointStatusClear(psInst->ui32USBBase,
                                          psInst->ui8OUTEndpoint,
                                          ui32EPStatus);

            //
            // Acknowledge the data, thus freeing the host to send the
            // next packet.
            //
            MAP_USBDevEndpointDataAck(psInst->ui32USBBase,
                                      psInst->ui8OUTEndpoint, true);

            //
            // Clear the flag we set to indicate that a packet read is
            // pending.
            //
            SetDeferredOpFlag(&psInst->ui16DeferredOpFlags, BULK_DO_PACKET_RX,
                              false);
        }

        //
        // If all went well, tell the caller how many bytes they got.
        //
        if(i32Retcode != -1)
        {
            return(ui32Count);
        }
    }

    //
    // No packet was available or an error occurred while reading so tell
    // the caller no bytes were returned.
    //
    return(0);
}

//*****************************************************************************
//
//! Returns the number of free bytes in the transmit buffer.
//!
//! \param pvBulkDevice is the pointer to the device instance structure as
//! returned by USBDBulkInit().
//!
//! This function returns the maximum number of bytes that can be passed on a
//! call to USBDBulkPacketWrite and accepted for transmission.  The value
//! returned will be the maximum USB packet size (64) if no transmission is
//! currently outstanding or 0 if a transmission is in progress.
//!
//! \return Returns the number of bytes available in the transmit buffer.
//
//*****************************************************************************
uint32_t
USBDBulkTxPacketAvailable(void *pvBulkDevice)
{
    tBulkInstance *psInst;

    ASSERT(pvBulkDevice);

    //
    // Get our instance data pointer.
    //
    psInst = &((tUSBDBulkDevice *)pvBulkDevice)->sPrivateData;

    //
    // Do we have a packet transmission currently ongoing?
    //
    if(psInst->iBulkTxState != eBulkStateIdle)
    {
        //
        // We are not ready to receive a new packet so return 0.
        //
        return(0);
    }
    else
    {
        //
        // We can receive a packet so return the max packet size for the
        // relevant endpoint.
        //
        return(DATA_IN_EP_MAX_SIZE);
    }
}

//*****************************************************************************
//
//! Determines whether a packet is available and, if so, the size of the
//! buffer required to read it.
//!
//! \param pvBulkDevice is the pointer to the device instance structure as
//! returned by USBDBulkInit().
//!
//! This function may be used to determine if a received packet remains to be
//! read and allows the application to determine the buffer size needed to
//! read the data.
//!
//! \return Returns 0 if no received packet remains unprocessed or the
//! size of the packet if a packet is waiting to be read.
//
//*****************************************************************************
uint32_t
USBDBulkRxPacketAvailable(void *pvBulkDevice)
{
    uint32_t ui32EPStatus, ui32Size;
    tBulkInstance *psInst;

    ASSERT(pvBulkDevice);

    //
    // Get a pointer to the bulk device instance data pointer
    //
    psInst = &((tUSBDBulkDevice *)pvBulkDevice)->sPrivateData;

    //
    // Does the relevant endpoint FIFO have a packet waiting for us?
    //
    ui32EPStatus = MAP_USBEndpointStatus(psInst->ui32USBBase,
                                         psInst->ui8OUTEndpoint);

    if(ui32EPStatus & USB_DEV_RX_PKT_RDY)
    {
        //
        // Yes - a packet is waiting.  How big is it?
        //
        ui32Size = MAP_USBEndpointDataAvail(psInst->ui32USBBase,
                                            psInst->ui8OUTEndpoint);

        return(ui32Size);
    }
    else
    {
        //
        // There is no packet waiting to be received.
        //
        return(0);
    }
}

//*****************************************************************************
//
//! Reports the device power status (bus- or self-powered) to the USB library.
//!
//! \param pvBulkDevice is the pointer to the bulk device instance structure.
//! \param ui8Power indicates the current power status, either
//! \b USB_STATUS_SELF_PWR or \b USB_STATUS_BUS_PWR.
//!
//! Applications which support switching between bus- or self-powered
//! operation should call this function whenever the power source changes
//! to indicate the current power status to the USB library.  This information
//! is required by the USB library to allow correct responses to be provided
//! when the host requests status from the device.
//!
//! \return None.
//
//*****************************************************************************
void
USBDBulkPowerStatusSet(void *pvBulkDevice, uint8_t ui8Power)
{
    ASSERT(pvBulkDevice);

    //
    // Pass the request through to the lower layer.
    //
    USBDCDPowerStatusSet(0, ui8Power);
}

//*****************************************************************************
//
//! Requests a remote wake up to resume communication when in suspended state.
//!
//! \param pvBulkDevice is the pointer to the bulk device instance structure.
//!
//! When the bus is suspended, an application which supports remote wake up
//! (advertised to the host via the configuration descriptor) may call this
//! function to initiate remote wake up signaling to the host.  If the remote
//! wake up feature has not been disabled by the host, this will cause the bus
//! to resume operation within 20mS.  If the host has disabled remote wake up,
//! \b false will be returned to indicate that the wake up request was not
//! successful.
//!
//! \return Returns \b true if the remote wake up is not disabled and the
//! signaling was started or \b false if remote wake up is disabled or if
//! signaling is currently ongoing following a previous call to this function.
//
//*****************************************************************************
bool
USBDBulkRemoteWakeupRequest(void *pvBulkDevice)
{
    ASSERT(pvBulkDevice);

    //
    // Pass the request through to the lower layer.
    //
    return(USBDCDRemoteWakeupRequest(0));
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
