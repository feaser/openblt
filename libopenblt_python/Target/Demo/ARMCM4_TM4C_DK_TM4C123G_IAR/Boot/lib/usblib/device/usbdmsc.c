//*****************************************************************************
//
// usbdmsc.c - USB mass storage device class driver.
//
// Copyright (c) 2009-2013 Texas Instruments Incorporated.  All rights reserved.
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
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/usbdrv.h"
#include "usblib/usblib.h"
#include "usblib/usblibpriv.h"
#include "usblib/usbmsc.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdmsc.h"

//*****************************************************************************
//
//! \addtogroup msc_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// These are the internal flags used with the ui32Flags member variable.
//
//*****************************************************************************
#define USBD_FLAG_DMA_IN        0x00000001
#define USBD_FLAG_DMA_OUT       0x00000002

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
// These are fields that are used by the USB descriptors for the Mass Storage
// Class.
//
//*****************************************************************************
#define USB_MSC_SUBCLASS_SCSI   0x6
#define USB_MSC_PROTO_BULKONLY  0x50

//*****************************************************************************
//
// Endpoints to use for each of the required endpoints in the driver.
//
//*****************************************************************************
#define DATA_IN_ENDPOINT        USB_EP_1
#define DATA_OUT_ENDPOINT       USB_EP_1

//*****************************************************************************
//
// Maximum packet size for the bulk endpoints is 64 bytes.
//
//*****************************************************************************
#define DATA_IN_EP_MAX_SIZE     64
#define DATA_OUT_EP_MAX_SIZE    64

//*****************************************************************************
//
// These defines control the sizes of USB transfers for data and commands.
//
//*****************************************************************************
#define MAX_TRANSFER_SIZE       512
#define COMMAND_BUFFER_SIZE     64

//*****************************************************************************
//
// The local buffer used to read in commands and process them.
//
//*****************************************************************************
static uint8_t g_pui8Command[COMMAND_BUFFER_SIZE];

//*****************************************************************************
//
// The current transfer state is held in these variables.
//
//*****************************************************************************
static tMSCCSW g_sSCSICSW;

//*****************************************************************************
//
// The current state for the SCSI commands that are being handled and are
// stored in the tMSCInstance.ui8SCSIState structure member.
//
//*****************************************************************************

//
// No command in process.
//
#define STATE_SCSI_IDLE             0x00

//
// Sending and reading logical blocks.
//
#define STATE_SCSI_SEND_BLOCKS      0x01

//
// Receiving and writing logical blocks.
//
#define STATE_SCSI_RECEIVE_BLOCKS   0x02

//
// Send the status once the previous transfer is complete.
//
#define STATE_SCSI_SEND_STATUS      0x03

//
// Status was prepared to be sent and now waiting for it to have gone out.
//
#define STATE_SCSI_SENT_STATUS      0x04

//*****************************************************************************
//
// Device Descriptor.  This is stored in RAM to allow several fields to be
// changed at runtime based on the client's requirements.
//
//*****************************************************************************
static uint8_t g_pui8MSCDeviceDescriptor[] =
{
    18,                             // Size of this structure.
    USB_DTYPE_DEVICE,               // Type of this structure.
    USBShort(0x110),                // USB version 1.1 (if we say 2.0, hosts
                                    // assume
                                    // high-speed - see USB 2.0 spec 9.2.6.6)
    0,                              // USB Device Class (spec 5.1.1)
    0,                              // USB Device Sub-class (spec 5.1.1)
    0,                              // USB Device protocol (spec 5.1.1)
    64,                             // Maximum packet size for default pipe.
    USBShort(0),                    // Vendor ID (filled in during
                                    // USBDCDCInit).
    USBShort(0),                    // Product ID (filled in during
                                    // USBDCDCInit).
    USBShort(0x100),                // Device Version BCD.
    1,                              // Manufacturer string identifier.
    2,                              // Product string identifier.
    3,                              // Product serial number.
    1                               // Number of configurations.
};

//*****************************************************************************
//
// Mass storage device configuration descriptor.
//
// It is vital that the configuration descriptor bConfigurationValue field
// (byte 6) is 1 for the first configuration and increments by 1 for each
// additional configuration defined here.  This relationship is assumed in the
// device stack for simplicity even though the USB 2.0 specification imposes
// no such restriction on the bConfigurationValue values.
//
// Note that this structure is deliberately located in RAM since we need to
// be able to patch some values in it based on client requirements.
//
//*****************************************************************************
static uint8_t g_pui8MSCDescriptor[] =
{
    //
    // Configuration descriptor header.
    //
    9,                              // Size of the configuration descriptor.
    USB_DTYPE_CONFIGURATION,        // Type of this descriptor.
    USBShort(32),                   // The total size of this full structure.
    1,                              // The number of interfaces in this
                                    // configuration.
    1,                              // The unique value for this configuration.
    0,                              // The string identifier that describes
                                    // this configuration.
    USB_CONF_ATTR_SELF_PWR,         // Bus Powered, Self Powered, remote wake
                                    // up.
    250,                            // The maximum power in 2mA increments.
};

//*****************************************************************************
//
// The remainder of the configuration descriptor is stored in flash since we
// don't need to modify anything in it at runtime.
//
//*****************************************************************************
const uint8_t g_pui8MSCInterface[MSCINTERFACE_SIZE] =
{
    //
    // Vendor-specific Interface Descriptor.
    //
    9,                              // Size of the interface descriptor.
    USB_DTYPE_INTERFACE,            // Type of this descriptor.
    0,                              // The index for this interface.
    0,                              // The alternate setting for this
                                    // interface.
    2,                              // The number of endpoints used by this
                                    // interface.
    USB_CLASS_MASS_STORAGE,         // The interface class
    USB_MSC_SUBCLASS_SCSI,          // The interface sub-class.
    USB_MSC_PROTO_BULKONLY,         // The interface protocol for the sub-class
                                    // specified above.
    0,                              // The string index for this interface.

    //
    // Endpoint Descriptor
    //
    7,                              // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,             // Descriptor type is an endpoint.
    USB_EP_DESC_IN | USBEPToIndex(DATA_IN_ENDPOINT),
    USB_EP_ATTR_BULK,               // Endpoint is a bulk endpoint.
    USBShort(DATA_IN_EP_MAX_SIZE),  // The maximum packet size.
    0,                              // The polling interval for this endpoint.

    //
    // Endpoint Descriptor
    //
    7,                              // The size of the endpoint descriptor.
    USB_DTYPE_ENDPOINT,             // Descriptor type is an endpoint.
    USB_EP_DESC_OUT | USBEPToIndex(DATA_OUT_ENDPOINT),
    USB_EP_ATTR_BULK,               // Endpoint is a bulk endpoint.
    USBShort(DATA_OUT_EP_MAX_SIZE), // The maximum packet size.
    0,                              // The polling interval for this endpoint.
};

//*****************************************************************************
//
// The mass storage configuration descriptor is defined as two sections,
// one containing just the 9 byte USB configuration descriptor and the other
// containing everything else that is sent to the host along with it.
//
//*****************************************************************************
const tConfigSection g_sMSCConfigSection =
{
    sizeof(g_pui8MSCDescriptor),
    g_pui8MSCDescriptor
};

const tConfigSection g_sMSCInterfaceSection =
{
    sizeof(g_pui8MSCInterface),
    g_pui8MSCInterface
};

