//*****************************************************************************
//
// usbhaudio.c - USB host audio driver.
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
#include "inc/hw_types.h"
#include "driverlib/usbdrv.h"
#include "usblib/usblib.h"
#include "usblib/usblibpriv.h"
#include "usblib/usbaudio.h"
#include "usblib/host/usbhost.h"
#include "usblib/host/usbhostpriv.h"
#include "usblib/host/usbhaudio.h"

//*****************************************************************************
//
//! \addtogroup usblib_host_class
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// These defines are used with the USBHostAudioFormatSet()
// USBHostAudioFormatGet() to parse out interface number and alternate
// setting number for an interface.
//
//*****************************************************************************
#define INTERFACE_NUM_M         0x000000FF
#define INTERFACE_ALTSETTING_M  0x0000FF00
#define INTERFACE_ALTSETTING_S  8

//*****************************************************************************
//
// Used to indicate an invalid interface descriptor number.
//
//*****************************************************************************
#define INVALID_INTERFACE       0xffffffff

//*****************************************************************************
//
// Forward declarations for the driver open and close calls.
//
//*****************************************************************************
static void *USBAudioOpen(tUSBHostDevice *psDevice);
static void USBAudioClose(void *pvInstance);

//*****************************************************************************
//
// This is the structure for an instance of a USB host audio driver.
//
//*****************************************************************************
struct tUSBHostAudioInstance
{
    //
    // Save the device instance.
    //
    tUSBHostDevice *psDevice;

    //
    // Used to save the call back.
    //
    tUSBHostAudioCallback pfnCallback;

    //
    // This is the control interface.
    //
    uint8_t ui8IControl;

    //
    // This is the output streaming interface.
    //
    uint8_t ui8OutInterface;

    //
    // This is the currently selected active output interface used with
    // ui8OutInterface interface.
    //
    uint8_t ui8OutAltSetting;

    //
    // This is the streaming interface.
    //
    uint8_t ui8InInterface;

    //
    // This is the currently selected active input interface used with
    // ui8InInterface interface.
    //
    uint8_t ui8InAltSetting;

    //
    // The Isochronous endpoint addresses.
    //
    uint8_t ui8IsochInAddress;
    uint8_t ui8IsochOutAddress;

    tACInputTerminal *psInTerminal;
    tACOutputTerminal *psOutTerminal;

    //
    // Holds the identifier for the Feature Unit for controlling volume.
    //
    uint8_t ui8VolumeID;

    tACFeatureUnit *psFeatureUnit;

    //
    // Holds what types of controls are enabled on the device.
    //
    uint16_t pui16Controls[3];

    //
    // Isochronous IN pipe.
    //
    uint32_t ui32IsochInPipe;
    uint16_t ui16PipeSizeIn;
    tUSBHostAudioCallback pfnInCallback;
    void *pvInBuffer;

    //
    // Isochronous OUT pipe.
    //
    uint32_t ui32IsochOutPipe;
    uint16_t ui16PipeSizeOut;
    tUSBHostAudioCallback pfnOutCallback;
    void *pvOutBuffer;

    //
    // State flags for this audio instance.
    //
    uint32_t ui32Flags;
};

//*****************************************************************************
//
// The internal flags for an audio interface.
//
//*****************************************************************************
#define AUDIO_FLAG_OUT_ACTIVE   1   // Audio output is active.
#define AUDIO_FLAG_IN_ACTIVE    2   // Audio input is active.

//*****************************************************************************
//
// The USB Host audio instance.
//
//*****************************************************************************
static tUSBHostAudioInstance g_sAudioDevice =
{
    0
};

//*****************************************************************************
//
//! This constant global structure defines the Audio Class Driver that is
//! provided with the USB library.
//
//*****************************************************************************
const tUSBHostClassDriver g_sUSBHostAudioClassDriver =
{
    USB_CLASS_AUDIO,
    USBAudioOpen,
    USBAudioClose,
    0
};

//*****************************************************************************
//
// This is the internal function that handles callbacks from the USB IN pipe.
//
//*****************************************************************************
static void
PipeCallbackIN(uint32_t ui32Pipe, uint32_t ui32Event)
{
    //
    // Only handle the data available callback and pass it on to the
    // application.
    //
    if(ui32Event == USB_EVENT_RX_AVAILABLE)
    {
        if(g_sAudioDevice.pfnInCallback)
        {
            g_sAudioDevice.pfnInCallback(&g_sAudioDevice,
                                         USB_EVENT_RX_AVAILABLE, 0,
                                         g_sAudioDevice.pvInBuffer);
        }
    }
}

//*****************************************************************************
//
// This is the internal function that handles callbacks from the USB OUT pipe.
//
//*****************************************************************************
static void
PipeCallbackOUT(uint32_t ui32Pipe, uint32_t ui32Event)
{
    //
    // Only handle the transmit complete callback and pass it on to the
    // application.
    //
    if(ui32Event == USB_EVENT_TX_COMPLETE)
    {
        if(g_sAudioDevice.pfnOutCallback)
        {
            g_sAudioDevice.pfnOutCallback(&g_sAudioDevice,
                                          USB_EVENT_TX_COMPLETE, 0,
                                          g_sAudioDevice.pvOutBuffer);
        }
    }
}

