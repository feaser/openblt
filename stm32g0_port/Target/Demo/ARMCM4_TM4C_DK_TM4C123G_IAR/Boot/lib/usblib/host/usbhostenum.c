//*****************************************************************************
//
// usbhostenum.c - Device enumeration code for the USB host library.
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
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/usbdrv.h"
#include "driverlib/rtos_bindings.h"
#include "usblib/usblib.h"
#include "usblib/usblibpriv.h"
#include "usblib/host/usbhost.h"
#include "usblib/host/usbhostpriv.h"
#include "usblib/host/usbhhub.h"

#ifdef INCLUDE_DEBUG_OUTPUT
#include "utils/uartstdio.h"
#define DEBUG_OUTPUT UARTprintf
#else
#define DEBUG_OUTPUT while(0)((int32_t (*)(char *, ...))0)
#endif

//*****************************************************************************
//
//! \addtogroup usblib_hcd
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// External prototypes.
//
//*****************************************************************************
extern tUSBMode g_iUSBMode;

extern void OTGDeviceDisconnect(uint32_t ui32Index);

//*****************************************************************************
//
// Internal function prototypes.
//
//*****************************************************************************
static void USBHCDEP0StateTx(void);
static void USBHCDEnumHandler(void);
static void USBHCDClearFeature(uint32_t ui32DevAddress, uint32_t ui32Endpoint,
                               uint32_t ui32Feature);

//*****************************************************************************
//
// Automatic power enable.
//
//*****************************************************************************
#define USB_HOST_PWREN_AUTO     0x00000002

//*****************************************************************************
//
// Flags used to signal between the interrupt handler and USBHCDMain().
//
//*****************************************************************************
#define INT_EVENT_VBUS_ERR      0x01
#define INT_EVENT_CONNECT       0x02
#define INT_EVENT_DISCONNECT    0x04
#define INT_EVENT_POWER_FAULT   0x08
#define INT_EVENT_SOF           0x10
#define INT_EVENT_ENUM          0x20

//*****************************************************************************
//
// Flags used with the tUSBHostDevice.ui32Flags structure member.
//
//*****************************************************************************
#define USBHDEV_FLAG_NOTIFYINT  0x00000001
#define USBHDEV_FLAG_ALLOCATED  0x80000000

//*****************************************************************************
//
// This holds the current power configuration that is used when USBHCDInit()
// is called.
//
//*****************************************************************************
static uint32_t g_ui32PowerConfig = USBHCD_VBUS_AUTO_HIGH;

//*****************************************************************************
//
// The states for endpoint 0 during enumeration.
//
//*****************************************************************************
typedef enum
{
    //
    // The USB device is waiting on a request from the host controller on
    // endpoint 0.
    //
    eEP0StateIdle,

    //
    // Setup packet is expecting data IN.
    //
    eEP0StateSetupIN,

    //
    // Setup packet is sending data OUT.
    //
    eEP0StateSetupOUT,

    //
    // The USB device is receiving data from the device due to an SETUP IN
    // request.
    //
    eEP0StateRx,

    //
    // The USB device has completed the IN or OUT request and is now waiting
    // for the host to acknowledge the end of the IN/OUT transaction.  This
    // is the status phase for a USB control transaction.
    //
    eEP0StateStatus,

    //
    // This state is for when a response only has a status phase and no
    // data phase.
    //
    eEP0StateStatusIN,

    //
    // This endpoint has signaled a stall condition and is waiting for the
    // stall to be acknowledged by the host controller.
    //
    eEP0StateStall,

    //
    // An error has occurred on endpoint 0.
    //
    eEP0StateError
}
tEP0State;

//*****************************************************************************
//
// This structure holds the full state for the device enumeration.
//
//*****************************************************************************
typedef struct
{
    //
    // This is the pointer to the current data being sent out or received
    // on endpoint 0.
    //
    uint8_t *pui8Data;

    //
    // This is the number of bytes that remain to be sent from or received
    // into the g_DeviceState.pEP0Data data buffer.
    //
    volatile uint32_t ui32BytesRemaining;

    //
    // The amount of data being sent/received due to a request.
    //
    uint32_t ui32DataSize;

    //
    // This is the current device address in use by endpoint 0.
    //
    uint32_t ui32DevAddress;

    //
    // The maximum packet size for the device responding to the setup packet.
    //
    uint32_t ui32MaxPacketSize;

    //
    // The host controller's state.
    //
    tEP0State iState;
}
tHostState;

//*****************************************************************************
//
// This variable holds the current state of endpoint 0.
//
//*****************************************************************************
static volatile tHostState g_sUSBHEP0State =
{
    0,                          // pui8Data
    0,                          // ui32BytesRemaining
    0,                          // ui32DataSize
    0,                          // ui32DevAddress
    0,                          // ui32MaxPacketSize
    eEP0StateIdle               // iState
};

//*****************************************************************************
//
// The global delay time for use by SysCtlDelay() function.  This is
// initialized to an appropriate value for a 50MHz clock.  The correct value
// will be set in USBHCDInit().
//
//*****************************************************************************
static uint32_t g_ui32Tickms = 0;
static volatile uint32_t g_ui32CurrentTick = 0;

//*****************************************************************************
//
// The current active drivers.
//
//*****************************************************************************
static int32_t g_pi32USBHActiveDriver[MAX_USB_DEVICES + 1];
static void *g_ppvDriverInstance[MAX_USB_DEVICES + 1];

//*****************************************************************************
//
// This is the structure used to hold the information for a given USB pipe
// that is attached to a device.
//
//*****************************************************************************
typedef struct
{
    //
    // The current address for this pipe.
    //
    tUSBHostDevice *psDevice;

    //
    // The current address for this pipe.
    //
    uint8_t ui8EPNumber;

    //
    // The DMA channel assigned to this endpoint.
    //
    uint8_t ui8DMAChannel;

    //
    // The current type for this pipe.
    //
    uint32_t ui32Type;

    //
    // The millisecond interval for this pipe.
    //
    uint32_t ui32Interval;

    //
    // The next tick value to trigger and event on this pipe.
    //
    uint32_t ui32NextEventTick;

    //
    // The current call back for this pipe.
    //
    tHCDPipeCallback pfnCallback;

    //
    // The pointer to which IN data must be copied.
    //
    uint8_t *pui8ReadPtr;

    //
    // The number of bytes of read data to copy.
    //
    uint32_t ui32ReadSize;

    //
    // The state of a given USB pipe.
    //
    volatile enum
    {
        ePipeReading,
        ePipeReadDMA,
        ePipeReadDMAWait,
        ePipeReadSingle,
        ePipeDataReady,
        ePipeDataSent,
        ePipeWriting,
        ePipeWriteDMA,
        ePipeWriteDMASend,
        ePipeWriteDMAWait,
        ePipeWriteSingle,
        ePipeStalled,
        ePipeError,
        ePipeIdle,
        ePipeDisabled
    }
    iState;

    //
    // The actual FIFO offset allocated to this endpoint.
    //
    uint16_t ui16FIFOAddr;

    //
    // The size of the FIFO entry based on the size parameter.  These are
    // equivalent to the USB_FIFO_SZ_* values in usb.h.
    //
    uint8_t ui8FIFOSize;

    //
    // The bit offset in the allocation structure.
    //
    uint8_t ui8FIFOBitOffset;
}
tUSBHCDPipe;

//*****************************************************************************
//
// The internal state of the device.
//
//*****************************************************************************
typedef enum
{
    eHCDDevDisconnected,
    eHCDDevConnected,
    eHCDDevConnectedHub,
    eHCDDevRequest,
    eHCDDevReset,
    eHCDDevAddressed,
    eHCDDevConfigured,
    eHCDDevGetStrings,
    eHCDDevError,
    eHCDVBUSError,
    eHCDPowerFault,
    eHCDIdle
}
tUSBHDeviceState;

static void ProcessUSBDeviceStateMachine(tUSBHDeviceState iOldState,
                                         uint32_t ui32DevIndex);

//*****************************************************************************
//
// This is a fixed number as it relates to the maximum number of USB pipes
// available on any USB controller.  The actual number on a given device may
// be less than this number.
//
//*****************************************************************************
#define MAX_NUM_PIPES           15

//*****************************************************************************
//
// This is a fixed number as it relates to the number of USB pipes available
// in the USB controller.
//
//*****************************************************************************
#define MAX_NUM_DMA_CHANNELS    6

//*****************************************************************************
//
// Marker for an unused DMA channel slot.
//
//*****************************************************************************
#define USBHCD_DMA_UNUSED       0xff

//*****************************************************************************
//
// These definitions are used to manipulate the values returned as allocated
// USB pipes.
//
//*****************************************************************************
#define EP_PIPE_TYPE_LOW_SPEED  0x02000000
#define EP_PIPE_USE_UDMA        0x01000000
#define EP_PIPE_TYPE_ISOC       0x00800000
#define EP_PIPE_TYPE_INTR       0x00400000
#define EP_PIPE_TYPE_BULK       0x00200000
#define EP_PIPE_TYPE_CONTROL    0x00100000
#define EP_PIPE_TYPE_IN         0x00020000
#define EP_PIPE_TYPE_OUT        0x00010000
#define EP_PIPE_IDX_M           0x0000ffff

//*****************************************************************************
//
// This creates a USB pipe handle from an index.
//
//*****************************************************************************
#define OUT_PIPE_HANDLE(ui32Idx)                                              \
                                (g_sUSBHCD.psUSBOUTPipes[ui32Idx].ui32Type |  \
                                 ui32Idx)
#define IN_PIPE_HANDLE(ui32Idx) (g_sUSBHCD.psUSBINPipes[ui32Idx].ui32Type |   \
                                 ui32Idx)

//*****************************************************************************
//
// Converts from an endpoint specifier to the offset of the endpoint's
// control/status registers.
//
//*****************************************************************************
#define EP_OFFSET(Endpoint)     (Endpoint - 0x10)

//*****************************************************************************
//
// This structure holds the state information for a given host controller.
//
//*****************************************************************************
typedef struct
{
    uint32_t ui32USBBase;

    tUSBHCDPipe sUSBControlPipe;
    tUSBHCDPipe psUSBOUTPipes[MAX_NUM_PIPES];
    tUSBHCDPipe psUSBINPipes[MAX_NUM_PIPES];

    //
    // Each devices state.  We support a total of (MAX_USB_DEVICES + 1) devices
    // to allow for the use if MAX_USB_DEVICES through a single hub (which is
    // itself a device).
    //
    tUSBHostDevice psUSBDevice[MAX_USB_DEVICES + 1];

    //
    // Holds the current state of the device.
    //
    volatile tUSBHDeviceState piDeviceState[MAX_USB_DEVICES + 1];

    //
    // Pointer to the memory pool for this controller.
    //
    void *pvPool;

    //
    // The pool size for this controller.
    //
    uint32_t ui32PoolSize;

    //
    // The number of endpoint pairs supported by the controller.
    //
    uint32_t ui32NumEndpoints;

    //
    // The class drivers for this controller.
    //
    const tUSBHostClassDriver * const *ppsClassDrivers;

    //
    // The number of class drivers.
    //
    uint32_t ui32NumClassDrivers;

    //
    // This is the index in the driver list of the event driver.
    //
    int32_t i32EventDriver;

    //
    // These are the generic event information used by the event driver.
    //
    uint32_t ui32EventEnables;

    uint32_t ui32Class;

    //
    // The DMA instance information for this class.
    //
    tUSBDMAInstance *psDMAInstance;

    //
    // The interrupt number for this instance.
    //
    uint32_t ui32IntNum;

    //
    // The pending interrupt events that are processed in USBHCDMain().
    //
    uint32_t ui32IntEvents;

    //
    // Features.
    //
    uint32_t ui32Features;

    //
    // The host initiated resume duration in us.
    //
    uint32_t ui32LPMHIRD;
}
tUSBHCD;

//*****************************************************************************
//
// The global to hold all of the state information for a given host controller.
//
//*****************************************************************************
static tUSBHCD g_sUSBHCD;

//*****************************************************************************
//
// Allocates the memory needed to support configuration descriptors for
// devices.
//
//*****************************************************************************
uint32_t
ConfigDescAlloc(tUSBHostDevice *psDevice, uint32_t ui32Size)
{
    uint32_t ui32Idx, ui32BlockSize, ui32PoolSize;
    uint8_t *pui8Pool;

    if(g_sUSBHCD.psUSBDevice[0].psConfigDescriptor == 0)
    {
        //
        // 32 bit align the allocation.
        //
        ui32Size = (ui32Size + 3) & ~3;

        if(g_sUSBHCD.ui32PoolSize < ui32Size)
        {
            return(0);
        }

        //
        // Allocate the root connection to the device.
        //
        g_sUSBHCD.psUSBDevice[0].psConfigDescriptor = g_sUSBHCD.pvPool;
        g_sUSBHCD.psUSBDevice[0].ui32ConfigDescriptorSize = ui32Size;

        //
        // Allocate the hub memory pools (these can end up null).
        //
        pui8Pool = (uint8_t *)g_sUSBHCD.pvPool + ui32Size;
        ui32PoolSize = g_sUSBHCD.ui32PoolSize - ui32Size;

        //
        // Divide the pool up into blocks, one for each supported port.  We make
        // sure that each block is a multiple of 4 bytes.
        //
        ui32BlockSize = (ui32PoolSize / MAX_USB_DEVICES) & ~3;
        for(ui32Idx = 1; ui32Idx < MAX_USB_DEVICES; ui32Idx++)
        {
            g_sUSBHCD.psUSBDevice[ui32Idx].psConfigDescriptor =
                        (tConfigDescriptor *)(pui8Pool +
                                              (ui32Idx * ui32BlockSize));
            g_sUSBHCD.psUSBDevice[ui32Idx].ui32ConfigDescriptorSize =
                            ui32BlockSize;
        }
    }
    return(ui32Size);
}

//*****************************************************************************
//
// Frees the memory needed to support configuration descriptors for
// devices.
//
//*****************************************************************************
void
ConfigDescFree(tUSBHostDevice *psDevice)
{
    uint32_t ui32Idx;

    //
    // If this is the root device then deallocate.
    //
    if(&g_sUSBHCD.psUSBDevice[0] == psDevice)
    {
        for(ui32Idx = 0; ui32Idx < MAX_USB_DEVICES; ui32Idx++)
        {
            g_sUSBHCD.psUSBDevice[ui32Idx].ui32Flags &=
                                                    ~USBHDEV_FLAG_ALLOCATED;
            g_sUSBHCD.psUSBDevice[ui32Idx].psConfigDescriptor = 0;
            g_sUSBHCD.psUSBDevice[ui32Idx].ui32ConfigDescriptorSize = 0;
        }
    }
}

//*****************************************************************************
//
// Return the device index from a ui32Instance value passed from an external
// source.
//
//*****************************************************************************
static uint8_t
HCDInstanceToDevIndex(uint32_t ui32Instance)
{
    uint32_t ui32DevIndex;

    //
    // Get the device instance from the instance value.
    //
    ui32DevIndex = (ui32Instance & 0xff);

    //
    // If the above math went negative or is too large just return 0xff.
    //
    if(ui32DevIndex > MAX_USB_DEVICES)
    {
        ui32DevIndex = 0xff;
    }

    return(ui32DevIndex);
}

//=============================================================================
//
// This is the internal function that will map an event to a valid event flag.
//
// \param ui32Event specifies which event flag to retrieve.
//
// \return The event flag or 0 if there is no support event flag for the
// event specified by the \e ui32Event parameter.
//
//=============================================================================
static uint32_t
GetEventFlag(uint32_t ui32Event)
{
    uint32_t ui32EventFlag;

    ui32EventFlag = 0;

    //
    // Search for a valid event flag for the requested event.
    //
    switch(ui32Event)
    {
        case USB_EVENT_SOF:
        {
            ui32EventFlag |= USBHCD_EVFLAG_SOF;
            break;
        }
        case USB_EVENT_CONNECTED:
        {
            ui32EventFlag |= USBHCD_EVFLAG_CONNECT;
            break;
        }
        case USB_EVENT_DISCONNECTED:
        {
            ui32EventFlag |= USBHCD_EVFLAG_DISCNCT;
            break;
        }
        case USB_EVENT_UNKNOWN_CONNECTED:
        {
            ui32EventFlag |= USBHCD_EVFLAG_UNKCNCT;
            break;
        }
        case USB_EVENT_POWER_FAULT:
        {
            ui32EventFlag |= USBHCD_EVFLAG_PWRFAULT;
            break;
        }
        case USB_EVENT_POWER_DISABLE:
        {
            ui32EventFlag |= USBHCD_EVFLAG_PWRDIS;
            break;
        }
        case USB_EVENT_POWER_ENABLE:
        {
            ui32EventFlag |= USBHCD_EVFLAG_PWREN;
            break;
        }
        default:
        {
            break;
        }
    }
    return(ui32EventFlag);
}

//=============================================================================
//
//! This function is called to enable a specific USB HCD event notification.
//!
//! \param ui32Index specifies which USB controller to use.
//! \param pvEventDriver is the event driver structure that was passed into
//! the USBHCDRegisterDrivers() function as part of the array of
//! tUSBHostClassDriver structures.
//! \param ui32Event is the event to enable.
//!
//! This function is called to enable event callbacks for a specific USB HCD
//! event.  The requested event is passed in the \e ui32Event parameter.  Not
//! all events can be enables so the function will return zero if the event
//! provided cannot be enabled.  The \e pvEventDriver is a pointer to the
//! event driver structure that the caller passed into the
//! USBHCDRegisterDrivers() function.  This structure is typically declared
//! with the DECLARE_EVENT_DRIVER() macro and included as part of the array
//! of pointers to tUSBHostClassDriver structures that is passed to the
//! USBHCDRegisterDrivers() function.
//!
//! \return This function returns a non-zero number if the event was
//! successfully enabled and returns zero if the event cannot be enabled.
//
//=============================================================================
int32_t
USBHCDEventEnable(uint32_t ui32Index, void *pvEventDriver, uint32_t ui32Event)
{
    int32_t i32Ret;
    uint32_t ui32EventFlag;

    ASSERT(ui32Index == 0);

    //
    // Default the return to fail the call unless a valid event is found.
    //
    i32Ret = 0;

    //
    // Get the event flag for this event.
    //
    ui32EventFlag = GetEventFlag(ui32Event);

    //
    // Check if there was an event flag for the corresponding event.
    //
    if(ui32EventFlag)
    {
        //
        // Set the enable for this event.
        //
        g_sUSBHCD.ui32EventEnables |= ui32EventFlag;

        //
        // Indicate that the event was valid and is now enabled.
        //
        i32Ret = 1;
    }

    return(i32Ret);
}

//=============================================================================
//
//! This function is called to disable a specific USB HCD event notification.
//!
//! \param ui32Index specifies which USB controller to use.
//! \param pvEventDriver is the event driver structure that was passed into
//! the USBHCDRegisterDrivers() function as part of the array of
//! tUSBHostClassDriver structures.
//! \param ui32Event is the event to disable.
//!
//! This function is called to disable event callbacks for a specific USB HCD
//! event.  The requested event is passed in the \e ui32Event parameter.  Not
//! all events can be enables so the function will return zero if the event
//! provided cannot be enabled.  The \e pvEventDriver is a pointer to the
//! event driver structure that the caller passed into the
//! USBHCDRegisterDrivers() function.  This structure is typically declared
//! with the DECLARE_EVENT_DRIVER() macro and included as part of the array
//! of pointers to tUSBHostClassDriver structures that is passed to the
//! USBHCDRegisterDrivers() function.
//!
//! \return This function returns a non-zero number if the event was
//! successfully disabled and returns zero if the event cannot be disabled.
//
//=============================================================================
int32_t
USBHCDEventDisable(uint32_t ui32Index, void *pvEventDriver, uint32_t ui32Event)
{
    int32_t i32Ret;
    uint32_t ui32EventFlag;

    ASSERT(ui32Index == 0);

    //
    // Default the return to fail the call unless a valid event is found.
    //
    i32Ret = 0;

    //
    // Get the event flag for this event.
    //
    ui32EventFlag = GetEventFlag(ui32Event);

    //
    // Check if there was an event flag for the corresponding event.
    //
    if(ui32EventFlag)
    {
        //
        // Clear the enable for this event.
        //
        g_sUSBHCD.ui32EventEnables &= ~ui32EventFlag;

        //
        // Indicate that the event was valid and is now disabled.
        //
        i32Ret = 1;
    }

    return(i32Ret);
}

//*****************************************************************************
//
// If there is an event driver this function will send out a generic connection
// event USB_EVENT_UNKNOWN_CONNECTED indicating that an unknown connection
// event has occurred.
//
//*****************************************************************************
static void
SendUnknownConnect(uint32_t ui32Index, uint32_t ui32Class)
{
    tEventInfo sEvent;

    //
    // If there is an event driver registered and it has a event handler and
    // the USBHCD_EVFLAG_UNKCNCT is enabled then call the function.
    //
    sEvent.ui32Event = USB_EVENT_UNKNOWN_CONNECTED;
    sEvent.ui32Instance = ui32Class;
    InternalUSBHCDSendEvent(0, &sEvent, USBHCD_EVFLAG_UNKCNCT);
}

