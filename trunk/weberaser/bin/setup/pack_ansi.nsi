Var MSG
Var Dialog
Var BGImage        
Var ImageHandle
Var Btn_Next
Var Btn_Agreement
;��ӭҳ�洰�ھ��
Var Hwnd_Welcome
Var Bool_IsExtend
Var ck_xieyi
Var Bool_Xieyi
Var Lbl_Xieyi
Var ck_sysstup
Var Bool_Sysstup
Var Lbl_Sysstup
Var Btn_Zidingyi
Var Btn_Zuixiaohua
Var Btn_Guanbi
Var Txt_Browser
Var Btn_Browser
Var Edit_BrowserBg
Var Lbl_Path

Var ck_DeskTopLink
Var Lbl_DeskTopLink
Var Bool_DeskTopLink

Var ck_ToolBarLink
Var Lbl_ToolBarLink
Var Bool_ToolBarLink

Var ck_StartTimeDo
Var Lbl_StartTimeDo
Var Bool_StartTimeDo

Var Btn_Install
Var Btn_Return

Var WarningForm
Var Handle_Font
Var Handle_Font9
Var Int_FontOffset

;����������
Var Lbl_Sumary
Var PB_ProgressBar
Var Bmp_Finish
Var Btn_FreeUse

;��̬��ȡ������
Var str_ChannelID

Var bool_needinstofficeandbind
;---------------------------ȫ�ֱ���ű�Ԥ����ĳ���-----------------------------------------------------
; MUI Ԥ���峣��
!define MUI_ABORTWARNING
!define MUI_PAGE_FUNCTION_ABORTWARNING onClickGuanbi
;��װͼ���·������
!define MUI_ICON "images\fav.ico"
;ж��ͼ���·������
!define MUI_UNICON "images\unis.ico"

!define INSTALL_CHANNELID "0001"

!define PRODUCT_NAME "WebEraser"
!define SHORTCUT_NAME "�����Ƥ��"
!define PRODUCT_VERSION "1.0.0.1"
!define VERSION_LASTNUMBER 1
!define NeedSpace 10240
!define EM_OUTFILE_NAME "WebEraserSetup_${INSTALL_CHANNELID}.exe"

!define EM_BrandingText "${PRODUCT_NAME}${PRODUCT_VERSION}"
!define PRODUCT_PUBLISHER "WebEraser"
!define PRODUCT_WEB_SITE ""
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_NAME}.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_MAININFO_FORSELF "Software\${PRODUCT_NAME}"

;ж�ذ����أ��벻Ҫ���״򿪣�
;!define SWITCH_CREATE_UNINSTALL_PAKAGE 1

;CRCCheck on
;---------------------------�������ѹ�����ͣ�Ҳ����ͨ���������ű����ƣ�------------------------------------
SetCompressor /SOLID lzma
SetCompressorDictSize 32
BrandingText "${EM_BrandingText}"
SetCompress force
SetDatablockOptimize on
;XPStyle on
; ------ MUI �ִ����涨�� (1.67 �汾���ϼ���) ------
!include "MUI2.nsh"
!include "WinCore.nsh"
;�����ļ�����ͷ�ļ�
!include "FileFunc.nsh"
!include "nsWindows.nsh"
!include "WinMessages.nsh"
!include "WordFunc.nsh"

!define MUI_CUSTOMFUNCTION_GUIINIT onGUIInit
!define MUI_CUSTOMFUNCTION_UNGUIINIT un.myGUIInit

!insertmacro MUI_LANGUAGE "SimpChinese"
SetFont ���� 9
!define TEXTCOLOR "4D4D4D"
RequestExecutionLevel admin

VIProductVersion ${PRODUCT_VERSION}
VIAddVersionKey /LANG=2052 "ProductName" "${SHORTCUT_NAME}"
VIAddVersionKey /LANG=2052 "Comments" ""
VIAddVersionKey /LANG=2052 "CompanyName" "�����д�����ó�����޹�˾"
;VIAddVersionKey /LANG=2052 "LegalTrademarks" "WebEraser"
VIAddVersionKey /LANG=2052 "LegalCopyright" "Copyright (c) 2015-2017 �����д�����ó�����޹�˾"
VIAddVersionKey /LANG=2052 "FileDescription" "${SHORTCUT_NAME}��װ����"
VIAddVersionKey /LANG=2052 "FileVersion" ${PRODUCT_VERSION}
VIAddVersionKey /LANG=2052 "ProductVersion" ${PRODUCT_VERSION}
VIAddVersionKey /LANG=2052 "OriginalFilename" ${EM_OUTFILE_NAME}

;�Զ���ҳ��
Page custom CheckMessageBox
Page custom WelcomePage
Page custom LoadingPage
UninstPage custom un.MyUnstallMsgBox
UninstPage custom un.MyUnstall


;------------------------------------------------------MUI �ִ����涨���Լ���������------------------------
;Ӧ�ó�����ʾ����
Name "${SHORTCUT_NAME} ${PRODUCT_VERSION}"
;Ӧ�ó������·��
!ifdef SWITCH_CREATE_UNINSTALL_PAKAGE
	OutFile "uninst\_uninst.exe"
!else
	OutFile "bin\${EM_OUTFILE_NAME}"
!endif
InstallDir "$PROGRAMFILES\WebEraser"
InstallDirRegKey HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"

Section MainSetup
SectionEnd

Var isMainUIShow
Function HandlePageChange
	${If} $MSG = 0x408
		;MessageBox MB_ICONINFORMATION|MB_OK "$9,$0"
		${If} $9 != "userchoice"
			Abort
		${EndIf}
		StrCpy $9 ""
	${EndIf}
FunctionEnd

Function un.HandlePageChange
	${If} $MSG = 0x408
		;MessageBox MB_ICONINFORMATION|MB_OK "$9,$0"
		${If} $9 != "userchoice"
			Abort
		${EndIf}
		StrCpy $9 ""
	${EndIf}
FunctionEnd

Function Random
	Exch $0
	Push $1
	System::Call 'kernel32::QueryPerformanceCounter(*l.r1)'
	System::Int64Op $1 % $0
	Pop $0
	Pop $1
	Exch $0
FunctionEnd

Function CloseExe
	FindWindow $R0 "{D8BD00DC-74BF-45ad-B8CB-61CB31C2CE84}_weberaser"
	${If} $R0 != 0
		SendMessage $R0 1324 0 0
	${EndIf}
	${For} $R3 0 3
		FindProcDLL::FindProc "${PRODUCT_NAME}.exe"
		${If} $R3 == 3
		${AndIf} $R0 != 0
			KillProcDLL::KillProc "${PRODUCT_NAME}.exe"
		${ElseIf} $R0 != 0
			Sleep 250
		${Else}
			${Break}
		${EndIf}
	${Next}
FunctionEnd

Var Bool_IsUpdate
Function DoInstall
  ;�ͷ����õ�publicĿ¼
  SetOutPath "$TEMP\${PRODUCT_NAME}"
  StrCpy $1 ${NSIS_MAX_STRLEN}
  StrCpy $0 ""
  System::Call '$TEMP\${PRODUCT_NAME}\EraserSetUp::GetProfileFolder(t) i(.r0).r2' 
  ${If} $0 == ""
	HideWindow
	MessageBox MB_ICONINFORMATION|MB_OK "�ܱ�Ǹ������������֮��Ĵ���,�볢�����°�װ������������뵽�ٷ���վѰ�����"
	Call OnClickQuitOK
  ${EndIf}
  IfFileExists "$0" +4 0
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "�ܱ�Ǹ������������֮��Ĵ���,�볢�����°�װ������������뵽�ٷ���վѰ�����"
  Call OnClickQuitOK
  
  ;������
  IfFileExists "$0\WebEraser\userconfig.dat" 0 +3
  IfFileExists "$0\WebEraser\userconfig.dat.bak" +2 0
  Rename "$0\WebEraser\userconfig.dat" "$0\WebEraser\userconfig.dat.bak"

  
  SetOutPath "$0"
  SetOverwrite on
  File /r "input_config\*.*"
  ;��ɾ��װ
  RMDir /r "$INSTDIR\program"
  RMDir /r "$INSTDIR\xar"
  RMDir /r "$INSTDIR\res"
  RMDir /r "$INSTDIR\appimage"
  ;�ļ���ռ�����һ������
  StrCpy $R4 "$INSTDIR\program\EraserNet32.dll"
  IfFileExists $R4 0 RenameOK
  Delete $R4
  IfFileExists $R4 0 RenameOK
  BeginRename:
  Push "1000" 
  Call Random
  Pop $0
  IfFileExists "$R4.$0" BeginRename
  Rename $R4 "$R4.$0"
  RenameOK:
  
  ;�ͷ��������ļ�����װĿ¼
  SetOutPath "$INSTDIR"
  SetOverwrite on
  File /r "input_main\*.*"
  ;WriteUninstaller "$INSTDIR\uninst.exe"
  File "uninst\uninst.exe"
  
  ;���һ��ע�����
  ${If} $Bool_StartTimeDo == 1
	 IfFileExists "$TEMP\${PRODUCT_NAME}\EraserSvc.dll" 0 +6
	 System::Call '$TEMP\${PRODUCT_NAME}\EraserSvc::SetupInstallService() ?u' 
  ${Else}
	 IfFileExists "$TEMP\${PRODUCT_NAME}\EraserSvc.dll" 0 +3
	 System::Call '$TEMP\${PRODUCT_NAME}\EraserSvc::SetupUninstallService() ?u'
  ${EndIf}
  
  StrCpy $Bool_IsUpdate 0 
  ReadRegStr $0 HKLM "${PRODUCT_MAININFO_FORSELF}" "Path"
  IfFileExists $0 0 +2
  StrCpy $Bool_IsUpdate 1 
  
  ;�ϱ�ͳ��
  SetOutPath "$TEMP\${PRODUCT_NAME}"
  ${GetParameters} $R1
  ${GetOptions} $R1 "/source"  $R0
  IfErrors 0 +2
  StrCpy $R0 $str_ChannelID
  ;�Ƿ�Ĭ��װ
  ${GetParameters} $R1
  ${GetOptions} $R1 "/s"  $R2
  StrCpy $R3 "0"
  IfErrors 0 +2
  StrCpy $R3 "1"
  ${If} $Bool_IsUpdate == 0
	System::Call '$TEMP\${PRODUCT_NAME}\EraserSetUp::SendAnyHttpStat(t "install", t "${VERSION_LASTNUMBER}", t "$R0", i 1) '
	System::Call '$TEMP\${PRODUCT_NAME}\EraserSetUp::SendAnyHttpStat(t "installmethod", t "${VERSION_LASTNUMBER}", t "$R3", i 1) '
	System::Call "$TEMP\${PRODUCT_NAME}\EraserSetUp::Send2LvdunAnyHttpStat(t '1', t '$R0', t '${PRODUCT_VERSION}')"
  ${Else}
	System::Call '$TEMP\${PRODUCT_NAME}\EraserSetUp::SendAnyHttpStat(t "update", t "${VERSION_LASTNUMBER}", t "$R0", i 1)'
	System::Call '$TEMP\${PRODUCT_NAME}\EraserSetUp::SendAnyHttpStat(t "updatemethod", t "${VERSION_LASTNUMBER}", t "$R3", i 1)'
  ${EndIf}  
 ;д�����õ�ע�����Ϣ
  WriteRegStr HKLM "${PRODUCT_MAININFO_FORSELF}" "InstallSource" $str_ChannelID
  WriteRegStr HKLM "${PRODUCT_MAININFO_FORSELF}" "InstDir" "$INSTDIR"
  System::Call '$TEMP\${PRODUCT_NAME}\EraserSetUp::GetTime(*l) i(.r0).r1'
  WriteRegStr HKLM "${PRODUCT_MAININFO_FORSELF}" "InstallTimes" "$0"
  WriteRegStr HKLM "${PRODUCT_MAININFO_FORSELF}" "Path" "$INSTDIR\program\${PRODUCT_NAME}.exe"
  
  ReadRegStr $0 HKLM "${PRODUCT_MAININFO_FORSELF}" "PeerId"
  ${If} $0 == ""
	System::Call '$TEMP\${PRODUCT_NAME}\EraserSetUp::GetPeerID(t) i(.r0).r1'
	WriteRegStr HKLM "${PRODUCT_MAININFO_FORSELF}" "PeerId" "$0"
  ${EndIf}
  
  
  ;д��ͨ�õ�ע�����Ϣ
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\program\${PRODUCT_NAME}.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\program\${PRODUCT_NAME}.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
FunctionEnd

Var ck_bindinfo1
Var lbl_bindinfo1
Var bool_bindinfo1
Var ck_bindinfo2
Var lbl_bindinfo2
Var bool_bindinfo2
Var ck_bindinfo3
Var lbl_bindinfo3
Var bool_bindinfo3
Var ck_bindinfo4
Var lbl_bindinfo4
Var bool_bindinfo4