//*****************************************************************************
//
// Finds a given terminal and type in an audio configuration descriptor.
//
//*****************************************************************************
static tDescriptorHeader *
AudioTerminalGet(tConfigDescriptor *psConfigDesc, uint32_t ui32Terminal,
                 uint32_t ui32TerminalType)
{
    tACOutputTerminal *psOutput;
    tDescriptorHeader *psHeader;
    int32_t i32BytesRemaining;

    psHeader = (tDescriptorHeader *)psConfigDesc;
    i32BytesRemaining = psConfigDesc->wTotalLength;

    while(i32BytesRemaining > 0)
    {
        //
        // Output and input terminals are the same past the bDescriptorSubtype
        // and wTerminalType that are being searched for.
        //
        psOutput = (tACOutputTerminal *)psHeader;

        //
        // Only CS_INTERFACE descriptors can be a terminal.
        //
        if((psHeader->bDescriptorType == USB_DTYPE_CS_INTERFACE) &&
           (ui32Terminal == psOutput->bDescriptorSubtype))
        {
            if((psOutput->bDescriptorSubtype == USB_AI_OUTPUT_TERMINAL) ||
               (psOutput->bDescriptorSubtype == USB_AI_INPUT_TERMINAL))

            {
                //
                // If this was the terminal type that was requested, the
                // return it.
                //
                if(psOutput->wTerminalType == ui32TerminalType)
                {
                    return(psHeader);
                }
            }
            else if(psOutput->bDescriptorSubtype == USB_AI_FEATURE_UNIT)
            {
                return(psHeader);
            }
        }

        //
        // Decrease the bytes remaining by the size of this descriptor.
        //
        i32BytesRemaining -=  psHeader->bLength;

        //
        // Move the pointer to the next header.
        //
        psHeader = (tDescriptorHeader *)((uint32_t)psHeader +
                                         psHeader->bLength);
    }
    return((tDescriptorHeader *)0);
}

//*****************************************************************************
//
// This function returns the interface number for the control interface
// in the structure passed in the psConfigDesc.
//
// \param psConfigDescriptor is a pointer to the memory containing a valid
//        configuration descriptor for a device.
//
// This function searches a configuration descriptor for a control interface
// descriptor.  The function only search for the first descriptor and then
// returns when it finds one.
//
// \return The first control interface descriptor number for an audio device
//         or INVALID_INTERFACE if no control interface descriptor was found.
//
//*****************************************************************************
static uint32_t
AudioControlGet(tConfigDescriptor *psConfigDesc)
{
    tDescriptorHeader *psHeader;
    tInterfaceDescriptor *psInterface;
    uint32_t ui32Interface;
    int32_t i32Bytes;

    psHeader = (tDescriptorHeader *)psConfigDesc;
    i32Bytes = psConfigDesc->wTotalLength;

    //
    // Initialize the interface number to an invalid value.
    //
    ui32Interface = INVALID_INTERFACE;

    //
    // Search the whole configuration descriptor.
    //
    while(i32Bytes > 0)
    {
        //
        // Find an interface descriptor and see if it is a control interface.
        //
        if(psHeader->bDescriptorType == USB_DTYPE_INTERFACE)
        {
            psInterface = (tInterfaceDescriptor *)psHeader;

            //
            // If this is the control interface then return the value to the
            // caller.
            //
            if(psInterface->bInterfaceSubClass == USB_ASC_AUDIO_CONTROL)
            {
                ui32Interface = psInterface->bInterfaceNumber;

                break;
            }
        }

        //
        // Decrease the bytes remaining by the size of this descriptor.
        //
        i32Bytes -= psHeader->bLength;

        //
        // Move the pointer to the next header.
        //
        psHeader = (tDescriptorHeader*)((uint32_t)psHeader +
                                        psHeader->bLength);
    }
    return(ui32Interface);
}

