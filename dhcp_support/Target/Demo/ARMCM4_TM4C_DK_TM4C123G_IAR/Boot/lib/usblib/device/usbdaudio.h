//*****************************************************************************
//
// usbdaudio.h - USB audio device class driver.
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

#ifndef __USBDAUDIO_H__
#define __USBDAUDIO_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! \addtogroup audio_device_class_api
//! @{
//
//*****************************************************************************

typedef void (* tUSBAudioBufferCallback)(void *pvBuffer, uint32_t ui32Param,
                                         uint32_t ui32Event);

//*****************************************************************************
//
// PRIVATE
//
// This structure defines the private instance data and state variables for the
// audio device class.  The memory for this structure is pointed to by
// the pi16PrivateData field in the tUSBDAudioDevice structure passed on
// USBDAudioInit() and should not be modified by any code outside of the audio
// device.
//
//*****************************************************************************
typedef struct
{
    //
    // Base address for the USB controller.
    //
    uint32_t ui32USBBase;

    //
    // The device info to interact with the lower level DCD code.
    //
    tDeviceInfo sDevInfo;

    //
    // The maximum volume expressed as an 8.8 signed value.
    //
    int16_t i16VolumeMax;

    //
    // The minimum volume expressed as an 8.8 signed value.
    //
    int16_t i16VolumeMin;

    //
    // The minimum volume step expressed as an 8.8 signed value.
    //
    int16_t i16VolumeStep;

    struct
    {
        //
        // Pointer to a buffer provided by caller.
        //
        void *pvData;

        //
        // Size of the data area provided in pvData in bytes.
        //
        uint32_t ui32Size;

        //
        // Number of valid bytes copied into the pvData area.
        //
        uint32_t ui32NumBytes;

        //
        // The buffer callback for this function.
        //
        tUSBAudioBufferCallback pfnCallback;
    }
    sBuffer;

    //
    // Pending request type.
    //
    uint16_t ui16RequestType;

    //
    // Pending request.
    //
    uint8_t ui8Request;

    //
    // Pending update value.
    //
    uint16_t ui16Update;

    //
    // Current Volume setting.
    //
    int16_t i16Volume;

    //
    // Current Mute setting.
    //
    uint8_t ui8Mute;

    //
    // Current Sample rate, this is not writable but the host will try.
    //
    uint32_t ui32SampleRate;

    //
    // The OUT endpoint in use by this instance.
    //
    uint8_t ui8OUTEndpoint;

    //
    // The OUT endpoint DMA channel in use by this instance.
    //
    uint8_t ui8OUTDMA;

    //
    // The control interface number associated with this instance.
    //
    uint8_t ui8InterfaceControl;

    //
    // The audio interface number associated with this instance.
    //
    uint8_t ui8InterfaceAudio;

    //
    // A copy of the DMA instance data used with calls to USBLibDMA functions.
    //
    tUSBDMAInstance *psDMAInstance;
}
tAudioInstance;

//*****************************************************************************
//
// This is the size of the g_pui8IADAudioDescriptor array in bytes.
//
//*****************************************************************************
#define AUDIODESCRIPTOR_SIZE    (8)

//*****************************************************************************
//
// This is the size of the g_pui8AudioControlInterface array in bytes.
//
//*****************************************************************************
#define CONTROLINTERFACE_SIZE   (52)

//*****************************************************************************
//
// This is the size of the g_pui8AudioStreamInterface array in bytes.
//
//*****************************************************************************
#define STREAMINTERFACE_SIZE    (52)

//*****************************************************************************
//
//! The size of the memory that should be allocated to create a configuration
//! descriptor for a single instance of the USB Audio Device.
//! This does not include the configuration descriptor which is automatically
//! ignored by the composite device class.
//
//*****************************************************************************
#define COMPOSITE_DAUDIO_SIZE   (AUDIODESCRIPTOR_SIZE +                       \
                                 CONTROLINTERFACE_SIZE + STREAMINTERFACE_SIZE)