Var bool_installoffice
Function CmdSilentInstall
	${GetParameters} $R1
	${GetOptions} $R1 "/s"  $R0
	IfErrors FunctionReturn 0
	SetSilent silent
	ReadRegStr $0 HKLM "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	${If} $0 != ""
		StrCpy $INSTDIR "$0"
	${EndIf}
	ReadRegStr $0 HKLM "${PRODUCT_MAININFO_FORSELF}" "Path"
	IfFileExists $0 0 StartInstall
		;System::Call 'EraserSetUp::GetFileVersionString(t $0, t) i(r0, .r1).r2'
		${GetFileVersion} $0 $1
		${VersionCompare} $1 ${PRODUCT_VERSION} $2
		${If} $2 == "2" ;�Ѱ�װ�İ汾���ڸð汾
			Goto StartInstall
		${Else}
			${GetParameters} $R1
			${GetOptions} $R1 "/write"  $R0
			IfErrors 0 +2
			Abort
			StrCpy $bool_installoffice "true"
			Goto StartInstall
		${EndIf}
	StartInstall:
	
	;���˳���Ϣ
	Call CloseExe
	Call DoInstall
	SetOutPath "$INSTDIR\program"
	CreateDirectory "$SMPROGRAMS\${SHORTCUT_NAME}"
	CreateShortCut "$SMPROGRAMS\${SHORTCUT_NAME}\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startmenuprograms" "$INSTDIR\res\WebEraser.TrayIcon.Open.ico"
	CreateShortCut "$SMPROGRAMS\${SHORTCUT_NAME}\ж��${SHORTCUT_NAME}.lnk" "$INSTDIR\uninst.exe"
	;������������
	ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
	${VersionCompare} "$R0" "6.0" $2
	${if} $2 == 2
		CreateShortCut "$QUICKLAUNCH\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom toolbar" "$INSTDIR\res\WebEraser.TrayIcon.Open.ico"
		CreateShortCut "$STARTMENU\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startbar" "$INSTDIR\res\WebEraser.TrayIcon.Open.ico"
		SetOutPath "$TEMP\${PRODUCT_NAME}"
		IfFileExists "$TEMP\${PRODUCT_NAME}\EraserSetUp.dll" 0 +2
		System::Call '$TEMP\${PRODUCT_NAME}\EraserSetUp::PinToStartMenu4XP(b true, t "$STARTMENU\${SHORTCUT_NAME}.lnk")'
	${else}
		Call GetPinPath
		${If} $0 != "" 
		${AndIf} $0 != 0
			ExecShell taskbarunpin "$0\TaskBar\${SHORTCUT_NAME}.lnk"
			StrCpy $R0 "$0\TaskBar\${SHORTCUT_NAME}.lnk"
			Call RefreshIcon
			Sleep 500
			SetOutPath "$INSTDIR\program"
			CreateShortCut "$INSTDIR\program\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom toolbar" "$INSTDIR\res\WebEraser.TrayIcon.Open.ico"
			ExecShell taskbarpin "$INSTDIR\program\${SHORTCUT_NAME}.lnk" "/sstartfrom toolbar"
			
			ExecShell startunpin "$0\StartMenu\${SHORTCUT_NAME}.lnk"
			Sleep 1000
			CreateShortCut "$STARTMENU\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startbar" "$INSTDIR\res\WebEraser.TrayIcon.Open.ico"
			StrCpy $R0 "$STARTMENU\${SHORTCUT_NAME}.lnk" 
			Call RefreshIcon
			Sleep 200
			ExecShell startpin "$STARTMENU\${SHORTCUT_NAME}.lnk" "/sstartfrom startbar"
		${EndIf}
	${Endif}
	
	SetOutPath "$INSTDIR\program"
	;�����ݷ�ʽ
	CreateShortCut "$DESKTOP\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom desktop" "$INSTDIR\res\WebEraser.TrayIcon.Open.ico"
	${RefreshShellIcons}
	StrCpy $R0 "$DESKTOP\${SHORTCUT_NAME}.lnk"
	Call RefreshIcon
	;��Ĭ��װ���������п�������
	${GetParameters} $R1
	${GetOptions} $R1 "/setboot"  $R0
	IfErrors +3 0
	StrCpy $bool_installoffice "true"
	WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME}" '"$INSTDIR\program\${PRODUCT_NAME}.exe" /embedding /sstartfrom sysboot'
	System::Call '$TEMP\${PRODUCT_NAME}\EraserSetUp::GetTime(*l) i(.r0).r1'
	WriteRegStr HKLM "${PRODUCT_MAININFO_FORSELF}" "ShowIntroduce" "$0"
	${GetParameters} $R1
	${GetOptions} $R1 "/run"  $R0
	IfErrors 0 +3
	Call ExitWithCheck
	Abort
	StrCpy $bool_installoffice "true"
	${If} $R0 == ""
	${OrIf} $R0 == 0
		StrCpy $R0 "/embedding"
	${EndIf}
	SetOutPath "$INSTDIR\program"
	ExecShell open "${PRODUCT_NAME}.exe" "$R0 /sstartfrom installfinish" SW_SHOWNORMAL
	Call ExitWithCheck
	Abort
	FunctionReturn:
FunctionEnd

Function ExitWithCheck
	System::Call "$TEMP\${PRODUCT_NAME}\EraserSetUp::WaitForStat()"
	${If} $bool_installoffice == "true"
	${AndIf} $bool_needinstofficeandbind == "true"
		;System::Call "$TEMP\${PRODUCT_NAME}\EraserSetUp::LoadLuaRunTime(t '$INSTDIR\program', t '')"
		System::Call "$TEMP\${PRODUCT_NAME}\EraserSetUp::SetUpExit()"
	${Else}
		System::Call "$TEMP\${PRODUCT_NAME}\EraserSetUp::SetUpExit()"
	${EndIf}
FunctionEnd

Function UnstallOnlyFile
	;ɾ��
	RMDir /r "$1\xar"
	Delete "$1\uninst.exe"
	RMDir /r "$1\program"
	RMDir /r "$1\res"
	
	 ;�ļ���ռ�����һ������
	StrCpy $R4 "$1\program\EraserNet32.dll"
	IfFileExists $R4 0 RenameOK
	BeginRename:
	Push "1000" 
	Call Random
	Pop $2
	IfFileExists "$R4.$2" BeginRename
	Rename $R4 "$R4.$2"
	Delete /REBOOTOK "$R4.$2"
	RenameOK:
	
	StrCpy "$R0" "$1"
	System::Call 'Shlwapi::PathIsDirectoryEmpty(t R0)i.s'
	Pop $R1
	${If} $R1 == 1
		RMDir /r "$1"
	${EndIf}
FunctionEnd


Function CmdUnstall
	${GetOptions} $R1 "/uninstall"  $R0
	IfErrors FunctionReturn 0
	SetSilent silent
	;ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	IfFileExists $INSTDIR +2 0
	Abort
	;���˳���Ϣ
	Call CloseExe
	ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
	${VersionCompare} "$R0" "6.0" $2
	${if} $2 == 2
		Delete "$QUICKLAUNCH\${SHORTCUT_NAME}.lnk"
		SetOutPath "$TEMP\${PRODUCT_NAME}"
		IfFileExists "$TEMP\${PRODUCT_NAME}\EraserSetUp.dll" 0 +2
		System::Call '$TEMP\${PRODUCT_NAME}\EraserSetUp::PinToStartMenu4XP(b 0, t "$STARTMENU\${SHORTCUT_NAME}.lnk")'
	${else}
		Call GetPinPath
		${If} $0 != "" 
		${AndIf} $0 != 0
			ExecShell taskbarunpin "$0\TaskBar\${SHORTCUT_NAME}.lnk"
			StrCpy $R0 "$0\TaskBar\${SHORTCUT_NAME}.lnk"
			Call RefreshIcon
			Sleep 200
			ExecShell startunpin "$0\StartMenu\${SHORTCUT_NAME}.lnk"
			StrCpy $R0 "$0\StartMenu\${SHORTCUT_NAME}.lnk"
			Call RefreshIcon
			Sleep 200
		${EndIf}
	${Endif}
	;����ж�ط���
	IfFileExists "$TEMP\${PRODUCT_NAME}\EraserSvc.dll" 0 +2
    System::Call '$TEMP\${PRODUCT_NAME}\EraserSvc::SetupUninstallService() ?u'
	StrCpy $1 $INSTDIR
	Call UnstallOnlyFile
	
	;��ȡ������
	ReadRegStr $R4 HKLM "${PRODUCT_MAININFO_FORSELF}" "InstallSource"
	${If} $R4 != ""
	${AndIf} $R4 != 0
		StrCpy $str_ChannelID $R4
	${EndIF}
	
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	IfFileExists "$TEMP\${PRODUCT_NAME}\EraserSetUp.dll" 0 +2
	System::Call '$TEMP\${PRODUCT_NAME}\EraserSetUp::SendAnyHttpStat(t "uninstall", t "${VERSION_LASTNUMBER}", t "$str_ChannelID", i 1) '
	System::Call '$TEMP\${PRODUCT_NAME}\EraserSetUp::Send2LvdunAnyHttpStat(t "uninstall", t "$str_ChannelID")'
	ReadRegStr $0 HKLM "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	${If} $0 == "$INSTDIR"
		DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
		DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
		 ;ɾ�����õ�ע�����Ϣ
		DeleteRegKey HKLM "${PRODUCT_MAININFO_FORSELF}"
		DeleteRegValue HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME}"
	${EndIf}
	IfFileExists "$DESKTOP\${SHORTCUT_NAME}.lnk" 0 +2
		Delete "$DESKTOP\${SHORTCUT_NAME}.lnk"
	IfFileExists "$STARTMENU\${SHORTCUT_NAME}.lnk" 0 +2
		Delete "$STARTMENU\${SHORTCUT_NAME}.lnk"
	RMDir /r "$SMPROGRAMS\${SHORTCUT_NAME}"
	Abort
	FunctionReturn:
FunctionEnd

Function UpdateChanel
	ReadRegStr $R0 HKLM "${PRODUCT_MAININFO_FORSELF}" "InstallSource"
	${If} $R0 != 0
	${AndIf} $R0 != ""
	${AndIf} $R0 != "unknown"
		StrCpy $str_ChannelID $R0
	${Else}
		System::Call 'kernel32::GetModuleFileName(i 0, t R2R2, i 256)'
		Push $R2
		Push "\"
		Call GetLastPart
		Pop $R1
		${WordReplace} "$R1" "WebEraserSetup_" "" "+" $R3
		${WordReplace} "$R3" ".exe" "" "+" $R4
		;MessageBox MB_ICONINFORMATION|MB_OK $R4
		${If} $R4 == 0
		${OrIf} $R4 == ""
			StrCpy $str_ChannelID ${INSTALL_CHANNELID}
		${ElseIf} $R1 == $R3
		${OrIf} $R3 == $R4
			StrCpy $str_ChannelID "unknown"
		${Else}
			StrCpy $str_ChannelID $R4
		${EndIf}
	${EndIf}
FunctionEnd
	
