library openblt_usb;
//***************************************************************************************
// Project Name: MicroBoot Interface for Borland Delphi
//  Description: XCP - USB interface for MicroBoot
//    File Name: openblt_usb.dpr
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


//***************************************************************************************
// Includes
//***************************************************************************************
uses
  Windows,
  Messages,
  Graphics,
  Controls,
  Forms,
  Dialogs,
  SysUtils,
  Classes,
  Extctrls,
  XcpProtection in '..\XcpProtection.pas',
  SRecReader in '..\SRecReader.pas',
  XcpDataFile in '..\XcpDataFile.pas',
  XcpLoader in '..\XcpLoader.pas',
  XcpTransport in 'XcpTransport.pas',
  XcpSettings in 'XcpSettings.pas' {XcpSettingsForm},
  UsbBulkLib in 'UsbBulkLib.pas';


//***************************************************************************************
// Global Constants
//***************************************************************************************
const kMaxProgLen	= 256; // maximum number of bytes to progam at one time


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

type
  TEventHandlers = class // create a dummy class
    procedure OnTimeout(Sender: TObject);
  end;

//***************************************************************************************
// Global Variables
//***************************************************************************************
var
  //--- begin of don't change ---
  AppOnStarted  : TStartedEvent;
  AppOnProgress : TProgressEvent;
  AppOnDone     : TDoneEvent;
  AppOnError    : TErrorEvent;
  AppOnLog      : TLogEvent;
  AppOnInfo     : TInfoEvent;
  //--- end of don't change ---
  timer         : TTimer;
  events        : TEventHandlers;
  loader        : TXcpLoader;
  datafile      : TXcpDataFile;
  progdata      : array of Byte;
  progfile      : string;
  stopRequest   : boolean;


//***************************************************************************************
// NAME:           MbiCallbackOnStarted
// PARAMETER:      length of the file that is being downloaded.
// RETURN VALUE:   none
// DESCRIPTION:    Wrapper function for safely calling an application callback
//
//***************************************************************************************
procedure MbiCallbackOnStarted(length: Longword);
begin
  if Assigned(AppOnStarted) then
  begin
    AppOnStarted(length);
  end;
end; //** end of MbiCallbackOnStarted ***


//***************************************************************************************
// NAME:           MbiCallbackOnProgress
// PARAMETER:      progress of the file download.
// RETURN VALUE:   none
// DESCRIPTION:    Wrapper function for safely calling an application callback
//
//***************************************************************************************
procedure MbiCallbackOnProgress(progress: Longword);
begin
  if Assigned(AppOnProgress) then
  begin
    AppOnProgress(progress);
  end;
end; //** end of MbiCallbackOnProgress ***


//***************************************************************************************
// NAME:           MbiCallbackOnDone
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Wrapper function for safely calling an application callback
//
//***************************************************************************************
procedure MbiCallbackOnDone;
begin
  if Assigned(AppOnDone) then
  begin
    AppOnDone;
  end;
end; //** end of MbiCallbackOnDone ***


//***************************************************************************************
// NAME:           MbiCallbackOnError
// PARAMETER:      info about the error that occured.
// RETURN VALUE:   none
// DESCRIPTION:    Wrapper function for safely calling an application callback
//
//***************************************************************************************
procedure MbiCallbackOnError(error: ShortString);
begin
  if Assigned(AppOnError) then
  begin
    AppOnError(error);
  end;
end; //** end of MbiCallbackOnError ***


//***************************************************************************************
// NAME:           MbiCallbackOnLog
// PARAMETER:      info on the log event.
// RETURN VALUE:   none
// DESCRIPTION:    Wrapper function for safely calling an application callback
//
//***************************************************************************************
procedure MbiCallbackOnLog(info:  ShortString);
begin
  if Assigned(AppOnLog) then
  begin
    AppOnLog(info);
  end;
end; //** end of MbiCallbackOnLog ***


