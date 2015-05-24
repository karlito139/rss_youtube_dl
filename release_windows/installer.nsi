; localtube.nsi
;
; This script is perhaps one of the simplest NSIs you can make. All of the
; optional settings are left to their default settings. The installer simply 
; prompts the user asking them where to install, and drops a copy of example1.nsi
; there. 

;--------------------------------

; The name of the installer
Name "Localtube"

; The file to write
OutFile "localtube_installer.exe"

; The default installation directory
InstallDir $PROGRAMFILES\localtube

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\localtube" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel user

;--------------------------------

; Pages
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "" ;No components page, name is not important

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File icudt53.dll
  File icuin53.dll
  File icuuc53.dll
  File libgcc_s_dw2-1.dll
  File libstdc++-6.dll
  File libwinpthread-1.dll
  File Qt5Core.dll
  File Qt5Gui.dll
  File Qt5Network.dll
  File Qt5Widgets.dll
  FILE libssl32.dll
  File ssleay32.dll
  File localtube.exe


  SetOutPath $INSTDIR\platforms
  File platforms\qwindows.dll



	
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\localtube "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\localtube" "DisplayName" "localtube"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\localtube" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\localtube" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\localtube" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

SectionEnd ; end the section


; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\localtube"
  CreateShortCut "$SMPROGRAMS\localtube\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\localtube\localtube.lnk" "$INSTDIR\localtube.exe" "" "$INSTDIR\localtube.exe" 0
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Auto start at boot"

  CreateShortCut "$SMPROGRAMS\Startup\localtube.lnk" "$INSTDIR\localtube.exe" "" "$INSTDIR\localtube.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\localtube"
  DeleteRegKey HKLM SOFTWARE\localtube

  ; Remove files and uninstaller
  Delete $INSTDIR\icudt53.dll
  Delete $INSTDIR\icuin53.dll
  Delete $INSTDIR\icuuc53.dll
  Delete $INSTDIR\libgcc_s_dw2-1.dll
  Delete $INSTDIR\libstdc++-6.dll
  Delete $INSTDIR\libwinpthread-1.dll
  Delete $INSTDIR\Qt5Core.dll
  Delete $INSTDIR\Qt5Gui.dll
  Delete $INSTDIR\Qt5Network.dll
  Delete $INSTDIR\Qt5Widgets.dll
  Delete $INSTDIR\libssl32.dll
  Delete $INSTDIR\ssleay32.dll
  Delete $INSTDIR\platforms\qwindows.dll

  Delete $INSTDIR\localtube.exe
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\localtube\*.*"
  Delete "$SMPROGRAMS\Startup\localtube.lnk"

  ; Remove directories used
  RMDir "$SMPROGRAMS\localtube\platforms"
  RMDir "$SMPROGRAMS\localtube"
  RMDir "$INSTDIR"

SectionEnd


