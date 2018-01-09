unit XcpProtection;
//***************************************************************************************
//  Description: XCP seed and key resource protection interface.
//    File Name: XcpProtection.pas
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
// Includes
//***************************************************************************************
uses
  Windows, Messages, SysUtils, Classes, Forms;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
// DLL Interface Methods
type
  TXcpGetAvailablePrivileges = function(resourcePtr: PByte) : DWORD; cdecl;
  TXcpComputKeyFromSeed = function(resource: Byte; seedLen: Byte; seedPtr: PByteArray;
                                   keyLenPtr: PByte; keyPtr: PByteArray) : DWORD; cdecl;

type
  TXcpProtection = class(TObject)
  private
    FLibHandle             : THandle;
    FLibInitialized        : Boolean;
    FGetAvailablePrivileges: TXcpGetAvailablePrivileges;
    FComputKeyFromSeed     : TXcpComputKeyFromSeed;
  public
    constructor Create(libFile: string);
    destructor  Destroy; override;
    function    GetPrivileges(resourcePtr: PByte) : DWORD;
    function    ComputKeyFromSeed(resource: Byte; seedLen: Byte; seedPtr: PByteArray;
                                  keyLenPtr: PByte; keyPtr: PByteArray) : DWORD;
  end;


implementation

//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor
//
//***************************************************************************************
constructor TXcpProtection.Create(libFile: string);
begin
  // call inherited constructor
  inherited Create;

  // library not yet initialized
  FLibInitialized := false;

  // attempt to obtain the library handle
  if (FileExists(libFile)) and (LowerCase(ExtractFileExt(libFile)) = '.dll') then
  begin
    FLibHandle := LoadLibrary(PChar(libFile));

    if FLibHandle <> 0 then FLibInitialized := true;
  end;

  // only continue if everything was okay sofar
  if FLibInitialized = false then Exit;

  // attempt to obtain the function pointers from the interface library
  @FComputKeyFromSeed      := GetProcAddress(FLibHandle, 'XCP_ComputeKeyFromSeed');
  @FGetAvailablePrivileges := GetProcAddress(FLibHandle, 'XCP_GetAvailablePrivileges');

  // check if the functions were found in the interface library
  if not Assigned(FComputKeyFromSeed)      then FLibInitialized := false;
  if not Assigned(FGetAvailablePrivileges) then FLibInitialized := false;
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Destroy
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class destructor
//
//***************************************************************************************
destructor TXcpProtection.Destroy;
begin
  // release the library and its handle
  if FLibHandle <> 0 then
  begin
    FreeLibrary(FLibHandle);
  end;

  // call inherited destructor
  inherited;
end; //*** end of Destroy ***


//***************************************************************************************
// NAME:           GetPrivileges
// PARAMETER:      resourcePtr : pointer where to store the supported resources
//                               for the key computation
// RETURN VALUE:   0 if success
// DESCRIPTION:    obtains the privileges with available unlock algorithms in the
//                 external library file
//
//***************************************************************************************
function TXcpProtection.GetPrivileges(resourcePtr: PByte) : DWORD;
begin
  if FLibInitialized then
    result := FGetAvailablePrivileges(resourcePtr)
  else
    result := 0;
end; //*** end of GetPrivileges ***


//***************************************************************************************
// NAME:           ComputKeyFromSeed
// PARAMETER:      resource : resource for which the unlock key is requested
//                 seedLen  : length of the seed
//                 seedPtr  : pointer to the seed data
//                 keyLenPtr: pointer where to store the key length
//                 keyPtr   : pointer where to store the key data
// RETURN VALUE:   0 if success
// DESCRIPTION:    Computes the key for the requested resource.
//
//***************************************************************************************
function TXcpProtection.ComputKeyFromSeed(resource: Byte; seedLen: Byte;
                                          seedPtr: PByteArray; keyLenPtr: PByte;
                                          keyPtr: PByteArray) : DWORD;
begin
  if FLibInitialized then
    result := FComputKeyFromSeed(resource, seedLen, seedPtr, keyLenPtr, keyPtr)
  else
    result := 0;
end; //*** end of ComputKeyFromSeed ***


end.
//******************************** end of XcpProtection.pas *****************************