//*****************************************************************************
//
// Internal memory allocation space is two 32-bit values where each
// bit represents a 64 byte block in the FIFO.  This requires 64 bits for
// the 4096 bytes of FIFO available.
//
//*****************************************************************************
static uint32_t g_pui32Alloc[2];

//*****************************************************************************
//
// This function handles freeing FIFO memory that has been allocated using the
// FIFOAlloc() function.
//
//*****************************************************************************
static void
FIFOFree(tUSBHCDPipe *psUSBPipe)
{
    uint32_t ui32Mask;

    //
    // Calculate the mask value to use to clear off the allocated blocks used
    // by the USB pipe specified by psUSBPipe.
    //
    ui32Mask = (1 << (psUSBPipe->ui8FIFOSize - 2)) - 1;
    ui32Mask = ui32Mask << psUSBPipe->ui8FIFOBitOffset;

    //
    // Determine which 32 bit word to access based on the size.
    //
    if(psUSBPipe->ui8FIFOSize > USB_FIFO_SZ_64)
    {
        //
        // If the FIFO size is greater than 64 then use the upper 32 bits.
        //
        g_pui32Alloc[1] &= ~ui32Mask;
    }
    else
    {
        //
        // If the FIFO size is less than or equal to 64 then use the lower
        // 32 bits.
        //
        g_pui32Alloc[0] &= ~ui32Mask;
    }
}

//*****************************************************************************
//
// This function is used to allocate FIFO memory to a given USB pipe.
//
// \param psUSBPipe is the USB pipe that needs FIFO memory allocated.
// \param ui32Size is the minimum size in bytes of the FIFO to allocate.
//
// This function will allocate \e ui32Size bytes to the USB pipe in the
// \e psUSBPipe parameter.  The function will fill the psUSBPipe structure
// members ui8FIFOSize and ui8FIFOAddr with values that can be used with the
// USBFIFOConfigSet() API.  This allocation uses a first fit algorithm.
//
// \return This function returns the size of the block allocated.
//
//*****************************************************************************
static uint32_t
FIFOAlloc(tUSBHCDPipe *psUSBPipe, uint32_t ui32Size)
{
    uint32_t ui32Blocks, ui32Start, ui32BlockSize, ui32Temp, ui32Index;
    uint16_t ui16FIFOAddr;

    //
    // Save which 32 bit value to access, the upper is for blocks greater
    // than 64 and the lower is for block 64 or less.
    //
    if(ui32Size > 64)
    {
        ui32Index = 1;
    }
    else
    {
        ui32Index = 0;
    }

    //
    // Initial FIFO address is 0.
    //
    ui16FIFOAddr = 0;

    //
    // Initialize the bit pattern and bit location.
    //
    ui32Blocks = 1;
    ui32Start = 0;

    //
    // The initial block size is always the minimum size of 64 bytes.
    //
    ui32BlockSize = 64;

    //
    // The initial size and offset are 64 and 0.
    //
    psUSBPipe->ui8FIFOBitOffset = 0;
    psUSBPipe->ui8FIFOSize = 3;

    //
    // Scan through 32 bits looking for a memory block large enough to fill
    // the request.
    //
    while(ui16FIFOAddr <= 32)
    {
        //
        // If the pattern is zero then it is a possible match.
        //
        if((g_pui32Alloc[ui32Index] & ui32Blocks) == 0)
        {
            //
            // If the size is large enough then save it and break out of the
            // loop.
            //
            if(ui32BlockSize >= ui32Size)
            {
                //
                // Mark the memory as allocated.
                //
                g_pui32Alloc[ui32Index] |= ui32Blocks;

                break;
            }

            //
            // Increment the size of the FIFO block.
            //
            psUSBPipe->ui8FIFOSize++;

            //
            // Add in a new bit to the size of the allocation.
            //
            ui32Blocks = ui32Blocks | (ui32Blocks << 1) ;

            //
            // Double the current size.
            //
            ui32BlockSize <<= 1;

        }
        else
        {
            //
            // Need to start over looking because the last allocation match
            // failed, so reset the bit offset to the current location and the
            // size to 64 bytes.
            //
            psUSBPipe->ui8FIFOBitOffset = ui16FIFOAddr;
            psUSBPipe->ui8FIFOSize = 3;

            //
            // Reset the block size to the minimum (64 bytes).
            //
            ui32BlockSize = 64;

            //
            // Store the current starting bit location and set the block mask
            // to this value.
            //
            ui32Start = 1 << ui16FIFOAddr;
            ui32Blocks = ui32Start;
        }

        //
        // Increase the address of the FIFO offset.
        //
        ui16FIFOAddr++;
    }

    //
    // If there was no block large enough then fail this call.
    //
    if(ui16FIFOAddr > 32)
    {
        ui32BlockSize = 0;
        psUSBPipe->ui16FIFOAddr = 0;
        psUSBPipe->ui8FIFOBitOffset = 0;
        psUSBPipe->ui8FIFOSize = 0;
    }
    else
    {
        //
        // Calculate the offset in the FIFO.
        //
        ui32Temp = psUSBPipe->ui8FIFOBitOffset * 64;

        //
        // Sizes greater than 64 are allocated in the second half of the FIFO
        // memory space.
        //
        if(ui32Size > 64)
        {
            ui32Temp += 2048;
        }

        //
        // Convert this to the value that can be set in the USB controller.
        //
        psUSBPipe->ui16FIFOAddr = (uint16_t)ui32Temp;
    }
    return(ui32BlockSize);
}

//*****************************************************************************
//
//! This function is used to allocate a USB HCD pipe.
//!
//! \param ui32Index specifies which USB controller to use.
//! \param ui32EndpointType is the type of endpoint that this pipe will be
//! communicating with.
//! \param psDevice is the device instance associated with this endpoint.
//! \param ui32Size is the size of the FIFO in bytes.
//! \param pfnCallback is the function that will be called when events occur on
//! this USB Pipe.
//!
//! Since there are a limited number of USB HCD pipes that can be used in the
//! host controller, this function is used to temporarily or permanently
//! acquire one of the endpoints.  Unlike the USBHCDPipeAlloc() function this
//! function allows the caller to specify the size of the FIFO allocated to
//! this endpoint in the \e ui32Size parameter.  This function also provides a
//! method to register a callback for status changes on this endpoint.  If no
//! callbacks are desired then the \e pfnCallback function should be set to 0.
//! The callback should be used when using the USBHCDPipeSchedule() function
//! so that the caller is notified when the action is complete.
//!
//! \return This function returns a value indicating which pipe was reserved.
//! If the value is 0 then there were no pipes currently available.  This value
//! should be passed to any USBHCDPipe APIs to indicate which pipe is being
//! accessed.
//
//*****************************************************************************
uint32_t
USBHCDPipeAllocSize(uint32_t ui32Index, uint32_t ui32EndpointType,
                    tUSBHostDevice *psDevice, uint32_t ui32Size,
                    tHCDPipeCallback pfnCallback)
{
    int32_t i32Idx;

    uint32_t ui32HubAddr;

    ASSERT(ui32Index == 0);

    //
    // Find a USB pipe that is free.
    //
    for(i32Idx = 0; i32Idx < MAX_NUM_PIPES; i32Idx++)
    {
        //
        // Handle OUT Pipes.
        //
        if(ui32EndpointType & EP_PIPE_TYPE_OUT)
        {
            //
            // A zero address indicates free.
            //
            if(g_sUSBHCD.psUSBOUTPipes[i32Idx].psDevice == 0)
            {
                //
                // Set up uDMA for the pipe.
                //
                if(ui32EndpointType & EP_PIPE_USE_UDMA)
                {
                    //
                    // Allocate a DMA channel to the endpoint.
                    //
                    g_sUSBHCD.psUSBOUTPipes[i32Idx].ui8DMAChannel =
                        USBLibDMAChannelAllocate(g_sUSBHCD.psDMAInstance,
                                                 IndexToUSBEP(i32Idx + 1),
                                                 ui32Size,
                                                 USB_DMA_EP_TX |
                                                 USB_DMA_EP_HOST);

                    //
                    // If no DMA channel was available then just disable DMA
                    // on this pipe.
                    //
                    if(g_sUSBHCD.psUSBOUTPipes[i32Idx].ui8DMAChannel == 0)
                    {
                        ui32EndpointType &= ~EP_PIPE_USE_UDMA;
                    }
                }

                //
                // Save the endpoint type and device address and callback
                // function.
                //
                g_sUSBHCD.psUSBOUTPipes[i32Idx].ui32Type = ui32EndpointType;
                g_sUSBHCD.psUSBOUTPipes[i32Idx].psDevice = psDevice;
                g_sUSBHCD.psUSBOUTPipes[i32Idx].pfnCallback = pfnCallback;

                //
                // Clear out any pending status on this endpoint in case it
                // was in use before a allowing a new device class to use it.
                //
                MAP_USBHostEndpointStatusClear(USB0_BASE,
                                               IndexToUSBEP(i32Idx + 1),
                                               USB_HOST_OUT_STATUS);

                //
                // Initialize the endpoint as idle.
                //
                g_sUSBHCD.psUSBOUTPipes[i32Idx].iState = ePipeIdle;

                //
                // Allocate space in the FIFO for this endpoint.
                //
                if(FIFOAlloc(&g_sUSBHCD.psUSBOUTPipes[i32Idx], ui32Size) != 0)
                {
                    //
                    // Configure the FIFO.
                    //
                    MAP_USBFIFOConfigSet(USB0_BASE,
                                IndexToUSBEP(i32Idx + 1),
                                g_sUSBHCD.psUSBOUTPipes[i32Idx].ui16FIFOAddr,
                                g_sUSBHCD.psUSBOUTPipes[i32Idx].ui8FIFOSize,
                                USB_EP_HOST_OUT);
                }

                //
                // Set the function address for this endpoint.
                //
                MAP_USBHostAddrSet(USB0_BASE, IndexToUSBEP(i32Idx + 1),
                                   psDevice->ui32Address, USB_EP_HOST_OUT);

                //
                // Set the hub and port address for the endpoint.
                //
                ui32HubAddr = psDevice->ui8Hub | (psDevice->ui8HubPort << 8);
                USBHostHubAddrSet(USB0_BASE, IndexToUSBEP(i32Idx + 1),
                                  ui32HubAddr, (USB_EP_HOST_OUT |
                                  (psDevice->bLowSpeed ?
                                   USB_EP_SPEED_LOW : USB_EP_SPEED_FULL)));
                break;
            }
        }
        //
        // Handle IN Pipes.
        //
        else if(ui32EndpointType & EP_PIPE_TYPE_IN)
        {
            //
            // A zero address indicates free.
            //
            if(g_sUSBHCD.psUSBINPipes[i32Idx].psDevice == 0)
            {
                //
                // Set up uDMA for the pipe.
                //
                if(ui32EndpointType & EP_PIPE_USE_UDMA)
                {
                    //
                    // Allocate a DMA channel to the endpoint.
                    //
                    g_sUSBHCD.psUSBINPipes[i32Idx].ui8DMAChannel =
                            USBLibDMAChannelAllocate(g_sUSBHCD.psDMAInstance,
                                                     IndexToUSBEP(i32Idx + 1),
                                                     ui32Size,
                                                     USB_DMA_EP_RX |
                                                     USB_DMA_EP_HOST);

                    //
                    // If no DMA channel was available then just disable DMA
                    // on this pipe.
                    //
                    if(g_sUSBHCD.psUSBINPipes[i32Idx].ui8DMAChannel == 0)
                    {
                        ui32EndpointType &= ~EP_PIPE_USE_UDMA;
                    }
                }

                //
                // Save the endpoint type and device address and callback
                // function.
                //
                g_sUSBHCD.psUSBINPipes[i32Idx].ui32Type = ui32EndpointType;
                g_sUSBHCD.psUSBINPipes[i32Idx].psDevice = psDevice;
                g_sUSBHCD.psUSBINPipes[i32Idx].pfnCallback = pfnCallback;

                //
                // Clear out any pending status on this endpoint in case it
                // was in use before a allowing a new device class to use it.
                //
                MAP_USBHostEndpointStatusClear(USB0_BASE,
                                               IndexToUSBEP(i32Idx + 1),
                                               USB_HOST_IN_STATUS);

                //
                // Allocate space in the FIFO for this endpoint.
                //
                if(FIFOAlloc(&g_sUSBHCD.psUSBINPipes[i32Idx], ui32Size) != 0)
                {
                    //
                    // Configure the FIFO.
                    //
                    MAP_USBFIFOConfigSet(USB0_BASE,
                                IndexToUSBEP(i32Idx + 1),
                                g_sUSBHCD.psUSBINPipes[i32Idx].ui16FIFOAddr,
                                g_sUSBHCD.psUSBINPipes[i32Idx].ui8FIFOSize,
                                USB_EP_HOST_IN);
                }

                //
                // Set the function address for this endpoint.
                //
                MAP_USBHostAddrSet(USB0_BASE, IndexToUSBEP(i32Idx + 1),
                                   psDevice->ui32Address, USB_EP_HOST_IN);

                //
                // Set the hub and port address for the endpoint.
                //
                ui32HubAddr = psDevice->ui8Hub | (psDevice->ui8HubPort << 8);
                USBHostHubAddrSet(USB0_BASE, IndexToUSBEP(i32Idx + 1),
                                  ui32HubAddr, (USB_EP_HOST_IN |
                                  (psDevice->bLowSpeed ?
                                   USB_EP_SPEED_LOW : USB_EP_SPEED_FULL)));

                //
                // Reset the state of the pipe to idle.
                //
                g_sUSBHCD.psUSBINPipes[i32Idx].iState = ePipeIdle;

                break;
            }
        }
    }

    //
    // Did not find a free pipe.
    //
    if(i32Idx == MAX_NUM_PIPES)
    {
        return(0);
    }

    //
    // Return the pipe index and type that was allocated.
    //
    return(ui32EndpointType | i32Idx);
}

//*****************************************************************************
//
//! This function is used to allocate a USB HCD pipe.
//!
//! \param ui32Index specifies which USB controller to use.
//! \param ui32EndpointType is the type of endpoint that this pipe will be
//! communicating with.
//! \param psDevice is the device instance associated with this endpoint.
//! \param pfnCallback is the function that will be called when events occur on
//! this USB Pipe.
//!
//! Since there are a limited number of USB HCD pipes that can be used in the
//! host controller, this function is used to temporarily or permanently
//! acquire one of the endpoints.  It also provides a method to register a
//! callback for status changes on this endpoint.  If no callbacks are desired
//! then the \e pfnCallback function should be set to 0.  The callback should
//! be used when using the USBHCDPipeSchedule() function so that the caller is
//! notified when the action is complete.
//!
//! \return This function returns a value indicating which pipe was reserved.
//! If the value is 0 then there were no pipes currently available.  This value
//! should be passed to any USBHCDPipe APIs to indicate which pipe is being
//! accessed.
//
//*****************************************************************************
uint32_t
USBHCDPipeAlloc(uint32_t ui32Index, uint32_t ui32EndpointType,
                tUSBHostDevice *psDevice, tHCDPipeCallback pfnCallback)
{
    //
    // The old API allocated only 64 bytes to each endpoint.
    //
    return(USBHCDPipeAllocSize(ui32Index, ui32EndpointType, psDevice, 64,
                               pfnCallback));
}

//*****************************************************************************
//
//! This function is used to configure a USB HCD pipe.
//!
//! This should be called after allocating a USB pipe with a call to
//! USBHCDPipeAlloc().  It is used to set the configuration associated with an
//! endpoint like the max payload and target endpoint.  The \e ui32MaxPayload
//! parameter is typically read directly from the devices endpoint descriptor
//! and is expressed in bytes.
//!
//! Setting the \e ui32Interval parameter depends on the type of endpoint being
//! configured.  For endpoints that do not need to use the \e ui32Interval
//! parameter \e ui32Interval should be set to 0.  For Bulk \e ui32Interval is
//! a value from 2-16 and will set the NAK timeout value as
//! 2^(\e ui32Interval-1) frames.  For interrupt endpoints \e ui32Interval is a
//! value from 1-255 and is the count in frames between polling the endpoint.
//! For isochronous endpoints \e ui32Interval ranges from 1-16 and is the
//! polling interval in frames represented as 2^(\e ui32Interval-1) frames.
//!
//! \param ui32Pipe is the allocated endpoint to modify.
//! \param ui32MaxPayload is maximum data that can be handled per transaction.
//! \param ui32Interval is the polling interval for data transfers expressed in
//! frames.
//! \param ui32TargetEndpoint is the target endpoint on the device to
//! communicate with.
//!
//! \return If the call was successful, this function returns zero any other
//! value indicates an error.
//
//*****************************************************************************
uint32_t
USBHCDPipeConfig(uint32_t ui32Pipe, uint32_t ui32MaxPayload,
                 uint32_t ui32Interval, uint32_t ui32TargetEndpoint)
{
    uint32_t ui32Flags;
    uint32_t ui32Index;

    //
    // Get the index number from the allocated pipe.
    //
    ui32Index = (ui32Pipe & EP_PIPE_IDX_M);

    //
    // Set the direction.
    //
    if(ui32Pipe & EP_PIPE_TYPE_OUT)
    {
        //
        // Set the mode for this endpoint.
        //
        if(g_sUSBHCD.psUSBOUTPipes[ui32Index].ui32Type & EP_PIPE_TYPE_BULK)
        {
            ui32Flags = USB_EP_MODE_BULK;
        }
        else if(g_sUSBHCD.psUSBOUTPipes[ui32Index].ui32Type &
                EP_PIPE_TYPE_INTR)
        {
            ui32Flags = USB_EP_MODE_INT;
        }
        else if(g_sUSBHCD.psUSBOUTPipes[ui32Index].ui32Type &
                EP_PIPE_TYPE_ISOC)
        {
            ui32Flags = USB_EP_MODE_ISOC;
        }
        else
        {
            ui32Flags = USB_EP_MODE_CTRL;
        }

        ui32Flags |= USB_EP_HOST_OUT;

        g_sUSBHCD.psUSBOUTPipes[ui32Index].ui8EPNumber =
                                                (uint8_t)ui32TargetEndpoint;

        //
        // Save the interval and the next tick to trigger a scheduler event.
        //
        g_sUSBHCD.psUSBOUTPipes[ui32Index].ui32Interval = ui32Interval;
        g_sUSBHCD.psUSBOUTPipes[ui32Index].ui32NextEventTick =
                                            ui32Interval + g_ui32CurrentTick;

        //
        // Set the device speed.
        //
        ui32Flags |= (g_sUSBHCD.psUSBOUTPipes[ui32Index].psDevice->bLowSpeed ?
                                      USB_EP_SPEED_LOW : USB_EP_SPEED_FULL);
        //
        // Set up the appropriate flags if uDMA is used.
        //
        if(ui32Pipe & EP_PIPE_USE_UDMA)
        {
            ui32Flags |= USB_EP_DMA_MODE_0 | USB_EP_AUTO_SET;
        }
    }
    else
    {
        //
        // Set the mode for this endpoint.
        //
        if(g_sUSBHCD.psUSBINPipes[ui32Index].ui32Type & EP_PIPE_TYPE_BULK)
        {
            ui32Flags = USB_EP_MODE_BULK;
        }
        else if(g_sUSBHCD.psUSBINPipes[ui32Index].ui32Type & EP_PIPE_TYPE_INTR)
        {
            ui32Flags = USB_EP_MODE_INT;
        }
        else if(g_sUSBHCD.psUSBINPipes[ui32Index].ui32Type & EP_PIPE_TYPE_ISOC)
        {
            ui32Flags = USB_EP_MODE_ISOC;
        }
        else
        {
            ui32Flags = USB_EP_MODE_CTRL;
        }
        ui32Flags |= USB_EP_HOST_IN;

        g_sUSBHCD.psUSBINPipes[ui32Index].ui8EPNumber =
                                                (uint8_t)ui32TargetEndpoint;

        //
        // Save the interval and the next tick to trigger a scheduler event.
        //
        g_sUSBHCD.psUSBINPipes[ui32Index].ui32Interval = ui32Interval;
        g_sUSBHCD.psUSBINPipes[ui32Index].ui32NextEventTick =
                                            ui32Interval + g_ui32CurrentTick;

        //
        // Set the device speed.
        //
        ui32Flags |= (g_sUSBHCD.psUSBINPipes[ui32Index].psDevice->bLowSpeed ?
                                      USB_EP_SPEED_LOW : USB_EP_SPEED_FULL);
        //
        // Set up the appropriate flags if uDMA is used.
        //
        if(ui32Pipe & EP_PIPE_USE_UDMA)
        {
            ui32Flags |= USB_EP_DMA_MODE_1 | USB_EP_AUTO_CLEAR |
                         USB_EP_AUTO_REQUEST;
        }
    }


    //
    // Configure the endpoint according to the flags determined above.
    //
    USBHostEndpointConfig(USB0_BASE,
                          IndexToUSBEP((ui32Pipe & EP_PIPE_IDX_M) + 1),
                          ui32MaxPayload, ui32Interval, ui32TargetEndpoint,
                          ui32Flags);

    return(0);
}

