//*****************************************************************************
//
// usbhhub.c - This file contains the host HID driver.
//
// Copyright (c) 2011-2013 Texas Instruments Incorporated.  All rights reserved.
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
#include "inc/hw_ints.h"
#include "inc/hw_sysctl.h"
#include "driverlib/usbdrv.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rtos_bindings.h"
#include "usblib/usblib.h"
#include "usblib/host/usbhost.h"
#include "usblib/host/usbhostpriv.h"
#include "usblib/host/usbhhub.h"
#ifdef INCLUDE_DEBUG_OUTPUT
#include "utils/uartstdio.h"
#define DEBUG_OUTPUT UARTprintf
#else
#define DEBUG_OUTPUT while(0)((int (*)(char *, ...))0)
#endif

//*****************************************************************************
//
//! \addtogroup usblib_host_class
//! @{
//
//*****************************************************************************

#ifdef ewarm
#pragma pack(1)
#endif

//*****************************************************************************
//
//! The USB standard hub descriptor structure.  Full documentation for the
//! contents of this structure can be found in chapter 11.23.2.1 of the USB
//! 2.0 specification.
//
//*****************************************************************************
typedef struct
{
    //
    //! The total number of bytes in the descriptor (including this field).
    //
    uint8_t bLength;

    //
    //! The descriptor type. For a hub descriptor, this will be USB_DTYPE_HUB
    //! (0x29 or 41 decimal).
    //
    uint8_t bDescType;

    //
    //! The number of downstream-facing ports that the hub supports.
    //
    uint8_t bNbrPorts;

    //
    //! Characteristics of the hub device including its power switching
    //! capabilities and over-current protection mode.
    //
    uint16_t wHubCharacteristics;

    //
    //! The time between the start of the power-on sequence for a port and
    //! the power to the port becoming stable.  This is expressed in 2mS units.
    //
    uint8_t bPwrOn2PwrGood;

    //
    //! The maximum current requirement for the hub circuitry in mA.
    //
    uint8_t bHubContrCurrent;

    //
    //! The last two fields in the structure are bit masks indicating which
    //! downstream ports support removable devices and, following this, another
    //! obsolete field from USB1.0 related to port power control.  Each field
    //! is byte aligned and contains a bit for each hub port.  This structure
    //! definition is set up with enough storage to handle ROOT_HUB_MAX_PORTS
    //! ports but beware that the actual size of each field is dependent upon
    //! the bNbrPorts field above.
    //
    uint8_t PortInfo[((ROOT_HUB_MAX_PORTS + 7) / 8) * 2];
}
PACKED tUsbHubDescriptor;

#ifdef ewarm
#pragma pack()
#endif

//*****************************************************************************
//
// This structure holds all data specific to a single hub port.
//
//*****************************************************************************
typedef struct
{
    //
    // The handle used by the HCD layer to identify this device.
    //
    uint32_t ui32DevHandle;

    //
    // The current state of the port.
    //
    volatile tHubPortState iState;

    //
    // General counter used in various states.
    //
    volatile uint32_t ui32Count;

    //
    // A flag used to indicate that the downstream device is a low speed
    // device.
    //
    bool bLowSpeed;

    //
    // This flag is set if the hub reports that a change is pending on this
    // port.
    //
    volatile bool bChanged;
}
tHubPort;

//*****************************************************************************
//
// USB hub flags values for tHubInstance.ui32Flags.
//
//*****************************************************************************
#define USBLIB_HUB_ACTIVE       0x00000001

//*****************************************************************************
//
// This is the structure that holds all of the data for a given instance of
// a Hub device.
//
//*****************************************************************************
struct tHubInstance
{
    //
    // Save the device instance.
    //
    tUSBHostDevice *psDevice;

    //
    // Used to save the callback function pointer.
    //
    tUSBHHubCallback pfnCallback;

    //
    // Callback data provided by caller.
    //
    uint32_t ui32CBData;

    //
    // Interrupt IN pipe.
    //
    uint32_t ui32IntInPipe;

    //
    // Hub characteristics as reported in the class-specific hub descriptor.
    //
    uint16_t ui16HubCharacteristics;

    //
    // The number of downstream-facing ports the hub supports.
    //
    uint8_t ui8NumPorts;

    //
    // The number of ports on the hub that we can actually talk to.  This will
    // be the smaller of the number of ports on the hub and MAX_USB_DEVICES.
    //
    uint8_t ui8NumPortsInUse;

    //
    // The size of a status change packet sent by the hub.  This is determined
    // from the number of ports supported by the hub.
    //
    uint8_t ui8ReportSize;

    //
    // Flags indicating whether the hub is connected.
    //
    uint32_t ui32Flags;

    //
    // Flag indicating that a device is currently in process of being
    // enumerated.
    //
    volatile bool bEnumerationBusy;

    //
    // This is valid if bEnumerationBusy is set and indicates the port
    // that is in the process of enumeration.
    //
    uint8_t ui8EnumIdx;

