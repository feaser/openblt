//*****************************************************************************
//
// usbdhidmouse.c - USB HID Mouse device class driver.
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
#include "driverlib/debug.h"
#include "driverlib/usbdrv.h"
#include "usblib/usblib.h"
#include "usblib/usblibpriv.h"
#include "usblib/device/usbdevice.h"
#include "usblib/usbhid.h"
#include "usblib/device/usbdhid.h"
#include "usblib/device/usbdhidmouse.h"

//*****************************************************************************
//
//! \addtogroup hid_mouse_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// HID device configuration descriptor.
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
uint8_t g_pui8MouseDescriptor[] =
{
    //
    // Configuration descriptor header.
    //
    9,                          // Size of the configuration descriptor.
    USB_DTYPE_CONFIGURATION,    // Type of this descriptor.
    USBShort(34),               // The total size of this full structure.
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
uint8_t g_pui8HIDInterface[HIDINTERFACE_SIZE] =
{
    //
    // HID Device Class Interface Descriptor.
    //
    9,                          // Size of the interface descriptor.
    USB_DTYPE_INTERFACE,        // Type of this descriptor.
    0,                          // The index for this interface.
    0,                          // The alternate setting for this interface.
    1,                          // The number of endpoints used by this
                                // interface.
    USB_CLASS_HID,              // The interface class
    USB_HID_SCLASS_BOOT,        // The interface sub-class.
    USB_HID_PROTOCOL_MOUSE,     // The interface protocol for the sub-class
                                // specified above.
    4,                          // The string index for this interface.
};

const uint8_t g_pui8HIDInEndpoint[HIDINENDPOINT_SIZE] =
{
    //
    // Interrupt IN endpoint descriptor
    //
    7,                          // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,         // Descriptor type is an endpoint.
    USB_EP_DESC_IN | USBEPToIndex(USB_EP_1),
    USB_EP_ATTR_INT,            // Endpoint is an interrupt endpoint.
    USBShort(USBFIFOSizeToBytes(USB_FIFO_SZ_64)),
                                // The maximum packet size.
    16,                         // The polling interval for this endpoint.
};

//*****************************************************************************
//
// The report descriptor for the mouse class device.
//
//*****************************************************************************
static const uint8_t g_pui8MouseReportDescriptor[] =
{
    UsagePage(USB_HID_GENERIC_DESKTOP),
    Usage(USB_HID_MOUSE),
    Collection(USB_HID_APPLICATION),
        Usage(USB_HID_POINTER),
        Collection(USB_HID_PHYSICAL),

            //
            // The buttons.
            //
            UsagePage(USB_HID_BUTTONS),
            UsageMinimum(1),
            UsageMaximum(3),
            LogicalMinimum(0),
            LogicalMaximum(1),

            //
            // 3 - 1 bit values for the buttons.
            //
            ReportSize(1),
            ReportCount(3),
            Input(USB_HID_INPUT_DATA | USB_HID_INPUT_VARIABLE |
                  USB_HID_INPUT_ABS),

            //
            // 1 - 5 bit unused constant value to fill the 8 bits.
            //
            ReportSize(5),
            ReportCount(1),
            Input(USB_HID_INPUT_CONSTANT | USB_HID_INPUT_ARRAY |
                  USB_HID_INPUT_ABS),

            //
            // The X and Y axis.
            //
            UsagePage(USB_HID_GENERIC_DESKTOP),
            Usage(USB_HID_X),
            Usage(USB_HID_Y),
            LogicalMinimum(-127),
            LogicalMaximum(127),

            //
            // 2 - 8 bit Values for x and y.
            //
            ReportSize(8),
            ReportCount(2),
            Input(USB_HID_INPUT_DATA | USB_HID_INPUT_VARIABLE |
                  USB_HID_INPUT_RELATIVE),

            //
            // 2 - 8 bit Values for x and y.  // Padding
            //
            ReportSize(8),
            ReportCount(MOUSE_REPORT_SIZE - 3),
            Input(USB_HID_INPUT_CONSTANT | USB_HID_INPUT_ARRAY |
                  USB_HID_INPUT_ABS),

        EndCollection,
    EndCollection,
};

//*****************************************************************************
//
// The HID descriptor for the mouse device.
//
//*****************************************************************************
static const tHIDDescriptor g_sMouseHIDDescriptor =
{
    9,                              // bLength
    USB_HID_DTYPE_HID,              // bDescriptorType
    0x111,                          // bcdHID (version 1.11 compliant)
    0,                              // bCountryCode (not localized)
    1,                              // bNumDescriptors
    {
        {
            USB_HID_DTYPE_REPORT,   // Report descriptor
            sizeof(g_pui8MouseReportDescriptor)
                                    // Size of report descriptor
        }
    }
};

//*****************************************************************************
//
// The HID configuration descriptor is defined as four or five sections
// depending upon the client's configuration choice.  These sections are:
//
// 1.  The 9 byte configuration descriptor (RAM).
// 2.  The interface descriptor (RAM).
// 3.  The HID report and physical descriptors (provided by the client)
//     (FLASH).
// 4.  The mandatory interrupt IN endpoint descriptor (FLASH).
// 5.  The optional interrupt OUT endpoint descriptor (FLASH).
//
//*****************************************************************************
const tConfigSection g_sHIDConfigSection =
{
    sizeof(g_pui8MouseDescriptor),
    g_pui8MouseDescriptor
};

const tConfigSection g_sHIDInterfaceSection =
{
    sizeof(g_pui8HIDInterface),
    g_pui8HIDInterface
};

const tConfigSection g_sHIDInEndpointSection =
{
    sizeof(g_pui8HIDInEndpoint),
    g_pui8HIDInEndpoint
};

//*****************************************************************************
//
// Place holder for the user's HID descriptor block.
//
//*****************************************************************************
tConfigSection g_sHIDDescriptorSection =
{
   sizeof(g_sMouseHIDDescriptor),
   (const uint8_t *)&g_sMouseHIDDescriptor
};

//*****************************************************************************
//
// This array lists all the sections that must be concatenated to make a
// single, complete HID configuration descriptor.
//
//*****************************************************************************
const tConfigSection *g_psHIDSections[] =
{
    &g_sHIDConfigSection,
    &g_sHIDInterfaceSection,
    &g_sHIDDescriptorSection,
    &g_sHIDInEndpointSection,
};

#define NUM_HID_SECTIONS        (sizeof(g_psHIDSections) /                    \
                                 sizeof(g_psHIDSections[0]))

