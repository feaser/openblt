unit TransportXcpRs232Dialog;
//***************************************************************************************
//  Description: Implements the XCP on RS232 transport layer dialog.
//    File Name: transportxcprs232dialog.pas
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
  //------------------------------ TTransportXcpRs232Form -------------------------------
  TTransportXcpRs232Form = class(TForm)
    Label1: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private
    FTransportXcpRs232Config: TTransportXcpRs232Config;
  public
    procedure LoadConfig(Config: TTransportXcpRs232Config);
    procedure SaveConfig(Config: TTransportXcpRs232Config);
  end;


implementation

{$R *.lfm}

//---------------------------------------------------------------------------------------
//-------------------------------- TTransportXcpRs232Form -------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           FormCreate
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form constructor.
//
//***************************************************************************************
procedure TTransportXcpRs232Form.FormCreate(Sender: TObject);
begin
  // Create configuration group instance.
  FTransportXcpRs232Config := TTransportXcpRs232Config.Create;
end; //*** end of FormCreate ***


//***************************************************************************************
// NAME:           FormDestroy
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Form destructor.
//
//***************************************************************************************
procedure TTransportXcpRs232Form.FormDestroy(Sender: TObject);
begin
  // Release the configuration group instance.
  FTransportXcpRs232Config.Free;
end; //*** end of FormDestroy ***


//***************************************************************************************
// NAME:           LoadConfig
// PARAMETER:      Config Configuration instance to load from.
// RETURN VALUE:   none
// DESCRIPTION:    Loads the configuration values from the specified instance and
//                 initializes the user interface accordingly.
//
//***************************************************************************************
procedure TTransportXcpRs232Form.LoadConfig(Config: TTransportXcpRs232Config);
begin
  // Load configuration.
  FTransportXcpRs232Config.Device := Config.Device;
  FTransportXcpRs232Config.Baudrate := Config.Baudrate;
  { TODO : Initialize user interface. }
end; //*** end of LoadConfig ***


//***************************************************************************************
// NAME:           SaveConfig
// PARAMETER:      Config Configuration instance to save to.
// RETURN VALUE:   none
// DESCRIPTION:    Reads the configuration values from the user interface and stores them
//                 in the specified instance.
//
//***************************************************************************************
procedure TTransportXcpRs232Form.SaveConfig(Config: TTransportXcpRs232Config);
begin
  // Start out with default configuration settings.
  FTransportXcpRs232Config.Defaults;
  { TODO : Read configuration from the user interface. }
  // Store configuration.
  Config.Device := FTransportXcpRs232Config.Device;
  Config.Baudrate := FTransportXcpRs232Config.Baudrate;
end; //*** end of SaveConfig ***


end.
//******************************** end of transportxcprs232dialog.pas *******************

