unit CurrentConfig;
//***************************************************************************************
//  Description: Program configuration management and persistency.
//    File Name: currentconfig.pas
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
  Classes, SysUtils, Fgl, XMLConf, LazFileUtils;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  //------------------------------ TConfigGroup -----------------------------------------
  TConfigGroup = class (TObject)
  protected
    FName: String;
  public
    procedure Defaults; virtual; abstract;
    procedure LoadFromFile(XmlConfig: TXMLConfig); virtual; abstract;
    procedure SaveToFile(XmlConfig: TXMLConfig); virtual; abstract;
    property Name: String read FName;
  end;

  //------------------------------ TConfigGroupList -------------------------------------
  TConfigGroupList = specialize TFPGObjectList<TConfigGroup>;

  //------------------------------ TCurrentConfig ---------------------------------------
  TCurrentConfig = class (TObject)
  private
    FConfigFile: String;
    FGroups: TConfigGroupList;
    function    GetGroup(Name: String): TConfigGroup;
  public
    constructor Create;
    destructor  Destroy; override;
    procedure   LoadFromFile;
    procedure   SaveToFile;
    procedure   AddGroup(Group: TConfigGroup);
    property    ConfigFile: String read FConfigFile;
    property    Groups[Name: String]: TConfigGroup read GetGroup;
  end;


implementation
//---------------------------------------------------------------------------------------
//-------------------------------- TCurrentConfig ---------------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor.
//
//***************************************************************************************
constructor TCurrentConfig.Create;
begin
  // Call inherited constructor.
  inherited Create;
  // Set fields.
  FConfigFile := GetAppConfigFile(False, True);
  // Validate the configuration file.
  Assert(FConfigFile <> '', 'Could not get application configuration filename.');
  // Create instance of the groups list.
  FGroups := TConfigGroupList.Create;
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Destroy
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class destructor.
//
//***************************************************************************************
destructor TCurrentConfig.Destroy;
begin
  // Free the groups list instance. Note that this automatically frees the config groups
  // in the list.
  FGroups.Free;
  // call inherited destructor
  inherited Destroy;
end; //*** end of Destroy ***


//***************************************************************************************
// NAME:           LoadFromFile
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Loads the program's configuration from the configuration file.
//
//***************************************************************************************
procedure TCurrentConfig.LoadFromFile;
var
  idx: Integer;
  xmlConfig: TXMLConfig;
begin
  // Loop through all groups to set defaults just in case the configuration file does
  // no exist.
  for idx := 0 to (FGroups.Count - 1) do
  begin
    // Request group to load its settings from the configuration file.
    FGroups[idx].Defaults;
  end;

  // Check that the configuration file exists.
  if FileExists(configFile) then
  begin
    // Construct XML configuration object.
    xmlConfig := TXMLConfig.Create(nil);
    xmlConfig.Filename := configFile;
    // Loop through all groups.
    for idx := 0 to (FGroups.Count - 1) do
    begin
      // Request group to load its settings from the configuration file.
      FGroups[idx].LoadFromFile(xmlConfig);
    end;
    // Release the XML configuration object.
    xmlConfig.Free;
  end;
end; //*** end of LoadFromFile ***


//***************************************************************************************
// NAME:           SaveToFile
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Saves the program's configuration to the configuration file.
//
//***************************************************************************************
procedure TCurrentConfig.SaveToFile;
var
  idx: Integer;
  configDir: String;
  xmlConfig: TXMLConfig;
begin
  // Extract the directory of the config file.
  configDir := ExtractFilePath(FConfigFile);
  // Validate the directory.
  Assert(configDir <> '', 'Configuration directory is invalid.');
  // Double check that the directory is actually there.
  if not DirectoryExists(configDir) then
  begin
    // Force the directory creation.
    ForceDirectories(configDir);
  end;
  // Only save settings if the directory is there and is writable.
  if DirectoryExists(configDir) and DirectoryIsWritable(configDir) then
  begin
    // Construct XML configuration object.
    xmlConfig := TXMLConfig.Create(nil);
    xmlConfig.Filename := configFile;
    // Loop through all groups.
    for idx := 0 to (FGroups.Count - 1) do
    begin
      // Request group to save its settings to the configuration file.
      FGroups[idx].SaveToFile(xmlConfig);
    end;
    // Write and release the XML configuration object.
    xmlConfig.Flush;
    xmlConfig.Free;
  end;
end; //*** end of SaveToFile ***


//***************************************************************************************
// NAME:           AddGroup
// PARAMETER:      Group The configuration group to add.
// RETURN VALUE:   none
// DESCRIPTION:    Adds a configuration group under management of the current
//                 configuration.
//
//***************************************************************************************
procedure TCurrentConfig.AddGroup(Group: TConfigGroup);
begin
  // Check parameters.
  Assert(Group <> nil, 'Invalid group specified as a parameter.');
  // Add the group.
  FGroups.Add(Group);
end; //*** end of AddGroup ***


//***************************************************************************************
// NAME:           GetGroup
// PARAMETER:      Name Name of the configuration group to obtain.
// RETURN VALUE:   Configuration group.
// DESCRIPTION:    Obtains the configuration group based on the specified name.
//
//***************************************************************************************
function TCurrentConfig.GetGroup(Name: String): TConfigGroup;
var
  idx: Integer;
begin
  // Initialize the result value.
  Result := nil;
  // Check parameters.
  Assert(Name <> '', 'Group name can not be empty.');
  // Loop through all groups.
  for idx := 0 to (FGroups.Count - 1) do
  begin
    // Is this the group we are looking for?
    if FGroups[idx].Name = Name then
    begin
      // Set the result value.
      Result := FGroups[idx];
      // No need to continue looping.
      Break;
    end;
  end;
  // Verify the result value.
  Assert(Result <> nil, 'Invalid group name specified.');
end; //*** end of GetGroup ***


end.
//******************************** end of currentconfig.pas *****************************

