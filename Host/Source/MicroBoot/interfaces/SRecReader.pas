unit SRecReader;
//***************************************************************************************
// Project Name: Motorola S-Record Reader
//  Description: Class used to read S-Record files. Supports both S19 and S28 records.
//    File Name: SRecReader.pas
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
  Windows, Messages, SysUtils, Classes, Forms;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  PByte = ^Byte;
  TSRecLineType = (ltInvalid, ltS0, ltS1, ltS2, ltS3, ltS7, ltS8, ltS9);
  TSRecType = (tpS1, tpS2, tpS3, tpMixed);

type
  TSRecData = record
    val  : Byte;
    addr : LongWord;
  end;

type
  PRegion = ^TRegion;
  TRegion = record
    addr : LongWord;
    size : LongWord;
  end;

type
  TSRecReader = class(TObject)
  private
    FFileName   : String;
    FDataSize   : Longword;
    FFirstAddr  : LongWord;
    FLastAddr   : LongWord;
    FFileType   : TSRecType;
    FRegions    : TList;
    FRegionsCnt : Word;
    function    GetLineType(line: String) : TSRecLineType;
    function    GetLineSize(line: String; var size: Word) : Boolean;
    function    GetLineData(line: String; index: Word; var data: TSRecData) : Boolean;
    function    GetLineAddress(line: String; var address: LongWord) : Boolean;
    procedure   UpdateRegions(addr: LongWord; size: Word);
  public
    constructor Create;
    destructor  Destroy; override;
    function    SetFileName(fileName : String) : Boolean;
    function    GetFileName : String;
    function    GetDataSize : Longword;
    function    GetFirstAddress : Longword;
    function    GetLastAddress : Longword;
    function    GetRegionCount : Word;
    procedure   GetRegion(index : Word; var addr : Longword; var length : Longword);
    procedure   GetData(var buffer : array of Byte; addr : Longword; length : Longword);
    function    GetFileType : TSRecType;
    function    BlankCheck(addr : Longword; length : Longword) : Boolean;
  end;


implementation

//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructore
//
//***************************************************************************************
constructor TSRecReader.Create;
begin
  inherited Create;
  FRegions := TList.Create;
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Destroy
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class destructor
//
//***************************************************************************************
destructor TSRecReader.Destroy;
var
  index : Word;
  region: PRegion;
begin
  if FRegions.Count > 0 then
  begin
    for index := 0 to FRegions.Count-1 do
    begin
      region := FRegions.Items[index];
      Dispose(region);
    end;
  end;

  FRegions.Free;
  inherited;
end; //*** end of Destroy ***


//***************************************************************************************
// NAME:           UpdateRegions
// PARAMETER:      addr: start address of data
//                 size: length of data
// RETURN VALUE:   none
// DESCRIPTION:    Processes the data by categorizing it into a region.
//
//***************************************************************************************
procedure TSRecReader.UpdateRegions(addr: LongWord; size: Word);
var
  index : Word;
  added : Boolean;
  region: PRegion;
begin
  added := False;

  if FRegions.Count > 0 then
  begin
    // loop through all existing regions
    for index := 0 to FRegions.Count-1 do
    begin
      // set pointer to this region
      region := FRegions.Items[index];

      // does data fit at the end?
      if (region^.addr+region^.size) = addr then
      begin
        // add at the end of this region
        region^.size := region^.size + size;
        added := True;
      end
      // does data fit at the start?
      else if region^.addr = (addr+size) then
      begin
        // add at the start of this region
        region^.addr := region^.addr - size;
        added := True;
      end;

      // was data added to a region?
      if added then
      begin
        Break; // no need to continue loop
      end;
    end;
  end;

  // data couldn't be added to an existing region?
  if not added then
  begin
    // create a new region and add the data there
    New(region);
    region^.addr := addr;
    region^.size := size;
    FRegions.Add(region);
  end;
end; //*** end of UpdateRegions ***


//***************************************************************************************
// NAME:           GetLineType
// PARAMETER:      Line from S-Record
// RETURN VALUE:   line type
// DESCRIPTION:    Determines what type of S-Record line we're dealing with.
//
//***************************************************************************************
function TSRecReader.GetLineType(line: String) : TSRecLineType;
begin
  Result := ltInvalid;

  if Pos('S0', UpperCase(line)) > 0 then Result := ltS0;
  if Pos('S1', UpperCase(line)) > 0 then Result := ltS1;
  if Pos('S2', UpperCase(line)) > 0 then Result := ltS2;
  if Pos('S3', UpperCase(line)) > 0 then Result := ltS3;
  if Pos('S7', UpperCase(line)) > 0 then Result := ltS7;
  if Pos('S8', UpperCase(line)) > 0 then Result := ltS8;
  if Pos('S9', UpperCase(line)) > 0 then Result := ltS9;
