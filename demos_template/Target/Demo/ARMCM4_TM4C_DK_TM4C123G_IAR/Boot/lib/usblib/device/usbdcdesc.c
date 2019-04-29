//*****************************************************************************
//
// usbcdesc.c - Config descriptor parsing functions.
//
// Copyright (c) 2007-2013 Texas Instruments Incorporated.  All rights reserved.
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
#include "driverlib/usbdrv.h"
#include "usblib/usblib.h"
#include "usblib/usblibpriv.h"
#include "usblib/device/usbdevice.h"

//*****************************************************************************
//
// The functions in this file mirror the descriptor parsing APIs available
// in usblib.h but parse configuration descriptors defined in terms of a list
// of sections rather than as a single block of descriptor data.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup device_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! \internal
//!
//! Walk to the next descriptor after the supplied one within a section-based
//! config descriptor.
//!
//! \param psConfig points to the header structure for the configuration
//! descriptor which contains \e pi16Desc.
//! \param pui32Sec points to a variable containing the section within
//! \e psConfig which contains \e pi16Desc.
//! \param pi16Desc points to the descriptor that we want to step past.
//!
//! This function walks forward one descriptor within a configuration
//! descriptor.  The value returned is a pointer to the header of the next
//! descriptor after the descriptor supplied in \e pi16Desc.  If the next
//! descriptor is in the next section, \e *pui32Sec will be incremented
//! accordingly.
//!
//! \return Returns a pointer to the next descriptor in the configuration
//! descriptor.
//
//*****************************************************************************
static tDescriptorHeader *
NextConfigDescGet(const tConfigHeader *psConfig, uint32_t *pui32Sec,
                  tDescriptorHeader *psDesc)
{
    //
    // Determine where the next descriptor after the supplied one should be
    // assuming it is within the current section.
    //
    psDesc = NEXT_USB_DESCRIPTOR(psDesc);

    //
    // Did we run off the end of the section?
    //
    if((uint8_t *)psDesc >= (psConfig->psSections[*pui32Sec]->pui8Data +
                             psConfig->psSections[*pui32Sec]->ui16Size))
    {
        //
        // Yes - move to the next section.
        //
        (*pui32Sec)++;

        //
        // Are we still within the configuration descriptor?
        //
        if(*pui32Sec < psConfig->ui8NumSections)
        {
            //
            // Yes - the new descriptor is at the start of the new section.
            //
            psDesc =
                (tDescriptorHeader *)psConfig->psSections[*pui32Sec]->pui8Data;
        }
        else
        {
            //
            // No - we ran off the end of the descriptor so return NULL.
            //
            psDesc = (tDescriptorHeader *)0;
        }
    }

    //
    // Return the new descriptor pointer.
    //
    return(psDesc);
}

