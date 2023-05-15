//*****************************************************************************
//
// usbhmsc.c - USB MSC host driver.
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
#include "driverlib/usbdrv.h"
#include "usblib/usblib.h"
#include "usblib/usblibpriv.h"
#include "usblib/usbmsc.h"
#include "usblib/host/usbhost.h"
#include "usblib/host/usbhostpriv.h"
#include "usblib/host/usbhmsc.h"
#include "usblib/host/usbhscsi.h"

//*****************************************************************************
//
//! \addtogroup usblib_host_class
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Forward declarations for the driver open and close calls.
//
//*****************************************************************************
static void *USBHMSCOpen(tUSBHostDevice *psDevice);
static void USBHMSCClose(void *pvInstance);

//*****************************************************************************
//
// This is the structure for an instance of a USB MSC host driver.
//
//*****************************************************************************
struct tUSBHMSCInstance
{
    //
    // Save the device instance.
    //
    tUSBHostDevice *psDevice;

    //
    // Used to save the callback.
    //
    tUSBHMSCCallback pfnCallback;

    //
    // The Maximum LUNs
    //
    uint32_t ui32MaxLUN;

    //
    // The total number of blocks associated with this device.
    //
    uint32_t ui32NumBlocks;

    //
    // The size of the blocks associated with this device.
    //
    uint32_t ui32BlockSize;

    //
    // Bulk IN pipe.
    //
    uint32_t ui32BulkInPipe;

    //
    // Bulk OUT pipe.
    //
    uint32_t ui32BulkOutPipe;
};

//*****************************************************************************
//
// The array of USB MSC host drivers.
//
//*****************************************************************************
static tUSBHMSCInstance g_sUSBHMSCDevice =
{
    0
};

//*****************************************************************************
//
//! This constant global structure defines the Mass Storage Class Driver that
//! is provided with the USB library.
//
//*****************************************************************************
const tUSBHostClassDriver g_sUSBHostMSCClassDriver =
{
    USB_CLASS_MASS_STORAGE,
    USBHMSCOpen,
    USBHMSCClose,
    0
};

