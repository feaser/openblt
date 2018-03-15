unit ConfigGroups;
//***************************************************************************************
//  Description: Configuration groups available to the program.
//    File Name: configgroups.pas
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
  Classes, SysUtils, CurrentConfig, XMLConf;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  //------------------------------ TMainWindowConfig ------------------------------------
  TMainWindowConfig = class (TConfigGroup)
  private
    FWidth: Integer;
    FHeight: Integer;
  public
    const GROUP_NAME='MainWindow';
    constructor Create;
    procedure Defaults; override;
    procedure LoadFromFile(XmlConfig: TXMLConfig); override;
    procedure SaveToFile(XmlConfig: TXMLConfig); override;
    property Width: Integer read FWidth write FWidth;
    property Height: Integer read FHeight write FHeight;
  end;

  //------------------------------ TSessionConfig ---------------------------------------
  TSessionConfig = class (TConfigGroup)
  private
    FSession: String;
  public
    const GROUP_NAME='Session';
    constructor Create;
    procedure Defaults; override;
    procedure LoadFromFile(XmlConfig: TXMLConfig); override;
    procedure SaveToFile(XmlConfig: TXMLConfig); override;
    property Session: String read FSession write FSession;
  end;

  //------------------------------ TSessionXcpConfig ------------------------------------
  TSessionXcpConfig = class (TConfigGroup)
  private
    FTimeoutT1: Integer;
    FTimeoutT3: Integer;
    FTimeoutT4: Integer;
    FTimeoutT5: Integer;
    FTimeoutT7: Integer;
    FConnectMode: Integer;
    FSeedKey: String;
  public
    const GROUP_NAME='Session/Xcp';
    constructor Create;
    procedure Defaults; override;
    procedure LoadFromFile(XmlConfig: TXMLConfig); override;
    procedure SaveToFile(XmlConfig: TXMLConfig); override;
    property TimeoutT1: Integer read FTimeoutT1 write FTimeoutT1;
    property TimeoutT3: Integer read FTimeoutT3 write FTimeoutT3;
    property TimeoutT4: Integer read FTimeoutT4 write FTimeoutT4;
    property TimeoutT5: Integer read FTimeoutT5 write FTimeoutT5;
    property TimeoutT7: Integer read FTimeoutT7 write FTimeoutT7;
    property ConnectMode: Integer read FConnectMode write FConnectMode;
    property SeedKey: String read FSeedKey write FSeedKey;
  end;


implementation
//---------------------------------------------------------------------------------------
//-------------------------------- TMainWindowConfig ------------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor.
//
//***************************************************************************************
constructor TMainWindowConfig.Create;
begin
  // Call inherited constructor.
  inherited Create;
  // Set fields.
  FName := GROUP_NAME;
  Defaults;
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Defaults
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Sets default values for this group's settings.
//
//***************************************************************************************
procedure TMainWindowConfig.Defaults;
begin
  FWidth := 500;
  FHeight := 180;
end; //*** end of Defaults ***


//***************************************************************************************
// NAME:           LoadFromFile
// PARAMETER:      XmlConfig XML configuration instance.
// RETURN VALUE:   none
// DESCRIPTION:    Loads this group's configuration settings using the XML configuration
//                 instance.
//
//***************************************************************************************
procedure TMainWindowConfig.LoadFromFile(XmlConfig: TXMLConfig);
begin
  // Open this group's key.
  XmlConfig.OpenKey(UnicodeString(Self.Name));
  // Load all settings.
  FWidth := XmlConfig.GetValue('width', FWidth);
  FHeight := XmlConfig.GetValue('height', FHeight);
  // Close this group's key.
  XmlConfig.CloseKey;
end; //*** end of LoadFromFile ***/


//***************************************************************************************
// NAME:           SaveToFile
// PARAMETER:      XmlConfig XML configuration instance.
// RETURN VALUE:   none
// DESCRIPTION:    Saves this group's configuration settings using the XML configuration
//                 instance.
//
//***************************************************************************************
procedure TMainWindowConfig.SaveToFile(XmlConfig: TXMLConfig);
begin
  // Open this group's key.
  XmlConfig.OpenKey(UnicodeString(Self.Name));
  // Store all settings.
  XmlConfig.SetValue('width', FWidth);
  XmlConfig.SetValue('height', FHeight);
  // Close this group's key.
  xmlConfig.CloseKey;
end; //*** end of SaveToFile ***


//---------------------------------------------------------------------------------------
//-------------------------------- TSessionConfig ---------------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor.
//
//***************************************************************************************
constructor TSessionConfig.Create;
begin
  // Call inherited constructor.
  inherited Create;
  // Set fields.
  FName := GROUP_NAME;
  Defaults;
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Defaults
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Sets default values for this group's settings.
//
//***************************************************************************************
procedure TSessionConfig.Defaults;
begin
  FSession := 'xcp';