    //
    // The state of each of the ports we support on the hub.
    //
    tHubPort psPorts[MAX_USB_DEVICES];

    //
    // The interrupt number for this instance.
    //
    uint32_t ui32IntNum;
};

//*****************************************************************************
//
//! Forward references to the hub class driver functions.
//
//*****************************************************************************
static void *HubDriverOpen(tUSBHostDevice *psDevice);
static void HubDriverClose(void *pvHubDevice);

//*****************************************************************************
//
//! This constant global structure defines the Hub Class Driver that is
//! provided with the USB library.
//
//*****************************************************************************
const tUSBHostClassDriver g_sUSBHubClassDriver =
{
    USB_CLASS_HUB,
    HubDriverOpen,
    HubDriverClose,
    0
};

//*****************************************************************************
//
// The instance data storage for attached hub.
//
//*****************************************************************************
static tHubInstance g_sRootHub;

//*****************************************************************************
//
// Hub and port state change flags as reported via the hub's IN endpoint.
//
//*****************************************************************************
static volatile uint32_t g_ui32ChangeFlags;

//
// Note: The following assumes ROOT_HUB_MAX_PORTS is less than 32!
//
static uint32_t g_ui32HubChanges;

//*****************************************************************************
//
// This function is called to send a request to the hub to set a feature on
// a given port.
//
// \param psHubInstance is the hub device instance.
// \param ui8Port is the port number for this request.
// \param ui16Feature is one of the HUB_FEATURE_PORT_* values.
//
// This function will send the set feature request to the hub indicated by the
// \e psHubInstance parameter.  The \e ui8Port value indicates which port
// number to send this request to and can range from 0 to the number of valid
// ports on the given hub.  A \e ui8Port value of 0 is an access to the hub
// itself and not one of the hub ports.  The \e ui16Feature is the feature
//  request toset on the given port.  For example, a \e ui16Feature value of
// \e HUB_FEATURE_PORT_RESET and \e ui8Port value of 1 will cause reset
// signaling to hub port 1.
//
// \return None.
//
//*****************************************************************************
static void
HubSetPortFeature(tHubInstance *psHubInstance, uint8_t ui8Port,
                  uint16_t ui16Feature)
{
    tUSBRequest sSetupPacket;
    tUSBHostDevice *psDevice;

    //
    // Retrieve the hub instance and device pointer.
    //
    psDevice = psHubInstance->psDevice;

    //
    // This is a standard OUT request.
    //
    sSetupPacket.bmRequestType = USB_RTYPE_DIR_OUT | USB_RTYPE_CLASS |
                                 USB_RTYPE_OTHER;

    //
    // Set the field to clear the requested port feature.
    //
    sSetupPacket.bRequest = USBREQ_SET_FEATURE;
    sSetupPacket.wValue = ui16Feature;
    sSetupPacket.wIndex = ui8Port;
    sSetupPacket.wLength = 0;

    //
    // Send the request.
    //
    USBHCDControlTransfer(0, &sSetupPacket, psDevice, 0, 0,
                          psDevice->sDeviceDescriptor.bMaxPacketSize0);
}

//*****************************************************************************
//
// This function is called to send a request to the hub to clear a feature on
// a given port.
//
// \param psHubInstance is the hub device instance.
// \param ui8Port is the port number for this request.
// \param ui16Feature is one of the HUB_FEATURE_PORT_* values.
//
// This function will send the clear feature request to the hub indicated by
// the \e psHubInstance parameter.  The \e ui8Port value indicates which port
// number to send this request to and can range from 0 to the number of valid
// ports on the given hub.  A \e ui8Port value of 0 is an access to the hub
// itself and not one of the hub ports.  The \e ui16Feature is the feature
// request to clear on the given port.  For example, a \e ui16Feature value of
// \e HUB_FEATURE_C_PORT_RESET and \e ui8Port value of 1 will clear the reset
// complete signaling on hub port 1.  Values like the reset feature will
// remain set until actively cleared by this function.
//
// \return None.
//
//*****************************************************************************
static void
HubClearPortFeature(tHubInstance *psHubInstance, uint8_t ui8Port,
                    uint16_t ui16Feature)
{
    tUSBRequest sSetupPacket;
    tUSBHostDevice *psDevice;

    //
    // Retrieve the hub instance and device pointer.
    //
    psDevice = psHubInstance->psDevice;

    //
    // This is a standard OUT request.
    //
    sSetupPacket.bmRequestType = USB_RTYPE_DIR_OUT | USB_RTYPE_CLASS |
                                 USB_RTYPE_OTHER;

    //
    // Set the field to clear the requested port feature.
    //
    sSetupPacket.bRequest = USBREQ_CLEAR_FEATURE;
    sSetupPacket.wValue = ui16Feature;
    sSetupPacket.wIndex = ui8Port;
    sSetupPacket.wLength = 0;

    //
    // Send the request.
    //
    USBHCDControlTransfer(0, &sSetupPacket, psDevice, 0, 0,
                          psDevice->sDeviceDescriptor.bMaxPacketSize0);
}

