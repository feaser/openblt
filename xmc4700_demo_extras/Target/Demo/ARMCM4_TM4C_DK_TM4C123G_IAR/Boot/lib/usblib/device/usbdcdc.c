//*****************************************************************************
//
// usbdcdc.c - USB CDC ACM (serial) device class driver.
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
#include "usblib/usbcdc.h"
#include "usblib/usblibpriv.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdcomp.h"
#include "usblib/device/usbdcdc.h"

//*****************************************************************************
//
//! \addtogroup cdc_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Some assumptions and deviations from the CDC specification
// ----------------------------------------------------------
//
// 1.  Although the CDC specification indicates that the following requests
// should be supported by ACM CDC devices, these don't seem relevant to a
// virtual COM port implementation and are never seen when connecting to a
// Windows host and running either Hyperterminal or TeraTerm.  As a result,
// this implementation does not support them and stalls endpoint 0 if they are
// received.
//          - SEND_ENCAPSULATED_COMMAND
//          - GET_ENCAPSULATED_RESPONSE
//          - SET_COMM_FEATURE
//          - GET_COMM_FEATURE
//          - CLEAR_COMM_FEATURE
//
// 2.  The CDC specification is very clear on the fact that an ACM device
// should offer two interfaces - a control interface offering an interrupt IN
// endpoint and a data interface offering bulk IN and OUT endpoints.  Using
// this descriptor configuration, however, Windows insists on enumerating the
// device as two separate entities resulting in two virtual COM ports or one
// COM port and an Unknown Device (depending upon INF contents) appearing
// in Device Manager.  This implementation, derived by experimentation and
// examination of other virtual COM and CDC solutions, uses only a single
// interface combining all three endpoints.  This appears to satisfy
// Windows2000, XP and Vista and operates as intended using the Hyperterminal
// and TeraTerm terminal emulators.  Your mileage may vary with other
// (untested) operating systems!
//
//*****************************************************************************

//*****************************************************************************
//
// The subset of endpoint status flags that we consider to be reception
// errors.  These are passed to the client via USB_EVENT_ERROR if seen.
//
//*****************************************************************************
#define USB_RX_ERROR_FLAGS      (USBERR_DEV_RX_DATA_ERROR | \
                                 USBERR_DEV_RX_OVERRUN |    \
                                 USBERR_DEV_RX_FIFO_FULL)

//*****************************************************************************
//
// Size of the buffer to hold request-specific data read from the host.  This
// must be sized to accommodate the largest request structure that we intend
// processing.
//
//*****************************************************************************
#define MAX_REQUEST_DATA_SIZE   sizeof(tLineCoding)

//*****************************************************************************
//
// Flags that may appear in ui16DeferredOpFlags to indicate some operation that
// has been requested but could not be processed at the time it was received.
//
//*****************************************************************************
#define CDC_DO_SERIAL_STATE_CHANGE                                            \
                                0
#define CDC_DO_SEND_BREAK       1
#define CDC_DO_CLEAR_BREAK      2
#define CDC_DO_LINE_CODING_CHANGE                                             \
                                3
#define CDC_DO_LINE_STATE_CHANGE                                              \
                                4
#define CDC_DO_PACKET_RX        5

//*****************************************************************************
//
// The subset of deferred operations which result in the receive channel
// being blocked.
//
//*****************************************************************************
#define RX_BLOCK_OPS            ((1 << CDC_DO_SEND_BREAK) |                   \
                                 (1 << CDC_DO_LINE_CODING_CHANGE) |           \
                                 (1 << CDC_DO_LINE_STATE_CHANGE))

//*****************************************************************************
//
// Endpoints to use for each of the required endpoints in the driver.
//
//*****************************************************************************
#define CONTROL_ENDPOINT        USB_EP_1
#define DATA_IN_ENDPOINT        USB_EP_2
#define DATA_OUT_ENDPOINT       USB_EP_1

//*****************************************************************************
//
// The following are the USB interface numbers for the CDC serial device.
//
//*****************************************************************************
#define SERIAL_INTERFACE_CONTROL                                              \
                                0
#define SERIAL_INTERFACE_DATA   1

//*****************************************************************************
//
// Maximum packet size for the bulk endpoints used for serial data
// transmission and reception and the associated FIFO sizes to set aside
// for each endpoint.
//
//*****************************************************************************
#define DATA_IN_EP_FIFO_SIZE    USB_FIFO_SZ_64
#define DATA_OUT_EP_FIFO_SIZE   USB_FIFO_SZ_64
#define CTL_IN_EP_FIFO_SIZE     USB_FIFO_SZ_16

#define DATA_IN_EP_MAX_SIZE     USBFIFOSizeToBytes(DATA_IN_EP_FIFO_SIZE)
#define DATA_OUT_EP_MAX_SIZE    USBFIFOSizeToBytes(DATA_IN_EP_FIFO_SIZE)
#define CTL_IN_EP_MAX_SIZE      USBFIFOSizeToBytes(CTL_IN_EP_FIFO_SIZE)

//*****************************************************************************
//
// The collection of serial state flags indicating character errors.
//
//*****************************************************************************
#define USB_CDC_SERIAL_ERRORS   (USB_CDC_SERIAL_STATE_OVERRUN |               \
                                 USB_CDC_SERIAL_STATE_PARITY |                \
                                 USB_CDC_SERIAL_STATE_FRAMING)

//*****************************************************************************
//
// Device Descriptor.  This is stored in RAM to allow several fields to be
// changed at runtime based on the client's requirements.
//
//*****************************************************************************
uint8_t g_pui8CDCSerDeviceDescriptor[] =
{
    18,                             // Size of this structure.
    USB_DTYPE_DEVICE,               // Type of this structure.
    USBShort(0x110),                // USB version 1.1 (if we say 2.0, hosts
                                    // assume high-speed - see USB 2.0 spec
                                    // 9.2.6.6)
    USB_CLASS_CDC,                  // USB Device Class (spec 5.1.1)
    0,                              // USB Device Sub-class (spec 5.1.1)
    USB_CDC_PROTOCOL_NONE,          // USB Device protocol (spec 5.1.1)
    64,                             // Maximum packet size for default pipe.
    USBShort(0),                    // Vendor ID (filled in during
                                    // USBDCDCInit).
    USBShort(0),                    // Product ID (filled in during
                                    // USBDCDCInit).
    USBShort(0x100),                // Device Version BCD.
    1,                              // Manufacturer string identifier.
    2,                              // Product string identifier.
    3,                              // Product serial number.
    1                               // Number of configurations.
};

//*****************************************************************************
//
// CDC Serial configuration descriptor.
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
uint8_t g_pui8CDCSerDescriptor[] =
{
    //
    // Configuration descriptor header.
    //
    9,                              // Size of the configuration descriptor.
    USB_DTYPE_CONFIGURATION,        // Type of this descriptor.
    USBShort(9),                    // The total size of this full structure,
                                    // this will be patched so it is just set
                                    // to the size of this structure.
    2,                              // The number of interfaces in this
                                    // configuration.
    1,                              // The unique value for this configuration.
    5,                              // The string identifier that describes
                                    // this configuration.
    USB_CONF_ATTR_SELF_PWR,         // Bus Powered, Self Powered, remote wake
                                    // up.
    250,                            // The maximum power in 2mA increments.
};

const tConfigSection g_sCDCSerConfigSection =
{
    sizeof(g_pui8CDCSerDescriptor),
    g_pui8CDCSerDescriptor
};

//*****************************************************************************
//
// This is the Interface Association Descriptor for the serial device used in
// composite devices.
//
//*****************************************************************************
uint8_t g_pui8IADSerDescriptor[SERDESCRIPTOR_SIZE] =
{

    8,                              // Size of the interface descriptor.
    USB_DTYPE_INTERFACE_ASC,        // Interface Association Type.
    0x0,                            // Default starting interface is 0.
    0x2,                            // Number of interfaces in this
                                    // association.
    USB_CLASS_CDC,                  // The device class for this association.
    USB_CDC_SUBCLASS_ABSTRACT_MODEL,
                                    // The device subclass for this
                                    // association.
    USB_CDC_PROTOCOL_V25TER,        // The protocol for this association.
    0                               // The string index for this association.
};

const tConfigSection g_sIADSerConfigSection =
{
    sizeof(g_pui8IADSerDescriptor),
    g_pui8IADSerDescriptor
};

//*****************************************************************************
//
// This is the control interface for the serial device.
//
//*****************************************************************************
const uint8_t g_pui8CDCSerCommInterface[SERCOMMINTERFACE_SIZE] =
{
    //
    // Communication Class Interface Descriptor.
    //
    9,                              // Size of the interface descriptor.
    USB_DTYPE_INTERFACE,            // Type of this descriptor.
    SERIAL_INTERFACE_CONTROL,       // The index for this interface.
    0,                              // The alternate setting for this
                                    // interface.
    1,                              // The number of endpoints used by this
                                    // interface.
    USB_CLASS_CDC,                  // The interface class constant defined by
                                    // USB-IF (spec 5.1.3).
    USB_CDC_SUBCLASS_ABSTRACT_MODEL,
                                    // The interface sub-class constant
                                    // defined by USB-IF (spec 5.1.3).
    USB_CDC_PROTOCOL_V25TER,        // The interface protocol for the sub-class
                                    // specified above.
    4,                              // The string index for this interface.

    //
    // Communication Class Interface Functional Descriptor - Header
    //
    5,                              // Size of the functional descriptor.
    USB_CDC_CS_INTERFACE,           // CDC interface descriptor
    USB_CDC_FD_SUBTYPE_HEADER,      // Header functional descriptor
    USBShort(0x110),                // Complies with CDC version 1.1

    //
    // Communication Class Interface Functional Descriptor - ACM
    //
    4,                              // Size of the functional descriptor.
    USB_CDC_CS_INTERFACE,           // CDC interface descriptor
    USB_CDC_FD_SUBTYPE_ABSTRACT_CTL_MGMT,
    USB_CDC_ACM_SUPPORTS_LINE_PARAMS | USB_CDC_ACM_SUPPORTS_SEND_BREAK,

    //
    // Communication Class Interface Functional Descriptor - Unions
    //
    5,                              // Size of the functional descriptor.
    USB_CDC_CS_INTERFACE,           // CDC interface descriptor
    USB_CDC_FD_SUBTYPE_UNION,
    SERIAL_INTERFACE_CONTROL,
    SERIAL_INTERFACE_DATA,          // Data interface number

    //
    // Communication Class Interface Functional Descriptor - Call Management
    //
    5,                              // Size of the functional descriptor.
    USB_CDC_CS_INTERFACE,           // CDC interface descriptor
    USB_CDC_FD_SUBTYPE_CALL_MGMT,
    USB_CDC_CALL_MGMT_HANDLED,
    SERIAL_INTERFACE_DATA,          // Data interface number

    //
    // Endpoint Descriptor (interrupt, IN)
    //
    7,                              // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,             // Descriptor type is an endpoint.
    USB_EP_DESC_IN | USBEPToIndex(CONTROL_ENDPOINT),
    USB_EP_ATTR_INT,                // Endpoint is an interrupt endpoint.
    USBShort(CTL_IN_EP_MAX_SIZE),   // The maximum packet size.
    1                               // The polling interval for this endpoint.
};

