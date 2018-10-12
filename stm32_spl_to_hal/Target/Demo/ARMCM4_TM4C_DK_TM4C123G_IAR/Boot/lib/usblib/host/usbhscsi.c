//*****************************************************************************
//
// usbhscsi.c - USB host SCSI layer used by the USB host MSC driver.
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
#include "usblib/usbmsc.h"
#include "usblib/host/usbhost.h"
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
// This is the data verify tag passed between requests.
//
//*****************************************************************************
#define CBW_TAG_VALUE           0x54231990

//*****************************************************************************
//
//! This function is used to issue SCSI commands via USB.
//!
//! \param ui32InPipe is the USB IN pipe to use for this command.
//! \param ui32OutPipe is the USB OUT pipe to use for this command.
//! \param psSCSICmd is the SCSI command structure to send.
//! \param pui8Data is pointer to the command data to be sent.
//! \param pui32Size is the number of bytes is the number of bytes expected or
//! sent by the command.
//!
//! This internal function is used to handle SCSI commands sent by other
//! functions.  It serves as a layer between the SCSI command and the USB
//! interface being used to send the command.  The \e pSCSI parameter contains
//! the SCSI command to send.  For commands that expect data back, the
//! \e pui8Data is the buffer to store the data into and \e pui32Size is used
//! to store the amount of data to request as well as used to indicate how many
//! bytes were filled into the \e pui8Data buffer on return.  For commands that
//! are sending data, \e pui8Data is the data to be sent and \e pui32Size is
//! the number of bytes to send.
//!
//! \return This function returns the SCSI status from the command.  The value
//! will be either \b SCSI_CMD_STATUS_PASS or \b SCSI_CMD_STATUS_FAIL.
//
//*****************************************************************************
static uint32_t
USBHSCSISendCommand(uint32_t ui32InPipe, uint32_t ui32OutPipe,
                    tMSCCBW *psSCSICmd, uint8_t *pui8Data, uint32_t *pui32Size)
{
    tMSCCSW sCmdStatus;
    uint32_t ui32Bytes;

    //
    // Initialize the command status.
    //
    sCmdStatus.dCSWSignature = 0;
    sCmdStatus.dCSWTag = 0;
    sCmdStatus.bCSWStatus = SCSI_CMD_STATUS_FAIL;

    //
    // Set the CBW signature and tag.
    //
    psSCSICmd->dCBWSignature = CBW_SIGNATURE;
    psSCSICmd->dCBWTag = CBW_TAG_VALUE;

    //
    // Set the size of the data to be returned by the device.
    //
    psSCSICmd->dCBWDataTransferLength = *pui32Size;

    //
    // Send the command.
    //
    ui32Bytes = USBHCDPipeWrite(ui32OutPipe, (uint8_t*)psSCSICmd,
                                sizeof(tMSCCBW));

    //
    // If no bytes went out then the command failed.
    //
    if(ui32Bytes == 0)
    {
        return(SCSI_CMD_STATUS_FAIL);
    }

    //
    // Only request data if there is data to request.
    //
    if(psSCSICmd->dCBWDataTransferLength != 0)
    {
        //
        // See if this is a read or a write.
        //
        if(psSCSICmd->bmCBWFlags & CBWFLAGS_DIR_IN)
        {
            //
            // Read the data back.
            //
            *pui32Size = USBHCDPipeRead(ui32InPipe, pui8Data, *pui32Size);
        }
        else
        {
            //
            // Write the data out.
            //
            *pui32Size = USBHCDPipeWrite(ui32OutPipe, pui8Data, *pui32Size);
        }
    }

    //
    // Get the status of the command.
    //
    ui32Bytes = USBHCDPipeRead(ui32InPipe, (uint8_t *)&sCmdStatus,
                               sizeof(tMSCCSW));


    //
    // If the status was invalid or did not have the correct signature then
    // indicate a failure.
    //
    if((ui32Bytes == 0) || (sCmdStatus.dCSWSignature != CSW_SIGNATURE) ||
       (sCmdStatus.dCSWTag != CBW_TAG_VALUE))
    {
        return(SCSI_CMD_STATUS_FAIL);
    }

    //
    // Return the status.
    //
    return((uint32_t)sCmdStatus.bCSWStatus);
}

