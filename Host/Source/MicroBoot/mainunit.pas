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
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, StdCtrls, LCLType,
  ExtCtrls, ComCtrls, CurrentConfig, ConfigGroups, SettingsDialog, FirmwareUpdate,
  StopWatch, FileLogger;


//***************************************************************************************
// Constant declarations.
//***************************************************************************************
const
  PROGRAM_NAME_STR = 'MicroBoot';
  PROGRAM_VERSION_STR = 'v2.05';


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  //------------------------------ TUserInterfaceSetting --------------------------------
  TUserInterfaceSetting = ( UIS_DEFAULT = 0,
                            UIS_FIRMWARE_UPDATE );

  //------------------------------ TMainForm --------------------------------------------
  TMainForm = class(TForm)
    BtnExit: TButton;
    BtnSettings: TButton;
    BtnBrowse: TButton;
    ImgHeader: TImage;
    LblElapsedTime: TLabel;
    LblFirmwareUpdateInfo: TLabel;
    LblProgramConfig: TLabel;
    LblProgramName: TLabel;
    OpenDialog: TOpenDialog;
    PnlBodyMain: TPanel;
    PnlBodyRight: TPanel;
    PnlHeader: TPanel;
    PnlFooterButtons: TPanel;
    PnlFooter: TPanel;
    PnlBody: TPanel;
    PgbFirmwareUpdate: TProgressBar;
    TmrClose: TTimer;
    procedure BtnBrowseClick(Sender: TObject);
    procedure BtnExitClick(Sender: TObject);
    procedure BtnSettingsClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure PnlBodyMainResize(Sender: TObject);
    procedure TmrCloseTimer(Sender: TObject);
  private
    FCurrentConfig: TCurrentConfig;
    FFirmwareUpdate: TFirmwareUpdate;
    FUISetting: TUserInterfaceSetting;
    FStopWatch: TStopWatch;
    FFileLogger: TFileLogger;
    FHFreeSpaceProgressBar: Integer;
    FCmdOptionFileFound: Boolean;
    FFirmwareFile: String;
    procedure ParseCommandLine;
    function  StartFirmwareUpdate: Boolean;
    procedure FinishFirmwareUpdate(CloseProgram: Boolean);
    procedure CancelFirmwareUpdate;
    procedure HandleFirmwareUpdateError(ErrorString: String);
    procedure UpdateUserInterface;
    procedure UpdateElapsedTime(Interval: String);
    procedure StopWatchUpdateEvent(Sender: TObject; Interval: String);
    procedure FirmwareUpdateStarted(Sender: TObject);
    procedure FirmwareUpdateStopped(Sender: TObject);
    procedure FirmwareUpdateDone(Sender: TObject);
    procedure FirmwareUpdateInfo(Sender: TObject; InfoString: String);
    procedure FirmwareUpdateLog(Sender: TObject; LogString: String);
    procedure FirmwareUpdateProgress(Sender: TObject; Percentage: Integer);
    procedure FirmwareUpdateError(Sender: TObject; ErrorString: String);
    function  GetConfigSummary: String;
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
  PnlHeader.Caption := '';
  PnlBody.Caption := '';
  PnlBodyMain.Caption := '';
  PnlBodyRight.Caption := '';
  PnlFooter.Caption := '';
  PnlFooterButtons.Caption := '';
  // Store the default difference in width between the progress bar and its parent panel.
  FHFreeSpaceProgressBar := PnlBodyMain.Width - PgbFirmwareUpdate.Width;
  // Initialize the user interface.
  FUISetting := UIS_DEFAULT;
  UpdateUserInterface();
  // Initialize fields.
  FCmdOptionFileFound := False;
  FFirmwareFile := '';
  // Parse the command line.
  ParseCommandLine;
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
  // Update the program configuration label.
  LblProgramConfig.Caption := GetConfigSummary;
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
  // Create and configure stopwatch instance.
  FStopWatch := TStopWatch.Create;
  FStopWatch.OnUpdate := @StopWatchUpdateEvent;
  // Create the file logger instance.
  FFileLogger := TFileLogger.Create;
  // Automatically kick off the firmware update procedure if a firmware file was
  // specified on the command line.
  if FCmdOptionFileFound then
  begin
    StartFirmwareUpdate;
  end;
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
  // Release the file logger instance.
  FFileLogger.Free;
  // Release stopwatch instance.
  FStopWatch.Free;
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
// NAME:           ParseCommandLine
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Parses the command line parameters.
//
//***************************************************************************************
procedure TMainForm.ParseCommandLine;
begin
  // The program currently support one command line parameter, which is the firmware
  // file. If a valid file is specified, the firmware update should start automatically.
  if ParamCount = 1 then
  begin
    // Check if parameter contains an existing file.
    if FileExists(ParamStr(1)) then
    begin
      // Store the filename.
      FFirmwareFile := ParamStr(1);
      // Set flag for later processing.
      FCmdOptionFileFound := True;
    end;
  end;