//*****************************************************************************
//
// The header for the single configuration we support.  This is the root of
// the data structure that defines all the bits and pieces that are pulled
// together to generate the configuration descriptor.  Note that this must be
// in RAM since we need to include or exclude the final section based on
// client supplied initialization parameters.
//
//*****************************************************************************
tConfigHeader g_sHIDConfigHeader =
{
    NUM_HID_SECTIONS,
    g_psHIDSections
};

//*****************************************************************************
//
// Configuration Descriptor.
//
//*****************************************************************************
const tConfigHeader * const g_ppsHIDConfigDescriptors[] =
{
    &g_sHIDConfigHeader
};

//*****************************************************************************
//
// The HID class descriptor table.  For the mouse class, we have only a single
// report descriptor.
//
//*****************************************************************************
static const uint8_t * const g_pui8MouseClassDescriptors[] =
{
    g_pui8MouseReportDescriptor
};

//*****************************************************************************
//
// Forward references for mouse device callback functions.
//
//*****************************************************************************
static uint32_t HIDMouseRxHandler(void *pvMouseDevice, uint32_t ui32Event,
                                  uint32_t ui32MsgData, void *pvMsgData);
static uint32_t HIDMouseTxHandler(void *pvMouseDevice, uint32_t ui32Event,
                                  uint32_t ui32MsgData, void *pvMsgData);

//*****************************************************************************
//
// The HID mouse report offsets for this mouse application.
//
//*****************************************************************************
#define HID_REPORT_BUTTONS      0
#define HID_REPORT_X            1
#define HID_REPORT_Y            2