//*****************************************************************************
//
//! This will issue the SCSI inquiry command to a device.
//!
//! \param ui32InPipe is the USB IN pipe to use for this command.
//! \param ui32OutPipe is the USB OUT pipe to use for this command.
//! \param pui8Data is the data buffer to return the results into.
//! \param pui32Size is the size of buffer that was passed in on entry and the
//! number of bytes returned.
//!
//! This function should be used to issue a SCSI Inquiry command to a mass
//! storage device.  To allow for multiple devices, the \e ui32InPipe and
//! \e ui32OutPipe parameters indicate which USB pipes to use for this call.
//!
//! \note The \e pui8Data buffer pointer should have at least
//! \b SCSI_INQUIRY_DATA_SZ bytes of data or this function will overflow the
//! buffer.
//!
//! \return This function returns the SCSI status from the command.  The value
//! will be either \b SCSI_CMD_STATUS_PASS or \b SCSI_CMD_STATUS_FAIL.
//
//*****************************************************************************
uint32_t
USBHSCSIInquiry(uint32_t ui32InPipe, uint32_t ui32OutPipe,
                uint8_t *pui8Data, uint32_t *pui32Size)
{
    tMSCCBW sSCSICmd;
    uint32_t *pui32Data;

    //
    // Create a local 32-bit pointer to the command.
    //
    pui32Data = (uint32_t *)sSCSICmd.CBWCB;

    //
    // The number of bytes of data that the host expects to transfer on the
    // Bulk-In or Bulk-Out endpoint (as indicated by the Direction bit) during
    // the execution of this command.  If this field is zero, the device and
    // the host shall transfer no data between the CBW and the associated CSW,
    // and the device shall ignore the value of the Direction bit in
    // bmCBWFlags.
    //
    *pui32Size = SCSI_INQUIRY_DATA_SZ;

    //
    // This is an IN request.
    //
    sSCSICmd.bmCBWFlags = CBWFLAGS_DIR_IN;

    //
    // Only handle LUN 0.
    //
    sSCSICmd.bCBWLUN = 0;

    //
    // This is the length of the command itself.
    //
    sSCSICmd.bCBWCBLength = 6;

    //
    // Send Inquiry command with no request for vital product data.
    //
    pui32Data[0] = SCSI_INQUIRY_CMD;

    //
    // Allocation length.
    //
    pui32Data[1] = SCSI_INQUIRY_DATA_SZ;
    pui32Data[2] = 0;
    pui32Data[3] = 0;

    //
    // Send the command and get the results.
    //
    return(USBHSCSISendCommand(ui32InPipe, ui32OutPipe, &sSCSICmd, pui8Data,
                               pui32Size));
}

//*****************************************************************************
//
//! This will issue the SCSI read capacity command to a device.
//!
//! \param ui32InPipe is the USB IN pipe to use for this command.
//! \param ui32OutPipe is the USB OUT pipe to use for this command.
//! \param pui8Data is the data buffer to return the results into.
//! \param pui32Size is the size of buffer that was passed in on entry and the
//! number of bytes returned.
//!
//! This function should be used to issue a SCSI Read Capacity command
//! to a mass storage device that is connected.  To allow for multiple devices,
//! the \e ui32InPipe and \e ui32OutPipe parameters indicate which USB pipes to
//! use for this call.
//!
//! \note The \e pui8Data buffer pointer should have at least
//! \b SCSI_READ_CAPACITY_SZ bytes of data or this function will overflow the
//! buffer.
//!
//! \return This function returns the SCSI status from the command.  The value
//! will be either \b SCSI_CMD_STATUS_PASS or \b SCSI_CMD_STATUS_FAIL.
//
//*****************************************************************************
uint32_t
USBHSCSIReadCapacity(uint32_t ui32InPipe, uint32_t ui32OutPipe,
                     uint8_t *pui8Data, uint32_t *pui32Size)
{
    tMSCCBW sSCSICmd;
    uint32_t *pui32Data;

    //
    // Create a local 32-bit pointer to the command.
    //
    pui32Data = (uint32_t *)sSCSICmd.CBWCB;

    //
    // Set the size of the command data.
    //
    *pui32Size = SCSI_READ_CAPACITY_SZ;

    //
    // This is an IN request.
    //
    sSCSICmd.bmCBWFlags = CBWFLAGS_DIR_IN;

    //
    // Only handle LUN 0.
    //
    sSCSICmd.bCBWLUN = 0;

    //
    // Set the length of the command itself.
    //
    sSCSICmd.bCBWCBLength = 12;

    //
    // Only use the first byte and set it to the Read Capacity command.  The
    // rest are set to 0.
    //
    pui32Data[0] = SCSI_READ_CAPACITY;
    pui32Data[1] = 0;
    pui32Data[2] = 0;
    pui32Data[3] = 0;

    //
    // Send the command and get the results.
    //
    return(USBHSCSISendCommand(ui32InPipe, ui32OutPipe, &sSCSICmd, pui8Data,
                               pui32Size));
}

