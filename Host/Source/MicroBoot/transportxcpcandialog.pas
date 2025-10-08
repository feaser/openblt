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

  { TTransportXcpCanForm }

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
    procedure CmbDeviceSelect(Sender: TObject);
    procedure EdtCanIdChange(Sender: TObject);
    procedure EdtCanIdKeyPress(Sender: TObject; var Key: char);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private
    FTransportXcpCanConfig: TTransportXcpCanConfig;
    FHwCanFdSupported: Boolean;
    procedure PopulateBaudrateCombo(CanFdSupported: Boolean);
  public
    procedure LoadConfig(Config: TTransportXcpCanConfig);
    procedure SaveConfig(Config: TTransportXcpCanConfig);
  end;


implementation

{$R *.lfm}

//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  TCanBaudrateRec = record
    DisplayStr: string;
    Baudrate: Integer;
    BrsBaudrate: Integer;
  end;

  {$IFNDEF UNIX}
  TCanInterfaceRec = record
    DisplayStr: string;
    InterfaceStr: string;
    CanFdSupported: Boolean;
  end;
  {$ENDIF}

const
  CanBaudrateRecArray: array[0..24] of TCanBaudrateRec =
  (
    (DisplayStr: '1 MBit/sec';   Baudrate: 1000000; BrsBaudrate: 0),
    (DisplayStr: '800 kBit/sec'; Baudrate:  800000; BrsBaudrate: 0),
    (DisplayStr: '500 kBit/sec'; Baudrate:  500000; BrsBaudrate: 0),
    (DisplayStr: '250 kBit/sec'; Baudrate:  250000; BrsBaudrate: 0),
    (DisplayStr: '125 kBit/sec'; Baudrate:  125000; BrsBaudrate: 0),
    (DisplayStr: '100 kBit/sec'; Baudrate:  100000; BrsBaudrate: 0),
    (DisplayStr: '50 kBit/sec';  Baudrate:   50000; BrsBaudrate: 0),
    (DisplayStr: '20 kBit/sec';  Baudrate:   20000; BrsBaudrate: 0),
    (DisplayStr: '10 kBit/sec';  Baudrate:   10000; BrsBaudrate: 0),
    // CAN FD entries.
    (DisplayStr: '[CAN FD] 1 MBit/sec - data 8 MBit/sec';      Baudrate: 1000000; BrsBaudrate: 8000000),
    (DisplayStr: '[CAN FD] 1 MBit/sec - data 5 MBit/sec';      Baudrate: 1000000; BrsBaudrate: 5000000),
    (DisplayStr: '[CAN FD] 1 MBit/sec - data 4 MBit/sec';      Baudrate: 1000000; BrsBaudrate: 4000000),
    (DisplayStr: '[CAN FD] 1 MBit/sec - data 2 MBit/sec';      Baudrate: 1000000; BrsBaudrate: 2000000),
    (DisplayStr: '[CAN FD] 1 MBit/sec'; { no BRS }             Baudrate: 1000000; BrsBaudrate: 1000000),
    (DisplayStr: '[CAN FD] 500 kBit/sec - data 8 MBit/sec';    Baudrate:  500000; BrsBaudrate: 8000000),
    (DisplayStr: '[CAN FD] 500 kBit/sec - data 5 MBit/sec';    Baudrate:  500000; BrsBaudrate: 5000000),
    (DisplayStr: '[CAN FD] 500 kBit/sec - data 4 MBit/sec';    Baudrate:  500000; BrsBaudrate: 4000000),
    (DisplayStr: '[CAN FD] 500 kBit/sec - data 2 MBit/sec';    Baudrate:  500000; BrsBaudrate: 2000000),
    (DisplayStr: '[CAN FD] 500 kBit/sec - data 1 MBit/sec';    Baudrate:  500000; BrsBaudrate: 1000000),
    (DisplayStr: '[CAN FD] 500 kBit/sec'; { no BRS }           Baudrate:  500000; BrsBaudrate:  500000),
    (DisplayStr: '[CAN FD] 250 kBit/sec - data 4 MBit/sec';    Baudrate:  250000; BrsBaudrate: 4000000),
    (DisplayStr: '[CAN FD] 250 kBit/sec - data 2 MBit/sec';    Baudrate:  250000; BrsBaudrate: 2000000),
    (DisplayStr: '[CAN FD] 250 kBit/sec - data 1 MBit/sec';    Baudrate:  250000; BrsBaudrate: 1000000),
    (DisplayStr: '[CAN FD] 250 kBit/sec - data 500 kBit/sec';  Baudrate:  250000; BrsBaudrate:  500000),
    (DisplayStr: '[CAN FD] 250 kBit/sec'; { no BRS }           Baudrate:  250000; BrsBaudrate:  250000)
  );

  {$IFNDEF UNIX}
  CanInterfaceRecArray: array[0..4] of TCanInterfaceRec =
  (
    (DisplayStr: 'Peak System PCAN-USB'; InterfaceStr: 'peak_pcanusb';    CanFdSupported: False),
    (DisplayStr: 'Kvaser CanLib Driver'; InterfaceStr: 'kvaser_canlib';   CanFdSupported: False),
    (DisplayStr: 'Lawicel CANUSB';       InterfaceStr: 'lawicel_canusb';  CanFdSupported: False),
    (DisplayStr: 'Vector XL Driver';     InterfaceStr: 'vector_xldriver'; CanFdSupported: True),
    (DisplayStr: 'Ixxat VCI Driver';     InterfaceStr: 'ixxat_vcidriver'; CanFdSupported: True)
  );
  {$ENDIF}


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
var
  idx: Integer;
