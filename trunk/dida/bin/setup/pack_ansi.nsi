Var MSG
Var Dialog
Var BGImage  
Var BGImage2      
Var ImageHandle
Var Btn_Next
Var Btn_Agreement
;��ӭҳ�洰�ھ��
Var Hwnd_Welcome
Var Bool_IsExtend
Var ck_xieyi
Var Bool_Xieyi
Var ck_sysstup
Var Bool_Sysstup
Var Btn_Zidingyi
Var Btn_Zuixiaohua
Var Btn_Guanbi
Var Txt_Browser
Var Btn_Browser
Var Edit_BrowserBg

Var ck_DeskTopLink
Var Bool_DeskTopLink

Var ck_ToolBarLink
Var Bool_ToolBarLink


Var Btn_Install
Var Btn_Return

Var WarningForm
Var Handle_Font
Var Int_FontOffset

;����������
Var Lbl_Sumary
Var Bmp_Finish
Var Btn_FreeUse

;��̬��ȡ������
Var str_ChannelID
;---------------------------ȫ�ֱ���ű�Ԥ����ĳ���-----------------------------------------------------
; MUI Ԥ���峣��
!define MUI_ABORTWARNING
!define MUI_PAGE_FUNCTION_ABORTWARNING onClickGuanbi
;��װͼ���·������
!define MUI_ICON "images\fav.ico"
;ж��ͼ���·������
!define MUI_UNICON "images\unis.ico"

!define INSTALL_CHANNELID "0001"

!define PRODUCT_NAME "DIDA"
!define SHORTCUT_NAME "�δ�����"
!define PRODUCT_VERSION "1.0.0.1"
!define VERSION_LASTNUMBER 1
!define NeedSpace 10240
!define EM_OUTFILE_NAME "DIDASetup_${INSTALL_CHANNELID}.exe"

!define EM_BrandingText "${PRODUCT_NAME}${PRODUCT_VERSION}"
!define PRODUCT_PUBLISHER "DIDA"
!define PRODUCT_WEB_SITE "http://www.lvdun123.com/"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_NAME}.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_MAININFO_FORSELF "Software\${PRODUCT_NAME}"

;ж�ذ����أ��벻Ҫ���״򿪣�
; !define SWITCH_CREATE_UNINSTALL_PAKAGE 1

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
RequestExecutionLevel highest

VIProductVersion ${PRODUCT_VERSION}
VIAddVersionKey /LANG=2052 "ProductName" "${SHORTCUT_NAME}"
VIAddVersionKey /LANG=2052 "Comments" ""
VIAddVersionKey /LANG=2052 "CompanyName" "�ɶ������ǻ����Ƽ����޹�˾"
;VIAddVersionKey /LANG=2052 "LegalTrademarks" "DIDA"
VIAddVersionKey /LANG=2052 "LegalCopyright" "Copyright (c) 2014-2016 �ɶ������ǻ����Ƽ����޹�˾"
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
InstallDir "$PROGRAMFILES\DIDA"
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
	${For} $R3 0 6
		FindProcDLL::FindProc "${PRODUCT_NAME}.exe"
		${If} $R3 == 6
		${AndIf} $R0 != 0
			MessageBox MB_OK|MB_ICONSTOP "�ܱ�Ǹ������������֮��Ĵ���,�볢�����²���������������뵽�ٷ���վѰ�����"
			Abort
		${ElseIf} $R0 != 0
			KillProcDLL::KillProc "${PRODUCT_NAME}.exe"
			Sleep 250
		${Else}
			${Break}
		${EndIf}
	${Next}
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	System::Call "$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::FindClockWindow() i.r0"
	MessageBox MB_OK|MB_ICONSTOP "$0"
	${If} $0 != 0
		SendMessage $0 1344 0 100
	${EndIf}
	Sleep 250
FunctionEnd

Var Bool_IsUpdate
Function DoInstall
  ;�ͷ����õ�publicĿ¼
  SetOutPath "$TEMP\${PRODUCT_NAME}"
  StrCpy $1 ${NSIS_MAX_STRLEN}
  StrCpy $0 ""
  System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::GetProfileFolder(t) i(.r0).r2' 
  ${If} $0 == ""
	HideWindow
	MessageBox MB_ICONINFORMATION|MB_OK "�ܱ�Ǹ������������֮��Ĵ���,�볢�����°�װ������������뵽�ٷ���վѰ�����"
	Call OnClickQuitOK
  ${EndIf}
  IfFileExists "$0" +4 0
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "�ܱ�Ǹ������������֮��Ĵ���,�볢�����°�װ������������뵽�ٷ���վѰ�����"
  Call OnClickQuitOK
  
  SetOutPath "$0"
  SetOverwrite on
  File /r "input_config\*.*"
  ;��ɾ��װ
  RMDir /r "$INSTDIR\program"
  RMDir /r "$INSTDIR\xar"
  RMDir /r "$INSTDIR\res"
  ;�ļ���ռ�����һ������
  StrCpy $R4 "$INSTDIR\program\DiDaCalendar64.dll"
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
  StrCpy $R4 "$INSTDIR\program\DiDaCalendar.dll"
  IfFileExists $R4 0 RenameOK2
  Delete $R4
  IfFileExists $R4 0 RenameOK2
  BeginRename2:
  Push "1000" 
  Call Random
  Pop $0
  IfFileExists "$R4.$0" BeginRename2
  Rename $R4 "$R4.$0"
  RenameOK2:
  
  ;�ͷ��������ļ�����װĿ¼
  SetOutPath "$INSTDIR\program"
  SetOverwrite on
  File /r "input_main\program\*.*"
  SetOutPath "$INSTDIR\xar"
  SetOverwrite on
  File /r "input_main\xar\*.*"
  SetOutPath "$INSTDIR\res"
  SetOverwrite off
  File /r "input_main\res\*.*"
  ;WriteUninstaller "$INSTDIR\uninst.exe"
  
  SetOutPath "$INSTDIR"
  SetOverwrite on
  File "uninst\uninst.exe"
  
  
  StrCpy $Bool_IsUpdate 0 
  ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "Path"
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
	System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::SendAnyHttpStat(t "install", t "${VERSION_LASTNUMBER}", t "$R0", i 1) '
	System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::SendAnyHttpStat(t "installmethod", t "${VERSION_LASTNUMBER}", t "$R3", i 1) '
	;System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::Send2LvdunAnyHttpStat(t "install", t "$R0")'
  ${Else}
	System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::SendAnyHttpStat(t "update", t "${VERSION_LASTNUMBER}", t "$R0", i 1)'
	System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::SendAnyHttpStat(t "updatemethod", t "${VERSION_LASTNUMBER}", t "$R3", i 1)'
  ${EndIf}  
 ;д�����õ�ע�����Ϣ
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallSource" $str_ChannelID
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir" "$INSTDIR"
  System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::GetTime(*l) i(.r0).r1'
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallTimes" "$0"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "Path" "$INSTDIR\program\${PRODUCT_NAME}.exe"
  
  ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "PeerId"
  ${If} $0 == ""
	System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::GetPeerID(t) i(.r0).r1'
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "PeerId" "$0"
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