Function .onInit
	${If} ${SWITCH_CREATE_UNINSTALL_PAKAGE} == 1 
		WriteUninstaller "$EXEDIR\uninst.exe"
		Abort
	${EndIf}
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "Global\{D8BD00DC-74BF-45ad-B8CB-61CB31C2CE84}_INSTALL_MUTEX") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +2
	Abort
	StrCpy $Int_FontOffset 4
	CreateFont $Handle_Font "����" 10 0
	CreateFont $Handle_Font9 "����" 9 0
	IfFileExists "$FONTS\msyh.ttf" 0 +4
	CreateFont $Handle_Font "΢���ź�" 10 0
	CreateFont $Handle_Font9 "΢���ź�" 9 0
	StrCpy $Int_FontOffset 0
	
	Call UpdateChanel
	Call InitInsConfParam
	
	SetOutPath "$TEMP"
	SetOverwrite on
	File ".\webindcfg.dat"
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	SetOverwrite on
	File "bin\EraserSetUp.dll"
	File "input_main\program\Microsoft.VC90.CRT.manifest"
	File "input_main\program\msvcp90.dll"
	File "input_main\program\msvcr90.dll"
	File "input_main\program\EraserSvc.dll"
	File "input_main\program\Microsoft.VC90.ATL.manifest"
	File "input_main\program\ATL90.dll"
	File "license\license.txt"
	
	
	;�Ƿ���Ҫװoffice�Լ�չʾ����λ��
	System::Call "kernel32::GetModuleFileName(i 0, t R2R2, i 256)"
	Push $R2
	Push "\"
	Call GetLastPart
	Pop $R1
	${If} $R1 != "WebEraserSetup${PRODUCT_VERSION}.exe"
		StrCpy $bool_needinstofficeandbind "true"
	${EndIf}
	
	Call CmdSilentInstall
	;Call CmdUnstall
	;�н���İ�װ����ini�ļ�
	${If} $bool_needinstofficeandbind == "true"
		;System::Call "$TEMP\${PRODUCT_NAME}\EraserSetUp::DownLoadIniConfig()"
	${EndIf}
	
	ReadRegStr $0 HKLM "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	${If} $0 != ""
		StrCpy $INSTDIR "$0"
	${EndIf}
	InitPluginsDir
    File "/ONAME=$PLUGINSDIR\bg.bmp" "images\bg.bmp"
	File "/ONAME=$PLUGINSDIR\btn_next.bmp" "images\btn_next.bmp"
	File "/oname=$PLUGINSDIR\btn_agreement1.bmp" "images\btn_agreement1.bmp"
	File "/oname=$PLUGINSDIR\btn_agreement2.bmp" "images\btn_agreement2.bmp"
	File "/oname=$PLUGINSDIR\checkbox1.bmp" "images\checkbox1.bmp"
	File "/oname=$PLUGINSDIR\checkbox2.bmp" "images\checkbox2.bmp"
	File "/oname=$PLUGINSDIR\btn_min.bmp" "images\btn_min.bmp"
	File "/oname=$PLUGINSDIR\btn_close.bmp" "images\btn_close.bmp"
	File "/oname=$PLUGINSDIR\btn_change.bmp" "images\btn_change.bmp"
	File "/oname=$PLUGINSDIR\edit_bg.bmp" "images\edit_bg.bmp"
	File "/oname=$PLUGINSDIR\btn_install.bmp" "images\btn_install.bmp"
	File "/oname=$PLUGINSDIR\quit.bmp" "images\quit.bmp"
	File "/oname=$PLUGINSDIR\btn_quitsure.bmp" "images\btn_quitsure.bmp"
	File "/oname=$PLUGINSDIR\btn_quitreturn.bmp" "images\btn_quitreturn.bmp"   	
   	File "/oname=$PLUGINSDIR\loading1.bmp" "images\loading1.bmp"
    File "/oname=$PLUGINSDIR\loading2.bmp" "images\loading2.bmp"
	;File "/oname=$PLUGINSDIR\loading3.bmp" "images\loading3.bmp"
	File "/oname=$PLUGINSDIR\loading_finish.bmp" "images\loading_finish.bmp"
	File "/oname=$PLUGINSDIR\btn_freeuse.bmp" "images\btn_freeuse.bmp"
    
	SkinBtn::Init "$PLUGINSDIR\btn_next.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_agreement1.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_agreement2.bmp"
	SkinBtn::Init "$PLUGINSDIR\checkbox1.bmp"
	SkinBtn::Init "$PLUGINSDIR\checkbox2.bmp"
	SkinBtn::Init "$PLUGINSDIR\checkbox2.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_min.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_close.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_change.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_install.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_quitsure.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_quitreturn.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_freeuse.bmp"
FunctionEnd


Function InitInsConfParam
	StrCpy $Bool_StartTimeDo 1
FunctionEnd


Function onMsgBoxCloseCallback
  ${If} $MSG = ${WM_CLOSE}
   Call OnClickQuitOK
  ${Else}
	Call HandlePageChange
  ${EndIf}
FunctionEnd

Var Hwnd_MsgBox
Var btn_MsgBoxSure
Var btn_MsgBoxCancel
Var lab_MsgBoxText
Var lab_MsgBoxText2
Function GsMessageBox
	IsWindow $Hwnd_MsgBox Create_End
	GetDlgItem $0 $HWNDPARENT 1
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 2
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 3
    ShowWindow $0 ${SW_HIDE}
	HideWindow
    nsDialogs::Create 1044
    Pop $Hwnd_MsgBox
    ${If} $Hwnd_MsgBox == error
        Abort
    ${EndIf}
    SetCtlColors $Hwnd_MsgBox ""  transparent ;�������͸��

    ${NSW_SetWindowSize} $HWNDPARENT 287 129 ;�ı䴰���С
    ${NSW_SetWindowSize} $Hwnd_MsgBox 287 129 ;�ı�Page��С
	System::Call  'User32::GetDesktopWindow() i.r8'
	${NSW_CenterWindow} $HWNDPARENT $8
	;System::Call "user32::SetForegroundWindow(i $HWNDPARENT)"
	
	StrCpy $3 3
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 12 $3 40 20 "��ʾ"
	Pop $0
    SetCtlColors $0 "ffffff" transparent ;�������͸��
	SendMessage $0 ${WM_SETFONT} $Handle_Font9 0
	
	${NSD_CreateButton} 106 96 70 25 ''
	Pop $btn_MsgBoxSure
	StrCpy $1 $btn_MsgBoxSure
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_quitsure.bmp $1
	SkinBtn::onClick $1 $R7

	${NSD_CreateButton} 201 96 70 25 ''
	Pop $btn_MsgBoxCancel
	StrCpy $1 $btn_MsgBoxCancel
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_quitreturn.bmp $1
	GetFunctionAddress $0 OnClickQuitOK
	SkinBtn::onClick $1 $0
	
	StrCpy $3 45
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 46 $3 250 20 $R6
	Pop $lab_MsgBoxText
    SetCtlColors $lab_MsgBoxText "333333" transparent ;�������͸��
	SendMessage $lab_MsgBoxText ${WM_SETFONT} $Handle_Font 0
	
	StrCpy $3 65
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 46 $3 250 20 $R8
	Pop $lab_MsgBoxText2
    SetCtlColors $lab_MsgBoxText2 "333333" transparent ;�������͸��
	SendMessage $lab_MsgBoxText2 ${WM_SETFONT} $Handle_Font 0
	
	
	GetFunctionAddress $0 onGUICallback
    ;��������ͼ
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\quit.bmp $ImageHandle
	
	WndProc::onCallback $BGImage $0 ;�����ޱ߿����ƶ�
	
	GetFunctionAddress $0 onMsgBoxCloseCallback
	WndProc::onCallback $HWNDPARENT $0 ;����ر���Ϣ
	
	nsDialogs::Show
	${NSD_FreeImage} $ImageHandle
	Create_End:
	HideWindow
	ShowWindow $Hwnd_MsgBox ${SW_HIDE}
	System::Call  'User32::GetDesktopWindow() i.r8'
	${NSW_CenterWindow} $HWNDPARENT $8
	system::Call `user32::SetWindowText(i $lab_MsgBoxText, t "$R6")`
	system::Call `user32::SetWindowText(i $lab_MsgBoxText2, t "$R8")`
	SkinBtn::onClick $btn_MsgBoxSure $R7
	
	ShowWindow $HWNDPARENT ${SW_SHOW}
	ShowWindow $Hwnd_MsgBox ${SW_SHOW}
	Call SetWindowShowTop
FunctionEnd

Function ClickSure2
	ShowWindow $Hwnd_MsgBox ${SW_HIDE}
	ShowWindow $HWNDPARENT ${SW_HIDE}
	${If} $9 != 0
		SendMessage $9 1324 0 0
		${For} $R3 0 3
			FindProcDLL::FindProc "${PRODUCT_NAME}.exe"
			${If} $R3 == 3
			${AndIf} $R0 != 0
				KillProcDLL::KillProc "${PRODUCT_NAME}.exe"
			${ElseIf} $R0 != 0
				Sleep 250
			${Else}
				${Break}
			${EndIf}
		${Next}
	${EndIf}
	StrCpy $R9 1 ;Goto the next page
    Call RelGotoPage
FunctionEnd

Function ClickSure1
	ShowWindow $Hwnd_MsgBox ${SW_HIDE}
	ShowWindow $HWNDPARENT ${SW_HIDE}
	Sleep 100
	;���˳���Ϣ
	FindWindow $9 "{D8BD00DC-74BF-45ad-B8CB-61CB31C2CE84}_weberaser"
	${If} $9 != 0
		StrCpy $R6 "��⵽${SHORTCUT_NAME}�������У�"
		StrCpy $R8 "�Ƿ�ǿ�ƽ�����"
		GetFunctionAddress $R7 ClickSure2
		Call GsMessageBox
	${Else}
		Call ClickSure2
	${EndIf}
FunctionEnd

Function CheckMessageBox
	ReadRegStr $0 HKLM "${PRODUCT_MAININFO_FORSELF}" "Path"
	IfFileExists $0 0 StartInstall
	${GetFileVersion} $0 $1
	${VersionCompare} $1 ${PRODUCT_VERSION} $2
	${GetParameters} $R1
	${GetOptions} $R1 "/write"  $R0
	IfErrors 0 +3
	push "false"
	pop $R0
	${If} $2 == "2" ;�Ѱ�װ�İ汾���ڸð汾
		Call ClickSure1
	${ElseIf} $2 == "0" ;�汾��ͬ
	${OrIf} $2 == "1"	;�Ѱ�װ�İ汾���ڸð汾
		 ${If} $R0 == "false"
			StrCpy $R6 "��⵽�Ѱ�װ${SHORTCUT_NAME} $1��"
			StrCpy $R8 "�Ƿ񸲸ǰ�װ��"
			GetFunctionAddress $R7 ClickSure1
			Call GsMessageBox
		${Else}
			Call ClickSure1
		${EndIf}
	${EndIf}
	Goto EndFunction
	StartInstall:
	Call ClickSure1
	EndFunction:
FunctionEnd

Function onGUIInit
	;�����߿�
    System::Call `user32::SetWindowLong(i$HWNDPARENT,i${GWL_STYLE},0x9480084C)i.R0`
    ;����һЩ���пؼ�
    GetDlgItem $0 $HWNDPARENT 1034
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1035
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1036
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1037
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1038
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1039
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1256
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1028
    ShowWindow $0 ${SW_HIDE}
FunctionEnd

;�����ޱ߿��ƶ�
Function onGUICallback
  ${If} $MSG = ${WM_LBUTTONDOWN}
    SendMessage $HWNDPARENT ${WM_NCLBUTTONDOWN} ${HTCAPTION} $0
  ${EndIf}
FunctionEnd

Function onCloseCallback
	${If} $MSG = ${WM_CLOSE}
		Call onClickGuanbi
	${Else} 
		Call HandlePageChange
	${EndIf}
FunctionEnd

;��һ����ť�¼�
Function onClickNext
	Call OnClick_Install
FunctionEnd

;Э�鰴ť�¼�
Function onClickAgreement
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	ExecShell open license.txt /x SW_SHOWNORMAL
FunctionEnd

;-----------------------------------------Ƥ����ͼ����----------------------------------------------------
Function SkinBtn_Next
  SkinBtn::Set /IMGID=$PLUGINSDIR\btn_next.bmp $1
FunctionEnd

Function SkinBtn_Agreement1
  SkinBtn::Set /IMGID=$PLUGINSDIR\btn_agreement1.bmp $1
FunctionEnd

Function OnClick_CheckXieyi
	${IF} $Bool_Xieyi == 1
        IntOp $Bool_Xieyi $Bool_Xieyi - 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox1.bmp $ck_xieyi
		EnableWindow $Btn_Next 0
		EnableWindow $Btn_Zidingyi 0
    ${ELSE}
        IntOp $Bool_Xieyi $Bool_Xieyi + 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $ck_xieyi
		EnableWindow $Btn_Next 1
		EnableWindow $Btn_Zidingyi 1
    ${EndIf}
	ShowWindow $ck_xieyi ${SW_HIDE}
	ShowWindow $ck_xieyi ${SW_SHOW}
FunctionEnd


Function OnClick_CheckSysstup
	${IF} $Bool_Sysstup == 1
        IntOp $Bool_Sysstup $Bool_Sysstup - 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox1.bmp $ck_sysstup
    ${ELSE}
        IntOp $Bool_Sysstup $Bool_Sysstup + 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $ck_sysstup
    ${EndIf}
	ShowWindow $ck_sysstup ${SW_HIDE}
	ShowWindow $ck_sysstup ${SW_SHOW}
FunctionEnd



Function OnClick_BrowseButton
	Pop $0
	Push $INSTDIR ; input string "C:\Program Files\ProgramName"
	Call GetParent
	Pop $R0 ; first part "C:\Program Files"

	Push $INSTDIR ; input string "C:\Program Files\ProgramName"
	Push "\" ; input chop char
	Call GetLastPart
	Pop $R1 ; last part "ProgramName"
	${If} $R1 == 0
	${Orif} $R1 == ""
		StrCpy $R1 "WebEraser"
	${EndIf}

	nsDialogs::SelectFolderDialog "��ѡ�� $R0 ��װ���ļ���:" "$R0"
	Pop $0
	${If} $0 == "error" # returns 'error' if 'cancel' was pressed?
		Return
	${EndIf}
	${If} $0 != ""
	StrCpy $INSTDIR "$0\$R1"
	${WordReplace} $INSTDIR  "\\" "\" "+" $0
	StrCpy $INSTDIR "$0"
	system::Call `user32::SetWindowText(i $Txt_Browser, t "$INSTDIR")`
	${EndIf}
