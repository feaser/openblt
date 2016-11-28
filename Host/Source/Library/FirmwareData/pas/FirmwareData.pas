unit FirmwareData;
//***************************************************************************************
//  Description: Class for managing and manipulating firmware data.
//    File Name: FirmwareData.pas
//
//---------------------------------------------------------------------------------------
//                          C O P Y R I G H T
//---------------------------------------------------------------------------------------
//   Copyright (c) 2016 by Feaser    http://www.feaser.com    All rights reserved
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
  Windows, Messages, SysUtils, Classes, Math, Generics.Collections, Generics.Defaults;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  //---------------------------------- TDataSegment -------------------------------------
  TDataSegment = class(TObject)
  private
    // array with actual data bytes of the segment.
    FDataBytes: array of Byte;
    // base memory address for the data of this segment.
    FBaseAddress: Longword;
    // number of data bytes in this segment.
    FDataSize: Integer;
    procedure   SetBaseAddress(value: Longword);
    function    GetLastAddress: Longword;
    function    GetData(index: Integer): Byte;
    procedure   GrowDataArray(numOfBytesToAdd: Integer);
  public
    constructor Create;
    destructor  Destroy; override;
    procedure   Clear;
    function    Add(data: array of Byte; length: Integer; address: Longword): Boolean;
    function    Remove(length: Integer; address: Longword): Boolean;
    procedure   Dump;
    property Data[index: Integer]: Byte read GetData;
    property Size: Integer read FDataSize;
    property BaseAddress: Longword read FBaseAddress write SetBaseAddress;
    property LastAddress: Longword read GetLastAddress;
  end;

  //---------------------------------- TFirmwareFileType --------------------------------
  TFirmwareFileType =
  (
    FFT_UNKNOWN,
    FFT_SRECORD,
    FFT_BINARY
  );


  //---------------------------------- TFirmwareFileHandler -----------------------------
  TFirmwareFileHandler = class(TObject)
  type
    TFirmwareFileDataReadEvent = procedure(sender: TObject; data: array of Byte; length: Integer; address: Longword) of object;
  protected
    // event handler for when a chunk of data was read from the firmware file
    FOnDataRead: TFirmwareFileDataReadEvent;
  public
    constructor Create; virtual;
    function Load(firmwareFile: String): Boolean; virtual; abstract;
    function Save(firmwareFile: String; segments: TObjectList<TDataSegment>): Boolean; virtual; abstract;
    property OnDataRead: TFirmwareFileDataReadEvent read FOnDataRead write FOnDataRead;
  end;

  //---------------------------------- TSRecordFileHandler ------------------------------
  TSRecordFileHandler = class(TFirmwareFileHandler)
  type
    TSRecordLineType = (ltInvalid, ltS0, ltS1, ltS2, ltS3, ltS7, ltS8, ltS9);
  private
    FDataBytesPerLineOnSave: Integer;
    class function GetLineType(line: String): TSRecordLineType; static;
    function GetLineData(line: String; var data: array of Byte; var length: Integer; var address: Longword): Boolean;
    function ConstructLine(data: array of Byte; length: Integer; address: Longword): String;
  public
    constructor Create; override;
    function Load(firmwareFile: String): Boolean; override;
    function Save(firmwareFile: String; segments: TObjectList<TDataSegment>): Boolean; override;
    class function IsSRecordFile(firmwareFile: String): Boolean; static;
    property DataBytesPerLineOnSave: Integer read FDataBytesPerLineOnSave write FDataBytesPerLineOnSave;
  end;

  //---------------------------------- TBinaryFileHandler -------------------------------
  TBinaryFileHandler = class(TFirmwareFileHandler)
  private
  public
    constructor Create; override;
    function Load(firmwareFile: String): Boolean; override;
    function Save(firmwareFile: String; segments: TObjectList<TDataSegment>): Boolean; override;
  end;

  //---------------------------------- TFirmwareData ------------------------------------
  TFirmwareData = class(TObject)
  private
    // list with data segments of the firmware
    FSegmentList: TObjectList<TDataSegment>;
    function    GetSegmentCount: Integer;
    function    GetSegment(index: Integer): TDataSegment;
    procedure   SortSegments;
    function    FindSegmentIdx(address: Longword): Integer;
    function    FindPrevSegmentIdx(address: Longword): Integer;
    function    FindNextSegmentIdx(address: Longword): Integer;
    function    GetFirmwareFileType(firmwareFile: String): TFirmwareFileType;
    procedure   FirmwareFileDataRead(sender: TObject; data: array of Byte; length: Integer; address: Longword);
  public
    constructor Create;
    destructor  Destroy; override;
    function    AddData(data: array of Byte; length: Integer; address: Longword): Boolean;
    function    RemoveData(length: Integer; address: Longword): Boolean;
    procedure   ClearData;
    function    LoadFromFile(firmwareFile: String; append: Boolean): Boolean;
    function    SaveToFile(firmwareFile: String; firmwareFileType: TFirmwareFileType): Boolean;
    procedure   Dump;
    property SegmentCount: Integer read GetSegmentCount;
    property Segment[index: Integer]: TDataSegment read GetSegment;
  end;


implementation
//---------------------------------------------------------------------------------------
//-------------------------------- TDataSegment -----------------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor
//
//***************************************************************************************
constructor TDataSegment.Create;
begin
  // call inherited constructor
  inherited Create;
  // clear segment contents
  Clear;
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Destroy
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class destructor
//
//***************************************************************************************
destructor TDataSegment.Destroy;
begin
  // release allocated array memory
  SetLength(FDataBytes, 0);
  // call inherited destructor
  inherited;
end; //*** end of Destroy ***


//***************************************************************************************
// NAME:           SetBaseAddress
// PARAMETER:      value New base address.
// RETURN VALUE:   none
// DESCRIPTION:    Setter for base address.
//
//***************************************************************************************
procedure TDataSegment.SetBaseAddress(value: Longword);
begin
  FBaseAddress := value;
end; //*** end of SetBaseAddress ***


//***************************************************************************************
// NAME:           GetLastAddress
// PARAMETER:      none
// RETURN VALUE:   Last address.
// DESCRIPTION:    Getter for last address in the segment.
//
//***************************************************************************************
function TDataSegment.GetLastAddress: Longword;
begin
  Result := 0;
  if FDataSize > 0 then
    Result := (FBaseAddress + LongWord(FDataSize)) - 1;
end; //*** end of GetLastAddress ***


//***************************************************************************************
// NAME:           GetData
// PARAMETER:      index Index into the data byte array.
// RETURN VALUE:   Byte value.
// DESCRIPTION:    Getter for a byte value from the array at the specified index.
//
//***************************************************************************************
function TDataSegment.GetData(index: Integer): Byte;
begin
  Result := 0;
  if (index < FDataSize) and (index >= 0) then
    Result := FDataBytes[index];
