unit Main;

{
$GPGGA

Global Positioning System Fix Data

Name 	Example Data 	Description
Sentence Identifier 	$GPGGA 	Global Positioning System Fix Data
Time 	170834 	17:08:34 Z
Latitude 	4124.8963, N 	41d 24.8963' N or 41d 24' 54" N
Longitude 	08151.6838, W 	81d 51.6838' W or 81d 51' 41" W
Fix Quality:
- 0 = Invalid
- 1 = GPS fix
- 2 = DGPS fix 	1 	Data is from a GPS fix
Number of Satellites 	05 	5 Satellites are in view
Horizontal Dilution of Precision (HDOP) 	1.5 	Relative accuracy of horizontal position
Altitude 	280.2, M 	280.2 meters above mean sea level
Height of geoid above WGS84 ellipsoid 	-34.0, M 	-34.0 meters
Time since last DGPS update 	blank 	No last update
DGPS reference station id 	blank 	No station id
Checksum 	*75 	Used by program to check for transmission errors


$GPRMC

Recommended minimum specific GPS/Transit data

eg1. $GPRMC,081836,A,3751.65,S,14507.36,E,000.0,360.0,130998,011.3,E*62
eg2. $GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68


           225446       Time of fix 22:54:46 UTC
           A            Navigation receiver warning A = OK, V = warning
           4916.45,N    Latitude 49 deg. 16.45 min North
           12311.12,W   Longitude 123 deg. 11.12 min West
           000.5        Speed over ground, Knots
           054.7        Course Made Good, True
           191194       Date of fix  19 November 1994
           020.3,E      Magnetic variation 20.3 deg East
           *68          mandatory checksum


eg3. $GPRMC,220516,A,5133.82,N,00042.24,W,173.8,231.8,130694,004.2,W*70
              1    2    3    4    5     6    7    8      9     10  11 12


      1   220516     Time Stamp
      2   A          validity - A-ok, V-invalid
      3   5133.82    current Latitude
      4   N          North/South
      5   00042.24   current Longitude
      6   W          East/West
      7   173.8      Speed in knots
      8   231.8      True course
      9   130694     Date Stamp
      10  004.2      Variation
      11  W          East/West
      12  *70        checksum


eg4. $GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh
1    = UTC of position fix
2    = Data status (V=navigation receiver warning)
3    = Latitude of fix
4    = N or S
5    = Longitude of fix
6    = E or W
7    = Speed over ground in knots
8    = Track made good in degrees True
9    = UT date
10   = Magnetic variation degrees (Easterly var. subtracts from true course)
11   = E or W
12   = Checksum




$GPGSA

GPS DOP and active satellites

eg1. $GPGSA,A,3,,,,,,16,18,,22,24,,,3.6,2.1,2.2*3C
eg2. $GPGSA,A,3,19,28,14,18,27,22,31,39,,,,,1.7,1.0,1.3*35


1    = Mode:
       M=Manual, forced to operate in 2D or 3D
       A=Automatic, 3D/2D
2    = Mode:
       1=Fix not available
       2=2D
       3=3D
3-14 = IDs of SVs used in position fix (null for unused fields)
15   = PDOP
16   = HDOP
17   = VDOP



}



interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, CPort, SerialComm, ExtCtrls, Tools_unit, jpeg;

