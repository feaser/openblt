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

  //------------------------------ TFirmwareUpdateProgressEvent -------------------------
  TFirmwareUpdateProgressEvent = procedure(Sender: TObject; Percentage: Integer) of object;

  //------------------------------ TFirmwareUpdateErrorEvent ----------------------------
  TFirmwareUpdateErrorEvent = procedure(Sender: TObject; ErrorString: String) of object;

  //------------------------------ TFirmwareUpdateState ---------------------------------
  TFirmwareUpdateState = ( FUS_IDLE = 0,
                           FUS_INITIALIZING,
                           FUS_LOADING_FIRMWARE,
                           FUS_ERASING_MEMORY,
                           FUS_PROGRAMMING_MEMORY,
                           FUS_CLEANING_UP );

  //------------------------------ TFirmwareUpdateThread --------------------------------
  TFirmwareUpdateThread = class(TThread)
    private
      FFirmwareUpdate: TFirmwareUpdate;
      FFirmwareFile: String;
      FState: TFirmwareUpdateState;
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
    property OnProgress: TFirmwareUpdateProgressEvent read FProgressEvent write FProgressEvent;
    property OnError: TFirmwareUpdateErrorEvent read FErrorEvent write FErrorEvent;
  end;


implementation
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
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Execute
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Thread execution function.
//
//***************************************************************************************
procedure TFirmwareUpdateThread.Execute;
begin
  // Enter thread's execution loop.
  while not Terminated do
  begin
    { TODO : Implement thread's execution method. }
    Sleep(50);
  end;
end; //*** end of Execute ***


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
      // Trigger the OnStarted event, if assigned.
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
  end;
  // Trigger the OnStopped event, if assigned.
  if Assigned(FStoppedEvent) then
  begin
    FStoppedEvent(Self);
  end;
end;  //*** end of Stop ***

end.
//******************************** end of firmwareupdate.pas ****************************


