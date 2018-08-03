//*****************************************************************************
//
// usbhhidkeyboard.c - This file holds the application interfaces for USB
//                     keyboard devices.
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

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_types.h"
#include "usblib/usblib.h"
#include "usblib/host/usbhost.h"
#include "usblib/usbhid.h"
#include "usblib/host/usbhhid.h"
#include "usblib/host/usbhhidkeyboard.h"

//*****************************************************************************
//
//! \addtogroup usblib_host_device
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Prototypes for local functions.
//
//*****************************************************************************
static uint32_t USBHKeyboardCallback(void *pvKeyboard, uint32_t ui32Event,
                                     uint32_t ui32MsgParam, void *pvMsgData);

//*****************************************************************************
//
// The size of a USB keyboard report.
//
//*****************************************************************************
#define USBHKEYB_REPORT_SIZE    8

//*****************************************************************************
//
// These are the flags for the tUSBHKeyboard.ui32HIDFlags member variable.
//
//*****************************************************************************
#define USBHKEYB_DEVICE_PRESENT 0x00000001

//*****************************************************************************
//
// This is the structure definition for a keyboard device instance.
//
//*****************************************************************************
struct tUSBHKeyboard
{
    //
    // Global flags for an instance of a keyboard.
    //
    uint32_t ui32HIDFlags;

    //
    // The applications registered callback.
    //
    tUSBHIDKeyboardCallback pfnCallback;

    //
    // The HID instance pointer for this keyboard instance.
    //
    tHIDInstance *psHIDInstance;

    //
    // NUM_LOCK, CAPS_LOCK, SCROLL_LOCK, COMPOSE or KANA keys.
    //
    uint8_t ui8KeyModSticky;

    //
    // This is the current state of the keyboard modifier keys.
    //
    uint8_t ui8KeyModState;

    //
    // This holds the keyboard usage codes for keys that are being held down.
    //
    uint8_t pui8KeyState[6];

    //
    // This is a local buffer to hold the current HID report that comes up
    // from the HID driver layer.
    //
    uint8_t pui8Buffer[USBHKEYB_REPORT_SIZE];
};

//*****************************************************************************
//
// This is the per instance information for a keyboard device.
//
//*****************************************************************************
static tUSBHKeyboard g_sUSBHKeyboard =
{
    0
};

//*****************************************************************************
//
//! This function is used open an instance of a keyboard.
//!
//! \param pfnCallback is the callback function to call when new events occur
//! with the keyboard returned.
//! \param pui8Buffer is the memory used by the keyboard to interact with the
//! USB keyboard.
//! \param ui32Size is the size of the buffer provided by \e pui8Buffer.
//!
//! This function is used to open an instance of the keyboard.  The value
//! returned from this function should be used as the instance identifier for
//! all other USBHKeyboard calls.  The \e pui8Buffer memory buffer is used to
//! access the keyboard.  The buffer size required is at least enough to hold
//! a normal report descriptor for the device.  If there is not enough space
//! only a partial report descriptor will be read out.
//!
//! \return Returns the instance identifier for the keyboard that is attached.
//! If there is no keyboard present this will return 0.
//
//*****************************************************************************
tUSBHKeyboard *
USBHKeyboardOpen(tUSBHIDKeyboardCallback pfnCallback, uint8_t *pui8Buffer,
                 uint32_t ui32Size)
{
    //
    // Save the callback and data pointers.
    //
    g_sUSBHKeyboard.pfnCallback = pfnCallback;

    //
    // Save the instance pointer for the HID device that was opened.
    //
    g_sUSBHKeyboard.psHIDInstance =
        USBHHIDOpen(eUSBHHIDClassKeyboard, USBHKeyboardCallback,
                    (void *)&g_sUSBHKeyboard);

    return(&g_sUSBHKeyboard);
}

