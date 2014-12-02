; -- 64BitTwoArch.iss --
; Demonstrates how to install a program built for two different
; architectures (x86 and x64) using a single installer.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!
#define AppName "ScopeSync MIDI Controllers Package"
#define RegSubKey "Software\ScopeSync\Packages"

[Setup]
AppName={#AppName}
AppVersion=0.42b
DefaultDirName={pf}\{#AppName}
DefaultGroupName={#AppName}
UninstallDisplayIcon={app}\{#AppName}.ico
AppPublisher=BC Modular
AppPublisherURL=http://www.bcmodular.co.uk/
AppCopyright=Copyright (C) 2014 BC Modular
AppContact=
AppSupportURL=http://bcmodular.co.uk/forum/
AppUpdatesURL=http://www.bcmodular.co.uk
Compression=lzma2
SolidCompression=yes
LicenseFile=license.txt
; "ArchitecturesInstallIn64BitMode=x64" requests that the install be
; done in "64-bit mode" on x64, meaning it should use the native
; 64-bit Program Files directory and the 64-bit view of the registry.
; On all other architectures it will install in "32-bit mode".
ArchitecturesInstallIn64BitMode=x64
; Note: We don't set ProcessorsAllowed because we want this
; installation to run on all architectures (including Itanium,
; since it's capable of running 32-bit code too).
WizardImageFile=C:\development\github\scopesync\Work In Progress\MIDI Controllers\Setup\Images\MIDIctrlrWizImage.bmp
VersionInfoVersion=0.4.2

[Types]
Name: "full"; Description: "Full installation"

[Files]
; Install MyProg-x64.exe if running in 64-bit mode (x64; see above),
; MyProg.exe otherwise.
; Place all x64 files here
; Place all x86 files here, first one should be marked 'solidbreak'
; Place all common files here, first one should be marked 'solidbreak'
Source: "..\Configurations\*"; DestDir: "{code:GetSSDir}\MIDI Controllers"; Flags: ignoreversion recursesubdirs solidbreak;
Source: "..\Layouts\*"; DestDir: "{code:GetSSDir}\MIDI Controllers"; Flags: ignoreversion recursesubdirs;

[Icons]
Name: "{group}\Uninstall {#AppName}"; Filename: "{uninstallexe}"

[Code]
// global vars
var
  DirPage: TInputDirWizardPage;
  DirPageID: Integer;

function GetSSDir(Param: String): String;
var
  SSDir: String;
begin
  SSDir := DirPage.Values[0];
  RegWriteStringValue(HKEY_CURRENT_USER, '{#RegSubKey}', 'Software\BCModular\ScopeSync', SSDir);
  { Return the selected SSDir }
  Result := SSDir;
end;

procedure InitializeWizard;
var
  SSDir: String;
begin
  if not(RegQueryStringValue(HKEY_CURRENT_USER, '{#RegSubKey}', 'Software\BCModular\ScopeSync', SSDir)) then
  begin
    SSDir := ExpandConstant('{pf}\ScopeSync');
  end;

  DirPage := CreateInputDirPage(
    wpSelectComponents,
    'Select Directories',
    'Where should files be installed?',
    '',
    False,
    '{#AppName}'
  );

  DirPage.Add('ScopeSync Installation Directory');
  DirPage.Values[0] := SSDir;

end;