//*****************************************************************************
//
// If it exists, finds the correct audio interface for a given audio format.
//
//*****************************************************************************
static uint32_t
AudioGetInterface(tUSBHostAudioInstance *psAudioDevice, uint16_t ui16Format,
                  uint32_t ui32SampleRate, uint32_t ui32Bytes,
                  uint32_t ui32Channels, uint32_t ui32Flags)
{
    tDescriptorHeader *psHeader;
    tInterfaceDescriptor *psInterface;
    tEndpointDescriptor *pINEndpoint, *pOUTEndpoint;
    tACHeader *pACHeader;
    tACGeneral *pGeneral;
    tASFormat *pFormat;
    tEndpointDescriptor *pEndpoint;
    uint8_t *pui8Value;
    uint32_t ui32Value;
    int32_t i32Bytes, i32Idx;

    //
    // Initialize the Interface pointer to null.
    //
    psInterface = 0;
    pINEndpoint = 0;
    pOUTEndpoint = 0;

    //
    // Start at the top of the configuration descriptor.
    //
    psHeader = (tDescriptorHeader *)psAudioDevice->psDevice->psConfigDescriptor;

    i32Bytes = psAudioDevice->psDevice->psConfigDescriptor->wTotalLength;

    while(i32Bytes > 0)
    {
        if(psHeader->bDescriptorType == USB_DTYPE_INTERFACE)
        {
            //
            // If a new interface was found and the last one satisfied all
            // requirements then a valid interface was found so break out.
            //
            if(psInterface)
            {
                break;
            }

            //
            // Get the new interface pointer.
            //
            psInterface = (tInterfaceDescriptor *)psHeader;

            //
            // Reset the endpoints on finding a new interface descriptor.
            //
            pINEndpoint = 0;
            pOUTEndpoint = 0;

            //
            // If this is not a valid audio streaming interface then reset
            // the interface pointer to null.
            //
            if((psInterface->bNumEndpoints == 0) ||
               (psInterface->bInterfaceClass != USB_CLASS_AUDIO) ||
               (psInterface->bInterfaceSubClass != USB_ASC_AUDIO_STREAMING))
            {
                psInterface = 0;
            }
        }
        if((psInterface) &&
           (psHeader->bDescriptorType == USB_DTYPE_CS_INTERFACE))
        {
            pACHeader = (tACHeader *)psHeader;

            //
            // If this is a General descriptor the check if the format matches.
            //
            if(pACHeader->bDescriptorSubtype == USB_AS_GENERAL)
            {
                //
                // Just save the pointer to the format descriptor.
                //
                pGeneral = (tACGeneral *)psHeader;

                //
                // If this interface has the wrong format then set it to null
                // so that the rest of this interface is ignored.
                //
                if(pGeneral->wFormatTag != ui16Format)
                {
                    psInterface = 0;
                }
            }
            else if(pACHeader->bDescriptorSubtype == USB_AS_FORMAT_TYPE)
            {
                pFormat = (tASFormat *)psHeader;

                //
                // If the number of bytes per sample and number of channels do
                // not match then reset the interface pointer so that the rest
                // of this interface is ignored.
                //
                if((pFormat->bNrChannels != ui32Channels) ||
                   (pFormat->bSubFrameSize != ui32Bytes))
                {
                    psInterface = 0;
                }
                else
                {
                    pui8Value = &pFormat->tSamFreq;

                    //
                    // Attempt to find the sample rate in the sample rate
                    // table for this interface.
                    //
                    for(i32Idx = 0; i32Idx < pFormat->bSamFreqType; i32Idx++)
                    {
                        ui32Value = (*((uint32_t *)&pui8Value[i32Idx * 3]) &
                                    0xffffff);

                        if(ui32Value == ui32SampleRate)
                        {
                            break;
                        }
                    }

                    //
                    // If the sample rate was not found then set the interface
                    // pointer to null so that the rest of this interface is
                    // ignored.
                    //
                    if(i32Idx == pFormat->bSamFreqType)
                    {
                        psInterface = 0;
                    }
                }
            }
        }
        else if((psInterface) &&
                (psHeader->bDescriptorType == USB_DTYPE_ENDPOINT))
        {
            pEndpoint = (tEndpointDescriptor *)psHeader;

            //
            // See what direction is being requested.
            //
            if(ui32Flags & USBH_AUDIO_FORMAT_IN)
            {
                //
                // If this is an input endpoint and is just a feed back input
                // then ignore it.
                //
                if(pEndpoint->bEndpointAddress & USB_EP_DESC_IN)
                {
                    if((pEndpoint->bmAttributes & USB_EP_ATTR_USAGE_M)
                        == USB_EP_ATTR_USAGE_FEEDBACK)
                    {
                        psInterface = 0;
                    }
                    else
                    {
                        //
                        // Save this endpoint as a possible valid endpoint
                        //
                        pINEndpoint = pEndpoint;
                    }
                }
            }
            else
            {
                //
                // If this is an output endpoint and is just a feed back input
                // then ignore it.
                //
                if((pEndpoint->bEndpointAddress & USB_EP_DESC_IN) == 0)
                {
                    if((pEndpoint->bmAttributes & USB_EP_ATTR_USAGE_M)
                        == USB_EP_ATTR_USAGE_FEEDBACK)
                    {
                        psInterface = 0;
                    }
                    else
                    {
                        //
                        // Save this endpoint as a possible valid endpoint;
                        //
                        pOUTEndpoint = pEndpoint;
                    }
                }
            }
        }

        //
        // Decrease the bytes remaining by the size of this descriptor.
        //
        i32Bytes -= psHeader->bLength;

        //
        // Move the pointer to the next header.
        //
        psHeader = (tDescriptorHeader*)((uint32_t)psHeader +
                                        psHeader->bLength);
    }

    //
    // If there is still a valid interface then return the values.
    //
    if(psInterface)
    {
        //
        // Check a valid IN endpoint descriptor.
        //
        if(pINEndpoint)
        {
            //
            // Save the endpoint address.
            //
            g_sAudioDevice.ui8IsochInAddress = pINEndpoint->bEndpointAddress &
                                               USB_EP_DESC_NUM_M;

            //
            // If there is no current pipe then just allocate a new one with
            // the settings for this interface.
            //
            if(g_sAudioDevice.ui32IsochInPipe == 0)
            {
                //
                // Allocate the USB Pipe for this Isochronous IN end point.
                //
                g_sAudioDevice.ui32IsochInPipe =
                    USBHCDPipeAllocSize(0, USBHCD_PIPE_ISOC_IN_DMA,
                                        g_sAudioDevice.psDevice,
                                        pINEndpoint->wMaxPacketSize,
                                        PipeCallbackIN);
            }
            else if(g_sAudioDevice.ui16PipeSizeIn < pINEndpoint->wMaxPacketSize)
            {
                //
                // Free the old endpoint and allocate a new one.
                //
                USBHCDPipeFree(g_sAudioDevice.ui32IsochInPipe);

                //
                // Allocate the USB Pipe for this Isochronous IN end point.
                //
                g_sAudioDevice.ui32IsochInPipe =
                    USBHCDPipeAllocSize(0, USBHCD_PIPE_ISOC_IN_DMA,
                                        g_sAudioDevice.psDevice,
                                        pINEndpoint->wMaxPacketSize,
                                        PipeCallbackIN);

                //
                // Save the new size of the maximum packet size for this
                // USB pipe.
                //
                g_sAudioDevice.ui16PipeSizeIn = pINEndpoint->wMaxPacketSize;
            }

            //
            // Configure the USB pipe as a Isochronous IN end point.
            //
            USBHCDPipeConfig(g_sAudioDevice.ui32IsochInPipe,
                             pINEndpoint->wMaxPacketSize,
                             0,
                             g_sAudioDevice.ui8IsochInAddress);
        }

        //
        // Check a valid OUT endpoint descriptor.
        //
        if(pOUTEndpoint)
        {
            //
            // Save the endpoint address.
            //
            g_sAudioDevice.ui8IsochOutAddress =
                            pOUTEndpoint->bEndpointAddress & USB_EP_DESC_NUM_M;

            //
            // If there is no current pipe then just allocate a new one with
            // the settings for this interface.
            //
            if(g_sAudioDevice.ui32IsochOutPipe == 0)
            {
                //
                // Allocate the USB Pipe for this Isochronous OUT end point.
                //
                g_sAudioDevice.ui32IsochOutPipe =
                    USBHCDPipeAllocSize(0, USBHCD_PIPE_ISOC_OUT_DMA,
                                        g_sAudioDevice.psDevice,
                                        pOUTEndpoint->wMaxPacketSize,
                                        PipeCallbackOUT);
            }
            else if(g_sAudioDevice.ui16PipeSizeOut <
                    pOUTEndpoint->wMaxPacketSize)
            {
                //
                // Free the old endpoint and allocate a new one.
                //
                USBHCDPipeFree(g_sAudioDevice.ui32IsochOutPipe);

                //
                // Allocate the USB Pipe for this Isochronous OUT end point.
                //
                g_sAudioDevice.ui32IsochOutPipe =
                    USBHCDPipeAllocSize(0, USBHCD_PIPE_ISOC_OUT_DMA,
                                        g_sAudioDevice.psDevice,
                                        pOUTEndpoint->wMaxPacketSize,
                                        PipeCallbackOUT);

                //
                // Save the new size of the maximum packet size for this
                // USB pipe.
                //
                g_sAudioDevice.ui16PipeSizeOut = pOUTEndpoint->wMaxPacketSize;
            }

            //
            // Configure the USB pipe as a Isochronous OUT end point.
            //
            USBHCDPipeConfig(g_sAudioDevice.ui32IsochOutPipe,
                             pOUTEndpoint->wMaxPacketSize, 0,
                             g_sAudioDevice.ui8IsochOutAddress);
        }

        return(psInterface->bInterfaceNumber |
               (psInterface->bAlternateSetting << INTERFACE_ALTSETTING_S));
    }
    return(INVALID_INTERFACE);
}