FunctionEnd

Function GetParent
  Exch $R0 ; input string
  Push $R1
  Push $R2
  Push $R3
  StrCpy $R1 0
  StrLen $R2 $R0
  loop:
    IntOp $R1 $R1 + 1
    IntCmp $R1 $R2 get 0 get
    StrCpy $R3 $R0 1 -$R1
    StrCmp $R3 "\" get
    Goto loop
  get:
    StrCpy $R0 $R0 -$R1
    Pop $R3
    Pop $R2
    Pop $R1
    Exch $R0 ; output string
FunctionEnd

Function GetLastPart
  Exch $0 ; chop char
  Exch
  Exch $1 ; input string
  Push $2
  Push $3
  StrCpy $2 0
  loop:
    IntOp $2 $2 - 1
    StrCpy $3 $1 1 $2
    StrCmp $3 "" 0 +3
      StrCpy $0 ""
      Goto exit2
    StrCmp $3 $0 exit1
    Goto loop
  exit1:
    IntOp $2 $2 + 1
    StrCpy $0 $1 "" $2
  exit2:
    Pop $3
    Pop $2
    Pop $1
    Exch $0 ; output string
FunctionEnd

;����Ŀ¼�¼�
Function OnChange_DirRequest
	System::Call "user32::GetWindowText(i $Txt_Browser, t .r0, i ${NSIS_MAX_STRLEN})"
	StrCpy $INSTDIR $0
	StrCpy $6 $INSTDIR 2
	StrCpy $7 $INSTDIR 3
	${If} $INSTDIR == ""
	${OrIf} $INSTDIR == 0
	${OrIf} $INSTDIR == $6
	${OrIf} $INSTDIR == $7
		EnableWindow $Btn_Install 0
		EnableWindow $Btn_Return 0
	${Else}
		StrCpy $8 ""
		${DriveSpace} $7 "/D=F /S=K" $8
		${If} $8 == ""
			EnableWindow $Btn_Install 0
			EnableWindow $Btn_Return 0
			Abort
		${EndIf}
		IntCmp ${NeedSpace} $8 0 0 ErrorChunk
		EnableWindow $Btn_Install 1
		EnableWindow $Btn_Return 1
		Goto EndFunction
		ErrorChunk:
			MessageBox MB_OK|MB_ICONSTOP "����ʣ��ռ䲻�㣬��Ҫ����${NeedSpace}KB"
			EnableWindow $Btn_Install 0
			EnableWindow $Btn_Return 0
		EndFunction:
	${EndIf}
FunctionEnd

Function onClickZidingyi
	ShowWindow $Btn_Next ${SW_HIDE}
	ShowWindow $Btn_Agreement ${SW_HIDE}
	ShowWindow $Lbl_Xieyi ${SW_HIDE}
	ShowWindow $Lbl_Sysstup ${SW_HIDE}
	ShowWindow $ck_xieyi ${SW_HIDE}
	ShowWindow $ck_sysstup ${SW_HIDE}
	ShowWindow $Btn_Zidingyi ${SW_HIDE}
	ShowWindow $ck_StartTimeDo ${SW_HIDE}
	ShowWindow $Lbl_StartTimeDo ${SW_HIDE}
	ShowWindow $Edit_BrowserBg 1
	ShowWindow $Txt_Browser 1
	ShowWindow $Btn_Browser 1
	ShowWindow $Lbl_Path 1
	ShowWindow $ck_DeskTopLink 1
	ShowWindow $Lbl_DeskTopLink 1	
	ShowWindow $ck_ToolBarLink 1
	ShowWindow $Lbl_ToolBarLink 1
	ShowWindow $Btn_Install 1
	ShowWindow $Btn_Return 1
FunctionEnd

Function OnClick_Return
	ShowWindow $Btn_Next 1
	ShowWindow $Btn_Agreement 1
	ShowWindow $Lbl_Xieyi 1
	ShowWindow $Lbl_Sysstup 1
	ShowWindow $ck_xieyi 1
	ShowWindow $ck_sysstup 1
	ShowWindow $Btn_Zidingyi 1
	ShowWindow $ck_StartTimeDo 1
	ShowWindow $Lbl_StartTimeDo 1
	ShowWindow $Edit_BrowserBg ${SW_HIDE}
	ShowWindow $Txt_Browser ${SW_HIDE}
	ShowWindow $Btn_Browser ${SW_HIDE}
	ShowWindow $Lbl_Path ${SW_HIDE}
	ShowWindow $ck_DeskTopLink ${SW_HIDE}
	ShowWindow $Lbl_DeskTopLink ${SW_HIDE}	
	ShowWindow $ck_ToolBarLink ${SW_HIDE}
	ShowWindow $Lbl_ToolBarLink ${SW_HIDE}
	ShowWindow $Btn_Install ${SW_HIDE}
	ShowWindow $Btn_Return ${SW_HIDE}
FunctionEnd

Function onClickZuixiaohua
	 ShowWindow $HWNDPARENT 2
FunctionEnd

Function onWarningGUICallback
  ${If} $MSG = ${WM_LBUTTONDOWN}
    SendMessage $WarningForm ${WM_NCLBUTTONDOWN} ${HTCAPTION} $0
  ${EndIf}
FunctionEnd

Function onClickGuanbi
	${If} $isMainUIShow != "true"
		Call OnClickQuitOK
		Abort
	${EndIf}
	IsWindow $WarningForm Create_End
	!define Style ${WS_VISIBLE}|${WS_OVERLAPPEDWINDOW}
	${NSW_CreateWindowEx} $WarningForm $hwndparent ${ExStyle} ${Style} "" 1018

	${NSW_SetWindowSize} $WarningForm 287 129
	EnableWindow $hwndparent 0
	System::Call `user32::SetWindowLong(i $WarningForm,i ${GWL_STYLE},0x9480084C)i.R0`
	
	StrCpy $3 3
	IntOp $3 $3 + $Int_FontOffset
	${NSW_CreateLabel} 12 $3 40 20 "��ʾ"
	Pop $0
    SetCtlColors $0 "ffffff" transparent ;�������͸��
	SendMessage $0 ${WM_SETFONT} $Handle_Font9 0
	
	${NSW_CreateButton} 106 95 70 25 ''
	Pop $R0
	StrCpy $1 $R0
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_quitsure.bmp $1
	${NSW_OnClick} $R0 OnClickQuitOK

	${NSW_CreateButton} 198 95 70 25 ''
	Pop $R0
	StrCpy $1 $R0
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_quitreturn.bmp $1
	${NSW_OnClick} $R0 OnClickQuitCancel

	StrCpy $3 53
	IntOp $3 $3 + $Int_FontOffset
	${NSW_CreateLabel} 35 $3 250 20 "��ȷ��Ҫ�˳������Ƥ����װ����"
	Pop $4
    SetCtlColors $4 "333333" transparent ;�������͸��
	SendMessage $4 ${WM_SETFONT} $Handle_Font 0
	
	${NSW_CreateBitmap} 0 0 100% 100% ""
  	Pop $1
	${NSW_SetImage} $1 $PLUGINSDIR\quit.bmp $ImageHandle
	GetFunctionAddress $0 onWarningGUICallback
	WndProc::onCallback $1 $0 ;�����ޱ߿����ƶ�
	${NSW_CenterWindow} $WarningForm $hwndparent
	${NSW_Show}
	Create_End:
	ShowWindow $WarningForm ${SW_SHOW}
	Abort
FunctionEnd

Function OnClickQuitOK
	HideWindow
	Call ExitWithCheck2
FunctionEnd

Function ExitWithCheck2
	StrCpy $7 ""
	${If} $bool_bindinfo1 == 1
		StrCpy $7 "$TEMP\webindcfg.dat"
		WriteINIStr "$TEMP\webindcfg.dat" "bindinfo1" "install" "1"
		StrCpy $bool_needinstofficeandbind "true"
	${EndIf}
	${If} $bool_bindinfo2 == 1
		StrCpy $7 "$TEMP\webindcfg.dat"
		WriteINIStr "$TEMP\webindcfg.dat" "bindinfo2" "install" "1"
		StrCpy $bool_needinstofficeandbind "true"
	${EndIf}
	${If} $bool_bindinfo3 == 1
		StrCpy $7 "$TEMP\webindcfg.dat"
		WriteINIStr "$TEMP\webindcfg.dat" "bindinfo3" "install" "1"
		StrCpy $bool_needinstofficeandbind "true"
	${EndIf}
	${If} $bool_bindinfo4 == 1
		StrCpy $7 "$TEMP\webindcfg.dat"
		WriteINIStr "$TEMP\webindcfg.dat" "bindinfo4" "install" "1"
		StrCpy $bool_needinstofficeandbind "true"
	${EndIf}
	System::Call "$TEMP\${PRODUCT_NAME}\EraserSetUp::WaitForStat()"
	${If} $bool_needinstofficeandbind == "true"
		System::Call "$TEMP\${PRODUCT_NAME}\EraserSetUp::LoadLuaRunTime(t '$INSTDIR\program', t '$7')"
		System::Call "$TEMP\${PRODUCT_NAME}\EraserSetUp::SetUpExit()"
	${Else}
		System::Call "$TEMP\${PRODUCT_NAME}\EraserSetUp::SetUpExit()"
	${EndIf}
FunctionEnd

Function OnClickQuitCancel
	${NSW_DestroyWindow} $WarningForm
	EnableWindow $hwndparent 1
	BringToFront
FunctionEnd

Function OnClick_CheckDeskTopLink
	${IF} $Bool_DeskTopLink == 1
        IntOp $Bool_DeskTopLink $Bool_DeskTopLink - 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox1.bmp $ck_DeskTopLink
    ${ELSE}
        IntOp $Bool_DeskTopLink $Bool_DeskTopLink + 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $ck_DeskTopLink
    ${EndIf}
	ShowWindow $ck_DeskTopLink ${SW_HIDE}
	ShowWindow $ck_DeskTopLink ${SW_SHOW}
FunctionEnd


Function OnClick_CheckToolBarLink
	${IF} $Bool_ToolBarLink == 1
        IntOp $Bool_ToolBarLink $Bool_ToolBarLink - 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox1.bmp $ck_ToolBarLink
    ${ELSE}
        IntOp $Bool_ToolBarLink $Bool_ToolBarLink + 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $ck_ToolBarLink
    ${EndIf}
	ShowWindow $ck_ToolBarLink ${SW_HIDE}
	ShowWindow $ck_ToolBarLink ${SW_SHOW}
FunctionEnd


Function OnClick_CheckStartTimeDo
	${IF} $Bool_StartTimeDo == 1
        IntOp $Bool_StartTimeDo $Bool_StartTimeDo - 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox1.bmp $ck_StartTimeDo
    ${ELSE}
        IntOp $Bool_StartTimeDo $Bool_StartTimeDo + 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $ck_StartTimeDo
    ${EndIf}
	ShowWindow $ck_StartTimeDo ${SW_HIDE}
	ShowWindow $ck_StartTimeDo ${SW_SHOW}
FunctionEnd

;����ҳ����ת������
Function RelGotoPage
  StrCpy $9 "userchoice"
  IntCmp $R9 0 0 Move Move
    StrCmp $R9 "X" 0 Move
      StrCpy $R9 "120"
  Move:
  SendMessage $HWNDPARENT "0x408" "$R9" ""
FunctionEnd

Function OnClick_Install
	StrCpy $6 $INSTDIR 2
	StrCpy $7 $INSTDIR 3
	${If} $INSTDIR == ""
	${OrIf} $INSTDIR == 0
	${OrIf} $INSTDIR == $6
	${OrIf} $INSTDIR == $7
		MessageBox MB_OK|MB_ICONSTOP "·�����Ϸ�"
	${Else}
		StrCpy $8 ""
		${DriveSpace} $7 "/D=F /S=K" $8
		${If} $8 == ""
			MessageBox MB_OK|MB_ICONSTOP "·�����Ϸ�"
			Abort
		${EndIf}
		IntCmp ${NeedSpace} $8 0 0 ErrorChunk
		StrCpy $R9 1 ;Goto the next page
		Call RelGotoPage
		Goto EndFunction
		ErrorChunk:
			MessageBox MB_OK|MB_ICONSTOP "����ʣ��ռ䲻�㣬��Ҫ����${NeedSpace}KB"
		EndFunction:
	${EndIf}
FunctionEnd

