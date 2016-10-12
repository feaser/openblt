unit uBootInterface;
//***************************************************************************************
// Project Name: TMicroBootInterface component for Borland Delphi
//  Description: Encapsulates the MicroBoot DLL interface
//    File Name: uBootInterface.pas
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
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
// DLL Interface Callbacks - modifications requires potential update of all interfaces!
type
  TStartedEvent  = procedure(length: Longword) of object;
  TProgressEvent = procedure(progress: Longword) of object;
  TDoneEvent     = procedure  of object;
  TErrorEvent    = procedure(error: ShortString) of object;
  TLogEvent      = procedure(info:  ShortString) of object;
  TInfoEvent     = procedure(info:  ShortString) of object;

// DLL Interface Methods - modifications requires potential update of all interfaces!
type
  TDllMbiInit        = procedure(cbStarted: TStartedEvent; cbProgress: TProgressEvent;
                                 cbDone: TDoneEvent; cbError: TErrorEvent;
                                 cbLog: TLogEvent; cbInfo: TInfoEvent); stdcall;
  TDllMbiStart       = procedure(fileName: ShortString); stdcall;
  TDllMbiStop        = procedure; stdcall;
  TDllMbiDeInit      = procedure; stdcall;
  TDllMbiName        = function : ShortString; stdcall;
  TDllMbiDescription = function : ShortString; stdcall;
  TDllMbiVersion     = function : Longword; stdcall;
  TDllMbiConfigure   = procedure; stdcall;
  TDllMbiVInterface  = function : Longword; stdcall;

// Interface Class
type
  TMicroBootInterface = class(TComponent)
  private
    { Private declarations }
    DllMbiInit        : TDllMbiInit;
    DllMbiStart       : TDllMbiStart;
    DllMbiStop        : TDllMbiStop;
    DllMbiDeInit      : TDllMbiDeInit;
    DllMbiName        : TDllMbiName;
    DllMbiDescription : TDllMbiDescription;
    DllMbiVersion     : TDllMbiVersion;
    DllMbiConfigure   : TDllMbiConfigure;
    DllMbiVInterface  : TDllMbiVInterface;
  protected
    { Protected declarations }
    FLibraryFile   : string;
    FLibraryHandle : THandle;
    FOnStarted     : TStartedEvent;
    FOnProgress    : TProgressEvent;
    FOnDone        : TDoneEvent;
    FOnError       : TErrorEvent;
    FOnLog         : TLogEvent;
    FOnInfo        : TInfoEvent;
  public
    { Public declarations }
    constructor Create(AOwner: TComponent); override;
    destructor  Destroy; override;
    function    Enable(libraryFile: string; evStarted: TStartedEvent;
                       evProgress: TProgressEvent; evDone: TDoneEvent;
                       evError: TErrorEvent; evLog: TLogEvent;
                       evInfo: TInfoEvent) : Boolean;
    procedure   Disable;
    procedure   Download(fileName: ShortString);
    procedure   Cancel;
    function    Name : ShortString;
    function    Description : ShortString;
    function    Version : Longword;
    procedure   Configure;
    function    VInterface : Longword;
  published
    { Published declarations }
  end;


implementation
//***************************************************************************************
// NAME:           Create
// PARAMETER:      AOwner : owner of the component
// RETURN VALUE:   none
// DESCRIPTION:    Component constructor. Calls TComponent's constructor and initializes
//                 the private property variables to their default values.
//
//***************************************************************************************
constructor TMicroBootInterface.Create(AOwner: TComponent);
begin
  // call inherited constructor
  inherited Create( AOwner );

  // initialize the callback pointers
  FOnStarted  := nil;
  FOnProgress := nil;
  FOnDone     := nil;
  FOnError    := nil;
  FOnLog      := nil;
  FOnInfo     := nil;

  // initialize the properties
  FLibraryFile   := '';
  FLibraryHandle := 0;
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Destroy
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Component destructor. Calls TComponent's destructor
//
//***************************************************************************************
destructor TMicroBootInterface.Destroy;
begin
  if FLibraryHandle <> 0 then
  begin
    FreeLibrary(FLibraryHandle); // release the handle
  end;

  inherited Destroy; // call inherited destructor
