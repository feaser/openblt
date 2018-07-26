//*****************************************************************************
//
// usblibpriv.h - Private header file used to share internal variables and
//                function prototypes between the various modules in the USB
//                library.  This header MUST NOT be used by application code.
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

#ifndef __USBLIBPRIV_H__
#define __USBLIBPRIV_H__

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
//! \addtogroup usblib_dma_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Internal interrupt handlers called from the main vectors in device and
// host mode.
//
//*****************************************************************************
extern void USBDeviceIntHandlerInternal(uint32_t ui32Index,
                                        uint32_t ui32Status);
extern void USBHostIntHandlerInternal(uint32_t ui32Index, uint32_t ui32Status);

//*****************************************************************************
//
// The maximum number of tick handlers that can be registered in a system.
//
//*****************************************************************************
#define MAX_USB_TICK_HANDLERS       6

//*****************************************************************************
//
// This value defines the number of SOF ticks that must pass before a call
// is made to InternalUSBStartOfFrameTick.  The value 5 ensures that the
// function is called every 5 milliseconds assuming that SOF interrupts are
// enabled and SOF is present.
//
//*****************************************************************************
#define USB_SOF_TICK_DIVIDE 5

//*****************************************************************************
//
// Tick handler function pointer type.
//
//*****************************************************************************
typedef void(* tUSBTickHandler)(void *pvInstance, uint32_t ui32TicksmS);

//*****************************************************************************
//
// Internal functions use to initialize the tick handler and register tick
// callbacks.
//
//*****************************************************************************
extern void InternalUSBTickInit(void);
extern void InternalUSBTickReset(void);
extern int32_t InternalUSBRegisterTickHandler(tUSBTickHandler pfnHandler,
                                              void *pvInstance);
extern void InternalUSBStartOfFrameTick(uint32_t ui32TicksmS);
extern void InternalUSBHCDSendEvent(uint32_t ui32Index, tEventInfo *psEvent,
                                    uint32_t ui32EvFlag);

//*****************************************************************************
//
// g_ui32CurrentUSBTick holds the elapsed time in milliseconds since the
// tick module was first initialized based on calls to the function
// InternalUSBStartOfFrameTick.  The granularity is USB_SOF_TICK_DIVIDE
// milliseconds.
//
//*****************************************************************************
extern uint32_t g_ui32CurrentUSBTick;

//*****************************************************************************
//
// g_ui32USBSOFCount is a global counter for Start of Frame interrupts.  It is
// incremented by the low level device- or host-mode interrupt handlers.
//
//*****************************************************************************
extern uint32_t g_ui32USBSOFCount;

//*****************************************************************************
//
// InternalUSBGetTime is a macro which will return the system time in
// milliseconds as calculated based on calls to the function
// InternalUSBStartOfFrameTick.  The granularity is USB_SOF_TICK_DIVIDE
// milliseconds.
//
// Currently, this merely returns the value of a global variable.
//
//*****************************************************************************
#define InternalUSBGetTime()    g_ui32CurrentUSBTick

//*****************************************************************************
//
// Macros to convert between USB controller base address and an index.  These
// are currently trivial but are included to allow for the possibility of
// supporting more than one controller in the future.
//
//*****************************************************************************
#define USBBaseToIndex(BaseAddr)(0)
#define USBIndexToBase(Index)   (USB0_BASE)

//
// Maximum number of channels for Type 0 USB controllers.
//
#define USB_MAX_DMA_CHANNELS_0  6

//
// Maximum number of channels for all other USB controllers.
//
#define USB_MAX_DMA_CHANNELS    8

//*****************************************************************************
//
// Values returned by the USBLibDMAChannelStatus() function.
//
//*****************************************************************************
#define USBLIBSTATUS_DMA_IDLE   0x00000000
#define USBLIBSTATUS_DMA_COMPLETE                                             \
                                0x00000001
#define USBLIBSTATUS_DMA_ERROR  0x00000002
#define USBLIBSTATUS_DMA_PENDING                                              \
                                0x00000004

//*****************************************************************************
//
// DMA endpoint types used with the USBLibDMAChannelAllocate() function.
//
//*****************************************************************************
#define USB_DMA_EP_RX           0x00000080
#define USB_DMA_EP_TX           0x00000000
#define USB_DMA_EP_HOST         0x00000040
#define USB_DMA_EP_DEVICE       0x00000000
#define USB_DMA_EP_TYPE_CTRL    0x00000000
#define USB_DMA_EP_TYPE_ISOC    0x00000001
#define USB_DMA_EP_TYPE_BULK    0x00000002
#define USB_DMA_EP_TYPE_INT     0x00000003
#define USB_DMA_EP_TYPE_M       0x00000003

