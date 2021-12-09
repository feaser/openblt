unit CustomUtil;
//***************************************************************************************
//  Description: Contains custom utility functions and procedures.
//    File Name: customutil.pas
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
  Classes, SysUtils;


//***************************************************************************************
// Prototypes
//***************************************************************************************
function  CustomUtilValidateNumberRange(Source: String; Min: Integer; Max: Integer; IsHex: Boolean = False): String;
procedure CustomUtilValidateKeyAsInt(var Key: Char);
procedure CustomUtilValidateKeyAsHex(var Key: Char);


implementation
//***************************************************************************************
// NAME:           CustomUtilValidateNumberRange
// PARAMETER:      Source The source string to validate.
//                 Min The minimum allowed value of the number in the string.
//                 Max The maximum allowed value of the number in the string.
// RETURN VALUE:   The same as the source string, if successful. A range limited value
//                 otherwise. '0' in case of error.
// DESCRIPTION:    Validates if the string contains a number in the specified range.
//
//***************************************************************************************
function CustomUtilValidateNumberRange(Source: String; Min: Integer; Max: Integer; IsHex: Boolean): String;
var
  Value: Int64;
begin
  // Check parameters.
  Assert(Source <> '', 'Source string cannot be empty.');
  Assert(Min < Max, 'Invalid range specified.');
  // Attempt to convert the contents of the string to a number.
  try
    if IsHex then
    begin
      Value := StrToInt64('$' + Source);
      // Set initial result.
      Result := Format('%.x', [Value]);
    end
    else
    begin
      Value := StrToInt64(Source);
      // Set initial result.
      Result := IntToStr(Value);
    end;
    // Check lower range.
    if Value < Min then
    begin
      if IsHex then
        Result := Format('%.x', [Min])
      else
        Result := IntToStr(Min);
    end
    // Check upper range
    else if Value > Max then
    begin
      if IsHex then
        Result := Format('%.x', [Max])
      else
        Result := IntToStr(Max);
    end;
  except
    // Default to 0 in case the string could not be converted to a number.
    Result := '0';
  end;
end; //*** end of CustomUtilValidateNumberRange ***


//***************************************************************************************
// NAME:           CustomUtilValidateKeyAsInt
// PARAMETER:      Key Value of the key that was pressed.
// RETURN VALUE:   none
// DESCRIPTION:    Checks if the specified key contains a character that in the range
//                 0..9. Additionally, CTRL-V, -X, -C, -A and backspace are allowed. Can
//                 be used in the OnKeyPress events to validate the pressed key.
//
//***************************************************************************************
procedure CustomUtilValidateKeyAsInt(var Key: Char);
begin
  if not (Key In ['0'..'9', #8, ^V, ^X, ^C, ^A]) then
  begin
    // Ignore it.
    Key := #0;
  end;
end; //*** end of CustomUtilValidateKeyAsInt ***


//***************************************************************************************
// NAME:           CustomUtilValidateKeyAsHex
// PARAMETER:      Key Value of the key that was pressed.
// RETURN VALUE:   none
// DESCRIPTION:    Checks if the specified key contains a character that in the range
//                 0..9 and a..f. Additionally, CTRL-V, -X, -C, -A and backspace are
//                 allowed. Can be used in the OnKeyPress events to validate the pressed
//                 key. Note that hexadecimal keys (a..f) are automatically converted to
//                 upper case.
//
//***************************************************************************************
procedure CustomUtilValidateKeyAsHex(var Key: Char);
begin
  if not (Key In ['0'..'9', 'a'..'f', 'A'..'F', #8, ^V, ^X, ^C, ^A]) then
  begin
    // Ignore it.
    Key := #0;
  end;
  // Convert a..f to upper case
  if Key In ['a'..'f'] then
  begin
    Key := UpCase(Key);
  end;
end; //*** end of CustomUtilValidateKeyAsHex ***

end.
//******************************** end of customutil.pas ********************************

