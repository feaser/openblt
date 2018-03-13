unit SettingsDialog;
//***************************************************************************************
//  Description: Implements the settings dialog for configuring MicroBoot.
//    File Name: settingsdialog.pas
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
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, ExtCtrls,
  StdCtrls, ComCtrls, CurrentConfig, ConfigGroups, SessionXcpDialog;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
//------------------------------ TSettingsForm ------------------------------------------
type

  { TSettingsForm }

  TSettingsForm = class(TForm)
    BtnCancel: TButton;
    BtnOk: TButton;
    CmbProtocol: TComboBox;
    CmbInterface: TComboBox;
    LblProtocol: TLabel;
    LblInterface: TLabel;
    PageCtrlSettings: TPageControl;
    PnlMiscellaneousBody: TPanel;
    PnlCommunicationBody: TPanel;
    PnlCommunicationTop: TPanel;
    PnlSessionBody: TPanel;
    PnlSessionTop: TPanel;
    PnlBody: TPanel;
    PnlFooterButtons: TPanel;
    PnlFooter: TPanel;
    TabSessionProtocol: TTabSheet;
    TabCommunicationInterface: TTabSheet;
    TabMiscellaneous: TTabSheet;
    procedure BtnCancelClick(Sender: TObject);
    procedure BtnOkClick(Sender: TObject);
    procedure CmbProtocolChange(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private
    FCurrentConfig: TCurrentConfig;
    FSessionConfig: TSessionConfig;
    FSessionXcpForm: TSessionXcpForm;
    procedure UpdateSessionPanel;
  public
    constructor Create(TheOwner: TComponent; CurrentConfig: TCurrentConfig); reintroduce;
  end;


implementation

{$R *.lfm}

//---------------------------------------------------------------------------------------
//-------------------------------- TSettingsForm ----------------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           FormCreate
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form constructor.
//
//***************************************************************************************
procedure TSettingsForm.FormCreate(Sender: TObject);
begin
  // Clear panel captions as these are only needed as hint during design time.
  PnlBody.Caption := '';
  PnlFooter.Caption := '';
  PnlFooterButtons.Caption := '';
  PnlSessionTop.Caption := '';
  PnlSessionBody.Caption := '';
  PnlCommunicationTop.Caption := '';
  PnlCommunicationBody.Caption := '';
  PnlMiscellaneousBody.Caption := '';
  // Set the active page on the page control.
  PageCtrlSettings.ActivePage := TabSessionProtocol;
  // Set fixed space between labels and the related controls.
  CmbProtocol.Left := LblProtocol.Left + LblProtocol.Width + 8;
  CmbInterface.Left := LblInterface.Left + LblInterface.Width + 8;
  // Construct the session configuration instance and initialize its settings.
  FSessionConfig := TSessionConfig.Create;
  with FCurrentConfig.Groups[TSessionConfig.GROUP_NAME] as TSessionConfig do
  begin
    FSessionConfig.Session := Session;
  end;
  { TODO : Construct the transport configuration instance and initialize its settings. }
  // Construct all embeddable dialogs and initialize their configuration settings.
  FSessionXcpForm := TSessionXcpForm.Create(Self);
  FSessionXcpForm.Parent := PnlSessionBody;
  FSessionXcpForm.BorderStyle := bsNone;
  FSessionXcpForm.Align := alClient;
  with FCurrentConfig.Groups[TSessionXcpConfig.GROUP_NAME] as TSessionXcpConfig do
  begin
    FSessionXcpForm.Config.TimeoutT1 := TimeoutT1;
    FSessionXcpForm.Config.TimeoutT3 := TimeoutT3;
    FSessionXcpForm.Config.TimeoutT4 := TimeoutT4;
    FSessionXcpForm.Config.TimeoutT5 := TimeoutT5;
    FSessionXcpForm.Config.TimeoutT7 := TimeoutT7;
    FSessionXcpForm.Config.ConnectMode := ConnectMode;
    FSessionXcpForm.Config.SeedKey := SeedKey;
  end;
  // Embed the correct session dialog based on the currently configured session.
  UpdateSessionPanel;
  { TODO : Continue with constructing the transport settings embeddable dialogs and init their settings. }
end; //*** end of FormCreate ***


//***************************************************************************************
// NAME:           FormDestroy
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form destructor.
//
//***************************************************************************************
procedure TSettingsForm.FormDestroy(Sender: TObject);
begin
  // Release the session configuration instance.
  FSessionConfig.Free;
end; //*** end of FormDestroy ***


//***************************************************************************************
// NAME:           BtnOkClick
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the button is clicked.
//
//***************************************************************************************
procedure TSettingsForm.BtnOkClick(Sender: TObject);
begin
  // Update the session settings in current config.
  with FCurrentConfig.Groups[TSessionConfig.GROUP_NAME] as TSessionConfig do
  begin
    Session := FSessionConfig.Session;
  end;
  // Update the XCP session settings in current config.
  with FCurrentConfig.Groups[TSessionXcpConfig.GROUP_NAME] as TSessionXcpConfig do
  begin
    TimeoutT1 := FSessionXcpForm.Config.TimeoutT1;
    TimeoutT3 := FSessionXcpForm.Config.TimeoutT3;
    TimeoutT4 := FSessionXcpForm.Config.TimeoutT4;
    TimeoutT5 := FSessionXcpForm.Config.TimeoutT5;
    TimeoutT7 := FSessionXcpForm.Config.TimeoutT7;
    ConnectMode := FSessionXcpForm.Config.ConnectMode;
    SeedKey := FSessionXcpForm.Config.SeedKey;
  end;
  { TODO : Update the settings in FCurrentConfig based on the other configured settings. }
  // Set the modal result value, which also closes the dialog.
  ModalResult := mrOK;
end; //*** end of BtnOkClick ***


//***************************************************************************************
// NAME:           CmbProtocolChange
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the selected entry in the combobox
//                 changed.
//
//***************************************************************************************
procedure TSettingsForm.CmbProtocolChange(Sender: TObject);
begin
  // Configure the correct protocol session based on the selected combobox entry.
  if CmbProtocol.Text = 'XCP version 1.0' then
  begin
    FSessionConfig.Session := 'xcp';
  end
  // Unknown protocol session
  else
  begin
    Assert(False, 'Unknown session protocol encountered in the combobox.');
  end;
  // Embed the correct session dialog based on the currently configured session.
  UpdateSessionPanel;
end; //*** end of CmbProtocolChange ***


//***************************************************************************************
// NAME:           BtnCancelClick
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the button is clicked.
//
//***************************************************************************************
procedure TSettingsForm.BtnCancelClick(Sender: TObject);
begin
  // Set the modal result value, which also closes the dialog.
  ModalResult := mrCancel;
end; //*** end of BtnCancelClick ***


//***************************************************************************************
// NAME:           UpdateSessionPanel
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Embeds the correct protocol session configuration dialog.
//
//***************************************************************************************
procedure TSettingsForm.UpdateSessionPanel;
begin
  // First hide all protocol session related forms.
  FSessionXcpForm.Hide;
  // Show the correct protocol session form.
  if FSessionConfig.Session = 'xcp' then
  begin
    CmbProtocol.ItemIndex := 0;
    FSessionXcpForm.Show;
  end
  // Default configuration
  else
  begin
    CmbProtocol.ItemIndex := 0;
    FSessionXcpForm.Show;
  end;
end; //*** end of UpdateSessionPanel ***


//***************************************************************************************
// NAME:           Create
// PARAMETER:      CurrentConfig Current configuration instance.
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor.
//
//***************************************************************************************
constructor TSettingsForm.Create(TheOwner: TComponent; CurrentConfig: TCurrentConfig);
begin
  // Call the inherited constructor.
  inherited Create(TheOwner);
  // Check parameters.
  Assert(CurrentConfig <> nil, 'Current configuration instance cannot be null');
  // Store the configuration instance.
  FCurrentConfig := CurrentConfig;
end; //*** end of Create ***

end.
//******************************** end of settingsdialog.pas ****************************