//*****************************************************************************
//
// This array lists all the sections that must be concatenated to make a
// single, complete bulk device configuration descriptor.
//
//*****************************************************************************
const tConfigSection *g_psMSCSections[] =
{
    &g_sMSCConfigSection,
    &g_sMSCInterfaceSection
};

#define NUM_MSC_SECTIONS        (sizeof(g_psMSCSections) /                    \
                                 sizeof(g_psMSCSections[0]))

//*****************************************************************************
//
// The header for the single configuration we support.  This is the root of
// the data structure that defines all the bits and pieces that are pulled
// together to generate the configuration descriptor.
//
//*****************************************************************************
const tConfigHeader g_sMSCConfigHeader =
{
    NUM_MSC_SECTIONS,
    g_psMSCSections
};

//*****************************************************************************
//
// Configuration Descriptor.
//
//*****************************************************************************
const tConfigHeader * const g_ppsMSCConfigDescriptors[] =
{
    &g_sMSCConfigHeader
};

//*****************************************************************************
//
// Various internal handlers needed by this class.
//
//*****************************************************************************
static void HandleDisconnect(void *pvMSCDevice);
static void ConfigChangeHandler(void *pvMSCDevice, uint32_t ui32Value);
static void HandleEndpoints(void *pvMSCDevice, uint32_t ui32Status);
static void HandleRequests(void *pvMSCDevice, tUSBRequest *psUSBRequest);
static void USBDSCSISendStatus(tUSBDMSCDevice *psMSCDevice);
uint32_t USBDSCSICommand(tUSBDMSCDevice *psMSCDevice, tMSCCBW *psSCSICBW);
static void HandleDevice(void *pvMSCDevice, uint32_t ui32Request,
                         void *pvRequestData);

//*****************************************************************************
//
// The device information structure for the USB MSC device.
//
//*****************************************************************************
const tCustomHandlers g_sMSCHandlers =
{
    //
    // GetDescriptor
    //
    0,

    //
    // RequestHandler
    //
    HandleRequests,

    //
    // InterfaceChange
    //
    0,

    //
    // ConfigChange
    //
    ConfigChangeHandler,

    //
    // DataReceived
    //
    0,

    //
    // DataSentCallback
    //
    0,

    //
    // ResetHandler
    //
    0,

    //
    // SuspendHandler
    //
    0,

    //
    // ResumeHandler
    //
    0,

    //
    // DisconnectHandler
    //
    HandleDisconnect,

    //
    // EndpointHandler
    //
    HandleEndpoints,

    //
    // Device handler
    //
    HandleDevice
};

//*****************************************************************************
//
//! This function is used by an application if it can detect insertion or
//! removal of the media.
//!
//! \param pvMSCDevice is the mass storage device instance that had a media
//! change.
//! \param iMediaStatus is the updated status for the media.
//!
//! This function should be called by an application when it detects a change
//! in the status of the media in use by the USB mass storage class.  The
//! \e iMediaStatus parameter will indicate the new status of the media and
//! can also indicate that the application has no knowledge of the media state.
//!
//! There are currently the three following values for the \e iMediaStatus
//! parameter:
//! - eUSBDMSCMediaPresent indicates that the media is present or has been
//! added.
//! - eUSBDMSCMediaNotPresent indicates that the media is not present or was
//! removed.
//! - eUSBDMSCMediaUnknown indicates that the application has no knowledge of
//! the media state and the USB mass storage class.
//!
//! It will be left up to the application to call this function whenever it
//! detects a change or simply call it once with eUSBDMSCMediaUnknown and
//! allow the mass storage class to infer the state from the remaining device
//! APIs.
//!
//! \note It is recommended that the application use this function to inform
//! the mass storage class of media state changes as it will lead to a more
//! responsive system.
//!
//! \return None.
//
//*****************************************************************************
void
USBDMSCMediaChange(void *pvMSCDevice, tUSBDMSCMediaStatus iMediaStatus)
{
    tUSBDMSCDevice *psMSCDevice;

    //
    // Create a device instance pointer.
    //
    psMSCDevice = pvMSCDevice;

    //
    // Save the current media status.
    //
    psMSCDevice->sPrivateData.iMediaStatus = iMediaStatus;
}

