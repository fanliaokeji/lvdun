Var MSG
Var Dialog
Var BGImage  
Var BGImage2      
Var ImageHandle
Var Btn_Next
Var Btn_Agreement
;欢迎页面窗口句柄
Var Hwnd_Welcome
Var ck_xieyi
Var Bool_Xieyi

Var Bool_Bind360
Var ck_bind360
Var Lbl_bind360

Var ck_sysstup
Var Lbl_sysstup
Var Bool_Sysstup

Var ck_assoc
Var Lbl_assoc
Var Bool_assoc

Var Btn_Zidingyi
Var Btn_Zuixiaohua
Var Btn_Guanbi
Var Txt_Browser
Var Btn_Browser
Var Edit_BrowserBg

Var Btn_Install
Var Btn_Return

Var WarningForm
Var Handle_Font
Var Int_FontOffset

;进度条界面
Var Lbl_Sumary
Var Bmp_Finish
Var Btn_FreeUse

;动态获取渠道号
Var str_ChannelID

;全局编译脚本预定义的常量
; MUI 预定义常量
!define MUI_ABORTWARNING
!define MUI_PAGE_FUNCTION_ABORTWARNING onClickGuanbi
!define MUI_CUSTOMFUNCTION_GUIINIT onGUIInit

;MUI 现代界面定义 (1.67 版本以上兼容)
!include "MUI2.nsh"
!include "WinCore.nsh"
;引用文件函数头文件
!include "FileFunc.nsh"
!include "nsWindows.nsh"
!include "WinMessages.nsh"
!include "WordFunc.nsh"
;操作64位dll
!include "Library.nsh"

!include "cfg.nsh"
OutFile "..\bin\${EM_OUTFILE_NAME}"

;自定义页面
Page custom CheckMessageBox
Page custom WelcomePage
Page custom LoadingPage

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
	${FKillProc} ${EXE_NAME}
FunctionEnd

Function GetJPString
	Push $0
	Push $1
	Push $2
	Push $3
	StrCpy $1 "1"
	ClearErrors
	ReadRegStr $0 HKCU "Software\Meitu\KanKan" "AppPath"
	IfErrors 0 +2
	StrCpy $1 "0"
	
	StrCpy $2 "1"
	ClearErrors
	ReadRegStr $0 HKLM "SOFTWARE\2345Pic" "Path"
	IfErrors 0 +2
	StrCpy $2 "0"
	
	StrCpy $3 "1"
	ClearErrors
	ReadRegStr $0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\iSee图片专家" "UninstallString"
	IfErrors 0 +2
	StrCpy $3 "0"
	
	StrCpy $0 "$1_$2_$3"
	Pop $3
	Pop $2
	Pop $1
	Exch $0
FunctionEnd

;卸载旧的（注册表名称为kuaikan）
Function UninstallOld
	Exch $0
	Push $1
	Push $2
	Push $3
	Push $4
	Push $5
	IfFileExists "$0\kuaikan" 0 +3
	Rename "$0\kuaikan" "$0\kuaikantu"
	RMDir /r "$0\kuaikan"

	ReadRegStr $1 HKLM "Software\kuaikan" "InstDir"
	IfFileExists $1 0 EndFunc
	Delete "$DESKTOP\快看.lnk"
	;解锁任务栏
	ReadRegStr $3 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
	${VersionCompare} $3 "6.0" $4
	${if} $4 == 2
		Delete "$QUICKLAUNCH\快看.lnk"
		SetOutPath "$TEMP\${PRODUCT_NAME}"
		IfFileExists "$TEMP\${PRODUCT_NAME}\kksetuphelper.dll" 0 +2
		System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::PinToStartMenu4XP(b 0, t '$STARTMENU\快看.lnk')"
	${else}
		System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::NewGetOSVersionInfo(t .r3)"
		;2 前<后，1 前 > 后， 0 相等 
		${VersionCompare} $3 "6.3" $4
		${If} $4 == 2
			Call GetPinPath
			${If} $0 != "" 
			${AndIf} $0 != 0
				ExecShell taskbarunpin "$0\TaskBar\快看.lnk"
				ExecShell startunpin "$0\StartMenu\快看.lnk"
			${EndIf}
		${Endif}
	${Endif}
	Delete "$STARTMENU\快看.lnk"
	Delete "$SMPROGRAMS\快看图\快看.lnk"
	Delete "$SMPROGRAMS\快看图\卸载快看.lnk"
	RMDir /r "$1\program"
	RMDir /r "$1\xar"
	RMDir /r "$1\res"
	Delete "$1\uninst.exe"
	System::Call 'Shlwapi::PathIsDirectoryEmpty(t r1)i.s'
	Pop $2
	${If} $2 == 1
		RMDir /r "$1"
	${EndIf}
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\kuaikan"
	DeleteRegKey HKLM "Software\kuaikan"
	DeleteRegKey HKCU "Software\kuaikan"
	DeleteRegValue HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "kuaikan"
	;先取消关联再删除key
	System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::SetAssociateOld(t '.jpg;.jpeg;.jpe;.bmp;.png;.gif;.tiff;.tif;.psd;.ico;.pcx;.tga;.wbm;.ras;.mng;.cr2;.nef;.arw;.dng;.srf;.raf;.wmf;', b 0)"
	System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::CreateImgKeyOld(t '.jpg;.jpeg;.jpe;.bmp;.png;.gif;.tiff;.tif;.psd;.ico;.pcx;.tga;.wbm;.ras;.mng;.cr2;.nef;.arw;.dng;.srf;.raf;.wmf;', b 0)"
	EndFunc:
	Pop $5
	Pop $4
	Pop $3
	Pop $2
	Pop $1
	Pop $0
FunctionEnd

