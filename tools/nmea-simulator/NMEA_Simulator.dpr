program NMEA_Simulator;

uses
  Forms,
  Main in 'Main.pas' {Form1},
  SerialComm in 'SerialComm.pas',
  Tools_unit in 'Tools_unit.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
