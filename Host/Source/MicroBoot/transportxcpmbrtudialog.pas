unit TransportXcpMbRtuDialog;
//***************************************************************************************
//  Description: Implements the XCP on Modbus RTU transport layer dialog.
//    File Name: transportxcpmbrtudialog.pas
//
//---------------------------------------------------------------------------------------
//                          C O P Y R I G H T
//---------------------------------------------------------------------------------------
//   Copyright (c) 2023 by Feaser    http://www.feaser.com    All rights reserved
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
  ConfigGroups;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  //------------------------------ TTransportXcpMbRtuForm -------------------------------

  { TTransportXcpMbRtuForm }

  TTransportXcpMbRtuForm = class(TForm)
    CmbParity: TComboBox;
    CmbDevice: TComboBox;
    CmbBaudrate: TComboBox;
    CmbDestinationAddress: TComboBox;
    CmbStopbits: TComboBox;
    LblBaudrate: TLabel;
    LblParity: TLabel;
    LblDevice: TLabel;
    LblDestinationAddress: TLabel;
    LblStopbits: TLabel;
    LlbCommunication: TLabel;
    procedure CmbParityChange(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private
    FTransportXcpMbRtuConfig: TTransportXcpMbRtuConfig;
  public
    procedure LoadConfig(Config: TTransportXcpMbRtuConfig);
    procedure SaveConfig(Config: TTransportXcpMbRtuConfig);
  end;


implementation

{$R *.lfm}

//---------------------------------------------------------------------------------------
//-------------------------------- TTransportXcpMbRtuForm -------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           FormCreate
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form constructor.
//
//***************************************************************************************
procedure TTransportXcpMbRtuForm.FormCreate(Sender: TObject);
var
  portIdx: Integer;
begin
  // Create configuration group instance.
  FTransportXcpMbRtuConfig := TTransportXcpMbRtuConfig.Create;
  // Populate the device combobox with platform specific items.
  CmbDevice.Items.Clear;
  {$IFDEF UNIX}
  for portIdx := 0 to 3 do
  begin
    CmbDevice.Items.Add('/dev/ttyUSB' + IntToStr(portIdx));
  end;
  for portIdx := 0 to 3 do
  begin
    CmbDevice.Items.Add('/dev/ttyACM' + IntToStr(portIdx));
  end;
  for portIdx := 0 to 7 do
  begin
    CmbDevice.Items.Add('/dev/ttyS' + IntToStr(portIdx));
  end;
  {$ELSE}
  for portIdx := 1 to 16 do
  begin
    CmbDevice.Items.Add('COM' + IntToStr(portIdx));
  end;
  {$ENDIF}
  CmbDevice.ItemIndex := 0;
  CmbParity.ItemIndex := 0;
  CmbStopbits.ItemIndex := 0;
  CmbDestinationAddress.ItemIndex := 0;
end; //*** end of FormCreate ***


//***************************************************************************************
// NAME:           CmbParityChange
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the combobox selection changes.
//
//***************************************************************************************
procedure TTransportXcpMbRtuForm.CmbParityChange(Sender: TObject);
begin
  // No parity selected?
  if CmbParity.ItemIndex = 0 then
  begin
    // Switch stop bits to 2 because Modbus RTU requires to stopbits without parity.
    CmbStopbits.ItemIndex := 1;
  end
  else
  begin
    // Switch stop bits to 1 because Modbus RTU requires this with parity.
    CmbStopbits.ItemIndex := 0;
  end;
end; //*** end of CmbParityChange ***


//***************************************************************************************
// NAME:           FormDestroy
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form destructor.
//
//***************************************************************************************
procedure TTransportXcpMbRtuForm.FormDestroy(Sender: TObject);
begin
  // Release the configuration group instance.
  FTransportXcpMbRtuConfig.Free;
end; //*** end of FormDestroy ***


//***************************************************************************************
// NAME:           LoadConfig
// PARAMETER:      Config Configuration instance to load from.
// RETURN VALUE:   none
// DESCRIPTION:    Loads the configuration values from the specified instance and
//                 initializes the user interface accordingly.
//
//***************************************************************************************
procedure TTransportXcpMbRtuForm.LoadConfig(Config: TTransportXcpMbRtuConfig);
var
  baudIdx: Integer;
begin
  // Load configuration.
  FTransportXcpMbRtuConfig.Device := Config.Device;
  FTransportXcpMbRtuConfig.Parity := Config.Parity;
  FTransportXcpMbRtuConfig.Stopbits := Config.Stopbits;
  FTransportXcpMbRtuConfig.Baudrate := Config.Baudrate;
  FTransportXcpMbRtuConfig.DestinationAddress := Config.DestinationAddress;
  // Initialize user interface.
  if FTransportXcpMbRtuConfig.Device = '' then
    CmbDevice.Text := CmbDevice.Items[0]
  else
    CmbDevice.Text := FTransportXcpMbRtuConfig.Device;
  CmbBaudrate.ItemIndex := 0;
  for baudIdx := 0 to (CmbDevice.Items.Count - 1) do
  begin
    // Is this combobox entry the currently configured value?
    if StrToInt(CmbBaudrate.Items[baudIdx]) = FTransportXcpMbRtuConfig.Baudrate then
    begin
      // Select this item in the combobox.
      CmbBaudrate.ItemIndex := baudIdx;
      // Match found so no need to continue looping.
      Break;
    end;
  end;
  if FTransportXcpMbRtuConfig.Parity <= 2 then
  begin
    CmbParity.ItemIndex := FTransportXcpMbRtuConfig.Parity;
  end
  else
  begin
    CmbParity.ItemIndex := 0;
  end;
  if (FTransportXcpMbRtuConfig.Stopbits > 0) and
     (FTransportXcpMbRtuConfig.Stopbits <= 2) then
  begin
    CmbStopbits.ItemIndex := FTransportXcpMbRtuConfig.Stopbits - 1;
  end
  else
  begin
    CmbStopbits.ItemIndex := 0;
  end;
  if (FTransportXcpMbRtuConfig.DestinationAddress > 0) and
     (FTransportXcpMbRtuConfig.DestinationAddress <= 247) then
  begin
    CmbDestinationAddress.ItemIndex := FTransportXcpMbRtuConfig.DestinationAddress - 1;
  end
  else
  begin
    CmbDestinationAddress.ItemIndex := 0;
  end;
end; //*** end of LoadConfig ***


//***************************************************************************************
// NAME:           SaveConfig
// PARAMETER:      Config Configuration instance to save to.
// RETURN VALUE:   none
// DESCRIPTION:    Reads the configuration values from the user interface and stores them
//                 in the specified instance.
//
//***************************************************************************************
procedure TTransportXcpMbRtuForm.SaveConfig(Config: TTransportXcpMbRtuConfig);
begin
  // Start out with default configuration settings.
  FTransportXcpMbRtuConfig.Defaults;
  // Read configuration from the user interface.
  FTransportXcpMbRtuConfig.Device := CmbDevice.Text;
  FTransportXcpMbRtuConfig.Baudrate := StrToInt(CmbBaudrate.Text);
  FTransportXcpMbRtuConfig.Parity := CmbParity.ItemIndex;
  FTransportXcpMbRtuConfig.Stopbits := CmbStopbits.ItemIndex + 1;
  FTransportXcpMbRtuConfig.DestinationAddress := CmbDestinationAddress.ItemIndex + 1;
  // Store configuration.
  Config.Device := FTransportXcpMbRtuConfig.Device;
  Config.Baudrate := FTransportXcpMbRtuConfig.Baudrate;
  Config.Parity := FTransportXcpMbRtuConfig.Parity;
  Config.Stopbits := FTransportXcpMbRtuConfig.Stopbits;
  Config.DestinationAddress := FTransportXcpMbRtuConfig.DestinationAddress;
end; //*** end of SaveConfig ***


end.
//******************************** end of transportxcpmbrtudialog.pas *******************