end; //*** end of GetLineType ***


//***************************************************************************************
// NAME:           GetLineSize
// PARAMETER:      Line from S-Record
// RETURN VALUE:   Number of data bytes
// DESCRIPTION:    Obtains the number of databytes in the specified S-Record line.
//
//***************************************************************************************
function TSRecReader.GetLineSize(line: String; var size: Word) : Boolean;
var
  unusedBytes : Byte;
begin
  Result := false; // init

  if GetLineType(line) = ltS1 then
    unusedBytes := 3 // 2 for address and 1 for checksum
  else if GetLineType(line) = ltS2 then
    unusedBytes := 4 // 3 for address and 1 for checksum
  else if GetLineType(line) = ltS3 then
    unusedBytes := 5 // 4 for address and 1 for checksum
  else
    Exit; // not a S1 or S2 line

  size := StrToInt('$' + Copy(line, 3, 2)) - unusedBytes;
  Result := true;
end; //*** end of GetLineSize ***


//***************************************************************************************
// NAME:           GetLineData
// PARAMETER:      Line from S-Record
// RETURN VALUE:   data at a specific index (starts at 0)
// DESCRIPTION:    Obtains the data at a specific index in the specified S-Record line.
//
//***************************************************************************************
function TSRecReader.GetLineData(line: String; index: Word; var data: TSRecData) : Boolean;
var
  dataOffset : Byte;
  lineSize   : Word;
  lineAddr   : LongWord;
begin
  Result := false; // init

  if GetLineType(line) = ltS1 then
    dataOffset := 9 // characters untill we reach actual data
  else if GetLineType(line) = ltS2 then
    dataOffset := 11 // characters untill we reach actual data
  else if GetLineType(line) = ltS3 then
    dataOffset := 13 // characters untill we reach actual data
  else
    Exit; // not a S1, S2 or S3 line

  // make sure index parameter is valid
  if not GetLineSize(line, lineSize) then
    Exit
  else
    if index > (lineSize-1) then Exit;

  // obtain the start address of the line
  if not GetLineAddress(line, lineAddr) then
    Exit;

  data.val := StrToInt('$' + Copy(line, dataOffset+(index*2), 2));
  data.addr := lineAddr + index;

  Result := true;
end; //*** end of GetLineData ***


//***************************************************************************************
// NAME:           GetLineAddress
// PARAMETER:      Line from S-Record
// RETURN VALUE:   Start address
// DESCRIPTION:    Obtains the start address as specified in the S-Record line.
//
//***************************************************************************************
function TSRecReader.GetLineAddress(line: String; var address: LongWord) : Boolean;
var
  addrLength : Byte;
begin
  Result := false; // init

  if GetLineType(line) = ltS1 then
    addrLength := 4 // 2 byte address
  else if GetLineType(line) = ltS2 then
    addrLength := 6 // 3 byte address
  else if GetLineType(line) = ltS3 then
    addrLength := 8 // 4 byte address
  else
    Exit; // not a S1, S2 or S3 line

  address := StrToInt('$' + Copy(line, 5, addrLength));
  Result := true;
end; //*** end of GetLineAddress ***


//***************************************************************************************
// NAME:           SetFileName
// PARAMETER:      Filename with full path
// RETURN VALUE:   True when the S-Record was valid and could be loaded, otherwise False.
// DESCRIPTION:    Function verifies that the file is actually a S-Record. If so, then
//                 the most class properties are set based on the info in the S-Record.
//
//***************************************************************************************
function TSRecReader.SetFileName(fileName : String) : Boolean;
var
  SRecFile   : TextFile;
  Line       : string;
  ValidSRec  : Boolean;
  LineSize   : Word;
  LineAddr   : LongWord;
  S1RecFound : Boolean;
  S2RecFound : Boolean;
  S3RecFound : Boolean;