end; //*** end of GetData ***


//***************************************************************************************
// NAME:           Clear
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Allocates more space to the data array if necessary. Allocation is
//                 done in chunks of DATA_ARRAY_GROWTH_STEP, because this is more
//                 run-time efficient.
//
//***************************************************************************************
procedure TDataSegment.GrowDataArray(numOfBytesToAdd: Integer);
const
  DATA_ARRAY_GROWTH_STEP: Integer = 1024;
var
  numOfBytesToGrow: Integer;
  numOfStepsToGrow: Integer;
  desiredArrayLength: Integer;
begin
  if numOfBytesToAdd > 0 then
  begin
    // check if more space needs to be allocated
    if Length(FDataBytes) < (FDataSize + numOfBytesToAdd) then
    begin
      // determine how many bytes the array needs to grow
      numOfBytesToGrow := (FDataSize + numOfBytesToAdd) - Length(FDataBytes);
      if numOfBytesToGrow > 0 then
      begin
        // determine how many growth steps to add
        numOfStepsToGrow := numOfBytesToGrow div DATA_ARRAY_GROWTH_STEP;
        if (numOfBytesToGrow mod DATA_ARRAY_GROWTH_STEP) > 0 then
          numOfStepsToGrow := numOfStepsToGrow + 1;
        // determine desired new array length
        desiredArrayLength := Length(FDataBytes) + (numOfStepsToGrow * DATA_ARRAY_GROWTH_STEP);
        // grow the array
        SetLength(FDataBytes, desiredArrayLength);
      end;
    end;
  end;
end; //*** end of GrowDataArray ***


//***************************************************************************************
// NAME:           Clear
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Clears all databytes from the segment and resets its base address.
//
//***************************************************************************************
procedure TDataSegment.Clear;
begin
  FBaseAddress := 0;
  FDataSize := 0;
  SetLength(FDataBytes, 0);
end; //*** end of Clear


//***************************************************************************************
// NAME:           Add
// PARAMETER:      data Array with bytes to add to the segment.
//                 length Number of bytes in the array.
//                 address Address where to start adding bytes at in the segment.
// RETURN VALUE:   True if the data was added to the segment, False if it couldn't be
//                 added. This latter situation happens if the data is not aligned to
//                 the data the is already present in the segment.
// DESCRIPTION:    Adds data bytes to the segment starting at the specified address. This
//                 function allows a new chunk of data to be added at the front or the
//                 rear of the segment, as well as overwriting existing data.
//
//***************************************************************************************
function TDataSegment.Add(data: array of Byte; length: Integer; address: Longword): Boolean;
var
  byteIdx: Integer;
  numBytesToAppend: Integer;
begin
  // init result
  Result := False;

  // check if there is something to add
  if length <= 0 then
    Exit;
  // the following checks assume there is already data in the segment
  if FDataSize > 0 then
  begin
    // check if the new data does not fit at the end
    if address > (GetLastAddress + 1) then
      Exit;
    // check if new data does not fit at the start
    if (address + Longword(length)) < FBaseAddress then
      Exit;
  end;

  // still here some there is something to add. check if the segment is currently empty
  if (FDataSize = 0) then
  begin
    // make sure enough elements are allocated in the data array
    GrowDataArray(length);
    // set the base address
    FBaseAddress := address;
    // add the data
    for byteIdx := 0 to (length - 1) do
      FDataBytes[byteIdx] := data[byteIdx];
    // set the new size
    FDataSize := length;
    // success
    Result := True;
  end
  // check if all data is for overwriting existing data
  else if (address >= FBaseAddress) and ((address + Longword(length - 1)) <= GetLastAddress) then
  begin
    // overwrite the data
    for byteIdx := 0 to (length - 1) do
      FDataBytes[(address - FBaseAddress) + Longword(byteIdx)] := data[byteIdx];
    // success
    Result := True;
  end
  // check if data should be appended at the end including partial overwrite at the end
  else if (address >= FBaseAddress) and ((address + Longword(length - 1)) > GetLastAddress) then
  begin
    // determine minimal required growth of the array
    numBytesToAppend := (address + Longword(length)) - (FBaseAddress + Longword(FDataSize));
    // make sure enough elements are allocated in the data array
    GrowDataArray(numBytesToAppend);
    // add the data
    for byteIdx := 0 to (length - 1) do
      FDataBytes[(address - FBaseAddress) + Longword(byteIdx)] := data[byteIdx];
    // set the new size
    FDataSize := FDataSize + numBytesToAppend;
    // success
    Result := True;
  end
  // check if data should be appended at the start including partial overwrite at the start
  else if (address < FBaseAddress) and ((address + Longword(length - 1)) <= GetLastAddress) then
  begin
    // determine minimal required growth of the array
    numBytesToAppend := FBaseAddress - address;
    // make sure enough elements are allocated in the data array
    GrowDataArray(numBytesToAppend);
    // set the base address
    FBaseAddress := address;
    // move current contents
    {for byteIdx := 0 to (FDataSize - 1) do
      FDataBytes[numBytesToAppend + byteIdx] := FDataBytes[byteIdx];}
    for byteIdx := (FDataSize - 1) downto 0 do
      FDataBytes[numbytesToAppend + byteIdx] := FDataBytes[byteIdx];
    // add the new data
    for byteIdx := 0 to (length - 1) do
      FDataBytes[byteIdx] := data[byteIdx];
    // set the new size
    FDataSize := FDataSize + numBytesToAppend;
    // success
    Result := True;
  end
  // check if data should be both appended at the start and the end. this is the case when
  // the to be added data is larger then the current segment and overlaps the entire current
  // segment
  else if (address < FBaseAddress) and ((address + Longword(length - 1)) > GetLastAddress) then
  begin
    // set the base address
    FBaseAddress := address;
    // make sure enough elements are allocated in the data array
    GrowDataArray(length);
    // add the new data. no need to first move current contents because they will be
    // fully overwritten anyways
    for byteIdx := 0 to (length - 1) do
      FDataBytes[byteIdx] := data[byteIdx];
    // set the new size
    FDataSize := length;
    // success
    Result := True;
  end;
end; //*** end of Add ***


//***************************************************************************************
// NAME:           Remove
// PARAMETER:      length Number of bytes to remove
//                 address Address where to start removing data from.
// RETURN VALUE:   True if the data was removed, False if the data could not be removed
//                 because this class cannot split a segment.
// DESCRIPTION:    Removes data from the segment. Note that the to be removed data
//                 must be aligned to the start or the end of the segment, because this
//                 class cannot split a segment.
//
//***************************************************************************************
function TDataSegment.Remove(length: Integer; address: Longword): Boolean;
var
  numOfBytesToRemove: Integer;
  byteIdx: Integer;
