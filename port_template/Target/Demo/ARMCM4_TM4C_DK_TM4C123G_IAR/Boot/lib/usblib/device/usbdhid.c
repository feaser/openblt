//*****************************************************************************
//
// usbdhid.c - USB HID device class driver.
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
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/usbdrv.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "usblib/usblib.h"
#include "usblib/usblibpriv.h"
#include "usblib/usbhid.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdhid.h"
#include "usblib/usblibpriv.h"

//*****************************************************************************
//
//! \addtogroup hid_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// The subset of endpoint status flags that we consider to be reception
// errors.  These are passed to the client via USB_EVENT_ERROR if seen.
//
//*****************************************************************************
#define USB_RX_ERROR_FLAGS      (USBERR_DEV_RX_DATA_ERROR |                   \
                                 USBERR_DEV_RX_OVERRUN |                      \
                                 USBERR_DEV_RX_FIFO_FULL)

//*****************************************************************************
//
// Marker used to indicate that a given HID descriptor cannot be found in the
// client-supplied list.
//
//*****************************************************************************
#define HID_NOT_FOUND           0xFFFFFFFF

//*****************************************************************************
//
// Flags that may appear in ui16DeferredOpFlags to indicate some operation that
// has been requested but could not be processed at the time it was received.
// Each deferred operation is defined as the bit number that should be set in
// tHIDInstance->ui16DeferredOpFlags to indicate that the operation is pending.
//
//*****************************************************************************
#define HID_DO_PACKET_RX        5
#define HID_DO_SEND_IDLE_REPORT 6

//*****************************************************************************
//
// Endpoints to use for each of the required endpoints in the driver.
//
//*****************************************************************************
#define INT_IN_ENDPOINT         USB_EP_3
#define INT_OUT_ENDPOINT        USB_EP_3

//*****************************************************************************
//
// Device Descriptor.  This is stored in RAM to allow several fields to be
// changed at runtime based on the client's requirements.
//
//*****************************************************************************
uint8_t g_pui8HIDDeviceDescriptor[] =
{
    18,                         // Size of this structure.
    USB_DTYPE_DEVICE,           // Type of this structure.
    USBShort(0x110),            // USB version 1.1 (if we say 2.0, hosts assume
                                // high-speed - see USB 2.0 spec 9.2.6.6)
    USB_CLASS_DEVICE,           // USB Device Class
    0,                          // USB Device Sub-class
    USB_HID_PROTOCOL_NONE,      // USB Device protocol
    USBDHID_MAX_PACKET,         // Maximum packet size for default pipe.
    USBShort(0),                // Vendor ID (VID).
    USBShort(0),                // Product ID (PID).
    USBShort(0x100),            // Device Version BCD.
    1,                          // Manufacturer string identifier.
    2,                          // Product string identifier.
    3,                          // Product serial number.
    1                           // Number of configurations.
};

//*****************************************************************************
//
// Forward references for device handler callbacks
//
//*****************************************************************************
static void HandleGetDescriptor(void *pvHIDInstance, tUSBRequest *psUSBRequest);
static void HandleRequest(void *pvHIDInstance, tUSBRequest *psUSBRequest);
static void HandleConfigChange(void *pvHIDInstance, uint32_t ui32Info);
static void HandleEP0DataReceived(void *pvHIDInstance, uint32_t ui32Info);
static void HandleEP0DataSent(void *pvHIDInstance, uint32_t ui32Info);
static void HandleReset(void *pvHIDInstance);
static void HandleSuspend(void *pvHIDInstance);
static void HandleResume(void *pvHIDInstance);
static void HandleDisconnect(void *pvHIDInstance);
static void HandleEndpoints(void *pvHIDInstance, uint32_t ui32Status);
static void HandleDevice(void *pvHIDInstance, uint32_t ui32Request,
                         void *pvRequestData);

//*****************************************************************************
//
// The device information structure for the USB HID devices.
//
//*****************************************************************************
const tCustomHandlers g_sHIDHandlers =
{
    //
    // GetDescriptor
    //
    HandleGetDescriptor,

    //
    // RequestHandler
    //
    HandleRequest,

    //
    // InterfaceChange
    //
    0,

    //
    // ConfigChange
    //
    HandleConfigChange,

    //
    // DataReceived
    //
    HandleEP0DataReceived,

    //
    // DataSentCallback
    //
    HandleEP0DataSent,

    //
    // ResetHandler
    //
    HandleReset,

    //
    // SuspendHandler
    //
    HandleSuspend,

    //
    // ResumeHandler
    //
    HandleResume,

    //
    // DisconnectHandler
    //
    HandleDisconnect,

    //
    // EndpointHandler
    //
    HandleEndpoints,

    //
    // Device handler.
    //
    HandleDevice
};

//*****************************************************************************
//
// Set or clear deferred operation flags in an "atomic" manner.
//
// \param pui16DeferredOp points to the flags variable which is to be modified.
// \param ui16Bit indicates which bit number is to be set or cleared.
// \param bSet indicates the state that the flag must be set to.  If \b true,
// the flag is set, if \b false, the flag is cleared.
//
// This function safely sets or clears a bit in a flag variable.  The operation
// makes use of bitbanding to ensure that the operation is atomic (no read-
// modify-write is required).
//
// \return None.
//
//*****************************************************************************
static void
SetDeferredOpFlag(volatile uint16_t *pui16DeferredOp, uint16_t ui16Bit,
                  bool bSet)
{
    //
    // Set the flag bit to 1 or 0 using a bitband access.
    //
    HWREGBITH(pui16DeferredOp, ui16Bit) = bSet ? 1 : 0;
}

//*****************************************************************************
//
// This function is called to clear the counter used to keep track of the time
// elapsed since a given report was last sent.
//
// \param psHIDDevice points to the HID device structure whose report timer is
// to be cleared.
// \param ui8ReportID is the first byte of the report to be sent.  If this
// device offers more than one input report, this value is used to find the
// relevant report timer structure in the psHIDDevice structure.
//
// \return None.
//
//*****************************************************************************
static void
ClearReportTimer(const tUSBDHIDDevice *psHIDDevice, uint8_t ui8ReportID)
{
    uint32_t ui32Loop;

    if(psHIDDevice->ui8NumInputReports > 1)
    {
        //
        // We have more than 1 input report so the report must begin with a
        // byte containing the report ID.  Scan the table we were provided
        // when the device was initialized to find the entry for this report.
        //
        for(ui32Loop = 0; ui32Loop < psHIDDevice->ui8NumInputReports;
            ui32Loop++)
        {
            if(psHIDDevice->psReportIdle[ui32Loop].ui8ReportID == ui8ReportID)
            {
                break;
            }
        }
    }
    else
    {
        ui32Loop = 0;
    }

    //
    // If we drop out of the loop with an index less than ui8NumInputReports,
    // we found the relevant report so clear its timer.
    //
    if(ui32Loop < psHIDDevice->ui8NumInputReports)
    {
        psHIDDevice->psReportIdle[ui32Loop].ui32TimeSinceReportmS = 0;
    }
}

//*****************************************************************************
//
// This function is called to clear the idle period timers for each input
// report supported by the device.
//
// \param psHIDDevice points to the HID device structure whose timers are to be
// cleared.
// \param ui32TimemS is the elapsed time in milliseconds since the last call
// to this function.
//
// \return None.
//
//*****************************************************************************
static void
ClearIdleTimers(const tUSBDHIDDevice *psHIDDevice)
{
    uint32_t ui32Loop;

    //
    // Clear the "time till next report" counters for each input report.
    //
    for(ui32Loop = 0; ui32Loop < psHIDDevice->ui8NumInputReports; ui32Loop++)
    {
        psHIDDevice->psReportIdle[ui32Loop].ui16TimeTillNextmS =
                        psHIDDevice->psReportIdle[ui32Loop].ui8Duration4mS * 4;
    }
}

