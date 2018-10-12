unit FirmwareUpdate;
//***************************************************************************************
//  Description: Contains the classes for handling firmwware updates through LibOpenBLT.
//    File Name: firmwareupdate.pas
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
  {$IFDEF UNIX}{$IFDEF UseCThreads}
    cthreads,
    cmem, // the c memory manager is on some systems much faster for multi-threading
  {$ENDIF}{$ENDIF}
  Classes, SysUtils, CurrentConfig, ConfigGroups, OpenBlt;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  // Forward declarations
  TFirmwareUpdate = class;

  //------------------------------ TFirmwareUpdateStartedEvent --------------------------
  TFirmwareUpdateStartedEvent = procedure(Sender: TObject) of object;

  //------------------------------ TFirmwareUpdateStoppedEvent --------------------------
  TFirmwareUpdateStoppedEvent = procedure(Sender: TObject) of object;

  //------------------------------ TFirmwareUpdateDoneEvent -----------------------------
  TFirmwareUpdateDoneEvent = procedure(Sender: TObject) of object;

  //------------------------------ TFirmwareUpdateInfoEvent -----------------------------
  TFirmwareUpdateInfoEvent = procedure(Sender: TObject; InfoString: String) of object;

  //------------------------------ TFirmwareUpdateLogEvent ------------------------------
  TFirmwareUpdateLogEvent = procedure(Sender: TObject; LogString: String) of object;

  //------------------------------ TFirmwareUpdateProgressEvent -------------------------
  TFirmwareUpdateProgressEvent = procedure(Sender: TObject; Percentage: Integer) of object;

  //------------------------------ TFirmwareUpdateErrorEvent ----------------------------
  TFirmwareUpdateErrorEvent = procedure(Sender: TObject; ErrorString: String) of object;

  //------------------------------ TFirmwareUpdateState ---------------------------------
  TFirmwareUpdateState = ( FUS_IDLE = 0,
                           FUS_INITIALIZING,
                           FUS_CONNECTING,
                           FUS_LOADING_FIRMWARE,
                           FUS_ERASING_MEMORY,
                           FUS_PROGRAMMING_MEMORY,
                           FUS_FINISHING_UP );

  //------------------------------ TFirmwareUpdateThread --------------------------------
  TFirmwareUpdateThread = class(TThread)
    private
      FFirmwareUpdate: TFirmwareUpdate;
      FFirmwareFile: String;
      FState: TFirmwareUpdateState;
      FInfoString: String;
      FLogString: String;
      FErrorString: String;
      FPercentage: Integer;
      procedure Initialize;
      procedure Cleanup;
      function GetSessionProtocolName: String;
      procedure LogSessionProtocolSettings;
      function GetTransportLayerName: String;
      procedure LogTransportLayerSettings;
      procedure SynchronizeStartedEvent;
      procedure SynchronizeStoppedEvent;
      procedure SynchronizeDoneEvent;
      procedure SynchronizeInfoEvent;
      procedure SynchronizeLogEvent;
      procedure SynchronizeProgressEvent;
      procedure SynchronizeErrorEvent;
    protected
      procedure Execute; override;
    public
      constructor Create(CreateSuspended : Boolean; FirmwareUpdate: TFirmwareUpdate); reintroduce;
      property FirmwareFile: String read FFirmwareFile write FFirmwareFile;
      property State: TFirmwareUpdateState read FState write FState;
    end;

  //------------------------------ TFirmwareUpdate --------------------------------------
  TFirmwareUpdate = class (TObject)
  private
    FCurrentConfig: TCurrentConfig;
    FWorkerThread: TFirmwareUpdateThread;
    FStartedEvent: TFirmwareUpdateStartedEvent;
    FStoppedEvent: TFirmwareUpdateStoppedEvent;
    FDoneEvent: TFirmwareUpdateDoneEvent;
    FInfoEvent: TFirmwareUpdateInfoEvent;
    FLogEvent: TFirmwareUpdateLogEvent;
    FProgressEvent: TFirmwareUpdateProgressEvent;
    FErrorEvent: TFirmwareUpdateErrorEvent;
  public
    constructor Create(CurrentConfig: TCurrentConfig); reintroduce;
    destructor  Destroy; override;
    function Start(FirmwareFile: String): Boolean;
    procedure Stop;
    property OnStarted: TFirmwareUpdateStartedEvent read FStartedEvent write FStartedEvent;
    property OnStopped: TFirmwareUpdateStoppedEvent read FStoppedEvent write FStoppedEvent;
    property OnDone: TFirmwareUpdateDoneEvent read FDoneEvent write FDoneEvent;
    property OnInfo: TFirmwareUpdateInfoEvent read FInfoEvent write FInfoEvent;
    property OnLog: TFirmwareUpdateLogEvent read FLogEvent write FLogEvent;
    property OnProgress: TFirmwareUpdateProgressEvent read FProgressEvent write FProgressEvent;
    property OnError: TFirmwareUpdateErrorEvent read FErrorEvent write FErrorEvent;
  end;


implementation
//---------------------------------------------------------------------------------------
//-------------------------------- TFirmwareUpdate --------------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           Create
// PARAMETER:      CurrentConfig Current configuration instance.
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor.
//
//***************************************************************************************
constructor TFirmwareUpdate.Create(CurrentConfig: TCurrentConfig);
begin
  // Call the inherited constructor.
  inherited Create;
  // Check parameters.
  Assert(CurrentConfig <> nil, 'Current configuration instance cannot be null');
  // Store the configuration instance.
  FCurrentConfig := CurrentConfig;
  // Initialize fields.
  FStartedEvent := nil;
  FStoppedEvent := nil;
  FDoneEvent := nil;
  FInfoEvent := nil;
  FLogEvent := nil;
  FProgressEvent := nil;
  FErrorEvent := nil;
  FWorkerThread := nil;
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Destroy
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class destructor.
//
//***************************************************************************************
destructor TFirmwareUpdate.Destroy;
begin
  // Check if the worker thread is instanced.
  if Assigned(FWorkerThread) then
  begin
    // Set termination request for the worker thread.
    FWorkerThread.Terminate;
    // Wait for thread termination to complete.
    FWorkerThread.WaitFor;
    // Release the thread instance.
    FWorkerThread.Free;
  end;
  // call inherited destructor
  inherited Destroy;