//*****************************************************************************
//
//! This function is used to return the current status of a USB HCD pipe.
//!
//! This function will return the current status for a given USB pipe.  If
//! there is no status to report this call will simply return
//! \b USBHCD_PIPE_NO_CHANGE.
//!
//! \param ui32Pipe is the USB pipe for this status request.
//!
//! \return This function returns the current status for the given endpoint.
//! This will be one of the \b USBHCD_PIPE_* values.
//
//*****************************************************************************
uint32_t
USBHCDPipeStatus(uint32_t ui32Pipe)
{
    return(0);
}

//*****************************************************************************
//
//! This function is used to write data to a USB HCD pipe.
//!
//! \param ui32Pipe is the USB pipe to put data into.
//! \param pui8Data is a pointer to the data to send.
//! \param ui32Size is the amount of data to send.
//!
//! This function will block until it has sent as much data as was
//! requested using the USB pipe's FIFO.  The caller should have registered a
//! callback with the USBHCDPipeAlloc() call in order to be informed when the
//! data has been transmitted.  The value returned by this function can be less
//! than the \e ui32Size requested if the USB pipe has less space available
//! than this request is making.
//!
//! \return This function returns the number of bytes that were scheduled to
//! be sent on the given USB pipe.
//
//*****************************************************************************
uint32_t
USBHCDPipeWrite(uint32_t ui32Pipe, uint8_t *pui8Data, uint32_t ui32Size)
{
    uint32_t ui32Endpoint, ui32RemainingBytes, ui32ByteToSend, ui32PipeIdx;
    bool bUseDMA;

    //
    // Determine which endpoint interface that this pipe is using.
    //
    ui32Endpoint = IndexToUSBEP((EP_PIPE_IDX_M & ui32Pipe) + 1);

    //
    // Get index used for looking up pipe data
    //
    ui32PipeIdx = ui32Pipe & EP_PIPE_IDX_M;

    //
    // Set the total number of bytes to send out.
    //
    ui32RemainingBytes = ui32Size;

    //
    // Default to using DMA.
    //
    bUseDMA = false;

    //
    // Initialize the bytes to send to all of the remaining bytes.
    //
    ui32ByteToSend = ui32RemainingBytes;

    //
    // Send all of the requested data.
    //
    while(ui32RemainingBytes != 0)
    {
        //
        // If uDMA is not enabled for this pipe, or if the uDMA workaround
        // is applied, then don't use uDMA for this transfer.
        //
        if(ui32Pipe & EP_PIPE_USE_UDMA)
        {
            //
            // Disable the USB interrupt.
            //
            OS_INT_DISABLE(g_sUSBHCD.ui32IntNum);

            //
            // Start the DMA transfer.
            //
            if(USBLibDMATransfer(g_sUSBHCD.psDMAInstance,
                            g_sUSBHCD.psUSBOUTPipes[ui32PipeIdx].ui8DMAChannel,
                            pui8Data, ui32RemainingBytes) != 0)
            {
                if(ui32RemainingBytes < 64)
                {
                    g_sUSBHCD.psUSBOUTPipes[ui32PipeIdx].iState =
                                                            ePipeWriteDMASend;
                }
                else if((ui32RemainingBytes % 64) == 0)
                {
                    g_sUSBHCD.psUSBOUTPipes[ui32PipeIdx].iState =
                                                                ePipeWriteDMA;
                }
                else
                {
                    g_sUSBHCD.psUSBOUTPipes[ui32PipeIdx].iState =
                                                        ePipeWriteDMASend;
                }

                bUseDMA = true;
            }

            //
            // Enable the USB interrupt.
            //
            OS_INT_ENABLE(g_sUSBHCD.ui32IntNum);
        }

        if(bUseDMA == false)
        {
            //
            // Only send 64 bytes at a time if not using DMA.
            //
            if(ui32ByteToSend > 64)
            {
                ui32ByteToSend = 64;
            }
            else
            {
                //
                // Send the requested number of bytes.
                //
                ui32ByteToSend = ui32Size;
            }

            //
            // Start a write request.
            //
            g_sUSBHCD.psUSBOUTPipes[ui32PipeIdx].iState = ePipeWriting;

            //
            // Disable uDMA on the USB endpoint
            //
            MAP_USBEndpointDMADisable(USB0_BASE, ui32Endpoint,
                                      USB_EP_HOST_OUT);

            //
            // Put the data in the buffer.
            //
            MAP_USBEndpointDataPut(USB0_BASE, ui32Endpoint, pui8Data,
                                   ui32ByteToSend);

            //
            // Schedule the data to be sent.
            //
            MAP_USBEndpointDataSend(USB0_BASE, ui32Endpoint, USB_TRANS_OUT);
        }

        //
        // Wait for a status change.
        //
        while(1)
        {
            //
            // If an error event occurs then exit out of the loop.
            //
            if(g_sUSBHCD.ui32IntEvents & (INT_EVENT_DISCONNECT |
                                          INT_EVENT_VBUS_ERR |
                                          INT_EVENT_POWER_FAULT))
            {
                //
                // Set the pipe state to error.
                //
                g_sUSBHCD.psUSBOUTPipes[ui32PipeIdx].iState = ePipeError;

                //
                // Needs to be set to exit out of large while loop.
                //
                ui32RemainingBytes = 0;

                break;
            }
            //
            // If the data was successfully sent then decrement the count and
            // continue.
            //
            else if(g_sUSBHCD.psUSBOUTPipes[ui32PipeIdx].iState ==
                    ePipeDataSent)
            {
                //
                // Decrement the remaining data and advance the pointer.
                //
                ui32RemainingBytes -= ui32ByteToSend;
                pui8Data += ui32ByteToSend;

                //
                // If there are less than 64 bytes to send then this is the
                // last of the data to go out.
                //
                if(ui32RemainingBytes < 64)
                {
                    ui32ByteToSend = ui32RemainingBytes;
                }
                break;
            }
            else if(g_sUSBHCD.psUSBOUTPipes[ui32PipeIdx].iState ==
                    ePipeStalled)
            {
                //
                // Zero out the size so that the caller knows that no data was
                // written.
                //
                ui32Size = 0;

                //
                // Needs to be set to exit out of large while loop.
                //
                ui32RemainingBytes = 0;

                //
                // If DMA is being used, then disable the channel.
                //
                if(bUseDMA == true)
                {
                    //
                    // Disable the DMA channel.
                    //
                    USBLibDMAChannelDisable(g_sUSBHCD.psDMAInstance,
                        g_sUSBHCD.psUSBOUTPipes[ui32PipeIdx].ui8DMAChannel);
                }

                //
                // This is the actual endpoint number.
                //
                USBHCDClearFeature(
                    g_sUSBHCD.psUSBOUTPipes[ui32PipeIdx].psDevice->ui32Address,
                    ui32Pipe, USB_FEATURE_EP_HALT);

                //
                // If there was a stall, then no more data is coming so break
                // out.
                //
                break;
            }
            else if(g_sUSBHCD.psUSBOUTPipes[ui32PipeIdx].iState == ePipeError)
            {
                //
                // An error occurred so stop this transaction and set the
                // number of bytes to zero.
                //
                ui32Size = 0;

                //
                // Needs to be set to exit out of large while loop.
                //
                ui32RemainingBytes = 0;

                break;
            }
        }
    }

    //
    // Go Idle once this state has been reached.
    //
    g_sUSBHCD.psUSBOUTPipes[ui32PipeIdx].iState = ePipeIdle;

    return(ui32Size);
}

//*****************************************************************************
//
//! This function is used to schedule and IN transaction on a USB HCD pipe.
//!
//! \param ui32Pipe is the USB pipe to read data from.
//! \param pui8Data is a pointer to store the data that is received.
//! \param ui32Size is the size in bytes of the buffer pointed to by
//! \e pui8Data.
//!
//! This function will not block depending on the type of pipe passed in will
//! schedule either a send of data to the device or a read of data from the
//! device.  In either case the amount of data will be limited to what will
//! fit in the FIFO for a given endpoint.
//!
//! \return This function returns the number of bytes that were sent in the
//! case of a transfer of data or it will return 0 for a request on a USB IN
//! pipe.
//
//*****************************************************************************
uint32_t
USBHCDPipeSchedule(uint32_t ui32Pipe, uint8_t *pui8Data, uint32_t ui32Size)
{
    uint32_t ui32Endpoint, ui32PipeIdx;

    //
    // Get index used for looking up pipe data
    //
    ui32PipeIdx = ui32Pipe & EP_PIPE_IDX_M;

    //
    // Determine which endpoint interface that this pipe is using.
    //
    ui32Endpoint = IndexToUSBEP((EP_PIPE_IDX_M & ui32Pipe) + 1);

    if(ui32Pipe & EP_PIPE_TYPE_OUT)
    {
        //
        // Check if uDMA is enabled on this pipe.
        //
        if(ui32Pipe & EP_PIPE_USE_UDMA)
        {
            //
            // Start a write request.
            //
            g_sUSBHCD.psUSBOUTPipes[EP_PIPE_IDX_M & ui32Pipe].iState =
                                                            ePipeWriteDMASend;

            USBLibDMATransfer(g_sUSBHCD.psDMAInstance,
                            g_sUSBHCD.psUSBOUTPipes[ui32PipeIdx].ui8DMAChannel,
                            pui8Data, ui32Size);
        }
        else
        {
            //
            // Start a write request.
            //
            g_sUSBHCD.psUSBOUTPipes[EP_PIPE_IDX_M & ui32Pipe].iState =
                                                                ePipeWriting;

            //
            // Put the data in the buffer.
            //
            MAP_USBEndpointDataPut(USB0_BASE, ui32Endpoint, pui8Data,
                                   ui32Size);

            //
            // Schedule the data to be sent.
            //
            MAP_USBEndpointDataSend(USB0_BASE, ui32Endpoint, USB_TRANS_OUT);
        }
    }
    else
    {
        //
        // If uDMA is not enabled for this pipe, or if the uDMA workaround
        // is applied, then do not use uDMA for this transfer.
        //
        if((ui32Pipe & EP_PIPE_USE_UDMA) == 0)
        {
            //
            // Start a read request.
            //
            g_sUSBHCD.psUSBINPipes[EP_PIPE_IDX_M & ui32Pipe].iState =
                                                                ePipeReading;

            //
            // Disable uDMA on the endpoint
            //
            MAP_USBEndpointDMADisable(USB0_BASE, ui32Endpoint, USB_EP_HOST_IN);
        }
        //
        // Otherwise, uDMA should be used for this transfer, so set up
        // the uDMA channel in advance of triggering the IN request.
        //
        else
        {
            g_sUSBHCD.psUSBINPipes[EP_PIPE_IDX_M & ui32Pipe].iState =
                                                                ePipeReadDMA;

            USBLibDMATransfer(g_sUSBHCD.psDMAInstance,
                            g_sUSBHCD.psUSBINPipes[ui32PipeIdx].ui8DMAChannel,
                            pui8Data, ui32Size);
        }

        //
        // Remember details of the buffer into which the data will be read.
        //
        g_sUSBHCD.psUSBINPipes[ui32PipeIdx].pui8ReadPtr = pui8Data;
        g_sUSBHCD.psUSBINPipes[ui32PipeIdx].ui32ReadSize = ui32Size;

        //
        // Trigger a request for data from the device.
        //
        MAP_USBHostRequestIN(USB0_BASE, ui32Endpoint);

        //
        // No data was put into or read from the buffer.
        //
        ui32Size = 0;
    }
    return(ui32Size);
}

//*****************************************************************************
//
//! This function is used to read data from a USB HCD pipe.
//!
//! \param ui32Pipe is the USB pipe to read data from.
//! \param pui8Data is a pointer to store the data that is received.
//! \param ui32Size is the size in bytes of the buffer pointed to by
//! \e pui8Data.
//!
//! This function will not block and will only read as much data as requested
//! or as much data is currently available from the USB pipe.  The caller
//! should have registered a callback with the USBHCDPipeAlloc() call in order
//! to be informed when the data has been received.  The value returned by this
//! function can be less than the \e ui32Size requested if the USB pipe has
//! less data available than was requested.
//!
//! \return This function returns the number of bytes that were returned in the
//! \e pui8Data buffer.
//
//*****************************************************************************
uint32_t
USBHCDPipeReadNonBlocking(uint32_t ui32Pipe, uint8_t *pui8Data,
                          uint32_t ui32Size)
{
    uint32_t ui32Endpoint;

    //
    // Determine which endpoint interface that this pipe is using.
    //
    ui32Endpoint = IndexToUSBEP((EP_PIPE_IDX_M & ui32Pipe) + 1);

    //
    // Read the data out of the USB endpoint interface.
    //
    MAP_USBEndpointDataGet(USB0_BASE, ui32Endpoint, pui8Data, &ui32Size);

    //
    // Acknowledge that the data was read from the endpoint.
    //
    MAP_USBHostEndpointDataAck(USB0_BASE, ui32Endpoint);

    //
    // Go Idle once this state has been reached.
    //
    g_sUSBHCD.psUSBINPipes[EP_PIPE_IDX_M & ui32Pipe].iState = ePipeIdle;

    return(ui32Size);
}

//*****************************************************************************
//
//! This function acknowledges data received via an interrupt IN pipe.
//!
//! \param ui32Pipe is the USB INT pipe whose last packet is to be
//! acknowledged.
//!
//! This function is used to acknowledge reception of data on an interrupt IN
//! pipe.  A transfer on an interrupt IN endpoint is scheduled via a call to
//! USBHCDPipeSchedule() and the application is notified when data is received
//! using a USB_EVENT_RX_AVAILABLE event.  In the handler for this event, the
//! application must call USBHCDPipeDataAck() to have the USB controller ACK
//! the data from the device and complete the transaction.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDPipeDataAck(uint32_t ui32Pipe)
{
    uint32_t ui32Endpoint;

    //
    // Determine which endpoint interface that this pipe is using.
    //
    ui32Endpoint = IndexToUSBEP((EP_PIPE_IDX_M & ui32Pipe) + 1);

    //
    // Acknowledge that the data was read from the endpoint.
    //
    USBHostEndpointDataAck(USB0_BASE, ui32Endpoint);

    //
    // Go Idle once this state has been reached.
    //
    g_sUSBHCD.psUSBINPipes[EP_PIPE_IDX_M & ui32Pipe].iState = ePipeIdle;
}

//*****************************************************************************
//
//! This function is used to read data from a USB HCD pipe.
//!
//! \param ui32Pipe is the USB pipe to read data from.
//! \param pui8Data is a pointer to store the data that is received.
//! \param ui32Size is the size in bytes of the buffer pointed to by
//! \e pui8Data.
//!
//! This function will block and will only return when it has read as much data
//! as requested from the USB pipe.  The caller must register a callback with
//! the USBHCDPipeAlloc() call in order to be informed when the data has been
//! received.  If the caller provides a non-zero pointer in the \e pui8Data
//! parameter then the data is copied into the buffer before the callback
//! occurs.  If the caller provides a zero in \e pui8Data parameter
//! then the caller is responsible for reading the data out of the FIFO when
//! the \b USB_EVENT_RX_AVAILABLE callback event occurs.   The value returned
//! by this function can be less than the \e ui32Size requested if the USB pipe
//! has less data available than was requested.
//!
//! \return This function returns the number of bytes that were returned in the
//! \e pui8Data buffer.
//
//*****************************************************************************
uint32_t
USBHCDPipeRead(uint32_t ui32Pipe, uint8_t *pui8Data, uint32_t ui32Size)
{
    uint32_t ui32Endpoint, ui32RemainingBytes, ui32BytesRead, ui32PipeIdx;
    bool bUseDMA;

    //
    // Get index used for looking up pipe data
    //
    ui32PipeIdx = ui32Pipe & EP_PIPE_IDX_M;

    //
    // Initialized the number of bytes read.
    //
    ui32BytesRead = 0;

    //
    // Determine which endpoint interface that this pipe is using.
    //
    ui32Endpoint = IndexToUSBEP(ui32PipeIdx + 1);

    //
    // Set the remaining bytes to received.
    //
    ui32RemainingBytes = ui32Size;

    //
    // Default to using DMA.
    //
    bUseDMA = true;

    //
    // Continue until all data requested has been received.
    //
    while(ui32RemainingBytes != 0)
    {
        //
        // Start a read request.
        //
        g_sUSBHCD.psUSBINPipes[ui32PipeIdx].iState = ePipeReading;

        //
        // Try the DMA transfer should be used for this transfer, so set up
        // the uDMA channel in advance of triggering the IN request.
        //
        if(ui32Pipe & EP_PIPE_USE_UDMA)
        {
            //
            // Disable the USB interrupt.
            //
            OS_INT_DISABLE(g_sUSBHCD.ui32IntNum);

            if(USBLibDMATransfer(g_sUSBHCD.psDMAInstance,
                            g_sUSBHCD.psUSBINPipes[ui32PipeIdx].ui8DMAChannel,
                            pui8Data, ui32Size) != 0)
            {
                g_sUSBHCD.psUSBINPipes[ui32PipeIdx].iState = ePipeReadDMA;

                ui32BytesRead = ui32Size;
            }
            else
            {
                bUseDMA = false;
            }

            //
            // Disable the USB interrupt.
            //
            OS_INT_ENABLE(g_sUSBHCD.ui32IntNum);
        }

        //
        // If unable to use DMA then get ready to transfer without DMA.
        //
        if(bUseDMA == false)
        {
            //
            // Disable uDMA on the endpoint
            //
            MAP_USBEndpointDMADisable(USB0_BASE, ui32Endpoint, USB_EP_HOST_IN);

            //
            // Set up for the next transaction.
            //
            g_sUSBHCD.psUSBINPipes[ui32PipeIdx].pui8ReadPtr = pui8Data;
            g_sUSBHCD.psUSBINPipes[ui32PipeIdx].ui32ReadSize =
                        (ui32RemainingBytes < 64) ? ui32RemainingBytes : 64;
        }

        //
        // Trigger a request for data from the device.
        //
        MAP_USBHostRequestIN(USB0_BASE, ui32Endpoint);

        //
        // Wait for a status change.
        //
        while(1)
        {
            //
            // Check if the device stalled the request.
            //
            if(g_sUSBHCD.psUSBINPipes[ui32PipeIdx].iState == ePipeStalled)
            {
                //
                // Zero out the size so that the caller knows that no data was
                // read.
                //
                ui32Size = 0;

                //
                // There are also no remaining bytes to read.
                //
                ui32RemainingBytes = 0;

                //
                // If DMA is being used, then disable the channel.
                //
                if(bUseDMA == true)
                {
                    USBLibDMAChannelDisable(
                            g_sUSBHCD.psDMAInstance,
                            g_sUSBHCD.psUSBINPipes[ui32PipeIdx].ui8DMAChannel);
                }

                //
                // This is the actual endpoint number.
                //
                USBHCDClearFeature(
                    g_sUSBHCD.psUSBINPipes[ui32PipeIdx].psDevice->ui32Address,
                    ui32Pipe, USB_FEATURE_EP_HALT);

                //
                // If there was a stall, then no more data is coming so break
                // out.
                //
                break;
            }

            //
            // If any error event occurs then exit out of the loop.
            //
            if(g_sUSBHCD.ui32IntEvents & (INT_EVENT_DISCONNECT |
                                          INT_EVENT_VBUS_ERR |
                                          INT_EVENT_POWER_FAULT))
            {
                //
                // Set the pipe state to error.
                //
                g_sUSBHCD.psUSBINPipes[ui32PipeIdx].iState = ePipeError;
                break;
            }

            //
            // If data is ready then return it.
            //
            if(g_sUSBHCD.psUSBINPipes[ui32PipeIdx].iState == ePipeDataReady)
            {
                //
                // If not using DMA then read the data from the USB.  Otherwise
                // the data will already be in the buffer.
                //
                if(bUseDMA == false)
                {
                    //
                    // Compute bytes to transfer and set up transfer
                    //
                    ui32BytesRead =
                            ui32RemainingBytes > 64 ? 64 : ui32RemainingBytes;

                    //
                    // Acknowledge that the data was read from the endpoint.
                    //
                    MAP_USBHostEndpointDataAck(USB0_BASE, ui32Endpoint);
                }

                //
                // Subtract the number of bytes read from the bytes remaining.
                //
                ui32RemainingBytes -= ui32BytesRead;

                //
                // If there were less than 64 bytes read, then this was a short
                // packet and no more data will be returned.
                //
                if(ui32BytesRead < 64)
                {
                    //
                    // Subtract off the bytes that were not received and exit
                    // the loop.
                    //
                    ui32Size = ui32Size - ui32RemainingBytes;
                    break;
                }
                else
                {
                    //
                    // Move the buffer ahead to receive more data into the
                    // buffer.
                    //
                    pui8Data += 64;
                }
                break;
            }
            else if(g_sUSBHCD.psUSBINPipes[ui32PipeIdx].iState == ePipeError)
            {
                //
                // An error occurred so stop this transaction and set the
                // number of bytes to zero.
                //
                ui32Size = 0;
                ui32RemainingBytes = 0;

                break;
            }
            else if((g_sUSBHCD.psUSBINPipes[ui32PipeIdx].iState ==
                     ePipeReadDMAWait) &&
                    (USBLibDMAChannelStatus(g_sUSBHCD.psDMAInstance,
                        g_sUSBHCD.psUSBINPipes[ui32PipeIdx].ui8DMAChannel) &
                     USBLIBSTATUS_DMA_COMPLETE))
            {
                break;
            }
        }
    }

    //
    // Go Idle once this state has been reached.
    //
    g_sUSBHCD.psUSBINPipes[ui32PipeIdx].iState = ePipeIdle;

    return(ui32Size);
}

