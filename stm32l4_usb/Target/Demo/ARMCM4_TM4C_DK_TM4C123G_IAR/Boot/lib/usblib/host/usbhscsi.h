//*****************************************************************************
//
// usbhscsi.h - Definitions for the USB host SCSI layer.
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

#ifndef __USBHSCSI_H__
#define __USBHSCSI_H__

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
//! \addtogroup usblib_host_class
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Prototypes for the APIs exported by the USB SCSI layer.
//
//*****************************************************************************
extern uint32_t USBHSCSIInquiry(uint32_t ui32InPipe, uint32_t ui32OutPipe,
                                uint8_t *pui8Buffer, uint32_t *pui32Size);
extern uint32_t USBHSCSIReadCapacity(uint32_t ui32InPipe, uint32_t ui32OutPipe,
                                     uint8_t *pui8Data, uint32_t *pui32Size);
extern uint32_t USBHSCSIReadCapacities(uint32_t ui32InPipe,
                                       uint32_t ui32OutPipe, uint8_t *pui8Data,
                                       uint32_t *pui32Size);
extern uint32_t USBHSCSIModeSense6(uint32_t ui32InPipe, uint32_t ui32OutPipe,
                                   uint32_t ui32Flags, uint8_t *pui8Data,
                                   uint32_t *pui32Size);
extern uint32_t USBHSCSITestUnitReady(uint32_t ui32InPipe,
                                      uint32_t ui32OutPipe);
extern uint32_t USBHSCSIRequestSense(uint32_t ui32InPipe, uint32_t ui32OutPipe,
                                     uint8_t *pui8Data, uint32_t *pui32Size);
extern uint32_t USBHSCSIRead10(uint32_t ui32InPipe, uint32_t ui32OutPipe,
                               uint32_t ui32LBA, uint8_t *pui8Data,
                               uint32_t *pui32Size, uint32_t ui32NumBlocks);
extern uint32_t USBHSCSIWrite10(uint32_t ui32InPipe, uint32_t ui32OutPipe,
                                uint32_t ui32LBA, uint8_t *pui8Data,
                                uint32_t *pui32Size, uint32_t ui32NumBlocks);

//*****************************************************************************
//
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

#endif // __USBHSCSI_H__