end; //*** end of Destroy ***


//***************************************************************************************
// NAME:           Start
// PARAMETER:      FirmwareFile Filename and path of the firmware file with program data
//                 that is to be programmed on the target using the bootloader.
// RETURN VALUE:   True if successful, False otherwise.
// DESCRIPTION:    Starts the firmware update procedure.
//
//***************************************************************************************
function TFirmwareUpdate.Start(FirmwareFile: String): Boolean;
begin
  // Initialize the result.
  Result := False;
  // Check if the worker thread is terminated but not yet freed from a previous update.
  if Assigned(FWorkerThread) then
  begin
    if FWorkerThread.Finished then
    begin
      // Free it.
      FreeAndNil(FWorkerThread);
    end;
  end;
  // Only start a firmware update if another one is not already in progress.
  if not Assigned(FWorkerThread) then
  begin
    // Only start the firmware update if the specified file exists.
    if FileExists(FirmwareFile) then
    begin
      // Create the worker thread in a suspended state.
      FWorkerThread := TFirmwareUpdateThread.Create(True, Self);
      // Only continue if the worker thread could be instanced.
      if Assigned(FWorkerThread) then
      begin
        // Pass the firmware file on to the worker thread.
        FWorkerThread.FirmwareFile := FirmwareFile;
        // Set the initial state for the worker thread so it knows where to start.
        FWorkerThread.State := FUS_INITIALIZING;
        // Start the worker thread, which handles the actual firmware update.
        FWorkerThread.Start;
        // Update the result.
        Result := True;
      end;
    end;
  end;
end; //*** end of Start ***


//***************************************************************************************
// NAME:           Stop
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Cancel an active firmware update procedure, if any.
//
//***************************************************************************************
procedure TFirmwareUpdate.Stop;
begin
  // No need to stop the worker thread if it is not instanced.
  if Assigned(FWorkerThread) then
  begin
    // Set worker thread state to idle.
    FWorkerThread.State := FUS_IDLE;
    // Set termination request for the worker thread.
    FWorkerThread.Terminate;
    // Wait for thread termination to complete.
    FWorkerThread.WaitFor;
    // Release the thread instance.
    FreeAndNil(FWorkerThread);
  end;
end;  //*** end of Stop ***


//---------------------------------------------------------------------------------------
//-------------------------------- TFirmwareUpdateThread --------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           Create
// PARAMETER:      CreateSuspended True to suspend the thread after creation.
//                 FirmwareUpdate Instance of the TFirmwareUpdate class, needed to
//                 trigger its events.
// RETURN VALUE:   none
// DESCRIPTION:    Thread constructor.
//
//***************************************************************************************
constructor TFirmwareUpdateThread.Create(CreateSuspended : Boolean; FirmwareUpdate: TFirmwareUpdate);
begin
  // Call inherited constructor.
  inherited Create(CreateSuspended);
  // Configure the thread to not automatically free itself upon termination.
  FreeOnTerminate := False;
  // Initialize fields.
  FFirmwareUpdate := FirmwareUpdate;
  FFirmwareFile := '';
  FState := FUS_IDLE;
  FInfoString := '';
  FLogString := '';
  FErrorString := '';
  FPercentage := 0;
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Execute
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Thread execution function.
//
//***************************************************************************************
procedure TFirmwareUpdateThread.Execute;
const
  ERASE_SIZE_MAX = 32768;
  PROGRAM_SIZE_MAX = 256;
var
  initialized: Boolean;
  errorDetected: Boolean;
  firmwareDataTotalSize: LongWord;
  firmwareDataTotalSegments: LongWord;
  firmwareDataBaseAddress: LongWord;
  segmentIdx: LongWord;
  segmentLen: LongWord;
  segmentBase: LongWord;
  segmentData: PByte;
  eraseCurrentLen: LongWord;
  eraseCurrentBase: LongWord;
  eraseStillLeft: LongWord;
  eraseProgressPct: Integer;
  eraseProgressLen: LongWord;
  programCurrentLen: LongWord;
  programCurrentBase: LongWord;
  programCurrentDataPtr: PByte;
  programStillLeft: LongWord;
  programProgressPct: Integer;
  programProgressLen: LongWord;
