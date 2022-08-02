//*****************************************************************************
//
// usbdaudio.c - USB audio device class driver.
//
// Copyright (c) 2009-2013 Texas Instruments Incorporated.  All rights reserved.
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
#include "usblib/usbaudio.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdaudio.h"

//*****************************************************************************
//
//! \addtogroup audio_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// The following are the USB audio descriptor identifiers.
//
//*****************************************************************************
#define AUDIO_IN_TERMINAL_ID    1
#define AUDIO_OUT_TERMINAL_ID   2
#define AUDIO_CONTROL_ID        3

//*****************************************************************************
//
// The following are the USB interface numbers for this audio device.
//
//*****************************************************************************
#define AUDIO_INTERFACE_CONTROL 0
#define AUDIO_INTERFACE_OUTPUT  1

//*****************************************************************************
//
// Endpoints to use for each of the required endpoints in the driver.
//
//*****************************************************************************
#define ISOC_OUT_ENDPOINT       USB_EP_1

//*****************************************************************************
//
// Max size is (48000 samples/sec * 4 bytes/sample) * 0.001 seconds/frame.
//
//*****************************************************************************
#define ISOC_OUT_EP_MAX_SIZE    ((48000*4)/1000)

//*****************************************************************************
//
// Device Descriptor.  This is stored in RAM to allow several fields to be
// changed at runtime based on the client's requirements.
//
//*****************************************************************************
static uint8_t g_pui8AudioDeviceDescriptor[] =
{
    18,                     // Size of this structure.
    USB_DTYPE_DEVICE,       // Type of this structure.
    USBShort(0x110),        // USB version 1.1 (if we say 2.0, hosts assume
                            // high-speed - see USB 2.0 spec 9.2.6.6)
    0,                      // USB Device Class (spec 5.1.1)
    0,                      // USB Device Sub-class (spec 5.1.1)
    0,                      // USB Device protocol (spec 5.1.1)
    64,                     // Maximum packet size for default pipe.
    USBShort(0),            // Vendor ID (filled in during USBDAudioInit).
    USBShort(0),            // Product ID (filled in during USBDAudioInit).
    USBShort(0x100),        // Device Version BCD.
    1,                      // Manufacturer string identifier.
    2,                      // Product string identifier.
    3,                      // Product serial number.
    1                       // Number of configurations.
};

//*****************************************************************************
//
// Audio class device configuration descriptor.
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
static uint8_t g_pui8AudioDescriptor[] =
{
    //
    // Configuration descriptor header.
    //
    9,                          // Size of the configuration descriptor.
    USB_DTYPE_CONFIGURATION,    // Type of this descriptor.
    USBShort(32),               // The total size of this full structure.
    2,                          // The number of interfaces in this
                                // configuration.
    1,                          // The unique value for this configuration.
    0,                          // The string identifier that describes this
                                // configuration.
    USB_CONF_ATTR_BUS_PWR,      // Bus Powered, Self Powered, remote wake up.
    250,                        // The maximum power in 2mA increments.
};

//*****************************************************************************
//
// This is the Interface Association Descriptor for the serial device used in
// composite devices.
//
//*****************************************************************************
uint8_t g_pui8IADAudioDescriptor[AUDIODESCRIPTOR_SIZE] =
{

    8,                          // Size of the interface descriptor.
    USB_DTYPE_INTERFACE_ASC,    // Interface Association Type.
    0x0,                        // Default starting interface is 0.
    0x2,                        // Number of interfaces in this association.
    USB_CLASS_AUDIO,            // The device class for this association.
    USB_SUBCLASS_UNDEFINED,     // The device subclass for this association.
    USB_PROTOCOL_UNDEFINED,     // The protocol for this association.
    0                           // The string index for this association.
};

const tConfigSection g_sIADAudioConfigSection =
{
    sizeof(g_pui8IADAudioDescriptor),
    g_pui8IADAudioDescriptor
};