//*****************************************************************************
//
// This function is used to open an instance of the USB host audio driver.
//
// \param psDevice is a pointer to the device information structure.
//
// This function attempts to open an instance of the USB host audio driver
// based on the information contained in the psDevice structure.  This call
// fails if there are not sufficient resources to open the device.  The
// function returns a value that should be passed back into USBHostAudioClose()
// when the driver is no longer needed.
//
// \return The function returns a pointer to a USB host audio driver
// instance.
//
//*****************************************************************************
static void *
USBAudioOpen(tUSBHostDevice *psDevice)
{
    uint32_t ui32Temp;
    tConfigDescriptor *psConfigDesc;

    //
    // Don't allow the device to be opened without closing first.
    //
    if(g_sAudioDevice.psDevice)
    {
        return(0);
    }

    //
    // Save the Host device pointer.
    //
    g_sAudioDevice.psDevice = psDevice;

    //
    // Save a shorter name for the configuration descriptor.
    //
    psConfigDesc = psDevice->psConfigDescriptor;

    //
    // Find the input terminal.
    //
    g_sAudioDevice.psInTerminal =
        (tACInputTerminal *)AudioTerminalGet(psConfigDesc,
                                             USB_AI_INPUT_TERMINAL,
                                             USB_TTYPE_STREAMING);

    //
    // Find the output terminal.
    //
    g_sAudioDevice.psOutTerminal =
        (tACOutputTerminal *)AudioTerminalGet(psConfigDesc,
                                              USB_AI_OUTPUT_TERMINAL,
                                              USB_TTYPE_STREAMING);

    //
    // Find the feature unit.
    g_sAudioDevice.psFeatureUnit =
        (tACFeatureUnit *)AudioTerminalGet(psConfigDesc,
                                             USB_AI_FEATURE_UNIT, 0);

    //
    // Need some kind of terminal to send or receive audio from.
    //
    if((g_sAudioDevice.psOutTerminal == 0) &&
       (g_sAudioDevice.psInTerminal == 0))
    {
        return(0);
    }

    //
    // Find the Audio control interface.
    //
    ui32Temp = AudioControlGet(psConfigDesc);

    if(ui32Temp == INVALID_INTERFACE)
    {
        return(0);
    }

    //
    // Save the control interface index and increment the number
    // of interfaces that have been found.
    //
    g_sAudioDevice.ui8IControl = (uint8_t)ui32Temp;

    //
    // If the call back exists, call it with an Open event.
    //
    if(g_sAudioDevice.pfnCallback != 0)
    {
        g_sAudioDevice.pfnCallback(&g_sAudioDevice,
                                   USBH_AUDIO_EVENT_OPEN, 0, 0);
    }

    //
    // If a feature unit was found, save the ID
    //
    if(g_sAudioDevice.psFeatureUnit != 0)
    {
        g_sAudioDevice.ui8VolumeID = g_sAudioDevice.psFeatureUnit->bUnitID;
    }

    //
    // Allocate the USB Pipe for this Isochronous IN end point.
    //
    g_sAudioDevice.ui32IsochInPipe =
        USBHCDPipeAllocSize(0, USBHCD_PIPE_ISOC_IN_DMA,
                            g_sAudioDevice.psDevice, 256, PipeCallbackIN);
    g_sAudioDevice.ui16PipeSizeIn = 256;

    //
    // Allocate the USB Pipe for this Isochronous OUT end point.
    //
    g_sAudioDevice.ui32IsochOutPipe =
        USBHCDPipeAllocSize(0, USBHCD_PIPE_ISOC_OUT_DMA,
                            g_sAudioDevice.psDevice, 256, PipeCallbackOUT);
    g_sAudioDevice.ui16PipeSizeOut = 256;

    //
    // Clear the flags.
    //
    g_sAudioDevice.ui32Flags = 0;

    //
    // Return the only instance of this device.
    //
    return(&g_sAudioDevice);
}

