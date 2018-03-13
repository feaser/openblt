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
    procedure LoadFromFile(xmlConfig: TXMLConfig); override;
    procedure SaveToFile(xmlConfig: TXMLConfig); override;
    property Width: Integer read FWidth write FWidth;
    property Height: Integer read FHeight write FHeight;
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
// PARAMETER:      xmlConfig XML configuration instance.
// RETURN VALUE:   none
// DESCRIPTION:    Loads this group's configuration settings using the XML configuration
//                 instance.
//
//***************************************************************************************
procedure TMainWindowConfig.LoadFromFile(xmlConfig: TXMLConfig);
begin
  // Open this group's key.
  xmlConfig.OpenKey(WideString(Self.Name));
  // Load all settings.
  FWidth := xmlConfig.GetValue('width', FWidth);
  FHeight := xmlConfig.GetValue('height', FHeight);
  // Close this group's key.
  xmlConfig.CloseKey;
end; //*** end of LoadFromFile ***/


//***************************************************************************************
// NAME:           SaveToFile
// PARAMETER:      xmlConfig XML configuration instance.
// RETURN VALUE:   none
// DESCRIPTION:    Saves this group's configuration settings using the XML configuration
//                 instance.
//
//***************************************************************************************
procedure TMainWindowConfig.SaveToFile(xmlConfig: TXMLConfig);
begin
  // Open this group's key.
  xmlConfig.OpenKey(WideString(Self.Name));
  // Store all settings.
  xmlConfig.SetValue('width', FWidth);
  xmlConfig.SetValue('height', FHeight);
  // Close this group's key.
  xmlConfig.CloseKey;
end; //*** end of SaveToFile ***


end.
//******************************** end of configgroups.pas ******************************