begin
  Result := True;

  // if there is nothing to remove then we are done already
  if (length <= 0) or (FDataSize = 0) then
  begin
    Exit;
  end;

  // if the data is not in this segment the we are also done already
  if (address > GetLastAddress) or ((address + Longword(length - 1)) < FBaseAddress) then
  begin
    Exit;
  end;

  // check if the to be removed data overlaps with either the end or the start of the
  // segment. if not, then we cannot remove the data because this class cannot split the
  // segment
  if (address > FBaseAddress) and ((address + Longword(length - 1)) < GetLastAddress) then
  begin
    Result := False;
    Exit;
  end;

  // check if the entire segment should be removed
  if (address <= FBaseAddress) and ((address + Longword(length - 1)) >= GetLastAddress) then
  begin
    Clear;
  end
  // check if the to be removed data is at the start of the segment
  else if (address <= FBaseAddress) then
  begin
    numOfBytesToRemove := (address + Longword(length)) - FBaseAddress;
    // move remaining data to the start of the array
    for byteIdx := 0 to (FDataSize - numOfBytesToRemove - 1) do
      FDataBytes[byteIdx] := FDataBytes[byteIdx + numOfBytesToRemove];
    // adjust size and base address
    FDataSize := FDataSize - numOfBytesToRemove;
    FBaseAddress := FBaseAddress + Longword(numOfBytesToRemove);
  end
  // check if the to be removed data is at the end of the segment
  else if (address > FBaseAddress) and ((address + Longword(length - 1)) >= GetLastAddress) then
  begin
    numOfBytesToRemove := GetLastAddress - address + 1;
    FDataSize := FDataSize - numOfBytesToRemove;
  end;
end; //*** end of Remove ***


//***************************************************************************************
// NAME:           Dump
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Dumps the segment contents to the standard output for debugging
//                 purposes.
//
//***************************************************************************************
procedure TDataSegment.Dump;
var
  line: String;
  byteCnt: Integer;
begin
  {$IFDEF DEBUG}
  // output address and size
  Writeln('Segment base address  = $' + Format('%.8X', [BaseAddress]));
  Writeln('Segment data size     = ' + IntToStr(Size));
  // output raw data
  Writeln('Segment data contents = ' + sLineBreak);
  line := '  ';
  for byteCnt := 1 to Size do
  begin
    line := line + Format('%.2X ', [Data[byteCnt - 1]]);
    if (byteCnt mod 16) = 0 then
    begin
      Writeln(line);
      line := '  ';
    end;
  end;
  Writeln(line);
  {$ENDIF}
end; //*** end of Dump


//---------------------------------------------------------------------------------------
//-------------------------------- TFirmwareFileHandler ---------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor
//
//***************************************************************************************
constructor TFirmwareFileHandler.Create;
begin
  // call inherited constructor
  inherited Create;
  // init fields
  FOnDataRead := nil;
end; //*** end of Create ***


//---------------------------------------------------------------------------------------
//-------------------------------- TSRecordFileHandler ----------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor
//
//***************************************************************************************
constructor TSRecordFileHandler.Create;
begin
  // call inherited constructor
  inherited Create;
  // set default number of data bytes to add to a line when saving an s-record
  FDataBytesPerLineOnSave := 16;
end; //*** end of Create ***

//***************************************************************************************
// NAME:           Load
// PARAMETER:      firmwareFile Filename with path of the file to load.
// RETURN VALUE:   True is successful, False otherwise.
// DESCRIPTION:    Loads the data in the specified firmware file. The OnDataRead event
//                 handler is called each time a chunk of data was read from the file.
//
//***************************************************************************************
function TSRecordFileHandler.Load(firmwareFile: String): Boolean;
var
  srecordFile: TextFile;
  line: String;
  lineData: array of Byte;
  lineLength: Integer;
  lineAddr: Longword;
begin
  // init result value and locals
  Result := True;

  // first check if the file actually exists
  if not FileExists(firmwareFile) then
  begin
    Result := False;
    Exit;
  end;

  // check if the event handler is configured, otherwise it is pointless to go through
  // the file
  if not Assigned(FOnDataRead) then
  begin
    Result := False;
    Exit;
  end;

  // create array with sufficient length
  SetLength(lineData, 1024);
  // go through the lines in the file to try and detect a line that is formatted as an
  // S-record. start by getting the file handle and going to the start of the file
  AssignFile(srecordFile, firmwareFile);
  Reset(srecordFile);
  // loop through the lines
  while not Eof(srecordFile) do
  begin
    // read the next line from the file
    ReadLn(srecordFile, line);
    // parse the line to extract the data bytes and address info
    if GetLineData(line, lineData, lineLength, lineAddr) then
    begin
      // invoke the event handler to inform about the new data
      FOnDataRead(Self, lineData, lineLength, lineAddr);
    end;
  end;
  // close the file
  CloseFile(srecordFile);
  // release array
  SetLength(lineData, 0);
end; //*** end of Load ***


//***************************************************************************************
// NAME:           Save
// PARAMETER:      firmwareFile Filename with path of the file to save.
//                 segments List with data segments that need to be saved.
// RETURN VALUE:   True is successful, False otherwise.
// DESCRIPTION:    Saves the firmware data to the specified firmware file.
//
//***************************************************************************************
function TSRecordFileHandler.Save(firmwareFile: String; segments: TObjectList<TDataSegment>): Boolean;
var
  srecordFile: TextFile;
  segmentIdx: Integer;
  byteIdx: Integer;
  line: String;
  programData: array of Byte;
  currentAddress: Longword;
  currentByteCnt: Integer;
  firmwareFileBytes: TBytes;
  headerByteCount: Integer;
  checksumCalc: Byte;
  addrByteCnt: Integer;
