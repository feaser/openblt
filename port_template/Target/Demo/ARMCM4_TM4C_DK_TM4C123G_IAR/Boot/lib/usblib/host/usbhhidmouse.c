//*****************************************************************************
//
// usbhhidmouse.c - This file holds the application interfaces for USB
//                  mouse devices.
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
#include "usblib/usblib.h"
#include "usblib/host/usbhost.h"
#include "usblib/usbhid.h"
#include "usblib/host/usbhhid.h"
#include "usblib/host/usbhhidmouse.h"

//*****************************************************************************
//
//! \addtogroup usblib_host_device
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Prototypes for local functions.
//
//*****************************************************************************
static uint32_t USBHMouseCallback(void *pvMouse, uint32_t ui32Event,
                                  uint32_t ui32MsgParam, void *pvMsgData);

//*****************************************************************************
//
// The size of a USB mouse report.
//
//*****************************************************************************
#define USBHMS_REPORT_SIZE      4

//*****************************************************************************
//
// These are the flags for the tUSBHMouse.ui32HIDFlags member variable.
//
//*****************************************************************************
#define USBHMS_DEVICE_PRESENT   0x00000001

//*****************************************************************************
//
// This is the structure definition for a mouse device instance.
//
//*****************************************************************************
struct tUSBHMouse
{
    //
    // Global flags for an instance of a mouse.
    //
    uint32_t ui32HIDFlags;

    //
    // The applications registered callback.
    //
    tUSBHIDMouseCallback pfnCallback;

    //
    // The current state of the buttons.
    //
    uint8_t ui8Buttons;

    //
    // This is a local buffer to hold the current HID report that comes up
    // from the HID driver layer.
    //
    uint8_t pui8Buffer[USBHMS_REPORT_SIZE];

    //
    // Heap data for the mouse currently used to read the HID Report
    // Descriptor.
    //
    uint8_t *pui8Heap;

    //
    // Size of the heap in bytes.
    //
    uint32_t ui32HeapSize;

    //
    // This is the instance value for the HID device that will be used for the
    // mouse.
    //
    tHIDInstance *psHIDInstance;
};

//*****************************************************************************
//
// This is the per instance information for a mouse device.
//
//*****************************************************************************
static tUSBHMouse g_sUSBHMouse =
{
    0
};

//*****************************************************************************
//
//! This function is used open an instance of a mouse.
//!
//! \param pfnCallback is the callback function to call when new events occur
//! with the mouse returned.
//! \param pui8Buffer is the memory used by the driver to interact with the
//! USB mouse.
//! \param ui32Size is the size of the buffer provided by \e pui8Buffer.
//!
//! This function is used to open an instance of the mouse.  The value
//! returned from this function should be used as the instance identifier for
//! all other USBHMouse calls.  The \e pui8Buffer memory buffer is used to
//! access the mouse.  The buffer size required is at least enough to hold
//! a normal report descriptor for the device.
//!
//! \return Returns the instance identifier for the mouse that is attached.
//! If there is no mouse present this will return 0.
//
//*****************************************************************************
tUSBHMouse *
USBHMouseOpen(tUSBHIDMouseCallback pfnCallback, uint8_t *pui8Buffer,
              uint32_t ui32Size)
{
    //
    // Save the callback and data pointers.
    //
    g_sUSBHMouse.pfnCallback = pfnCallback;

    //
    // Save the instance pointer for the HID device that was opened.
    //
    g_sUSBHMouse.psHIDInstance = USBHHIDOpen(eUSBHHIDClassMouse,
                                             USBHMouseCallback,
                                             (void *)&g_sUSBHMouse);

    //
    // Save the heap buffer and size.
    //
    g_sUSBHMouse.pui8Heap = pui8Buffer;
    g_sUSBHMouse.ui32HeapSize = ui32Size;

    return(&g_sUSBHMouse);
}

//*****************************************************************************
//
//! This function is used close an instance of a mouse.
//!
//! \param psMsInstance is the instance value for this mouse.
//!
//! This function is used to close an instance of the mouse that was opened
//! with a call to USBHMouseOpen().  The \e psMsInstance value is the value
//! that was returned when the application called USBHMouseOpen().
//!
//! \return Returns 0.
//
//*****************************************************************************
uint32_t
USBHMouseClose(tUSBHMouse *psMsInstance)
{
    //
    // Reset the callback to null.
    //
    psMsInstance->pfnCallback = 0;

    //
    // Call the HID driver layer to close out this instance.
    //
    USBHHIDClose(psMsInstance->psHIDInstance);

    return(0);
}

//*****************************************************************************
//
//! This function is used to initialize a mouse interface after a mouse has
//! been detected.
//!
//! \param psMsInstance is the instance value for this mouse.
//!
//! This function should be called after receiving a \b USB_EVENT_CONNECTED
//! event in the callback function provided by USBHMouseOpen(), however it
//! should only be called outside of the callback function.  This will
//! initialize the mouse interface and determine how it reports events to the
//! USB host controller.  The \e psMsInstance value is the value that was
//! returned when the application called USBHMouseOpen().  This function only
//! needs to be called once per connection event but it should be called every
//! time a \b USB_EVENT_CONNECTED event occurs.
//!
//! \return Non-zero values should be assumed to indicate an error condition.
//
//*****************************************************************************
uint32_t
USBHMouseInit(tUSBHMouse *psMsInstance)
{
    //
    // Set the initial rate to only update on mouse state changes.
    //
    USBHHIDSetIdle(psMsInstance->psHIDInstance, 0, 0);

    //
    // Read out the Report Descriptor from the mouse and parse it for
    // the format of the reports coming back from the mouse.
    //
    USBHHIDGetReportDescriptor(psMsInstance->psHIDInstance,
                               psMsInstance->pui8Heap,
                               psMsInstance->ui32HeapSize);

    //
    // Set the mouse to boot protocol.
    //
    USBHHIDSetProtocol(psMsInstance->psHIDInstance, 1);

    return(0);
}