//*****************************************************************************
//
//! This function is used to open an instance of the MSC driver.
//!
//! \param psDevice is a pointer to the device information structure.
//!
//! This function will attempt to open an instance of the MSC driver based on
//! the information contained in the \e psDevice structure.  This call can fail
//! if there are not sufficient resources to open the device.  The function
//! returns a value that should be passed back into USBMSCClose() when the
//! driver is no longer needed.
//!
//! \return The function will return a pointer to a MSC driver instance.
//
//*****************************************************************************
static void *
USBHMSCOpen(tUSBHostDevice *psDevice)
{
    int32_t i32Idx;
    tEndpointDescriptor *psEndpointDescriptor;
    tInterfaceDescriptor *psInterface;

    //
    // Don't allow the device to be opened without closing first.
    //
    if(g_sUSBHMSCDevice.psDevice)
    {
        return(0);
    }

    //
    // Save the device pointer.
    //
    g_sUSBHMSCDevice.psDevice = psDevice;

    //
    // Get the interface descriptor.
    //
    psInterface = USBDescGetInterface(psDevice->psConfigDescriptor, 0, 0);

    //
    // Loop through the endpoints of the device.
    //
    for(i32Idx = 0; i32Idx < 3; i32Idx++)
    {
        //
        // Get the first endpoint descriptor.
        //
        psEndpointDescriptor =
            USBDescGetInterfaceEndpoint(psInterface, i32Idx,
                                        psDevice->ui32ConfigDescriptorSize);

        //
        // If no more endpoints then break out.
        //
        if(psEndpointDescriptor == 0)
        {
            break;
        }

        //
        // See if this is a bulk endpoint.
        //
        if((psEndpointDescriptor->bmAttributes & USB_EP_ATTR_TYPE_M) ==
           USB_EP_ATTR_BULK)
        {
            //
            // See if this is bulk IN or bulk OUT.
            //
            if(psEndpointDescriptor->bEndpointAddress & USB_EP_DESC_IN)
            {
                //
                // Allocate the USB Pipe for this Bulk IN endpoint.
                //
                g_sUSBHMSCDevice.ui32BulkInPipe =
                    USBHCDPipeAllocSize(0, USBHCD_PIPE_BULK_IN_DMA,
                                        psDevice,
                                        psEndpointDescriptor->wMaxPacketSize,
                                        0);
                //
                // Configure the USB pipe as a Bulk IN endpoint.
                //
                USBHCDPipeConfig(g_sUSBHMSCDevice.ui32BulkInPipe,
                                 psEndpointDescriptor->wMaxPacketSize,
                                 0,
                                 (psEndpointDescriptor->bEndpointAddress &
                                  USB_EP_DESC_NUM_M));
            }
            else
            {
                //
                // Allocate the USB Pipe for this Bulk OUT endpoint.
                //
                g_sUSBHMSCDevice.ui32BulkOutPipe =
                    USBHCDPipeAllocSize(0, USBHCD_PIPE_BULK_OUT_DMA,
                                        psDevice,
                                        psEndpointDescriptor->wMaxPacketSize,
                                        0);
                //
                // Configure the USB pipe as a Bulk OUT endpoint.
                //
                USBHCDPipeConfig(g_sUSBHMSCDevice.ui32BulkOutPipe,
                                 psEndpointDescriptor->wMaxPacketSize,
                                 0,
                                 (psEndpointDescriptor->bEndpointAddress &
                                  USB_EP_DESC_NUM_M));
            }
        }
    }

    //
    // If the callback exists, call it with an Open event.
    //
    if(g_sUSBHMSCDevice.pfnCallback != 0)
    {
        g_sUSBHMSCDevice.pfnCallback(&g_sUSBHMSCDevice, MSC_EVENT_OPEN, 0);
    }


    g_sUSBHMSCDevice.ui32MaxLUN = 0xffffffff;

    //
    // Return the only instance of this device.
    //
    return(&g_sUSBHMSCDevice);
}

//*****************************************************************************
//
//! This function is used to release an instance of the MSC driver.
//!
//! \param pvInstance is an instance pointer that needs to be released.
//!
//! This function will free up any resources in use by the MSC driver instance
//! that is passed in.  The \e pvInstance pointer should be a valid value that
//! was returned from a call to USBMSCOpen().
//!
//! \return None.
//
//*****************************************************************************
static void
USBHMSCClose(void *pvInstance)
{
    //
    // Do nothing if there is not a driver open.
    //
    if(g_sUSBHMSCDevice.psDevice == 0)
    {
        return;
    }

    //
    // Reset the device pointer.
    //
    g_sUSBHMSCDevice.psDevice = 0;

    //
    // Free the Bulk IN pipe.
    //
    if(g_sUSBHMSCDevice.ui32BulkInPipe != 0)
    {
        USBHCDPipeFree(g_sUSBHMSCDevice.ui32BulkInPipe);
    }

    //
    // Free the Bulk OUT pipe.
    //
    if(g_sUSBHMSCDevice.ui32BulkOutPipe != 0)
    {
        USBHCDPipeFree(g_sUSBHMSCDevice.ui32BulkOutPipe);
    }

    //
    // If the callback exists then call it.
    //
    if(g_sUSBHMSCDevice.pfnCallback != 0)
    {
        g_sUSBHMSCDevice.pfnCallback(&g_sUSBHMSCDevice, MSC_EVENT_CLOSE, 0);
    }
}