//*****************************************************************************
//
//! This function is used to release a USB pipe.
//!
//! \param ui32Pipe is the allocated USB pipe to release.
//!
//! This function is used to release a USB pipe that was allocated by a call to
//! USBHCDPipeAlloc() for use by some other device endpoint in the system.
//! Freeing an unallocated or invalid pipe will not generate an error and will
//! instead simply return.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDPipeFree(uint32_t ui32Pipe)
{
    uint32_t ui32Index;

    //
    // Get the index number from the allocated pipe.
    //
    ui32Index = (ui32Pipe & EP_PIPE_IDX_M);

    if(ui32Pipe & EP_PIPE_TYPE_OUT)
    {
        //
        // Clear the address and type for this endpoint to free it up.
        //
        g_sUSBHCD.psUSBOUTPipes[ui32Index].psDevice = 0;
        g_sUSBHCD.psUSBOUTPipes[ui32Index].ui32Type = 0;
        g_sUSBHCD.psUSBOUTPipes[ui32Index].pfnCallback = 0;

        //
        // Check if this pipe has allocated a DMA channel.
        //
        if(g_sUSBHCD.psUSBOUTPipes[ui32Index].ui8DMAChannel !=
           USBHCD_DMA_UNUSED)
        {
            //
            // Release the DMA channel associated with this endpoint.
            //
            USBLibDMAChannelRelease(g_sUSBHCD.psDMAInstance,
                    g_sUSBHCD.psUSBOUTPipes[ui32Index].ui8DMAChannel);

            //
            // Clear out the current channel in use by this pipe.
            //
            g_sUSBHCD.psUSBOUTPipes[ui32Index].ui8DMAChannel =
                                                            USBHCD_DMA_UNUSED;
        }

        //
        // Free up the FIFO memory used by this endpoint.
        //
        if(g_sUSBHCD.psUSBOUTPipes[ui32Index].ui8FIFOSize)
        {
            FIFOFree(&g_sUSBHCD.psUSBOUTPipes[ui32Index]);
        }

        //
        // Set the function address for this endpoint back to zero.
        //
        USBHostAddrSet(USB0_BASE, IndexToUSBEP(ui32Index + 1),
                       0, USB_EP_HOST_OUT);

        //
        // Set the hub and port address for the endpoint back to zero and the
        // speed back to LOW.
        //
        USBHostHubAddrSet(USB0_BASE, IndexToUSBEP(ui32Index + 1),
                          0, (USB_EP_HOST_OUT | USB_EP_SPEED_LOW));
    }
    else if(ui32Pipe & EP_PIPE_TYPE_IN)
    {
        //
        // Clear the address and type for this endpoint to free it up.
        //
        g_sUSBHCD.psUSBINPipes[ui32Index].psDevice = 0;
        g_sUSBHCD.psUSBINPipes[ui32Index].ui32Type = 0;
        g_sUSBHCD.psUSBINPipes[ui32Index].pfnCallback = 0;

        //
        // Check if this pipe has allocated a DMA channel.
        //
        if(g_sUSBHCD.psUSBINPipes[ui32Index].ui8DMAChannel !=
           USBHCD_DMA_UNUSED)
        {
            //
            // Release the DMA channel associated with this endpoint.
            //
            USBLibDMAChannelRelease(g_sUSBHCD.psDMAInstance,
                    g_sUSBHCD.psUSBINPipes[ui32Index].ui8DMAChannel);

            //
            // Clear out the current channel in use by this pipe.
            //
            g_sUSBHCD.psUSBINPipes[ui32Index].ui8DMAChannel =
                                                            USBHCD_DMA_UNUSED;
        }

        //
        // Free up the FIFO memory used by this endpoint.
        //
        if(g_sUSBHCD.psUSBINPipes[ui32Pipe & EP_PIPE_IDX_M].ui8FIFOSize)
        {
            FIFOFree(&g_sUSBHCD.psUSBINPipes[ui32Pipe & EP_PIPE_IDX_M]);
        }

        //
        // Set the function address for this endpoint back to zero.
        //
        USBHostAddrSet(USB0_BASE, IndexToUSBEP(ui32Index + 1),
                       0, USB_EP_HOST_IN);

        //
        // Set the hub and port address for the endpoint back to zero and the
        // speed back to LOW.
        //
        USBHostHubAddrSet(USB0_BASE, IndexToUSBEP(ui32Index + 1),
                          0, (USB_EP_HOST_IN | USB_EP_SPEED_LOW));

        //
        // Clear any pending IN transactions.
        //
        USBHostRequestINClear(USB0_BASE, IndexToUSBEP(ui32Index + 1));
    }
}

//*****************************************************************************
//
// This internal function initializes the HCD code.
//
// \param ui32Index specifies which USB controller to use.
// \param pvPool is a pointer to the data to use as a memory pool for this
// controller.
// \param ui32PoolSize is the size in bytes of the buffer passed in as pvPool.
//
// This function will perform all the necessary operations to allow the USB
// host controller to begin enumeration and communication with a device.  This
// function should typically be called once at the start of an application
// before any other calls are made to the host controller.
//
// \return None.
//
//*****************************************************************************
static void
USBHCDInitInternal(uint32_t ui32Index, void *pvPool, uint32_t ui32PoolSize)
{
    int32_t i32Idx;

    ASSERT(ui32Index == 0);

    //
    // Get the number of endpoints supported by this device.
    //
    g_sUSBHCD.ui32NumEndpoints = USBNumEndpointsGet(USB0_BASE);

    //
    // The first 64 Bytes are allocated to endpoint 0.
    //
    g_pui32Alloc[0] = 1;
    g_pui32Alloc[1] = 0;

    //
    // Save the base address for this controller.
    //
    g_sUSBHCD.ui32USBBase = USB0_BASE;

    //
    // Save the USB interrupt number.
    //
    g_sUSBHCD.ui32IntNum = INT_USB0_BLIZZARD;

    //
    // All Pipes are unused at start.
    //
    for(i32Idx = 0; i32Idx < MAX_NUM_PIPES; i32Idx++)
    {
        g_sUSBHCD.psUSBINPipes[i32Idx].psDevice = 0;
        g_sUSBHCD.psUSBINPipes[i32Idx].ui32Type = USBHCD_PIPE_UNUSED;
        g_sUSBHCD.psUSBINPipes[i32Idx].ui8DMAChannel = USBHCD_DMA_UNUSED;
        g_sUSBHCD.psUSBOUTPipes[i32Idx].psDevice = 0;
        g_sUSBHCD.psUSBOUTPipes[i32Idx].ui32Type = USBHCD_PIPE_UNUSED;
        g_sUSBHCD.psUSBOUTPipes[i32Idx].ui8DMAChannel = USBHCD_DMA_UNUSED;
    }

    //
    // Make sure that the hub driver is initialized since it is called even
    // if it is not present in the system.
    //
    USBHHubInit();

    //
    // Initialize the DMA interface.
    //
    g_sUSBHCD.psDMAInstance = USBLibDMAInit(g_sUSBHCD.ui32USBBase);

    //
    // Initialized the device structures.
    //
    for(i32Idx = 0; i32Idx <= MAX_USB_DEVICES; i32Idx++)
    {
        //
        // Clear the configuration descriptor and state.
        //
        g_sUSBHCD.piDeviceState[i32Idx] = eHCDIdle;
        g_sUSBHCD.psUSBDevice[i32Idx].psConfigDescriptor = 0;
        g_sUSBHCD.psUSBDevice[i32Idx].bConfigRead = false;

        //
        // Initialize the device descriptor.
        //
        g_sUSBHCD.psUSBDevice[i32Idx].sDeviceDescriptor.bLength = 0;
        g_sUSBHCD.psUSBDevice[i32Idx].sDeviceDescriptor.bMaxPacketSize0 = 0;

        //
        // Initialize the device address.
        //
        g_sUSBHCD.psUSBDevice[i32Idx].ui32Address = 0;

        //
        // Set the current interface to 0.
        //
        g_sUSBHCD.psUSBDevice[i32Idx].ui32Interface = 0;

        //
        // Clear the active driver for the device.
        //
        g_pi32USBHActiveDriver[i32Idx] = -1;

        //
        // Initialize the device flags.
        //
        g_sUSBHCD.psUSBDevice[i32Idx].ui32Flags = 0;
    }

    //
    // Allocate the memory needed for reading descriptors.
    //
    g_sUSBHCD.pvPool = pvPool;
    g_sUSBHCD.ui32PoolSize = ui32PoolSize;

    //
    // Initialize the device class.
    //
    g_sUSBHCD.ui32Class = USB_CLASS_EVENTS;

    //
    // Default enable connect, disconnect, unknown device and power fault
    // event notifications.
    //
    g_sUSBHCD.ui32EventEnables = USBHCD_EVFLAG_CONNECT |
                                 USBHCD_EVFLAG_UNKCNCT |
                                 USBHCD_EVFLAG_DISCNCT |
                                 USBHCD_EVFLAG_PWRFAULT |
                                 USBHCD_EVFLAG_PWREN |
                                 USBHCD_EVFLAG_PWRDIS;

    //
    // Initialize the USB tick module.
    //
    InternalUSBTickInit();

    //
    // Only do hardware update if the stack is in Host mode, do not touch the
    // hardware for OTG mode operation.
    //
    if((g_iUSBMode == eUSBModeHost) || (g_iUSBMode == eUSBModeForceHost))
    {
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
                                           USB_INTCTRL_SOF |
                                           USB_INTCTRL_SESSION |
                                           USB_INTCTRL_BABBLE |
                                           USB_INTCTRL_CONNECT |
                                           USB_INTCTRL_RESUME |
                                           USB_INTCTRL_SUSPEND |
                                           USB_INTCTRL_VBUS_ERR |
                                           USB_INTCTRL_MODE_DETECT |
                                           USB_INTCTRL_POWER_FAULT);

        MAP_USBIntEnableEndpoint(USB0_BASE, USB_INTEP_ALL);

        //
        // Enable the USB interrupt.
        //
        OS_INT_ENABLE(g_sUSBHCD.ui32IntNum);

        //
        // There is no automatic power in pure host mode.
        //
        USBHCDPowerConfigSet(ui32Index, (g_ui32PowerConfig &
                                         ~USB_HOST_PWREN_AUTO));

        //
        // Force the power on as well as this point.
        //
        MAP_USBHostPwrEnable(USB0_BASE);

        //
        // This is required to get into host mode on some parts.
        //
        USBOTGSessionRequest(USB0_BASE, true);
    }
}

//*****************************************************************************
//
//! This function is used to set the power pin and power fault configuration.
//!
//! \param ui32Index specifies which USB controller to use.
//! \param ui32PwrConfig is the power configuration to use for the application.
//!
//! This function must be called before HCDInit() is called so that the power
//! pin configuration can be set before power is enabled.  The \e ui32PwrConfig
//! flags specify the power fault level sensitivity, the power fault action,
//! and the power enable pin level and source.
//!
//! One of the following can be selected as the power fault level sensitivity:
//!
//! - \b USBHCD_FAULT_LOW - An external power fault is indicated by the pin
//!                         being driven low.
//! - \b USBHCD_FAULT_HIGH - An external power fault is indicated by the pin
//!                          being driven high.
//!
//! One of the following can be selected as the power fault action:
//!
//! - \b USBHCD_FAULT_VBUS_NONE - No automatic action when power fault
//!   detected.
//! - \b USBHCD_FAULT_VBUS_TRI - Automatically Tri-state the USBnEPEN pin on a
//!                              power fault.
//! - \b USBHCD_FAULT_VBUS_DIS - Automatically drive the USBnEPEN pin to it's
//!                              inactive state on a power fault.
//!
//! One of the following can be selected as the power enable level and source:
//!
//! - \b USBHCD_VBUS_MANUAL - Power control is completely managed by the
//!                           application, the USB library will provide a
//!                           power callback to request power state changes.
//! - \b USBHCD_VBUS_AUTO_LOW - USBEPEN is driven low by the USB controller
//!                             automatically if USBOTGSessionRequest() has
//!                             enabled a session.
//! - \b USBHCD_VBUS_AUTO_HIGH - USBEPEN is driven high by the USB controller
//!                              automatically if USBOTGSessionRequest() has
//!                              enabled a session.
//!
//! If USBHCD_VBUS_MANUAL is used then the application must provide an
//! event driver to receive the USB_EVENT_POWER_ENABLE and
//! USB_EVENT_POWER_DISABLE events and enable and disable power to VBUS when
//! requested by the USB library.  The application should respond to a power
//! control callback by enabling or disabling VBUS as soon as possible and
//! before returning from the callback function.
//!
//! \note The following values should no longer be used with the USB
//! library: \b USB_HOST_PWRFLT_LOW, \b USB_HOST_PWRFLT_HIGH,
//! \b USB_HOST_PWRFLT_EP_NONE, \b USB_HOST_PWRFLT_EP_TRI,
//! \b USB_HOST_PWRFLT_EP_LOW, \b USB_HOST_PWRFLT_EP_HIGH,
//! \b USB_HOST_PWREN_LOW, \b USB_HOST_PWREN_HIGH, \b USB_HOST_PWREN_VBLOW, and
//! \b USB_HOST_PWREN_VBHIGH.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDPowerConfigInit(uint32_t ui32Index, uint32_t ui32PwrConfig)
{
    ASSERT(ui32Index == 0);

    //
    // Save the value as it will be used later.
    //
    g_ui32PowerConfig = ui32PwrConfig;
}

//*****************************************************************************
//
//! This function is used to get the power pin and power fault configuration.
//!
//! \param ui32Index specifies which USB controller to use.
//!
//! This function will return the current power control pin configuration as
//! set by the USBHCDPowerConfigInit() function or the defaults if not yet set.
//! See the USBHCDPowerConfigInit() documentation for the meaning of the bits
//! that are returned by this function.
//!
//! \return The configuration of the power control pins.
//!
//*****************************************************************************
uint32_t
USBHCDPowerConfigGet(uint32_t ui32Index)
{
    ASSERT(ui32Index == 0);

    //
    // Save the value as it will be used later.
    //
    return(g_ui32PowerConfig);
}

//*****************************************************************************
//
//! This function is used to set the power pin and power fault configuration.
//!
//! \param ui32Index specifies which USB controller to use.
//! \param ui32Config specifies which USB power configuration to use.
//!
//! This function will set the current power control pin configuration as
//! set by the USBHCDPowerConfigInit() function or the defaults if not yet set.
//! See the USBHCDPowerConfigInit() documentation for the meaning of the bits
//! that are set by this function.
//!
//! \return Returns zero to indicate the power setting is now active.
//!
//*****************************************************************************
uint32_t
USBHCDPowerConfigSet(uint32_t ui32Index, uint32_t ui32Config)
{
    ASSERT(ui32Index == 0);

    //
    // Remember the current setting.
    //
    g_ui32PowerConfig = ui32Config;

    //
    // Clear out the two flag bits.
    //
    ui32Config = g_ui32PowerConfig & ~(USBHCD_VBUS_MANUAL |
                                       USBHCD_FAULT_VBUS_DIS);

    //
    // If there is an automatic disable power action specified then set the
    // polarity of the signal to match EPEN.
    //
    if(g_ui32PowerConfig & USBHCD_FAULT_VBUS_DIS)
    {
        //
        // Insure that the assumption below is true.
        //
        ASSERT((USBHCD_VBUS_AUTO_HIGH & 1) == 1);
        ASSERT((USBHCD_VBUS_AUTO_LOW & 1) == 0);

        //
        // This is taking advantage of the difference between
        // USBHCD_VBUS_AUTO_LOW and USBHCD_VBUS_AUTO_HIGH being that bit
        // one is set when EPEN is active high.
        //
        if(g_ui32PowerConfig & 1)
        {
            g_ui32PowerConfig |= USB_HOST_PWRFLT_EP_LOW;
            ui32Config |= USB_HOST_PWRFLT_EP_LOW;
        }
        else
        {
            g_ui32PowerConfig |= USB_HOST_PWRFLT_EP_HIGH;
            ui32Config |= USB_HOST_PWRFLT_EP_HIGH;
        }
    }

    //
    // Initialize the power configuration.
    //
    MAP_USBHostPwrConfig(USB0_BASE, ui32Config);

    //
    // If not in manual mode then just turn on power.
    //
    if((g_ui32PowerConfig & USBHCD_VBUS_MANUAL) == 0)
    {
        //
        // Power the USB bus.
        //
        MAP_USBHostPwrEnable(USB0_BASE);
    }

    //
    // Return success.
    //
    return(0);
}

//*****************************************************************************
//
//! This function returns if the current power settings will automatically
//! handle enabling and disabling VBUS power.
//!
//! \param ui32Index specifies which USB controller to query.
//!
//! This function returns if the current power control pin configuration will
//! automatically apply power or whether it will be left to the application
//! to turn on power when it is notified.
//!
//! \return A non-zero value indicates that power is automatically applied and
//! a value of zero indicates that the application must manually apply power.
//!
//*****************************************************************************
uint32_t
USBHCDPowerAutomatic(uint32_t ui32Index)
{
    //
    // Check if the controller is automatically applying power or not.
    //
    if(g_ui32PowerConfig & USBHCD_VBUS_MANUAL)
    {
        return(0);
    }
    return(1);
}

//*****************************************************************************
//
//! This function is used to initialize the HCD code.
//!
//! \param ui32Index specifies which USB controller to use.
//! \param pvPool is a pointer to the data to use as a memory pool for this
//! controller.
//! \param ui32PoolSize is the size in bytes of the buffer passed in as
//! \e pvPool.
//!
//! This function will perform all the necessary operations to allow the USB
//! host controller to begin enumeration and communication with devices.  This
//! function should typically be called once at the start of an application
//! once all of the device and class drivers are ready for normal operation.
//! This call will start up the USB host controller and any connected device
//! will immediately start the enumeration sequence.
//!
//! The USBStackModeSet() function can be called with eUSBModeHost in order to
//! cause the USB library to force the USB operating mode to a host controller.
//! This allows the application to used the USBVBUS and USBID pins as GPIOs on
//! devices that support forcing OTG to operate as a host only controller.  By
//! default the USB library will assume that the USBVBUS and USBID pins are
//! configured as USB pins and not GPIOs.
//!
//! The memory pool passed to this function must be at least as large as a
//! typical configuration descriptor for devices that are to be supported.
//! This value is application-dependent however it should never be less than 32
//! bytes and, in most cases, should be at least 64 bytes.  If there is not
//! sufficient memory to load a configuration descriptor from a device, the
//! device will not be recognized by the USB library's host controller driver.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDInit(uint32_t ui32Index, void *pvPool, uint32_t ui32PoolSize)
{
    int32_t i32Driver;

    //
    // Check the arguments.
    //
    ASSERT(ui32Index == 0);

    //
    // Make sure there is at least enough to read the configuration descriptor.
    //
    ASSERT(ui32PoolSize >= sizeof(tConfigDescriptor));

    //
    // Should not call this if the stack is in device mode.
    //
    ASSERT(g_iUSBMode != eUSBModeDevice);
    ASSERT(g_iUSBMode != eUSBModeForceDevice);

    //
    // If the mode was not set then default to eUSBModeHost.
    //
    if(g_iUSBMode == eUSBModeNone)
    {
        g_iUSBMode = eUSBModeHost;
    }

    //
    // Reset the USB controller.
    //
    MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_USB0);

    //
    // Enable Clocking to the USB controller.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);

    //
    // Turn on USB Phy clock.
    //
    MAP_SysCtlUSBPLLEnable();

    //
    // If the application not requesting OTG mode then set the mode to forced
    // host mode.  If the mode is actually eUSBModeHost, this will be switched
    // off when ID pin detection is complete and the ID is no longer in use.
    //
    if(g_iUSBMode != eUSBModeOTG)
    {
        //
        // Force Host mode on devices that support force host mode.
        //
        MAP_USBHostMode(USB0_BASE);
    }

    //
    // Call our internal function to perform the initialization.
    //
    USBHCDInitInternal(ui32Index, pvPool, ui32PoolSize);

    //
    // No event driver is present by default.
    //
    g_sUSBHCD.i32EventDriver = -1;

    //
    // Search through the Host Class driver list for the devices class.
    //
    for(i32Driver = 0; i32Driver < g_sUSBHCD.ui32NumClassDrivers; i32Driver++)
    {
        if(g_sUSBHCD.ppsClassDrivers[i32Driver]->ui32InterfaceClass ==
           USB_CLASS_EVENTS)
        {
            //
            // Event driver was found so remember it.
            //
            g_sUSBHCD.i32EventDriver = i32Driver;
        }
    }

    //
    // Get the number of ticks per millisecond, this is only used by blocking
    // delays using the SysCtlDelay() function.
    //
    g_ui32Tickms = MAP_SysCtlClockGet() / 3000;
}