end; //*** end of Defaults ***


//***************************************************************************************
// NAME:           LoadFromFile
// PARAMETER:      XmlConfig XML configuration instance.
// RETURN VALUE:   none
// DESCRIPTION:    Loads this group's configuration settings using the XML configuration
//                 instance.
//
//***************************************************************************************
procedure TSessionConfig.LoadFromFile(XmlConfig: TXMLConfig);
begin
  // Open this group's key.
  XmlConfig.OpenKey(UnicodeString(Self.Name));
  // Load all settings.
  FSession := String(XmlConfig.GetValue('session', UnicodeString(FSession)));
  // Close this group's key.
  XmlConfig.CloseKey;
end; //*** end of LoadFromFile ***/


//***************************************************************************************
// NAME:           SaveToFile
// PARAMETER:      XmlConfig XML configuration instance.
// RETURN VALUE:   none
// DESCRIPTION:    Saves this group's configuration settings using the XML configuration
//                 instance.
//
//***************************************************************************************
procedure TSessionConfig.SaveToFile(XmlConfig: TXMLConfig);
begin
  // Open this group's key.
  XmlConfig.OpenKey(UnicodeString(Self.Name));
  // Store all settings.
  XmlConfig.SetValue('session', UnicodeString(FSession));
  // Close this group's key.
  XmlConfig.CloseKey;
end; //*** end of SaveToFile ***


//---------------------------------------------------------------------------------------
//-------------------------------- TSessionXcpConfig ------------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor.
//
//***************************************************************************************
constructor TSessionXcpConfig.Create;
begin
  // Call inherited constructor.
  inherited Create;
  // Set fields.
  FName := GROUP_NAME;
  Defaults;
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Defaults
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Sets default values for this group's settings.
//
//***************************************************************************************
procedure TSessionXcpConfig.Defaults;
begin
  FTimeoutT1 := 1000;
  FTimeoutT3 := 2000;
  FTimeoutT4 := 10000;
  FTimeoutT5 := 1000;
  FTimeoutT7 := 2000;
  FConnectMode := 0;
  FSeedKey := '';
end; //*** end of Defaults ***


//***************************************************************************************
// NAME:           LoadFromFile
// PARAMETER:      XmlConfig XML configuration instance.
// RETURN VALUE:   none
// DESCRIPTION:    Loads this group's configuration settings using the XML configuration
//                 instance.
//
//***************************************************************************************
procedure TSessionXcpConfig.LoadFromFile(XmlConfig: TXMLConfig);
begin
  // Open this group's key.
  XmlConfig.OpenKey(UnicodeString(Self.Name));
  // Load all settings.
  FTimeoutT1 := XmlConfig.GetValue('timeout_t1', FTimeoutT1);
  FTimeoutT3 := XmlConfig.GetValue('timeout_t3', FTimeoutT3);
  FTimeoutT4 := XmlConfig.GetValue('timeout_t4', FTimeoutT4);
  FTimeoutT5 := XmlConfig.GetValue('timeout_t5', FTimeoutT5);
  FTimeoutT7 := XmlConfig.GetValue('timeout_t7', FTimeoutT7);
  FConnectMode := XmlConfig.GetValue('connect_mode', FConnectMode);
  FSeedKey := String(XmlConfig.GetValue('seed_key', UnicodeString(FSeedKey)));
  // Close this group's key.
  XmlConfig.CloseKey;
end; //*** end of LoadFromFile ***/


//***************************************************************************************
// NAME:           SaveToFile
// PARAMETER:      XmlConfig XML configuration instance.
// RETURN VALUE:   none
// DESCRIPTION:    Saves this group's configuration settings using the XML configuration
//                 instance.
//
//***************************************************************************************
procedure TSessionXcpConfig.SaveToFile(XmlConfig: TXMLConfig);
begin
  // Open this group's key.
  XmlConfig.OpenKey(UnicodeString(Self.Name));
  // Store all settings.
  XmlConfig.SetValue('timeout_t1', FTimeoutT1);
  XmlConfig.SetValue('timeout_t3', FTimeoutT3);
  XmlConfig.SetValue('timeout_t4', FTimeoutT4);
  XmlConfig.SetValue('timeout_t5', FTimeoutT5);
  XmlConfig.SetValue('timeout_t7', FTimeoutT7);
  XmlConfig.SetValue('connect_mode', FConnectMode);
  XmlConfig.SetValue('seed_key', UnicodeString(FSeedKey));
  // Close this group's key.
  XmlConfig.CloseKey;
end; //*** end of SaveToFile ***


end.
//******************************** end of configgroups.pas ******************************