begin
  // Initialize locals.
  initialized := False;
  // Trigger the started event.
  Synchronize(@SynchronizeStartedEvent);
  // Enter thread's execution loop.
  while not Terminated do
  begin
    // --------------------------- Initializing -----------------------------------------
    if FState = FUS_INITIALIZING then
    begin
      // Initialize error flag.
      errorDetected := False;
      // Update the info.
      FInfoString := 'Starting firmware update';
      Synchronize(@SynchronizeInfoEvent);
      // Update the log.
      FLogString := FInfoString;
      Synchronize(@SynchronizeLogEvent);
      FLogString := 'Specified firmware file: ' + FFirmwareFile;
      Synchronize(@SynchronizeLogEvent);
      FLogString := 'Using LibOpenBLT version ' + BltVersionGetString;
      Synchronize(@SynchronizeLogEvent);
      FLogString := 'Detected session protocol: ' + GetSessionProtocolName;
      Synchronize(@SynchronizeLogEvent);
      FLogString := 'Using session protocol settings:';
      Synchronize(@SynchronizeLogEvent);
      LogSessionProtocolSettings;
      FLogString := 'Detected transport layer: ' + GetTransportLayerName;
      Synchronize(@SynchronizeLogEvent);
      FLogString := 'Using transport layer settings:';
      Synchronize(@SynchronizeLogEvent);
      LogTransportLayerSettings;
      FLogString := 'Initializing firmware update engine';
      Synchronize(@SynchronizeLogEvent);
      // Initialize LibOpenBLT modules.
      Initialize;
      initialized := True;
      // Transition to the next state if all is okay.
      if not errorDetected then
      begin
        FState := FUS_LOADING_FIRMWARE;
      end;
    end
    // --------------------------- Loading firmware data --------------------------------
    else if FState = FUS_LOADING_FIRMWARE then
    begin
      // Initialize error flag.
      errorDetected := False;
      // Update the info.
      FInfoString := 'Loading firmware data from file';
      Synchronize(@SynchronizeInfoEvent);
      // Update the log.
      FLogString := FInfoString;
      Synchronize(@SynchronizeLogEvent);
      // Load firmware data from the file.
      if BltFirmwareLoadFromFile(PAnsiChar(AnsiString(FFirmwareFile)), 0) <> BLT_RESULT_OK then
      begin
        // Set error flag.
        errorDetected := True;
        // Cancel firmware update procedure by transitioning to the idle state.
        FState := FUS_IDLE;
        // Update the log.
        FLogString := 'Error occured while loading firmware data from the file';
        Synchronize(@SynchronizeLogEvent);
        // Trigger error.
        FErrorString := FLogString;
        Synchronize(@SynchronizeErrorEvent);
      end
      // Display information regarding the loaded firmware data.
      else
      begin
        // Store the number of segments.
        firmwareDataTotalSegments := BltFirmwareGetSegmentCount();
        // Initialize locals.
        firmwareDataTotalSize := 0;
        segmentBase := 0;
        segmentLen := 0;
        // Loop through all segments.
        for segmentIdx := 0 to (firmwareDataTotalSegments - 1) do
        begin
          // Extract segment info.
          segmentData := BltFirmwareGetSegment(segmentIdx, segmentBase, segmentLen);
          // Validate the segment info
          if (segmentData = nil) or (segmentLen = 0) then
          begin
            // Set error flag.
            errorDetected := True;
            // Cancel firmware update procedure by transitioning to the idle state.
            FState := FUS_IDLE;
            // Update the log.
            FLogString := 'Invalid segment encountered in the firmware data';
            Synchronize(@SynchronizeLogEvent);
            // Trigger error.
            FErrorString := FLogString;
            Synchronize(@SynchronizeErrorEvent);
            // No need to continue looping through segments.
            Break;
          end
          // Segment is valid.
          else
          begin
            // Update total size.
            firmwareDataTotalSize := firmwareDataTotalSize + segmentLen;
            // If it is the first segment, then store the base address.
            if segmentIdx = 0 then
            begin
              firmwareDataBaseAddress := segmentBase;
            end;
          end;
        end;
        // Sanity check to make sure there was actually firmware data present.
        if not errorDetected then
        begin
          if firmwareDataTotalSize = 0 then
          begin
            // Set error flag.
            errorDetected := True;
            // Cancel firmware update procedure by transitioning to the idle state.
            FState := FUS_IDLE;
            // Update the log.
            FLogString := 'Firmware data is empty. Cannot continue with firmware update';
            Synchronize(@SynchronizeLogEvent);
            // Trigger error.
            FErrorString := FLogString;
            Synchronize(@SynchronizeErrorEvent);
          end;
        end;
      end;
      // Display information about the loaded firmware data
      if not errorDetected then
      begin
        // Update the log.
        FLogString := '  -> Number of segments: ' + IntToStr(firmwareDataTotalSegments);
        Synchronize(@SynchronizeLogEvent);
        FLogString := '  -> Base memory address: ' + Format('%.8xh', [firmwareDataBaseAddress]);
        Synchronize(@SynchronizeLogEvent);
        FLogString := '  -> Total data size: ' + IntToStr(firmwareDataTotalSize);
        Synchronize(@SynchronizeLogEvent);
      end;
      // Transition to the next state if all is okay.
      if not errorDetected then
      begin
        FState := FUS_CONNECTING;
      end;
    end
    // --------------------------- Connecting to target ---------------------------------
    else if FState = FUS_CONNECTING then
    begin
      // Initialize error flag.
      errorDetected := False;
      // Update the info.
      FInfoString := 'Connecting to the target';
      Synchronize(@SynchronizeInfoEvent);
      // Update the log.
      FLogString := FInfoString;
      Synchronize(@SynchronizeLogEvent);
      // Attempt connection with the target.
      if BltSessionStart() <> BLT_RESULT_OK then
      begin
        // Not yet successful. Request the user to reset the system if it takes too long.
        FInfoString := 'Connecting to the target (reset your target if this takes a long time)';
        Synchronize(@SynchronizeInfoEvent);
        // Update the log.
        FLogString := 'First connection attempt failed';
        Synchronize(@SynchronizeLogEvent);
        FLogString := 'Switching to backdoor entry mode';
        Synchronize(@SynchronizeLogEvent);
        // Now keep retrying until successful
        while BltSessionStart() <> BLT_RESULT_OK do
        begin
          // Check for thread termination request
          if Terminated then
          begin
            // Set error flag to force idle mode after breaking this loop.
            errorDetected := True;
            // Update the log.
            FLogString := 'Cancellation request detected, so stopping firmware update';
            Synchronize(@SynchronizeLogEvent);
            // Trigger the stopped event.
            Synchronize(@SynchronizeStoppedEvent);
            // Cancel firmware update procedure by transitioning to the idle state.
            FState := FUS_IDLE;
            // Stop looping.
            Break;
          end;
          // Delay a bit to not starve the CPU.
          Sleep(20);
        end;
      end;
      // Transition to the next state if all is okay.
      if not errorDetected then
      begin
        FState := FUS_ERASING_MEMORY;
      end;
    end
    // --------------------------- Erasing memory ---------------------------------------
    else if FState = FUS_ERASING_MEMORY then
    begin
      // Initialize error flag.
      errorDetected := False;
      // Reset progress variables
      eraseProgressPct := 0;
      eraseProgressLen := 0;
      // Loop through all segments.
      for segmentIdx := 0 to (firmwareDataTotalSegments - 1) do
      begin
        // Don't bother looping if an error was detected.
        if errorDetected then
        begin
          Break;
        end;
        // Extract segment info.
        eraseCurrentBase := 0;
        eraseStillLeft := 0;
        segmentData := BltFirmwareGetSegment(segmentIdx, eraseCurrentBase, eraseStillLeft);
        // Perform erase in chunks of maximum ERASE_SIZE_MAX. Otherwise the erase
        // operation can take a long time, which would lead to a non-responsive user
        // interface.
        while eraseStillLeft > 0 do
        begin
          // Check for cancellation request.
          if Terminated then
          begin
            // Set error flag to force idle mode after breaking this loop.
            errorDetected := True;
            // Update the log.
            FLogString := 'Cancellation request detected, so stopping firmware update';
            Synchronize(@SynchronizeLogEvent);
            // Trigger the stopped event.
            Synchronize(@SynchronizeStoppedEvent);
            // Cancel firmware update procedure by transitioning to the idle state.
            FState := FUS_IDLE;
            // Stop looping.
            Break;
          end;
          // Determine chunk size.
          eraseCurrentLen := ERASE_SIZE_MAX;
          if eraseCurrentLen > eraseStillLeft then
          begin
            eraseCurrentLen := eraseStillLeft;
          end;
          // Update the info.
          FInfoString := Format('Erasing %u bytes starting at %.8xh', [eraseCurrentLen, eraseCurrentBase]);
          Synchronize(@SynchronizeInfoEvent);
          // Update the log.
          FLogString := FInfoString;
          Synchronize(@SynchronizeLogEvent);
          // Perform the erase operation.
          if BltSessionClearMemory(eraseCurrentBase, eraseCurrentLen) <> BLT_RESULT_OK then
          begin
            // Set error flag.
            errorDetected := True;
            // Cancel firmware update procedure by transitioning to the idle state.
            FState := FUS_IDLE;
            // Update the log.
            FLogString := Format('Could not erase memory at %.8xh', [eraseCurrentBase]);
            Synchronize(@SynchronizeLogEvent);
            // Trigger error.
            FErrorString := FLogString;
            Synchronize(@SynchronizeErrorEvent);
            // Stop looping
            Break;
          end
          // Erase operation was successful. Update loop variables for the next chunk.
          else
          begin
            eraseStillLeft := eraseStillLeft - eraseCurrentLen;
            eraseCurrentBase := eraseCurrentBase + eraseCurrentLen;
            // Update erase progress
            eraseProgressLen := eraseProgressLen + eraseCurrentLen;
            eraseProgressPct := (Int64(eraseProgressLen) * 100) div firmwareDataTotalSize;
            // Dedicate the first 20% of the total firmware update progress to the
            // erase operation.
            FPercentage := (eraseProgressPct * 20) div 100;
            Synchronize(@SynchronizeProgressEvent);
          end;
        end;
      end;
      // Transition to the next state if all is okay.
      if not errorDetected then
      begin
        FState := FUS_PROGRAMMING_MEMORY;
      end;
    end
    // --------------------------- Programming memory -----------------------------------
    else if FState = FUS_PROGRAMMING_MEMORY then
    begin
      // Initialize error flag.
      errorDetected := False;
      // Reset progress variables
      programProgressPct := 0;
      programProgressLen := 0;
      // Loop through all segments.
      for segmentIdx := 0 to (firmwareDataTotalSegments - 1) do
      begin
        // Don't bother looping if an error was detected.
        if errorDetected then
        begin
          Break;
        end;
        // Extract segment info.
        programCurrentBase := 0;
        programStillLeft := 0;
        programCurrentDataPtr := BltFirmwareGetSegment(segmentIdx, programCurrentBase, programStillLeft);
        // Perform programming in chunks of maximum PROGRAM_SIZE_MAX. Otherwise the
        // programming operation can take a long time, which would lead to a non-
        // responsive user interface.
        while programStillLeft > 0 do
        begin
          // Check for cancellation request.
          if Terminated then
          begin
            // Set error flag to force idle mode after breaking this loop.
            errorDetected := True;
            // Update the log.
            FLogString := 'Cancellation request detected, so stopping firmware update';
            Synchronize(@SynchronizeLogEvent);
            // Trigger the stopped event.
            Synchronize(@SynchronizeStoppedEvent);
            // Cancel firmware update procedure by transitioning to the idle state.
            FState := FUS_IDLE;
            // Stop looping.
            Break;
          end;
          // Determine chunk size.
          programCurrentLen := PROGRAM_SIZE_MAX;
          if programCurrentLen > programStillLeft then
          begin
            programCurrentLen := programStillLeft;
          end;
          // Update the info.
          FInfoString := Format('Programming %u bytes starting at %.8xh', [programCurrentLen, programCurrentBase]);
          Synchronize(@SynchronizeInfoEvent);
          // Update the log.
          FLogString := FInfoString;
          Synchronize(@SynchronizeLogEvent);
          // Perform the programming operation.
          if BltSessionWriteData(programCurrentBase, programCurrentLen, programCurrentDataPtr) <> BLT_RESULT_OK then
          begin
            // Set error flag.
            errorDetected := True;
            // Cancel firmware update procedure by transitioning to the idle state.
            FState := FUS_IDLE;
            // Update the log.
            FLogString := Format('Could not program memory at %.8xh', [programCurrentBase]);
            Synchronize(@SynchronizeLogEvent);
            // Trigger error.
            FErrorString := FLogString;
            Synchronize(@SynchronizeErrorEvent);
            // Stop looping
            Break;
          end
          // Program operation was successful. Update loop variables for the next chunk.
          else
          begin
            programStillLeft := programStillLeft - programCurrentLen;
            programCurrentBase := programCurrentBase + programCurrentLen;
            programCurrentDataPtr := programCurrentDataPtr + programCurrentLen;
            // Update programming progress
            programProgressLen := programProgressLen + programCurrentLen;
            programProgressPct := (Int64(programProgressLen) * 100) div firmwareDataTotalSize;
            // Dedicate the remaining 80% of the total firmware update progress to the
            // programing operation.
            FPercentage := 20 + ((programProgressPct * 80) div 100);
            Synchronize(@SynchronizeProgressEvent);
          end;
        end;
      end;
      // Transition to the next state if all is okay.
      if not errorDetected then
      begin
        FState := FUS_FINISHING_UP;
      end;
    end
    // --------------------------- Finishing up -----------------------------------------
    else if FState = FUS_FINISHING_UP then
    begin
      // Initialize error flag.
      errorDetected := False;
      // Update the info.
      FInfoString := 'Finishing programming session';
      Synchronize(@SynchronizeInfoEvent);
      // Update the log.
      FLogString := FInfoString;
      Synchronize(@SynchronizeLogEvent);
      // Stop the session
      BltSessionStop();
      // Update the info.
      FInfoString := 'Firmware update completed successfully';
      Synchronize(@SynchronizeInfoEvent);
      // Update the log.
      FLogString := FInfoString;
      Synchronize(@SynchronizeLogEvent);
      // Trigger the OnDone event
      Synchronize(@SynchronizeDoneEvent);
      // Transition back to the idle state.
      FState := FUS_IDLE;
    end
    // --------------------------- Idle -------------------------------------------------
    else
    begin
      // Idle mode means that the worker thread is all done and can be exited.
      Break;
    end;
  end;
  // Cleanup LibOpenBLT modules if initialized.
  if initialized then
  begin
    FLogString := 'Cleaning up firmware update engine';
    Synchronize(@SynchronizeLogEvent);
    initialized := False;
    Cleanup;
  end;