Function CmdSilentInstall
	${GetParameters} $R1
	${GetOptions} $R1 "/s"  $R0
	IfErrors FunctionReturn 0
	SetSilent silent
	;SetOutPath "$TEMP\${PRODUCT_NAME}"
	;SetOverwrite on
	;File "bin\DIDASetUpHelper.dll"
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	${If} $0 != ""
		StrCpy $INSTDIR "$0"
	${EndIf}
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "Path"
	IfFileExists $0 0 StartInstall
		;System::Call 'DIDASetUpHelper::GetFileVersionString(t $0, t) i(r0, .r1).r2'
		${GetFileVersion} $0 $1
		${VersionCompare} $1 ${PRODUCT_VERSION} $2
		${If} $2 == "2" ;�Ѱ�װ�İ汾���ڸð汾
			Goto StartInstall
		${Else}
			${GetParameters} $R1
			${GetOptions} $R1 "/write"  $R0
			IfErrors 0 +2
			Abort
			Goto StartInstall
		${EndIf}
	StartInstall:
	
	;���˳���Ϣ
	Call CloseExe
	Call DoInstall
	;Sleep 2000
	SetOutPath "$INSTDIR\program"
	CreateDirectory "$SMPROGRAMS\${SHORTCUT_NAME}"
	CreateShortCut "$SMPROGRAMS\${SHORTCUT_NAME}\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startmenuprograms" "$INSTDIR\res\shortcut.ico"
	CreateShortCut "$SMPROGRAMS\${SHORTCUT_NAME}\ж��${SHORTCUT_NAME}.lnk" "$INSTDIR\uninst.exe"
	;������������
	ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
	${VersionCompare} "$R0" "6.0" $2
	${if} $2 == 2
		CreateShortCut "$QUICKLAUNCH\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom toolbar" "$INSTDIR\res\shortcut.ico"
		CreateShortCut "$STARTMENU\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startbar" "$INSTDIR\res\shortcut.ico"
		SetOutPath "$TEMP\${PRODUCT_NAME}"
		IfFileExists "$TEMP\${PRODUCT_NAME}\DIDASetUpHelper.dll" 0 +2
		System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::PinToStartMenu4XP(b true, t "$STARTMENU\${SHORTCUT_NAME}.lnk")'
	${else}
		Call GetPinPath
		${If} $0 != "" 
		${AndIf} $0 != 0
			ExecShell taskbarunpin "$0\TaskBar\${SHORTCUT_NAME}.lnk"
			StrCpy $R0 "$0\TaskBar\${SHORTCUT_NAME}.lnk"
			Call RefreshIcon
			Sleep 500
			SetOutPath "$INSTDIR\program"
			CreateShortCut "$INSTDIR\program\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom toolbar" "$INSTDIR\res\shortcut.ico"
			ExecShell taskbarpin "$INSTDIR\program\${SHORTCUT_NAME}.lnk" "/sstartfrom toolbar"
			
			ExecShell startunpin "$0\StartMenu\${SHORTCUT_NAME}.lnk"
			Sleep 1000
			CreateShortCut "$STARTMENU\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startbar" "$INSTDIR\res\shortcut.ico"
			StrCpy $R0 "$STARTMENU\${SHORTCUT_NAME}.lnk" 
			Call RefreshIcon
			Sleep 200
			ExecShell startpin "$STARTMENU\${SHORTCUT_NAME}.lnk" "/sstartfrom startbar"
		${EndIf}
	${Endif}
	
	SetOutPath "$INSTDIR\program"
	;�����ݷ�ʽ
	CreateShortCut "$DESKTOP\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom desktop" "$INSTDIR\res\shortcut.ico"
	${RefreshShellIcons}
	StrCpy $R0 "$DESKTOP\${SHORTCUT_NAME}.lnk"
	Call RefreshIcon
	;��Ĭ��װ���������п�������
	${GetParameters} $R1
	${GetOptions} $R1 "/setboot"  $R0
	IfErrors +2 0
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME}" '"$INSTDIR\program\${PRODUCT_NAME}.exe" /embedding /ssartfrom sysboot'
	System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::GetTime(*l) i(.r0).r1'
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "ShowIntroduce" "$0"
	${GetParameters} $R1
	${GetOptions} $R1 "/run"  $R0
	IfErrors +7 0
	${If} $R0 == ""
	${OrIf} $R0 == 0
		StrCpy $R0 "/embedding"
	${EndIf}
	SetOutPath "$INSTDIR\program"
	ExecShell open "${PRODUCT_NAME}.exe" "$R0 /sstartfrom installfinish" SW_SHOWNORMAL
	System::Call "$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::SoftExit()"
	Abort
	FunctionReturn:
