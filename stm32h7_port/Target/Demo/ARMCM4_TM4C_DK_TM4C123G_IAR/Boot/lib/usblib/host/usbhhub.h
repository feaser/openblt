//*****************************************************************************
//
// usbhhub.h - This hold the host driver for hid class.
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

#ifndef __USBHHUB_H__
#define __USBHHUB_H__

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

typedef struct tHubInstance tHubInstance;

extern const tUSBHostClassDriver g_sUSBHubClassDriver;

//*****************************************************************************
//
// The USB standard allows for up to 127 downstream ports on a single hub.
// This would require rather more memory than we would like to set aside so the
// default configuration of the hub driver supports hubs with up to 7
// downstream-facing ports.  In practice, this should be more than enough
// since this covers the vast majority of consumer hubs.  Note that, by
// default, we will only support 4 devices so you can't fully populate a 7 port
// hub and have everything work.
//
// Feel free to change this but bad things will happen if you increase it above
// 31 since we assume the reports will always fit inside a 4 byte buffer.
//
//*****************************************************************************
#define ROOT_HUB_MAX_PORTS 7

//*****************************************************************************
//
// Values used as the ui16Feature parameter to USBHHubClearHubFeature().
//
//*****************************************************************************
#define HUB_FEATURE_C_HUB_LOCAL_POWER                                         \
                                0
#define HUB_FEATURE_C_HUB_OVER_CURRENT                                        \
                                1

//*****************************************************************************
//
// Values used as the ui16Feature parameter to USBHHubSetPortFeature() and
// USBHHubClearPortFeature().
//
//*****************************************************************************
#define HUB_FEATURE_PORT_CONNECTION                                           \
                                0
#define HUB_FEATURE_PORT_ENABLE 1
#define HUB_FEATURE_PORT_SUSPEND                                              \
                                2
#define HUB_FEATURE_PORT_OVER_CURRENT                                         \
                                3
#define HUB_FEATURE_PORT_RESET  4
#define HUB_FEATURE_PORT_POWER  8
#define HUB_FEATURE_PORT_LOW_SPEED                                            \
                                9
#define HUB_FEATURE_C_PORT_CONNECTION                                         \
                                16
#define HUB_FEATURE_C_PORT_ENABLE                                             \
                                17
#define HUB_FEATURE_C_PORT_SUSPEND                                            \
                                18
#define HUB_FEATURE_C_PORT_OVER_CURRENT                                       \
                                19
#define HUB_FEATURE_C_PORT_RESET                                              \
                                20
#define HUB_FEATURE_PORT_TEST   21
#define HUB_FEATURE_PORT_INDICATOR                                            \
                                22

//*****************************************************************************
//
// Values returned via the *pui16HubStatus and *pui16HubChange parameters
// passed to USBHHubGetHubStatus().  These may be ORed together into the
// returned status value.
//
//*****************************************************************************
#define HUB_STATUS_PWR_LOST     1
#define HUB_STATUS_OVER_CURRENT 2

//*****************************************************************************
//
// Values returned via the *pui16PortStatus parameter passed to
// USBHHubGetPortStatus().  These may be ORed together into the returned status
// value.
//
//*****************************************************************************
#define HUB_PORT_STATUS_DEVICE_PRESENT                                        \
                                0x0001
#define HUB_PORT_STATUS_ENABLED 0x0002
#define HUB_PORT_STATUS_SUSPENDED                                             \
                                0x0004
#define HUB_PORT_STATUS_OVER_CURRENT                                          \
                                0x0008
#define HUB_PORT_STATUS_RESET   0x0010
#define HUB_PORT_STATUS_POWERED 0x0100
#define HUB_PORT_STATUS_LOW_SPEED                                             \
                                0x0200
#define HUB_PORT_STATUS_HIGH_SPEED                                            \
                                0x0400
#define HUB_PORT_STATUS_TEST_MODE                                             \
                                0x0800
#define HUB_PORT_STATUS_INDICATOR_CONTROL                                     \
                                0x1000

//*****************************************************************************
//
// Values returned via the *pui16PortChange parameter passed to
// USBHHubGetPortStatus().  These may be ORed together into the returned status
// value.
//
//*****************************************************************************
#define HUB_PORT_CHANGE_DEVICE_PRESENT                                        \
                                0x0001
#define HUB_PORT_CHANGE_ENABLED 0x0002
#define HUB_PORT_CHANGE_SUSPENDED                                             \
                                0x0004
#define HUB_PORT_CHANGE_OVER_CURRENT                                          \
                                0x0008
#define HUB_PORT_CHANGE_RESET   0x0010

//*****************************************************************************
//
// The prototype for the USB Hub host driver callback function.
//
//*****************************************************************************
typedef void (*tUSBHHubCallback)(tHubInstance *psHubInstance,
                                 uint32_t ui32Event, uint32_t ui32MsgParam,
                                 void *pvMsgData);

//*****************************************************************************
//
// Public function prototypes for the HUB class driver.
//
//*****************************************************************************
extern tHubInstance * USBHHubOpen(tUSBHHubCallback pfnCallback);
extern void USBHHubClose(tHubInstance *psHubInstance);

#ifdef __cplusplus
}
#endif

#endif // __USBHHUB_H__