end; //*** end of Execute ***


//***************************************************************************************
// NAME:           Initialize
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Initializes the firmware update process.
//
//***************************************************************************************
procedure TFirmwareUpdateThread.Initialize;
var
  sessionConfig: TSessionConfig;
  sessionXcpConfig: TSessionXcpConfig;
  transportConfig: TTransportConfig;
  transportXcpRs232Config: TTransportXcpRs232Config;
  transportXcpCanConfig: TTransportXcpCanConfig;
  transportXcpTcpIpConfig: TTransportXcpTcpIpConfig;
  sessionType: LongWord;
  transportType: LongWord;
  sessionSettingsXcp: tBltSessionSettingsXcpV10;
  transportSettingsXcpRs232: tBltTransportSettingsXcpV10Rs232;
  transportSettingsXcpCan: tBltTransportSettingsXcpV10Can;
  transportSettingsXcpNet: tBltTransportSettingsXcpV10Net;
  sessionSettingsPtr: Pointer;
  transportSettingsPtr: Pointer;
begin
  // Initialize locals.
  sessionSettingsPtr := nil;
  transportSettingsPtr := nil;
  // Initialize the firmware data module using the S-record parser.
  BltFirmwareInit(BLT_FIRMWARE_PARSER_SRECORD);
  // Determine the session protocol to use and set its settings.
  sessionConfig := FFirmwareUpdate.FCurrentConfig.Groups[TSessionConfig.GROUP_NAME]
                   as TSessionConfig;
  // ------------------------------------ XCP version 1.0 -------------------------------
  if sessionConfig.Session = 'xcp' then
  begin
    // Store the session protocol type.
    sessionType := BLT_SESSION_XCP_V10;
    // Obtain access to the related configuration group.
    sessionXcpConfig := FFirmwareUpdate.FCurrentConfig.Groups[TSessionXcpConfig.GROUP_NAME]
                        as TSessionXcpConfig;
    // Copy over the settings.
    sessionSettingsXcp.timeoutT1 := sessionXcpConfig.TimeoutT1;
    sessionSettingsXcp.timeoutT3 := sessionXcpConfig.TimeoutT3;
    sessionSettingsXcp.timeoutT4 := sessionXcpConfig.TimeoutT4;
    sessionSettingsXcp.timeoutT5 := sessionXcpConfig.TimeoutT5;
    sessionSettingsXcp.timeoutT6 := sessionXcpConfig.TimeoutT6;
    sessionSettingsXcp.timeoutT7 := sessionXcpConfig.TimeoutT7;
    sessionSettingsXcp.connectMode := sessionXcpConfig.ConnectMode;
    sessionSettingsXcp.seedKeyFile := PAnsiChar(AnsiString(sessionXcpConfig.SeedKey));
    // Point the session settings pointer to this one.
    sessionSettingsPtr := @sessionSettingsXcp;
    // Determine the transport layer and its settings.
    transportConfig := FFirmwareUpdate.FCurrentConfig.Groups[TTransportConfig.GROUP_NAME]
                       as TTransportConfig;
    // ---------------------------------- XCP on RS232 ----------------------------------
    if transportConfig.Transport = 'xcp_rs232' then
    begin
      // Store the transport layer type.
      transportType := BLT_TRANSPORT_XCP_V10_RS232;
      // Obtain access to the related configuration group.
      transportXcpRs232Config := FFirmwareUpdate.FCurrentConfig.Groups[TTransportXcpRs232Config.GROUP_NAME]
                                 as TTransportXcpRs232Config;
      // Copy over the settings.
      transportSettingsXcpRs232.portName := PAnsiChar(AnsiString(transportXcpRs232Config.Device));
      transportSettingsXcpRs232.baudrate := transportXcpRs232Config.Baudrate;
      // Point the transport settings pointer to this one.
      transportSettingsPtr := @transportSettingsXcpRs232;
    end
    // ---------------------------------- XCP on CAN ------------------------------------
    else if transportConfig.Transport = 'xcp_can' then
    begin
      // Store the transport layer type.
      transportType := BLT_TRANSPORT_XCP_V10_CAN;
      // Obtain access to the related configuration group.
      transportXcpCanConfig := FFirmwareUpdate.FCurrentConfig.Groups[TTransportXcpCanConfig.GROUP_NAME]
                               as TTransportXcpCanConfig;
      // Copy over the settings.
      transportSettingsXcpCan.deviceName := PAnsiChar(AnsiString(transportXcpCanConfig.Device));
      transportSettingsXcpCan.deviceChannel := transportXcpCanConfig.Channel;
      transportSettingsXcpCan.baudrate := transportXcpCanConfig.Baudrate;
      transportSettingsXcpCan.transmitId := transportXcpCanConfig.TransmitId;
      transportSettingsXcpCan.receiveId := transportXcpCanConfig.ReceiveId;
      transportSettingsXcpCan.useExtended := transportXcpCanConfig.ExtendedId;
      // Point the transport settings pointer to this one.
      transportSettingsPtr := @transportSettingsXcpCan;
    end
    // ---------------------------------- XCP on USB ------------------------------------
    else if transportConfig.Transport = 'xcp_usb' then
    begin
      // Store the transport layer type.
      transportType := BLT_TRANSPORT_XCP_V10_USB;
      // No settings to copy over for USB.
    end
    // ---------------------------------- XCP on TCP/IP ---------------------------------
    else if transportConfig.Transport = 'xcp_net' then
    begin
      // Store the transport layer type.
      transportType := BLT_TRANSPORT_XCP_V10_NET;
      // Obtain access to the related configuration group.
      transportXcpTcpIpConfig := FFirmwareUpdate.FCurrentConfig.Groups[TTransportXcpTcpIpConfig.GROUP_NAME]
                                 as TTransportXcpTcpIpConfig;
      // Copy over the settings.
      transportSettingsXcpNet.address := PAnsiChar(AnsiString(transportXcpTcpIpConfig.Address));
      transportSettingsXcpNet.port := transportXcpTcpIpConfig.Port;
      // Point the transport settings pointer to this one.
      transportSettingsPtr := @transportSettingsXcpNet;
    end;
  end;
  // Initialize the session module using the detected settings.
  BltSessionInit(sessionType, sessionSettingsPtr, transportType, transportSettingsPtr);