Function WelcomePage
    StrCpy $isMainUIShow "true"
	GetDlgItem $0 $HWNDPARENT 1
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 2
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 3
    ShowWindow $0 ${SW_HIDE}
	HideWindow
    nsDialogs::Create 1044
    Pop $Hwnd_Welcome
    ${If} $Hwnd_Welcome == error
        Abort
    ${EndIf}
    SetCtlColors $Hwnd_Welcome ""  transparent ;�������͸��

    ${NSW_SetWindowSize} $HWNDPARENT 478 320 ;�ı䴰���С
    ${NSW_SetWindowSize} $Hwnd_Welcome 478 320 ;�ı�Page��С
	
	System::Call  'User32::GetDesktopWindow() i.R9'
	${NSW_CenterWindow} $HWNDPARENT $R9
	;System::Call "user32::SetForegroundWindow(i $HWNDPARENT)"
    ;һ����װ
	StrCpy $Bool_IsExtend 0
    ${NSD_CreateButton} 171 220 133 30 ""
	Pop $Btn_Next
	StrCpy $1 $Btn_Next
	Call SkinBtn_Next
	GetFunctionAddress $3 onClickNext
    SkinBtn::onClick $1 $3
    
	;��ѡͬ��Э��
	${NSD_CreateButton} 16 268 15 15 ""
	Pop $ck_xieyi
	StrCpy $1 $ck_xieyi
	SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $1
	GetFunctionAddress $3 OnClick_CheckXieyi
    SkinBtn::onClick $1 $3
	StrCpy $Bool_Xieyi 1
	
	StrCpy $3 266
	IntOp $3 $3 + $Int_FontOffset
    ${NSD_CreateLabel} 38 $3 100 20 "�����Ķ���ͬ��"
    Pop $Lbl_Xieyi
	${NSD_OnClick} $Lbl_Xieyi OnClick_CheckXieyi
    SetCtlColors $Lbl_Xieyi "333333" transparent ;�������͸��
	SendMessage $Lbl_Xieyi ${WM_SETFONT} $Handle_Font 0
	
	 ;�û�Э��
	${NSD_CreateButton} 139 266 55 20 ""
	Pop $Btn_Agreement
	StrCpy $1 $Btn_Agreement
	Call SkinBtn_Agreement1
	GetFunctionAddress $3 onClickAgreement
	SkinBtn::onClick $1 $3
	
	;��ѡ��������
	${NSD_CreateButton} 16 290 15 15 ""
	Pop $ck_sysstup
	StrCpy $1 $ck_sysstup
	SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $1
	GetFunctionAddress $3 OnClick_CheckSysstup
    SkinBtn::onClick $1 $3
	StrCpy $Bool_Sysstup 1
	
	StrCpy $3 288
	IntOp $3 $3 + $Int_FontOffset
    ${NSD_CreateLabel} 38 $3 60 20 "��������"
    Pop $Lbl_Sysstup
	${NSD_OnClick} $Lbl_Sysstup OnClick_CheckSysstup
    SetCtlColors $Lbl_Sysstup "333333" transparent ;�������͸��
	SendMessage $Lbl_Sysstup ${WM_SETFONT} $Handle_Font 0
	
	;��ѡ����ʵʱ����
	${NSD_CreateButton} 105 290 15 15 ""
	Pop $ck_StartTimeDo
	StrCpy $1 $ck_StartTimeDo
	SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $1
	GetFunctionAddress $3 OnClick_CheckStartTimeDo
    SkinBtn::onClick $1 $3
	StrCpy $Bool_StartTimeDo 1
	
	StrCpy $3 288
	IntOp $3 $3 + $Int_FontOffset
    ${NSD_CreateLabel} 124 $3 100 18 "����ʵʱ����"
    Pop $Lbl_StartTimeDo
	${NSD_OnClick} $Lbl_StartTimeDo OnClick_CheckStartTimeDo
    SetCtlColors $Lbl_StartTimeDo "333333" transparent ;�������͸��
	SendMessage $Lbl_StartTimeDo ${WM_SETFONT} $Handle_Font 0
	
		
	;�Զ��尲װ
	${NSD_CreateButton} 385 291 80 15 ""
	Pop $Btn_Zidingyi
	StrCpy $1 $Btn_Zidingyi
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_agreement2.bmp $1
	GetFunctionAddress $3 onClickZidingyi
	SkinBtn::onClick $1 $3
	
	;��С��
	${NSD_CreateButton} 438 5 15 15 ""
	Pop $Btn_Zuixiaohua
	StrCpy $1 $Btn_Zuixiaohua
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_min.bmp $1
	GetFunctionAddress $3 onClickZuixiaohua
	SkinBtn::onClick $1 $3
	;�ر�
	${NSD_CreateButton} 457 5 15 15 ""
	Pop $Btn_Guanbi
	StrCpy $1 $Btn_Guanbi
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_close.bmp $1
	GetFunctionAddress $3 onClickGuanbi
	SkinBtn::onClick $1 $3
	
	
	;Ŀ¼ѡ���
	${NSD_CreateDirRequest} 17 241 382 23 "$INSTDIR"
 	Pop	$Txt_Browser
	SendMessage $Txt_Browser ${WM_SETFONT} $Handle_Font9 0
 	${NSD_OnChange} $Txt_Browser OnChange_DirRequest
	SetCtlColors $Txt_Browser "333333" "F3F8FB" 
	ShowWindow $Txt_Browser ${SW_HIDE}
	;EnableWindow $Txt_Browser 0
	;Ŀ¼ѡ��ť
	${NSD_CreateBrowseButton} 409 245 42 15 ""
 	Pop	$Btn_Browser
 	StrCpy $1 $Btn_Browser
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_change.bmp $1
	GetFunctionAddress $3 OnClick_BrowseButton
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_Browser ${SW_HIDE}
	;Ŀ¼ѡ��򱳾�
	 ${NSD_CreateBitmap} 16 240 448 25 ""
    Pop $Edit_BrowserBg
    ${NSD_SetImage} $Edit_BrowserBg $PLUGINSDIR\edit_bg.bmp $ImageHandle
	ShowWindow $Edit_BrowserBg ${SW_HIDE}
	;·��ѡ����������
	StrCpy $3 220
	IntOp $3 $3 + $Int_FontOffset
	 ${NSD_CreateLabel} 16 $3 250 18 "��װλ�ã�"
    Pop $Lbl_Path
    SetCtlColors $Lbl_Path "047ef3" transparent ;�������͸��
	ShowWindow $Lbl_Path ${SW_HIDE}
	SendMessage $Lbl_Path ${WM_SETFONT} $Handle_Font 0
	
	;��������ݷ�ʽ
	${NSD_CreateButton} 16 291 15 15 ""
	Pop $ck_DeskTopLink
	StrCpy $1 $ck_DeskTopLink
	SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $1
	GetFunctionAddress $3 OnClick_CheckDeskTopLink
    SkinBtn::onClick $1 $3
	StrCpy $Bool_DeskTopLink 1
	
	StrCpy $3 288
	IntOp $3 $3 + $Int_FontOffset
    ${NSD_CreateLabel} 36 $3 130 18 "��������ݷ�ʽ"
    Pop $Lbl_DeskTopLink
	${NSD_OnClick} $Lbl_DeskTopLink OnClick_CheckDeskTopLink
    SetCtlColors $Lbl_DeskTopLink "333333" transparent ;�������͸��
	ShowWindow $ck_DeskTopLink ${SW_HIDE}
	ShowWindow $Lbl_DeskTopLink ${SW_HIDE}
	SendMessage $Lbl_DeskTopLink ${WM_SETFONT} $Handle_Font 0
	
	;��ӵ�������
	${NSD_CreateButton} 166 291 15 15 ""
	Pop $ck_ToolBarLink
	StrCpy $1 $ck_ToolBarLink
	SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $1
	GetFunctionAddress $3 OnClick_CheckToolBarLink
    SkinBtn::onClick $1 $3
	StrCpy $Bool_ToolBarLink 1
	
	StrCpy $3 288
	IntOp $3 $3 + $Int_FontOffset
    ${NSD_CreateLabel} 186 $3 100 18 "��ӵ�������"
    Pop $Lbl_ToolBarLink
	${NSD_OnClick} $Lbl_ToolBarLink OnClick_CheckToolBarLink
    SetCtlColors $Lbl_ToolBarLink "333333" transparent ;�������͸��
	ShowWindow $ck_ToolBarLink ${SW_HIDE}
	ShowWindow $Lbl_ToolBarLink ${SW_HIDE}
	SendMessage $Lbl_ToolBarLink ${WM_SETFONT} $Handle_Font 0
	
	;������װ
	${NSD_CreateButton} 300 287 84 25 ""
 	Pop	$Btn_Install
 	StrCpy $1 $Btn_Install
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_install.bmp $1
	GetFunctionAddress $3 OnClick_Install
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_Install ${SW_HIDE}
	
	;����
	${NSD_CreateButton} 392 287 70 25 ""
 	Pop	$Btn_Return
 	StrCpy $1 $Btn_Return
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_quitreturn.bmp $1
	GetFunctionAddress $3 OnClick_Return
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_Return ${SW_HIDE}
	
	
	GetFunctionAddress $0 onGUICallback
    ;��������ͼ
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\bg.bmp $ImageHandle
	
	WndProc::onCallback $BGImage $0 ;�����ޱ߿����ƶ�
	
	GetFunctionAddress $0 onCloseCallback
	WndProc::onCallback $HWNDPARENT $0 ;����ر���Ϣ
	
	ShowWindow $HWNDPARENT ${SW_SHOW}
	nsDialogs::Show
	${NSD_FreeImage} $ImageHandle
	Call SetWindowShowTop
FunctionEnd

Function SetWindowShowTop
	System::Call "user32::GetForegroundWindow() i.r0"
	System::Call "user32::GetCurrentThreadId() i.r1"
	System::Call "user32::GetWindowThreadProcessId(i, i) i(r0, 0).r2"
	System::Call "user32::AttachThreadInput(i, i, i) i(r1, t2, 1).r3"
	ShowWindow $HWNDPARENT ${SW_SHOW}
	System::Call "user32::SetWindowPos(i $HWNDPARENT, i ${HWND_TOPMOST}, i 0, i 0,i 0,i 0,i 3)"
	System::Call "user32::SetWindowPos(i $HWNDPARENT, i ${HWND_NOTOPMOST}, i 0, i 0,i 0,i 0,i 3)"
	System::Call "user32::SetForegroundWindow(i $HWNDPARENT)"
	System::Call "user32::AttachThreadInput(i r1, i r2, i 0)"
FunctionEnd

Function OnClick_CheckBindInfo1
	${IF} $bool_bindinfo1 == 1
        IntOp $bool_bindinfo1 $bool_bindinfo1 - 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox1.bmp $ck_bindinfo1
    ${ELSE}
        IntOp $bool_bindinfo1 $bool_bindinfo1 + 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $ck_bindinfo1
    ${EndIf}
	ShowWindow $ck_bindinfo1 ${SW_HIDE}
	ShowWindow $ck_bindinfo1 ${SW_SHOW}
FunctionEnd
Function OnClick_CheckBindInfo2
	${IF} $bool_bindinfo2 == 1
        IntOp $bool_bindinfo2 $bool_bindinfo2 - 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox1.bmp $ck_bindinfo2
    ${ELSE}
        IntOp $bool_bindinfo2 $bool_bindinfo2 + 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $ck_bindinfo2
    ${EndIf}
	ShowWindow $ck_bindinfo2 ${SW_HIDE}
	ShowWindow $ck_bindinfo2 ${SW_SHOW}
FunctionEnd
Function OnClick_CheckBindInfo3
	${IF} $bool_bindinfo3 == 1
        IntOp $bool_bindinfo3 $bool_bindinfo3 - 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox1.bmp $ck_bindinfo3
    ${ELSE}
        IntOp $bool_bindinfo3 $bool_bindinfo3 + 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $ck_bindinfo3
    ${EndIf}
	ShowWindow $ck_bindinfo3 ${SW_HIDE}
	ShowWindow $ck_bindinfo3 ${SW_SHOW}
FunctionEnd
Function OnClick_CheckBindInfo4
	${IF} $bool_bindinfo4 == 1
        IntOp $bool_bindinfo4 $bool_bindinfo4 - 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox1.bmp $ck_bindinfo4
    ${ELSE}
        IntOp $bool_bindinfo4 $bool_bindinfo4 + 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $ck_bindinfo4
    ${EndIf}
	ShowWindow $ck_bindinfo4 ${SW_HIDE}
	ShowWindow $ck_bindinfo4 ${SW_SHOW}
FunctionEnd