end; //*** end of ParseCommandLine ***


//***************************************************************************************
// NAME:           PnlBodyMainResize
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the panel is resized.
//
//***************************************************************************************
procedure TMainForm.PnlBodyMainResize(Sender: TObject);
begin
  // Also resize the progress bar.
  PgbFirmwareUpdate.Width := PnlBodyMain.Width - FHFreeSpaceProgressBar;
end; //*** end of PnlBodyMainResize ***


//***************************************************************************************
// NAME:           TmrCloseTimer
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the timer expires.
//
//***************************************************************************************
procedure TMainForm.TmrCloseTimer(Sender: TObject);
begin
  // Disable the timer, because it is a one-shot timer.
  TmrClose.Enabled := False;
  // Close the program.
  Close;
end; //*** end of TmrCloseTimer ***


//***************************************************************************************
// NAME:           StartFirmwareUpdate
// PARAMETER:      none
// RETURN VALUE:   True if successful, False otherwise.
// DESCRIPTION:    Starts the firmware update procedure.
//***************************************************************************************
function  TMainForm.StartFirmwareUpdate: Boolean;
var
  miscellaneousConfig: TMiscellaneousConfig;
begin
  // Initialize the result.
  Result := False;
  // Attempt to start the firmware update.
  if FFirmwareUpdate.Start(FFirmwareFile) then
  begin
    // Update the user interface setting.
    FUISetting := UIS_FIRMWARE_UPDATE;
    // Update the user interface.
    UpdateUserInterface;
    // Determine if file logging is requested.
    miscellaneousConfig := FCurrentConfig.Groups[TMiscellaneousConfig.GROUP_NAME]
                           as TMiscellaneousConfig;
    if (miscellaneousConfig.Logging <> 0) and (miscellaneousConfig.LogFile <> '') then
    begin
      // Configure and start file logging.
      FFileLogger.LogFile := miscellaneousConfig.LogFile;
      FFileLogger.Start;
    end;
    // Start the stop watch refresh timer.
    FStopWatch.Start;
  end;
end; //*** end of StartFirmwareUpdate ***


//***************************************************************************************
// NAME:           FinishFirmwareUpdate
// PARAMETER:      CloseProgram True if the program should be closed, false otherwise.
// RETURN VALUE:   none
// DESCRIPTION:    Finished the firmware update after the firmware update procedure
//                 completed.
//***************************************************************************************
procedure TMainForm.FinishFirmwareUpdate(CloseProgram: Boolean);
begin
  // Stop file logging.
  FFileLogger.Stop;
  // Close the program if requested.
  if CloseProgram then
  begin
    // Start timer to perform a delayed closing of the program. This procedure could be
    // called from one of the OnXxx event handlers of the firmware update class. These
    // events are synchronized to the main loop, meaning that the internal thread of the
    // firmware update class is suspended until the event function completes. When you
    // close the program, it will also free the firmware update class, which in turn
    // terminates its internal thread. This could deadlock, because it might still be
    // suspended. The timer makes it possible for the internal thread of the firmware
    // update class to complete and terminate itself, preventing the deadlock situation.
    TmrClose.Enabled := True;
  end
  else
  begin
    // Stop the stop watch refresh timer.
    FStopWatch.Stop;
    // Update the user interface setting.
    FUISetting := UIS_DEFAULT;
    // Update the user interface.
    UpdateUserInterface;
  end;