//*****************************************************************************
//
//! This will issue the SCSI read capacities command to a device.
//!
//! \param ui32InPipe is the USB IN pipe to use for this command.
//! \param ui32OutPipe is the USB OUT pipe to use for this command.
//! \param pui8Data is the data buffer to return the results into.
//! \param pui32Size is the size of buffer that was passed in on entry and the
//! number of bytes returned.
//!
//! This function should be used to issue a SCSI Read Capacities command
//! to a mass storage device that is connected.  To allow for multiple devices,
//! the \e ui32InPipe and \e ui32OutPipe parameters indicate which USB pipes to
//! use for this call.
//!
//! \return This function returns the SCSI status from the command.  The value
//! will be either \b SCSI_CMD_STATUS_PASS or \b SCSI_CMD_STATUS_FAIL.
//
//*****************************************************************************
uint32_t
USBHSCSIReadCapacities(uint32_t ui32InPipe, uint32_t ui32OutPipe,
                       uint8_t *pui8Data, uint32_t *pui32Size)
{
    tMSCCBW sSCSICmd;
    uint32_t *pui32Data;

    //
    // Create a local 32-bit pointer to the command.
    //
    pui32Data = (uint32_t *)sSCSICmd.CBWCB;

    //
    // This is an IN request.
    //
    sSCSICmd.bmCBWFlags = CBWFLAGS_DIR_IN;

    //
    // Only handle LUN 0.
    //
    sSCSICmd.bCBWLUN = 0;

    //
    // Set the length of the command itself.
    //
    sSCSICmd.bCBWCBLength = 12;

    //
    // Only use the first byte and set it to the Read Capacity command.  The
    // rest are set to 0.
    //
    pui32Data[0] = SCSI_READ_CAPACITIES;
    pui32Data[1] = 0;
    pui32Data[2] = 0;
    pui32Data[3] = 0;

    //
    // Send the command and get the results.
    //
    return(USBHSCSISendCommand(ui32InPipe, ui32OutPipe, &sSCSICmd, pui8Data,
                               pui32Size));
}