begin
  // first reset all the internal properties
  FFileName  := '';
  FDataSize  := 0;
  FFirstAddr := $ffffffff;
  FLastAddr  := 0;
  FFileType := tpS1;
  FRegionsCnt := 0;
  FRegions.Clear;

  // init locals
  S1RecFound := false;
  S2RecFound := false;
  S3RecFound := false;
  Result := false;
  ValidSRec := false;

  // 1. Verify if file exists
  if not FileExists(fileName) then Exit;

  // 2. Verify if file contains S-Records
  AssignFile(SRecFile, fileName); // get file handle
  Reset(SRecFile); // go to start of file
  while not Eof(SRecFile) do
  begin
    ReadLn(SRecFile, Line); // read line from file
    if (GetLineType(Line) = ltS1) or (GetLineType(Line) = ltS2)  or
       (GetLineType(Line) = ltS3) then
    begin
      ValidSRec := true;
      Break; // valid S-Record
    end;
  end;
  CloseFile(SRecFile); // release file
  if not ValidSRec then Exit;

  // 3. Calculate datasize, first address and last address
  AssignFile(SRecFile, fileName); // get file handle
  Reset(SRecFile); // go to start of file
  while not Eof(SRecFile) do
  begin
    ReadLn(SRecFile, Line); // read line from file
    LineSize := 0; // init
    if GetLineSize(Line, LineSize) then
      FDataSize := FDataSize + LineSize; // add to previous value

    if GetLineAddress(Line, LineAddr) then
    begin
      if LineAddr < FFirstAddr then
        FFirstAddr := LineAddr;

      if LineAddr > FLastAddr then
        FLastAddr := LineAddr + LineSize -1;
    end;

    // check line type
    if GetLineType(line) = ltS1 then
      S1RecFound := true;
    if GetLineType(line) = ltS2 then
      S2RecFound := true;
    if GetLineType(line) = ltS3 then
      S3RecFound := true;

  end;
  CloseFile(SRecFile); // release file

  // set file type
  if (S1RecFound) and (not S2RecFound) and (not S3RecFound) then
    FFileType := tpS1
  else if (S2RecFound) and (not S1RecFound) and (not S3RecFound) then
    FFileType := tpS2
  else if (not S2RecFound) and (not S1RecFound) and (S3RecFound) then
    FFileType := tpS3
  else
    FFileType := tpMixed;

  // 4. Determine regions
  AssignFile(SRecFile, fileName); // get file handle
  Reset(SRecFile); // go to start of file
  while not Eof(SRecFile) do
  begin
    ReadLn(SRecFile, Line); // read line from file
    LineSize := 0; // init

    if GetLineAddress(Line, LineAddr) then
    begin
      if GetLineSize(Line, LineSize) then
      begin
        UpdateRegions(LineAddr, LineSize);
      end;
    end;
  end;
  CloseFile(SRecFile); // release file

  // set region count
  FRegionsCnt := FRegions.Count;

  // 5. Verify properties and if ok, set the FFilename  property
  if (FDataSize <> 0) and (FFirstAddr <> $ffffffff) and (FLastAddr <> 0) then
  begin
    FFileName  := fileName; // set the filename property
    Result := true;
  end;
end; //*** end of SetFileName ***


//***************************************************************************************
// NAME:           GetFileName
// PARAMETER:      none
// RETURN VALUE:   Filename with full path or '' when invalid.
// DESCRIPTION:    S-Record filename that is configured to be read be this class.
//
//***************************************************************************************
function TSRecReader.GetFileName : String;
begin
  Result := FFileName;
end; //*** end of GetFileName ***


//***************************************************************************************
// NAME:           GetDataSize
// PARAMETER:      none
// RETURN VALUE:   Size of data.
// DESCRIPTION:    Obtains the number of databytes in the S-Record. 0xFF values are
//                 not included.
//
//***************************************************************************************
function TSRecReader.GetDataSize : Longword;
begin
  Result := FDataSize;
end; //*** end of GetDataSize ***


//***************************************************************************************
// NAME:           GetFirstAddress
// PARAMETER:      none
// RETURN VALUE:   First address in S-Record.
// DESCRIPTION:    Obtains the first memory address with data in the S-Record.
//
//***************************************************************************************
function TSRecReader.GetFirstAddress : Longword;
begin
  Result := FFirstAddr;
end; //*** end of GetFirstAddress ***


//***************************************************************************************
// NAME:           GetLastAddress
// PARAMETER:      none
// RETURN VALUE:   Last address in S-Record.
// DESCRIPTION:    Obtains the last memory address with data in the S-Record.
//
//***************************************************************************************
function TSRecReader.GetLastAddress : Longword;
begin
  Result := FLastAddr;
end; //*** end of GetLastAddress ***


