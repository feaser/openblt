//*****************************************************************************
//
// usbdconfig.c - High level USB device configuration function.
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
#include "usblib/usblib.h"
#include "usblib/usblibpriv.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdevicepriv.h"

//*****************************************************************************
//
//! \addtogroup device_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Structure used in compiling FIFO size and endpoint properties from a
// configuration descriptor.
//
//*****************************************************************************
typedef struct
{
    uint32_t pui32Size[2];
}
tUSBEndpointInfo;

//*****************************************************************************
//
// Indices used when accessing the tUSBEndpointInfo structure.
//
//*****************************************************************************
#define EP_INFO_IN              0
#define EP_INFO_OUT             1

//*****************************************************************************
//
// Given a maximum packet size and the user's FIFO scaling requirements,
// determine the flags to use to configure the endpoint FIFO and the number
// of bytes of FIFO space occupied.
//
//*****************************************************************************
static uint32_t
GetEndpointFIFOSize(uint32_t ui32MaxPktSize, uint32_t *pupBytesUsed)
{
    uint32_t ui32Loop, ui32FIFOSize;

    //
    // Now we need to find the nearest supported size that accommodates the
    // requested size.  Step through each of the supported sizes until we
    // find one that will do.
    //
    for(ui32Loop = USB_FIFO_SZ_8; ui32Loop <= USB_FIFO_SZ_2048; ui32Loop++)
    {
        //
        // How many bytes does this FIFO value represent?
        //
        ui32FIFOSize = USBFIFOSizeToBytes(ui32Loop);

        //
        // Is this large enough to hold one packet.
        //
        if(ui32FIFOSize >= ui32MaxPktSize)
        {
            //
            // Return the FIFO size setting and the USB_FIFO_SZ_ value.
            //
            *pupBytesUsed = ui32FIFOSize;

            return(ui32Loop);
        }
    }

    //
    // If we drop out, we can't support the FIFO size requested.  Signal a
    // problem by returning 0 in the pBytesUsed
    //
    *pupBytesUsed = 0;

    return(USB_FIFO_SZ_8);
}

//*****************************************************************************
//
// Translate a USB endpoint descriptor into the values we need to pass to the
// USBDevEndpointConfigSet() API.
//
//*****************************************************************************
static void
GetEPDescriptorType(tEndpointDescriptor *psEndpoint, uint32_t *pui32EPIndex,
                    uint32_t *pui32MaxPktSize, uint32_t *pui32Flags)
{
    //
    // Get the endpoint index.
    //
    *pui32EPIndex = psEndpoint->bEndpointAddress & USB_EP_DESC_NUM_M;

    //
    // Extract the maximum packet size.
    //
    *pui32MaxPktSize = psEndpoint->wMaxPacketSize & USB_EP_MAX_PACKET_COUNT_M;

    //
    // Is this an IN or an OUT endpoint?
    //
    *pui32Flags = (psEndpoint->bEndpointAddress & USB_EP_DESC_IN) ?
                  USB_EP_DEV_IN : USB_EP_DEV_OUT;

    //
    // Set the endpoint mode.
    //
    switch(psEndpoint->bmAttributes & USB_EP_ATTR_TYPE_M)
    {
        case USB_EP_ATTR_CONTROL:
        {
            *pui32Flags |= USB_EP_MODE_CTRL;
            break;
        }
        case USB_EP_ATTR_BULK:
        {
            *pui32Flags |= USB_EP_MODE_BULK;
            break;
        }
        case USB_EP_ATTR_INT:
        {
            *pui32Flags |= USB_EP_MODE_INT;
            break;
        }
        case USB_EP_ATTR_ISOC:
        {
            *pui32Flags |= USB_EP_MODE_ISOC;
            break;
        }
    }
}

