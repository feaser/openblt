unit XcpDataFile;
//***************************************************************************************
//  Description: XCP data file interface.
//    File Name: XcpDataFile.pas
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
// You should have received a copy of the GNU General Public License along with OpenBLT.
// If not, see <http://www.gnu.org/licenses/>.
//
// A special exception to the GPL is included to allow you to distribute a combined work
// that includes OpenBLT without being obliged to provide the source code for any
// proprietary components. The exception text is included at the bottom of the license
// file <license.html>.
//
//***************************************************************************************
interface


//***************************************************************************************
// Includes
//***************************************************************************************
uses
  Windows, Messages, SysUtils, Classes, Forms, SRecReader;


//***************************************************************************************
// Type Definitions
//***************************************************************************************
type
  TXcpDataFile = class(TObject)
  private
    FDataFileReady : Boolean;
    FRegionCount   : Word;
    FDataFile      : TSRecReader;
  public
    constructor Create(dataFile: string);
    destructor  Destroy; override;
    function    GetDataCnt : LongWord;
    function    GetRegionCnt : Word;
    procedure   GetRegionInfo(region : Word; var addr : Longword; var len : Longword);
    function    GetRegionData(region : Word; var data : array of Byte) : LongWord;
  end;


implementation

//***************************************************************************************
// NAME:           Create
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class constructor
//
//***************************************************************************************
constructor TXcpDataFile.Create(dataFile: string);
begin
  // call inherited constructor
  inherited Create;

  // instantiate S-Record reader object
  FDataFile := TSRecReader.Create;

  // reset data file ready flag
  FDataFileReady := false;

  // reset the region count
  FRegionCount := 0;

  // open the data file as an S-Record file
  if FDataFile.SetFileName(dataFile) then
  begin
    // set data file ready flag
    FDataFileReady := true;

    // obtain the number of data regions
    FRegionCount := FDataFile.GetRegionCount;
  end;
end; //*** end of Create ***


//***************************************************************************************
// NAME:           Destroy
// PARAMETER:      none
// RETURN VALUE:   none
// DESCRIPTION:    Class destructor
//
//***************************************************************************************
destructor TXcpDataFile.Destroy;
begin
  // release S-Record reader object
  FDataFile.Free;

  // call inherited destructor
  inherited;
end; //*** end of Destroy ***


//***************************************************************************************
// NAME:           GetDataCnt
// PARAMETER:      none
// RETURN VALUE:   number of data bytes
// DESCRIPTION:    Returns the number of data bytes in the data file
//
//***************************************************************************************
function TXcpDataFile.GetDataCnt : LongWord;
begin
  // init return value
  result := 0;

  // verify data file state
  if not FDataFileReady then Exit;

  // return the number of data bytes in the data file
  result := FDataFile.GetDataSize;
end; //*** end of GetDataCnt ***


//***************************************************************************************
// NAME:           GetRegionCnt
// PARAMETER:      none
// RETURN VALUE:   number of data regions
// DESCRIPTION:    Returns the number of data regions in the data file
//
//***************************************************************************************
function TXcpDataFile.GetRegionCnt : Word;
begin
  // init return value
  result := 0;

  // verify data file state
  if not FDataFileReady then Exit;

  // return the number of data regions
  result := FRegionCount;
end; //*** end of GetRegionCnt ***


//***************************************************************************************
// NAME:           GetRegionInfo
// PARAMETER:      region index and parameters where to store the region start address
//                 and length info.
// RETURN VALUE:   none
// DESCRIPTION:    Returns the number of data bytes in the specified region and its
//                 start address.
//
//***************************************************************************************
procedure TXcpDataFile.GetRegionInfo(region : Word; var addr : Longword; var len : Longword);
begin
  // init return parameters
  addr := 0;
  len  := 0;

  // verify data file state
  if not FDataFileReady then Exit;

  // validate input parameter
  if region > FRegionCount-1 then Exit;

  // obtain the region information
  FDataFile.GetRegion(region, addr, len);
end; //*** end of GetRegionInfo ***


//***************************************************************************************
// NAME:           GetRegionData
// PARAMETER:      region index and a pointer to the data buffer where the data will be
//                 stored.
// RETURN VALUE:   Number of data bytes stored in the data buffer
// DESCRIPTION:    Reads the data from a specific region into the specified data buffer.
//
//***************************************************************************************
function TXcpDataFile.GetRegionData(region : Word; var data : array of Byte) : LongWord;
var
  addr : LongWord;
  len  : LongWord;
begin
  // init return value
  result := 0;

  // verify data file state
  if not FDataFileReady then Exit;

  // validate input parameter
  if region > FRegionCount-1 then Exit;

  // obtain region info
  FDataFile.GetRegion(region, addr, len);

  // obtain the region data
  FDataFile.GetData(data, addr, len);

  // return the number of data bytes stored in the data buffer
  result := len;
end; //*** end of GetRegionData ***


end.
//******************************** end of XcpDataFile.pas *******************************