const tConfigSection g_sCDCSerCommInterfaceSection =
{
    sizeof(g_pui8CDCSerCommInterface),
    g_pui8CDCSerCommInterface
};

//*****************************************************************************
//
// This is the Data interface for the serial device.
//
//*****************************************************************************
const uint8_t g_pui8CDCSerDataInterface[SERDATAINTERFACE_SIZE] =
{
    //
    // Communication Class Data Interface Descriptor.
    //
    9,                              // Size of the interface descriptor.
    USB_DTYPE_INTERFACE,            // Type of this descriptor.
    SERIAL_INTERFACE_DATA,          // The index for this interface.
    0,                              // The alternate setting for this
                                    // interface.
    2,                              // The number of endpoints used by this
                                    // interface.
    USB_CLASS_CDC_DATA,             // The interface class constant defined by
                                    // USB-IF (spec 5.1.3).
    0,                              // The interface sub-class constant
                                    // defined by USB-IF (spec 5.1.3).
    USB_CDC_PROTOCOL_NONE,          // The interface protocol for the sub-class
                                    // specified above.
    0,                              // The string index for this interface.

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
    7,                              // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,             // Descriptor type is an endpoint.
    USB_EP_DESC_OUT | USBEPToIndex(DATA_OUT_ENDPOINT),
    USB_EP_ATTR_BULK,               // Endpoint is a bulk endpoint.
    USBShort(DATA_OUT_EP_MAX_SIZE), // The maximum packet size.
    0,                              // The polling interval for this endpoint.
};

const tConfigSection g_sCDCSerDataInterfaceSection =
{
    sizeof(g_pui8CDCSerDataInterface),
    g_pui8CDCSerDataInterface
};

//*****************************************************************************
//
// This array lists all the sections that must be concatenated to make a
// single, complete CDC ACM configuration descriptor.
//
//*****************************************************************************
const tConfigSection *g_psCDCSerSections[] =
{
    &g_sCDCSerConfigSection,
    &g_sCDCSerCommInterfaceSection,
    &g_sCDCSerDataInterfaceSection,
};

#define NUM_CDCSER_SECTIONS     (sizeof(g_psCDCSerSections) /                 \
                                 sizeof(g_psCDCSerSections[0]))

//*****************************************************************************
//
// The header for the single configuration.  This is the root of the data
// structure that defines all the bits and pieces that are pulled together to
// generate the configuration descriptor.
//
//*****************************************************************************
const tConfigHeader g_sCDCSerConfigHeader =
{
    NUM_CDCSER_SECTIONS,
    g_psCDCSerSections
};

//*****************************************************************************
//
// This array lists all the sections that must be concatenated to make a
// single, complete CDC ACM configuration descriptor used in composite devices.
// The only addition is the g_sIADSerConfigSection.
//
//*****************************************************************************
const tConfigSection *g_psCDCCompSerSections[] =
{
    &g_sCDCSerConfigSection,
    &g_sIADSerConfigSection,
    &g_sCDCSerCommInterfaceSection,
    &g_sCDCSerDataInterfaceSection,
};

#define NUM_COMP_CDCSER_SECTIONS (sizeof(g_psCDCCompSerSections) /            \
                                  sizeof(g_psCDCCompSerSections[0]))

//*****************************************************************************
//
// The header for the composite configuration.  This is the root of the data
// structure that defines all the bits and pieces that are pulled together to
// generate the configuration descriptor.
//
//*****************************************************************************
const tConfigHeader g_sCDCCompSerConfigHeader =
{
    NUM_COMP_CDCSER_SECTIONS,
    g_psCDCCompSerSections
};

//*****************************************************************************
//
// Configuration Descriptor for the CDC serial class device.
//
//*****************************************************************************
const tConfigHeader * const g_ppCDCSerConfigDescriptors[] =
{
    &g_sCDCSerConfigHeader
};

//*****************************************************************************
//
// Configuration Descriptor for the CDC serial class device used in a composite
// device.
//
//*****************************************************************************
const tConfigHeader * const g_pCDCCompSerConfigDescriptors[] =
{
    &g_sCDCCompSerConfigHeader
};

//*****************************************************************************
//
// Forward references for device handler callbacks
//
//*****************************************************************************
static void HandleRequests(void *pvCDCDevice, tUSBRequest *pUSBRequest);
static void HandleConfigChange(void *pvCDCDevice, uint32_t ui32Info);
static void HandleEP0Data(void *pvCDCDevice, uint32_t ui32DataSize);
static void HandleDisconnect(void *pvCDCDevice);
static void HandleEndpoints(void *pvCDCDevice, uint32_t ui32Status);
static void HandleSuspend(void *pvCDCDevice);
static void HandleResume(void *pvCDCDevice);
static void HandleDevice(void *pvCDCDevice, uint32_t ui32Request,
                         void *pvRequestData);

