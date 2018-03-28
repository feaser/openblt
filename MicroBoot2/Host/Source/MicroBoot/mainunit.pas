unit MainUnit;
//***************************************************************************************
//  Description: Contains the main user interface for MicroBoot.
//    File Name: mainunit.pas
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
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, StdCtrls,
  ExtCtrls, CurrentConfig, ConfigGroups, SettingsDialog, FirmwareUpdate;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  //------------------------------ TMainForm --------------------------------------------

  { TMainForm }

  TMainForm = class(TForm)
    BtnExit: TButton;
    BtnSettings: TButton;
    BtnStart: TButton;
    BtnStop: TButton;
    EdtFirmwareFile: TEdit;
    LblInfo: TLabel;
    MmoLog: TMemo;
    PnlFooterButtons: TPanel;
    PnlFooter: TPanel;
    PnlBody: TPanel;
    procedure BtnExitClick(Sender: TObject);
    procedure BtnSettingsClick(Sender: TObject);
    procedure BtnStartClick(Sender: TObject);
    procedure BtnStopClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private
    FCurrentConfig: TCurrentConfig;
    FFirmwareUpdate: TFirmwareUpdate;
    procedure FirmwareUpdateStarted(Sender: TObject);
    procedure FirmwareUpdateStopped(Sender: TObject);
    procedure FirmwareUpdateDone(Sender: TObject);
    procedure FirmwareUpdateInfo(Sender: TObject; InfoString: String);
    procedure FirmwareUpdateLog(Sender: TObject; LogString: String);
    procedure FirmwareUpdateProgress(Sender: TObject; Percentage: Integer);
    procedure FirmwareUpdateError(Sender: TObject; ErrorString: String);
  public
  end;


//***************************************************************************************
// Global Variables
//***************************************************************************************
var
  MainForm: TMainForm;

implementation

{$R *.lfm}

//---------------------------------------------------------------------------------------
//-------------------------------- TMainForm --------------------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           FormCreate
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form constructor.
//
//***************************************************************************************
procedure TMainForm.FormCreate(Sender: TObject);
var
  mainWindowConfig: TMainWindowConfig;
begin
  // Clear panel captions as these are only needed as hint during design time.
  PnlBody.Caption := '';
  PnlFooter.Caption := '';
  PnlFooterButtons.Caption := '';
  // Create instance to manage the program's configuration and add the configuration
  // group instances.
  FCurrentConfig := TCurrentConfig.Create;
  FCurrentConfig.AddGroup(TMainWindowConfig.Create);
  FCurrentConfig.AddGroup(TMiscellaneousConfig.Create);
  FCurrentConfig.AddGroup(TSessionConfig.Create);
  FCurrentConfig.AddGroup(TSessionXcpConfig.Create);
  FCurrentConfig.AddGroup(TTransportConfig.Create);
  FCurrentConfig.AddGroup(TTransportXcpRs232Config.Create);
  FCurrentConfig.AddGroup(TTransportXcpCanConfig.Create);
  FCurrentConfig.AddGroup(TTransportXcpUsbConfig.Create);
  FCurrentConfig.AddGroup(TTransportXcpTcpIpConfig.Create);
  // Load the program's configuration from the configuration file.
  FCurrentConfig.LoadFromFile;
  // Set main window configuration settings.
  mainWindowConfig := FCurrentConfig.Groups[TMainWindowConfig.GROUP_NAME]
                      as TMainWindowConfig;
  MainForm.Width := mainWindowConfig.Width;
  MainForm.Height := mainWindowConfig.Height;
  // Create instance of the firmware update class.
  FFirmwareUpdate := TFirmwareUpdate.Create(FCurrentConfig);
  // Register its event handlers.
  FFirmwareUpdate.OnStarted := @FirmwareUpdateStarted;
  FFirmwareUpdate.OnStopped := @FirmwareUpdateStopped;
  FFirmwareUpdate.OnDone := @FirmwareUpdateDone;
  FFirmwareUpdate.OnInfo := @FirmwareUpdateInfo;
  FFirmwareUpdate.OnLog := @FirmwareUpdateLog;
  FFirmwareUpdate.OnProgress := @FirmwareUpdateProgress;
  FFirmwareUpdate.OnError := @FirmwareUpdateError;
end; //*** end of FormCreate


//***************************************************************************************
// NAME:           FormDestroy
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form destructor.
//
//***************************************************************************************
procedure TMainForm.FormDestroy(Sender: TObject);
var
  mainWindowConfig: TMainWindowConfig;
begin
  // Release instance of the firmware update class.
  FFirmwareUpdate.Free;
  // Store main window configuration settings.
  mainWindowConfig := FCurrentConfig.Groups[TMainWindowConfig.GROUP_NAME]
                      as TMainWindowConfig;
  mainWindowConfig.Width := MainForm.Width;
  mainWindowConfig.Height := MainForm.Height;
  // Save the program's configuration to the configuration file.
  FCurrentConfig.SaveToFile;
  // Release the instance that manages the program's configuration.
  FCurrentConfig.Free;
end; //*** end of FormDestroy ***