type
  TForm1 = class(TForm)
    mNMEA: TMemo;
    ComPort: TComPort;
    tGPS: TTimer;
    Button1: TButton;
    gGPGGA: TGroupBox;
    Label3: TLabel;
    Label4: TLabel;
    eSatFixQuality: TEdit;
    eNbSat: TEdit;
    bDecSatFixQuality: TButton;
    bIncSatFixQuality: TButton;
    bDecNbSat: TButton;
    bIncNbSat: TButton;
    gGPRMC: TGroupBox;
    bDecHdg: TButton;
    bIncHdg: TButton;
    bDecSpd: TButton;
    bIncSpd: TButton;
    bDecAlt: TButton;
    bIncAlt: TButton;
    bValidSatFix: TButton;
    lFixValid: TLabel;
    eStartLatitude: TEdit;
    eLatD: TEdit;
    Label1: TLabel;
    Label2: TLabel;
    eStartLongitude: TEdit;
    eLonD: TEdit;
    gNav: TGroupBox;
    bNorth: TButton;
    bSouth: TButton;
    bEast: TButton;
    bWest: TButton;
    GroupBox4: TGroupBox;
    bOpenComPort: TButton;
    eComPort: TEdit;
    eBaudRate: TEdit;
    lBaudRate: TLabel;
    lComPort: TLabel;
    bStart: TButton;
    bStop: TButton;
    Label7: TLabel;
    Label8: TLabel;
    Label9: TLabel;
    eHdg: TEdit;
    eAltitude: TEdit;
    gGPGSA: TGroupBox;
    Label10: TLabel;
    bFixNo: TButton;
    bFix2D: TButton;
    bFix3D: TButton;
    Label5: TLabel;
    Label6: TLabel;
    bDecVDOP: TButton;
    eVdop: TEdit;
    bIncVDOP: TButton;
    bDecHDOP: TButton;
    eHdop: TEdit;
    bIncHDOP: TButton;
    bClearLog: TButton;
    Label11: TLabel;
    bDecPDOP: TButton;
    bIncPDOP: TButton;
    ePDOP: TEdit;
    Image1: TImage;
    Label12: TLabel;
    eSpd: TEdit;
    GroupBox1: TGroupBox;
    cbGlitchLatitude: TCheckBox;
    cbGlitchCRC: TCheckBox;
    bStartNav: TButton;
    Label13: TLabel;
    Label14: TLabel;
    lLat: TLabel;
    lLon: TLabel;
    procedure bStartNavClick(Sender: TObject);
    procedure cbGlitchCRCClick(Sender: TObject);
    procedure cbGlitchLatitudeClick(Sender: TObject);
    procedure bGlitchLatitudeClick(Sender: TObject);
    procedure bIncPDOPClick(Sender: TObject);
    procedure bDecPDOPClick(Sender: TObject);
    procedure bClearLogClick(Sender: TObject);
    procedure bFix3DClick(Sender: TObject);
    procedure bFix2DClick(Sender: TObject);
    procedure bFixNoClick(Sender: TObject);
    procedure bIncHDOPClick(Sender: TObject);
    procedure bIncVDOPClick(Sender: TObject);
    procedure bDecHDOPClick(Sender: TObject);
    procedure bDecVDOPClick(Sender: TObject);
    procedure bDecNbSatClick(Sender: TObject);
    procedure bIncNbSatClick(Sender: TObject);
    procedure bIncSatFixQualityClick(Sender: TObject);
    procedure bDecSatFixQualityClick(Sender: TObject);
    procedure bValidSatFixClick(Sender: TObject);
    procedure bDecAltClick(Sender: TObject);
    procedure bDecSpdClick(Sender: TObject);
    procedure bDecHdgClick(Sender: TObject);
    procedure bIncHdgClick(Sender: TObject);
    procedure bIncAltClick(Sender: TObject);
    procedure bWestClick(Sender: TObject);
    procedure bEastClick(Sender: TObject);
    procedure bSouthClick(Sender: TObject);
    procedure bNorthClick(Sender: TObject);
    procedure bIncSpdClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure tGPSTimer(Sender: TObject);
    procedure bStopClick(Sender: TObject);
    procedure bStartClick(Sender: TObject);
    procedure bOpenComPortClick(Sender: TObject);
  private

    procedure updateFixMode();
    procedure initValues();
    procedure addEvent(strEvent : string);
    function makeNMEA(NMEAType : string) : string;
    


    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

  COM_port : integer;
  COM_baudrate : integer;
  COM_parity : string;

    comPortIsOpen : integer = 0;

    strSatTime : string;
    strSatFix : string;
    strLat : string;
    strLatD : string;
    strLon : string;
    strLonD : string;
    strSpeedKnots : string;
    strBearing : string;
    strUTCDate : string;
    strHexChecksum : string;

    strAltitude : string;
    strFixQuality : string;
    strNbSat : string;
    strHDOP : string;
    strVDOP : string;

    decFixMode : integer;        // 1=no fix 1=2D 3=3D

    // Set to 1 to corrupt CRC.
    CRCGlitch : integer = 0;
    LatitudeGlitch : integer = 0;
    


    decSatTime : double; // = 165724.000;
    decLat : double; //= 0543.1333;
    decLatD : double; //= 'N';
    decLon : double; //  '04509.2333';
    decSpeedKnots : double; // = '104,4';
    decBearing : double; //  '128';
    decUTCDate : double; //  '200804';
    decHexChecksum : double; //  'AB';

    decAltitude : double;

    decFixQuality : integer;
    decNbSat : integer;
    decHDOP : integer;
    decVDOP : integer;
    decPDOP : integer;



    moveNorth : integer;
    moveSouth : integer;
    moveEast : integer;
    moveWest : integer;

    sendingInProgress : integer = 0;