end; //*** end of Destroy ***


//***************************************************************************************
// NAME:           Enable
// PARAMETER:      name of library file and pointers to the callback functions.
// RETURN VALUE:   true: interface library ready, false: error occurred.
// DESCRIPTION:    Used to connect the interface library to the application.
//
//***************************************************************************************
function TMicroBootInterface.Enable(libraryFile: string; evStarted: TStartedEvent;
                                    evProgress: TProgressEvent; evDone: TDoneEvent;
                                    evError: TErrorEvent; evLog: TLogEvent;
                                    evInfo :TInfoEvent) : Boolean;
var
  Initialized : Boolean;
begin
  Initialized := True;

  // first make sure the interface is disabled
  Disable;

  // set the library file
  if (FileExists(libraryFile)) and (LowerCase(ExtractFileExt(libraryFile)) = '.dll') then
  begin
    FLibraryFile := libraryFile;
  end;

  // set the callback functions
  if Assigned(evStarted)  then FOnStarted  := evStarted;
  if Assigned(evProgress) then FOnProgress := evProgress;
  if Assigned(evDone)     then FOnDone     := evDone;
  if Assigned(evError)    then FOnError    := evError;
  if Assigned(evLog)      then FOnLog      := evLog;
  if Assigned(evInfo)     then FOnInfo     := evInfo;

  // check if callback functions are configured properly
  if not Assigned(FOnStarted)  then Initialized := False;
  if not Assigned(FOnProgress) then Initialized := False;
  if not Assigned(FOnDone)     then Initialized := False;
  if not Assigned(FOnError)    then Initialized := False;
  if not Assigned(FOnLog)      then Initialized := False;
  if not Assigned(FOnInfo)     then Initialized := False;

  // check if a proper library file is configured
  if FLibraryFile = '' then Initialized := False;

  // only continue if everything was okay sofar
  if Initialized = True then
  begin
    // attempt to obtain a handle to the interface library
    FLibraryHandle := LoadLibrary(PChar(FLibraryFile));
    if FLibraryHandle = 0 then Initialized := False;
  end;

  // only continue if everything was okay sofar
  if Initialized = True then
  begin
    // attempt to obtain the function pointers from the interface library
    @DllMbiInit        := GetProcAddress(FLibraryHandle, 'MbiInit');
    @DllMbiStart       := GetProcAddress(FLibraryHandle, 'MbiStart');
    @DllMbiStop        := GetProcAddress(FLibraryHandle, 'MbiStop');
    @DllMbiDeInit      := GetProcAddress(FLibraryHandle, 'MbiDeInit');
    @DllMbiName        := GetProcAddress(FLibraryHandle, 'MbiName');
    @DllMbiDescription := GetProcAddress(FLibraryHandle, 'MbiDescription');
    @DllMbiVersion     := GetProcAddress(FLibraryHandle, 'MbiVersion');
    @DllMbiConfigure   := GetProcAddress(FLibraryHandle, 'MbiConfigure');
    @DllMbiVInterface  := GetProcAddress(FLibraryHandle, 'MbiVInterface');
  end;

  // check if the functions were found in the interface library
  if not Assigned(DllMbiInit)        then Initialized := False;
  if not Assigned(DllMbiStart)       then Initialized := False;
  if not Assigned(DllMbiStop)        then Initialized := False;
  if not Assigned(DllMbiDeInit)      then Initialized := False;
  if not Assigned(DllMbiName)        then Initialized := False;
  if not Assigned(DllMbiDescription) then Initialized := False;
  if not Assigned(DllMbiVersion)     then Initialized := False;
  if not Assigned(DllMbiConfigure)   then Initialized := False;
  if not Assigned(DllMbiVInterface)  then Initialized := False;

  // only continue if everything was okay sofar
  if Initialized = True then
  begin
    // pass callback function pointers on to the interface library
    DllMbiInit(FOnStarted, FOnProgress, FOnDone, FOnError, FOnLog, FOnInfo);
  end
  else
  begin
    // error occured so make sure to reset the handle to the interface library
    FLibraryHandle := 0;
  end;

  Result := Initialized;