//*****************************************************************************
//
// The device information structure for the USB serial device.
//
//*****************************************************************************
const tCustomHandlers g_sCDCHandlers =
{
    //
    // GetDescriptor
    //
    0,

    //
    // RequestHandler
    //
    HandleRequests,

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
    HandleEP0Data,

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
    // Device handler.
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
// Determines whether or not a client has consumed all received data previously
// passed to it.
//
//! \param psCDCDevice is the pointer to the device instance structure as returned
//! by USBDCDCInit().
//
// This function is called to determine whether or not a device has consumed
// all data previously passed to it via its receive callback.
//
// \return Returns \b true on success or \b false on failure.
//
//*****************************************************************************
static bool
DeviceConsumedAllData(const tUSBDCDCDevice *psCDCDevice)
{
    uint32_t ui32Remaining;

    //
    // Send the device an event requesting that it tell us how many bytes
    // of data it still has to process.
    //
    ui32Remaining = psCDCDevice->pfnRxCallback(psCDCDevice->pvRxCBData,
    USB_EVENT_DATA_REMAINING, 0, (void *)0);

    //
    // If any data remains to be processed, return false, else return true.
    //
    return(ui32Remaining ? false : true);
}

//*****************************************************************************
//
// Notifies the client that it should set or clear a break condition.
//
// \param psCDCDevice is the pointer to the device instance structure as returned
// by USBDCDCInit().
// \param bSend is \b true if a break condition is to be set or \b false if
// it is to be cleared.
//
// This function is called to instruct the client to start or stop sending a
// break condition on its serial transmit line.
//
// \return Returns \b true on success or \b false on failure.
//
//*****************************************************************************
static void
SendBreak(tUSBDCDCDevice *psCDCDevice, bool bSend)
{
    tCDCSerInstance *psInst;

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &psCDCDevice->sPrivateData;

    //
    // Set the break state flags as necessary.  If we are turning the break on,
    // set the flag to tell ourselves that we need to notify the client when
    // it is time to turn it off again.
    //
    SetDeferredOpFlag(&psInst->ui16DeferredOpFlags, CDC_DO_SEND_BREAK, false);
    SetDeferredOpFlag(&psInst->ui16DeferredOpFlags, CDC_DO_CLEAR_BREAK, bSend);

    //
    // Tell the client to start or stop sending the break.
    //
    psCDCDevice->pfnControlCallback(psCDCDevice->pvControlCBData,
                                 (bSend ? USBD_CDC_EVENT_SEND_BREAK :
                                          USBD_CDC_EVENT_CLEAR_BREAK), 0,
                                 (void *)0);
}

//*****************************************************************************
//
// Notifies the client of a host request to set the serial communication
// parameters.
//
// \param psCDCDevice is the device instance whose communication parameters are to
// be set.
//
// This function is called to notify the client when the host requests a change
// in the serial communication parameters (baud rate, parity, number of bits
// per character and number of stop bits) to use.
//
// \return None.
//
//*****************************************************************************
static void
SendLineCodingChange(tUSBDCDCDevice *psCDCDevice)
{
    tCDCSerInstance *psInst;

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &psCDCDevice->sPrivateData;

    //
    // Clear the flag we use to tell ourselves that the line coding change has
    // yet to be notified to the client.
    //
    SetDeferredOpFlag(&psInst->ui16DeferredOpFlags, CDC_DO_LINE_CODING_CHANGE,
                      false);

    //
    // Tell the client to update their serial line coding parameters.
    //
    psCDCDevice->pfnControlCallback(psCDCDevice->pvControlCBData,
                                 USBD_CDC_EVENT_SET_LINE_CODING, 0,
                                 &(psInst->sLineCoding));
}

//*****************************************************************************
//
// Notifies the client of a host request to set the RTS and DTR handshake line
// states.
//
// \param psCDCDevice is the device instance whose break condition is to be set or
// cleared.
//
// This function is called to notify the client when the host requests a change
// in the state of one or other of the RTS and DTR handshake lines.
//
// \return None.
//
//*****************************************************************************
static void
SendLineStateChange(tUSBDCDCDevice *psCDCDevice)
{
    tCDCSerInstance *psInst;

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &psCDCDevice->sPrivateData;

    //
    // Clear the flag we use to tell ourselves that the line coding change has
    // yet to be notified to the client.
    //
    SetDeferredOpFlag(&psInst->ui16DeferredOpFlags, CDC_DO_LINE_STATE_CHANGE,
                      false);

    //
    // Tell the client to update their serial line coding parameters.
    //
    psCDCDevice->pfnControlCallback(psCDCDevice->pvControlCBData,
                                 USBD_CDC_EVENT_SET_CONTROL_LINE_STATE,
                                 psInst->ui16ControlLineState,
                                 (void *)0);
}

//*****************************************************************************
//
// Notifies the client of a break request if no data remains to be processed.
//
// \param psCDCDevice is the device instance that is to be commanded to send a
// break condition.
//
// This function is called when the host requests that the device set a break
// condition on the serial transmit line.  If no data received from the host
// remains to be processed, the break request is passed to the control
// callback.  If data is outstanding, the call is ignored (with the operation
// being retried on the next timer tick).
//
// \return Returns \b true if the break notification was sent, \b false
// otherwise.
//
//*****************************************************************************
static bool
CheckAndSendBreak(tUSBDCDCDevice *psCDCDevice, uint16_t ui16Duration)
{
    bool bCanSend;

    //
    // Has the client consumed all data received from the host yet?
    //
    bCanSend = DeviceConsumedAllData(psCDCDevice);

    //
    // Can we send the break request?
    //
    if(bCanSend)
    {
        //
        // Pass the break request on to the client since no data remains to be
        // consumed.
        //
        SendBreak(psCDCDevice, (ui16Duration ? true : false));
    }

    //
    // Tell the caller whether or not we sent the notification.
    //
    return(bCanSend);
}

//*****************************************************************************
//
// Notifies the client of a request to change the serial line parameters if no
// data remains to be processed.
//
// \param psCDCDevice is the device instance whose line coding parameters are to
// be changed.
//
// This function is called when the host requests that the device change the
// serial line coding parameters.  If no data received from the host remains
// to be processed, the request is passed to the control callback.  If data is
// outstanding, the call is ignored (with the operation being retried on the
// next timer tick).
//
// \return Returns \b true if the notification was sent, \b false otherwise.
//
//*****************************************************************************
static bool
CheckAndSendLineCodingChange(tUSBDCDCDevice *psCDCDevice)
{
    bool bCanSend;

    //
    // Has the client consumed all data received from the host yet?
    //
    bCanSend = DeviceConsumedAllData(psCDCDevice);

    //
    // Can we send the break request?
    //
    if(bCanSend)
    {
        //
        // Pass the request on to the client since no data remains to be
        // consumed.
        //
        SendLineCodingChange(psCDCDevice);
    }

    //
    // Tell the caller whether or not we sent the notification.
    //
    return(bCanSend);
}

//*****************************************************************************
//
// Notifies the client of a request to change the handshake line states if no
// data remains to be processed.
//
// \param psCDCDevice is the device instance whose handshake line states are to
// be changed.
//
// This function is called when the host requests that the device change the
// state of one or other of the RTS or DTR handshake lines.  If no data
// received from the host remains to be processed, the request is passed to
// the control callback.  If data is outstanding, the call is ignored (with
// the operation being retried on the next timer tick).
//
// \return Returns \b true if the notification was sent, \b false otherwise.
//
//*****************************************************************************
static bool
CheckAndSendLineStateChange(tUSBDCDCDevice *psCDCDevice)
{
    bool bCanSend;

    //
    // Has the client consumed all data received from the host yet?
    //
    bCanSend = DeviceConsumedAllData(psCDCDevice);

    //
    // Can we send the break request?
    //
    if(bCanSend)
    {
        //
        // Pass the request on to the client since no data remains to be
        // consumed.
        //
        SendLineStateChange(psCDCDevice);
    }

    //
    // Tell the caller whether or not we sent the notification.
    //
    return(bCanSend);
}

//*****************************************************************************
//
// Notifies the client of a change in the serial line state.
//
// \param psInst is the instance whose serial state is to be reported.
//
// This function is called to send the current serial state information to
// the host via the the interrupt IN endpoint.  This notification informs the
// host of problems or conditions such as parity errors, breaks received,
// framing errors, etc.
//
// \return Returns \b true on success or \b false on failure.
//
//*****************************************************************************
static bool
SendSerialState(tUSBDCDCDevice *psCDCDevice)
{
    tUSBRequest sRequest;
    uint16_t ui16SerialState;
    tCDCSerInstance *psInst;
    int32_t i32Retcode;

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &psCDCDevice->sPrivateData;

    //
    // Remember that we are in the middle of sending a notification.
    //
    psInst->iCDCInterruptState = eCDCStateWaitData;

    //
    // Clear the flag we use to indicate that a send is required.
    //
    SetDeferredOpFlag(&psInst->ui16DeferredOpFlags, CDC_DO_SERIAL_STATE_CHANGE,
                      false);
    //
    // Take a snapshot of the serial state.
    //
    ui16SerialState = psInst->ui16SerialState;

    //
    // Build the request we will use to send the notification.
    //
    sRequest.bmRequestType = (USB_RTYPE_DIR_IN | USB_RTYPE_CLASS |
                                USB_RTYPE_INTERFACE);
    sRequest.bRequest = USB_CDC_NOTIFY_SERIAL_STATE;
    sRequest.wValue = 0;
    sRequest.wIndex = 0;
    sRequest.wLength = USB_CDC_NOTIFY_SERIAL_STATE_SIZE;

    //
    // Write the request structure to the USB FIFO.
    //
    i32Retcode = MAP_USBEndpointDataPut(psInst->ui32USBBase,
                                        psInst->ui8ControlEndpoint,
                                        (uint8_t *)&sRequest,
                                        sizeof(tUSBRequest));
    i32Retcode = MAP_USBEndpointDataPut(psInst->ui32USBBase,
                                        psInst->ui8ControlEndpoint,
                                        (uint8_t *)&ui16SerialState,
                                        USB_CDC_NOTIFY_SERIAL_STATE_SIZE);

    //
    // Did we correctly write the data to the endpoint FIFO?
    //
    if(i32Retcode != -1)
    {
        //
        // We put the data into the FIFO so now schedule it to be
        // sent.
        //
        i32Retcode = MAP_USBEndpointDataSend(psInst->ui32USBBase,
                                             psInst->ui8ControlEndpoint,
                                             USB_TRANS_IN);
    }

    //
    // If an error occurred, mark the endpoint as idle (to prevent possible
    // lockup) and return an error.
    //
    if(i32Retcode == -1)
    {
        psInst->iCDCInterruptState = eCDCStateIdle;
        return(false);
    }
    else
    {
        //
        // Everything went fine.  Clear the error bits that we just notified
        // and return true.
        //
        psInst->ui16SerialState &= ~(ui16SerialState & USB_CDC_SERIAL_ERRORS);
        return(true);
    }
}

//*****************************************************************************
//
// Receives notifications related to data received from the host.
//
// \param psCDCDevice is the device instance whose endpoint is to be processed.
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
bool
ProcessDataFromHost(tUSBDCDCDevice *psCDCDevice, uint32_t ui32Status)
{
    uint32_t ui32EPStatus, ui32Size;
    tCDCSerInstance *psInst;

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &psCDCDevice->sPrivateData;

    //
    // Get the endpoint status to see why we were called.
    //
    ui32EPStatus = MAP_USBEndpointStatus(psInst->ui32USBBase,
                                         psInst->ui8BulkOUTEndpoint);

    //
    // Clear the status bits.
    //
    MAP_USBDevEndpointStatusClear(psInst->ui32USBBase,
                                  psInst->ui8BulkOUTEndpoint,
                                  ui32EPStatus);

    //
    // Has a packet been received?
    //
    if(ui32EPStatus & USB_DEV_RX_PKT_RDY)
    {
        //
        // Set the flag we use to indicate that a packet read is pending.  This
        // will be cleared if the packet is read.  If the client doesn't read
        // the packet in the context of the USB_EVENT_RX_AVAILABLE callback,
        // the event will be notified later during tick processing.
        //
        SetDeferredOpFlag(&psInst->ui16DeferredOpFlags, CDC_DO_PACKET_RX,
                          true);

        //
        // Is the receive channel currently blocked?
        //
        if(!psInst->bControlBlocked && !psInst->bRxBlocked)
        {
            //
            // How big is the packet we have just been received?
            //
            ui32Size = MAP_USBEndpointDataAvail(psInst->ui32USBBase,
                                                psInst->ui8BulkOUTEndpoint);

            //
            // The receive channel is not blocked so let the caller know
            // that a packet is waiting.  The parameters are set to indicate
            // that the packet has not been read from the hardware FIFO yet.
            //
            psCDCDevice->pfnRxCallback(psCDCDevice->pvRxCBData,
                                    USB_EVENT_RX_AVAILABLE, ui32Size,
                                    (void *)0);
        }
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
            // This is an error we report to the client so...
            //
            psCDCDevice->pfnRxCallback(psCDCDevice->pvRxCBData, USB_EVENT_ERROR,
                                    (ui32EPStatus & USB_RX_ERROR_FLAGS),
                                    (void *)0);
        }

        return(false);
    }

    return(true);
}

