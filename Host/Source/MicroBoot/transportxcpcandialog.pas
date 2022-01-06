unit TransportXcpCanDialog;
//***************************************************************************************
//  Description: Implements the XCP on CAN transport layer dialog.
//    File Name: transportxcpcandialog.pas
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
  //------------------------------ TTransportXcpCanForm ---------------------------------
  TTransportXcpCanForm = class(TForm)
    CbxExtended: TCheckBox;
    CmbDevice: TComboBox;
    CmbChannel: TComboBox;
    CmbBaudrate: TComboBox;
    EdtReceiveId: TEdit;
    EdtTransmitId: TEdit;
    LblReceiveId: TLabel;
    LblTransmitId: TLabel;
    LblIdentifiers: TLabel;
    LblBaudrate: TLabel;
    LblChannel: TLabel;
    LblDevice: TLabel;
    LblCommunication: TLabel;
    procedure CbxExtendedChange(Sender: TObject);
    procedure EdtCanIdChange(Sender: TObject);
    procedure EdtCanIdKeyPress(Sender: TObject; var Key: char);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private
    FTransportXcpCanConfig: TTransportXcpCanConfig;
  public
    procedure LoadConfig(Config: TTransportXcpCanConfig);
    procedure SaveConfig(Config: TTransportXcpCanConfig);
  end;


implementation

{$R *.lfm}

//---------------------------------------------------------------------------------------
//-------------------------------- TTransportXcpCanForm ---------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           FormCreate
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form constructor.
//
//***************************************************************************************
procedure TTransportXcpCanForm.FormCreate(Sender: TObject);
{$IFDEF UNIX}
var
  idx: Integer;
{$ENDIF}
begin
  // Create configuration group instance.
  FTransportXcpCanConfig := TTransportXcpCanConfig.Create;
  {$IFDEF UNIX}
  // By default the device combobox is a dropdown list with the possible values that are
  // supported under Windows. When using a Unix-based OS it should contain different
  // entries and have a standard dropdown style, such that the user could manually enter
  // a device as well.
  CmbDevice.Style := csDropDown;
  CmbDevice.Items.Clear;
  for idx := 0 to 3 do
  begin
    CmbDevice.Items.Add('can' + IntToStr(idx));
  end;
  for idx := 0 to 3 do
  begin
    CmbDevice.Items.Add('slcan' + IntToStr(idx));
  end;
  CmbDevice.ItemIndex := 0;
  {$ENDIF}
end; //*** end of FormCreate ***


//***************************************************************************************
// NAME:           EdtCanIdChange
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the contents in one of the CAN
//                 identifier edit boxes changed.
//
//***************************************************************************************
procedure TTransportXcpCanForm.EdtCanIdChange(Sender: TObject);
var
  canIdEdtBox: TEdit;
  maxIdValue: Integer;
begin
  // Make sure the event source is an instance of class TEdit.
  Assert(Sender.InheritsFrom(TEdit), 'Event is triggered by an invalid sender.');
  canIdEdtBox := Sender as TEdit;
  // Validate the edit box contents to make sure that it is a number within an allowed
  // range.
  maxIdValue := $7FF;
  if CbxExtended.Checked then
    maxIdValue := $1FFFFFFF;
  if canIdEdtBox.Text <> '' then
    canIdEdtBox.Text := CustomUtilValidateNumberRange(canIdEdtBox.Text, 0, maxIdValue, True)
end; //*** end of EdtCanIdChange


//***************************************************************************************
// NAME:           CbxExtendedChange
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the state of the checkbox changed.
//
//***************************************************************************************
procedure TTransportXcpCanForm.CbxExtendedChange(Sender: TObject);
begin
  // If it change from 29-bit to 11-bit, the currently entered values of the CAN
  // identifiers might be to large. Validate and change where necessary.
  EdtCanIdChange(EdtTransmitId);
  EdtCanIdChange(EdtReceiveId);
end; //*** end of CbxExtendedChange ***


//***************************************************************************************
// NAME:           EdtCanIdKeyPress
// PARAMETER:      Sender Source of the event.
//                 Key Key that was pressed.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when a key on one or the CAN identifier
//                 edit boxes was pressed.
//
//***************************************************************************************
procedure TTransportXcpCanForm.EdtCanIdKeyPress(Sender: TObject; var Key: char);
begin
  // Validate the key to make sure it is a character that is part of a hexadecimal
  // number.
  CustomUtilValidateKeyAsHex(Key);
end; //*** end of EdtCanIdKeyPress ***


//***************************************************************************************
// NAME:           FormDestroy
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form destructor.
//
//***************************************************************************************
procedure TTransportXcpCanForm.FormDestroy(Sender: TObject);
begin
  // Release the configuration group instance.
  FTransportXcpCanConfig.Free;
end; //*** end of FormDestroy ***


