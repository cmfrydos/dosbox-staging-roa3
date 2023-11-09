#include "installer.iss"

[Setup]
OutputBaseFilename=riva-dosbox-logger-win-x86-{#MyAppVersion}-setup
ArchitecturesAllowed=x64 x86

[Files]
; Including files from different directories
Source: "files\install\common\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs
Source: "files\install\x86\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs
Source: "..\vs\Win32\Release\dosbox.exe"; DestName: "dosbox-roa3.exe"; DestDir: "{app}\dosbox-roa3"; Flags: ignoreversion

; Updated paths for 7z, sha1, and patch directories
Source: "files\tmp\x86\7z\*"; DestDir: "{tmp}\7z"; Flags: ignoreversion recursesubdirs deleteafterinstall
Source: "files\tmp\common\sha1\sha1sum.exe"; DestDir: "{tmp}\sha1"; Flags: ignoreversion recursesubdirs deleteafterinstall
Source: "files\tmp\common\patch\*"; DestDir: "{tmp}\patch"; Flags: ignoreversion recursesubdirs deleteafterinstall