//*****************************************************************************
//
//! This will issue the SCSI Mode Sense(6) command to a device.
//!
//! \param ui32InPipe is the USB IN pipe to use for this command.
//! \param ui32OutPipe is the USB OUT pipe to use for this command.
//! \param ui32Flags is a combination of flags defining the exact query that is
//! to be made.
//! \param pui8Data is the data buffer to return the results into.
//! \param pui32Size is the size of the buffer on entry and number of bytes
//! read on exit.
//!
//! This function should be used to issue a SCSI Mode Sense(6) command
//! to a mass storage device.  To allow for multiple devices,the \e ui32InPipe
//! and \e ui32OutPipe parameters indicate which USB pipes to use for this
//! call. The call will return at most the number of bytes in the \e pui32Size
//! parameter, however it can return less and change the \e pui32Size parameter
//! to the number of valid bytes in the \e *pui32Size buffer.
//!
//! The \e ui32Flags parameter is a combination of the following three sets of
//! definitions:
//!
//! One of the following values must be specified:
//!
//! - \b SCSI_MS_PC_CURRENT request for current settings.
//! - \b SCSI_MS_PC_CHANGEABLE request for changeable settings.
//! - \b SCSI_MS_PC_DEFAULT request for default settings.
//! - \b SCSI_MS_PC_SAVED request for the saved values.
//!
//! One of these following values must also be specified to determine the page
//! code for the request:
//!
//! - \b SCSI_MS_PC_VENDOR is the vendor specific page code.
//! - \b SCSI_MS_PC_DISCO is the disconnect/reconnect page code.
//! - \b SCSI_MS_PC_CONTROL is the control page code.
//! - \b SCSI_MS_PC_LUN is the protocol specific LUN page code.
//! - \b SCSI_MS_PC_PORT is the protocol specific port page code.
//! - \b SCSI_MS_PC_POWER is the power condition page code.
//! - \b SCSI_MS_PC_INFORM is the informational exceptions page code.
//! - \b SCSI_MS_PC_ALL will request all pages codes supported by the device.
//!
//! The last value is optional and supports the following global flag:
//! - \b SCSI_MS_DBD disables returning block descriptors.
//!
//! Example: Request for all current settings.
//!
//! \verbatim
//! SCSIModeSense6(ui32InPipe, ui32OutPipe,
//!                SCSI_MS_PC_CURRENT | SCSI_MS_PC_ALL,
//!                pui8Data, pui32Size);
//! \endverbatim
//!
//! \return This function returns the SCSI status from the command.  The value
//! will be either \b SCSI_CMD_STATUS_PASS or \b SCSI_CMD_STATUS_FAIL.
//
//*****************************************************************************
uint32_t
USBHSCSIModeSense6(uint32_t ui32InPipe, uint32_t ui32OutPipe,
                   uint32_t ui32Flags, uint8_t *pui8Data,
                   uint32_t *pui32Size)
{
    tMSCCBW sSCSICmd;
    uint32_t *pui32Data;

    //
    // Create a local 32-bit pointer to the command.
    //
    pui32Data = (uint32_t *)sSCSICmd.CBWCB;

    //
    // This is an IN request.
    //
    sSCSICmd.bmCBWFlags = CBWFLAGS_DIR_IN;

    //
    // Only handle LUN 0.
    //
    sSCSICmd.bCBWLUN = 0;

    //
    // Set the size of the command data.
    //
    sSCSICmd.bCBWCBLength = 6;

    //
    // Set the options for the Mode Sense Command (6).
    //
    pui32Data[0] = (SCSI_MODE_SENSE_6 | ui32Flags);
    pui32Data[1] = (uint8_t)*pui32Size;
    pui32Data[2] = 0;
    pui32Data[3] = 0;

    //
    // Send the command and get the results.
    //
    return(USBHSCSISendCommand(ui32InPipe, ui32OutPipe, &sSCSICmd, pui8Data,
                               pui32Size));
}

//*****************************************************************************
//
//! This function issues a SCSI Test Unit Ready command to a device.
//!
//! \param ui32InPipe is the USB IN pipe to use for this command.
//! \param ui32OutPipe is the USB OUT pipe to use for this command.
//!
//! This function is used to issue a SCSI Test Unit Ready command to a device.
//! This call will simply return the results of issuing this command.
//!
//! \return This function returns the results of the SCSI Test Unit Ready
//! command.  The value will be either \b SCSI_CMD_STATUS_PASS or
//! \b SCSI_CMD_STATUS_FAIL.
//
//*****************************************************************************
uint32_t
USBHSCSITestUnitReady(uint32_t ui32InPipe, uint32_t ui32OutPipe)
{
    tMSCCBW sSCSICmd;
    uint32_t ui32Size;
    uint32_t *pui32Data;

    //
    // Create a local 32-bit pointer to the command.
    //
    pui32Data = (uint32_t *)sSCSICmd.CBWCB;

    //
    // No data in this command.
    //
    ui32Size = 0;

    //
    // This is an IN request.
    //
    sSCSICmd.bmCBWFlags = CBWFLAGS_DIR_IN;

    //
    // Only handle LUN 0.
    //
    sSCSICmd.bCBWLUN = 0;

    //
    // Set the size of the command data.
    //
    sSCSICmd.bCBWCBLength = 6;

    //
    // Set the parameter options.
    //
    pui32Data[0] = SCSI_TEST_UNIT_READY;
    pui32Data[1] = 0;
    pui32Data[2] = 0;
    pui32Data[3] = 0;

    //
    // Send the command and get the results.
    //
    return(USBHSCSISendCommand(ui32InPipe, ui32OutPipe, &sSCSICmd, 0,
                               &ui32Size));
}