//*****************************************************************************
//
//! Configure the USB controller appropriately for the device whose
//! configuration descriptor is passed.
//!
//! \param psDevInst is a pointer to the device instance being configured.
//! \param psConfig is a pointer to the configuration descriptor that the
//! USB controller is to be set up to support.
//!
//! This function may be used to initialize a USB controller to operate as
//! the device whose configuration descriptor is passed.  The function
//! enables the USB controller, partitions the FIFO appropriately and
//! configures each endpoint required by the configuration.  If the supplied
//! configuration supports multiple alternate settings for any interface,
//! the USB FIFO is set up assuming the worst case use (largest packet size
//! for a given endpoint in any alternate setting using that endpoint) to
//! allow for on-the-fly alternate setting changes later.  On return from this
//! function, the USB controller is configured for correct operation of
//! the default configuration of the device described by the descriptor passed.
//!
//! USBDCDConfig() is an optional call and applications may chose to make
//! direct calls to SysCtlPeripheralEnable(), SysCtlUSBPLLEnable(),
//! USBDevEndpointConfigSet() and USBFIFOConfigSet() instead of using this
//! function.  If this function is used, it must be called prior to
//! USBDCDInit() since this call assumes that the low level hardware
//! configuration has been completed before it is made.
//!
//! \return Returns \b true on success or \b false on failure.
//
//*****************************************************************************
bool
USBDeviceConfig(tDCDInstance *psDevInst, const tConfigHeader *psConfig)
{
    uint32_t ui32Loop, ui32Count, ui32NumInterfaces, ui32EpIndex, ui32EpType,
             ui32MaxPkt, ui32NumEndpoints, ui32Flags, ui32BytesUsed,
             ui32Section;
    tInterfaceDescriptor *psInterface;
    tEndpointDescriptor *psEndpoint;
    tUSBEndpointInfo psEPInfo[NUM_USB_EP - 1];

    //
    // A valid device instance is required.
    //
    ASSERT(psDevInst != 0);

    //
    // Catch bad pointers in a debug build.
    //
    ASSERT(psConfig);

    //
    // Clear out our endpoint info.
    //
    for(ui32Loop = 0; ui32Loop < (NUM_USB_EP - 1); ui32Loop++)
    {
        psEPInfo[ui32Loop].pui32Size[EP_INFO_IN] = 0;
        psEPInfo[ui32Loop].pui32Size[EP_INFO_OUT] = 0;
    }

    //
    // How many (total) endpoints does this configuration describe?
    //
    ui32NumEndpoints = USBDCDConfigDescGetNum(psConfig,
                                              USB_DTYPE_ENDPOINT);

    //
    // How many interfaces are included?
    //
    ui32NumInterfaces = USBDCDConfigDescGetNum(psConfig,
                                               USB_DTYPE_INTERFACE);

    //
    // Look at each endpoint and determine the largest max packet size for
    // each endpoint.  This will determine how we partition the USB FIFO.
    //
    for(ui32Loop = 0; ui32Loop < ui32NumEndpoints; ui32Loop++)
    {
        //
        // Get a pointer to the endpoint descriptor.
        //
        psEndpoint = (tEndpointDescriptor *)USBDCDConfigDescGet(
                                psConfig, USB_DTYPE_ENDPOINT, ui32Loop,
                                &ui32Section);

        //
        // Extract the endpoint number and whether it is an IN or OUT
        // endpoint.
        //
        ui32EpIndex = (uint32_t)
                        psEndpoint->bEndpointAddress & USB_EP_DESC_NUM_M;
        ui32EpType = (psEndpoint->bEndpointAddress & USB_EP_DESC_IN) ?
                     EP_INFO_IN : EP_INFO_OUT;

        //
        // Make sure the endpoint number is valid for our controller.  If not,
        // return false to indicate an error.  Note that 0 is invalid since
        // you shouldn't reference endpoint 0 in the config descriptor.
        //
        if((ui32EpIndex >= NUM_USB_EP) || (ui32EpIndex == 0))
        {
            return(false);
        }

        //
        // Does this endpoint have a max packet size requirement larger than
        // any previous use we have seen?
        //
        if(psEndpoint->wMaxPacketSize >
           psEPInfo[ui32EpIndex - 1].pui32Size[ui32EpType])
        {
            //
            // Yes - remember the new maximum packet size.
            //
            psEPInfo[ui32EpIndex - 1].pui32Size[ui32EpType] =
                psEndpoint->wMaxPacketSize;
        }
    }

    //
    // At this point, we have determined the maximum packet size required
    // for each endpoint by any possible alternate setting of any interface
    // in this configuration.  Now determine the endpoint settings required
    // for the interface setting we are actually going to use.
    //
    for(ui32Loop = 0; ui32Loop < ui32NumInterfaces; ui32Loop++)
    {
        //
        // Get the next interface descriptor in the configuration descriptor.
        //
        psInterface = USBDCDConfigGetInterface(psConfig, ui32Loop,
                                               USB_DESC_ANY, &ui32Section);

        //
        // Is this the default interface (bAlternateSetting set to 0)?
        //
        if(psInterface && (psInterface->bAlternateSetting == 0))
        {
            //
            // This is an interface we are interested in so gather the
            // information on its endpoints.
            //
            ui32NumEndpoints = (uint32_t)psInterface->bNumEndpoints;

            //
            // Walk through each endpoint in this interface and configure
            // it appropriately.
            //
            for(ui32Count = 0; ui32Count < ui32NumEndpoints; ui32Count++)
            {
                //
                // Get a pointer to the endpoint descriptor.
                //
                psEndpoint = USBDCDConfigGetInterfaceEndpoint(psConfig,
                                            psInterface->bInterfaceNumber,
                                            psInterface->bAlternateSetting,
                                            ui32Count);

                //
                // Make sure we got a good pointer.
                //
                if(psEndpoint)
                {
                    //
                    // Determine maximum packet size and flags from the
                    // endpoint descriptor.
                    //
                    GetEPDescriptorType(psEndpoint, &ui32EpIndex, &ui32MaxPkt,
                                        &ui32Flags);

                    //
                    // Make sure no-one is trying to configure endpoint 0.
                    //
                    if(!ui32EpIndex)
                    {
                        return(false);
                    }

                    //
                    // Set the endpoint configuration.
                    //
                    USBDevEndpointConfigSet(USB0_BASE,
                                            IndexToUSBEP(ui32EpIndex),
                                            ui32MaxPkt, ui32Flags);
                }
            }
        }
    }

    //
    // At this point, we have configured all the endpoints that are to be
    // used by this configuration's alternate setting 0.  Now we go on and
    // partition the FIFO based on the maximum packet size information we
    // extracted earlier.  Endpoint 0 is automatically configured to use the
    // first MAX_PACKET_SIZE_EP0 bytes of the FIFO so we start from there.
    //
    ui32Count = MAX_PACKET_SIZE_EP0;
    for(ui32Loop = 1; ui32Loop < NUM_USB_EP; ui32Loop++)
    {
        //
        // Configure the IN endpoint at this index if it is referred to
        // anywhere.
        //
        if(psEPInfo[ui32Loop - 1].pui32Size[EP_INFO_IN])
        {
            //
            // What FIFO size flag do we use for this endpoint?
            //
            ui32MaxPkt = GetEndpointFIFOSize(
                                psEPInfo[ui32Loop - 1].pui32Size[EP_INFO_IN],
                                &ui32BytesUsed);

            //
            // The FIFO space could not be allocated.
            //
            if(ui32BytesUsed == 0)
            {
                return(false);
            }

            //
            // Now actually configure the FIFO for this endpoint.
            //
            USBFIFOConfigSet(USB0_BASE, IndexToUSBEP(ui32Loop), ui32Count,
                             ui32MaxPkt, USB_EP_DEV_IN);
            ui32Count += ui32BytesUsed;
        }

        //
        // Configure the OUT endpoint at this index.
        //
        if(psEPInfo[ui32Loop - 1].pui32Size[EP_INFO_OUT])
        {
            //
            // What FIFO size flag do we use for this endpoint?
            //
            ui32MaxPkt = GetEndpointFIFOSize(
                                psEPInfo[ui32Loop - 1].pui32Size[EP_INFO_OUT],
                                &ui32BytesUsed);

            //
            // The FIFO space could not be allocated.
            //
            if(ui32BytesUsed == 0)
            {
                return(false);
            }

            //
            // Now actually configure the FIFO for this endpoint.
            //
            USBFIFOConfigSet(USB0_BASE, IndexToUSBEP(ui32Loop), ui32Count,
                             ui32MaxPkt, USB_EP_DEV_OUT);
            ui32Count += ui32BytesUsed;
        }

    }

    //
    // If we get to the end, all is well.
    //
    return(true);
}