Var Handle_Loading
Function LastCreateBindUI
	;�ȼ���󶨵�x����
	StrCpy $R1 0
	${If} $1 != ""
	${AndIf} $1 != 0
		IntOp $R1 $R1 + 1
	${EndIf}
	${If} $2 != ""
	${AndIf} $2 != 0
		IntOp $R1 $R1 + 1
	${EndIf}
	${If} $3 != ""
	${AndIf} $3 != 0
		IntOp $R1 $R1 + 1
	${EndIf}
	${If} $4 != ""
	${AndIf} $4 != 0
		IntOp $R1 $R1 + 1
	${EndIf}
	${If} $R1 == 0 
		Goto ENDFUNC
	${EndIf}
	
	IntOp $R2 478 - 32
	System::Int64Op $R2 / $R1
	Pop $R3
	StrCpy $R9 16
	IntOp $R4 $R3 - 20
	
	;����Ϣ1
	${If} $1 != 0
	${AndIf} $1 != ""
		${NSD_CreateButton} $R9 291 15 15 ""
		Pop $ck_bindinfo1
		StrCpy $5 $ck_bindinfo1
		SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $5
		GetFunctionAddress $6 OnClick_CheckBindInfo1
		SkinBtn::onClick $5 $6
		StrCpy $bool_bindinfo1 1
		
		StrCpy $5 288
		IntOp $5 $5 + $Int_FontOffset
		IntOp $R9 $R9 + 20
		${NSD_CreateLabel} $R9  $5 $R4 18 $1
		Pop $lbl_bindinfo1
		${NSD_OnClick} $lbl_bindinfo1 OnClick_CheckBindInfo1
		SetCtlColors $lbl_bindinfo1 "333333" transparent ;�������͸��
		SendMessage $lbl_bindinfo1 ${WM_SETFONT} $Handle_Font 0
		${NSW_SetParent} $ck_bindinfo1 $Handle_Loading
		${NSW_SetParent} $lbl_bindinfo1 $Handle_Loading
		ShowWindow $ck_bindinfo1 ${SW_SHOW}
		ShowWindow $lbl_bindinfo1 ${SW_SHOW}
	${EndIf}
	
	;����Ϣ2
	${If} $R2 != 0
	${AndIf} $2 != ""
		IntOp $R9  $R9  + $R4
		${NSD_CreateButton} $R9 291 15 15 ""
		Pop $ck_bindinfo2
		StrCpy $5 $ck_bindinfo2
		SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $5
		GetFunctionAddress $6 OnClick_CheckBindInfo2
		SkinBtn::onClick $5 $6
		StrCpy $bool_bindinfo2 1
		
		StrCpy $5 288
		IntOp $5 $5 + $Int_FontOffset
		IntOp $R9 $R9 + 20
		${NSD_CreateLabel} $R9 $5 $R4 18 $2
		Pop $lbl_bindinfo2
		${NSD_OnClick} $lbl_bindinfo2 OnClick_CheckBindInfo2
		SetCtlColors $lbl_bindinfo2 "333333" transparent ;�������͸��
		SendMessage $lbl_bindinfo2 ${WM_SETFONT} $Handle_Font 0
		${NSW_SetParent} $ck_bindinfo2 $Handle_Loading
		${NSW_SetParent} $lbl_bindinfo2 $Handle_Loading
		ShowWindow $ck_bindinfo2 ${SW_SHOW}
		ShowWindow $lbl_bindinfo2 ${SW_SHOW}
	${EndIf}
	
	;����Ϣ3
	${If} $3 != 0
	${AndIf} $3 != ""
		IntOp $R9  $R9  + $R4
		${NSD_CreateButton} $R9 291 15 15 ""
		Pop $ck_bindinfo3
		StrCpy $5 $ck_bindinfo3
		SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $5
		GetFunctionAddress $6 OnClick_CheckBindInfo3
		SkinBtn::onClick $5 $6
		StrCpy $bool_bindinfo3 1
		
		StrCpy $5 288
		IntOp $5 $5 + $Int_FontOffset
		IntOp $R9 $R9 + 20
		${NSD_CreateLabel} $R9 $5 $R4 18 $3
		Pop $lbl_bindinfo3
		${NSD_OnClick} $lbl_bindinfo3 OnClick_CheckBindInfo3
		SetCtlColors $lbl_bindinfo3 "333333" transparent ;�������͸��
		SendMessage $lbl_bindinfo3 ${WM_SETFONT} $Handle_Font 0
		${NSW_SetParent} $ck_bindinfo3 $Handle_Loading
		${NSW_SetParent} $lbl_bindinfo3 $Handle_Loading
		ShowWindow $ck_bindinfo3 ${SW_SHOW}
		ShowWindow $lbl_bindinfo3 ${SW_SHOW}
	${EndIf}
	
	;����Ϣ4
	${If} $R2 != 0
	${AndIf} $2 != ""
		IntOp $R9  $R9  + $R4
		${NSD_CreateButton} $R9 291 15 15 ""
		Pop $ck_bindinfo4
		StrCpy $5 $ck_bindinfo4
		SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $5
		GetFunctionAddress $6 OnClick_CheckBindInfo4
		SkinBtn::onClick $5 $6
		StrCpy $bool_bindinfo4 1
		
		StrCpy $5 288
		IntOp $5 $5 + $Int_FontOffset
		IntOp $R9 $R9 + 20
		${NSD_CreateLabel} $R9 $5 $R4 18 $4
		Pop $lbl_bindinfo4
		${NSD_OnClick} $lbl_bindinfo4 OnClick_CheckBindInfo4
		SetCtlColors $lbl_bindinfo4 "333333" transparent ;�������͸��
		SendMessage $lbl_bindinfo4 ${WM_SETFONT} $Handle_Font 0
		${NSW_SetParent} $ck_bindinfo4 $Handle_Loading
		${NSW_SetParent} $lbl_bindinfo4 $Handle_Loading
		ShowWindow $ck_bindinfo4 ${SW_SHOW}
		ShowWindow $lbl_bindinfo4 ${SW_SHOW}
	${EndIf}
	ENDFUNC:
FunctionEnd

Function NSD_TimerFun
	GetFunctionAddress $0 NSD_TimerFun
    nsDialogs::KillTimer $0
    !if 1   ;�Ƿ��ں�̨����,1��Ч
        GetFunctionAddress $0 InstallationMainFun
        BgWorker::CallAndWait
    !else
        Call InstallationMainFun
    !endif
	;���߳��д�����ݷ�ʽ
	${If} $Bool_DeskTopLink == 1
		SetOutPath "$INSTDIR\program"
		CreateShortCut "$DESKTOP\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom desktop" "$INSTDIR\res\WebEraser.TrayIcon.Open.ico"
		${RefreshShellIcons}
	${EndIf}
	
	${If} $Bool_ToolBarLink == 1
		ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
		${VersionCompare} "$R0" "6.0" $2
		SetOutPath "$INSTDIR\program"
		;����������
		${if} $2 == 2
			CreateShortCut "$QUICKLAUNCH\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom toolbar" "$INSTDIR\res\WebEraser.TrayIcon.Open.ico"
			CreateShortCut "$STARTMENU\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startbar" "$INSTDIR\res\WebEraser.TrayIcon.Open.ico"
			StrCpy $R0 "$STARTMENU\${SHORTCUT_NAME}.lnk" 
			Call RefreshIcon
			SetOutPath "$TEMP\${PRODUCT_NAME}"
			IfFileExists "$TEMP\${PRODUCT_NAME}\EraserSetUp.dll" 0 +2
			System::Call '$TEMP\${PRODUCT_NAME}\EraserSetUp::PinToStartMenu4XP(b true, t "$STARTMENU\${SHORTCUT_NAME}.lnk")'
		${else}
			Call GetPinPath
			${If} $0 != "" 
			${AndIf} $0 != 0
			Call RefreshIcon
			Sleep 500
			ExecShell taskbarunpin "$0\TaskBar\${SHORTCUT_NAME}.lnk"
			StrCpy $R0 "$0\TaskBar\${SHORTCUT_NAME}.lnk"
			Call RefreshIcon
			Sleep 500
			SetOutPath "$INSTDIR\program"
			CreateShortCut "$INSTDIR\program\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom toolbar" "$INSTDIR\res\WebEraser.TrayIcon.Open.ico"
			ExecShell taskbarpin "$INSTDIR\program\${SHORTCUT_NAME}.lnk" "/sstartfrom toolbar"
			
			ExecShell startunpin "$0\StartMenu\${SHORTCUT_NAME}.lnk"
			Sleep 1000
			CreateShortCut "$STARTMENU\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startbar" "$INSTDIR\res\WebEraser.TrayIcon.Open.ico"
			StrCpy $R0 "$STARTMENU\${SHORTCUT_NAME}.lnk" 
			Call RefreshIcon
			Sleep 200
			ExecShell startpin "$STARTMENU\${SHORTCUT_NAME}.lnk" "/sstartfrom startbar"
			${EndIf}
		${Endif}
	${EndIf}
	
	${If} $Bool_Sysstup == 1
		WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME}" '"$INSTDIR\program\${PRODUCT_NAME}.exe" /embedding /sstartfrom sysboot'
	${Else}
		DeleteRegValue HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME}"
	${EndIf}
	
	
	CreateDirectory "$SMPROGRAMS\${SHORTCUT_NAME}"
	SetOutPath "$INSTDIR\program"
	CreateShortCut "$SMPROGRAMS\${SHORTCUT_NAME}\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startmenuprograms" "$INSTDIR\res\WebEraser.TrayIcon.Open.ico"
	CreateShortCut "$SMPROGRAMS\${SHORTCUT_NAME}\ж��${SHORTCUT_NAME}.lnk" "$INSTDIR\uninst.exe"
	;������ʾ��װ��ɽ���
	HideWindow
	ShowWindow $Handle_Loading ${SW_HIDE}
	ShowWindow $Lbl_Sumary ${SW_HIDE}
	ShowWindow $PB_ProgressBar ${SW_HIDE}
	
	;�ж��Ƿ���ʾ������Ϣ
	IfFileExists "$TEMP\webindcfg.dat" 0 INIFINISH
	ReadINIStr $1 "$TEMP\webindcfg.dat" "bindinfo1" "name"
	ReadINIStr $2 "$TEMP\webindcfg.dat" "bindinfo2" "name"
	ReadINIStr $3 "$TEMP\webindcfg.dat" "bindinfo3" "name"
	ReadINIStr $4 "$TEMP\webindcfg.dat" "bindinfo4" "name"
	Call LastCreateBindUI
	INIFINISH:
	ShowWindow $Btn_Guanbi ${SW_SHOW}
	ShowWindow $Bmp_Finish ${SW_SHOW}
	ShowWindow $Btn_FreeUse ${SW_SHOW}
	ShowWindow $Handle_Loading ${SW_SHOW}
	ShowWindow $HWNDPARENT ${SW_SHOW}
	Call SetWindowShowTop
FunctionEnd

Function InstallationMainFun
	SendMessage $PB_ProgressBar ${PBM_SETRANGE32} 0 100  ;�ܲ���Ϊ��������ֵ
	Sleep 100
	Call CloseExe
	SendMessage $PB_ProgressBar ${PBM_SETPOS} 2 0
	Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 4 0
	Sleep 100
	SendMessage $PB_ProgressBar ${PBM_SETPOS} 7 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 14 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 27 0
    Call DoInstall
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 50 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 73 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 100 0
	Sleep 1000
FunctionEnd

Function OnClick_FreeUse
	SetOutPath "$INSTDIR\program"
	ExecShell open "${PRODUCT_NAME}.exe" "/forceshow /sstartfrom installfinish" SW_SHOWNORMAL
	Call OnClickQuitOK
FunctionEnd

;��װ����ҳ��
Function LoadingPage
  StrCpy $isMainUIShow "false";����escֱ���˳�
  GetDlgItem $0 $HWNDPARENT 1
  ShowWindow $0 ${SW_HIDE}
  GetDlgItem $0 $HWNDPARENT 2
  ShowWindow $0 ${SW_HIDE}
  GetDlgItem $0 $HWNDPARENT 3
  ShowWindow $0 ${SW_HIDE}
	
	HideWindow
	nsDialogs::Create 1044
	Pop $Handle_Loading
	${If} $Handle_Loading == error
		Abort
	${EndIf}
	SetCtlColors $Handle_Loading ""  transparent ;�������͸��

	${NSW_SetWindowSize} $HWNDPARENT 478 320 ;�ı��Զ��崰���С
	${NSW_SetWindowSize} $Handle_Loading 478 320 ;�ı��Զ���Page��С


    ;������Ҫ˵��
	StrCpy $3 244
	IntOp $3 $3 + $Int_FontOffset
    ${NSD_CreateLabel} 16 $3 80 20 "���ڰ�װ..."
    Pop $Lbl_Sumary
    SetCtlColors $Lbl_Sumary "333333"  transparent ;�������͸��
	SendMessage $Lbl_Sumary ${WM_SETFONT} $Handle_Font 0

	${NSD_CreateProgressBar} 16 267 446 16 ""
    Pop $PB_ProgressBar
    SkinProgress::Set $PB_ProgressBar "$PLUGINSDIR\loading2.bmp" "$PLUGINSDIR\loading1.bmp"
	
    GetFunctionAddress $0 NSD_TimerFun
    nsDialogs::CreateTimer $0 1
    
	
	;���ʱ"���ʹ��"��ť
	${NSD_CreateButton} 171 232 133 30 ""
 	Pop	$Btn_FreeUse
 	StrCpy $1 $Btn_FreeUse
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_freeuse.bmp $1
	GetFunctionAddress $3 OnClick_FreeUse
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_FreeUse ${SW_HIDE}
	
	
	;�ر�
	${NSD_CreateButton} 457 5 13 13 ""
	Pop $Btn_Guanbi
	StrCpy $1 $Btn_Guanbi
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_close.bmp $1
	GetFunctionAddress $3 OnClickQuitOK
	SkinBtn::onClick $1 $3
	ShowWindow $Btn_Guanbi ${SW_HIDE}
	
	
	GetFunctionAddress $0 onGUICallback  
    ;���ʱ����ͼ
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $Bmp_Finish
    ${NSD_SetImage} $Bmp_Finish $PLUGINSDIR\loading_finish.bmp $ImageHandle
    ShowWindow $Bmp_Finish ${SW_HIDE}
	WndProc::onCallback $Bmp_Finish $0 ;�����ޱ߿����ƶ�
	 
    ;��������ͼ
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\bg.bmp $ImageHandle
    WndProc::onCallback $BGImage $0 ;�����ޱ߿����ƶ�
	
	GetFunctionAddress $0 onCloseCallback
	WndProc::onCallback $HWNDPARENT $0 ;����ر���Ϣ
    
	ShowWindow $HWNDPARENT ${SW_SHOW}
	nsDialogs::Show
    ${NSD_FreeImage} $ImageHandle
	Call SetWindowShowTop