//*****************************************************************************
//
// This is the internal instance data for the DMA functions and should not
// be modified outside the usbdma.c file.
//
//*****************************************************************************
struct tUSBDMAInstance
{
    uint32_t ui32Base;

    uint32_t ui32IntNum;

    uint32_t pui32Config[USB_MAX_DMA_CHANNELS];

    uint32_t pui32MaxPacketSize[USB_MAX_DMA_CHANNELS];

    uint32_t *ppui32Data[USB_MAX_DMA_CHANNELS];

    uint32_t pui32Count[USB_MAX_DMA_CHANNELS];

    uint8_t pui8Endpoint[USB_MAX_DMA_CHANNELS];

    uint32_t pui32EPDMAMode0[USB_MAX_DMA_CHANNELS];

    uint32_t pui32EPDMAMode1[USB_MAX_DMA_CHANNELS];

    uint32_t ui32Pending;

    uint32_t ui32Complete;

    void (* pfnArbSizeSet)(tUSBDMAInstance *psUSBDMAInst, uint32_t ui32Channel,
                           uint32_t ui32ArbSize);
    uint32_t (* pfnChannelAllocate)(tUSBDMAInstance *psUSBDMAInst,
                                    uint8_t ui8Endpoint,
                                    uint32_t ui32MaxPacketSize,
                                    uint32_t ui32Config);
    void (* pfnChannelEnable)(tUSBDMAInstance *psUSBDMAInst,
                              uint32_t ui32Channel);
    void (* pfnChannelDisable)(tUSBDMAInstance *psUSBDMAInst,
                               uint32_t ui32Channel);
    void (* pfnChannelRelease)(tUSBDMAInstance *psUSBDMAInst,
                               uint8_t ui8Endpoint);
    uint32_t (* pfnChannelStatus)(tUSBDMAInstance *psUSBDMAInst,
                                  uint32_t ui32Channel);
    void (* pfnChannelIntDisable)(tUSBDMAInstance *psUSBDMAInst,
                                  uint32_t ui32Channel);
    void (* pfnChannelIntEnable)(tUSBDMAInstance *psUSBDMAInst,
                                 uint32_t ui32Channel);
    void (* pfnIntHandler)(tUSBDMAInstance *psUSBDMAInst,
                           uint32_t ui32Status);
    uint32_t (* pfnIntStatus)(tUSBDMAInstance *psUSBDMAInst);
    void (* pfnIntStatusClear)(tUSBDMAInstance *psUSBDMAInst,
                               uint32_t ui32Status);
    uint32_t (* pfnStatus)(tUSBDMAInstance *psUSBDMAInst);
    uint32_t (* pfnTransfer)(tUSBDMAInstance *psUSBDMAInst,
                             uint32_t ui32Channel, void *pvBuffer,
                             uint32_t ui32Size);
    void (* pfnUnitSizeSet)(tUSBDMAInstance *psUSBDMAInst,
                            uint32_t ui32Channel,
                            uint32_t ui32BitSize);
};

//*****************************************************************************
//
// These are the USB libraries DMA functions.
//
//*****************************************************************************
extern tUSBDMAInstance * USBLibDMAInit(uint32_t ui32Index);

//*****************************************************************************
//
//! This function returns the current DMA status for a given DMA channel.
//!
//! \param psUSBDMAInst is the DMA structure pointer for this instance.
//! \param ui32Channel is the DMA channel number used to retrieve the DMA
//! status.
//!
//! This function returns the current status of a DMA transfer on a given
//! DMA channel.  The DMA channel is specified by the \e ui32Channel parameter.
//!
//! \return This function returns one of the \b USBLIBSTATUS_DMA_* values.
//
//*****************************************************************************
#define USBLibDMAChannelStatus(psUSBDMAInst, ui32Channel)                     \
          psUSBDMAInst->pfnChannelStatus(psUSBDMAInst, ui32Channel)

//*****************************************************************************
//
//! This function is used to return any global status information for USB DMA.
//!
//! \param psUSBDMAInst is a generic instance pointer that can be used to
//! distinguish between different hardware instances.
//!
//! This function performs returns the global status for the USB DMA
//! interface.
//!
//! \return Always returns 0.
//
//*****************************************************************************
#define USBLibDMAStatus(psUSBDMAInst)    psUSBDMAInst->pfnStatus(psUSBDMAInst)

