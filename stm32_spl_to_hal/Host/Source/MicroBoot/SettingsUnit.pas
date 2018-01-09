unit SettingsUnit;
//***************************************************************************************
// Project Name: MicroBoot for Borland Delphi
//  Description: Contains the Settings Window for MicroBoot
//    File Name: SettingsUnit.pas
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
  StdCtrls, ExtCtrls;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  TsettingsForm = class(TForm)
    btnOk: TButton;
    pnlFooter: TPanel;
    grbTargetInterface: TGroupBox;
    cbbInterfaces: TComboBox;
    btnOptions: TButton;
    procedure btnOkClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure btnOptionsClick(Sender: TObject);
    procedure cbbInterfacesChange(Sender: TObject);
  private
    { Private declarations }
    libFileNameList : TStrings;
    libFileInfoList : TStrings;
    ExePath         : string;
  public
    { Public declarations }
  end;

//***************************************************************************************
// Global Variables
//***************************************************************************************
var
  settingsForm: TsettingsForm;

implementation

//***************************************************************************************
// Local Includes
//***************************************************************************************
uses MainUnit;

{$R *.DFM}


//***************************************************************************************
// NAME:           btnOkClick
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Closes the dialog and sends a positive response back to the
//                 application.
//
//***************************************************************************************
procedure TsettingsForm.btnOkClick(Sender: TObject);
begin
  ModalResult := mrOK;
end; //*** end of btnOkClick ***


//***************************************************************************************
// NAME:           FormCreate
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Constructs the form an creates instances of the objects we intend to
//                 use.
//
//***************************************************************************************
procedure TsettingsForm.FormCreate(Sender: TObject);
begin
  // instanciate string lists
  libFileNameList := TStringList.Create;
  libFileInfoList := TStringList.Create;

  ExePath := ExtractFilePath(Application.ExeName);

end; //*** end of FormCreate ***


//***************************************************************************************
// NAME:           FormDestroy
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Destroys the form an frees instances of the objects we used.
//
//***************************************************************************************
procedure TsettingsForm.FormDestroy(Sender: TObject);
begin
  libFileNameList.Free;
  libFileInfoList.Free;
end; //*** end of FormDestroy ***


//***************************************************************************************
// NAME:           FormShow
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Obtains list with interface libraries and adds each one of these
//                 to the combobox with a description so it's easy for users to select
//                 one.
//
//***************************************************************************************
procedure TsettingsForm.FormShow(Sender: TObject);
var
  cnt : integer;
  activeLib : string;
begin
  // clear string lists before using them
  libFileNameList.Clear;
  libFileInfoList.Clear;
  cbbInterfaces.Items.Clear;

  // obtian list with available Mbi interface DLL's that are found in the EXE path
  mainForm.GetInterfaceFileList(libFileNameList);

  activeLib := ExtractFileName(mainForm.GetActiveMbi);

  for cnt := 0 to libFileNameList.Count-1  do
  begin
    cbbInterfaces.Items.Add(mainForm.GetMbiInfoString(ExePath +
                            libFileNameList[cnt]));

    //select the active one
    if libFileNameList[cnt] = activeLib then
    begin
      cbbInterfaces.ItemIndex := cnt;
    end;
  end;
end; //*** end of FormShow ***


//***************************************************************************************
// NAME:           btnOptionsClick
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Submits a request to the interface library to display extra
//                 configuration options.
//
//***************************************************************************************
procedure TsettingsForm.btnOptionsClick(Sender: TObject);
var
  cnt : integer;
  activeLib : string;
begin
  // submit configuration request to interface library
  mainForm.ConfigureMbi;

  // clear string lists before using them
  libFileNameList.Clear;
  libFileInfoList.Clear;
  cbbInterfaces.Items.Clear;

  // obtian list with available Mbi interface DLL's that are found in the EXE path
  mainForm.GetInterfaceFileList(libFileNameList);

  activeLib := ExtractFileName(mainForm.GetActiveMbi);

  for cnt := 0 to libFileNameList.Count-1  do
  begin
    cbbInterfaces.Items.Add(mainForm.GetMbiInfoString(ExePath +
                            libFileNameList[cnt]));

    //select the active one
    if libFileNameList[cnt] = activeLib then
    begin
      cbbInterfaces.ItemIndex := cnt;
    end;
  end;
end; //*** end of btnOptionsClick ***


//***************************************************************************************
// NAME:           cbbInterfacesChange
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Updates the interface library that is linked to the application. 
//
//***************************************************************************************
procedure TsettingsForm.cbbInterfacesChange(Sender: TObject);
begin
  // enable the selected mbi interface
  mainForm.SetActiveMbi(ExePath + libFileNameList[cbbInterfaces.ItemIndex]);
end; //*** end of cbbInterfacesChange ***

end.
//******************************** end of SettingsUnit.pas ******************************

