; -- 64BitTwoArch.iss --
; Demonstrates how to install a program built for two different
; architectures (x86 and x64) using a single installer.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!
#define AppName "ScopeSync"
#define RegSubKey "Software\BCModular\ScopeSync"

[Setup]
AppName={#AppName}
AppVersion=0.4.0
DefaultDirName={pf}\{#AppName}
DefaultGroupName={#AppName}
UninstallDisplayIcon={app}\{#AppName}.ico
AppPublisher=BC Modular
AppPublisherURL=http://www.bcmodular.co.uk/
AppCopyright=Copyright (C) 2014 BC Modular
AppContact=support@bcmodular.co.uk
AppSupportURL=http://bcmodular.co.uk/forum/
AppUpdatesURL=http://www.scopesync.co.uk/
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

[Types]
Name: "typical"; Description: "Typical installation"
Name: "full"; Description: "Full installation"
Name: "compact"; Description: "Compact installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Files]
; Install MyProg-x64.exe if running in 64-bit mode (x64; see above),
; MyProg.exe otherwise.
; Place all x64 files here
Source: "..\VST Plugin\ScopeSyncVST-x64.dll"; DestDir: "{code:GetVST2Dir}\{#AppName}"; Flags: ignoreversion; Check: Is64BitInstallMode; Components:VSTPlugin\64VST2
Source: "..\VST Plugin\ScopeSyncVST-x64.vst3"; DestDir: "{cf}\VST3\{#AppName}"; Flags: ignoreversion; Check: Is64BitInstallMode; Components:VSTPlugin\64VST3
; Place all x86 files here, first one should be marked 'solidbreak'
; Place all common files here, first one should be marked 'solidbreak'
Source: "..\Scope DLL\*"; DestDir: "{code:GetScopeDir}\App\Dll"; Flags: ignoreversion solidbreak; Components: ScopeDLL
Source: "..\BC Modular Module\*"; DestDir: "{code:GetModularDir}"; Flags: ignoreversion; Components: BCMod
Source: "..\VST Plugin\ScopeSyncVST.dll"; DestDir: "{code:GetVST2Dir}\{#AppName}"; Flags: ignoreversion; Components:VSTPlugin\32VST2
Source: "..\VST Plugin\ScopeSyncVST.vst3"; DestDir: "{cf32}\{#AppName}"; Flags: ignoreversion; Components:VSTPlugin\32VST3
Source: "..\Configurations\*"; DestDir: "{app}\Configurations"; Flags: ignoreversion recursesubdirs; Components: Configurations
Source: "..\Layouts\*"; DestDir: "{app}\Layouts"; Flags: ignoreversion recursesubdirs; Components: Layouts
Source: "..\Presets\*"; DestDir: "{app}\Presets"; Flags: ignoreversion recursesubdirs; Components: Presets
Source: "..\Tutorials\*"; DestDir: "{app}\Tutorials"; Flags: ignoreversion recursesubdirs; Components: Tutorials
Source: "..\Max For Live\ScopeSync\*"; DestDir: "{code:GetAbletonUserLibDir}\{#AppName}"; Flags: ignoreversion; Components: M4L
Source: "..\Max Package\*"; DestDir: "{code:GetMaxDir}\Packages"; Flags: ignoreversion recursesubdirs; Components: M4L

[Icons]
Name: "{group}\Uninstall {#AppName}"; Filename: "{uninstallexe}"

[Components]
Name: "ScopeDLL"; Description: "Scope Module"; Types: full typical compact
Name: "BCMod"; Description: "BC Modular Modules"; Types: full typical compact
Name: "Configurations"; Description: "Configuration Library"; Types: full typical compact
Name: "Layouts"; Description: "Layout Library"; Types: full typical compact
Name: "Presets"; Description: "Preset Library"; Types: full typical compact
Name: "Tutorials"; Description: "Tutorials"; Types: full typical
Name: "VSTPlugin"; Description: "VST Plugins"; Types: full typical compact
Name: "VSTPlugin\32VST2"; Description: "32-bit VST2 Plugin"; Types: full typical compact
Name: "VSTPlugin\64VST2"; Description: "64-bit VST2 Plugin"; Types: full typical compact; Check: Is64BitInstallMode
Name: "VSTPlugin\32VST3"; Description: "32-bit VST3 Plugin (Experimental)"
Name: "VSTPlugin\64VST3"; Description: "64-bit VST3 Plugin (Experimental)"; Check: Is64BitInstallMode
Name: "M4L"; Description: "Max For Live Patches"; Types: full

[Registry]
Root: HKCU; Subkey: "Software\ScopeSync"; ValueType: string; ValueName: "InstallLocation"; ValueData: "{app}"
[Code]
// global vars
var
  ScopeDirPage1: TInputDirWizardPage;
  ScopeDirPage1ID: Integer;
  ScopeDirPage2: TInputDirWizardPage;
  ScopeDirPage2ID: Integer;
  AdditionalDirPage1: TInputDirWizardPage;
  AdditionalDirPage1ID: Integer;
  AdditionalDirPage2: TInputDirWizardPage;  
  AdditionalDirPage2ID: Integer;

function GetScopeDir(Param: String): String;
var
  ScopeDir: String;
begin
  ScopeDir := ScopeDirPage1.Values[0];
  RegWriteStringValue(HKEY_CURRENT_USER, '{#RegSubKey}', 'ScopeDir', ScopeDir);
  { Return the selected ScopeDir }
  Result := ScopeDir;
end;

function GetModularDir(Param: String): String;
var
  ModularDir: String;
begin
  ModularDir := ScopeDirPage2.Values[0];
  RegWriteStringValue(HKEY_CURRENT_USER, '{#RegSubKey}', 'ModularDir', ModularDir);
  { Return the selected ModularDir }
  Result := ModularDir;
end;

function GetVST2Dir(Param: String): String;
var
  VST2Dir: String;
begin
  VST2Dir := AdditionalDirPage1.Values[0];
  RegWriteStringValue(HKEY_CURRENT_USER, '{#RegSubKey}', 'VST2Dir', VST2Dir);
  { Return the selected VST2Dir }
  Result := VST2Dir;
end;

function GetAbletonUserLibDir(Param: String): String;
var
  AbletonUserLibDir: String;
begin
  AbletonUserLibDir := AdditionalDirPage2.Values[0];
  RegWriteStringValue(HKEY_CURRENT_USER, '{#RegSubKey}', 'AbletonUserLibDir', AbletonUserLibDir);
  { Return the selected AbletonUserLibDir }
  Result := AbletonUserLibDir;
end;
     
function GetMaxDir(Param: String): String;
var
  MaxDir: String;
begin
  MaxDir := AdditionalDirPage2.Values[1];
  RegWriteStringValue(HKEY_CURRENT_USER, '{#RegSubKey}', 'MaxDir', MaxDir);
  { Return the selected MaxDir }
  Result := MaxDir;
end;
     
procedure InitializeWizard;
var
  VST2Dir: String;
  ScopeDir: String;
  ModularDir: String;
  AbletonUserLibDir: String;
  MaxDir: String;
  VST3Text: String;
begin
  if not(RegQueryStringValue(HKEY_CURRENT_USER, '{#RegSubKey}', 'VST2Dir', VST2Dir)) then
  begin
    VST2Dir := ExpandConstant('{sd}\VST');
  end;  

  if not(RegQueryStringValue(HKEY_CURRENT_USER, '{#RegSubKey}', 'ScopeDir', ScopeDir)) then
  begin
    ScopeDir := ExpandConstant('{pf}\Scope PCI');
  end; 

  if not(RegQueryStringValue(HKEY_CURRENT_USER, '{#RegSubKey}', 'ModularDir', ModularDir)) then
  begin
    ModularDir := ExpandConstant('{pf}\Scope PCI\Modular Modules\ScopeSync');
  end; 

  if not(RegQueryStringValue(HKEY_CURRENT_USER, '{#RegSubKey}', 'AbletonUserLibDir', AbletonUserLibDir)) then
  begin
    AbletonUserLibDir := ExpandConstant('{userdocs}\Ableton\User Library');
  end; 

  if not(RegQueryStringValue(HKEY_CURRENT_USER, '{#RegSubKey}', 'MaxDir', MaxDir)) then
  begin
    MaxDir := ExpandConstant('{pf32}\Cycling ''74\Max 6.1');
  end; 

  ScopeDirPage1 := CreateInputDirPage(
    wpSelectComponents,
    'Select Scope Directory',
    'Where should Scope files be installed?',
    '',
    False,
    '{#AppName}'
  );
  ScopeDirPage1.Add('Scope Installation Directory');
  ScopeDirPage1ID := ScopeDirPage1.ID;
  ScopeDirPage1.Values[0] := ScopeDir;
  
  ScopeDirPage2 := CreateInputDirPage(
    ScopeDirPage1ID,
    'Select Scope Modular Modules Directory',
    'Where should Scope Modular Modules be installed?',
    '',
    False,
    '{#AppName}'
  );
  ScopeDirPage2.Add('Scope Modular Modules Directory');
  ScopeDirPage2ID := ScopeDirPage2.ID;
  ScopeDirPage2.Values[0] := ModularDir;
  
  VST3Text := 'N.B. If selected, VST3 files will be installed in the standard locations, e.g. ' + ExpandConstant('{cf}\VST3\');
  
  AdditionalDirPage1 := CreateInputDirPage(
    ScopeDirPage2ID,
    'Select VST2 Directory',
    'Where should VST2 files be installed?',
    VST3Text,
    False,
    '{#AppName}'
  );
  AdditionalDirPage1.Add('VST2 Plugin Directory');
  AdditionalDirPage1ID := AdditionalDirPage1.ID;
  AdditionalDirPage1.Values[0] := VST2Dir;
  
  AdditionalDirPage2 := CreateInputDirPage(
    AdditionalDirPage1ID,
    'Select Max For Live Directories',
    'Where should Max For Live files be installed?',
    '',
    False,
    '{#AppName}'
  );
  AdditionalDirPage2.Add('Ableton User Library (for M4L patches)');
  AdditionalDirPage2.Add('Max Installation Directory (for Max Package)');
  AdditionalDirPage2ID := AdditionalDirPage2.ID;
  AdditionalDirPage2.Values[0] := AbletonUserLibDir;
  AdditionalDirPage2.Values[1] := MaxDir;

end;

function ShouldSkipPage(PageID: Integer): Boolean;
begin
  // initialize result to not skip any page (not necessary, but safer)
  Result := False;
  // if the page that is asked to be skipped is your custom page, then...
  if PageID = ScopeDirPage1ID then
    Result := not IsComponentSelected('ScopeDLL')
  else if PageID = ScopeDirPage2ID then
    Result := not IsComponentSelected('BCMod')
  else if PageID = AdditionalDirPage1ID then
    Result := not (IsComponentSelected('VSTPlugin\32VST2') or IsComponentSelected('VSTPlugin\64VST2'))
  else if PageID = AdditionalDirPage2ID then
    Result := not IsComponentSelected('M4L');
end;