//*****************************************************************************
//
//! This function is used to initialize the HCD class driver list.
//!
//! \param ui32Index specifies which USB controller to use.
//! \param ppsHClassDrvs is an array of host class drivers that are
//! supported on this controller.
//! \param ui32NumDrivers is the number of entries in the \e pHostClassDrivers
//! array.
//!
//! This function will set the host classes supported by the host controller
//! specified by the \e ui32Index parameter.  This function should be called
//! before enabling the host controller driver with the USBHCDInit() function.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDRegisterDrivers(uint32_t ui32Index,
                      const tUSBHostClassDriver * const *ppsHClassDrvs,
                      uint32_t ui32NumDrivers)
{
    ASSERT(ui32Index == 0);

    //
    // Save the class drivers.
    //
    g_sUSBHCD.ppsClassDrivers = ppsHClassDrvs;

    //
    // Save the number of class drivers.
    //
    g_sUSBHCD.ui32NumClassDrivers = ui32NumDrivers;
}

//*****************************************************************************
//
//! This function is used to terminate the HCD code.
//!
//! \param ui32Index specifies which USB controller to release.
//!
//! This function will clean up the USB host controller and disable it in
//! preparation for shutdown or a switch to USB device mode.  Once this call is
//! made, \e USBHCDInit() may be called to reinitialize the controller and
//! prepare for host mode operation.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDTerm(uint32_t ui32Index)
{
    int32_t i32Idx;

    ASSERT(ui32Index == 0);

    //
    // End the session.
    //
    USBOTGSessionRequest(USB0_BASE, false);

    //
    // Remove power from the USB bus.
    //
    MAP_USBHostPwrDisable(USB0_BASE);

    //
    // Disable USB interrupts.
    //
    OS_INT_DISABLE(g_sUSBHCD.ui32IntNum);

    MAP_USBIntDisableControl(USB0_BASE, USB_INTCTRL_ALL);

    MAP_USBIntDisableEndpoint(USB0_BASE, USB_INTEP_ALL);

    //
    // Set the host controller state back to it's initial values.
    //
    for(i32Idx = 0; i32Idx < MAX_NUM_PIPES; i32Idx++)
    {
        g_sUSBHCD.psUSBINPipes[i32Idx].ui32Type = USBHCD_PIPE_UNUSED;
        g_sUSBHCD.psUSBOUTPipes[i32Idx].ui32Type = USBHCD_PIPE_UNUSED;
    }

    //
    // Free the memory used by the configuration descriptor.
    //
    ConfigDescFree(&g_sUSBHCD.psUSBDevice[0]);

    g_sUSBHCD.piDeviceState[0] = eHCDIdle;
    g_sUSBHCD.psUSBDevice[0].psConfigDescriptor = 0;
    g_sUSBHCD.psUSBDevice[0].bConfigRead = false;
    g_sUSBHCD.psUSBDevice[0].sDeviceDescriptor.bLength = 0;
    g_sUSBHCD.psUSBDevice[0].sDeviceDescriptor.bMaxPacketSize0 = 0;
    g_sUSBHCD.psUSBDevice[0].ui32Address = 0;
    g_sUSBHCD.psUSBDevice[0].ui32Interface = 0;
    g_sUSBHCD.pvPool = 0;
    g_sUSBHCD.ui32PoolSize = 0;
}

//*****************************************************************************
//
//! This function generates reset signaling on the USB bus.
//!
//! \param ui32Index specifies which USB controller to use.
//!
//! This function handles sending out reset signaling on the USB bus.  After
//! returning from this function, any attached device on the USB bus should
//! have returned to it's reset state.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDReset(uint32_t ui32Index)
{
    ASSERT(ui32Index == 0);

    //
    // Start the reset signaling.
    //
    MAP_USBHostReset(USB0_BASE, 1);

    //
    // Wait 20ms
    //
    OS_DELAY(g_ui32Tickms * 20);

    //
    // End reset signaling on the bus.
    //
    MAP_USBHostReset(USB0_BASE, 0);

    //
    // Need to wait at least 10ms to let the device recover from
    // the reset.  This is the delay specified in the USB 2.0 spec.
    // We will hold the reset for 20ms.
    //
    OS_DELAY(g_ui32Tickms * 20);
}

//*****************************************************************************
//
//! This function will generate suspend signaling on the USB bus.
//!
//! \param ui32Index specifies which USB controller to use.
//!
//! This function is used to generate suspend signaling on the USB bus.  In
//! order to leave the suspended state, the application should call
//! USBHCDResume().
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDSuspend(uint32_t ui32Index)
{
    ASSERT(ui32Index == 0);

    //
    // Start the suspend signaling.
    //
    MAP_USBHostSuspend(USB0_BASE);
}

//*****************************************************************************
//
//! This function will generate resume signaling on the USB bus.
//!
//! \param ui32Index specifies which USB controller to use.
//!
//! This function is used to generate resume signaling on the USB bus in order
//! to cause  USB devices to leave their suspended state.  This call should
//! not be made unless a preceding call to USBHCDSuspend() has been made.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDResume(uint32_t ui32Index)
{
    ASSERT(ui32Index == 0);

    //
    // Start the resume signaling.
    //
    MAP_USBHostResume(USB0_BASE, 1);

    //
    // Wait 100ms
    //
    OS_DELAY(g_ui32Tickms * 100);

    //
    // End reset signaling on the bus.
    //
    MAP_USBHostResume(USB0_BASE, 0);
}

//*****************************************************************************
//
//! This function issues a request for the current configuration descriptor
//! from a device.
//!
//! \param ui32Index specifies which USB controller to use.
//! \param psDevice is a pointer to the device structure that holds the buffer
//! to store the configuration descriptor.
//!
//! This function will request the configuration descriptor from the device.
//! The \e psDevice->ConfigDescriptor member variable is used to hold the data
//! for this request.  This buffer will be allocated from the pool provided by
//! the HCDInit() function.  \e psDevice->sDeviceDescriptor.bMaxPacketSize0
//! should be valid prior to this call in order to correctly receive the
//! configuration descriptor.  If this variable is not valid then this call
//! will not return accurate data.
//!
//! \return The number of bytes returned due to the request.  This value can be
//! zero if the device did not respond.
//
//*****************************************************************************
static uint32_t
USBHCDGetConfigDescriptor(uint32_t ui32Index, tUSBHostDevice *psDevice)
{
    tUSBRequest sSetupPacket;
    uint32_t ui32Bytes;
    tConfigDescriptor sConfigDescriptor;

    ASSERT(ui32Index == 0);

    ui32Bytes = 0;

    //
    // This is a Standard Device IN request.
    //
    sSetupPacket.bmRequestType =
        USB_RTYPE_DIR_IN | USB_RTYPE_STANDARD | USB_RTYPE_DEVICE;

    //
    // Request a Device Descriptor.
    //
    sSetupPacket.bRequest = USBREQ_GET_DESCRIPTOR;
    sSetupPacket.wValue = USB_DTYPE_CONFIGURATION << 8;

    //
    // Index is always 0 for device configurations requests.
    //
    sSetupPacket.wIndex = 0;

    //
    // Only ask for the configuration header first to see how big the
    // whole thing is.
    //
    if(!psDevice->bConfigRead)
    {
        //
        // Only request the space available.
        //
        sSetupPacket.wLength = sizeof(tConfigDescriptor);

        //
        // Put the setup packet in the buffer.
        //
        ui32Bytes =
            USBHCDControlTransfer(0, &sSetupPacket, psDevice,
                                  (uint8_t *)&sConfigDescriptor,
                                  sizeof(tConfigDescriptor),
                                  psDevice->sDeviceDescriptor.bMaxPacketSize0);
    }

    //
    // If the Configuration header was successfully returned then get the
    // full configuration descriptor.
    //
    if(ui32Bytes == sizeof(tConfigDescriptor))
    {
        //
        // Save the total size and request the full configuration descriptor.
        //
        sSetupPacket.wLength = sConfigDescriptor.wTotalLength;

        //
        // Not enough space to hold this configuration descriptor.
        //
        if(ConfigDescAlloc(psDevice, sConfigDescriptor.wTotalLength) == 0)
        {
            return(0);
        }

        //
        // Don't allow the buffer to be larger than was allocated.
        //
        if(sSetupPacket.wLength > psDevice->ui32ConfigDescriptorSize)
        {
            return(0);
        }

        //
        // Put the setup packet in the buffer.
        //
        ui32Bytes =
            USBHCDControlTransfer(0, &sSetupPacket, psDevice,
                                  (uint8_t *)psDevice->psConfigDescriptor,
                                  sSetupPacket.wLength,
                                  psDevice->sDeviceDescriptor.bMaxPacketSize0);

        //
        // If we read the descriptor, remember the fact.
        //
        if(ui32Bytes)
        {
            psDevice->bConfigRead = true;
        }
    }

    return(ui32Bytes);
}

//*****************************************************************************
//
//! This function issues a request for a device descriptor from a device.
//!
//! \param ui32Index specifies which USB controller to use.
//! \param psDevice is a pointer to the device structure that holds the buffer
//! to store the device descriptor into.
//!
//! This function will request the device descriptor from the device.  The
//! \e psDevice->sDeviceDescriptor descriptor is used to hold the data for this
//! request.  \e psDevice->sDeviceDescriptor.bMaxPacketSize0 should be
//! initialized to zero or to the valid maximum packet size if it is known.  If
//! this variable is not set to zero, then this call will determine the maximum
//! packet size for endpoint 0 and save it in the structure member
//! bMaxPacketSize0.
//!
//! \return The number of bytes returned due to the request.  This value can be
//! zero if the device did not respond.
//
//*****************************************************************************
static uint32_t
USBHCDGetDeviceDescriptor(uint32_t ui32Index, tUSBHostDevice *psDevice)
{
    tUSBRequest sSetupPacket;
    uint32_t ui32Bytes;

    ASSERT(ui32Index == 0);

    //
    // This is a Standard Device IN request.
    //
    sSetupPacket.bmRequestType =
        USB_RTYPE_DIR_IN | USB_RTYPE_STANDARD | USB_RTYPE_DEVICE;

    //
    // Request a Device Descriptor.
    //
    sSetupPacket.bRequest = USBREQ_GET_DESCRIPTOR;
    sSetupPacket.wValue = USB_DTYPE_DEVICE << 8;

    //
    // Index is always 0 for device requests.
    //
    sSetupPacket.wIndex = 0;

    //
    // All devices must have at least an 8 byte max packet size so just ask
    // for 8 bytes to start with.
    //
    sSetupPacket.wLength = sizeof(tDeviceDescriptor);

    ui32Bytes = 0;

    //
    // Discover the max packet size for endpoint 0.
    //
    if(psDevice->sDeviceDescriptor.bMaxPacketSize0 == 0)
    {
        //
        // Put the setup packet in the buffer.
        //
        ui32Bytes =
            USBHCDControlTransfer(ui32Index, &sSetupPacket, psDevice,
                                  (uint8_t *)&(psDevice->sDeviceDescriptor),
                                  sizeof(tDeviceDescriptor), 8);
    }

    //
    // Now get the full descriptor now that the actual maximum packet size
    // is known.
    //
    if(ui32Bytes < sizeof(tDeviceDescriptor))
    {
        sSetupPacket.wLength = (uint16_t)sizeof(tDeviceDescriptor);

        ui32Bytes =
            USBHCDControlTransfer(ui32Index, &sSetupPacket, psDevice,
                                  (uint8_t *)&(psDevice->sDeviceDescriptor),
                                  sizeof(tDeviceDescriptor),
                                  psDevice->sDeviceDescriptor.bMaxPacketSize0);
    }

    return(ui32Bytes);
}

//*****************************************************************************
//
//! This function is used to send the set address command to a device.
//!
//! \param ui32DevIndex is the index of the device whose address is to be
//! set.  This value must be 0 to indicate that the device is connected
//! directly to the host controller.  Higher values indicate devices connected
//!  via a hub.
//! \param ui32DevAddress is the new device address to use for a device.
//!
//! The USBHCDSetAddress() function is used to set the USB device address, once
//! a device has been discovered on the bus.  This call is typically issued
//! following a USB reset triggered by a call the USBHCDReset().  The
//! address passed into this function via the \e ui32DevAddress parameter is
//! used for all further communications with the device after this function
//! returns.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDSetAddress(uint32_t ui32DevIndex, uint32_t ui32DevAddress)
{
    tUSBRequest sSetupPacket;

    //
    // This is a Standard Device OUT request.
    //
    sSetupPacket.bmRequestType =
        USB_RTYPE_DIR_OUT | USB_RTYPE_STANDARD | USB_RTYPE_DEVICE;

    //
    // Request a Device Descriptor.
    //
    sSetupPacket.bRequest = USBREQ_SET_ADDRESS;
    sSetupPacket.wValue = ui32DevAddress;

    //
    // Index is always 0 for device requests.
    //
    sSetupPacket.wIndex = 0;

    //
    // Only request the space available.
    //
    sSetupPacket.wLength = 0;

    //
    // Put the setup packet in the buffer.
    //
    USBHCDControlTransfer(0, &sSetupPacket,
                          &g_sUSBHCD.psUSBDevice[ui32DevIndex], 0,
                          0, MAX_PACKET_SIZE_EP0);

    //
    // Must delay 2ms after setting the address.
    //
    OS_DELAY(g_ui32Tickms * 2);
}

//*****************************************************************************
//
//! This function is used to send a Clear Feature request to a device.
//!
//! \param ui32DevAddress is the USB bus address of the device that will
//! receive this request.
//! \param ui32Pipe is the pipe that will be used to send the request.
//! \param ui32Feature is one of the USB_FEATURE_* definitions.
//!
//! This function will issue a Clear Feature request to the device indicated
//! by the \e ui32DevAddress parameter.  The \e ui32Pipe parameter is the USB
//! pipe that should be used to send this request.  The \e ui32Feature
//! parameter should be one of the following values:
//!
//! * \b USB_FEATURE_EP_HALT is used to end a HALT condition on a devices
//!   endpoint.
//! * \b USB_FEATURE_REMOTE_WAKE is used to disable a device's remote wake
//!   feature.
//! * \b USB_FEATURE_TEST_MODE is used take the USB device out of test mode.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDClearFeature(uint32_t ui32DevAddress, uint32_t ui32Pipe,
                   uint32_t ui32Feature)
{
    tUSBRequest sSetupPacket;
    uint32_t ui32Index;

    //
    // Get the index number from the allocated pipe.
    //
    ui32Index = (ui32Pipe & EP_PIPE_IDX_M);

    //
    // This is a Standard Device OUT request.
    //
    sSetupPacket.bmRequestType =
        USB_RTYPE_DIR_OUT | USB_RTYPE_STANDARD | USB_RTYPE_ENDPOINT;

    //
    // Request a Device Descriptor.
    //
    sSetupPacket.bRequest = USBREQ_CLEAR_FEATURE;
    sSetupPacket.wValue = ui32Feature;

    //
    // Set the endpoint to access.
    //
    if(ui32Pipe & EP_PIPE_TYPE_IN)
    {
        sSetupPacket.wIndex = g_sUSBHCD.psUSBINPipes[ui32Index].ui8EPNumber |
                              0x80;
    }
    else
    {
        sSetupPacket.wIndex = g_sUSBHCD.psUSBOUTPipes[ui32Index].ui8EPNumber;
    }

    //
    // This is always 0.
    //
    sSetupPacket.wLength = 0;

    //
    // Put the setup packet in the buffer.
    //
    USBHCDControlTransfer(0, &sSetupPacket,
                          &g_sUSBHCD.psUSBDevice[ui32DevAddress - 1], 0, 0,
                          MAX_PACKET_SIZE_EP0);

    //
    // Set the endpoint to access.
    //
    if(ui32Pipe & EP_PIPE_TYPE_IN)
    {
        MAP_USBEndpointDataToggleClear(USB0_BASE,
                                       IndexToUSBEP(ui32Index + 1),
                                       USB_EP_HOST_IN);
    }
    else
    {
        MAP_USBEndpointDataToggleClear(USB0_BASE,
                                       IndexToUSBEP(ui32Index + 1),
                                       USB_EP_HOST_OUT);
    }

    //
    // Must delay 2ms after clearing the feature.
    //
    OS_DELAY(g_ui32Tickms * 2);
}

//*****************************************************************************
//
//! This function is used to set the current configuration for a device.
//!
//! \param ui32Index specifies which USB controller to use.
//! \param ui32Device is the USB device for this function.
//! \param ui32Configuration is one of the devices valid configurations.
//!
//! This function is used to set the current device configuration for a USB
//! device.  The \e ui32Configuration value must be one of the configuration
//! indexes that was returned in the configuration descriptor from the device,
//! or a value of 0.  If 0 is passed in, the device will return to it's
//! addressed state and no longer be in a configured state.  If the value is
//! non-zero then the device will change to the requested configuration.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDSetConfig(uint32_t ui32Index, uint32_t ui32Device,
                uint32_t ui32Configuration)
{
    tUSBRequest sSetupPacket;
    tUSBHostDevice *psDevice;

    ASSERT(ui32Index == 0);

    psDevice = (tUSBHostDevice *)ui32Device;

    //
    // This is a Standard Device OUT request.
    //
    sSetupPacket.bmRequestType =
        USB_RTYPE_DIR_OUT | USB_RTYPE_STANDARD | USB_RTYPE_DEVICE;

    //
    // Request a Device Descriptor.
    //
    sSetupPacket.bRequest = USBREQ_SET_CONFIG;
    sSetupPacket.wValue = ui32Configuration;

    //
    // Index is always 0 for device requests.
    //
    sSetupPacket.wIndex = 0;

    //
    // Only request the space available.
    //
    sSetupPacket.wLength = 0;

    //
    // Put the setup packet in the buffer.
    //
    USBHCDControlTransfer(0, &sSetupPacket, psDevice, 0, 0,
                          MAX_PACKET_SIZE_EP0);
}

//*****************************************************************************
//
//! This function is used to set the current interface and alternate setting
//! for an interface on a device.
//!
//! \param ui32Index specifies which USB controller to use.
//! \param ui32Device is the USB device for this function.
//! \param ui32Interface is one of the valid interface numbers for a device.
//! \param ui32AltSetting is one of the valid alternate interfaces for the
//! \e ui32Interface number.
//!
//! This function is used to change the alternate setting for one of the valid
//! interfaces on a USB device.  The \e ui32Device specifies the device
//! instance that was returned when the device was connected.  This call will
//! set the USB device's interface based on the \e ui32Interface and
//! \e ui32AltSetting.
//!
//! \b Example: Set the USB device interface 2 to alternate setting 1.
//!
//! \verbatim
//! USBHCDSetInterface(0, ui32Device, 2, 1);
//! \endverbatim
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDSetInterface(uint32_t ui32Index, uint32_t ui32Device,
                   uint32_t ui32Interface, uint32_t ui32AltSetting)
{
    tUSBRequest sSetupPacket;
    tUSBHostDevice *psDevice;

    ASSERT(ui32Index == 0);

    psDevice = (tUSBHostDevice *)ui32Device;

    //
    // This is a Standard Device OUT request.
    //
    sSetupPacket.bmRequestType =
        USB_RTYPE_DIR_OUT | USB_RTYPE_STANDARD | USB_RTYPE_INTERFACE;

    //
    // Request a Device Descriptor.
    //
    sSetupPacket.bRequest = USBREQ_SET_INTERFACE;

    //
    // Index is the interface to access.
    //
    sSetupPacket.wIndex = ui32Interface;

    //
    // wValue is the alternate setting.
    //
    sSetupPacket.wValue = ui32AltSetting;


    //
    // Only request the space available.
    //
    sSetupPacket.wLength = 0;

    //
    // Put the setup packet in the buffer.
    //
    USBHCDControlTransfer(0, &sSetupPacket, psDevice, 0, 0,
                          MAX_PACKET_SIZE_EP0);
}