//*****************************************************************************
//
//! Configure the affected USB endpoints appropriately for one alternate
//! interface setting.
//!
//! \param psDevInst is a pointer to the device instance being configured.
//! \param psConfig is a pointer to the configuration descriptor that contains
//! the interface whose alternate settings is to be configured.
//! \param ui8InterfaceNum is the number of the interface whose alternate
//! setting is to be configured.  This number corresponds to the
//! bInterfaceNumber field in the desired interface descriptor.
//! \param ui8AlternateSetting is the alternate setting number for the desired
//! interface.  This number corresponds to the bAlternateSetting field in the
//! desired interface descriptor.
//!
//! This function may be used to reconfigure the endpoints of an interface
//! for operation in one of the interface's alternate settings.  Note that this
//! function assumes that the endpoint FIFO settings will not need to change
//! and only the endpoint mode is changed.  This assumption is valid if the
//! USB controller was initialized using a previous call to USBDCDConfig().
//!
//! In reconfiguring the interface endpoints, any additional configuration
//! bits set in the endpoint configuration other than the direction (\b
//! USB_EP_DEV_IN or \b USB_EP_DEV_OUT) and mode (\b USB_EP_MODE_MASK) are
//! preserved.
//!
//! \return Returns \b true on success or \b false on failure.
//
//*****************************************************************************
bool
USBDeviceConfigAlternate(tDCDInstance *psDevInst,
                         const tConfigHeader *psConfig,
                         uint8_t ui8InterfaceNum,
                         uint8_t ui8AlternateSetting)
{
    uint32_t ui32NumInterfaces, ui32NumEndpoints, ui32Loop, ui32Count,
             ui32MaxPkt, ui32Flags, ui32Section, ui32EpIndex;
    tInterfaceDescriptor *psInterface;
    tEndpointDescriptor *psEndpoint;

    //
    // How many interfaces are included in the descriptor?
    //
    ui32NumInterfaces = USBDCDConfigDescGetNum(psConfig,
                                               USB_DTYPE_INTERFACE);

    //
    // Find the interface descriptor for the supplied interface and alternate
    // setting numbers.
    //

    for(ui32Loop = 0; ui32Loop < ui32NumInterfaces; ui32Loop++)
    {
        //
        // Get the next interface descriptor in the configuration descriptor.
        //
        psInterface = USBDCDConfigGetInterface(psConfig, ui32Loop,
                                               USB_DESC_ANY, &ui32Section);

        //
        // Is this the default interface (bAlternateSetting set to 0)?
        //
        if(psInterface &&
           (psInterface->bInterfaceNumber == ui8InterfaceNum) &&
           (psInterface->bAlternateSetting == ui8AlternateSetting))
        {
            //
            // This is an interface we are interested in and the descriptor
            // representing the alternate setting we want so go ahead and
            // reconfigure the endpoints.
            //

            //
            // How many endpoints does this interface have?
            //
            ui32NumEndpoints = (uint32_t)psInterface->bNumEndpoints;

            //
            // Walk through each endpoint in turn.
            //
            for(ui32Count = 0; ui32Count < ui32NumEndpoints; ui32Count++)
            {
                //
                // Get a pointer to the endpoint descriptor.
                //
                psEndpoint = USBDCDConfigGetInterfaceEndpoint(psConfig,
                                              psInterface->bInterfaceNumber,
                                              psInterface->bAlternateSetting,
                                              ui32Count);

                //
                // Make sure we got a good pointer.
                //
                if(psEndpoint)
                {
                    //
                    // Determine maximum packet size and flags from the
                    // endpoint descriptor.
                    //
                    GetEPDescriptorType(psEndpoint, &ui32EpIndex, &ui32MaxPkt,
                                        &ui32Flags);

                    //
                    // Make sure no-one is trying to configure endpoint 0.
                    //
                    if(!ui32EpIndex)
                    {
                        return(false);
                    }

                    //
                    // Set the endpoint configuration.
                    //
                    USBDevEndpointConfigSet(USB0_BASE,
                                            IndexToUSBEP(ui32EpIndex),
                                            ui32MaxPkt, ui32Flags);
                }
            }

            //
            // At this point, we have reconfigured the desired interface so
            // return indicating all is well.
            //
            return(true);
        }
    }

    return(false);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
