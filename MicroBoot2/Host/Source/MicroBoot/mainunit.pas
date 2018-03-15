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
  ExtCtrls, CurrentConfig, ConfigGroups, OpenBlt, SettingsDialog;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  //------------------------------ TMainForm ----------------------------------------------
  TMainForm = class(TForm)
    BtnExit: TButton;
    BtnSettings: TButton;
    LblLibOpenBltVersion: TLabel;
    PnlFooterButtons: TPanel;
    PnlFooter: TPanel;
    PnlBody: TPanel;
    procedure BtnExitClick(Sender: TObject);
    procedure BtnSettingsClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private
    FCurrentConfig: TCurrentConfig;
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
  FCurrentConfig.AddGroup(TSessionConfig.Create);
  FCurrentConfig.AddGroup(TSessionXcpConfig.Create);
  // Load the program's configuration from the configuration file.
  FCurrentConfig.LoadFromFile;
  // Read and show the LibOpenBLT version in a label.
  LblLibOpenBltVersion.Caption := 'LibOpenBLT version: ' + BltVersionGetString;
  // Set main window configuration settings.
  mainWindowConfig := FCurrentConfig.Groups[TMainWindowConfig.GROUP_NAME]
                      as TMainWindowConfig;
  MainForm.Width := mainWindowConfig.Width;
  MainForm.Height := mainWindowConfig.Height;
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

end.
//******************************** end of mainunit.pas **********************************

