//*****************************************************************************
//
// usbdcdc.h - USBLib support for generic CDC ACM (serial) device.
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

#ifndef __USBDCDC_H__
#define __USBDCDC_H__

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
//! \addtogroup cdc_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// PRIVATE
//
// The first few sections of this header are private defines that are used by
// the USB CDC Serial code and are here only to help with the application
// allocating the correct amount of memory for the CDC Serial device code.
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
    // Unconfigured.
    //
    eCDCStateUnconfigured,

    //
    // No outstanding transaction remains to be completed.
    //
    eCDCStateIdle,

    //
    // Waiting on completion of a send or receive transaction.
    //
    eCDCStateWaitData,

    //
    // Waiting for client to process data.
    //
    eCDCStateWaitClient
}
tCDCState;

//*****************************************************************************
//
// PRIVATE
//
// This structure defines the private instance data and state variables for the
// CDC Serial device.  The memory for this structure is allocated in the
// tUSBDCDCDevice structure passed on USBDCDCInit().
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
    // The state of the serial receive state.
    //
    volatile tCDCState iCDCRxState;

    //
    // The state of the serial transmit state.
    //
    volatile tCDCState iCDCTxState;

    //
    // The state of the serial request state.
    //
    volatile tCDCState iCDCRequestState;

    //
    // The state of the serial interrupt state.
    //
    volatile tCDCState iCDCInterruptState;

    //
    // The current pending request.
    //
    volatile uint8_t ui8PendingRequest;

    //
    // The current break duration used during send break requests.
    //
    uint16_t ui16BreakDuration;

    //
    // The current line control state for the serial port.
    //
    uint16_t ui16ControlLineState;

    //
    // The general serial state.
    //
    uint16_t ui16SerialState;

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
    // The current serial line coding.
    //
    tLineCoding sLineCoding;

    //
    // Serial port receive is blocked.
    //
    volatile bool bRxBlocked;

    //
    // Serial control port is blocked.
    //
    volatile bool bControlBlocked;

    //
    // The connection status of the device.
    //
    volatile bool bConnected;

    //
    // The control endpoint number, this is modified in composite devices.
    //
    uint8_t ui8ControlEndpoint;

    //
    // The IN endpoint number, this is modified in composite devices.
    //
    uint8_t ui8BulkINEndpoint;

    //
    // The OUT endpoint number, this is modified in composite devices.
    //
    uint8_t ui8BulkOUTEndpoint;

    //
    // The interface number for the control interface, this is modified in
    // composite devices.
    //
    uint8_t ui8InterfaceControl;

    //
    // The interface number for the data interface, this is modified in
    // composite devices.
    //
    uint8_t ui8InterfaceData;
}
tCDCSerInstance;

//*****************************************************************************
//
// The following defines are used when working with composite devices.
//
//*****************************************************************************

//*****************************************************************************
//
// This is the size of the g_pui8IADSerDescriptor array in bytes.
//
//*****************************************************************************
#define SERDESCRIPTOR_SIZE      (8)

//*****************************************************************************
//
// This is the size of the g_pui8CDCSerCommInterface array in bytes.
//
//*****************************************************************************
#define SERCOMMINTERFACE_SIZE   (35)

//*****************************************************************************
//
// This is the size of the g_pui8CDCSerDataInterface array in bytes.
//
//*****************************************************************************
#define SERDATAINTERFACE_SIZE    (23)

//*****************************************************************************
//
//! The size of the memory that should be allocated to create a configuration
//! descriptor for a single instance of the USB Serial CDC Device.
//! This does not include the configuration descriptor which is automatically
//! ignored by the composite device class.
//
//*****************************************************************************
#define COMPOSITE_DCDC_SIZE     (SERDESCRIPTOR_SIZE + SERCOMMINTERFACE_SIZE + \
                                 SERDATAINTERFACE_SIZE)

//*****************************************************************************
//
// CDC-specific events These events are provided to the application in the
// \e ui32Msg parameter of the tUSBCallback function.
//
//*****************************************************************************

//
//! The host requests that the device send a BREAK condition on its
//! serial communication channel.  The BREAK should remain active until
//! a USBD_CDC_EVENT_CLEAR_BREAK event is received.
//
#define USBD_CDC_EVENT_SEND_BREAK (USBD_CDC_EVENT_BASE + 0)