begin
  // init result
  Result := True;

  // check if there is actually something to write
  if segments.Count <= 0 then
  begin
    // no program data to write
    Result := False;
    Exit;
  end;

  // open the firmware file for writing
  AssignFile(srecordFile, firmwareFile);
  ReWrite(srecordFile);

  // ---- add the S0 header line that contains the filename ----
  firmwareFileBytes := TEncoding.UTF8.GetBytes(firmwareFile);
  headerByteCount := 3 + Length(firmwareFileBytes);
  line := 'S0' + Format('%.2X', [headerByteCount]) + '0000';
  for byteIdx := 0 to (Length(firmwareFileBytes) - 1) do
  begin
    line := line + Format('%.2X', [firmwareFileBytes[byteIdx]]);
  end;
  // compute checksum
  checksumCalc := 0;
  for byteIdx := 0 to (headerByteCount - 1) do
  begin
    checksumCalc := checksumCalc + StrToInt('$' + Copy(line, 3+(byteIdx*2), 2));
  end;
  // convert to one's complement and add it
  checksumCalc := not checksumCalc;
  line := line + Format('%.2X', [checksumCalc]);
  // add it to the file
  WriteLn(srecordFile, line);

  // ---- add the program data lines ----
  // init program data array
  SetLength(programData, DataBytesPerLineOnSave);
  // loop through all segments
  for segmentIdx := 0 to (segments.Count - 1) do
  begin
    // set current address and byte count
    currentAddress := segments[segmentIdx].BaseAddress;
    currentByteCnt := 0;
    // progress the data
    for byteIdx := 0 to (segments[segmentIdx].Size - 1) do
    begin
      // add the program data byte
      programData[currentByteCnt] := segments[segmentIdx].Data[byteIdx];
      currentByteCnt := currentByteCnt + 1;
      // check if desired program data bytes per line is reached
      if currentByteCnt = DataBytesPerLineOnSave then
      begin
        // construct the s-record line and add it to the file
        line := ConstructLine(programData, currentByteCnt, currentAddress);
        WriteLn(srecordFile, line);
        // refresh loop variables
        currentAddress := currentAddress + Longword(currentByteCnt);
        currentByteCnt := 0;
      end;
    end;
    // check if there are still bytes left to write to the file
    if currentByteCnt > 0 then
    begin
      // construct the s-record line and add it to the file
      line := ConstructLine(programData, currentByteCnt, currentAddress);
      WriteLn(srecordFile, line);
    end;
  end;

  // ---- add the termination line ----
  // determine the line type to use
  if segments[0].BaseAddress >= $FFFFFF then
  begin
    addrByteCnt := 4;
    line := 'S705' + Format('%.8X', [segments[0].BaseAddress]);
  end
  else if segments[0].BaseAddress >= $FFFF then
  begin
    addrByteCnt := 3;
    line := 'S804' + Format('%.6X', [segments[0].BaseAddress]);
  end
  else
  begin
    addrByteCnt := 2;
    line := 'S903' + Format('%.4X', [segments[0].BaseAddress]);
  end;
  // compute checksum
  checksumCalc := 0;
  for byteIdx := 0 to addrByteCnt do
  begin
    checksumCalc := checksumCalc + StrToInt('$' + Copy(line, 3+(byteIdx*2), 2));
  end;
  // convert to one's complement and add it
  checksumCalc := not checksumCalc;
  line := line + Format('%.2X', [checksumCalc]);
  WriteLn(srecordFile, line);

  // close the file
  CloseFile(srecordFile);
end; //*** end of Save ***


//***************************************************************************************
// NAME:           IsSRecordFile
// PARAMETER:      firmwareFile Filename with path of the file to check.
// RETURN VALUE:   True is the file has the S-Record format, False otherwise.
// DESCRIPTION:    Checks if the file contains data formatted as an S-Record.
//
//***************************************************************************************
class function TSRecordFileHandler.IsSRecordFile(firmwareFile: String): Boolean;
var
  srecordFile: TextFile;
  line: String;
begin
  // init result value and locals
  Result := False;

  // first check if the file actually exists
  if not FileExists(firmwareFile) then
    Exit;

  // go through the lines in the file to try and detect a line that is formatted as an
  // S-record. start by getting the file handle and going to the start of the file
  AssignFile(srecordFile, firmwareFile);
  Reset(srecordFile);
  // loop through the lines
  while not Eof(srecordFile) do
  begin
    ReadLn(srecordFile, line); // read line from file
    if (TSRecordFileHandler.GetLineType(line) = ltS1) or
       (TSRecordFileHandler.GetLineType(line) = ltS2) or
       (TSRecordFileHandler.GetLineType(line) = ltS3) then
    begin
      // valid S-Record
      Result := true;
      // no need to continue looping
      Break;
    end;
  end;
  // close the file
  CloseFile(srecordFile);
end; //*** end of IsSRecordFile ***


//***************************************************************************************
// NAME:           GetLineType
// PARAMETER:      Line from S-Record
// RETURN VALUE:   line type
// DESCRIPTION:    Determines what type of S-Record line we're dealing with.
//
//***************************************************************************************
class function TSRecordFileHandler.GetLineType(line: String): TSRecordLineType;
begin
  Result := ltInvalid;

  if Pos('S0', UpperCase(line)) > 0 then
  begin
   Result := ltS0;
   Exit;
  end;

  if Pos('S1', UpperCase(line)) > 0 then
  begin
    Result := ltS1;
    Exit;
  end;

  if Pos('S2', UpperCase(line)) > 0 then
  begin
    Result := ltS2;
    Exit;
  end;

  if Pos('S3', UpperCase(line)) > 0 then
  begin
    Result := ltS3;
    Exit;
  end;

  if Pos('S7', UpperCase(line)) > 0 then
  begin
    Result := ltS7;
    Exit;
  end;

  if Pos('S8', UpperCase(line)) > 0 then
  begin
    Result := ltS8;
    Exit;
  end;

  if Pos('S9', UpperCase(line)) > 0 then
  begin
   Result := ltS9;
   Exit;
  end;
end; //*** end of GetLineType ***


//***************************************************************************************
// NAME:           GetLineData
// PARAMETER:      line Line from S-Record.
//                 data Array where the data bytes are to be stored.
//                 length Storage for number of bytes that were read.
//                 address Storage for the address found on the S-Record line.
// RETURN VALUE:   True is successful, False otherwise
// DESCRIPTION:    Extracts the data bytes and address from the S-Record line.
//
//***************************************************************************************
function TSRecordFileHandler.GetLineData(line: String; var data: array of Byte; var length: Integer; var address: Longword): Boolean;
var
  lineType: TSRecordLineType;
  byteCount: Integer;
  byteIdx: Integer;
  checksumRead: Byte;
  checksumCalc: Byte;
  addrByteCnt: Integer;