//*****************************************************************************
//
// The internal function to see if a new schedule event should occur.
//
// This function is called by the main interrupt handler due to start of frame
// interrupts to determine if a new scheduler event should be sent to the USB
// pipe.
//
// \return None.
//
//*****************************************************************************
void
USBHostCheckPipes(void)
{
    int32_t i32Idx;

    g_ui32CurrentTick++;

    for(i32Idx = 0; i32Idx < g_sUSBHCD.ui32NumEndpoints; i32Idx++)
    {
        //
        // Skip unused pipes.
        //
        if(g_sUSBHCD.psUSBINPipes[i32Idx].ui32Type == USBHCD_PIPE_UNUSED)
        {
            continue;
        }

        //
        // If the tick has expired and it has an interval then update it.
        //
        if((g_sUSBHCD.psUSBINPipes[i32Idx].ui32Interval != 0) &&
           (g_sUSBHCD.psUSBINPipes[i32Idx].ui32NextEventTick ==
            g_ui32CurrentTick))
        {
            //
            // Schedule the next event.
            //
            g_sUSBHCD.psUSBINPipes[i32Idx].ui32NextEventTick +=
                g_sUSBHCD.psUSBINPipes[i32Idx].ui32Interval;

            //
            // If the pipe is IDLE and there is a callback, let the higher
            // level drivers know that a new transfer can be scheduled.
            //
            if((g_sUSBHCD.psUSBINPipes[i32Idx].iState == ePipeIdle) &&
               (g_sUSBHCD.psUSBINPipes[i32Idx].pfnCallback))
            {
                g_sUSBHCD.psUSBINPipes[i32Idx].pfnCallback(
                                                        IN_PIPE_HANDLE(i32Idx),
                                                        USB_EVENT_SCHEDULER);
            }
        }
    }
}

//*****************************************************************************
//
// The internal USB host mode interrupt handler.
//
// \param ui32Index is the USB controller associated with this interrupt.
// \param ui32Status is the current interrupt status as read via a call to
// \e USBIntStatusControl().
//
// This the main USB interrupt handler called when operating in host mode.
// This handler will branch the interrupt off to the appropriate handlers
// depending on the current status of the USB controller.
//
// The two-tiered structure for the interrupt handler ensures that it is
// possible to use the same handler code in both host and OTG modes and
// means that device code can be excluded from applications that only require
// support for USB host mode operation.
//
// \return None.
//
//*****************************************************************************
void
USBHostIntHandlerInternal(uint32_t ui32Index, uint32_t ui32Status)
{
    uint32_t ui32EPStatus, ui32DMAIntStatus, ui32Idx, ui32DevIndex;
    static uint32_t ui32SOFDivide = 0;
    int32_t i32ClassDrvr;

    //
    // By default, assume we are dealing with the device directly connected
    // to the host controller and that we need to notify its class driver of
    // this interrupt.
    //
    g_sUSBHCD.psUSBDevice[0].ui32Flags |= USBHDEV_FLAG_NOTIFYINT;

    if(ui32Status & USB_INTCTRL_SOF)
    {
        //
        // Indicate that a start of frame has occurred.
        //
        g_sUSBHCD.ui32IntEvents |= INT_EVENT_SOF;
    }

    //
    // A power fault has occurred so notify the application.
    //
    if(ui32Status & USB_INTCTRL_POWER_FAULT)
    {
        //
        // Indicate that a power fault has occurred.
        //
        g_sUSBHCD.ui32IntEvents |= INT_EVENT_POWER_FAULT;

        //
        // Turn off power to the bus.
        //
        MAP_USBHostPwrDisable(USB0_BASE);

        //
        // Disable USB interrupts.
        //
        OS_INT_DISABLE(g_sUSBHCD.ui32IntNum);

        return;
    }

    //
    // In the event of a USB VBUS error, end the session and remove power to
    // the device.
    //
    if(ui32Status & USB_INTCTRL_VBUS_ERR)
    {
        //
        // Set the VBUS error event.  We deliberately clear all other events
        // since this one means anything else that is outstanding is
        // irrelevant.
        //
        g_sUSBHCD.ui32IntEvents = INT_EVENT_VBUS_ERR;
        return;
    }

    //
    // Received a reset from the host.
    //
    if(ui32Status & USB_INTCTRL_BABBLE)
    {
    }

    //
    // Suspend was signaled on the bus.
    //
    if(ui32Status & USB_INTCTRL_SUSPEND)
    {
    }

    //
    // Start the session.
    //
    if(ui32Status & USB_INTCTRL_SESSION)
    {
        //
        // Power the USB bus.
        //
        MAP_USBHostPwrEnable(USB0_BASE);

        USBOTGSessionRequest(USB0_BASE, true);
    }

    //
    // Resume was signaled on the bus.
    //
    if(ui32Status & USB_INTCTRL_RESUME)
    {
    }

    //
    // Device connected so tell the main routine to issue a reset.
    //
    if(ui32Status & USB_INTCTRL_CONNECT)
    {
        //
        // Set the connect flag and clear disconnect if it happens to be set.
        //
        g_sUSBHCD.ui32IntEvents |= INT_EVENT_CONNECT;
        g_sUSBHCD.ui32IntEvents &= ~INT_EVENT_DISCONNECT;

        //
        // Power the USB bus.
        //
        MAP_USBHostPwrEnable(USB0_BASE);
    }

    //
    // Handle the ID detection so that the ID pin can be used as a
    // GPIO in eUSBModeHost.
    //
    if(ui32Status & USB_INTCTRL_MODE_DETECT)
    {
        //
        // If in eUSBModeHost mode then switch back to OTG detection
        // so that VBUS can be monitored but free up the ID pin.
        //
        if(g_iUSBMode == eUSBModeHost)
        {
            USBOTGMode(USB0_BASE);
        }
    }

    //
    // Device was unplugged.
    //
    if(ui32Status & USB_INTCTRL_DISCONNECT)
    {
        //
        // Set the disconnect flag and clear connect if it happens to be set.
        //
        g_sUSBHCD.ui32IntEvents |= INT_EVENT_DISCONNECT;
        g_sUSBHCD.ui32IntEvents &= ~INT_EVENT_CONNECT;
    }

    //
    // Start of Frame was received.
    //
    if(ui32Status & USB_INTCTRL_SOF)
    {
        //
        // Increment the global Start of Frame counter.
        //
        g_ui32USBSOFCount++;

        //
        // Increment our SOF divider.
        //
        ui32SOFDivide++;

        //
        // Have we counted enough SOFs to allow us to call the tick function?
        //
        if(ui32SOFDivide == USB_SOF_TICK_DIVIDE)
        {
            //
            // Yes - reset the divider and call the SOF tick handler.
            //
            ui32SOFDivide = 0;
            InternalUSBStartOfFrameTick(USB_SOF_TICK_DIVIDE);
        }
    }

    //
    // Check to see if any DMA transfers are pending
    //
    ui32DMAIntStatus = USBLibDMAIntStatus(g_sUSBHCD.psDMAInstance);

    if(ui32DMAIntStatus)
    {
        //
        // Handle any DMA interrupt processing.
        //
        USBLibDMAIntHandler(g_sUSBHCD.psDMAInstance, ui32DMAIntStatus);

        for(ui32Idx = 0; ui32Idx < MAX_NUM_PIPES; ui32Idx++)
        {
            if((g_sUSBHCD.psUSBINPipes[ui32Idx].iState == ePipeReadDMAWait) ||
               (g_sUSBHCD.psUSBINPipes[ui32Idx].iState == ePipeReadDMA))
            {
                //
                // If the DMA channel transfer is complete, send an ack.
                //
                if(USBLibDMAChannelStatus(g_sUSBHCD.psDMAInstance,
                       g_sUSBHCD.psUSBINPipes[ui32Idx].ui8DMAChannel) ==
                   USBLIBSTATUS_DMA_COMPLETE)
                {
                    //
                    // Acknowledge the IN request.
                    //
                    MAP_USBHostEndpointDataAck(USB0_BASE,
                                               IndexToUSBEP(ui32Idx + 1));

                    //
                    // If using uDMA then the endpoint status interrupt will
                    // not occur.  So process the data ready event here.
                    //
                    g_sUSBHCD.psUSBINPipes[ui32Idx].iState = ePipeDataReady;

                    //
                    // Only call a handler if one is present.
                    //
                    if(g_sUSBHCD.psUSBINPipes[ui32Idx].pfnCallback)
                    {
                        g_sUSBHCD.psUSBINPipes[ui32Idx].pfnCallback(
                            IN_PIPE_HANDLE(ui32Idx), USB_EVENT_RX_AVAILABLE);
                    }

                    //
                    // Remember that we need to notify this device's class
                    // driver that an interrupt occurred.
                    //
                    g_sUSBHCD.psUSBINPipes[ui32Idx].psDevice->ui32Flags |=
                                                    USBHDEV_FLAG_NOTIFYINT;
                }
            }
            else if(g_sUSBHCD.psUSBOUTPipes[ui32Idx].iState ==
                    ePipeWriteDMASend)
            {
                //
                // If the uDMA channel transfer is complete, then tell
                // the USB controller to go ahead and send the data
                //
                if(USBLibDMAChannelStatus(g_sUSBHCD.psDMAInstance,
                       g_sUSBHCD.psUSBOUTPipes[ui32Idx].ui8DMAChannel) &
                   USBLIBSTATUS_DMA_COMPLETE)
                {
                    MAP_USBEndpointDataSend(USB0_BASE,
                                            IndexToUSBEP(ui32Idx + 1),
                                            USB_TRANS_OUT);

                    //
                    // Now waiting on the final endpoint interrupt.
                    //
                    g_sUSBHCD.psUSBOUTPipes[ui32Idx].iState =
                                                            ePipeWriteDMAWait;
                }
            }
            else if(g_sUSBHCD.psUSBOUTPipes[ui32Idx].iState == ePipeWriteDMA)
            {
                //
                // Data was transmitted successfully.
                //
                g_sUSBHCD.psUSBOUTPipes[ui32Idx].iState = ePipeDataSent;

                //
                // Only call a handler if one is present.
                //
                if(g_sUSBHCD.psUSBOUTPipes[ui32Idx].pfnCallback)
                {
                    //
                    // Notify the pipe that its last transaction was completed.
                    //
                    g_sUSBHCD.psUSBOUTPipes[ui32Idx].pfnCallback(
                            OUT_PIPE_HANDLE(ui32Idx),
                            USB_EVENT_TX_COMPLETE);
                }
            }
        }
    }

    //
    // Get the current endpoint interrupt status.
    //
    ui32Status = MAP_USBIntStatusEndpoint(USB0_BASE);

    //
    // Handle end point 0 interrupts.
    //
    if(ui32Status & USB_INTEP_0)
    {
        //
        // Indicate that a start of frame has occurred.
        //
        g_sUSBHCD.ui32IntEvents |= INT_EVENT_ENUM;
    }

    for(ui32Idx = 0; ui32Idx < MAX_NUM_PIPES; ui32Idx++)
    {
        //
        // Check the next pipe, the first time through this will clear out
        // any interrupts dealing with endpoint zero since it was handled
        // above.
        //
        ui32Status >>= 1;

        //
        // Break out if there are no more pending interrupts.
        //
        if(ui32Status == 0)
        {
            break;
        }

        //
        // Check the status of the receive(IN) pipes.
        //
        if(ui32Status & 0x10000)
        {
            //
            // Clear the status flag for the IN Pipe.
            //
            ui32Status &= ~0x10000;

            //
            // Read the status of the endpoint connected to this pipe.
            //
            ui32EPStatus = MAP_USBEndpointStatus(USB0_BASE,
                                               IndexToUSBEP(ui32Idx + 1));

            //
            // Check if the device stalled the request.
            //
            if(ui32EPStatus & USB_HOST_IN_STALL)
            {
                //
                // Clear the stall condition on this endpoint pipe.
                //
                MAP_USBHostEndpointStatusClear(USB0_BASE,
                                               IndexToUSBEP(ui32Idx + 1),
                                               USB_HOST_IN_STALL);

                //
                // Save the STALLED state.
                //
                g_sUSBHCD.psUSBINPipes[ui32Idx].iState = ePipeStalled;

                //
                // Notify the pipe that it was stalled.
                //
                if(g_sUSBHCD.psUSBINPipes[ui32Idx].pfnCallback)
                {
                    g_sUSBHCD.psUSBINPipes[ui32Idx].pfnCallback(
                                                    IN_PIPE_HANDLE(ui32Idx),
                                                    USB_EVENT_STALL);
                }
            }
            else if(ui32EPStatus & USB_HOST_IN_ERROR)
            {
                //
                // We can no longer communicate with this device for some
                // reason.  It may have been disconnected from a hub, for
                // example.  Merely clear the status and continue.
                //
                USBHostEndpointStatusClear(USB0_BASE,
                                           IndexToUSBEP(ui32Idx + 1),
                                           USB_HOST_IN_ERROR);

                //
                // Save the STALLED state.
                //
                g_sUSBHCD.psUSBINPipes[ui32Idx].iState = ePipeError;

                //
                // Notify the pipe that it was stalled.
                //
                if(g_sUSBHCD.psUSBINPipes[ui32Idx].pfnCallback)
                {
                    g_sUSBHCD.psUSBINPipes[ui32Idx].pfnCallback(
                                                    IN_PIPE_HANDLE(ui32Idx),
                                                    USB_EVENT_ERROR);
                }
            }
            //
            // Handle the case where the pipe is reading a single packet.
            //
            else if(g_sUSBHCD.psUSBINPipes[ui32Idx].iState == ePipeReadDMA)
            {
                //
                // Enable the DMA channel and wait for it to complete.
                //
                g_sUSBHCD.psUSBINPipes[ui32Idx].iState = ePipeReadDMAWait;

                USBLibDMAChannelEnable(
                        g_sUSBHCD.psDMAInstance,
                        g_sUSBHCD.psUSBINPipes[ui32Idx].ui8DMAChannel);
            }
            else if(g_sUSBHCD.psUSBINPipes[ui32Idx].iState == ePipeReading)
            {
                //
                // Data is available.
                //
                g_sUSBHCD.psUSBINPipes[ui32Idx].iState = ePipeDataReady;

                //
                // Read the data out of the USB endpoint interface into the
                // buffer provided by the caller to USBHCDPipeRead() or
                // USBHCDPipeSchedule() if a buffer was provided already.
                //
                if(g_sUSBHCD.psUSBINPipes[ui32Idx].pui8ReadPtr)
                {
                    USBEndpointDataGet(USB0_BASE, IndexToUSBEP(ui32Idx + 1),
                                g_sUSBHCD.psUSBINPipes[ui32Idx].pui8ReadPtr,
                                &g_sUSBHCD.psUSBINPipes[ui32Idx].ui32ReadSize);
                }

                //
                // Notify the pipe that its last transaction was completed.
                //
                if(g_sUSBHCD.psUSBINPipes[ui32Idx].pfnCallback)
                {
                    g_sUSBHCD.psUSBINPipes[ui32Idx].pfnCallback(
                                                    IN_PIPE_HANDLE(ui32Idx),
                                                    USB_EVENT_RX_AVAILABLE);
                }

            }

            //
            // Remember that we need to notify this device's class
            // driver that an interrupt occurred.
            //
            g_sUSBHCD.psUSBINPipes[ui32Idx].psDevice->ui32Flags |=
                                                    USBHDEV_FLAG_NOTIFYINT;
        }

        //
        // Check the status of the transmit(OUT) pipes.
        //
        if(ui32Status & 1)
        {
            //
            // Read the status of the endpoint connected to this pipe.
            //
            ui32EPStatus = MAP_USBEndpointStatus(USB0_BASE,
                                               IndexToUSBEP(ui32Idx + 1));

            //
            // Check if the device stalled the request.
            //
            if(ui32EPStatus & USB_HOST_OUT_STALL)
            {
                //
                // Clear the stall condition on this endpoint pipe.
                //
                MAP_USBHostEndpointStatusClear(USB0_BASE,
                                               IndexToUSBEP(ui32Idx + 1),
                                               USB_HOST_OUT_STALL);

                //
                // Save the STALLED state.
                //
                g_sUSBHCD.psUSBOUTPipes[ui32Idx].iState = ePipeStalled;

                //
                // Only call a handler if one is present.
                //
                if(g_sUSBHCD.psUSBOUTPipes[ui32Idx].pfnCallback)
                {
                    //
                    // Notify the pipe that it was stalled.
                    //
                    g_sUSBHCD.psUSBOUTPipes[ui32Idx].pfnCallback(
                            OUT_PIPE_HANDLE(ui32Idx),
                            USB_EVENT_STALL);
                }
            }
            else if(ui32EPStatus & USB_HOST_OUT_ERROR)
            {
                //
                // Clear the error condition on this endpoint pipe.
                //
                MAP_USBHostEndpointStatusClear(USB0_BASE,
                                               IndexToUSBEP(ui32Idx + 1),
                                               USB_HOST_OUT_ERROR);

                //
                // Save the Pipes error state.
                //
                g_sUSBHCD.psUSBOUTPipes[ui32Idx].iState = ePipeError;

                //
                // Only call a handler if one is present.
                //
                if(g_sUSBHCD.psUSBOUTPipes[ui32Idx].pfnCallback)
                {
                    //
                    // Notify the pipe that had an error.
                    //
                    g_sUSBHCD.psUSBOUTPipes[ui32Idx].pfnCallback(
                            OUT_PIPE_HANDLE(ui32Idx),
                            USB_EVENT_ERROR);
                }
            }
            else if((g_sUSBHCD.psUSBOUTPipes[ui32Idx].iState ==
                     ePipeWriting) ||
                    (g_sUSBHCD.psUSBOUTPipes[ui32Idx].iState ==
                     ePipeWriteDMAWait))
            {
                //
                // Data was transmitted successfully.
                //
                g_sUSBHCD.psUSBOUTPipes[ui32Idx].iState = ePipeDataSent;

                //
                // Only call a handler if one is present.
                //
                if(g_sUSBHCD.psUSBOUTPipes[ui32Idx].pfnCallback)
                {
                    //
                    // Notify the pipe that its last transaction was completed.
                    //
                    g_sUSBHCD.psUSBOUTPipes[ui32Idx].pfnCallback(
                            OUT_PIPE_HANDLE(ui32Idx),
                            USB_EVENT_TX_COMPLETE);
                }
            }

            //
            // Clear the stall condition on this endpoint pipe.
            //
            MAP_USBHostEndpointStatusClear(USB0_BASE,
                                           IndexToUSBEP(ui32Idx + 1),
                                           ui32EPStatus);
            //
            // Remember that we need to notify this device's class
            // driver that an interrupt occurred.
            //
            if(g_sUSBHCD.psUSBOUTPipes[ui32Idx].psDevice)
            {
                g_sUSBHCD.psUSBINPipes[ui32Idx].psDevice->ui32Flags |=
                                                    USBHDEV_FLAG_NOTIFYINT;
            }
        }
    }

    //
    // Send back notifications to any class driver whose endpoint required
    // service during the handler.
    //
    for(ui32DevIndex = 0; ui32DevIndex <= MAX_USB_DEVICES; ui32DevIndex++)
    {
        //
        // Which class driver does this device use?
        //
        i32ClassDrvr = g_pi32USBHActiveDriver[ui32DevIndex];

        //
        // If a class driver is in use, and one of its endpoints was serviced
        // and the class driver has an interrupt callback...
        //
        if((i32ClassDrvr >= 0) &&
           (g_sUSBHCD.psUSBDevice[ui32DevIndex].ui32Flags &
            USBHDEV_FLAG_NOTIFYINT) &&
           (g_sUSBHCD.ppsClassDrivers[i32ClassDrvr]->pfnIntHandler))
        {
            //
            // ...call the class driver's interrupt notification callback.
            //
            g_sUSBHCD.ppsClassDrivers[i32ClassDrvr]->pfnIntHandler(
                                            g_ppvDriverInstance[ui32DevIndex]);
        }
    }
}

//*****************************************************************************
//
//! The USB host mode interrupt handler for controller index 0.
//!
//! This the main USB interrupt handler entry point.  This handler will branch
//! the interrupt off to the appropriate handlers depending on the current
//! status of the USB controller.   This function must be placed in the
//! interrupt table in order for the USB Library host stack to function.
//!
//! \return None.
//
//*****************************************************************************
void
USB0HostIntHandler(void)
{
    uint32_t ui32Status;

    //
    // Get the control interrupt status.
    //
    ui32Status = MAP_USBIntStatusControl(USB0_BASE);

    //
    // Call the internal handler to process the interrupts.
    //
    USBHostIntHandlerInternal(0, ui32Status);
}

