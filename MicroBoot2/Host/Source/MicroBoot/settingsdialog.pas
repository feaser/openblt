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
  StdCtrls, ComCtrls, CurrentConfig;


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
    procedure FormCreate(Sender: TObject);
  private
    FCurrentConfig: TCurrentConfig;
  public
    constructor Create(TheOwner: TComponent; CurrentConfig: TCurrentConfig); reintroduce;
  end;


implementation

{$R *.lfm}

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
  //LblProtocol.Left := CmbProtocol.Left - LblProtocol.Width - 5;
  //LblInterface.Left := CmbInterface.Left - LblInterface.Width - 5;

end; //*** end of FormCreate ***


//***************************************************************************************
// NAME:           BtnOkClick
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the button is clicked.
//
//***************************************************************************************
procedure TSettingsForm.BtnOkClick(Sender: TObject);
begin
  { TODO : Update the settings in FCurrentConfig based on the dialog configured settings. }
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


//---------------------------------------------------------------------------------------
//-------------------------------- TSettingsForm ----------------------------------------
//---------------------------------------------------------------------------------------
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