begin
  // init result
  Result := True;
  // read out the line type
  lineType := TSRecordFileHandler.GetLineType(line);
  // set line type specific settings
  case lineType of
    ltS1:
      begin
        addrByteCnt := 2;
      end;
    ltS2:
      begin
        addrByteCnt := 3;
      end;
    ltS3:
      begin
        addrByteCnt := 4;
      end;
  else
    // line does not contain program data
    Result := False;
    Exit;
  end;

  // extract count value from the line
  byteCount := StrToInt('$' + Copy(line, 3, 2));
  // extract address
  address := StrToInt('$' + Copy(line, 5, addrByteCnt*2));
  // determine number of data bytes = total bytes - address - checksum
  length := byteCount - addrByteCnt - 1;
  // read the checksum
  checksumRead := StrToInt('$' + Copy(line, (5+(addrByteCnt*2))+(length*2), 2));
  // compute checksum
  checksumCalc := 0;
  for byteIdx := 0 to (byteCount - 1) do
  begin
    checksumCalc := checksumCalc + StrToInt('$' + Copy(line, 3+(byteIdx*2), 2));
  end;
  // convert to one's complement
  checksumCalc := not checksumCalc;
  // validate checksum
  if checksumCalc <> checksumRead then
  begin
    // line contains an invalid checksum
    Result := False;
    Exit;
  end;
  // read all the data bytes
  for byteIdx := 0 to (length - 1) do
  begin
    data[byteIdx] := StrToInt('$' + Copy(line, (5+(addrByteCnt*2))+(byteIdx*2), 2));
  end;
end; //*** end of GetLineData ***


//***************************************************************************************
// NAME:           ConstructLine
// PARAMETER:      data Array with data bytes.
//                 length Number of bytes in the array.
//                 address Base address of the data.
// RETURN VALUE:   The constructed line if successful, '' otherwise.
// DESCRIPTION:    Constructs an S-record line with program data.
//
//***************************************************************************************
function TSRecordFileHandler.ConstructLine(data: array of Byte; length: Integer; address: Longword): String;
var
  addrByteCnt: Integer;
  byteCount: Integer;
  addressStr: String;
  byteIdx: Integer;
  checksumCalc: Byte;
begin
  // determine the line type to use
  if address >= $FFFFFF then
  begin
    addrByteCnt := 4;
    addressStr := Format('%.8X', [address]);
    Result := 'S3';
  end
  else if address >= $FFFF then
  begin
    addrByteCnt := 3;
    addressStr := Format('%.6X', [address]);
    Result := 'S2';
  end
  else
  begin
    addrByteCnt := 2;
    addressStr := Format('%.4X', [address]);
    Result := 'S1';
  end;
  // determine number of bytes after the Sx, excluding checksum
  byteCount := addrByteCnt + length + 1;
  // add the count and address
  Result := Result + Format('%.2X', [byteCount]) + addressStr;
  // add all the data bytes
  for byteIdx := 0 to (length - 1) do
  begin
    Result := Result + Format('%.2X', [data[byteIdx]]);
  end;
  // compute checksum
  checksumCalc := 0;
  for byteIdx := 0 to (byteCount - 1) do
  begin
    checksumCalc := checksumCalc + StrToInt('$' + Copy(Result, 3+(byteIdx*2), 2));
  end;
  // convert to one's complement
  checksumCalc := not checksumCalc;
  // add the checksum
  Result := Result + Format('%.2X', [checksumCalc]);
end; //*** end of ConstructLine ***/


//---------------------------------------------------------------------------------------
//-------------------------------- TBinaryFileHandler -----------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor
//
//***************************************************************************************
constructor TBinaryFileHandler.Create;
begin
  // call inherited constructor
  inherited Create;
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Load
// PARAMETER:      firmwareFile Filename with path of the file to load.
// RETURN VALUE:   True is successful, False otherwise.
// DESCRIPTION:    Loads the data in the specified firmware file. The OnDataRead event
//                 handler is called each time a chunk of data was read from the file.
//
//***************************************************************************************
function TBinaryFileHandler.Load(firmwareFile: String): Boolean;
begin
  // loading from a binary file is not yet supported
  Result := False;
end; //*** end of Load ***


//***************************************************************************************
// NAME:           Save
// PARAMETER:      firmwareFile Filename with path of the file to save.
//                 segments List with data segments that need to be saved.
// RETURN VALUE:   True is successful, False otherwise.
// DESCRIPTION:    Saves the firmware data to the specified firmware file.
//
//***************************************************************************************
function TBinaryFileHandler.Save(firmwareFile: String; segments: TObjectList<TDataSegment>): Boolean;
var
  startAddr: Longword;
  endAddr: Longword;
  segmentIdx: Integer;
  progData: array of Byte;
  progLen: Longword;
  byteIdx: Longword;
  binaryFile: File;
begin
  // init result and locals
  Result := False;
  startAddr := $FFFFFFFF;
  endAddr := 0;

  // first need to determine the start and end addresses for the firmware data
  for segmentIdx := 0 to (segments.Count - 1) do
  begin
    if segments[segmentIdx].BaseAddress < startAddr then
      startAddr := segments[segmentIdx].BaseAddress;
    if segments[segmentIdx].LastAddress > endAddr then
      endAddr := segments[segmentIdx].LastAddress;
  end;

  // plausibility check
  if startAddr > endAddr then
    Exit;

  // calculate program length
  progLen := endAddr - startAddr + 1;

  // init array size such that it can hold all program data, including filler bytes
  // for possible
  SetLength(progData, progLen);
  // fill it completely with filler bytes
  for byteIdx := 0 to (progLen - 1) do
    progData[byteIdx] := $FF;

  // add the segment data to the program data array
  for segmentIdx := 0 to (segments.Count - 1) do
  begin
    // loop through segment data bytes one-by-one
    for byteIdx := 0 to (segments[segmentIdx].Size - 1) do
    begin
      // at the byte at the correct index
      progData[(segments[segmentIdx].BaseAddress - startAddr) + byteIdx] := segments[segmentIdx].Data[byteIdx];
    end;
  end;

  // open the firmware file for writing
  AssignFile(binaryFile, firmwareFile);
  // define a record to be of size 1 byte.
  ReWrite(binaryFile, 1);

  // write all program bytes one-by-one to the file
  for byteIdx := 0 to (progLen - 1) do
  begin
    BlockWrite(binaryFile, progData[byteIdx], 1);
  end;

  // clean up
  CloseFile(binaryFile);
  Result := True;
end; //*** end of Save ***


//---------------------------------------------------------------------------------------
//-------------------------------- TFirmwareData ----------------------------------------
//---------------------------------------------------------------------------------------
//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor
//
//***************************************************************************************
constructor TFirmwareData.Create;
begin
  // call inherited constructor
  inherited Create;
  // create empty data segments list and set it to own the segments for automatic freeing
  FSegmentList := TObjectList<TDataSegment>.Create();
  FSegmentList.OwnsObjects := True;
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Destroy
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class destructor
//
//***************************************************************************************
destructor TFirmwareData.Destroy;
begin
  // release the data segments list
  FSegmentList.Free;
  // call inherited destructor
  inherited;
end; //*** end of Destroy ***