end; //*** end of Initialize ***


//***************************************************************************************
// NAME:           Cleanup
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Cleans up the firmware update process.
//
//***************************************************************************************
procedure TFirmwareUpdateThread.Cleanup;
begin
  // Terminate the session.
  BltSessionTerminate();
  // Terminate the firmware data module.
  BltFirmwareTerminate();
end; //*** end of Cleanup ***


//***************************************************************************************
// NAME:           GetSessionProtocolName
// PARAMETER:      none
// RETURN VALUE:   Name of the configured session protocol.
// DESCRIPTION:    Obtains the name of the session protocol that will be used for the
//                 firmware update.
//
//***************************************************************************************
function TFirmwareUpdateThread.GetSessionProtocolName: String;
var
  sessionConfig: TSessionConfig;
begin
  // Initialize the result.
  Result := 'Unknown session protocol';
  // Obtain access to the related configuration group.
  sessionConfig := FFirmwareUpdate.FCurrentConfig.Groups[TSessionConfig.GROUP_NAME]
                   as TSessionConfig;
  // Filter on the configured session protocol.
  if sessionConfig.Session = 'xcp' then
  begin
    Result := 'XCP version 1.0';
  end;
end; //*** end of GetSessionProtocolName ***


//***************************************************************************************
// NAME:           LogSessionProtocolSettings
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Logs the settings of the session protocol that will be used for the
//                 firmware update.
//
//***************************************************************************************
procedure TFirmwareUpdateThread.LogSessionProtocolSettings;
var
  sessionConfig: TSessionConfig;
  sessionXcpConfig: TSessionXcpConfig;
