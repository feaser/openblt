unit FileLogger;
//***************************************************************************************
//  Description: Contains functionality for logging events to a file.
//    File Name: filelogger.pas
//
//---------------------------------------------------------------------------------------
//                          C O P Y R I G H T
//---------------------------------------------------------------------------------------
//   Copyright (c) 2018 by Feaser    http://www.feaser.com    All rights reserved
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
{$IFDEF FPC}
{$MODE objfpc}{$H+}
{$ENDIF}

interface
//***************************************************************************************
// Includes
//***************************************************************************************
uses
  Classes, SysUtils, FileUtil, EventLog, LazFileUtils;

//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  //------------------------------ TFileLoggerEntryType ---------------------------------
  TFileLoggerEntryType = ( FLET_INFO = 0,
                           FLET_ERROR );

  //------------------------------ TFileLoggerStartedEvent ------------------------------
  TFileLoggerStartedEvent = procedure(Sender: TObject) of object;

  //------------------------------ TFileLoggerStoppedEvent ------------------------------
  TFileLoggerStoppedEvent = procedure(Sender: TObject) of object;

  //------------------------------ TFileLoggerLogEvent ----------------------------------
  TFileLoggerLogEvent = procedure(Sender: TObject; LogString: String; EntryType: TFileLoggerEntryType) of object;

  //------------------------------ TFileLogger ------------------------------------------
  TFileLogger = class(TObject)
  private
    FStartedEvent: TFileLoggerStartedEvent;
    FStoppedEvent: TFileLoggerStoppedEvent;
    FLogEvent: TFileLoggerLogEvent;
    FEventLog: TEventLog;
    FStarted: Boolean;
    FLogFile: String;
  public
    constructor Create;
    destructor  Destroy; override;
    function    Start: Boolean;
    procedure   Stop;
    procedure   Log(LogString: String; EntryType: TFileLoggerEntryType = FLET_INFO);
    property    LogFile: String read FLogFile write FLogFile;
    property    Started: Boolean read FStarted;
    property    OnStarted: TFileLoggerStartedEvent read FStartedEvent write FStartedEvent;
    property    OnStopped: TFileLoggerStoppedEvent read FStoppedEvent write FStoppedEvent;
    property    OnLog: TFileLoggerLogEvent read FLogEvent write FLogEvent;
  end;


implementation
//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor
//
//***************************************************************************************
constructor TFileLogger.Create;
begin
  // Call inherited constructor.
  inherited Create;
  // Initialize fields
  FStartedEvent := nil;
  FStoppedEvent := nil;
  FLogEvent := nil;
  FEventLog := nil;
  FStarted := False;
  FLogFile := '';
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Destroy
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class destructor.
//
//***************************************************************************************
destructor TFileLogger.Destroy;
begin
  // Stop logging.
  Stop;
  // Call inherited destructor.
  inherited Destroy;
end; //*** end of Destroy ***


//***************************************************************************************
// NAME:           Start
// PARAMETER:      none
// RETURN VALUE:   True if successful, False otherwise.
// DESCRIPTION:    Starts the logger.
//
//***************************************************************************************
function TFileLogger.Start: Boolean;
var
  logDir: String;
begin
  // Initialize the result.
  Result := False;
  // Make sure logging is stopped.
  Stop;
  // Only continue if the log file was set.
  if FLogFile <> '' then
  begin
    // Extract the directory of the log file.
    logDir := ExtractFilePath(FLogFile);
    // If the directory is empty, then it means the directory of the application.
    if logDir = '' then
    begin
      // set directory to application directory.
      logDir := ProgramDirectory;
    end;
    // Double check that the directory is actually there.
    if not DirectoryExists(logDir) then
    begin
      // Force the directory creation.
      ForceDirectories(logDir);
    end;
    // Only attempt to start logging if the directory is there and is writable.
    if DirectoryExists(logDir) and DirectoryIsWritable(logDir) then
    begin
      // Create, configure and start an eventlog instance.
      FEventLog := TEventLog.Create(nil);
      FEventLog.LogType := ltFile;
      FEventLog.FileName := FLogFile;
      FEventLog.Active := True;
      // Update state.
      FStarted := True;
      // Update the result.
      Result := True;
      // Trigger the event if it is set.
      if Assigned(FStartedEvent) then
      begin
        FStartedEvent(Self);
      end;
    end;
  end;
end; //*** end of Start ***


//***************************************************************************************
// NAME:           Stop
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Stops the logger.
//
//***************************************************************************************
procedure TFileLogger.Stop;
begin
  // Check if the eventlog was instanced.
  if Assigned(FEventLog) then
  begin
    // Deactivate logging.
    FEventLog.Active := False;
    // Update state.
    FStarted := False;
    // Release the instance.
    FreeAndNil(FEventLog);
  end;
  // Trigger the event if it is set.
  if Assigned(FStoppedEvent) then
  begin
    FStoppedEvent(Self);
  end;
end; //*** end of Stop ***


//***************************************************************************************
// NAME:           Log
// PARAMETER:      LogString The string to log.
//                 EntryType The type of the log entry.
// RETURN VALUE:   none
// DESCRIPTION:    Logs a string.
//
//***************************************************************************************
procedure TFileLogger.Log(LogString: String; EntryType: TFileLoggerEntryType);
begin
  // Only log if the event log is instanced and logging was started.
  if Assigned(FEventLog) then
  begin
    if FStarted then
    begin
      // Enter the log message in the requested format.
      if EntryType = FLET_INFO then
      begin
        FEventLog.Info(LogString);
      end
      else if EntryType = FLET_ERROR then
      begin
        FEventLog.Error(LogString);
      end;
    end;
  end;
  // Trigger the event if it is set.
  if Assigned(FLogEvent) then
  begin
    FLogEvent(Self, LogString, EntryType);
  end;
end; //*** end of Log ***


end.
//******************************** end of filelogger.pas ********************************