Var Bool_IsUpdate
Function DoInstall
	;获取public目录
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	StrCpy $1 ${NSIS_MAX_STRLEN}
	StrCpy $0 ""
	System::Call '$TEMP\${PRODUCT_NAME}\kksetuphelper::GetProfileFolder(t) i(.r0).r2' 
	${If} $0 == ""
		HideWindow
		MessageBox MB_ICONINFORMATION|MB_OK "很抱歉，发生了意料之外的错误,请尝试重新安装"
		Call OnClickQuitOK
	${EndIf}
	IfFileExists "$0" +4 0
	HideWindow
	MessageBox MB_ICONINFORMATION|MB_OK "很抱歉，发生了意料之外的错误,请尝试重新安装"
	Call OnClickQuitOK
	
	Push $0
	Call UninstallOld
	
	;释放配置
	SetOutPath "$0"
	SetOverwrite off
	File /r "..\input_config\*.*"

	;先删再装
	RMDir /r "$INSTDIR\program"
	RMDir /r "$INSTDIR\xar"
	RMDir /r "$INSTDIR\res"
	Delete "$INSTDIR\uninst.exe"


	;释放主程序文件到安装目录
	SetOutPath "$INSTDIR"
	SetOverwrite on
	File /r "..\input_main\*.*"
	File "..\uninst\uninst.exe"


	StrCpy $Bool_IsUpdate 0 
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "Path"
	${NSISLOG} "DoInstall r0 = $0"
	IfFileExists $0 0 +2
	StrCpy $Bool_IsUpdate 1 
	${NSISLOG} "DoInstall Bool_IsUpdate = $Bool_IsUpdate"

	;上报统计
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	System::Call "kernel32::GetCommandLineA() t.R1"
	System::Call "kernel32::GetModuleFileName(i 0, t R2R2, i 256)"
	${WordReplace} $R1 $R2 "" +1 $R3
	${StrFilter} "$R3" "-" "" "" $R4
	;是否静默安装
	StrCpy $R3 "0"
	ClearErrors
	${GetOptions} $R4 "/s"  $R2
	IfErrors 0 +2
	StrCpy $R3 "1"

	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "PeerId"
	${If} $0 == ""
		System::Call '$TEMP\${PRODUCT_NAME}\kksetuphelper::GetPeerID(t) i(.r0).r1'
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "PeerId" "$0"
	${EndIf}
	${WordFind} "${PRODUCT_VERSION}" "." -1 $R1
	;获取竞品信息
	Call GetJPString
	Pop $R3
	${If} $Bool_IsUpdate == 0
		${SendStat} "install" "$R1" "$str_ChannelID" 1
		${SendStat} "installmethod" "$R1" "$R3" 1
		System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::Send2KKAnyHttpStat(t '1', t '$str_ChannelID', t '${PRODUCT_VERSION}')"
	${Else}
		${SendStat} "update" "$R1" "$str_ChannelID" 1
		${SendStat} "updatemethod" "$R1" "$R3" 1
	${EndIf}  
	;写入自用的注册表信息
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallSource" $str_ChannelID
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir" "$INSTDIR"
	System::Call '$TEMP\${PRODUCT_NAME}\kksetuphelper::GetTime(*l) i(.r0).r1'
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallTimes" "$0"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallType" "$Bool_IsUpdate"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "Path" "$INSTDIR\program\${EXE_NAME}.exe"
	;注册表增加版本信息
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "Ver" ${PRODUCT_VERSION}

	;写入通用的注册表信息
	WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\program\${EXE_NAME}.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\program\${EXE_NAME}.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
	;写入文件关联root项
	System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::CreateImgKey(t '.jpg;.jpeg;.jpe;.bmp;.png;.gif;.tiff;.tif;.psd;.ico;.pcx;.tga;.wbm;.ras;.mng;.cr2;.nef;.arw;.dng;.srf;.raf;.wmf;', b true)"
FunctionEnd

Function GetCmdLine
	Push $R1
	Push $R2
	Push $R3
	System::Call "kernel32::GetCommandLineA() t.R1"
	System::Call "kernel32::GetModuleFileName(i 0, t R2R2, i 256)"
	${WordReplace} $R1 $R2 "" +1 $R3
	${StrFilter} "$R3" "-" "" "" $R1
	Pop $R3
	Pop $R2
	Exch $R1
FunctionEnd

