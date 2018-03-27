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
  Classes, SysUtils, CurrentConfig, ConfigGroups, OpenBlt;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
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

  //------------------------------ TFirmwareUpdate --------------------------------------
  TFirmwareUpdate = class (TObject)
  private
    FCurrentConfig: TCurrentConfig;
    FInProgress: Boolean;
    FStartedEvent: TFirmwareUpdateStartedEvent;
    FStoppedEvent: TFirmwareUpdateStoppedEvent;
    FDoneEvent: TFirmwareUpdateDoneEvent;
    FInfoEvent: TFirmwareUpdateInfoEvent;
    FProgressEvent: TFirmwareUpdateProgressEvent;
    FErrorEvent: TFirmwareUpdateErrorEvent;
  public
    constructor Create(CurrentConfig: TCurrentConfig); reintroduce;
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
  FInProgress := False;
  FStartedEvent := nil;
  FStoppedEvent := nil;
  FDoneEvent := nil;
  FInfoEvent := nil;
  FProgressEvent := nil;
  FErrorEvent := nil;
end; //*** end of Create ***


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
  Result := True;
  { TODO : Start the firmware update }
  // ##Vg Probably want to use a background thread for doing the actual firmware update.
  //      Note that this means the events need to be synchronized somehow I think.
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
  { TODO : Stop the firmware update }
end;  //*** end of Stop ***

end.
//******************************** end of firmwareupdate.pas ****************************


