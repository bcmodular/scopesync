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
Compression=lzma2
SolidCompression=yes
; "ArchitecturesInstallIn64BitMode=x64" requests that the install be
; done in "64-bit mode" on x64, meaning it should use the native
; 64-bit Program Files directory and the 64-bit view of the registry.
; On all other architectures it will install in "32-bit mode".
ArchitecturesInstallIn64BitMode=x64
; Note: We don't set ProcessorsAllowed because we want this
; installation to run on all architectures (including Itanium,
; since it's capable of running 32-bit code too).

[Files]
; Install MyProg-x64.exe if running in 64-bit mode (x64; see above),
; MyProg.exe otherwise.
; Place all x64 files here
Source: "..\VST Plugin\ScopeSyncVST-x64.dll"; DestDir: "{code:GetVST2Dir}\{#AppName}"; Check: Is64BitInstallMode; Components:VSTPlugin\64VST2
Source: "..\VST Plugin\ScopeSyncVST-x64.vst3"; DestDir: "{cf}\VST3\{#AppName}"; Check: Is64BitInstallMode; Components:VSTPlugin\64VST3
; Place all x86 files here, first one should be marked 'solidbreak'
; Place all common files here, first one should be marked 'solidbreak'
Source: "..\Scope DLL\*"; DestDir: "{code:GetScopeDir}\App\Dll"; Flags: solidbreak; Components: ScopeDLL
Source: "..\BC Modular Module\*"; DestDir: "{code:GetScopeDir}\Modular Modules\{#AppName}"; Components: BCMod
Source: "..\VST Plugin\ScopeSyncVST.dll"; DestDir: "{code:GetVST2Dir}\{#AppName}"; Components:VSTPlugin\32VST2
Source: "..\VST Plugin\ScopeSyncVST.vst3"; DestDir: "{cf32}\{#AppName}"; Components:VSTPlugin\32VST3
Source: "..\Configurations\*"; DestDir: "{app}\Configurations"; Flags: recursesubdirs; Components: Configurations
Source: "..\Layouts\*"; DestDir: "{app}\Layouts"; Flags: recursesubdirs; Components: Layouts
Source: "..\Presets\*"; DestDir: "{app}\Presets"; Flags: recursesubdirs; Components: Presets
Source: "..\Tutorials\*"; DestDir: "{app}\Tutorials"; Flags: recursesubdirs; Components: Tutorials
Source: "..\Max For Live\ScopeSync\*"; DestDir: "{code:GetAbletonUserLibDir}\{#AppName}"; Components: M4L
Source: "..\Max Package\*"; DestDir: "{code:GetMaxDir}\Packages"; Flags: recursesubdirs; Components: M4L

[Icons]
Name: "{group}\Uninstall {#AppName}"; Filename: "{uninstallexe}"

[Components]
Name: "ScopeDLL"; Description: "Scope Module"; Types: full compact
Name: "BCMod"; Description: "BC Modular Modules"; Types: full compact
Name: "Configurations"; Description: "Configuration Library"; Types: full compact
Name: "Layouts"; Description: "Layout Library"; Types: full compact
Name: "Presets"; Description: "Preset Library"; Types: full compact
Name: "Tutorials"; Description: "Tutorials"; Types: full
Name: "VSTPlugin"; Description: "VST Plugins"; Types: full compact
Name: "VSTPlugin\32VST2"; Description: "32-bit VST2 Plugin"; Types: full compact
Name: "VSTPlugin\64VST2"; Description: "64-bit VST2 Plugin"; Types: full compact; Check: Is64BitInstallMode
Name: "VSTPlugin\32VST3"; Description: "32-bit VST3 Plugin (Experimental)"
Name: "VSTPlugin\64VST3"; Description: "64-bit VST3 Plugin (Experimental)"; Check: Is64BitInstallMode
Name: "M4L"; Description: "Max For Live Patches"; Types: full

[Code]
// global vars
var
  ScopeDirPage: TInputDirWizardPage;
  AdditionalDirPage: TInputDirWizardPage;
  
function GetScopeDir(Param: String): String;
var
  ScopeDir: String;
begin
  ScopeDir := ScopeDirPage.Values[0];
  RegWriteStringValue(HKEY_CURRENT_USER, '{#RegSubKey}', 'ScopeDir', ScopeDir);
  { Return the selected ScopeDir }
  Result := ScopeDir;
end;

function GetModularDir(Param: String): String;
var
  ModularDir: String;
begin
  ModularDir := ScopeDirPage.Values[1];
  RegWriteStringValue(HKEY_CURRENT_USER, '{#RegSubKey}', 'ModularDir', ModularDir);
  { Return the selected ModularDir }
  Result := ModularDir;
end;

function GetVST2Dir(Param: String): String;
var
  VST2Dir: String;
begin
  VST2Dir := AdditionalDirPage.Values[0];
  RegWriteStringValue(HKEY_CURRENT_USER, '{#RegSubKey}', 'VST2Dir', VST2Dir);
  { Return the selected VST2Dir }
  Result := VST2Dir;
end;

function GetAbletonUserLibDir(Param: String): String;
var
  AbletonUserLibDir: String;
begin
  AbletonUserLibDir := AdditionalDirPage.Values[1];
  RegWriteStringValue(HKEY_CURRENT_USER, '{#RegSubKey}', 'AbletonUserLibDir', AbletonUserLibDir);
  { Return the selected AbletonUserLibDir }
  Result := AbletonUserLibDir;
end;
     
function GetMaxDir(Param: String): String;
var
  MaxDir: String;
begin
  MaxDir := AdditionalDirPage.Values[2];
  RegWriteStringValue(HKEY_CURRENT_USER, '{#RegSubKey}', 'MaxDir', MaxDir);
  { Return the selected MaxDir }
  Result := MaxDir;
end;
     
// custom wizard page setup, for data dir.
procedure InitializeWizard;
var
  VST2Dir: String;
  ScopeDir: String;
  ModularDir: String;
  AbletonUserLibDir: String;
  MaxDir: String;
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
    ModularDir := ExpandConstant('{pf}\Scope PCI\Modular Modules');
  end; 

  if not(RegQueryStringValue(HKEY_CURRENT_USER, '{#RegSubKey}', 'AbletonUserLibDir', AbletonUserLibDir)) then
  begin
    AbletonUserLibDir := ExpandConstant('{userdocs}\Ableton\User Library');
  end; 

  if not(RegQueryStringValue(HKEY_CURRENT_USER, '{#RegSubKey}', 'MaxDir', MaxDir)) then
  begin
    MaxDir := ExpandConstant('{pf32}\Cycling ''74\Max 6.1');
  end; 

  ScopeDirPage := CreateInputDirPage(
    wpSelectDir,
    'Select Scope Directories',
    'Where should Scope files be installed?',
    '',
    False,
    '{#AppName}'
  );
  ScopeDirPage.Add('Scope Installation Directory');
  ScopeDirPage.Add('Scope Modular Modules Directory');
  
  ScopeDirPage.Values[0] := ScopeDir;
  ScopeDirPage.Values[1] := ModularDir;
  
  AdditionalDirPage := CreateInputDirPage(
    wpSelectDir,
    'Select Plugin Directories',
    'Where should plugin files be installed?',
    '',
    False,
    '{#AppName}'
  );
  AdditionalDirPage.Add('VST2 Plugin Directory');
  AdditionalDirPage.Add('Ableton User Library (for M4L patches)');
  AdditionalDirPage.Add('Max Installation Directory (for M4L support)');
 
  AdditionalDirPage.Values[0] := VST2Dir;
  AdditionalDirPage.Values[1] := AbletonUserLibDir;
  AdditionalDirPage.Values[2] := MaxDir;
end;
