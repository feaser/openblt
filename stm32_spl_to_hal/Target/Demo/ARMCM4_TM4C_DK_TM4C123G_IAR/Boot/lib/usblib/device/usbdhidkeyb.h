//*****************************************************************************
//
// usbdhidkeyb.h - Definitions used by HID keyboard class devices.
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

#ifndef __USBDHIDKEYB_H__
#define __USBDHIDKEYB_H__

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
//! \addtogroup hid_keyboard_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! The maximum number of simultaneously-pressed, non-modifier keys that the
//! HID BIOS keyboard protocol can send at once.  Attempts to send more pressed
//! keys than this will result in a rollover error being reported to the host
//! and KEYB_ERR_TOO_MANY_KEYS being returned from
//! USBDHIDKeyboardKeyStateChange.
//
//*****************************************************************************
#define KEYB_MAX_CHARS_PER_REPORT \
                                6

//*****************************************************************************
//
// PRIVATE
//
// The first few sections of this header are private defines that are used by
// the USB HID keyboard code and are here only to help with the application
// allocating the correct amount of memory for the USB HID Keyboard device
// code.
//
//*****************************************************************************

//*****************************************************************************
//
// PRIVATE
//
// This enumeration holds the various states that the keyboard can be in during
// normal operation.
//
//*****************************************************************************
typedef enum
{
    //
    // Unconfigured.
    //
    HID_KEYBOARD_STATE_UNCONFIGURED,

    //
    // No keys to send and not waiting on data.
    //
    HID_KEYBOARD_STATE_IDLE,

    //
    // Waiting on report data from the host.
    //
    HID_KEYBOARD_STATE_WAIT_DATA,

    //
    // Waiting on data to be sent out.
    //
    HID_KEYBOARD_STATE_SEND
}
tKeyboardState;

//*****************************************************************************
//
// PRIVATE
//
// The size of the keyboard input and output reports.
//
//*****************************************************************************
#define KEYB_IN_REPORT_SIZE 8
#define KEYB_OUT_REPORT_SIZE 1

//*****************************************************************************
//
// PRIVATE
//
// This structure defines the private instance data structure for the USB HID
// keyboard device.  This structure forms the RAM workspace used by each
// instance of the keyboard.
//
//*****************************************************************************
typedef struct
{
    //
    // The USB configuration number set by the host or 0 of the device is
    // currently unconfigured.
    //
    uint8_t ui8USBConfigured;

    //
    // The protocol requested by the host, USB_HID_PROTOCOL_BOOT or
    // USB_HID_PROTOCOL_REPORT.
    //
    uint8_t ui8Protocol;

    //
    // The current states that the keyboard LEDs are to be set to.
    //
    volatile uint8_t ui8LEDStates;

    //
    // The total number of keys currently pressed.  This indicates the number
    // of key press entries in the pui8KeysPressed array.
    //
    uint8_t ui8KeyCount;

    //
    // The current state of the keyboard interrupt IN endpoint.
    //
    volatile tKeyboardState eKeyboardState;

    //
    // A flag to indicate that the application pressed or released a key
    // but that we couldn't send the report immediately.
    //
    volatile bool bChangeMade;

    //
    // A buffer used to receive output reports from the host.
    //
    uint8_t pui8DataBuffer[KEYB_OUT_REPORT_SIZE];

    //
    // A buffer used to hold the last input report sent to the host.
    //
    uint8_t pui8Report[KEYB_IN_REPORT_SIZE];

    //
    // A buffer containing the usage codes of all non-modifier keys currently
    // in the pressed state.
    //
    uint8_t pui8KeysPressed[KEYB_MAX_CHARS_PER_REPORT];

    //
    // The idle timeout control structure for our input report.  This is
    // required by the lower level HID driver.
    //
    tHIDReportIdle sReportIdle;

    //
    // This is needed for the lower level HID driver.
    //
    tUSBDHIDDevice sHIDDevice;
}
tHIDKeyboardInstance;

