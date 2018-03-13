unit SessionXcpDialog;
//***************************************************************************************
//  Description: Implements the XCP session dialog.
//    File Name: sessionxcpdialog.pas
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
  ExtCtrls, ConfigGroups;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
//------------------------------ TSessionXcpForm ----------------------------------------
type

  { TSessionXcpForm }

  TSessionXcpForm = class(TForm)
    BtnSeedKey: TButton;
    CmbConnectMode: TComboBox;
    EdtSeedKey: TEdit;
    EdtTimeoutT1: TEdit;
    EdtTimeoutT3: TEdit;
    EdtTimeoutT4: TEdit;
    EdtTimeoutT5: TEdit;
    EdtTimeoutT7: TEdit;
    LblConnection: TLabel;
    LblSeedKey: TLabel;
    LblSecurity: TLabel;
    LblTimeoutT1: TLabel;
    LblTimeouts: TLabel;
    LblConnectMode: TLabel;
    LblTimeoutT3: TLabel;
    LblTimeoutT4: TLabel;
    LblTimeoutT5: TLabel;
    LblTimeoutT7: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private
    FSessionXcpConfig: TSessionXcpConfig;
  public
    property Config: TSessionXcpConfig read FSessionXcpConfig;
  end;


implementation

{$R *.lfm}

//---------------------------------------------------------------------------------------
//-------------------------------- TSessionXcpForm --------------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           FormCreate
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form constructor.
//
//***************************************************************************************
procedure TSessionXcpForm.FormCreate(Sender: TObject);
begin
  // Create configuration group instance.
  FSessionXcpConfig := TSessionXcpConfig.Create;
  // Align browse button vertically to the related edit box.
  BtnSeedKey.Top := EdtSeedKey.Top;
  BtnSeedKey.Height := EdtSeedKey.Height;
end; //*** end of FormCreate ***


//***************************************************************************************
// NAME:           FormDestroy
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form destructor.
//
//***************************************************************************************
procedure TSessionXcpForm.FormDestroy(Sender: TObject);
begin
  // Release the configuration group instance.
  FSessionXcpConfig.Free;
end; //*** end of FormDestroy ***


end.
//******************************** end of sessionxcpdialog.pas **************************