//*****************************************************************************
//
//! This function opens the class driver.
//!
//! \param ui32Index specifies which USB controller to use.
//! \param ui32DeviceNum is the device number for the driver to load.
//!
//! This function opens the driver needed based on the class value found in
//! the device's interface descriptor.
//!
//! \return This function returns -1 if no driver is found, or it returns the
//! index of the driver found in the list of host class drivers.
//
//*****************************************************************************
static int32_t
USBHCDOpenDriver(uint32_t ui32Index, uint32_t ui32DeviceNum)
{
    int32_t i32Driver;
    uint32_t ui32Class;
    tInterfaceDescriptor *psInterface;
    tEventInfo sEvent;

    ASSERT(ui32Index == 0);

    //
    // Get the interface descriptor.
    //
    psInterface = USBDescGetInterface(
                    g_sUSBHCD.psUSBDevice[ui32DeviceNum].psConfigDescriptor,
                    g_sUSBHCD.psUSBDevice[ui32DeviceNum].ui32Interface,
                    USB_DESC_ANY);

    //
    // Read the interface class.
    //
    ui32Class = psInterface->bInterfaceClass;

    //
    // Search through the Host Class driver list for the devices class.
    //
    for(i32Driver = 0; i32Driver < g_sUSBHCD.ui32NumClassDrivers; i32Driver++)
    {
        //
        // If a driver was found call the open for this driver and save which
        // driver is in use.
        //
        if(g_sUSBHCD.ppsClassDrivers[i32Driver]->ui32InterfaceClass ==
           ui32Class)
        {
            //
            // Call the open function for the class driver.
            //
            g_ppvDriverInstance[ui32DeviceNum] =
                    g_sUSBHCD.ppsClassDrivers[i32Driver]->pfnOpen(
                            &g_sUSBHCD.psUSBDevice[ui32DeviceNum]);

            //
            // If the driver was successfully loaded then break out of the
            // loop.
            //
            if(g_ppvDriverInstance[ui32DeviceNum] != 0)
            {
                break;
            }
        }
    }

    //
    // If no drivers were found then return -1 to indicate an invalid
    // driver instance.
    //
    if(i32Driver == g_sUSBHCD.ui32NumClassDrivers)
    {
        //
        // Send an unknown connection event.
        //
        SendUnknownConnect(ui32Index, ui32Class);

        //
        // Indicate that no driver was found.
        //
        i32Driver = -1;
    }
    else
    {
        //
        // If the connect event is enabled then send the event.
        //
        sEvent.ui32Event = USB_EVENT_CONNECTED;
        sEvent.ui32Instance = (ui32Index << 16) | ui32DeviceNum;
        InternalUSBHCDSendEvent(0, &sEvent, USBHCD_EVFLAG_CONNECT);
    }

    return(i32Driver);
}

//*****************************************************************************
//
// This function will send an event to a registered event driver.
//
// \param ui32Index specifies which USB controller to use.
// \param psEvent is a pointer to the event structure to send.
//
// This function is only used internally to the USB library and will check
// if an event driver is registered and send on the event.
//
// Note: This function should not be called outside of the USB library.
//
// \return None.
//
//*****************************************************************************
void
InternalUSBHCDSendEvent(uint32_t ui32Index, tEventInfo *psEvent,
                        uint32_t ui32EvFlag)
{
    //
    // Make sure that an event driver has been registered.
    //
    if((g_sUSBHCD.i32EventDriver != -1) &&
       (g_sUSBHCD.ppsClassDrivers[g_sUSBHCD.i32EventDriver]->pfnIntHandler) &&
       (g_sUSBHCD.ui32EventEnables & ui32EvFlag))
    {
        g_sUSBHCD.ppsClassDrivers[g_sUSBHCD.i32EventDriver]->pfnIntHandler(
                                                                    psEvent);
    }
}

//*****************************************************************************
//
// This function handles the necessary clean up for device disconnect.
//
// \param ui32Index is the device number for the device that was disconnected.
//
// This function handles all of the necessary clean up after a device
// disconnect has been detected by the stack.  This includes calling back the
// appropriate driver if necessary.
//
// \return None.
//
//*****************************************************************************
static void
USBHCDDeviceDisconnected(uint32_t ui32Index, uint32_t ui32DevIndex)
{
    tEventInfo sEvent;

    ASSERT(ui32Index == 0);
    ASSERT(ui32DevIndex <= MAX_USB_DEVICES);

    //
    // If there is an event driver with a valid event handler and the
    // USBHCD_EVFLAG_DISCNCT is enabled, then call the registered event
    // handler.
    //
    sEvent.ui32Event = USB_EVENT_DISCONNECTED;
    sEvent.ui32Instance = (ui32Index << 16) | ui32DevIndex;
    InternalUSBHCDSendEvent(0, &sEvent, USBHCD_EVFLAG_DISCNCT);

    //
    // Reset the class.
    //
    g_sUSBHCD.ui32Class = USB_CLASS_EVENTS;

    //
    // Free configuration descriptor memory if any has been allocated.
    //
    if(g_sUSBHCD.psUSBDevice[ui32DevIndex].ui32Flags & USBHDEV_FLAG_ALLOCATED)
    {
        //
        // Free the memory used by the configuration descriptor.
        //
        ConfigDescFree(&g_sUSBHCD.psUSBDevice[ui32DevIndex]);

        g_sUSBHCD.psUSBDevice[ui32DevIndex].ui32Flags = 0;
        g_sUSBHCD.psUSBDevice[ui32DevIndex].bConfigRead = false;
    }

    //
    // Reset the max packet size so that this will be re-read from new devices.
    //
    g_sUSBHCD.psUSBDevice[ui32DevIndex].sDeviceDescriptor.bMaxPacketSize0 = 0;

    //
    // No longer have a device descriptor.
    //
    g_sUSBHCD.psUSBDevice[ui32DevIndex].sDeviceDescriptor.bLength = 0;

    //
    // No longer addressed.
    //
    g_sUSBHCD.psUSBDevice[ui32DevIndex].ui32Address = 0;

    //
    // If this was an active driver then close it out.
    //
    if(g_pi32USBHActiveDriver[ui32DevIndex] >= 0)
    {
        //
        // Call the driver Close entry point.
        //
        g_sUSBHCD.ppsClassDrivers[g_pi32USBHActiveDriver[ui32DevIndex]]->
            pfnClose(g_ppvDriverInstance[ui32DevIndex]);

        //
        // No active driver now present.
        //
        g_pi32USBHActiveDriver[ui32DevIndex] = -1;
        g_ppvDriverInstance[ui32DevIndex] = 0;
    }

    //
    // This call is necessary for OTG controllers to know that the host
    // stack has completed handling the disconnect of the device before
    // removing power and returning to a state that can allow OTG
    // negotiations once again.
    // We only do this if the disconnected device
    // was attached directly to us (device index 0).
    //
    if((ui32DevIndex == 0) && (g_iUSBMode == eUSBModeOTG))
    {
        OTGDeviceDisconnect(0);
    }
}

//*****************************************************************************
//
//! This function is the main routine for the Host Controller Driver.
//!
//! This function is the main routine for the host controller driver, and must
//! be called periodically by the main application outside of a callback
//! context.  This allows for a simple cooperative system to access the the
//! host controller driver interface without the need for an RTOS.  All time
//! critical operations are handled in interrupt context but all blocking
//! operations are run from the this function to allow them to block and wait
//! for completion without holding off other interrupts.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDMain(void)
{
    tUSBHDeviceState iOldState;
    int32_t i32Dev;
    tEventInfo sEvent;

    //
    // Save the old state to detect changes properly.
    //
    iOldState = g_sUSBHCD.piDeviceState[0];

    //
    // Fix up the state if any important interrupt events occurred.
    //
    if(g_sUSBHCD.ui32IntEvents)
    {
        //
        // Disable the USB interrupt.
        //
        OS_INT_DISABLE(g_sUSBHCD.ui32IntNum);

        if(g_sUSBHCD.ui32IntEvents & INT_EVENT_POWER_FAULT)
        {
            //
            // A power fault has occurred so notify the application if there
            // is an event handler and the event has been enabled.
            //
            sEvent.ui32Event = USB_EVENT_POWER_FAULT;
            sEvent.ui32Instance = 0;
            InternalUSBHCDSendEvent(0, &sEvent, USBHCD_EVFLAG_PWRFAULT);

            g_sUSBHCD.piDeviceState[0] = eHCDPowerFault;
        }
        else if(g_sUSBHCD.ui32IntEvents & INT_EVENT_VBUS_ERR)
        {
            //
            // A VBUS error has occurred.  This event trumps connect and
            // disconnect since it will cause a controller reset.
            //
            g_sUSBHCD.piDeviceState[0] = eHCDVBUSError;
        }
        else
        {
            //
            // Has a device connected?
            //
            if(g_sUSBHCD.ui32IntEvents & INT_EVENT_CONNECT)
            {
                g_sUSBHCD.piDeviceState[0] = eHCDDevReset;
                g_sUSBHCD.psUSBDevice[0].ui8Hub = 0;
                g_sUSBHCD.psUSBDevice[0].ui8HubPort = 0;
            }
            else
            {
                //
                // Has a device disconnected?
                //
                if(g_sUSBHCD.ui32IntEvents & INT_EVENT_DISCONNECT)
                {
                    g_sUSBHCD.piDeviceState[0] = eHCDDevDisconnected;
                }
            }

            //
            // Handle the start of frame event
            //
            if(g_sUSBHCD.ui32IntEvents & INT_EVENT_SOF)
            {
                //
                // If the connect event is enabled then send the event.
                //
                sEvent.ui32Event = USB_EVENT_SOF;
                sEvent.ui32Instance = 0;
                InternalUSBHCDSendEvent(0, &sEvent, USBHCD_EVFLAG_SOF);

                USBHostCheckPipes();

                //
                // Call the hub driver to have it perform any necessary
                // processing to handle downstream devices.
                //
                USBHHubMain();
            }
        }

        //
        // Clear the flags.
        //
        g_sUSBHCD.ui32IntEvents = 0;

        //
        // Enable the USB interrupt.
        //
        OS_INT_ENABLE(g_sUSBHCD.ui32IntNum);
    }

    //
    // Process the state machine for each connected device.  Yes, the exit
    // condition for this loop is correct since we support (MAX_USB_DEVICES+1)
    // devices (the hub counts as one).
    //
    for(i32Dev = 0; i32Dev <= MAX_USB_DEVICES; i32Dev++)
    {
        //
        // If this is not the first device (i.e. the one directly connected to
        // the host controller) then set the old state to the current state
        // since we won't have mucked with it in any of the previous code.
        //
        if(i32Dev != 0)
        {
            iOldState = g_sUSBHCD.piDeviceState[i32Dev];
        }

        //
        // Process the state machine for this device.
        //
        ProcessUSBDeviceStateMachine(iOldState, i32Dev);
    }
}