//*****************************************************************************
//
//! This function retrieves the maximum number of the logical units on a
//! mass storage device.
//!
//! \param psDevice is the device instance pointer for this request.
//! \param ui32Interface is the interface number on the device specified by the
//! \e ui32Address parameter.
//! \param pui8MaxLUN is the byte value returned from the device for the
//! device's maximum logical unit.
//!
//! The device will return one byte of data that contains the maximum LUN
//! supported by the device.  For example, if the device supports four LUNs
//! then the LUNs would be numbered from 0 to 3 and the return value would be
//! 3.  If no LUN is associated with the device, the value returned shall be 0.
//!
//! \return None.
//
//*****************************************************************************
static void
USBHMSCGetMaxLUN(tUSBHostDevice *psDevice, uint32_t ui32Interface,
                 uint8_t *pui8MaxLUN)
{
    tUSBRequest sSetupPacket;

    //
    // This is a Class specific interface IN request.
    //
    sSetupPacket.bmRequestType =
        USB_RTYPE_DIR_IN | USB_RTYPE_CLASS | USB_RTYPE_INTERFACE;

    //
    // Request a the Max LUN for this interface.
    //
    sSetupPacket.bRequest = USBREQ_GET_MAX_LUN;
    sSetupPacket.wValue = 0;

    //
    // Indicate the interface to use.
    //
    sSetupPacket.wIndex = (uint16_t)ui32Interface;

    //
    // Only request a single byte of data.
    //
    sSetupPacket.wLength = 1;

    //
    // Put the setup packet in the buffer and send the command.
    //
    if(USBHCDControlTransfer(0, &sSetupPacket, psDevice, pui8MaxLUN, 1,
                             MAX_PACKET_SIZE_EP0) != 1)
    {
        *pui8MaxLUN = 0;
    }
}

//*****************************************************************************
//
//! This function checks if a drive is ready to be accessed.
//!
//! \param psMSCInstance is the device instance to use for this read.
//!
//! This function checks if the current device is ready to be accessed.
//! It uses the \e psMSCInstance parameter to determine which device to check
//! and returns zero when the device is ready.  Any non-zero return code
//! indicates that the device was not ready.
//!
//! \return This function returns zero if the device is ready and it
//! returns a other value if the device is not ready or if an error occurred.
//
//*****************************************************************************
int32_t
USBHMSCDriveReady(tUSBHMSCInstance *psMSCInstance)
{
    uint8_t ui8MaxLUN, pui8Buffer[SCSI_INQUIRY_DATA_SZ];
    uint32_t ui32Size;

    //
    // If there is no device present then return an error.
    //
    if(psMSCInstance->psDevice == 0)
    {
        return(-1);
    }

    //
    // Only request the maximum number of LUNs once.
    //
    if(g_sUSBHMSCDevice.ui32MaxLUN == 0xffffffff)
    {
        //
        // Get the Maximum LUNs on this device.
        //
        USBHMSCGetMaxLUN(g_sUSBHMSCDevice.psDevice,
                         g_sUSBHMSCDevice.psDevice->ui32Interface, &ui8MaxLUN);

        //
        // Save the Maximum number of LUNs on this device.
        //
        g_sUSBHMSCDevice.ui32MaxLUN = ui8MaxLUN;
    }

    //
    // Just return if the device is returning not present.
    //
    ui32Size = SCSI_REQUEST_SENSE_SZ;
    if(USBHSCSIRequestSense(psMSCInstance->ui32BulkInPipe,
                            psMSCInstance->ui32BulkOutPipe, pui8Buffer,
                            &ui32Size) != SCSI_CMD_STATUS_PASS)
    {
        return(-1);
    }

    if((pui8Buffer[SCSI_RS_SKEY] == SCSI_RS_KEY_UNIT_ATTN) &&
       (pui8Buffer[SCSI_RS_SKEY_AD_SKEY] == SCSI_RS_KEY_NOTPRSNT))
    {
        return(-1);
    }

    //
    // Issue a SCSI Inquiry to get basic information on the device
    //
    ui32Size = SCSI_INQUIRY_DATA_SZ;
    if((USBHSCSIInquiry(psMSCInstance->ui32BulkInPipe,
                        psMSCInstance->ui32BulkOutPipe, pui8Buffer,
                        &ui32Size) != SCSI_CMD_STATUS_PASS))
    {
        return(-1);
    }

    //
    // Get the size of the drive.
    //
    ui32Size = SCSI_INQUIRY_DATA_SZ;
    if(USBHSCSIReadCapacity(psMSCInstance->ui32BulkInPipe,
                            psMSCInstance->ui32BulkOutPipe, pui8Buffer,
                            &ui32Size) != SCSI_CMD_STATUS_PASS)
    {
        //
        // Get the current sense data from the device to see why it failed
        // the Read Capacity command.
        //
        ui32Size = SCSI_REQUEST_SENSE_SZ;
        USBHSCSIRequestSense(psMSCInstance->ui32BulkInPipe,
                             psMSCInstance->ui32BulkOutPipe, pui8Buffer,
                             &ui32Size);

        //
        // If the read capacity failed then check if the drive is ready.
        //
        if(USBHSCSITestUnitReady(psMSCInstance->ui32BulkInPipe,
                                 psMSCInstance->ui32BulkOutPipe) !=
           SCSI_CMD_STATUS_PASS)
        {
            //
            // Get the current sense data from the device to see why it failed
            // the Test Unit Ready command.
            //
            ui32Size = SCSI_REQUEST_SENSE_SZ;
            USBHSCSIRequestSense(psMSCInstance->ui32BulkInPipe,
                                 psMSCInstance->ui32BulkOutPipe, pui8Buffer,
                                 &ui32Size);
        }

        return(-1);
    }
    else
    {
        //
        // Read the block size out, value is stored big endian.
        //
        psMSCInstance->ui32BlockSize =
             (pui8Buffer[7] | (pui8Buffer[6] << 8) | pui8Buffer[5] << 16 |
              (pui8Buffer[4] << 24));

        //
        // Read the block size out.
        //
        psMSCInstance->ui32NumBlocks =
            (pui8Buffer[3] | (pui8Buffer[2] << 8) | pui8Buffer[1] << 16 |
             (pui8Buffer[0] << 24));
    }

    //
    // See if the drive is ready to use.
    //
    if(USBHSCSITestUnitReady(psMSCInstance->ui32BulkInPipe,
                             psMSCInstance->ui32BulkOutPipe) !=
       SCSI_CMD_STATUS_PASS)
    {
        //
        // Get the current sense data from the device to see why it failed
        // the Test Unit Ready command.
        //
        ui32Size = SCSI_REQUEST_SENSE_SZ;
        USBHSCSIRequestSense(psMSCInstance->ui32BulkInPipe,
                             psMSCInstance->ui32BulkOutPipe, pui8Buffer,
                             &ui32Size);

        return(-1);
    }

    //
    // Success.
    //
    return(0);
}