//*****************************************************************************
//
// Main HID device class event handler function.
//
// \param pvMouseDevice is the event callback pointer provided during
// USBDHIDInit().  This is a pointer to our HID device structure
// (&g_sHIDMouseDevice).
// \param ui32Event identifies the event we are being called back for.
// \param ui32MsgData is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the HID device class driver to inform the
// application of particular asynchronous events related to operation of the
// mouse HID device.
//
// \return Returns a value which is event-specific.
//
//*****************************************************************************
static uint32_t
HIDMouseRxHandler(void *pvMouseDevice, uint32_t ui32Event,
                  uint32_t ui32MsgData, void *pvMsgData)
{
    tHIDMouseInstance *psInst;
    tUSBDHIDMouseDevice *psMouseDevice;

    //
    // Make sure we did not get a NULL pointer.
    //
    ASSERT(pvMouseDevice);

    //
    // Get a pointer to our instance data
    //
    psMouseDevice = (tUSBDHIDMouseDevice *)pvMouseDevice;
    psInst = &psMouseDevice->sPrivateData;

    //
    // Which event were we sent?
    //
    switch(ui32Event)
    {
        //
        // The host has connected to us and configured the device.
        //
        case USB_EVENT_CONNECTED:
        {
            psInst->ui8USBConfigured = true;

            //
            // Pass the information on to the client.
            //
            psMouseDevice->pfnCallback(psMouseDevice->pvCBData,
                                       USB_EVENT_CONNECTED, 0, (void *)0);

            break;
        }

        //
        // The host has disconnected from us.
        //
        case USB_EVENT_DISCONNECTED:
        {
            psInst->ui8USBConfigured = false;

            //
            // Pass the information on to the client.
            //
            psMouseDevice->pfnCallback(psMouseDevice->pvCBData,
                                       USB_EVENT_DISCONNECTED, 0, (void *)0);

            break;
        }

        //
        // The host is polling us for a particular report and the HID driver
        // is asking for the latest version to transmit.
        //
        case USBD_HID_EVENT_IDLE_TIMEOUT:
        case USBD_HID_EVENT_GET_REPORT:
        {
            //
            // We only support a single input report so we don't need to check
            // the ui32MsgValue parameter in this case.  Set the report pointer
            // in *pvMsgData and return the length of the report in bytes.
            //
            *(uint8_t **)pvMsgData = psInst->pui8Report;
            return(8);
        }

        //
        // The device class driver has completed sending a report to the
        // host in response to a Get_Report request.
        //
        case USBD_HID_EVENT_REPORT_SENT:
        {
            //
            // We have nothing to do here.
            //
            break;
        }

        //
        // This event is sent in response to a host Set_Report request.  The
        // mouse device has no output reports so we return a NULL pointer and
        // zero length to cause this request to be stalled.
        //
        case USBD_HID_EVENT_GET_REPORT_BUFFER:
        {
            //
            // We are being asked for a report that does not exist for
            // this device.  Return 0 to indicate that we are not providing
            // a buffer.
            //
            return(0);
        }

        //
        // The host is asking us to set either boot or report protocol (not
        // that it makes any difference to this particular mouse).
        //
        case USBD_HID_EVENT_SET_PROTOCOL:
        {
            psInst->ui8Protocol = ui32MsgData;
            break;
        }

        //
        // The host is asking us to tell it which protocol we are currently
        // using, boot or request.
        //
        case USBD_HID_EVENT_GET_PROTOCOL:
        {
            return(psInst->ui8Protocol);
        }

        //
        // Pass ERROR, SUSPEND and RESUME to the client unchanged.
        //
        case USB_EVENT_ERROR:
        case USB_EVENT_SUSPEND:
        case USB_EVENT_RESUME:
        {
            return(psMouseDevice->pfnCallback(psMouseDevice->pvCBData,
                                              ui32Event, ui32MsgData,
                                              pvMsgData));
        }

        //
        // We ignore all other events.
        //
        default:
        {
            break;
        }
    }
    return(0);
}

