unit MiscellaneousDialog;
//***************************************************************************************
//  Description: Implements the miscellaneous settings dialog.
//    File Name: miscellaneousdialog.pas
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
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, StdCtrls, StrUtils,
  ConfigGroups;

//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  //------------------------------ TMiscellaneousForm -----------------------------------

  { TMiscellaneousForm }

  TMiscellaneousForm = class(TForm)
    BtnLogFile: TButton;
    CbxLogging: TCheckBox;
    EdtLogFile: TEdit;
    LblLogFile: TLabel;
    LblLogging: TLabel;
    SaveDialog: TSaveDialog;
    procedure BtnLogFileClick(Sender: TObject);
    procedure CbxLoggingChange(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private
    FMiscellaneousConfig: TMiscellaneousConfig;
    procedure UpdateUserInterface;
  public
    procedure LoadConfig(Config: TMiscellaneousConfig);
    procedure SaveConfig(Config: TMiscellaneousConfig);
  end;


implementation

{$R *.lfm}

//---------------------------------------------------------------------------------------
//-------------------------------- TMiscellaneousForm -----------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           FormCreate
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form constructor.
//
//***************************************************************************************
procedure TMiscellaneousForm.FormCreate(Sender: TObject);
begin
  // Create configuration group instance.
  FMiscellaneousConfig := TMiscellaneousConfig.Create;
  // Align browse button vertically to the related edit box.
  BtnLogFile.Top := EdtLogFile.Top;
  BtnLogFile.Height := EdtLogFile.Height + 1;
  // Empty the log-file edit box.
  EdtLogFile.Text := '';
end; //*** end of FormCreate ***


//***************************************************************************************
// NAME:           CbxLoggingChange
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the checkbox changes.
//
//***************************************************************************************
procedure TMiscellaneousForm.CbxLoggingChange(Sender: TObject);
begin
  // Update the user interface.
  UpdateUserInterface;
end; //*** end of CbxLoggingChange ***


//***************************************************************************************
// NAME:           BtnLogFileClick
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the button is clicked.
//
//***************************************************************************************
procedure TMiscellaneousForm.BtnLogFileClick(Sender: TObject);
var
  initialDir: String;
  logFile: String;
begin
  // If a file is already specified in the associated edit box, then use that directory.
  // Otherwise use the program's current working directory as the initial directory.
  initialDir := GetCurrentDir;
  if EdtLogFile.Text <> '' then
  begin
    if DirectoryExists(ExtractFileDir(EdtLogFile.Text)) then
      initialDir := ExtractFileDir(EdtLogFile.Text);
  end;
  SaveDialog.InitialDir := initialDir;

  // Display the dialog to prompt the user to pick a file.
  if SaveDialog.Execute then
  begin
    // Read out the selected file.
    logFile := SaveDialog.FileName;
    // Make it a relative path if it is in the current working directory or a
    // subdirectory there of.
    if AnsiStartsText(GetCurrentDir, logFile) then
    begin
      logFile := ExtractRelativepath(GetCurrentDir + PathDelim,
                 ExtractFilePath(logFile)) + ExtractFileName(logFile);
    end;
    // Set the filename in the associated edit box.
    EdtLogFile.Text := logFile;
  end;
end; //*** end of BtnLogFileClick ***


//***************************************************************************************
// NAME:           FormDestroy
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form destructor.
//
//***************************************************************************************
procedure TMiscellaneousForm.FormDestroy(Sender: TObject);
begin
  // Release the configuration group instance.
  FMiscellaneousConfig.Free;
end; //*** end of FormDestroy ***


//***************************************************************************************
// NAME:           UpdateUserInterface
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Refreshes the user interface.
//
//***************************************************************************************
procedure TMiscellaneousForm.UpdateUserInterface;
begin
  EdtLogFile.Enabled := CbxLogging.Checked;
  BtnLogFile.Enabled := CbxLogging.Checked;
end; //*** end of UpdateUserInterface ***


//***************************************************************************************
// NAME:           LoadConfig
// PARAMETER:      Config Configuration instance to load from.
// RETURN VALUE:   none
// DESCRIPTION:    Loads the configuration values from the specified instance and
//                 initializes the user interface accordingly.
//
//***************************************************************************************
procedure TMiscellaneousForm.LoadConfig(Config: TMiscellaneousConfig);
begin
  // Load configuration.
  FMiscellaneousConfig.Logging := Config.Logging;
  FMiscellaneousConfig.LogFile := Config.LogFile;
  // Initialize user interface.
  if FMiscellaneousConfig.Logging = 0 then
    CbxLogging.Checked := False
  else
    CbxLogging.Checked := True;
  EdtLogFile.Text := FMiscellaneousConfig.LogFile;
  // Update the user interface.
  UpdateUserInterface;
end; //*** end of LoadConfig ***


//***************************************************************************************
// NAME:           SaveConfig
// PARAMETER:      Config Configuration instance to save to.
// RETURN VALUE:   none
// DESCRIPTION:    Reads the configuration values from the user interface and stores them
//                 in the specified instance.
//
//***************************************************************************************
procedure TMiscellaneousForm.SaveConfig(Config: TMiscellaneousConfig);
begin
  // Start out with default configuration settings.
  FMiscellaneousConfig.Defaults;
  // Read configuration from the user interface.
  if CbxLogging.Checked then
    FMiscellaneousConfig.Logging := 1
  else
    FMiscellaneousConfig.Logging := 0;
  FMiscellaneousConfig.LogFile := EdtLogFile.Text;
  // Store configuration.
  Config.Logging := FMiscellaneousConfig.Logging;
  Config.LogFile := FMiscellaneousConfig.LogFile;
end; //*** end of SaveConfig ***

end.
//******************************** end of miscellaneousdialog.pas ***********************