begin
  // Create configuration group instance.
  FTransportXcpCanConfig := TTransportXcpCanConfig.Create;
  // Assume the hardware does not support CAN FD by default.
  FHwCanFdSupported := False;
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
  {$ELSE}
  CmbDevice.Items.Clear;
  for idx := 0 to (Length(CanInterfaceRecArray) - 1) do
  begin
    CmbDevice.Items.Add(CanInterfaceRecArray[idx].DisplayStr);
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
// NAME:           CmbDeviceSelect
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when an item was selected from the
//                 combobox list.
//
//***************************************************************************************
procedure TTransportXcpCanForm.CmbDeviceSelect(Sender: TObject);
{$IFNDEF UNIX}
var
  HwCanFdSupportedPrev: Boolean;
  BaudrateFound: Boolean;
  BaudrateItem: Integer;
{$ENDIF}
begin
  {$IFNDEF UNIX}
  // Backup the current state of the hardware defined CAN FD supported flag.
  HwCanFdSupportedPrev := FHwCanFdSupported;
  // Update the flag based on the newly selected item.
  FHwCanFdSupported := CanInterfaceRecArray[CmbDevice.ItemIndex].CanFdSupported;
  // Did the CAN FD support change?
  if FHwCanFdSupported <> HwCanFdSupportedPrev then
  begin
    // Re-populare the baudrate list because more or less entires might now be needed.
    PopulateBaudrateCombo(FHwCanFdSupported);
    // Make sure the currently configured baudrate is still possible. If not, update it.
    // First sanitize it in case no CAN FD is supported.
    if not FHwCanFdSupported then
    begin
      FTransportXcpCanConfig.BrsBaudrate := 0;
    end;
    // Init flag.
    BaudrateFound := False;
    // Loop through the baudrates array to try and find the index that matches the
    // configuration. This index is the same as the combobox item to select.
    for BaudrateItem := 0 to (Length(CanBaudrateRecArray) - 1) do
    begin
      if (FTransportXcpCanConfig.Baudrate = CanBaudrateRecArray[BaudrateItem].Baudrate) and
         (FTransportXcpCanConfig.BrsBaudrate = CanBaudrateRecArray[BaudrateItem].BrsBaudrate) then
      begin
        // Found the one to select.
        CmbBaudrate.ItemIndex := BaudrateItem;
        // All done. set the flag and stop the loop.
        BaudrateFound := True;
        Break;
      end;
    end;
    // Could the baudrate selection not be restored?
    if not BaudrateFound then
    begin
      // Set the default 500 kbits/sec baudrate configuration.
      CmbBaudrate.ItemIndex := 2;
    end;
  end;
  {$ENDIF}
end; //*** end of CmbDeviceSelect ***


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
var
  baudrateItem: Integer;
  {$IFNDEF UNIX}
  deviceItem: Integer;
  {$ENDIF}
