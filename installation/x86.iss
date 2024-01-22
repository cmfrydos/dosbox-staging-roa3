#include "installer.iss"

[Setup]
OutputBaseFilename=riva-dosbox-logger-win-x86-{#MyAppVersion}-setup
ArchitecturesAllowed=x64 x86

[Files]
; Including files from different directories
Source: "files\install\common\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs
Source: "..\vs\Win32\Release\dosbox-roa3.exe"; DestName: "dosbox-roa3.exe"; DestDir: "{app}\dosbox-roa3"; Flags: ignoreversion

Source: "..\vcpkg_installed\x86-windows\x86-windows\bin\libpng16.dll";  DestDir: "{app}\dosbox-roa3"; Flags: ignoreversion
Source: "..\vcpkg_installed\x86-windows\x86-windows\bin\SDL2_net.dll";  DestDir: "{app}\dosbox-roa3"; Flags: ignoreversion
Source: "..\vcpkg_installed\x86-windows\x86-windows\bin\SDL2_image.dll";  DestDir: "{app}\dosbox-roa3"; Flags: ignoreversion
Source: "..\vcpkg_installed\x86-windows\x86-windows\bin\SDL2.dll";  DestDir: "{app}\dosbox-roa3"; Flags: ignoreversion

Source: "..\vcpkg_installed\x86-windows\x86-windows\bin\mt32emu-2.dll";  DestDir: "{app}\dosbox-roa3"; Flags: ignoreversion
Source: "..\vcpkg_installed\x86-windows\x86-windows\bin\ogg.dll";  DestDir: "{app}\dosbox-roa3"; Flags: ignoreversion
Source: "..\vcpkg_installed\x86-windows\x86-windows\bin\opus.dll";  DestDir: "{app}\dosbox-roa3"; Flags: ignoreversion
Source: "..\vcpkg_installed\x86-windows\x86-windows\bin\libfluidsynth-3.dll";  DestDir: "{app}\dosbox-roa3"; Flags: ignoreversion

Source: "..\vcpkg_installed\x86-windows\x86-windows\bin\iir.dll";  DestDir: "{app}\dosbox-roa3"; Flags: ignoreversion
Source: "..\vcpkg_installed\x86-windows\x86-windows\bin\libspeexdsp.dll";  DestDir: "{app}\dosbox-roa3"; Flags: ignoreversion
Source: "..\vcpkg_installed\x86-windows\x86-windows\bin\glib-2.0-0.dll";  DestDir: "{app}\dosbox-roa3"; Flags: ignoreversion

Source: "..\vcpkg_installed\x86-windows\x86-windows\bin\iconv-2.dll";  DestDir: "{app}\dosbox-roa3"; Flags: ignoreversion
Source: "..\vcpkg_installed\x86-windows\x86-windows\bin\intl-8.dll";  DestDir: "{app}\dosbox-roa3"; Flags: ignoreversion
Source: "..\vcpkg_installed\x86-windows\x86-windows\bin\pcre2-8.dll";  DestDir: "{app}\dosbox-roa3"; Flags: ignoreversion

Source: "..\vcpkg_installed\x86-windows\x86-windows\bin\zlib1.dll";  DestDir: "{app}\dosbox-roa3"; Flags: ignoreversion

; Updated paths for 7z, sha1, and patch directories
Source: "files\tmp\x86\7z\*"; DestDir: "{tmp}\7z"; Flags: ignoreversion recursesubdirs deleteafterinstall
Source: "files\tmp\common\sha1\sha1sum.exe"; DestDir: "{tmp}\sha1"; Flags: ignoreversion recursesubdirs deleteafterinstall
Source: "files\tmp\common\patch\*"; DestDir: "{tmp}\patch"; Flags: ignoreversion recursesubdirs deleteafterinstall