end; //*** end of Enable ***


//***************************************************************************************
// NAME:           Download
// PARAMETER:      filename with full path
// RETURN VALUE:   none
// DESCRIPTION:    Requests the interface library to start the download of a file.
//
//***************************************************************************************
procedure TMicroBootInterface.Download(fileName: ShortString);
begin
  // only continue with we have a valid interface library handle
  if FLibraryHandle <> 0 then
  begin
    // pass control for file download to the library
    DllMbiStart(fileName);
  end;
end; //*** end of Download ***


//***************************************************************************************
// NAME:           Cancel
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Cancels a possible active file download.
//
//***************************************************************************************
procedure TMicroBootInterface.Cancel;
begin
  // only continue with we have a valid interface library handle
  if FLibraryHandle <> 0 then
  begin
    DllMbiStop; // let interface library handle the stop request
  end;
end; //*** end of Cancel ***


//***************************************************************************************
// NAME:           Disable
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Disables the interface library.
//
//***************************************************************************************
procedure TMicroBootInterface.Disable;
begin
  // only continue with we have a valid interface library handle
  if FLibraryHandle <> 0 then
  begin
    DllMbiDeInit; // inform the dll aswell that we're disabling
    FreeLibrary(FLibraryHandle); // release the handle
  end;

  // initialize the callback pointers
  FOnStarted  := nil;
  FOnProgress := nil;
  FOnDone     := nil;
  FOnError    := nil;
  FOnLog      := nil;
  FOnInfo     := nil;

  // initialize the properties
  FLibraryFile   := '';
  FLibraryHandle := 0;
end; //*** end of Disable ***


//***************************************************************************************
// NAME:           Name
// PARAMETER:      none
// RETURN VALUE:   Name of the interface library
// DESCRIPTION:    Obtains the name of the interface library.
//
//***************************************************************************************
function TMicroBootInterface.Name : ShortString;
begin
  // only continue with we have a valid interface library handle
  if FLibraryHandle <> 0 then
  begin
    Result := DllMbiName; // obtain the request info from the interface
  end
  else
  begin
    Result := '';
  end;
end; //*** end of Name ***


//***************************************************************************************
// NAME:           Description
// PARAMETER:      none
// RETURN VALUE:   Description of the interface library
// DESCRIPTION:    Obtains the description of the interface library.
//
//***************************************************************************************
function TMicroBootInterface.Description : ShortString;
begin
  // only continue with we have a valid interface library handle
  if FLibraryHandle <> 0 then
  begin
    Result := DllMbiDescription; // obtain the request info from the interface
  end
  else
  begin
    Result := '';
  end;
end; //*** end of Description ***


//***************************************************************************************
// NAME:           Version
// PARAMETER:      none
// RETURN VALUE:   version of the library interface
// DESCRIPTION:    Obtains the version of the interface library.
//
//***************************************************************************************
function TMicroBootInterface.Version : Longword;
begin
  // only continue with we have a valid interface library handle
  if FLibraryHandle <> 0 then
  begin
    Result := DllMbiVersion; // obtain the request info from the interface
  end
  else
  begin
    Result := 0;
  end;
end; //*** end of Version ***


//***************************************************************************************
// NAME:           VInterface
// PARAMETER:      none
// RETURN VALUE:   Version of uBootInterface.pas
// DESCRIPTION:    Obtains the version of the uBootInterface that is supported by the
//                 interface library.
//
//***************************************************************************************
function TMicroBootInterface.VInterface : Longword;
begin
  // only continue with we have a valid interface library handle
  if FLibraryHandle <> 0 then
  begin
    Result := DllMbiVInterface; // obtain the request info from the interface
  end
  else
  begin
    Result := 0;
  end;
end; //*** end of Version ***


//***************************************************************************************
// NAME:           Configure
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Used to request the configuration of the interface library.
//
//***************************************************************************************
procedure TMicroBootInterface.Configure;
begin
  // only continue with we have a valid interface library handle
  if FLibraryHandle <> 0 then
  begin
    DllMbiConfigure; // let interface handle the configuration request
  end;
end; //*** end of Configure ***


end.
//******************************* end of uBootInterface.pas *****************************


