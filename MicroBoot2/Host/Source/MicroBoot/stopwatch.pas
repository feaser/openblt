unit StopWatch;
//***************************************************************************************
//  Description: StopWatch timer for counting minutes and seconds.
//    File Name: stopwatch.pas
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
  Classes, SysUtils, ExtCtrls;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  //------------------------------ TStopWatchUpdateEvent --------------------------------
  TStopWatchUpdateEvent = procedure(Sender: TObject; Interval: String) of object;

  //------------------------------ TStopWatch -------------------------------------------
  TStopWatch = class(TObject)
  private
    FStartTime: TDateTime;
    FRunning: Boolean;
    FInterval: String;
    FInternalTimer: TTimer;
    FUpdateEvent: TStopWatchUpdateEvent;
    function    GetInterval: String;
    procedure   InternalTimerOnTimer(Sender: TObject);
  public
    constructor Create;
    destructor  Destroy; override;
    procedure   Start;
    procedure   Stop;
    property    Interval: String read GetInterval;
    property    OnUpdate: TStopWatchUpdateEvent read FUpdateEvent write FUpdateEvent;
  end;


implementation
//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor
//
//***************************************************************************************
constructor TStopWatch.Create;
begin
  // Call inherited constructor.
  inherited Create;
  // Initialize variables.
  FRunning := False;
  FInterval := '';
  FUpdateEvent := nil;
  // Create timer instance.
  FInternalTimer := TTimer.Create(nil);
  // Configure the timer instance.
  FInternalTimer.Enabled := False;
  FInternalTimer.Interval := 100;
  FInternalTimer.OnTimer := @InternalTimerOnTimer;
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Destroy
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class destructor.
//
//***************************************************************************************
destructor TStopWatch.Destroy;
begin
  // Stop the stopwatch.
  Stop;
  // Release timer instance.
  FInternalTimer.Free;
  // Call inherited destructor.
  inherited Destroy;
end; //*** end of Destroy ***


//***************************************************************************************
// NAME:           Start
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Starts the stopwatch timer
//
//***************************************************************************************
procedure TStopWatch.Start;
begin
  // Store the start time.
  FStartTime := Time;
  // Start the stopwatch.
  FRunning := True;
  // Start the internal timer.
  FInternalTimer.Enabled := True;
end; //*** end of Start ***


//***************************************************************************************
// NAME:           Stop
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Stops the stopwatch timer
//
//***************************************************************************************
procedure TStopWatch.Stop;
begin
  // Stop the internal timer.
  FInternalTimer.Enabled := False;
  // Stop the stopwatch.
  FRunning := False;
end; //*** end of Stop ***


//***************************************************************************************
// NAME:           GetInterval
// PARAMETER:      none
// RETURN VALUE:   Stopwatch time as string in format [min]:[sec].
// DESCRIPTION:    Obtains the stopwatch time as a formatted string.
//
//***************************************************************************************
function TStopWatch.GetInterval : String;
var
  hr  : word;
  min : word;
  sec : word;
  ms  : word;
begin
  // Decode the elased stopwatch time.
  DecodeTime(Time-FStartTime, hr, min, sec, ms);
  // Check if stopwatch is running.
  if not FRunning then
  begin
    min := 0;
    sec := 0;
  end;
  // Update the formatted stopwatch time string.
  Result := Format('%2.2d:%2.2d', [min, sec]);
end; //*** end of GetInterval ***


//***************************************************************************************
// NAME:           InternalTimerOnTimer
// PARAMETER:      Sender Source of the event.
// RETURN VALUE:   none
// DESCRIPTION:    Event handler that gets called when the timer expires.
//
//***************************************************************************************
procedure TStopWatch.InternalTimerOnTimer(Sender: TObject);
begin
  // Trigger the OnUpdate method.
  if Assigned(FUpdateEvent) then
  begin
    FUpdateEvent(Self, GetInterval);
  end;
end; //*** end of InternalTimerOnTimer ***


end.
//******************************** end of stopwatch.pas *********************************