//*****************************************************************************
//
//! This function is used close an instance of a keyboard.
//!
//! \param psKbInstance is the instance value for this keyboard.
//!
//! This function is used to close an instance of the keyboard that was opened
//! with a call to USBHKeyboardOpen().  The \e psKbInstance value is the
//! value that was returned when the application called USBHKeyboardOpen().
//!
//! \return This function returns 0 to indicate success any non-zero value
//! indicates an error condition.
//
//*****************************************************************************
uint32_t
USBHKeyboardClose(tUSBHKeyboard *psKbInstance)
{
    //
    // Reset the callback to null.
    //
    psKbInstance->pfnCallback = 0;

    //
    // Call the HID driver layer to close out this instance.
    //
    USBHHIDClose(psKbInstance->psHIDInstance);

    return(0);
}

//*****************************************************************************
//
//! This function is used to map a USB usage ID to a printable character.
//!
//! \param psKbInstance is the instance value for this keyboard.
//! \param psTable is the table to use to map the usage ID to characters.
//! \param ui8UsageID is the USB usage ID to map to a character.
//!
//! This function is used to map a USB usage ID to a character.  The provided
//! \e psTable is used to perform the mapping and is described by the
//! tHIDKeyboardUsageTable type defined structure.  See the documentation on
//! the tHIDKeyboardUsageTable structure for more details on the internals of
//! this structure.  This function uses the current state of the shift keys
//! and the Caps Lock key to modify the data returned by this function.  The
//! psTable structure has values indicating which keys are modified by Caps
//! and alternate values for shifted cases.  The number of bytes returned from
//! Lock this function depends on the \e psTable structure passed in as it
//! holds the number of bytes per character in the table.
//!
//! \return Returns the character value for the given usage id.
//
//*****************************************************************************
uint32_t
USBHKeyboardUsageToChar(tUSBHKeyboard *psKbInstance,
                        const tHIDKeyboardUsageTable *psTable,
                        uint8_t ui8UsageID)
{
    uint32_t ui32Value, ui32Offset, ui32Shift;
    const uint8_t *pui8KeyBoardMap;
    const uint16_t *pui16KeyBoardMap;

    //
    // The added offset for the shifted character value.
    //
    ui32Shift = 0;

    //
    // Offset in the table for the character.
    //
    ui32Offset = (ui8UsageID * psTable->ui8BytesPerChar * 2);

    //
    // Handle the case where CAPS lock has been set.
    //
    if(psKbInstance->ui8KeyModSticky &= HID_KEYB_CAPS_LOCK)
    {
        //
        // See if this usage ID is modified by Caps Lock by checking the packed
        // bit array in the pui32ShiftState member of the psTable array.
        //
        if((psTable->pui32CapsLock[ui8UsageID >> 5]) >>
           (ui8UsageID & 0x1f) & 1)
        {
            ui32Shift = psTable->ui8BytesPerChar;
        }
    }

    //
    // Now handle if a shift key is being held.
    //
    if((psKbInstance->ui8KeyModState & 0x22) != 0)
    {
        //
        // Not shifted yet so we need to shift.
        //
        if(ui32Shift == 0)
        {
            ui32Shift = psTable->ui8BytesPerChar;
        }
        else
        {
            //
            // Unshift because CAPS LOCK and shift were pressed.
            //
            ui32Shift = 0;
        }
    }

    //
    // One byte per character.
    //
    if(psTable->ui8BytesPerChar == 1)
    {
        //
        // Get the base address of the table.
        //
        pui8KeyBoardMap = psTable->pvCharMapping;

        ui32Value = pui8KeyBoardMap[ui32Offset + ui32Shift];
    }
    //
    // Two bytes per character.
    //
    else if(psTable->ui8BytesPerChar == 2)
    {
        //
        // Get the base address of the table.
        //
        pui16KeyBoardMap = (uint16_t *)psTable->pvCharMapping;

        ui32Value = pui16KeyBoardMap[ui32Offset + ui32Shift];
    }
    //
    // All other sizes are unsupported for now.
    //
    else
    {
        ui32Value = 0;
    }

    return(ui32Value);
}

