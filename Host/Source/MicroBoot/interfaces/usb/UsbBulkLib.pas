unit UsbBulkLib;
//***************************************************************************************
// Project Name: Wrapper interface for accessing the UsbBulkLib DLL.
//  Description: UsbBulkLib DLL interface unit for Delphi
//    File Name: UsbBulkLib.pas
//
//---------------------------------------------------------------------------------------
//                          C O P Y R I G H T
//---------------------------------------------------------------------------------------
//   Copyright (c) 2011 by Feaser    http://www.feaser.com    All rights reserved
//
//   This software has been carefully tested, but is not guaranteed for any particular
// purpose. The author does not offer any warranties and does not guarantee the accuracy,
//   adequacy, or completeness of the software and is not responsible for any errors or
//              omissions or the results obtained from use of the software.
//
//---------------------------------------------------------------------------------------
//                            L I C E N S E
//---------------------------------------------------------------------------------------
// This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE. See the GNU General Public License for more details.
//
// You have received a copy of the GNU General Public License along with OpenBLT. It 
// should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
//
//***************************************************************************************
interface

//***************************************************************************************
// Global includes
//****************************************************************************************
uses
  SysUtils;


//***************************************************************************************
// Global constant declarations
//****************************************************************************************
const
  UBL_ERROR   = 0;
  UBL_OKAY    = 1;
  UBL_TIMEOUT = 2;


//***************************************************************************************
// Function prototypes
//****************************************************************************************
function  UblOpen(guid: PGUID): Byte; stdcall;
procedure UblClose; stdcall;
function  UblTransmit(data: PByteArray; len: Word): Byte; stdcall;
function  UblReceive(data: PByteArray; len: Word; timeout: Longword): Byte; stdcall;

  
implementation
//***************************************************************************************
// Local constant declarations
//****************************************************************************************
const DLL_Name = 'UsbBulkLib.dll';


//***************************************************************************************
// NAME:           UblOpen
// PARAMETER:      guid pointer to GUID of the USB bulk device as found in the driver's 
//                      INF-file.
// RETURN VALUE:   UBL_OKAY if successful, UBL_ERROR otherwise.
// DESCRIPTION:    Opens and configures the connection with the USB bulk device.
//
//***************************************************************************************
function UblOpen(guid: PGUID): Byte; stdcall;
external DLL_Name;


//***************************************************************************************
// NAME:           UblClose
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Closes the connection with the USB bulk device and frees all the 
//                 related handles.
//
//***************************************************************************************
procedure UblClose; stdcall;
external DLL_Name;


//***************************************************************************************
// NAME:           UblDllTransmit
// PARAMETER:      data    pointer to byte array with transmit data.
//                 len     number of bytes to transmit.
// RETURN VALUE:   UBL_OKAY if successful, UBL_ERROR otherwise.
// DESCRIPTION:    Starts transmission of the data on the bulk OUT pipe. Because USB
//                 bulk transmissions are quick, this function does not use the 
//                 overlapped functionality, which means the caller is blocked until
//                 the tranmission completed.
//
//***************************************************************************************
function UblTransmit(data: PByteArray; len: Word): Byte; stdcall;
external DLL_Name;


//***************************************************************************************
// NAME:           UblDllReceive
// PARAMETER:      data    pointer to byte array where the data will be stored.
//                 len     number of bytes to receive.
//                 timeout max time in milliseconds for the read to complete.
// RETURN VALUE:   UBL_OKAY if successful, UBL_TIMEOUT if failure due to timeout or 
//                 UBL_ERROR otherwise.
// DESCRIPTION:    Starts the asynchronous reception of the data from the bulk IN pipe. 
//                 This function makes use of the overlapped functionality, which means 
//                 the calling thread if placed into sleep mode until the reception is 
//                 complete.
//
//***************************************************************************************
function UblReceive(data: PByteArray; len: Word; timeout: Longword): Byte; stdcall;
external DLL_Name;


end.
//********************************** end of UsbBulkLib.pas ******************************