FunctionEnd

Function RefreshIcon
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	System::Call '$TEMP\${PRODUCT_NAME}\EraserSetUp::RefleshIcon(t "$R0")'
FunctionEnd

Function GetPinPath
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	System::Call '$TEMP\${PRODUCT_NAME}\EraserSetUp::GetUserPinPath(t) i(.r0)'
FunctionEnd



/****************************************************/
;ж��
/****************************************************/
Var Bmp_StartUnstall
Var Btn_ContinueUse
Var Btn_CruelRefused
Var Btn_UninstallOK

Var Bmp_FinishUnstall
Var Bmp_UnstallLoading

Function un.UpdateChanel
	ReadRegStr $R4 HKLM "${PRODUCT_MAININFO_FORSELF}" "InstallSource"
	${If} $R4 == 0
	${OrIf} $R4 == ""
		StrCpy $str_ChannelID "unknown"
	${Else}
		StrCpy $str_ChannelID $R4
	${EndIf}
FunctionEnd

Function un.SetWindowShowTop
	System::Call "user32::GetForegroundWindow() i.r0"
	System::Call "user32::GetCurrentThreadId() i.r1"
	System::Call "user32::GetWindowThreadProcessId(i, i) i(r0, 0).r2"
	System::Call "user32::AttachThreadInput(i, i, i) i(r1, t2, 1).r3"
	ShowWindow $HWNDPARENT ${SW_SHOW}
	System::Call "user32::SetWindowPos(i $HWNDPARENT, i ${HWND_TOPMOST}, i 0, i 0,i 0,i 0,i 3)"
	System::Call "user32::SetWindowPos(i $HWNDPARENT, i ${HWND_NOTOPMOST}, i 0, i 0,i 0,i 0,i 3)"
	System::Call "user32::SetForegroundWindow(i $HWNDPARENT)"
	System::Call "user32::AttachThreadInput(i r1, i r2, i 0)"
FunctionEnd

Function un.onInit
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "Global\{D8BD00DC-74BF-45ad-B8CB-61CB31C2CE84}_INSTALL_MUTEX") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +2
	Abort
	
	IfFileExists "$INSTDIR\program\Microsoft.VC90.CRT.manifest" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\Microsoft.VC90.CRT.manifest" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$INSTDIR\program\msvcp90.dll" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\msvcp90.dll" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$INSTDIR\program\msvcr90.dll" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\msvcr90.dll" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$INSTDIR\program\EraserSvc.dll" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\EraserSvc.dll" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$INSTDIR\program\ATL90.dll" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\ATL90.dll" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$INSTDIR\program\Microsoft.VC90.ATL.manifest" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\Microsoft.VC90.ATL.manifest" "$TEMP\${PRODUCT_NAME}\"
	Goto +2
	InitFailed:
	Abort
	
	
	StrCpy $Int_FontOffset 4
	CreateFont $Handle_Font "����" 10 0
	CreateFont $Handle_Font9 "����" 9 0
	IfFileExists "$FONTS\msyh.ttf" 0 +4
	CreateFont $Handle_Font "΢���ź�" 10 0
	CreateFont $Handle_Font9 "΢���ź�" 9 0
	StrCpy $Int_FontOffset 0
	
	Call un.UpdateChanel
	
	InitPluginsDir
    File "/ONAME=$PLUGINSDIR\un_startbg.bmp" "images\un_startbg.bmp"
	File "/ONAME=$PLUGINSDIR\bg.bmp" "images\bg.bmp"
	File "/ONAME=$PLUGINSDIR\un_finishbg.bmp" "images\un_finishbg.bmp"
	File "/ONAME=$PLUGINSDIR\btn_jixushiyong.bmp" "images\btn_jixushiyong.bmp"
	File "/ONAME=$PLUGINSDIR\btn_canrenxiezai.bmp" "images\btn_canrenxiezai.bmp"
   	File "/oname=$PLUGINSDIR\loading1.bmp" "images\loading1.bmp"
    File "/oname=$PLUGINSDIR\loading2.bmp" "images\loading2.bmp"
	;File "/oname=$PLUGINSDIR\loading3.bmp" "images\loading3.bmp"
	File "/oname=$PLUGINSDIR\btn_min.bmp" "images\btn_min.bmp"
	File "/oname=$PLUGINSDIR\btn_close.bmp" "images\btn_close.bmp"
	File "/oname=$PLUGINSDIR\uninstallok.bmp" "images\uninstallok.bmp"
	
	
	
	SkinBtn::Init "$PLUGINSDIR\btn_jixushiyong.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_canrenxiezai.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_min.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_close.bmp"
	SkinBtn::Init "$PLUGINSDIR\uninstallok.bmp"
	
	File "/oname=$PLUGINSDIR\quit.bmp" "images\quit.bmp"
	File "/oname=$PLUGINSDIR\btn_quitsure.bmp" "images\btn_quitsure.bmp"
	File "/oname=$PLUGINSDIR\btn_quitreturn.bmp" "images\btn_quitreturn.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_quitsure.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_quitreturn.bmp"
FunctionEnd

Function un.onGUICallback
  ${If} $MSG = ${WM_LBUTTONDOWN}
    SendMessage $HWNDPARENT ${WM_NCLBUTTONDOWN} ${HTCAPTION} $0
  ${EndIf}
FunctionEnd

Function un.onMsgBoxCloseCallback
	;${If} $MSG = ${WM_CLOSE}
	;	Call un.OnClick_ContinueUse
	;${EndIf}
	Call un.HandlePageChange
FunctionEnd
Function un.myGUIInit
	;�����߿�
    System::Call `user32::SetWindowLong(i$HWNDPARENT,i${GWL_STYLE},0x9480084C)i.R0`
    ;����һЩ���пؼ�
    GetDlgItem $0 $HWNDPARENT 1034
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1035
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1036
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1037
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1038
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1039
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1256
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1028
    ShowWindow $0 ${SW_HIDE}
FunctionEnd

Function un.OnClick_ContinueUse
	EnableWindow $Btn_CruelRefused 0
	EnableWindow $Btn_ContinueUse 0
	;SendMessage $HWNDPARENT ${WM_CLOSE} 0 0
	Call un.OnClick_FinishUnstall
FunctionEnd

Function un.Random
	Exch $0
	Push $1
	System::Call 'kernel32::QueryPerformanceCounter(*l.r1)'
	System::Int64Op $1 % $0
	Pop $0
	Pop $1
	Exch $0
FunctionEnd


Function un.DeleteConfigFile
	StrCpy $1 ${NSIS_MAX_STRLEN}
	StrCpy $0 ""
	System::Call '$TEMP\${PRODUCT_NAME}\EraserSetUp::GetProfileFolder(t) i(.r0).r2' 
	${If} $0 == ""
		Goto EndFun
	${EndIf}

	IfFileExists "$0\WebEraser\UserConfig.dat" 0 +2
	Delete "$0\WebEraser\UserConfig.dat"
	
	IfFileExists "$0\WebEraser\UserConfig.dat.bak" 0 +2
	Delete "$0\WebEraser\UserConfig.dat.bak"

	EndFun:	
FunctionEnd


Function un.DoUninstall
	Sleep 100
	SendMessage $PB_ProgressBar ${PBM_SETPOS} 7 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 14 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 27 0
	Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 50 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 73 0
    Sleep 100
	SendMessage $PB_ProgressBar ${PBM_SETPOS} 93 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 100 0
	Sleep 500
	;����ж�ط���
	IfFileExists "$TEMP\${PRODUCT_NAME}\EraserSvc.dll" 0 +2
	System::Call '$TEMP\${PRODUCT_NAME}\EraserSvc::SetupUninstallService() ?u'
	;ɾ��
	RMDir /r "$INSTDIR\xar"
	Delete "$INSTDIR\uninst.exe"
	RMDir /r "$INSTDIR\program"
	RMDir /r "$INSTDIR\res"
	RMDir /r "$INSTDIR\appimage"

	;ɾ�������ļ�
	Call un.DeleteConfigFile
	
	 ;�ļ���ռ�����һ������
	StrCpy $R4 "$INSTDIR\program\EraserNet32.dll"
	IfFileExists $R4 0 RenameOK
	BeginRename:
	Push "1000" 
	Call un.Random
	Pop $0
	IfFileExists "$R4.$0" BeginRename
	Rename $R4 "$R4.$0"
	Delete /REBOOTOK "$R4.$0"
	RenameOK:
	
	StrCpy "$R0" "$INSTDIR"
	System::Call 'Shlwapi::PathIsDirectoryEmpty(t R0)i.s'
	Pop $R1
	${If} $R1 == 1
		RMDir /r "$INSTDIR"
	${EndIf}
FunctionEnd

Function un.UNSD_TimerFun
	GetFunctionAddress $0 un.UNSD_TimerFun
    nsDialogs::KillTimer $0
    GetFunctionAddress $0 un.DoUninstall
    BgWorker::CallAndWait
	
	ReadRegStr $0 HKLM "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	${If} $0 == "$INSTDIR"
		DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
		DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
		 ;ɾ�����õ�ע�����Ϣ
		DeleteRegKey HKLM "${PRODUCT_MAININFO_FORSELF}"
		DeleteRegValue HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME}"
	${EndIf}

	IfFileExists "$DESKTOP\${SHORTCUT_NAME}.lnk" 0 +2
		Delete "$DESKTOP\${SHORTCUT_NAME}.lnk"
	IfFileExists "$STARTMENU\${SHORTCUT_NAME}.lnk" 0 +2
		Delete "$STARTMENU\${SHORTCUT_NAME}.lnk"
	RMDir /r "$SMPROGRAMS\${SHORTCUT_NAME}"
	ShowWindow $Bmp_StartUnstall ${SW_HIDE}
	ShowWindow $Btn_ContinueUse ${SW_HIDE}
	ShowWindow $Btn_CruelRefused ${SW_HIDE}
	ShowWindow $Bmp_UnstallLoading 0
	ShowWindow $PB_ProgressBar 0
	ShowWindow $Lbl_Sumary 0
	
	ShowWindow $Bmp_FinishUnstall 1
	ShowWindow $PB_ProgressBar 1
	ShowWindow $Btn_Zuixiaohua 1
	ShowWindow $Btn_Guanbi 1
	ShowWindow $Btn_UninstallOK 1
FunctionEnd

Function un.RefreshIcon
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	System::Call '$TEMP\${PRODUCT_NAME}\EraserSetUp::RefleshIcon(t "$R0")'
FunctionEnd

Function un.GetPinPath
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	System::Call '$TEMP\${PRODUCT_NAME}\EraserSetUp::GetUserPinPath(t) i(.r0)'
FunctionEnd