implementation

{$R *.dfm}



procedure TForm1.updateFixMode();
begin
end;

procedure TForm1.initValues();
begin
    decSatTime := 165724.000;
    decLat :=  0543.1333;
    decLon := 04509.2333;
    decSpeedKnots := 0;
    decBearing := 0;
    decUTCDate :=  200804;

    decAltitude := 850;


    strSatFix := 'A';     // A=fixed V=inValid.
    strLatD := 'N';
    strLonD := 'E';


    decFixQuality := 1;
    decNbSat := 5;
    decHDOP  := 4;
    decVDOP := 4;
    decPDOP := 4;


    decFixMode := 3;


    moveNorth := 0;
    moveSouth := 0;
    moveEast := 0;
    moveWest := 0;

    strLat := FormatFloat('####.####', decLat);
    while (length(strLat) < 9) do
    begin
        strLat := '0' +  strLat;
    end;

    strLon := FormatFloat('#####.####', decLon);
    while (length(strLon) < 10) do
    begin
        strLon := '0' +  strLon;
    end;

    if (strSatFix = 'A') then
    begin
        bValidSatFix.Caption := 'Switch to inValid';
    end else
    begin
       bValidSatFix.Caption := 'Switch to vAlid';
    end;

    eLatD.Text :=  strLatD;
    eLonD.Text :=   strLonD;
    eStartLatitude.Text := strLat;
    eStartLongitude.Text := strLon;

    eSatFixQuality.Text := intToStr(decFixQuality);




    eSpd.Text := format('%.1f', [decSpeedKnots * 1.83]);
    eAltitude.Text := format('%.1f', [decAltitude]);
    eHdg.text := format('%.1f', [decBearing]);

    eNbSat.Text := intToStr(decNbSat);

    eVDOP.Text := intToStr(decVDOP);
    eHDOP.Text := intToStr(decHDOP);
    ePDOP.Text := intToStr(decPDOP);

    strSatTime := '165724.000';

   // updateFixMode();

end;


// --------------------------------------------------------------------------
//
// Ajout d'une ligne dans le memo lEvents
//
// --------------------------------------------------------------------------
procedure TForm1.addEvent(strEvent : string);
begin
  mNMEA.Text := strEvent + chr(13) + chr(10) + mNMEA.Text;
end;



procedure TForm1.bClearLogClick(Sender: TObject);
begin
  mNMEA.Text := '';
end;

procedure TForm1.bDecAltClick(Sender: TObject);
begin
  if (decAltitude > 0) then
  begin
    decAltitude := decAltitude - 10;
    eAltitude.Text := format('%.1f', [decAltitude]);
  end;
end;