//*****************************************************************************
//
// This function is called periodically to allow us to process the report idle
// timers.
//
// \param psHIDDevice points to the HID device structure whose timers are to be
// updated.
// \param ui32ElapsedmS indicates the number of milliseconds that have elapsed
// since the last call to this function.
//
// \return None.
//
//*****************************************************************************
static void
ProcessIdleTimers(const tUSBDHIDDevice *psHIDDevice, uint32_t ui32ElapsedmS)
{
    uint32_t ui32Loop, ui32SizeReport;
    void *pvReport;
    tHIDInstance *psInst;
    bool bDeferred;

    //
    // Get our instance data pointer
    //
    psInst = &((tUSBDHIDDevice *)psHIDDevice)->sPrivateData;

    //
    // We have not had to defer any report transmissions yet.
    //
    bDeferred = false;

    //
    // Look at each of the input report idle timers in turn.
    //
    for(ui32Loop = 0; ui32Loop < psHIDDevice->ui8NumInputReports; ui32Loop++)
    {
        //
        // Update the time since the last report was sent.
        //
        psHIDDevice->psReportIdle[ui32Loop].ui32TimeSinceReportmS +=
                                                                ui32ElapsedmS;

        //
        // Is this timer running?
        //
        if(psHIDDevice->psReportIdle[ui32Loop].ui8Duration4mS)
        {
            //
            // Yes - is it about to expire?
            //
            if(psHIDDevice->psReportIdle[ui32Loop].ui16TimeTillNextmS <=
               ui32ElapsedmS)
            {
                //
                // The timer is about to expire.  Can we send a report right
                // now?
                //
                if((psInst->iHIDTxState == eHIDStateIdle) &&
                   (psInst->bSendInProgress == false))
                {
                    //
                    // We can send a report so send a message to the
                    // application to retrieve its latest report for
                    // transmission to the host.
                    //
                    ui32SizeReport = psHIDDevice->pfnRxCallback(
                               psHIDDevice->pvRxCBData,
                               USBD_HID_EVENT_IDLE_TIMEOUT,
                               psHIDDevice->psReportIdle[ui32Loop].ui8ReportID,
                               &pvReport);

                    //
                    // Schedule the report for transmission.
                    //
                    USBDHIDReportWrite((void *)psHIDDevice, pvReport,
                                       ui32SizeReport, true);

                    //
                    // Reload the timer for the next period.
                    //
                    psHIDDevice->psReportIdle[ui32Loop].ui16TimeTillNextmS =
                        psHIDDevice->psReportIdle[ui32Loop].ui8Duration4mS * 4;
                }
                else
                {
                    //
                    // We can't send the report straight away so flag it for
                    // transmission as soon as the previous transmission ends.
                    //
                    psHIDDevice->psReportIdle[ui32Loop].ui16TimeTillNextmS = 0;
                    bDeferred = true;
                }
            }
            else
            {
                //
                // The timer is not about to expire.  Update the time till the
                // next report transmission.
                //
                psHIDDevice->psReportIdle[ui32Loop].ui16TimeTillNextmS -=
                                                                ui32ElapsedmS;
            }
        }
    }

    //
    // If we had to defer transmission of any report, remember this so that we
    // will process it as soon as possible.
    //
    SetDeferredOpFlag(&psInst->ui16DeferredOpFlags, HID_DO_SEND_IDLE_REPORT,
                      bDeferred);
}

static void
SetIdleTimeout(const tUSBDHIDDevice *psHIDDevice, uint8_t ui8ReportID,
               uint8_t ui8Timeout4mS)
{
    uint32_t ui32Loop;
    bool bReportNeeded;
    tHIDReportIdle *psIdle;

    //
    // Remember that we have not found any report that needs to be sent
    // immediately.
    //
    bReportNeeded = false;

    //
    // Search through all the input reports looking for ones that fit the
    // requirements.
    //
    for(ui32Loop = 0; ui32Loop < psHIDDevice->ui8NumInputReports; ui32Loop++)
    {
        psIdle = &psHIDDevice->psReportIdle[ui32Loop];

        //
        // If the report ID passed matches the report ID in the idle timer
        // control structure or we were passed a report ID of zero, which
        // indicates that all timers are to be set...
        //
        if(!ui8ReportID || (ui8ReportID == psIdle->ui8ReportID))
        {
            //
            // Save the new duration for the idle timer.
            //
            psIdle->ui8Duration4mS = ui8Timeout4mS;

            //
            // Are we enabling the idle timer?  If so, fix up the time until it
            // needs to fire.
            //
            if(ui8Timeout4mS)
            {
                //
                // Determine what the timeout is for this report given the time
                // since the last report of this type was sent.
                //
                if(psIdle->ui32TimeSinceReportmS >=
                   ((uint32_t)ui8Timeout4mS * 4))
                {
                    psIdle->ui16TimeTillNextmS = 0;
                    bReportNeeded = true;
                }
                else
                {
                    psIdle->ui16TimeTillNextmS =
                                        (((uint16_t)ui8Timeout4mS * 4) -
                                         psIdle->ui32TimeSinceReportmS);
                }
            }
        }
    }

    //
    // If we get to here and bReportNeeded is true, this means we need to
    // send back at least one of the input reports as soon as possible.  Try
    // to do this immediately.
    //
    if(bReportNeeded)
    {
        ProcessIdleTimers(psHIDDevice, 0);
    }
}

//*****************************************************************************
//
// Find the idle timeout for a given HID input report.
//
// \param psHIDDevice points to the HID device whose report idle timeout is to
// be found.
// \param ui8ReportID identifies the report whose timeout is requested.  If 0,
// the timeout for the first report is returns, regardless of its ID (or
// whether it has one).
//
// This function returns the current idle timeout for a given HID input report.
// The value returned is expressed in terms of 4mS intervals.  Convert to
// milliseconds by multiplying by 4.  If the return value is 0, this indicates
// that an infinite timeout is currently set and the device will not send the
// report unless a state change occurs.
//
// \return Returns the current idle timeout for the given report.
//
//*****************************************************************************
static uint32_t
GetIdleTimeout(const tUSBDHIDDevice *psHIDDevice, uint8_t ui8ReportID)
{
    uint32_t ui32Loop;
    tHIDReportIdle *psIdle;

    //
    // Search through all the input reports looking for ones that fit the
    // requirements.
    //
    for(ui32Loop = 0; ui32Loop < psHIDDevice->ui8NumInputReports; ui32Loop++)
    {
        psIdle = &psHIDDevice->psReportIdle[ui32Loop];

        //
        // If the report ID passed matches the report ID in the idle timer
        // control structure or we were passed a report ID of zero, which
        // indicates that all timers are to be set...
        //
        if(!ui8ReportID || (ui8ReportID == psIdle->ui8ReportID))
        {
            //
            // We found a report matching the required ID or we were not passed
            // an ID and we are looking at the first report information.
            //
            return((uint32_t)psIdle->ui8Duration4mS);
        }
    }

    //
    // If we drop out, the report could not be found so we need to indicate
    // an error.
    //
    return(HID_NOT_FOUND);
}

//*****************************************************************************
//
// Find the n-th HID class descriptor of a given type in the client-provided
// descriptor table.
//
// \param psHIDDevice points to the HID device which is to be searched for the
// required class descriptor.
// \param ui8Type is the type of class descriptor being requested.  This will
// be either USB_HID_DTYPE_REPORT or USB_HID_DTYPE_PHYSICAL.
// \param ui32Index is the zero-based index of the descriptor that is being
// requested.
//
// This function parses the supplied HID descriptor to find the index into the
// sClassDescriptor array that corresponds to the requested descriptor.  If
// a descriptor with the requested index does not exist, HID_NOT_FOUND will be
// returned unless the request is for a physical descriptor and at least one
// such descriptor exists.  In this case, the index returned will be for the
// last physical descriptor (as required by the HID spec 7.1.1).
//
// \return Returns the index of the descriptor within the sClassDescriptor
// of the tHIDDevice structure if found or HID_NOT_FOUND otherwise.
//
//*****************************************************************************
static uint32_t
FindHIDDescriptor(const tUSBDHIDDevice *psHIDDevice, uint8_t ui8Type,
                  uint32_t ui32Index, uint32_t *pui32Len)
{
    bool bFoundType;
    uint32_t ui32Loop, ui32Count, ui32LastFound;
    const tHIDClassDescriptorInfo *psDesc;

    //
    // Remember that we have not found any descriptor with a matching type yet.
    //
    bFoundType = false;
    ui32Count = 0;
    ui32LastFound = 0;

    //
    // Walk through all the class descriptors looking for the one which
    // matches the requested index and type.
    //
    for(ui32Loop = 0; ui32Loop < psHIDDevice->psHIDDescriptor->bNumDescriptors;
        ui32Loop++)
    {
        psDesc = &(psHIDDevice->psHIDDescriptor->sClassDescriptor[ui32Loop]);
        if(psDesc->bDescriptorType == ui8Type)
        {
            //
            // We found a descriptor of the correct type.  Is this the
            // correct index?
            //
            bFoundType = true;

            //
            // Is this the descriptor we are looking for?
            //
            if(ui32Count == ui32Index)
            {
                //
                // Yes - we found it so return the index and size to the
                // caller.
                //
                *pui32Len = (uint32_t)psDesc->wDescriptorLength;
                return(ui32Loop);
            }
            else
            {
                //
                // Update our count and keep looking.  Remember where we were
                // when we found this descriptor in case we need to return the
                // last physical descriptor.
                //
                ui32Count++;
                ui32LastFound = ui32Loop;
            }
        }
    }

    //
    // If we drop out, we did not find the requested descriptor.  Now handle
    // the special case of a physical descriptor - if we found any physical
    // descriptors, return the last one.
    //
    if((ui8Type == USB_HID_DTYPE_PHYSICAL) && bFoundType)
    {
        //
        // Get the length of the last descriptor we found.
        //
        psDesc =
            &(psHIDDevice->psHIDDescriptor->sClassDescriptor[ui32LastFound]);
        *pui32Len = (uint32_t)psDesc->wDescriptorLength;

        //
        // Return the index to the caller.
        //
        return(ui32LastFound);
    }
    else
    {
        //
        // We could not find the descriptor so return an appropriate error.
        //
        return(HID_NOT_FOUND);
    }
}