//*****************************************************************************
//
// This function is used to retrieve the current status of a port on the
// hub.
//
// \param psHubInstance is the hub device instance.
// \param ui8Port is the port number for this request.
// \param pui16PortStatus is a pointer to the memory to store the current
// status of the port.
// \param pui16PortChange is a pointer to the memory to store the current
// change status of the ports.
//
// This function is used to retrieve the current overall status and change
// status for the port given in the \e ui8Port parameter.  The \e ui8Port value
// indicates which port number to send this request to and can range from 0 to
// the number of valid ports on the given hub.  A \e ui8Port value of 0 is an
// access to the hub itself and not one of the hub ports.
//
// \return None.
//
//*****************************************************************************
static bool
HubGetPortStatus(tHubInstance *psHubInstance, uint8_t ui8Port,
                 uint16_t *pui16PortStatus, uint16_t *pui16PortChange)
{
    uint32_t ui32Data, ui32Read;
    tUSBRequest sSetupPacket;
    tUSBHostDevice *psDevice;

    //
    // Retrieve the device pointer.
    //
    psDevice = psHubInstance->psDevice;

    //
    // This is a standard OUT request.
    //
    sSetupPacket.bmRequestType = USB_RTYPE_DIR_IN | USB_RTYPE_CLASS |
                                 USB_RTYPE_OTHER;

    //
    // Set the fields to get the hub status.
    //
    sSetupPacket.bRequest = USBREQ_GET_STATUS;
    sSetupPacket.wValue = 0;
    sSetupPacket.wIndex = (uint16_t)ui8Port;
    sSetupPacket.wLength = 4;

    //
    // Send the request.
    //
    ui32Read = USBHCDControlTransfer(0, &sSetupPacket, psDevice,
                                (uint8_t *)&ui32Data, 4,
                                psDevice->sDeviceDescriptor.bMaxPacketSize0);

    //
    // Check that we received the correct number of bytes.
    //
    if(ui32Read != 4)
    {
        return(false);
    }
    else
    {
        //
        // We got 4 bytes from the device. Now translate these into the 2
        // 16-bit values we pass back to the caller.
        //
        *pui16PortStatus = (uint16_t)(ui32Data & 0xFFFF);
        *pui16PortChange = (uint16_t)(ui32Data >> 16);

        DEBUG_OUTPUT("Port %d, status 0x%04x, change 0x%04x\n", ui8Port,
                     *pui16PortStatus, *pui16PortChange);
    }

    //
    // All is well.
    //
    return(true);
}

//*****************************************************************************
//
// This function handles callbacks for the interrupt IN endpoint for the hub
// device.
//
//*****************************************************************************
static void
HubIntINCallback(uint32_t ui32Pipe, uint32_t ui32Event)
{
    switch (ui32Event)
    {
        //
        // Handles a request to schedule a new request on the interrupt IN
        // pipe.
        //
        case USB_EVENT_SCHEDULER:
        {
            //
            // Set things up to read the next change indication from the hub.
            //
            USBHCDPipeSchedule(ui32Pipe, (uint8_t *)&g_ui32HubChanges,
                               (uint32_t)g_sRootHub.ui8ReportSize);
            break;
        }

        //
        // Called when new data is available on the interrupt IN pipe.
        //
        case USB_EVENT_RX_AVAILABLE:
        {
            //
            // For data transfers on INT IN endpoints, we need to acknowledge
            // the data from this callback.
            //
            USBHCDPipeDataAck(ui32Pipe);

            //
            // Update our global "ports needing service" flags with the latest
            // information we have just received.
            //
            g_ui32ChangeFlags |= g_ui32HubChanges;

            //
            // Send the report data to the USB host hub device class driver if
            // we have been given a callback function.
            //
            if(g_sRootHub.pfnCallback)
            {
                g_sRootHub.pfnCallback((void *)g_sRootHub.ui32CBData,
                                       USB_EVENT_RX_AVAILABLE,
                                       ui32Pipe, &g_ui32HubChanges);
            }

            break;
        }
        case USB_EVENT_ERROR:
        {
            break;
        }
    }
}

//*****************************************************************************
//
// Query the class-specific hub descriptor.
//
//*****************************************************************************
static bool
GetHubDescriptor(tUsbHubDescriptor *psDesc)
{
    uint32_t ui32Read;
    tUSBRequest sSetupPacket;
    tUSBHostDevice *psDevice;

    //
    // Retrieve the device pointer.
    //
    psDevice = g_sRootHub.psDevice;

    //
    // This is a standard OUT request.
    //
    sSetupPacket.bmRequestType = USB_RTYPE_DIR_IN | USB_RTYPE_CLASS |
                                 USB_RTYPE_DEVICE;

    //
    // Set the fields to get the hub descriptor.  Initially, we request only
    // the first 4 bytes of the descriptor.  This will give us the size which
    // we use to determine how many bytes to read to get the full descriptor.
    // This is necessary since we don't know how many ports the hub can support
    // and we only support up to MAX_USB_DEVICES.
    //
    sSetupPacket.bRequest = USBREQ_GET_DESCRIPTOR;
    sSetupPacket.wValue = (USB_DTYPE_HUB << 8);
    sSetupPacket.wIndex = 0;
    sSetupPacket.wLength = sizeof(tUsbHubDescriptor);

    //
    // Send the request.
    //
    ui32Read = USBHCDControlTransfer(0, &sSetupPacket, psDevice,
                                (void *)psDesc, sizeof(tUsbHubDescriptor),
                                psDevice->sDeviceDescriptor.bMaxPacketSize0);

     //
     // Make sure we got at least some data.
     //
     if(ui32Read == 0)
     {
         return(false);
     }

    //
    // All is well.
    //
    return(true);
}