//***************************************************************************************
// NAME:           GetRegionCount
// PARAMETER:      none
// RETURN VALUE:   Number of address regions
// DESCRIPTION:    Obtains the number of address regions found in the S-record.
//
//***************************************************************************************
function TSRecReader.GetRegionCount : Word;
begin
  Result := FRegionsCnt;
end;  //*** end of GetRegionCount ***


//***************************************************************************************
// NAME:           GetRegion
// PARAMETER:      index: region index to retrieve info from
//                 addr: destination for region's start address
//                 length: destination for region's length
// RETURN VALUE:   none
// DESCRIPTION:    Obtains the address region info.
//
//***************************************************************************************
procedure TSRecReader.GetRegion(index : Word; var addr : Longword; var length : Longword);
var
  region: PRegion;
begin
  // initialize return values
  addr := 0;
  length := 0;

  if FRegions.Count > 0 then
  begin
    if index < FRegions.Count then
    begin
      // set pointer to this region
      region := FRegions.Items[index];

      // obtain region info
      addr := region^.addr;
      length := region^.size;
    end;
  end;
end; //*** end of GetRegion ***


//***************************************************************************************
// NAME:           GetData
// PARAMETER:      buffer is a pointer to a byte buffer where the data will be written
//                 to from memory address addr to addr+length.
// RETURN VALUE:   none
// DESCRIPTION:    Obtains the data in the S-Record for a memory block that starts at
//                 addr and goes until addr+length and stores the data in buffer. empty
//                 data will be filled with 0xFF.
//
//***************************************************************************************
procedure TSRecReader.GetData(var buffer : array of Byte; addr : Longword; length : Longword);
var
  data        : TSRecData;
  line        : string;
  lineAddr    : LongWord;
  lineSize    : Word;
  SRecFile    : TextFile;
  cnt         : Word;
  ok2Continue : boolean;
begin
  // check parameters
  if length = 0 then Exit;

  // first init entire buffer to 0xff
  for cnt := 0 to length-1  do
  begin
    buffer[cnt] := $ff;
  end;

  // go through each line to see if it has data for us
  AssignFile(SRecFile, FFileName); // get file handle
  Reset(SRecFile); // go to start of file
  while not Eof(SRecFile) do
  begin
    // this can take a while so process messages to not stall the parent app
    Application.ProcessMessages;
    
    ReadLn(SRecFile, line); // read line from file
    ok2Continue := true; // init

    // obtain line properties
    if not GetLineAddress(line, lineAddr) then ok2Continue := false;
    if not GetLineSize(line, lineSize) then ok2Continue := false;

    if ok2Continue then
    begin
      // process all data on the line
      for cnt := 0 to lineSize-1 do
      begin
        // read data info
        if not GetLineData(line, cnt, data) then ok2Continue := false;
        if ok2Continue then
        begin
          // is this one for us?
          if (data.addr >= addr) and (data.addr <= (addr+length-1)) then
          begin
            // store it in the memory buffer
            buffer[data.addr-addr] := data.val;
          end;
        end;
      end;
    end;

  end;
  CloseFile(SRecFile); // release file
end; //*** end of GetData ***


//***************************************************************************************
// NAME:           GetFileType
// PARAMETER:      none
// RETURN VALUE:   S-Record file type
// DESCRIPTION:    Determines is the file contains just S1 data lines, just S2 data
//                 lines. or a combination of these two.
//
//***************************************************************************************
function TSRecReader.GetFileType : TSRecType;
begin
  Result := FFileType;
end; //*** end of GetFileType ***


//***************************************************************************************
// NAME:           BlankCheck
// PARAMETER:      checks from addr to addr+length-1.
// RETURN VALUE:   true if all bytes are 0xff, false otherwise
// DESCRIPTION:    Checks if a memory range in the S-Record file is empty (0xff) or not.
//
//***************************************************************************************
function TSRecReader.BlankCheck(addr : Longword; length : Longword) : Boolean;
var
  buffer : array of Byte;
  cnt    : LongWord;
begin
  Result := true;
  SetLength(buffer, length); // init size of the dynamic array
  GetData(buffer, addr, length); // fill it with the data contents from the S-Record

  for cnt := 0 to length-1 do
  begin
    if buffer[cnt] <> $ff then
    begin
      Result := false; // memory range is not blank
      Break; // no need to continue loop
    end;
  end;
end; //*** end of BlankCheck ***

end.
//******************************** end of SRecReader.pas ********************************