FunctionEnd

Function UnstallOnlyFile
	;ɾ��
	RMDir /r "$1\appimage"
	RMDir /r "$1\xar"
	Delete "$1\uninst.exe"
	RMDir /r "$1\program"
	RMDir /r "$1\res"
	
	 ;�ļ���ռ�����һ������
	StrCpy $R4 "$1\program\GsNet32.dll"
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
		IfFileExists "$TEMP\${PRODUCT_NAME}\DIDASetUpHelper.dll" 0 +2
		System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::PinToStartMenu4XP(b 0, t "$STARTMENU\${SHORTCUT_NAME}.lnk")'
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
	StrCpy $1 $INSTDIR
	Call UnstallOnlyFile
	
	;��ȡ������
	ReadRegStr $R4 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallSource"
	${If} $R4 != ""
	${AndIf} $R4 != 0
		StrCpy $str_ChannelID $R4
	${EndIF}
	
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	IfFileExists "$TEMP\${PRODUCT_NAME}\DIDASetUpHelper.dll" 0 +2
	System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::SendAnyHttpStat(t "uninstall", t "${VERSION_LASTNUMBER}", t "$str_ChannelID", i 1) '
	;System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::Send2LvdunAnyHttpStat(t "uninstall", t "$str_ChannelID")'
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	${If} $0 == "$INSTDIR"
		DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
		DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
		 ;ɾ�����õ�ע�����Ϣ
		DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}"
		DeleteRegValue ${PRODUCT_UNINST_ROOT_KEY} "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME}"
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
	ReadRegStr $R0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallSource"
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
		${WordReplace} "$R1" "DIDASetup_" "" "+" $R3
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
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "DIDASETUP_INSTALL_MUTEX") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +2
	Abort
	StrCpy $Int_FontOffset 4
	CreateFont $Handle_Font "����" 10 0
	IfFileExists "$FONTS\msyh.ttf" 0 +3
	CreateFont $Handle_Font "΢���ź�" 10 0
	StrCpy $Int_FontOffset 0
	
	Call UpdateChanel
	
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	SetOverwrite on
	File "bin\DIDASetUpHelper.dll"
	File "input_main\program\Microsoft.VC90.CRT.manifest"
	File "input_main\program\msvcp90.dll"
	File "input_main\program\msvcr90.dll"
	File "input_main\program\Microsoft.VC90.ATL.manifest"
	File "input_main\program\ATL90.dll"
	File "license\license.txt"
	Call CmdSilentInstall
	Call CmdUnstall
	
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	${If} $0 != ""
		StrCpy $INSTDIR "$0"
	${EndIf}
	InitPluginsDir
    File "/ONAME=$PLUGINSDIR\bg.bmp" "images\bg.bmp"
	File "/ONAME=$PLUGINSDIR\bg2.bmp" "images\bg2.bmp"
	 
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
	File "/oname=$PLUGINSDIR\btn_return.bmp" "images\btn_return.bmp"
	File "/oname=$PLUGINSDIR\quit.bmp" "images\quit.bmp"
	File "/oname=$PLUGINSDIR\btn_quitsure.bmp" "images\btn_quitsure.bmp"
	File "/oname=$PLUGINSDIR\btn_quitreturn.bmp" "images\btn_quitreturn.bmp"   	
   	File "/oname=$PLUGINSDIR\loading1.bmp" "images\loading1.bmp"
    File "/oname=$PLUGINSDIR\loading2.bmp" "images\loading2.bmp"
	File "/oname=$PLUGINSDIR\loading3.bmp" "images\loading3.bmp"
	File "/oname=$PLUGINSDIR\loading_head.bmp" "images\loading_head.bmp"
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
	SkinBtn::Init "$PLUGINSDIR\btn_return.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_quitsure.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_quitreturn.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_freeuse.bmp"
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

    ${NSW_SetWindowSize} $HWNDPARENT 254 121 ;�ı䴰���С
    ${NSW_SetWindowSize} $Hwnd_MsgBox 254 121 ;�ı�Page��С
	System::Call  'User32::GetDesktopWindow() i.r8'
	${NSW_CenterWindow} $HWNDPARENT $8
	;System::Call "user32::SetForegroundWindow(i $HWNDPARENT)"
	
	${NSD_CreateButton} 84 85 70 25 ''
	Pop $btn_MsgBoxSure
	StrCpy $1 $btn_MsgBoxSure
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_quitsure.bmp $1
	SkinBtn::onClick $1 $R7

	${NSD_CreateButton} 165 85 70 25 ''
	Pop $btn_MsgBoxCancel
	StrCpy $1 $btn_MsgBoxCancel
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_quitreturn.bmp $1
	GetFunctionAddress $0 OnClickQuitOK
	SkinBtn::onClick $1 $0
	
	StrCpy $3 33
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 65 $3 250 20 $R6
	Pop $lab_MsgBoxText
    SetCtlColors $lab_MsgBoxText "00A0F0" transparent ;�������͸��
	SendMessage $lab_MsgBoxText ${WM_SETFONT} $Handle_Font 0
	
	StrCpy $3 53
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 65 $3 250 20 $R8
	Pop $lab_MsgBoxText2
    SetCtlColors $lab_MsgBoxText2 "00A0F0" transparent ;�������͸��
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
	BringToFront
FunctionEnd

