unit SessionXcpDialog;
//***************************************************************************************
//  Description: Implements the XCP session dialog.
//    File Name: sessionxcpdialog.pas
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
  ExtCtrls, ConfigGroups, StrUtils, CustomUtil;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  //------------------------------ TSessionXcpForm ----------------------------------------

  { TSessionXcpForm }

  TSessionXcpForm = class(TForm)
    BtnSeedKey: TButton;
    CmbConnectMode: TComboBox;
    EdtSeedKey: TEdit;
    EdtTimeoutT1: TEdit;
    EdtTimeoutT3: TEdit;
    EdtTimeoutT4: TEdit;
    EdtTimeoutT5: TEdit;
    EdtTimeoutT6: TEdit;
    EdtTimeoutT7: TEdit;
    LblConnection: TLabel;
    LblSeedKey: TLabel;
    LblSecurity: TLabel;
    LblTimeoutT1: TLabel;
    LblTimeouts: TLabel;
    LblConnectMode: TLabel;
    LblTimeoutT3: TLabel;
    LblTimeoutT4: TLabel;
    LblTimeoutT5: TLabel;
    LblTimeoutT6: TLabel;
    LblTimeoutT7: TLabel;
    OpenDialog: TOpenDialog;
    procedure BtnSeedKeyClick(Sender: TObject);
    procedure EdtTimeoutChange(Sender: TObject);
    procedure EdtTimeoutKeyPress(Sender: TObject; var Key: char);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private
    FSessionXcpConfig: TSessionXcpConfig;
  public
    procedure LoadConfig(Config: TSessionXcpConfig);
    procedure SaveConfig(Config: TSessionXcpConfig);
  end;


implementation

{$R *.lfm}

//---------------------------------------------------------------------------------------
//-------------------------------- TSessionXcpForm --------------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           FormCreate
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form constructor.
//
//***************************************************************************************
procedure TSessionXcpForm.FormCreate(Sender: TObject);
begin
  // Create configuration group instance.
  FSessionXcpConfig := TSessionXcpConfig.Create;
  // Align browse button vertically to the related edit box.
  BtnSeedKey.Top := EdtSeedKey.Top;
  BtnSeedKey.Height := EdtSeedKey.Height + 1;
end; //*** end of FormCreate ***


//***************************************************************************************
// NAME:           FormDestroy
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form destructor.
//
//***************************************************************************************
procedure TSessionXcpForm.FormDestroy(Sender: TObject);
begin
  // Release the configuration group instance.
  FSessionXcpConfig.Free;
end; //*** end of FormDestroy ***


//***************************************************************************************
// NAME:           BtnSeedKeyClick
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the button is clicked.
//
//***************************************************************************************
procedure TSessionXcpForm.BtnSeedKeyClick(Sender: TObject);
var
  initialDir: String;
  sharedLibrary: String;
begin
  // If a file is already specified in the associated edit box, then use that directory.
  // Otherwise use the program's current working directory as the initial directory.
  initialDir := GetCurrentDir;
  if EdtSeedKey.Text <> '' then
  begin
    if DirectoryExists(ExtractFileDir(EdtSeedKey.Text)) then
      initialDir := ExtractFileDir(EdtSeedKey.Text);
  end;
  OpenDialog.InitialDir := initialDir;

  // Display the dialog to prompt the user to pick a file.
  if OpenDialog.Execute then
  begin
    // Read out the selected file.
    sharedLibrary := OpenDialog.FileName;
    // Make it a relative path if it is in the current working directory or a
    // subdirectory there of.
    if AnsiStartsText(GetCurrentDir, sharedLibrary) then
    begin
      sharedLibrary := ExtractRelativepath(GetCurrentDir + PathDelim,
                       ExtractFilePath(sharedLibrary)) + ExtractFileName(sharedLibrary);
    end;
    // Set the filename in the associated edit box.
    EdtSeedKey.Text := sharedLibrary;
  end;
end; //*** end of BtnSeedKeyClick ***


//***************************************************************************************
// NAME:           EdtTimeoutChange
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the contents in one of the Timeout
//                 edit boxes changed.
//
//***************************************************************************************
procedure TSessionXcpForm.EdtTimeoutChange(Sender: TObject);
var
  timeoutEdtBox: TEdit;
begin
  // Make sure the event source is an instance of class TEdit.
  Assert(Sender.InheritsFrom(TEdit), 'Event is triggered by an invalid sender.');
  timeoutEdtBox := Sender as TEdit;
  // Validate the edit box contents to make sure that it is a number within an allowed
  // range.
  if timeoutEdtBox.Text <> '' then
    timeoutEdtBox.Text := CustomUtilValidateNumberRange(timeoutEdtBox.Text, 0, 65535)
end;  //*** end of EdtTimeoutChange ***


