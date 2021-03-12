//*****************************************************************************
//
// usbddfu-rt.h - Definitions used by runtime DFU class devices.
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

#ifndef __USBDDFURT_H__
#define __USBDDFURT_H__

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
//! \addtogroup dfu_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// This is the size of the g_pui8DFUInterface array in bytes.
//
//*****************************************************************************
#define DFUINTERFACE_SIZE       (9)

//*****************************************************************************
//
// This is the size of the g_pui8DFUFunctionalDesc array in bytes.
//
//*****************************************************************************
#define DFUFUNCTIONALDESC_SIZE  (9)

//*****************************************************************************
//
//! The size of the memory that should be allocated to create a configuration
//! descriptor for a single instance of the DFU runtime device.  This does not
//! include the configuration descriptor which is automatically ignored by the
//! composite device class.
//!
//! This label is used to compute the value which will be passed to the
//! USBDCompositeInit function in the ui32Size parameter.
//
//*****************************************************************************
#define COMPOSITE_DDFU_SIZE     (DFUINTERFACE_SIZE + DFUFUNCTIONALDESC_SIZE)

//*****************************************************************************
//
//! This value is passed to the client via the callback function provided in
//! the tUSBDDFUDevice structure and indicates that the host has sent a DETACH
//! request to the DFU interface. This request indicates that the device detach
//! from the USB bus and reattach in DFU mode in preparation for a firmware
//! upgrade.  Currently, this is the only event that the DFU runtime class
//! reports to the client.
//!
//! When this event is received, the client should call USBDDFUUpdateBegin()
//! from a non-interrupt context at its earliest opportunity.
//
//*****************************************************************************
#define USBD_DFU_EVENT_DETACH (USBD_DFU_EVENT_BASE + 0)

//*****************************************************************************
//
// PRIVATE
//
// This structure defines the private instance data and state variables for
// DFU devices.  The memory for this structure is included in the
// sPrivateData field in the tUSBDDFUDevice structure passed in the
// USBDDFUCompositeInit() function.
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
    // The DFU class interface number, this is modified in composite devices.
    //
    uint8_t ui8Interface;

    //
    // The connection status of the device.
    //
    bool bConnected;
}
tDFUInstance;

//*****************************************************************************
//
//! The structure used by the application to define operating parameters for
//! the DFU device.  Note that, unlike all other devices, this structure does
//! not contain any fields which configure the device descriptor sent back to
//! the host.  The DFU runtime device class must be used as part of a composite
//! device since all it provides is the capability to signal the device to
//! switch into DFU mode in preparation for a firmware upgrade.  Creating a
//! device with nothing but DFU runtime mode capability is rather pointless
//! so this is not supported.
//
//*****************************************************************************
typedef struct
{
    //
    //! A pointer to the callback function which will be called to notify
    //! the application of DETACH requests.
    //
    const tUSBCallback pfnCallback;

    //
    //! A client-supplied pointer which will be sent as the first
    //! parameter in all calls made to the pfnCallback function.
    //
    void * const pvCBData;

    //
    //! The private instance data for this device class.  This
    //! memory must remain accessible for as long as the DFU device is in use
    //! and must not be modified by any code outside the DFU class driver.
    //
    tDFUInstance sPrivateData;
}
tUSBDDFUDevice;

//*****************************************************************************
//
// API Function Prototypes
//
//*****************************************************************************
extern void *USBDDFUCompositeInit(uint32_t ui32Index,
                                  tUSBDDFUDevice *psDFUDevice,
                                  tCompositeEntry *psCompEntry);
extern void USBDDFUCompositeTerm(void *pvDFUInstance);
extern void USBDDFUUpdateBegin(void);

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

#endif // __USBDDFURT_H__