Function ClickSure2
	ShowWindow $Hwnd_MsgBox ${SW_HIDE}
	ShowWindow $HWNDPARENT ${SW_HIDE}
	${For} $R3 0 6
		FindProcDLL::FindProc "${PRODUCT_NAME}.exe"
		${If} $R3 == 6
		${AndIf} $R0 != 0
			MessageBox MB_OK|MB_ICONSTOP "�ܱ�Ǹ������������֮��Ĵ���,�볢�����²���������������뵽�ٷ���վѰ�����"
			Abort
		${ElseIf} $R0 != 0
			KillProcDLL::KillProc "${PRODUCT_NAME}.exe"
			Sleep 250
		${Else}
			${Break}
		${EndIf}
	${Next}
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	System::Call "$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::FindClockWindow() i.r0"
	${If} $0 != 0
		SendMessage $0 1344 0 100
	${EndIf}
	Sleep 250
	StrCpy $R9 1 ;Goto the next page
    Call RelGotoPage
FunctionEnd

Function ClickSure1
	ShowWindow $Hwnd_MsgBox ${SW_HIDE}
	ShowWindow $HWNDPARENT ${SW_HIDE}
	Sleep 100
	;���˳���Ϣ
	FindProcDLL::FindProc "${PRODUCT_NAME}.exe"
	${If} $R0 != 0
		StrCpy $R6 "���${PRODUCT_NAME}.exe�������У�"
		StrCpy $R8 "�Ƿ�ǿ�ƽ�����"
		GetFunctionAddress $R7 ClickSure2
		Call GsMessageBox
	${Else}
		Call ClickSure2
	${EndIf}
FunctionEnd

Function CheckMessageBox
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "Path"
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
		StrCpy $R1 "DIDA"
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
	;HideWindow
	ShowWindow $Btn_Next ${SW_HIDE}
	ShowWindow $Btn_Agreement ${SW_HIDE}
	ShowWindow $ck_xieyi ${SW_HIDE}
	ShowWindow $ck_sysstup ${SW_HIDE}
	ShowWindow $Btn_Zidingyi ${SW_HIDE}
	ShowWindow $BGImage ${SW_HIDE}
	
	ShowWindow $BGImage2 1
	ShowWindow $Edit_BrowserBg 1
	ShowWindow $Txt_Browser 1
	ShowWindow $Btn_Browser 1
	ShowWindow $ck_DeskTopLink 1
	ShowWindow $ck_ToolBarLink 1
	ShowWindow $Btn_Install 1
	ShowWindow $Btn_Return 1
	;ShowWindow $HWNDPARENT ${SW_SHOW}
	
	ShowWindow $Btn_Zuixiaohua 1
	ShowWindow $Btn_Guanbi 1
	BringToFront
FunctionEnd