//*****************************************************************************
//
// Schedule transmission of the next packet forming part of an input report.
//
// \param psHIDInst points to the HID device instance whose input report is to
// be sent.
//
// This function is called to transmit the next packet of an input report
// passed to the driver via a call to USBDHIDReportWrite.  If any data remains
// to be sent, a USB packet is written to the FIFO and scheduled for
// transmission to the host.  The function ensures that reports are sent as
// a sequence of full packets followed by either a single int16_t packet or a
// packet with no data to indicate the end of the transaction.
//
//*****************************************************************************
static int32_t
ScheduleReportTransmission(tHIDInstance *psHIDInst)
{
    uint32_t ui32NumBytes;
    uint8_t *pui8Data;
    int32_t i32Retcode;

    //
    // Set the number of bytes to send this iteration.
    //
    ui32NumBytes = (uint32_t)(psHIDInst->ui16InReportSize -
                              psHIDInst->ui16InReportIndex);

    //
    // Limit individual transfers to the maximum packet size for the endpoint.
    //
    if(ui32NumBytes > USBDHID_MAX_PACKET)
    {
        ui32NumBytes = USBDHID_MAX_PACKET;
    }

    //
    // Where are we sending this data from?
    //
    pui8Data = psHIDInst->pui8InReportData + psHIDInst->ui16InReportIndex;

    //
    // Put the data in the correct FIFO.
    //
    i32Retcode = MAP_USBEndpointDataPut(psHIDInst->ui32USBBase,
                                        psHIDInst->ui8INEndpoint,
                                        pui8Data, ui32NumBytes);

    if(i32Retcode != -1)
    {
        //
        // Update the count and index ready for the next time round.
        //
        psHIDInst->ui16InReportIndex += ui32NumBytes;

        //
        // Send out the current data.
        //
        i32Retcode = MAP_USBEndpointDataSend(psHIDInst->ui32USBBase,
                                             psHIDInst->ui8INEndpoint,
                                             USB_TRANS_IN);
    }

    //
    // Tell the caller how we got on.
    //
    return(i32Retcode);
}

//*****************************************************************************
//
// Receives notifications related to data received from the host.
//
// \param psHIDDevice is the device instance whose endpoint is to be processed.
// \param ui32Status is the USB interrupt status that caused this function to
// be called.
//
// This function is called from HandleEndpoints for all interrupts signaling
// the arrival of data on the interrupt OUT endpoint (in other words, whenever
// the host has sent us a packet of data).  We inform the client that a packet
// is available and, on return, check to see if the packet has been read.  If
// not, we schedule another notification to the client for a later time.
//
// \return Returns \b true on success or \b false on failure.
//
//*****************************************************************************
static bool
ProcessDataFromHost(tUSBDHIDDevice *psHIDDevice, uint32_t ui32Status)
{
    uint32_t ui32EPStatus, ui32Size;
    tHIDInstance *psInst;

    //
    // Get a pointer to our instance data.
    //
    psInst = &psHIDDevice->sPrivateData;

    //
    // Get the endpoint status to see why we were called.
    //
    ui32EPStatus = MAP_USBEndpointStatus(USB0_BASE, psInst->ui8OUTEndpoint);

    //
    // Clear the status bits.
    //
    MAP_USBDevEndpointStatusClear(USB0_BASE, psInst->ui8OUTEndpoint,
                                  ui32EPStatus);

    //
    // Has a packet been received?
    //
    if(ui32EPStatus & USB_DEV_RX_PKT_RDY)
    {
        //
        // Set the flag we use to indicate that a packet read is pending.  This
        // will be cleared if the packet is read.  If the client does not read
        // the packet in the context of the USB_EVENT_RX_AVAILABLE callback,
        // the event will be signaled later during tick processing.
        //
        SetDeferredOpFlag(&psInst->ui16DeferredOpFlags, HID_DO_PACKET_RX,
                          true);

        //
        // How big is the packet we have just been sent?
        //
        ui32Size = MAP_USBEndpointDataAvail(psInst->ui32USBBase,
                                            psInst->ui8OUTEndpoint);

        //
        // The receive channel is not blocked so let the caller know
        // that a packet is waiting.  The parameters are set to indicate
        // that the packet has not been read from the hardware FIFO yet.
        //
        psHIDDevice->pfnRxCallback(psHIDDevice->pvRxCBData,
                                   USB_EVENT_RX_AVAILABLE, ui32Size,
                                   (void *)0);
    }
    else
    {
        //
        // No packet was received.  Some error must have been reported.  Check
        // and pass this on to the client if necessary.
        //
        if(ui32EPStatus & USB_RX_ERROR_FLAGS)
        {
            //
            // This is an error we report to the client so...
            //
            psHIDDevice->pfnRxCallback(psHIDDevice->pvRxCBData,
                                       USB_EVENT_ERROR,
                                       (ui32EPStatus & USB_RX_ERROR_FLAGS),
                                       (void *)0);
        }
        return(false);
    }

    return(true);
}

//*****************************************************************************
//
// Receives notifications related to data sent to the host.
//
// \param psHIDDevice is the device instance whose endpoint is to be processed.
// \param ui32Status is the USB interrupt status that caused this function to
// be called.
//
// This function is called from HandleEndpoints for all interrupts originating
// from the interrupt IN endpoint (in other words, whenever data has been
// transmitted to the USB host).  We examine the cause of the interrupt and,
// if due to completion of a transmission, notify the client.
//
// \return Returns \b true on success or \b false on failure.
//
//*****************************************************************************
static bool
ProcessDataToHost(tUSBDHIDDevice *psHIDDevice, uint32_t ui32Status)
{
    tHIDInstance *psInst;
    uint32_t ui32EPStatus;

    //
    // Get a pointer to our instance data.
    //
    psInst = &psHIDDevice->sPrivateData;

    //
    // Get the endpoint status to see why we were called.
    //
    ui32EPStatus = MAP_USBEndpointStatus(psInst->ui32USBBase,
                                         psInst->ui8INEndpoint);

    //
    // Clear the status bits.
    //
    MAP_USBDevEndpointStatusClear(psInst->ui32USBBase, psInst->ui8INEndpoint,
                                  ui32EPStatus);

    //
    // Our last packet was transmitted successfully.  Is there any more data to
    // send or have we finished sending the whole report?  We know we finished
    // if the ui16InReportIndex has reached the ui16InReportSize value.
    //
    if(psInst->ui16InReportSize == psInst->ui16InReportIndex)
    {
        //
        // We finished sending the last report so are idle once again.
        //
        psInst->iHIDTxState = eHIDStateIdle;

        //
        // Notify the client that the report transmission completed.
        //
        psHIDDevice->pfnTxCallback(psHIDDevice->pvTxCBData,
                                   USB_EVENT_TX_COMPLETE,
                                   psInst->ui16InReportSize, (void *)0);

        //
        // Do we have any reports to send as a result of idle timer timeouts?
        //
        if(psInst->ui16DeferredOpFlags & (1 << HID_DO_SEND_IDLE_REPORT))
        {
            //
            // Yes - send reports for any timers that expired recently.
            //
            ProcessIdleTimers(psHIDDevice, 0);
        }
    }
    else
    {
        //
        // There must be more data or a zero length packet waiting to be sent
        // so go ahead and do this.
        //
        ScheduleReportTransmission(psInst);
    }

    return(true);
}