//
//! The host requests that the device stop sending a BREAK condition on its
//! serial communication channel.
//
#define USBD_CDC_EVENT_CLEAR_BREAK (USBD_CDC_EVENT_BASE + 1)

//
//! The host requests that the device set the RS232 signaling lines to
//! a particular state.  The ui32MsgValue parameter contains the RTS and
//! DTR control line states as defined in table 51 of the USB CDC class
//! definition and is a combination of the following values:
//!
//! (RTS) USB_CDC_DEACTIVATE_CARRIER or USB_CDC_ACTIVATE_CARRIER
//! (DTR) USB_CDC_DTE_NOT_PRESENT or USB_CDC_DTE_PRESENT
//
#define USBD_CDC_EVENT_SET_CONTROL_LINE_STATE (USBD_CDC_EVENT_BASE + 2)

//
//! The host requests that the device set the RS232 communication
//! parameters.  The pvMsgData parameter points to a tLineCoding structure
//! defining the required number of bits per character, parity mode,
//! number of stop bits and the baud rate.
//
#define USBD_CDC_EVENT_SET_LINE_CODING (USBD_CDC_EVENT_BASE + 3)

//
//! The host is querying the current RS232 communication parameters.  The
//! pvMsgData parameter points to a tLineCoding structure that the
//! application must fill with the current settings prior to returning
//! from the callback.
//
#define USBD_CDC_EVENT_GET_LINE_CODING (USBD_CDC_EVENT_BASE + 4)

//*****************************************************************************
//
//! The structure used by the application to define operating parameters for
//! the CDC device.
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
    //! the application of all asynchronous control events related to the
    //! operation of the device.
    //
    const tUSBCallback pfnControlCallback;

    //
    //! A client-supplied pointer which will be sent as the first
    //! parameter in all calls made to the control channel callback,
    //! pfnControlCallback.
    //
    void *pvControlCBData;

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
    //! must contain the following string descriptor pointers in this order.
    //! Language descriptor, Manufacturer name string (language 1), Product
    //! name string (language 1), Serial number string (language 1),
    //! Control interface description string (language 1), Configuration
    //! description string (language 1).
    //!
    //! If supporting more than 1 language, the strings for indices 1 through 5
    //! must be repeated for each of the other languages defined in the
    //! language descriptor.
    //
    const uint8_t * const *ppui8StringDescriptors;

    //
    //! The number of descriptors provided in the ppStringDescriptors
    //! array.  This must be 1 + (5 * number of supported languages).
    //
    const uint32_t ui32NumStringDescriptors;

    //
    //! The private instance data for this device.  This memory
    //! must remain accessible for as long as the CDC device is in use and
    //! must not be modified by any code outside the CDC class driver.
    //
    tCDCSerInstance sPrivateData;
}
tUSBDCDCDevice;

//*****************************************************************************
//
// API Function Prototypes
//
//*****************************************************************************
extern void *USBDCDCCompositeInit(uint32_t ui32Index,
                                  tUSBDCDCDevice *psCDCDevice,
                                  tCompositeEntry *psCompEntry);
extern void *USBDCDCInit(uint32_t ui32Index,
                         tUSBDCDCDevice *psCDCDevice);
extern void USBDCDCTerm(void *pvCDCDevice);
extern void *USBDCDCSetControlCBData(void *pvCDCDevice, void *pvCBData);
extern void *USBDCDCSetRxCBData(void *pvCDCDevice, void *pvCBData);
extern void *USBDCDCSetTxCBData(void *pvCDCDevice, void *pvCBData);
extern uint32_t USBDCDCPacketWrite(void *pvCDCDevice, uint8_t *pi8Data,
                                   uint32_t ui32Length, bool bLast);
extern uint32_t USBDCDCPacketRead(void *pvCDCDevice, uint8_t *pi8Data,
                                  uint32_t ui32Length, bool bLast);
extern uint32_t USBDCDCTxPacketAvailable(void *pvCDCDevice);
extern uint32_t USBDCDCRxPacketAvailable(void *pvCDCDevice);
extern void USBDCDCSerialStateChange(void *pvCDCDevice, uint16_t ui16State);
extern void USBDCDCPowerStatusSet(void *pvCDCDevice, uint8_t ui8Power);
extern bool USBDCDCRemoteWakeupRequest(void *pvCDCDevice);

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

#endif // __USBDCDC_H__