//***************************************************************************************
// NAME:           MbiCallbackOnInfo
// PARAMETER:      details on the info event.
// RETURN VALUE:   none
// DESCRIPTION:    Wrapper function for safely calling an application callback
//
//***************************************************************************************
procedure MbiCallbackOnInfo(info:  ShortString);
begin
  if Assigned(AppOnInfo) then
  begin
    AppOnInfo(info);
  end;
end; //** end of MbiCallbackOnLog ***


//***************************************************************************************
// NAME:           LogData
// PARAMETER:      pointer to byte array and the data length
// RETURN VALUE:   none
// DESCRIPTION:    Writes the program data formatted to the logfile
//
//***************************************************************************************
procedure LogData(data : PByteArray; len : longword); stdcall;
var
  currentWriteCnt : byte;
  cnt : byte;
  logStr : string;
  bufferOffset    : longword;
begin
  bufferOffset := 0;

	while len > 0 do
	begin
  	// set the current write length optimized to log 32 bytes per line
	  currentWriteCnt := len mod 32;
 		if currentWriteCnt = 0 then currentWriteCnt := 32;
    logStr := '';

    // prepare the line to add to the log
    for cnt := 0 to currentWriteCnt-1 do
    begin
      logStr := logStr + Format('%2.2x ', [data[bufferOffset+cnt]]);
    end;

    // update the log
    MbiCallbackOnLog(ShortString(logStr));

    // update loop variables
	  len := len - currentWriteCnt;
    bufferOffset := bufferOffset + currentWriteCnt;
	end;
end; //*** end of LogData ***


//***************************************************************************************
// NAME:           OnTimeout
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Timer event handler. A timer is used in this example to simulate the
//                 progress of a file download. It also demonstrates how to use the
//                 application callbacks to keep the application informed.
//
//***************************************************************************************
procedure TEventHandlers.OnTimeout(Sender: TObject);
var
  errorInfo       : string;
  progress        : longword;
  regionCnt       : longword;
  currentWriteCnt : word;
  sessionStartResult : byte;
  bufferOffset    : longword;
  addr            : longword;
  len             : longword;
  dataSizeKB      : real;