end; //*** end of FinishFirmwareUpdate ***


//***************************************************************************************
// NAME:           CancelFirmwareUpdate
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Cancels an ongoing firmware update procedure.
//***************************************************************************************
procedure TMainForm.CancelFirmwareUpdate;
begin
  // Stop the stop watch refresh timer.
  FStopWatch.Stop;
  // Cancel the firmware update.
  FFirmwareUpdate.Stop;
  // Update the user interface setting.
  FUISetting := UIS_DEFAULT;
  // Update the user interface.
  UpdateUserInterface;
end; //*** end of CancelFirmwareUpdate ***


//***************************************************************************************
// NAME:           HandleFirmwareUpdateError
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Handles the situation when an error was detected during a firmware
//                 update.
//***************************************************************************************
procedure TMainForm.HandleFirmwareUpdateError(ErrorString: String);
var
  boxStyle: Integer;
begin
  // Stop the stop watch refresh timer.
  FStopWatch.Stop;
  // Configure the message box.
  boxStyle := MB_ICONERROR + MB_OK;
  // Display the message box.
  Application.MessageBox(PAnsiChar(AnsiString(ErrorString)), 'Error detected', boxStyle);
  // Update the user interface setting.
  FUISetting := UIS_DEFAULT;
  // Update the user interface.
  UpdateUserInterface;
end; //*** end of HandleFirmwareUpdateError ***


//***************************************************************************************
// NAME:           UpdateUserInterface
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Updates the user interface look and layout based on the current
//                 setting.
//
//***************************************************************************************
procedure TMainForm.UpdateUserInterface;
begin
  // Update look and layout for the default setting.
  if FUISetting = UIS_DEFAULT then
  begin
    Caption := PROGRAM_NAME_STR + ' ' + PROGRAM_VERSION_STR;
    LblFirmwareUpdateInfo.Caption := 'Select file to start the firmware update';
    LblElapsedTime.Caption := '';
    PgbFirmwareUpdate.Position := 0;
    BtnBrowse.Enabled := True;
    BtnSettings.Enabled := True;
    BtnExit.Caption := 'Exit';
  end
  // Update look and layout for the firmware update setting.
  else if FUISetting = UIS_FIRMWARE_UPDATE then
  begin
    Caption := PROGRAM_NAME_STR +' ' + PROGRAM_VERSION_STR + ' - ' +
               ExtractFileName(FFirmwareFile) + '..';
    UpdateElapsedTime('');
    PgbFirmwareUpdate.Position := 0;
    BtnBrowse.Enabled := False;
    BtnSettings.Enabled := False;
    BtnExit.Caption := 'Cancel';
  end;
end; //*** end of UpdateUserInterface ***


//***************************************************************************************
// NAME:           UpdateElapsedTime
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Updates the elapsed time on the user interface.
//
//***************************************************************************************
procedure TMainForm.UpdateElapsedTime(Interval: String);
begin
  LblElapsedTime.Caption := 'Elapsed time: ' + Interval;
end; //*** end of UpdateElapsedTime ***


//***************************************************************************************
// NAME:           StopWatchUpdateEvent
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the stopwatch got updated.
//
//***************************************************************************************
procedure TMainForm.StopWatchUpdateEvent(Sender: TObject; Interval: String);
begin
  // Update the elapsed time on the user interface.
  UpdateElapsedTime(Interval);
end; //*** end of StopWatchUpdateEvent ***