//***************************************************************************************
// NAME:           GetSegmentCount
// PARAMETER:      none
// RETURN VALUE:   Count of data segments.
// DESCRIPTION:    Getter for the count of data segments with firmware data.
//
//***************************************************************************************
function TFirmwareData.GetSegmentCount;
begin
  Result := FSegmentList.Count;
end; //*** end of GetSegmentCount ***


//***************************************************************************************
// NAME:           GetSegment
// PARAMETER:      index Index of the data segment to get.
// RETURN VALUE:   Data segment if successful, nil otherwise.
// DESCRIPTION:    Getter for a data segment at the specified index.
//
//***************************************************************************************
function TFirmwareData.GetSegment(index: Integer): TDataSegment;
begin
  Result := nil;
  if (index >= 0) and (index < FSegmentList.Count) then
    Result := FSegmentList[index];
end; //*** end of GetSegment ***


//***************************************************************************************
// NAME:           SortSegments
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Sorts the segments based on the base address of the segment.
//
//***************************************************************************************
procedure TFirmwareData.SortSegments;
begin
  FSegmentList.Sort(TComparer<TDataSegment>.Construct(
   function (const L, R: TDataSegment): integer
   begin
     result := L.BaseAddress - R.BaseAddress;
   end
  ));
end; //*** end of SortSegments ***


//***************************************************************************************
// NAME:           FindSegmentIdx
// PARAMETER:      address Address to match
// RETURN VALUE:   Segment index if found, -1 otherwise.
// DESCRIPTION:    Searches for a segment that contains the specified address.
//
//***************************************************************************************
function TFirmwareData.FindSegmentIdx(address: Longword): Integer;
var
  segmentIdx: Integer;
begin
  Result := -1;
  // loop through segments
  for segmentIdx := 0 to (GetSegmentCount - 1) do
  begin
    // does this address fall into this segment?
    if (address >= FSegmentList[segmentIdx].BaseAddress) and (address <= FSegmentList[segmentIdx].LastAddress) then
    begin
      // match found
      Result := segmentIdx;
      // no need to continue loop
      Break;
    end;
  end;
end; //*** end of FindSegmentIdx ***


//***************************************************************************************
// NAME:           FindPrevSegmentIdx
// PARAMETER:      address Address to match
// RETURN VALUE:   Segment index if found, -1 otherwise.
// DESCRIPTION:    Searches for the previous segment. So a segment who's lastaddress is
//                 closest to the specified address.
//
//***************************************************************************************
function TFirmwareData.FindPrevSegmentIdx(address: Longword): Integer;
var
  segmentIdx: Integer;
begin
  Result := -1;
  // loop through segments and keep in mind that they are ordered by increasing memory
  // addresses
  for segmentIdx := (GetSegmentCount - 1) downto 0 do
  begin
    if FSegmentList[segmentIdx].LastAddress < address then
    begin
      // match found
      Result := segmentIdx;
      Break;
    end;
  end;
end; //*** end of FindPrevSegmentIdx ***


//***************************************************************************************
// NAME:           FindNextSegmentIdx
// PARAMETER:      address Address to match
// RETURN VALUE:   Segment index if found, -1 otherwise.
// DESCRIPTION:    Searches for the next segment. So a segment who's baseaddress is
//                 closest to the specified address.
//
//***************************************************************************************
function TFirmwareData.FindNextSegmentIdx(address: Longword): Integer;
var
  segmentIdx: Integer;
begin
  Result := -1;
  // loop through segments and keep in mind that they are ordered by increasing memory
  // addresses
  for segmentIdx := 0 to (GetSegmentCount - 1) do
  begin
    if FSegmentList[segmentIdx].BaseAddress > address then
    begin
      // match found
      Result := segmentIdx;
      Break;
    end;
  end;
end; //*** end of FindNextSegmentIdx ***


//***************************************************************************************
// NAME:           GetFirmwareFileType
// PARAMETER:      firmwareFile Filename with path of the file to check.
// RETURN VALUE:   The type of the firmware file.
// DESCRIPTION:    Determines the type of the firmware file.
//
//***************************************************************************************
function TFirmwareData.GetFirmwareFileType(firmwareFile: String): TFirmwareFileType;
begin
  // init result to unknown file type
  Result := FFT_UNKNOWN;

  // check if the file is formatted as an S-Record
  if TSRecordFileHandler.IsSRecordFile(firmwareFile) then
    Result := FFT_SRECORD;
end; //*** end of GetFirmwareFileType ***


//***************************************************************************************
// NAME:           FirmwareFileDataRead
// PARAMETER:      sender Object that triggered the event
//                 data Array with data bytes that were read.
//                 length Number of data bytes that were read.
//                 address Start memory address that the bytes belong to.
// RETURN VALUE:   none
// DESCRIPTION:    Callback for when data was read from a firmware file during loading.
//
//***************************************************************************************
procedure TFirmwareData.FirmwareFileDataRead(sender: TObject; data: array of Byte; length: Integer; address: Longword);
begin
  // add the newly read firmware data
  AddData(data, length, address);
end; //*** end of FirmwareFileDataRead ***


//***************************************************************************************
// NAME:           AddData
// PARAMETER:      data Array with bytes to add.
//                 length Number of bytes in the array.
//                 address Address where to start adding bytes.
// RETURN VALUE:   True is successful, False otherwise.
// DESCRIPTION:    Adds firmware data to the data segments. Segments are automatically
//                 created and joined where needed.
//
//***************************************************************************************
function TFirmwareData.AddData(data: array of Byte; length: Integer; address: Longword): Boolean;
var
  firstSegmentIdx: Integer;
  lastSegmentIdx: Integer;
  segmentIdx: Integer;
  joinedData: array of Byte;
  joinedSize: Integer;
  byteIdx: Integer;