Function CmdSilentInstall
	Call GetCmdLine
	Pop $R4
	ClearErrors
	${GetOptions} $R4 "/s"  $R0
	IfErrors FunctionReturn 0
	SetSilent silent
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	${If} $0 != ""
		StrCpy $INSTDIR "$0"
	${EndIf}
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "Path"
	IfFileExists $0 0 StartInstall
		${GetFileVersion} $0 $1
		${VersionCompare} $1 ${PRODUCT_VERSION} $2
		${If} $2 == "2" ;已安装的版本低于该版本
			Goto StartInstall
		${Else}
			Call GetCmdLine
			Pop $R4
			ClearErrors
			${GetOptions} $R4 "/write"  $R0
			IfErrors 0 +2
			Abort
			Goto StartInstall
		${EndIf}
	StartInstall:
	
	;发退出消息
	Call CloseExe
	Call DoInstall
	;将安装方式写入注册表
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallMethod" "silent"
	SetOutPath "$INSTDIR\program"
	;开始菜单程序
	CreateDirectory "$SMPROGRAMS\快看图"
	CreateShortCut "$SMPROGRAMS\快看图\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${EXE_NAME}.exe" "/sstartfrom startmenuprograms" "$INSTDIR\res\shortcut.ico"
	CreateShortCut "$SMPROGRAMS\快看图\卸载${SHORTCUT_NAME}.lnk" "$INSTDIR\uninst.exe"
	;锁定到任务栏
	ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
	${VersionCompare} "$R0" "6.0" $2
	${if} $2 == 2
		;;;;CreateShortCut "$QUICKLAUNCH\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${EXE_NAME}.exe" "/sstartfrom toolbar"
		CreateShortCut "$STARTMENU\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${EXE_NAME}.exe" "/sstartfrom startbar" "$INSTDIR\res\shortcut.ico"
		SetOutPath "$TEMP\${PRODUCT_NAME}"
		IfFileExists "$TEMP\${PRODUCT_NAME}\kksetuphelper.dll" 0 +2
		System::Call '$TEMP\${PRODUCT_NAME}\kksetuphelper::PinToStartMenu4XP(b true, t "$STARTMENU\${SHORTCUT_NAME}.lnk")'
	${else}
		System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::NewGetOSVersionInfo(t .R2)"
		;2 前<后，1 前 > 后， 0 相等 
		${VersionCompare} "$R2" "6.3" $R3
		${If} $R3 == 2
			Call GetPinPath
			${If} $0 != "" 
			${AndIf} $0 != 0
				;;;;ExecShell taskbarunpin "$0\TaskBar\${SHORTCUT_NAME}.lnk"
				;;;;StrCpy $R0 "$0\TaskBar\${SHORTCUT_NAME}.lnk"
				;;;;Call RefreshIcon
				;;;;Sleep 500
				;;;;CreateShortCut "$INSTDIR\program\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${EXE_NAME}.exe" "/sstartfrom toolbar"
				;;;;ExecShell taskbarpin "$INSTDIR\program\${SHORTCUT_NAME}.lnk" "/sstartfrom toolbar"
			
				ExecShell startunpin "$0\StartMenu\${SHORTCUT_NAME}.lnk"
				Sleep 1000
				SetOutPath "$INSTDIR\program"
				CreateShortCut "$STARTMENU\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${EXE_NAME}.exe" "/sstartfrom startbar" "$INSTDIR\res\shortcut.ico"
				StrCpy $R0 "$STARTMENU\${SHORTCUT_NAME}.lnk" 
				Call RefreshIcon
				Sleep 200
				ExecShell startpin "$STARTMENU\${SHORTCUT_NAME}.lnk" "/sstartfrom startbar"
			${EndIf}
		${Endif}
	${Endif}
	;桌面
	;;;;CreateShortCut "$DESKTOP\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${EXE_NAME}.exe" "/sstartfrom desktop"
	${RefreshShellIcons}
	;静默安装根据命令行开机启动
	Call GetCmdLine
	Pop $R4
	StrCpy $Bool_Sysstup 0
	ClearErrors
	${GetOptions} $R4 "/setboot"  $R0
	IfErrors +2 0
	StrCpy $Bool_Sysstup 1
	${If} $Bool_Sysstup == 1
		${SetSysBoot}
	${EndIf}
	;处理文件关联
	${NSISLOG} "CmdSilentInstall Bool_IsUpdate = $Bool_IsUpdate"
	${If} $Bool_IsUpdate == 1 
		StrCpy $R1 "update"
	${Else}
		StrCpy $R1 "noupdate"
		;初次安装自动关联
		SetOutPath "$TEMP\${PRODUCT_NAME}"
		IfFileExists "$TEMP\${PRODUCT_NAME}\kksetuphelper.dll" 0 +2
		System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::SetAssociate(t '.jpg;.jpeg;.jpe;.bmp;.png;.gif;.tiff;.tif;.psd;.ico;.pcx;.tga;.wbm;.ras;.mng;.cr2;.nef;.arw;.dng;.srf;.raf;.wmf;', b true)"
		;修改设置项,不修改关联配置,只在初次安装的时候做
		StrCpy $Bool_assoc 1
		${NSISLOG} "CmdSilentInstall Bool_Sysstup = $Bool_Sysstup, Bool_assoc = $Bool_assoc"
		${If} $Bool_assoc != 1
		${OrIf} $Bool_Sysstup != 1
			Call NSISModifyCfgFile
		${EndIf}
	${EndIf}
	${NSISLOG} "CmdSilentInstall R1 = $R1"
	;处理/run
	Call GetCmdLine
	Pop $R4
	ClearErrors
	${GetOptions} $R4 "/run"  $R0
	IfErrors ExitInstal 0
	${If} $R0 == ""
	${OrIf} $R0 == 0
		StrCpy $R0 "/embedding"
	${EndIf}
	SetOutPath "$INSTDIR\program"
	ExecShell open "${EXE_NAME}.exe" "/sstartfrom installfinish /installmethod silent /installtype $R1 $R0" SW_SHOWNORMAL
	ExitInstal:
	Call OnClickQuitOK
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
		
		${WordFind2X} "$R1" "_" "." +1 $R3
		${If} $R3 == 0
		${OrIf} $R3 == ""
			StrCpy $str_ChannelID ${INSTALL_CHANNELID}
		${ElseIf} $R1 == $R3
			StrCpy $str_ChannelID "unknown"
		${Else}
			StrCpy $str_ChannelID $R3
		${EndIf}
	${EndIf}
FunctionEnd

Function UpdateFont
	StrCpy $Int_FontOffset 4
	CreateFont $Handle_Font "宋体" 10 0
	IfFileExists "$FONTS\msyh.ttf" 0 +3
	CreateFont $Handle_Font "微软雅黑" 10 0
	StrCpy $Int_FontOffset 0
FunctionEnd

