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

  //------------------------------ TMiscellaneousConfig ---------------------------------
  TMiscellaneousConfig = class (TConfigGroup)
  private
    FLogging: Integer;
    FLogFile: String;
  public
    const GROUP_NAME='Miscellaneus';
    constructor Create;
    procedure Defaults; override;
    procedure LoadFromFile(XmlConfig: TXMLConfig); override;
    procedure SaveToFile(XmlConfig: TXMLConfig); override;
    property Logging: Integer read FLogging write FLogging;
    property LogFile: String read FLogFile write FLogFile;
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
    FTimeoutT6: Integer;
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
    property TimeoutT6: Integer read FTimeoutT6 write FTimeoutT6;
    property TimeoutT7: Integer read FTimeoutT7 write FTimeoutT7;
    property ConnectMode: Integer read FConnectMode write FConnectMode;
    property SeedKey: String read FSeedKey write FSeedKey;
  end;

  //------------------------------ TTransportConfig -------------------------------------
  TTransportConfig = class (TConfigGroup)
  private
    FTransport: String;
  public
    const GROUP_NAME='Transport';
    constructor Create;
    procedure Defaults; override;
    procedure LoadFromFile(XmlConfig: TXMLConfig); override;
    procedure SaveToFile(XmlConfig: TXMLConfig); override;
    property Transport: String read FTransport write FTransport;
  end;

  //------------------------------ TTransportXcpRs232Config -----------------------------
  TTransportXcpRs232Config = class (TConfigGroup)
  private
    FDevice: String;
    FBaudrate: Integer;
  public
    const GROUP_NAME='Transport/Xcp/Rs232';
    constructor Create;
    procedure Defaults; override;
    procedure LoadFromFile(XmlConfig: TXMLConfig); override;
    procedure SaveToFile(XmlConfig: TXMLConfig); override;
    property Device: String read FDevice write FDevice;
    property Baudrate: Integer read FBaudrate write FBaudrate;
  end;

  //------------------------------ TTransportXcpCanConfig -------------------------------
  TTransportXcpCanConfig = class (TConfigGroup)
  private
    FDevice: String;
    FChannel: LongWord;
    FBaudrate: Integer;
    FTransmitId: LongWord;
    FReceiveId: LongWord;
    FExtendedId: Integer;
  public
    const GROUP_NAME='Transport/Xcp/Can';
    constructor Create;
    procedure Defaults; override;
    procedure LoadFromFile(XmlConfig: TXMLConfig); override;
    procedure SaveToFile(XmlConfig: TXMLConfig); override;
    property Device: String read FDevice write FDevice;
    property Channel: LongWord read FChannel write FChannel;
    property Baudrate: Integer read FBaudrate write FBaudrate;
    property TransmitId: LongWord read FTransmitId write FTransmitId;
    property ReceiveId: LongWord read FReceiveId write FReceiveId;
    property ExtendedId: Integer read FExtendedId write FExtendedId;
  end;

  //------------------------------ TTransportXcpUsbConfig -------------------------------
  TTransportXcpUsbConfig = class (TConfigGroup)
  private
  public
    const GROUP_NAME='Transport/Xcp/Usb';
    constructor Create;
    procedure Defaults; override;
    procedure LoadFromFile(XmlConfig: TXMLConfig); override;
    procedure SaveToFile(XmlConfig: TXMLConfig); override;
  end;

  //------------------------------ TTransportXcpTcpIpConfig -----------------------------
  TTransportXcpTcpIpConfig = class (TConfigGroup)
  private
    FAddress: String;
    FPort: Word;
  public
    const GROUP_NAME='Transport/Xcp/TcpIp';
    constructor Create;
    procedure Defaults; override;
    procedure LoadFromFile(XmlConfig: TXMLConfig); override;
    procedure SaveToFile(XmlConfig: TXMLConfig); override;
    property Address: String read FAddress write FAddress;
    property Port: Word read FPort write FPort;
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
  FWidth := 520;
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
//-------------------------------- TMiscellaneousConfig ---------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor.
//
//***************************************************************************************
constructor TMiscellaneousConfig.Create;
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
procedure TMiscellaneousConfig.Defaults;
begin
  FLogging := 0;
  FLogFile := '';
end; //*** end of Defaults ***