//*****************************************************************************
//
// HID device class transmit channel event handler function.
//
// \param pvMouseDevice is the event callback pointer provided during
// USBDHIDInit(). This is a pointer to our HID device structure
// (&g_sHIDMouseDevice).
// \param ui32Event identifies the event we are being called back for.
// \param ui32MsgData is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the HID device class driver to inform the
// application of particular asynchronous events related to report
// transmissions made using the interrupt IN endpoint.
//
// \return Returns a value which is event-specific.
//
//*****************************************************************************
static uint32_t
HIDMouseTxHandler(void *pvMouseDevice, uint32_t ui32Event,
                  uint32_t ui32MsgData, void *pvMsgData)
{
    tHIDMouseInstance *psInst;
    tUSBDHIDMouseDevice *psMouseDevice;

    //
    // Make sure we did not get a NULL pointer.
    //
    ASSERT(pvMouseDevice);

    //
    // Get a pointer to our instance data
    //
    psMouseDevice = (tUSBDHIDMouseDevice *)pvMouseDevice;
    psInst = &psMouseDevice->sPrivateData;

    //
    // Which event were we sent?
    //
    switch (ui32Event)
    {
        //
        // A report transmitted via the interrupt IN endpoint was acknowledged
        // by the host.
        //
        case USB_EVENT_TX_COMPLETE:
        {
            //
            // Our last transmission is complete.
            //
            psInst->iMouseState = eHIDMouseStateIdle;

            //
            // Pass the event on to the client.
            //
            psMouseDevice->pfnCallback(psMouseDevice->pvCBData,
                                       USB_EVENT_TX_COMPLETE, ui32MsgData,
                                       (void *)0);

            break;
        }

        //
        // We ignore all other events related to transmission of reports via
        // the interrupt IN endpoint.
        //
        default:
        {
            break;
        }
    }

    return(0);
}

//*****************************************************************************
//
//! Initializes HID mouse device operation for a given USB controller.
//!
//! \param ui32Index is the index of the USB controller which is to be
//! initialized for HID mouse device operation.
//! \param psMouseDevice points to a structure containing parameters
//! customizing the operation of the HID mouse device.
//!
//! An application wishing to offer a USB HID mouse interface to a USB host
//! must call this function to initialize the USB controller and attach the
//! mouse device to the USB bus.  This function performs all required USB
//! initialization.
//!
//! On successful completion, this function will return the \e psMouseDevice
//! pointer passed to it.  This must be passed on all future calls to the HID
//! mouse device driver.
//!
//! When a host connects and configures the device, the application callback
//! will receive \b USB_EVENT_CONNECTED after which calls can be made to
//! USBDHIDMouseStateChange() to report pointer movement and button presses
//! to the host.
//!
//! \note The application must not make any calls to the lower level USB device
//! interfaces if interacting with USB via the USB HID mouse device API.
//! Doing so will cause unpredictable (though almost certainly unpleasant)
//! behavior.
//!
//! \return Returns NULL on failure or the psMouseDevice pointer on success.
//
//*****************************************************************************
void *
USBDHIDMouseInit(uint32_t ui32Index, tUSBDHIDMouseDevice *psMouseDevice)
{
    void *pvRetcode;
    tUSBDHIDDevice *psHIDDevice;
    tConfigDescriptor *pConfigDesc;

    //
    // Check parameter validity.
    //
    ASSERT(psMouseDevice);
    ASSERT(psMouseDevice->ppui8StringDescriptors);
    ASSERT(psMouseDevice->pfnCallback);

    //
    // Get a pointer to the HID device data.
    //
    psHIDDevice = &psMouseDevice->sPrivateData.sHIDDevice;

    pConfigDesc = (tConfigDescriptor *)g_pui8MouseDescriptor;
    pConfigDesc->bmAttributes = psMouseDevice->ui8PwrAttributes;
    pConfigDesc->bMaxPower =  (uint8_t)(psMouseDevice->ui16MaxPowermA / 2);

    //
    // Call the common initialization routine.
    //
    pvRetcode = USBDHIDMouseCompositeInit(ui32Index, psMouseDevice, 0);

    //
    // If we initialized the HID layer successfully, pass our device pointer
    // back as the return code, otherwise return NULL to indicate an error.
    //
    if(pvRetcode)
    {
        //
        // Initialize the lower layer HID driver and pass it the various
        // structures and descriptors necessary to declare that we are a
        // keyboard.
        //
        pvRetcode = USBDHIDInit(ui32Index, psHIDDevice);

        return((void *)psMouseDevice);
    }
    else
    {
        return((void *)0);
    }
}