//*****************************************************************************
//
//! This function returns the current DMA interrupt status.
//!
//! \param psUSBDMAInst is the DMA structure pointer for this instance.
//!
//! This function returns the interrupt status for all DMA channels.  The value
//! returned is a per channel interrupt mapping with the DMA channels mapped
//! into bits 0-31 by channel number with channel 1 starting at bit 0.
//!
//! \note This function does not return an endpoint interrupt status, but the
//! interrupt status for the DMA interface used with the USB controller.
//!
//! \return This function returns the pending DMA interrupts.
//
//*****************************************************************************
#define USBLibDMAIntStatus(psUSBDMAInst)                                      \
                psUSBDMAInst->pfnIntStatus(psUSBDMAInst)

//*****************************************************************************
//
//! This function clears the requested DMA interrupt status.
//!
//! \param psUSBDMAInst is the DMA structure pointer for this instance.
//! \param ui32Status contains the interrupts to clear.
//!
//! This function clears the current DMA interrupt status for the
//! controller specified by the \e ui32Instance parameter.  The \e ui32Status
//! value has the same format as the value returned from the
//! USBLibDMAIntStatus() function which is a per channel interrupt mapping.
//! The DMA channels are mapped into bits 0-31 by channel number with channel 1
//!starting at bit 0.
//!
//! \return None.
//
//*****************************************************************************
#define USBLibDMAIntStatusClear(psUSBDMAInst, ui32Status)                     \
          psUSBDMAInst->pfnIntStatusClear(psUSBDMAInst, ui32Status)

//*****************************************************************************
//
//! This function enables DMA for a given channel.
//!
//! \param psUSBDMAInst is the DMA structure pointer for this instance.
//! \param ui32Channel is the DMA channel to enable.
//!
//! This function enables DMA on the channel number passed in the
//! \e ui32Channel parameter.
//!
//! \return None.
//
//*****************************************************************************
#define USBLibDMAChannelEnable(psUSBDMAInst, ui32Channel)                     \
          psUSBDMAInst->pfnChannelEnable(psUSBDMAInst, ui32Channel)

//*****************************************************************************
//
//! This function disables DMA for a given DMA channel.
//!
//! \param psUSBDMAInst is the DMA structure pointer for this instance.
//! \param ui32Channel is the DMA channel to disable.
//!
//! This function disables DMA on the channel number passed in the
//!\e ui32Channel parameter.
//!
//! \return None.
//
//*****************************************************************************
#define USBLibDMAChannelDisable(psUSBDMAInst, ui32Channel)                    \
          psUSBDMAInst->pfnChannelDisable(psUSBDMAInst, ui32Channel)

//*****************************************************************************
//
//! This function is configures a USB transfer on a given DMA channel.
//!
//! \param psUSBDMAInst is the DMA structure pointer for this instance.
//! \param ui32Channel is the DMA channel to use.
//! \param pvBuffer is a pointer to the buffer to use for the transfer.
//! \param ui32Size is the size of the data to be transferred in bytes.
//!
//! This function is called to configure a transfer using the USB
//! controller depending on the parameters.  The \e ui32Channel parameter
//! holds the channel number to use for this transfer which must have already
//! been allocated with a call to the USBLibDMAChannelAllocate() function.  The
//! transaction is configured to transfer \e ui32Size bytes to/from the buffer
//! held in the \e pvBuffer pointer.
//!
//! \return This function returns the number of bytes scheduled to be
//! transferred.
//
//*****************************************************************************
#define USBLibDMATransfer(psUSBDMAInst, ui32Channel, pvBuffer, ui32Size)      \
                    psUSBDMAInst->pfnTransfer(psUSBDMAInst, ui32Channel,      \
                                              pvBuffer, ui32Size)

//*****************************************************************************
//
//! This function is called by the USB interrupt handler.
//!
//! \param psUSBDMAInst is the DMA structure pointer for this instance.
//! \param ui32Status is the DMA interrupt status.
//!
//! This function is called by the USB interrupt handler to allow the DMA
//! interface to handle interrupts outside of the context of the normal USB
//! interrupt handler.  The \e ui32Status is the current DMA interrupt status
//! at the time of the USB interrupt.  Since some DMA controller interrupts are
//! cleared automatically when read, this value must be retrieved by calling
//! the USBLibDMAIntStatus() function and passed into this function.
//!
//! \return None.
//
//*****************************************************************************
#define USBLibDMAIntHandler(psUSBDMAInst, ui32Status)                         \
            psUSBDMAInst->pfnIntHandler(psUSBDMAInst, ui32Status)

