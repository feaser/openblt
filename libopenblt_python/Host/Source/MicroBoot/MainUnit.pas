unit MainUnit;
//***************************************************************************************
// Project Name: MicroBoot for Borland Delphi
//  Description: Contains the GUI for MicroBoot
//    File Name: MainUnit.pas
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
  ExtCtrls, StdCtrls, Menus, ComCtrls, uBootInterface, Registry, SettingsUnit, StopWatch;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  TmainForm = class(TForm)
    pnlHeader: TPanel;
    imgHeader: TImage;
    lblAppName: TLabel;
    lblInterfaceName: TLabel;
    bvlFooter: TBevel;
    btnCancel: TButton;
    btnSettings: TButton;
    ntbPages: TNotebook;
    edtDownloadFile: TEdit;
    btnBrowse: TButton;
    lblDownloadFile: TLabel;
    prgDownload: TProgressBar;
    lblDownloadProgress: TLabel;
    OpenDialog: TOpenDialog;
    Timer: TTimer;
    lblElapsedTime: TLabel;
    procedure btnCancelClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure btnBrowseClick(Sender: TObject);
    procedure btnSettingsClick(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure TimerTimer(Sender: TObject);
    procedure edtDownloadFileKeyPress(Sender: TObject; var Key: Char);
  private
    { Private declarations }
    MbiLogging    : Boolean;
    MbiLibFile    : ShortString;
    MbiInterfaced : Boolean;
    MbiInterface  : TMicroBootInterface;
    LogLines      : TStrings;
    ExePath       : string;
    StopWatch     : TStopWatch;
    StayOpen      : Boolean;
    FormCaption   : string;
    DownloadInProgress: Boolean;
    procedure OnMbiStarted(length: Longword);
    procedure OnMbiProgress(progress: Longword);
    procedure OnMbiDone;
    procedure OnMbiError(error: ShortString);
    procedure OnMbiLog(info:  ShortString);
    procedure OnMbiInfo(info:  ShortString);
    procedure StartFileDownload(fileName : ShortString);
    procedure UpdateInterfaceLabel;
    procedure ResetUserInterface;
  public
    { Public declarations }
    function  IsMbiInterface(libFile : string) : Boolean;
    function  GetMbiInfoString(libFile : string) : string;
    function  GetActiveMbi : string;
    procedure SetActiveMbi(libFile : string);
    procedure ConfigureMbi;
    procedure GetInterfaceFileList(fileList : TStrings);
  end;


//***************************************************************************************
// Global Variables
//***************************************************************************************
var
  mainForm: TmainForm;

implementation

{$R *.DFM}

//***************************************************************************************
// NAME:           OnMbiStarted
// PARAMETER:      length of the download in bytes.
// RETURN VALUE:   none
// DESCRIPTION:    Called by the Mbi interface DLL after successfully starting a down-
//                 load. The value of the length parameter can be used to set the max
//                 value of the progress bar.
//
//***************************************************************************************
procedure TmainForm.OnMbiStarted(length: Longword);
begin
  prgDownload.Max := length; // set max length for progress bar
  lblElapsedTime.Caption := 'Elapsed time: ' + StopWatch.Interval;
  StopWatch.Start; // start the stopwatch
  Timer.Enabled := true; // start the timer to update the stopwatch interval display
end; //*** end of OnMbiStarted ***


//***************************************************************************************
// NAME:           OnMbiProgress
// PARAMETER:      number of already downloaded bytes.
// RETURN VALUE:   none
// DESCRIPTION:    Called by the Mbi interface DLL to provide us with an update on the
//                 download progress. The progress parameter can be used to update the
//                 position of the progress bar.
//
//***************************************************************************************
procedure TmainForm.OnMbiProgress(progress: Longword);
begin
  prgDownload.Position := progress; // update the progress bar
  prgDownload.Position := progress-1; // fix for progress bar not going to 100%
  prgDownload.Position := progress; // update the progress bar
end; //*** end of OnMbiProgress ***


//***************************************************************************************
// NAME:           OnMbiDone
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Called by the Mbi interface DLL when a download was successfully
//                 completed.
//
//***************************************************************************************
procedure TmainForm.OnMbiDone;
begin
  DownloadInProgress := False; // reset flag
  Timer.Enabled := false; // stop the timer
  StopWatch.Stop; // stop the stopwatch
  mainForm.Caption := FormCaption; // restore caption

  if StayOpen then
    ResetUserInterface // reset the user interface to allow a new download to be started
  else
    Close; // done so close the application
end; //*** end of OnMbiDone ***


//***************************************************************************************
// NAME:           OnMbiError
// PARAMETER:      additional info on the error that occurred in string format.
// RETURN VALUE:   none
// DESCRIPTION:    Called by the Mbi interface DLL in case an error occurred. The para-
//                 meter contains more information on the error.
//
//***************************************************************************************
procedure TmainForm.OnMbiError(error: ShortString);
begin
  DownloadInProgress := False; // reset flag
  ShowMessage(String(error)); // display error
  Timer.Enabled := false; // stop the timer
  StopWatch.Stop; // stop the stopwatch
  mainForm.Caption := FormCaption; // restore caption
  ResetUserInterface; // download failed so reset user interface for retry
end; //*** end of OnMbiError ***


//***************************************************************************************
// NAME:           OnMbiLog
// PARAMETER:      info on the log event in string format.
// RETURN VALUE:   none
// DESCRIPTION:    Called by the Mbi interface DLL in case info for logging purpose
//                 was made available by the DLL.
//
//***************************************************************************************
procedure TmainForm.OnMbiLog(info:  ShortString);
begin
  if MbiLogging = True then
  begin
    LogLines.Add(String(info)); // add to log
  end;
end; //*** end of OnMbiLog ***


//***************************************************************************************
// NAME:           OnMbiInfo
// PARAMETER:      details on the info event in string format.
// RETURN VALUE:   none
// DESCRIPTION:    Called by the Mbi interface DLL in case details for info purposes
//                 were made available by the DLL.
//
//***************************************************************************************
procedure TmainForm.OnMbiInfo(info:  ShortString);
begin
  if NtbPages.PageIndex = 1 then
    lblDownloadProgress.Caption := String(info);
end; //*** end of OnMbiLog ***


//***************************************************************************************
// NAME:           GetActiveMbi
// PARAMETER:      none
// RETURN VALUE:   filename with full path
// DESCRIPTION:    Returns the file name with full path of the active Mbi interface
//                 library
//
//***************************************************************************************
function  TmainForm.GetActiveMbi : string;
begin
  if IsMbiInterface(String(MbiLibFile)) then
    Result := String(MbiLibFile)
  else
    Result := '';
end; //*** end of GetActiveMbi ***


//***************************************************************************************
// NAME:           SetActiveMbi
// PARAMETER:      filename with full path
// RETURN VALUE:   none
// DESCRIPTION:    Enables the Mbi interface library that is specified as the parameter.
//
//***************************************************************************************
procedure TmainForm.SetActiveMbi(libFile : string);
begin
  MbiInterfaced := false; // reset

  if IsMbiInterface(libFile) then
  begin
    MbiLibFile := ShortString(libFile);
    MbiInterfaced := MbiInterface.Enable(libFile, OnMbiStarted, OnMbiProgress,
                                         OnMbiDone, OnMbiError, OnMbiLog, OnMbiInfo);
  end;

  UpdateInterfaceLabel;
end; //*** end of SetActiveMbi ***


//***************************************************************************************
// NAME:           ConfigureMbi
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Submits request to the Mbi interface library for the user to configure
//                 the interface.
//
//***************************************************************************************
procedure TmainForm.ConfigureMbi;
begin
  if MbiInterfaced = True then
  begin
    MbiInterface.Configure;
  end;
end; //*** end of ConfigureMbi ***


//***************************************************************************************
// NAME:           IsMbiInterface
// PARAMETER:      filename with full path of the Mbi interface DLL
// RETURN VALUE:   true if it is a valid Mbi interface DLL, otherwise false
// DESCRIPTION:    Called to check whether a specified interface DLL is truly an Mbi
//                 interface DLL.
//
//***************************************************************************************
function TmainForm.IsMbiInterface(libFile : string) : Boolean;
var
  LibHandle : THandle;
  LibValid : Boolean;
begin
  LibValid := False;

  // make sure the file exists
  if FileExists(libFile) then
  begin
    // make sure it is a DLL file
    if LowerCase(ExtractFileExt(libFile)) = '.dll' then
    begin
      // make sure the DLL file is a microBoot interface library
      LibHandle := LoadLibrary(PChar(ExtractShortPathName(libFile))); // get handle
      if LibHandle <> 0 then
      begin
        if GetProcAddress(LibHandle, 'MbiInit') <> nil then
        begin
          LibValid := True;
          FreeLibrary(LibHandle);
        end;
      end;
    end;
  end;
  Result := LibValid;
end;
//*** end of IsMbiInterface ***


//***************************************************************************************
// NAME:           GetMbiInfoString
// PARAMETER:      filename with full path of the Mbi interface DLL
// RETURN VALUE:   string that described the name and version of the Mbi interface DLL
// DESCRIPTION:    Used to obtain a string that describes the Mbi interface DLL.
//
//***************************************************************************************
function TmainForm.GetMbiInfoString(libFile : string) : string;
var
  LibHandle      : THandle;
  DescriptionFnc : TDllMbiDescription;
  VersionFnc     : TDllMbiVersion;
  Major          : integer;
  Minor          : integer;
  Bugfix         : integer;
begin
  Result := '';

  // make sure the file is a valid Mbi interface library
  if IsMbiInterface(libFile) then
  begin
    LibHandle := LoadLibrary(PChar(ExtractShortPathName(libFile))); // get handle
    if LibHandle <> 0 then
    begin
      // obtain DLL function pointers
      @DescriptionFnc := GetProcAddress(LibHandle, 'MbiDescription');
      @VersionFnc     := GetProcAddress(LibHandle, 'MbiVersion');

      if Assigned(DescriptionFnc) then
      begin
        Result := Result + String(DescriptionFnc);
      end;

      if Assigned(VersionFnc) then
      begin
        // split up version numbers
        Major  := VersionFnc div 10000;
        Minor  := (versionFnc mod 10000) div 100;
        Bugfix := (versionFnc mod 100);

        Result := Result + ' (' + Format('v%d.%2.2d.%2.2d', [Major, Minor, Bugfix]) + ')';
      end;
      FreeLibrary(LibHandle);
    end;
  end;
end; //*** end of GetMbiInfoString ***


//***************************************************************************************
// NAME:           GetInterfaceFileList
// PARAMETER:      string list where filelist will be stored
// RETURN VALUE:   none
// DESCRIPTION:    Searches all the DLL files in the directory where the program's EXE
//                 runs from. If the found DLL file is a valib Mbi interface library,
//                 then it is added to the list.
//
//***************************************************************************************
procedure TmainForm.GetInterfaceFileList(fileList : TStrings);
var
  SR  : TSearchRec;
begin
  // search all dll's in the applicatioin's directory
  fileList.BeginUpdate;
  if FindFirst(ExePath + '*.dll', faAnyFile, SR) = 0 then
  begin
    repeat
      if (SR.Attr <> faDirectory) then
      begin
        if IsMbiInterface(ExePath + SR.Name) = True then
        fileList.Add(SR.Name);
      end;
    until FindNext(SR) <> 0;
    FindClose(SR);
  end;
  fileList.EndUpdate;
end; //*** end of GetInterfaceFileList ***


//***************************************************************************************
// NAME:           StartFileDownload
// PARAMETER:      file that is to be downloaded
// RETURN VALUE:   none
// DESCRIPTION:    Initiates the file download. The file is verified for existence and if
//                 all is okay, the next page is shown and the download is started.
//
//***************************************************************************************
procedure TmainForm.StartFileDownload(fileName : ShortString);
begin
  if FileExists(String(fileName)) and (MbiInterfaced = True) then
  begin
    mainForm.Caption := FormCaption + ' - Downloading ' +
                        ExtractFileName(String(fileName)) + '...';
    prgDownload.Position := 0; // reset the progress bar
    NtbPages.PageIndex := 1; // go to the next page
    btnSettings.Enabled := false; // settings can't be changed anymore
    btnCancel.Caption := 'Cancel';   // change caption to cancel download
    DownloadInProgress := True; // set flag
    MbiInterface.Download(fileName);
  end;
end; //*** end of StartFileDownload ***


//***************************************************************************************
// NAME:           UpdateInterfaceLabel
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Updates the interface label caption based on the active Mbi interface
//                 library
//
//***************************************************************************************
procedure TmainForm.UpdateInterfaceLabel;
begin
  // display interface library description
  if MbiInterfaced = True then
  begin
    lblInterfaceName.Caption := 'for ' + String(MbiInterface.Description);
  end
  else
  begin
    lblInterfaceName.Caption := 'Error - No Interface Library Loaded';
  end;
end; //*** end of UpdateInterfaceLabel ***


//***************************************************************************************
// NAME:
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Resets the user interface to the default state, which is the state
//                 when the program is started for the first time.
//
//***************************************************************************************
procedure TmainForm.ResetUserInterface;
begin
  // stop the timer
  Timer.Enabled := False;
  // stop the stopwatch
  StopWatch.Stop;
  // restore form caption
  mainForm.Caption := FormCaption;
  // clear download file
  edtDownloadFile.Text := '';
  // go to the default page
  NtbPages.PageIndex := 0;
  // enable settings button
  btnSettings.Enabled := True;
  // change caption to exit program
  btnCancel.Caption := 'Exit';
  // empty elapsted time label
  lblElapsedTime.Caption := '';
end; //*** end of ResetUserInterface ***


//***************************************************************************************
// NAME:           btnCancelClick
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Exits the application
//
//***************************************************************************************
procedure TmainForm.btnCancelClick(Sender: TObject);
begin
  // pass on cancel request to the library
  if MbiInterfaced = True then
  begin
    MbiInterface.Cancel;
  end;

  // no download in progress so just close the program
  if not DownloadInProgress then
  begin
    Close;
  end
end; //*** end of btnCancelClick ***


//***************************************************************************************
// NAME:           FormCreate
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Initializes all the class properties and attempts to search and enable
//                 the Mbi interface library.
//
//***************************************************************************************
procedure TmainForm.FormCreate(Sender: TObject);
var
  cnt            : integer;
  foundInterface : boolean;
  foundLibrary   : string;
  winRegistry    : TRegistry;
  libFileList    : TStrings;
begin
  btnCancel.Caption := 'Exit';   // change caption to exit program
  DownloadInProgress := False; // init flag
  FormCaption := mainForm.Caption; // backup original caption
  LogLines := TStringList.Create;
  StayOpen := false;
  MbiLogging := false;
  MbiInterfaced := false; // Mbi interface not enabled at startup
  MbiLibFile := ''; // reset lib file
  MbiInterface := TMicroBootInterface.Create(Self); // create instance
  foundInterface := false; // init before searching
  ExePath := ExtractFilePath(Application.ExeName);

  // determine if logging should be enabled
  if (ParamCount > 0) then
  begin
    // no options will be in Param 0
    for cnt := 1 to ParamCount do
    begin
      // look for -l option
      if System.Pos('-l', ParamStr(cnt)) > 0 then
      begin
        MbiLogging := True;
      end;
    end;
  end;

  // determine if tool should stay open after a download completion
  if (ParamCount > 0) then
  begin
    // no options will be in Param 0
    for cnt := 1 to ParamCount do
    begin
      // look for -s option
      if System.Pos('-s', ParamStr(cnt)) > 0 then
      begin
        StayOpen := True;
      end;
    end;
  end;

  // determine what interface library to use on startup
  // 1) -------- From commandline parameter ---------------
  foundLibrary  := '';
  // parameters okay, now extract the command line options if any
  if (ParamCount > 0) then
  begin
    // no options will be in Param 0
    for cnt := 1 to ParamCount do
    begin
      // look for -i option
      if System.Pos('-i', ParamStr(cnt)) > 0 then
      begin
        foundLibrary := ExePath + System.Copy(ParamStr(cnt),
                        System.Pos('-i', ParamStr(cnt))+2, Length(ParamStr(cnt)));
      end;
    end;
  end;

  // interface library specified on the commandline?
  if foundLibrary <> '' then
  begin
    // is it a valid Mbi interface library?
    if IsMbiInterface(foundLibrary) = True then
    begin
      MbiLibFile := ShortString(foundLibrary);
      foundInterface := True;
    end;
  end;

  // 2) -------- From registry ---------------
  if not foundInterface then
  begin
    // open registry key
    winRegistry := TRegistry.Create;
    winRegistry.RootKey := HKEY_CURRENT_USER;
    winRegistry.OpenKeyReadOnly('Software\Feaser\MicroBoot');

    // attempt to read out the stored interface filename (without path)
    if winRegistry.ReadString('Interface') <> '' then
    begin
      // obtain the interface library file name from the registry key
      foundLibrary := ExePath + winRegistry.ReadString('Interface');

      // is it a valid Mbi interface library?
      if IsMbiInterface(foundLibrary) = True then
      begin
        MbiLibFile := ShortString(foundLibrary);
        foundInterface := True;
      end;
    end;
    winRegistry.Free; // registry access no longer needed
  end;

  // 3) -------- first interface library found ---------------
  if not foundInterface then
  begin
    libFileList := TStringList.Create;
    libFileList.Clear;
    GetInterfaceFileList(libFileList);
    if libFileList.Count > 0 then
    begin
      foundLibrary := ExePath + libFileList.Strings[0];
      // is it a valid Mbi interface library?
      if IsMbiInterface(foundLibrary) = True then
      begin
        MbiLibFile := ShortString(foundLibrary);
        foundInterface := True;
      end;
    end;
    libFileList.Free;
  end;

  // did we find a Mbi interface library?
  if foundInterface = True then
  begin
    SetActiveMbi(String(MbiLibFile));
  end;

  // create the stopwatch timer
  StopWatch := TStopWatch.Create;
end; //*** end of FormCreate ***


//***************************************************************************************
// NAME:           FormDestroy
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    DeInitializes all the class properties that where instanciated.
//
//***************************************************************************************
procedure TmainForm.FormDestroy(Sender: TObject);
begin
  MbiInterface.Free; // release the interface
  LogLines.Free;

  // release the stopwatch timer
  StopWatch.Free;
end; //*** end of FormDestroy ***


//***************************************************************************************
// NAME:           FormShow
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Loads and displays the interface library description. If a valid
//                 download file is selected as a command line parameter the download
//                 is started right away. If the -p command line parameter was specified,
//                 then the open file dialog is displayed automatically.
//
//***************************************************************************************
procedure TmainForm.FormShow(Sender: TObject);
var
  cnt : integer;
begin
  UpdateInterfaceLabel;

  // was an existing download file specified as a command line param?
  if (ParamCount > 0) and (FileExists(ParamStr(ParamCount))) then
  begin
    edtDownloadFile.Text := ParamStr(ParamCount);
    StartFileDownload(ShortString(ParamStr(ParamCount)));
    Exit; // nothing more todo
  end;

  // was the -p command line option specified?
  // parameters okay, now extract the command line options if any
  if (ParamCount > 0) then
  begin
    // no options will be in Param 0
    for cnt := 1 to ParamCount do
    begin
      // look for -p option
      if System.Pos('-p', ParamStr(cnt)) > 0 then
      begin
        if OpenDialog.Execute then
        begin
          if FileExists(OpenDialog.FileName) then
          begin
            edtDownloadFile.Text := OpenDialog.FileName;
            StartFileDownload(ShortString(OpenDialog.FileName));
          end;
        end;
      end;
    end;
  end;
end; //*** end of FormShow ***


//***************************************************************************************
// NAME:           btnBrowseClick
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Prompts the user to select a file to download.
//
//***************************************************************************************
procedure TmainForm.btnBrowseClick(Sender: TObject);
begin
  if OpenDialog.Execute then
  begin
    if FileExists(OpenDialog.FileName) then
    begin
      edtDownloadFile.Text := OpenDialog.FileName;
      StartFileDownload(ShortString(OpenDialog.FileName));
    end;
  end;
end; //*** end of btnBrowseClick ***


//***************************************************************************************
// NAME:           btnSettingsClick
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Opens the settings form where the user can select and configure the
//                 Mbi interface library.
//
//***************************************************************************************
procedure TmainForm.btnSettingsClick(Sender: TObject);
var
  winRegistry    : TRegistry;
begin
  if SettingsForm.ShowModal = mrOK then
  begin
    if MbiInterfaced then
    begin
      // store last used library in register
      winRegistry := TRegistry.Create;
      winRegistry.RootKey := HKEY_CURRENT_USER;
      winRegistry.OpenKey('Software\Feaser\MicroBoot', true);
      winRegistry.WriteString('Interface', ExtractFileName(String(MbiLibFile)));
      winRegistry.Free;
    end;
    UpdateInterfaceLabel;
  end;
end; //*** end of btnSettingsClick ***


//***************************************************************************************
// NAME:           FormClose
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Saves the log to a file before closing the application.
//
//***************************************************************************************
procedure TmainForm.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  // save the log to a file before closing the app
  if MbiLogging = True then
  begin
    LogLines.SaveToFile(ExePath + 'log.txt');
  end;

  // pass on cancel request to the library if a download is in progress
  if MbiInterfaced = True then
  begin
    MbiInterface.Cancel;
  end;
end; //*** end of FormClose ***


//***************************************************************************************
// NAME:           TimeTimer
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Timer event handler to update stopwatch info
//
//***************************************************************************************
procedure TmainForm.TimerTimer(Sender: TObject);
begin
  lblElapsedTime.Caption := 'Elapsed time: ' + StopWatch.Interval;
end; //*** end of TimerTimer ***

procedure TmainForm.edtDownloadFileKeyPress(Sender: TObject;
  var Key: Char);
begin
  // filter out enter key
  if key = #13 then
  begin
    // ignore further enter key processing
    key := #0;

    // start the download
    if FileExists(edtDownloadFile.Text) then
    begin
      StartFileDownload(ShortString(edtDownloadFile.Text));
    end;

  end;
end;

end.
//******************************** end of MainUnit.pas **********************************