//*****************************************************************************
//
// This function is used to release an instance of the USB host audio driver.
//
// \param pvAudioDevice is an instance pointer that needs to be released.
//
// This function frees up any resources in use by the USB host audio
// driver instance that is passed in.  The \e pvAudioDevice pointer should be a
// valid value that was returned from a call to USBHostAudioOpen().
//
// \return None.
//
//*****************************************************************************
static void
USBAudioClose(void *pvAudioDevice)
{
    tUSBHostAudioInstance *psAudioDevice;

    psAudioDevice = (tUSBHostAudioInstance *)pvAudioDevice;

    //
    // Do nothing if there is not a driver open.
    //
    if(psAudioDevice->psDevice == 0)
    {
        return;
    }

    //
    // Reset the device pointer.
    //
    psAudioDevice->psDevice = 0;

    //
    // Free the Isochronous IN pipe.
    //
    if(psAudioDevice->ui32IsochInPipe != 0)
    {
        USBHCDPipeFree(psAudioDevice->ui32IsochInPipe);
    }

    //
    // Free the Isochronous OUT pipe.
    //
    if(psAudioDevice->ui32IsochOutPipe != 0)
    {
        USBHCDPipeFree(psAudioDevice->ui32IsochOutPipe);
    }

    //
    // If the call back exists then call it.
    //
    if(psAudioDevice->pfnCallback != 0)
    {
        psAudioDevice->pfnCallback(psAudioDevice, USBH_AUDIO_EVENT_CLOSE, 0,
                                   0);
    }
}

//*****************************************************************************
//
//! This function should be called before any devices are present to enable
//! the host audio class driver.
//!
//! \param ui32Index is the audio device to open (currently only 0 is
//! supported).
//! \param pfnCallback is the driver call back for host audio events.
//!
//! This function is called to open an instance of a host audio device and
//! should provide a valid callback function for host audio events in the
//! \e pfnCallback parameter.  This function must be called before the USB
//! host code can successfully enumerate an audio device.
//!
//! \return This function returns the driver instance to use for the other
//! host audio functions.  If there is no instance available at the time of
//! this call, this function returns zero.
//
//*****************************************************************************
tUSBHostAudioInstance *
USBHostAudioOpen(uint32_t ui32Index, tUSBHostAudioCallback pfnCallback)
{
    //
    // Only one audio device is supported at this time and on one instance
    // is supported so if there is already a call back then fail.
    //
    if((ui32Index != 0) || (g_sAudioDevice.pfnCallback))
    {
        return(0);
    }

    //
    // Save the call back.
    //
    g_sAudioDevice.pfnCallback = pfnCallback;

    //
    // Return the requested device instance.
    //
    return(&g_sAudioDevice);
}

//*****************************************************************************
//
//! This function should be called to release an audio device instance.
//!
//! \param psAudioInstance is the device instance that is to be released.
//!
//! This function is called when a host audio device needs to be released.
//! This could be in preparation for shutdown or a switch to USB device mode,
//! for example.  Following this call, the audio device is available and can
//! be opened again using a call to USBHostAudioOpen().  After calling this
//! function, the host audio driver will no longer provide any callbacks or
//! accept calls to other audio driver APIs.
//!
//! \return None.
//
//*****************************************************************************
void
USBHostAudioClose(tUSBHostAudioInstance *psAudioInstance)
{
    //
    // Close the audio device.
    //
    USBAudioClose(psAudioInstance);

    //
    // Clear the call back indicating that the device is now closed.
    //
    psAudioInstance->pfnCallback = 0;
}

//*****************************************************************************
//
// This function is used to request settings from a given audio interface.
//
// \param psAudioDevice is the audio device instance to access.
// \param ui32Interface is the interface to access.
// \param ui32Channel is the channel number to access.
// \param ui32Request is the audio device request.
//
// This function is used to get volume control parameters from a given
// interface and on a given channel.  The \e ui32Interface is the interface to
// make the request specified by \e ui32Channel and \e ui32Request.  The
// \e ui32Request parameter must be one of the USB_AC_GET_* values.
//
// \return This function returns the requested value.
//
//*****************************************************************************
static uint32_t
VolumeSettingGet(tUSBHostAudioInstance *psAudioDevice, uint32_t ui32Interface,
                 uint32_t ui32Channel, uint32_t ui32Request)
{
    uint32_t ui32Value;
    tUSBRequest sSetupPacket;

    ui32Value = 0;

    //
    // This is a Class specific Interface IN request.
    //
    sSetupPacket.bmRequestType = USB_RTYPE_DIR_IN | USB_RTYPE_CLASS |
                                 USB_RTYPE_INTERFACE;

    //
    // Request a Device Descriptor.
    //
    sSetupPacket.bRequest = (ui32Request & 0xff);

    //
    // Request for a string descriptor.
    //
    sSetupPacket.wValue = VOLUME_CONTROL | (ui32Channel & 0xff);

    //
    // Set the language ID.
    //
    sSetupPacket.wIndex = (psAudioDevice->ui8VolumeID << 8) |
                          (ui32Interface & 0xff);

    //
    // Only request the space available.
    //
    sSetupPacket.wLength = 2;

    //
    // Put the setup packet in the buffer.
    //
    USBHCDControlTransfer(0, &sSetupPacket, psAudioDevice->psDevice,
                (uint8_t *)&ui32Value, 4,
                psAudioDevice->psDevice->sDeviceDescriptor.bMaxPacketSize0);

    return(ui32Value);
}