procedure TForm1.bDecHdgClick(Sender: TObject);
begin
  if ((decBearing > 10) and (decBearing < 361)) then
  begin
    decBearing := decBearing - 10;
    eHdg.Text := format('%.1f', [decBearing]);
  end;
end;

procedure TForm1.bDecHDOPClick(Sender: TObject);
begin
 if (decHDOP > 0) then begin
   decHDOP := decHDOP-1;
   eHDOP.Text := intToStr(decHDOP);
 end;
end;

procedure TForm1.bDecNbSatClick(Sender: TObject);
begin
 if (decNbSat > 0) then begin
   decNbSat := decNbSat-1;
   eNbSat.Text := intToStr(decNbSat);
 end;
end;

procedure TForm1.bDecPDOPClick(Sender: TObject);
begin
 if (decPDOP > 0) then begin
   decPDOP := decPDOP-1;
   ePDOP.Text := intToStr(decPDOP);
 end;
end;

procedure TForm1.bDecSatFixQualityClick(Sender: TObject);
begin
 if (decFixQuality > 0) then begin
   decFixQuality := decFixQuality-1;
   eSatFixQuality.Text := intToStr(decFixQuality);
 end;
end;

procedure TForm1.bDecSpdClick(Sender: TObject);
begin
  if (decSpeedKnots > 0) then
  begin
    decSpeedKnots := decSpeedKnots - 1;
    eSpd.Text := format('%.1f', [decSpeedKnots * 1.83]);
  end;
end;

procedure TForm1.bDecVDOPClick(Sender: TObject);
begin
 if (decVDOP > 0) then begin
   decVDOP := decVDOP-1;
   eVDOP.Text := intToStr(decVDOP);
 end;
end;

procedure TForm1.bEastClick(Sender: TObject);
begin
  if (moveEast = 1) then
  begin
    moveEast := 0;
    bEast.Caption := 'Move east';
    bWest.Enabled := true;
  end else
  begin
    moveEast := 1;
    bEast.Caption := 'Stop';
    bWest.Enabled := false;
  end;
end;

procedure TForm1.bIncAltClick(Sender: TObject);
begin
  if (decAltitude >= 0) then
  begin
    decAltitude := decAltitude + 10;
    eAltitude.Text := format('%.1f', [decAltitude]);
    strAltitude := eAltitude.Text;
  end;
end;

procedure TForm1.bIncHdgClick(Sender: TObject);
begin
  if ((decBearing >= 0) and (decBearing < 351)) then
  begin
    decBearing := decBearing + 100;
    eHdg.Text := format('%.1f', [decBearing]);

  end;
end;

procedure TForm1.bIncHDOPClick(Sender: TObject);
begin
 if (decHDOP >= 0) then begin
   decHDOP := decHDOP+1;
   eHDOP.Text := intToStr(decHDOP);
 end;
end;

procedure TForm1.bIncNbSatClick(Sender: TObject);
begin
 if (decNbSat >= 0) then begin
   decNbSat := decNbSat+1;
   eNbSat.Text := intToStr(decNbSat);
 end;
end;

procedure TForm1.bIncPDOPClick(Sender: TObject);
begin
 if (decPDOP >= 0) then begin
   decPDOP := decPDOP+1;
   ePDOP.Text := intToStr(decPDOP);
 end;
end;

procedure TForm1.bIncSatFixQualityClick(Sender: TObject);
begin
 if (decFixQuality < 2) then begin
   decFixQuality := decFixQuality+1;
   eSatFixQuality.Text := intToStr(decFixQuality);
 end;
end;

procedure TForm1.bIncSpdClick(Sender: TObject);
begin
  if (decSpeedKnots >= 0) then
  begin
    decSpeedKnots := decSpeedKnots + 1;
    eSpd.Text := format('%.1f', [decSpeedKnots * 1.83]);
  end;
end;

procedure TForm1.bIncVDOPClick(Sender: TObject);
begin
 if (decVDOP >= 0) then begin
   decVDOP := decVDOP+1;
   eVDOP.Text := intToStr(decVDOP);
 end;