begin
  // Obtain access to the related configuration group.
  sessionConfig := FFirmwareUpdate.FCurrentConfig.Groups[TSessionConfig.GROUP_NAME]
                   as TSessionConfig;
  // Filter on the configured session protocol.
  if sessionConfig.Session = 'xcp' then
  begin
    // Obtain access to the related configuration group.
    sessionXcpConfig := FFirmwareUpdate.FCurrentConfig.Groups[TSessionXcpConfig.GROUP_NAME]
                        as TSessionXcpConfig;
    FLogString := '  -> Timeout T1: ' + IntToStr(sessionXcpConfig.TimeoutT1) + ' ms';
    Synchronize(@SynchronizeLogEvent);
    FLogString := '  -> Timeout T3: ' + IntToStr(sessionXcpConfig.TimeoutT3) + ' ms';
    Synchronize(@SynchronizeLogEvent);
    FLogString := '  -> Timeout T4: ' + IntToStr(sessionXcpConfig.TimeoutT4) + ' ms';
    Synchronize(@SynchronizeLogEvent);
    FLogString := '  -> Timeout T5: ' + IntToStr(sessionXcpConfig.TimeoutT5) + ' ms';
    Synchronize(@SynchronizeLogEvent);
    FLogString := '  -> Timeout T6: ' + IntToStr(sessionXcpConfig.TimeoutT6) + ' ms';
    Synchronize(@SynchronizeLogEvent);
    FLogString := '  -> Timeout T7: ' + IntToStr(sessionXcpConfig.TimeoutT7) + ' ms';
    Synchronize(@SynchronizeLogEvent);
    if sessionXcpConfig.SeedKey <> '' then
      FLogString := '  -> Seed/Key file: ' + sessionXcpConfig.SeedKey
    else
      FLogString := '  -> Seed/Key file: ' + 'None';
    Synchronize(@SynchronizeLogEvent);
    FLogString := '  -> Connection mode: ' + IntToStr(sessionXcpConfig.ConnectMode);
    Synchronize(@SynchronizeLogEvent);
  end
  else
  begin
    FLogString := '  -> Unknown session protocol settings';
    Synchronize(@SynchronizeLogEvent);
  end;