//*****************************************************************************
//
//! This function is used to assign a DMA channel to an endpoint.
//!
//! \param psUSBDMAInst is the DMA instance data for a USB controller.
//! \param ui8Endpoint is the endpoint number to assign a DMA channel.
//! \param ui32MaxPacketSize is the maximum packet size for the endpoint
//! assigned that is being assigned to the DMA channel.
//! \param ui32Config are the basic configuration options for the DMA channel.
//!
//! This function assigns a DMA channel to a given endpoint.  The
//! \e ui8Endpoint parameter is the zero based endpoint number that is assigned
//! a DMA channel.  The \e ui32Config parameter contains any configuration
//! options for the DMA channel.  The current options include the following:
//! - \b USB_DMA_EP_TX - this request is for a transmit DMA channel.
//! - \b USB_DMA_EP_RX - this request is for a receive DMA channel.
//!
//! \note The maximum number of available DMA channels to endpoints varies
//! between devices.
//!
//! \return Zero or the DMA channel assigned to the endpoint.
//
//*****************************************************************************
#define USBLibDMAChannelAllocate(psUSBDMAInst, ui8Endpoint, ui32MaxPacketSize,\
                                 ui32Config)                                  \
                     psUSBDMAInst->pfnChannelAllocate(psUSBDMAInst,           \
                                                      ui8Endpoint,            \
                                                      ui32MaxPacketSize,      \
                                                      ui32Config)

//*****************************************************************************
//
//! This function is used to free a DMA channel that was assigned to an
//! endpoint.
//!
//! \param psUSBDMAInst is the DMA instance data for a USB controller.
//! \param ui8Endpoint is the DMA channel number to free up.
//!
//! This function frees up a DMA channel that was allocated to an endpoint
//! by the USBLibDMAChannelAllocate() function.
//!
//! \return None.
//
//*****************************************************************************
#define USBLibDMAChannelRelease(psUSBDMAInst, ui8Endpoint)                    \
                  psUSBDMAInst->pfnChannelRelease(psUSBDMAInst, ui8Endpoint)

//*****************************************************************************
//
//! This function is used to set the individual transfer size of a DMA channel.
//!
//! \param psUSBDMAInst is the DMA instance data for a USB controller.
//! \param ui32Channel is the DMA channel number to modify.
//! \param ui32BitSize is the individual transfer size in bits(8, 16 or 32).
//!
//! This function configures the individual transfer size of the DMA channel
//! provided in the \e ui32Channel parameter.  The \e ui32Channel must already
//! be allocated to an endpoint by calling the USBLibDMAChannelAllocate()
//! function.  The \e ui32BitSize parameter should be on of the following
//! values: 8, 16 or 32.
//!
//! \return None.
//
//*****************************************************************************
#define USBLibDMAUnitSizeSet(psUSBDMAInst, ui32Channel, ui32BitSize)          \
                      psUSBDMAInst->pfnUnitSizeSet(psUSBDMAInst, ui32Channel, \
                                                   ui32BitSize);

//*****************************************************************************
//
//! This function is used to set the arbitration size for a DMA channel.
//!
//! \param psUSBDMAInst is the DMA instance data for a USB controller.
//! \param ui32Channel is the DMA channel number to modify.
//! \param ui32ArbSize is the transfer arbitration size in bytes.
//!
//! This function configures the individual transfer size of the DMA channel
//! provided in the \e ui32Channel parameter.  The \e ui32Channel must already
//! be allocated to an endpoint by calling the USBLibDMAChannelAllocate()
//! function.
//!
//! \return None.
//
//*****************************************************************************
#define USBLibDMAArbSizeSet(psUSBDMAInst, ui32Channel, ui32ArbSize)           \
                psUSBDMAInst->pfnArbSizeSet(psUSBDMAInst, ui32Channel,        \
                                            ui32ArbSize);

//*****************************************************************************
//
//! This function enables the DMA interrupt for a given channel.
//!
//! \param psUSBDMAInst is the DMA structure pointer for this instance.
//! \param ui32Channel is the DMA channel interrupt to enable.
//!
//! This function enables DMA interrupt on the channel number passed in the
//! \e ui32Channel parameter.
//!
//! \return None.
//
//*****************************************************************************
#define USBLibDMAChannelIntEnable(psUSBDMAInst, ui32Channel)                  \
                psUSBDMAInst->pfnChannelIntEnable(psUSBDMAInst, ui32Channel)

//*****************************************************************************
//
//! This function disables DMA interrupt for a given DMA channel.
//!
//! \param psUSBDMAInst is the DMA structure pointer for this instance.
//! \param ui32Channel is the DMA channel interrupt to disable.
//!
//! This function disables the DMA interrupt on the channel number passed in
//! the \e ui32Channel parameter.
//!
//! \return None.
//
//*****************************************************************************
#define USBLibDMAChannelIntDisable(psUSBDMAInst, ui32Channel)                 \
        psUSBDMAInst->pfnChannelIntDisable(psUSBDMAInst, ui32Channel)

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

#endif // __USBLIBPRIV_H__
