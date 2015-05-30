; -- localtube.iss --

[Setup]
AppName=localtube
AppVersion=1.0
DefaultDirName={pf}\localtube
DefaultGroupName=localtube
UninstallDisplayIcon={app}\localtube.exe
Compression=lzma2
SolidCompression=yes
OutputDir=userdocs:localtube
SetupIconFile="E:\creation\localtube\images\icon.ico"
OutputBaseFilename=localtube_installer

[Files]
Source: "release_windows\localtube.exe"; DestDir: "{app}"
Source: "release_windows\icudt53.dll"; DestDir: "{app}"
Source: "release_windows\icuin53.dll"; DestDir: "{app}"
Source: "release_windows\icuuc53.dll"; DestDir: "{app}"
Source: "release_windows\libgcc_s_dw2-1.dll"; DestDir: "{app}"
Source: "release_windows\libstdc++-6.dll"; DestDir: "{app}"
Source: "release_windows\libwinpthread-1.dll"; DestDir: "{app}"
Source: "release_windows\Qt5Core.dll"; DestDir: "{app}"
Source: "release_windows\Qt5Gui.dll"; DestDir: "{app}"
Source: "release_windows\Qt5Network.dll"; DestDir: "{app}"
Source: "release_windows\Qt5Widgets.dll"; DestDir: "{app}"
Source: "release_windows\Qt5Network.dll"; DestDir: "{app}"
Source: "release_windows\libeay32.dll"; DestDir: "{app}"
Source: "release_windows\ssleay32.dll"; DestDir: "{app}"
Source: "release_windows\platforms\qwindows.dll"; DestDir: "{app}\platforms"

[Icons]
Name: "{group}\localtube"; Filename: "{app}\localtube.exe"
Name: "{userstartup}\localtube"; Filename: "{app}\localtube.exe"; Check: autoStartApp

[Run]
Filename: "{app}\localtube.exe"; Description: "Launch localtube"; Flags: nowait postinstall skipifsilent

[Code]
var
  Opt : TInputOptionWizardPage; 

procedure InitializeWizard();
var
  I : Integer; 
  ControlType : Boolean;
begin
  Opt := CreateInputOptionPage(1,'Options','','',false, false);
  Opt.Add('Start localtube at boot.');
  Opt.Values[0] := true;
end;

function autoStartApp: Boolean;
begin
  Result := Opt.Values[0];
end;