//*****************************************************************************
//
//! Initializes HID mouse device operation for a given USB controller.
//!
//! \param ui32Index is the index of the USB controller which is to be
//! initialized for HID mouse device operation.
//! \param psMouseDevice points to a structure containing parameters
//! customizing the operation of the HID mouse device.
//! \param psCompEntry is the composite device entry to initialize when
//! creating a composite device.
//!
//! This call is very similar to USBDHIDMouseInit() except that it is used for
//! initializing an instance of the HID mouse device for use in a composite
//! device.  If this HID mouse is part of a composite device, then the
//! \e psCompEntry should point to the composite device entry to initialize.
//! This is part of the array that is passed to the USBDCompositeInit()
//! function.
//!
//! \return Returns zero on failure or a non-zero instance value that should be
//! used with the remaining USB HID Mouse APIs.
//
//*****************************************************************************
void *
USBDHIDMouseCompositeInit(uint32_t ui32Index,
                          tUSBDHIDMouseDevice *psMouseDevice,
                          tCompositeEntry *psCompEntry)
{
    tHIDMouseInstance *psInst;
    tUSBDHIDDevice *psHIDDevice;


    //
    // Check parameter validity.
    //
    ASSERT(psMouseDevice);
    ASSERT(psMouseDevice->ppui8StringDescriptors);
    ASSERT(psMouseDevice->pfnCallback);

    //
    // Get a pointer to our instance data
    //
    psInst = &psMouseDevice->sPrivateData;

    //
    // Get a pointer to the HID device data.
    //
    psHIDDevice = &psMouseDevice->sPrivateData.sHIDDevice;

    //
    // Initialize the various fields in our instance structure.
    //
    psInst->ui8USBConfigured = 0;
    psInst->ui8Protocol = USB_HID_PROTOCOL_REPORT;
    psInst->sReportIdle.ui8Duration4mS = 0;
    psInst->sReportIdle.ui8ReportID = 0;
    psInst->sReportIdle.ui32TimeSinceReportmS = 0;
    psInst->sReportIdle.ui16TimeTillNextmS = 0;
    psInst->iMouseState = eHIDMouseStateUnconfigured;

    //
    // Initialize the HID device class instance structure based on input from
    // the caller.
    //
    psHIDDevice->ui16PID = psMouseDevice->ui16PID;
    psHIDDevice->ui16VID = psMouseDevice->ui16VID;
    psHIDDevice->ui16MaxPowermA = psMouseDevice->ui16MaxPowermA;
    psHIDDevice->ui8PwrAttributes = psMouseDevice->ui8PwrAttributes;
    psHIDDevice->ui8Subclass = USB_HID_SCLASS_BOOT;
    psHIDDevice->ui8Protocol = USB_HID_PROTOCOL_MOUSE;
    psHIDDevice->ui8NumInputReports = 1;
    psHIDDevice->psReportIdle = &psInst->sReportIdle;
    psHIDDevice->pfnRxCallback = HIDMouseRxHandler;
    psHIDDevice->pvRxCBData = (void *)psMouseDevice;
    psHIDDevice->pfnTxCallback = HIDMouseTxHandler;
    psHIDDevice->pvTxCBData = (void *)psMouseDevice;
    psHIDDevice->bUseOutEndpoint = false;
    psHIDDevice->psHIDDescriptor = &g_sMouseHIDDescriptor;
    psHIDDevice->ppui8ClassDescriptors = g_pui8MouseClassDescriptors;
    psHIDDevice->ppui8StringDescriptors =
                                    psMouseDevice->ppui8StringDescriptors;
    psHIDDevice->ui32NumStringDescriptors =
                                    psMouseDevice->ui32NumStringDescriptors;
    psHIDDevice->ppsConfigDescriptor = g_ppsHIDConfigDescriptors;

    //
    // Initialize the lower layer HID driver and pass it the various structures
    // and descriptors necessary to declare that we are a keyboard.
    //
    return(USBDHIDCompositeInit(ui32Index, psHIDDevice, psCompEntry));
}

