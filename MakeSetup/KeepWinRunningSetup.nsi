!include "FileFunc.nsh"

!define MYAPP KeepWinRunning
!define AUTHOR "Peter Pagel"
;--------------------------------

; The name of the installer
Name "${MYAPP}"
!define ARP "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MYAPP}"

!define WINSYS32 $%windir%\system32
!define /date CRDATE "%Y%m%d"

; The file to write
OutFile "${MYAPP}Setup${CRDATE}.exe"

XPStyle on

; The default installation directory
InstallDir "$PROGRAMFILES\${AUTHOR}\${MYAPP}"

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\${MYAPP}" "Install_Dir"

!macro IsUserAdmin RESULT
 !define Index "Line${__LINE__}"
   StrCpy ${RESULT} 0
   System::Call '*(&i1 0,&i4 0,&i1 5)i.r0'
   System::Call 'advapi32::AllocateAndInitializeSid(i r0,i 2,i 32,i 544,i 0,i 0,i 0,i 0,i 0, \
   i 0,*i .R0)i.r5'
   System::Free $0
   System::Call 'advapi32::CheckTokenMembership(i n,i R0,*i .R1)i.r5'
   StrCmp $5 0 ${Index}_Error
   StrCpy ${RESULT} $R1
   Goto ${Index}_End
 ${Index}_Error:
   StrCpy ${RESULT} -1
 ${Index}_End:
   System::Call 'advapi32::FreeSid(i R0)i.r5'
 !undef Index
!macroend

;--------------------------------

Icon ..\${MYAPP}\res\${MYAPP}.ico


; Pages

Page license
Page components checkRuntime
Page directory
Page instfiles
PageEx license
   Caption ": $(myHint)"
   LicenseText "$(myHintText)"
   LicenseData "$(ReadmeFile)"
PageExEnd


UninstPage uninstConfirm
UninstPage instfiles


LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\German.nlf"

LicenseLangString myLicenseData ${LANG_ENGLISH} "license.txt"
LicenseLangString myLicenseData ${LANG_GERMAN} "licenseDEU.txt"
LicenseData $(myLicenseData)

LangString Sec1Name ${LANG_ENGLISH} "Main Application (required)"
LangString Sec1Name ${LANG_GERMAN} "Programm (muss)"

LangString myHint ${LANG_ENGLISH} "Hint"
LangString myHint ${LANG_GERMAN} "Hinweis"

LangString myHintText ${LANG_ENGLISH} "Readme"
LangString myHintText ${LANG_GERMAN} "Bedienung"

LicenseLangString ReadmeFile ${LANG_ENGLISH} "readme.txt"
LicenseLangString ReadmeFile ${LANG_GERMAN} "readmeDEU.txt"

LangString requTxt ${LANG_ENGLISH} "is required,$\nplease see"
LangString requTxt ${LANG_GERMAN} "wird benötigt,$\nerhältlich unter"

LangString adminOnly ${LANG_ENGLISH} "For Administrators only"
LangString adminOnly ${LANG_GERMAN} "Nur für Administratoren"


Function checkRuntime
  ReadRegStr $1 HKLM "SOFTWARE\Wow6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\x86" "Installed"
  StrCmp $1 1 RTinstalled
    MessageBox MB_OK "Visual C++ Redistributable for Visual Studio 2015 (x86) $(requTxt) https://www.microsoft.com/download"
    Quit

RTinstalled:

FunctionEnd

;--------------------------------

; The stuff to install
Section $(Sec1Name)

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  !insertmacro IsUserAdmin $0
; $0 = 1 if the user belongs to the administrator's group
; $0 = 0 if not
; $0 = -1 if there was an error
  StrCmp $0 "1" IsAdmin
    MessageBox MB_OK $(adminOnly)
    Quit
  
IsAdmin:

  ; Put files there
  File license.txt
  File licenseDEU.txt
  File readme.txt
  File readmeDEU.txt
  File "..\Release\${MYAPP}.exe"
  File "..\Release\${MYAPP}DEU.dll"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\${MYAPP} "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "${ARP}" "DisplayName" "${MYAPP}"
  WriteRegStr HKLM "${ARP}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKLM "${ARP}" "DisplayVersion" "${CRDATE}"
  WriteRegStr HKLM "${ARP}" "DisplayIcon" '"$INSTDIR\${MYAPP}.exe"'
  WriteRegStr HKLM "${ARP}" "Publisher" "${AUTHOR}"
  WriteRegDWORD HKLM "${ARP}" "NoModify" 1
  WriteRegDWORD HKLM "${ARP}" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  WriteRegDWORD HKLM "${ARP}" "EstimatedSize" "$0"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Run" "${MYAPP}" "$INSTDIR\${MYAPP}.exe"

SectionEnd


;--------------------------------

; Uninstaller

Section "Uninstall"
  
  !insertmacro IsUserAdmin $0
; $0 = 1 if the user belongs to the administrator's group
; $0 = 0 if not
; $0 = -1 if there was an error
  StrCmp $0 "1" IsAdmin
    MessageBox MB_OK $(adminOnly)
    Quit
  
IsAdmin:
  ; Remove registry keys
  DeleteRegKey HKLM "${ARP}"
  DeleteRegKey HKLM SOFTWARE\${MYAPP}
  DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Run" "${MYAPP}" 

  IntOp $0 0 + 0
EnumStart:
  EnumRegKey $R1 HKEY_USERS "" $0
  IntOp $0 $0 + 1
  StrCmp $R1 ".DEFAULT" EnumStart
  StrCmp $R1 "" EnumEnd
  DeleteRegKey HKU "$R1\Software\${AUTHOR}\${MYAPP}"
  Goto EnumStart
EnumEnd:

  ; Remove files and uninstaller
  Delete "$INSTDIR\${MYAPP}.exe"
  Delete "$INSTDIR\${MYAPP}DEU.dll"
  Delete "$INSTDIR\*.txt"
  Delete "$INSTDIR\uninstall.exe"

  ; Remove directories used
  RMDir "$INSTDIR"

SectionEnd

Function .onInit

	;Language selection dialog

	Push ""
	Push ${LANG_ENGLISH}
	Push English
	Push ${LANG_GERMAN}
	Push Deutsch
	Push A ; A means auto count languages
	       ; for the auto count to work the first empty push (Push "") must remain
	LangDLL::LangDialog "Installer Language" "Please select the language of the installer"

	Pop $LANGUAGE
	StrCmp $LANGUAGE "cancel" 0 +2
		Abort
FunctionEnd