//*****************************************************************************
//
//! This structure is used by the application to define operating parameters
//! for the HID keyboard device.
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

    //! A pointer to the callback function which will be called to notify
    //! the application of general events and those related to reception of
    //! Output and Feature reports via the (optional) interrupt OUT endpoint.
    //
    const tUSBCallback pfnCallback;

    //
    //! A client-supplied pointer which will be sent as the first
    //! parameter in all calls made to the keyboard callback,
    //! pfnCallback.
    //
    void *pvCBData;

    //
    //! A pointer to the string descriptor array for this device.  This array
    //! must contain the following string descriptor pointers in this order.
    //! Language descriptor, Manufacturer name string (language 1), Product
    //! name string (language 1), Serial number string (language 1),HID
    //! Interface description string (language 1), Configuration description
    //! string (language 1).
    //!
    //! If supporting more than 1 language, the descriptor block (except for
    //! string descriptor 0) must be repeated for each language defined in the
    //! language descriptor.
    //
    const uint8_t * const *ppui8StringDescriptors;

    //
    //! The number of descriptors provided in the ppStringDescriptors
    //! array.  This must be (1 + (5 * (num languages))).
    //
    const uint32_t ui32NumStringDescriptors;

    //
    //! The private instance data for this device.  This memory must
    //! remain accessible for as long as the keyboard device is in use and
    //! must not be modified by any code outside the HID keyboard driver.
    //
    tHIDKeyboardInstance sPrivateData;
}
tUSBDHIDKeyboardDevice;

//*****************************************************************************
//
// Keyboard-specific device class driver events
//
//*****************************************************************************

//*****************************************************************************
//
//! This event indicates that the keyboard LED states are to be set.  The
//! ui32MsgValue parameter contains the requested state for each of the LEDs
//! defined as a collection of ORed bits where a 1 indicates that the LED is
//! to be turned on and a 0 indicates that it should be turned off.  The
//! individual LED bits are defined using labels HID_KEYB_NUM_LOCK,
//! HID_KEYB_CAPS_LOCK, HID_KEYB_SCROLL_LOCK, HID_KEYB_COMPOSE and
//! HID_KEYB_KANA.
//
//*****************************************************************************
#define USBD_HID_KEYB_EVENT_SET_LEDS                                          \
                                USBD_HID_KEYB_EVENT_BASE

//*****************************************************************************
//
//! This return code from USBDHIDKeyboardKeyStateChange indicates success.
//
//*****************************************************************************
#define KEYB_SUCCESS            0

//*****************************************************************************
//
//! This return code from USBDHIDKeyboardKeyStateChange indicates that an
//! attempt has been made to record more than 6 simultaneously pressed,
//! non-modifier keys.  The USB HID BIOS keyboard protocol allows no more than
//! 6 pressed keys to be reported at one time.  Until at least one key is
//! released, the device will report a roll over error to the host each time it
//! is asked for the keyboard input report.
//
//*****************************************************************************
#define KEYB_ERR_TOO_MANY_KEYS  1

//*****************************************************************************
//
//! This return code from USBDHIDKeyboardKeyStateChange indicates that an
//! error was reported while attempting to send a report to the host.  A client
//! should assume that the host has disconnected if this return code is seen.
//
//*****************************************************************************
#define KEYB_ERR_TX_ERROR       2

//*****************************************************************************
//
//! USBDHIDKeyboardKeyStateChange returns this value if it is called with the
//! bPress parameter set to false but with a ui8UsageCode parameter which does
//! does not indicate a key that is currently recorded as being pressed.  This
//! may occur if an attempt was previously made to report more than 6 pressed
//! keys and the earlier pressed keys are released before the later ones.  This
//! condition is benign and should not be used to indicate a host disconnection
//! or serious error.
//
//*****************************************************************************
#define KEYB_ERR_NOT_FOUND      3

//*****************************************************************************
//
//! USBDHIDKeyboardKeyStateChange returns this value if it is called before the
//! USB host has connected and configured the device.  Any key usage code
//! passed will be stored and passed to the host once configuration completes.
//
//*****************************************************************************
#define KEYB_ERR_NOT_CONFIGURED 4

//*****************************************************************************
//
// API Function Prototypes
//
//*****************************************************************************
extern void *USBDHIDKeyboardInit(uint32_t ui32Index,
                                 tUSBDHIDKeyboardDevice *psHIDKbDevice);
extern void *USBDHIDKeyboardCompositeInit(uint32_t ui32Index,
                                        tUSBDHIDKeyboardDevice *psHIDKbDevice,
                                        tCompositeEntry *psCompEntry);
extern void USBDHIDKeyboardTerm(void *pvKeyboardInstance);
extern void *USBDHIDKeyboardSetCBData(void *pvKeyboardInstance,
                                      void *pvCBData);
extern uint32_t USBDHIDKeyboardKeyStateChange(void *pvKeyboardInstance,
                                              uint8_t ui8Modifiers,
                                              uint8_t ui8UsageCode,
                                              bool bPressed);
extern void USBDHIDKeyboardPowerStatusSet(void *pvKeyboardInstance,
                                          uint8_t ui8Power);
extern bool USBDHIDKeyboardRemoteWakeupRequest(void *pvKeyboardInstance);

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

#endif // __USBDHIDKEYB_H__