end; //*** end of LogSessionProtocolSettings ***


//***************************************************************************************
// NAME:           GetTransportLayerName
// PARAMETER:      none
// RETURN VALUE:   Name of the configured transport layer.
// DESCRIPTION:    Obtains the name of the tansport layer that will be used for the
//                 firmware update.
//
//***************************************************************************************
function TFirmwareUpdateThread.GetTransportLayerName: String;
var
  transportConfig: TTransportConfig;
begin
  // Initialize the result.
  Result := 'Unknown transport layer';
  // Obtain access to the related configuration group.
  transportConfig := FFirmwareUpdate.FCurrentConfig.Groups[TTransportConfig.GROUP_NAME]
                     as TTransportConfig;
  // Filter on the configured transport layer.
  if transportConfig.Transport = 'xcp_rs232' then
  begin
    Result := 'XCP on RS232';
  end
  else if transportConfig.Transport = 'xcp_can' then
  begin
    Result := 'XCP on CAN';
  end
  else if transportConfig.Transport = 'xcp_usb' then
  begin
    Result := 'XCP on USB';
  end
  else if transportConfig.Transport = 'xcp_net' then
  begin
    Result := 'XCP on TCP/IP';
  end;
end; //*** end of GetTransportLayerName ***


//***************************************************************************************
// NAME:           LogTransportLayerSettings
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Logs the settings of the transport layer that will be used for the
//                 firmware update.
//
//***************************************************************************************
procedure TFirmwareUpdateThread.LogTransportLayerSettings;
var
  transportConfig: TTransportConfig;
  transportXcpRs232Config: TTransportXcpRs232Config;
  transportXcpCanConfig: TTransportXcpCanConfig;
  transportXcpTcpIpConfig: TTransportXcpTcpIpConfig;
begin
  // Obtain access to the related configuration group.
  transportConfig := FFirmwareUpdate.FCurrentConfig.Groups[TTransportConfig.GROUP_NAME]
                     as TTransportConfig;
  // Filter on the configured transport layer.
  // ------------------------------------ XCP on RS232 ----------------------------------
  if transportConfig.Transport = 'xcp_rs232' then
  begin
    // Obtain access to the related configuration group.
    transportXcpRs232Config := FFirmwareUpdate.FCurrentConfig.Groups[TTransportXcpRs232Config.GROUP_NAME]
                               as TTransportXcpRs232Config;
    FLogString := '  -> Device: ' + transportXcpRs232Config.Device;
    Synchronize(@SynchronizeLogEvent);
    FLogString := '  -> Baudrate: ' + IntToStr(transportXcpRs232Config.Baudrate) + ' bit/sec';
    Synchronize(@SynchronizeLogEvent);
  end
  // ------------------------------------ XCP on CAN ------------------------------------
  else if transportConfig.Transport = 'xcp_can' then
  begin
    // Obtain access to the related configuration group.
    transportXcpCanConfig := FFirmwareUpdate.FCurrentConfig.Groups[TTransportXcpCanConfig.GROUP_NAME]
                             as TTransportXcpCanConfig;
    FLogString := '  -> Device: ' + transportXcpCanConfig.Device + ' (channel ' +
                  IntToStr(transportXcpCanConfig.Channel) +  ' )';
    Synchronize(@SynchronizeLogEvent);
    FLogString := '  -> Baudrate: ' + IntToStr(transportXcpCanConfig.Baudrate) + ' bit/sec';
    Synchronize(@SynchronizeLogEvent);
    FLogString := '  -> Transmit CAN identifer: ' + Format('%.xh', [transportXcpCanConfig.TransmitId]);
    Synchronize(@SynchronizeLogEvent);
    FLogString := '  -> Receive CAN identifer: ' + Format('%.xh', [transportXcpCanConfig.ReceiveId]);
    Synchronize(@SynchronizeLogEvent);
    FLogString := '  -> Use 29-bit CAN identifiers: ';
    if transportXcpCanConfig.ExtendedId > 0 then
      FLogString := FLogString + 'Yes'
    else
      FLogString := FLogString + 'No';
    Synchronize(@SynchronizeLogEvent);
  end
  // ------------------------------------ XCP on USB ------------------------------------
  else if transportConfig.Transport = 'xcp_usb' then
  begin
    FLogString := '  -> No additional settings required';
    Synchronize(@SynchronizeLogEvent);
  end
  // ------------------------------------ XCP on TCP/IP ---------------------------------
  else if transportConfig.Transport = 'xcp_net' then
  begin
    // Obtain access to the related configuration group.
    transportXcpTcpIpConfig := FFirmwareUpdate.FCurrentConfig.Groups[TTransportXcpTcpIpConfig.GROUP_NAME]
                               as TTransportXcpTcpIpConfig;
    FLogString := '  -> Address: ' + transportXcpTcpIpConfig.Address;
    Synchronize(@SynchronizeLogEvent);
    FLogString := '  -> Port: ' + IntToStr(transportXcpTcpIpConfig.Port);
    Synchronize(@SynchronizeLogEvent);
  end
  else
  begin
    FLogString := '  -> Unknown transport layer settings';
    Synchronize(@SynchronizeLogEvent);
  end;