end;

procedure TForm1.bNorthClick(Sender: TObject);
begin
  if (moveNorth = 1) then
  begin
    moveNorth := 0;
    bNorth.Caption := 'Move north';
    bSouth.Enabled := true;
  end else
  begin
    moveNorth := 1;
    bNorth.Caption := 'Stop';
    bSouth.Enabled := false;
  end;
end;

procedure TForm1.bOpenComPortClick(Sender: TObject);
begin

  COM_port := strToInt(eComPort.Text);
  COM_baudrate := strToInt(eBaudRate.Text);
  COM_parity := 'even';

  comPortIsOpen := 0;



  try
     if (OpenComPort(ComPort, COM_port, COM_baudrate, COM_parity) = 1) then
     begin
        addEvent('Comport successfuly opened !');
          comPortIsOpen := 1;

     end else addEvent('Error while opening com port.');
     Except
      on e : Exception do
        begin
          showmessage('Error while trying to open comport ' + intToStr(COM_port));
        end;
      end;



end;

procedure TForm1.bSouthClick(Sender: TObject);
begin
  if (moveSouth = 1) then
  begin
    moveSouth := 0;
    bSouth.Caption := 'Move south';
    bNorth.Enabled := true;
  end else
  begin
    moveSouth := 1;
    bSouth.Caption := 'Stop';
    bNorth.Enabled := false;
  end;
end;

procedure TForm1.bStartClick(Sender: TObject);
begin
  tGPS.enabled := true;
end;

procedure TForm1.bStartNavClick(Sender: TObject);
begin

if (comPortIsOpen = 0) then begin
  showmessage('Please open com port first.');
  exit;
end;

if (bStartNav.Caption = 'START') then
begin
   tGPS.Enabled := true;
   bStartNav.Caption := 'STOP';
end else
begin
   tGPS.Enabled := false;
   bStartNav.Caption := 'START';
end;
end;

procedure TForm1.bStopClick(Sender: TObject);
begin
  tGPS.enabled := false;
end;

procedure TForm1.Button1Click(Sender: TObject);
begin
  Application.Terminate;
end;

procedure TForm1.bGlitchLatitudeClick(Sender: TObject);
begin
  decLat := 0;
end;

procedure TForm1.bFix2DClick(Sender: TObject);
begin
        decFixMode := 2;
end;

procedure TForm1.bFix3DClick(Sender: TObject);
begin
        decFixMode := 3;
end;

procedure TForm1.bFixNoClick(Sender: TObject);
begin
  decFixMode := 1;
end;

procedure TForm1.bValidSatFixClick(Sender: TObject);
begin
  if (strSatFix = 'A') then
  begin
        strSatFix := 'V';
        bValidSatFix.Caption := 'Switch to vAlid';
  end else
  begin
        strSatFix := 'A';
        bValidSatFix.Caption := 'Switch to inValid';
  end;
end;

procedure TForm1.bWestClick(Sender: TObject);
begin
  if (moveWest = 1) then
  begin
    moveWest := 0;
    bWest.Caption := 'Move west';
    bEast.Enabled := true;
  end else
  begin
    moveWest := 1;
    bWest.Caption := 'Stop';
    bEast.Enabled := false;
  end;
end;

procedure TForm1.cbGlitchCRCClick(Sender: TObject);
begin
if (cbGlitchCRC.Checked) then
begin
    CRCGlitch := 1;
end else
begin
      CRCGlitch := 0;
end;
end;

procedure TForm1.cbGlitchLatitudeClick(Sender: TObject);
begin
if (cbGlitchLatitude.Checked) then
begin
        LatitudeGlitch := 1;
end else
begin
        LatitudeGlitch := 0;
end;

end;

procedure TForm1.FormCreate(Sender: TObject);
begin
  mNMEA.Text := 'Welcome to GPS Simulator. Please set com port parameters, then open com port and click START to navigate.';
  DecimalSeparator := '.';
  initValues();