Function OnClick_Return
	;HideWindow
	ShowWindow $BGImage 1
	ShowWindow $Btn_Next 1
	ShowWindow $Btn_Agreement 1
	ShowWindow $ck_xieyi 1
	ShowWindow $ck_sysstup 1
	ShowWindow $Btn_Zidingyi 1
	
	
	ShowWindow $Edit_BrowserBg ${SW_HIDE}
	ShowWindow $Txt_Browser ${SW_HIDE}
	ShowWindow $Btn_Browser ${SW_HIDE}
	ShowWindow $ck_DeskTopLink ${SW_HIDE}
	ShowWindow $ck_ToolBarLink ${SW_HIDE}
	ShowWindow $Btn_Install ${SW_HIDE}
	ShowWindow $Btn_Return ${SW_HIDE}
	ShowWindow $BGImage2 ${SW_HIDE}
	;ShowWindow $HWNDPARENT ${SW_SHOW}
	
	ShowWindow $Btn_Zuixiaohua 1
	ShowWindow $Btn_Guanbi 1
	BringToFront
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

	${NSW_SetWindowSize} $WarningForm 254 121
	EnableWindow $hwndparent 0
	System::Call `user32::SetWindowLong(i $WarningForm,i ${GWL_STYLE},0x9480084C)i.R0`
	${NSW_CreateButton} 84 85 71 26 ''
	Pop $R0
	StrCpy $1 $R0
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_quitsure.bmp $1
	${NSW_OnClick} $R0 OnClickQuitOK

	${NSW_CreateButton} 165 85 71 26 ''
	Pop $R0
	StrCpy $1 $R0
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_quitreturn.bmp $1
	${NSW_OnClick} $R0 OnClickQuitCancel

	StrCpy $3 33
	IntOp $3 $3 + $Int_FontOffset
	${NSW_CreateLabel} 65 $3 190 45 "��ȷ��Ҫ�˳��δ�������װ����"
	Pop $4
    SetCtlColors $4 "00A0F0" transparent ;�������͸��
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
	System::Call "$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::SoftExit()"
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

    ;һ����װ
	StrCpy $Bool_IsExtend 0
    ${NSD_CreateButton} 180 251 110 30 ""
	Pop $Btn_Next
	StrCpy $1 $Btn_Next
	Call SkinBtn_Next
	GetFunctionAddress $3 onClickNext
    SkinBtn::onClick $1 $3
    
	;��ѡͬ��Э��
	${NSD_CreateButton} 221 289 20 20 ""
	Pop $ck_xieyi
	StrCpy $1 $ck_xieyi
	SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $1
	GetFunctionAddress $3 OnClick_CheckXieyi
    SkinBtn::onClick $1 $3
	StrCpy $Bool_Xieyi 1

	
	;��ѡ��������
	${NSD_CreateButton} 386 268 20 20 ""
	Pop $ck_sysstup
	StrCpy $1 $ck_sysstup
	SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $1
	GetFunctionAddress $3 OnClick_CheckSysstup
    SkinBtn::onClick $1 $3
	StrCpy $Bool_Sysstup 1
	
	
    ;�û�Э��
	${NSD_CreateButton} 396 291 68 15 ""
	Pop $Btn_Agreement
	StrCpy $1 $Btn_Agreement
	Call SkinBtn_Agreement1
	GetFunctionAddress $3 onClickAgreement
	SkinBtn::onClick $1 $3
		
	;�Զ��尲װ
	${NSD_CreateButton} 32 291 80 20 ""
	Pop $Btn_Zidingyi
	StrCpy $1 $Btn_Zidingyi
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_agreement2.bmp $1
	GetFunctionAddress $3 onClickZidingyi
	SkinBtn::onClick $1 $3
	
	;��С��
	${NSD_CreateButton} 433 5 15 15 ""
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
	${NSD_CreateDirRequest} 39 249 353 23 "$INSTDIR"
 	Pop	$Txt_Browser
	SendMessage $Txt_Browser ${WM_SETFONT} $Handle_Font 0
	SetCtlColors $Txt_Browser "0"  "f3f8fb" ;������ɰ�ɫ
 	${NSD_OnChange} $Txt_Browser OnChange_DirRequest
	ShowWindow $Txt_Browser ${SW_HIDE}
	;Ŀ¼ѡ��ť
	${NSD_CreateBrowseButton} 395 250 45 20 ""
 	Pop	$Btn_Browser
 	StrCpy $1 $Btn_Browser
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_change.bmp $1
	GetFunctionAddress $3 OnClick_BrowseButton
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_Browser ${SW_HIDE}
	;Ŀ¼ѡ��򱳾�
	 ${NSD_CreateBitmap} 24 243 425 35 ""
    Pop $Edit_BrowserBg
    ${NSD_SetImage} $Edit_BrowserBg $PLUGINSDIR\edit_bg.bmp $ImageHandle
	ShowWindow $Edit_BrowserBg ${SW_HIDE}
	
	;��������ݷ�ʽ
	${NSD_CreateButton} 25 286 20 20 ""
	Pop $ck_DeskTopLink
	StrCpy $1 $ck_DeskTopLink
	SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $1
	GetFunctionAddress $3 OnClick_CheckDeskTopLink
    SkinBtn::onClick $1 $3
	StrCpy $Bool_DeskTopLink 1
	ShowWindow $ck_DeskTopLink ${SW_HIDE}
	
	;��ӵ�������
	${NSD_CreateButton} 169 286 20 20 ""
	Pop $ck_ToolBarLink
	StrCpy $1 $ck_ToolBarLink
	SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $1
	GetFunctionAddress $3 OnClick_CheckToolBarLink
    SkinBtn::onClick $1 $3
	StrCpy $Bool_ToolBarLink 1
	ShowWindow $ck_ToolBarLink ${SW_HIDE}
	
	;������װ
	${NSD_CreateButton} 304 281 80 30 ""
 	Pop	$Btn_Install
 	StrCpy $1 $Btn_Install
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_install.bmp $1
	GetFunctionAddress $3 OnClick_Install
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_Install ${SW_HIDE}
	
	;����
	${NSD_CreateButton} 388 280 66 30 ""
 	Pop	$Btn_Return
 	StrCpy $1 $Btn_Return
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_return.bmp $1
	GetFunctionAddress $3 OnClick_Return
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_Return ${SW_HIDE}
	
	
	GetFunctionAddress $0 onGUICallback
	
	;��������ͼ2
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage2
    ${NSD_SetImage} $BGImage2 $PLUGINSDIR\bg2.bmp $ImageHandle
	ShowWindow $BGImage2 ${SW_HIDE}
	WndProc::onCallback $BGImage2 $0 ;�����ޱ߿����ƶ�
	
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
	BringToFront
FunctionEnd

Var Handle_Loading
Var loadingbgWnd
Var loadingForground
Var loadingArrow
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
		CreateShortCut "$DESKTOP\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom desktop" "$INSTDIR\res\shortcut.ico"
		${RefreshShellIcons}
	${EndIf}
	
	
	${If} $Bool_ToolBarLink == 1
		ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
		${VersionCompare} "$R0" "6.0" $2
		SetOutPath "$INSTDIR\program"
		;����������
		${if} $2 == 2
			CreateShortCut "$QUICKLAUNCH\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom toolbar" "$INSTDIR\res\shortcut.ico"
			CreateShortCut "$STARTMENU\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startbar" "$INSTDIR\res\shortcut.ico"
			StrCpy $R0 "$STARTMENU\${SHORTCUT_NAME}.lnk" 
			Call RefreshIcon
			SetOutPath "$TEMP\${PRODUCT_NAME}"
			IfFileExists "$TEMP\${PRODUCT_NAME}\DIDASetUpHelper.dll" 0 +2
			System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::PinToStartMenu4XP(b true, t "$STARTMENU\${SHORTCUT_NAME}.lnk")'
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
			CreateShortCut "$INSTDIR\program\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom toolbar" "$INSTDIR\res\shortcut.ico"
			ExecShell taskbarpin "$INSTDIR\program\${SHORTCUT_NAME}.lnk" "/sstartfrom toolbar"
			
			ExecShell startunpin "$0\StartMenu\${SHORTCUT_NAME}.lnk"
			Sleep 1000
			CreateShortCut "$STARTMENU\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startbar" "$INSTDIR\res\shortcut.ico"
			StrCpy $R0 "$STARTMENU\${SHORTCUT_NAME}.lnk" 
			Call RefreshIcon
			Sleep 200
			ExecShell startpin "$STARTMENU\${SHORTCUT_NAME}.lnk" "/sstartfrom startbar"
			${EndIf}
		${Endif}
	${EndIf}
	
	${If} $Bool_Sysstup == 1
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME}" '"$INSTDIR\program\${PRODUCT_NAME}.exe" /embedding /ssartfrom sysboot'
	${Else}
		DeleteRegValue ${PRODUCT_UNINST_ROOT_KEY} "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME}"
	${EndIf}
	
	
	CreateDirectory "$SMPROGRAMS\${SHORTCUT_NAME}"
	SetOutPath "$INSTDIR\program"
	CreateShortCut "$SMPROGRAMS\${SHORTCUT_NAME}\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startmenuprograms" "$INSTDIR\res\shortcut.ico"
	CreateShortCut "$SMPROGRAMS\${SHORTCUT_NAME}\ж��${SHORTCUT_NAME}.lnk" "$INSTDIR\uninst.exe"

	;������ʾ��װ��ɽ���
	HideWindow
	ShowWindow $Handle_Loading ${SW_HIDE}
	ShowWindow $Lbl_Sumary ${SW_HIDE}
	ShowWindow $loadingForground ${SW_HIDE}
	ShowWindow $loadingbgWnd ${SW_HIDE}
	
	ShowWindow $Btn_Guanbi ${SW_SHOW}
	ShowWindow $Bmp_Finish ${SW_SHOW}
	ShowWindow $Btn_FreeUse ${SW_SHOW}
	ShowWindow $Handle_Loading ${SW_SHOW}
	ShowWindow $HWNDPARENT ${SW_SHOW}
	BringToFront