//*****************************************************************************
//
//! This function is used to get the current volume setting for a given
//! audio device.
//!
//! \param psAudioInstance is an instance of the USB audio device.
//! \param ui32Interface is the interface number to use to query the current
//! volume setting.
//! \param ui32Channel is the 0 based channel number to query.
//!
//! The function is used to retrieve the current volume setting for an audio
//! device on the channel specified by \e ui32Channel.  The \e ui32Interface is
//! ignored for now and should be set to 0 to access the default audio control
//! interface.  The \e ui32Channel value starts with 0 which is the master
//! audio volume control interface.  The remaining \e ui32Channel values
//! provide access to various other audio channels, with 1 and 2 being left and
//! right audio channels.
//!
//! \note On devices that do not support volume control interfaces, this
//! call returns 0, indicating a 0db setting.
//!
//! \return Returns the current volume setting for the requested interface.
//
//*****************************************************************************
uint32_t
USBHostAudioVolumeGet(tUSBHostAudioInstance *psAudioInstance,
                      uint32_t ui32Interface, uint32_t ui32Channel)
{
    return(VolumeSettingGet(psAudioInstance, ui32Interface, ui32Channel,
                            USB_AC_GET_CUR));
}

//*****************************************************************************
//
//! This function is used to get the maximum volume setting for a given
//! audio device.
//!
//! \param psAudioInstance is an instance of the USB audio device.
//! \param ui32Interface is the interface number to use to query the maximum
//! volume control value.
//! \param ui32Channel is the 0 based channel number to query.
//!
//! The function is used to retrieve the maximum volume setting for an audio
//! device on the channel specified by \e ui32Channel.  The \e ui32Interface is
//! ignored for now and should be set to 0 to access the default audio control
//! interface.  The \e ui32Channel value starts with 0 which is the master
//! audio volume control interface.  The remaining \e ui32Channel values
//! provide access to various other audio channels, with 1 and 2 being left and
//! right audio channels.
//!
//! \note On devices that do not support volume control interfaces, this
//! call returns 0, indicating a 0db setting.
//!
//! \return Returns the maximum volume setting for the requested interface.
//
//*****************************************************************************
uint32_t
USBHostAudioVolumeMaxGet(tUSBHostAudioInstance *psAudioInstance,
                         uint32_t ui32Interface, uint32_t ui32Channel)
{
    return(VolumeSettingGet(psAudioInstance, ui32Interface, ui32Channel,
                            USB_AC_GET_MAX));
}

//*****************************************************************************
//
//! This function is used to get the minimum volume setting for a given
//! audio device.
//!
//! \param psAudioInstance is an instance of the USB audio device.
//! \param ui32Interface is the interface number to use to query the minimum
//! volume control value.
//! \param ui32Channel is the 0 based channel number to query.
//!
//! The function is used to retrieve the minimum volume setting for an audio
//! device on the channel specified by \e ui32Channel.  The \e ui32Interface is
//! ignored for now and should be set to 0 to access the default audio control
//! interface.  The \e ui32Channel value starts with 0 which is the master
//! audio volume control interface.  The remaining \e ui32Channel values
//! provide access to various other audio channels, with 1 and 2 being left and
//! right audio channels.
//!
//! \note On devices that do not support volume control interfaces, this
//! call returns 0, indicating a 0db setting.
//!
//! \return Returns the minimum volume setting for the requested interface.
//
//*****************************************************************************
uint32_t
USBHostAudioVolumeMinGet(tUSBHostAudioInstance *psAudioInstance,
                         uint32_t ui32Interface, uint32_t ui32Channel)
{
    return(VolumeSettingGet(psAudioInstance, ui32Interface, ui32Channel,
                            USB_AC_GET_MIN));
}

//*****************************************************************************
//
//! This function is used to get the volume control resolution for a given
//! audio device.
//!
//! \param psAudioInstance is an instance of the USB audio device.
//! \param ui32Interface is the interface number to use to query the resolution
//! for the volume control.
//! \param ui32Channel is the 0 based channel number to query.
//!
//! The function is used to retrieve the volume control resolution for an audio
//! device on the channel specified by \e ui32Channel.  The \e ui32Interface is
//! ignored for now and should be set to 0 to access the default audio control
//! interface.  The \e ui32Channel value starts with 0 which is the master
//! audio volume control interface.  The remaining \e ui32Channel values
//! provide access to various other audio channels, with 1 and 2 being left and
//! right audio channels.
//!
//! \note On devices that do not support volume control interfaces, this
//! call returns 0, indicating a 0db setting.
//!
//! \return Returns the volume control resolution for the requested interface.
//
//*****************************************************************************
uint32_t
USBHostAudioVolumeResGet(tUSBHostAudioInstance *psAudioInstance,
                         uint32_t ui32Interface, uint32_t ui32Channel)
{
    return(VolumeSettingGet(psAudioInstance, ui32Interface, ui32Channel,
                            USB_AC_GET_RES));
}