//*****************************************************************************
//
//! This function is used to set one of the fixed modifier keys on a keyboard.
//!
//! \param psKbInstance is the instance value for this keyboard.
//! \param ui32Modifiers is a bit mask of the modifiers to set on the keyboard.
//!
//! This function is used to set the modifier key states on a keyboard.  The
//! \e ui32Modifiers value is a bitmask of the following set of values:
//! - HID_KEYB_NUM_LOCK
//! - HID_KEYB_CAPS_LOCK
//! - HID_KEYB_SCROLL_LOCK
//! - HID_KEYB_COMPOSE
//! - HID_KEYB_KANA
//!
//! Not all of these will be supported on all keyboards however setting values
//! on a keyboard that does not have them should have no effect.  The
//! \e psKbInstance value is the value that was returned when the application
//! called USBHKeyboardOpen().  If the value \b HID_KEYB_CAPS_LOCK is used it
//! will modify the values returned from the USBHKeyboardUsageToChar()
//! function.
//!
//! \return This function returns 0 to indicate success any non-zero value
//! indicates an error condition.
//
//*****************************************************************************
uint32_t
USBHKeyboardModifierSet(tUSBHKeyboard *psKbInstance, uint32_t ui32Modifiers)
{
    //
    // Remember the fact that this is set.
    //
    psKbInstance->ui8KeyModSticky = (uint8_t)ui32Modifiers;

    //
    // Set the LEDs on the keyboard.
    //
    USBHHIDSetReport(psKbInstance->psHIDInstance, 0,
                     (uint8_t *)&ui32Modifiers, 1);

    return(0);
}

//*****************************************************************************
//
//! This function is used to initialize a keyboard interface after a keyboard
//! has been detected.
//!
//! \param psKbInstance is the instance value for this keyboard.
//!
//! This function should be called after receiving a \b USB_EVENT_CONNECTED
//! event in the callback function provided by USBHKeyboardOpen(), however this
//! function should only be called outside the callback function.  This will
//! initialize the keyboard interface and determine the keyboard's
//! layout and how it reports keys to the USB host controller.  The
//! \e psKbInstance value is the value that was returned when the application
//! called USBHKeyboardOpen().  This function only needs to be called once
//! per connection event but it should be called every time a
//! \b USB_EVENT_CONNECTED event occurs.
//!
//! \return This function returns 0 to indicate success any non-zero value
//! indicates an error condition.
//
//*****************************************************************************
uint32_t
USBHKeyboardInit(tUSBHKeyboard *psKbInstance)
{
    uint8_t ui8ModData;

    //
    // Set the initial rate to only update on keyboard state changes.
    //
    USBHHIDSetIdle(psKbInstance->psHIDInstance, 0, 0);

    //
    // Read out the Report Descriptor from the keyboard and parse it for
    // the format of the reports coming back from the keyboard.
    //
    USBHHIDGetReportDescriptor(psKbInstance->psHIDInstance,
                               psKbInstance->pui8Buffer,
                               USBHKEYB_REPORT_SIZE);

    //
    // Set the keyboard to boot protocol.
    //
    USBHHIDSetProtocol(psKbInstance->psHIDInstance, 1);

    //
    // Used to clear the initial state of all on keyboard modifiers.
    //
    ui8ModData = 0;

    //
    // Update the keyboard LED state.
    //
    USBHHIDSetReport(psKbInstance->psHIDInstance, 0, &ui8ModData, 1);

    return(0);
}

