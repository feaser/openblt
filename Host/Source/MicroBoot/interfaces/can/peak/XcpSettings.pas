unit XcpSettings;
//***************************************************************************************
//  Description: XCP settings interface for CAN
//    File Name: XcpSettings.pas
//
//---------------------------------------------------------------------------------------
//                          C O P Y R I G H T
//---------------------------------------------------------------------------------------
//   Copyright (c) 2011 by Feaser    http://www.feaser.com    All rights reserved
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
interface

//***************************************************************************************
// Includes
//***************************************************************************************
uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, ComCtrls, ExtCtrls, IniFiles, Vcl.Imaging.pngimage;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  TXcpSettingsForm = class(TForm)
    pnlFooter: TPanel;
    btnOK: TButton;
    btnCancel: TButton;
    pageControl: TPageControl;
    tabXcp: TTabSheet;
    tabCan: TTabSheet;
    iconCan: TImage;
    lblCan: TLabel;
    lblXcp: TLabel;
    iconXcp2: TImage;
    lblHardware: TLabel;
    cmbHardware: TComboBox;
    lblChannel: TLabel;
    cmbChannel: TComboBox;
    lblBaudRate: TLabel;
    chbExtendedId: TCheckBox;
    lblT1: TLabel;
    lblT3: TLabel;
    lblT4: TLabel;
    lblT5: TLabel;
    lblT7: TLabel;
    edtT1: TEdit;
    edtT3: TEdit;
    edtT4: TEdit;
    edtT5: TEdit;
    edtT7: TEdit;
    tabProt: TTabSheet;
    iconXcp1: TImage;
    lblPort: TLabel;
    edtSeedKey: TEdit;
    btnBrowse: TButton;
    lblTransmitId: TLabel;
    Label1: TLabel;
    edtTransmitId: TEdit;
    edtReceiveId: TEdit;
    openDialog: TOpenDialog;
    edtTconnect: TEdit;
    lblTconnect: TLabel;
    cmbBaudrate: TComboBox;
    procedure btnOKClick(Sender: TObject);
    procedure btnCancelClick(Sender: TObject);
    procedure btnBrowseClick(Sender: TObject);
    procedure cmbHardwareChange(Sender: TObject);
    procedure edtTransmitIdChange(Sender: TObject);
    procedure edtTransmitIdKeyPress(Sender: TObject; var Key: Char);
    procedure edtReceiveIdKeyPress(Sender: TObject; var Key: Char);
    procedure edtReceiveIdChange(Sender: TObject);
  private
    { Private declarations }
    procedure ValidateHexCanIdInputChange(EdtID: TEdit);
    procedure ValidateHexCanIdInputPress(Sender: TObject; var Key: char);
  public
    { Public declarations }
    procedure SetAvailableChannels;
  end;

type
  TXcpSettings = class(TObject)
  private
    FSettingsForm : TXcpSettingsForm;
    FIniFile      : string;
  public
    constructor Create(iniFile : string);
    destructor  Destroy; override;
    function    Configure : Boolean;
  end;


implementation
{$R *.DFM}

//***************************************************************************************
// NAME:           SetAvailableChannels
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Updates the items in the channels combobox based on the selected
//                 hardware.
//
//***************************************************************************************
procedure TXcpSettingsForm.SetAvailableChannels;
var
  maxChannels: Integer;
  channelCnt: Integer;
  oldSelectedIdx: Integer;
begin
  // init to safe value
  maxChannels := 2;

  case cmbHardware.ItemIndex of
    0 , 1: { PCAN USB or PCAN PCI }
    begin
      maxChannels := 8;
    end;
    2: { PCAN PC Card }
    begin
      maxChannels := 2;
    end;
  end;

  // backup currently selected channel
  oldSelectedIdx := cmbChannel.ItemIndex;

  // update the combobox contents
  cmbChannel.Items.Clear;
  for channelCnt := 1 to maxChannels do
  begin
    cmbChannel.Items.Add('Channel' + InttoStr(channelCnt));
  end;
  cmbChannel.DropDownCount := maxChannels;

  // restore the selected channel
  if oldSelectedIdx >= (maxChannels) then
  begin
    cmbChannel.ItemIndex := 0;
  end
  else
  begin
    cmbChannel.ItemIndex := oldSelectedIdx;
  end;