//*****************************************************************************
//
// Open an instance of the hub driver.  This is called when the USB host
// has enumerated a new hub device.
//
//*****************************************************************************
static void *
HubDriverOpen(tUSBHostDevice *psDevice)
{
    tEndpointDescriptor *psEndpointDescriptor;
    tInterfaceDescriptor *psInterface;
    tUsbHubDescriptor sHubDesc;
    bool bRetcode;
    uint32_t ui32Loop;

    //
    // If we are already talking to a hub, fail the call.  We only support
    // a single hub.
    //
    if(g_sRootHub.ui32Flags & USBLIB_HUB_ACTIVE)
    {
        return(0);
    }

    //
    // Get pointers to the device descriptors we need to look at.
    //
    psInterface = USBDescGetInterface(psDevice->psConfigDescriptor, 0, 0);
    psEndpointDescriptor = USBDescGetInterfaceEndpoint(psInterface, 0,
                                      psDevice->ui32ConfigDescriptorSize);

    //
    // If there are no endpoints, something is wrong since a hub must have
    // a single INT endpoint for signaling.
    //
    if(psEndpointDescriptor == 0)
    {
        return 0;
    }

    //
    // Make sure we really are talking to a hub.
    //
    if((psInterface->bInterfaceClass != USB_CLASS_HUB) ||
       (psInterface->bInterfaceSubClass != 0))
    {
        //
        // Something is wrong - this isn't a hub or, if it is, we don't
        // understand the protocol it is using.
        //
        return(0);
    }

    //
    // Remember the device information for later.
    //
    g_sRootHub.psDevice = psDevice;

    //
    // A hub must support an interrupt endpoint so check this.
    //
    if((psEndpointDescriptor->bmAttributes & USB_EP_ATTR_TYPE_M) ==
       USB_EP_ATTR_INT)
    {
        //
        // The endpoint is the correct type. Is it an IN endpoint?
        //
        if(psEndpointDescriptor->bEndpointAddress & USB_EP_DESC_IN)
        {
            //
            // Yes - all is well with the hub endpoint so allocate a pipe to
            // handle traffic from the hub.
            //
            g_sRootHub.ui32IntInPipe = USBHCDPipeAlloc(0, USBHCD_PIPE_INTR_IN,
                                                       psDevice,
                                                       HubIntINCallback);
            USBHCDPipeConfig(g_sRootHub.ui32IntInPipe,
                             psEndpointDescriptor->wMaxPacketSize,
                             psEndpointDescriptor->bInterval,
                             psEndpointDescriptor->bEndpointAddress &
                             USB_EP_DESC_NUM_M);
        }
    }

    //
    // Did we allocate the endpoint successfully?
    //
    if(!g_sRootHub.ui32IntInPipe)
    {
        //
        // No - return an error.
        //
        return 0;
    }

    //
    // Assuming we have a callback, call it to tell the owner that a hub is
    // now connected.
    //
    if(g_sRootHub.pfnCallback != 0)
    {
        g_sRootHub.pfnCallback((void *)g_sRootHub.ui32CBData,
                               USB_EVENT_CONNECTED, (uint32_t)&g_sRootHub, 0);
    }

    //
    // Get the hub descriptor and store information we'll need for later.
    //
    bRetcode = GetHubDescriptor(&sHubDesc);
    if(bRetcode)
    {

        //
        // We read the descriptor successfully so extract the parts we need.
        //
        g_sRootHub.ui8NumPorts = sHubDesc.bNbrPorts;
        g_sRootHub.ui16HubCharacteristics = sHubDesc.wHubCharacteristics;
        g_sRootHub.ui8NumPortsInUse =
            (sHubDesc.bNbrPorts > MAX_USB_DEVICES) ? MAX_USB_DEVICES :
                                                     sHubDesc.bNbrPorts;

        //
        // The size of the status change report that the hub sends is dependent
        // upon the number of ports that the hub supports.  Calculate this by
        // adding 1 to the number of ports (bit 0 of the report is the hub
        // status, higher bits are one per port) then dividing by 8 (bits per
        // byte) and rounding up.
        //
        g_sRootHub.ui8ReportSize = ((sHubDesc.bNbrPorts + 1) + 7) / 8;

        //
        // Enable power to all ports on the hub.
        //
        for(ui32Loop = 1; ui32Loop <= sHubDesc.bNbrPorts; ui32Loop++)
        {
            //
            // Turn on power to this port.
            //
            HubSetPortFeature(&g_sRootHub, ui32Loop,
                              HUB_FEATURE_PORT_POWER);
        }

        //
        // Clear out our port state structures.
        //
        for(ui32Loop = 0; ui32Loop < MAX_USB_DEVICES; ui32Loop++)
        {
            g_sRootHub.psPorts[ui32Loop].bChanged = false;
            g_sRootHub.psPorts[ui32Loop].iState = ePortIdle;
        }
    }
    else
    {
        //
        // Oops - we can't read the hub descriptor!  Tidy up and return
        // an error.
        //
        USBHCDPipeFree(g_sRootHub.ui32IntInPipe);
        g_sRootHub.pfnCallback = 0;
        g_sRootHub.ui32Flags &= ~USBLIB_HUB_ACTIVE;
        return(0);
    }

    //
    // If we get here, all is well so remember that the hub is connected and
    // active.
    //
    g_sRootHub.ui32Flags |= USBLIB_HUB_ACTIVE;

    //
    // Return our instance data pointer to the caller to use as a handle.
    //
    return((void *)&g_sRootHub);
}