Function .onInit
	${InitMutex}
	Call UpdateFont
	Call UpdateChanel
	
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	SetOverwrite on
	File "..\bin\kksetuphelper.dll"
	File "..\input_main\program\Microsoft.VC90.CRT.manifest"
	File "..\input_main\program\msvcp90.dll"
	File "..\input_main\program\msvcr90.dll"
	File "..\input_main\program\Microsoft.VC90.ATL.manifest"
	File "..\input_main\program\ATL90.dll"
	File "..\license\license.txt"
	Call CmdSilentInstall
	
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	${If} $0 != ""
		StrCpy $INSTDIR "$0"
	${EndIf}
	
	InitPluginsDir
    File "/ONAME=$PLUGINSDIR\bg.bmp" "..\images\install\bg.bmp"
	File "/ONAME=$PLUGINSDIR\bg2.bmp" "..\images\install\bg2.bmp"
	File "/ONAME=$PLUGINSDIR\btn_install2.bmp" "..\images\install\btn_install2.bmp"
	File "/oname=$PLUGINSDIR\btn_agreement.bmp" "..\images\install\btn_agreement.bmp"
	File "/oname=$PLUGINSDIR\btn_custom.bmp" "..\images\install\btn_custom.bmp"
	File "/oname=$PLUGINSDIR\btn_uncheck.bmp" "..\images\btn_uncheck.bmp"
	File "/oname=$PLUGINSDIR\btn_check.bmp" "..\images\btn_check.bmp"
	File "/oname=$PLUGINSDIR\btn_min.bmp" "..\images\btn_min.bmp"
	File "/oname=$PLUGINSDIR\btn_close.bmp" "..\images\btn_close.bmp"
	File "/oname=$PLUGINSDIR\btn_browse.bmp" "..\images\install\btn_browse.bmp"
	File "/oname=$PLUGINSDIR\editbg.bmp" "..\images\install\editbg.bmp"
	File "/oname=$PLUGINSDIR\btn_install.bmp" "..\images\install\btn_install.bmp"
	File "/oname=$PLUGINSDIR\btn_return.bmp" "..\images\install\btn_return.bmp"
	File "/oname=$PLUGINSDIR\quit.bmp" "..\images\quit.bmp"
	File "/oname=$PLUGINSDIR\btn_yes.bmp" "..\images\btn_yes.bmp"
	File "/oname=$PLUGINSDIR\btn_no.bmp" "..\images\btn_no.bmp"   	
   	File "/oname=$PLUGINSDIR\loadingbackground.bmp" "..\images\loadingbackground.bmp"
    File "/oname=$PLUGINSDIR\loadingforeground.bmp" "..\images\loadingforeground.bmp"
	File "/oname=$PLUGINSDIR\loading_head.bmp" "..\images\install\loading_head.bmp"
	File "/oname=$PLUGINSDIR\loading_finish.bmp" "..\images\install\loading_finish.bmp"
	File "/oname=$PLUGINSDIR\btn_beginuse.bmp" "..\images\install\btn_beginuse.bmp"
    
	SkinBtn::Init "$PLUGINSDIR\btn_install2.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_agreement.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_custom.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_uncheck.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_check.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_min.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_close.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_browse.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_install.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_return.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_yes.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_no.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_beginuse.bmp"
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
        Call OnClickQuitOK
    ${EndIf}
    SetCtlColors $Hwnd_MsgBox ""  transparent ;背景设成透明

    ${NSW_SetWindowSize} $HWNDPARENT 309 150 ;改变窗体大小
    ${NSW_SetWindowSize} $Hwnd_MsgBox 309 150 ;改变Page大小
	System::Call  'User32::GetDesktopWindow() i.r8'
	${NSW_CenterWindow} $HWNDPARENT $8
	;System::Call "user32::SetForegroundWindow(i $HWNDPARENT)"
	
	StrCpy $3 4
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 12 $3 65 20 "提示"
	Pop $0
    SetCtlColors $0 "FFFFFF" transparent ;背景设成透明
	SendMessage $0 ${WM_SETFONT} $Handle_Font 0
	
	;关闭
	${CreateButton} 279 3 22 22 "btn_close.bmp" OnClickQuitOK $0
	
	${NSD_CreateButton} 139 114 73 25 ''
	Pop $btn_MsgBoxSure
	StrCpy $1 $btn_MsgBoxSure
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_yes.bmp $1
	SkinBtn::onClick $1 $R7

	${NSD_CreateButton} 223 114 73 25 ''
	Pop $btn_MsgBoxCancel
	StrCpy $1 $btn_MsgBoxCancel
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_no.bmp $1
	GetFunctionAddress $0 OnClickQuitOK
	SkinBtn::onClick $1 $0
	
	${If} $R8 == ""
		StrCpy $3 60
	${Else}
		StrCpy $3 55
	${EndIf}
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 66 $3 250 20 $R6
	Pop $lab_MsgBoxText
    SetCtlColors $lab_MsgBoxText "FFFFFF" transparent ;背景设成透明
	SendMessage $lab_MsgBoxText ${WM_SETFONT} $Handle_Font 0
	
	StrCpy $3 75
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 66 $3 250 20 $R8
	Pop $lab_MsgBoxText2
    SetCtlColors $lab_MsgBoxText2 "FFFFFF" transparent ;背景设成透明
	SendMessage $lab_MsgBoxText2 ${WM_SETFONT} $Handle_Font 0
	
	
	GetFunctionAddress $0 onGUICallback
    ;贴背景大图
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\quit.bmp $ImageHandle
	
	WndProc::onCallback $BGImage $0 ;处理无边框窗体移动
	
	GetFunctionAddress $0 onMsgBoxCloseCallback
	WndProc::onCallback $HWNDPARENT $0 ;处理关闭消息
	
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
	
	ShowWindow $Hwnd_MsgBox ${SW_SHOW}
	BringToFront
FunctionEnd

Function ClickSure2
	${FKillProc} ${EXE_NAME}
	Call ClickSure3
FunctionEnd

Function ClickSure1
	;ShowWindow $Hwnd_MsgBox ${SW_HIDE}
	;ShowWindow $HWNDPARENT ${SW_HIDE}
	Sleep 100
	;发退出消息
	FindProcDLL::FindProc "${EXE_NAME}.exe"
	${If} $R0 != 0
		StrCpy $R6 "检测到${SHORTCUT_NAME}正在运行，是否强制结束？"
		StrCpy $R8 ""
		GetFunctionAddress $R7 ClickSure2
		Call GsMessageBox
	${Else}
		Call ClickSure2
	${EndIf}
FunctionEnd

Function ClickSure3
	StrCpy $R9 1 ;Goto the next page
    Call RelGotoPage
FunctionEnd

Function CheckMessageBox
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "Path"
	IfFileExists $0 0 StartInstall
	${GetFileVersion} $0 $1
	${VersionCompare} $1 ${PRODUCT_VERSION} $2
	System::Call "kernel32::GetCommandLineA() t.R1"
	System::Call "kernel32::GetModuleFileName(i 0, t R2R2, i 256)"
	${WordReplace} $R1 $R2 "" +1 $R3
	${StrFilter} "$R3" "-" "" "" $R4
	ClearErrors
	${GetOptions} $R4 "/write"  $R0
	IfErrors 0 +3
	push "false"
	pop $R0
	${If} $2 == "2" ;已安装的版本低于该版本
		Call ClickSure1
	${ElseIf} $2 == "0" ;版本相同
	${OrIf} $2 == "1"	;已安装的版本高于该版本
		 ${If} $R0 == "false"
			StrCpy $R6 "检测到已安装${SHORTCUT_NAME}$1，"
			StrCpy $R8 "是否覆盖安装？"
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
	;消除边框
    System::Call `user32::SetWindowLong(i$HWNDPARENT,i${GWL_STYLE},0x9480084C)i.R0`
    ;隐藏一些既有控件
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

;处理无边框移动
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

;协议按钮事件
Function onClickAgreement
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	ExecShell open license.txt /x SW_SHOWNORMAL
FunctionEnd

Function OnClick_CheckXieyi
	${IF} $Bool_Xieyi == 1
        IntOp $Bool_Xieyi $Bool_Xieyi - 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\btn_uncheck.bmp $ck_xieyi
		EnableWindow $Btn_Next 0
		EnableWindow $Btn_Zidingyi 0
    ${ELSE}
        IntOp $Bool_Xieyi $Bool_Xieyi + 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\btn_check.bmp $ck_xieyi
		EnableWindow $Btn_Next 1
		EnableWindow $Btn_Zidingyi 1
    ${EndIf}
	ShowWindow $ck_xieyi ${SW_HIDE}
	ShowWindow $ck_xieyi ${SW_SHOW}
FunctionEnd