//*****************************************************************************
//
//! \internal
//!
//! Returns a pointer to the n-th interface descriptor in a configuration
//! descriptor with the supplied interface number.
//!
//! \param psConfig points to the header structure for the configuration
//! descriptor to search.
//! \param ui8InterfaceNumber is the interface number of the descriptor to
//! query.
//! \param ui32Index is the zero based index of the descriptor.
//! \param pui32Section points to storage which is written with the index
//! of the section containing the returned descriptor.
//!
//! This function returns a pointer to the n-th interface descriptor in the
//! supplied configuration which has the requested interface number.  It may be
//! used by a client to retrieve the descriptors for each alternate setting
//! of a given interface within the configuration passed.
//!
//! \return Returns a pointer to the n-th interface descriptor with interface
//! number as specified or NULL of this descriptor does not exist.
//
//*****************************************************************************
static tInterfaceDescriptor *
ConfigAlternateInterfaceGet(const tConfigHeader *psConfig,
                            uint8_t ui8InterfaceNumber, uint32_t ui32Index,
                            uint32_t *pui32Section)
{
    tDescriptorHeader *psDescCheck;
    uint32_t ui32Count, ui32Sec;

    //
    // Set up for our descriptor counting loop.
    //
    psDescCheck = (tDescriptorHeader *)psConfig->psSections[0]->pui8Data;
    ui32Count = 0;
    ui32Sec = 0;

    //
    // Keep looking through the supplied data until we reach the end.
    //
    while(psDescCheck)
    {
        //
        // Does this descriptor match the type passed (if a specific type
        // has been specified)?
        //
        if((psDescCheck->bDescriptorType == USB_DTYPE_INTERFACE) &&
            (((tInterfaceDescriptor *)psDescCheck)->bInterfaceNumber ==
             ui8InterfaceNumber))
        {
            //
            // This is an interface descriptor for interface
            // ui8InterfaceNumber.  Determine if this is the n-th one we have
            // found and, if so, return its pointer.
            //
            if(ui32Count == ui32Index)
            {
                //
                // Found it - return the pointer and section number.
                //
                *pui32Section = ui32Sec;
                return((tInterfaceDescriptor *)psDescCheck);
            }

            //
            // Increment our count of matching descriptors found and go back
            // to look for another since we have not yet reached the n-th
            // match.
            //
            ui32Count++;
        }

        //
        // Move on to the next descriptor.
        //
        psDescCheck = NextConfigDescGet(psConfig, &ui32Sec, psDescCheck);
    }

    //
    // If we drop out the end of the loop, we did not find the requested
    // descriptor so return NULL.
    //
    return((tInterfaceDescriptor *)0);
}

//*****************************************************************************
//
//! \internal
//!
//! Determines the total length of a configuration descriptor defined in terms
//! of a collection of concatenated sections.
//!
//! \param psConfig points to the header structure for the configuration
//! descriptor whose size is to be determined.
//!
//! \return Returns the number of bytes in the configuration descriptor will
//! result from concatenating the required sections.
//
//*****************************************************************************
uint32_t
USBDCDConfigDescGetSize(const tConfigHeader *psConfig)
{
    uint32_t ui32Loop, ui32Len;

    ui32Len = 0;

    //
    // Determine the size of the whole descriptor by adding the sizes of
    // each section which will be concatenated to produce it.
    //
    for(ui32Loop = 0; ui32Loop < psConfig->ui8NumSections; ui32Loop++)
    {
        ui32Len += psConfig->psSections[ui32Loop]->ui16Size;
    }

    return(ui32Len);
}

//*****************************************************************************
//
//! \internal
//!
//! Determines the number of individual descriptors of a particular type within
//! a supplied configuration descriptor.
//!
//! \param psConfig points to the header structure for the configuration
//! descriptor that is to be searched.
//! \param ui32Type identifies the type of descriptor that is to be counted.
//! If the value is \b USB_DESC_ANY, the function returns the total number of
//! descriptors regardless of type.
//!
//! This function can be used to count the number of descriptors of a
//! particular type within a configuration descriptor.  The caller can provide
//! a specific type value which the function matches against the second byte
//! of each descriptor or, alternatively, can specify \b USB_DESC_ANY to have
//! the function count all descriptors regardless of their type.
//!
//! The search performed by this function traverses through the list of
//! sections comprising the configuration descriptor.  Note that the similar
//! top-level function, USBDescGetNum(), searches through a single, contiguous
//! block of data to perform the same enumeration.
//!
//! \return Returns the number of descriptors found in the supplied block of
//! data.
//
//*****************************************************************************
uint32_t
USBDCDConfigDescGetNum(const tConfigHeader *psConfig, uint32_t ui32Type)
{
    uint32_t ui32Section, ui32NumDescs;

    //
    // Initialize our counts.
    //
    ui32NumDescs = 0;

    //
    // Determine the number of descriptors of the given type in each of the
    // sections comprising the configuration descriptor.  Note that this
    // assumes each section contains only whole descriptors!
    //
    for(ui32Section = 0; ui32Section < (uint32_t)psConfig->ui8NumSections;
        ui32Section++)
    {
        ui32NumDescs += USBDescGetNum(
            (tDescriptorHeader *)psConfig->psSections[ui32Section]->pui8Data,
            psConfig->psSections[ui32Section]->ui16Size, ui32Type);
    }

    return(ui32NumDescs);
}