//*****************************************************************************
//
//! This function is used to set the automatic poll rate of the keyboard.
//!
//! \param psKbInstance is the instance value for this keyboard.
//! \param ui32PollRate is the rate in ms to cause the keyboard to update the
//! host regardless of no change in key state.
//!
//! This function will allow an application to tell the keyboard how often it
//! should send updates to the USB host controller regardless of any changes
//! in keyboard state.  The \e psKbInstance value is the value that was
//! returned when the application called USBHKeyboardOpen().  The
//! \e ui32PollRate is the new value in ms for the update rate on the keyboard.
//! This value is initially set to 0 which indicates that the keyboard should
//! only to update when the keyboard state changes.  Any value other than 0 can
//! be used to force the keyboard to generate auto-repeat sequences for the
//! application.
//!
//! \return This function returns 0 to indicate success any non-zero value
//! indicates an error condition.
//
//*****************************************************************************
uint32_t
USBHKeyboardPollRateSet(tUSBHKeyboard *psKbInstance, uint32_t ui32PollRate)
{
    //
    // Send the Set Idle command to the USB keyboard.
    //
    USBHHIDSetIdle(psKbInstance->psHIDInstance, ui32PollRate, 0);

    return(0);
}

//*****************************************************************************
//
// This is an internal function used to modify the current keyboard state.
//
// This function checks for changes in the keyboard state due to a new report
// being received from the device.  It first checks if this is a "roll-over"
// case by seeing if 0x01 is in the first position of the new keyboard report.
// This indicates that too many keys were pressed to handle and to ignore this
// report.  Next the keyboard modifier state is stored and if any changes are
// detected a \b USBH_EVENT_HID_KB_MOD event is sent back to the application.
// Then this function will check for any keys that have been released and send
// a \b USBH_EVENT_HID_KB_REL even for each of these keys.  The last check is
// for any new keys that are pressed and a \b USBH_EVENT_HID_KB_PRESS event
// will be sent for each new key pressed.
//
// \return None.
//
//*****************************************************************************
static void
UpdateKeyboardState(tUSBHKeyboard *psKbInstance)
{
    int32_t i32NewKey, i32OldKey;

    //
    // rollover code so ignore this buffer.
    //
    if(psKbInstance->pui8Buffer[2] == 0x01)
    {
        return;
    }

    //
    // Handle the keyboard modifier states.
    //
    if(psKbInstance->ui8KeyModState != psKbInstance->pui8Buffer[0])
    {
        //
        // Notify the application of the event.
        //
        psKbInstance->pfnCallback(0, USBH_EVENT_HID_KB_MOD,
                                  psKbInstance->pui8Buffer[0], 0);

        //
        // Save the new state of the modifier keys.
        //
        psKbInstance->ui8KeyModState = psKbInstance->pui8Buffer[0];
    }

    //
    // This loop checks for keys that have been released to make room for new
    // ones that may have been pressed.
    //
    for(i32OldKey = 2; i32OldKey < 8; i32OldKey++)
    {
        //
        // If there is no old key pressed in this entry go to the next one.
        //
        if(psKbInstance->pui8KeyState[i32OldKey] == 0)
        {
            continue;
        }

        //
        // Check if this old key is still in the list of currently pressed
        // keys.
        //
        for(i32NewKey = 2; i32NewKey < 8; i32NewKey++)
        {
            //
            // Break out if the key is still present.
            //
            if(psKbInstance->pui8Buffer[i32NewKey] ==
               psKbInstance->pui8KeyState[i32OldKey])
            {
                break;
            }
        }
        //
        // If the old key was no longer in the list of pressed keys then
        // notify the application of the key release.
        //
        if(i32NewKey == 8)
        {
            //
            // Send the key release notification to the application.
            //
            psKbInstance->pfnCallback(0, USBH_EVENT_HID_KB_REL,
                                      psKbInstance->pui8KeyState[i32OldKey],
                                      0);
            //
            // Remove the old key from the currently held key list.
            //
            psKbInstance->pui8KeyState[i32OldKey] = 0;

        }
    }

    //
    // This loop checks for new keys that have been pressed.
    //
    for(i32NewKey = 2; i32NewKey < 8; i32NewKey++)
    {
        //
        // The new list is empty so no new keys are pressed.
        //
        if(psKbInstance->pui8Buffer[i32NewKey] == 0)
        {
            break;
        }

        //
        // This loop checks if the current key was already pressed.
        //
        for(i32OldKey = 2; i32OldKey < 8; i32OldKey++)
        {
            //
            // If it is in both lists then it was already pressed so ignore it.
            //
            if(psKbInstance->pui8Buffer[i32NewKey] ==
               psKbInstance->pui8KeyState[i32OldKey])
            {
                break;
            }
        }
        //
        // The key in the new list was not found so it is new.
        //
        if(i32OldKey == 8)
        {
            //
            // Look for a free location to store this key usage code.
            //
            for(i32OldKey = 2; i32OldKey < 8; i32OldKey++)
            {
                //
                // If an empty location is found, store it and notify the
                // application.
                //
                if(psKbInstance->pui8KeyState[i32OldKey] == 0)
                {
                    //
                    // Save the newly pressed key.
                    //
                    psKbInstance->pui8KeyState[i32OldKey] =
                                        psKbInstance->pui8Buffer[i32NewKey];

                    //
                    // Notify the application of the new key that has been
                    // pressed.
                    //
                    psKbInstance->pfnCallback( 0, USBH_EVENT_HID_KB_PRESS,
                                        psKbInstance->pui8Buffer[i32NewKey],
                                        0);

                    break;
                }
            }
        }
    }
}