static void
ProcessUSBDeviceStateMachine(tUSBHDeviceState iOldState,
                             uint32_t ui32DevIndex)
{
    switch(g_sUSBHCD.piDeviceState[ui32DevIndex])
    {
        //
        // There was a power fault condition so shut down and wait for the
        // application to re-initialized the system.
        //
        case eHCDPowerFault:
        {
            break;
        }

        //
        // There was a VBUS error so handle it.
        //
        case eHCDVBUSError:
        {
            //
            // Disable USB interrupts.
            //
            OS_INT_DISABLE(g_sUSBHCD.ui32IntNum);

            //
            // If there was a device in any state of connection then indicate
            // that it has been disconnected.
            //
            if((iOldState != eHCDIdle) && (iOldState != eHCDPowerFault))
            {
                //
                // Handle device disconnect.
                //
                USBHCDDeviceDisconnected(0, ui32DevIndex);
            }

            //
            // Reset the controller.
            //
            MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_USB0);

            //
            // Wait for 100ms before trying to re-power the device.
            //
            OS_DELAY(g_ui32Tickms * 100);

            //
            // Re-initialize the HCD.
            //
            USBHCDInitInternal(0, g_sUSBHCD.pvPool, g_sUSBHCD.ui32PoolSize);

            break;
        }
        //
        // Trigger a reset to the connected device.
        //
        case eHCDDevReset:
        {
            if(!ui32DevIndex)
            {
                //
                // Trigger a Reset.  This is only ever done for devices
                // attached directly to the controller.
                //
                DEBUG_OUTPUT("USB reset.\n");
                USBHCDReset(0);
            }

            //
            // The state moves to connected but not configured.
            //
            g_sUSBHCD.piDeviceState[0] = eHCDDevConnected;

            //
            // Remember that we don't have a valid configuration descriptor
            // yet.
            //
            g_sUSBHCD.psUSBDevice[0].bConfigRead = false;

            break;
        }
        //
        // Device connection has been established now start enumerating
        // the device.
        //
        case eHCDDevConnected:
        case eHCDDevConnectedHub:
        {
            //
            // First check if we have read the device descriptor at all
            // before proceeding.
            //
            if(g_sUSBHCD.psUSBDevice[ui32DevIndex].sDeviceDescriptor.bLength ==
               0)
            {
                //
                // Initialize a request for the device descriptor.
                //
                DEBUG_OUTPUT("Connection %d - getting device descriptor\n",
                             ui32DevIndex);

                if(USBHCDGetDeviceDescriptor(0,
                        &g_sUSBHCD.psUSBDevice[ui32DevIndex]) == 0)
                {
                    //
                    // If the device descriptor cannot be read then the device
                    // will be treated as unknown.
                    //
                    g_sUSBHCD.piDeviceState[ui32DevIndex] = eHCDDevError;

                    DEBUG_OUTPUT("Connection %d - failed to get descriptor\n",
                                 ui32DevIndex);

                    //
                    // Send an unknown connection event.
                    //
                    SendUnknownConnect(0, 1);

                    //
                    // If the device is connected via a hub, tell the hub
                    // driver that we experienced an error enumerating the
                    // device.
                    //
                    if(g_sUSBHCD.psUSBDevice[ui32DevIndex].ui8Hub)
                    {
                        USBHHubEnumerationError(
                            g_sUSBHCD.psUSBDevice[ui32DevIndex].ui8Hub,
                            g_sUSBHCD.psUSBDevice[ui32DevIndex].ui8HubPort);
                    }
                }
            }
            //
            // If we have the device descriptor then move on to setting
            // the address of the device.
            //
            else if(g_sUSBHCD.psUSBDevice[ui32DevIndex].ui32Address == 0)
            {
                DEBUG_OUTPUT("Connection %d - setting address %d\n",
                             ui32DevIndex, ui32DevIndex + 1);

                //
                // Send the set address command.
                //
                USBHCDSetAddress(ui32DevIndex, (ui32DevIndex + 1));

                //
                // Save the address.
                //
                g_sUSBHCD.psUSBDevice[ui32DevIndex].ui32Address =
                                                            (ui32DevIndex + 1);

                //
                // Move on to the addressed state.
                //
                g_sUSBHCD.piDeviceState[ui32DevIndex] = eHCDDevAddressed;
            }
            break;
        }
        case eHCDDevAddressed:
        {
            //
            // First check if we have read the configuration descriptor.
            //
            if(!g_sUSBHCD.psUSBDevice[ui32DevIndex].bConfigRead)
            {
                DEBUG_OUTPUT("Connection %d - getting config descriptor\n",
                             ui32DevIndex);

                //
                // Initialize a request for the configuration descriptor.
                //
                if(USBHCDGetConfigDescriptor(0,
                                    &g_sUSBHCD.psUSBDevice[ui32DevIndex]) == 0)
                {
                    //
                    // If the device descriptor cannot be read then the device
                    // will be treated as unknown.
                    //
                    g_sUSBHCD.piDeviceState[ui32DevIndex] = eHCDDevError;

                    DEBUG_OUTPUT("Connection %d - failed to get descriptor\n",
                                 ui32DevIndex);

                    //
                    // Send an unknown connection event.
                    //
                    SendUnknownConnect(0, 0);

                    //
                    // If the device is connected via a hub, tell the hub
                    // driver that we experienced an error enumerating the
                    // device.
                    //
                    if(g_sUSBHCD.psUSBDevice[ui32DevIndex].ui8Hub)
                    {
                        USBHHubEnumerationError(
                            g_sUSBHCD.psUSBDevice[ui32DevIndex].ui8Hub,
                            g_sUSBHCD.psUSBDevice[ui32DevIndex].ui8HubPort);
                    }
                }
            }
            //
            // Now have addressed and received the device configuration,
            // so get ready to set the device configuration.
            //
            else
            {
                DEBUG_OUTPUT("Connection %d - setting configuration.\n",
                             ui32DevIndex);

                //
                // Use the first configuration to set the device
                // configuration.
                //
                USBHCDSetConfig(0,
                            (uint32_t)&g_sUSBHCD.psUSBDevice[ui32DevIndex], 1);

                //
                // Move on to the configured state.
                //
                g_sUSBHCD.piDeviceState[ui32DevIndex] = eHCDDevConfigured;

                //
                // Open the driver for the device.
                //
                g_pi32USBHActiveDriver[ui32DevIndex] = USBHCDOpenDriver(0,
                                                                ui32DevIndex);

                //
                // If the device is connected via a hub, tell the hub
                // driver that enumeration is complete.
                //
                if(g_sUSBHCD.psUSBDevice[ui32DevIndex].ui8Hub)
                {
                    USBHHubEnumerationComplete(
                            g_sUSBHCD.psUSBDevice[ui32DevIndex].ui8Hub,
                            g_sUSBHCD.psUSBDevice[ui32DevIndex].ui8HubPort);
                }
            }
            break;
        }
        //
        // The device was making a request and is now complete.
        //
        case eHCDDevRequest:
        {
            g_sUSBHCD.piDeviceState[ui32DevIndex] = eHCDDevConnected;
            break;
        }
        //
        // The strings are currently not accessed.
        //
        case eHCDDevGetStrings:
        {
            break;
        }
        //
        // Basically Idle at this point.
        //
        case eHCDDevDisconnected:
        {
            DEBUG_OUTPUT("Connection %d - disconnected.\n",
                         ui32DevIndex);

            //
            // Handle device disconnect.
            //
            USBHCDDeviceDisconnected(0, ui32DevIndex);

            //
            // Return to the Idle state.
            //
            g_sUSBHCD.piDeviceState[ui32DevIndex] = eHCDIdle;
            break;
        }

        //
        // Connection and enumeration is complete so allow this function
        // to exit.
        //
        case eHCDDevConfigured:
        {
            break;
        }

        //
        // Poorly behaving device are in limbo in this state until removed.
        //
        case eHCDDevError:
        {
            DEBUG_OUTPUT("Connection %d - Error!\n", ui32DevIndex);

            //
            // If this device is connected directly to us, tidy up and ignore
            // it until it is removed.  If the device is connected to a hub,
            // we just leave it in the error state until it is removed.
            //
            if(ui32DevIndex == 0)
            {
                g_sUSBHCD.ui32IntEvents |= INT_EVENT_DISCONNECT;
                g_sUSBHCD.piDeviceState[ui32DevIndex] = eHCDIdle;
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
//! This function completes a control transaction to a device.
//!
//! \param ui32Index is the controller index to use for this transfer.
//! \param psSetupPacket is the setup request to be sent.
//! \param psDevice is the device instance pointer for this request.
//! \param pui8Data is the data to send for OUT requests or the receive buffer
//! for IN requests.
//! \param ui32Size is the size of the buffer in pui8Data.
//! \param ui32MaxPacketSize is the maximum packet size for the device for this
//! request.
//!
//! This function handles the state changes necessary to send a control
//! transaction to a device.  This function should not be called from within
//! an interrupt callback as it is a blocking function.
//!
//! \return The number of bytes of data that were sent or received as a result
//! of this request.
//
//*****************************************************************************
uint32_t
USBHCDControlTransfer(uint32_t ui32Index, tUSBRequest *psSetupPacket,
                      tUSBHostDevice *psDevice, uint8_t *pui8Data,
                      uint32_t ui32Size, uint32_t ui32MaxPacketSize)
{
    uint32_t ui32Remaining;
    uint32_t ui32DataSize;

    //
    // Debug sanity check.
    //
    ASSERT(g_sUSBHEP0State.iState == eEP0StateIdle);
    ASSERT(ui32Index == 0);

    //
    // Initialize the state of the data for this request.
    //
    g_sUSBHEP0State.pui8Data = pui8Data;
    g_sUSBHEP0State.ui32BytesRemaining = ui32Size;
    g_sUSBHEP0State.ui32DataSize = ui32Size;

    //
    // Set the maximum packet size.
    //
    g_sUSBHEP0State.ui32MaxPacketSize = ui32MaxPacketSize;

    //
    // Save the current address.
    //
    g_sUSBHEP0State.ui32DevAddress = psDevice->ui32Address;

    //
    // Set the address the host will used to communicate with the device.
    //
    MAP_USBHostAddrSet(USB0_BASE, USB_EP_0, g_sUSBHEP0State.ui32DevAddress,
                       USB_EP_HOST_OUT);

    //
    // Put the data in the correct FIFO.
    //
    MAP_USBEndpointDataPut(USB0_BASE, USB_EP_0, (uint8_t *)psSetupPacket,
                           sizeof(tUSBRequest));

    //
    // If this is an IN request, change to that state.
    //
    if(psSetupPacket->bmRequestType & USB_RTYPE_DIR_IN)
    {
        g_sUSBHEP0State.iState = eEP0StateSetupIN;
    }
    else
    {
        //
        // If there is no data then this is not an OUT request.
        //
        if(ui32Size != 0)
        {
            //
            // Since there is data, this is an OUT request.
            //
            g_sUSBHEP0State.iState = eEP0StateSetupOUT;
        }
        else
        {
            //
            // Otherwise this request has no data and just a status phase.
            //
            g_sUSBHEP0State.iState = eEP0StateStatusIN;
        }
    }

    //
    // Make sure we are talking to the correct device.
    //
    USBHostHubAddrSet(USB0_BASE, USB_EP_0,
                      (psDevice->ui8Hub | (psDevice->ui8HubPort << 8)),
                      USB_EP_HOST_OUT | (psDevice->bLowSpeed ?
                      USB_EP_SPEED_LOW : USB_EP_SPEED_FULL));

    //
    // Send the Setup packet.
    //
    MAP_USBEndpointDataSend(USB0_BASE, USB_EP_0, USB_TRANS_SETUP);

    //
    // Block until endpoint 0 returns to the IDLE state.
    //
    while(g_sUSBHEP0State.iState != eEP0StateIdle)
    {
        OS_INT_DISABLE(g_sUSBHCD.ui32IntNum);

        if((g_sUSBHCD.ui32IntEvents & (INT_EVENT_ENUM | INT_EVENT_SOF)) ==
           (INT_EVENT_ENUM | INT_EVENT_SOF))
        {
            g_sUSBHCD.ui32IntEvents &= ~(INT_EVENT_ENUM | INT_EVENT_SOF);

            USBHCDEnumHandler();
        }

        OS_INT_ENABLE(g_sUSBHCD.ui32IntNum);

        if(g_sUSBHEP0State.iState == eEP0StateError)
        {
            return(0xffffffff);
        }

        //
        // If we aborted the transfer due to an error, tell the caller
        // that no bytes were transferred.
        //
        if(g_sUSBHCD.ui32IntEvents & (INT_EVENT_VBUS_ERR |
                                      INT_EVENT_DISCONNECT))
        {
            return(0xffffffff);
        }
    }

    //
    // Calculate and return the number of bytes that were sent or received.
    // The extra copy into local variables is required to prevent some
    // compilers from warning about undefined order of volatile access.
    //
    ui32DataSize = g_sUSBHEP0State.ui32DataSize;
    ui32Remaining = g_sUSBHEP0State.ui32BytesRemaining;

    return(ui32DataSize - ui32Remaining);
}

//*****************************************************************************
//
// Starts enumerating a new device connected via the hub.
//
// \param ui32Index is the index of the USB controller to use.
// \param ui32Hub is the hub address from which the connection is being made.
// \param ui32Port is the hub port number that the new device is connected to.
// \param pui8ConfigPool is memory to be used to store the device's
// configuration descriptor.
// \param ui32ConfigSize is the number of bytes available in the buffer pointed
// to by pui8ConfigPool.
//
// This function is called by the hub class driver after it has detected a new
// device connection and reset the device.
//
// \return Returns the index of the device allocated or 0 if no resources are
//         available. Device index 0 is the hub itself.
//
//*****************************************************************************
uint32_t
USBHCDHubDeviceConnected(uint32_t ui32Index, uint8_t ui8Hub,
                         uint8_t ui8Port, bool bLowSpeed)
{
    uint32_t ui32DevIndex;

    //
    // Debug sanity checks.
    //
    ASSERT(ui32Index == 0);
    ASSERT(ui8Port);

    DEBUG_OUTPUT("Connection from hub %d, port %d.\n", ui8Hub, ui8Port);

    //
    // Look for a free slot in the device table.
    //
    for(ui32DevIndex = 1; ui32DevIndex <= MAX_USB_DEVICES; ui32DevIndex++)
    {
        if((g_sUSBHCD.psUSBDevice[ui32DevIndex].ui32Flags &
            USBHDEV_FLAG_ALLOCATED) == 0)
        {
            //
            // We found one. Set the state to ensure that it gets enumerated.
            //
            g_sUSBHCD.psUSBDevice[ui32DevIndex].ui32Flags =
                                                        USBHDEV_FLAG_ALLOCATED;
            g_sUSBHCD.psUSBDevice[ui32DevIndex].psConfigDescriptor->bLength = 0;
            g_sUSBHCD.psUSBDevice[ui32DevIndex].ui8Hub = ui8Hub;
            g_sUSBHCD.psUSBDevice[ui32DevIndex].ui8HubPort = ui8Port;
            g_sUSBHCD.psUSBDevice[ui32DevIndex].bLowSpeed = bLowSpeed;
            g_sUSBHCD.psUSBDevice[ui32DevIndex].sDeviceDescriptor.bLength = 0;

            //
            // Set the state to ensure enumeration begins.
            //
            g_sUSBHCD.piDeviceState[ui32DevIndex] = eHCDDevConnectedHub;

            DEBUG_OUTPUT("Allocating device %d\n", ui32DevIndex);

            //
            // Pass the device index back to the hub driver.
            //
            return(ui32DevIndex);
        }
    }

    //
    // If we get here, there are device slots available so send back an invalid
    // device index to tell the caller to ignore this device.
    //
    return(0);
}

//*****************************************************************************
//
// Called when a device is disconnected from a hub.
//
// \param ui32Index is the index of the USB controller to use.
// \param ui32DevIndex is the device index for the USB device that was
// disconnected.
//
//*****************************************************************************
void
USBHCDHubDeviceDisconnected(uint32_t ui32Index, uint32_t ui32DevIndex)
{
    //
    // Debug sanity checks.
    //
    ASSERT(ui32Index == 0);
    ASSERT(ui32DevIndex && (ui32DevIndex <= MAX_USB_DEVICES));

    DEBUG_OUTPUT("Disconnection from hub %d, port %d, device %d\n",
            g_sUSBHCD.psUSBDevice[ui32DevIndex].ui8Hub,
            g_sUSBHCD.psUSBDevice[ui32DevIndex].ui8HubPort, ui32DevIndex);

    //
    // Set the device state to ensure that USBHCDMain cleans it up.
    //
    g_sUSBHCD.piDeviceState[ui32DevIndex] = eHCDDevDisconnected;
}

//*****************************************************************************
//
// This is the endpoint 0 interrupt handler.
//
// \return None.
//
//*****************************************************************************
static void
USBHCDEnumHandler(void)
{
    uint32_t ui32EPStatus;
    uint32_t ui32DataSize;

    //
    // Get the end point 0 status.
    //
    ui32EPStatus = MAP_USBEndpointStatus(USB0_BASE, USB_EP_0);

    //
    // If there was an error then go to the error state.
    //
    if(ui32EPStatus == USB_HOST_EP0_ERROR)
    {
        //
        // Clear this status indicating that the status packet was
        // received.
        //
        MAP_USBHostEndpointStatusClear(USB0_BASE, USB_EP_0,
                                       USB_HOST_EP0_ERROR);
        MAP_USBFIFOFlush(USB0_BASE, USB_EP_0, 0);

        //
        // Just go back to the idle state.
        //
        g_sUSBHEP0State.iState = eEP0StateError;

        return;
    }

    switch(g_sUSBHEP0State.iState)
    {
        //
        // Handle the status state, this is a transitory state from
        // USB_STATE_TX or USB_STATE_RX back to USB_STATE_IDLE.
        //
        case eEP0StateStatus:
        {
            //
            // Handle the case of a received status packet.
            //
            if(ui32EPStatus & (USB_HOST_EP0_RXPKTRDY | USB_HOST_EP0_STATUS))
            {
                //
                // Clear this status indicating that the status packet was
                // received.
                //
                MAP_USBHostEndpointStatusClear(USB0_BASE, USB_EP_0,
                                               (USB_HOST_EP0_RXPKTRDY |
                                                USB_HOST_EP0_STATUS));
            }

            //
            // Just go back to the idle state.
            //
            g_sUSBHEP0State.iState = eEP0StateIdle;

            break;
        }

        //
        // This state triggers a STATUS IN request from the device.
        //
        case eEP0StateStatusIN:
        {
            //
            // Generate an IN request from the device.
            //
            MAP_USBHostRequestStatus(USB0_BASE);

            //
            // Change to the status phase and wait for the response.
            //
            g_sUSBHEP0State.iState =  eEP0StateStatus;

            break;
        }

        //
        // In the IDLE state the code is waiting to receive data from the host.
        //
        case eEP0StateIdle:
        {
            break;
        }

        //
        // Data is still being sent to the host so handle this in the
        // EP0StateTx() function.
        //
        case eEP0StateSetupOUT:
        {
            //
            // Send remaining data if necessary.
            //
            USBHCDEP0StateTx();

            break;
        }

        //
        // Handle the receive state for commands that are receiving data on
        // endpoint 0.
        //
        case eEP0StateSetupIN:
        {
            //
            // Generate a new IN request to the device.
            //
            MAP_USBHostRequestIN(USB0_BASE, USB_EP_0);

            //
            // Proceed to the RX state to receive the requested data.
            //
            g_sUSBHEP0State.iState =  eEP0StateRx;

            break;
        }

        //
        // The endpoint remains in this state until all requested data has
        // been received.
        //
        case eEP0StateRx:
        {
            //
            // There was a stall on endpoint 0 so go back to the idle state
            // as this command has been terminated.
            //
            if(ui32EPStatus & USB_HOST_EP0_RX_STALL)
            {
                g_sUSBHEP0State.iState = eEP0StateIdle;

                //
                // Clear the stalled state on endpoint 0.
                //
                MAP_USBHostEndpointStatusClear(USB0_BASE, USB_EP_0,
                                               ui32EPStatus);
                break;
            }

            //
            // Set the number of bytes to get out of this next packet.
            //
            ui32DataSize = g_sUSBHEP0State.ui32BytesRemaining;
            if(ui32DataSize > g_sUSBHEP0State.ui32MaxPacketSize)
            {
                //
                // Don't send more than EP0_MAX_PACKET_SIZE bytes.
                //
                ui32DataSize = MAX_PACKET_SIZE_EP0;
            }

            if(ui32DataSize != 0)
            {
                //
                // Get the data from the USB controller end point 0.
                //
                MAP_USBEndpointDataGet(USB0_BASE, USB_EP_0,
                                       g_sUSBHEP0State.pui8Data,
                                       &ui32DataSize);
            }

            //
            // Advance the pointer.
            //
            g_sUSBHEP0State.pui8Data += ui32DataSize;

            //
            // Decrement the number of bytes that are being waited on.
            //
            g_sUSBHEP0State.ui32BytesRemaining -= ui32DataSize;

            //
            // Need to ack the data on end point 0 in this case
            // without setting data end.
            //
            MAP_USBHostEndpointDataAck(USB0_BASE, USB_EP_0);

            //
            // If there was not more than the maximum packet size bytes of data
            // the this was a int16_t packet and indicates that this transfer
            // is complete.  If there were exactly
            // g_sUSBHEP0State.ui32MaxPacketSize remaining then there still
            // needs to be null packet sent before this transfer is complete.
            //
            if((ui32DataSize < g_sUSBHEP0State.ui32MaxPacketSize) ||
               (g_sUSBHEP0State.ui32BytesRemaining == 0))
            {
                //
                // Return to the idle state.
                //
                g_sUSBHEP0State.iState =  eEP0StateStatus;

                //
                // No more data.
                //
                g_sUSBHEP0State.pui8Data = 0;

                //
                // Send a null packet to acknowledge that all data was
                // received.
                //
                MAP_USBEndpointDataSend(USB0_BASE, USB_EP_0, USB_TRANS_STATUS);
            }
            else
            {
                //
                // Request more data.
                //
                MAP_USBHostRequestIN(USB0_BASE, USB_EP_0);
            }
            break;
        }

        //
        // The device stalled endpoint zero so check if the stall needs to be
        // cleared once it has been successfully sent.
        //
        case eEP0StateStall:
        {
            //
            // Reset the global end point 0 state to IDLE.
            //
            g_sUSBHEP0State.iState = eEP0StateIdle;

            break;
        }

        //
        // Halt on an unknown state, but only in DEBUG builds.
        //
        default:
        {
            ASSERT(0);
            break;
        }
    }
}

//*****************************************************************************
//
// This internal function handles sending data on endpoint 0.
//
// \return None.
//
//*****************************************************************************
static void
USBHCDEP0StateTx(void)
{
    uint32_t ui32NumBytes;
    uint8_t *pui8Data;

    //
    // In the TX state on endpoint 0.
    //
    g_sUSBHEP0State.iState = eEP0StateSetupOUT;

    //
    // Set the number of bytes to send this iteration.
    //
    ui32NumBytes = g_sUSBHEP0State.ui32BytesRemaining;

    //
    // Limit individual transfers to 64 bytes.
    //
    if(ui32NumBytes > 64)
    {
        ui32NumBytes = 64;
    }

    //
    // Save the pointer so that it can be passed to the USBEndpointDataPut()
    // function.
    //
    pui8Data = (uint8_t *)g_sUSBHEP0State.pui8Data;

    //
    // Advance the data pointer and counter to the next data to be sent.
    //
    g_sUSBHEP0State.ui32BytesRemaining -= ui32NumBytes;
    g_sUSBHEP0State.pui8Data += ui32NumBytes;

    //
    // Put the data in the correct FIFO.
    //
    MAP_USBEndpointDataPut(USB0_BASE, USB_EP_0, pui8Data, ui32NumBytes);

    //
    // If this is exactly 64 then don't set the last packet yet.
    //
    if(ui32NumBytes == 64)
    {
        //
        // There is more data to send or exactly 64 bytes were sent, this
        // means that there is either more data coming or a null packet needs
        // to be sent to complete the transaction.
        //
        MAP_USBEndpointDataSend(USB0_BASE, USB_EP_0, USB_TRANS_OUT);
    }
    else
    {
        //
        // Send the last bit of data.
        //
        MAP_USBEndpointDataSend(USB0_BASE, USB_EP_0, USB_TRANS_OUT);

        //
        // Now go to the status state and wait for the transmit to complete.
        //
        g_sUSBHEP0State.iState = eEP0StateStatusIN;
    }
}

//*****************************************************************************
//
//! This function returns the USB hub port for the requested device instance.
//!
//! \param ui32Instance is a unique value indicating which device to query.
//!
//! This function returns the USB hub port for the device that is associated
//! with the \e ui32Instance parameter.  The caller must use the value for
//! \e ui32Instance was passed to the application when it receives a
//! USB_EVENT_CONNECTED event.  The function returns the USB hub port for
//! the interface number specified by the \e ui32Interface parameter.
//!
//! \return The USB hub port for the requested interface.
//
//*****************************************************************************
uint8_t
USBHCDDevHubPort(uint32_t ui32Instance)
{
    uint32_t ui32DevIndex;

    ui32DevIndex = HCDInstanceToDevIndex(ui32Instance);

    if(ui32DevIndex == 0xff)
    {
        return(ui32DevIndex);
    }

    return(g_sUSBHCD.psUSBDevice[ui32DevIndex].ui8HubPort);
}

//*****************************************************************************
//
//! This function will return the USB address for the requested device
//! instance.
//!
//! \param ui32Instance is a unique value indicating which device to query.
//!
//! This function returns the USB address for the device that is associated
//! with the \e ui32Instance parameter.  The caller must use a value for
//! \e ui32Instance have been passed to the application when it receives a
//! \b USB_EVENT_CONNECTED event.  The function will return the USB address for
//! the interface number specified by the \e ui32Interface parameter.
//!
//! \return The USB address for the requested interface.
//
//*****************************************************************************
uint8_t
USBHCDDevAddress(uint32_t ui32Instance)
{
    uint32_t ui32DevIndex;

    ui32DevIndex = HCDInstanceToDevIndex(ui32Instance);

    if(ui32DevIndex == 0xff)
    {
        return(ui32DevIndex);
    }

    return(g_sUSBHCD.psUSBDevice[ui32DevIndex].ui32Address);
}

//*****************************************************************************
//
//! This function will return the USB class for the requested device
//! instance.
//!
//! \param ui32Instance is a unique value indicating which device to query.
//! \param ui32Interface is the interface number to query for the USB class.
//!
//! This function returns the USB class for the device that is associated
//! with the \e ui32Instance parameter.  The caller must use a value for
//! \e ui32Instance have been passed to the application when it receives a
//! USB_EVENT_CONNECTED event.  The function will return the USB class for
//! the interface number specified by the \e ui32Interface parameter.  If
//! \e ui32Interface is set to 0xFFFFFFFF then the function will return the USB
//! class for the first interface that is found in the device's USB
//! descriptors.
//!
//! \return The USB class for the requested interface.
//
//*****************************************************************************
uint8_t
USBHCDDevClass(uint32_t ui32Instance, uint32_t ui32Interface)
{
    uint32_t ui32DevIndex;
    tInterfaceDescriptor *psInterface;

    ui32DevIndex = HCDInstanceToDevIndex(ui32Instance);

    //
    // If the instance was not valid return an undefined class.
    //
    if(ui32DevIndex == 0xff)
    {
        return(USB_CLASS_DEVICE);
    }

    //
    // Get the interface descriptor.
    //
    psInterface = USBDescGetInterface(
                        g_sUSBHCD.psUSBDevice[ui32DevIndex].psConfigDescriptor,
                        g_sUSBHCD.psUSBDevice[ui32DevIndex].ui32Interface,
                        ui32Interface);

    //
    // Make sure that the interface requested actually exists.
    //
    if(psInterface)
    {
        //
        // Return the interface class.
        //
        return(psInterface->bInterfaceClass);
    }

    //
    // No valid interface so return an undefined class.
    //
    return(USB_CLASS_DEVICE);
}

//*****************************************************************************
//
//! This function will return the USB subclass for the requested device
//! instance.
//!
//! \param ui32Instance is a unique value indicating which device to query.
//! \param ui32Interface is the interface number to query for the USB subclass.
//!
//! This function returns the USB subclass for the device that is associated
//! with the \e ui32Instance parameter.  The caller must use a value for
//! \e ui32Instance have been passed to the application when it receives a
//! \b USB_EVENT_CONNECTED event.  The function will return the USB subclass
//! for the interface number specified by the \e ui32Interface parameter.  If
//! \e ui32Interface is set to 0xFFFFFFFF then the function will return the USB
//! subclass for the first interface that is found in the device's USB
//! descriptors.
//!
//! \return The USB subclass for the requested interface.
//
//*****************************************************************************
uint8_t
USBHCDDevSubClass(uint32_t ui32Instance, uint32_t ui32Interface)
{
    uint32_t ui32DevIndex;
    tInterfaceDescriptor *psInterface;

    ui32DevIndex = HCDInstanceToDevIndex(ui32Instance);

    //
    // If the instance was not valid return an undefined subclass.
    //
    if(ui32DevIndex == 0xff)
    {
        return(USB_SUBCLASS_UNDEFINED);
    }

    //
    // Get the interface descriptor.
    //
    psInterface = USBDescGetInterface(
                        g_sUSBHCD.psUSBDevice[ui32DevIndex].psConfigDescriptor,
                        g_sUSBHCD.psUSBDevice[ui32DevIndex].ui32Interface,
                        ui32Interface);

    //
    // Make sure that the interface requested actually exists.
    //
    if(psInterface)
    {
        //
        // Return the interface subclass.
        //
        return(psInterface->bInterfaceSubClass);
    }

    //
    // No valid interface so return an undefined subclass.
    //
    return(USB_SUBCLASS_UNDEFINED);
}

//*****************************************************************************
//
//! This function returns the USB protocol for the requested device instance.
//!
//! \param ui32Instance is a unique value indicating which device to query.
//! \param ui32Interface is the interface number to query for the USB protocol.
//!
//! This function returns the USB protocol for the device that is associated
//! with the \e ui32Instance parameter.  The caller must use a value for
//! \e ui32Instance have been passed to the application when it receives a
//! USB_EVENT_CONNECTED event.  The function will return the USB protocol for
//! the interface number specified by the \e ui32Interface parameter.  If
//! \e ui32Interface is set to 0xFFFFFFFF then the function will return the USB
//! protocol for the first interface that is found in the device's USB
//! descriptors.
//!
//! \return The USB protocol for the requested interface.
//
//*****************************************************************************
uint8_t
USBHCDDevProtocol(uint32_t ui32Instance, uint32_t ui32Interface)
{
    uint32_t ui32DevIndex;
    tInterfaceDescriptor *psInterface;

    ui32DevIndex = HCDInstanceToDevIndex(ui32Instance);

    //
    // If the instance was not valid return an undefined protocol.
    //
    if(ui32DevIndex == 0xff)
    {
        return(USB_PROTOCOL_UNDEFINED);
    }

    //
    // Get the interface descriptor.
    //
    psInterface = USBDescGetInterface(
                        g_sUSBHCD.psUSBDevice[ui32DevIndex].psConfigDescriptor,
                        g_sUSBHCD.psUSBDevice[ui32DevIndex].ui32Interface,
                        ui32Interface);

    //
    // Make sure that the interface requested actually exists.
    //
    if(psInterface)
    {
        //
        // Return the interface protocol.
        //
        return(psInterface->bInterfaceProtocol);
    }

    //
    // No valid interface so return an undefined protocol.
    //
    return(USB_PROTOCOL_UNDEFINED);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
