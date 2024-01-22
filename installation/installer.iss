#define MyAppName "Riva Dosbox Logger"
#define MyAppVersion "0.1.11"

#define MyAppRegPath "Software\tinion\riva-dosbox-logger"

[Setup]
AppName={#MyAppName}
AppVersion={#MyAppVersion}
VersionInfoVersion=0.1.11.0
DefaultDirName={pf}\{#MyAppName}
OutputDir=out
SetupIconFile="files\install\common\logeyes.ico"
DefaultGroupName = "{cm:GameTitle}"
UninstallDisplayIcon={app}\logeyes.ico
WizardSmallImageFile=img/orig_small_55x55.bmp,img/orig_small_64x68.bmp,img/orig_small_83x80.bmp,img/orig_small_92x97.bmp,img/orig_small_110x106.bmp,img/orig_small_119x123.bmp,img/orig_small_138x140.bmp
WizardImageFile=img/orig_large_164x314.bmp,img/orig_large_192x386.bmp,img/orig_large_246x459.bmp,img/orig_large_273x556.bmp,img/orig_large_328x604.bmp,img/orig_large_355x700.bmp,img/orig_large_410x797.bmp
WizardStyle=modern
AppMutex=dosbox-roa3-logger-running-mutex
SetupMutex=dosbox-roa3-logger-running-mutex-setup
AppPublisher=tinion
AppPublisherURL=https://www.crystals-dsa-foren.de/member.php?action=profile&uid=5305
AppSupportURL=https://www.crystals-dsa-foren.de/showthread.php?tid=6059&pid=171570#pid171570
AppUpdatesURL=https://github.com/cmfrydos/dosbox-staging-roa3/tags
AllowNoIcons=true
SolidCompression=yes
Compression=lzma2/ultra64
LZMAUseSeparateProcess=yes
LZMADictionarySize=1048576
LZMANumFastBytes=273

// 100MB + Riva Installation + 2 * Extract a 750
ExtraDiskSpaceRequired = 1600000000
ReserveBytes = 1600000000 

[Dirs]
Name: "{tmp}\patch\instcd_chg\TEMP"; Flags: deleteafterinstall
Name: "{tmp}\patch\instcd_chg\CTEMP"; Flags: deleteafterinstall
Name: "{app}\DSA3"; Permissions: users-modify

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl, lang\English.isl"
Name: "de"; MessagesFile: "compiler:Languages\German.isl, lang\German.isl"
Name: "fr"; MessagesFile: "compiler:Languages\French.isl, lang\French.isl"


[Icons]
Name: "{group}\{cm:GameTitle}"; Filename: "{app}\dosbox-roa3\dosbox-roa3.exe"; Parameters: "-conf ""..\main_config.conf"" -conf ""..\game_autoexec.conf"" -console"; IconFilename: "{app}\logeyes.ico"; Check: InstallStartmenuGroup
Name: "{group}\Uninstall {cm:GameTitle}"; Filename: "{uninstallexe}"; IconFilename: "{app}\logeyes.ico"; Check: InstallStartmenuGroup

Name: "{commondesktop}\{cm:GameTitle}"; Filename: "{app}\dosbox-roa3\dosbox-roa3.exe"; Tasks: desktopicon; Parameters: "-conf ""..\main_config.conf"" -conf ""..\game_autoexec.conf"" -console"; IconFilename: "{app}\logeyes.ico"
Name: "{app}\Readme"; Filename: "{app}\readme.txt"; Tasks: showreadme; IconFilename: "{app}\logeyes.ico"

Name: "{app}\{cm:GameTitle}"; Filename: "{app}\dosbox-roa3\dosbox-roa3.exe"; Parameters: "-conf ""..\main_config.conf"" -conf ""..\game_autoexec.conf"" -console"; IconFilename: "{app}\logeyes.ico"
Name: "{app}\Uninstall {cm:GameTitle}"; Filename: "{uninstallexe}"; IconFilename: "{app}\logeyes.ico"


[Tasks]
Name: "desktopicon"; Description: "{cm:TaskDescriptionDesktopIcon}"; GroupDescription: "{cm:TaskGroupDescriptionShortcut}"
Name: "showreadme"; Description: "{cm:TaskDescriptionShowReadme}"; GroupDescription: "{cm:TaskGroupDescriptionPostInstall}"; Flags: unchecked
Name: "startgame"; Description: "{cm:TaskDescriptionStartGame}"; GroupDescription: "{cm:TaskGroupDescriptionPostInstall}"; Flags: unchecked
Name: "patchgame"; Description: "{cm:TaskDescriptionPatchGame}"; GroupDescription: "{cm:TaskGroupDescriptionPostInstall}"; Flags: unchecked; Check: ShouldRunPatch

[Run]
Filename: "{app}\readme.txt"; Description: "{cm:DescriptionOpenReadme}"; Flags: postinstall skipifsilent shellexec; Tasks: showreadme
Filename: "icacls"; Parameters: """{app}\DSA3"" /grant Users:W /T"; Description: "{cm:DescriptionSettingFolderPermissions}"; Flags: runhidden shellexec waituntilterminated
Filename: "icacls"; Parameters: """{app}\main_config.conf"" /grant Users:W"; Description: "{cm:DescriptionSettingFolderPermissions}"; Flags: runhidden shellexec waituntilterminated
Filename: "icacls"; Parameters: """{app}\game_autoexec.conf"" /grant Users:W"; Description: "{cm:DescriptionSettingFolderPermissions}"; Flags: runhidden shellexec waituntilterminated
Filename: "icacls"; Parameters: """{app}\patcg_autoexec.conf"" /grant Users:W"; Description: "{cm:DescriptionSettingFolderPermissions}"; Flags: runhidden shellexec waituntilterminated


Filename: "{app}\dosbox-roa3\dosbox-roa3.exe"; Parameters: "-conf ""..\main_config.conf"" -conf ""..\patch_autoexec.conf"" -console"; Description: "{cm:DescriptionPatchGame}"; Flags: postinstall skipifsilent; Tasks: patchgame; Check: ShouldRunPatch
Filename: "{app}\dosbox-roa3\dosbox-roa3.exe"; Parameters: "-conf ""..\main_config.conf"" -conf ""..\game_autoexec.conf"" -console"; Description: "{cm:DescriptionLaunchGame}"; Flags: postinstall skipifsilent nowait; Tasks: startgame

[Registry]
Root: HKLM; Subkey: "{#MyAppRegPath}"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"; Flags: uninsdeletevalue
Root: HKLM; Subkey: "{#MyAppRegPath}"; ValueType: string; ValueName: "Version"; ValueData: "{#MyAppVersion}"; Flags: uninsdeletevalue


[UninstallDelete]
Type: files; Name: "{app}\readme.txt"    
Type: files; Name: "{app}\main_config.conf"         
Type: files; Name: "{app}\game_autoexec.conf"
Type: files; Name: "{app}\patch_autoexec.conf"
Type: filesandordirs; Name: "{app}\DSA3"
Type: filesandordirs; Name: "{app}\dosbox-roa3"
Type: files; Name: "{userappdata}\{#MyAppName}\log.txt"
Type: files; Name: "{userappdata}\{#MyAppName}\log_old.txt"
Type: files; Name: "{userappdata}\{#MyAppName}\rivaBin.cfg"
Type: filesandordirs; Name: "{userappdata}\{#MyAppName}"


[Code]
type
  TGameVersion = (Auto, GoG, Heldenedition, Steam, OtherExistingCD, OtherExistingImage, OtherInstallCD, OtherInstallImage, OtherExtract, None);
  TInstallationInOriginalGame = (OriginalFolder, SeparateInstall);
  TInstallationFolder = (AppData, Programs, SrcFolder, ManualSelect);
  TFileLinking = (LinkOriginal, CopyFiles);


const
  ModThreadURL = 'https://www.crystals-dsa-foren.de/showthread.php?tid=6059';
  ModGithubURL = 'https://github.com/cmfrydos/dosbox-staging-roa3/tags';

var
  GameDir: string;
  GameCD : string;
  GameImage : string;
  InstallRivaDir : string;
  GameVersion : TGameVersion;
  InstallWelcomePage : TOutputMsgWizardPage;
  InstallOutroPage: TWizardPage;
  InstallOutroPageLabel1: TLabel;
  InstallOutroPageLabel2: TLabel;
  InstallOutroPageLabel3: TLabel;
  InstallOutroPageLink1: TLabel;
  InstallOutroPageLink2: TLabel;
  InstallOutroPageDisclaimerLabel: TLabel;

  Page1GameVersion: TInputOptionWizardPage;
  Page2SelectGameFolder : TInputDirWizardPage;
  Page3SelectGameCD: TWizardPage;
  
  Page4SelectGameImage: TInputFileWizardPage;
  Page5OriginalGameFolder: TInputOptionWizardPage;
  Page6BroadDestination: TInputOptionWizardPage;
  Page7GameFileLinkCopy: TInputOptionWizardPage;

  SelectGameCDCombo: TNewComboBox;
  SelectGameCDLabel: TLabel;

  InstallToAppData : Boolean;
  CustomInstallDir : Boolean;
  RivaExeSha1: string;

  CustomForm: TSetupForm;
  RepairButton: TNewButton;
  UninstallButton: TNewButton;
  CancelButton: TNewButton;
  InfoLabel: TNewStaticText;
  DoUpdate : boolean;
  CopyGameDir : boolean;


procedure OpenBrowser(Url: string);
var
  ErrorCode: Integer;
begin
  ShellExec('open', Url, '', '', SW_SHOWNORMAL, ewNoWait, ErrorCode);
end;

procedure LinkClick(Sender: TObject);
begin
  if Sender = InstallOutroPageLink1 then
    OpenBrowser(ModThreadURL)
  else if Sender = InstallOutroPageLink2 then
    OpenBrowser(ModGithubURL);
end;

procedure InitializeOutroPage;
begin
  InstallOutroPage := CreateCustomPage(wpInfoAfter,  ExpandConstant('{cm:InstallOutroPageTitle}'),ExpandConstant('{cm:InstallOutroPageSubtitle}'));

  InstallOutroPageLabel1 := TLabel.Create(WizardForm);
  InstallOutroPageLabel1.Parent := InstallOutroPage.Surface;
  InstallOutroPageLabel1.WordWrap := True;
  InstallOutroPageLabel1.AutoSize := False;
  
  
  InstallOutroPageLabel1.Caption := ExpandConstant('{cm:InstallOutroPageDescriptionPart1}');
  
  InstallOutroPageLabel1.Left := 0;
  InstallOutroPageLabel1.Top := 8;
  InstallOutroPageLabel1.Width := InstallOutroPage.SurfaceWidth;
  InstallOutroPageLabel1.AutoSize := True;


  InstallOutroPageLink1 := TLabel.Create(WizardForm);
  InstallOutroPageLink1.Parent := InstallOutroPage.Surface;
  InstallOutroPageLink1.WordWrap := True;
  InstallOutroPageLink1.AutoSize := False;
  InstallOutroPageLink1.Caption := 'Crystals-DSA-Foren.de > DSA 3 - Schatten über Riva DOS 1996 > Proben und Zufallsereignisse in Riva';
  InstallOutroPageLink1.Font.Style := [fsUnderline, fsBold];
  InstallOutroPageLink1.Font.Color := clBlue;
  InstallOutroPageLink1.Cursor := crHand;
  InstallOutroPageLink1.OnClick := @LinkClick;
  InstallOutroPageLink1.Left := 0;
  InstallOutroPageLink1.Top := InstallOutroPageLabel1.Top + InstallOutroPageLabel1.Height + 4;
  InstallOutroPageLink1.Width := InstallOutroPage.SurfaceWidth;
  InstallOutroPageLink1.AutoSize := True;

  InstallOutroPageLabel2 := TLabel.Create(WizardForm);
  InstallOutroPageLabel2.Parent := InstallOutroPage.Surface;
  InstallOutroPageLabel2.WordWrap := True;
  InstallOutroPageLabel2.AutoSize := False;
  InstallOutroPageLabel2.Caption := ExpandConstant('{cm:InstallOutroPageDescriptionPart2}');
  InstallOutroPageLabel2.Left := 0;
  InstallOutroPageLabel2.Top := InstallOutroPageLink1.Top + InstallOutroPageLink1.Height + 14;
  InstallOutroPageLabel2.Width := InstallOutroPage.SurfaceWidth;
  InstallOutroPageLabel2.AutoSize := True;

  InstallOutroPageLink2 := TLabel.Create(WizardForm);
  InstallOutroPageLink2.Parent := InstallOutroPage.Surface;
  InstallOutroPageLink2.Caption := ModGithubURL;
  InstallOutroPageLink2.Font.Style := [fsUnderline, fsBold];
  InstallOutroPageLink2.Font.Color := clBlue;
  InstallOutroPageLink2.Cursor := crHand;
  InstallOutroPageLink2.OnClick := @LinkClick;
  InstallOutroPageLink2.AutoSize := True;
  InstallOutroPageLink2.Left := 0;
  InstallOutroPageLink2.Top := InstallOutroPageLabel2.Top + InstallOutroPageLabel2.Height + 4;


  InstallOutroPageDisclaimerLabel := TLabel.Create(WizardForm);
  InstallOutroPageDisclaimerLabel.Parent := InstallOutroPage.Surface;
  InstallOutroPageDisclaimerLabel.Caption := #13#10 + ExpandConstant('{cm:InstallOutroPageDescriptionPart3}');
  InstallOutroPageDisclaimerLabel.Font.Style := [fsBold, fsUnderline];
  InstallOutroPageDisclaimerLabel.AutoSize := True;
  InstallOutroPageDisclaimerLabel.Left := 0;
  InstallOutroPageDisclaimerLabel.Top := InstallOutroPageLink2.Top + InstallOutroPageLink2.Height + 4;

  InstallOutroPageLabel3 := TLabel.Create(WizardForm);
  InstallOutroPageLabel3.Parent := InstallOutroPage.Surface;
  InstallOutroPageLabel3.WordWrap := True;
  InstallOutroPageLabel3.AutoSize := False;
  InstallOutroPageLabel3.Caption := ExpandConstant('{cm:InstallOutroPageDescriptionDisclaimer}');
  InstallOutroPageLabel3.Left := 0;
  InstallOutroPageLabel3.Top := InstallOutroPageDisclaimerLabel.Top + InstallOutroPageDisclaimerLabel.Height + 4;
  InstallOutroPageLabel3.Width := InstallOutroPage.SurfaceWidth;
  InstallOutroPageLabel3.AutoSize := True;
end;

function ShouldRunPatch: Boolean;
begin
    Result := (GameVersion in [OtherInstallCD, OtherInstallImage]) and 
    ((RivaExeSha1 = '41b0b3e2c38b1ca9aa170477f366151976da03a0') or (RivaExeSha1 = ''));
end;

function CopyDSA3Folder(Param: String) : string;
begin
    Result := '-1';
    if(GameDir <> '') and CopyGameDir then
      Result := GameDir + '\DSA3';
end;

function InstallStartmenuGroup() : Boolean;
begin
  Result := (not InstallToAppData) and (not GameVersion = OtherExtract);
end;

function CustomMsgBox(repairbuttontext, message: string): Integer;
var
  ButtonWidth1: Integer;
  ButtonWidth2: Integer;
  ButtonWidth3: Integer;
  ButtonMargin : Integer;
begin

  ButtonWidth1 := 150;
  ButtonWidth2 := 80;
  ButtonWidth3 := 80;
  ButtonMargin := 20;
  
  CustomForm := CreateCustomForm;
  CustomForm.SetBounds(0, 0, ButtonWidth1 + ButtonWidth2 + ButtonWidth3 + 4 * ButtonMargin, 150);
  CustomForm.Position := poScreenCenter;
  CustomForm.Caption := ExpandConstant('{cm:ChoiceCaption}');

  InfoLabel := TNewStaticText.Create(CustomForm);
  InfoLabel.Parent := CustomForm;
  InfoLabel.Caption := message;
  InfoLabel.SetBounds(ButtonMargin, 20, ButtonWidth1 + ButtonWidth2 + ButtonWidth3 + 2 * ButtonMargin, 40); // Adjusted the width
  InfoLabel.AutoSize := False;
  InfoLabel.WordWrap := True;

   // Increased the width of the RepairButton by 60%

  RepairButton := TNewButton.Create(CustomForm);
  RepairButton.Parent := CustomForm;
  RepairButton.Caption := repairbuttontext;
  RepairButton.ModalResult := mrYes;
  RepairButton.SetBounds(ButtonMargin, 70, ButtonWidth1, 25);

  UninstallButton := TNewButton.Create(CustomForm);
  UninstallButton.Parent := CustomForm;
  UninstallButton.Caption := ExpandConstant('{cm:UninstallButtonText}');
  UninstallButton.ModalResult := mrNo;
  UninstallButton.SetBounds(2 * ButtonMargin + ButtonWidth1, 70, ButtonWidth2, 25); // Adjusted the position based on the RepairButton width

  CancelButton := TNewButton.Create(CustomForm);
  CancelButton.Parent := CustomForm;
  CancelButton.Caption := ExpandConstant('{cm:CancelButtonText}');
  CancelButton.ModalResult := mrCancel;
  CancelButton.SetBounds(3 * ButtonMargin + ButtonWidth1 + ButtonWidth2, 70, ButtonWidth3, 25); // Adjusted the position

  Result := CustomForm.ShowModal;
end;



function CheckRivaInGameDir(dir : string): Boolean;
begin
  Result := False;
  if FileExists(dir + '\RIVA.EXE') then
  begin
    Result := True;
  end;
end;

function CheckRivaCD(dir: string): Boolean;
begin
  Result := (FileExists(dir + 'INSTALL.EXE') and FileExists(dir + 'MAIN\RIVA.EXE'));
end;


function RPos(const ASub, AIn: string; AStart: Integer): Integer;
var
  i: Integer;
begin
  Result := 0;
  for i := Length(AIn) downto 1 do
    if Copy(AIn, i, Length(ASub)) = ASub then
    begin
      Result := i;
      break;
    end;
end;

function RemoveEnding(const FilePath: string): string;
begin
  Result := Copy(FilePath, 1, RPos('.', FilePath, -1) - 1);
end;

function CheckRivaImage(image: string): Boolean;
var 
  FileWithoutFileEnding: string;
begin
  FileWithoutFileEnding := RemoveEnding(image);
  Result := (FileExists(FileWithoutFileEnding + '.CUE') and FileExists(FileWithoutFileEnding + '.BIN')) or FileExists(FileWithoutFileEnding + '.ISO');
end;

function CheckRivaVersionInGameDir(dir : string): TGameVersion;
var      
  LowerDir: string;
  ParentDir: string;
begin
  LowerDir := LowerCase(dir);
  // MsgBox(LowerDir, mbInformation, MB_OK);
  Result := None;
  if DirExists(dir + '\cloud_saves') then
  begin
    Result := GoG;
  end
  else if Pos('\steamapps\', dir) > 0 then
  begin
    Result := Steam;
  end else 
  if ExtractFileName(RemoveBackslash(LowerDir)) = 'dsa3' then
  begin
    ParentDir := ExtractFilePath(RemoveBackslash(LowerDir));

    // Check if there's a 'dsaschatt' folder in the parent directory
    if DirExists(ParentDir + 'dsaschatt') then
    begin
      Result := Heldenedition;
    end;
  end;
end;


#include "steam.iss"




function CheckForGogGame(out dir: string): Boolean;
var
  Names: TArrayOfString;
  I: Integer;
  GameName : String;
begin
  Result := False;

  if RegGetSubkeyNames(HKLM, 'SOFTWARE\WOW6432Node\GOG.com\Games\', Names) then
  begin
    for I := 0 to GetArrayLength(Names) - 1 do
    begin
      // MsgBox(Names[I], mbInformation, MB_OK);
      if RegQueryStringValue(HKLM, 'SOFTWARE\WOW6432Node\GOG.com\Games\' + Names[I], 'gameName', GameName) then
      begin
        if GameName= 'Realms of Arkania 3 - Shadows over Riva' then
        begin
          if RegQueryStringValue(HKLM, 'SOFTWARE\WOW6432Node\GOG.com\Games\' + Names[I], 'DOSBOXFOLDER', GameDir) then
          begin
            dir := ExtractFileDir(GameDir);  // One directory up from the DOSBOXFOLDER
            MsgBox(ExpandConstant('{cm:GogFoundMessage,' + dir + '}'), mbInformation, MB_OK);


            Result := True;
            Exit
          end
        end;
      end
    end;
  end;
end;


function CheckForHeldeneditionGame(out dir: string): Boolean;
var
  i: Integer;
  DriveLetter: Char;
  TestDir: string;
begin
  Result := False;
  for i := ord('A') to ord('Z') do
  begin
    DriveLetter := chr(i);
    TestDir := DriveLetter + ':\dosgames\DSA3';
    if DirExists(TestDir) then
    begin
      dir := TestDir;
      MsgBox(ExpandConstant('{cm:HeldeneditionFoundMessage,' + dir + '}'), mbInformation, MB_OK);
      Result := True;
      Break;
    end;
  end;
end;




function CheckForAnyGame(out dir: string): Boolean;
begin
  Result := True;
  dir := ''
  if CheckForGogGame(dir) then
  begin
  end
  else if CheckForHeldeneditionGame(dir) then
  begin
  end
  else if CheckForSteamGame(dir) then
  begin
  end
  else
    Result := False;

end;

function CheckForGame(var dir: string): Boolean;
begin
  Result := True;
  
  repeat
    begin
      if not BrowseForFolder(    ExpandConstant('{cm:SelectGameDirectoryMessage}'), dir, True) then
      begin
        Result := False;  // User cancelled
        break;
      end;

      if not CheckRivaInGameDir(dir) then
        MsgBox(ExpandConstant('{cm:InvalidGameDirectoryMessage}'), mbInformation, MB_OK);
    end;
  until (CheckRivaInGameDir(dir))
      
  GameVersion := CheckRivaVersionInGameDir(dir);
end;

function CheckForGameCD(var dir: string): Boolean;
begin
  Result := True;
  repeat
    begin
      if not BrowseForFolder(ExpandConstant('{cm:SelectCDRomMessage}'), dir, True) then
      begin
        Result := False;  // User cancelled
        break;
      end;
      if not CheckRivaCD(dir) then
        MsgBox(ExpandConstant('{cm:InvalidCDRomMessage}'), mbInformation, MB_OK);
    end;
  until (CheckRivaCD(dir));
end;

function IsAppInstalled(out InstallPath: string): Boolean;
begin
  Result := False;
  if RegQueryStringValue(HKLM, '{#MyAppRegPath}', 'InstallPath', InstallPath) then
    if DirExists(InstallPath) then
      Result := True;
end;

function InitializeSetup(): Boolean;
var 
InstallPath: string;
Choice: Integer;
ResultCode: Integer;
RegVersion: string;
x : string;
begin
  Result := True; // Continue with the installation
  if IsAppInstalled(InstallPath) then
  begin
    Result := False; // Exit the installer
    RegQueryStringValue(HKEY_LOCAL_MACHINE, '{#MyAppRegPath}', 'Version', RegVersion);

    // Present the user with options
    if RegVersion = '{#MyAppVersion}' then
      Choice := CustomMsgBox(ExpandConstant('{cm:RepairButtonText}'), ExpandConstant('{cm:InfoMessageInstalledRepair}'))
    else
      Choice := CustomMsgBox(ExpandConstant('{cm:UpdateRepairButtonText}'), ExpandConstant('{cm:InfoMessageInstalledRepairUpdate}'));

    if Choice = mrYes then  // Repair logic here
    begin
      Result := True; // Continue with the installation
      DoUpdate := True;
    end
    else if Choice = mrNo then
    begin
      // Uninstall logic here
      Exec(ExpandConstant(InstallPath + '\unins000.exe'), '', '', SW_SHOW, ewWaitUntilTerminated, ResultCode)
    end;
  end;
end;



#ifdef UNICODE
  #define AW "W"
#else
  #define AW "A"
#endif
type
  TDriveType = (
    dtUnknown,
    dtNoRootDir,
    dtRemovable,
    dtFixed,
    dtRemote,
    dtCDROM,
    dtRAMDisk
  );
  TDriveTypes = set of TDriveType;

function GetDriveType(lpRootPathName: string): UINT;
  external 'GetDriveType{#AW}@kernel32.dll stdcall';
function GetLogicalDriveStrings(nBufferLength: DWORD; lpBuffer: string): DWORD;
  external 'GetLogicalDriveStrings{#AW}@kernel32.dll stdcall';
function GetVolumeInformation(lpRootPathName: string; lpVolumeNameBuffer: string; nVolumeNameSize: DWORD; var lpVolumeSerialNumber: DWORD; var lpMaximumComponentLength: DWORD; var lpFileSystemFlags: DWORD; lpFileSystemNameBuffer: string; nFileSystemNameSize: DWORD): BOOL;
  external 'GetVolumeInformation{#AW}@kernel32.dll stdcall';


function GetVolumeLabel(Drive: string): string;
var
  SerialNumber, MaxComponentLength, FileSystemFlags: DWORD;
  VolumeName: string;
begin
  SetLength(VolumeName, 256);
  if GetVolumeInformation(Drive + '\', VolumeName, Length(VolumeName), SerialNumber, MaxComponentLength, FileSystemFlags, '', 0) then
  begin
    SetLength(VolumeName, Pos(#0, VolumeName) - 1);
    Result := VolumeName;
  end
  else
    Result := '';
end;


#ifndef UNICODE
function IntToDriveType(Value: UINT): TDriveType;
begin
  Result := dtUnknown;
  case Value of
    1: Result := dtNoRootDir;
    2: Result := dtRemovable;
    3: Result := dtFixed;
    4: Result := dtRemote;
    5: Result := dtCDROM;
    6: Result := dtRAMDisk;
  end;
end;
#endif

function GetLogicalDrives(Filter: TDriveTypes; Drives: TStrings): Integer;
var
  S: string;
  I: Integer;
  DriveRoot: string;
begin
  Result := 0;

  I := GetLogicalDriveStrings(0, #0);
  if I > 0 then
  begin
    SetLength(S, I);
    if GetLogicalDriveStrings(Length(S), S) > 0 then
    begin
      S := TrimRight(S) + #0;
      I := Pos(#0, S);
      while I > 0 do
      begin
        DriveRoot := Copy(S, 1, I - 1);
        #ifdef UNICODE
        if (Filter = []) or
          (TDriveType(GetDriveType(DriveRoot)) in Filter) then
        #else
        if (Filter = []) or
          (IntToDriveType(GetDriveType(DriveRoot)) in Filter) then
        #endif
        begin
          Drives.Add(DriveRoot);
        end;
        Delete(S, 1, I);
        I := Pos(#0, S);
      end;
      Result := Drives.Count;
    end;
  end;
end;


procedure GameCDComboChange(Sender: TObject);
begin
  //GameCD := SelectGameCDCombo.Text;
  GameCD := Copy(SelectGameCDCombo.Text, 1, 3);
end;

procedure DetectAndPopulateCDDrives;
var
  StringList: TStringList;
  I: Integer;
  DriveLabel: string;
begin
  StringList := TStringList.Create;
  try
    if GetLogicalDrives([dtCDROM], StringList) > 0 then
    begin
      SelectGameCDCombo.Items.Clear;

      for I := 0 to StringList.Count - 1 do
      begin
        DriveLabel := GetVolumeLabel(StringList[I]);
        if DriveLabel <> '' then
          DriveLabel := ' (' + DriveLabel + ')';
        SelectGameCDCombo.Items.Add(StringList[I] + DriveLabel);
      end;

      if SelectGameCDCombo.Items.Count > 0 then
        SelectGameCDCombo.ItemIndex := 0
      else
        SelectGameCDCombo.Text := ''; // No CD drive found
    end;
  finally
    StringList.Free;
  end;
  GameCDComboChange(nil);
end;

procedure InitializeWizard();
var
  StringList: TStringList;
begin
  InstallWelcomePage := CreateOutputMsgPage(wpInfoBefore,
  ExpandConstant('{cm:InstallWelcomePageTitle}'),
  ExpandConstant('{cm:InstallWelcomePageSubtitle}'),
  ExpandConstant('{cm:InstallWelcomePageDescription}'));

  //InstallOutroPage := CreateOutputMsgPage(InstallWelcomePage//wpInfoAfter,
  //ExpandConstant('{cm:InstallOutroPageTitle}'),
  //ExpandConstant('{cm:InstallOutroPageSubtitle}'),
  //ExpandConstant('{cm:InstallOutroPageDescription}'));

  InitializeOutroPage();

  Page1GameVersion := CreateInputOptionPage(wpUserInfo,
  ExpandConstant('{cm:GameInstallationVersionCaption}'), 
  ExpandConstant('{cm:GameInstallationVersionPrompt}'), 
  ExpandConstant('{cm:ChooseInstallationMethodPrompt}'), True, False);


  Page1GameVersion.Add(ExpandConstant('{cm:AutoDetectOption}'));
  Page1GameVersion.Add(ExpandConstant('{cm:GoGVersionOption}'));
  Page1GameVersion.Add(ExpandConstant('{cm:HeldeneditionOption}'));
  Page1GameVersion.Add(ExpandConstant('{cm:SteamOption}'));
  Page1GameVersion.Add(ExpandConstant('{cm:OtherExistingCDOption}'));
  Page1GameVersion.Add(ExpandConstant('{cm:OtherExistingImageOption}'));
  Page1GameVersion.Add(ExpandConstant('{cm:OtherInstallCDOption}'));
  Page1GameVersion.Add(ExpandConstant('{cm:OtherInstallImageOption}'));
  Page1GameVersion.Add(ExpandConstant('{cm:OtherJustExtractOption}'));

  Page1GameVersion.Values[0] := True;


  Page2SelectGameFolder := CreateInputDirPage(Page1GameVersion.ID,
  ExpandConstant('{cm:LocateGame}'), ExpandConstant('{cm:WhereIsYourGameInstalled}'),
  ExpandConstant('{cm:SpecifyGameFolder}'),
  False, '');

  // Add item (with an empty caption)
  Page2SelectGameFolder.Add('');


  Page3SelectGameCD := CreateCustomPage(Page2SelectGameFolder.ID, ExpandConstant('{cm:InsertGameCD}'), ExpandConstant('{cm:InsertOriginalGameCD}'));

  SelectGameCDLabel := TLabel.Create(WizardForm);
  SelectGameCDLabel.Parent := Page3SelectGameCD.Surface;
  SelectGameCDLabel.WordWrap := true;
  SelectGameCDLabel.AutoSize := false;
  SelectGameCDLabel.Caption := ExpandConstant('{cm:ModInstallationRequires}');
  SelectGameCDLabel.Left := 0;
  SelectGameCDLabel.Top := 0;
  SelectGameCDLabel.Width := Page3SelectGameCD.SurfaceWidth;
  SelectGameCDLabel.AutoSize := True;

  SelectGameCDCombo := TNewComboBox.Create(WizardForm);
  SelectGameCDCombo.Parent := Page3SelectGameCD.Surface;
  SelectGameCDCombo.SetBounds(0, SelectGameCDLabel.Top + SelectGameCDLabel.Height + 8, Page3SelectGameCD.SurfaceWidth, SelectGameCDCombo.Height);
  SelectGameCDCombo.Style := csDropDownList;
  SelectGameCDCombo.OnChange := @GameCDComboChange;

  DetectAndPopulateCDDrives;

  Page4SelectGameImage := CreateInputFilePage(Page3SelectGameCD.ID,
        ExpandConstant('{cm:SelectGameImageLocation}'), ExpandConstant('{cm:WhereIsGameImageLocated}'),
        ExpandConstant('{cm:SelectISOOrCUE}'));
  // Add item for Image location
  Page4SelectGameImage.Add(ExpandConstant('{cm:GameImageLocation}'), 'Game Image Files (*.iso, *.cue)|*.iso;*.cue|All files|*.*', '.iso');

  Page5OriginalGameFolder := CreateInputOptionPage(Page4SelectGameImage.ID,
  ExpandConstant('{cm:SelectInstallFolderI}'), ExpandConstant('{cm:AskInstallMod}'),
  ExpandConstant('{cm:ChooseInstallMethod}'), True, False);
  Page5OriginalGameFolder.Add(ExpandConstant('{cm:InstallationMethodOriginal}'));
  Page5OriginalGameFolder.Add(ExpandConstant('{cm:InstallationMethodSeparate}'));
  Page5OriginalGameFolder.Values[1] := True;




  Page6BroadDestination := CreateInputOptionPage(Page5OriginalGameFolder.ID,
  ExpandConstant('{cm:SelectInstallFolderII}'), ExpandConstant('{cm:AskInstallModLocation}'),
  ExpandConstant('{cm:ChooseInstallDirectory}'), True, False);
  Page6BroadDestination.Add(ExpandConstant('{cm:InstallationFolderAppData}'));
  Page6BroadDestination.Add(ExpandConstant('{cm:InstallationFolderPrograms}'));
  Page6BroadDestination.Add(ExpandConstant('{cm:InstallationFolderSrc, ' + ExpandConstant('{src}') + '}'));
  Page6BroadDestination.Add(ExpandConstant('{cm:InstallationFolderManualSelect}'));
  Page6BroadDestination.Values[0] := True;


  Page7GameFileLinkCopy := CreateInputOptionPage(Page6BroadDestination.ID,
  ExpandConstant('{cm:SelectGameFileLinking}'), ExpandConstant('{cm:AskLinkOrCopy}'),
  ExpandConstant('{cm:ChooseLinkOrCopy}'), True, False);
  Page7GameFileLinkCopy.Add(ExpandConstant('{cm:GameFileLinkingLink}'));
  Page7GameFileLinkCopy.Add(ExpandConstant('{cm:GameFileLinkingCopy}'));
  Page7GameFileLinkCopy.Values[0] := True;
end;


function ShouldSkipPage(PageID: Integer): Boolean;
var 
  InstallGameVersion : TGameVersion;
  InstallGameInOriginal : TInstallationInOriginalGame;
begin
  Result := False;

  InstallGameVersion := TGameVersion(Page1GameVersion.SelectedValueIndex);
  InstallGameInOriginal := TInstallationInOriginalGame(Page5OriginalGameFolder.SelectedValueIndex);
  //(Auto, GoG, Heldenedition, Steam, OtherExistingCD, OtherExistingImage, OtherInstallCD, OtherInstallImage, OtherExtract, None);
  
  if (PageID = wpSelectProgramGroup) and (not InstallStartmenuGroup) then
    Result := True;

  if (PageID = wpSelectDir) and (not CustomInstallDir) then
    Result := True;

  if DoUpdate then
  begin
    Result := PageID in [Page1GameVersion.ID, Page4SelectGameImage.ID, Page5OriginalGameFolder.ID, Page6BroadDestination.ID, Page7GameFileLinkCopy.ID];
    exit;
  end;


  // Game Folder Select
  if PageID = Page2SelectGameFolder.ID then
    Result := InstallGameVersion in [OtherInstallCD, OtherInstallImage, OtherExtract]; 

  // Game CD Select
  if PageID = Page3SelectGameCD.ID then
    Result := not (InstallGameVersion in [OtherInstallCD, OtherExistingCD]); 

  // Image Select
  if PageID = Page4SelectGameImage.ID then
    Result := not (InstallGameVersion in [OtherInstallImage, OtherExistingImage]);  

  // Install in Original Game Folder
  if PageID = Page5OriginalGameFolder.ID then
    Result := InstallGameVersion in [OtherInstallCD, OtherInstallImage, OtherExtract];

  // Install where
  if PageID = Page6BroadDestination.ID then
    Result := InstallGameInOriginal in [OriginalFolder];

  // Link Game Files
  if PageID = Page7GameFileLinkCopy.ID then
  begin
    Result := InstallGameInOriginal in [OriginalFolder];
    Result := Result or (InstallGameVersion in [OtherInstallCD, OtherInstallImage, OtherExtract])
  end;
end;


type
  TCheckFunction = function(out dir: string): Boolean;


function NextButtonClick(CurPageID: Integer): Boolean;
var
  GameDirSelected: Boolean;
  AutoDetection: Boolean;
  InstallGameVersion : TGameVersion;
  CheckFunction : TCheckFunction;
  BroadDest : TInstallationFolder;
begin
  Result := True;
  InstallGameVersion := TGameVersion(Page1GameVersion.SelectedValueIndex);

  if CurPageID = Page1GameVersion.ID then
  begin
    AutoDetection := InstallGameVersion in [Auto, GoG, Heldenedition, Steam];
    if InstallGameVersion = Auto then CheckFunction := @CheckForAnyGame
    else if InstallGameVersion = GoG then CheckFunction := @CheckForGogGame
    else if InstallGameVersion = Heldenedition then CheckFunction := @CheckForHeldeneditionGame
    else if InstallGameVersion = Steam then CheckFunction := @CheckForSteamGame;
    GameDirSelected := False;
    GameVersion := InstallGameVersion;
    if AutoDetection then 
    begin 
      GameDirSelected := CheckFunction(GameDir);
      if GameDir = '' then
      begin
        MsgBox(ExpandConstant('{cm:RivaDetectionFailureMessage}'), mbInformation, MB_OK);
      end else begin
        Page2SelectGameFolder.Values[0] := GameDir;
        GameVersion := CheckRivaVersionInGameDir(GameDir);
      end;
    end;
  end;

  if CurPageID = Page2SelectGameFolder.ID then
  begin
    GameDir := Page2SelectGameFolder.Values[0];
    Result :=CheckRivaInGameDir(GameDir);
    if not Result then
      MsgBox(ExpandConstant('{cm:InvalidGameDirectoryMessage}'), mbError, MB_OK)
    else
      GameVersion := CheckRivaVersionInGameDir(GameDir)
  end;

  if CurPageID = Page3SelectGameCD.ID then
  begin
    Result := CheckRivaCD(GameCD);
    if not Result then MsgBox(ExpandConstant('{cm:InvalidCDRomMessage}'), mbError, MB_OK);
  end;

  if CurPageID = Page4SelectGameImage.ID then
  begin
    Result := CheckRivaImage(Page4SelectGameImage.Values[0]);
    if not Result then
      MsgBox(ExpandConstant('{cm:InvalidImageFileMessage}'), mbError, MB_OK)
    else 
      GameImage := Page4SelectGameImage.Values[0];
      GameVersion := TGameVersion(Page1GameVersion.SelectedValueIndex);
  end;

  if CurPageID = Page5OriginalGameFolder.ID then
  begin
    if(TInstallationInOriginalGame(Page5OriginalGameFolder.SelectedValueIndex) = OriginalFolder) then
    begin
      WizardForm.DirEdit.Text := GameDir + '\{#MyAppName}';
    end; 
  end;
   


  if CurPageID = Page6BroadDestination.ID then
  begin    
    BroadDest := TInstallationFolder(Page6BroadDestination.SelectedValueIndex);
    InstallToAppData := BroadDest = AppData;
    CustomInstallDir := BroadDest = ManualSelect;

    if BroadDest = SrcFolder then WizardForm.DirEdit.Text := ExpandConstant('{src}') + '\{#MyAppName}'
    else if BroadDest = AppData then WizardForm.DirEdit.Text := ExpandConstant('{userappdata}\{#MyAppName}')
    else if BroadDest = ManualSelect then WizardForm.DirEdit.Text := '';
  end;


  if CurPageID = Page7GameFileLinkCopy.ID then
  begin
    // (LinkOriginal, CopyFiles);
    CopyGameDir := TFileLinking(Page7GameFileLinkCopy.SelectedValueIndex) = CopyFiles;
  end;
end;


const
  NewLine = #13#10;
  Spaces = '      ';

procedure AppendToLog(Section, Value: string; var CustomLog : string );
begin
  CustomLog := CustomLog + NewLine + Section + NewLine + Spaces + Value + NewLine;
end;


procedure CurPageChanged(CurPageID: Integer);
var
  CustomLog: string;

begin
  if CurPageID = Page3SelectGameCD.ID then
  begin
    DetectAndPopulateCDDrives;
  end;
  // Check if the current page is the "Ready to Install" page
  if CurPageID = wpReady then
  begin
    CustomLog := ''; // Initialize the custom log

    // Log for Page1GameVersion (Game Installation Version)
    if GameVersion = None then
      AppendToLog(ExpandConstant('{cm:GameVersion}'), ExpandConstant('{cm:AutoDetectOption}'), CustomLog)
    else if GameVersion = Gog then
      AppendToLog(ExpandConstant('{cm:GameVersion}'), ExpandConstant('{cm:GoGVersionOption}'), CustomLog)
    else if GameVersion =Heldenedition then
      AppendToLog(ExpandConstant('{cm:GameVersion}'), ExpandConstant('{cm:HeldeneditionOption}'), CustomLog)
    else if GameVersion = Steam then
      AppendToLog(ExpandConstant('{cm:GameVersion}'), ExpandConstant('{cm:SteamOption}'), CustomLog)
    else if GameVersion = OtherExistingCD then
      AppendToLog(ExpandConstant('{cm:GameVersion}'), ExpandConstant('{cm:OtherExistingCDOption}'), CustomLog)
    else if GameVersion = OtherExistingImage then
      AppendToLog(ExpandConstant('{cm:GameVersion}'), ExpandConstant('{cm:OtherExistingImageOption}'), CustomLog)
    else if GameVersion = OtherInstallCD then
      AppendToLog(ExpandConstant('{cm:GameVersion}'), ExpandConstant('{cm:OtherInstallCDOption}'), CustomLog)
    else if GameVersion = OtherInstallImage then
      AppendToLog(ExpandConstant('{cm:GameVersion}'), ExpandConstant('{cm:OtherInstallImageOption}'), CustomLog)
    else if GameVersion = OtherExtract then
      AppendToLog(ExpandConstant('{cm:GameVersion}'), ExpandConstant('{cm:OtherJustExtractOption}'), CustomLog)
    else if GameVersion = Auto then
      AppendToLog(ExpandConstant('{cm:GameVersion}'), ExpandConstant('{cm:AutoDetectOption}'), CustomLog);

    // Log for Page5OriginalGameFolder (Installation Method)
    if(Assigned(Page5OriginalGameFolder)) then
    begin
      if Page5OriginalGameFolder.Values[0] then
        AppendToLog(ExpandConstant('{cm:InstallationMethod}'), ExpandConstant('{cm:OriginalGameFolder}'), CustomLog)
      else if Page5OriginalGameFolder.Values[1] then
        AppendToLog(ExpandConstant('{cm:InstallationMethod}'), ExpandConstant('{cm:SeparateRecommended}'), CustomLog);
    end;

    // Log for Page6BroadDestination (Installation Folder)
    if(Assigned(Page6BroadDestination)) then
    begin
      if Page6BroadDestination.Values[0] then
        AppendToLog(ExpandConstant('{cm:InstallationFolder}'), ExpandConstant('{cm:AppDataRecommended}'), CustomLog)
      else if Page6BroadDestination.Values[1] then
        AppendToLog(ExpandConstant('{cm:InstallationFolder}'), ExpandConstant('{cm:Programs}'), CustomLog)
      else if Page6BroadDestination.Values[2] then
        AppendToLog(ExpandConstant('{cm:InstallationFolder}'), ExpandConstant('{cm:SrcFolder, ' + ExpandConstant('{src}') + '}'  ), CustomLog)
      else if Page6BroadDestination.Values[3] then
        AppendToLog(ExpandConstant('{cm:InstallationFolder}'), ExpandConstant('{cm:ManualSelection}'), CustomLog);
    end;

    if(Assigned(Page7GameFileLinkCopy)) then
    begin
      // Log for Page7GameFileLinkCopy (Game File Linking)
      if Page7GameFileLinkCopy.Values[0] then
        AppendToLog(ExpandConstant('{cm:GameFileLinking}'), ExpandConstant('{cm:LinkToOriginal}'), CustomLog)
      else if Page7GameFileLinkCopy.Values[1] then
        AppendToLog(ExpandConstant('{cm:GameFileLinking}'), ExpandConstant('{cm:CopyToInstall}'), CustomLog);
    end;

    // Append your custom log to the default log
    WizardForm.ReadyMemo.Lines.Add(CustomLog);
  end;
end;


procedure ListFiles(const Directory: string; Files: TStrings);
var
  FindRec: TFindRec;
begin
  Files.Clear;
  if FindFirst(ExpandConstant(Directory + '*'), FindRec) then
  try
    repeat
      if FindRec.Attributes and FILE_ATTRIBUTE_DIRECTORY = 0 then
        Files.Add(FindRec.Name);
    until
      not FindNext(FindRec);
  finally
    FindClose(FindRec);
  end;
end;


// Exec with output stored in result.
// ResultString will only be altered if True is returned.
function ExecWithResult(
  Filename, Params, WorkingDir: String; ShowCmd: Integer;
  Wait: TExecWait; var ResultCode: Integer; var ResultString: String): Boolean;
var
  TempFilename: String;
  Command: String;
  ResultStringAnsi: AnsiString;
begin
  TempFilename := ExpandConstant('{tmp}\~execwithresult.txt');
  // Exec via cmd and redirect output to file.
  // Must use special string-behavior to work.
  Command :=
    Format('"%s" /S /C ""%s" %s > "%s""', [
      ExpandConstant('{cmd}'), Filename, Params, TempFilename]);
  Result :=
    Exec(ExpandConstant('{cmd}'), Command, WorkingDir, ShowCmd, Wait, ResultCode);
  if not Result then
    Exit;
  LoadStringFromFile(TempFilename, ResultStringAnsi); // Cannot fail
  // See https://stackoverflow.com/q/20912510/850848
  ResultString := ResultStringAnsi;
  DeleteFile(TempFilename);
  // Remove new-line at the end
  if (Length(ResultString) >= 2) and
     (ResultString[Length(ResultString) - 1] = #13) and
     (ResultString[Length(ResultString)] = #10) then
    Delete(ResultString, Length(ResultString) - 1, 2);
end;

function CombinePath(const Dir, FileName: string): string;
begin
  if Copy(Dir, Length(Dir), 1) = '\' then
    Result := Dir + FileName
  else
    Result := Dir + '\' + FileName;
end;


procedure ListFilesRec(const Directory: string; Files: TStrings);
var
  FindRec: TFindRec;
begin
  // Search for all items (files and folders) in the current directory
  if FindFirst(ExpandConstant(Directory + '\*.*'), FindRec) then
  try
    repeat
      // If the item is a directory and not a "." or ".." (current directory or parent directory)
      if (FindRec.Attributes and FILE_ATTRIBUTE_DIRECTORY) <> 0 then
      begin
        if (FindRec.Name <> '.') and (FindRec.Name <> '..') then
        begin
          // Recursively call the function for this subdirectory
          ListFilesRec(CombinePath(Directory, FindRec.Name), Files);
        end;
      end
      else
      begin
        // Add the file path to the list
        Files.Add(CombinePath(Directory, FindRec.Name));
      end;
    until not FindNext(FindRec);
  finally
    FindClose(FindRec);
  end;
end;

procedure ListMsgBox(Folder : String; Files: TStrings);
var
  Msg: string;
  I: Integer;
begin
  Msg := Folder;
  for I := 0 to Files.Count - 1 do
  begin
    Msg := Msg + Files[I] + #13#10;  // #13#10 is a newline in Windows
  end;
  MsgBox(Msg, mbInformation, MB_OK);
end;



function BoolToStr(b: Boolean): string;
begin
    if b then
        Result := 'True'
    else
        Result := 'False';
end;

function ExtractFile(const FilePath, OutputPath: string): Boolean;
var
  ResultCode: Integer;
  Output: string;
begin
  Result := ExecWithResult(ExpandConstant('{tmp}\7z\7z.exe'), 
                ExpandConstant('x "' + FilePath + '" -o"' + OutputPath + '\"'), 
                '', SW_HIDE, ewWaitUntilTerminated, ResultCode, Output) and (ResultCode = 0);
end;

function ExtractFileExtension(const FileName: string): string;
var
  I: Integer;
begin
  Result := '';
  for I := Length(FileName) downto 1 do
  begin
    if FileName[I] = '.' then
    begin
      Result := Copy(FileName, I, MaxInt);
      Break;
    end;
    if FileName[I] = '\' then Break;  // stop at directory separator
  end;
end;

function FileEndsWith(const FileName: string; const Ending: string) : boolean;
begin
  Result := ExtractFileExtension(LowerCase(FileName)) = LowerCase(Ending);
end;

function ManyPostfix(Files: TStrings; N: Integer): TStrings;
var
  I: Integer;
begin
  Result := TStringList.Create;

  for I := 0 to Files.Count - 1 do
  begin
    if Length(Files[I]) > N then
      Result.Add(Copy(Files[I], N + 1, MaxInt))
    else
      Result.Add('');
  end;
end;


function fullExtractRiva(path : string; outpath : string; index : Integer): String;
var
  Files: TStringList;
  RelFiles: TStrings;
  I: Integer;
  OutputPath: string;
  NextOutputPath: string;
begin
  Result := '';
  
  WizardForm.StatusLabel.Caption := ExpandConstant('{cm:ExtractingFiles}');
  WizardForm.FilenameLabel.Caption := path;

  // Extract the initial ISO/CUE file
  if not ExtractFile(path, outpath) then Exit;

  Files := TStringList.Create;
  try
    // List all files in the extracted directory
    ListFilesRec(outpath, Files);
    RelFiles := ManyPostfix(Files, Length(outpath));
    //ListMsgBox(outpath, RelFiles);
    
    // Check if INSTALL.EXE and MAIN\RIVA.EXE are present
    if (RelFiles.IndexOf('INSTALL.EXE') <> -1) and (RelFiles.IndexOf('MAIN\RIVA.EXE') <> -1) then
    begin
      Result := outpath;
      Exit;
    end;

    // Loop through files and extract ISO/CUE recursively
    for I := 0 to Files.Count - 1 do
    begin
      if (LowerCase(ExtractFileExtension(Files[I])) = '.iso') or 
         (LowerCase(ExtractFileExtension(Files[I])) = '.cue') then
      begin
        NextOutputPath := ExpandConstant('{tmp}\out' + IntToStr(index + 1) + '\');
        Result := fullExtractRiva(Files[I], NextOutputPath, index + 1);
        
        // If INSTALL.EXE is found in recursion, exit and return the path
        if Result <> '' then Exit;
      end;
    end;
  finally
    Files.Free;
  end;
end;



function MatchesPattern(s: string; patterns: Array of String): Boolean;
var
  i: Integer;
begin
  Result := False;
  for i := 0 to High(patterns) do
  begin
    if patterns[i] = s then
    begin
      Result := True;
      Break;
    end;
  end;
end;

function MatchesRawPattern(s: string): Boolean;
var
  baseName: string;
  ext: string;
  numPart: string;
  i: Integer;
begin
  Result := False;
  baseName := ExtractFileName(s);
  ext := ExtractFileExt(baseName);
  Delete(baseName, Length(baseName) - Length(ext) + 1, Length(ext));
  if (ext = '.ALF') or (ext = '.MOD') then
  begin
    numPart := '';
    for i := 1 to Length(baseName) do
    begin
      if (baseName[i] >= '0') and (baseName[i] <= '9') then
        numPart := numPart + baseName[i];
    end;
    if (numPart <> '') and (baseName = 'RAW' + numPart) then
      Result := True;
  end;
end;


procedure CopyAll(src, dst: string);
var
  FindRec: TFindRec;
  SourcePath, DestPath: string;
begin
  // Copy data
  if src<> '' then
  begin
    if FindFirst(src + '\*', FindRec) then
    begin
      try
        repeat
          SourcePath := src + '\' + FindRec.Name;
          DestPath := dst + '\' + FindRec.Name;
          if FindRec.Attributes and FILE_ATTRIBUTE_DIRECTORY = 0 then
          begin
            FileCopy(SourcePath, DestPath, False);
            WizardForm.ProgressGauge.Position := WizardForm.ProgressGauge.Position + 1;
          end
          else
          begin
            if (FindRec.Name <> '.') and (FindRec.Name <> '..') then
            begin
              CreateDir(DestPath);
              WizardForm.ProgressGauge.Position := WizardForm.ProgressGauge.Position + 1;
              CopyAll(SourcePath, DestPath);
            end;
          end;
        until not FindNext(FindRec);
      finally
        FindClose(FindRec);
      end;
    end;
  end;
end;


procedure CopyFilesFromSourceToDest(src, src2, dst, pref : string; FilterPatterns: Array of String );
var
  FindRec: TFindRec;
  SourcePath, DestPath, CompName: string;
begin
  // 1. Copy data from src to dst excluding FilterPatterns
  if (src <> '') and FindFirst(src + '\*', FindRec) then
  begin
    try
      repeat
        if(pref = '') then
          CompName := FindRec.Name
        else
          CompName := pref + '\' + FindRec.Name;

        SourcePath := src + '\' + FindRec.Name;
        DestPath := dst + '\' + FindRec.Name;
        if FindRec.Attributes and FILE_ATTRIBUTE_DIRECTORY = 0 then
        begin
          if not MatchesPattern(CompName, FilterPatterns) and not MatchesRawPattern(CompName) then
          begin
            WizardForm.FilenameLabel.Caption := CompName;
            FileCopy(SourcePath, DestPath, False);
            WizardForm.ProgressGauge.Position := WizardForm.ProgressGauge.Position + 1;
          end;
        end
        else
        begin
          if (FindRec.Name <> '.') and (FindRec.Name <> '..') then
          begin
            if not MatchesPattern(CompName, FilterPatterns) and not MatchesRawPattern(CompName) then
            begin
              WizardForm.FilenameLabel.Caption := CompName;
              CreateDir(DestPath);
              WizardForm.ProgressGauge.Position := WizardForm.ProgressGauge.Position + 1;
              if(pref = '') then
                CopyFilesFromSourceToDest(SourcePath, '', DestPath, FindRec.Name, FilterPatterns)
              else
                CopyFilesFromSourceToDest(SourcePath, '', DestPath, pref + '\' + FindRec.Name, FilterPatterns);

            end
          end;
        end;
      until not FindNext(FindRec);
    finally
      FindClose(FindRec);
    end;
  end;
  
  // 2. Copy data from src2 to dst
  if src2 <> '' then
  begin
    if FindFirst(src2 + '\*', FindRec) then
    begin
      try
        repeat
          SourcePath := src2 + '\' + FindRec.Name;
          DestPath := dst + '\' + FindRec.Name;
          if FindRec.Attributes and FILE_ATTRIBUTE_DIRECTORY = 0 then
          begin
            FileCopy(SourcePath, DestPath, False);
            WizardForm.ProgressGauge.Position := WizardForm.ProgressGauge.Position + 1;
          end
          else
          begin
            if (FindRec.Name <> '.') and (FindRec.Name <> '..') then
            begin
              CreateDir(DestPath);
              WizardForm.ProgressGauge.Position := WizardForm.ProgressGauge.Position + 1;
              CopyFilesFromSourceToDest(SourcePath, '', DestPath, '', FilterPatterns);
            end;
          end;
        until not FindNext(FindRec);
      finally
        FindClose(FindRec);
      end;
    end;
  end;
  
  // 3. Copy data from src under /MAIN subdirectory to dst
  if DirExists(src + '\MAIN') then
  begin
    CopyFilesFromSourceToDest('', src + '\MAIN', dst, '', FilterPatterns);
  end;
end;


procedure CountFileTasks(src, src2: string; FilterPatterns: Array of String; var TaskCount: Integer);
var
  FindRec: TFindRec;
  SourcePath, CompName: string;
begin
  // 1. Count tasks from src excluding FilterPatterns
  if (src <> '') and FindFirst(src + '\*', FindRec) then
  begin
    try
      repeat
        CompName := FindRec.Name;

        SourcePath := src + '\' + FindRec.Name;
        if FindRec.Attributes and FILE_ATTRIBUTE_DIRECTORY = 0 then
        begin
          if not MatchesPattern(CompName, FilterPatterns) and not MatchesRawPattern(CompName) then
          begin
            Inc(TaskCount); // Increase the task count for file copy
          end;
        end
        else
        begin
          if (FindRec.Name <> '.') and (FindRec.Name <> '..') then
          begin
            if not MatchesPattern(CompName, FilterPatterns) and not MatchesRawPattern(CompName) then
            begin
              Inc(TaskCount); // Increase the task count for directory creation
              CountFileTasks(SourcePath, '', FilterPatterns, TaskCount);
            end
          end;
        end;
      until not FindNext(FindRec);
    finally
      FindClose(FindRec);
    end;
  end;

  // 2. Count tasks from src2
  if (src2 <> '') and FindFirst(src2 + '\*', FindRec) then
  begin
    try
      repeat
        SourcePath := src2 + '\' + FindRec.Name;
        if FindRec.Attributes and FILE_ATTRIBUTE_DIRECTORY = 0 then
        begin
          Inc(TaskCount); // Increase the task count for file copy from src2
        end
        else
        begin
          if (FindRec.Name <> '.') and (FindRec.Name <> '..') then
          begin
            Inc(TaskCount); // Increase the task count for directory creation from src2
            CountFileTasks(SourcePath, '', FilterPatterns, TaskCount);
          end;
        end;
      until not FindNext(FindRec);
    finally
      FindClose(FindRec);
    end;
  end;
  
  // 3. Count tasks from src under /MAIN subdirectory
  if DirExists(src + '\MAIN') then
  begin
    CountFileTasks(src + '\MAIN', '', FilterPatterns, TaskCount);
  end;
end;


function GetSHA1OfFile(ShaPath: string; FileName: string) : string;
var
  ResultCode: Integer;
  SpacePos: Integer;
  Output: String;
  Success: Boolean;
begin
  Success := ExecWithResult(ShaPath, '"' + FileName + '"', '', SW_HIDE, ewWaitUntilTerminated, ResultCode, Output);
  
  if Success and (ResultCode = 0) then
  begin
    // Find the position of the space character
    SpacePos := Pos(' ', Output);
    if SpacePos > 0 then
      Result := Copy(Output, 1, SpacePos - 1)
    else
      Result := ''; // This means something unexpected happened with the output

    //MsgBox(Result, mbError, MB_OK);
  end
  else
  begin
    MsgBox(ExpandConstant('{cm:FailedToComputeHash,' + FileName + '}'), mbError, MB_OK);
    Result := '';
  end;
end;




function GetInstallFilters(): Array of String;
var
  FilterPatterns: Array of String;
begin
  SetArrayLength(FilterPatterns, 26); // Number of patterns excluding the regex ones
  FilterPatterns[0] := 'CSERVE';
  FilterPatterns[1] := 'INSTALL.EXE';
  FilterPatterns[2] := 'README.TXT';
  FilterPatterns[3] := 'DATA\DISTRIB.AIF';
  FilterPatterns[4] := 'DATA\H_ATTIC.SMK';
  FilterPatterns[5] := 'DATA\H_FANPRO.SMK';
  FilterPatterns[6] := 'DATA\H_VERT.SMK';
  FilterPatterns[7] := 'DATA\L_ATTIC.SMK';
  FilterPatterns[8] := 'DATA\L_FANPRO.SMK';
  FilterPatterns[9] := 'DATA\L_VERT.SMK';
  FilterPatterns[10] := 'DATA\PRESENT.AIF';
  FilterPatterns[11] := 'DATA\SCHATTEN.AIF';
  FilterPatterns[12] := 'DATA\SEQS.LST';
  FilterPatterns[13] := 'DATA\UNINSTAL.EXE';
  FilterPatterns[14] := 'DATA\SEQS.ALF';
  FilterPatterns[15] := 'DATA\SEQS.MOD';
  FilterPatterns[16] := 'DATA\SPEX.ALF';
  FilterPatterns[17] := 'DATA\SPEX.MOD';

  FilterPatterns[18] := 'DATA\HMISET.CFG';
  FilterPatterns[19] := 'DATA\RIVA.CFG';
  FilterPatterns[20] := 'DATA\LANG.CFG';
  FilterPatterns[21] := 'DATA\UNARCH.EXE';
  FilterPatterns[22] := 'DATA\PATCH.EXE';

  FilterPatterns[23] := 'HDM';
  FilterPatterns[24] := 'MAIN';
  FilterPatterns[25] := 'PERRY';
  Result := FilterPatterns;
end;


function ReadStringFromFile(const FileName: String): String;
var
  Lines: TArrayOfString;
  I: Integer;
begin
  Result := '';
  if LoadStringsFromFile(FileName, Lines) then
  begin
    for I := 0 to GetArrayLength(Lines)-1 do
    begin
      Result := Result + Lines[I] + #13#10;
    end;
  end
  else
  begin
    MsgBox('Failed to read from ' + FileName, mbError, MB_OK);
  end;
end;

procedure SetupConfig();
var
  ConfigContent, PatchContent: string;
  ShortcutPath: string;
  CueFileName : string;
  CueFiles: TStringList;
  i : integer;
  ImageMounting : string;
  ReadMe, AddString : string; 
  OldGameDir : string;
begin
  OldGameDir := GameDir;
  if CopyGameDir then
  begin
    CreateDir(ExpandConstant('{app}\DSA3'));
    CopyAll(GameDir, ExpandConstant('{app}\DSA3'));
    GameDir :=  ExpandConstant('{app}\DSA3');
  end;

  // Add Version to Readme
  // Use AppName and AppVersion from the [Setup] section
  AddString := '## -- ' + UpperCase(ExpandConstant('{#MyAppName}')) + ' --'  + #13#10 +
               '### -- VERSION ' + ExpandConstant('{#MyAppVersion}') + ' --' + #13#10 + #13#10;

  ReadMe := ReadStringFromFile(ExpandConstant('{app}\readme.txt'));
  ReadMe := AddString + ReadMe;
  SaveStringToFile(ExpandConstant('{app}\readme.txt'), ReadMe, False);

  

  if GameVersion = GoG then
  begin
    ImageMounting := 'mount c "' + GameDir + '\cloud_saves" -t overlay' + #13#10 +
                     'imgmount d "' + GameDir + '\game.ins" -t iso -fs iso' + #13#10 
  end else if (GameVersion = Steam) or (GameVersion = Heldenedition) then
  begin
      CueFiles := TStringList.Create;
      try
          ListFiles(ExpandConstant(OldGameDir + '\..\dsaschatt\'), CueFiles);

          // Assuming there's only one .cue file, get its name
          for i := 0 to CueFiles.Count - 1 do
          begin
              if ExtractFileExt(CueFiles.Strings[i]) = '.cue' then
              begin
                  ImageMounting := 'imgmount d "' + OldGameDir + '\..\dsaschatt' + '\' + CueFiles.Strings[i] + '" -t cdrom' + #13#10
                  Break;
              end;
          end;
      finally
          CueFiles.Free;
      end;
  end else
  begin 
    if(GameVersion = OtherExistingImage) or (GameVersion = OtherInstallImage) then
    begin
      if(FileEndsWith(GameImage, '.iso')) then
      begin
        ImageMounting := 'imgmount d "' + GameImage + '" -t iso -fs iso' + #13#10;     
      end else if FileEndsWith(GameImage, '.cue') then
      begin
        ImageMounting := 'imgmount d "' + GameImage + '" -t iso -fs iso' + #13#10;
      end; 
    end else if (GameVersion = OtherExistingCD) or (GameVersion = OtherInstallCD) then
    begin
       // CD ROM
      ImageMounting := 'mount d "' + GameCD + '" -t cdrom -usecd 0' + #13#10;
    end;
  end;
  if (GameVersion <> OtherExtract) and (GameVersion <> Auto) then
  begin
    ConfigContent := '[autoexec]' + #13#10 +
                      'mount c "' + GameDir + '"' + #13#10 +
                      ImageMounting +
                     'c:' + #13#10 +
                     'cls' + #13#10;


    if(IsTaskSelected('patchgame')) then
    begin
      RivaExeSha1 := GetSHA1OfFile(ExpandConstant('{tmp}\sha1\sha1sum.exe'), GameDir + '\RIVA.EXE');
      PatchContent :=  ConfigContent + '@ECHO OFF' + #13#10 + 
      'cls' + #13#10 + 
      'cd PATCH' + #13#10 +
      'RPAT_01'   + #13#10 +
      'copy *.* ..'   + #13#10 +
      'cd ..'     + #13#10 +
      'R_PATCH'   + #13#10 + 
      'choice /c:K /s /t:K,3 /n ' + ExpandConstant('{cm:ClosingMessage}') + #13#10 +
      'exit';

      if('41b0b3e2c38b1ca9aa170477f366151976da03a0' <> RivaExeSha1) then
      begin
        MsgBox(ExpandConstant('{cm:PatchNotApplied,' + RivaExeSha1 + '}'), mbError, MB_OK);
        PatchContent := '';
      end;
      SaveStringToFile(ExpandConstant('{app}\patch_autoexec.conf'), PatchContent, False);     
    end;
    ConfigContent := ConfigContent + '@ECHO OFF' + #13#10 + 'cls' + #13#10 + 'RIVA' + #13#10 + 'choice /c:K /s /t:K,3 /n ' + ExpandConstant('{cm:ClosingMessage}') + #13#10 + 'exit';  


    SaveStringToFile(ExpandConstant('{app}\game_autoexec.conf'), ConfigContent, False);           
    
  end;
end;


procedure CurStepChanged(CurStep: TSetupStep);
var
  ResultCode : Integer;
  Success : Boolean;
  Source : String;
  InstallFilters : Array of String;
  TaskCount : Integer;
begin
  if CurStep = ssPostInstall then
  begin

    if TGameVersion(Page1GameVersion.SelectedValueIndex) = OtherInstallImage then
    begin
      Source := fullExtractRiva(GameImage, ExpandConstant('{tmp}\out\'),0);;
    end else 
    begin
      Source := GameCD;
    end;

    if TGameVersion(Page1GameVersion.SelectedValueIndex) in [OtherInstallImage, OtherInstallCD] then
    begin
      InstallRivaDir :=  ExpandConstant('{app}\DSA3');
      CreateDir(InstallRivaDir);
      GameDir := InstallRivaDir;
      InstallFilters := GetInstallFilters();
      WizardForm.StatusLabel.Caption := ExpandConstant('{cm:InstallingGame}');
      TaskCount := 0;
      WizardForm.ProgressGauge.Position := 0;
      CountFileTasks(Source, ExpandConstant('{tmp}\patch\instcd_chg'), InstallFilters, TaskCount);
      WizardForm.ProgressGauge.Max := TaskCount;
      CopyFilesFromSourceToDest(Source, ExpandConstant('{tmp}\patch\instcd_chg'), InstallRivaDir, '', InstallFilters);
    end;


    SetupConfig();
  end;
end;