//*****************************************************************************
//
//! This function should be called before any devices are present to enable
//! the mass storage device class driver.
//!
//! \param ui32Drive is the drive number to open.
//! \param pfnCallback is the driver callback for any mass storage events.
//!
//! This function is called to open an instance of a mass storage device.  It
//! should be called before any devices are connected to allow for proper
//! notification of drive connection and disconnection.  The \e ui32Drive
//! parameter is a zero based index of the drives present in the system.
//! There are a constant number of drives, and this number should only
//! be greater than 0 if there is a USB hub present in the system.  The
//! application should also provide the \e pfnCallback to be notified of mass
//! storage related events like device enumeration and device removal.
//!
//! \return This function will return the driver instance to use for the other
//! mass storage functions.  If there is no driver available at the time of
//! this call, this function will return zero.
//
//*****************************************************************************
tUSBHMSCInstance *
USBHMSCDriveOpen(uint32_t ui32Drive, tUSBHMSCCallback pfnCallback)
{
    //
    // Only the first drive is supported and only one callback is supported.
    //
    if((ui32Drive != 0) || (g_sUSBHMSCDevice.pfnCallback))
    {
        return(0);
    }

    //
    // Save the callback.
    //
    g_sUSBHMSCDevice.pfnCallback = pfnCallback;

    //
    // Return the requested device instance.
    //
    return(&g_sUSBHMSCDevice);
}