FunctionEnd

Function InstallationMainFun
	Sleep 100
	Call CloseExe
	${NSW_SetWindowSize} $loadingForground 2 14
	${NSW_SetWindowPos} $loadingArrow 19 268
	Sleep 100
    ${NSW_SetWindowSize} $loadingForground 5 14
	${NSW_SetWindowPos} $loadingArrow 22 268
	Sleep 100
	${NSW_SetWindowSize} $loadingForground 11 14
	${NSW_SetWindowPos} $loadingArrow 28 268
    Sleep 100
    ${NSW_SetWindowSize} $loadingForground 21 14
	${NSW_SetWindowPos} $loadingArrow 38 268
    Sleep 200
    ${NSW_SetWindowSize} $loadingForground 41 14
	${NSW_SetWindowPos} $loadingArrow 58 268
	 Sleep 200
   Call DoInstall
    ${NSW_SetWindowSize} $loadingForground 71 14
	${NSW_SetWindowPos} $loadingArrow 88 268
    Sleep 100
    ${NSW_SetWindowSize} $loadingForground 111 14
	${NSW_SetWindowPos} $loadingArrow 128 268
    Sleep 200
    ${NSW_SetWindowSize} $loadingForground 161 14
	${NSW_SetWindowPos} $loadingArrow 178 268
	 Sleep 200
    ${NSW_SetWindowSize} $loadingForground 221 14
	${NSW_SetWindowPos} $loadingArrow 238 268
	 Sleep 200
    ${NSW_SetWindowSize} $loadingForground 291 14
	${NSW_SetWindowPos} $loadingArrow 308 268
	 Sleep 2000
    ${NSW_SetWindowSize} $loadingForground 371 14
	${NSW_SetWindowPos} $loadingArrow 388 268
	 Sleep 3000
    ${NSW_SetWindowSize} $loadingForground 415 14
	${NSW_SetWindowPos} $loadingArrow 432 268
	 Sleep 3000
    ${NSW_SetWindowSize} $loadingForground 440 14
	ShowWindow $loadingArrow 0
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
	StrCpy $3 245
	IntOp $3 $3 + $Int_FontOffset
    ${NSD_CreateLabel} 20 $3 90 20 "���ڰ�װ..."
    Pop $Lbl_Sumary
    SetCtlColors $Lbl_Sumary "${TEXTCOLOR}"  transparent ;�������͸��
	SendMessage $Lbl_Sumary ${WM_SETFONT} $Handle_Font 0
    
	;����������
	${NSD_CreateBitmap} 17 267 440 16 ""
    Pop $loadingbgWnd
    ${NSD_SetImage} $loadingbgWnd $PLUGINSDIR\loading1.bmp $ImageHandle
	
	;������ǰ��
	${NSD_CreateBitmap} 17 268 440 14 ""
    Pop $loadingForground
    ${NSD_SetImage} $loadingForground $PLUGINSDIR\loading2.bmp $ImageHandle
	${NSW_SetWindowSize} $loadingForground 0 14
	
	;��������ͷ
	${NSD_CreateBitmap} 17 268 25 14 ""
    Pop $loadingArrow
    ${NSD_SetImage} $loadingArrow $PLUGINSDIR\loading3.bmp $ImageHandle
	${NSW_SetWindowPos} $loadingArrow 17 268
	
	GetFunctionAddress $0 NSD_TimerFun
    nsDialogs::CreateTimer $0 1
	
	;���ʱ"���ʹ��"��ť
	${NSD_CreateButton} 177 247 125 30 ""
 	Pop	$Btn_FreeUse
 	StrCpy $1 $Btn_FreeUse
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_freeuse.bmp $1
	GetFunctionAddress $3 OnClick_FreeUse
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_FreeUse ${SW_HIDE}
	
	
	;�ر�
	${NSD_CreateButton} 457 5 15 15 ""
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
    ${NSD_SetImage} $BGImage $PLUGINSDIR\loading_head.bmp $ImageHandle
    WndProc::onCallback $BGImage $0 ;�����ޱ߿����ƶ�
	
	GetFunctionAddress $0 onCloseCallback
	WndProc::onCallback $HWNDPARENT $0 ;����ر���Ϣ
    
	ShowWindow $HWNDPARENT ${SW_SHOW}
	nsDialogs::Show
    ${NSD_FreeImage} $ImageHandle
	BringToFront
FunctionEnd

Function RefreshIcon
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::RefleshIcon(t "$R0")'
FunctionEnd