//*****************************************************************************
//
//! \internal
//!
//! Finds the n-th descriptor of a particular type within the supplied
//! configuration descriptor.
//!
//! \param psConfig points to the header structure for the configuration
//! descriptor that is to be searched.
//! \param ui32Type identifies the type of descriptor that is to be found.  If
//! the value is \b USB_DESC_ANY, the function returns a pointer to the n-th
//! descriptor regardless of type.
//! \param ui32Index is the zero based index of the descriptor whose pointer is
//! to be returned.  For example, passing value 1 in \e ui32Index returns the
//! second matching descriptor.
//! \param pui32Section points to storage which will receive the section index
//! containing the requested descriptor.
//!
//! Return a pointer to the n-th descriptor of a particular type found in the
//! configuration descriptor passed.
//!
//! The search performed by this function traverses through the list of
//! sections comprising the configuration descriptor.  Note that the similar
//! top-level function, USBDescGet(), searches through a single, contiguous
//! block of data to perform the same enumeration.
//!
//! \return Returns a pointer to the header of the required descriptor if
//! found or NULL otherwise.
//
//*****************************************************************************
tDescriptorHeader *
USBDCDConfigDescGet(const tConfigHeader *psConfig, uint32_t ui32Type,
                    uint32_t ui32Index, uint32_t *pui32Section)
{
    uint32_t ui32Section, ui32TotalDescs, ui32NumDescs;

    //
    // Initialize our counts.
    //
    ui32TotalDescs = 0;

    //
    // Determine the number of descriptors of the given type in each of the
    // sections comprising the configuration descriptor.  This allows us to
    // determine which section contains the descriptor we are being asked for.
    //
    for(ui32Section = 0; ui32Section < (uint32_t)psConfig->ui8NumSections;
        ui32Section++)
    {
        //
        // How many descriptors of the requested type exist in this section?
        //
        ui32NumDescs = USBDescGetNum(
            (tDescriptorHeader *)psConfig->psSections[ui32Section]->pui8Data,
            psConfig->psSections[ui32Section]->ui16Size, ui32Type);

        //
        // Does this section contain the descriptor whose index we are looking
        // for?
        //
        if((ui32TotalDescs + ui32NumDescs) > ui32Index)
        {
            //
            // We know the requested descriptor exists in the current
            // block so write the section number to the caller's storage.
            //
            *pui32Section = ui32Section;

            //
            // Now find the actual descriptor requested and return its pointer.
            //
            return(USBDescGet(
              (tDescriptorHeader *)psConfig->psSections[ui32Section]->pui8Data,
               psConfig->psSections[ui32Section]->ui16Size,
               ui32Type, ui32Index - ui32TotalDescs));
        }

        //
        // We have not found the required descriptor yet.  Update our running
        // count of the number of type matches found so far then move on to
        // the next section.
        //
        ui32TotalDescs += ui32NumDescs;
    }

    //
    // If we drop out of the loop, we can't find the requested descriptor
    // so return NULL.
    //
    return((tDescriptorHeader *)0);
}