//*****************************************************************************
//
//! This function is used to set the current volume setting for a given
//! audio device.
//!
//! \param psAudioInstance is an instance of the USB audio device.
//! \param ui32Interface is the interface number to use to set the current
//! volume setting.
//! \param ui32Channel is the 0 based channel number to query.
//! \param ui32Value is the value to write to the USB audio device.
//!
//! The function is used to set the current volume setting for an audio
//! device on the channel specified by \e ui32Channel.  The \e ui32Interface is
//! ignored for now and should be set to 0 to access the default audio control
//! interface.  The \e ui32Channel value starts with 0 which is the master
//! audio volume control interface.  The remaining \e ui32Channel values
//! provide access to various other audio channels, with 1 and 2 being left and
//! right audio channels.
//!
//! \note On devices that do not support volume control interfaces, this
//! call returns 0, indicating a 0db setting.
//!
//! \return None.
//
//*****************************************************************************
void
USBHostAudioVolumeSet(tUSBHostAudioInstance *psAudioInstance,
                      uint32_t ui32Interface, uint32_t ui32Channel,
                      uint32_t ui32Value)
{
    tUSBRequest sSetupPacket;

    //
    // This is a Class specific Interface OUT request.
    //
    sSetupPacket.bmRequestType = USB_RTYPE_DIR_OUT | USB_RTYPE_CLASS |
                                 USB_RTYPE_INTERFACE;

    //
    // Request is to set the current value.
    //
    sSetupPacket.bRequest = USB_AC_SET_CUR;

    //
    // Request the volume control.
    //
    sSetupPacket.wValue = VOLUME_CONTROL | (ui32Channel & 0xff);

    //
    // Set Volume control ID and interface to 0.
    //
    sSetupPacket.wIndex = psAudioInstance->ui8VolumeID << 8;

    //
    // Only request the space available.
    //
    sSetupPacket.wLength = 2;

    //
    // Put the setup packet in the buffer.
    //
    USBHCDControlTransfer(0, &sSetupPacket, psAudioInstance->psDevice,
            (uint8_t *)&ui32Value, 2,
            psAudioInstance->psDevice->sDeviceDescriptor.bMaxPacketSize0);
}

//*****************************************************************************
//
//! This function is called to determine if an audio format is supported by the
//! connected USB Audio device.
//!
//! \param psAudioInstance is the device instance for this call.
//! \param ui32SampleRate is the sample rate of the audio stream.
//! \param ui32Bits is the number of bits per sample in the audio stream.
//! \param ui32Channels is the number of channels in the audio stream.
//! \param ui32Flags is a set of flags to determine what type of interface to
//!        retrieve.
//!
//! This function is called when an application needs to determine which audio
//! formats are supported by a USB audio device that has been connected.  The
//! \e psAudioInstance value that is used with this call is the value that was
//! returned from the USBHostAudioOpen() function.  This call checks the
//! USB audio device to determine if it can support the values provided in the
//! \e ui32SampleRate, \e ui32Bits, and \e ui32Channels values.  The
//! \e ui32Flags currently only supports either the \b USBH_AUDIO_FORMAT_IN or
//! \b USBH_AUDIO_FORMAT_OUT values that indicates if a request is for an
//! audio input and an audio output.  If the format is supported this
//! function returns zero, and this function returns a non-zero value if the
//! format is not supported.  This function does not set the current output or
//! input format.
//!
//! \return A value of zero indicates the supplied format is supported and
//! a non-zero value indicates that the format is not supported.
//
//*****************************************************************************
uint32_t
USBHostAudioFormatGet(tUSBHostAudioInstance *psAudioInstance,
                      uint32_t ui32SampleRate, uint32_t ui32Bits,
                      uint32_t ui32Channels, uint32_t ui32Flags)
{
    //
    // Look for the requested format.
    //
    if(AudioGetInterface(psAudioInstance, USB_ADF_PCM, ui32SampleRate,
                         ui32Bits >> 3, ui32Channels, ui32Flags) !=
       INVALID_INTERFACE)
    {
        return(0);
    }
    return(1);
}

//*****************************************************************************
//
//! This function is called to set the current sample rate on an audio
//! interface.
//!
//! \param psAudioInstance specifies the device instance for this call.
//! \param ui32SampleRate is the sample rate in Hz.
//! \param ui32Bits is the number of bits per sample.
//! \param ui32Channels is then number of audio channels.
//! \param ui32Flags is a set of flags that determine the access type.
//!
//! This function is called when to set the current audio output or input
//! format for a USB audio device.  The \e psAudioInstance value that is used
//! with this call is the value that was returned from the USBHostAudioOpen()
//! function.  The application can use this call to insure that the audio
//! format is supported and set the format at the same time.  If the
//! application is just checking for supported rates, then it should call the
//! USBHostAudioFormatGet().
//!
//! \note This function must be called before attempting to send or receive
//! audio with the USBHostAudioPlay() or USBHostAudioRecord() functions.
//!
//! \return A non-zero value indicates the supplied format is not supported and
//! a zero value indicates that the format was supported and has been
//! configured.
//
//*****************************************************************************
uint32_t
USBHostAudioFormatSet(tUSBHostAudioInstance *psAudioInstance,
                      uint32_t ui32SampleRate, uint32_t ui32Bits,
                      uint32_t ui32Channels, uint32_t ui32Flags)
{
    uint32_t ui32Interface;

    //
    // Look for the requested format.
    //
    ui32Interface = AudioGetInterface(psAudioInstance, USB_ADF_PCM,
                                      ui32SampleRate, ui32Bits >> 3,
                                      ui32Channels, ui32Flags);

    if(ui32Interface == INVALID_INTERFACE)
    {
        return(1);
    }

    //
    // Determine if this is an input or output request.
    //
    if(ui32Flags & USBH_AUDIO_FORMAT_IN)
    {
        //
        // Get the active interface number and alternate setting for this
        // format.
        //
        psAudioInstance->ui8InInterface =
            (uint8_t)(ui32Interface & INTERFACE_NUM_M);
        psAudioInstance->ui8InAltSetting =
            (uint8_t)((ui32Interface & INTERFACE_ALTSETTING_M) >>
            INTERFACE_ALTSETTING_S);
    }
    else
    {
        //
        // Get the active interface number and alternate setting for this
        // format.
        //
        psAudioInstance->ui8OutInterface =
            (uint8_t)(ui32Interface & INTERFACE_NUM_M);
        psAudioInstance->ui8OutAltSetting =
            (uint8_t)((ui32Interface & INTERFACE_ALTSETTING_M) >>
            INTERFACE_ALTSETTING_S);
    }
    return(0);
}

