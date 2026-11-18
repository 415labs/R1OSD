unit SerialComm;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ExtCtrls, IniFiles, jpeg, CPort, SyncObjs;

function OpenComPort(_comPort : TComPort; port: integer; baudrate : integer; parity : string ) : integer;
function closeComPort(_comPort : TComPort) : integer;
function sendSerialData(CPort : TComPort; dataToSend : string) : integer;


procedure ComPortRxBuf(Sender: TObject; const Buffer; Count: Integer);
function getSerialData(CPort : TComPort; timeout: integer; lengthStr :integer) : string;
function getSerialDataForSyncInventory(CPort : TComPort; timeout: integer) : string;


implementation

procedure ComPortRxBuf(Sender: TObject; const Buffer; Count: Integer);
begin
  nil;
end;




// --------------------------------------------------------------------------
//
// Ouvre le port com
//
// --------------------------------------------------------------------------
function OpenComPort(_comPort : TComPort; port: integer; baudrate : integer; parity : string ) : integer;
var res : integer;
begin
  res := 0;
  _comPort.Port := 'COM' + intToStr(port);

  if (baudrate = 9600) then _comPort.BaudRate := br9600;
  if (baudrate = 38400) then _comPort.BaudRate := br38400;
  if (baudrate = 57600) then _comPort.BaudRate := br57600;
  if (baudrate = 115200) then _comPort.BaudRate := br115200;

  if (parity = 'even') then _comPort.Parity.bits := prEven;
  if (parity = 'mark') then _comPort.Parity.bits := prMark;
  if (parity = 'none') then _comPort.Parity.bits := prNone;
  if (parity = 'odd') then _comPort.Parity.bits := prOdd;
  if (parity = 'space') then _comPort.Parity.bits := prSpace;

  try
    _comPort.Open;
    res := 1;
  except
    res := 0;
  end;

  result := res;
end;
// --------------------------------------------------------------------------




//------------------------------------------------------
//
// Ferme le port com. 1 si OK, 0 si KO
//
//------------------------------------------------------

function closeComPort(_comPort : TComPort) : integer;
var res : integer;
begin
  res := 0;
  try
    _comPort.Close;
    res := 1;
  except
    on E: Exception do
      begin
        res := 0;
      end;
    end;
 result := res;
end;
//-------------------------------------------------------


//------------------------------------------------------
//
// Envoie des données sur le port COM precedemment ouvert.
//
//------------------------------------------------------


  function sendSerialData(CPort : TComPort; dataToSend : string) : integer;
  var res : integer;
    begin
      res := 0;
      try
        CPort.ClearBuffer(true, true);
        CPort.WriteStr(dataToSend);
        res := 1;
      except
        res := 0;
    end;
    result := res;
  end;

//-------------------------------------------------------




//------------------------------------------------------
//
// Recupere des données sur le port com avec un timeout
//
//------------------------------------------------------
function getSerialData(CPort : TComPort; timeout: integer; lengthStr : integer) : string;
var   rxData : string;
      Long : integer;
begin;
  try
    rxData := '';

    CPort.Timeouts.ReadTotalConstant := timeout;
    // Lecture bloquante de la longueur demandée
    Long := CPort.ReadStr(rxData,lengthStr);
    // Supression de ce qui traine dans le buffer d'entrée
    CPort.ClearBuffer(True,False);
    result := rxData;
  except
      abort;
  end;
end;
//-------------------------------------------------------





//------------------------------------------------------
//
// Recupere des données sur le port com avec un timeout
//
//------------------------------------------------------

function getSerialDataForSyncInventory(CPort : TComPort; timeout: integer) : string;
  var   Event: TEvent;
        rxData : string;
        Events: TComEvents;
        tmpData : string;
        Long : integer;
        nbChars : integer;
        
  begin;
  try
    rxData := '';
    CPort.Timeouts.ReadTotalConstant := timeout;
    // Supression de ce qui traine dans le buffer d'entrée
    CPort.ClearBuffer(True,False);
    // Lecture bloquante de la longueur demandée
    nbChars := CPort.ReadStr(rxData,1);

    if (nbChars > 0)  then
    begin
      nbChars := ord(rxData[1]) ;
      CPort.ReadStr(rxData,nbChars);
      result := chr(nbChars) + rxData;
    end;

  except
      abort;
  end;
  end;

//-------------------------------------------------------


end.