//*****************************************************************************
//
//! \internal
//!
//! Determines the number of different alternate configurations for a given
//! interface within a configuration descriptor.
//!
//! \param psConfig points to the header structure for the configuration
//! descriptor that is to be searched.
//! \param ui8InterfaceNumber is the interface number for which the number of
//! alternate configurations is to be counted.
//!
//! This function can be used to count the number of alternate settings for a
//! specific interface within a configuration.
//!
//! The search performed by this function traverses through the list of
//! sections comprising the configuration descriptor.  Note that the similar
//! top-level function, USBDescGetNumAlternateInterfaces(), searches through
//! a single, contiguous block of data to perform the same enumeration.
//!
//! \return Returns the number of alternate versions of the specified interface
//! or 0 if the interface number supplied cannot be found in the configuration
//! descriptor.
//
//*****************************************************************************
uint32_t
USBDCDConfigGetNumAlternateInterfaces(const tConfigHeader *psConfig,
                                      uint8_t ui8InterfaceNumber)
{
    tDescriptorHeader *psDescCheck;
    uint32_t ui32Count, ui32Sec;

    //
    // Set up for our descriptor counting loop.
    //
    psDescCheck = (tDescriptorHeader *)psConfig->psSections[0]->pui8Data;
    ui32Sec = 0;
    ui32Count = 0;

    //
    // Keep looking through the supplied data until we reach the end.
    //
    while(psDescCheck)
    {
        //
        // Is this an interface descriptor with the required interface number?
        //
        if((psDescCheck->bDescriptorType == USB_DTYPE_INTERFACE) &&
            (((tInterfaceDescriptor *)psDescCheck)->bInterfaceNumber ==
             ui8InterfaceNumber))
        {
            //
            // Yes - increment our count.
            //
            ui32Count++;
        }

        //
        // Move on to the next descriptor.
        //
        psDescCheck = NextConfigDescGet(psConfig, &ui32Sec, psDescCheck);
    }

    //
    // Return the descriptor count to the caller.
    //
    return(ui32Count);
}

//*****************************************************************************
//
//! \internal
//!
//! Returns a pointer to the n-th interface descriptor in a configuration
//! descriptor that applies to the supplied alternate setting number.
//!
//! \param psConfig points to the header structure for the configuration
//! descriptor that is to be searched.
//! \param ui32Index is the zero based index of the interface that is to be
//! found.  If \e ui32Alt is set to a value other than \b USB_DESC_ANY, this
//! is equivalent to the interface number being searched for.
//! \param ui32Alt is the alternate setting number which is to be
//! searched for.  If this value is \b USB_DESC_ANY, the alternate setting
//! is ignored and all interface descriptors are considered in the search.
//! \param pui32Section points to storage which will receive the index of the
//! config descriptor section which contains the requested interface
//! descriptor.
//!
//! Return a pointer to the n-th interface descriptor found in the supplied
//! configuration descriptor.  If \e ui32Alt is not \b USB_DESC_ANY, only
//! interface descriptors which are part of the supplied alternate setting are
//! considered in the search otherwise all interface descriptors are
//! considered.
//!
//! Note that, although alternate settings can be applied on an interface-by-
//! interface basis, the number of interfaces offered is fixed for a given
//! config descriptor.  Hence, this function will correctly find the unique
//! interface descriptor for that interface's alternate setting number \e
//! ui32Alt if \e ui32Index is set to the required interface number and
//! \e ui32Alt is set to a valid alternate setting number for that interface.
//!
//! The search performed by this function traverses through the list of
//! sections comprising the configuration descriptor.  Note that the similar
//! top-level function, USBDescGetInterface(), searches through a single,
//! contiguous block of data to perform the same enumeration.
//!
//! \return Returns a pointer to the required interface descriptor if
//! found or NULL otherwise.
//
//*****************************************************************************
tInterfaceDescriptor *
USBDCDConfigGetInterface(const tConfigHeader *psConfig, uint32_t ui32Index,
                         uint32_t ui32Alt, uint32_t *pui32Section)
{
    //
    // If we are being told to ignore the alternate configuration, this boils
    // down to a very simple query.
    //
    if(ui32Alt == USB_DESC_ANY)
    {
        //
        // Return the ui32Index-th interface descriptor we find in the
        // configuration descriptor.
        //
        return((tInterfaceDescriptor *)USBDCDConfigDescGet(psConfig,
                                                           USB_DTYPE_INTERFACE,
                                                           ui32Index,
                                                           pui32Section));
    }
    else
    {
        //
        // In this case, a specific alternate setting number is required.
        // Given that interface numbers are zero based indices, we can
        // pass the supplied ui32Index parameter directly as the interface
        // number to USBDescGetAlternateInterface() to retrieve the requested
        // interface descriptor pointer.
        //
        return(ConfigAlternateInterfaceGet(psConfig, ui32Index, ui32Alt,
                                           pui32Section));
    }
}