//*****************************************************************************
//
// This function is called to handle the interrupts on the Bulk endpoints for
// the mass storage class.
//
//*****************************************************************************
static void
HandleEndpoints(void *pvMSCDevice, uint32_t ui32Status)
{
    tUSBDMSCDevice *psMSCDevice;
    tMSCInstance *psInst;
    tMSCCBW *psSCSICBW;
    uint32_t ui32EPStatus, ui32Size;

    ASSERT(pvMSCDevice != 0);

    //
    // Determine if the serial device is in single or composite mode because
    // the meaning of ui32Index is different in both cases.
    //
    psMSCDevice = pvMSCDevice;

    //
    // Initialize the workspace in the passed instance structure.
    //
    psInst = &psMSCDevice->sPrivateData;

    //
    // Get the endpoints status.
    //
    ui32EPStatus = MAP_USBEndpointStatus(USB0_BASE, psInst->ui8OUTEndpoint);

    //
    // Handler for the bulk IN data endpoint.
    //
    if((ui32Status & (1 << USBEPToIndex(psInst->ui8INEndpoint))) ||
       ((psInst->ui32Flags & USBD_FLAG_DMA_IN) &&
       (USBLibDMAChannelStatus(psInst->psDMAInstance, psInst->ui8INDMA) &
        USBLIBSTATUS_DMA_COMPLETE)))
    {
        switch(psInst->ui8SCSIState)
        {
            //
            // Handle the case where we are sending out data due to a read
            // command.
            //
            case STATE_SCSI_SEND_BLOCKS:
            {
                //
                // Decrement the number of bytes left to send.
                //
                psInst->ui32BytesToTransfer -= MAX_TRANSFER_SIZE;

                //
                // If we are done then move on to the status phase.
                //
                if(psInst->ui32BytesToTransfer == 0)
                {
                    //
                    // Set the status so that it can be sent when this
                    // response has has be successfully sent.
                    //
                    g_sSCSICSW.bCSWStatus = 0;
                    g_sSCSICSW.dCSWDataResidue = 0;

                    //
                    // DMA has completed for the IN endpoint.
                    //
                    psInst->ui32Flags &= ~USBD_FLAG_DMA_IN;

                    //
                    // Disable uDMA on the endpoint
                    //
                    MAP_USBEndpointDMADisable(USB0_BASE, psInst->ui8INEndpoint,
                                              USB_EP_DEV_IN);

                    if(psMSCDevice->pfnEventCallback)
                    {
                        psMSCDevice->pfnEventCallback(0, USBD_MSC_EVENT_IDLE,
                                                      0, 0);
                    }

                    //
                    // Make sure that the transfer has actually finished.  If
                    // it has not there will be another interrupt to send
                    // out the status.
                    //
                    if(USBEndpointStatus(USB0_BASE,psInst->ui8INEndpoint) &
                       USB_DEV_TX_TXPKTRDY)
                    {
                        //
                        // Send back the status once this transfer is complete.
                        //
                        psInst->ui8SCSIState = STATE_SCSI_SEND_STATUS;
                    }
                    else
                    {
                        //
                        // Indicate success and no extra data coming.
                        //
                        USBDSCSISendStatus(psMSCDevice);
                    }

                    //
                    // The transfer is complete so don't read anymore data.
                    //
                    break;
                }

                //
                // Move on to the next Logical Block.
                //
                psInst->ui32CurrentLBA++;

                //
                // Read the new data and send it out.
                //
                if(psMSCDevice->sMediaFunctions.pfnBlockRead(psInst->pvMedia,
                                              (uint8_t *)psInst->pui32Buffer,
                                              psInst->ui32CurrentLBA, 1) == 0)
                {
                }

                //
                // Configure and enable DMA for the IN transfer.
                //
                USBLibDMATransfer(psInst->psDMAInstance,
                                  psInst->ui8INDMA, psInst->pui32Buffer,
                                  MAX_TRANSFER_SIZE);

                //
                // Start the DMA transfer.
                //
                USBLibDMAChannelEnable(psInst->psDMAInstance,
                                       psInst->ui8INDMA);

                break;
            }

            //
            // Handle sending status.
            //
            case STATE_SCSI_SEND_STATUS:
            {
                //
                // Indicate success and no extra data coming.
                //
                USBDSCSISendStatus(psMSCDevice);

                break;
            }

            //
            // Handle completing sending status.
            //
            case STATE_SCSI_SENT_STATUS:
            {
                psInst->ui8SCSIState = STATE_SCSI_IDLE;

                break;
            }

            //
            // These cases should not occur as the being in the IDLE state due
            // to an IN interrupt is invalid.
            //
            case STATE_SCSI_IDLE:
            default:
            {
                break;
            }
        }
    }

    //
    // Handler for the bulk OUT data endpoint.
    //
    if((ui32Status & (0x10000 << USBEPToIndex(psInst->ui8OUTEndpoint))) ||
        ((psInst->ui32Flags & USBD_FLAG_DMA_OUT) &&
       (USBLibDMAChannelStatus(psInst->psDMAInstance, psInst->ui8OUTDMA) &
        USBLIBSTATUS_DMA_COMPLETE)))
    {
        //
        // Get the endpoint status to see why we were called.
        //
        ui32EPStatus = MAP_USBEndpointStatus(USB0_BASE,
                                             psInst->ui8OUTEndpoint);

        switch(psInst->ui8SCSIState)
        {
            //
            // Receiving and writing bytes to the storage device.
            //
            case STATE_SCSI_RECEIVE_BLOCKS:
            {
                //
                // Update the current status for the buffer.
                //
                psInst->ui32BytesToTransfer -= MAX_TRANSFER_SIZE;

                //
                // Write the new data.
                //
                psMSCDevice->sMediaFunctions.pfnBlockWrite(psInst->pvMedia,
                                                (uint8_t *)psInst->pui32Buffer,
                                                psInst->ui32CurrentLBA, 1);

                //
                // Move on to the next Logical Block.
                //
                psInst->ui32CurrentLBA++;

                //
                // Check if all bytes have been received.
                //
                if(psInst->ui32BytesToTransfer == 0)
                {
                    //
                    // Set the status so that it can be sent when this response
                    // has be successfully sent.
                    //
                    g_sSCSICSW.bCSWStatus = 0;
                    g_sSCSICSW.dCSWDataResidue = 0;

                    //
                    // DMA has completed for the OUT endpoint.
                    //
                    psInst->ui32Flags &= ~USBD_FLAG_DMA_OUT;

                    //
                    // Indicate success and no extra data coming.
                    //
                    USBDSCSISendStatus(psMSCDevice);

                    //
                    // Disable uDMA on the endpoint
                    //
                    MAP_USBEndpointDMADisable(USB0_BASE,
                                              psInst->ui8OUTEndpoint,
                                              USB_EP_DEV_OUT);

                    //
                    // If there is an event callback then call it to notify
                    // that last operation has completed.
                    //
                    if(psMSCDevice->pfnEventCallback)
                    {
                        psMSCDevice->pfnEventCallback(0, USBD_MSC_EVENT_IDLE,
                                                      0, 0);
                    }
                }
                else
                {
                    //
                    // Configure and enable DMA for the OUT transfer.
                    //
                    USBLibDMATransfer(psInst->psDMAInstance,
                                      psInst->ui8OUTDMA, psInst->pui32Buffer,
                                      MAX_TRANSFER_SIZE);
                }

                break;
            }

            //
            // If there is an OUT transfer in idle state then it was a new
            // command.
            //
            case STATE_SCSI_IDLE:
            {
                //
                // Attempt to handle the new command.
                //

                //
                // Receive the command.
                //
                ui32Size = COMMAND_BUFFER_SIZE;
                MAP_USBEndpointDataGet(psInst->ui32USBBase,
                                       psInst->ui8OUTEndpoint,
                                       g_pui8Command, &ui32Size);
                psSCSICBW = (tMSCCBW *)g_pui8Command;

                //
                // Acknowledge the OUT data packet.
                //
                MAP_USBDevEndpointDataAck(psInst->ui32USBBase,
                                          psInst->ui8OUTEndpoint, false);

                //
                // If this is a valid CBW then handle it.
                //
                if(psSCSICBW->dCBWSignature == CBW_SIGNATURE)
                {
                    g_sSCSICSW.dCSWSignature = CSW_SIGNATURE;
                    g_sSCSICSW.dCSWTag = psSCSICBW->dCBWTag;
                    g_sSCSICSW.dCSWDataResidue = 0;
                    g_sSCSICSW.bCSWStatus = 0;

                    USBDSCSICommand(psMSCDevice, psSCSICBW);
                }
                else
                {
                    //
                    // Just return to the idle state since we are now out of
                    // sync with the host.  This should not happen, but this
                    // should allow the device to synchronize with the host
                    // controller.
                    //
                    psInst->ui8SCSIState = STATE_SCSI_IDLE;
                }

                break;
            }
            default:
            {
                break;
            }
        }

        //
        // Clear the status bits.
        //
        MAP_USBDevEndpointStatusClear(USB0_BASE, psInst->ui8OUTEndpoint,
                                      ui32EPStatus);
    }
}