//*****************************************************************************
//
//! This function issues a SCSI Request Sense command to a device.
//!
//! \param ui32InPipe is the USB IN pipe to use for this command.
//! \param ui32OutPipe is the USB OUT pipe to use for this command.
//! \param pui8Data is the data buffer to return the results into.
//! \param pui32Size is the size of the buffer on entry and number of bytes
//! read on exit.
//!
//! This function is used to issue a SCSI Request Sense command to a device.
//! It will return the data in the buffer pointed to by \e pui8Data.  The
//! parameter \e pui32Size should have the allocation size in bytes of the
//! buffer pointed to by \e pui8Data.
//!
//! \return This function returns the results of the SCSI Request Sense
//! command.  The value will be either \b SCSI_CMD_STATUS_PASS or
//! \b SCSI_CMD_STATUS_FAIL.
//
//*****************************************************************************
uint32_t
USBHSCSIRequestSense(uint32_t ui32InPipe, uint32_t ui32OutPipe,
                     uint8_t *pui8Data, uint32_t *pui32Size)
{
    tMSCCBW sSCSICmd;
    uint32_t *pui32Data;

    //
    // Create a local 32-bit pointer to the command.
    //
    pui32Data = (uint32_t *)sSCSICmd.CBWCB;

    //
    // This is an IN request.
    //
    sSCSICmd.bmCBWFlags = CBWFLAGS_DIR_IN;

    //
    // Only handle LUN 0.
    //
    sSCSICmd.bCBWLUN = 0;

    //
    // Set the size of the command data.
    //
    sSCSICmd.bCBWCBLength = 12;

    //
    // Set the parameter options.
    //
    pui32Data[0] = SCSI_REQUEST_SENSE;
    pui32Data[1] = 18;
    pui32Data[2] = 0;
    pui32Data[3] = 0;

    //
    // Send the command and get the results.
    //
    return(USBHSCSISendCommand(ui32InPipe, ui32OutPipe, &sSCSICmd, pui8Data,
                               pui32Size));
}

//*****************************************************************************
//
//! This function issues a SCSI Read(10) command to a device.
//!
//! \param ui32InPipe is the USB IN pipe to use for this command.
//! \param ui32OutPipe is the USB OUT pipe to use for this command.
//! \param ui32LBA is the logical block address to read.
//! \param pui8Data is the data buffer to return the data.
//! \param pui32Size is the size of the buffer on entry and number of bytes
//! read on exit.
//! \param ui32NumBlocks is the number of contiguous blocks to read from the
//! device.
//!
//! This function is used to issue a SCSI Read(10) command to a device.  The
//! \e ui32LBA parameter specifies the logical block address to read from the
//! device.  The data from this block will be returned in the buffer pointed to
//! by \e pui8Data.  The parameter \e pui32Size should indicate enough space to
//! hold a full block size, or only the first \e pui32Size bytes of the LBA are
//! returned.
//!
//! \return This function returns the results of the SCSI Read(10) command.
//! The value will be either \b SCSI_CMD_STATUS_PASS or
//! \b SCSI_CMD_STATUS_FAIL.
//
//*****************************************************************************
uint32_t
USBHSCSIRead10(uint32_t ui32InPipe, uint32_t ui32OutPipe,
               uint32_t ui32LBA, uint8_t *pui8Data,
               uint32_t *pui32Size, uint32_t ui32NumBlocks)
{
    tMSCCBW sSCSICmd;

    //
    // This is an IN request.
    //
    sSCSICmd.bmCBWFlags = CBWFLAGS_DIR_IN;

    //
    // Only handle LUN 0.
    //
    sSCSICmd.bCBWLUN = 0;

    //
    // Set the size of the command data.
    //
    sSCSICmd.bCBWCBLength = 10;

    //
    // Set the parameter options.
    //
    sSCSICmd.CBWCB[0] = SCSI_READ_10;

    //
    // Clear the reserved field.
    //
    sSCSICmd.CBWCB[1] = 0;

    //
    // LBA starts at offset 2.
    //
    sSCSICmd.CBWCB[2] = (uint8_t)(ui32LBA >> 24);
    sSCSICmd.CBWCB[3] = (uint8_t)(ui32LBA >> 16);
    sSCSICmd.CBWCB[4] = (uint8_t)(ui32LBA >> 8);
    sSCSICmd.CBWCB[5] = (uint8_t)ui32LBA;

    //
    // Clear the reserved field.
    //
    sSCSICmd.CBWCB[6] = 0;

    //
    // Transfer length in blocks starts at offset 2.
    // This also sets the Control value to 0 at offset 9.
    //
    sSCSICmd.CBWCB[7] = (ui32NumBlocks & 0xFF00) >> 8;
    *((uint32_t *)&sSCSICmd.CBWCB[8]) = (ui32NumBlocks & 0xFF);
    *((uint32_t *)&sSCSICmd.CBWCB[12]) = 0;

    //
    // Send the command and get the results.
    //
    return(USBHSCSISendCommand(ui32InPipe, ui32OutPipe, &sSCSICmd, pui8Data,
                               pui32Size));
}

