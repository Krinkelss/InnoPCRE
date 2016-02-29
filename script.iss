[Setup]
AppName=My Program
AppVerName=My Program version 1.5
DefaultDirName={pf}\My Program
DefaultGroupName=My Program

[Languages]
Name: rus; MessagesFile: compiler:Languages\Russian.isl

[Files]
Source: InnoPCRE.dll; Flags: dontcopy

[Icons]
Name: {group}\Uninstall; IconFilename: {app}\unins000.exe; Filename: {app}\unins000.exe

[Code]
function pcreInitialize( mPattern: String; mflags: String ): boolean; external 'pcreInitialize@files:InnoPCRE.dll stdcall';
function search( Str: String; Offset: integer ): integer; external 'search@files:InnoPCRE.dll stdcall';
function get_match( Num: Integer ): pChar; external 'get_match@files:InnoPCRE.dll stdcall';
function AllMatch(): Integer; external 'AllMatch@files:InnoPCRE.dll stdcall';
function NextPos(): Integer; external 'NextPos@files:InnoPCRE.dll stdcall';

procedure InitializeWizard();
var
  Errcode: integer;
  StrLen: integer;
  TestStr: String;
begin
    // Инициализация
    if pcreInitialize( '(One)', 'gi' ) = false then
    begin
      MsgBox( 'Error initialize pcre', mbError, MB_OK);
      Exit;
    end;

    // Пробуем искать
    TestStr := 'One 123 123 123 One';
    StrLen := Length( TestStr );
    while NextPos() < StrLen do
    begin
      Errcode := search( TestStr, NextPos() );
      if( Errcode > 0 ) then
      begin
        MsgBox( get_match( 0 ), mbError, MB_OK);
      end else
        Exit;
    end;
end;