end;

procedure TForm1.tGPSTimer(Sender: TObject);
begin
if (sendingInProgress = 0) then
begin
  sendingInProgress := 1;
  if (moveNorth = 1) then decLat:=decLat+0.1;
  if (moveSouth = 1) then decLat:=decLat-0.1;
  if (moveEast = 1) then decLon:=decLon+0.1;
  if (moveWest = 1) then decLon:=decLon-0.1;

  addEvent(makeNMEA('GPRMC'));
  sendSerialData(ComPort, makeNMEA('GPRMC') + chr(10) + chr(13) );
  wait(50);

   lLat.Caption := strLat + ' ' + strLatD;
   lLon.Caption := strLon + ' ' + strLonD;


  addEvent(makeNMEA('GPGGA'));
  sendSerialData(ComPort, makeNMEA('GPGGA') + chr(10) + chr(13) );
  wait(50);

  addEvent(makeNMEA('GPGSA'));
  sendSerialData(ComPort, makeNMEA('GPGSA') + chr(10) + chr(13) );
  wait(50);
  sendingInProgress := 0;
end;

end;


function TForm1.makeNMEA(NMEAType : string) : string;
  var res :  string;
  begin
  if (NMEAType = 'GPRMC') then
  begin

    strLat := FormatFloat('####.####', decLat);
    while (length(strLat) < 9) do
    begin
        strLat := '0' +  strLat;
    end;

    if (LatitudeGlitch = 1) then
    begin
      strLat := '10';
    end;

    strLon := FormatFloat('#####.####', decLon);
    while (length(strLon) < 10) do
    begin
        strLon := '0' +  strLon;
    end;

    strSpeedKnots := format('%.1f', [decSpeedKnots]);
    strBearing := format('%.1f', [decBearing]);

    res := 'GPRMC,' + strSatTime + ',' + strSatFix + ',' + strLat + ',' +
    strLatD + ',' + strLon + ',' + strLonD + ',' + strSpeedKnots + ',' +
    strBearing + ',' + strUTCDate +',0.0,E';
    strHexCheckSum := format('%0:.2x', [genCRC(res)]);
    res := '$' + res + '*' + strHexCheckSum;

    // Debug
    //res := '$GPRMC,165815.000,A,0543.4044,N,04509.2557,E,24.3,18.0,160309,0.0,E*6C';


  end;

  if (NMEAType = 'GPGSA') then
  begin
    res := 'GPGSA,A,' + intToStr(decFixMode) + ',04,05,,09,12,,,24,,,,,' + intToStr(decPDOP) + ','
    + intToStr(decHDOP) + ',' + intToStr(decVDOP);
    strHexCheckSum := format('%0:.2x', [genCRC(res)]);
    if (CRCGlitch = 1)  then strHexCheckSum := 'FF';
    res := '$' + res + '*' + strHexCheckSum;

    // debug
    //res :=' $GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,2.3,2.1*39';
  end;


  if (NMEAType = 'GPGGA') then
  begin



    strAltitude := format('%.1f', [decAltitude]);
    strFixQuality := intToStr(decFixQuality);
    strNbSat := intToStr(decNbSat);
    strHDOP := intToStr(decHDOP);
    strVDOP := intToStr(decVDOP);

    
    res := 'GPGGA,' + strSatTime + ',' + strLat + ',' + strLatD + ',' +
    strLon + ',' + strLonD + ',' + strFixQuality + ',' + strNbSat + ',' +
    strHDOP + ',' + strAltitude + ',M,,M,,';
    strHexCheckSum := format('%0:.2x', [genCRC(res)]);
    res := '$' + res + '*' + strHexCheckSum;

    // debug
    // res := '$GPGGA,165814.000,0543.3980,N,04509.2536,E,1,7,0.00,1006.0,M,,M,,*77';
  end;

  result := res;
end;

end.
