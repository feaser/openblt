//*****************************************************************************
//
// usbhaudio.h - USB host audio class driver.
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

#ifndef __USBHAUDIO_H__
#define __USBHAUDIO_H__

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
//! \addtogroup usblib_host_class
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// USB host audio specific events
//
//*****************************************************************************

//*****************************************************************************
//
//! This USB host audio event indicates that the device is connected and
//! ready to send or receive buffers.  The \e pvBuffer and \e ui32Param
//! values are not used in this event.
//
//*****************************************************************************
#define USBH_AUDIO_EVENT_OPEN   (USBH_AUDIO_EVENT_BASE + 0)

//*****************************************************************************
//
//! This USB host audio event indicates that the previously connected device
//! has been disconnected. The \e pvBuffer and \e ui32Param values are not used
//! in this event.
//
//*****************************************************************************
#define USBH_AUDIO_EVENT_CLOSE  (USBH_AUDIO_EVENT_BASE + 1)

//*****************************************************************************
//
// This definition is used with the USBHostAudioFormatGet() and
// USBHostAudioFormatSet() API's to determine if the audio input is being
// accesses(USBH_AUDIO_FORMAT_IN set) or audio output(USBH_AUDIO_FORMAT clear).
//
//*****************************************************************************
#define USBH_AUDIO_FORMAT_IN    0x00000001
#define USBH_AUDIO_FORMAT_OUT   0x00000000

typedef struct
{
    uint8_t ui8Channels;
    uint8_t ui8Bits;
    uint32_t ui32SampleRate;
}
tUSBAudioFormat;

typedef struct tUSBHostAudioInstance tUSBHostAudioInstance;

//*****************************************************************************
//
// The prototype for the host USB Audio driver callback function.
//
//*****************************************************************************
typedef void (*tUSBHostAudioCallback)(tUSBHostAudioInstance *psAudioInstance,
                                      uint32_t ui32Event,
                                      uint32_t ui32MsgParam,
                                      void *pvMsgData);

//*****************************************************************************
//
// API Function Prototypes
//
//*****************************************************************************
extern tUSBHostAudioInstance * USBHostAudioOpen(uint32_t ui32Index,
                                           tUSBHostAudioCallback pfnCallback);
extern void USBHostAudioClose(tUSBHostAudioInstance *psAudioInstance);
extern int32_t USBHostAudioPlay(tUSBHostAudioInstance *psAudioInstance,
                                void *pvBuffer, uint32_t ui32Size,
                                tUSBHostAudioCallback pfnCallback);

extern uint32_t USBHostAudioFormatGet(tUSBHostAudioInstance *psAudioInstance,
                                      uint32_t ui32SampleRate,
                                      uint32_t ui32Bits, uint32_t ui32Channels,
                                      uint32_t ui32Flags);
extern uint32_t USBHostAudioFormatSet(tUSBHostAudioInstance *psAudioInstance,
                                      uint32_t ui32SampleRate,
                                      uint32_t ui32Bits, uint32_t ui32Channels,
                                      uint32_t ui32Flags);

extern int32_t USBHostAudioRecord(tUSBHostAudioInstance *psAudioInstance,
                                  void *pvBuffer, uint32_t ui32Size,
                                  tUSBHostAudioCallback pfnAudioCallback);

extern uint32_t USBHostAudioVolumeGet(tUSBHostAudioInstance *psAudioInstance,
                                      uint32_t ui32Interface,
                                      uint32_t ui32Channel);

extern void USBHostAudioVolumeSet(tUSBHostAudioInstance *psAudioInstance,
                                  uint32_t ui32Interface, uint32_t ui32Channel,
                                  uint32_t ui32Value);

extern uint32_t USBHostAudioVolumeMaxGet(tUSBHostAudioInstance *psAudioInstance,
                                         uint32_t ui32Interface,
                                         uint32_t ui32Channel);

extern uint32_t USBHostAudioVolumeMinGet(tUSBHostAudioInstance *psAudioInstance,
                                         uint32_t ui32Interface,
                                         uint32_t ui32Channel);

extern uint32_t USBHostAudioVolumeResGet(tUSBHostAudioInstance *psAudioInstance,
                                           uint32_t ui32Interface,
                                           uint32_t ui32Channel);

//*****************************************************************************
//
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