//*****************************************************************************
//
// Device instance specific handler.
//
//*****************************************************************************
static void
HandleDevice(void *pvMSCDevice, uint32_t ui32Request, void *pvRequestData)
{
    tMSCInstance *psInst;
    uint8_t *pui8Data;

    //
    // Get the instance data pointers.
    //
    psInst = &((tUSBDMSCDevice *)pvMSCDevice)->sPrivateData;

    //
    // Create the 8-bit array used by the events supported by the USB MSC
    // class.
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

                //
                // If the DMA channel has already been allocated then clear
                // that channel and prepare to possibly use a new one.
                //
                if(psInst->ui8INDMA != 0)
                {
                    USBLibDMAChannelRelease(psInst->psDMAInstance,
                                            psInst->ui8INDMA);
                }

                //
                // Allocate a DMA channel to the endpoint.
                //
                psInst->ui8INDMA =
                    USBLibDMAChannelAllocate(psInst->psDMAInstance,
                                             psInst->ui8INEndpoint, 0,
                                             USB_DMA_EP_TX |
                                             USB_DMA_EP_DEVICE);

                //
                // Set the DMA individual transfer size.
                //
                USBLibDMAUnitSizeSet(psInst->psDMAInstance, psInst->ui8INDMA,
                                     32);

                //
                // Set the DMA arbitration size.
                //
                USBLibDMAArbSizeSet(psInst->psDMAInstance, psInst->ui8INDMA,
                                    16);
            }
            else
            {
                //
                // If the DMA channel has already been allocated then clear
                // that channel and prepare to possibly use a new one.
                //
                if(psInst->ui8OUTDMA != 0)
                {
                    USBLibDMAChannelRelease(psInst->psDMAInstance,
                                            psInst->ui8OUTDMA);
                }

                //
                // Allocate a DMA channel to the endpoint.
                //
                psInst->ui8OUTDMA =
                    USBLibDMAChannelAllocate(psInst->psDMAInstance,
                                             psInst->ui8OUTEndpoint, 0,
                                             USB_DMA_EP_RX |
                                             USB_DMA_EP_DEVICE);

                //
                // Set the DMA individual transfer size.
                //
                USBLibDMAUnitSizeSet(psInst->psDMAInstance, psInst->ui8OUTDMA,
                                     32);

                //
                // Set the DMA arbitration size.
                //
                USBLibDMAArbSizeSet(psInst->psDMAInstance, psInst->ui8OUTDMA,
                                    16);
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
HandleDisconnect(void *pvMSCDevice)
{
    tUSBDMSCDevice *psMSCDevice;

    ASSERT(pvMSCDevice != 0);

    //
    // Create the instance pointer.
    //
    psMSCDevice = (tUSBDMSCDevice *)pvMSCDevice;

    //
    // Close the drive requested.
    //
    if(psMSCDevice->sPrivateData.pvMedia != 0)
    {
        psMSCDevice->sPrivateData.pvMedia = 0;
        psMSCDevice->sMediaFunctions.pfnClose(0);
    }

    //
    // If we have a control callback, let the client know we are open for
    // business.
    //
    if(psMSCDevice->pfnEventCallback)
    {
        //
        // Pass the connected event to the client.
        //
        psMSCDevice->pfnEventCallback(pvMSCDevice, USB_EVENT_DISCONNECTED, 0,
                                      0);
    }
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever the device
// configuration changes.
//
//*****************************************************************************
static void
ConfigChangeHandler(void *pvMSCDevice, uint32_t ui32Value)
{
    tUSBDMSCDevice *psMSCDevice;

    ASSERT(pvMSCDevice != 0);

    //
    // Create the instance pointer.
    //
    psMSCDevice = (tUSBDMSCDevice *)pvMSCDevice;

    //
    // If the DMA channel has already been allocated then clear
    // that channel and prepare to possibly use a new one.
    //
    if(psMSCDevice->sPrivateData.ui8OUTDMA != 0)
    {
        USBLibDMAChannelRelease(psMSCDevice->sPrivateData.psDMAInstance,
                                psMSCDevice->sPrivateData.ui8OUTDMA);
    }

    //
    // Configure the DMA for the OUT endpoint.
    //
    psMSCDevice->sPrivateData.ui8OUTDMA =
        USBLibDMAChannelAllocate(psMSCDevice->sPrivateData.psDMAInstance,
                                 psMSCDevice->sPrivateData.ui8OUTEndpoint, 64,
                                 USB_DMA_EP_RX | USB_DMA_EP_DEVICE);

    USBLibDMAUnitSizeSet(psMSCDevice->sPrivateData.psDMAInstance,
                         psMSCDevice->sPrivateData.ui8OUTDMA, 32);

    USBLibDMAArbSizeSet(psMSCDevice->sPrivateData.psDMAInstance,
                        psMSCDevice->sPrivateData.ui8OUTDMA, 16);

    //
    // If the DMA channel has already been allocated then clear
    // that channel and prepare to possibly use a new one.
    //
    if(psMSCDevice->sPrivateData.ui8INDMA != 0)
    {
        USBLibDMAChannelRelease(psMSCDevice->sPrivateData.psDMAInstance,
                                psMSCDevice->sPrivateData.ui8INDMA);
    }

    //
    // Configure the DMA for the IN endpoint.
    //
    psMSCDevice->sPrivateData.ui8INDMA =
        USBLibDMAChannelAllocate(psMSCDevice->sPrivateData.psDMAInstance,
                                 psMSCDevice->sPrivateData.ui8INEndpoint, 64,
                                 USB_DMA_EP_TX | USB_DMA_EP_DEVICE);

    USBLibDMAUnitSizeSet(psMSCDevice->sPrivateData.psDMAInstance,
                         psMSCDevice->sPrivateData.ui8INDMA, 32);

    USBLibDMAArbSizeSet(psMSCDevice->sPrivateData.psDMAInstance,
                        psMSCDevice->sPrivateData.ui8INDMA, 16);

    //
    // If we have a control callback, let the client know we are open for
    // business.
    //
    if(psMSCDevice->pfnEventCallback)
    {
        //
        // Pass the connected event to the client.
        //
        psMSCDevice->pfnEventCallback(pvMSCDevice, USB_EVENT_CONNECTED, 0, 0);
    }
}

//*****************************************************************************
//
//! This function should be called once for the mass storage class device to
//! initialized basic operation and prepare for enumeration.
//!
//! \param ui32Index is the index of the USB controller to initialize for
//! mass storage class device operation.
//! \param psMSCDevice points to a structure containing parameters customizing
//! the operation of the mass storage device.
//!
//! In order for an application to initialize the USB device mass storage
//! class, it must first call this function with the a valid mass storage
//! device class structure in the \e psMSCDevice parameter.  This allows this
//! function to initialize the USB controller and device code to be prepared to
//! enumerate and function as a USB mass storage device.
//!
//! This function returns a void pointer that must be passed in to all other
//! APIs used by the mass storage class.
//!
//! See the documentation on the tUSBDMSCDevice structure for more information
//! on how to properly fill the structure members.
//!
//! \return Returns 0 on failure or a non-zero void pointer on success.
//
//*****************************************************************************
void *
USBDMSCInit(uint32_t ui32Index, tUSBDMSCDevice *psMSCDevice)
{
    tDeviceDescriptor *psDevDesc;
    tConfigDescriptor *pConfDesc;

    //
    // Check parameter validity.
    //
    ASSERT(ui32Index == 0);
    ASSERT(psMSCDevice);
    ASSERT(psMSCDevice->ppui8StringDescriptors);

    USBDMSCCompositeInit(ui32Index, psMSCDevice, 0);

    //
    // Fix up the device descriptor with the client-supplied values.
    //
    psDevDesc = (tDeviceDescriptor *)g_pui8MSCDeviceDescriptor;
    psDevDesc->idVendor = psMSCDevice->ui16VID;
    psDevDesc->idProduct = psMSCDevice->ui16PID;

    //
    // Fix up the configuration descriptor with client-supplied values.
    //
    pConfDesc = (tConfigDescriptor *)g_pui8MSCDescriptor;
    pConfDesc->bmAttributes = psMSCDevice->ui8PwrAttributes;
    pConfDesc->bMaxPower = (uint8_t)(psMSCDevice->ui16MaxPowermA / 2);

    //
    // All is well so now pass the descriptors to the lower layer and put
    // the bulk device on the bus.
    //
    USBDCDInit(ui32Index, &psMSCDevice->sPrivateData.sDevInfo,
               (void *)psMSCDevice);

    //
    // Return the pointer to the instance indicating that everything went well.
    //
    return((void *)psMSCDevice);
}

//*****************************************************************************
//
//! This function should be called once for the mass storage class device to
//! initialized basic operation and prepare for enumeration.
//!
//! \param ui32Index is the index of the USB controller to initialize for
//! mass storage class device operation.
//! \param psMSCDevice points to a structure containing parameters customizing
//! the operation of the mass storage device.
//! \param psCompEntry is the composite device entry to initialize when
//! creating a composite device.
//!
//! In order for an application to initialize the USB device mass storage
//! class, it must first call this function with the a valid mass storage
//! device class structure in the \e psMSCDevice parameter.  This allows this
//! function to initialize the USB controller and device code to be prepared to
//! enumerate and function as a USB mass storage device.  If this mass storage
//! device is part of a composite device, then the \e psCompEntry should
//! point to the composite device entry to initialize.  This is part of the
//! array that is passed to the USBDCompositeInit() function.
//!
//! This function returns a void pointer that must be passed in to all other
//! APIs used by the mass storage class.
//!
//! See the documentation on the tUSBDMSCDevice structure for more information
//! on how to properly fill the structure members.
//!
//! \return Returns zero on failure or a non-zero instance value that should be
//! used with the remaining USB mass storage APIs.
//
//*****************************************************************************
void *
USBDMSCCompositeInit(uint32_t ui32Index, tUSBDMSCDevice *psMSCDevice,
                     tCompositeEntry *psCompEntry)
{
    tMSCInstance *psInst;

    //
    // Check parameter validity.
    //
    ASSERT(ui32Index == 0);
    ASSERT(psMSCDevice);
    ASSERT(psMSCDevice->ppui8StringDescriptors);
    ASSERT(psCompEntry != 0);

    //
    // Initialize the workspace in the passed instance structure.
    //
    psInst = &psMSCDevice->sPrivateData;
    psInst->ui32USBBase = USB0_BASE;
    psInst->bConnected = false;
    psInst->iMediaStatus = eUSBDMSCMediaUnknown;

    //
    // Initialize the composite entry that is used by the composite device
    // class.
    //
    if(psCompEntry != 0)
    {
        psCompEntry->psDevInfo = &psInst->sDevInfo;
        psCompEntry->pvInstance = (void *)psMSCDevice;
    }

    //
    // Initialize the device information structure.
    //
    psInst->sDevInfo.psCallbacks = &g_sMSCHandlers;
    psInst->sDevInfo.pui8DeviceDescriptor = g_pui8MSCDeviceDescriptor;
    psInst->sDevInfo.ppsConfigDescriptors = g_ppsMSCConfigDescriptors;
    psInst->sDevInfo.ppui8StringDescriptors = 0;
    psInst->sDevInfo.ui32NumStringDescriptors = 0;

    //
    // Initialize the device info structure for the mass storage device.
    //
    USBDCDDeviceInfoInit(0, &psInst->sDevInfo);

    //
    // Set the initial interface and endpoints.
    //
    psInst->ui8Interface = 0;
    psInst->ui8OUTEndpoint = DATA_OUT_ENDPOINT;
    psInst->ui8INEndpoint = DATA_IN_ENDPOINT;

    //
    // Set the initial SCSI state to idle.
    //
    psInst->ui8SCSIState = STATE_SCSI_IDLE;

    //
    // Plug in the client's string stable to the device information
    // structure.
    //
    psInst->sDevInfo.ppui8StringDescriptors =
                                        psMSCDevice->ppui8StringDescriptors;
    psInst->sDevInfo.ui32NumStringDescriptors =
                                        psMSCDevice->ui32NumStringDescriptors;

    //
    // Open the drive requested.
    //
    psInst->pvMedia = psMSCDevice->sMediaFunctions.pfnOpen(0);

    if(psInst->pvMedia == 0)
    {
        //
        // There is no media currently present.
        //
        psInst->ui8SenseKey = SCSI_RS_KEY_NOT_READY;
        psInst->ui16AddSenseCode = SCSI_RS_MED_NOT_PRSNT;
    }
    else
    {
        //
        // Media is now ready for use.
        //
        psInst->ui8SenseKey = SCSI_RS_KEY_UNIT_ATTN;
        psInst->ui16AddSenseCode = SCSI_RS_MED_NOTRDY2RDY;
    }

    //
    // Enable Clocking to the USB controller.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);

    //
    // Turn on USB Phy clock.
    //
    MAP_SysCtlUSBPLLEnable();

    //
    // Get the DMA instance pointer.
    //
    psInst->psDMAInstance = USBLibDMAInit(0);

    //
    // Return the pointer to the instance indicating that everything went well.
    //
    return((void *)psMSCDevice);
}

//*****************************************************************************
//
//! Shuts down the mass storage device.
//!
//! \param pvMSCDevice is the pointer to the device instance structure as
//! returned by USBDMSCInit() or USBDMSCInitComposite().
//!
//! This function terminates mass storage operation for the instance supplied
//! and removes the device from the USB bus.  Following this call, the
//! \e pvMSCDevice instance may not me used in any other call to the mass
//! storage device other than USBDMSCInit() or USBDMSCInitComposite().
//!
//! \return None.
//
//*****************************************************************************
void
USBDMSCTerm(void *pvMSCDevice)
{
    tUSBDMSCDevice *psMSCDevice;

    ASSERT(pvMSCDevice != 0);

    //
    // Cleanly exit device mode.
    //
    USBDCDTerm(0);

    //
    // Create a device instance pointer.
    //
    psMSCDevice = pvMSCDevice;

    //
    // If the media was opened the close it out.
    //
    if(psMSCDevice->sPrivateData.pvMedia != 0)
    {
        psMSCDevice->sPrivateData.pvMedia = 0;
        psMSCDevice->sMediaFunctions.pfnClose(0);
    }
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever a non-standard
// request is received.
//
// \param pvMSCDevice is instance data for this request.
// \param pUSBRequest points to the request received.
//
// This call parses the provided request structure to determine the command.
// The only mass storage command supported over endpoint 0 is the Get Max LUN
// command.
//
// \return None.
//
//*****************************************************************************
static void
HandleRequests(void *pvMSCDevice, tUSBRequest *pUSBRequest)
{
    //
    // This class only support a single LUN.
    //
    static const uint8_t ui8MaxLun = 0;

    ASSERT(pvMSCDevice != 0);

    //
    // Determine the type of request.
    //
    switch(pUSBRequest->bRequest)
    {
        //
        // A Set Report request is received from the host when it sends an
        // Output report via endpoint 0.
        //
        case USBREQ_GET_MAX_LUN:
        {
            //
            // Send our response to the host.
            //
            USBDCDSendDataEP0(0, (uint8_t *)&ui8MaxLun, 1);

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
// This function is used to handle the SCSI Inquiry command when it is received
// from the host.
//
//*****************************************************************************
static void
USBDSCSIInquiry(tUSBDMSCDevice *psMSCDevice)
{
    int32_t i32Idx;
    tMSCInstance *psInst;
    uint32_t *pui32Data;

    //
    // Create a local 32-bit pointer to the command.
    //
    pui32Data = (uint32_t *)g_pui8Command;

    //
    // Create the serial instance data.
    //
    psInst = &psMSCDevice->sPrivateData;

    //
    // Direct Access device, Removable storage and SCSI 1 responses.
    //
    pui32Data[0] = SCSI_INQ_PDT_SBC | (SCSI_INQ_RMB << 8);

    //
    // Additional Length is fixed at 31 bytes.
    //
    pui32Data[1] = 31;

    //
    // Copy the Vendor string.
    //
    for(i32Idx = 0; i32Idx < 8; i32Idx++)
    {
        g_pui8Command[i32Idx + 8] = psMSCDevice->pui8Vendor[i32Idx];
    }

    //
    // Copy the Product string.
    //
    for(i32Idx = 0; i32Idx < 16; i32Idx++)
    {
        g_pui8Command[i32Idx + 16] = psMSCDevice->pui8Product[i32Idx];
    }

    //
    // Copy the Version string.
    //
    for(i32Idx = 0; i32Idx < 4; i32Idx++)
    {
        g_pui8Command[i32Idx + 32] = psMSCDevice->pui8Version[i32Idx];
    }

    //
    // Send the SCSI Inquiry Response.
    //
    MAP_USBEndpointDataPut(USB0_BASE, psInst->ui8INEndpoint, g_pui8Command,
                           36);

    //
    // Send the data to the host.
    //
    MAP_USBEndpointDataSend(USB0_BASE, psInst->ui8INEndpoint, USB_TRANS_IN);

    //
    // Set the status so that it can be sent when this response has
    // has be successfully sent.
    //
    g_sSCSICSW.bCSWStatus = 0;
    g_sSCSICSW.dCSWDataResidue = 0;

    psInst->ui8SCSIState = STATE_SCSI_SEND_STATUS;
}

//*****************************************************************************
//
// This function is used to handle the SCSI Read Capacities command when it is
// received from the host.
//
//*****************************************************************************
static void
USBDSCSIReadCapacities(tUSBDMSCDevice *psMSCDevice)
{
    uint32_t ui32Blocks;
    tMSCInstance *psInst;
    uint32_t *pui32Data;

    //
    // Create a local 32-bit pointer to the command.
    //
    pui32Data = (uint32_t *)g_pui8Command;

    //
    // Get our instance data pointer.
    //
    psInst = &psMSCDevice->sPrivateData;

    if(psInst->pvMedia != 0)
    {
        ui32Blocks =
                    psMSCDevice->sMediaFunctions.pfnNumBlocks(psInst->pvMedia);

        pui32Data[0] = 0x08000000;

        //
        // Fill in the number of blocks, the bytes endianness must be changed.
        //
        g_pui8Command[4] = ui32Blocks >> 24;
        g_pui8Command[5] = 0xff & (ui32Blocks >> 16);
        g_pui8Command[6] = 0xff & (ui32Blocks >> 8);
        g_pui8Command[7] = 0xff & (ui32Blocks);

        //
        // Current media capacity
        //
        g_pui8Command[8] = 0x2;

        //
        // Fill in the block size, which is fixed at DEVICE_BLOCK_SIZE.
        //
        g_pui8Command[9] = 0xff & (DEVICE_BLOCK_SIZE >> 16);
        g_pui8Command[10] = 0xff & (DEVICE_BLOCK_SIZE >> 8);
        g_pui8Command[11] = 0xff & DEVICE_BLOCK_SIZE;

        //
        // Send out the 12 bytes that are in this response.
        //
        MAP_USBEndpointDataPut(USB0_BASE, psInst->ui8INEndpoint, g_pui8Command,
                               12);
        MAP_USBEndpointDataSend(USB0_BASE, psInst->ui8INEndpoint,
                                USB_TRANS_IN);

        //
        // Set the status so that it can be sent when this response has
        // has be successfully sent.
        //
        g_sSCSICSW.bCSWStatus = 0;
        g_sSCSICSW.dCSWDataResidue = 0;
    }
    else
    {
        //
        // Set the status so that it can be sent when this response has
        // has be successfully sent.
        //
        g_sSCSICSW.bCSWStatus = 1;
        g_sSCSICSW.dCSWDataResidue = 0;

        //
        // Stall the IN endpoint
        //
        MAP_USBDevEndpointStall(USB0_BASE, psInst->ui8INEndpoint,
                                USB_EP_DEV_IN);

        //
        // Mark the sense code as valid and indicate that these is no media
        // present.
        //
        psInst->ui8ErrorCode = SCSI_RS_VALID | SCSI_RS_CUR_ERRORS;
        psInst->ui8SenseKey = SCSI_RS_KEY_NOT_READY;
        psInst->ui16AddSenseCode = SCSI_RS_MED_NOT_PRSNT;
    }

    psInst->ui8SCSIState = STATE_SCSI_SEND_STATUS;
}

//*****************************************************************************
//
// This function is used to handle the SCSI Read Capacity command when it is
// received from the host.
//
//*****************************************************************************
static void
USBDSCSIReadCapacity(tUSBDMSCDevice *psMSCDevice)
{
    uint32_t ui32Blocks;
    tMSCInstance *psInst;

    //
    // Get our instance data pointer.
    //
    psInst = &psMSCDevice->sPrivateData;

    ui32Blocks = psMSCDevice->sMediaFunctions.pfnNumBlocks(psInst->pvMedia);

    //
    // Only decrement if any blocks were found.
    //
    if(ui32Blocks != 0)
    {
        //
        // One less than the maximum number is the last addressable
        // block.
        //
        ui32Blocks--;
    }

    if(psInst->pvMedia != 0)
    {
        //
        // Fill in the number of blocks, the bytes endianness must be changed.
        //
        g_pui8Command[0] = 0xff & (ui32Blocks >> 24);
        g_pui8Command[1] = 0xff & (ui32Blocks >> 16);
        g_pui8Command[2] = 0xff & (ui32Blocks >> 8);
        g_pui8Command[3] = 0xff & (ui32Blocks);

        g_pui8Command[4] = 0;

        //
        // Fill in the block size, which is fixed at DEVICE_BLOCK_SIZE.
        //
        g_pui8Command[5] = 0xff & (DEVICE_BLOCK_SIZE >> 16);
        g_pui8Command[6] = 0xff & (DEVICE_BLOCK_SIZE >> 8);
        g_pui8Command[7] = 0xff & DEVICE_BLOCK_SIZE;

        //
        // Send the SCSI Inquiry Response.
        //
        MAP_USBEndpointDataPut(USB0_BASE, psInst->ui8INEndpoint, g_pui8Command,
                               8);
        MAP_USBEndpointDataSend(USB0_BASE, psInst->ui8INEndpoint,
                                USB_TRANS_IN);

        //
        // Set the status so that it can be sent when this response has
        // has be successfully sent.
        //
        g_sSCSICSW.bCSWStatus = 0;
        g_sSCSICSW.dCSWDataResidue = 0;
    }
    else
    {
        //
        // Set the status so that it can be sent when this response has
        // has be successfully sent.
        //
        g_sSCSICSW.bCSWStatus = 1;
        g_sSCSICSW.dCSWDataResidue = 0;

        //
        // Stall the IN endpoint
        //
        MAP_USBDevEndpointStall(USB0_BASE, psInst->ui8INEndpoint,
                                USB_EP_DEV_IN);

        //
        // Mark the sense code as valid and indicate that these is no media
        // present.
        //
        psInst->ui8ErrorCode = SCSI_RS_VALID | SCSI_RS_CUR_ERRORS;
        psInst->ui8SenseKey = SCSI_RS_KEY_NOT_READY;
        psInst->ui16AddSenseCode = SCSI_RS_MED_NOT_PRSNT;
    }

    psInst->ui8SCSIState = STATE_SCSI_SEND_STATUS;
}

//*****************************************************************************
//
// This function is used to handle the SCSI Request Sense command when it is
// received from the host.
//
//*****************************************************************************
static void
USBDSCSIRequestSense(tUSBDMSCDevice *psMSCDevice)
{
    tMSCInstance *psInst;

    //
    // Get our instance data pointer.
    //
    psInst = &psMSCDevice->sPrivateData;

    //
    // The request sense response.
    //
    g_pui8Command[0] = psInst->ui8ErrorCode;
    g_pui8Command[1] = 0;
    g_pui8Command[2] = psInst->ui8SenseKey;
    *(uint32_t *)&g_pui8Command[3] = 0;

    //
    // There are 10 more bytes of data.
    //
    g_pui8Command[7] = 10;

    *(uint32_t *)&g_pui8Command[8] = 0;

    //
    // Transition from not ready to ready.
    //
    *(uint16_t *)&g_pui8Command[12] = psInst->ui16AddSenseCode;
    *(uint32_t *)&g_pui8Command[14] = 0;

    //
    // Send the SCSI Inquiry Response.
    //
    MAP_USBEndpointDataPut(USB0_BASE, psInst->ui8INEndpoint, g_pui8Command,
                           18);
    MAP_USBEndpointDataSend(USB0_BASE, psInst->ui8INEndpoint, USB_TRANS_IN);

    //
    // Reset the valid flag on errors.
    //
    psInst->ui8ErrorCode = SCSI_RS_CUR_ERRORS;

    //
    // Set the status so that it can be sent when this response has
    // has be successfully sent.
    //
    g_sSCSICSW.bCSWStatus = 0;
    g_sSCSICSW.dCSWDataResidue = 0;

    //
    // Move on to the status phase.
    //
    psInst->ui8SCSIState = STATE_SCSI_SEND_STATUS;
}

//*****************************************************************************
//
// This function is used to handle the SCSI Read 10 command when it is
// received from the host.
//
//*****************************************************************************
static void
USBDSCSIRead10(tUSBDMSCDevice *psMSCDevice, tMSCCBW *psSCSICBW)
{
    uint16_t ui16NumBlocks;
    tMSCInstance *psInst;

    //
    // Default the number of blocks.
    //
    ui16NumBlocks = 0;

    //
    // Get our instance data pointer.
    //
    psInst = &psMSCDevice->sPrivateData;

    if(psInst->pvMedia != 0)
    {
        //
        // Get the logical block from the CBW structure. This switching
        // is required to convert from big to little endian.
        //
        psInst->ui32CurrentLBA = (psSCSICBW->CBWCB[2] << 24) |
                                 (psSCSICBW->CBWCB[3] << 16) |
                                 (psSCSICBW->CBWCB[4] << 8) |
                                 (psSCSICBW->CBWCB[5] << 0);

        //
        // More bytes to read.
        //
        ui16NumBlocks = (psSCSICBW->CBWCB[7] << 8) | psSCSICBW->CBWCB[8];

        //
        // Read the next logical block from the storage device.
        //
        if(psMSCDevice->sMediaFunctions.pfnBlockRead(psInst->pvMedia,
               (uint8_t *)psInst->pui32Buffer, psInst->ui32CurrentLBA, 1) == 0)
        {
            psInst->pvMedia = 0;
            psMSCDevice->sMediaFunctions.pfnClose(0);
        }
    }

    //
    // If there is media present then start transferring the data.
    //
    if(psInst->pvMedia != 0)
    {
        //
        // Configure and DMA for the IN transfer.
        //
        USBLibDMATransfer(psInst->psDMAInstance, psInst->ui8INDMA,
                          psInst->pui32Buffer, MAX_TRANSFER_SIZE);

        //
        // Remember that a DMA is in progress.
        //
        psInst->ui32Flags |= USBD_FLAG_DMA_IN;

        //
        // Schedule the remaining bytes to send.
        //
        psInst->ui32BytesToTransfer = (DEVICE_BLOCK_SIZE * ui16NumBlocks);

        //
        // Move on and start sending blocks.
        //
        psInst->ui8SCSIState = STATE_SCSI_SEND_BLOCKS;

        if(psMSCDevice->pfnEventCallback)
        {
            psMSCDevice->pfnEventCallback(0, USBD_MSC_EVENT_READING, 0, 0);
        }
    }
    else
    {
        //
        // Set the status so that it can be sent when this response has
        // has be successfully sent.
        //
        g_sSCSICSW.bCSWStatus = 1;
        g_sSCSICSW.dCSWDataResidue = 0;

        //
        // Stall the IN endpoint
        //
        MAP_USBDevEndpointStall(USB0_BASE, psInst->ui8INEndpoint,
                                USB_EP_DEV_IN);

        //
        // Mark the sense code as valid and indicate that these is no media
        // present.
        //
        psInst->ui8ErrorCode = SCSI_RS_VALID | SCSI_RS_CUR_ERRORS;
        psInst->ui8SenseKey = SCSI_RS_KEY_NOT_READY;
        psInst->ui16AddSenseCode = SCSI_RS_MED_NOT_PRSNT;

        psInst->ui8SCSIState = STATE_SCSI_SEND_STATUS;
    }
}

//*****************************************************************************
//
// This function is used to handle the SCSI Read 10 command when it is
// received from the host.
//
//*****************************************************************************
static void
USBDSCSIWrite10(tUSBDMSCDevice *psMSCDevice, tMSCCBW *psSCSICBW)
{
    uint16_t ui16NumBlocks;
    tMSCInstance *psInst;

    //
    // Get instance data pointers.
    //
    psInst = &psMSCDevice->sPrivateData;

    //
    // If there is media present then start transferring the data.
    //
    if(psInst->pvMedia != 0)
    {
        //
        // Get the logical block from the CBW structure. This switching
        // is required to convert from big to little endian.
        //
        psInst->ui32CurrentLBA = (psSCSICBW->CBWCB[2] << 24) |
                                 (psSCSICBW->CBWCB[3] << 16) |
                                 (psSCSICBW->CBWCB[4] << 8) |
                                 (psSCSICBW->CBWCB[5] << 0);

        //
        // More bytes to read.
        //
        ui16NumBlocks = (psSCSICBW->CBWCB[7] << 8) | psSCSICBW->CBWCB[8];

        psInst->ui32BytesToTransfer = DEVICE_BLOCK_SIZE * ui16NumBlocks;

        //
        // Start sending logical blocks, these are always multiples of
        // DEVICE_BLOCK_SIZE bytes.
        //
        psInst->ui8SCSIState = STATE_SCSI_RECEIVE_BLOCKS;

        //
        // Configure and enable DMA for the OUT transfer.
        //
        USBLibDMATransfer(psInst->psDMAInstance, psInst->ui8OUTDMA,
                          psInst->pui32Buffer, MAX_TRANSFER_SIZE);

        //
        // Remember that a DMA is in progress.
        //
        psInst->ui32Flags |= USBD_FLAG_DMA_OUT;

        //
        // Notify the application of the write event.
        //
        if(psMSCDevice->pfnEventCallback)
        {
            psMSCDevice->pfnEventCallback(0, USBD_MSC_EVENT_WRITING, 0, 0);
        }
    }
    else
    {
        //
        // Set the status so that it can be sent when this response has
        // has be successfully sent.
        //
        g_sSCSICSW.bCSWStatus = 1;
        g_sSCSICSW.dCSWDataResidue = 0;

        //
        // Stall the IN endpoint
        //
        MAP_USBDevEndpointStall(USB0_BASE, psInst->ui8OUTEndpoint,
                                USB_EP_DEV_OUT);

        //
        // Mark the sense code as valid and indicate that these is no media
        // present.
        //
        psInst->ui8ErrorCode = SCSI_RS_VALID | SCSI_RS_CUR_ERRORS;
        psInst->ui8SenseKey = SCSI_RS_KEY_NOT_READY;
        psInst->ui16AddSenseCode = SCSI_RS_MED_NOT_PRSNT;

        psInst->ui8SCSIState = STATE_SCSI_SEND_STATUS;
    }
}

//*****************************************************************************
//
// This function is used to handle the SCSI Mode Sense 6 command when it is
// received from the host.
//
//*****************************************************************************
static void
USBDSCSIModeSense6(tUSBDMSCDevice *psMSCDevice, tMSCCBW *psSCSICBW)
{
    tMSCInstance *psInst;

    //
    // Get our instance data pointer.
    //
    psInst = &psMSCDevice->sPrivateData;

    //
    // If there is media present send the response.
    //
    if(psInst->pvMedia != 0)
    {
        //
        // Three extra bytes in this response.
        //
        g_pui8Command[0] = 3;
        g_pui8Command[1] = 0;
        g_pui8Command[2] = 0;
        g_pui8Command[3] = 0;

        //
        // Manually send the response back to the host.
        //
        MAP_USBEndpointDataPut(USB0_BASE, psInst->ui8INEndpoint, g_pui8Command,
                               4);
        MAP_USBEndpointDataSend(USB0_BASE, psInst->ui8INEndpoint,
                                USB_TRANS_IN);

        //
        // Set the status so that it can be sent when this response has
        // has be successfully sent.
        //
        g_sSCSICSW.bCSWStatus = 0;
        g_sSCSICSW.dCSWDataResidue = psSCSICBW->dCBWDataTransferLength - 4;
    }
    else
    {
        //
        // Set the status so that it can be sent when this response has
        // has be successfully sent.
        //
        g_sSCSICSW.bCSWStatus = 1;
        g_sSCSICSW.dCSWDataResidue = 0;

        //
        // Stall the IN endpoint
        //
        MAP_USBDevEndpointStall(USB0_BASE, psInst->ui8INEndpoint,
                                USB_EP_DEV_IN);

        //
        // Mark the sense code as valid and indicate that these is no media
        // present.
        //
        psInst->ui8ErrorCode = SCSI_RS_VALID | SCSI_RS_CUR_ERRORS;
        psInst->ui8SenseKey = SCSI_RS_KEY_NOT_READY;
        psInst->ui16AddSenseCode = SCSI_RS_MED_NOT_PRSNT;
    }

    psInst->ui8SCSIState = STATE_SCSI_SEND_STATUS;
}

//*****************************************************************************
//
// This function is used to send out the response data based on the current
// status of the mass storage class.
//
//*****************************************************************************
static void
USBDSCSISendStatus(tUSBDMSCDevice *psMSCDevice)
{
    tMSCInstance *psInst;

    //
    // Get our instance data pointer.
    //
    psInst = &psMSCDevice->sPrivateData;

    //
    // Respond with the requested status.
    //
    MAP_USBEndpointDataPut(USB0_BASE, psInst->ui8INEndpoint,
                           (uint8_t *)&g_sSCSICSW, 13);
    MAP_USBEndpointDataSend(USB0_BASE, psInst->ui8INEndpoint, USB_TRANS_IN);

    //
    // Move the state to status sent so that the next interrupt will move the
    // statue to idle.
    //
    psInst->ui8SCSIState = STATE_SCSI_SENT_STATUS;
}

//*****************************************************************************
//
// This function is used to handle all SCSI commands.
//
//*****************************************************************************
uint32_t
USBDSCSICommand(tUSBDMSCDevice *psMSCDevice, tMSCCBW *psSCSICBW)
{
    uint32_t ui32RetCode, ui32TransferLength;
    tMSCInstance *psInst;

    //
    // Get our instance data pointer.
    //
    psInst = &psMSCDevice->sPrivateData;

    //
    // Initialize the return code.
    //
    ui32RetCode = 1;

    //
    // Save the transfer length because it may be overwritten by some calls.
    //
    ui32TransferLength = psSCSICBW->dCBWDataTransferLength;

    switch(psSCSICBW->CBWCB[0])
    {
        //
        // Respond to the SCSI Inquiry command.
        //
        case SCSI_INQUIRY_CMD:
        {
            USBDSCSIInquiry(psMSCDevice);

            break;
        }

        //
        // Respond to the test unit ready command.
        //
        case SCSI_TEST_UNIT_READY:
        {
            g_sSCSICSW.dCSWDataResidue = 0;

            if(psInst->pvMedia != 0)
            {
                //
                // Set the status to success for now, this could be different
                // if there is no media present.
                //
                g_sSCSICSW.bCSWStatus = 0;
            }
            else
            {
                //
                // Since there was no media, check for media here.
                //
                psInst->pvMedia = psMSCDevice->sMediaFunctions.pfnOpen(0);

                //
                // If it is still not present then fail this command.
                //
                if(psInst->pvMedia != 0)
                {
                    g_sSCSICSW.bCSWStatus = 0;
                }
                else
                {
                    g_sSCSICSW.bCSWStatus = 1;
                }
            }
            break;
        }

        //
        // Handle the Read Capacities command.
        //
        case SCSI_READ_CAPACITIES:
        {
            USBDSCSIReadCapacities(psMSCDevice);

            break;
        }

        //
        // Handle the Read Capacity command.
        //
        case SCSI_READ_CAPACITY:
        {
            USBDSCSIReadCapacity(psMSCDevice);

            break;
        }

        //
        // Handle the Request Sense command.
        //
        case SCSI_REQUEST_SENSE:
        {
            USBDSCSIRequestSense(psMSCDevice);

            break;
        }

        //
        // Handle the Read 10 command.
        //
        case SCSI_READ_10:
        {
            USBDSCSIRead10(psMSCDevice, psSCSICBW);

            break;
        }

        //
        // Handle the Write 10 command.
        //
        case SCSI_WRITE_10:
        {
            USBDSCSIWrite10(psMSCDevice, psSCSICBW);

            break;
        }

        //
        // Handle the Mode Sense 6 command.
        //
        case SCSI_MODE_SENSE_6:
        {
            USBDSCSIModeSense6(psMSCDevice, psSCSICBW);

            break;
        }
        default:
        {
            //
            // Set the status so that it can be sent when this response has
            // has be successfully sent.
            //
            g_sSCSICSW.bCSWStatus = 1;
            g_sSCSICSW.dCSWDataResidue = psSCSICBW->dCBWDataTransferLength;

            //
            // If there is data then there is more work to do.
            //
            if(psSCSICBW->dCBWDataTransferLength != 0)
            {
                if(psSCSICBW->bmCBWFlags & CBWFLAGS_DIR_IN)
                {
                    //
                    // Stall the IN endpoint
                    //
                    MAP_USBDevEndpointStall(USB0_BASE, psInst->ui8INEndpoint,
                                            USB_EP_DEV_IN);
                }
                else
                {
                    //
                    // Stall the OUT endpoint
                    //
                    MAP_USBDevEndpointStall(USB0_BASE, psInst->ui8OUTEndpoint,
                                            USB_EP_DEV_OUT);

                }
                //
                // Send the status once the stall occurs.
                //
                psInst->ui8SCSIState = STATE_SCSI_SEND_STATUS;
            }

            //
            // Set the sense codes.
            //
            psInst->ui8ErrorCode = SCSI_RS_VALID | SCSI_RS_CUR_ERRORS;
            psInst->ui8SenseKey = SCSI_RS_KEY_ILGL_RQST;
            psInst->ui16AddSenseCode = SCSI_RS_PV_INVALID;

            break;
        }
    }

    //
    // If there is no data then send out the current status.
    //
    if(ui32TransferLength == 0)
    {
        USBDSCSISendStatus(psMSCDevice);
    }
    return(ui32RetCode);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
