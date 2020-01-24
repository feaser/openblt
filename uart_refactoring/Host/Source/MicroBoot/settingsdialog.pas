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
  StdCtrls, ComCtrls, CurrentConfig, ConfigGroups, SessionXcpDialog,
  TransportXcpRs232Dialog, TransportXcpCanDialog, TransportXcpUsbDialog,
  TransportXcpTcpIpDialog, MiscellaneousDialog;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  //------------------------------ TSettingsForm ------------------------------------------
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
    procedure CmbInterfaceChange(Sender: TObject);
    procedure CmbProtocolChange(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure FormKeyPress(Sender: TObject; var Key: char);
  private
    FCurrentConfig: TCurrentConfig;
    FSessionConfig: TSessionConfig;
    FTransportConfig: TTransportConfig;
    FSessionXcpForm: TSessionXcpForm;
    FTransportXcpRs232Form: TTransportXcpRs232Form;
    FTransportXcpCanForm: TTransportXcpCanForm;
    FTransportXcpUsbForm: TTransportXcpUsbForm;
    FTransportXcpTcpIpForm: TTransportXcpTcpIpForm;
    FMiscellaneousForm: TMiscellaneousForm;
    procedure UpdateSessionPanel;
    procedure UpdateCommunicationPanel;
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
var
  sessionConfig: TSessionConfig;
  transportConfig: TTransportConfig;
  miscellaneousConfig: TMiscellaneousConfig;
  sessionXcpConfig: TSessionXcpConfig;
  transportXcpRs232Config: TTransportXcpRs232Config;
  transportXcpCanConfig: TTransportXcpCanConfig;
  transportXcpUsbConfig: TTransportXcpUsbConfig;
  transportXcpTcpIpConfig: TTransportXcpTcpIpConfig;
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
  PageCtrlSettings.ActivePage := TabCommunicationInterface;
  // Set fixed space between labels and the related controls.
  CmbProtocol.Left := LblProtocol.Left + LblProtocol.Width + 8;
  CmbInterface.Left := LblInterface.Left + LblInterface.Width + 8;
  // Construct the session configuration instance and initialize its settings.
  FSessionConfig := TSessionConfig.Create;
  sessionConfig := FCurrentConfig.Groups[TSessionConfig.GROUP_NAME] as TSessionConfig;
  FSessionConfig.Session := sessionConfig.Session;
  // Construct the transport configuration instance and initialize its settings.
  FTransportConfig := TTransportConfig.Create;
  transportConfig := FCurrentConfig.Groups[TTransportConfig.GROUP_NAME]
                     as TTransportConfig;
  FTransportConfig.Transport := transportConfig.Transport;
  // Construct all embeddable dialogs and initialize their configuration settings.
  // Miscellaneous settings embeddable dialog.
  FMiscellaneousForm := TMiscellaneousForm.Create(Self);
  FMiscellaneousForm.Parent := PnlMiscellaneousBody;
  FMiscellaneousForm.BorderStyle := bsNone;
  FMiscellaneousForm.Align := alClient;
  miscellaneousConfig := FCurrentConfig.Groups[TMiscellaneousConfig.GROUP_NAME]
                         as TMiscellaneousConfig;
  FMiscellaneousForm.LoadConfig(miscellaneousConfig);
  // XCP session embeddable dialog.
  FSessionXcpForm := TSessionXcpForm.Create(Self);
  FSessionXcpForm.Parent := PnlSessionBody;
  FSessionXcpForm.BorderStyle := bsNone;
  FSessionXcpForm.Align := alClient;
  sessionXcpConfig := FCurrentConfig.Groups[TSessionXcpConfig.GROUP_NAME]
                      as TSessionXcpConfig;
  FSessionXcpForm.LoadConfig(sessionXcpConfig);
  // XCP on RS232 transport layer embeddable dialog.
  FTransportXcpRs232Form := TTransportXcpRs232Form.Create(Self);
  FTransportXcpRs232Form.Parent := PnlCommunicationBody;
  FTransportXcpRs232Form.BorderStyle := bsNone;
  FTransportXcpRs232Form.Align := alClient;
  transportXcpRs232Config := FCurrentConfig.Groups[TTransportXcpRs232Config.GROUP_NAME]
                             as TTransportXcpRs232Config;
  FTransportXcpRs232Form.LoadConfig(transportXcpRs232Config);
  // XCP on CAN transport layer embeddable dialog.
  FTransportXcpCanForm := TTransportXcpCanForm.Create(Self);
  FTransportXcpCanForm.Parent := PnlCommunicationBody;
  FTransportXcpCanForm.BorderStyle := bsNone;
  FTransportXcpCanForm.Align := alClient;
  transportXcpCanConfig := FCurrentConfig.Groups[TTransportXcpCanConfig.GROUP_NAME]
                           as TTransportXcpCanConfig;
  FTransportXcpCanForm.LoadConfig(transportXcpCanConfig);
  // XCP on USB transport layer embeddable dialog.
  FTransportXcpUsbForm := TTransportXcpUsbForm.Create(Self);
  FTransportXcpUsbForm.Parent := PnlCommunicationBody;
  FTransportXcpUsbForm.BorderStyle := bsNone;
  FTransportXcpUsbForm.Align := alClient;
  transportXcpUsbConfig := FCurrentConfig.Groups[TTransportXcpUsbConfig.GROUP_NAME]
                           as TTransportXcpUsbConfig;
  FTransportXcpUsbForm.LoadConfig(transportXcpUsbConfig);
  // XCP on TCP/IP transport layer embeddable dialog.
  FTransportXcpTcpIpForm := TTransportXcpTcpIpForm.Create(Self);
  FTransportXcpTcpIpForm.Parent := PnlCommunicationBody;
  FTransportXcpTcpIpForm.BorderStyle := bsNone;
  FTransportXcpTcpIpForm.Align := alClient;
  transportXcpTcpIpConfig := FCurrentConfig.Groups[TTransportXcpTcpIpConfig.GROUP_NAME]
                             as TTransportXcpTcpIpConfig;
  FTransportXcpTcpIpForm.LoadConfig(transportXcpTcpIpConfig);
  // Embed the miscellaneous setting dialog.
  FMiscellaneousForm.Show;
  // Embed the correct session dialog based on the currently configured session.
  UpdateSessionPanel;
  // Embed the correct transport dialog based on the currently configured transport
  // layer.
  UpdateCommunicationPanel;
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
  // Release the configuration instances.
  FTransportConfig.Free;
  FSessionConfig.Free;
end; //*** end of FormDestroy ***


//***************************************************************************************
// NAME:           FormKeyPress
// PARAMETER:      Sender Signal source.
//                 Key The key's character code that was pressed
// RETURN VALUE:   None.
// DESCRIPTION:    Called when a key is pressed.
//
//***************************************************************************************
procedure TSettingsForm.FormKeyPress(Sender: TObject; var Key: char);
begin
  // Was the escape key pressed?
  if Key = Char(27) then
  begin
    // Simulate button cancel click.
    BtnCancelClick(Sender)
  end
  // Was the enter key pressed?
  else if Key = Char(13) then
  begin
    if ActiveControl.Name = 'BtnCancel' then
      // Simulate button cancel click.
      BtnCancelClick(Sender)
    else
      // Simulate button ok click.
      BtnOKClick(Sender);
  end;
end; //*** end of FormKeyPress ***


//***************************************************************************************
// NAME:           BtnOkClick
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the button is clicked.
//
//***************************************************************************************
procedure TSettingsForm.BtnOkClick(Sender: TObject);
var
  sessionConfig: TSessionConfig;
  sessionXcpConfig: TSessionXcpConfig;
  miscellaneousConfig: TMiscellaneousConfig;
  transportConfig: TTransportConfig;
  transportXcpRs232Config: TTransportXcpRs232Config;
  transportXcpCanConfig: TTransportXcpCanConfig;
  transportXcpUsbConfig: TTransportXcpUsbConfig;
  transportXcpTcpIpConfig: TTransportXcpTcpIpConfig;
begin
  // Update the session settings in current config.
  sessionConfig := FCurrentConfig.Groups[TSessionConfig.GROUP_NAME] as TSessionConfig;
  sessionConfig.Session := FSessionConfig.Session;
  // Update the XCP session settings in current config.
  sessionXcpConfig := FCurrentConfig.Groups[TSessionXcpConfig.GROUP_NAME]
                      as TSessionXcpConfig;
  FSessionXcpForm.SaveConfig(sessionXcpConfig);
  // Update the transport layer settings in current config.
  transportConfig := FCurrentConfig.Groups[TTransportConfig.GROUP_NAME]
                     as TTransportConfig;
  transportConfig.Transport := FTransportConfig.Transport;
  // Update the miscellanouse settings in the current config.
  miscellaneousConfig := FCurrentConfig.Groups[TMiscellaneousConfig.GROUP_NAME]
                         as TMiscellaneousConfig;
  FMiscellaneousForm.SaveConfig(miscellaneousConfig);
  // Update the XCP on RS232 transport layer settings in current config.
  transportXcpRs232Config := FCurrentConfig.Groups[TTransportXcpRs232Config.GROUP_NAME]
                             as TTransportXcpRs232Config;
  FTransportXcpRs232Form.SaveConfig(transportXcpRs232Config);
  // Update the XCP on CAN transport layer settings in current config.
  transportXcpCanConfig := FCurrentConfig.Groups[TTransportXcpCanConfig.GROUP_NAME]
                           as TTransportXcpCanConfig;
  FTransportXcpCanForm.SaveConfig(transportXcpCanConfig);
  // Update the XCP on USB transport layer settings in current config.
  transportXcpUsbConfig := FCurrentConfig.Groups[TTransportXcpUsbConfig.GROUP_NAME]
                           as TTransportXcpUsbConfig;
  FTransportXcpUsbForm.SaveConfig(transportXcpUsbConfig);
  // Update the XCP on TCP/IP transport layer settings in current config.
  transportXcpTcpIpConfig := FCurrentConfig.Groups[TTransportXcpTcpIpConfig.GROUP_NAME]
                             as TTransportXcpTcpIpConfig;
  FTransportXcpTcpIpForm.SaveConfig(transportXcpTcpIpConfig);
  // Set the modal result value, which also closes the dialog.
  ModalResult := mrOK;
end; //*** end of BtnOkClick ***


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
// NAME:           CmbInterfaceChange
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the selected entry in the combobox
//                 changed.
//
//***************************************************************************************
procedure TSettingsForm.CmbInterfaceChange(Sender: TObject);
begin
  // Configure the correct communication interface based on the selected combobox entry.
  if CmbInterface.Text = 'XCP on RS232' then
  begin
    FTransportConfig.Transport := 'xcp_rs232';
  end
  else if CmbInterface.Text = 'XCP on CAN' then
  begin
    FTransportConfig.Transport := 'xcp_can';
  end
  else if CmbInterface.Text = 'XCP on USB' then
  begin
    FTransportConfig.Transport := 'xcp_usb';
  end
  else if CmbInterface.Text = 'XCP on TCP/IP' then
  begin
    FTransportConfig.Transport := 'xcp_net';
  end
  // Unknown protocol session
  else
  begin
    Assert(False, 'Unknown communication interface encountered in the combobox.');
  end;
  // Embed the correct transport layer dialog based on the currently configured transport
  // layer
  UpdateCommunicationPanel;
end; //*** end of CmbInterfaceChange ***


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
// NAME:           UpdateCommunicationPanel
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Embeds the correct communication interface configuration dialog.
//
//***************************************************************************************
procedure TSettingsForm.UpdateCommunicationPanel;
begin
  // First hide all communication interface related forms.
  FTransportXcpRs232Form.Hide;
  FTransportXcpCanForm.Hide;
  FTransportXcpUsbForm.Hide;
  FTransportXcpTcpIpForm.Hide;
  // Show the correct communication interface form.
  if FTransportConfig.Transport = 'xcp_rs232' then
  begin
    CmbInterface.ItemIndex := 0;
    FTransportXcpRs232Form.Show;
  end
  else if FTransportConfig.Transport = 'xcp_can' then
  begin
    CmbInterface.ItemIndex := 1;
    FTransportXcpCanForm.Show;
  end
  else if FTransportConfig.Transport = 'xcp_usb' then
  begin
    CmbInterface.ItemIndex := 2;
    FTransportXcpUsbForm.Show;
  end
  else if FTransportConfig.Transport = 'xcp_net' then
  begin
    CmbInterface.ItemIndex := 3;
    FTransportXcpTcpIpForm.Show;
  end
  // Default configuration
  else
  begin
    CmbInterface.ItemIndex := 0;
    FTransportXcpRs232Form.Show;
  end;
end; //*** end of UpdateCommunicationPanel ***


//***************************************************************************************
// NAME:           Create
// PARAMETER:      TheOwner Owner of the settings form instance.
//                 CurrentConfig Current configuration instance.
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