//*****************************************************************************
//
//! The structure used by the application to define operating parameters for
//! the device audio class.
//
//*****************************************************************************
typedef struct
{
    //
    //! The vendor ID that this device is to present in the device descriptor.
    //
    const uint16_t ui16VID;

    //
    //! The product ID that this device is to present in the device descriptor.
    //
    const uint16_t ui16PID;

    //
    //! 8 byte vendor string.
    //
    const char pcVendor[8];

    //
    //! 16 byte vendor string.
    //
    const char pcProduct[16];

    //
    //! 4 byte vendor string.
    //
    const char pcVersion[4];

    //
    //! The maximum power consumption of the device, expressed in mA.
    //
    const uint16_t ui16MaxPowermA;

    //
    //! Indicates whether the device is self or bus-powered and whether or not
    //! it supports remote wake up.  Valid values are USB_CONF_ATTR_SELF_PWR or
    //! USB_CONF_ATTR_BUS_PWR, optionally ORed with USB_CONF_ATTR_RWAKE.
    //
    const uint8_t ui8PwrAttributes;

    //
    //! A pointer to the callback function which will be called to notify
    //! the application of events relating to the operation of the audio
    //! device.
    //
    const tUSBCallback pfnCallback;

    //
    //! A pointer to the string descriptor array for this device.  This array
    //! must contain the following string descriptor pointers in this order.
    //! Language descriptor, Manufacturer name string (language 1), Product
    //! name string (language 1), Serial number string (language 1), Audio
    //! Interface description string (language 1), Configuration description
    //! string (language 1).
    //!
    //! If supporting more than 1 language, the descriptor block (except for
    //! string descriptor 0) must be repeated for each language defined in the
    //! language descriptor.
    //!
    //
    const uint8_t * const *ppui8StringDescriptors;

    //
    //! The number of descriptors provided in the ppStringDescriptors
    //! array.  This must be 1 + ((5 + (number of strings)) *
    //!                           (number of languages)).
    //
    const uint32_t ui32NumStringDescriptors;

    //
    //! The maximum volume expressed as an 8.8 signed value.
    //
    const int16_t i16VolumeMax;

    //
    //! The minimum volume expressed as an 8.8 signed value.
    //
    const int16_t i16VolumeMin;

    //
    //! The minimum volume step expressed as an 8.8 signed value.
    //
    const int16_t i16VolumeStep;

    //
    //! The private instance data for the audio device.
    //
    tAudioInstance sPrivateData;
}
tUSBDAudioDevice;

//*****************************************************************************
//
// Audio specific device class driver events
//
//*****************************************************************************

//*****************************************************************************
//
//! This USB audio event indicates that the device is connected but not active.
//
//*****************************************************************************
#define USBD_AUDIO_EVENT_IDLE   (USBD_AUDIO_EVENT_BASE + 0)

//*****************************************************************************
//
//! This USB audio event indicates that the device is connected and is now
//! active.
//
//*****************************************************************************
#define USBD_AUDIO_EVENT_ACTIVE (USBD_AUDIO_EVENT_BASE + 1)

//*****************************************************************************
//
//! This USB audio event indicates that the device is returning a data buffer
//! provided by the USBAudioBufferOut() function back to the application with
//! valid audio data received from the USB host controller. The \e pvBuffer
//! parameter holds the pointer to the buffer with the new audio data and
//! the \e ui32Param value holds the amount of valid data in bytes that are
//! contained in the \e pvBuffer parameter.
//
//*****************************************************************************
#define USBD_AUDIO_EVENT_DATAOUT (USBD_AUDIO_EVENT_BASE + 2)

//*****************************************************************************
//
//! This USB audio event indicates that a volume change has occurred.  The
//! \e ui32Param value contains a signed 8.8 fixed point value that represents
//! the current volume gain/attenuation in decibels(dB).  The provided message
//! handler should be prepared to handle negative and positive values with the
//! value 0x8000 indicating maximum attenuation.  The \e pvBuffer parameter
//! should be ignored.
//
//*****************************************************************************
#define USBD_AUDIO_EVENT_VOLUME (USBD_AUDIO_EVENT_BASE + 4)

//*****************************************************************************
//
//! This USB audio event indicates that a mute request has occurred.  The
//! \e ui32Param value will either be a 1 to indicate that the audio is now
//! muted, and a value of 0 indicates that the audio has been unmuted.
//
//*****************************************************************************
#define USBD_AUDIO_EVENT_MUTE   (USBD_AUDIO_EVENT_BASE + 5)

//*****************************************************************************
//
// API Function Prototypes
//
//*****************************************************************************
extern void *USBDAudioInit(uint32_t ui32Index,
                           tUSBDAudioDevice *psAudioDevice);
extern void *USBDAudioCompositeInit(uint32_t ui32Index,
                                    tUSBDAudioDevice *psAudioDevice,
                                    tCompositeEntry *psCompEntry);
extern void USBDAudioTerm(void *pvAudioDevice);
extern int32_t USBAudioBufferOut(void *pvAudioDevice, void *pvBuffer,
                                 uint32_t ui32Size,
                                 tUSBAudioBufferCallback pfnCallback);

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif

