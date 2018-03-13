//*****************************************************************************
//
// usbmode.c - Functions related to dual mode USB device/host operation.
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
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_usb.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/usbdrv.h"
#include "driverlib/rtos_bindings.h"
#include "usblib/usblib.h"
#include "usblib/usblibpriv.h"
#include "usblib/device/usbdevice.h"
#include "usblib/host/usbhost.h"
#include "usblib/host/usbhostpriv.h"
#include "usblib/usblibpriv.h"

//*****************************************************************************
//
//! \addtogroup general_usblib_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// The following label defines interrupts that we will always pass to the host
// interrupt handler even if we are in dual mode and not yet sure of which
// mode we are operating in.
//
//*****************************************************************************
#define USB_HOST_INTS           (USB_INTCTRL_VBUS_ERR)

//*****************************************************************************
//
// Global variable indicating which mode of operation the application has
// requested.
//
//*****************************************************************************
volatile tUSBMode g_iUSBMode = eUSBModeNone;

//*****************************************************************************
//
// The default and the current polling rate for the USB OTG library.
//
//*****************************************************************************
volatile uint32_t g_ui32PollRate;

//*****************************************************************************
//
// The current time remaining in milliseconds before checking the cable
// connection.
//
//*****************************************************************************
volatile uint32_t g_ui32WaitTicks = 0;

//*****************************************************************************
//
// This enum holds the various states that we can be in while performing
// USB mode checking. This involves use of the OTG session request to poll
// the USB ID pin to determine whether a device or a host is connected.
//
//*****************************************************************************
typedef enum
{
    //
    // No checking is currently pending.
    //
    eUSBOTGModeIdle,

    //
    // Waiting on ID mode detection.
    //
    eUSBOTGModeWaitID,

    //
    // Waiting for next poll interval.
    //
    eUSBOTGModeWait,

    //
    // Now in B-side wait for connect.
    //
    eUSBOTGModeBWaitCon,

    //
    // Now in A-side device mode.
    //
    eUSBOTGModeBDevice,

    //
    // Now in A-side host mode.
    //
    eUSBOTGModeAHost,
}
tUSBOTGState;

volatile tUSBOTGState g_eOTGModeState;

//*****************************************************************************
//
// Global variable indicating whether we are currently operating in host or
// device mode if the user has requested Dual mode operation.
//
//*****************************************************************************
static volatile tUSBMode g_iDualMode = eUSBModeNone;

static void USBOTGRemovePower(uint32_t ui32Index);

//*****************************************************************************
//
// Global variable holding a pointer to the callback function which will be
// called when the USB mode changes between device and host.
//
//*****************************************************************************
static tUSBModeCallback g_pfnUSBModeCallback;

//*****************************************************************************
//
// This function is used to handle switching between host, device and
// unconfigured modes.
//
// \param iUSBMode is one of eUSBModeHost, eUSBModeDevice, or eUSBModeNone.
//
// Based on the current state held in g_iDualMode variable this function will
// handle the transition of the mode of operation in OTG mode and calling
// the callback function if it is present.
//
// \return None.
//
//*****************************************************************************
static void
USBOTGSetMode(tUSBMode iUSBMode)
{
    if((g_iDualMode != iUSBMode) || (g_iDualMode == eUSBModeNone))
    {
        //
        // If going from host mode to unconfigured mode then remove power.
        //
        if((g_iDualMode == eUSBModeHost) && (iUSBMode == eUSBModeNone))
        {
            //
            // Take the steps to remove power in the of host mode OTG.
            //
            USBOTGRemovePower(0);
        }

        //
        // If going from device mode to unconfigured mode then end the current
        // session.
        //
        if((g_iDualMode == eUSBModeDevice) && (iUSBMode == eUSBModeNone))
        {
            //
            // End the current session.
            //
            USBOTGSessionRequest(USB0_BASE, false);
        }

        //
        // Reset the delay whenever returning to eUSBModeNone.
        //
        if(iUSBMode == eUSBModeNone)
        {
            g_ui32WaitTicks = g_ui32PollRate;
        }

        //
        // Do we have a mode change callback installed?
        //
        if((g_pfnUSBModeCallback) && (g_iDualMode != iUSBMode))
        {
            //
            // Inform the callback of the new operating mode.
            //
            g_pfnUSBModeCallback(0, iUSBMode);
        }

        //
        // Save the new mode.
        //
        g_iDualMode = iUSBMode;
    }
}

//*****************************************************************************
//
//! Allows dual mode application to switch between USB device and host modes
//! and provides a method to force the controller into the desired mode.
//!
//! \param ui32Index specifies the USB controller whose mode of operation is to
//! be set.  This parameter must be set to 0.
//! \param iUSBMode indicates the mode that the application wishes to operate
//! in.  Valid values are \b eUSBModeDevice to operate as a USB device and
//! \b eUSBModeHost to operate as a USB host.
//! \param pfnCallback is a pointer to a function which the USB library will
//! call each time the mode is changed to indicate the new operating mode.  In
//! cases where \e iUSBMode is set to either \b eUSBModeDevice or
//! \b eUSBModeHost, the callback will be made immediately to allow the
//! application to perform any host or device specific initialization.
//!
//! This function allows a USB application that can operate in host
//! or device mode to indicate to the USB stack the mode that it wishes to
//! use.  The caller is responsible for cleaning up the interface and removing
//! itself from the bus prior to making this call and reconfiguring afterwards.
//! The \e pfnCallback function can be a NULL(0) value to indicate that no
//! notification is required.
//!
//! For successful dual mode mode operation, an application must register
//! USB0DualModeIntHandler() as the interrupt handler for the USB0 interrupt.
//! This handler is responsible for steering interrupts to the device or host
//! stack depending upon the chosen mode.  Devices which do not require dual
//! mode capability should register either \e USB0DeviceIntHandler() or
//! \e USB0HostIntHandler() instead.  Registering \e USB0DualModeIntHandler()
//! for a single mode application will result in an application binary larger
//! than required since library functions for both USB operating modes will be
//! included even though only one mode is required.
//!
//! Single mode applications (those offering exclusively USB device or USB
//! host functionality) are only required to call this function if they need to
//! force the mode of the controller to Host or Device mode.  This is usually
//! in the event that the application needs to reused the USBVBUS and/or USBID
//! pins as GPIOs.
//!
//! \return None.
//
//*****************************************************************************
void
USBStackModeSet(uint32_t ui32Index, tUSBMode iUSBMode,
                tUSBModeCallback pfnCallback)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Index == 0);

    //
    // Remember the mode so that we can steer the interrupts appropriately.
    //
    g_iUSBMode = iUSBMode;

    //
    // Remember the callback pointer.
    //
    g_pfnUSBModeCallback = pfnCallback;

    //
    // If we are being asked to be either a host or device, we will not be
    // trying to auto-detect the mode so make the callback immediately.
    //
    if((iUSBMode == eUSBModeDevice) || (iUSBMode == eUSBModeHost))
    {
        //
        // Make sure that a callback was provided.
        //
        if(g_pfnUSBModeCallback)
        {
            g_pfnUSBModeCallback(0, iUSBMode);
        }
    }
}

//*****************************************************************************
//
// Steers USB interrupts from controller to the correct handler in the USB
// stack.
//
// This interrupt handler is used in applications which require to operate
// in both host and device mode.  It steers the USB hardware interrupt to the
// correct handler in the USB stack depending upon the current operating mode
// of the application, USB device or host.
//
// For successful dual mode operation, an application must register
// USB0DualModeIntHandler() in the CPU vector table as the interrupt handler
// for the USB0 interrupt.  This handler is responsible for steering
// interrupts to the device or host stack depending upon the chosen mode.
//
// \note Devices which do not require dual mode capability should register
// either USB0DeviceIntHandler() or USB0HostIntHandler() instead.  Registering
// USB0DualModeIntHandler() for a single mode application will result in an
// application binary larger than required since library functions for both
// USB operating modes will be included even though only one mode is actually
// required.
//
// \return None.
//
//*****************************************************************************
void
USB0DualModeIntHandler(void)
{
    uint32_t ui32Status;

    //
    // Read the USB interrupt status.
    //
    ui32Status = USBIntStatusControl(USB0_BASE);

    //
    // Pass through the subset of interrupts that we always want
    // the host stack to see regardless of whether or not we
    // are actually in host mode at this point.
    //
    if(ui32Status & USB_HOST_INTS)
    {
        //
        // Call the host's interrupt handler.
        //
        USBHostIntHandlerInternal(0, ui32Status & USB_HOST_INTS);

        //
        // We have already processed these interrupts so clear them
        // from the status.
        //
        ui32Status &= ~USB_HOST_INTS;
    }

    //
    // Steer the interrupt to the appropriate handler within the stack
    // depending upon our current operating mode.  Note that we need to pass
    // the ui32Status parameter since the USB interrupt register is
    // clear-on-read.
    //
    switch(g_iUSBMode)
    {
        case eUSBModeNone:
        {
            //
            // No mode is set yet so we have no idea what to do.  Just ignore
            // the interrupt.
            //
            break;
        }

        //
        // Operating in pure host mode.
        //
        case eUSBModeHost:
        {
            //
            // Call the host interrupt handler if there is anything still to
            // process.
            //
            USBHostIntHandlerInternal(0, ui32Status);

            break;
        }

        //
        // Operating in pure device mode.
        //
        case eUSBModeDevice:
        {
            //
            // Call the device interrupt handler.
            //
            USBDeviceIntHandlerInternal(0, ui32Status);

            break;
        }

        default:
        {
            break;
        }
    }
}

//*****************************************************************************
//
// Close the Doxygen group general_usblib_api.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup dualmode_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! Initializes the USB controller for dual mode operation.
//!
//! \param ui32Index specifies the USB controller that is to be initialized for
//! dual mode operation.  This parameter must be set to 0.
//!
//! This function initializes the USB controller hardware into a state
//! suitable for dual mode operation.  Applications may use this function to
//! ensure that the controller is in a neutral state and able to receive
//! appropriate interrupts before host or device mode is chosen using a call
//! to USBStackModeSet().
//!
//! \return None.
//
//*****************************************************************************
void
USBDualModeInit(uint32_t ui32Index)
{
    //
    // We only support a single USB controller.
    //
    ASSERT(ui32Index == 0);

    //
    // Configure the End point 0.
    //
    USBHostEndpointConfig(USB0_BASE, USB_EP_0, 64, 0, 0,
                          (USB_EP_MODE_CTRL | USB_EP_SPEED_FULL |
                           USB_EP_HOST_OUT));

    //
    // Enable USB Interrupts.
    //
    MAP_USBIntEnableControl(USB0_BASE, USB_INTCTRL_RESET |
                                       USB_INTCTRL_DISCONNECT |
                                       USB_INTCTRL_SESSION |
                                       USB_INTCTRL_BABBLE |
                                       USB_INTCTRL_CONNECT |
                                       USB_INTCTRL_RESUME |
                                       USB_INTCTRL_SUSPEND |
                                       USB_INTCTRL_VBUS_ERR);

    //
    // Enable all endpoint interrupts.
    //
    MAP_USBIntEnableEndpoint(USB0_BASE, USB_INTEP_ALL);

    //
    // Initialize the USB tick module.
    //
    InternalUSBTickInit();

    //
    // Enable the USB interrupt.
    //
    OS_INT_ENABLE(g_psDCDInst[0].ui32IntNum);
    //
    // Turn on session request to enable ID pin checking.
    //
    USBOTGSessionRequest(USB0_BASE, true);

    //
    // Initialize the power configuration.
    //
    USBHostPwrConfig(USB0_BASE, USBHCDPowerConfigGet(ui32Index));

    //
    // If power enable is automatic then then USBHostPwrEnable() has to be
    // called to allow the USB controller to control the power enable pin.
    //
    if(USBHCDPowerAutomatic(ui32Index))
    {
        //
        // This will not turn on power but instead will allow the USB
        // controller to turn on power when needed.
        //
        USBHostPwrEnable(USB0_BASE);
    }
}

//*****************************************************************************
//
//! Returns the USB controller to the default mode when in dual mode operation.
//!
//! \param ui32Index specifies the USB controller whose dual mode operation is
//! to be ended.  This parameter must be set to 0.
//!
//! Applications using both host and device modes may call this function to
//! disable interrupts in preparation for shutdown or a change of operating
//! mode.
//!
//! \return None.
//
//*****************************************************************************
void
USBDualModeTerm(uint32_t ui32Index)
{
    //
    // We only support a single USB controller.
    //
    ASSERT(ui32Index == 0);

    //
    // Disable the USB interrupt.
    //
    OS_INT_DISABLE(g_psDCDInst[0].ui32IntNum);

    MAP_USBIntDisableControl(USB0_BASE, USB_INTCTRL_ALL);

    MAP_USBIntDisableEndpoint(USB0_BASE, USB_INTEP_ALL);
}

//*****************************************************************************
//
// Close the Doxygen group dualmode_api.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup usblib_otg
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! Returns the USB controller to and inactive state when in OTG mode
//! operation.
//!
//! \param ui32Index specifies the USB controller to end OTG mode operations.
//!
//! Applications using OTG mode may call this function to disable interrupts
//! in preparation for shutdown or a change of operating mode.
//!
//! \return None.
//
//*****************************************************************************
void
USBOTGModeTerm(uint32_t ui32Index)
{
    //
    // We only support a single USB controller.
    //
    ASSERT(ui32Index == 0);

    //
    // Disable the USB interrupt.
    //
    OS_INT_DISABLE(g_psDCDInst[0].ui32IntNum);

    //
    // Disable all control interrupts.
    //
    MAP_USBIntDisableControl(USB0_BASE, USB_INTCTRL_ALL);

    //
    // Disable all endpoint interrupts.
    //
    MAP_USBIntDisableEndpoint(USB0_BASE, USB_INTEP_ALL);

    //
    // Set the mode to none if it is not already.
    //
    USBOTGSetMode(eUSBModeNone);
}

//*****************************************************************************
//
//! Initializes the USB controller for OTG mode operation.
//!
//! \param ui32Index specifies the USB controller that is to be initialized for
//! OTG mode operation.
//! \param ui32PollingRate is the rate in milliseconds to poll the controller
//! for changes in mode.
//! \param pvPool is a pointer to the data to use as a memory pool for this
//! controller.
//! \param ui32PoolSize is the size in bytes of the buffer passed in as
//! \e pvPool.
//!
//! This function initializes the USB controller hardware into a state
//! suitable for OTG mode operation.  Applications must use this function to
//! ensure that the controller is in a neutral state and able to receive
//! appropriate interrupts before host or device mode is chosen by OTG
//! negotiation.  The \e ui32PollingRate parameter is used to set the rate at
//! which the USB library will poll the controller to determine the mode.  This
//! has the most effect on how quickly the USB library will detect changes when
//! going to host mode.  The parameters \e pvPool and \e ui32PoolSize are
//! passed on to the USB host library functions to provide memory for the USB
//! library when it is acting as a  host. Any device and host initialization
//! should have been called before calling this function to prevent the USB
//! library from attempting to run in device or host mode before the USB
//! library is fully configured.
//!
//! \return None.
//
//*****************************************************************************
void
USBOTGModeInit(uint32_t ui32Index, uint32_t ui32PollingRate,
               void *pvPool, uint32_t ui32PoolSize)
{
    //
    // We only support a single USB controller.
    //
    ASSERT(ui32Index == 0);

    //
    // This should never be called if not in OTG mode.
    //
    ASSERT(g_iUSBMode == eUSBModeOTG);

    //
    // Force OTG mode in all cases since anything else is invalid, but a DEBUG
    // build will still ASSERT above if this value is incorrect.
    //
    g_iUSBMode = eUSBModeOTG;

    //
    // Remember that we have not yet determined whether we are device or
    // host.
    //
    g_iDualMode = eUSBModeNone;

    //
    // Set the default polling rate.
    //
    g_ui32PollRate = ui32PollingRate;

    //
    // Enable the USB controller.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);

    //
    // Turn on USB Phy clock.
    //
    MAP_SysCtlUSBPLLEnable();

    //
    // Initialize the host controller stack.
    //
    USBHCDInit(ui32Index, pvPool, ui32PoolSize);

    //
    // Configure the End point 0.
    //
    USBHostEndpointConfig(USB0_BASE, USB_EP_0, 64, 0, 0,
                          (USB_EP_MODE_CTRL | USB_EP_SPEED_FULL |
                           USB_EP_HOST_OUT));

    //
    // Enable control interrupts.
    //
    MAP_USBIntEnableControl(USB0_BASE, USB_INTCTRL_RESET |
                                       USB_INTCTRL_DISCONNECT |
                                       USB_INTCTRL_SESSION |
                                       USB_INTCTRL_BABBLE |
                                       USB_INTCTRL_CONNECT |
                                       USB_INTCTRL_RESUME |
                                       USB_INTCTRL_SUSPEND |
                                       USB_INTCTRL_VBUS_ERR |
                                       USB_INTCTRL_MODE_DETECT |
                                       USB_INTCTRL_SOF);

    //
    // Make sure the mode OTG mode and not forced device or host.
    //
    USBOTGMode(USB0_BASE);

    //
    // Enable all endpoint interrupts.
    //
    MAP_USBIntEnableEndpoint(USB0_BASE, USB_INTEP_ALL);

    //
    // Initialize the power configuration.
    //
    USBHCDPowerConfigSet(ui32Index, USBHCDPowerConfigGet(ui32Index));

    //
    // If power enable is automatic then then USBHostPwrEnable() has to be
    // called to allow the USB controller to control the power enable pin.
    //
    if(USBHCDPowerAutomatic(ui32Index))
    {
        //
        // This will not turn on power but instead will allow the USB
        // controller to turn on power when needed.
        //
        USBHostPwrEnable(USB0_BASE);
    }

    //
    // Enable the USB interrupt.
    //
    OS_INT_ENABLE(INT_USB0_BLIZZARD);
}

//*****************************************************************************
//
// This function handles the steps required to remove power in OTG mode.
//
// \param ui32Index specifies which USB controller should remove power.
//
// This function will perform the steps required to remove power from the USB
// bus as required by the OTG specification.  This call will first issue a
// bus suspend followed by clearing the current session and then removing
// power.
//
// \return None.
//
//*****************************************************************************
static void
USBOTGRemovePower(uint32_t ui32Index)
{
    tEventInfo sEvent;

    //
    // Do suspend signaling.
    //
    USBHostSuspend(USB0_BASE);

    //
    // End the session in either device or host mode.
    //
    USBOTGSessionRequest(USB0_BASE, false);

    //
    // Check if the controller is automatically applying power or not.
    //
    if(USBHCDPowerAutomatic(ui32Index) == 0)
    {
        //
        // Call the registered event driver to allow it to disable power.
        //
        sEvent.ui32Event = USB_EVENT_POWER_DISABLE;
        sEvent.ui32Instance = 0;
        InternalUSBHCDSendEvent(0, &sEvent, USBHCD_EVFLAG_PWRDIS);
    }
}

//*****************************************************************************
//
//! This call sets the USB OTG controllers poll rate when checking for the mode
//! of the controller.
//!
//! \param ui32Index specifies which USB controller to set the polling rate.
//! \param ui32PollRate is the rate in milliseconds to poll for changes in the
//! controller mode.
//!
//! This function is called to set the USB OTG libraries polling rate when
//! checking the status of the cable.  The \e ui32PollRate value used sets the
//! rate in milliseconds that the USB OTG library will poll the cable to see
//! if the controller should enter host mode.  This value has no effect on
//! device detection rate as the controller will detect being connected to a
//! host controller automatically.  The \e ui32PollRate can be set to 0 to
//! disable polling.  The USB OTG library can still function with the polling
//! rate set to zero, however it will fail to detect host mode properly when no
//! device is present at the end of the USB OTG B side of the cable.
//!
//! \note This function should only be called on devices that support OTG
//! functionality.
//!
//! \return None.
//
//*****************************************************************************
void
USBOTGPollRate(uint32_t ui32Index, uint32_t ui32PollRate)
{
    //
    // Save the timeout.
    //
    g_ui32PollRate = ui32PollRate;
}

//*****************************************************************************
//
//! Handles OTG mode changes and also steers other USB interrupts from
//! the controller to the correct handler in the USB stack.
//!
//! This interrupt handler is used in applications which require to operate
//! in both host and device mode using OTG.  When in host or device mode, it
//! steers the USB hardware interrupt to the correct handler in the USB stack
//! depending upon the current operating mode.  It also handles other OTG
//! related interrupt events.
//!
//! For successful OTG mode operation, an application must register
//! USB0OTGModeIntHandler() in the CPU vector table as the interrupt handler
//! for the USB0 interrupt.
//!
//! \note This interrupt handler should only be used on controllers that
//! support OTG functionality.
//!
//! \return None.
//
//*****************************************************************************
void
USB0OTGModeIntHandler(void)
{
    uint32_t ui32Status;
    tEventInfo sEvent;

    //
    // Read the USB interrupt status.
    //
    ui32Status = USBIntStatusControl(USB0_BASE);

    //
    // Check if this was an mode detect interrupt and under manual power
    // control.
    //
    if((ui32Status & USB_INTCTRL_MODE_DETECT) &&
       (USBHCDPowerAutomatic(0) == 0))
    {
        uint32_t ui32Mode;

        ui32Mode = USBModeGet(USB0_BASE);

        switch(ui32Mode)
        {
            //
            // Device is on the A side of the cable and power needs to be
            // applied.
            //
            case USB_OTG_MODE_ASIDE_NPWR:
            case USB_OTG_MODE_ASIDE_SESS:
            case USB_OTG_MODE_ASIDE_AVAL:
            {
                //
                // Since power is not automatically enabled, call the
                // registered event handler to allow the application to turn
                // on power.
                //
                sEvent.ui32Event = USB_EVENT_POWER_ENABLE;
                sEvent.ui32Instance = 0;
                InternalUSBHCDSendEvent(0, &sEvent, USBHCD_EVFLAG_PWREN);

                break;
            }

            //
            // Device is on the B side of the cable and powered.
            //
            case USB_OTG_MODE_BSIDE_DEV:
            {
                //
                // Now in device mode on the B side of the cable and will wait
                // for a connect before becoming a device.
                //
                g_eOTGModeState = eUSBOTGModeBWaitCon;

                break;
            }

            //
            // Any other mode detect indicates eUSBModeNone.
            //
            default:
            {
                //
                // Just inform the application that the mode was not device
                // or host.
                //
                USBOTGSetMode(eUSBModeNone);

                break;
            }
        }
    }

    //
    // If there was a VBUS error then the power should be shut off and the
    // system is reset to waiting for detection again.
    //
    if(ui32Status & USB_INTCTRL_VBUS_ERR)
    {
        //
        // Just inform the application that the mode was not device
        // or host.
        //
        USBOTGSetMode(eUSBModeNone);

        //
        // Return to idle mode.
        //
        g_eOTGModeState = eUSBOTGModeWait;
    }

    //
    // If there is a disconnect interrupt and the controller was on the B side
    // cable as a device then go back to the IDLE state.
    //
    if((ui32Status & USB_INTCTRL_DISCONNECT) &&
       (g_eOTGModeState == eUSBOTGModeBDevice))
    {
        //
        // No longer a device so switch to unconfigured mode.
        //
        USBOTGSetMode(eUSBModeNone);

        //
        // Return to idle mode.
        //
        g_eOTGModeState = eUSBOTGModeWait;

        return;
    }

    //
    // Handle receiving a reset.
    //
    if((ui32Status & USB_INTCTRL_RESET)&&
       (g_eOTGModeState != eUSBOTGModeBDevice))
    {
        //
        // Getting a reset interrupt when not already a b side device indicates
        // that a host is resetting the device and the controller should
        // move to device mode.
        //
        g_eOTGModeState = eUSBOTGModeBDevice;

        //
        // Save the new mode.
        //
        USBOTGSetMode(eUSBModeDevice);
    }

    //
    // If there is a connect interrupt while the library is waiting for
    // one then move to full host mode state.
    //
    if(ui32Status & USB_INTCTRL_CONNECT)
    {
        //
        // Move to A side host state.
        //
        g_eOTGModeState = eUSBOTGModeAHost;

        //
        // Inform the application that controller is in host mode.
        //
        USBOTGSetMode(eUSBModeHost);
    }

    //
    // Call the correct device or host interrupt handler based on the current
    // mode of operation.
    //
    switch(g_eOTGModeState)
    {
        case eUSBOTGModeAHost:
        {
            //
            // Call the host interrupt handler if there is anything still to
            // process.
            //
            USBHostIntHandlerInternal(0, ui32Status);

            break;
        }

        //
        // Operating in pure device mode.
        //
        case eUSBOTGModeBDevice:
        {
            //
            // Call the device interrupt handler.
            //
            USBDeviceIntHandlerInternal(0, ui32Status);

            break;
        }
        default:
        {
            break;
        }
    }
}

//*****************************************************************************
//
// This function is called by the USB host stack code to indicated that it
// has completed handing the device disconnection.
//
// \param ui32Index specifies the USB controller that has completed disconnect.
//
// This internal library function is used when the hsot controller has
// completed any deferred handling when it has detected a device has been
// disconnected.  The functions main purpose is to return the OTG controller to
// a state that allows for resuming normal OTG cable detection and negotiation.
//
// \note This function should not be called outside the library.
//
//*****************************************************************************
void
OTGDeviceDisconnect(uint32_t ui32Index)
{
    //
    // This function is only valid when called in host mode.
    //
    if(g_eOTGModeState == eUSBOTGModeAHost)
    {
        //
        // No longer a host so switch to unconfigured mode.
        //
        USBOTGSetMode(eUSBModeNone);

        g_eOTGModeState = eUSBOTGModeWait;
    }
}

//*****************************************************************************
//
//! This function is the main routine for the OTG Controller Driver.
//!
//! \param ui32MsTicks is the number of milliseconds that have passed since the
//! last time this function was called.
//!
//! This function is the main routine for the USB controller when using the
//! library in OTG mode.  This routine must be called periodically by the main
//! application outside of a callback context.  The \e ui32MsTicks value is
//! used for basic timing needed by the USB library when operating in OTG mode.
//! This allows for a simple cooperative system to access the the OTG
//! controller driver interface without the need for an RTOS.  All time
//! critical operations are handled in interrupt context but all longer
//! operations are run from the this function to allow them to block and wait
//! for completion without holding off other interrupts.
//!
//! \return None.
//
//*****************************************************************************
void
USBOTGMain(uint32_t ui32MsTicks)
{
    tEventInfo sEvent;

    if(ui32MsTicks > g_ui32WaitTicks)
    {
        g_ui32WaitTicks = 0;
    }
    else
    {
        g_ui32WaitTicks -= ui32MsTicks;
    }

    switch(g_eOTGModeState)
    {
        case eUSBOTGModeIdle:
        {
            g_eOTGModeState = eUSBOTGModeWaitID;

            //
            // Initiate a session request and check the ID pin.
            //
            USBOTGSessionRequest(USB0_BASE, true);
            break;
        }
        case eUSBOTGModeWait:
        case eUSBOTGModeWaitID:
        {
            //
            // If reached the timeout and polling is enabled then look again.
            //
            if((g_ui32WaitTicks == 0) && (g_ui32PollRate != 0))
            {
                //
                // Remove the session request.
                //
                USBOTGSessionRequest(USB0_BASE, false);

                //
                // Return to idle mode.
                //
                USBOTGSetMode(eUSBModeNone);

                //
                // Check if the controller is automatically applying power or
                // not.
                //
                if(USBHCDPowerAutomatic(0) == 0)
                {
                    //
                    // Call the registered event driver to allow it to disable
                    // power.
                    //
                    sEvent.ui32Event = USB_EVENT_POWER_DISABLE;
                    sEvent.ui32Instance = 0;
                    InternalUSBHCDSendEvent(0, &sEvent, USBHCD_EVFLAG_PWRDIS);
                }

                //
                // Go back to the idle state.
                //
                g_eOTGModeState = eUSBOTGModeIdle;
            }
            break;
        }
        case eUSBOTGModeAHost:
        {
            //
            // Call the host main routine when acting as a host.
            //
            USBHCDMain();
            break;
        }
        case eUSBOTGModeBWaitCon:
        case eUSBOTGModeBDevice:
        default:
        {
            break;
        }
    }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