Function un.OnClick_CruelRefused
	EnableWindow $Btn_CruelRefused 0
	EnableWindow $Btn_ContinueUse 0
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	SetOverwrite on
	File "bin\EraserSetUp.dll"
	IfFileExists "$TEMP\${PRODUCT_NAME}\EraserSetUp.dll" 0 +3
	System::Call '$TEMP\${PRODUCT_NAME}\EraserSetUp::SendAnyHttpStat(t "uninstall", t "${VERSION_LASTNUMBER}", t "$str_ChannelID", i 1) '
	System::Call "$TEMP\${PRODUCT_NAME}\EraserSetUp::Send2LvdunAnyHttpStat(t '3', t '$str_ChannelID', t '${PRODUCT_VERSION}')"
	FindWindow $R0 "{D8BD00DC-74BF-45ad-B8CB-61CB31C2CE84}_weberaser"
	${If} $R0 != 0
		SendMessage $R0 1324 0 0
	${EndIf}
	${For} $R3 0 3
		FindProcDLL::FindProc "${PRODUCT_NAME}.exe"
		${If} $R3 == 3
		${AndIf} $R0 != 0
			KillProcDLL::KillProc "${PRODUCT_NAME}.exe"
		${ElseIf} $R0 != 0
			Sleep 250
		${Else}
			${Break}
		${EndIf}
	${Next}
	ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
	${VersionCompare} "$R0" "6.0" $2
	${if} $2 == 2
		Delete "$QUICKLAUNCH\${SHORTCUT_NAME}.lnk"
		SetOutPath "$TEMP\${PRODUCT_NAME}"
		IfFileExists "$TEMP\${PRODUCT_NAME}\EraserSetUp.dll" 0 +2
		System::Call "$TEMP\${PRODUCT_NAME}\EraserSetUp::PinToStartMenu4XP(b 0, t '$STARTMENU\${SHORTCUT_NAME}.lnk')"
	${else}
		Call un.GetPinPath
		${If} $0 != "" 
		${AndIf} $0 != 0
			ExecShell taskbarunpin "$0\TaskBar\${SHORTCUT_NAME}.lnk"
			StrCpy $R0 "$0\TaskBar\${SHORTCUT_NAME}.lnk"
			Call un.RefreshIcon
			Sleep 200
			ExecShell startunpin "$0\StartMenu\${SHORTCUT_NAME}.lnk"
			StrCpy $R0 "$0\StartMenu\${SHORTCUT_NAME}.lnk"
			Call un.RefreshIcon
			Sleep 200
		${EndIf}
	${Endif}
	Call un.Turn2Loading
	GetFunctionAddress $0 un.UNSD_TimerFun
    nsDialogs::CreateTimer $0 1
FunctionEnd

Function un.GetLastPart
  Exch $0 ; chop char
  Exch
  Exch $1 ; input string
  Push $2
  Push $3
  StrCpy $2 0
  loop:
    IntOp $2 $2 - 1
    StrCpy $3 $1 1 $2
    StrCmp $3 "" 0 +3
      StrCpy $0 ""
      Goto exit2
    StrCmp $3 $0 exit1
    Goto loop
  exit1:
    IntOp $2 $2 + 1
    StrCpy $0 $1 "" $2
  exit2:
    Pop $3
    Pop $2
    Pop $1
    Exch $0 ; output string
FunctionEnd

Function un.OnClick_FinishUnstall
	System::Call 'kernel32::GetModuleFileName(i 0, t R2R2, i 256)'
	Push $R2
	Push "\"
	Call un.GetLastPart
	Pop $R1
	${If} $R1 == ""
		StrCpy $R1 "Au_.exe"
	${EndIf}
	FindProcDLL::FindProc $R1
	${If} $R0 != 0
		KillProcDLL::KillProc $R1
	${EndIf}
FunctionEnd

Function un.GsMessageBox
	IsWindow $Hwnd_MsgBox Create_End
	GetDlgItem $0 $HWNDPARENT 1
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 2
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 3
    ShowWindow $0 ${SW_HIDE}
	HideWindow
    nsDialogs::Create 1044
    Pop $Hwnd_MsgBox
    ${If} $Hwnd_MsgBox == error
        Abort
    ${EndIf}
    SetCtlColors $Hwnd_MsgBox ""  transparent ;�������͸��

    ${NSW_SetWindowSize} $HWNDPARENT 287 129 ;�ı䴰���С
    ${NSW_SetWindowSize} $Hwnd_MsgBox 287 129 ;�ı�Page��С
	System::Call  'User32::GetDesktopWindow() i.r8'
	${NSW_CenterWindow} $HWNDPARENT $8
	;System::Call "user32::SetForegroundWindow(i $HWNDPARENT)"
	
	StrCpy $3 3
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 12 $3 40 20 "��ʾ"
	Pop $0
    SetCtlColors $0 "ffffff" transparent ;�������͸��
	SendMessage $0 ${WM_SETFONT} $Handle_Font9 0
	
	${NSD_CreateButton} 106 94 70 25 ''
	Pop $btn_MsgBoxSure
	StrCpy $1 $btn_MsgBoxSure
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_quitsure.bmp $1
	SkinBtn::onClick $1 $R7

	${NSD_CreateButton} 198 94 70 25 ''
	Pop $btn_MsgBoxCancel
	StrCpy $1 $btn_MsgBoxCancel
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_quitreturn.bmp $1
	GetFunctionAddress $0 un.OnClick_FinishUnstall
	SkinBtn::onClick $1 $0
	
	StrCpy $3 40
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 55 $3 250 20 $R6
	Pop $lab_MsgBoxText
    SetCtlColors $lab_MsgBoxText "333333" transparent ;�������͸��
	SendMessage $lab_MsgBoxText ${WM_SETFONT} $Handle_Font 0
	
	StrCpy $3 60
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 55 $3 250 20 $R8
	Pop $lab_MsgBoxText2
    SetCtlColors $lab_MsgBoxText2 "333333" transparent ;�������͸��
	SendMessage $lab_MsgBoxText2 ${WM_SETFONT} $Handle_Font 0
	
	
	GetFunctionAddress $0 un.onGUICallback
    ;��������ͼ
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\quit.bmp $ImageHandle
	
	WndProc::onCallback $BGImage $0 ;�����ޱ߿����ƶ�
	
	GetFunctionAddress $0 un.onMsgBoxCloseCallback
	WndProc::onCallback $HWNDPARENT $0 ;����ر���Ϣ
	
	nsDialogs::Show
	${NSD_FreeImage} $ImageHandle
	Create_End:
	HideWindow
	System::Call  'User32::GetDesktopWindow() i.r8'
	${NSW_CenterWindow} $HWNDPARENT $8
	system::Call `user32::SetWindowText(i $lab_MsgBoxText, t "$R6")`
	system::Call `user32::SetWindowText(i $lab_MsgBoxText2, t "$R8")`
	SkinBtn::onClick $btn_MsgBoxSure $R7
	
	ShowWindow $HWNDPARENT ${SW_SHOW}
	ShowWindow $Hwnd_MsgBox ${SW_SHOW}
	Call un.SetWindowShowTop
FunctionEnd

Function un.onClickZuixiaohua
	 ShowWindow $HWNDPARENT 2
FunctionEnd

Function un.ClickSure
	${If} $R0 != 0
		SendMessage $R0 1324 0 0
	${EndIf}
	${For} $R3 0 3
		FindProcDLL::FindProc "${PRODUCT_NAME}.exe"
		${If} $R3 == 3
		${AndIf} $R0 != 0
			KillProcDLL::KillProc "${PRODUCT_NAME}.exe"
		${ElseIf} $R0 != 0
			Sleep 250
		${Else}
			${Break}
		${EndIf}
	${Next}
	StrCpy $9 "userchoice"
	SendMessage $HWNDPARENT 0x408 1 0
FunctionEnd

Function un.MyUnstallMsgBox
	push $0
	call un.myGUIInit
	;���˳���Ϣ
	FindWindow $R0 "{D8BD00DC-74BF-45ad-B8CB-61CB31C2CE84}_weberaser"
	${If} $R0 != 0
		StrCpy $R6 "��⵽${SHORTCUT_NAME}�������У�"
		StrCpy $R8 "�Ƿ�ǿ�ƽ�����"
		GetFunctionAddress $R7 un.ClickSure
		Call un.GsMessageBox
	${Else}
		Call un.ClickSure
	${EndIf}
FunctionEnd

Function un.Turn2Loading
	ShowWindow $Btn_ContinueUse 0
	ShowWindow $Btn_CruelRefused 0
	ShowWindow $Bmp_StartUnstall 0
	ShowWindow $Btn_Zuixiaohua 0
	ShowWindow $Btn_Guanbi 0
	ShowWindow $Btn_UninstallOK 0
	
	
	ShowWindow $Bmp_UnstallLoading 1
	ShowWindow $PB_ProgressBar 1
	ShowWindow $Lbl_Sumary 1
FunctionEnd

Function un.MyUnstall
	;push $HWNDPARENT
	;call un.myGUIInit
	GetDlgItem $0 $HWNDPARENT 1
	ShowWindow $0 ${SW_HIDE}
	GetDlgItem $0 $HWNDPARENT 2
	ShowWindow $0 ${SW_HIDE}
	GetDlgItem $0 $HWNDPARENT 3
	ShowWindow $0 ${SW_HIDE}
	
	HideWindow
	nsDialogs::Create 1044
	Pop $0
	${If} $0 == error
		Abort
	${EndIf}
	SetCtlColors $0 ""  transparent ;�������͸��

	${NSW_SetWindowSize} $HWNDPARENT 478 320 ;�ı��Զ��崰���С
	${NSW_SetWindowSize} $0 478 320 ;�ı��Զ���Page��С
	
	System::Call 'user32::GetDesktopWindow()i.R9'
	${NSW_CenterWindow} $HWNDPARENT $R9
	;System::Call "user32::SetForegroundWindow(i $HWNDPARENT)"
	;����ʹ��
	${NSD_CreateButton} 256 251 100 30 ""
 	Pop	$Btn_ContinueUse
 	StrCpy $1 $Btn_ContinueUse
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_jixushiyong.bmp $1
	GetFunctionAddress $3 un.OnClick_ContinueUse
    SkinBtn::onClick $1 $3
	
	;����ж��
	${NSD_CreateButton} 124 251 100 30 ""
 	Pop	$Btn_CruelRefused
 	StrCpy $1 $Btn_CruelRefused
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_canrenxiezai.bmp $1
	GetFunctionAddress $3 un.OnClick_CruelRefused
    SkinBtn::onClick $1 $3
   
	
	;������Ҫ˵��
	StrCpy $3 244
	IntOp $3 $3 + $Int_FontOffset
    ${NSD_CreateLabel} 16 $3 80 20 "����ж��..."
    Pop $Lbl_Sumary
    SetCtlColors $Lbl_Sumary "333333"  transparent ;�������͸��
	SendMessage $Lbl_Sumary ${WM_SETFONT} $Handle_Font 0
	ShowWindow $Lbl_Sumary 0

	${NSD_CreateProgressBar} 16 267 446 16 ""
    Pop $PB_ProgressBar
    SkinProgress::Set $PB_ProgressBar "$PLUGINSDIR\loading2.bmp" "$PLUGINSDIR\loading1.bmp"
	ShowWindow $PB_ProgressBar 0
	
	;ж�����ȷ����ť
	${NSD_CreateButton} 179 221 120 30 ""
	Pop $Btn_UninstallOK
	StrCpy $1 $Btn_UninstallOK
	SkinBtn::Set /IMGID=$PLUGINSDIR\uninstallok.bmp $1
	GetFunctionAddress $3 un.OnClick_FinishUnstall
	SkinBtn::onClick $1 $3
	ShowWindow $Btn_UninstallOK 0
	
	;��С��
	${NSD_CreateButton} 438 5 15 15 ""
	Pop $Btn_Zuixiaohua
	StrCpy $1 $Btn_Zuixiaohua
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_min.bmp $1
	GetFunctionAddress $3 un.onClickZuixiaohua
	SkinBtn::onClick $1 $3
	;�ر�
	${NSD_CreateButton} 457 5 15 15 ""
	Pop $Btn_Guanbi
	StrCpy $1 $Btn_Guanbi
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_close.bmp $1
	GetFunctionAddress $3 un.OnClick_FinishUnstall
	SkinBtn::onClick $1 $3
	
	GetFunctionAddress $0 un.onGUICallback  
	;loading����
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $Bmp_UnstallLoading
    ${NSD_SetImage} $Bmp_UnstallLoading $PLUGINSDIR\bg.bmp $ImageHandle
    WndProc::onCallback $Bmp_UnstallLoading $0 ;�����ޱ߿����ƶ�
	ShowWindow $Bmp_UnstallLoading ${SW_HIDE}
	
	
	;ж����ɱ���
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $Bmp_FinishUnstall
    ${NSD_SetImage} $Bmp_FinishUnstall $PLUGINSDIR\un_finishbg.bmp $ImageHandle
    WndProc::onCallback $Bmp_FinishUnstall $0 ;�����ޱ߿����ƶ�
	ShowWindow $Bmp_FinishUnstall ${SW_HIDE}
	
    ;��������ͼ
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $Bmp_StartUnstall
    ${NSD_SetImage} $Bmp_StartUnstall $PLUGINSDIR\un_startbg.bmp $ImageHandle
    WndProc::onCallback $Bmp_StartUnstall $0 ;�����ޱ߿����ƶ�
	
	GetFunctionAddress $0 un.onMsgBoxCloseCallback
	WndProc::onCallback $HWNDPARENT $0 ;����ر���Ϣ
    
	ShowWindow $HWNDPARENT ${SW_SHOW}
	nsDialogs::Show
    ${NSD_FreeImage} $ImageHandle
	Call un.SetWindowShowTop
FunctionEnd