//***************************************************************************************
// NAME:           EdtTimeoutKeyPress
// PARAMETER:      Sender Source of the event.
//                 Key Key that was pressed.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when a key on one or the Timeout edit
//                 boxes was pressed.
//
//***************************************************************************************
procedure TSessionXcpForm.EdtTimeoutKeyPress(Sender: TObject; var Key: char);
begin
  // Validate the key to make sure it is a character that is part of a number.
  CustomUtilValidateKeyAsInt(Key);
end; //*** end of EdtTimeoutKeyPress ***


//***************************************************************************************
// NAME:           LoadConfig
// PARAMETER:      Config Configuration instance to load from.
// RETURN VALUE:   none
// DESCRIPTION:    Loads the configuration values from the specified instance and
//                 initializes the user interface accordingly.
//
//***************************************************************************************
procedure TSessionXcpForm.LoadConfig(Config: TSessionXcpConfig);
begin
  // Load configuration.
  FSessionXcpConfig.TimeoutT1 := Config.TimeoutT1;
  FSessionXcpConfig.TimeoutT3 := Config.TimeoutT3;
  FSessionXcpConfig.TimeoutT4 := Config.TimeoutT4;
  FSessionXcpConfig.TimeoutT5 := Config.TimeoutT5;
  FSessionXcpConfig.TimeoutT6 := Config.TimeoutT6;
  FSessionXcpConfig.TimeoutT7 := Config.TimeoutT7;
  FSessionXcpConfig.ConnectMode := Config.ConnectMode;
  FSessionXcpConfig.SeedKey := Config.SeedKey;
  // Initialize user interface.
  CmbConnectMode.ItemIndex := FSessionXcpConfig.ConnectMode;
  EdtSeedKey.Text := FSessionXcpConfig.SeedKey;
  EdtTimeoutT1.Text := IntToStr(FSessionXcpConfig.TimeoutT1);
  EdtTimeoutT3.Text := IntToStr(FSessionXcpConfig.TimeoutT3);
  EdtTimeoutT4.Text := IntToStr(FSessionXcpConfig.TimeoutT4);
  EdtTimeoutT5.Text := IntToStr(FSessionXcpConfig.TimeoutT5);
  EdtTimeoutT6.Text := IntToStr(FSessionXcpConfig.TimeoutT6);
  EdtTimeoutT7.Text := IntToStr(FSessionXcpConfig.TimeoutT7);
end; //*** end of LoadConfig ***


//***************************************************************************************
// NAME:           SaveConfig
// PARAMETER:      Config Configuration instance to save to.
// RETURN VALUE:   none
// DESCRIPTION:    Reads the configuration values from the user interface and stores them
//                 in the specified instance.
//
//***************************************************************************************
procedure TSessionXcpForm.SaveConfig(Config: TSessionXcpConfig);
begin
  // Start out with default configuration settings.
  FSessionXcpConfig.Defaults;
  // Read configuration from the user interface.
  FSessionXcpConfig.ConnectMode := CmbConnectMode.ItemIndex;
  FSessionXcpConfig.SeedKey := EdtSeedKey.Text;
  if EdtTimeoutT1.Text <> '' then
    FSessionXcpConfig.TimeoutT1 := StrToInt(EdtTimeoutT1.Text);
  if EdtTimeoutT3.Text <> '' then
    FSessionXcpConfig.TimeoutT3 := StrToInt(EdtTimeoutT3.Text);
  if EdtTimeoutT4.Text <> '' then
    FSessionXcpConfig.TimeoutT4 := StrToInt(EdtTimeoutT4.Text);
  if EdtTimeoutT5.Text <> '' then
    FSessionXcpConfig.TimeoutT5 := StrToInt(EdtTimeoutT5.Text);
  if EdtTimeoutT6.Text <> '' then
    FSessionXcpConfig.TimeoutT6 := StrToInt(EdtTimeoutT6.Text);
  if EdtTimeoutT7.Text <> '' then
    FSessionXcpConfig.TimeoutT7 := StrToInt(EdtTimeoutT7.Text);
  // Store configuration.
  Config.TimeoutT1 := FSessionXcpConfig.TimeoutT1;
  Config.TimeoutT3 := FSessionXcpConfig.TimeoutT3;
  Config.TimeoutT4 := FSessionXcpConfig.TimeoutT4;
  Config.TimeoutT5 := FSessionXcpConfig.TimeoutT5;
  Config.TimeoutT6 := FSessionXcpConfig.TimeoutT6;
  Config.TimeoutT7 := FSessionXcpConfig.TimeoutT7;
  Config.ConnectMode := FSessionXcpConfig.ConnectMode;
  Config.SeedKey := FSessionXcpConfig.SeedKey;
end; //*** end of SaveConfig ***


end.
//******************************** end of sessionxcpdialog.pas **************************