//*****************************************************************************
//
// Receives notifications related to interrupt messages sent to the host.
//
// \param psCDCDevice is the device instance whose endpoint is to be processed.
// \param ui32Status is the USB interrupt status that caused this function to
// be called.
//
// This function is called from HandleEndpoints for all interrupts originating
// from the interrupt IN endpoint (in other words, whenever a notification has
// been transmitted to the USB host).
//
// \return Returns \b true on success or \b false on failure.
//
//*****************************************************************************
bool
ProcessNotificationToHost(tUSBDCDCDevice *psCDCDevice, uint32_t ui32Status)
{
    uint32_t ui32EPStatus;
    tCDCSerInstance *psInst;
    bool bRetcode;

    //
    // Assume all will go well until we have reason to believe otherwise.
    //
    bRetcode = true;

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &psCDCDevice->sPrivateData;

    //
    // Get the endpoint status to see why we were called.
    //
    ui32EPStatus = MAP_USBEndpointStatus(psInst->ui32USBBase,
                                         psInst->ui8ControlEndpoint);

    //
    // Clear the status bits.
    //
    MAP_USBDevEndpointStatusClear(psInst->ui32USBBase,
                                  psInst->ui8ControlEndpoint, ui32EPStatus);

    //
    // Did the state change while we were waiting for the previous notification
    // to complete?
    //
    if(psInst->ui16DeferredOpFlags & (1 << CDC_DO_SERIAL_STATE_CHANGE))
    {
        //
        // The state changed while we were waiting so we need to schedule
        // another notification immediately.
        //
        bRetcode = SendSerialState(psCDCDevice);
    }
    else
    {
        //
        // Our last notification completed and we did not have any new
        // notifications to make so the interrupt channel is now idle again.
        //
        psInst->iCDCInterruptState = eCDCStateIdle;
    }

    //
    // Tell the caller how things went.
    //
    return(bRetcode);
}