//*****************************************************************************
//
// Close an instance of the hub driver.
//
//*****************************************************************************
static void
HubDriverClose(void *pvHubDevice)
{
    uint32_t ui32Loop;

    //
    // No device so just exit.
    //
    if(g_sRootHub.psDevice == 0)
    {
        return;
    }

    //
    // Disconnect any devices that are currently connected to the hub.
    //
    for(ui32Loop = 0; ui32Loop < MAX_USB_DEVICES; ui32Loop++)
    {
        //
        // Does this port have a device connected to it that we have previously
        // reported to the host control layer?h
        //
        if((g_sRootHub.psPorts[ui32Loop].iState == ePortActive) ||
           (g_sRootHub.psPorts[ui32Loop].iState == ePortResetWait) ||
           (g_sRootHub.psPorts[ui32Loop].iState == ePortEnumerated) ||
           (g_sRootHub.psPorts[ui32Loop].iState == ePortError))
        {
            //
            // Yes - tell the host controller to disconnect the device.
            //
            USBHCDHubDeviceDisconnected(0,
                g_sRootHub.psPorts[ui32Loop].ui32DevHandle);

        }

        //
        // Make sure that the state returns to idle.
        //
        g_sRootHub.psPorts[ui32Loop].iState = ePortIdle;

    }

    //
    // Reset the device pointer.
    //
    g_sRootHub.psDevice = 0;

    //
    // Mark the hub as absent.
    //
    g_sRootHub.ui32Flags &= ~USBLIB_HUB_ACTIVE;

    //
    // Note that we are not in the middle of enumerating anything.
    //
    g_sRootHub.bEnumerationBusy = false;

    //
    // Free the Interrupt IN pipe.
    //
    if(g_sRootHub.ui32IntInPipe != 0)
    {
        USBHCDPipeFree(g_sRootHub.ui32IntInPipe);
    }

    //
    // If the callback exists, call it with a DISCONNECTED event.
    //
    if(g_sRootHub.pfnCallback != 0)
    {
        g_sRootHub.pfnCallback((void *)g_sRootHub.ui32CBData,
                               USB_EVENT_DISCONNECTED, (uint32_t)&g_sRootHub,
                               0);
    }
}

//*****************************************************************************
//
// Perform any processing required as a result of a change in the reset
// signaling for a given port.
//
//*****************************************************************************
static void
HubDriverReset(uint8_t ui8Port, bool bResetActive)
{
    //
    // Did the reset sequence end or begin?
    //
    if(!bResetActive)
    {
        //
        // The reset ended.  Now wait for at least 10ms before signaling
        // USB enumeration code that a new device is waiting to be enumerated.
        //
        g_sRootHub.psPorts[ui8Port].iState = ePortResetWait;

        //
        // Set the wait to 10ms (10 frames) from now.
        //
        g_sRootHub.psPorts[ui8Port].ui32Count = 10;
    }
    else
    {
        //
        // Was this device previously active?
        //
        if(g_sRootHub.psPorts[ui8Port].iState == ePortActive)
        {
            USBHCDHubDeviceDisconnected(0,
                g_sRootHub.psPorts[ui8Port].ui32DevHandle);
        }

        //
        // The reset is active so mark our port as in reset.
        //
        g_sRootHub.psPorts[ui8Port].iState = ePortResetActive;
    }
}

//*****************************************************************************
//
// Start the process of enumerating a new device by issuing a reset to the
// appropriate downstream port.
//
//*****************************************************************************
static void
HubDriverDeviceReset(uint8_t ui8Port)
{
    DEBUG_OUTPUT("Starting enumeration for port %d\n", ui8Port);

    //
    // Record the fact that we are in the process of enumerating a device.
    //
    g_sRootHub.bEnumerationBusy = true;

    //
    // Save the port that is being enumerated.
    //
    g_sRootHub.ui8EnumIdx = ui8Port;

    //
    // Mark the port as being reset.
    //
    g_sRootHub.psPorts[ui8Port].iState = ePortResetActive;

    //
    // Initiate a reset on the relevant port to start the enumeration process.
    //
    HubSetPortFeature(&g_sRootHub, ui8Port, HUB_FEATURE_PORT_RESET);
}