//*****************************************************************************
//
//! This function should be called to release a drive instance.
//!
//! \param psMSCInstance is the device instance that is to be released.
//!
//! This function is called when an MSC drive is to be released in preparation
//! for shutdown or a switch to USB device mode, for example.  Following this
//! call, the drive is available for other clients who may open it again using
//! a call to USBHMSCDriveOpen().
//!
//! \return None.
//
//*****************************************************************************
void
USBHMSCDriveClose(tUSBHMSCInstance *psMSCInstance)
{
    //
    // Close the drive (if it is already open)
    //
    USBHMSCClose((void *)psMSCInstance);

    //
    // Clear the callback indicating that the device is now closed.
    //
    psMSCInstance->pfnCallback = 0;
}

//*****************************************************************************
//
//! This function performs a block read to an MSC device.
//!
//! \param psMSCInstance is the device instance to use for this read.
//! \param ui32LBA is the logical block address to read on the device.
//! \param pui8Data is a pointer to the returned data buffer.
//! \param ui32NumBlocks is the number of blocks to read from the device.
//!
//! This function will perform a block sized read from the device associated
//! with the \e psMSCInstance parameter.  The \e ui32LBA parameter specifies
//! the logical block address to read on the device.  This function will only
//! perform \e ui32NumBlocks block sized reads.  In most cases this is a read
//! of 512 bytes of data.  The \e *pui8Data buffer should be at least
//! \e ui32NumBlocks * 512 bytes in size.
//!
//! \return The function returns zero for success and any negative value
//! indicates a failure.
//
//*****************************************************************************
int32_t
USBHMSCBlockRead(tUSBHMSCInstance *psMSCInstance, uint32_t ui32LBA,
                 uint8_t *pui8Data, uint32_t ui32NumBlocks)
{
    uint32_t ui32Size;

    //
    // If there is no device present then return an error.
    //
    if(psMSCInstance->psDevice == 0)
    {
        return(-1);
    }

    //
    // Calculate the actual byte size of the read.
    //
    ui32Size = psMSCInstance->ui32BlockSize * ui32NumBlocks;

    //
    // Perform the SCSI read command.
    //
    if(USBHSCSIRead10(psMSCInstance->ui32BulkInPipe,
                      psMSCInstance->ui32BulkOutPipe, ui32LBA, pui8Data,
                      &ui32Size, ui32NumBlocks) != SCSI_CMD_STATUS_PASS)
    {
        return(-1);
    }

    //
    // Success.
    //
    return(0);
}

//*****************************************************************************
//
//! This function performs a block write to an MSC device.
//!
//! \param psMSCInstance is the device instance to use for this write.
//! \param ui32LBA is the logical block address to write on the device.
//! \param pui8Data is a pointer to the data to write out.
//! \param ui32NumBlocks is the number of blocks to write to the device.
//!
//! This function will perform a block sized write to the device associated
//! with the \e psMSCInstance parameter.  The \e ui32LBA parameter specifies
//! the logical block address to write on the device.  This function will only
//! perform \e ui32NumBlocks block sized writes.  In most cases this is a write
//! of 512 bytes of data.  The \e *pui8Data buffer should contain at least
//! \e ui32NumBlocks * 512 bytes in size to prevent unwanted data being written
//! to the device.
//!
//! \return The function returns zero for success and any negative value
//! indicates a failure.
//
//*****************************************************************************
int32_t
USBHMSCBlockWrite(tUSBHMSCInstance *psMSCInstance, uint32_t ui32LBA,
                  uint8_t *pui8Data, uint32_t ui32NumBlocks)
{
    uint32_t ui32Size;

    //
    // If there is no device present then return an error.
    //
    if(psMSCInstance->psDevice == 0)
    {
        return(-1);
    }

    //
    // Calculate the actual byte size of the write.
    //
    ui32Size = psMSCInstance->ui32BlockSize * ui32NumBlocks;

    //
    // Perform the SCSI write command.
    //
    if(USBHSCSIWrite10(psMSCInstance->ui32BulkInPipe,
                       psMSCInstance->ui32BulkOutPipe, ui32LBA, pui8Data,
                       &ui32Size, ui32NumBlocks) != SCSI_CMD_STATUS_PASS)
    {
        return(-1);
    }

    //
    // Success.
    //
    return(0);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