//*****************************************************************************
//
// The remainder of the configuration descriptor is stored in flash since we
// don't need to modify anything in it at runtime.
//
//*****************************************************************************
const uint8_t g_pui8AudioControlInterface[CONTROLINTERFACE_SIZE] =
{
    //
    // Vendor-specific Interface Descriptor.
    //
    9,                          // Size of the interface descriptor.
    USB_DTYPE_INTERFACE,        // Type of this descriptor.
    AUDIO_INTERFACE_CONTROL,    // The index for this interface.
    0,                          // The alternate setting for this interface.
    0,                          // The number of endpoints used by this
                                // interface.
    USB_CLASS_AUDIO,            // The interface class
    USB_ASC_AUDIO_CONTROL,      // The interface sub-class.
    0,                          // The interface protocol for the sub-class
                                // specified above.
    0,                          // The string index for this interface.

    //
    // Audio Header Descriptor.
    //
    9,                          // The size of this descriptor.
    USB_DTYPE_CS_INTERFACE,     // Interface descriptor is class specific.
    USB_ACDSTYPE_HEADER,        // Descriptor sub-type is HEADER.
    USBShort(0x0100),           // Audio Device Class Specification Release
                                // Number in Binary-Coded Decimal.
                                // Total number of bytes in
                                // g_pui8AudioControlInterface
    USBShort((9 + 9 + 12 + 13 + 9)),
    1,                          // Number of streaming interfaces.
    1,                          // Index of the first and only streaming
                                // interface.

    //
    // Audio Input Terminal Descriptor.
    //
    12,                         // The size of this descriptor.
    USB_DTYPE_CS_INTERFACE,     // Interface descriptor is class specific.
    USB_ACDSTYPE_IN_TERMINAL,   // Descriptor sub-type is INPUT_TERMINAL.
    AUDIO_IN_TERMINAL_ID,       // Terminal ID for this interface.
                                // USB streaming interface.
    USBShort(USB_TTYPE_STREAMING),
    0,                          // ID of the Output Terminal to which this
                                // Input Terminal is associated.
    2,                          // Number of logical output channels in the
                                // Terminal's output audio channel cluster.
    USBShort((USB_CHANNEL_L |   // Describes the spatial location of the
             USB_CHANNEL_R)),   // logical channels.
    0,                          // Channel Name string index.
    0,                          // Terminal Name string index.

    //
    // Audio Feature Unit Descriptor
    //
    13,                         // The size of this descriptor.
    USB_DTYPE_CS_INTERFACE,     // Interface descriptor is class specific.
    USB_ACDSTYPE_FEATURE_UNIT,  // Descriptor sub-type is FEATURE_UNIT.
    AUDIO_CONTROL_ID,           // Unit ID for this interface.
    AUDIO_IN_TERMINAL_ID,       // ID of the Unit or Terminal to which this
                                // Feature Unit is connected.
    2,                          // Size in bytes of an element of the
                                // bmaControls() array that follows.
                                // Master Mute control.
    USBShort(USB_ACONTROL_MUTE),
                                // Left channel volume control.
    USBShort(USB_ACONTROL_VOLUME),
                                // Right channel volume control.
    USBShort(USB_ACONTROL_VOLUME),
    0,                          // Feature unit string index.

    //
    // Audio Output Terminal Descriptor.
    //
    9,                          // The size of this descriptor.
    USB_DTYPE_CS_INTERFACE,     // Interface descriptor is class specific.
    USB_ACDSTYPE_OUT_TERMINAL,  // Descriptor sub-type is INPUT_TERMINAL.
    AUDIO_OUT_TERMINAL_ID,      // Terminal ID for this interface.
                                // Output type is a generic speaker.
    USBShort(USB_ATTYPE_SPEAKER),
    AUDIO_IN_TERMINAL_ID,       // ID of the input terminal to which this
                                // output terminal is connected.
    AUDIO_CONTROL_ID,           // ID of the feature unit that this output
                                // terminal is connected to.
    0,                          // Output terminal string index.

};

//*****************************************************************************
//
// The audio streaming interface descriptor.  This describes the two valid
// interfaces for this class.  The first interface has no endpoints and is used
// by host operating systems to put the device in idle mode, while the second
// is used when the audio device is active.
//
//*****************************************************************************
const uint8_t g_pui8AudioStreamInterface[STREAMINTERFACE_SIZE] =
{
    //
    // Vendor-specific Interface Descriptor.
    //
    9,                          // Size of the interface descriptor.
    USB_DTYPE_INTERFACE,        // Type of this descriptor.
    AUDIO_INTERFACE_OUTPUT,     // The index for this interface.
    0,                          // The alternate setting for this interface.
    0,                          // The number of endpoints used by this
                                // interface.
    USB_CLASS_AUDIO,            // The interface class
    USB_ASC_AUDIO_STREAMING,    // The interface sub-class.
    0,                          // Unused must be 0.
    0,                          // The string index for this interface.

    //
    // Vendor-specific Interface Descriptor.
    //
    9,                          // Size of the interface descriptor.
    USB_DTYPE_INTERFACE,        // Type of this descriptor.
    1,                          // The index for this interface.
    1,                          // The alternate setting for this interface.
    1,                          // The number of endpoints used by this
                                // interface.
    USB_CLASS_AUDIO,            // The interface class
    USB_ASC_AUDIO_STREAMING,    // The interface sub-class.
    0,                          // Unused must be 0.
    0,                          // The string index for this interface.

    //
    // Class specific Audio Streaming Interface descriptor.
    //
    7,                          // Size of the interface descriptor.
    USB_DTYPE_CS_INTERFACE,     // Interface descriptor is class specific.
    USB_ASDSTYPE_GENERAL,       // General information.
    AUDIO_IN_TERMINAL_ID,       // ID of the terminal to which this streaming
                                // interface is connected.
    1,                          // One frame delay.
    USBShort(USB_ADF_PCM),      //

    //
    // Format type Audio Streaming descriptor.
    //
    11,                         // Size of the interface descriptor.
    USB_DTYPE_CS_INTERFACE,     // Interface descriptor is class specific.
    USB_ASDSTYPE_FORMAT_TYPE,   // Audio Streaming format type.
    USB_AF_TYPE_TYPE_I,         // Type I audio format type.
    2,                          // Two audio channels.
    2,                          // Two bytes per audio sub-frame.
    16,                         // 16 bits per sample.
    1,                          // One sample rate provided.
    USB3Byte(48000),            // Only 48000 sample rate supported.

    //
    // Endpoint Descriptor
    //
    9,                              // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,             // Descriptor type is an endpoint.
                                    // OUT endpoint with address
                                    // ISOC_OUT_ENDPOINT.
    USB_EP_DESC_OUT | USBEPToIndex(ISOC_OUT_ENDPOINT),
    USB_EP_ATTR_ISOC |              // Endpoint is an adaptive isochronous data
    USB_EP_ATTR_ISOC_ADAPT |        //  endpoint.
    USB_EP_ATTR_USAGE_DATA,
    USBShort(ISOC_OUT_EP_MAX_SIZE), // The maximum packet size.
    1,                              // The polling interval for this endpoint.
    0,                              // Refresh is unused.
    0,                              // Synch endpoint address.

    //
    // Audio Streaming Isochronous Audio Data Endpoint Descriptor
    //
    7,                              // The size of the descriptor.
    USB_ACSDT_ENDPOINT,             // Audio Class Specific Endpoint
                                    // Descriptor.
    USB_ASDSTYPE_GENERAL,           // This is a general descriptor.
    USB_EP_ATTR_ACG_SAMPLING,       // Sampling frequency is supported.
    USB_EP_LOCKDELAY_UNDEF,         // Undefined lock delay units.
    USBShort(0),                    // No lock delay.
};

