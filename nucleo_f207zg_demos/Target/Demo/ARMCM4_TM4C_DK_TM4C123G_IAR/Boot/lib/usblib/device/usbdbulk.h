//*****************************************************************************
//
// usbdcdc.h - USBLib support for a generic bulk device.
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

#ifndef __USBDBULK_H__
#define __USBDBULK_H__

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
//! \addtogroup bulk_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// PRIVATE
//
// The first few sections of this header are private defines that are used by
// the USB Bulk example code and are here only to help with the application
// allocating the correct amount of memory for the Bulk example device code.
//
//*****************************************************************************

//*****************************************************************************
//
// PRIVATE
//
// This enumeration holds the various states that the device can be in during
// normal operation.
//
//*****************************************************************************
typedef enum
{
    //
    // Not configured.
    //
    eBulkStateUnconfigured,

    //
    // No outstanding transaction remains to be completed.
    //
    eBulkStateIdle,

    //
    // Waiting on completion of a send or receive transaction.
    //
    eBulkStateWaitData,

    //
    // Waiting for client to process data.
    //
    eBulkStateWaitClient
}
tBulkState;

//*****************************************************************************
//
// PRIVATE
//
// This structure defines the private instance data and state variables for the
// Bulk only example device.  The memory for this structure is inlcluded in
// the sPrivateData field in the tUSBDBulkDevice structure passed on
// USBDBulkInit().
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
    // The state of the bulk receive channel.
    //
    volatile tBulkState iBulkRxState;

    //
    // The state of the bulk transmit channel.
    //
    volatile tBulkState iBulkTxState;

    //
    // State of any pending operations that could not be handled immediately
    // upon receipt.
    //
    volatile uint16_t ui16DeferredOpFlags;

    //
    // Size of the last transmit.
    //
    uint16_t ui16LastTxSize;

    //
    // The connection status of the device.
    //
    volatile bool bConnected;

    //
    // The IN endpoint number, this is modified in composite devices.
    //
    uint8_t ui8INEndpoint;

    //
    // The OUT endpoint number, this is modified in composite devices.
    //
    uint8_t ui8OUTEndpoint;

    //
    // The bulk class interface number, this is modified in composite devices.
    //
    uint8_t ui8Interface;
}
tBulkInstance;

//*****************************************************************************
//
// This is the size of the g_pui8BulkInterface array in bytes.
//
//*****************************************************************************
#define BULKINTERFACE_SIZE      (23)

//*****************************************************************************
//
//! The size of the memory that should be allocated to create a configuration
//! descriptor for a single instance of the USB Bulk Device.
//! This does not include the configuration descriptor which is automatically
//! ignored by the composite device class.
//
//*****************************************************************************
#define COMPOSITE_DBULK_SIZE    (BULKINTERFACE_SIZE)

//*****************************************************************************
//
//! The structure used by the application to define operating parameters for
//! the bulk device.
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
    //! The maximum power consumption of the device, expressed in milliamps.
    //
    const uint16_t ui16MaxPowermA;

    //
    //! Indicates whether the device is self- or bus-powered and whether or not
    //! it supports remote wakeup.  Valid values are USB_CONF_ATTR_SELF_PWR or
    //! USB_CONF_ATTR_BUS_PWR, optionally ORed with USB_CONF_ATTR_RWAKE.
    //
    const uint8_t ui8PwrAttributes;

    //
    //! A pointer to the callback function which will be called to notify
    //! the application of events related to the device's data receive channel.
    //
    const tUSBCallback pfnRxCallback;

    //
    //! A client-supplied pointer which will be sent as the first
    //! parameter in all calls made to the receive channel callback,
    //! pfnRxCallback.
    //
    void *pvRxCBData;

    //
    //! A pointer to the callback function which will be called to notify
    //! the application of events related to the device's data transmit
    //! channel.
    //
    const tUSBCallback pfnTxCallback;

    //
    //! A client-supplied pointer which will be sent as the first
    //! parameter in all calls made to the transmit channel callback,
    //! pfnTxCallback.
    //
    void *pvTxCBData;

    //
    //! A pointer to the string descriptor array for this device.  This array
    //! must contain pointers to the following string descriptors in this
    //! order.  Language descriptor, Manufacturer name string (language 1),
    //! Product name string (language 1), Serial number string (language 1),
    //! Interface description string (language 1) and Configuration description
    //! string (language 1).
    //!
    //! If supporting more than 1 language, the strings for indices 1 through 5
    //! must be repeated for each of the other languages defined in the
    //! language descriptor.
    //
    const uint8_t * const *ppui8StringDescriptors;

    //
    //! The number of descriptors provided in the ppStringDescriptors array.
    //! This must be 1 + (5 * number of supported languages).
    //
    const uint32_t ui32NumStringDescriptors;

    //
    //! The private instance data for this device.  This memory must
    //! not be modified by any code outside the bulk class driver.
    //
    tBulkInstance sPrivateData;
}
tUSBDBulkDevice;

//*****************************************************************************
//
// API Function Prototypes
//
//*****************************************************************************
extern void *USBDBulkInit(uint32_t ui32Index, tUSBDBulkDevice *psBulkDevice);
extern void *USBDBulkCompositeInit(uint32_t ui32Index,
                                   tUSBDBulkDevice *psBulkDevice,
                                   tCompositeEntry *psCompEntry);
extern void USBDBulkTerm(void *pvBulkInstance);
extern void *USBDBulkSetRxCBData(void *pvBulkInstance, void *pvCBData);
extern void *USBDBulkSetTxCBData(void *pvBulkInstance, void *pvCBData);
extern uint32_t USBDBulkPacketWrite(void *pvBulkInstance, uint8_t *pi8Data,
                                    uint32_t ui32Length, bool bLast);
extern uint32_t USBDBulkPacketRead(void *pvBulkInstance, uint8_t *pi8Data,
                                   uint32_t ui32Length, bool bLast);
extern uint32_t USBDBulkTxPacketAvailable(void *pvBulkInstance);
extern uint32_t USBDBulkRxPacketAvailable(void *pvBulkInstance);
extern void USBDBulkPowerStatusSet(void *pvBulkInstance, uint8_t ui8Power);
extern bool USBDBulkRemoteWakeupRequest(void *pvBulkInstance);

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

#endif // __USBDBULK_H__
