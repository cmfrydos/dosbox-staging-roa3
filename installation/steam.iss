[Code]
function GetSteamRegistryValue(const SubKey, ValueName: string): string;
begin
  Result := '';

  // Try to open the registry key
  if RegQueryStringValue(HKLM, SubKey, ValueName, Result) then
  begin
    
  end else Result := '';

end;

function GetSteamLibraryfoldersVdfLocation(): string;
var
  InstallPath: string;
begin
  // Check for 64-bit registry path
  InstallPath := GetSteamRegistryValue('SOFTWARE\Wow6432Node\Valve\Steam', 'InstallPath');

  // If not found, check 32-bit registry path
  if InstallPath = '' then
    InstallPath := GetSteamRegistryValue('SOFTWARE\Valve\Steam', 'InstallPath');

  // If still not found, return an empty string
  if InstallPath = '' then
    Result := '';
    Result := InstallPath + '\steamapps\libraryfolders.vdf';
end;


function GetSteamLibraryFolderPathForApp(const FileName: string; const AppId: string): string;
var
  Lines: TStrings;
  I: Integer;
  Path: string;
  InAppsSection: Boolean;
begin
  Result := '';  // default value
  InAppsSection := False;

  Lines := TStringList.Create;
  try
    // Load the file
    Lines.LoadFromFile(FileName);

    // Iterate over the lines
    for I := 0 to Lines.Count - 1 do
    begin
      // Check if we're inside the "apps" section
      if Pos('"apps"', Lines[I]) > 0 then
        InAppsSection := True;

      if Pos('}', Lines[I]) > 0 then
        InAppsSection := False;

      // If we're in the "apps" section and we find the desired AppId, return the path
      if InAppsSection and (Pos('"' + AppId + '"', Lines[I]) > 0) then
      begin
        Result := Path;
        Exit;
      end;

      // Extract the path for the current library folder
      if Pos('"path"', Lines[I]) > 0 then
      begin
        
        Path := Copy(Lines[I], Pos('"path"', Lines[I]) + 6, Length(Lines[I]));
        Path := Copy(Path, Pos('"', Path) + 1, Length(Path));
        Path := Copy(Path, 0, Pos('"', Path) - 1);
        StringChangeEx(Path, '\\', '\', True);
      end;
    end;

  finally
    Lines.Free;
  end;
end;



function GetSteamGameDirectory(const Path, AppId: string): string;
var
  Lines: TStrings;
  I: Integer;
  InstallDir: string;
begin
  Result := '';;

  Lines := TStringList.Create;
  try
    // Load the appmanifest file for the given AppId
    Lines.LoadFromFile(Path + '\appmanifest_' + AppId + '.acf');

    // Iterate over the lines
    for I := 0 to Lines.Count - 1 do
    begin

      // If AppId is found, look for the installdir key and extract its value
      if (Pos('"installdir"', Lines[I]) > 0) then
      begin
        InstallDir := Copy(Lines[I], Pos('"installdir"', Lines[I]) + 14, Length(Lines[I]));
        InstallDir := Copy(InstallDir, Pos('"', InstallDir) + 1, Length(InstallDir));
        InstallDir := Copy(InstallDir, 0, Pos('"', InstallDir) - 1);
        
        Break;  // We've found what we need, no need to continue
      end;
    end;

    // Construct the final game directory path using the InstallDir
    if InstallDir <> '' then
    begin
      Result := Path + '\common\' + InstallDir + '\dsa3';
      MsgBox(ExpandConstant('{cm:SteamFoundAtMessage,' + Result + '}'), mbInformation, MB_OK);
    end;
  finally
    Lines.Free;
  end;
end;


function CheckForSteamGame(out dir: string): Boolean;
var
  SteamVDF : string;
  Path : string;
begin
  Result := False;   
  dir := ''
  SteamVDF := GetSteamLibraryfoldersVdfLocation();
  if SteamVDF <> '' then
  begin
    Path := GetSteamLibraryFolderPathForApp(SteamVDF,'270760');
    if Path <> '' then
    begin
      Path := GetSteamGameDirectory(Path + '\steamapps', '270760')
      if Path <> '' then
      begin
        dir := Path
        Result := True; 
      end;
    end;
  end;
end;