//*****************************************************************************
//
// The audio device configuration descriptor is defined as three sections,
// one containing just the 9 byte USB configuration descriptor.  The second
// holds the audio streaming interface and the third holds the audio control
// interface.
//
//*****************************************************************************
const tConfigSection g_sAudioConfigSection =
{
    sizeof(g_pui8AudioDescriptor),
    g_pui8AudioDescriptor
};

const tConfigSection g_sAudioStreamInterfaceSection =
{
    sizeof(g_pui8AudioStreamInterface),
    g_pui8AudioStreamInterface
};

const tConfigSection g_sAudioControlInterfaceSection =
{
    sizeof(g_pui8AudioControlInterface),
    g_pui8AudioControlInterface
};

//*****************************************************************************
//
// This array lists all the sections that must be concatenated to make a
// single, complete audio device configuration descriptor.
//
//*****************************************************************************
const tConfigSection *g_psAudioSections[] =
{
    &g_sAudioConfigSection,
    &g_sIADAudioConfigSection,
    &g_sAudioControlInterfaceSection,
    &g_sAudioStreamInterfaceSection
};

#define NUM_AUDIO_SECTIONS      (sizeof(g_psAudioSections) /                  \
                                 sizeof(g_psAudioSections[0]))

//*****************************************************************************
//
// The header for the single configuration we support.  This is the root of
// the data structure that defines all the bits and pieces that are pulled
// together to generate the configuration descriptor.
//
//*****************************************************************************
const tConfigHeader g_sAudioConfigHeader =
{
    NUM_AUDIO_SECTIONS,
    g_psAudioSections
};

//*****************************************************************************
//
// Configuration Descriptor.
//
//*****************************************************************************
const tConfigHeader * const g_ppAudioConfigDescriptors[] =
{
    &g_sAudioConfigHeader
};

//*****************************************************************************
//
// Various internal handlers needed by this class.
//
//*****************************************************************************
static void HandleDisconnect(void *pvAudioDevice);
static void InterfaceChange(void *pvAudioDevice, uint8_t ui8Interface,
                            uint8_t ui8AlternateSetting);
static void ConfigChangeHandler(void *pvAudioDevice, uint32_t ui32Value);
static void DataReceived(void *pvAudioDevice, uint32_t ui32Info);
static void HandleEndpoints(void *pvAudioDevice, uint32_t ui32Status);
static void HandleRequests(void *pvAudioDevice, tUSBRequest *psUSBRequest);
static void HandleDevice(void *pvAudioDevice, uint32_t ui32Request,
                         void *pvRequestData);

//*****************************************************************************
//
// The device information structure for the USB Audio device.
//
//*****************************************************************************
static const tCustomHandlers g_sAudioHandlers =
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
    // ResumeHandler
    //
    0,

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
// This function is called to handle data being received back from the host so
// that the application callback can be called when the new data is ready.
//
//*****************************************************************************
static void
DataReceived(void *pvAudioDevice, uint32_t ui32Info)
{
    tAudioInstance *psInst;
    tUSBDAudioDevice *psAudioDevice;

    ASSERT(pvAudioDevice != 0);

    //
    // Create the instance pointer.
    //
    psAudioDevice = (tUSBDAudioDevice *)pvAudioDevice;

    //
    // Make a copy of this pointer for ease of use in this function.
    //
    psInst = &psAudioDevice->sPrivateData;

    //
    // If there is an update pending and the request was to set a current
    // value then check which value was set.
    //
    if(psInst->ui16Update && (psInst->ui8Request == USB_AC_SET_CUR))
    {
        //
        // Only handling interface requests.
        //
        if((psInst->ui16RequestType & USB_RTYPE_RECIPIENT_M) ==
           USB_RTYPE_INTERFACE)
        {
            if(psInst->ui16Update == VOLUME_CONTROL)
            {
                //
                // Inform the callback of the new volume.
                //
                psAudioDevice->pfnCallback(0, USBD_AUDIO_EVENT_VOLUME,
                                      psInst->i16Volume, 0);
            }
            else if(psAudioDevice->sPrivateData.ui16Update == MUTE_CONTROL)
            {
                //
                // Inform the callback of the new data.
                //
                psAudioDevice->pfnCallback(0, USBD_AUDIO_EVENT_MUTE,
                                      psInst->ui8Mute, 0);
            }
        }
        psInst->ui16Update = 0;
    }
}