Function GetPinPath
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::GetUserPinPath(t) i(.r0)'
FunctionEnd



/****************************************************/
;ж��
/****************************************************/
Var Bmp_StartUnstall
Var Btn_ContinueUse
Var Btn_CruelRefused

Var Bmp_FinishUnstall
Var Btn_FinishUnstall

Function un.UpdateChanel
	ReadRegStr $R4 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallSource"
	${If} $R4 == 0
	${OrIf} $R4 == ""
		StrCpy $str_ChannelID "unknown"
	${Else}
		StrCpy $str_ChannelID $R4
	${EndIf}
FunctionEnd

Function un.onInit
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "DIDASETUP_INSTALL_MUTEX") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +2
	Abort
	
	IfFileExists "$INSTDIR\program\Microsoft.VC90.CRT.manifest" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\Microsoft.VC90.CRT.manifest" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$INSTDIR\program\msvcp90.dll" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\msvcp90.dll" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$INSTDIR\program\msvcr90.dll" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\msvcr90.dll" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$INSTDIR\program\ATL90.dll" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\ATL90.dll" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$INSTDIR\program\Microsoft.VC90.ATL.manifest" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\Microsoft.VC90.ATL.manifest" "$TEMP\${PRODUCT_NAME}\"
	Goto +2
	InitFailed:
	Abort
	
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	SetOverwrite on
	File "bin\DIDASetUpHelper.dll"
	
	StrCpy $Int_FontOffset 4
	CreateFont $Handle_Font "����" 10 0
	IfFileExists "$FONTS\msyh.ttf" 0 +3
	CreateFont $Handle_Font "΢���ź�" 10 0
	StrCpy $Int_FontOffset 0
	
	Call un.UpdateChanel
	
	InitPluginsDir
    File `/ONAME=$PLUGINSDIR\un_startbg.bmp` `images\un_startbg.bmp`
	File `/ONAME=$PLUGINSDIR\un_finishbg.bmp` `images\un_finishbg.bmp`
	File `/ONAME=$PLUGINSDIR\btn_jixushiyong.bmp` `images\btn_jixushiyong.bmp`
	File `/ONAME=$PLUGINSDIR\btn_canrenxiezai.bmp` `images\btn_canrenxiezai.bmp`
	File `/ONAME=$PLUGINSDIR\btn_xiezaiwancheng.bmp` `images\btn_xiezaiwancheng.bmp`
	
	SkinBtn::Init "$PLUGINSDIR\btn_jixushiyong.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_canrenxiezai.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_xiezaiwancheng.bmp"
	
	File `/oname=$PLUGINSDIR\quit.bmp` `images\quit.bmp`
	File `/oname=$PLUGINSDIR\btn_quitsure.bmp` `images\btn_quitsure.bmp`
	File `/oname=$PLUGINSDIR\btn_quitreturn.bmp` `images\btn_quitreturn.bmp`
	SkinBtn::Init "$PLUGINSDIR\btn_quitsure.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_quitreturn.bmp"
FunctionEnd

Function un.onGUICallback
  ${If} $MSG = ${WM_LBUTTONDOWN}
    SendMessage $HWNDPARENT ${WM_NCLBUTTONDOWN} ${HTCAPTION} $0
  ${EndIf}
FunctionEnd

Function un.onMsgBoxCloseCallback
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


Function un.DoUninstall
	;ɾ��
	RMDir /r "$INSTDIR\xar"
	Delete "$INSTDIR\uninst.exe"
	RMDir /r "$INSTDIR\program"
	RMDir /r "$INSTDIR\res"
	
	 ;�ļ���ռ�����һ������
	StrCpy $R4 "$INSTDIR\program\DiDaCalendar64.dll"
	IfFileExists $R4 0 RenameOK
	BeginRename:
	Push "1000" 
	Call un.Random
	Pop $0
	IfFileExists "$R4.$0" BeginRename
	Rename $R4 "$R4.$0"
	Delete /REBOOTOK "$R4.$0"
	RenameOK:
	StrCpy $R4 "$INSTDIR\program\DiDaCalendar.dll"
	IfFileExists $R4 0 RenameOK2
	BeginRename2:
	Push "1000" 
	Call un.Random
	Pop $0
	IfFileExists "$R4.$0" BeginRename2
	Rename $R4 "$R4.$0"
	Delete /REBOOTOK "$R4.$0"
	RenameOK2:
	
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
	
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	${If} $0 == "$INSTDIR"
		DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
		DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
		 ;ɾ�����õ�ע�����Ϣ
		DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}"
		DeleteRegValue ${PRODUCT_UNINST_ROOT_KEY} "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME}"
	${EndIf}

	IfFileExists "$DESKTOP\${SHORTCUT_NAME}.lnk" 0 +2
		Delete "$DESKTOP\${SHORTCUT_NAME}.lnk"
	IfFileExists "$STARTMENU\${SHORTCUT_NAME}.lnk" 0 +2
		Delete "$STARTMENU\${SHORTCUT_NAME}.lnk"
	RMDir /r "$SMPROGRAMS\${SHORTCUT_NAME}"
	ShowWindow $Bmp_StartUnstall ${SW_HIDE}
	ShowWindow $Btn_ContinueUse ${SW_HIDE}
	ShowWindow $Btn_CruelRefused ${SW_HIDE}
	ShowWindow $Bmp_FinishUnstall 1
	ShowWindow $Btn_FinishUnstall 1
FunctionEnd

Function un.RefreshIcon
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::RefleshIcon(t "$R0")'
FunctionEnd

Function un.GetPinPath
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::GetUserPinPath(t) i(.r0)'
FunctionEnd