//***************************************************************************************
// NAME:           LoadFromFile
// PARAMETER:      XmlConfig XML configuration instance.
// RETURN VALUE:   none
// DESCRIPTION:    Loads this group's configuration settings using the XML configuration
//                 instance.
//
//***************************************************************************************
procedure TMiscellaneousConfig.LoadFromFile(XmlConfig: TXMLConfig);
begin
  // Open this group's key.
  XmlConfig.OpenKey(UnicodeString(Self.Name));
  // Load all settings.
  FLogging := XmlConfig.GetValue('logging', FLogging);
  FLogFile := String(XmlConfig.GetValue('log_file', UnicodeString(FLogFile)));
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
procedure TMiscellaneousConfig.SaveToFile(XmlConfig: TXMLConfig);
begin
  // Open this group's key.
  XmlConfig.OpenKey(UnicodeString(Self.Name));
  // Store all settings.
  XmlConfig.SetValue('logging', FLogging);
  XmlConfig.SetValue('log_file', UnicodeString(FLogFile));
  // Close this group's key.
  XmlConfig.CloseKey;
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
  FTimeoutT6 := 50;
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
  FTimeoutT6 := XmlConfig.GetValue('timeout_t6', FTimeoutT6);
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
  XmlConfig.SetValue('timeout_t6', FTimeoutT6);
  XmlConfig.SetValue('timeout_t7', FTimeoutT7);
  XmlConfig.SetValue('connect_mode', FConnectMode);
  XmlConfig.SetValue('seed_key', UnicodeString(FSeedKey));
  // Close this group's key.
  XmlConfig.CloseKey;
end; //*** end of SaveToFile ***


//---------------------------------------------------------------------------------------
//-------------------------------- TTransportConfig -------------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor.
//
//***************************************************************************************
constructor TTransportConfig.Create;
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
procedure TTransportConfig.Defaults;
begin
  FTransport := 'xcp_rs232';
end; //*** end of Defaults ***


//***************************************************************************************
// NAME:           LoadFromFile
// PARAMETER:      XmlConfig XML configuration instance.
// RETURN VALUE:   none
// DESCRIPTION:    Loads this group's configuration settings using the XML configuration
//                 instance.
//
//***************************************************************************************
procedure TTransportConfig.LoadFromFile(XmlConfig: TXMLConfig);
begin
  // Open this group's key.
  XmlConfig.OpenKey(UnicodeString(Self.Name));
  // Load all settings.
  FTransport := String(XmlConfig.GetValue('transport', UnicodeString(FTransport)));
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
procedure TTransportConfig.SaveToFile(XmlConfig: TXMLConfig);
begin
  // Open this group's key.
  XmlConfig.OpenKey(UnicodeString(Self.Name));
  // Store all settings.
  XmlConfig.SetValue('transport', UnicodeString(FTransport));
  // Close this group's key.
  XmlConfig.CloseKey;
end; //*** end of SaveToFile ***


//---------------------------------------------------------------------------------------
//-------------------------------- TTransportXcpRs232Config -----------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor.
//
//***************************************************************************************
constructor TTransportXcpRs232Config.Create;
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
procedure TTransportXcpRs232Config.Defaults;
begin
  FDevice := '';
  FBaudrate := 57600;
end; //*** end of Defaults ***


//***************************************************************************************
// NAME:           LoadFromFile
// PARAMETER:      XmlConfig XML configuration instance.
// RETURN VALUE:   none
// DESCRIPTION:    Loads this group's configuration settings using the XML configuration
//                 instance.
//
//***************************************************************************************
procedure TTransportXcpRs232Config.LoadFromFile(XmlConfig: TXMLConfig);
begin
  // Open this group's key.
  XmlConfig.OpenKey(UnicodeString(Self.Name));
  // Load all settings.
  FDevice := String(XmlConfig.GetValue('device', UnicodeString(FDevice)));
  FBaudrate := XmlConfig.GetValue('baudrate', FBaudrate);
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
procedure TTransportXcpRs232Config.SaveToFile(XmlConfig: TXMLConfig);
begin
  // Open this group's key.
  XmlConfig.OpenKey(UnicodeString(Self.Name));
  // Store all settings.
  XmlConfig.SetValue('device', UnicodeString(FDevice));
  XmlConfig.SetValue('baudrate', FBaudrate);
  // Close this group's key.
  XmlConfig.CloseKey;
end; //*** end of SaveToFile ***


//---------------------------------------------------------------------------------------
//-------------------------------- TTransportXcpCanConfig -------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor.
//
//***************************************************************************************
constructor TTransportXcpCanConfig.Create;
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
procedure TTransportXcpCanConfig.Defaults;
begin
  FDevice := '';
  FChannel := 0;
  FBaudrate := 500000;
  FTransmitId := $667;
  FReceiveId := $7E1;
  FExtendedId := 0;
end; //*** end of Defaults ***