//*****************************************************************************
//
// Receives notifications related to data sent to the host.
//
// \param psCDCDevice is the device instance whose endpoint is to be processed.
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
bool
ProcessDataToHost(tUSBDCDCDevice *psCDCDevice, uint32_t ui32Status)
{
    tCDCSerInstance *psInst;
    uint32_t ui32EPStatus, ui32Size;
    bool bSentFullPacket;

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &psCDCDevice->sPrivateData;

    //
    // Get the endpoint status to see why we were called.
    //
    ui32EPStatus = MAP_USBEndpointStatus(psInst->ui32USBBase,
                                         psInst->ui8BulkINEndpoint);

    //
    // Clear the status bits.
    //
    MAP_USBDevEndpointStatusClear(psInst->ui32USBBase,
                                  psInst->ui8BulkINEndpoint, ui32EPStatus);

    //
    // Our last transmission completed.  Clear our state back to idle and
    // see if we need to send any more data.
    //
    psInst->iCDCTxState = eCDCStateIdle;

    //
    // If this notification is not as a result of sending a zero-length packet,
    // call back to the client to let it know we sent the last thing it passed
    // us.
    //
    if(psInst->ui16LastTxSize)
    {
        //
        // Have we just sent a 64 byte packet?
        //
        bSentFullPacket = (psInst->ui16LastTxSize == DATA_IN_EP_MAX_SIZE) ?
                           true : false;

        //
        // Notify the client that the last transmission completed.
        //
        ui32Size = (uint32_t)psInst->ui16LastTxSize;
        psInst->ui16LastTxSize = 0;
        psCDCDevice->pfnTxCallback(psCDCDevice->pvTxCBData, USB_EVENT_TX_COMPLETE,
                                ui32Size, (void *)0);

        //
        // If we had previously sent a full packet and the callback didn't
        // schedule a new transmission, send a zero length packet to indicate
        // the end of the transfer.
        //
        if(bSentFullPacket && !psInst->ui16LastTxSize)
        {
            //
            // We can expect another transmit complete notification after doing
            // this.
            //
            psInst->iCDCTxState = eCDCStateWaitData;

            //
            // Send the zero-length packet.
            //
            MAP_USBEndpointDataSend(psInst->ui32USBBase,
                                    psInst->ui8BulkINEndpoint,
                                    USB_TRANS_IN);
        }
    }

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
HandleEndpoints(void *pvCDCDevice, uint32_t ui32Status)
{
    tUSBDCDCDevice *psCDCDeviceInst;
    tCDCSerInstance *psInst;

    ASSERT(pvCDCDevice != 0);

    //
    // Determine if the serial device is in single or composite mode because
    // the meaning of ui32Index is different in both cases.
    //
    psCDCDeviceInst = pvCDCDevice;
    psInst = &psCDCDeviceInst->sPrivateData;

    //
    // Handler for the interrupt IN notification endpoint.
    //
    if(ui32Status & (1 << USBEPToIndex(psInst->ui8ControlEndpoint)))
    {
        //
        // We have sent an interrupt notification to the host.
        //
        ProcessNotificationToHost(psCDCDeviceInst, ui32Status);
    }

    //
    // Handler for the bulk OUT data endpoint.
    //
    if(ui32Status & (0x10000 << USBEPToIndex(psInst->ui8BulkOUTEndpoint)))
    {
        //
        // Data is being sent to us from the host.
        //
        ProcessDataFromHost(psCDCDeviceInst, ui32Status);
    }

    //
    // Handler for the bulk IN data endpoint.
    //
    if(ui32Status & (1 << USBEPToIndex(psInst->ui8BulkINEndpoint)))
    {
        ProcessDataToHost(psCDCDeviceInst, ui32Status);
    }
}

//*****************************************************************************
//
// Called by the USB stack whenever a configuration change occurs.
//
//*****************************************************************************
static void
HandleConfigChange(void *pvCDCDevice, uint32_t ui32Info)
{
    tCDCSerInstance *psInst;
    tUSBDCDCDevice *psCDCDevice;

    ASSERT(pvCDCDevice != 0);

    //
    // The CDC device structure pointer.
    //
    psCDCDevice = (tUSBDCDCDevice *)pvCDCDevice;

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &psCDCDevice->sPrivateData;

    //
    // Set all our endpoints to idle state.
    //
    psInst->iCDCInterruptState = eCDCStateIdle;
    psInst->iCDCRequestState = eCDCStateIdle;
    psInst->iCDCRxState = eCDCStateIdle;
    psInst->iCDCTxState = eCDCStateIdle;

    //
    // If we are not currently connected so let the client know we are open
    // for business.
    //
    if(!psInst->bConnected)
    {
        //
        // Pass the connected event to the client.
        //
        psCDCDevice->pfnControlCallback(psCDCDevice->pvControlCBData,
                                     USB_EVENT_CONNECTED, 0, (void *)0);
    }

    //
    // Remember that we are connected.
    //
    psInst->bConnected = true;
}

//*****************************************************************************
//
// USB data received callback.
//
// This function is called by the USB stack whenever any data requested from
// EP0 is received.
//
//*****************************************************************************
static void
HandleEP0Data(void *pvCDCDevice, uint32_t ui32DataSize)
{
    tUSBDCDCDevice *psCDCDevice;
    tCDCSerInstance *psInst;
    bool bRetcode;

    ASSERT(pvCDCDevice != 0);

    //
    // The CDC device structure pointer.
    //
    psCDCDevice = (tUSBDCDCDevice *)pvCDCDevice;

    //
    // If we were not passed any data, just return.
    //
    if(ui32DataSize == 0)
    {
        return;
    }

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &psCDCDevice->sPrivateData;

    //
    // Make sure we are actually expecting something.
    //
    if(psInst->iCDCRequestState != eCDCStateWaitData)
    {
        return;
    }

    //
    // Process the data received.  This will be a request-specific data
    // block associated with the last request received.
    //
    switch (psInst->ui8PendingRequest)
    {
        //
        // We just got the line coding structure.  Make sure the client has
        // read all outstanding data then pass it back to initiate a change
        // in the line state.
        //
        case USB_CDC_SET_LINE_CODING:
        {
            if(ui32DataSize != sizeof(tLineCoding))
            {
                USBDCDStallEP0(0);
            }
            else
            {
                //
                // Set the flag telling us that we need to send a line coding
                // notification to the client.
                //
                SetDeferredOpFlag(&psInst->ui16DeferredOpFlags,
                                  CDC_DO_LINE_CODING_CHANGE, true);

                //
                // See if we can send the notification immediately.
                //
                bRetcode = CheckAndSendLineCodingChange(psCDCDevice);

                //
                // If we could not send the line coding change request to the
                // client, block reception of more data from the host until
                // previous data is processed and we send the change request.
                //
                if(!bRetcode)
                {
                    psInst->bRxBlocked = true;
                }
            }
            break;
        }

            //
            // Oops - we seem to be waiting on a request which has not yet been
            // coded here.  Flag the error and stall EP0 anyway (even though
            // this would indicate a coding error).
            //
        default:
        {
            USBDCDStallEP0(0);
            ASSERT(0);
            break;
        }
    }

    //
    // All is well.  Set the state back to IDLE.
    //
    psInst->iCDCRequestState = eCDCStateIdle;
}

//*****************************************************************************
//
// Device instance specific handler.
//
//*****************************************************************************
static void
HandleDevice(void *pvCDCDevice, uint32_t ui32Request, void *pvRequestData)
{
    tCDCSerInstance *psInst;
    uint8_t *pui8Data;
    tUSBDCDCDevice *psCDCDevice;

    //
    // The CDC device structure pointer.
    //
    psCDCDevice = (tUSBDCDCDevice *)pvCDCDevice;

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &psCDCDevice->sPrivateData;

    //
    // Create the 8-bit array used by the events supported by the USB CDC
    // serial class.
    //
    pui8Data = (uint8_t *)pvRequestData;

    switch(ui32Request)
    {
        //
        // This was an interface change event.
        //
        case USB_EVENT_COMP_IFACE_CHANGE:
        {
            //
            // Save the change to the appropriate interface number.
            //
            if(pui8Data[0] == SERIAL_INTERFACE_CONTROL)
            {
                psInst->ui8InterfaceControl = pui8Data[1];
            }
            else if(pui8Data[0] == SERIAL_INTERFACE_DATA)
            {
                psInst->ui8InterfaceData = pui8Data[1];
            }
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
                //
                // Determine which IN endpoint to modify.
                //
                if((pui8Data[0] & 0x7f) == USBEPToIndex(CONTROL_ENDPOINT))
                {
                    psInst->ui8ControlEndpoint =
                        IndexToUSBEP((pui8Data[1] & 0x7f));
                }
                else
                {
                    psInst->ui8BulkINEndpoint =
                        IndexToUSBEP((pui8Data[1] & 0x7f));
                }
            }
            else
            {
                //
                // Extract the new endpoint number.
                //
                psInst->ui8BulkOUTEndpoint =
                    IndexToUSBEP(pui8Data[1] & 0x7f);
            }
            break;
        }

        //
        // Handle class specific reconfiguring of the configuration descriptor
        // once the composite class has built the full descriptor.
        //
        case USB_EVENT_COMP_CONFIG:
        {
            //
            // This sets the bFirstInterface of the Interface Association
            // descriptor to the first interface which is the control
            // interface used by this instance.
            //
            pui8Data[2] = psInst->ui8InterfaceControl;

            //
            // This sets the bMasterInterface of the Union descriptor to the
            // Control interface and the bSlaveInterface of the Union
            // Descriptor to the Data interface used by this instance.
            //
            pui8Data[29] = psInst->ui8InterfaceControl;
            pui8Data[30] = psInst->ui8InterfaceData;

            //
            // This sets the bDataInterface of the Union descriptor to the
            // Data interface used by this instance.
            pui8Data[35] = psInst->ui8InterfaceData;
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
// USB non-standard request callback.
//
// This function is called by the USB stack whenever any non-standard request
// is made to the device.  The handler should process any requests that it
// supports or stall EP0 in any unsupported cases.
//
//*****************************************************************************
static void
HandleRequests(void *pvCDCDevice, tUSBRequest *pUSBRequest)
{
    tUSBDCDCDevice *psCDCDevice;
    tCDCSerInstance *psInst;
    bool bRetcode;

    ASSERT(pvCDCDevice != 0);

    //
    // The CDC device structure pointer.
    //
    psCDCDevice = (tUSBDCDCDevice *)pvCDCDevice;

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &psCDCDevice->sPrivateData;

    //
    // Only handle requests meant for this interface.
    //
    if(pUSBRequest->wIndex != psInst->ui8InterfaceControl)
    {
        return;
    }

    //
    // Handle each of the requests that we expect from the host.
    //
    switch(pUSBRequest->bRequest)
    {
        case USB_CDC_SEND_ENCAPSULATED_COMMAND:
        {
            //
            // This implementation makes use of no communication protocol so
            // this request is meaningless.  We stall endpoint 0 if we receive
            // it.
            //
            USBDCDStallEP0(0);
            break;
        }

        case USB_CDC_GET_ENCAPSULATED_RESPONSE:
        {
            //
            // This implementation makes use of no communication protocol so
            // this request is meaningless.  We stall endpoint 0 if we receive
            // it.
            //
            USBDCDStallEP0(0);
            break;
        }

        case USB_CDC_SET_COMM_FEATURE:
        {
            //
            // This request is apparently required by an ACM device but does
            // not appear relevant to a virtual COM port and is never used by
            // Windows (or, at least, is not seen when using Hyperterminal or
            // TeraTerm via a Windows virtual COM port).  We stall endpoint 0
            // to indicate that we do not support the request.
            //
            USBDCDStallEP0(0);
            break;
        }

        case USB_CDC_GET_COMM_FEATURE:
        {
            //
            // This request is apparently required by an ACM device but does
            // not appear relevant to a virtual COM port and is never used by
            // Windows (or, at least, is not seen when using Hyperterminal or
            // TeraTerm via a Windows virtual COM port).  We stall endpoint 0
            // to indicate that we do not support the request.
            //
            USBDCDStallEP0(0);
            break;
        }

        case USB_CDC_CLEAR_COMM_FEATURE:
        {
            //
            // This request is apparently required by an ACM device but does
            // not appear relevant to a virtual COM port and is never used by
            // Windows (or, at least, is not seen when using Hyperterminal or
            // TeraTerm via a Windows virtual COM port).  We stall endpoint 0
            // to indicate that we do not support the request.
            //
            USBDCDStallEP0(0);
            break;
        }

        //
        // Set the serial communication parameters.
        //
        case USB_CDC_SET_LINE_CODING:
        {
            //
            // Remember the request we are processing.
            //
            psInst->ui8PendingRequest = USB_CDC_SET_LINE_CODING;

            //
            // Set the state to indicate we are waiting for data.
            //
            psInst->iCDCRequestState = eCDCStateWaitData;

            //
            // Now read the payload of the request.  We handle the actual
            // operation in the data callback once this data is received.
            //
            USBDCDRequestDataEP0(0, (uint8_t *)&psInst->sLineCoding,
                                 sizeof(tLineCoding));

            //
            // ACK what we have already received.  We must do this after
            // requesting the data or we get into a race condition where the
            // data may return before we have set the stack state appropriately
            // to receive it.
            //
            MAP_USBDevEndpointDataAck(psInst->ui32USBBase, USB_EP_0, false);

            break;
        }

        //
        // Return the serial communication parameters.
        //
        case USB_CDC_GET_LINE_CODING:
        {
            tLineCoding sLineCoding;

            //
            // ACK what we have already received
            //
            MAP_USBDevEndpointDataAck(psInst->ui32USBBase, USB_EP_0, false);

            //
            // Ask the client for the current line coding.
            //
            psCDCDevice->pfnControlCallback(psCDCDevice->pvControlCBData,
                                         USBD_CDC_EVENT_GET_LINE_CODING, 0,
                                         &sLineCoding);

            //
            // Send the line coding information back to the host.
            //
            USBDCDSendDataEP0(0, (uint8_t *)&sLineCoding, sizeof(tLineCoding));

            break;
        }

        case USB_CDC_SET_CONTROL_LINE_STATE:
        {
            //
            // ACK what we have already received
            //
            MAP_USBDevEndpointDataAck(psInst->ui32USBBase, USB_EP_0, false);

            //
            // Set the handshake lines as required.
            //
            psInst->ui16ControlLineState = pUSBRequest->wValue;

            //
            // Remember that we are due to notify the client of a line
            // state change.
            //
            SetDeferredOpFlag(&psInst->ui16DeferredOpFlags,
                              CDC_DO_LINE_STATE_CHANGE, true);

            //
            // See if we can notify now.
            //
            bRetcode = CheckAndSendLineStateChange(psCDCDevice);

            //
            // If we could not send the line state change request to the
            // client, block reception of more data from the host until
            // previous data is processed and we send the change request.
            //
            if(!bRetcode)
            {
                psInst->bRxBlocked = true;
            }

            break;
        }

        case USB_CDC_SEND_BREAK:
        {
            //
            // ACK what we have already received
            //
            MAP_USBDevEndpointDataAck(psInst->ui32USBBase, USB_EP_0, false);

            //
            // Keep a copy of the requested break duration.
            //
            psInst->ui16BreakDuration = pUSBRequest->wValue;

            //
            // Remember that we need to send a break request.
            //
            SetDeferredOpFlag(&psInst->ui16DeferredOpFlags,
                              CDC_DO_SEND_BREAK, true);

            //
            // Send the break request if all outstanding receive data has been
            // processed.
            //
            bRetcode = CheckAndSendBreak(psCDCDevice, pUSBRequest->wValue);

            //
            // If we could not send the line coding change request to the
            // client, block reception of more data from the host until
            // previous data is processed and we send the change request.
            //
            if(!bRetcode)
            {
                psInst->bRxBlocked = true;
            }

            break;
        }

        //
        // These are valid CDC requests but not ones that an ACM device should
        // receive.
        //
        case USB_CDC_SET_AUX_LINE_STATE:
        case USB_CDC_SET_HOOK_STATE:
        case USB_CDC_PULSE_SETUP:
        case USB_CDC_SEND_PULSE:
        case USB_CDC_SET_PULSE_TIME:
        case USB_CDC_RING_AUX_JACK:
        case USB_CDC_SET_RINGER_PARMS:
        case USB_CDC_GET_RINGER_PARMS:
        case USB_CDC_SET_OPERATION_PARMS:
        case USB_CDC_GET_OPERATION_PARMS:
        case USB_CDC_SET_LINE_PARMS:
        case USB_CDC_GET_LINE_PARMS:
        case USB_CDC_DIAL_DIGITS:
        case USB_CDC_SET_UNIT_PARAMETER:
        case USB_CDC_GET_UNIT_PARAMETER:
        case USB_CDC_CLEAR_UNIT_PARAMETER:
        case USB_CDC_GET_PROFILE:
        case USB_CDC_SET_ETHERNET_MULTICAST_FILTERS:
        case USB_CDC_SET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER:
        case USB_CDC_GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER:
        case USB_CDC_SET_ETHERNET_PACKET_FILTER:
        case USB_CDC_GET_ETHERNET_STATISTIC:
        case USB_CDC_SET_ATM_DATA_FORMAT:
        case USB_CDC_GET_ATM_DEVICE_STATISTICS:
        case USB_CDC_SET_ATM_DEFAULT_VC:
        case USB_CDC_GET_ATM_VC_STATISTICS:
        {
            USBDCDStallEP0(0);
            break;
        }

        default:
        {
            //
            // This request is not part of the CDC specification.
            //
            USBDCDStallEP0(0);
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
HandleDisconnect(void *pvCDCDevice)
{
    tUSBDCDCDevice *psCDCDevice;
    tCDCSerInstance *psInst;

    ASSERT(pvCDCDevice != 0);

    //
    // The CDC device structure pointer.
    //
    psCDCDevice = (tUSBDCDCDevice *)pvCDCDevice;

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &psCDCDevice->sPrivateData;

    //
    // If we are not currently connected and we have a control callback,
    // let the client know we are open for business.
    //
    if(psInst->bConnected)
    {
        //
        // Pass the disconnected event to the client.
        //
        psCDCDevice->pfnControlCallback(psCDCDevice->pvControlCBData,
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
HandleSuspend(void *pvCDCDevice)
{
    const tUSBDCDCDevice *psCDCDevice;

    ASSERT(pvCDCDevice != 0);

    //
    // The CDC device structure pointer.
    //
    psCDCDevice = (const tUSBDCDCDevice *)pvCDCDevice;

    //
    // Pass the event on to the client.
    //
    psCDCDevice->pfnControlCallback(psCDCDevice->pvControlCBData,
                                    USB_EVENT_SUSPEND, 0, (void *)0);
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the bus is taken
// out of suspend state.
//
//*****************************************************************************
static void
HandleResume(void *pvCDCDevice)
{
    tUSBDCDCDevice *psCDCDevice;

    ASSERT(pvCDCDevice != 0);

    //
    // The CDC device structure pointer.
    //
    psCDCDevice = (tUSBDCDCDevice *)pvCDCDevice;

    //
    // Pass the event on to the client.
    //
    psCDCDevice->pfnControlCallback(psCDCDevice->pvControlCBData,
                                    USB_EVENT_RESUME, 0, (void *)0);
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
CDCTickHandler(void *pvCDCDevice, uint32_t ui32TimemS)
{
    bool bCanSend;
    tUSBDCDCDevice *psCDCDevice;
    tCDCSerInstance *psInst;
    uint32_t ui32Size;

    ASSERT(pvCDCDevice != 0);

    //
    // The CDC device structure pointer.
    //
    psCDCDevice = (tUSBDCDCDevice *)pvCDCDevice;

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &psCDCDevice->sPrivateData;

    //
    // Is there any outstanding operation that we should try to perform?
    //
    if(psInst->ui16DeferredOpFlags)
    {
        //
        // Yes - we have at least one deferred operation pending.  First check
        // to see if it is time to turn off a break condition.
        //
        if(psInst->ui16DeferredOpFlags & (1 << CDC_DO_CLEAR_BREAK))
        {
            //
            // Will our break timer expire this time?
            //
            if(psInst->ui16BreakDuration <= ui32TimemS)
            {
                //
                // Yes - turn off the break condition.
                //
                SendBreak(psCDCDevice, false);
            }
            else
            {
                //
                // We have not timed out yet.  Decrement the break timer.
                //
                psInst->ui16BreakDuration -= (uint16_t)ui32TimemS;
            }
        }

        // Now check to see if the client has any data remaining to be
        // processed.  This information is needed by the remaining deferred
        // operations which are waiting for the receive pipe to be emptied
        // before they can be carried out.
        //
        bCanSend = DeviceConsumedAllData(psCDCDevice);

        //
        // Has all outstanding data been consumed?
        //
        if(bCanSend)
        {
            //
            // Yes - go ahead and notify the client of the various things
            // it has been asked to do while we waited for data to be
            // consumed.
            //

            //
            // Do we need to start sending a break condition?
            //
            if(psInst->ui16DeferredOpFlags & (1 << CDC_DO_SEND_BREAK))
            {
                SendBreak(psCDCDevice, true);
            }

            //
            // Do we need to set the RTS/DTR states?
            //
            if(psInst->ui16DeferredOpFlags & (1 << CDC_DO_LINE_STATE_CHANGE))
            {
                SendLineStateChange(psCDCDevice);
            }

            //
            // Do we need to change the line coding parameters?
            //
            if(psInst->ui16DeferredOpFlags & (1 << CDC_DO_LINE_CODING_CHANGE))
            {
                SendLineCodingChange(psCDCDevice);
            }

            //
            // NOTE: We do not need to handle CDC_DO_SERIAL_STATE_CHANGE here
            // since this is handled in the transmission complete notification
            // for the control IN endpoint (ProcessNotificationToHost()).
            //

            //
            // If all the deferred operations which caused the receive channel
            // to be blocked are now handled, we can unblock receive and handle
            // any packet that is currently waiting to be received.
            //
            if(!(psInst->ui16DeferredOpFlags & RX_BLOCK_OPS))
            {
                //
                // We can remove the receive block.
                //
                psInst->bRxBlocked = false;
            }
        }

        //
        // Is the receive channel unblocked?
        //
        if(!psInst->bRxBlocked)
        {
            //
            // Do we have a deferred receive waiting
            //
            if(psInst->ui16DeferredOpFlags & (1 << CDC_DO_PACKET_RX))
            {
                //
                // Yes - how big is the waiting packet?
                //
                ui32Size = MAP_USBEndpointDataAvail(psInst->ui32USBBase,
                                                    psInst->ui8BulkOUTEndpoint);

                // Tell the client that there is a packet waiting for it.
                //
                psCDCDevice->pfnRxCallback(psCDCDevice->pvRxCBData,
                                        USB_EVENT_RX_AVAILABLE, ui32Size,
                                        (void *)0);
            }
        }
    }

    return;
}

//*****************************************************************************
//
//! Initializes CDC device operation when used with a composite device.
//!
//! \param ui32Index is the index of the USB controller in use.
//! \param psCDCDevice points to a structure containing parameters customizing
//! the operation of the CDC device.
//! \param psCompEntry is the composite device entry to initialize when
//! creating a composite device.
//!
//! This call is very similar to USBDCDCInit() except that it is used for
//! initializing an instance of the serial device for use in a composite
//! device.  When this CDC serial device is part of a composite device, then
//! the \e psCompEntry should point to the composite device entry to
//! initialize.  This is part of the array that is passed to the
//! USBDCompositeInit() function.
//!
//! \return Returns zero on failure or a non-zero instance value that should be
//! used with the remaining USB CDC APIs.
//
//*****************************************************************************
void *
USBDCDCCompositeInit(uint32_t ui32Index, tUSBDCDCDevice *psCDCDevice,
                     tCompositeEntry *psCompEntry)
{
    tCDCSerInstance *psInst;

    //
    // Check parameter validity.
    //
    ASSERT(ui32Index == 0);
    ASSERT(psCDCDevice);
    ASSERT(psCDCDevice->pfnControlCallback);
    ASSERT(psCDCDevice->pfnRxCallback);
    ASSERT(psCDCDevice->pfnTxCallback);

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &psCDCDevice->sPrivateData;

    //
    // Initialize the composite entry that is used by the composite device
    // class.
    //
    if(psCompEntry != 0)
    {
        psCompEntry->psDevInfo = &psInst->sDevInfo;
        psCompEntry->pvInstance = (void *)psCDCDevice;
    }

    //
    // Initialize the device information structure.
    //
    psInst->sDevInfo.psCallbacks = &g_sCDCHandlers;
    psInst->sDevInfo.pui8DeviceDescriptor = g_pui8CDCSerDeviceDescriptor;

    //
    // The CDC serial configuration is different for composite devices and
    // stand alone devices.
    //
    if(psCompEntry == 0)
    {
        psInst->sDevInfo.ppsConfigDescriptors = g_ppCDCSerConfigDescriptors;
    }
    else
    {
        psInst->sDevInfo.ppsConfigDescriptors = g_pCDCCompSerConfigDescriptors;
    }
    psInst->sDevInfo.ppui8StringDescriptors = 0;
    psInst->sDevInfo.ui32NumStringDescriptors = 0;

    //
    // Set the default endpoint and interface assignments.
    //
    psInst->ui8BulkINEndpoint = DATA_IN_ENDPOINT;
    psInst->ui8BulkOUTEndpoint = DATA_OUT_ENDPOINT;
    psInst->ui8InterfaceControl = SERIAL_INTERFACE_CONTROL;
    psInst->ui8InterfaceData = SERIAL_INTERFACE_DATA;

    //
    // By default do not use the interrupt control endpoint.  The single
    // instance CDC serial device will turn this on in USBDCDCInit();
    //
    psInst->ui8ControlEndpoint = CONTROL_ENDPOINT;

    //
    // Initialize the workspace in the passed instance structure.
    //
    psInst->ui32USBBase = USB0_BASE;
    psInst->iCDCRxState = eCDCStateUnconfigured;
    psInst->iCDCTxState = eCDCStateUnconfigured;
    psInst->iCDCInterruptState = eCDCStateUnconfigured;
    psInst->iCDCRequestState = eCDCStateUnconfigured;
    psInst->ui8PendingRequest = 0;
    psInst->ui16BreakDuration = 0;
    psInst->ui16SerialState = 0;
    psInst->ui16DeferredOpFlags = 0;
    psInst->ui16ControlLineState = 0;
    psInst->bRxBlocked = false;
    psInst->bControlBlocked = false;
    psInst->bConnected = false;

    //
    // Initialize the device info structure for the serial device.
    //
    USBDCDDeviceInfoInit(0, &psInst->sDevInfo);

    //
    // Plug in the client's string stable to the device information
    // structure.
    //
    psInst->sDevInfo.ppui8StringDescriptors =
                                        psCDCDevice->ppui8StringDescriptors;
    psInst->sDevInfo.ui32NumStringDescriptors =
                                        psCDCDevice->ui32NumStringDescriptors;

    //
    // Initialize the USB tick module, this will prevent it from being
    // initialized later in the call to USBDCDInit();
    //
    InternalUSBTickInit();

    //
    // Register our tick handler (this must be done after USBDCDInit).
    //
    InternalUSBRegisterTickHandler(CDCTickHandler, (void *)psCDCDevice);

    //
    // Return the pointer to the instance indicating that everything went well.
    //
    return((void *)psCDCDevice);
}

//*****************************************************************************
//
//! Initializes CDC device operation for a given USB controller.
//!
//! \param ui32Index is the index of the USB controller which is to be
//! initialized for CDC device operation.
//! \param psCDCDevice points to a structure containing parameters customizing
//! the operation of the CDC device.
//!
//! An application wishing to make use of a USB CDC communication channel and
//! appear as a virtual serial port on the host system must call this function
//! to initialize the USB controller and attach the device to the USB bus.
//! This function performs all required USB initialization.
//!
//! The value returned by this function is the \e psCDCDevice pointer passed
//! to it if successful.  This pointer must be passed to all later calls to the
//! CDC class driver to identify the device instance.
//!
//! The USB CDC device class driver  offers packet-based transmit and receive
//! operation.  If the application would rather use block based communication
//! with transmit and receive buffers, USB buffers on the transmit and receive
//! channels may be used to offer this functionality.
//!
//! Transmit Operation:
//!
//! Calls to USBDCDCPacketWrite() must send no more than 64 bytes of data at a
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
//! callback with event \b USB_EVENT_RX_AVAILABLE.  The application must then
//! call USBDCDCPacketRead(), passing a buffer capable of holding the received
//! packet to retrieve the data and acknowledge reception to the USB host.  The
//! size of the received packet may be queried by calling
//! USBDCDCRxPacketAvailable().
//!
//! \note The application must not make any calls to the low level USB Device
//! API if interacting with USB via the CDC device class API.  Doing so
//! will cause unpredictable (though almost certainly unpleasant) behavior.
//!
//! \return Returns NULL on failure or the psCDCDevice pointer on success.
//
//*****************************************************************************
void *
USBDCDCInit(uint32_t ui32Index, tUSBDCDCDevice *psCDCDevice)
{
    void *pvRet;
    tCDCSerInstance *psInst;
    tDeviceDescriptor *psDevDesc;
    tConfigDescriptor *psConfigDesc;

    //
    // Initialize the internal state for this class.
    //
    pvRet = USBDCDCCompositeInit(ui32Index, psCDCDevice, 0);

    if(pvRet)
    {
        //
        // Fix up the device descriptor with the client-supplied values.
        //
        psDevDesc = (tDeviceDescriptor *)g_pui8CDCSerDeviceDescriptor;
        psDevDesc->idVendor = psCDCDevice->ui16VID;
        psDevDesc->idProduct = psCDCDevice->ui16PID;

        //
        // Fix up the configuration descriptor with client-supplied values.
        //
        psConfigDesc = (tConfigDescriptor *)g_pui8CDCSerDescriptor;
        psConfigDesc->bmAttributes = psCDCDevice->ui8PwrAttributes;
        psConfigDesc->bMaxPower = (uint8_t)(psCDCDevice->ui16MaxPowermA / 2);

        //
        // Create an instance pointer to the private data area.
        //
        psInst = &psCDCDevice->sPrivateData;

        //
        // Enable the default interrupt control endpoint if this class is not
        // being used in a composite device.
        //
        psInst->ui8ControlEndpoint = CONTROL_ENDPOINT;

        //
        // Use the configuration descriptor with the interrupt control
        // endpoint.
        //
        psInst->sDevInfo.ppsConfigDescriptors = g_ppCDCSerConfigDescriptors;

        //
        // All is well so now pass the descriptors to the lower layer and put
        // the CDC device on the bus.
        //
        USBDCDInit(ui32Index, &psInst->sDevInfo, (void *)psCDCDevice);
    }

    return(pvRet);
}

//*****************************************************************************
//
//! Shuts down the CDC device instance.
//!
//! \param pvCDCDevice is the pointer to the device instance structure as
//! returned by USBDCDCInit().
//!
//! This function terminates CDC operation for the instance supplied and
//! removes the device from the USB bus.  This function should not be called
//! if the CDC device is part of a composite device and instead the
//! USBDCompositeTerm() function should be called for the full composite
//! device.
//!
//! Following this call, the \e pvCDCDevice instance should not me used in
//! any other calls.
//!
//! \return None.
//
//*****************************************************************************
void
USBDCDCTerm(void *pvCDCDevice)
{
    tCDCSerInstance *psInst;

    ASSERT(pvCDCDevice);

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &((tUSBDCDCDevice *)pvCDCDevice)->sPrivateData;

    //
    // Terminate the requested instance.
    //
    USBDCDTerm(USBBaseToIndex(psInst->ui32USBBase));

    psInst->ui32USBBase = 0;

    return;
}

//*****************************************************************************
//
//! Sets the client-specific pointer for the control callback.
//!
//! \param pvCDCDevice is the pointer to the device instance structure as
//! returned by USBDCDCInit().
//! \param pvCBData is the pointer that client wishes to be provided on each
//! event sent to the control channel callback function.
//!
//! The client uses this function to change the callback pointer passed in
//! the first parameter on all callbacks to the \e pfnControlCallback function
//! passed on USBDCDCInit().
//!
//! If a client wants to make runtime changes in the callback pointer, it must
//! ensure that the psCDCDevice structure passed to USBDCDCInit() resides in
//! RAM.  If this structure is in flash, callback pointer changes will not be
//! possible.
//!
//! \return Returns the previous callback pointer that was being used for
//! this instance's control callback.
//
//*****************************************************************************
void *
USBDCDCSetControlCBData(void *pvCDCDevice, void *pvCBData)
{
    tUSBDCDCDevice *psBulkDevice;
    void *pvOldValue;

    ASSERT(pvCDCDevice);

    //
    // The CDC device structure pointer.
    //
    psBulkDevice = (tUSBDCDCDevice *)pvCDCDevice;

    //
    // Set the callback pointer for the control channel after remembering the
    // previous value.
    //
    pvOldValue = psBulkDevice->pvControlCBData;
    psBulkDevice->pvControlCBData = pvCBData;

    //
    // Return the previous callback data value.
    //
    return(pvOldValue);
}

//*****************************************************************************
//
//! Sets the client-specific data parameter for the receive channel callback.
//!
//! \param pvCDCDevice is the pointer to the device instance structure as
//! returned by USBDCDCInit().
//! \param pvCBData is the pointer that client wishes to be provided on each
//! event sent to the receive channel callback function.
//!
//! The client uses this function to change the callback pointer passed in
//! the first parameter on all callbacks to the \e pfnRxCallback function
//! passed on USBDCDCInit().
//!
//! If a client wants to make runtime changes in the callback pointer, it must
//! ensure that the psCDCDevice structure passed to USBDCDCInit() resides in
//! RAM.  If this structure is in flash, callback data changes will not be
//! possible.
//!
//! \return Returns the previous callback pointer that was being used for
//! this instance's receive callback.
//
//*****************************************************************************
void *
USBDCDCSetRxCBData(void *pvCDCDevice, void *pvCBData)
{
    tUSBDCDCDevice *psBulkDevice;
    void *pvOldValue;

    ASSERT(pvCDCDevice);

    //
    // The CDC device structure pointer.
    //
    psBulkDevice = (tUSBDCDCDevice *)pvCDCDevice;

    //
    // Set the callback data for the receive channel after remembering the
    // previous value.
    //
    pvOldValue = psBulkDevice->pvRxCBData;
    psBulkDevice->pvRxCBData = pvCBData;

    //
    // Return the previous callback pointer.
    //
    return(pvOldValue);
}

//*****************************************************************************
//
//! Sets the client-specific data parameter for the transmit callback.
//!
//! \param pvCDCDevice is the pointer to the device instance structure as
//! returned by USBDCDCInit().
//! \param pvCBData is the pointer that client wishes to be provided on each
//! event sent to the transmit channel callback function.
//!
//! The client uses this function to change the callback pointer passed in
//! the first parameter on all callbacks to the \e pfnTxCallback function
//! passed on USBDCDCInit().
//!
//! If a client wants to make runtime changes in the callback pointer, it must
//! ensure that the psCDCDevice structure passed to USBDCDCInit() resides in
//! RAM.  If this structure is in flash, callback data changes will not be
//! possible.
//!
//! \return Returns the previous callback pointer that was being used for
//! this instance's transmit callback.
//
//*****************************************************************************
void *
USBDCDCSetTxCBData(void *pvCDCDevice, void *pvCBData)
{
    tUSBDCDCDevice *psBulkDevice;
    void *pvOldValue;

    ASSERT(pvCDCDevice);

    //
    // The CDC device structure pointer.
    //
    psBulkDevice = (tUSBDCDCDevice *)pvCDCDevice;

    //
    // Set the callback data for the transmit channel after remembering the
    // previous value.
    //
    pvOldValue = psBulkDevice->pvTxCBData;
    psBulkDevice->pvTxCBData = pvCBData;

    //
    // Return the previous callback pointer.
    //
    return(pvOldValue);
}

//*****************************************************************************
//
//! Transmits a packet of data to the USB host via the CDC data interface.
//!
//! \param pvCDCDevice is the pointer to the device instance structure as
//! returned by USBDCDCInit().
//! \param pi8Data points to the first byte of data which is to be transmitted.
//! \param ui32Length is the number of bytes of data to transmit.
//! \param bLast indicates whether more data is to be written before a packet
//! should be scheduled for transmission.  If \b true, the client will make
//! a further call to this function.  If \b false, no further call will be
//! made and the driver should schedule transmission of a short packet.
//!
//! This function schedules the supplied data for transmission to the USB
//! host in a single USB packet.  If no transmission is currently ongoing
//! the data is immediately copied to the relevant USB endpoint FIFO.  If the
//! \e bLast parameter is \b true, the newly written packet is then scheduled
//! for transmission.  Whenever a USB packet is acknowledged by the host, a
//! \b USB_EVENT_TX_COMPLETE event will be sent to the application transmit
//! callback indicating that more data can now be transmitted.
//!
//! The maximum value for \e ui32Length is 64 bytes (the maximum USB packet
//! size for the bulk endpoints in use by CDC).  Attempts to send more data
//! than this will result in a return code of 0 indicating that the data cannot
//! be sent.
//!
//! \return Returns the number of bytes actually sent.  At this level, this
//! will either be the number of bytes passed (if less than or equal to the
//! maximum packet size for the USB endpoint in use and no outstanding
//! transmission ongoing) or 0 to indicate a failure.
//
//*****************************************************************************
uint32_t
USBDCDCPacketWrite(void *pvCDCDevice, uint8_t *pi8Data, uint32_t ui32Length,
                   bool bLast)
{
    tCDCSerInstance *psInst;
    int32_t i32Retcode;

    ASSERT(pvCDCDevice);

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &((tUSBDCDCDevice *)pvCDCDevice)->sPrivateData;

    //
    // Can we send the data provided?
    //
    if((ui32Length > DATA_IN_EP_MAX_SIZE) ||
       (psInst->iCDCTxState != eCDCStateIdle))
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
                                        psInst->ui8BulkINEndpoint, pi8Data,
                                        ui32Length);

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
            psInst->iCDCTxState = eCDCStateWaitData;
            i32Retcode = MAP_USBEndpointDataSend(psInst->ui32USBBase,
                                                 psInst->ui8BulkINEndpoint,
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
//! Reads a packet of data received from the USB host via the CDC data
//! interface.
//!
//! \param pvCDCDevice is the pointer to the device instance structure as
//! returned by USBDCDCInit().
//! \param pi8Data points to a buffer into which the received data will be
//! written.
//! \param ui32Length is the size of the buffer pointed to by \e pi8Data.
//! \param bLast indicates whether the client will make a further call to
//! read additional data from the packet.
//!
//! This function reads up to ui32Length bytes of data received from the USB
//! host into the supplied application buffer.
//!
//! \note The \e bLast parameter is ignored in this implementation since the
//! end of a packet can be determined without relying upon the client to
//! provide this information.
//!
//! \return Returns the number of bytes of data read.
//
//*****************************************************************************
uint32_t
USBDCDCPacketRead(void *pvCDCDevice, uint8_t *pi8Data, uint32_t ui32Length,
                  bool bLast)
{
    uint32_t ui32EPStatus, ui32Count, ui32Pkt;
    tCDCSerInstance *psInst;
    int32_t i32Retcode;

    ASSERT(pvCDCDevice);

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &((tUSBDCDCDevice *)pvCDCDevice)->sPrivateData;

    //
    // Does the relevant endpoint FIFO have a packet waiting for us?
    //
    ui32EPStatus = MAP_USBEndpointStatus(psInst->ui32USBBase,
                                         psInst->ui8BulkOUTEndpoint);

    if(ui32EPStatus & USB_DEV_RX_PKT_RDY)
    {
        //
        // If receive is currently blocked or the buffer we were passed is
        // (potentially) too small, set the flag telling us that we have a
        // packet waiting but return 0.
        //
        if(psInst->bRxBlocked || psInst->bControlBlocked)
        {
            SetDeferredOpFlag(&psInst->ui16DeferredOpFlags, CDC_DO_PACKET_RX,
                              true);
            return(0);
        }
        else
        {
            //
            // It is OK to receive the new packet.  How many bytes are
            // available for us to receive?
            //
            ui32Pkt = MAP_USBEndpointDataAvail(psInst->ui32USBBase,
                                               psInst->ui8BulkOUTEndpoint);

            //
            // Get as much data as we can.
            //
            ui32Count = ui32Length;
            i32Retcode = MAP_USBEndpointDataGet(psInst->ui32USBBase,
                                                psInst->ui8BulkOUTEndpoint,
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
                                              psInst->ui8BulkOUTEndpoint,
                                              ui32EPStatus);

                //
                // Acknowledge the data, thus freeing the host to send the
                // next packet.
                //
                MAP_USBDevEndpointDataAck(psInst->ui32USBBase,
                                          psInst->ui8BulkOUTEndpoint,
                                          true);

                //
                // Clear the flag we set to indicate that a packet read is
                // pending.
                //
                SetDeferredOpFlag(&psInst->ui16DeferredOpFlags,
                                  CDC_DO_PACKET_RX, false);

            }

            //
            // If all went well, tell the caller how many bytes they got.
            //
            if(i32Retcode != -1)
            {
                return(ui32Count);
            }
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
//! \param pvCDCDevice is the pointer to the device instance structure as
//! returned by USBDCDCInit().
//!
//! This function returns the maximum number of bytes that can be passed on a
//! call to USBDCDCPacketWrite() and accepted for transmission.  The value
//! returned will be the maximum USB packet size if no transmission is
//! currently outstanding or 0 if a transmission is in progress.
//!
//! \return Returns the number of bytes available in the transmit buffer.
//
//*****************************************************************************
uint32_t
USBDCDCTxPacketAvailable(void *pvCDCDevice)
{
    tCDCSerInstance *psInst;

    ASSERT(pvCDCDevice);

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &((tUSBDCDCDevice *)pvCDCDevice)->sPrivateData;

    //
    // Do we have a packet transmission currently ongoing?
    //
    if(psInst->iCDCTxState != eCDCStateIdle)
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
//! \param pvCDCDevice is the pointer to the device instance structure as
//! returned by USBDCDCInit().
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
USBDCDCRxPacketAvailable(void *pvCDCDevice)
{
    uint32_t ui32EPStatus, ui32Size;
    tCDCSerInstance *psInst;

    ASSERT(pvCDCDevice);

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &((tUSBDCDCDevice *)pvCDCDevice)->sPrivateData;

    //
    // If receive is currently blocked, return 0.
    //
    if(psInst->bRxBlocked || psInst->bControlBlocked)
    {
        return(0);
    }

    //
    // Does the relevant endpoint FIFO have a packet waiting for us?
    //
    ui32EPStatus = MAP_USBEndpointStatus(psInst->ui32USBBase,
                                         psInst->ui8BulkOUTEndpoint);

    if(ui32EPStatus & USB_DEV_RX_PKT_RDY)
    {
        //
        // Yes - a packet is waiting.  How big is it?
        //
        ui32Size = MAP_USBEndpointDataAvail(psInst->ui32USBBase,
                                            psInst->ui8BulkOUTEndpoint);

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
//! Informs the CDC module of changes in the serial control line states or
//! receive error conditions.
//!
//! \param pvCDCDevice is the pointer to the device instance structure as
//! returned by USBDCDCInit().
//! \param ui16State indicates the states of the various control lines and
//! any receive errors detected.  Bit definitions are as for the USB CDC
//! SerialState asynchronous notification and are defined in header file
//! usbcdc.h.
//!
//! The application should call this function whenever the state of any of
//! the incoming RS232 handshake signals changes or in response to a receive
//! error or break condition.  The \e ui16State parameter is the ORed
//! combination of the following flags with each flag indicating the presence
//! of that condition.
//!
//! - USB_CDC_SERIAL_STATE_OVERRUN
//! - USB_CDC_SERIAL_STATE_PARITY
//! - USB_CDC_SERIAL_STATE_FRAMING
//! - USB_CDC_SERIAL_STATE_RING_SIGNAL
//! - USB_CDC_SERIAL_STATE_BREAK
//! - USB_CDC_SERIAL_STATE_TXCARRIER
//! - USB_CDC_SERIAL_STATE_RXCARRIER
//!
//! This function should be called only when the state of any flag changes.
//!
//! \return None.
//
//*****************************************************************************
void
USBDCDCSerialStateChange(void *pvCDCDevice, uint16_t ui16State)
{
    tCDCSerInstance *psInst;

    ASSERT(pvCDCDevice);

    //
    // Get a pointer to the CDC device instance data pointer
    //
    psInst = &((tUSBDCDCDevice *)pvCDCDevice)->sPrivateData;

    //
    // Add the newly reported state bits to the current collection.  We do this
    // in case two state changes occur back-to-back before the first has been
    // notified.  There are two distinct types of signals that we report here
    // and we deal with them differently:
    //
    // 1.  Errors (overrun, parity, framing error) are ORed together so that
    //     any reported error is sent on the next notification.
    // 2.  Signal line states (RI, break, TX carrier, RX carrier) always
    //     report the last state notified to us.  The implementation here will
    //     send an interrupt showing the last state but, if two state changes
    //     occur very quickly, the host may receive a notification containing
    //     the same state that was last reported (in other words, a short pulse
    //     will be lost).  It would be possible to reduce the likelihood of
    //     this happening by building a queue of state changes and sending
    //     these in order but you are left with exactly the same problem if the
    //     queue fills up.  For now, therefore, we run the risk of missing very
    //     short pulses on the "steady-state" signal lines.
    //
    psInst->ui16SerialState |= (ui16State & USB_CDC_SERIAL_ERRORS);
    psInst->ui16SerialState &= ~USB_CDC_SERIAL_ERRORS;
    psInst->ui16SerialState |= (ui16State & ~USB_CDC_SERIAL_ERRORS);

    //
    // Set the flag indicating that a serial state change is to be sent.
    //
    SetDeferredOpFlag(&psInst->ui16DeferredOpFlags, CDC_DO_SERIAL_STATE_CHANGE,
                      true);

    //
    // Can we send the state change immediately?
    //
    if(psInst->iCDCInterruptState == eCDCStateIdle)
    {
        //
        // The interrupt channel is free so send the notification immediately.
        // If we can't do this, the tick timer will catch this next time
        // round.
        //
        psInst->iCDCInterruptState = eCDCStateWaitData;
        SendSerialState(pvCDCDevice);
    }

    return;
}

//*****************************************************************************
//
//! Reports the device power status (bus- or self-powered) to the USB library.
//!
//! \param pvCDCDevice is the pointer to the CDC device instance structure.
//! \param ui8Power indicates the current power status, either \b
//! USB_STATUS_SELF_PWR or \b USB_STATUS_BUS_PWR.
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
USBDCDCPowerStatusSet(void *pvCDCDevice, uint8_t ui8Power)
{
    ASSERT(pvCDCDevice);

    //
    // Pass the request through to the lower layer.
    //
    USBDCDPowerStatusSet(0, ui8Power);
}

//*****************************************************************************
//
//! Requests a remote wakeup to resume communication when in suspended state.
//!
//! \param pvCDCDevice is the pointer to the CDC device instance structure.
//!
//! When the bus is suspended, an application which supports remote wakeup
//! (advertised to the host via the configuration descriptor) may call this
//! function to initiate remote wakeup signaling to the host.  If the remote
//! wakeup feature has not been disabled by the host, this will cause the bus
//! to resume operation within 20mS.  If the host has disabled remote wakeup,
//! \b false will be returned to indicate that the wakeup request was not
//! successful.
//!
//! \return Returns \b true if the remote wakeup is not disabled and the
//! signaling was started or \b false if remote wakeup is disabled or if
//! signaling is currently ongoing following a previous call to this function.
//
//*****************************************************************************
bool
USBDCDCRemoteWakeupRequest(void *pvCDCDevice)
{
    ASSERT(pvCDCDevice);

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