begin
  timer.Enabled := False;

  // connect the transport layer
  MbiCallbackOnInfo('Connecting to target via USB.');
  MbiCallbackOnLog('Connecting to target via USB. t='+ShortString(TimeToStr(Time)));
  Application.ProcessMessages;
  if not loader.Connect then
  begin
    // update the user info
    MbiCallbackOnInfo('Could not connect via USB. Retrying. Reset your target if this takes a long time.');
    MbiCallbackOnLog('Transport layer connection failed. t='+ShortString(TimeToStr(Time)));
    MbiCallbackOnLog('Retrying transport layer connection. Reset your target if this takes a long time. t='+ShortString(TimeToStr(Time)));
    Application.ProcessMessages;
    // continuously try to coonect the transport layer
    while not loader.Connect do
    begin
      Application.ProcessMessages;
      Sleep(5);
      if stopRequest then
      begin
        MbiCallbackOnError('Transport layer connection cancelled by user.');
        Exit;
      end;
    end;
  end;

  //---------------- start the programming session --------------------------------------
  MbiCallbackOnLog('Starting the programming session. t='+ShortString(TimeToStr(Time)));

  // try initial connect via XCP
  if loader.StartProgrammingSession <> kProgSessionStarted then
  begin
    // update the user info
    MbiCallbackOnInfo('Could not connect. Retrying. Reset your target if this takes a long time.');
    MbiCallbackOnLog('Connect failed. Switching to backdoor entry mode. t='+ShortString(TimeToStr(Time)));
    Application.ProcessMessages;
    // continuously try to connect via XCP true the backdoor
    sessionStartResult := kProgSessionGenericError;
    while sessionStartResult <> kProgSessionStarted do
    begin
      sessionStartResult := loader.StartProgrammingSession;
      Application.ProcessMessages;
      Sleep(5);
      // don't retry if the error was caused by not being able to unprotect the programming resource
      if sessionStartResult = kProgSessionUnlockError then
      begin
        MbiCallbackOnLog('Security issue. Could not unprotect the programming resource. Check your configured XCP protection DLL. t='+ShortString(TimeToStr(Time)));
        MbiCallbackOnError('Security issue. Could not unprotect the programming resource.');
        Exit;
      end;

      // check if the user cancelled
      if stopRequest then
      begin
        // disconnect the transport layer
        MbiCallbackOnLog('Disconnecting the transport layer. t='+ShortString(TimeToStr(Time)));
        loader.Disconnect;
        MbiCallbackOnLog('Programming session cancelled by user. t='+ShortString(TimeToStr(Time)));
        MbiCallbackOnError('Programming session cancelled by user.');
        Exit;
      end;
    end;
  end;

  // still here so programming session was started
  MbiCallbackOnLog('Programming session started. t='+ShortString(TimeToStr(Time)));

  // create the datafile object
  datafile := TXcpDataFile.Create(progfile);

  // compute the size in kbytes
  dataSizeKB := datafile.GetDataCnt / 1024;

  // Call application callback when we start the actual download
  MbiCallbackOnStarted(datafile.GetDataCnt);

  // Init progress to 0 progress
  progress := 0;
  MbiCallbackOnProgress(progress);

  //---------------- next clear the memory regions --------------------------------------
  // update the user info
  MbiCallbackOnInfo('Erasing memory...');

  for regionCnt := 0 to datafile.GetRegionCnt-1 do
  begin
    // check if the user cancelled
    if stopRequest then
    begin
      // disconnect the transport layer
      MbiCallbackOnLog('Disconnecting the transport layer. t='+ShortString(TimeToStr(Time)));
      loader.Disconnect;
      MbiCallbackOnLog('Programming session cancelled by user. t='+ShortString(TimeToStr(Time)));
      MbiCallbackOnError('Programming session cancelled by user.');
      Exit;
    end;

    // obtain the region info
    datafile.GetRegionInfo(regionCnt, addr, len);

    // erase the memory
    MbiCallbackOnLog('Clearing Memory '+ShortString(Format('addr:0x%x,len:0x%x',[addr,len]))+'. t='+ShortString(TimeToStr(Time)));
    if not loader.ClearMemory(addr, len) then
    begin
      loader.GetLastError(errorInfo);
      MbiCallbackOnLog('Could not clear memory ('+ShortString(errorInfo)+'). t='+ShortString(TimeToStr(Time)));
      MbiCallbackOnError('Could not clear memory ('+ShortString(errorInfo)+').');
      datafile.Free;
      Exit;
    end;
    MbiCallbackOnLog('Memory cleared. t='+ShortString(TimeToStr(Time)));
  end;

  //---------------- next program the memory regions ------------------------------------
  for regionCnt := 0 to datafile.GetRegionCnt-1 do
  begin
    // update the user info
    MbiCallbackOnInfo('Reading file...');

    // obtain the region info
    datafile.GetRegionInfo(regionCnt, addr, len);
    // dynamically allocated buffer memory
    SetLength(progdata, len);
    // obtain the regiond data
    datafile.GetRegionData(regionCnt, progdata);

    bufferOffset := 0;
  	while len > 0 do
  	begin
      // check if the user cancelled
      if stopRequest then
      begin
        // disconnect the transport layer
        MbiCallbackOnLog('Disconnecting the transport layer. t='+ShortString(TimeToStr(Time)));
        loader.Disconnect;
        MbiCallbackOnLog('Programming session cancelled by user. t='+ShortString(TimeToStr(Time)));
        MbiCallbackOnError('Programming session cancelled by user.');
        Exit;
      end;

	  	// set the current write length taking into account kMaxProgLen
		  currentWriteCnt := len mod kMaxProgLen;
  		if currentWriteCnt = 0 then currentWriteCnt := kMaxProgLen;

      // program the data
      MbiCallbackOnLog('Programming Data '+ShortString(Format('addr:0x%x,len:0x%x',[addr,currentWriteCnt]))+'. t='+ShortString(TimeToStr(Time)));
      LogData(@progdata[bufferOffset], currentWriteCnt);

      if not loader.WriteData(addr, currentWriteCnt, @progdata[bufferOffset]) then
      begin
        loader.GetLastError(errorInfo);
        MbiCallbackOnLog('Could not program data ('+ShortString(errorInfo)+'). t='+ShortString(TimeToStr(Time)));
        MbiCallbackOnError('Could not program data ('+ShortString(errorInfo)+').');
        datafile.Free;
        Exit;
      end;
      MbiCallbackOnLog('Data Programmed. t='+ShortString(TimeToStr(Time)));

      // update progress
      progress := progress + currentWriteCnt;
      MbiCallbackOnProgress(progress);

		  // update loop variables
		  len := len - currentWriteCnt;
		  addr := addr + currentWriteCnt;
      bufferOffset := bufferOffset + currentWriteCnt;

      // update the user info
      MbiCallbackOnInfo('Programming data... ' + ShortString(Format('(%.1n of %.1n Kbytes)',[(progress/1024), dataSizeKB])));

	  end;
  end;

  //---------------- stop the programming session ---------------------------------------
  MbiCallbackOnLog('Stopping the programming session. t='+ShortString(TimeToStr(Time)));
  if not loader.StopProgrammingSession then
  begin
    loader.GetLastError(errorInfo);
    MbiCallbackOnLog('Could not stop the programming session ('+ShortString(errorInfo)+'). t='+ShortString(TimeToStr(Time)));
    MbiCallbackOnError('Could not stop the programming session ('+ShortString(errorInfo)+').');
    datafile.Free;
    Exit;
  end;
  MbiCallbackOnLog('Programming session stopped. t='+ShortString(TimeToStr(Time)));

  // all done so set progress to 100% and finish up
  progress := datafile.GetDataCnt;
  datafile.Free;
  MbiCallbackOnProgress(progress);
  MbiCallbackOnLog('File successfully downloaded t='+ShortString(TimeToStr(Time)));
  MbiCallbackOnDone;
