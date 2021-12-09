//*****************************************************************************
//
// usbtick.c - Functions related to USB stack tick timer handling.
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
#include "driverlib/debug.h"
#include "usblib/usblib.h"
#include "usblib/usblibpriv.h"

//*****************************************************************************
//
//! \addtogroup general_usblib_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// These are the internal timer tick handlers used by the USB stack.  Handlers
// in g_pfnTickHandlers are called in the context of the USB SOF interrupt
// every USB_SOF_TICK_DIVIDE milliseconds.
//
//*****************************************************************************
tUSBTickHandler g_pfnTickHandlers[MAX_USB_TICK_HANDLERS];
void *g_pvTickInstance[MAX_USB_TICK_HANDLERS];

//*****************************************************************************
//
// Flag to indicate whether or not we have been initialized.
//
//*****************************************************************************
bool g_bUSBTimerInitialized = false;

//*****************************************************************************
//
// This is the current tick value in ms for the system.  This is used for all
// instances of USB controllers and for all timer tick handlers.
//
//*****************************************************************************
uint32_t g_ui32CurrentUSBTick = 0;

//*****************************************************************************
//
// This is the total number of SOF interrupts received since the system
// booted.  The value is incremented by the low level device- or host-interrupt
// handler functions.
//
//*****************************************************************************
uint32_t g_ui32USBSOFCount = 0;

//*****************************************************************************
//
// This internal function initializes the variables used in processing timer
// ticks.
//
// This function should only be called from within the USB library.  It is set
// up to ensure that it can be called multiple times if necessary without
// the previous configuration being erased (to cater for OTG mode switching).
//
// \return None.
//
//*****************************************************************************
void
InternalUSBTickInit(void)
{
    uint32_t ui32Loop;

    if(!g_bUSBTimerInitialized)
    {
        for(ui32Loop = 0; ui32Loop < MAX_USB_TICK_HANDLERS; ui32Loop++)
        {
            g_pfnTickHandlers[ui32Loop] = (tUSBTickHandler)0;
            g_pvTickInstance[ui32Loop] = 0;
        }

        g_bUSBTimerInitialized = true;
    }
}

//*****************************************************************************
//
// This internal function resets the USB tick handler.
//
// This function should only be called from within the USB library.  It will
// clear out the tick handler state and should be called to allow the tick
// handlers to be initialized once USBDCDInit() function is called.
//
// \return None.
//
//*****************************************************************************
void
InternalUSBTickReset(void)
{
    //
    // Reset the initialized flag so that the next time InternalUSBTickInit()
    // is called.
    //
    g_bUSBTimerInitialized = 0;
}

//*****************************************************************************
//
// This internal function handles registering OTG, Host, or Device SOF timer
// handler functions.
//
// \param pfHandler specifies the handler to call for the given type of
// handler.
// \param pvInstance is the instance pointer that will be returned to the
// function provided in the \e pfHandler function.
//
// This function should only be called inside the USB library and only as a
// result to a call to reinitialize the stack in a new mode.  Currently the
// following 3 types of timer tick handlers can be registered:
// TICK_HANDLER_OTG, TICK_HANDLER_HOST, or TICK_HANDLER_DEVICE.  Handlers
// registered via this function are called in the context of the SOF interrupt.
//
// \return A value of zero means that the tick handler was registered and any
// other value indicates an error.
//
//*****************************************************************************
int32_t
InternalUSBRegisterTickHandler(tUSBTickHandler pfHandler, void *pvInstance)
{
    int32_t i32Idx;

    for(i32Idx = 0; i32Idx < MAX_USB_TICK_HANDLERS; i32Idx++)
    {
        if(g_pfnTickHandlers[i32Idx] == 0)
        {
            //
            // Save the handler.
            //
            g_pfnTickHandlers[i32Idx] = pfHandler;

            //
            // Save the instance data.
            //
            g_pvTickInstance[i32Idx] = pvInstance;
        }
    }

    if(i32Idx == MAX_USB_TICK_HANDLERS)
    {
        return(-1);
    }
    return(0);
}

//*****************************************************************************
//
//! \internal
//!
//! Calls internal handlers in response to a tick based on the start of frame
//! interrupt.
//!
//! \param ui32TicksmS specifies how many milliseconds have passed since the
//! last call to this function.
//!
//! This function is called every 5mS in the context of the Start of Frame
//! (SOF) interrupt.  It is used to call any registered internal tick
//! functions.
//!
//! This function should only be called from within the USB library.
//!
//! \return None.
//
//*****************************************************************************
void
InternalUSBStartOfFrameTick(uint32_t ui32TicksmS)
{
    int32_t i32Idx;

    //
    // Advance time.
    //
    g_ui32CurrentUSBTick += ui32TicksmS;

    //
    // Call any registered SOF tick handlers.
    //
    for(i32Idx = 0; i32Idx < MAX_USB_TICK_HANDLERS; i32Idx++)
    {
        if(g_pfnTickHandlers[i32Idx])
        {
            g_pfnTickHandlers[i32Idx](g_pvTickInstance[i32Idx], ui32TicksmS);
        }
    }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