//*****************************************************************************
//
// This function handles updating the state of the mouse buttons and axis.
//
// \param psMsInstance is the pointer to an instance of the mouse data.
//
// This function will check for updates to buttons or X/Y movements and send
// callbacks to the mouse callback function.
//
// \return None.
//
//*****************************************************************************
static void
UpdateMouseState(tUSBHMouse *psMsInstance)
{
    uint32_t ui32Button;

    if(psMsInstance->pui8Buffer[0] != psMsInstance->ui8Buttons)
    {
        for(ui32Button = 1; ui32Button <= 0x4; ui32Button <<= 1)
        {
            if(((psMsInstance->pui8Buffer[0] & ui32Button) != 0) &&
               ((psMsInstance->ui8Buttons & ui32Button) == 0))
            {
                //
                // Send the mouse button press notification to the application.
                //
                psMsInstance->pfnCallback(0, USBH_EVENT_HID_MS_PRESS,
                                          ui32Button, 0);
            }
            if(((psMsInstance->pui8Buffer[0] & ui32Button) == 0) &&
               ((psMsInstance->ui8Buttons & ui32Button) != 0))
            {
                //
                // Send the mouse button release notification to the
                // application.
                //
                psMsInstance->pfnCallback(0, USBH_EVENT_HID_MS_REL,
                                         ui32Button, 0);
            }
        }

        //
        // Save the new state.
        //
        psMsInstance->ui8Buttons = psMsInstance->pui8Buffer[0];
    }

    if(psMsInstance->pui8Buffer[1] != 0)
    {
        //
        // Send the mouse button release notification to the
        // application.
        //
        psMsInstance->pfnCallback(0, USBH_EVENT_HID_MS_X,
                                  (uint32_t)psMsInstance->pui8Buffer[1], 0);
    }

    if(psMsInstance->pui8Buffer[2] != 0)
    {
        //
        // Send the mouse button release notification to the
        // application.
        //
        psMsInstance->pfnCallback(0, USBH_EVENT_HID_MS_Y,
                                  (uint32_t)psMsInstance->pui8Buffer[2], 0);
    }
}

//*****************************************************************************
//
//! This function handles event callbacks from the USB HID driver layer.
//!
//! \param pvMouse is the pointer that was passed in to the USBHHIDOpen()
//! call.
//! \param ui32Event is the event that has been passed up from the HID driver.
//! \param ui32MsgParam has meaning related to the \e ui32Event that occurred.
//! \param pvMsgData has meaning related to the \e ui32Event that occurred.
//!
//! This function will receive all event updates from the HID driver layer.
//! The mouse driver itself will mostly be concerned with report callbacks
//! from the HID driver layer and parsing them into keystrokes for the
//! application that has registered for callbacks with the USBHMouseOpen()
//! call.
//!
//! \return Non-zero values should be assumed to indicate an error condition.
//
//*****************************************************************************
uint32_t
USBHMouseCallback(void *pvMouse, uint32_t ui32Event,
                  uint32_t ui32MsgParam, void *pvMsgData)
{
    tUSBHMouse *psMsInstance;

    //
    // Recover the pointer to the instance data.
    //
    psMsInstance = (tUSBHMouse *)pvMouse;

    switch(ui32Event)
    {
        //
        // New mouse has been connected so notify the application.
        //
        case USB_EVENT_CONNECTED:
        {
            //
            // Remember that a mouse is present.
            //
            psMsInstance->ui32HIDFlags |= USBHMS_DEVICE_PRESENT;

            //
            // Notify the application that a new mouse was connected.
            //
            psMsInstance->pfnCallback(0, ui32Event, ui32MsgParam, pvMsgData);

            break;
        }
        case USB_EVENT_DISCONNECTED:
        {
            //
            // No mouse is present.
            //
            psMsInstance->ui32HIDFlags &= ~USBHMS_DEVICE_PRESENT;

            //
            // Notify the application that the mouse was disconnected.
            //
            psMsInstance->pfnCallback(0, ui32Event, ui32MsgParam, pvMsgData);

            break;
        }
        case USB_EVENT_RX_AVAILABLE:
        {
            //
            // New mouse report structure was received.
            //
            USBHHIDGetReport(psMsInstance->psHIDInstance, 0,
                             psMsInstance->pui8Buffer, USBHMS_REPORT_SIZE);

            //
            // Update the current state of the mouse and notify the application
            // of any changes.
            //
            UpdateMouseState(psMsInstance);

            break;
        }
    }
    return(0);
}

//*****************************************************************************
//
//! @}
//
//*****************************************************************************