end; //*** end of OnTimeout ***


//***************************************************************************************
// NAME:           MbiInit
// PARAMETER:      callback function pointers
// RETURN VALUE:   none
// DESCRIPTION:    Called by the application to initialize the interface library.
//
//***************************************************************************************
procedure MbiInit(cbStarted: TStartedEvent; cbProgress: TProgressEvent;
                  cbDone: TDoneEvent; cbError: TErrorEvent; cbLog: TLogEvent;
                  cbInfo: TInfoEvent); stdcall;
begin
  //--- begin of don't change ---
  AppOnStarted  := cbStarted;
  AppOnProgress := cbProgress;
  AppOnDone     := cbDone;
  AppOnLog      := cbLog;
  AppOnInfo     := cbInfo;
  AppOnError    := cbError;
  //--- end of don't change ---

  // create xcp loader object
  loader := TXcpLoader.Create;

  // update to the latest configuration
  loader.Configure(ExtractFilePath(ParamStr(0))+'openblt_usb.ini');

  // create and init a timer
  events := TEventHandlers.Create;
  timer := TTimer.Create(nil);
  timer.Enabled := False;
  timer.Interval := 100;
  timer.OnTimer  := events.OnTimeout;
end; //*** end of MbiInit ***


//***************************************************************************************
// NAME:           MbiStart
// PARAMETER:      filename of the file that is to be downloaded.
// RETURN VALUE:   none
// DESCRIPTION:    Called by the application to request the interface library to download
//                 the file that is passed as a parameter.
//
//***************************************************************************************
procedure MbiStart(fileName: ShortString); stdcall;
begin
  // update the user info
  MbiCallbackOnInfo('');

  // start the log
  MbiCallbackOnLog('--- Downloading "'+fileName+'" ---');

  // reset stop request
  stopRequest := false;

  // start the startup timer which gives microBoot a chance to paint itself
  timer.Enabled := True;

  // store the program's filename
  progfile := String(fileName);