begin
  Result := True;

  // find the starting and ending segment index
  firstSegmentIdx := FindSegmentIdx(address);
  lastSegmentIdx := FindSegmentIdx(address + Longword(length) - 1);

  // try to snap segments if they are directly next to another one
  if firstSegmentIdx = -1 then
  begin
    segmentIdx := FindPrevSegmentIdx(address);
    if segmentIdx <> - 1 then
    begin
      if address = (FSegmentList[segmentIdx].LastAddress + 1) then
        firstSegmentIdx := segmentIdx;
    end;
  end;
  if lastSegmentIdx = -1 then
  begin
    segmentIdx := FindNextSegmentIdx(address + Longword(length) - 1);
    if segmentIdx <> - 1 then
    begin
      if (address + Longword(length)) = FSegmentList[segmentIdx].BaseAddress then
        lastSegmentIdx := segmentIdx;
    end;
  end;

  // begin and end belongs to existing segments?
  if (firstSegmentIdx <> -1) and (lastSegmentIdx <> -1) then
  begin
    // create new data array with a copy of the first segment at the start and a copy
    // of the last segment at the end.
    joinedSize := (FSegmentList[lastSegmentIdx].LastAddress + 1) - FSegmentList[firstSegmentIdx].BaseAddress;
    SetLength(joinedData, joinedSize);
    for byteIdx := 0 to (FSegmentList[firstSegmentIdx].Size - 1) do
      joinedData[byteIdx] := FSegmentList[firstSegmentIdx].Data[byteIdx];
    for byteIdx := 0 to (FSegmentList[lastSegmentIdx].Size - 1) do
      joinedData[(joinedSize - FSegmentList[lastSegmentIdx].Size) + byteIdx] := FSegmentList[lastSegmentIdx].Data[byteIdx];
    // now remove the affected segments in preparation to replace them with 1 big new one
    // but not the first one, because this one will be resized to be a big one that holds
    // all the data. keep in mind that the indexes change after deleting a segment, so
    // the to be deleted segment is always at index firstSegmentIdx + 1
    for segmentIdx := (firstSegmentIdx + 1) to lastSegmentIdx do
    begin
      FSegmentList.Delete(firstSegmentIdx + 1);
    end;
    // add the backed up data to the first segment, which will automatically be expanded
    Result := FSegmentList[firstSegmentIdx].Add(joinedData, joinedSize, FSegmentList[firstSegmentIdx].BaseAddress);
    // now add the actual data
    if Result then
      Result := FSegmentList[firstSegmentIdx].Add(data, length, address);
    // release array
    SetLength(joinedData, 0);
    // make sure segments are properly sorted
    SortSegments;
    // all done
    Exit;
  end;

  // begin and end do not belong to existing segments
  if (firstSegmentIdx = -1) and (lastSegmentIdx = -1) then
  begin
    // it could be there there are existing segments between the range that should be
    // removed. try to match the first and last segment index to snap to these.
    firstSegmentIdx := FindNextSegmentIdx(address);
    lastSegmentIdx := FindPrevSegmentIdx(address + Longword(length) - 1);
    // if these are both valid values, then there are segments in between that should
    // be removed
    if (firstSegmentIdx <> -1) and (lastSegmentIdx <> -1) then
    begin
      // remove the segments. keep in mind that the indexes change after deleting a
      // segment, so the to be deleted segment is always at index firstSegmentIdx
      for segmentIdx := firstSegmentIdx to lastSegmentIdx do
      begin
        FSegmentList.Delete(firstSegmentIdx);
      end;
    end;
    // now add the data as a new segment
    segmentIdx := FSegmentList.Add(TDataSegment.Create);
    if segmentIdx >= 0 then
      Result := FSegmentList[segmentIdx].Add(data, length, address)
    else
      Result := False;
    // make sure segments are properly sorted
    SortSegments;
    // all done
    Exit;
  end;

  // begin belongs to existing segments but the end does not?
  if (firstSegmentIdx <> -1) and (lastSegmentIdx = -1) then
  begin
    // snap last segment to the closest known one
    lastSegmentIdx := FindPrevSegmentIdx(address + Longword(length) - 1);
    // remove the overlapping segments, excluding the first one. keep in mind that the
    // indexes change after deleting a segment, so the to be deleted segment is always at
    // index firstSegmentIdx + 1
    for segmentIdx := (firstSegmentIdx + 1) to lastSegmentIdx do
    begin
      FSegmentList.Delete(firstSegmentIdx + 1);
    end;
    // now add the data to the first segment, which will automatically expand it
    Result := FSegmentList[firstSegmentIdx].Add(data, length, address);
    // make sure segments are properly sorted
    SortSegments;
    // all done
    Exit;
  end;

  // begin does not belong to an existing segment but the end does
  if (firstSegmentIdx = -1) and (lastSegmentIdx <> -1) then
  begin
    // snap first segment to the closest known one
    firstSegmentIdx := FindNextSegmentIdx(address);
    // remove the overlapping segments, excluding the last one. keep in mind that the
    // indexes change after deleting a segment, so the to be deleted segment is always at
    // index firstSegmentIdx
    for segmentIdx := firstSegmentIdx to (lastSegmentIdx - 1) do
    begin
      FSegmentList.Delete(firstSegmentIdx);
    end;
    // note that last segment index changed because we deleted segments so refresh it
    lastSegmentIdx := FindSegmentIdx(address + Longword(length) - 1);
    // try to snap it
    if lastSegmentIdx = -1 then
    begin
      segmentIdx := FindNextSegmentIdx(address + Longword(length) - 1);
      if segmentIdx <> - 1 then
      begin
        if (address + Longword(length)) = FSegmentList[segmentIdx].BaseAddress then
          lastSegmentIdx := segmentIdx;
      end;
    end;
    // now add the data to the first last, which will automatically expand it
    if lastSegmentIdx <> -1 then
    begin
      Result := FSegmentList[lastSegmentIdx].Add(data, length, address);
    end
    else
    begin
      Result := False;
    end;
    // make sure segments are properly sorted
    SortSegments;
    // all done
    Exit;
  end;
end; //*** end of AddData ***


//***************************************************************************************
// NAME:           RemoveData
// PARAMETER:      length Number of bytes to remove.
//                 address Address where to start removing from.
// RETURN VALUE:   True is successful, False otherwise.
// DESCRIPTION:    Removes firmware data from the data segments.
//
//***************************************************************************************
function TFirmwareData.RemoveData(length: Integer; address: Longword): Boolean;
var
  firstSegmentIdx: Integer;
  lastSegmentIdx: Integer;
  segmentDelCnt: Integer;
  segmentIdx: Integer;
  byteIdx: Integer;
  remainderData: array of Byte;
  remainderLen: Integer;
  remainderAddr: Longword;