//*****************************************************************************
//
// This function is called to handle the interrupts on the isochronous endpoint
// for the audio device class.
//
//*****************************************************************************
static void
HandleEndpoints(void *pvAudioDevice, uint32_t ui32Status)
{
    uint32_t ui32EPStatus;
    tAudioInstance *psInst;
    tUSBDAudioDevice *psAudioDevice;
    uint32_t ui32Size;

    ASSERT(pvAudioDevice != 0);

    //
    // The audio device structure pointer.
    //
    psAudioDevice = (tUSBDAudioDevice *)pvAudioDevice;

    //
    // Create a pointer to the audio instance data.
    //
    psInst = &psAudioDevice->sPrivateData;

    //
    // Read out the current endpoint status.
    //
    ui32EPStatus = MAP_USBEndpointStatus(USB0_BASE, psInst->ui8OUTEndpoint);

    //
    // See if there is a receive interrupt pending.
    //
    if(ui32Status & (0x10000 << USBEPToIndex(psInst->ui8OUTEndpoint)))
    {
        //
        // Get the amount of data available in the FIFO.
        //
        ui32Size = USBEndpointDataAvail(psInst->ui32USBBase,
                                        psInst->ui8OUTEndpoint);

        //
        // Clear the status bits.
        //
        MAP_USBDevEndpointStatusClear(USB0_BASE, psInst->ui8OUTEndpoint,
                                      ui32EPStatus);

        //
        // Configure the next DMA transfer.
        //
        USBLibDMATransfer(psInst->psDMAInstance, psInst->ui8OUTDMA,
                          psInst->sBuffer.pvData, ui32Size);
    }
    else if((USBLibDMAChannelStatus(psInst->psDMAInstance,
                                    psInst->ui8OUTDMA) ==
            USBLIBSTATUS_DMA_COMPLETE))
    {
        USBEndpointDMADisable(USB0_BASE,
                              psInst->ui8OUTEndpoint, USB_EP_DEV_OUT);

        //
        // Acknowledge that the data was read, this will not cause a bus
        // acknowledgment.
        //
        MAP_USBDevEndpointDataAck(USB0_BASE, psInst->ui8OUTEndpoint, 0);

        //
        // Inform the callback of the new data.
        //
        psInst->sBuffer.pfnCallback(psInst->sBuffer.pvData,
                                    psInst->sBuffer.ui32Size,
                                    USBD_AUDIO_EVENT_DATAOUT);
    }
}