//***************************************************************************************
// NAME:           FirmwareUpdateStarted
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when a firmware update just started.
//
//***************************************************************************************
procedure TMainForm.FirmwareUpdateStarted(Sender: TObject);
begin
  MmoLog.Lines.Add('[EVENT] OnStarted');
  { TODO : Implement firmware update OnStarted event handler. }
end; //*** end of FirmwareUpdateStarted ***


//***************************************************************************************
// NAME:           FirmwareUpdateStopped
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when a firmware update was stopped.
//
//***************************************************************************************
procedure TMainForm.FirmwareUpdateStopped(Sender: TObject);
begin
  LblInfo.Caption := '';
  MmoLog.Lines.Add('[EVENT] OnStopped');
  { TODO : Implement firmware update OnStopped event handler. }
end; //*** end of FirmwareUpdateStopped ***


//***************************************************************************************
// NAME:           FirmwareUpdateDone
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when a firmware update finished.
//
//***************************************************************************************
procedure TMainForm.FirmwareUpdateDone(Sender: TObject);
begin
  LblInfo.Caption := '';
  MmoLog.Lines.Add('[EVENT] OnDone');
  { TODO : Implement firmware update OnDone event handler. }
end; //*** end of FirmwareUpdateDone ***


//***************************************************************************************
// NAME:           FirmwareUpdateInfo
// PARAMETER:      Sender Source of the event.
//                 InfoString One liner with info text.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when a firmware update process has new
//                 info to report. The info string can be used to update a label on the
//                 user interface to inform the user of what the firmware updater is
//                 currently working on.
//
//***************************************************************************************
procedure TMainForm.FirmwareUpdateInfo(Sender: TObject; InfoString: String);
begin
  // Display info on the user interface.
  LblInfo.Caption := InfoString;
end; //*** end of FirmwareUpdateInfo ***


//***************************************************************************************
// NAME:           FirmwareUpdateLog
// PARAMETER:      Sender Source of the event.
//                 LogString Text for logging purposes.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when a firmware update process has new
//                 log information to report. The log string can be used to display
//                 details regarding the firmware update process to the user or to write
//                 this information to a log-file.
//
//***************************************************************************************
procedure TMainForm.FirmwareUpdateLog(Sender: TObject; LogString: String);
begin
  MmoLog.Lines.Add(LogString);
  { TODO : Implement firmware update OnLog event handler. }
end; //*** end of FirmwareUpdateLog ***


//***************************************************************************************
// NAME:           FirmwareUpdateProgress
// PARAMETER:      Sender Source of the event.
//                 Percentage Firmware update progress as a percentage (0..100).
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when a firmware update process has new
//                 progress to report. The progress information can be used to update
//                 a progress bar for example.
//
//***************************************************************************************
procedure TMainForm.FirmwareUpdateProgress(Sender: TObject; Percentage: Integer);
begin
  MmoLog.Lines.Add('[EVENT] OnProgress: ' + IntToStr(Percentage));
  { TODO : Implement firmware update OnProgress event handler. }
end; //*** end of FirmwareUpdateProgress ***


//***************************************************************************************
// NAME:           FirmwareUpdateError
// PARAMETER:      Sender Source of the event.
//                 ErrorString Descriptive text regarding the error that occurred.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when an error was detected during the
//                 firmware update process. This information can be used for logging
//                 purposes and also to stop the firmware update process.
//
//***************************************************************************************
procedure TMainForm.FirmwareUpdateError(Sender: TObject; ErrorString: String);
begin
  LblInfo.Caption := '';
  MmoLog.Lines.Add('[EVENT] OnError: ' + ErrorString);
  { TODO : Implement firmware update OnError event handler. }
end; //*** end of FirmwareUpdateError ***


//***************************************************************************************
// NAME:           BtnExitClick
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the button is clicked.
//
//***************************************************************************************
procedure TMainForm.BtnExitClick(Sender: TObject);
begin
  // Exit the program.
  Close;
end; //*** end of BtnExitClick ***


//***************************************************************************************
// NAME:           BtnSettingsClick
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the button is clicked.
//
//***************************************************************************************
procedure TMainForm.BtnSettingsClick(Sender: TObject);
var
  settingsDialog: TSettingsForm;
begin
  // Create the dialog and make us the owner.
  settingsDialog := TSettingsForm.Create(Self, FCurrentConfig);
  // Show the dialog in the modal state.
  settingsDialog.ShowModal;
  // Release the dialog.
  settingsDialog.Free;
end; //*** end of BtnSettingsClick ***


//***************************************************************************************
// NAME:           BtnStartClick
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the button is clicked.
//
//***************************************************************************************
procedure TMainForm.BtnStartClick(Sender: TObject);
begin
  // Clear the memo.
  MmoLog.Clear;
  // Start the firmware update.
  if not FFirmwareUpdate.Start(EdtFirmwareFile.Text) then
  begin
    MmoLog.Lines.Add('[ERROR] Could not start firmware update.');
  end;
end; //*** end of BtnStartClick ***


//***************************************************************************************
// NAME:           BtnStopClick
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the button is clicked.
//
//***************************************************************************************
procedure TMainForm.BtnStopClick(Sender: TObject);
begin
  // Stop the firmware update.
  FFirmwareUpdate.Stop;
end; //*** end of BtnStopClick ***

end.
//******************************** end of mainunit.pas **********************************