Function OnClick_Bind360
	${IF} $Bool_Bind360 == 1
        IntOp $Bool_Bind360 $Bool_Bind360 - 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\btn_uncheck.bmp $ck_bind360
    ${ELSE}
        IntOp $Bool_Bind360 $Bool_Bind360 + 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\btn_check.bmp $ck_bind360
    ${EndIf}
	ShowWindow $ck_bind360 ${SW_HIDE}
	ShowWindow $ck_bind360 ${SW_SHOW}
FunctionEnd

Function OnClick_CheckSysstup
	${IF} $Bool_Sysstup == 1
        IntOp $Bool_Sysstup $Bool_Sysstup - 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\btn_uncheck.bmp $ck_sysstup
    ${ELSE}
        IntOp $Bool_Sysstup $Bool_Sysstup + 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\btn_check.bmp $ck_sysstup
    ${EndIf}
FunctionEnd

Function OnClick_CheckAssoc
	${IF} $Bool_assoc == 1
        IntOp $Bool_assoc $Bool_assoc - 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\btn_uncheck.bmp $ck_assoc
    ${ELSE}
        IntOp $Bool_assoc $Bool_assoc + 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\btn_check.bmp $ck_assoc
    ${EndIf}
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
		StrCpy $R1 "kuaikantu"
	${EndIf}

	nsDialogs::SelectFolderDialog "请选择 $R0 安装的文件夹:" "$R0"
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

;更改目录事件
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
		${DriveSpace} $7 "/D=F /S=M" $8
		${If} $8 == ""
			EnableWindow $Btn_Install 0
			EnableWindow $Btn_Return 0
			Goto EndFunction
		${EndIf}
		IntCmp ${NeedSpace} $8 0 0 ErrorChunk
		EnableWindow $Btn_Install 1
		EnableWindow $Btn_Return 1
		Goto EndFunction
		ErrorChunk:
			MessageBox MB_OK|MB_ICONSTOP "磁盘剩余空间不足，需要至少${NeedSpace}M"
			EnableWindow $Btn_Install 0
			EnableWindow $Btn_Return 0
		EndFunction:
	${EndIf}
FunctionEnd

Function onClickZidingyi
	;HideWindow
	ShowWindow $ck_bind360 ${SW_HIDE}
	ShowWindow $Lbl_bind360 ${SW_HIDE}
	ShowWindow $Btn_Next ${SW_HIDE}
	ShowWindow $Btn_Agreement ${SW_HIDE}
	ShowWindow $ck_xieyi ${SW_HIDE}
	ShowWindow $Btn_Zidingyi ${SW_HIDE}
	ShowWindow $BGImage ${SW_HIDE}
	
	
	
	ShowWindow $BGImage2 1
	ShowWindow $Edit_BrowserBg 1
	ShowWindow $Txt_Browser 1
	ShowWindow $Btn_Browser 1
	ShowWindow $Btn_Install 1
	ShowWindow $Btn_Return 1
	
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
	ShowWindow $Btn_Zidingyi 1
	ShowWindow $ck_bind360 1
	ShowWindow $Lbl_bind360 1
	
	
	ShowWindow $Edit_BrowserBg ${SW_HIDE}
	ShowWindow $Txt_Browser ${SW_HIDE}
	ShowWindow $Btn_Browser ${SW_HIDE}
	ShowWindow $Btn_Install ${SW_HIDE}
	ShowWindow $Btn_Return ${SW_HIDE}
	ShowWindow $BGImage2 ${SW_HIDE}
	
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
	${EndIf}
	IsWindow $WarningForm Create_End
	!define Style ${WS_VISIBLE}|${WS_OVERLAPPEDWINDOW}
	${NSW_CreateWindowEx} $WarningForm $hwndparent ${ExStyle} ${Style} "" 1018

	${NSW_SetWindowSize} $WarningForm 309 150
	EnableWindow $hwndparent 0
	System::Call `user32::SetWindowLong(i $WarningForm,i ${GWL_STYLE},0x9480084C)i.R0`
	
	;标题栏
	StrCpy $3 4
	IntOp $3 $3 + $Int_FontOffset
	${NSW_CreateLabel} 12 $3 65 20 "提示"
	Pop $5
    SetCtlColors $5 "FFFFFF" transparent ;背景设成透明
	SendMessage $5 ${WM_SETFONT} $Handle_Font 0
	
	;关闭
	${NSW_CreateButton} 279 5 22 22 ''
	Pop $6
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_close.bmp $6
	${NSW_OnClick} $6 OnClickQuitCancel
	
	${NSW_CreateButton} 139 114 73 25 ''
	Pop $R0
	StrCpy $1 $R0
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_yes.bmp $1
	${NSW_OnClick} $R0 OnClickQuitOK

	${NSW_CreateButton} 223 114 73 25 ''
	Pop $R0
	StrCpy $1 $R0
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_no.bmp $1
	${NSW_OnClick} $R0 OnClickQuitCancel

	StrCpy $3 60
	IntOp $3 $3 + $Int_FontOffset
	${NSW_CreateLabel} 66 $3 190 45 "您确定要退出${SHORTCUT_NAME}安装程序？"
	Pop $4
    SetCtlColors $4 "FFFFFF" transparent ;背景设成透明
	SendMessage $4 ${WM_SETFONT} $Handle_Font 0
	
	${NSW_CreateBitmap} 0 0 100% 100% ""
  	Pop $1
	${NSW_SetImage} $1 $PLUGINSDIR\quit.bmp $ImageHandle
	GetFunctionAddress $0 onWarningGUICallback
	WndProc::onCallback $1 $0 ;处理无边框窗体移动
	${NSW_CenterWindow} $WarningForm $hwndparent
	${NSW_Show}
	Create_End:
	ShowWindow $WarningForm ${SW_SHOW}
FunctionEnd

Function OnClickQuitOK
	HideWindow
	System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::WaitForStat()"
	RMDir /r /REBOOTOK $PLUGINSDIR
	RMDir /r /REBOOTOK "$TEMP\${PRODUCT_NAME}"
	System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::SetUpExit()"
FunctionEnd

Function OnClickQuitCancel
	${NSW_DestroyWindow} $WarningForm
	EnableWindow $hwndparent 1
	BringToFront
FunctionEnd

