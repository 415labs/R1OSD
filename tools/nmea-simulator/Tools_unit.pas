unit Tools_unit;

interface

uses Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms, Dialogs, StdCtrls, IniFiles;

  function genCRC(data : string) : integer;

  function Replace(Dest, SubStr, Str: string): string;
  procedure Wait(Value : DWORD);
  procedure logIt(logType : string; logLine : string);
  function switch(variable : integer) : integer;
  function Hex2Int(hex:string):integer;
  Function HexToDec ( Hex : String ) : Integer ;





implementation

//------------------------------------------------------
//
// tranforme une valeur hexa en int
//
//------------------------------------------------------

function Hex2Int(hex : string) : Integer;

var   i,count: Integer;
begin
   val('$'+hex,i,count);
   result := i;
end;


Function HexToDec ( Hex : String ) : Integer ;
Const
  HexChars = '0123456789ABCDEF' ;
Var
  I , L : Byte ;
  H : Integer ;
Begin
  Hex := UpperCase ( Hex ) ;
  Result := 0 ;
  L := Length ( Hex ) ;
  H := 1 ;
  For I := L DownTo 1 Do
    Begin
      Result := Result + H * ( Pos ( Hex [ I ] , HexChars ) - 1 ) ;
      H := H * 16 ;
    End ;
End ;



// --------------------------------------------------------------------------
//
// Switch (0->1 ou 1->0) d'une variable de type int.
//
// --------------------------------------------------------------------------

function switch(variable : integer) : integer;
begin
  if (variable = 1) then variable := 0 else variable := 1;
  result := variable;
end;

// --------------------------------------------------------------------------
//
// Fonction de remplacement de chaine de caractere
//
// --------------------------------------------------------------------------

function Replace(Dest, SubStr, Str: string): string;
var
  Position: Integer;
begin
while (Pos(SubStr, Dest) <> 0) do
  begin
    Position:=Pos(SubStr, Dest);
    Delete(Dest, Position, Length(SubStr));
    Insert(Str, Dest, Position);
  end;
  Result:=Dest;
end;
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
//
// Attente de x ms
//
// --------------------------------------------------------------------------

procedure Wait(Value : DWORD);
var
  TC : DWORD;
begin
  TC := GetTickcount;
  while (GetTickcount - TC) < Value do Application.ProcessMessages;
end;
// --------------------------------------------------------------------------



// --------------------------------------------------------------------------
//
// Procedure permettant d'ecrire des données dans un fichier texte
//
// --------------------------------------------------------------------------
procedure appendDataToTextFile(textFileName, dataToWrite : string);
  Var F: TextFile;
  begin

   if not FileExists(ExtractFilePath(Application.ExeName)+ textFileName) then
   begin
    AssignFile(F,ExtractFilePath(Application.ExeName)+ textFileName);
    Rewrite(F);
    CloseFile(F);
   end;

     AssignFile(F,ExtractFilePath(Application.ExeName)+ textFileName);
     Append(F); //Prépare un fichier existant pour l'ajout de texte
     Writeln(F,dataToWrite); //Writeln écrit une marque fin de ligne
     CloseFile(F);
  end;

//------------------------------------------------------
//
// Loggue un evenement
//
// Logline: ligne à logger
// Type:
// DEBUG
// RFID
// IR
//------------------------------------------------------

procedure logIt(logType : string; logLine : string);
var logDate : string;
    dataToAppend : string;

begin
  logDate := FormatDateTime('yyyymmdd', Date);
  dataToAppend := FormatDateTime('dd/mm/yyyy', Date) + ' ' + TimeToStr(Time) + '|' + intToStr(getTickCount()) + '|' + logLine;
  appendDataToTextFile(logDate + '_LOG_' + logType + '.txt', dataToAppend);
end;




// --------------------------------------------------------------------------
//
// Calcul CRC
//
// --------------------------------------------------------------------------

function genCRC(data : string) : integer;

var
 i:integer;
 lastResult : integer;
 curChar : char;

begin
 i := 0;
 lastResult := 0;
 while (i<=(Length(data)) ) do
   begin
     curChar :=  data[i]; // copy(data,i,1);
     lastResult := Ord(curChar) xor Ord(lastResult);
     i:=i+1;
   end;
   Result := lastResult;
end;




end.