//*****************************************************************************
//
// Device instance specific handler.
//
//*****************************************************************************
static void
HandleDevice(void *pvAudioDevice, uint32_t ui32Request, void *pvRequestData)
{
    tAudioInstance *psInst;
    uint8_t *pui8Data;
    tUSBDAudioDevice *psAudioDevice;

    //
    // The audio device structure pointer.
    //
    psAudioDevice = (tUSBDAudioDevice *)pvAudioDevice;

    //
    // Create a pointer to the audio instance data.
    //
    psInst = &psAudioDevice->sPrivateData;

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
            if(pui8Data[0] == AUDIO_INTERFACE_CONTROL)
            {
                psInst->ui8InterfaceControl = pui8Data[1];
            }
            else if(pui8Data[0] == AUDIO_INTERFACE_OUTPUT)
            {
                psInst->ui8InterfaceAudio = pui8Data[1];
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
            if((pui8Data[0] & USB_EP_DESC_IN) == 0)
            {
                //
                // Extract the new endpoint number without the DIR bit.
                //
                psInst->ui8OUTEndpoint = IndexToUSBEP(pui8Data[1] & 0x7f);

                //
                // If the DMA channel has already been allocated then clear
                // that channel and prepare to possibly use a new one.
                //
                if(psInst->ui8OUTDMA != 0)
                {
                    USBLibDMAChannelRelease(psInst->psDMAInstance,
                                            psInst->ui8OUTDMA);
                }

                //
                // Allocate a DMA channel to the endpoint.
                //
                psInst->ui8OUTDMA =
                    USBLibDMAChannelAllocate(psInst->psDMAInstance,
                                             psInst->ui8OUTEndpoint,
                                             ISOC_OUT_EP_MAX_SIZE,
                                             (USB_DMA_EP_RX |
                                              USB_DMA_EP_TYPE_ISOC |
                                              USB_DMA_EP_DEVICE));

                //
                // Set the DMA individual transfer size.
                //
                USBLibDMAUnitSizeSet(psInst->psDMAInstance, psInst->ui8OUTDMA,
                                     32);

                //
                // Set the DMA arbitration size.
                //
                USBLibDMAArbSizeSet(psInst->psDMAInstance, psInst->ui8OUTDMA,
                                    16);
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
HandleDisconnect(void *pvAudioDevice)
{
    const tUSBDAudioDevice *psAudioDevice;

    ASSERT(pvAudioDevice != 0);

    //
    // The audio device structure pointer.
    //
    psAudioDevice = (const tUSBDAudioDevice *)pvAudioDevice;

    //
    // Inform the application that the device has been disconnected.
    //
    psAudioDevice->pfnCallback(0, USB_EVENT_DISCONNECTED, 0, 0);
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the device
// interface changes.  This occurs when the audio device transitions between
// being active and inactive.  Interface AUDIO_INTERFACE_CONTROL is the
// inactive interface that has no endpoints, while interface
// AUDIO_INTERFACE_AUDIO has the single Isochronous OUT endpoint.
//
//*****************************************************************************
static void
InterfaceChange(void *pvAudioDevice, uint8_t ui8Interface,
                uint8_t ui8AlternateSetting)
{
    const tUSBDAudioDevice *psAudioDevice;

    ASSERT(pvAudioDevice != 0);

    //
    // The audio device structure pointer.
    //
    psAudioDevice = (const tUSBDAudioDevice *)pvAudioDevice;

    //
    // Check which interface to change into.
    //
    if(ui8AlternateSetting == 0)
    {
        //
        // Alternate setting 0 is an inactive state.
        //
        if(psAudioDevice->pfnCallback)
        {
            psAudioDevice->pfnCallback(0, USBD_AUDIO_EVENT_IDLE, 0, 0);
        }
    }
    else
    {
        //
        // Alternate setting 1 is the active state.
        //
        if(psAudioDevice->pfnCallback)
        {
            psAudioDevice->pfnCallback(0, USBD_AUDIO_EVENT_ACTIVE, 0, 0);
        }
    }
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the device
// configuration changes.
//
//*****************************************************************************
static void
ConfigChangeHandler(void *pvAudioDevice, uint32_t ui32Value)
{
    const tUSBDAudioDevice *psAudioDevice;

    ASSERT(pvAudioDevice != 0);

    //
    // The audio device structure pointer.
    //
    psAudioDevice = (const tUSBDAudioDevice *)pvAudioDevice;

    //
    // If we have a control callback, let the client know we are open for
    // business.
    //
    if(psAudioDevice->pfnCallback)
    {
        //
        // Pass the connected event to the client.
        //
        psAudioDevice->pfnCallback(pvAudioDevice, USB_EVENT_CONNECTED, 0, 0);
    }
}

//*****************************************************************************
//
//! This function should be called once for the audio class device to
//! initialized basic operation and prepare for enumeration.
//!
//! \param ui32Index is the index of the USB controller to initialize for
//! audio class device operation.
//! \param psAudioDevice points to a structure containing parameters
//! customizing the operation of the audio device.
//!
//! In order for an application to initialize the USB audio device class, it
//! must first call this function with the a valid audio device class structure
//! in the \e psAudioDevice parameter.  This allows this function to initialize
//! the USB controller and device code to be prepared to enumerate and function
//! as a USB audio device.
//!
//! This function returns a void pointer that must be passed in to all other
//! APIs used by the audio class.
//!
//! See the documentation on the tUSBDAudioDevice structure for more
//! information on how to properly fill the structure members.
//!
//! \return Returns 0 on failure or a non-zero void pointer on success.
//
//*****************************************************************************
void *
USBDAudioInit(uint32_t ui32Index, tUSBDAudioDevice *psAudioDevice)
{
    tConfigDescriptor *psConfigDesc;
    tDeviceDescriptor *psDevDesc;

    //
    // Check parameter validity.
    //
    ASSERT(ui32Index == 0);
    ASSERT(psAudioDevice);
    ASSERT(psAudioDevice->ppui8StringDescriptors);

    //
    // Composite Init handles all initialization that is not specific to a
    // multiple instance device.
    //
    USBDAudioCompositeInit(ui32Index, psAudioDevice, 0);

    //
    // Fix up the device descriptor with the client-supplied values.
    //
    psDevDesc = (tDeviceDescriptor *)g_pui8AudioDeviceDescriptor;
    psDevDesc->idVendor = psAudioDevice->ui16VID;
    psDevDesc->idProduct = psAudioDevice->ui16PID;

    //
    // Fix up the configuration descriptor with client-supplied values.
    //
    psConfigDesc = (tConfigDescriptor *)g_pui8AudioDescriptor;
    psConfigDesc->bmAttributes = psAudioDevice->ui8PwrAttributes;
    psConfigDesc->bMaxPower = (uint8_t)(psAudioDevice->ui16MaxPowermA / 2);

    //
    // All is well so now pass the descriptors to the lower layer and put
    // the bulk device on the bus.
    //
    USBDCDInit(ui32Index, &psAudioDevice->sPrivateData.sDevInfo,
               (void *)psAudioDevice);

    //
    // Configure the DMA for the OUT endpoint.
    //
    psAudioDevice->sPrivateData.ui8OUTDMA =
        USBLibDMAChannelAllocate(psAudioDevice->sPrivateData.psDMAInstance,
                                 psAudioDevice->sPrivateData.ui8OUTEndpoint,
                                 ISOC_OUT_EP_MAX_SIZE,
                                 USB_DMA_EP_RX | USB_DMA_EP_TYPE_ISOC |
                                 USB_DMA_EP_DEVICE);

    USBLibDMAUnitSizeSet(psAudioDevice->sPrivateData.psDMAInstance,
                         psAudioDevice->sPrivateData.ui8OUTDMA, 32);
    USBLibDMAArbSizeSet(psAudioDevice->sPrivateData.psDMAInstance,
                        psAudioDevice->sPrivateData.ui8OUTDMA, 16);

    //
    // Return the pointer to the instance indicating that everything went well.
    //
    return((void *)psAudioDevice);
}

//*****************************************************************************
//
//! This function should be called once for the audio class device to
//! initialized basic operation and prepare for enumeration.
//!
//! \param ui32Index is the index of the USB controller to initialize for
//! audio class device operation.
//! \param psAudioDevice points to a structure containing parameters
//! customizing the operation of the audio device.
//! \param psCompEntry is the composite device entry to initialize when
//! creating a composite device.
//!
//! In order for an application to initialize the USB audio device class, it
//! must first call this function with the a valid audio device class structure
//! in the \e psAudioDevice parameter.  This allows this function to initialize
//! the USB controller and device code to be prepared to enumerate and function
//! as a USB audio device.  When this audio device is part of a composite
//! device, then the \e psCompEntry should point to the composite device entry
//! to initialize. This is part of the array that is passed to the
//! USBDCompositeInit() function.
//!
//! This function returns a void pointer that must be passed in to all other
//! APIs used by the audio class.
//!
//! See the documentation on the tUSBDAudioDevice structure for more
//! information on how to properly fill the structure members.
//!
//! \return Returns zero on failure or a non-zero instance value that should be
//! used with the remaining USB audio APIs.
//
//*****************************************************************************
void *
USBDAudioCompositeInit(uint32_t ui32Index, tUSBDAudioDevice *psAudioDevice,
                       tCompositeEntry *psCompEntry)
{
    tAudioInstance *psInst;

    //
    // Check parameter validity.
    //
    ASSERT(ui32Index == 0);
    ASSERT(psAudioDevice);
    ASSERT(psAudioDevice->ppui8StringDescriptors);

    //
    // Initialize the workspace in the passed instance structure.
    //
    psInst = &psAudioDevice->sPrivateData;
    psInst->ui32USBBase = USB0_BASE;

    //
    // Initialize the composite entry that is used by the composite device
    // class.
    //
    if(psCompEntry != 0)
    {
        psCompEntry->psDevInfo = &psInst->sDevInfo;
        psCompEntry->pvInstance = (void *)psAudioDevice;
    }

    //
    // Initialize the device information structure.
    //
    psInst->sDevInfo.psCallbacks = &g_sAudioHandlers;
    psInst->sDevInfo.pui8DeviceDescriptor = g_pui8AudioDeviceDescriptor;
    psInst->sDevInfo.ppsConfigDescriptors = g_ppAudioConfigDescriptors;
    psInst->sDevInfo.ppui8StringDescriptors = 0;
    psInst->sDevInfo.ui32NumStringDescriptors = 0;

    //
    // Initialize the device info structure for the HID device.
    //
    USBDCDDeviceInfoInit(0, &psInst->sDevInfo);

    //
    // The Control interface is at index 0.
    //
    psInst->ui8InterfaceControl = AUDIO_INTERFACE_CONTROL;

    //
    // The Audio interface is at index 1.
    //
    psInst->ui8InterfaceAudio = AUDIO_INTERFACE_OUTPUT;

    //
    // Set the default Isochronous OUT endpoint.
    //
    psInst->ui8OUTEndpoint = ISOC_OUT_ENDPOINT;
    psInst->ui8OUTDMA = 0;

    //
    // Set the initial buffer to null.
    //
    psInst->sBuffer.pvData = 0;

    //
    // Save the volume settings.
    //
    psInst->i16VolumeMax = psAudioDevice->i16VolumeMax;
    psInst->i16VolumeMin = psAudioDevice->i16VolumeMin;
    psInst->i16VolumeStep = psAudioDevice->i16VolumeStep;

    //
    // No update pending to any command.
    //
    psInst->ui16Update = 0;

    //
    // Plug in the client's string stable to the device information
    // structure.
    //
    psInst->sDevInfo.ppui8StringDescriptors =
                                    psAudioDevice->ppui8StringDescriptors;
    psInst->sDevInfo.ui32NumStringDescriptors =
                                    psAudioDevice->ui32NumStringDescriptors;

    //
    // Get the DMA instance pointer.
    //
    psInst->psDMAInstance = USBLibDMAInit(0);

    //
    // Return the pointer to the instance indicating that everything went well.
    //
    return((void *)psAudioDevice);
}

//*****************************************************************************
//
//! Shuts down the audio device.
//!
//! \param pvAudioDevice is the pointer to the device instance structure as
//! returned by USBDAudioInit().
//!
//! This function terminates audio interface for the instance supplied.  This
//! function should not be called if the audio device is part of a composite
//! device and instead the USBDCompositeTerm() function should be called for
//! the full composite device.
//! Following this call, the \e pvAudioDevice instance should not me used in
//! any other calls.
//!
//! \return None.
//
//*****************************************************************************
void
USBDAudioTerm(void *pvAudioDevice)
{
    ASSERT(pvAudioDevice != 0);

    //
    // Cleanly exit device mode.
    //
    USBDCDTerm(0);
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever a non-standard
// request is received.
//
// \param pvAudioDevice is the instance data for this request.
// \param psUSBRequest points to the request received.
//
// This call parses the provided request structure to the type of request and
// will respond to all commands that are understood by the class.
//
// \return None.
//
//*****************************************************************************
static void
HandleRequests(void *pvAudioDevice, tUSBRequest *psUSBRequest)
{
    uint32_t ui32Control, ui32Recipient, ui32Stall;
    tAudioInstance *psInst;
    tUSBDAudioDevice *psAudioDevice;

    ASSERT(pvAudioDevice != 0);

    //
    // The audio device structure pointer.
    //
    psAudioDevice = (tUSBDAudioDevice *)pvAudioDevice;

    //
    // Create a pointer to the audio instance data.
    //
    psInst = &psAudioDevice->sPrivateData;

    //
    // Make sure to acknowledge that the data was read, this will not send and
    // ACK that has already been done at this point.  This just tells the
    // hardware that the data was read.
    //
    MAP_USBDevEndpointDataAck(USB0_BASE, USB_EP_0, false);

    //
    // Don't stall by default.
    //
    ui32Stall = 0;

    //
    // Get the request type.
    //
    ui32Recipient = psUSBRequest->bmRequestType & USB_RTYPE_RECIPIENT_M;

    //
    // Save the request type and request value.
    //
    psInst->ui16RequestType = psUSBRequest->bmRequestType;
    psInst->ui8Request = psUSBRequest->bRequest;

    //
    // Check if this is an endpoint request to the audio streaming endpoint.
    //
    if((ui32Recipient == USB_RTYPE_ENDPOINT) &&
       (psUSBRequest->wIndex == USBEPToIndex(psInst->ui8OUTEndpoint)))
    {
        //
        // Determine the type of request.
        //
        switch(psInst->ui8Request)
        {
            case USB_AC_SET_CUR:
            {
                //
                // Handle retrieving the sample rate.
                //
                if(psUSBRequest->wValue == SAMPLING_FREQ_CONTROL)
                {
                    //
                    // Retrieve the requested sample rate.
                    //
                    USBDCDRequestDataEP0(0,
                                         (uint8_t *)&psInst->ui32SampleRate,
                                         3);

                    //
                    // Save what we are updating.
                    //
                    psInst->ui16Update = SAMPLING_FREQ_CONTROL;
                }
                break;
            }
            case USB_AC_GET_CUR:
            {
                //
                // Handle retrieving the sample rate.
                //
                if(psUSBRequest->wValue == SAMPLING_FREQ_CONTROL)
                {
                    //
                    // Send back the current sample rate.
                    //
                    USBDCDSendDataEP0(0,
                                      (uint8_t *)&psInst->ui32SampleRate,
                                      3);
                }
                break;
            }
            default:
            {
                //
                // Stall on unknown commands.
                //
                ui32Stall = 1;
                break;
            }
        }
    }
    else if(ui32Recipient == USB_RTYPE_INTERFACE)
    {
        //
        // Make sure the request was for the control interface.
        //
        if((uint8_t)psUSBRequest->wIndex != psInst->ui8InterfaceControl)
        {
            return;
        }

        //
        // Extract the control value from the message.
        //
        ui32Control = psUSBRequest->wValue & USB_CS_CONTROL_M;

        //
        // Handle an audio control request to the feature control unit.
        //
        if((AUDIO_CONTROL_ID << 8) ==
           (psUSBRequest->wIndex & USB_CS_CONTROL_M))
        {
            //
            // Determine the type of request.
            //
            switch(psInst->ui8Request)
            {
                case USB_AC_GET_MAX:
                {
                    if(ui32Control == VOLUME_CONTROL)
                    {
                        //
                        // Return the maximum volume setting.
                        //
                        USBDCDSendDataEP0(0,
                                          (uint8_t *)&psInst->i16VolumeMax,
                                          2);
                    }
                    else
                    {
                        //
                        // Stall on unknown commands.
                        //
                        ui32Stall = 1;
                    }
                    break;
                }
                case USB_AC_GET_MIN:
                {
                    if(ui32Control == VOLUME_CONTROL)
                    {
                        //
                        // Return the minimum volume setting.
                        //
                        USBDCDSendDataEP0(0,
                                          (uint8_t *)&psInst->i16VolumeMin,
                                          2);
                    }
                    else
                    {
                        //
                        // Stall on unknown commands.
                        //
                        ui32Stall = 1;
                    }
                    break;
                }
                case USB_AC_GET_RES:
                {
                    if(ui32Control == VOLUME_CONTROL)
                    {
                        //
                        // Return the volume step setting.
                        //
                        USBDCDSendDataEP0(0,
                                          (uint8_t *)&psInst->i16VolumeStep,
                                          2);
                    }
                    else
                    {
                        //
                        // Stall on unknown commands.
                        //
                        ui32Stall = 1;
                    }
                    break;
                }
                case USB_AC_GET_CUR:
                {
                    if(ui32Control == VOLUME_CONTROL)
                    {
                        //
                        // Send back the current volume level.
                        //
                        USBDCDSendDataEP0(0,
                                          (uint8_t *)&psInst->i16Volume,
                                          2);
                    }
                    else if(ui32Control == MUTE_CONTROL)
                    {
                        //
                        // Send back the current mute value.
                        //
                        USBDCDSendDataEP0(0,
                                          (uint8_t *)&psInst->ui8Mute, 1);
                    }
                    else
                    {
                        //
                        // Stall on unknown commands.
                        //
                        ui32Stall = 1;
                    }
                    break;
                }
                case USB_AC_SET_CUR:
                {
                    if(ui32Control == VOLUME_CONTROL)
                    {
                        //
                        // Read the new volume level.
                        //
                        USBDCDRequestDataEP0(0,
                                             (uint8_t *)&psInst->i16Volume,
                                             2);

                        //
                        // Save what we are updating.
                        //
                        psInst->ui16Update = VOLUME_CONTROL;
                    }
                    else if(ui32Control == MUTE_CONTROL)
                    {
                        //
                        // Read the new mute setting.
                        //
                        USBDCDRequestDataEP0(0,
                                             (uint8_t *)&psInst->ui8Mute,
                                             1);

                        //
                        // Save what we are updating.
                        //
                        psInst->ui16Update = MUTE_CONTROL;
                    }
                    else
                    {
                        //
                        // Stall on unknown commands.
                        //
                        ui32Stall = 1;
                    }
                    break;
                }
                case USB_AC_SET_RES:
                {
                    if(ui32Control == VOLUME_CONTROL)
                    {
                        //
                        // Read the new volume step setting.
                        //
                        USBDCDRequestDataEP0(0,
                            (uint8_t *)&psInst->i16VolumeStep, 2);

                        //
                        // Save what we are updating.
                        //
                        psInst->ui16Update = VOLUME_CONTROL;
                    }
                    else
                    {
                        //
                        // Stall on unknown commands.
                        //
                        ui32Stall = 1;
                    }
                    break;
                }
                default:
                {
                    //
                    // Stall on unknown commands.
                    //
                    ui32Stall = 1;
                    break;
                }
            }
        }
    }

    //
    // Stall on all unknown commands.
    //
    if(ui32Stall)
    {
        USBDCDStallEP0(0);
    }
}

//*****************************************************************************
//
//! This function is used to supply buffers to the audio class to be filled
//! from the USB host device.
//!
//! \param pvAudioDevice is the pointer to the device instance structure as
//! returned by USBDAudioInit() or USBDAudioCompositeInit().
//! \param pvBuffer is a pointer to the buffer to fill with audio data.
//! \param ui32Size is the size in bytes of the buffer pointed to by the
//! \e pvBuffer
//! parameter.
//! \param pfnCallback is a callback that will provide notification when this
//! buffer has valid data.
//!
//! This function fills the buffer pointed to by the \e pvBuffer parameter with
//! at most \e ui32Size one packet of data from the host controller.  The
//! \e ui32Size has a minimum value of \b ISOC_OUT_EP_MAX_SIZE since each USB
//! packet can be at most \b ISOC_OUT_EP_MAX_SIZE bytes in size.  Since the
//! audio data may not be received in amounts that evenly fit in the buffer
//! provided, the buffer may not be completely filled.  The \e pfnCallback
//! function will provide the amount of valid data that was actually stored in
//! the buffer provided.  The function will return zero if the buffer could be
//! scheduled to be filled, otherwise the function will return a non-zero value
//! if there was some reason that the buffer could not be added.
//!
//! \return Returns 0 to indicate success any other value indicates that the
//! buffer will not be filled.
//
//*****************************************************************************
int32_t
USBAudioBufferOut(void *pvAudioDevice, void *pvBuffer, uint32_t ui32Size,
                  tUSBAudioBufferCallback pfnCallback)
{
    tAudioInstance *psInst;
    tUSBDAudioDevice *psAudioDevice;

    //
    // Make sure we were not passed NULL pointers.
    //
    ASSERT(pvAudioDevice != 0);
    ASSERT(pvBuffer != 0);

    //
    // Buffer must be at least one packet in size.
    //
    ASSERT(ui32Size >= ISOC_OUT_EP_MAX_SIZE);
    ASSERT(pfnCallback);

    //
    // The audio device structure pointer.
    //
    psAudioDevice = (tUSBDAudioDevice *)pvAudioDevice;

    //
    // Create a pointer to the audio instance data.
    //
    psInst = &psAudioDevice->sPrivateData;

    //
    // Initialize the buffer instance.
    //
    psInst->sBuffer.pvData = pvBuffer;
    psInst->sBuffer.ui32Size = ui32Size;
    psInst->sBuffer.ui32NumBytes = 0;
    psInst->sBuffer.pfnCallback = pfnCallback;

    return(0);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