//***************************************************************************************
// NAME:           LoadFromFile
// PARAMETER:      XmlConfig XML configuration instance.
// RETURN VALUE:   none
// DESCRIPTION:    Loads this group's configuration settings using the XML configuration
//                 instance.
//
//***************************************************************************************
procedure TTransportXcpCanConfig.LoadFromFile(XmlConfig: TXMLConfig);
begin
  // Open this group's key.
  XmlConfig.OpenKey(UnicodeString(Self.Name));
  // Load all settings.
  FDevice := String(XmlConfig.GetValue('device', UnicodeString(FDevice)));
  FChannel := XmlConfig.GetValue('channel', FChannel);
  FBaudrate := XmlConfig.GetValue('baudrate', FBaudrate);
  FTransmitId := XmlConfig.GetValue('transmit_id', FTransmitId);
  FReceiveId := XmlConfig.GetValue('receive_id', FReceiveId);
  FExtendedId := XmlConfig.GetValue('extended_id', FExtendedId);
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
procedure TTransportXcpCanConfig.SaveToFile(XmlConfig: TXMLConfig);
begin
  // Open this group's key.
  XmlConfig.OpenKey(UnicodeString(Self.Name));
  // Store all settings.
  XmlConfig.SetValue('device', UnicodeString(FDevice));
  XmlConfig.SetValue('channel', FChannel);
  XmlConfig.SetValue('baudrate', FBaudrate);
  XmlConfig.SetValue('transmit_id', FTransmitId);
  XmlConfig.SetValue('receive_id', FReceiveId);
  XmlConfig.SetValue('extended_id', FExtendedId);
  // Close this group's key.
  XmlConfig.CloseKey;
end; //*** end of SaveToFile ***


//---------------------------------------------------------------------------------------
//-------------------------------- TTransportXcpUsbConfig -------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor.
//
//***************************************************************************************
constructor TTransportXcpUsbConfig.Create;
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
procedure TTransportXcpUsbConfig.Defaults;
begin
  // USB transport layer currently does not require any additional settings.
end; //*** end of Defaults ***


//***************************************************************************************
// NAME:           LoadFromFile
// PARAMETER:      XmlConfig XML configuration instance.
// RETURN VALUE:   none
// DESCRIPTION:    Loads this group's configuration settings using the XML configuration
//                 instance.
//
//***************************************************************************************
procedure TTransportXcpUsbConfig.LoadFromFile(XmlConfig: TXMLConfig);
begin
  // Open this group's key.
  XmlConfig.OpenKey(UnicodeString(Self.Name));
  // Load all settings.
  // USB transport layer currently does not require any additional settings.
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
procedure TTransportXcpUsbConfig.SaveToFile(XmlConfig: TXMLConfig);
begin
  // Open this group's key.
  XmlConfig.OpenKey(UnicodeString(Self.Name));
  // Store all settings.
  // USB transport layer currently does not require any additional settings.
  // Close this group's key.
  XmlConfig.CloseKey;
end; //*** end of SaveToFile ***


//---------------------------------------------------------------------------------------
//-------------------------------- TTransportXcpTcpIpConfig -----------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor.
//
//***************************************************************************************
constructor TTransportXcpTcpIpConfig.Create;
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
procedure TTransportXcpTcpIpConfig.Defaults;
begin
  FAddress := '192.168.178.23';
  FPort := 1000;
end; //*** end of Defaults ***


//***************************************************************************************
// NAME:           LoadFromFile
// PARAMETER:      XmlConfig XML configuration instance.
// RETURN VALUE:   none
// DESCRIPTION:    Loads this group's configuration settings using the XML configuration
//                 instance.
//
//***************************************************************************************
procedure TTransportXcpTcpIpConfig.LoadFromFile(XmlConfig: TXMLConfig);
begin
  // Open this group's key.
  XmlConfig.OpenKey(UnicodeString(Self.Name));
  // Load all settings.
  FAddress := String(XmlConfig.GetValue('address', UnicodeString(FAddress)));
  FPort := XmlConfig.GetValue('port', FPort);
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
procedure TTransportXcpTcpIpConfig.SaveToFile(XmlConfig: TXMLConfig);
begin
  // Open this group's key.
  XmlConfig.OpenKey(UnicodeString(Self.Name));
  // Store all settings.
  XmlConfig.SetValue('address', UnicodeString(FAddress));
  XmlConfig.SetValue('port', FPort);
  // Close this group's key.
  XmlConfig.CloseKey;
end; //*** end of SaveToFile ***


end.
//******************************** end of configgroups.pas ******************************