//*****************************************************************************
//
// A new device has been connected to the hub.  Allocate resources to manage
// it and pass details back to the main USB host enumeration code to have the
// device enumerated.
//
//*****************************************************************************
static void
HubDriverDeviceConnect(uint8_t ui8Port, bool bLowSpeed)
{
    DEBUG_OUTPUT("HubDriverDeviceConnect\n");

    //
    // We've allocated a port table entry so fill it in then initiate a reset
    // on the device.
    //
    g_sRootHub.psPorts[ui8Port].bChanged = false;
    g_sRootHub.psPorts[ui8Port].bLowSpeed = bLowSpeed;

    //
    // Mark the port as having a device present but not enumerated.
    //
    DEBUG_OUTPUT("Deferring enumeration for port %d\n", ui8Port);
    g_sRootHub.psPorts[ui8Port].iState = ePortConnected;

    //
    // Wait 100ms to reset the device.
    //
    g_sRootHub.psPorts[ui8Port].ui32Count = 100;
}

//*****************************************************************************
//
// An existing device has been removed from the hub.  Tidy up and let the main
// USB host code know so that it can free device resources.
//
//*****************************************************************************
static void
HubDriverDeviceDisconnect(uint8_t ui8Port)
{
    //
    // This is a device we are currently managing.  Have we already informed
    // the host controller that it is present?
    //
    if((g_sRootHub.psPorts[ui8Port].iState == ePortActive) ||
       (g_sRootHub.psPorts[ui8Port].iState == ePortResetWait) ||
       (g_sRootHub.psPorts[ui8Port].iState == ePortEnumerated) ||
       (g_sRootHub.psPorts[ui8Port].iState == ePortError))
    {
        //
        // Yes - tell the host controller that the device is not longer
        // connected.
        //
        USBHCDHubDeviceDisconnected(0,
                                g_sRootHub.psPorts[ui8Port].ui32DevHandle);
    }

    //
    // If the device was being enumerated, make sure we clear the flag
    // indicating that an enumeration is still ongoing.
    //
    if((g_sRootHub.psPorts[ui8Port].iState == ePortResetActive) ||
       (g_sRootHub.psPorts[ui8Port].iState == ePortResetWait) ||
       (g_sRootHub.psPorts[ui8Port].iState == ePortActive))
    {
        g_sRootHub.bEnumerationBusy = false;
    }

    //
    // Free up the port state structure.
    //
    g_sRootHub.psPorts[ui8Port].iState = ePortIdle;
}