Function un.OnClick_CruelRefused
	EnableWindow $Btn_CruelRefused 0
	EnableWindow $Btn_ContinueUse 0
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	IfFileExists "$TEMP\${PRODUCT_NAME}\DIDASetUpHelper.dll" 0 +2
	System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::SendAnyHttpStat(t "uninstall", t "${VERSION_LASTNUMBER}", t "$str_ChannelID", i 1) '
	;System::Call '$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::Send2LvdunAnyHttpStat(t "uninstall", t "$str_ChannelID")'
	${For} $R3 0 6
		FindProcDLL::FindProc "${PRODUCT_NAME}.exe"
		${If} $R3 == 6
		${AndIf} $R0 != 0
			MessageBox MB_OK|MB_ICONSTOP "�ܱ�Ǹ������������֮��Ĵ���,�볢�����²���������������뵽�ٷ���վѰ�����"
			Abort
		${ElseIf} $R0 != 0
			KillProcDLL::KillProc "${PRODUCT_NAME}.exe"
			Sleep 250
		${Else}
			${Break}
		${EndIf}
	${Next}
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	System::Call "$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::FindClockWindow() i.r0"
	${If} $0 != 0
		SendMessage $0 1344 0 100
	${EndIf}
	Sleep 250
	
	ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
	${VersionCompare} "$R0" "6.0" $2
	${if} $2 == 2
		Delete "$QUICKLAUNCH\${SHORTCUT_NAME}.lnk"
		SetOutPath "$TEMP\${PRODUCT_NAME}"
		IfFileExists "$TEMP\${PRODUCT_NAME}\DIDASetUpHelper.dll" 0 +2
		System::Call "$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::PinToStartMenu4XP(b 0, t '$STARTMENU\${SHORTCUT_NAME}.lnk')"
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
	;SendMessage $HWNDPARENT ${WM_CLOSE} 0 0
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
    ${NSW_SetWindowSize} $HWNDPARENT 254 121 ;�ı䴰���С
    ${NSW_SetWindowSize} $Hwnd_MsgBox 254 121 ;�ı�Page��С
	System::Call  'User32::GetDesktopWindow() i.r8'
	${NSW_CenterWindow} $HWNDPARENT $8
	
	${NSD_CreateButton} 84 85 71 26 ''
	Pop $btn_MsgBoxSure
	StrCpy $1 $btn_MsgBoxSure
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_quitsure.bmp $1
	SkinBtn::onClick $1 $R7

	${NSD_CreateButton} 165 85 71 26 ''
	Pop $btn_MsgBoxCancel
	StrCpy $1 $btn_MsgBoxCancel
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_quitreturn.bmp $1
	GetFunctionAddress $0 un.OnClick_FinishUnstall
	SkinBtn::onClick $1 $0
	
	StrCpy $3 33
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 65 $3 250 20 $R6
	Pop $lab_MsgBoxText
    SetCtlColors $lab_MsgBoxText "00A0F0" transparent ;�������͸��
	SendMessage $lab_MsgBoxText ${WM_SETFONT} $Handle_Font 0
	
	StrCpy $3 53
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 65 $3 250 20 $R8
	Pop $lab_MsgBoxText2
    SetCtlColors $lab_MsgBoxText2 "00A0F0" transparent ;�������͸��
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
	BringToFront
FunctionEnd

Function un.ClickSure
	${For} $R3 0 6
		FindProcDLL::FindProc "${PRODUCT_NAME}.exe"
		${If} $R3 == 6
		${AndIf} $R0 != 0
			MessageBox MB_OK|MB_ICONSTOP "�ܱ�Ǹ������������֮��Ĵ���,�볢�����²���������������뵽�ٷ���վѰ�����"
			Abort
		${ElseIf} $R0 != 0
			KillProcDLL::KillProc "${PRODUCT_NAME}.exe"
			Sleep 250
		${Else}
			${Break}
		${EndIf}
	${Next}
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	System::Call "$TEMP\${PRODUCT_NAME}\DIDASetUpHelper::FindClockWindow() i.r0"
	${If} $0 != 0
		SendMessage $0 1344 0 100
	${EndIf}
	Sleep 250
	StrCpy $9 "userchoice"
	SendMessage $HWNDPARENT 0x408 1 0
FunctionEnd

Function un.MyUnstallMsgBox
	push $0
	call un.myGUIInit
	;���˳���Ϣ
	FindProcDLL::FindProc "${PRODUCT_NAME}.exe"
	${If} $R0 != 0
		StrCpy $R6 "���${PRODUCT_NAME}.exe�������У�"
		StrCpy $R8 "�Ƿ�ǿ�ƽ�����"
		GetFunctionAddress $R7 un.ClickSure
		Call un.GsMessageBox
	${Else}
		Call un.ClickSure
	${EndIf}
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
	${NSD_CreateButton} 242 265 80 30 ""
 	Pop	$Btn_ContinueUse
 	StrCpy $1 $Btn_ContinueUse
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_jixushiyong.bmp $1
	GetFunctionAddress $3 un.OnClick_ContinueUse
    SkinBtn::onClick $1 $3
	
	;����ж��
	${NSD_CreateButton} 155 265 80 30 ""
 	Pop	$Btn_CruelRefused
 	StrCpy $1 $Btn_CruelRefused
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_canrenxiezai.bmp $1
	GetFunctionAddress $3 un.OnClick_CruelRefused
    SkinBtn::onClick $1 $3
	
	;ж�����
	${NSD_CreateButton} 183 265 80 30 ""
 	Pop	$Btn_FinishUnstall
 	StrCpy $1 $Btn_FinishUnstall
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_xiezaiwancheng.bmp $1
	GetFunctionAddress $3 un.OnClick_FinishUnstall
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_FinishUnstall ${SW_HIDE}
   
	
	GetFunctionAddress $0 un.onGUICallback  
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
	BringToFront
FunctionEnd