//*****************************************************************************
//
// Called by the USB stack for any activity involving one of our endpoints
// other than EP0.  This function is a fan out that merely directs the call to
// the correct handler depending upon the endpoint and transaction direction
// signaled in ui32Status.
//
//*****************************************************************************
static void
HandleEndpoints(void *pvHIDInstance, uint32_t ui32Status)
{
    tUSBDHIDDevice *psHIDInst;
    tHIDInstance *psInst;

    ASSERT(pvHIDInstance != 0);

    //
    // Determine if the serial device is in single or composite mode because
    // the meaning of ui32Index is different in both cases.
    //
    psHIDInst = (tUSBDHIDDevice *)pvHIDInstance;
    psInst = &psHIDInst->sPrivateData;

    //
    // Handler for the interrupt OUT data endpoint.
    //
    if(ui32Status & (0x10000 << USBEPToIndex(psInst->ui8OUTEndpoint)))
    {
        //
        // Data is being sent to us from the host.
        //
        ProcessDataFromHost(pvHIDInstance, ui32Status);
    }

    //
    // Handler for the interrupt IN data endpoint.
    //
    if(ui32Status & (1 << USBEPToIndex(psInst->ui8INEndpoint)))
    {
        ProcessDataToHost(pvHIDInstance, ui32Status);
    }
}

//*****************************************************************************
//
// Called by the USB stack whenever a configuration change occurs.
//
//*****************************************************************************
static void
HandleConfigChange(void *pvHIDInstance, uint32_t ui32Info)
{
    tHIDInstance *psInst;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvHIDInstance != 0);

    //
    // Create the instance pointer.
    //
    psHIDDevice = pvHIDInstance;

    //
    // Get a pointer to our instance data.
    //
    psInst = &psHIDDevice->sPrivateData;

    //
    // Set all our endpoints to idle state.
    //
    psInst->iHIDRxState = eHIDStateIdle;
    psInst->iHIDTxState = eHIDStateIdle;

    //
    // If we are not currently connected let the client know we are open for
    // business.
    //
    if(!psInst->bConnected)
    {
        //
        // Pass the connected event to the client.
        //
        psHIDDevice->pfnRxCallback(psHIDDevice->pvRxCBData,
                                   USB_EVENT_CONNECTED, 0, (void *)0);
    }

    //
    // Clear the idle timers for each input report.
    //
    ClearIdleTimers(psHIDDevice);

    //
    // Remember that we are connected.
    //
    psInst->bConnected = true;
}