//*****************************************************************************
//
//! This function issues a SCSI Write(10) command to a device.
//!
//! This function is used to issue a SCSI Write(10) command to a device.  The
//! \e ui32LBA parameter specifies the logical block address on the device.
//! The data to write to this block should be in the buffer pointed to by
//! \e pui8Data parameter.  The parameter \e pui32Size should indicate the
//! amount of data to write to the specified LBA.
//!
//! \param ui32InPipe is the USB IN pipe to use for this command.
//! \param ui32OutPipe is the USB OUT pipe to use for this command.
//! \param ui32LBA is the logical block address to read.
//! \param pui8Data is the data buffer to write out.
//! \param pui32Size is the size of the buffer.
//! \param ui32NumBlocks is the number of contiguous blocks to write to the
//! device.
//!
//! \return This function returns the results of the SCSI Write(10) command.
//! The value will be either \b SCSI_CMD_STATUS_PASS or
//! \b SCSI_CMD_STATUS_FAIL.
//
//*****************************************************************************
uint32_t
USBHSCSIWrite10(uint32_t ui32InPipe, uint32_t ui32OutPipe,
                uint32_t ui32LBA, uint8_t *pui8Data,
                uint32_t *pui32Size, uint32_t ui32NumBlocks)
{
    tMSCCBW sSCSICmd;
    uint32_t *pui32Data;

    //
    // Create a local 32-bit pointer to the command.
    //
    pui32Data = (uint32_t *)sSCSICmd.CBWCB;

    //
    // This is an IN request.
    //
    sSCSICmd.bmCBWFlags = CBWFLAGS_DIR_OUT;

    //
    // Only handle LUN 0.
    //
    sSCSICmd.bCBWLUN = 0;

    //
    // Set the size of the command data.
    //
    sSCSICmd.bCBWCBLength = 10;

    //
    // Set the parameter options.
    //
    sSCSICmd.CBWCB[0] = SCSI_WRITE_10;

    //
    // Clear the reserved field.
    //
    sSCSICmd.CBWCB[1] = 0;

    //
    // LBA starts at offset 2.
    //
    sSCSICmd.CBWCB[2] = (uint8_t)(ui32LBA >> 24);
    sSCSICmd.CBWCB[3] = (uint8_t)(ui32LBA >> 16);
    sSCSICmd.CBWCB[4] = (uint8_t)(ui32LBA >> 8);
    sSCSICmd.CBWCB[5] = (uint8_t)ui32LBA;

    //
    // Clear the reserved field.
    //
    sSCSICmd.CBWCB[6] = 0;

    //
    // Set the transfer length in blocks.
    // This also sets the Control value to 0 at offset 9.
    //
    sSCSICmd.CBWCB[7] = (ui32NumBlocks & 0xFF00) >> 8;

    //
    // The blocks go into is byte offset 8 or word address 2.
    //
    pui32Data[2] = (ui32NumBlocks & 0xFF);

    //
    // The blocks go into is byte offset 12 or word address 3.
    //
    pui32Data[3] = 0;

    //
    // Send the command and get the results.
    //
    return(USBHSCSISendCommand(ui32InPipe, ui32OutPipe, &sSCSICmd, pui8Data,
                               pui32Size));
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