begin
  // Load configuration.
  FTransportXcpCanConfig.Device := Config.Device;
  FTransportXcpCanConfig.Channel := Config.Channel;
  FTransportXcpCanConfig.Baudrate := Config.Baudrate;
  FTransportXcpCanConfig.TransmitId := Config.TransmitId;
  FTransportXcpCanConfig.ReceiveId := Config.ReceiveId;
  FTransportXcpCanConfig.ExtendedId := Config.ExtendedId;
  FTransportXcpCanConfig.BrsBaudrate := Config.BrsBaudrate;
  // Initialize user interface.
  {$IFDEF UNIX}
  FHwCanFdSupported := True;
  if FTransportXcpCanConfig.Device = '' then
  begin
    CmbDevice.Text := CmbDevice.Items[0];
  end
  else
  begin
    CmbDevice.Text := FTransportXcpCanConfig.Device;
  end;
  {$ELSE}
  FHwCanFdSupported := False;
  // Match CAN device to the correct item in the combobox. Default to Peak PCAN-USB.
  CmbDevice.ItemIndex := 0;
  // Loop through the interfaces array to try and find the index that matches the
  // configuration. This index is the same as the combobox item to select.
  for deviceItem := 0 to (Length(CanInterfaceRecArray) - 1) do
  begin
    if FTransportXcpCanConfig.Device = CanInterfaceRecArray[deviceItem].InterfaceStr then
    begin
      // Found the one to select.
      CmbDevice.ItemIndex := deviceItem;
      // Update the CAN FD support flag.
      FHwCanFdSupported := CanInterfaceRecArray[deviceItem].CanFdSupported;
      // All done.
      Break;
    end;
  end;
  {$ENDIF}
  PopulateBaudrateCombo(FHwCanFdSupported);
  CmbChannel.ItemIndex := 0;
  if FTransportXcpCanConfig.Channel <= LongWord(CmbChannel.Items.Count) then
    CmbChannel.ItemIndex := FTransportXcpCanConfig.Channel;

  // First sanitize of the configuration to load.
  if not FHwCanFdSupported then
  begin
    FTransportXcpCanConfig.BrsBaudrate := 0;
  end;
  // Set the default 500 kbits/sec baudrate configuration.
  CmbBaudrate.ItemIndex := 2;
  // Loop through the baudrates array to try and find the index that matches the
  // configuration. This index is the same as the combobox item to select.
  for baudrateItem := 0 to (Length(CanBaudrateRecArray) - 1) do
  begin
    if (FTransportXcpCanConfig.Baudrate = CanBaudrateRecArray[baudrateItem].Baudrate) and
       (FTransportXcpCanConfig.BrsBaudrate = CanBaudrateRecArray[baudrateItem].BrsBaudrate) then
    begin
      // Found the one to select.
      CmbBaudrate.ItemIndex := baudrateItem;
      // All done.
      Break;
    end;
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
  // set the CAN device configuration.
  FTransportXcpCanConfig.Device := CanInterfaceRecArray[0].InterfaceStr;
  if CmbDevice.ItemIndex < Length(CanInterfaceRecArray) then
  begin
    FTransportXcpCanConfig.Device := CanInterfaceRecArray[CmbDevice.ItemIndex].InterfaceStr;
  end;
  {$ENDIF}
  FTransportXcpCanConfig.Channel := CmbChannel.ItemIndex;
  // Set the baudrate configuration. Default to 500 kbits/sec.
  FTransportXcpCanConfig.Baudrate := CanBaudrateRecArray[2].Baudrate;
  FTransportXcpCanConfig.BrsBaudrate := 0;
  if CmbBaudrate.ItemIndex < Length(CanBaudrateRecArray) then
  begin
    FTransportXcpCanConfig.Baudrate := CanBaudrateRecArray[CmbBaudrate.ItemIndex].Baudrate;
    FTransportXcpCanConfig.BrsBaudrate := CanBaudrateRecArray[CmbBaudrate.ItemIndex].BrsBaudrate;
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
  Config.BrsBaudrate := FTransportXcpCanConfig.BrsBaudrate;
end; //*** end of SaveConfig ***


//***************************************************************************************
// NAME:           PopulateBaudrateCombo
// PARAMETER:      CanFdSupported True if the CAN FD is supported, false for CAN classic.
// RETURN VALUE:   none
// DESCRIPTION:    Initializes the contents of the baudrate selection combobox, taking
//                 into account if the selected hardware supports CAN FD.
//
//***************************************************************************************
procedure TTransportXcpCanForm.PopulateBaudrateCombo(CanFdSupported: Boolean);
var
  maxEntries: Integer;
  itemIdx: Integer;
begin
  // Determine the number of entries to add to the combox box. Note that for CAN classic,
  // only the first 9 entries should be added.
  if CanFdSupported then
    maxEntries := Length(CanBaudrateRecArray)
  else
    maxEntries := 9;
  // Clean the baudrates combo box and add the entries one-by-one.
  CmbBaudrate.Items.Clear;
  for itemIdx := 0 to (maxEntries - 1) do
  begin
    CmbBaudrate.Items.Add(CanBaudrateRecArray[itemIdx].DisplayStr);
  end;
end; //*** end of PopulateBaudrateCombo ***

end.
//******************************** end of transportxcpcandialog.pas *********************