//*****************************************************************************
//
// Device instance specific handler.
//
//*****************************************************************************
static void
HandleDevice(void *pvHIDInstance, uint32_t ui32Request, void *pvRequestData)
{
    tHIDInstance *psInst;
    uint8_t *pui8Data;

    //
    // Create the serial instance data.
    //
    psInst = &((tUSBDHIDDevice *)pvHIDInstance)->sPrivateData;

    //
    // Create the int8_t array used by the events supported by the USB CDC
    // serial class.
    //
    pui8Data = (uint8_t *)pvRequestData;

    switch(ui32Request)
    {
        //
        // This was an interface change event.
        //
        case USB_EVENT_COMP_IFACE_CHANGE:
        {
            psInst->ui8Interface = pui8Data[1];
            break;
        }

        //
        // This was an endpoint change event.
        //
        case USB_EVENT_COMP_EP_CHANGE:
        {
            //
            // Determine if this is an IN or OUT endpoint that has changed.
            //
            if(pui8Data[0] & USB_EP_DESC_IN)
            {
                psInst->ui8INEndpoint = IndexToUSBEP((pui8Data[1] & 0x7f));
            }
            else
            {
                //
                // Extract the new endpoint number.
                //
                psInst->ui8OUTEndpoint = IndexToUSBEP(pui8Data[1] & 0x7f);
            }
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
// This function is called by the USB device stack whenever the device is
// disconnected from the host.
//
//*****************************************************************************
static void
HandleDisconnect(void *pvHIDInstance)
{
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvHIDInstance != 0);

    //
    // Create the instance pointer.
    //
    psHIDDevice = (tUSBDHIDDevice *)pvHIDInstance;

    //
    // If we are not currently connected so let the client know we are open
    // for business.
    //
    if(psHIDDevice->sPrivateData.bConnected)
    {
        //
        // Pass the disconnected event to the client.
        //
        psHIDDevice->pfnRxCallback(psHIDDevice->pvRxCBData,
                                   USB_EVENT_DISCONNECTED, 0, (void *)0);
    }

    //
    // Remember that we are no longer connected.
    //
    psHIDDevice->sPrivateData.bConnected = false;
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever a request for a
// non-standard descriptor is received.
//
// \param pvHIDInstance is the instance data for this request.
// \param psUSBRequest points to the request received.
//
// This call parses the provided request structure and determines which
// descriptor is being requested.  Assuming the descriptor can be found, it is
// scheduled for transmission via endpoint zero.  If the descriptor cannot be
// found, the endpoint is stalled to indicate an error to the host.
//
//*****************************************************************************
static void
HandleGetDescriptor(void *pvHIDInstance, tUSBRequest *psUSBRequest)
{
    uint32_t ui32Size, ui32Desc;
    const tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvHIDInstance != 0);

    //
    // Which device are we dealing with?
    //
    psHIDDevice = pvHIDInstance;

    //
    // Which type of class descriptor are we being asked for?
    //
    switch(psUSBRequest->wValue >> 8)
    {
        //
        // This is a request for a HID report or physical descriptor.
        //
        case USB_HID_DTYPE_REPORT:
        case USB_HID_DTYPE_PHYSICAL:
        {
            //
            // Find the index to the descriptor that is being queried.
            //
            ui32Size = 0;
            ui32Desc = FindHIDDescriptor(psHIDDevice,
                                         psUSBRequest->wValue >> 8,
                                         psUSBRequest->wValue & 0xFF,
                                         &ui32Size);

            //
            // Did we find the descriptor?
            //
            if(ui32Desc == HID_NOT_FOUND)
            {
                //
                // No - stall the endpoint and return.
                //
                USBDCDStallEP0(0);
                return;
            }

            //
            // If there is more data to send than the host requested then just
            // send the requested amount of data.
            //
            if(ui32Size > psUSBRequest->wLength)
            {
                ui32Size = psUSBRequest->wLength;
            }

            //
            // Send the data via endpoint 0.
            //
            USBDCDSendDataEP0(0,
                (uint8_t *)psHIDDevice->ppui8ClassDescriptors[ui32Desc],
                ui32Size);

            break;
        }

        //
        // This is a request for the HID descriptor (as found in the
        // configuration descriptor following the relevant interface).
        //
        case USB_HID_DTYPE_HID:
        {
            //
            // How big is the HID descriptor?
            //
            ui32Size = (uint32_t)psHIDDevice->psHIDDescriptor->bLength;

            //
            // If there is more data to send than the host requested then just
            // send the requested amount of data.
            //
            if(ui32Size > psUSBRequest->wLength)
            {
                ui32Size = psUSBRequest->wLength;
            }

            //
            // Send the data via endpoint 0.
            //
            USBDCDSendDataEP0(0, (uint8_t *)psHIDDevice->psHIDDescriptor,
                              ui32Size);
            break;
        }

        //
        // This was an unknown request so stall.
        //
        default:
        {
            USBDCDStallEP0(0);
            break;
        }
    }
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever a non-standard
// request is received.
//
// \param pvHIDInstance is the instance data for this HID device.
// \param psUSBRequest points to the request received.
//
// This call parses the provided request structure.  Assuming the request is
// understood, it is handled and any required response generated.  If the
// request cannot be handled by this device class, endpoint zero is stalled to
// indicate an error to the host.
//
//*****************************************************************************
static void
HandleRequest(void *pvHIDInstance, tUSBRequest *psUSBRequest)
{
    tHIDInstance *psInst;
    uint8_t ui8Protocol;
    uint32_t ui32Size, ui32Timeout;
    uint8_t *pui8Report;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvHIDInstance != 0);

    //
    // Which device are we dealing with?
    //
    psHIDDevice = pvHIDInstance;

    //
    // Get a pointer to our instance data.
    //
    psInst = &psHIDDevice->sPrivateData;

    //
    // Make sure the request was for this interface.
    //
    if(psUSBRequest->wIndex != psInst->ui8Interface)
    {
        return;
    }

    //
    // Determine the type of request.
    //
    switch(psUSBRequest->bRequest)
    {
        //
        // A Set Report request is received from the host when it sends an
        // Output report via endpoint 0.
        //
        case USBREQ_SET_REPORT:
        {
            //
            // Ask the application for a buffer large enough to hold the
            // report we are to be sent.
            //
            psInst->ui16OutReportSize = psUSBRequest->wLength;
            psInst->pui8OutReportData =
                (uint8_t *)psHIDDevice->pfnRxCallback(
                               psHIDDevice->pvRxCBData,
                               USBD_HID_EVENT_GET_REPORT_BUFFER,
                               psUSBRequest->wValue,
                               (void *)(uint32_t)(psUSBRequest->wLength));

            //
            // Did the client provide us a buffer?
            //
            if(!psInst->pui8OutReportData)
            {
                //
                // The application could not provide us a buffer so stall the
                // request.
                //
                USBDCDStallEP0(0);
            }
            else
            {
                //
                // The client provided us a buffer to read the report into
                // so request the data from the host.
                //

                //
                // Set the state to indicate we are waiting for data.
                //
                psInst->iHIDRxState = eHIDStateWaitData;

                //
                // Now read the payload of the request.  We handle the actual
                // operation in the data callback once this data is received.
                //
                USBDCDRequestDataEP0(0, psInst->pui8OutReportData,
                                     (uint32_t)psUSBRequest->wLength);

                //
                // Need to ACK the data on end point 0 in this case.  Do this
                // after requesting the data to prevent race conditions that
                // occur if you acknowledge before setting up to receive the
                // request data.
                //
                MAP_USBDevEndpointDataAck(psInst->ui32USBBase, USB_EP_0, false);
            }

            break;
        }

        //
        // A Get Report request is used by the host to poll a device for its
        // current state.
        //
        case USBREQ_GET_REPORT:
        {
            //
            // Get the latest report from the application.
            //
            ui32Size = psHIDDevice->pfnRxCallback(psHIDDevice->pvRxCBData,
                                            USBD_HID_EVENT_GET_REPORT,
                                            psUSBRequest->wValue, &pui8Report);

            //
            // Need to ACK the data on end point 0 in this case.
            //
            MAP_USBDevEndpointDataAck(psInst->ui32USBBase, USB_EP_0, true);

            //
            // ..then send back the requested report.
            //
            psInst->bGetRequestPending = true;
            USBDCDSendDataEP0(0, pui8Report, ui32Size);

            break;
        }

        //
        // A set IDLE request has been made.  This indicates to us how often a
        // given report should be sent back to the host in the absence of any
        // change in state of the device.
        //
        case USBREQ_SET_IDLE:
        {
            //
            // Set the idle timeout for the requested report(s).
            //
            SetIdleTimeout(psHIDDevice, psUSBRequest->wValue & 0xFF,
                           (psUSBRequest->wValue >> 8) & 0xFF);

            //
            // Need to ACK the data on end point 0 in this case.
            //
            MAP_USBDevEndpointDataAck(psInst->ui32USBBase, USB_EP_0, true);

            break;
        }

        //
        // A get IDLE request has been made.  This request queries the current
        // idle timeout for a given report.
        //
        case USBREQ_GET_IDLE:
        {
            //
            // Determine the timeout for the requested report.
            //
            ui32Timeout = GetIdleTimeout(psHIDDevice, psUSBRequest->wValue);

            if(ui32Timeout != HID_NOT_FOUND)
            {
                //
                // Need to ACK the data on end point 0 in this case.
                //
                MAP_USBDevEndpointDataAck(psInst->ui32USBBase, USB_EP_0, true);

                //
                // Send our response to the host.
                //
                USBDCDSendDataEP0(0, (uint8_t *)&ui32Timeout, 1);
            }
            else
            {
                //
                // The report ID was not found so stall the endpoint.
                //
                USBDCDStallEP0(0);
            }
            break;
        }

        //
        // Set either boot or report protocol for reports sent from the device.
        // This is only supported by devices in the boot subclass.
        //
        case USBREQ_SET_PROTOCOL:
        {
            if(psHIDDevice->ui8Subclass == USB_HID_SCLASS_BOOT)
            {
                //
                // We need to ACK the data on end point 0 in this case.
                //
                MAP_USBDevEndpointDataAck(psInst->ui32USBBase, USB_EP_0, true);

                //
                // We are a boot subclass device so pass this on to the
                // application.
                //
                psHIDDevice->pfnRxCallback(psHIDDevice->pvRxCBData,
                                           USBD_HID_EVENT_SET_PROTOCOL,
                                           psUSBRequest->wValue,
                                           (void *)0);
            }
            else
            {
                //
                // This is not a boot subclass device so stall the endpoint to
                // show that we don't support this request.
                //
                USBDCDStallEP0(0);
            }
            break;
        }

        //
        // Inform the host of the protocol, boot or report, that is currently
        // in use.  This is only supported by devices in the boot subclass.
        //
        case USBREQ_GET_PROTOCOL:
        {
            if(psHIDDevice->ui8Subclass == USB_HID_SCLASS_BOOT)
            {
                //
                // We need to ACK the data on end point 0 in this case.
                //
                MAP_USBDevEndpointDataAck(psInst->ui32USBBase, USB_EP_0, true);

                //
                // We are a boot subclass device so pass this on to the
                // application callback to get the answer.
                //
                ui8Protocol = (uint8_t)psHIDDevice->pfnRxCallback(
                    psHIDDevice->pvRxCBData, USBD_HID_EVENT_GET_PROTOCOL, 0,
                    (void *)0);

                //
                // Send our response to the host.
                //
                USBDCDSendDataEP0(0, (uint8_t *)&ui8Protocol, 1);
            }
            else
            {
                //
                // This is not a boot subclass device so stall the endpoint to
                // show that we don't support this request.
                //
                USBDCDStallEP0(0);
            }
            break;
        }

        //
        // This request was not recognized so stall.
        //
        default:
        {
            USBDCDStallEP0(0);
            break;
        }
    }
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the data requested
// on endpoint zero is received.
//
//*****************************************************************************
static void
HandleEP0DataReceived(void *pvHIDInstance, uint32_t ui32DataSize)
{
    tHIDInstance *psInst;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvHIDInstance != 0);

    //
    // Which device are we dealing with?
    //
    psHIDDevice = pvHIDInstance;

    //
    // If we were not passed any data, just return.
    //
    if(ui32DataSize == 0)
    {
        return;
    }

    //
    // Get our instance data pointer.
    //
    psInst = &psHIDDevice->sPrivateData;

    //
    // Make sure we are actually expecting something.
    //
    if(psInst->iHIDRxState != eHIDStateWaitData)
    {
        return;
    }

    //
    // Change the endpoint state back to idle now that we have been passed
    // the data we were waiting for.
    //
    psInst->iHIDRxState = eHIDStateIdle;

    //
    // The only things we ever request via endpoint zero are reports sent to
    // us via a Set_Report request.  Pass the newly received report on to
    // the client.
    //
    psHIDDevice->pfnRxCallback(psHIDDevice->pvRxCBData,
                               USBD_HID_EVENT_SET_REPORT,
                               psInst->ui16OutReportSize,
                               psInst->pui8OutReportData);
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the data sent on
// endpoint zero is received and acknowledged by the host.
//
//*****************************************************************************
static void
HandleEP0DataSent(void *pvHIDInstance, uint32_t ui32Info)
{
    tHIDInstance *psInst;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvHIDInstance != 0);

    //
    // Which device are we dealing with?
    //
    psHIDDevice = pvHIDInstance;

    //
    // Get our instance data pointer.
    //
    psInst = &psHIDDevice->sPrivateData;

    //
    // If we just sent a report in response to a Get_Report request, send an
    // event to the application telling it that the transmission completed.
    //
    if(psInst->bGetRequestPending)
    {
        //
        // Clear the flag now that we are sending the application callback.
        //
        psInst->bGetRequestPending = false;

        psHIDDevice->pfnRxCallback(psHIDDevice->pvRxCBData,
                                   USBD_HID_EVENT_REPORT_SENT, 0, (void *)0);
    }

    return;
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the device is
// reset.  If we are currently connected, send a disconnect event at this
// point.
//
//*****************************************************************************
static void
HandleReset(void *pvHIDInstance)
{
    ASSERT(pvHIDInstance != 0);

    //
    // Merely call the disconnect handler.  This causes a disconnect message to
    // be sent to the client if we think we are currently connected.
    //
    HandleDisconnect(pvHIDInstance);
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the bus is put into
// suspend state.
//
//*****************************************************************************
static void
HandleSuspend(void *pvHIDInstance)
{
    const tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvHIDInstance != 0);

    //
    // Create the instance pointer.
    //
    psHIDDevice = (const tUSBDHIDDevice *)pvHIDInstance;

    //
    // Pass the event on to the client.
    //
    psHIDDevice->pfnRxCallback(psHIDDevice->pvRxCBData, USB_EVENT_SUSPEND, 0,
                               (void *)0);
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the bus is taken
// out of suspend state.
//
//*****************************************************************************
static void
HandleResume(void *pvHIDInstance)
{
    const tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvHIDInstance != 0);

    //
    // Create the instance pointer.
    //
    psHIDDevice = (const tUSBDHIDDevice *)pvHIDInstance;

    //
    // Pass the event on to the client.
    //
    psHIDDevice->pfnRxCallback(psHIDDevice->pvRxCBData, USB_EVENT_RESUME, 0,
                               (void *)0);
}

//*****************************************************************************
//
// This function is called periodically and provides us with a time reference
// and method of implementing delayed or time-dependent operations.
//
// \param pvHIDInstance is the instance data for this request.
// \param ui32TimemS is the elapsed time in milliseconds since the last call
// to this function.
//
// \return None.
//
//*****************************************************************************
static void
HIDTickHandler(void *pvHIDInstance, uint32_t ui32TimemS)
{
    tHIDInstance *psInst;
    uint32_t ui32Size;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvHIDInstance != 0);

    //
    // Create the instance pointer.
    //
    psHIDDevice = (tUSBDHIDDevice *)pvHIDInstance;

    //
    // Get our instance data pointer.
    //
    psInst = &psHIDDevice->sPrivateData;

    //
    // If we are connected, process our idle timers.
    //
    if(psInst->bConnected)
    {
        ProcessIdleTimers(psHIDDevice, ui32TimemS);
    }

    //
    // Do we have a deferred receive waiting
    //
    if(psInst->ui16DeferredOpFlags & (1 << HID_DO_PACKET_RX))
    {
        //
        // Yes - how big is the waiting packet?
        //
        ui32Size = MAP_USBEndpointDataAvail(USB0_BASE, psInst->ui8OUTEndpoint);

        //
        // Tell the client that there is a packet waiting for it.
        //
        psHIDDevice->pfnRxCallback(psHIDDevice->pvRxCBData,
                                   USB_EVENT_RX_AVAILABLE, ui32Size,
                                   (void *)0);
    }

    return;
}

//*****************************************************************************
//
//! Initializes HID device operation for a given USB controller.
//!
//! \param ui32Index is the index of the USB controller which is to be
//! initialized for HID device operation.
//! \param psHIDDevice points to a structure containing parameters customizing
//! the operation of the HID device.
//!
//! An application wishing to offer a USB HID interface to a host system
//! must call this function to initialize the USB controller and attach the
//! device to the USB bus.  This function performs all required USB
//! initialization.
//!
//! On successful completion, this function will return the \e psHIDDevice
//! pointer passed to it.  This must be passed on all future calls from the
//! application to the HID device class driver.
//!
//! The USB HID device class API offers the application a report-based transmit
//! interface for Input reports.  Output reports may be received via the
//! control endpoint or via a dedicated Interrupt OUT endpoint.  If using the
//! dedicated endpoint, report data is delivered to the application packet-by-
//! packet.  If the application uses reports longer than \b USBDHID_MAX_PACKET
//! bytes and would rather receive full reports, it may use a USB buffer above
//! the receive channel to allow full reports to be read.
//!
//! Transmit Operation:
//!
//! Calls to USBDHIDReportWrite() pass complete reports to the driver for
//! transmission.  These will be transmitted to the host using as many USB
//! packets as are necessary to complete the transmission.
//!
//! Once a full Input report has been acknowledged by the USB host, a
//! \b USB_EVENT_TX_COMPLETE event is sent to the application transmit callback
//! to inform it that another report may be transmitted.
//!
//! Receive Operation (when using a dedicated interrupt OUT endpoint):
//!
//! An incoming USB data packet will result in a call to the application
//! callback with event \b USB_EVENT_RX_AVAILABLE.  The application must then
//! call USBDHIDPacketRead(), passing a buffer capable of holding the received
//! packet.  The size of the packet may be determined by calling function
//! USBDHIDRxPacketAvailable() prior to reading the packet.
//!
//! Receive Operation (when not using a dedicated OUT endpoint):
//!
//! If no dedicated OUT endpoint is used, Output and Feature reports are sent
//! from the host using the control endpoint, endpoint zero.  When such a
//! report is received, \b USBD_HID_EVENT_GET_REPORT_BUFFER is sent to the
//! application which must respond with a buffer large enough to hold the
//! report.  The device class driver will then copy the received report into
//! the supplied buffer before sending \b USBD_HID_EVENT_SET_REPORT to indicate
//! that the report is now available.
//!
//! \note The application must not make any calls to the low level USB device
//! interface if interacting with USB via the USB HID device class API.  Doing
//! so will cause unpredictable (though almost certainly unpleasant) behavior.
//!
//! \return Returns NULL on failure or the \e psHIDDevice pointer on success.
//
//*****************************************************************************
void *
USBDHIDInit(uint32_t ui32Index, tUSBDHIDDevice *psHIDDevice)
{
    tDeviceDescriptor *pi16DevDesc;

    //
    // Check parameter validity.
    //
    ASSERT(ui32Index == 0);
    ASSERT(psHIDDevice);
    ASSERT(psHIDDevice->ppui8StringDescriptors);
    ASSERT(psHIDDevice->pfnRxCallback);
    ASSERT(psHIDDevice->pfnTxCallback);
    ASSERT(psHIDDevice->ppui8ClassDescriptors);
    ASSERT(psHIDDevice->psHIDDescriptor);
    ASSERT((psHIDDevice->ui8NumInputReports == 0) || psHIDDevice->psReportIdle);

    USBDHIDCompositeInit(ui32Index, psHIDDevice, 0);

    //
    // Fix up the device descriptor with the client-supplied values.
    //
    pi16DevDesc = (tDeviceDescriptor *)psHIDDevice->sPrivateData.sDevInfo.pui8DeviceDescriptor;
    pi16DevDesc->idVendor = psHIDDevice->ui16VID;
    pi16DevDesc->idProduct = psHIDDevice->ui16PID;

    //
    // All is well so now pass the descriptors to the lower layer and put
    // the HID device on the bus.
    //
    USBDCDInit(ui32Index, &psHIDDevice->sPrivateData.sDevInfo,
               (void *)psHIDDevice);

    //
    // Return the pointer to the instance indicating that everything went well.
    //
    return((void *)psHIDDevice);
}

//*****************************************************************************
//
//! Initializes HID device operation for a given USB controller.
//!
//! \param ui32Index is the index of the USB controller which is to be
//! initialized for HID device operation.
//! \param psHIDDevice points to a structure containing parameters customizing
//! the operation of the HID device.
//! \param psCompEntry is the composite device entry to initialize when
//! creating a composite device.
//!
//! USB HID device classes call this function to initialize the lower level
//! HID interface in the USB controller.  If this HID device device is part of
//! a composite device, then the \e psCompEntry should point to the composite
//! device entry to initialize.  This is part of the array that is passed to
//! the USBDCompositeInit() function.
//!
//! \return Returns zero on failure or a non-zero instance value that should be
//! used with the remaining USB HID APIs.
//
//*****************************************************************************
void *
USBDHIDCompositeInit(uint32_t ui32Index, tUSBDHIDDevice *psHIDDevice,
                     tCompositeEntry *psCompEntry)
{
    tHIDInstance *psInst;
    tEndpointDescriptor *psEndpoint;

    //
    // Check parameter validity.
    //
    ASSERT(ui32Index == 0);
    ASSERT(psHIDDevice);
    ASSERT(psHIDDevice->ppsConfigDescriptor);
    ASSERT(psHIDDevice->ppui8StringDescriptors);
    ASSERT(psHIDDevice->pfnRxCallback);
    ASSERT(psHIDDevice->pfnTxCallback);
    ASSERT(psHIDDevice->ppui8ClassDescriptors);
    ASSERT(psHIDDevice->psHIDDescriptor);
    ASSERT((psHIDDevice->ui8NumInputReports == 0) || psHIDDevice->psReportIdle);

    //
    // Initialize the workspace in the passed instance structure.
    //
    psInst = &psHIDDevice->sPrivateData;

    //
    // Initialize the device information structure.
    //
    psInst->sDevInfo.psCallbacks = &g_sHIDHandlers;
    psInst->sDevInfo.pui8DeviceDescriptor = g_pui8HIDDeviceDescriptor;
    psInst->sDevInfo.ppsConfigDescriptors = psHIDDevice->ppsConfigDescriptor;
    psInst->sDevInfo.ppui8StringDescriptors =
                                        psHIDDevice->ppui8StringDescriptors;
    psInst->sDevInfo.ui32NumStringDescriptors =
                                        psHIDDevice->ui32NumStringDescriptors;

    //
    // Default the endpoints zero before looking for them in the configuration
    // descriptor.
    //
    psInst->ui8Interface = 0;
    psInst->ui8INEndpoint = 0;
    psInst->ui8OUTEndpoint = 0;

    //
    // Get the first endpoint descriptor on interface 0.
    //
    psEndpoint =
        USBDCDConfigGetInterfaceEndpoint(psHIDDevice->ppsConfigDescriptor[0],
                                         psInst->ui8Interface, 0, 0);

    if(psEndpoint)
    {
        if(psEndpoint->bEndpointAddress & 0x80)
        {
            psInst->ui8INEndpoint = IndexToUSBEP(psEndpoint->bEndpointAddress);
        }
        else
        {
            psInst->ui8OUTEndpoint = IndexToUSBEP(psEndpoint->bEndpointAddress);
        }
    }

    //
    // Get the second endpoint descriptor on interface 0.
    //
    psEndpoint =
        USBDCDConfigGetInterfaceEndpoint(psHIDDevice->ppsConfigDescriptor[0],
                                         psInst->ui8Interface, 0, 1);
    if(psEndpoint)
    {
        if(psEndpoint->bEndpointAddress & 0x80)
        {
            psInst->ui8INEndpoint = IndexToUSBEP(psEndpoint->bEndpointAddress);
        }
        else
        {
            psInst->ui8OUTEndpoint = IndexToUSBEP(psEndpoint->bEndpointAddress);
        }
    }

    //
    // Must have at least an IN endpoint.
    //
    if(psInst->ui8INEndpoint == 0)
    {
        return((void *)0);
    }

    //
    // Initialize the composite entry that is used by the composite device
    // class.
    //
    if(psCompEntry != 0)
    {
        psCompEntry->psDevInfo = &psInst->sDevInfo;
        psCompEntry->pvInstance = (void *)psHIDDevice;
    }

    psInst->ui32USBBase = USB0_BASE;
    psInst->iHIDRxState = eHIDStateUnconfigured;
    psInst->iHIDTxState = eHIDStateUnconfigured;
    psInst->ui16DeferredOpFlags = 0;
    psInst->bConnected = false;
    psInst->bGetRequestPending = false;
    psInst->bSendInProgress = false;
    psInst->ui16InReportIndex = 0;
    psInst->ui16InReportSize = 0;
    psInst->pui8InReportData = (uint8_t *)0;
    psInst->ui16OutReportSize = 0;
    psInst->pui8OutReportData = (uint8_t *)0;

    //
    // Initialize the device info structure for the HID device.
    //
    USBDCDDeviceInfoInit(0, &psInst->sDevInfo);

    //
    // Initialize the input report idle timers if any input reports exist.
    //
    ClearIdleTimers(psHIDDevice);

    //
    // Initialize the USB tick module, this will prevent it from being
    // initialized later in the call to USBDCDInit();
    //
    InternalUSBTickInit();

    //
    // Register our tick handler (this must be done after USBDCDInit).
    //
    InternalUSBRegisterTickHandler(HIDTickHandler, (void *)psHIDDevice);

    //
    // Return the pointer to the instance indicating that everything went well.
    //
    return((void *)psHIDDevice);
}

//*****************************************************************************
//
//! Shuts down the HID device.
//!
//! \param pvHIDInstance is the pointer to the device instance structure as
//! returned by USBDHIDInit().
//!
//! This function terminates HID operation for the instance supplied and
//! removes the device from the USB bus.  This function should not be called
//! if the HID device is part of a composite device and instead the
//! USBDCompositeTerm() function should be called for the full composite
//! device.
//!
//! Following this call, the \e pvHIDInstance instance should not me used in
//! any other calls.
//!
//! \return None.
//
//*****************************************************************************
void
USBDHIDTerm(void *pvHIDInstance)
{
    tHIDInstance *psInst;

    ASSERT(pvHIDInstance);

    //
    // Get a pointer to our instance data.
    //
    psInst = &((tUSBDHIDDevice *)pvHIDInstance)->sPrivateData;

    //
    // Terminate the requested instance.
    //
    USBDCDTerm(USBBaseToIndex(psInst->ui32USBBase));

    psInst->ui32USBBase = 0;
}

//*****************************************************************************
//
//! Sets the client-specific pointer parameter for the receive channel
//! callback.
//!
//! \param pvHIDInstance is the pointer to the device instance structure as
//! returned by USBDHIDInit().
//! \param pvCBData is the pointer that client wishes to be provided on each
//! event sent to the receive channel callback function.
//!
//! The client uses this function to change the callback pointer passed in
//! the first parameter on all callbacks to the \e pfnRxCallback function
//! passed on USBDHIDInit().
//!
//! If a client wants to make runtime changes in the callback pointer, it must
//! ensure that the pvHIDInstance structure passed to USBDHIDInit() resides in
//! RAM.  If this structure is in flash, callback data changes will not be
//! possible.
//!
//! \return Returns the previous callback pointer that was being used for
//! this instance's receive callback.
//
//*****************************************************************************
void *
USBDHIDSetRxCBData(void *pvHIDInstance, void *pvCBData)
{
    void *pvOldValue;

    ASSERT(pvHIDInstance);

    //
    // Set the callback data for the receive channel after remembering the
    // previous value.
    //
    pvOldValue = ((tUSBDHIDDevice *)pvHIDInstance)->pvRxCBData;
    ((tUSBDHIDDevice *)pvHIDInstance)->pvRxCBData = pvCBData;

    //
    // Return the previous callback data value.
    //
    return(pvOldValue);
}

//*****************************************************************************
//
//! Sets the client-specific data pointer for the transmit callback.
//!
//! \param pvHIDInstance is the pointer to the device instance structure as
//! returned by USBDHIDInit().
//! \param pvCBData is the pointer that client wishes to be provided on each
//! event sent to the transmit channel callback function.
//!
//! The client uses this function to change the callback data pointer passed in
//! the first parameter on all callbacks to the \e pfnTxCallback function
//! passed on USBDHIDInit().
//!
//! If a client wants to make runtime changes in the callback data, it must
//! ensure that the pvHIDInstance structure passed to USBDHIDInit() resides in
//! RAM.  If this structure is in flash, callback data changes will not be
//! possible.
//!
//! \return Returns the previous callback data pointer that was being used for
//! this instance's transmit callback.
//
//*****************************************************************************
void *
USBDHIDSetTxCBData(void *pvHIDInstance, void *pvCBData)
{
    void *pvOldValue;

    ASSERT(pvHIDInstance);

    //
    // Set the callback data for the transmit channel after remembering the
    // previous value.
    //
    pvOldValue = ((tUSBDHIDDevice *)pvHIDInstance)->pvTxCBData;
    ((tUSBDHIDDevice *)pvHIDInstance)->pvTxCBData = pvCBData;

    //
    // Return the previous callback data value.
    //
    return(pvOldValue);
}

//*****************************************************************************
//
//! Transmits a HID device report to the USB host via the HID interrupt IN
//! endpoint.
//!
//! \param pvHIDInstance is the pointer to the device instance structure as
//! returned by USBDHIDInit().
//! \param pi8Data points to the first byte of data which is to be transmitted.
//! \param ui32Length is the number of bytes of data to transmit.
//! \param bLast is ignored in this implementation.  This parameter is required
//! to ensure compatibility with other device class drivers and USB buffers.
//!
//! This function schedules the supplied data for transmission to the USB
//! host in a single USB transaction using as many packets as it takes to send
//! all the data in the report.  If no transmission is currently ongoing,
//! the first packet of data is immediately copied to the relevant USB endpoint
//! FIFO for transmission.  Whenever all the report data has been acknowledged
//! by the host, a \b USB_EVENT_TX_COMPLETE event will be sent to the
//! application transmit callback indicating that another report can now be
//! transmitted.
//!
//! The caller must ensure that the data pointed to by \e pui8Data remains
//! accessible and unaltered until the \b USB_EVENT_TX_COMPLETE is received.
//!
//! \return Returns the number of bytes actually scheduled for transmission.
//! At this level, this will either be the number of bytes passed or 0 to
//! indicate a failure.
//
//*****************************************************************************
uint32_t
USBDHIDReportWrite(void *pvHIDInstance, uint8_t *pi8Data, uint32_t ui32Length,
                   bool bLast)
{
    tHIDInstance *psInst;
    int32_t i32Retcode;

    ASSERT(pvHIDInstance);

    //
    // Get our instance data pointer
    //
    psInst = &((tUSBDHIDDevice *)pvHIDInstance)->sPrivateData;

    //
    // Set a flag indicating that we are currently in the process of sending
    // a packet.
    //
    psInst->bSendInProgress = true;

    //
    // Can we send the data provided?
    //
    if(psInst->iHIDTxState != eHIDStateIdle)
    {
        //
        // We are in the middle of sending another report.  Return 0 to
        // indicate that we can't send this report until the previous one
        // finishes.
        //
        psInst->bSendInProgress = false;
        return(0);
    }

    //
    // Clear the elapsed time since this report was last sent.
    //
    if(ui32Length)
    {
        ClearReportTimer(pvHIDInstance, *pi8Data);
    }

    //
    // Keep track of the whereabouts of the report so that we can send it in
    // multiple packets if necessary.
    //
    psInst->pui8InReportData = pi8Data;
    psInst->ui16InReportIndex = 0;
    psInst->ui16InReportSize = ui32Length;

    //
    // Schedule transmission of the first packet of the report.
    //
    psInst->iHIDTxState = eHIDStateWaitData;
    i32Retcode = ScheduleReportTransmission(psInst);

    //
    // Clear the flag we use to indicate that we are in the midst of sending
    // a packet.
    //
    psInst->bSendInProgress = false;

    //
    // Did an error occur while trying to send the data?
    //
    if(i32Retcode != -1)
    {
        //
        // No - tell the caller we sent all the bytes provided.
        //
        return(ui32Length);
    }
    else
    {
        //
        // Yes - tell the caller we could not send the data.
        //
        return(0);
    }
}

//*****************************************************************************
//
//! Reads a packet of data received from the USB host via the interrupt OUT
//! endpoint (if in use).
//!
//! \param pvHIDInstance is the pointer to the device instance structure as
//! returned by USBDHIDInit().
//! \param pi8Data points to a buffer into which the received data will be
//! written.
//! \param ui32Length is the size of the buffer pointed to by pi8Data.
//! \param bLast indicates whether the client will make a further call to
//! read additional data from the packet.
//!
//! This function reads up to \e ui32Length bytes of data received from the USB
//! host into the supplied application buffer.  If the driver detects that the
//! entire packet has been read, it is acknowledged to the host.
//!
//! The \e bLast parameter is ignored in this implementation since the end of
//! a packet can be determined without relying upon the client to provide
//! this information.
//!
//! \return Returns the number of bytes of data read.
//
//*****************************************************************************
uint32_t
USBDHIDPacketRead(void *pvHIDInstance, uint8_t *pi8Data, uint32_t ui32Length,
                  bool bLast)
{
    uint32_t ui32EPStatus, ui32Count, ui32Pkt;
    tHIDInstance *psInst;
    int32_t i32Retcode;

    ASSERT(pvHIDInstance);

    //
    // Get our instance data pointer
    //
    psInst = &((tUSBDHIDDevice *)pvHIDInstance)->sPrivateData;

    //
    // Does the relevant endpoint FIFO have a packet waiting for us?
    //
    ui32EPStatus = MAP_USBEndpointStatus(psInst->ui32USBBase,
                                         psInst->ui8OUTEndpoint);
    if(ui32EPStatus & USB_DEV_RX_PKT_RDY)
    {
        //
        // How many bytes are available for us to receive?
        //
        ui32Pkt = MAP_USBEndpointDataAvail(psInst->ui32USBBase,
                                           psInst->ui8OUTEndpoint);

        //
        // Get as much data as we can.
        //
        ui32Count = ui32Length;
        i32Retcode = MAP_USBEndpointDataGet(psInst->ui32USBBase,
                                            psInst->ui8OUTEndpoint,
                                            pi8Data, &ui32Count);

        //
        // Did we read the last of the packet data?
        //
        if(ui32Count == ui32Pkt)
        {
            //
            // Clear the endpoint status so that we know no packet is
            // waiting.
            //
            MAP_USBDevEndpointStatusClear(psInst->ui32USBBase,
                                          psInst->ui8OUTEndpoint,
                                          ui32EPStatus);

            //
            // Acknowledge the data, thus freeing the host to send the
            // next packet.
            //
            MAP_USBDevEndpointDataAck(psInst->ui32USBBase,
                                      psInst->ui8OUTEndpoint, true);

            //
            // Clear the flag we set to indicate that a packet read is
            // pending.
            //
            SetDeferredOpFlag(&psInst->ui16DeferredOpFlags,
                              HID_DO_PACKET_RX, false);
        }

        //
        // If all went well, tell the caller how many bytes they got.
        //
        if(i32Retcode != -1)
        {
            return(ui32Count);
        }
    }

    //
    // No packet was available or an error occurred while reading so tell
    // the caller no bytes were returned.
    //
    return(0);
}

//*****************************************************************************
//
//! Returns the number of free bytes in the transmit buffer.
//!
//! \param pvHIDInstance is the pointer to the device instance structure as
//! returned by USBDHIDInit().
//!
//! This function indicates to the caller whether or not it is safe to send a
//! new report using a call to USBDHIDReportWrite().  The value returned will
//! be the maximum USB packet size (\b USBDHID_MAX_PACKET) if no transmission
//! is currently outstanding or 0 if a transmission is in progress.  Since the
//! function USBDHIDReportWrite() can accept full reports longer than a single
//! USB packet, the caller should be aware that the returned value from this
//! class driver, unlike others, does not indicate the maximum size of report
//! that can be written but is merely an indication that another report can be
//! written.
//!
//! \return Returns 0 if an outgoing report is still being transmitted or
//! \b USBDHID_MAX_PACKET if no transmission is currently in progress.
//
//*****************************************************************************
uint32_t
USBDHIDTxPacketAvailable(void *pvHIDInstance)
{
    tHIDInstance *psInst;

    ASSERT(pvHIDInstance);

    //
    // Get our instance data pointer.
    //
    psInst = &((tUSBDHIDDevice *)pvHIDInstance)->sPrivateData;

    //
    // Do we have a packet transmission currently ongoing?
    //
    if(psInst->iHIDTxState != eHIDStateIdle)
    {
        //
        // We are not ready to receive a new packet so return 0.
        //
        return(0);
    }
    else
    {
        //
        // We can receive a packet so return the max packet size for the
        // relevant endpoint.
        //
        return(USBDHID_MAX_PACKET);
    }
}

//*****************************************************************************
//
//! Determines whether a packet is available and, if so, the size of the
//! buffer required to read it.
//!
//! \param pvHIDInstance is the pointer to the device instance structure as
//! returned by USBDHIDInit().
//!
//! This function may be used to determine if a received packet remains to be
//! read and allows the application to determine the buffer size needed to
//! read the data.
//!
//! \return Returns 0 if no received packet remains unprocessed or the
//! size of the packet if a packet is waiting to be read.
//
//*****************************************************************************
uint32_t
USBDHIDRxPacketAvailable(void *pvHIDInstance)
{
    uint32_t ui32EPStatus, ui32Size;
    tHIDInstance *psInst;

    ASSERT(pvHIDInstance);

    //
    // Get our instance data pointer
    //
    psInst = &((tUSBDHIDDevice *)pvHIDInstance)->sPrivateData;

    //
    // Does the relevant endpoint FIFO have a packet waiting for us?
    //
    ui32EPStatus = MAP_USBEndpointStatus(psInst->ui32USBBase,
                                         psInst->ui8OUTEndpoint);
    if(ui32EPStatus & USB_DEV_RX_PKT_RDY)
    {
        //
        // Yes - a packet is waiting.  How big is it?
        //
        ui32Size = MAP_USBEndpointDataAvail(psInst->ui32USBBase,
                                            psInst->ui8OUTEndpoint);

        return(ui32Size);
    }
    else
    {
        //
        // There is no packet waiting to be received.
        //
        return(0);
    }
}

//*****************************************************************************
//
//! Reports the device power status (bus- or self-powered) to the USB library.
//!
//! \param pvHIDInstance is the pointer to the HID device instance structure.
//! \param ui8Power indicates the current power status, either
//! \b USB_STATUS_SELF_PWR or \b USB_STATUS_BUS_PWR.
//!
//! Applications which support switching between bus- or self-powered
//! operation should call this function whenever the power source changes
//! to indicate the current power status to the USB library.  This information
//! is required by the USB library to allow correct responses to be provided
//! when the host requests status from the device.
//!
//! \return None.
//
//*****************************************************************************
void
USBDHIDPowerStatusSet(void *pvHIDInstance, uint8_t ui8Power)
{
    ASSERT(pvHIDInstance);

    //
    // Pass the request through to the lower layer.
    //
    USBDCDPowerStatusSet(0, ui8Power);
}

//*****************************************************************************
//
//! Requests a remote wake up to resume communication when in suspended state.
//!
//! \param pvHIDInstance is the pointer to the HID device instance structure.
//!
//! When the bus is suspended, an application which supports remote wake up
//! (advertised to the host via the configuration descriptor) may call this
//! function to initiate remote wake up signaling to the host.  If the remote
//! wake up feature has not been disabled by the host, this will cause the bus
//! to resume operation within 20mS.  If the host has disabled remote wake up,
//! \b false will be returned to indicate that the wake up request was not
//! successful.
//!
//! \return Returns \b true if the remote wake up is not disabled and the
//! signaling was started or \b false if remote wake up is disabled or if
//! signaling is currently ongoing following a previous call to this function.
//
//*****************************************************************************
bool
USBDHIDRemoteWakeupRequest(void *pvHIDInstance)
{
    ASSERT(pvHIDInstance);

    //
    // Pass the request through to the lower layer.
    //
    return(USBDCDRemoteWakeupRequest(0));
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