end; //*** end of SetAvailableChannels ***


//***************************************************************************************
// NAME:           ValidateHexCanIdInputChange
// PARAMETER:      EdtID Signal source.
// RETURN VALUE:   none.
// DESCRIPTION:    Checks to see if a valid hexadecimal CAN identifier was entered in
//                 the specified edit box. Should be called in the edit box's onChange
//                 event handler.
//
//***************************************************************************************
procedure TXcpSettingsForm.ValidateHexCanIdInputChange(EdtID: TEdit);
var
  value: Int64;
begin
  // prevent a message identifier > 0x1FFFFFFF from being entered
  if EdtID.Text <> '' then
  begin
    try
      value := StrToInt64('$' + EdtID.Text);
      if value < 0 then
      begin
        EdtID.Text := '0';
      end
      else if value > $1FFFFFFF then
      begin
        EdtID.Text := '1FFFFFFF';
      end;
      // automatically set extended if flag
      if value > $7ff then
        chbExtendedId.Checked := True;
    except
      // use id 0 if a non hex value was entered, for example through copy-paste
      EdtID.Text := '0';
    end;
  end;
end; //*** end of ValidateHexCanIdInputChange ***


//***************************************************************************************
// NAME:           ValidateHexCanIdInputPress
// PARAMETER:      Sender Signal source.
//                 Key The key's character code that was pressed.
// RETURN VALUE:   none.
// DESCRIPTION:    Checks to see if a valid hexadecimal CAN identifier was entered in
//                 the specified edit box. Should be called in the edit box's onPress
//                 event handler.
//
//***************************************************************************************
procedure TXcpSettingsForm.ValidateHexCanIdInputPress(Sender: TObject; var Key: char);
begin
  if not (AnsiChar(Key) In ['0'..'9', 'a'..'f', 'A'..'F', #8, ^V, ^C]) then // #8 = backspace
  begin
    // ignore it
    Key := #0;
  end;
  // convert a..f to upper case
  if AnsiChar(Key) In ['a'..'f'] then
  begin
    Key := UpCase(Key);
  end;
end; //*** end of ValidateHexCanIdInputPress ***


//***************************************************************************************
// NAME:           cmbHardwareChange
// PARAMETER:      none
// RETURN VALUE:   modal result
// DESCRIPTION:    Event handler for when the hardware combobox selection changed.
//
//***************************************************************************************
procedure TXcpSettingsForm.cmbHardwareChange(Sender: TObject);
begin
  SetAvailableChannels;
end; //*** end of cmbHardwareChange ***


//***************************************************************************************
// NAME:           edtTransmitIdChange
// PARAMETER:      Sender Signal source.
// RETURN VALUE:   None.
// DESCRIPTION:    Called when the text in the edit box changed.
//
//***************************************************************************************
procedure TXcpSettingsForm.edtReceiveIdChange(Sender: TObject);
begin
  ValidateHexCanIdInputChange(edtReceiveId);
end; //*** end of edtReceiveIdChange ***


//***************************************************************************************
// NAME:           edtReceiveIdKeyPress
// PARAMETER:      Sender Signal source.
//                 Key The key's character code that was pressed.
// RETURN VALUE:   None.
// DESCRIPTION:    Called when a key is pressed.
//
//***************************************************************************************
procedure TXcpSettingsForm.edtReceiveIdKeyPress(Sender: TObject; var Key: Char);
begin
  ValidateHexCanIdInputPress(edtReceiveId, Key);
end; //*** end of edtReceiveIdKeyPress ***


//***************************************************************************************
// NAME:           edtTransmitIdChange
// PARAMETER:      Sender Signal source.
// RETURN VALUE:   None.
// DESCRIPTION:    Called when the text in the edit box changed.
//
//***************************************************************************************
procedure TXcpSettingsForm.edtTransmitIdChange(Sender: TObject);
begin
  ValidateHexCanIdInputChange(edtTransmitId);
end; //*** end of edtTransmitIdChange ***


//***************************************************************************************
// NAME:           edtTransmitIdKeyPress
// PARAMETER:      Sender Signal source.
//                 Key The key's character code that was pressed.
// RETURN VALUE:   None.
// DESCRIPTION:    Called when a key is pressed.
//
//***************************************************************************************
procedure TXcpSettingsForm.edtTransmitIdKeyPress(Sender: TObject; var Key: Char);
begin
  ValidateHexCanIdInputPress(edtTransmitId, Key);
end; //*** end of edtTransmitIdKeyPress ***


//***************************************************************************************
// NAME:           btnOKClick
// PARAMETER:      none
// RETURN VALUE:   modal result
// DESCRIPTION:    Sets the module result to okay.
//
//***************************************************************************************
procedure TXcpSettingsForm.btnOKClick(Sender: TObject);
begin
  ModalResult := mrOK;
end; //*** end of btnOKClick ***


//***************************************************************************************
// NAME:           btnCancelClick
// PARAMETER:      none
// RETURN VALUE:   modal result
// DESCRIPTION:    Sets the module result to cancel.
//
//***************************************************************************************
procedure TXcpSettingsForm.btnCancelClick(Sender: TObject);
begin
  ModalResult := mrCancel;
end; //*** end of btnCancelClick ***


//***************************************************************************************
// NAME:           btnBrowseClick
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Prompts the user to select the seed/key dll file.
//
//***************************************************************************************
procedure TXcpSettingsForm.btnBrowseClick(Sender: TObject);
begin
  openDialog.InitialDir := ExtractFilePath(ParamStr(0));
  if openDialog.Execute then
  begin
    edtSeedKey.Text := openDialog.FileName;
  end;
end; //*** end of btnBrowseClick ***


//***************************************************************************************
// NAME:           Create
// PARAMETER:      Name of the INI file where the settings are and will be stored
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor
//
//***************************************************************************************
constructor TXcpSettings.Create(iniFile : string);
begin
  // call inherited constructor
  inherited Create;

  // set the inifile
  FIniFile := iniFile;

  // create an instance of the settings form
  FSettingsForm := TXcpSettingsForm.Create(nil);
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Destroy
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class destructor
//
//***************************************************************************************
destructor TXcpSettings.Destroy;
begin
  // releaase the settings form object
  FSettingsForm.Free;

  // call inherited destructor
  inherited;
end; //*** end of Destroy ***


//***************************************************************************************
// NAME:           Configure
// PARAMETER:      none
// RETURN VALUE:   True if configuration was successfully changed, False otherwise
// DESCRIPTION:    Allows the user to configure the XCP interface using a GUI.
//
//***************************************************************************************
function TXcpSettings.Configure : Boolean;
var
  settingsIni: TIniFile;
  settingsInt: Integer;
begin
  // initialize the return value
  result := false;

  // init the form elements using the configuration found in the INI
  if FileExists(FIniFile) then
  begin
    // create ini file object
    settingsIni := TIniFile.Create(FIniFile);

    // CAN related elements
    settingsInt := settingsIni.ReadInteger('can', 'hardware', 0);
    if settingsInt > FSettingsForm.cmbHardware.Items.Count then
      settingsInt := 0;
    FSettingsForm.cmbHardware.ItemIndex := settingsInt;
    FSettingsForm.SetAvailableChannels;

    settingsInt := settingsIni.ReadInteger('can', 'channel', 0);
    if settingsInt >= FSettingsForm.cmbChannel.Items.Count then
      settingsInt := 0;
    FSettingsForm.cmbChannel.ItemIndex := settingsInt;

    settingsInt := settingsIni.ReadInteger('can', 'baudrate', 2);
    if settingsInt >= FSettingsForm.cmbBaudrate.Items.Count then
      settingsInt := 2;
    FSettingsForm.cmbBaudrate.ItemIndex := settingsInt;

    FSettingsForm.chbExtendedId.Checked := settingsIni.ReadBool('can', 'extended', false);
    FSettingsForm.edtTransmitId.Text := Format('%x',[settingsIni.ReadInteger('can', 'txid', $667)]);
    FSettingsForm.edtReceiveId.Text := Format('%x',[settingsIni.ReadInteger('can', 'rxid', $7e1)]);

    // XCP related elements
    FSettingsForm.edtSeedKey.Text := settingsIni.ReadString('xcp', 'seedkey', '');
    FSettingsForm.edtT1.Text := IntToStr(settingsIni.ReadInteger('xcp', 't1', 1000));
    FSettingsForm.edtT3.Text := IntToStr(settingsIni.ReadInteger('xcp', 't3', 2000));
    FSettingsForm.edtT4.Text := IntToStr(settingsIni.ReadInteger('xcp', 't4', 10000));
    FSettingsForm.edtT5.Text := IntToStr(settingsIni.ReadInteger('xcp', 't5', 1000));
    FSettingsForm.edtT7.Text := IntToStr(settingsIni.ReadInteger('xcp', 't7', 2000));
    FSettingsForm.edtTconnect.Text := IntToStr(settingsIni.ReadInteger('xcp', 'tconnect', 20));

    // release ini file object
    settingsIni.Free;
  end
  else
  begin
    // set defaults
    // CAN related elements
    FSettingsForm.cmbHardware.ItemIndex := 0;
    FSettingsForm.SetAvailableChannels;
    FSettingsForm.cmbChannel.ItemIndex := 0;
    FSettingsForm.cmbBaudrate.ItemIndex := 2;
    FSettingsForm.chbExtendedId.Checked := false;
    FSettingsForm.edtTransmitId.Text := Format('%x',[$667]);
    FSettingsForm.edtReceiveId.Text := Format('%x',[$7e1]);

    // XCP related elements
    FSettingsForm.edtSeedKey.Text := '';
    FSettingsForm.edtT1.Text := IntToStr(1000);
    FSettingsForm.edtT3.Text := IntToStr(2000);
    FSettingsForm.edtT4.Text := IntToStr(10000);
    FSettingsForm.edtT5.Text := IntToStr(1000);
    FSettingsForm.edtT7.Text := IntToStr(2000);
    FSettingsForm.edtTconnect.Text := IntToStr(20);
  end;

  // show the form as modal so we can get the result here
  if FSettingsForm.ShowModal = mrOK then
  begin
    if FIniFile <> '' then
    begin
      // create ini file object
      settingsIni := TIniFile.Create(FIniFile);

      // CAN related elements
      settingsIni.WriteInteger('can', 'hardware', FSettingsForm.cmbHardware.ItemIndex);
      settingsIni.WriteInteger('can', 'channel', FSettingsForm.cmbChannel.ItemIndex);
      settingsIni.WriteInteger('can', 'baudrate', FSettingsForm.cmbBaudrate.ItemIndex);
      settingsIni.WriteBool('can', 'extended', FSettingsForm.chbExtendedId.Checked);
      settingsIni.WriteInteger('can', 'txid', StrToInt('$'+FSettingsForm.edtTransmitId.Text));
      settingsIni.WriteInteger('can', 'rxid', StrToInt('$'+FSettingsForm.edtReceiveId.Text));

      // XCP related elements
      settingsIni.WriteString('xcp', 'seedkey', FSettingsForm.edtSeedKey.Text);
      settingsIni.WriteInteger('xcp', 't1', StrToInt(FSettingsForm.edtT1.Text));
      settingsIni.WriteInteger('xcp', 't3', StrToInt(FSettingsForm.edtT3.Text));
      settingsIni.WriteInteger('xcp', 't4', StrToInt(FSettingsForm.edtT4.Text));
      settingsIni.WriteInteger('xcp', 't5', StrToInt(FSettingsForm.edtT5.Text));
      settingsIni.WriteInteger('xcp', 't7', StrToInt(FSettingsForm.edtT7.Text));
      settingsIni.WriteInteger('xcp', 'tconnect', StrToInt(FSettingsForm.edtTconnect.Text));

      // release ini file object
      settingsIni.Free;

      // indicate that the settings where successfully updated
      result := true;
    end;
  end;
end; //*** end of Configure ***


end.
//******************************** end of XcpSettings.pas *******************************


