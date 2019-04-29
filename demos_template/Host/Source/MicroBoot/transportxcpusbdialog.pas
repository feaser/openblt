unit TransportXcpUsbDialog;
//***************************************************************************************
//  Description: Implements the XCP on USB transport layer dialog.
//    File Name: transportxcpusbdialog.pas
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
  ConfigGroups;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  //------------------------------ TTransportXcpUsbForm ---------------------------------
  TTransportXcpUsbForm = class(TForm)
    EdtPID: TEdit;
    EdtVID: TEdit;
    LblPID: TLabel;
    LblVID: TLabel;
    LblCommunication: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private
    FTransportXcpUsbConfig: TTransportXcpUsbConfig;
  public
    procedure LoadConfig(Config: TTransportXcpUsbConfig);
    procedure SaveConfig(Config: TTransportXcpUsbConfig);
  end;


implementation

{$R *.lfm}

//---------------------------------------------------------------------------------------
//-------------------------------- TTransportXcpUsbForm ---------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           FormCreate
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form constructor.
//
//***************************************************************************************
procedure TTransportXcpUsbForm.FormCreate(Sender: TObject);
begin
  // Create configuration group instance.
  FTransportXcpUsbConfig := TTransportXcpUsbConfig.Create;
end; //*** end of FormCreate ***


//***************************************************************************************
// NAME:           FormDestroy
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form destructor.
//
//***************************************************************************************
procedure TTransportXcpUsbForm.FormDestroy(Sender: TObject);
begin
  // Release the configuration group instance.
  FTransportXcpUsbConfig.Free;
end; //*** end of FormDestroy ***


//***************************************************************************************
// NAME:           LoadConfig
// PARAMETER:      Config Configuration instance to load from.
// RETURN VALUE:   none
// DESCRIPTION:    Loads the configuration values from the specified instance and
//                 initializes the user interface accordingly.
//
//***************************************************************************************
procedure TTransportXcpUsbForm.LoadConfig(Config: TTransportXcpUsbConfig);
begin
  // Load configuration and initilize use interface. Note that USB does not require
  // any additional configuration so nothing need to be done here.
  Config := Config; // Suppress compiler hint due to unused parameter.
end; //*** end of LoadConfig ***


//***************************************************************************************
// NAME:           SaveConfig
// PARAMETER:      Config Configuration instance to save to.
// RETURN VALUE:   none
// DESCRIPTION:    Reads the configuration values from the user interface and stores them
//                 in the specified instance.
//
//***************************************************************************************
procedure TTransportXcpUsbForm.SaveConfig(Config: TTransportXcpUsbConfig);
begin
  // Start out with default configuration settings.
  FTransportXcpUsbConfig.Defaults;
  // Read configuration from the user interface and store the configuration. Note that
  // USB does not require any additional configuration so nothing needs to be done here.
  Config := Config; // Suppress compiler hint due to unused parameter.
end; //*** end of SaveConfig ***


end.
//******************************** end of transportxcpusbdialog.pas *********************