//*****************************************************************************
//
//! Shuts down the HID mouse device.
//!
//! \param pvMouseDevice is the pointer to the device instance structure.
//!
//! This function terminates HID mouse operation for the instance supplied
//! and removes the device from the USB bus.  Following this call, the
//! \e pvMouseDevice instance may not me used in any other call to the HID
//! mouse device other than USBDHIDMouseInit().
//!
//! \return None.
//
//*****************************************************************************
void
USBDHIDMouseTerm(void *pvMouseDevice)
{
    tUSBDHIDMouseDevice *psMouseDevice;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvMouseDevice);

    //
    // Get a pointer to the device.
    //
    psMouseDevice = (tUSBDHIDMouseDevice *)pvMouseDevice;

    //
    // Get a pointer to the HID device data.
    //
    psHIDDevice = &psMouseDevice->sPrivateData.sHIDDevice;

    //
    // Mark our device as no longer configured.
    //
    psMouseDevice->sPrivateData.ui8USBConfigured = 0;

    //
    // Terminate the low level HID driver.
    //
    USBDHIDTerm(psHIDDevice);
}

//*****************************************************************************
//
//! Sets the client-specific pointer parameter for the mouse callback.
//!
//! \param pvMouseDevice is the pointer to the mouse device instance structure.
//! \param pvCBData is the pointer that client wishes to be provided on
//! each event sent to the mouse callback function.
//!
//! The client uses this function to change the callback pointer passed in
//! the first parameter on all callbacks to the \e pfnCallback function
//! passed on USBDHIDMouseInit().
//!
//! If a client wants to make runtime changes in the callback pointer, it must
//! ensure that the pvMouseDevice structure passed to USBDHIDMouseInit()
//! resides in RAM.  If this structure is in flash, callback data changes are
//! not possible.
//!
//! \return Returns the previous callback pointer that was set for this
//! instance.
//
//*****************************************************************************
void *
USBDHIDMouseSetCBData(void *pvMouseDevice, void *pvCBData)
{
    void *pvOldCBData;
    tUSBDHIDMouseDevice *psMouse;

    //
    // Check for a NULL pointer in the device parameter.
    //
    ASSERT(pvMouseDevice);

    //
    // Get a pointer to our mouse device.
    //
    psMouse = (tUSBDHIDMouseDevice *)pvMouseDevice;

    //
    // Save the old callback pointer and replace it with the new value.
    //
    pvOldCBData = psMouse->pvCBData;
    psMouse->pvCBData = pvCBData;

    //
    // Pass the old callback pointer back to the caller.
    //
    return(pvOldCBData);
}