//*****************************************************************************
//
// This function is called periodically by USBHCDMain().  We use it to handle
// the hub port state machine.
//
//*****************************************************************************
void
USBHHubMain(void)
{
    uint16_t ui16Status, ui16Changed;
    uint_fast8_t ui8Port;
    bool bRetcode;

    //
    // If the hub is not present, just return.
    //
    if((g_sRootHub.ui32Flags & USBLIB_HUB_ACTIVE) == 0)
    {
        return;
    }

    //
    // Initialize the status variables.
    //
    ui16Status = 0;
    ui16Changed = 0;

    //
    // The hub is active and something changed. Check to see which port changed
    // state and handle as necessary.
    //
    for(ui8Port = 0; ui8Port <= g_sRootHub.ui8NumPortsInUse; ui8Port++)
    {
        //
        // Decrement any wait counter if there is one present.
        //
        if(g_sRootHub.psPorts[ui8Port].ui32Count != 0)
        {
            g_sRootHub.psPorts[ui8Port].ui32Count--;
        }

        //
        // Is this port waiting to be enumerated and is the last device
        // enumeration finished?
        //
        if((g_sRootHub.psPorts[ui8Port].iState == ePortConnected) &&
           (!g_sRootHub.bEnumerationBusy) &&
           (g_sRootHub.psPorts[ui8Port].ui32Count == 0))
        {
            //
            // Yes - start the enumeration processing for this device.
            //
            HubDriverDeviceReset(ui8Port);
        }

        //
        // If the state is ePortResetWait then the hub is waiting before
        // accessing device as the USB 2.0 specification requires.
        //
        if((g_sRootHub.psPorts[ui8Port].iState == ePortResetWait) &&
           (g_sRootHub.psPorts[ui8Port].ui32Count == 0))
        {
            //
            // Start the enumeration process if the timeout has passed and
            // the hub is waiting to start enumerating the device.
            //
            g_sRootHub.psPorts[ui8Port].iState = ePortActive;

            //
            // Call the main host controller layer to have it enumerate the
            // newly connected device.
            //
            g_sRootHub.psPorts[ui8Port].ui32DevHandle =
                USBHCDHubDeviceConnected(0, 1, ui8Port,
                                 g_sRootHub.psPorts[ui8Port].bLowSpeed);
        }

        //
        // If an enumeration is in progress and the loop is not on the port
        // being enumerated then skip the port.
        //
        if(g_sRootHub.bEnumerationBusy &&
           (g_sRootHub.ui8EnumIdx != ui8Port))
        {
            continue;
        }

        //
        // Did something change for this particular port?
        //
        if(g_ui32ChangeFlags & (1 << ui8Port))
        {
            //
            // Yes - query the port status.
            //
            bRetcode = HubGetPortStatus(&g_sRootHub, ui8Port,
                                        &ui16Status, &ui16Changed);

            //
            // Clear this change with the USB interrupt temporarily disabled to
            // ensure that we do not clear a flag that the interrupt routine
            // has just set.
            //
            OS_INT_DISABLE(g_sRootHub.ui32IntNum);
            g_ui32ChangeFlags &= ~(1 << ui8Port);
            OS_INT_ENABLE(g_sRootHub.ui32IntNum);

            //
            // If there was an error, go on and look at the next bit.
            //
            if(!bRetcode)
            {
                continue;
            }

            //
            // Now consider what changed and handle it as necessary.
            //

            //
            // Was a device connected to or disconnected from the port?
            //
            if(ui16Changed & HUB_PORT_CHANGE_DEVICE_PRESENT)
            {
                DEBUG_OUTPUT("Connection change on port %d\n", ui8Port);

                //
                // Clear the condition.
                //
                HubClearPortFeature(&g_sRootHub, ui8Port,
                                    HUB_FEATURE_C_PORT_CONNECTION);

                //
                // Was a device connected or disconnected?
                //
                if(ui16Status & HUB_PORT_STATUS_DEVICE_PRESENT)
                {
                    DEBUG_OUTPUT("Connected\n");

                    //
                    // A device was connected.
                    //
                    HubDriverDeviceConnect(ui8Port,
                            ((ui16Status & HUB_PORT_STATUS_LOW_SPEED) ?
                             true : false));
                }
                else
                {
                    DEBUG_OUTPUT("Disconnected\n");

                    //
                    // A device was disconnected.
                    //
                    HubDriverDeviceDisconnect(ui8Port);
                }
            }

            //
            // Did a reset on the port complete?
            //
            if(ui16Changed & HUB_PORT_CHANGE_RESET)
            {
                //
                // Clear the condition.
                //
                HubClearPortFeature(&g_sRootHub, ui8Port,
                                    HUB_FEATURE_C_PORT_RESET);

                //
                // Yes - query the port status.
                //
                bRetcode = HubGetPortStatus(&g_sRootHub, ui8Port,
                                            &ui16Status, &ui16Changed);

                DEBUG_OUTPUT("Reset %s for port %d\n",
                        ((ui16Status & HUB_PORT_STATUS_RESET) ? "asserted" :
                        "deasserted"), ui8Port);

                //
                // Handle the reset case.
                //
                HubDriverReset(ui8Port, (ui16Status & HUB_PORT_STATUS_RESET) ?
                               true : false);
            }

            //
            // Did an over-current reset on the port complete?
            //
            if(ui16Changed & HUB_PORT_CHANGE_OVER_CURRENT)
            {
                DEBUG_OUTPUT("Port %d over current.\n", ui8Port);

                //
                // Currently we ignore this and just clear the condition.
                //
                HubClearPortFeature(&g_sRootHub, ui8Port,
                                    HUB_FEATURE_C_PORT_OVER_CURRENT);
            }

            //
            // Has the port been enabled or disabled?
            //
            if(ui16Changed & HUB_PORT_CHANGE_ENABLED)
            {
                DEBUG_OUTPUT("Enable change for port %d.\n", ui8Port);

                //
                // Currently we ignore this and just clear the condition.
                //
                HubClearPortFeature(&g_sRootHub, ui8Port,
                                    HUB_FEATURE_C_PORT_ENABLE);
            }

            //
            // Has the port been suspended or resumed?
            //
            if(ui16Changed & HUB_PORT_CHANGE_SUSPENDED)
            {
                DEBUG_OUTPUT("Suspend change for port %d.\n", ui8Port);

                //
                // Currently we ignore this and just clear the condition.
                //
                HubClearPortFeature(&g_sRootHub, ui8Port,
                                    HUB_FEATURE_C_PORT_SUSPEND);
            }
        }
    }
}

//*****************************************************************************
//
//! Informs the hub class driver that a downstream device has been enumerated.
//!
//! \param ui8Hub is the address of the hub to which the downstream device
//! is attached.
//! \param ui8Port is the port on the hub to which the downstream device is
//! attached.
//!
//! This function is called by the host controller driver to inform the hub
//! class driver that a downstream device has been enumerated successfully.
//! The hub driver then moves on and continues enumeration of any other newly
//! connected devices.
//!
//! \return None.
//
//*****************************************************************************
void
USBHHubEnumerationComplete(uint8_t ui8Hub, uint8_t ui8Port)
{
    DEBUG_OUTPUT("Enumeration complete for hub %d, port %d\n", ui8Hub, ui8Port);

    //
    // Record the fact that the device is up and running.
    //
    g_sRootHub.psPorts[ui8Port].iState = ePortEnumerated;

    //
    // Clear the flag we use to defer further enumerations.  This will cause
    // the next connected device (if any) to start enumeration on the next
    // call to USBHHubMain().
    //
    g_sRootHub.bEnumerationBusy = false;
}