//*****************************************************************************
//
//! This function is called to send an audio buffer to the USB audio device.
//!
//! \param psAudioInstance specifies the device instance for this call.
//! \param pvBuffer is the audio buffer to send.
//! \param ui32Size is the size of the buffer in bytes.
//! \param pfnCallback is a pointer to a callback function that is called
//! when the buffer can be used again.
//!
//! This function is called when an application needs to schedule a new buffer
//! for output to the USB audio device.  Since this call schedules the transfer
//! and returns immediately, the application should provide a \e pfnCallback
//! function to be notified when the buffer can be used again by the
//! application.  The \e pfnCallback function provided is called with the
//! \e pvBuffer parameter set to the \e pvBuffer provided by this call, the
//! \e ui32Param can be ignored and the \e ui32Event parameter is
//! \b USB_EVENT_TX_COMPLETE.
//!
//! \return This function returns the number of bytes that were scheduled
//! to be sent.  If this function returns zero then there was no USB audio
//! device present or the request could not be satisfied at this time.
//
//*****************************************************************************
int32_t
USBHostAudioPlay(tUSBHostAudioInstance *psAudioInstance, void *pvBuffer,
                 uint32_t ui32Size, tUSBHostAudioCallback pfnCallback)
{
    uint32_t ui32Bytes;

    //
    // Make sure that there is a device present.
    //
    if(psAudioInstance->psDevice == 0)
    {
        return(0);
    }

    //
    // If the audio output interface is not active then select the current
    // active audio interface.
    //
    if(HWREGBITW(&psAudioInstance->ui32Flags, AUDIO_FLAG_OUT_ACTIVE) == 0)
    {
        //
        // Indicate the active audio interface has been selected.
        //
        HWREGBITW(&psAudioInstance->ui32Flags, AUDIO_FLAG_OUT_ACTIVE) = 1;

        //
        // Configure the USB audio device to use the selected audio interface.
        //
        USBHCDSetInterface(0, (uint32_t)psAudioInstance->psDevice,
                           psAudioInstance->ui8OutInterface,
                           psAudioInstance->ui8OutAltSetting);
    }

    //
    // Save the callback function and the buffer pointer.
    //
    psAudioInstance->pfnOutCallback = pfnCallback;
    psAudioInstance->pvOutBuffer = (void *)pvBuffer;

    //
    // Schedule the data to be written out to the FIFO.
    //
    ui32Bytes = USBHCDPipeSchedule(psAudioInstance->ui32IsochOutPipe, pvBuffer,
                                   ui32Size);

    //
    // Return the number of bytes scheduled to be sent.
    //
    return(ui32Bytes);
}

//*****************************************************************************
//
//! This function is called to provide an audio buffer to the USB audio device
//! for audio input.
//!
//! \param psAudioInstance specifies the device instance for this call.
//! \param pvBuffer is the audio buffer to send.
//! \param ui32Size is the size of the buffer in bytes.
//! \param pfnCallback is a pointer to a callback function that is called
//! when the buffer has been filled.
//!
//! This function is called when an application needs to schedule a new buffer
//! for input from the USB audio device.  Since this call schedules the
//! transfer and returns immediately, the application should provide a
//! \e pfnCallback function to be notified when the buffer has been filled with
//! audio data.  When the \e pfnCallback function is called, the \e pvBuffer
//! parameter is set to \e pvBuffer provided in this call, the \e ui32Param is
//! the number of valid bytes in the pvBuffer and the \e ui32Event is set to
//! \b USB_EVENT_RX_AVAILABLE.
//!
//! \return This function returns the number of bytes that were scheduled
//! to be sent.  If this function returns zero then there was no USB audio
//! device present or the device does not support audio input.
//
//*****************************************************************************
int32_t
USBHostAudioRecord(tUSBHostAudioInstance *psAudioInstance, void *pvBuffer,
                   uint32_t ui32Size, tUSBHostAudioCallback pfnCallback)
{
    uint32_t ui32Bytes;

    //
    // Make sure that there is a device present.
    //
    if(psAudioInstance->psDevice == 0)
    {
        return(0);
    }

    //
    // If the audio input interface is not active then select the current
    // active audio interface.
    //
    if(HWREGBITW(&psAudioInstance->ui32Flags, AUDIO_FLAG_IN_ACTIVE) == 0)
    {
        //
        // Indicate the active audio interface has been selected.
        //
        HWREGBITW(&psAudioInstance->ui32Flags, AUDIO_FLAG_IN_ACTIVE) = 1;

        //
        // Configure the USB audio device to use the selected audio interface.
        //
        USBHCDSetInterface(0, (uint32_t)psAudioInstance->psDevice,
                        psAudioInstance->ui8InInterface,
                        psAudioInstance->ui8InAltSetting);
    }

    //
    // Save the callback function and the buffer pointer.
    //
    psAudioInstance->pfnInCallback = pfnCallback;
    psAudioInstance->pvInBuffer = (void *)pvBuffer;

    //
    // Schedule the data to be written out to the FIFO.
    //
    ui32Bytes = USBHCDPipeSchedule(psAudioInstance->ui32IsochInPipe, pvBuffer,
                                   ui32Size);

    //
    // Return the number of bytes scheduled to be sent.
    //
    return(ui32Bytes);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