;处理页面跳转的命令
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
		MessageBox MB_OK|MB_ICONSTOP "路径不合法"
		Goto EndFunction
	${Else}
		StrCpy $8 ""
		${DriveSpace} $7 "/D=F /S=M" $8
		${If} $8 == ""
			MessageBox MB_OK|MB_ICONSTOP "路径不合法"
			Goto EndFunction
		${EndIf}
		IntCmp ${NeedSpace} $8 0 0 ErrorChunk
		StrCpy $R9 1 ;Goto the next page
		Call RelGotoPage
		Goto EndFunction
		ErrorChunk:
			MessageBox MB_OK|MB_ICONSTOP "磁盘剩余空间不足，需要至少${NeedSpace}M"
	${EndIf}
	EndFunction:
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
        Call OnClickQuitOK
    ${EndIf}
    SetCtlColors $Hwnd_Welcome ""  transparent ;背景设成透明

    ${NSW_SetWindowSize} $HWNDPARENT 548 388 ;改变窗体大小
    ${NSW_SetWindowSize} $Hwnd_Welcome 548 388 ;改变Page大小
	
	System::Call  'User32::GetDesktopWindow() i.R9'
	${NSW_CenterWindow} $HWNDPARENT $R9

    ;一键安装
	${CreateButton} 195 209 160 48 "btn_install2.bmp" OnClick_Install $Btn_Next
	;自定义安装
	${CreateButton} 215 277 120 32 "btn_custom.bmp" onClickZidingyi $Btn_Zidingyi
	;勾选同意协议
	${CreateButton} 32 345 13 13 "btn_check.bmp" OnClick_CheckXieyi $ck_xieyi
	StrCpy $Bool_Xieyi 1
	StrCpy $Bool_Bind360 1
	;用户协议
	${CreateButton} 163 345 78 14 "btn_agreement.bmp" onClickAgreement $Btn_Agreement
	;捆绑360
	${CreateButton} 352 345 13 13 "btn_check.bmp" OnClick_Bind360 $ck_bind360
	${CreateLabel} 372 341 116 18 "安装360安全套装" "FFFFFF" $Handle_Font OnClick_Bind360 $Lbl_bind360
	
	;最小化
	${CreateButton} 488 3 22 22 "btn_min.bmp" onClickZuixiaohua $Btn_Zuixiaohua
	;关闭
	${CreateButton} 520 3 22 22 "btn_close.bmp" onClickGuanbi $Btn_Guanbi
	
	
	;目录选择框
	${NSD_CreateDirRequest} 35 140 402 23 "$INSTDIR"
 	Pop	$Txt_Browser
	SendMessage $Txt_Browser ${WM_SETFONT} $Handle_Font 0
	SetCtlColors $Txt_Browser "0"  "f3f8fb" ;背景设成白色
 	${NSD_OnChange} $Txt_Browser OnChange_DirRequest
	ShowWindow $Txt_Browser ${SW_HIDE}
	;目录选择按钮
	${NSD_CreateBrowseButton} 437 138 81 27 ""
 	Pop	$Btn_Browser
 	StrCpy $1 $Btn_Browser
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_browse.bmp $1
	GetFunctionAddress $3 OnClick_BrowseButton
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_Browser ${SW_HIDE}
	;目录选择框背景
	 ${NSD_CreateBitmap} 33 138 484 27 ""
    Pop $Edit_BrowserBg
    ${NSD_SetImage} $Edit_BrowserBg $PLUGINSDIR\editbg.bmp $ImageHandle
	ShowWindow $Edit_BrowserBg ${SW_HIDE}
	
	;立即安装
	${CreateButton} 331 328 103 30 "btn_install.bmp" OnClick_Install $Btn_Install
	;返回
	${CreateButton} 443 328 73 30 "btn_return.bmp" OnClick_Return $Btn_Return
	
	
	 GetFunctionAddress $0 onGUICallback
	;贴背景大图2
	${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage2
    ${NSD_SetImage} $BGImage2 $PLUGINSDIR\bg2.bmp $ImageHandle
	ShowWindow $BGImage2 ${SW_HIDE}
	WndProc::onCallback $BGImage2 $0 ;处理无边框窗体移动
	
    ;贴背景大图
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\bg.bmp $ImageHandle
	WndProc::onCallback $BGImage $0 ;处理无边框窗体移动
	
	Call OnClick_Return
	
	GetFunctionAddress $0 onCloseCallback
	WndProc::onCallback $HWNDPARENT $0 ;处理关闭消息
	
	nsDialogs::Show
	${NSD_FreeImage} $ImageHandle
	;ShowWindow $HWNDPARENT ${SW_HIDE}
	BringToFront
FunctionEnd

Function DoBind360
	${If} $Bool_Bind360 == 1
		SetOutPath "$TEMP\${PRODUCT_NAME}"
		System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::DownLoadUrlAndInstall(t 'http://dl.360safe.com/pclianmeng/c/13__3112239__3f7372633d6c6d266c733d6e33323563303134383964__68616f2e3336302e636e__0c39.exe', t '$TEMP\13__3112239__3f7372633d6c6d266c733d6e33323563303134383964__68616f2e3336302e636e__0c39.exe', t '', i 60000)"
		;System::Call 'Urlmon.DLL::URLDownloadToFile(i0, t"http://dl.360safe.com/pclianmeng/c/13__3112239__3f7372633d6c6d266c733d6e33323563303134383964__68616f2e3336302e636e__0c39.exe", t"$TEMP\13__3112239__3f7372633d6c6d266c733d6e33323563303134383964__68616f2e3336302e636e__0c39.exe", i0,i0)i.s'
		IfFileExists "$TEMP\13__3112239__3f7372633d6c6d266c733d6e33323563303134383964__68616f2e3336302e636e__0c39.exe" 0 +2
			ExecShell open "$TEMP\13__3112239__3f7372633d6c6d266c733d6e33323563303134383964__68616f2e3336302e636e__0c39.exe" "" SW_SHOWNORMAL
	${EndIf}
FunctionEnd

Var Handle_Loading
Var PB_ProgressBar
Function NSD_TimerFun
	GetFunctionAddress $0 NSD_TimerFun
    nsDialogs::KillTimer $0
    !if 1   ;是否在后台运行,1有效
        GetFunctionAddress $0 InstallationMainFun
        BgWorker::CallAndWait
    !else
        Call InstallationMainFun
    !endif
	;主线程中创建快捷方式
	ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
	${VersionCompare} "$R0" "6.0" $2
	SetOutPath "$INSTDIR\program"
	;快速启动栏
	${if} $2 == 2
		CreateShortCut "$QUICKLAUNCH\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${EXE_NAME}.exe" "/sstartfrom toolbar"
		CreateShortCut "$STARTMENU\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${EXE_NAME}.exe" "/sstartfrom startbar" "$INSTDIR\res\shortcut.ico"
		StrCpy $R0 "$STARTMENU\${SHORTCUT_NAME}.lnk" 
		Call RefreshIcon
		SetOutPath "$TEMP\${PRODUCT_NAME}"
		IfFileExists "$TEMP\${PRODUCT_NAME}\kksetuphelper.dll" 0 +2
		System::Call '$TEMP\${PRODUCT_NAME}\kksetuphelper::PinToStartMenu4XP(b true, t "$STARTMENU\${SHORTCUT_NAME}.lnk")'
	${else}
		System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::NewGetOSVersionInfo(t .R2)"
		;2 前<后，1 前 > 后， 0 相等 
		${VersionCompare} "$R2" "6.3" $R3
		${If} $R3 == 2
			Call GetPinPath
			${If} $0 != "" 
			${AndIf} $0 != 0	
				ExecShell taskbarunpin "$0\TaskBar\${SHORTCUT_NAME}.lnk"
				StrCpy $R0 "$0\TaskBar\${SHORTCUT_NAME}.lnk"
				Call RefreshIcon
				Sleep 500
				SetOutPath "$INSTDIR\program"
				CreateShortCut "$INSTDIR\program\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${EXE_NAME}.exe" "/sstartfrom toolbar"
				ExecShell taskbarpin "$INSTDIR\program\${SHORTCUT_NAME}.lnk" "/sstartfrom toolbar"
			
				ExecShell startunpin "$0\StartMenu\${SHORTCUT_NAME}.lnk"
				Sleep 1000
				SetOutPath "$INSTDIR\program"
				CreateShortCut "$STARTMENU\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${EXE_NAME}.exe" "/sstartfrom startbar" "$INSTDIR\res\shortcut.ico"
				StrCpy $R0 "$STARTMENU\${SHORTCUT_NAME}.lnk" 
				Call RefreshIcon
				Sleep 200
				ExecShell startpin "$STARTMENU\${SHORTCUT_NAME}.lnk" "/sstartfrom startbar"
			${EndIf}
		${Endif}
	${Endif}
	
	;开始菜单程序
	CreateDirectory "$SMPROGRAMS\快看图"
	SetOutPath "$INSTDIR\program"
	CreateShortCut "$SMPROGRAMS\快看图\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${EXE_NAME}.exe" "/sstartfrom startmenuprograms" "$INSTDIR\res\shortcut.ico"
	CreateShortCut "$SMPROGRAMS\快看图\卸载${SHORTCUT_NAME}.lnk" "$INSTDIR\uninst.exe"
	;桌面
	CreateShortCut "$DESKTOP\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${EXE_NAME}.exe" "/sstartfrom desktop" "$INSTDIR\res\shortcut.ico"
	${RefreshShellIcons}
	
	;最后才显示安装完成界面
	;HideWindow
	ShowWindow $Handle_Loading ${SW_HIDE}
	ShowWindow $Lbl_Sumary ${SW_HIDE}
	ShowWindow $PB_ProgressBar ${SW_HIDE}
	
	ShowWindow $Bmp_Finish ${SW_SHOW}
	ShowWindow $ck_sysstup ${SW_SHOW}
	ShowWindow $Lbl_sysstup ${SW_SHOW}
	ShowWindow $ck_assoc ${SW_SHOW}
	ShowWindow $Lbl_assoc ${SW_SHOW}
	ShowWindow $Btn_Zuixiaohua ${SW_SHOW}
	ShowWindow $Btn_Guanbi ${SW_SHOW}
	ShowWindow $Btn_FreeUse ${SW_SHOW}
	ShowWindow $Handle_Loading ${SW_SHOW}
	EnableWindow $Btn_Guanbi 1
	;ShowWindow $HWNDPARENT ${SW_HIDE}
	BringToFront
FunctionEnd

Function InstallationMainFun
	SendMessage $PB_ProgressBar ${PBM_SETRANGE32} 0 100  ;总步长为顶部定义值
	Sleep 1
	Call CloseExe
	SendMessage $PB_ProgressBar ${PBM_SETPOS} 2 0
	Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 14 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 27 0
    Call DoInstall
	;将安装方式写入注册表
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallMethod" "nosilent"
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 50 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 73 0
    ;Call DoBind360
	Sleep 300
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 100 0
	Sleep 1000
FunctionEnd

Function NSISModifyCfgFile
	push $0
	push $1
	push $2
	push $3
	StrCpy $1 ${NSIS_MAX_STRLEN}
	StrCpy $0 ""
	System::Call '$TEMP\${PRODUCT_NAME}\kksetuphelper::GetProfileFolder(t) i(.r0).r2' 
	StrCpy $3 ""
	${If} $0 != ""
		ClearErrors
		;注意这次修改的是UserConfig2.lua， 在xar里面会合并到UserConfig.lua
		FileOpen $1 "$0\${PRODUCT_NAME}\UserConfig2.lua" r
		IfErrors done
		FileRead $1 $2
		${While} $2 != ''
			StrCpy $3 "$3$2"
			FileSeek $1 0 CUR
			FileRead $1 $2
		${EndWhile}
		FileClose $1
		done:
	${EndIf}
	${If} $3 != ""
		${NSISLOG} "NSISModifyCfgFile Bool_Sysstup = $Bool_Sysstup, Bool_assoc = $Bool_assoc"
		${If} $Bool_Sysstup != 1
			${WordReplace} $3 "$\"sysboot$\"" "$\"sysboot_del$\"" "+*" $3
		${EndIf}
		${If} $Bool_assoc != 1
			${WordReplace} $3 "$\"associate$\"" "$\"associate_del$\"" "+*" $3
		${EndIf}
		ClearErrors
		FileOpen $1 "$0\${PRODUCT_NAME}\UserConfig2.lua" w
		IfErrors done2
		FileWrite $1 $3
		FileClose $1
		done2:
	${EndIf}
	pop $3
	pop $2
	pop $1
	pop $0
FunctionEnd

Function onLastClose
	${If} $Bool_Sysstup == 1
		${SetSysBoot}
	${Else}
		${UnSetSysBoot}
	${EndIf}
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	${If} $Bool_assoc == 1
		IfFileExists "$TEMP\${PRODUCT_NAME}\kksetuphelper.dll" 0 +2
		System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::SetAssociate(t '.jpg;.jpeg;.jpe;.bmp;.png;.gif;.tiff;.tif;.psd;.ico;.pcx;.tga;.wbm;.ras;.mng;.cr2;.nef;.arw;.dng;.srf;.raf;.wmf;', b true)"
	${Else}
		IfFileExists "$TEMP\${PRODUCT_NAME}\kksetuphelper.dll" 0 +2
		System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::SetAssociate(t '.jpg;.jpeg;.jpe;.bmp;.png;.gif;.tiff;.tif;.psd;.ico;.pcx;.tga;.wbm;.ras;.mng;.cr2;.nef;.arw;.dng;.srf;.raf;.wmf;', b 0)"
	${EndIf}
	;修改设置项
	${If} $Bool_assoc != 1
	${OrIf} $Bool_Sysstup != 1
		Call NSISModifyCfgFile
	${EndIf}
	HideWindow
	Call DoBind360
	Call OnClickQuitOK
FunctionEnd

Function OnClick_FreeUse
	${If} $Bool_Sysstup == 1
		${SetSysBoot}
	${Else}
		${UnSetSysBoot}
	${EndIf}
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	${If} $Bool_assoc == 1
		IfFileExists "$TEMP\${PRODUCT_NAME}\kksetuphelper.dll" 0 +2
		System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::SetAssociate(t '.jpg;.jpeg;.jpe;.bmp;.png;.gif;.tiff;.tif;.psd;.ico;.pcx;.tga;.wbm;.ras;.mng;.cr2;.nef;.arw;.dng;.srf;.raf;.wmf;', b true)"
	${Else}
		IfFileExists "$TEMP\${PRODUCT_NAME}\kksetuphelper.dll" 0 +2
		System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::SetAssociate(t '.jpg;.jpeg;.jpe;.bmp;.png;.gif;.tiff;.tif;.psd;.ico;.pcx;.tga;.wbm;.ras;.mng;.cr2;.nef;.arw;.dng;.srf;.raf;.wmf;', b 0)"
	${EndIf}
	;修改设置项
	${If} $Bool_assoc != 1
	${OrIf} $Bool_Sysstup != 1
		Call NSISModifyCfgFile
	${EndIf}
	SetOutPath "$INSTDIR\program"
	${If} $Bool_IsUpdate == 1 
		StrCpy $R1 "update"
	${Else}
		StrCpy $R1 "noupdate"
	${EndIf}
	ExecShell open "${EXE_NAME}.exe" "/forceshow /sstartfrom installfinish /installmethod nosilent /installtype $R1" SW_SHOWNORMAL
	HideWindow
	Call DoBind360
	Call OnClickQuitOK
FunctionEnd

;安装进度页面
Function LoadingPage
  StrCpy $isMainUIShow "false";按下esc直接退出
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
		Call OnClickQuitOK
	${EndIf}
	SetCtlColors $Handle_Loading ""  transparent ;背景设成透明

	${NSW_SetWindowSize} $HWNDPARENT 548 388 ;改变自定义窗体大小
	${NSW_SetWindowSize} $Handle_Loading 548 388 ;改变自定义Page大小


    ;创建简要说明
	StrCpy $3 317
	IntOp $3 $3 + $Int_FontOffset
    ${NSD_CreateLabel} 33 $3 120 20 "正在提取文件..."
    Pop $Lbl_Sumary
    SetCtlColors $Lbl_Sumary "FFFFFF"  transparent ;背景设成透明
	SendMessage $Lbl_Sumary ${WM_SETFONT} $Handle_Font 0
    
	;进度条
	 ${NSD_CreateProgressBar} 33 345 484 14 ""
    Pop $PB_ProgressBar
    SkinProgress::Set $PB_ProgressBar "$PLUGINSDIR\loadingforeground.bmp" "$PLUGINSDIR\loadingbackground.bmp"
    GetFunctionAddress $0 NSD_TimerFun
    nsDialogs::CreateTimer $0 1
	
	;完成时"开始使用"按钮
	${CreateButton} 195 241 160 48 "btn_beginuse.bmp" OnClick_FreeUse $Btn_FreeUse
	ShowWindow $Btn_FreeUse ${SW_HIDE}
	;勾选开机启动
	${CreateButton} 32 345 13 13 "btn_check.bmp" OnClick_CheckSysstup $ck_sysstup
	${CreateLabel} 52 341 136 18 "开机自动启动快看图" "FFFFFF" $Handle_Font OnClick_CheckSysstup $Lbl_sysstup
	;勾选自动关联
	${CreateButton} 226 345 13 13 "btn_check.bmp" OnClick_CheckAssoc $ck_assoc
	${CreateLabel} 246 341 116 18 "自动关联图片格式" "FFFFFF" $Handle_Font OnClick_CheckAssoc $Lbl_assoc
	;初始化变量值
	StrCpy $Bool_sysstup 1
	StrCpy $Bool_assoc 1
	ShowWindow $ck_sysstup ${SW_HIDE}
	ShowWindow $Lbl_sysstup ${SW_HIDE}
	ShowWindow $ck_assoc ${SW_HIDE}
	ShowWindow $Lbl_assoc ${SW_HIDE}
	
	;最小化
	${CreateButton} 488 3 22 22 "btn_min.bmp" onClickZuixiaohua $Btn_Zuixiaohua
	;关闭
	${CreateButton} 520 3 22 22 "btn_close.bmp" onLastClose $Btn_Guanbi
	EnableWindow $Btn_Guanbi 0
	
	GetFunctionAddress $0 onGUICallback  
    ;完成时背景图
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $Bmp_Finish
    ${NSD_SetImage} $Bmp_Finish $PLUGINSDIR\loading_finish.bmp $ImageHandle
    ShowWindow $Bmp_Finish ${SW_HIDE}
	WndProc::onCallback $Bmp_Finish $0 ;处理无边框窗体移动
	 
    ;贴背景大图
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\loading_head.bmp $ImageHandle
    WndProc::onCallback $BGImage $0 ;处理无边框窗体移动
	
	;处理关闭消息
	GetFunctionAddress $0 onCloseCallback
	WndProc::onCallback $HWNDPARENT $0 
    ShowWindow $HWNDPARENT ${SW_SHOW}
	nsDialogs::Show
    ${NSD_FreeImage} $ImageHandle
	BringToFront
FunctionEnd

Function RefreshIcon
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	System::Call '$TEMP\${PRODUCT_NAME}\kksetuphelper::RefleshIcon(t "$R0")'
FunctionEnd

Function GetPinPath
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	System::Call '$TEMP\${PRODUCT_NAME}\kksetuphelper::GetUserPinPath(t) i(.r0)'
FunctionEnd