//***************************************************************************************
// NAME:           FirmwareUpdateStarted
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when a firmware update just started.
//
//***************************************************************************************
procedure TMainForm.FirmwareUpdateStarted(Sender: TObject);
begin
  // Nothing need to be done here for now.
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
  // Finish up to firmware update but do not close the program, because the firmware
  // update was cancelled. This makes if possible for the user to retry.
  FinishFirmwareUpdate(False);
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
  // Finish firmware update and close the program
  FinishFirmwareUpdate(True);
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
  // Display the info on the user interface.
  LblFirmwareUpdateInfo.Caption := InfoString;
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
  // Pass the log event on to the file logger, if active.
  if FFileLogger.Started then
  begin
    FFileLogger.Log(LogString);
  end;
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
  // Display the progress on the user interface.
  PgbFirmwareUpdate.Position := Percentage;
  // Fix for progress bar not going 100%
  PgbFirmwareUpdate.Position := Percentage - 1;
  // Update progress bar one more time.
  PgbFirmwareUpdate.Position := Percentage;
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
  // Handle the error.
  HandleFirmwareUpdateError(ErrorString);
end; //*** end of FirmwareUpdateError ***


//***************************************************************************************
// NAME:           GetConfigSummary
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   Configuration summary.
// DESCRIPTION:    Obtains a string that contains a summary of the current active
//                 configuration, for example: 'for OpenBLT using XCP on UART'.
//
//***************************************************************************************
function TMainForm.GetConfigSummary: String;
var
  sessionConfig: TSessionConfig;
  transportConfig: TTransportConfig;
begin
  // Initialize the result.
  Result := 'Unknown configuration';
  // Obtain access to the session configuration group.
  sessionConfig := FCurrentConfig.Groups[TSessionConfig.GROUP_NAME]
                   as TSessionConfig;
  // Obtain access to the transport configuration group.
  transportConfig := FCurrentConfig.Groups[TTransportConfig.GROUP_NAME]
                     as TTransportConfig;
  // Filter on the configured session protocol.
  if sessionConfig.Session = 'xcp' then
  begin
    Result := 'for OpenBLT using XCP ';
    if transportConfig.Transport = 'xcp_rs232' then
    begin
      Result := Result +  'on RS232';
    end
    else if transportConfig.Transport = 'xcp_can' then
    begin
      Result := Result +  'on CAN';
    end
    else if transportConfig.Transport = 'xcp_usb' then
    begin
      Result := Result +  'on USB';
    end
    else if transportConfig.Transport = 'xcp_net' then
    begin
      Result := Result +  'on TCP/IP';
    end;
  end;
end; //*** end of GetConfigSummary ***


//***************************************************************************************
// NAME:           BtnExitClick
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the button is clicked.
//
//***************************************************************************************
procedure TMainForm.BtnExitClick(Sender: TObject);
begin
  if BtnExit.Caption = 'Exit' then
  begin
    // Exit the program.
    Close;
  end
  else
  begin
    // Cancel the firmware update.
    CancelFirmwareUpdate;
  end;
end; //*** end of BtnExitClick ***


//***************************************************************************************
// NAME:           BtnBrowseClick
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the button is clicked.
//
//***************************************************************************************
procedure TMainForm.BtnBrowseClick(Sender: TObject);
begin
  // Reset firmware file name.
  FFirmwareFile := '';
  // Display the dialog to prompt the user to pick a file.
  if OpenDialog.Execute then
  begin
    // Read out the selected file.
    FFirmwareFile := OpenDialog.FileName;
    // Start the actual firmware update.
    StartFirmwareUpdate;
  end;
end; //*** end of BtnBrowseClick ***


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
  if settingsDialog.ShowModal = mrOK then
  begin
    // Save the new settings to the file.
    FCurrentConfig.SaveToFile;
    // Update the program configuration label.
    LblProgramConfig.Caption := GetConfigSummary;
  end;
  // Release the dialog.
  settingsDialog.Free;
end; //*** end of BtnSettingsClick ***

end.
//******************************** end of mainunit.pas **********************************

