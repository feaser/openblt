unit TransportXcpTcpIpDialog;
//***************************************************************************************
//  Description: Implements the XCP on TCP/IP transport layer dialog.
//    File Name: transportxcptcpipdialog.pas
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
  ConfigGroups, CustomUtil;

//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  //------------------------------ TTransportXcpTcpIpForm -------------------------------
  TTransportXcpTcpIpForm = class(TForm)
    EdtPort: TEdit;
    EdtAddress: TEdit;
    LblPort: TLabel;
    LblAddress: TLabel;
    LblCommunication: TLabel;
    procedure EdtPortChange(Sender: TObject);
    procedure EdtPortKeyPress(Sender: TObject; var Key: char);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private
    FTransportXcpTcpIpConfig: TTransportXcpTcpIpConfig;
  public
    procedure LoadConfig(Config: TTransportXcpTcpIpConfig);
    procedure SaveConfig(Config: TTransportXcpTcpIpConfig);
  end;


implementation

{$R *.lfm}

//---------------------------------------------------------------------------------------
//-------------------------------- TTransportXcpTcpIpForm -------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           FormCreate
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form constructor.
//
//***************************************************************************************
procedure TTransportXcpTcpIpForm.FormCreate(Sender: TObject);
begin
  // Create configuration group instance.
  FTransportXcpTcpIpConfig := TTransportXcpTcpIpConfig.Create;
end; //*** end of FormCreate ***


//***************************************************************************************
// NAME:           EdtPortChange
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the contents in one of the Timeout
//                 edit boxes changed.
//
//***************************************************************************************
procedure TTransportXcpTcpIpForm.EdtPortChange(Sender: TObject);
var
  portEdtBox: TEdit;
begin
  // Make sure the event source is an instance of class TEdit.
  Assert(Sender.InheritsFrom(TEdit), 'Event is triggered by an invalid sender.');
  portEdtBox := Sender as TEdit;
  // Validate the edit box contents to make sure that it is a number within an allowed
  // range.
  if portEdtBox.Text <> '' then
    portEdtBox.Text := CustomUtilValidateNumberRange(portEdtBox.Text, 0, 65535)
end; //*** end of EdtPortChange ***


//***************************************************************************************
// NAME:           EdtPortKeyPress
// PARAMETER:      Sender Source of the event.
//                 Key Key that was pressed.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when a key on one or the Timeout edit
//                 boxes was pressed.
//
//***************************************************************************************
procedure TTransportXcpTcpIpForm.EdtPortKeyPress(Sender: TObject; var Key: char);
begin
  // Validate the key to make sure it is a character that is part of a number.
  CustomUtilValidateKeyAsInt(Key);
end; //*** end of EdtPortKeyPress ***


//***************************************************************************************
// NAME:           FormDestroy
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form destructor.
//
//***************************************************************************************
procedure TTransportXcpTcpIpForm.FormDestroy(Sender: TObject);
begin
  // Release the configuration group instance.
  FTransportXcpTcpIpConfig.Free;
end; //*** end of FormDestroy ***


//***************************************************************************************
// NAME:           LoadConfig
// PARAMETER:      Config Configuration instance to load from.
// RETURN VALUE:   none
// DESCRIPTION:    Loads the configuration values from the specified instance and
//                 initializes the user interface accordingly.
//
//***************************************************************************************
procedure TTransportXcpTcpIpForm.LoadConfig(Config: TTransportXcpTcpIpConfig);
begin
  // Load configuration.
  FTransportXcpTcpIpConfig.Address := Config.Address;
  FTransportXcpTcpIpConfig.Port := Config.Port;
  // Initialize user interface.
  if FTransportXcpTcpIpConfig.Address = '' then
    EdtAddress.Text := '192.168.178.23'
  else
    EdtAddress.Text := FTransportXcpTcpIpConfig.Address;
  EdtPort.Text := IntToStr(FTransportXcpTcpIpConfig.Port);
end; //*** end of LoadConfig ***


//***************************************************************************************
// NAME:           SaveConfig
// PARAMETER:      Config Configuration instance to save to.
// RETURN VALUE:   none
// DESCRIPTION:    Reads the configuration values from the user interface and stores them
//                 in the specified instance.
//
//***************************************************************************************
procedure TTransportXcpTcpIpForm.SaveConfig(Config: TTransportXcpTcpIpConfig);
begin
  // Start out with default configuration settings.
  FTransportXcpTcpIpConfig.Defaults;
  // Read configuration from the user interface.
  FTransportXcpTcpIpConfig.Address := EdtAddress.Text;
  FTransportXcpTcpIpConfig.Port := StrToInt(EdtPort.Text);
  // Store configuration.
  Config.Address := FTransportXcpTcpIpConfig.Address;
  Config.Port := FTransportXcpTcpIpConfig.Port;
end; //*** end of SaveConfig ***


end.
//******************************** end of transportxcptcpipdialog.pas *******************

