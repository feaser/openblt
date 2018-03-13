unit XcpSettings;
//***************************************************************************************
//  Description: XCP settings interface for SCI
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
    lblXcp: TLabel;
    iconXcp2: TImage;
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
    openDialog: TOpenDialog;
    edtTconnect: TEdit;
    lblTconnect: TLabel;
    tabSession: TTabSheet;
    iconXcp3: TImage;
    lblXcpSession: TLabel;
    lblConnectMode: TLabel;
    cmbConnectMode: TComboBox;
    procedure btnOKClick(Sender: TObject);
    procedure btnCancelClick(Sender: TObject);
    procedure btnBrowseClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
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
begin
  // initialize the return value
  result := false;

  // init the form elements using the configuration found in the INI
  if FileExists(FIniFile) then
  begin
    // create ini file object
    settingsIni := TIniFile.Create(FIniFile);

    // XCP related elements
    FSettingsForm.edtSeedKey.Text := settingsIni.ReadString('xcp', 'seedkey', ExtractFilePath(ParamStr(0))+'');
    FSettingsForm.edtT1.Text := IntToStr(settingsIni.ReadInteger('xcp', 't1', 1000));
    FSettingsForm.edtT3.Text := IntToStr(settingsIni.ReadInteger('xcp', 't3', 2000));
    FSettingsForm.edtT4.Text := IntToStr(settingsIni.ReadInteger('xcp', 't4', 10000));
    FSettingsForm.edtT5.Text := IntToStr(settingsIni.ReadInteger('xcp', 't5', 1000));
    FSettingsForm.edtT7.Text := IntToStr(settingsIni.ReadInteger('xcp', 't7', 2000));
    FSettingsForm.edtTconnect.Text := IntToStr(settingsIni.ReadInteger('xcp', 'tconnect', 20));
    FSettingsForm.cmbConnectMode.ItemIndex := settingsIni.ReadInteger('xcp', 'connectmode', 0);

    // release ini file object
    settingsIni.Free;
  end
  else
  begin
    // set defaults
    // XCP related elements
    FSettingsForm.edtSeedKey.Text := ExtractFilePath(ParamStr(0))+'';
    FSettingsForm.edtT1.Text := IntToStr(1000);
    FSettingsForm.edtT3.Text := IntToStr(2000);
    FSettingsForm.edtT4.Text := IntToStr(10000);
    FSettingsForm.edtT5.Text := IntToStr(1000);
    FSettingsForm.edtT7.Text := IntToStr(2000);
    FSettingsForm.edtTconnect.Text := IntToStr(20);
    FSettingsForm.cmbConnectMode.ItemIndex := 0;
  end;

  // show the form as modal so we can get the result here
  if FSettingsForm.ShowModal = mrOK then
  begin
    if FIniFile <> '' then
    begin
      // create ini file object
      settingsIni := TIniFile.Create(FIniFile);

      // XCP related elements
      settingsIni.WriteString('xcp', 'seedkey', FSettingsForm.edtSeedKey.Text);
      settingsIni.WriteInteger('xcp', 't1', StrToInt(FSettingsForm.edtT1.Text));
      settingsIni.WriteInteger('xcp', 't3', StrToInt(FSettingsForm.edtT3.Text));
      settingsIni.WriteInteger('xcp', 't4', StrToInt(FSettingsForm.edtT4.Text));
      settingsIni.WriteInteger('xcp', 't5', StrToInt(FSettingsForm.edtT5.Text));
      settingsIni.WriteInteger('xcp', 't7', StrToInt(FSettingsForm.edtT7.Text));
      settingsIni.WriteInteger('xcp', 'tconnect', StrToInt(FSettingsForm.edtTconnect.Text));
      settingsIni.WriteInteger('xcp', 'connectmode', FSettingsForm.cmbConnectMode.ItemIndex);

      // release ini file object
      settingsIni.Free;

      // indicate that the settings where successfully updated
      result := true;
    end;
  end;
end; //*** end of Configure ***


end.
//******************************** end of XcpSettings.pas *******************************