//***************************************************************************************
// NAME:           LoadConfig
// PARAMETER:      Config Configuration instance to load from.
// RETURN VALUE:   none
// DESCRIPTION:    Loads the configuration values from the specified instance and
//                 initializes the user interface accordingly.
//
//***************************************************************************************
procedure TTransportXcpCanForm.LoadConfig(Config: TTransportXcpCanConfig);
begin
  // Load configuration.
  FTransportXcpCanConfig.Device := Config.Device;
  FTransportXcpCanConfig.Channel := Config.Channel;
  FTransportXcpCanConfig.Baudrate := Config.Baudrate;
  FTransportXcpCanConfig.TransmitId := Config.TransmitId;
  FTransportXcpCanConfig.ReceiveId := Config.ReceiveId;
  FTransportXcpCanConfig.ExtendedId := Config.ExtendedId;
  // Initialize user interface.
  {$IFDEF UNIX}
  if FTransportXcpCanConfig.Device = '' then
    CmbDevice.Text := CmbDevice.Items[0]
  else
    CmbDevice.Text := FTransportXcpCanConfig.Device;
  {$ELSE}
  // Match CAN device to the correct item in the combobox. Default to Peak PCAN-USB.
  CmbDevice.ItemIndex := 0;
  if FTransportXcpCanConfig.Device = 'kvaser_leaflight' then
    CmbDevice.ItemIndex := 1
  else if FTransportXcpCanConfig.Device = 'lawicel_canusb' then
    CmbDevice.ItemIndex := 2
  else if FTransportXcpCanConfig.Device = 'vector_xldriver' then
    CmbDevice.ItemIndex := 3
  else if FTransportXcpCanConfig.Device = 'ixxat_vcidriver' then
    CmbDevice.ItemIndex := 4;
  {$ENDIF}
  CmbChannel.ItemIndex := 0;
  if FTransportXcpCanConfig.Channel <= LongWord(CmbChannel.Items.Count) then
    CmbChannel.ItemIndex := FTransportXcpCanConfig.Channel;
  case FTransportXcpCanConfig.Baudrate of
    1000000: CmbBaudrate.ItemIndex := 0;
     800000: CmbBaudrate.ItemIndex := 1;
     500000: CmbBaudrate.ItemIndex := 2;
     250000: CmbBaudrate.ItemIndex := 3;
     125000: CmbBaudrate.ItemIndex := 4;
     100000: CmbBaudrate.ItemIndex := 5;
      50000: CmbBaudrate.ItemIndex := 6;
      20000: CmbBaudrate.ItemIndex := 7;
      10000: CmbBaudrate.ItemIndex := 8;
  else
    CmbBaudrate.ItemIndex := 2;
  end;
  if FTransportXcpCanConfig.ExtendedId = 0 then
    CbxExtended.Checked := False
  else
    CbxExtended.Checked := True;
  EdtTransmitId.Text := Format('%.x', [FTransportXcpCanConfig.TransmitId]);
  EdtReceiveId.Text := Format('%.x', [FTransportXcpCanConfig.ReceiveId]);
end; //*** end of LoadConfig ***


//***************************************************************************************
// NAME:           SaveConfig
// PARAMETER:      Config Configuration instance to save to.
// RETURN VALUE:   none
// DESCRIPTION:    Reads the configuration values from the user interface and stores them
//                 in the specified instance.
//
//***************************************************************************************
procedure TTransportXcpCanForm.SaveConfig(Config: TTransportXcpCanConfig);
begin
  // Start out with default configuration settings.
  FTransportXcpCanConfig.Defaults;
  // Read configuration from the user interface.
  {$IFDEF UNIX}
  FTransportXcpCanConfig.Device := CmbDevice.Text;
  {$ELSE}
  // Convert combobox item index to CAN device string. Default to Peak PCAN-USB.
  FTransportXcpCanConfig.Device := 'peak_pcanusb';
  if CmbDevice.ItemIndex = 1 then
    FTransportXcpCanConfig.Device := 'kvaser_leaflight'
  else if CmbDevice.ItemIndex = 2 then
    FTransportXcpCanConfig.Device := 'lawicel_canusb'
  else if CmbDevice.ItemIndex = 3 then
    FTransportXcpCanConfig.Device := 'vector_xldriver'
  else if CmbDevice.ItemIndex = 4 then
    FTransportXcpCanConfig.Device := 'ixxat_vcidriver';
  {$ENDIF}
  FTransportXcpCanConfig.Channel := CmbChannel.ItemIndex;
  case CmbBaudrate.ItemIndex of
    0: FTransportXcpCanConfig.Baudrate := 1000000;
    1: FTransportXcpCanConfig.Baudrate :=  800000;
    2: FTransportXcpCanConfig.Baudrate :=  500000;
    3: FTransportXcpCanConfig.Baudrate :=  250000;
    4: FTransportXcpCanConfig.Baudrate :=  125000;
    5: FTransportXcpCanConfig.Baudrate :=  100000;
    6: FTransportXcpCanConfig.Baudrate :=   50000;
    7: FTransportXcpCanConfig.Baudrate :=   20000;
    8: FTransportXcpCanConfig.Baudrate :=   10000;
  else
    FTransportXcpCanConfig.Baudrate :=  500000;
  end;
  if EdtTransmitId.Text <> '' then
    FTransportXcpCanConfig.TransmitId := StrToInt('$' + EdtTransmitId.Text);
  if EdtReceiveId.Text <> '' then
  FTransportXcpCanConfig.ReceiveId := StrToInt('$' + EdtReceiveId.Text);
  if CbxExtended.Checked then
    FTransportXcpCanConfig.ExtendedId := 1
  else
    FTransportXcpCanConfig.ExtendedId := 0;
  // Store configuration.
  Config.Device := FTransportXcpCanConfig.Device;
  Config.Channel := FTransportXcpCanConfig.Channel;
  Config.Baudrate := FTransportXcpCanConfig.Baudrate;
  Config.TransmitId := FTransportXcpCanConfig.TransmitId;
  Config.ReceiveId := FTransportXcpCanConfig.ReceiveId;
  Config.ExtendedId := FTransportXcpCanConfig.ExtendedId;
end; //*** end of SaveConfig ***


end.
//******************************** end of transportxcpcandialog.pas *********************