//*****************************************************************************
//
//! This function handles event callbacks from the USB HID driver layer.
//!
//! \param pvKeyboard is the pointer that was passed in to the USBHHIDOpen()
//! call.
//! \param ui32Event is the event that has been passed up from the HID driver.
//! \param ui32MsgParam has meaning related to the \e ui32Event that occurred.
//! \param pvMsgData has meaning related to the \e ui32Event that occurred.
//!
//! This function will receive all event updates from the HID driver layer.
//! The keyboard driver itself will mostly be concerned with report callbacks
//! from the HID driver layer and parsing them into keystrokes for the
//! application that has registered for callbacks with the USBHKeyboardOpen()
//! call.
//!
//! \return Non-zero values should be assumed to indicate an error condition.
//
//*****************************************************************************
static uint32_t
USBHKeyboardCallback(void *pvKeyboard, uint32_t ui32Event,
                     uint32_t ui32MsgParam, void *pvMsgData)
{
    tUSBHKeyboard *psKbInstance;

    //
    // Recover the pointer to the instance data.
    //
    psKbInstance = (tUSBHKeyboard *)pvKeyboard;

    switch (ui32Event)
    {
        //
        // New keyboard has been connected so notify the application.
        //
        case USB_EVENT_CONNECTED:
        {
            //
            // Remember that a keyboard is present.
            //
            psKbInstance->ui32HIDFlags |= USBHKEYB_DEVICE_PRESENT;

            //
            // Notify the application that a new keyboard was connected.
            //
            psKbInstance->pfnCallback(0, ui32Event, ui32MsgParam, pvMsgData);

            break;
        }
        case USB_EVENT_DISCONNECTED:
        {
            //
            // No keyboard is present.
            //
            psKbInstance->ui32HIDFlags &= ~USBHKEYB_DEVICE_PRESENT;

            //
            // Notify the application that the keyboard was disconnected.
            //
            psKbInstance->pfnCallback(0, ui32Event, ui32MsgParam, pvMsgData);

            break;
        }
        case USB_EVENT_RX_AVAILABLE:
        {
            //
            // New keyboard report structure was received.
            //
            USBHHIDGetReport(psKbInstance->psHIDInstance, 0,
                             psKbInstance->pui8Buffer,
                             USBHKEYB_REPORT_SIZE);

            //
            // Update the application on the changes in the keyboard state.
            //
            UpdateKeyboardState(psKbInstance);

            break;
        }
    }
    return(0);
}

//*****************************************************************************
//
//! @}
//
//*****************************************************************************