//*****************************************************************************
//
//! Informs the hub class driver that a downstream device failed to enumerate.
//!
//! \param ui8Hub is the address of the hub to which the downstream device
//! is attached.
//! \param ui8Port is the port on the hub to which the downstream device is
//! attached.
//!
//! This function is called by the host controller driver to inform the hub
//! class driver that an attempt to enumerate a downstream device has failed.
//! The hub driver then cleans up and continues enumeration of any other newly
//! connected devices.
//!
//! \return None.
//
//*****************************************************************************
void
USBHHubEnumerationError(uint8_t ui8Hub, uint8_t ui8Port)
{
    DEBUG_OUTPUT("Enumeration error for hub %d, port %d\n", ui8Hub, ui8Port);

    //
    // Record the fact that the device is not working correctly.
    //
    g_sRootHub.psPorts[ui8Port].iState = ePortError;

    //
    // Clear the flag we use to defer further enumerations.  This will cause
    // the next connected device (if any) to start enumeration on the next
    // call to USBHHubMain().
    //
    g_sRootHub.bEnumerationBusy = false;
}

//*****************************************************************************
//
//! This function is used to enable the host hub class driver before any
//! devices are present.
//!
//! \param pfnCallback is the driver call back for host hub events.
//!
//! This function is called to open an instance of a host hub device and
//! provides a valid callback function for host hub events in the
//! \e pfnCallback parameter.  This function must be called before the USB
//! host code can successfully enumerate a hub device or any devices attached
//! to the hub.  The \e pui8HubPool is memory provided to the hub class to
//! manage the devices that are connected to the hub.  The \e ui32PoolSize is
//! the number of bytes and should be at least 32 bytes per device including
//! the hub device itself.  A simple formula for providing memory to the hub
//! class is \b MAX_USB_DEVICES * 32 bytes of data to allow for proper
//! enumeration of connected devices.  The value for \b MAX_USB_DEVICES is
//! defined in the usblib.h file and controls the number of devices
//! supported by the USB library.  The \e ui32NumHubs parameter
//! defaults to one and only one buffer of size tHubInstance is required to
//! be passed in the \e psHubInstance parameter.
//!
//! \note Changing the value of \b MAX_USB_DEVICES requires a rebuild of the
//! USB library to have an effect on the library.
//!
//! \return This function returns the driver instance to use for the other
//! host hub functions.  If there is no instance available at the time of
//! this call, this function returns zero.
//
//*****************************************************************************
tHubInstance *
USBHHubOpen(tUSBHHubCallback pfnCallback)
{
    //
    // Only one hub is supported.
    //
    if(g_sRootHub.pfnCallback)
    {
        DEBUG_OUTPUT("USBHHubOpen failed - already connected.\n");
        return(0);
    }

    //
    // Save the instance data for this device.
    //
    g_sRootHub.pfnCallback = pfnCallback;

    DEBUG_OUTPUT("USBHHubOpen completed.\n");

    //
    // Return the device instance pointer.
    //
    return(&g_sRootHub);
}

//*****************************************************************************
//
//! This function is used to release a hub device instance.
//!
//! \param psHubInstance is the hub device instance that is to be released.
//!
//! This function is called when an instance of the hub device must be
//! released.  This function is typically made in preparation for shutdown or a
//! switch to function as a USB device when in OTG mode.  Following this call,
//! the hub device is no longer available, but it can be opened again using a
//! call to USBHHubOpen().  After calling USBHHubClose(), the host hub driver
//! no longer provides any callbacks or accepts calls to other hub driver APIs.
//!
//! \return None.
//
//*****************************************************************************
void
USBHHubClose(tHubInstance *psHubInstance)
{
    //
    // Forget the instance pointer and callback.
    //
    psHubInstance->psDevice = 0;
    psHubInstance->pfnCallback = 0;

    DEBUG_OUTPUT("USBHHubClose completed.\n");
}

//*****************************************************************************
//
// This function is used to initialize the Hub driver.  This is an internal
// function that should not be called by the application.
//
//*****************************************************************************
void
USBHHubInit(void)
{
    //
    // Initialize Hub state.
    //
    g_ui32ChangeFlags = 0;
    g_ui32HubChanges = 0;

    if(g_sRootHub.psDevice != 0)
    {
        //
        // Save the USB interrupt number.
        //
        g_sRootHub.ui32IntNum = INT_USB0_BLIZZARD;
    }
}

//*****************************************************************************
//
//! @}
//
//*****************************************************************************