//*****************************************************************************
//
//! Reports a mouse state change, pointer movement or button press, to the USB
//! host.
//!
//! \param pvMouseDevice is the pointer to the mouse device instance structure.
//! \param i8DeltaX is the relative horizontal pointer movement that the
//! application wishes to report.  Valid values are in the range [-127, 127]
//! with positive values indicating movement to the right.
//! \param i8DeltaY is the relative vertical pointer movement that the
//! application wishes to report.  Valid values are in the range [-127, 127]
//! with positive values indicating downward movement.
//! \param ui8Buttons is a bit mask indicating which (if any) of the three
//! mouse buttons is pressed.  Valid values are logical OR combinations of
//! \b MOUSE_REPORT_BUTTON_1, \b MOUSE_REPORT_BUTTON_2 and
//! \b MOUSE_REPORT_BUTTON_3.
//!
//! This function is called to report changes in the mouse state to the USB
//! host.  These changes can be movement of the pointer, reported relative to
//! its previous position, or changes in the states of up to 3 buttons that
//! the mouse may support.  The return code indicates whether or not the
//! mouse report could be sent to the host.  In cases where a previous
//! report is still being transmitted, \b MOUSE_ERR_TX_ERROR will be returned
//! and the state change will be ignored.
//!
//! \return Returns \b MOUSE_SUCCESS on success, \b MOUSE_ERR_TX_ERROR if an
//! error occurred while attempting to schedule transmission of the mouse
//! report to the host (typically due to a previous report which has not yet
//! completed transmission or due to disconnection of the host) or \b
//! MOUSE_ERR_NOT_CONFIGURED if called before a host has connected to and
//! configured the device.
//
//*****************************************************************************
uint32_t
USBDHIDMouseStateChange(void *pvMouseDevice, int8_t i8DeltaX, int8_t i8DeltaY,
                        uint8_t ui8Buttons)
{
    uint32_t ui32Retcode, ui32Count;
    tHIDMouseInstance *psInst;
    tUSBDHIDMouseDevice *psMouseDevice;
    tUSBDHIDDevice *psHIDDevice;

    //
    // Get a pointer to the device.
    //
    psMouseDevice = (tUSBDHIDMouseDevice *)pvMouseDevice;

    //
    // Get a pointer to the HID device data.
    //
    psHIDDevice = &psMouseDevice->sPrivateData.sHIDDevice;

    //
    // Get a pointer to our instance data
    //
    psInst = &psMouseDevice->sPrivateData;

    //
    // Update the global mouse report with the information passed.
    //
    psInst->pui8Report[HID_REPORT_BUTTONS] = ui8Buttons;
    psInst->pui8Report[HID_REPORT_X] = (uint8_t)i8DeltaX;
    psInst->pui8Report[HID_REPORT_Y] = (uint8_t)i8DeltaY;

    //
    // If we are not configured, return an error here before trying to send
    // anything.
    //
    if(!psInst->ui8USBConfigured)
    {
        return(MOUSE_ERR_NOT_CONFIGURED);
    }

    //
    // Only send a report if the transmitter is currently free.
    //
    if(USBDHIDTxPacketAvailable((void *)psHIDDevice))
    {
        //
        // Send the report to the host.
        //
        psInst->iMouseState = eHIDMouseStateSend;
        ui32Count = USBDHIDReportWrite((void *)psHIDDevice,
                                       psInst->pui8Report, MOUSE_REPORT_SIZE,
                                       true);

        //
        // Did we schedule a packet for transmission correctly?
        //
        if(!ui32Count)
        {
            //
            // No - report the error to the caller.
            //
            ui32Retcode = MOUSE_ERR_TX_ERROR;
        }
        else
        {
            ui32Retcode = MOUSE_SUCCESS;
        }
    }
    else
    {
        ui32Retcode = MOUSE_ERR_TX_ERROR;
    }
    //
    // Return the relevant error code to the caller.
    //
    return(ui32Retcode);
}

//*****************************************************************************
//
//! Reports the device power status (bus- or self-powered) to the USB library.
//!
//! \param pvMouseDevice is the pointer to the mouse device instance structure.
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
USBDHIDMousePowerStatusSet(void *pvMouseDevice, uint8_t ui8Power)
{
    tUSBDHIDMouseDevice *psMouseDevice;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvMouseDevice);

    //
    // Get the keyboard device pointer.
    //
    psMouseDevice = (tUSBDHIDMouseDevice *)pvMouseDevice;

    //
    // Get a pointer to the HID device data.
    //
    psHIDDevice = &psMouseDevice->sPrivateData.sHIDDevice;

    //
    // Pass the request through to the lower layer.
    //
    USBDHIDPowerStatusSet((void *)psHIDDevice, ui8Power);
}

//*****************************************************************************
//
//! Requests a remote wake up to resume communication when in suspended state.
//!
//! \param pvMouseDevice is the pointer to the mouse device instance structure.
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
USBDHIDMouseRemoteWakeupRequest(void *pvMouseDevice)
{
    tUSBDHIDMouseDevice *psMouseDevice;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvMouseDevice);

    //
    // Get the keyboard device pointer.
    //
    psMouseDevice = (tUSBDHIDMouseDevice *)pvMouseDevice;

    //
    // Get a pointer to the HID device data.
    //
    psHIDDevice = &psMouseDevice->sPrivateData.sHIDDevice;

    //
    // Pass the request through to the lower layer.
    //
    return(USBDHIDRemoteWakeupRequest((void *)&psHIDDevice));
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