//*****************************************************************************
//
//! \internal
//!
//! Return a pointer to the n-th endpoint descriptor in a particular interface
//! within a configuration descriptor.
//!
//! \param psConfig points to the header structure for the configuration
//! descriptor that is to be searched.
//! \param ui32InterfaceNumber is the interface number whose endpoint is to be
//! found.
//! \param ui32AltCfg is the alternate setting number which is to be searched
//! for.  This must be a valid alternate setting number for the requested
//! interface.
//! \param ui32Index is the zero based index of the endpoint that is to be
//! found within the appropriate alternate setting for the interface.
//!
//! Return a pointer to the n-th endpoint descriptor found in the supplied
//! interface descriptor.  If the \e ui32Index parameter is invalid (greater
//! than or equal to the bNumEndpoints field of the interface descriptor) or
//! the endpoint descriptor cannot be found, the function will return NULL.
//!
//! The search performed by this function traverses through the list of
//! sections comprising the configuration descriptor.  Note that the similar
//! top-level function, USBDescGetInterfaceEndpoint(), searches through a
//! single, contiguous block of data to perform the same enumeration.
//!
//! \return Returns a pointer to the requested endpoint descriptor if
//! found or NULL otherwise.
//
//*****************************************************************************
tEndpointDescriptor *
USBDCDConfigGetInterfaceEndpoint(const tConfigHeader *psConfig,
                                 uint32_t ui32InterfaceNumber,
                                 uint32_t ui32AltCfg, uint32_t ui32Index)
{
    tInterfaceDescriptor *psInterface;
    tDescriptorHeader *psEndpoint;
    uint32_t ui32Section, ui32Count;

    //
    // Find the requested interface descriptor.
    //
    psInterface = USBDCDConfigGetInterface(psConfig, ui32InterfaceNumber,
                                           ui32AltCfg, &ui32Section);

    //
    // Did we find the requested interface?
    //
    if(psInterface)
    {
        //
        // Is the index passed valid?
        //
        if(ui32Index >= psInterface->bNumEndpoints)
        {
            //
            // It's out of bounds so return a NULL.
            //
            return((tEndpointDescriptor *)0);
        }
        else
        {
            //
            // Endpoint index is valid so find the descriptor.  We start from
            // the interface descriptor and look for following endpoint
            // descriptors.
            //
            ui32Count = 0;
            psEndpoint = (tDescriptorHeader *)psInterface;

            while(psEndpoint)
            {
                if(psEndpoint->bDescriptorType == USB_DTYPE_ENDPOINT)
                {
                    //
                    // We found an endpoint descriptor.  Have we reached the
                    // one we want?
                    //
                    if(ui32Count == ui32Index)
                    {
                        //
                        // Yes - return the descriptor pointer to the caller.
                        //
                        return((tEndpointDescriptor *)psEndpoint);
                    }

                    //
                    // Move on to look for the next endpoint.
                    //
                    ui32Count++;
                }

                //
                // Move to the next descriptor.
                //
                psEndpoint = NextConfigDescGet(psConfig, &ui32Section,
                                               psEndpoint);
            }
        }
    }

    //
    // We could not find the requested interface or we got to the end of the
    // descriptor without finding the requested endpoint.
    //
    return((tEndpointDescriptor *)0);

}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