end; //*** end of LogTransportLayerSettings ***


//***************************************************************************************
// NAME:           SynchronizeStartedEvent
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Synchronizes to the main thread to execute the code inside this
//                 procedure. This function should only be called from thread level,
//                 so from Execute-method in the following manner: Synchronize(@<name>).
//
//***************************************************************************************
procedure TFirmwareUpdateThread.SynchronizeStartedEvent;
begin
  // Only continue if the event is set.
  if Assigned(FFirmwareUpdate.FStartedEvent) then
  begin
    // Trigger the event.
    FFirmwareUpdate.FStartedEvent(FFirmwareUpdate);
  end;
end; //*** end of SynchronizeStartedEvent ***


//***************************************************************************************
// NAME:           SynchronizeStoppedEvent
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Synchronizes to the main thread to execute the code inside this
//                 procedure. This function should only be called from thread level,
//                 so from Execute-method in the following manner: Synchronize(@<name>).
//
//***************************************************************************************
procedure TFirmwareUpdateThread.SynchronizeStoppedEvent;
begin
  // Only continue if the event is set.
  if Assigned(FFirmwareUpdate.FStoppedEvent) then
  begin
    // Trigger the event.
    FFirmwareUpdate.FStoppedEvent(FFirmwareUpdate);
  end;
end; //*** end of SynchronizeStoppedEvent ***


//***************************************************************************************
// NAME:           SynchronizeDoneEvent
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Synchronizes to the main thread to execute the code inside this
//                 procedure. This function should only be called from thread level,
//                 so from Execute-method in the following manner: Synchronize(@<name>).
//
//***************************************************************************************
procedure TFirmwareUpdateThread.SynchronizeDoneEvent;
begin
  // Only continue if the event is set.
  if Assigned(FFirmwareUpdate.FDoneEvent) then
  begin
    // Trigger the event.
    FFirmwareUpdate.FDoneEvent(FFirmwareUpdate);
  end;
end; //*** end of SynchronizeDoneEvent ***


//***************************************************************************************
// NAME:           SynchronizeInfoEvent
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Synchronizes to the main thread to execute the code inside this
//                 procedure. This function should only be called from thread level,
//                 so from Execute-method in the following manner: Synchronize(@<name>).
//                 Make sure field FInfoString is set to the desired value.
//
//***************************************************************************************
procedure TFirmwareUpdateThread.SynchronizeInfoEvent;
begin
  // Only continue if the event is set.
  if Assigned(FFirmwareUpdate.FInfoEvent) then
  begin
    // Trigger the event.
    FFirmwareUpdate.FInfoEvent(FFirmwareUpdate, FInfoString);
  end;
end; //*** end of SynchronizeInfoEvent ***


//***************************************************************************************
// NAME:           SynchronizeLogEvent
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Synchronizes to the main thread to execute the code inside this
//                 procedure. This function should only be called from thread level,
//                 so from Execute-method in the following manner: Synchronize(@<name>).
//                 Make sure field FLogString is set to the desired value.
//
//***************************************************************************************
procedure TFirmwareUpdateThread.SynchronizeLogEvent;
begin
  // Only continue if the event is set.
  if Assigned(FFirmwareUpdate.FLogEvent) then
  begin
    // Trigger the event.
    FFirmwareUpdate.FLogEvent(FFirmwareUpdate, FLogString);
  end;
end; //*** end of SynchronizeLogEvent ***


//***************************************************************************************
// NAME:           SynchronizeProgressEvent
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Synchronizes to the main thread to execute the code inside this
//                 procedure. This function should only be called from thread level,
//                 so from Execute-method in the following manner: Synchronize(@<name>).
//                 Make sure field FPercentage is set to the desired value.
//
//***************************************************************************************
procedure TFirmwareUpdateThread.SynchronizeProgressEvent;
begin
  // Only continue if the event is set.
  if Assigned(FFirmwareUpdate.FProgressEvent) then
  begin
    // Trigger the event.
    FFirmwareUpdate.FProgressEvent(FFirmwareUpdate, FPercentage);
  end;
end; //*** end of SynchronizeProgressEvent ***


//***************************************************************************************
// NAME:           SynchronizeErrorEvent
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Synchronizes to the main thread to execute the code inside this
//                 procedure. This function should only be called from thread level,
//                 so from Execute-method in the following manner: Synchronize(@<name>).
//                 Make sure field FLogString is set to the desired value.
//
//***************************************************************************************
procedure TFirmwareUpdateThread.SynchronizeErrorEvent;
begin
  // Only continue if the event is set.
  if Assigned(FFirmwareUpdate.FErrorEvent) then
  begin
    // Trigger the event.
    FFirmwareUpdate.FErrorEvent(FFirmwareUpdate, FErrorString);
  end;
end; //*** end of SynchronizeErrorEvent ***

end.
//******************************** end of firmwareupdate.pas ****************************