end; //*** end of MbiStart ***


//***************************************************************************************
// NAME:           MbiStop
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Called by the application to request the interface library to stop
//                 a download that could be in progress.
//
//***************************************************************************************
procedure MbiStop; stdcall;
begin
  // set stop request
  stopRequest := true;
end; //*** end of MbiStop ***


//***************************************************************************************
// NAME:           MbiDeInit
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Called by the application to uninitialize the interface library.
//
//***************************************************************************************
procedure MbiDeInit; stdcall;
begin
  // release xcp loader object
  loader.Free;

  // release the timer and events object
  timer.Free;
  events.Free;

  //--- begin of don't change ---
  AppOnStarted  := nil;
  AppOnProgress := nil;
  AppOnDone     := nil;
  AppOnLog      := nil;
  AppOnInfo     := nil;
  AppOnError    := nil;
  //--- end of don't change ---
end; //*** end of MbiDeInit ***


//***************************************************************************************
// NAME:           MbiName
// PARAMETER:      none
// RETURN VALUE:   name of the interface library
// DESCRIPTION:    Called by the application to obtain the name of the interface library.
//
//***************************************************************************************
function MbiName : ShortString; stdcall;
begin
  Result := 'OpenBLT USB';
end; //*** end of MbiName ***


//***************************************************************************************
// NAME:           MbiDescription
// PARAMETER:      none
// RETURN VALUE:   description of the interface library
// DESCRIPTION:    Called by the application to obtain the description of the interface
//                 library.
//
//***************************************************************************************
function MbiDescription : ShortString; stdcall;
begin
  Result := 'OpenBLT using USB';
end; //*** end of MbiDescription ***


//***************************************************************************************
// NAME:           MbiVersion
// PARAMETER:      none
// RETURN VALUE:   version number
// DESCRIPTION:    Called by the application to obtain the version number of the
//                 interface library.
//
//***************************************************************************************
function MbiVersion : Longword; stdcall;
begin
  Result := 10000; // v1.00.00
end; //*** end of MbiVersion ***


//***************************************************************************************
// NAME:           MbiVInterface
// PARAMETER:      none
// RETURN VALUE:   version number of the supported interface
// DESCRIPTION:    Called by the application to obtain the version number of the
//                 Mbi interface uBootInterface.pas (not the interface library). This can
//                 be used by the application for backward compatibility.
//
//***************************************************************************************
function MbiVInterface : Longword; stdcall;
begin
  Result := 10001; // v1.00.01
end; //*** end of MbiVInterface ***


//***************************************************************************************
// NAME:           MbiConfigure
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Called by the application to enable the user to configure the inter-
//                 face library through the application.
//
//***************************************************************************************
procedure MbiConfigure; stdcall;
var
  settings : TXcpSettings;
begin
  // create xcp settings object
  settings := TXcpSettings.Create(ExtractFilePath(ParamStr(0))+'openblt_usb.ini');

  // display the modal configuration dialog
  settings.Configure;

  // release the xcp settings object
  settings.Free;

  // update to the latest configuration
  loader.Configure(ExtractFilePath(ParamStr(0))+'openblt_usb.ini');
end; //*** end of MbiConfigure ***


//***************************************************************************************
// External Declarations
//***************************************************************************************
exports
  //--- begin of don't change ---
  MbiInit,
  MbiStart,
  MbiStop,
  MbiDeInit,
  MbiName,
  MbiDescription,
  MbiVersion,
  MbiConfigure,
  MbiVInterface;
  //--- end of don't change ---
end.
//********************************** end of openblt_usb.dpr *****************************