begin
  Result := True;

  // find the starting and ending segment index
  firstSegmentIdx := FindSegmentIdx(address);
  lastSegmentIdx := FindSegmentIdx(address + Longword(length) - 1);

  // in case the start and end is not in a segment, try to align it to the closest one
  if firstSegmentIdx = -1 then
    firstSegmentIdx := FindNextSegmentIdx(address);
  if lastSegmentIdx = -1 then
    lastSegmentIdx := FindPrevSegmentIdx(address + Longword(length) - 1);

  // after the align operation both indexes must be valid, otherwise there are no
  // segments to remove, which means we are done already
  if (firstSegmentIdx = -1) or (lastSegmentIdx = -1) then
    Exit;

  // check if a segment split is needed, which is a special case and should be done first
  if (firstSegmentIdx = lastSegmentIdx) and
      (address > FSegmentList[firstSegmentIdx].BaseAddress) and
      ((address + Longword(length) - 1) < FSegmentList[lastSegmentIdx].LastAddress) then
  begin
    // copy remainder data after the split to a temporary buffer
    remainderAddr := address + Longword(length);
    remainderLen := (FSegmentList[firstSegmentIdx].LastAddress + 1) - remainderAddr;
    SetLength(remainderData, remainderLen);
    for byteIdx := 0 to (remainderLen - 1) do
      remainderData[byteIdx] := FSegmentList[firstSegmentIdx].Data[(FSegmentList[firstSegmentIdx].Size - remainderLen) + byteIdx];
    // create a new segment where the remainder data will be copied to
    segmentIdx := FSegmentList.Add(TDataSegment.Create);
    if segmentIdx >= 0 then
      Result := FSegmentList[segmentIdx].Add(remainderData, remainderLen, remainderAddr)
    else
    begin
      // this should not happen and indicates a severe error
      Result := False;
    end;
    // the part after the split can be safely removed no. by removing the length of the
    // segment, it is guaranteerd that the remainder after the split is also removed
    if Result then
    begin
      Result := FSegmentList[firstSegmentIdx].Remove(FSegmentList[firstSegmentIdx].Size, address);
    end;
    // a segment was added so perform sorting
    SortSegments;
    // all done
    Exit;
  end;

  // begin and end belongs to existing segments? note that this should always be the
  // case because of the segment alignment that is performed at the start.
  if (firstSegmentIdx <> -1) and (lastSegmentIdx <> -1) then
  begin
    // remove bytes from the end of the first segment. note that the remove will only
    // operates on the specified segment so no need to worry that it removes too many
    Result := FSegmentList[firstSegmentIdx].Remove(length, address);
    // remove bytes from the end of the last segment. note that the remove will only
    // operates on the specified segment so no need to worry that it removes too many
    if Result then
      Result := FSegmentList[lastSegmentIdx].Remove(length, address);
    // remove overlapping segments if any, but not the first and the last one. keep in
    // mind that the indexes change after deleting a segment so the to be deleted segment
    // is always firstSegmentIdx + 1
    if Result then
    begin
      segmentDelCnt := 0;
      for segmentIdx := (firstSegmentIdx + 1) to (lastSegmentIdx - 1) do
      begin
        FSegmentList.Delete(firstSegmentIdx + 1);
        segmentDelCnt := segmentDelCnt + 1;
      end;
      // refresh last segment index
      lastSegmentIdx := lastSegmentIdx - segmentDelCnt;
      // check if last segment is now empty and delete it if so
      if FSegmentList[lastSegmentIdx].Size = 0 then
        FSegmentList.Delete(lastSegmentIdx);
      // check if first segment is now empty and delete it if so
      if FSegmentList[firstSegmentIdx].Size = 0 then
        FSegmentList.Delete(firstSegmentIdx);
    end;
    // no need to sort again so all done
    Exit;
  end;
end; //*** end of RemoveData ***


//***************************************************************************************
// NAME:           ClearData
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Clear all segments with firmware data.
//
//***************************************************************************************
procedure TFirmwareData.ClearData;
begin
  FSegmentList.Clear;
end; //*** end of ClearData ***


//***************************************************************************************
// NAME:           LoadFromFile
// PARAMETER:      firmwareFile Filename with full path of the firmware file to load.
//                 append True to append the firmware data to what is currently loaded,
//                        False to clear the current firmware data first.
// RETURN VALUE:   True if successful, False otherwise.
// DESCRIPTION:    Loads firmware data from a firmware file.
//
//***************************************************************************************
function TFirmwareData.LoadFromFile(firmwareFile: String; append: Boolean): Boolean;
var
  firmwareFileType: TFirmwareFileType;
  firmwareFileHandler: TFirmwareFileHandler;
begin

  // init locals and the result
  Result := False;
  firmwareFileHandler := nil;

  // determine firmware file type
  firmwareFileType := GetFirmwareFileType(firmwareFile);

  // check if the file type is an S-record and if so, load it
  if firmwareFileType = FFT_SRECORD then
  begin
    // create instance of the firmware file handler
    firmwareFileHandler := TSRecordFileHandler.Create;
  end;

  // check if the firmware file handler object was instantiated, which flags that a
  // firmware file can be loaded through it
  if Assigned(firmwareFileHandler) then
  begin
    // clear the current firmware data if we should not append the new data from the file
    if not append then
    begin
      ClearData;
    end;

    // set onload handler which does the actual data processing
    firmwareFileHandler.OnDataRead := FirmwareFileDataRead;
    // load data from the file
    Result := firmwareFileHandler.Load(firmwareFile);

    // release instance of the firmware file handler
    firmwareFileHandler.Free
  end;
end; //*** end of LoadFromFile ***


//***************************************************************************************
// NAME:           SaveToFile
// PARAMETER:      firmwareFile Filename with full path of the firmware file to save.
//                 firwareFileType Firmware file type to use when saving.
// RETURN VALUE:   True if successful, False otherwise.
// DESCRIPTION:    Saves firmware data to a firmware file of the specified format.
//
//***************************************************************************************
function TFirmwareData.SaveToFile(firmwareFile: String; firmwareFileType: TFirmwareFileType): Boolean;
var
  firmwareFileHandler: TFirmwareFileHandler;
begin
  // init result
  Result := False;

  // check if the file type is an S-record and if so, save it
  if firmwareFileType = FFT_SRECORD then
  begin
    // create instance of the firmware file handler
    firmwareFileHandler := TSRecordFileHandler.Create;
    // perform firmware file save operation
    Result := firmwareFileHandler.Save(firmwareFile, FSegmentList);
    // release the firmware file handler
    firmwareFileHandler.Free;
  end
  // check if the file type is a binary file and if so, save it
  else if firmwareFileType = FFT_BINARY then
  begin
    // create instance of the firmware file handler
    firmwareFileHandler := TBinaryFileHandler.Create;
    // perform firmware file save operation
    Result := firmwareFileHandler.Save(firmwareFile, FSegmentList);
    // release the firmware file handler
    firmwareFileHandler.Free;
  end;
end; //*** end of SaveToFile ***


//***************************************************************************************
// NAME:           Dump
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Dumps the segment contents to the standard output for debugging
//                 purposes.
//
//***************************************************************************************
procedure TFirmwareData.Dump;
var
  segmentIdx: Integer;
begin
  {$IFDEF DEBUG}
  for segmentIdx := 0 to (SegmentCount - 1) do
  begin
    Writeln('Segment index         = ' + IntToStr(segmentIdx));
    Segment[segmentIdx].Dump;
  end;
  {$ENDIF}
end; //*** end of DumpFirmwareData ***


end.
//******************************** end of FirmwareData.pas ******************************

