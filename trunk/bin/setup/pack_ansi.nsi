Var MSG
Var Dialog
Var BGImage        
Var ImageHandle
Var Btn_Next
Var Btn_Agreement
;欢迎页面窗口句柄
Var Hwnd_Welcome
Var Bool_IsExtend
Var ck_xieyi
Var Bool_Xieyi
Var Lbl_Xieyi
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

Var ck_StartTimeDo
Var Lbl_StartTimeDo
Var Bool_StartTimeDo

Var Btn_Install
Var Btn_Return

Var WarningForm
Var Handle_Font
Var Int_FontOffset

;进度条界面
Var Lbl_Sumary
Var PB_ProgressBar
Var Bmp_Finish
Var Btn_FreeUse

;动态获取渠道号
Var str_ChannelID
;---------------------------全局编译脚本预定义的常量-----------------------------------------------------
; MUI 预定义常量
!define MUI_ABORTWARNING
!define MUI_PAGE_FUNCTION_ABORTWARNING onClickGuanbi
;安装图标的路径名字
!define MUI_ICON "images\fav.ico"
;卸载图标的路径名字
!define MUI_UNICON "images\unis.ico"

!define INSTALL_CHANNELID "0001"

!define PRODUCT_NAME "GreenShield"
!define SHORTCUT_NAME "绿盾"
!define PRODUCT_VERSION "1.0.0.4"
!define VERSION_LASTNUMBER 4
!define NeedSpace 10240
!define EM_OUTFILE_NAME "GsSetup_${INSTALL_CHANNELID}.exe"

!define EM_BrandingText "${PRODUCT_NAME}${PRODUCT_VERSION}"
!define PRODUCT_PUBLISHER "GreenShield"
!define PRODUCT_WEB_SITE "http://www.lvdun123.com/"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_NAME}.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_MAININFO_FORSELF "Software\${PRODUCT_NAME}"

;卸载包开关（请不要轻易打开）
;!define SWITCH_CREATE_UNINSTALL_PAKAGE 1

;CRCCheck on
;---------------------------设置软件压缩类型（也可以通过外面编译脚本控制）------------------------------------
SetCompressor /SOLID lzma
SetCompressorDictSize 32
BrandingText "${EM_BrandingText}"
SetCompress force
SetDatablockOptimize on
;XPStyle on
; ------ MUI 现代界面定义 (1.67 版本以上兼容) ------
!include "MUI2.nsh"
!include "WinCore.nsh"
;引用文件函数头文件
!include "FileFunc.nsh"
!include "nsWindows.nsh"
!include "WinMessages.nsh"
!include "WordFunc.nsh"

!define MUI_CUSTOMFUNCTION_GUIINIT onGUIInit
!define MUI_CUSTOMFUNCTION_UNGUIINIT un.myGUIInit

!insertmacro MUI_LANGUAGE "SimpChinese"
SetFont 宋体 9
!define TEXTCOLOR "4D4D4D"
RequestExecutionLevel highest

VIProductVersion ${PRODUCT_VERSION}
VIAddVersionKey /LANG=2052 "ProductName" "${SHORTCUT_NAME}广告管家"
VIAddVersionKey /LANG=2052 "Comments" ""
VIAddVersionKey /LANG=2052 "CompanyName" "成都市灵智互动科技有限公司"
;VIAddVersionKey /LANG=2052 "LegalTrademarks" "GreenShield"
VIAddVersionKey /LANG=2052 "LegalCopyright" "Copyright (c) 2014-2016 成都市灵智互动科技有限公司"
VIAddVersionKey /LANG=2052 "FileDescription" "${SHORTCUT_NAME}广告管家安装程序"
VIAddVersionKey /LANG=2052 "FileVersion" ${PRODUCT_VERSION}
VIAddVersionKey /LANG=2052 "ProductVersion" ${PRODUCT_VERSION}
VIAddVersionKey /LANG=2052 "OriginalFilename" ${EM_OUTFILE_NAME}

;自定义页面
Page custom CheckMessageBox
Page custom WelcomePage
Page custom LoadingPage
UninstPage custom un.MyUnstallMsgBox
UninstPage custom un.MyUnstall


;------------------------------------------------------MUI 现代界面定义以及函数结束------------------------
;应用程序显示名字
Name "${SHORTCUT_NAME} ${PRODUCT_VERSION}"
;应用程序输出路径
!ifdef SWITCH_CREATE_UNINSTALL_PAKAGE
	OutFile "uninst\_uninst.exe"
!else
	OutFile "bin\${EM_OUTFILE_NAME}"
!endif
InstallDir "$PROGRAMFILES\GreenShield"
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
	FindWindow $R0 "{B239B46A-6EDA-4a49-8CEE-E57BB352F933}_dsmainmsg"
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

Function DoInstall
  ;释放配置到public目录
  SetOutPath "$TEMP\${PRODUCT_NAME}"
  StrCpy $1 ${NSIS_MAX_STRLEN}
  StrCpy $0 ""
  System::Call '$TEMP\${PRODUCT_NAME}\DsSetUpHelper::GetProfileFolder(t) i(.r0).r2' 
  ${If} $0 == ""
	HideWindow
	MessageBox MB_ICONINFORMATION|MB_OK "很抱歉，发生了意料之外的错误,请尝试重新安装，如果还不行请加QQ群67542242寻求帮助"
	Call OnClickQuitOK
  ${EndIf}
  IfFileExists "$0" +4 0
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "很抱歉，发生了意料之外的错误,请尝试重新安装，如果还不行请加QQ群67542242寻求帮助"
  Call OnClickQuitOK
 
  SetOutPath "$0\GreenShield"
  File "input_config\GreenShield\DataV.dat"
  File "input_config\GreenShield\DataW.dat"
  
  ;重命名
  ;Delete "$0\GreenShield\*.bak"
  IfFileExists "$0\GreenShield\VideoList.dat" 0 +3
  IfFileExists "$0\GreenShield\VideoList.dat.bak" +2 0
  Rename "$0\GreenShield\VideoList.dat" "$0\GreenShield\VideoList.dat.bak"
  IfFileExists "$0\GreenShield\UserConfig.dat" 0 +3
  IfFileExists "$0\GreenShield\UserConfig.dat.bak" +2 0
  Rename "$0\GreenShield\UserConfig.dat" "$0\GreenShield\UserConfig.dat.bak"
  IfFileExists "$0\GreenShield\FilterConfig.dat" 0 +3
  IfFileExists "$0\GreenShield\FilterConfig.dat.bak" +2 0
  Rename "$0\GreenShield\FilterConfig.dat" "$0\GreenShield\FilterConfig.dat.bak"
  
  SetOutPath "$0"
  SetOverwrite on
  File /r "input_config\*.*"
  ;先删再装
  RMDir /r "$INSTDIR\program"
  RMDir /r "$INSTDIR\xar"
  RMDir /r "$INSTDIR\res"
  RMDir /r "$INSTDIR\appimage"
  ;文件被占用则改一下名字
  StrCpy $R4 "$INSTDIR\program\GsNet32.dll"
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
  
  ;释放主程序文件到安装目录
  SetOutPath "$INSTDIR"
  SetOverwrite on
  File /r "input_main\*.*"
  ;WriteUninstaller "$INSTDIR\uninst.exe"
  File "uninst\uninst.exe"
  
  ;最后一步注册服务
  IfFileExists "$TEMP\${PRODUCT_NAME}\GsSvc.dll" 0 +2
  System::Call '$TEMP\${PRODUCT_NAME}\GsSvc::SetupInstallService() ?u'
  
  ;上报统计
  SetOutPath "$TEMP\${PRODUCT_NAME}"
  ${GetParameters} $R1
  ${GetOptions} $R1 "/source"  $R0
  IfErrors 0 +2
  StrCpy $R0 $str_ChannelID
  ;是否静默安装
  ${GetParameters} $R1
  ${GetOptions} $R1 "/s"  $R2
  StrCpy $R3 "0"
  IfErrors 0 +2
  StrCpy $R3 "1"
  System::Call '$TEMP\${PRODUCT_NAME}\DsSetUpHelper::SendAnyHttpStat(t "install", t "${VERSION_LASTNUMBER}", t "$R0", i 1) '
  System::Call '$TEMP\${PRODUCT_NAME}\DsSetUpHelper::SendAnyHttpStat(t "installmethod", t "${VERSION_LASTNUMBER}", t "$R3", i 1) '
  System::Call '$TEMP\${PRODUCT_NAME}\DsSetUpHelper::Send2LvdunAnyHttpStat(t "install", t "$R0")'
  ;写入自用的注册表信息
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallSource" $str_ChannelID
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir" "$INSTDIR"
  System::Call '$TEMP\${PRODUCT_NAME}\DsSetUpHelper::GetTime(*l) i(.r0).r1'
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallTimes" "$0"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "Path" "$INSTDIR\program\${PRODUCT_NAME}.exe"
  
  ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "PeerId"
  ${If} $0 == ""
	System::Call '$TEMP\${PRODUCT_NAME}\DsSetUpHelper::GetPeerID(t) i(.r0).r1'
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "PeerId" "$0"
  ${EndIf}
  
  
  ;写入通用的注册表信息
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\program\${PRODUCT_NAME}.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\program\${PRODUCT_NAME}.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  ;SetOutPath "$INSTDIR"
  ;WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
FunctionEnd

Function CmdSilentInstall
	${GetParameters} $R1
	${GetOptions} $R1 "/s"  $R0
	IfErrors FunctionReturn 0
	SetSilent silent
	;SetOutPath "$TEMP\${PRODUCT_NAME}"
	;SetOverwrite on
	;File "bin\DsSetUpHelper.dll"
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	${If} $0 != ""
		StrCpy $INSTDIR "$0"
	${EndIf}
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "Path"
	IfFileExists $0 0 StartInstall
		;System::Call 'DsSetUpHelper::GetFileVersionString(t $0, t) i(r0, .r1).r2'
		${GetFileVersion} $0 $1
		${VersionCompare} $1 ${PRODUCT_VERSION} $2
		${If} $2 == "2" ;已安装的版本低于该版本
			Goto StartInstall
		${Else}
			${GetParameters} $R1
			${GetOptions} $R1 "/write"  $R0
			IfErrors 0 +2
			Abort
			Goto StartInstall
		${EndIf}
	StartInstall:
	
	;发退出消息
	Call CloseExe
	 ;先卸载旧的
	;ReadRegStr $4 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	;${If} $4 != 0
	;${AndIf} $4 != ""
	;IfFileExists $4 0 NextAction
	;StrCpy $1 $4
	;Call UnstallOnlyFile
	;NextAction:
	;${EndIf}
	Call DoInstall
	SetOutPath "$INSTDIR"
	CreateDirectory "$SMPROGRAMS\${SHORTCUT_NAME}"
	CreateShortCut "$SMPROGRAMS\${SHORTCUT_NAME}\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startmenuprograms"
	CreateShortCut "$SMPROGRAMS\${SHORTCUT_NAME}\卸载${SHORTCUT_NAME}.lnk" "$INSTDIR\uninst.exe"
	;锁定到任务栏
	ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
	${VersionCompare} "$R0" "6.0" $2
	${if} $2 == 2
		CreateShortCut "$QUICKLAUNCH\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom toolbar"
		CreateShortCut "$STARTMENU\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startbar"
		SetOutPath "$TEMP\${PRODUCT_NAME}"
		IfFileExists "$TEMP\${PRODUCT_NAME}\DsSetUpHelper.dll" 0 +2
		System::Call '$TEMP\${PRODUCT_NAME}\DsSetUpHelper::PinToStartMenu4XP(b true, t "$STARTMENU\${SHORTCUT_NAME}.lnk")'
		SetOutPath "$INSTDIR"
	${else}
		ExecShell taskbarunpin "$DESKTOP\${SHORTCUT_NAME}.lnk" "/sstartfrom toolbar"
		CreateShortCut "$DESKTOP\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom toolbar"
		ExecShell taskbarpin "$DESKTOP\${SHORTCUT_NAME}.lnk" "/sstartfrom toolbar"
		;锁定到开始菜单栏
		ExecShell startunpin "$DESKTOP\${SHORTCUT_NAME}.lnk" "/sstartfrom startbar"
		CreateShortCut "$DESKTOP\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startbar"
		ExecShell startpin "$DESKTOP\${SHORTCUT_NAME}.lnk" "/sstartfrom startbar"
		; 创建开始菜单快捷方式
		CreateShortCut "$STARTMENU\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startmenu"
	${Endif}
	
	;桌面快捷方式
	CreateShortCut "$DESKTOP\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom desktop"
	;静默安装也写开机启动
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME}" '"$INSTDIR\program\${PRODUCT_NAME}.exe" /embedding'
	${GetParameters} $R1
	${GetOptions} $R1 "/run"  $R0
	IfErrors +7 0
	${If} $R0 == ""
	${OrIf} $R0 == 0
		StrCpy $R0 "/embedding"
	${EndIf}
	SetOutPath "$INSTDIR\program"
	ExecShell open "${PRODUCT_NAME}.exe" "$R0 /sstartfrom installfinish" SW_SHOWNORMAL
	Abort
	FunctionReturn:
FunctionEnd

Function UnstallOnlyFile
	;删除
	RMDir /r "$1\appimage"
	RMDir /r "$1\xar"
	Delete "$1\uninst.exe"
	RMDir /r "$1\program"
	RMDir /r "$1\res"
	
	 ;文件被占用则改一下名字
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
	;发退出消息
	Call CloseExe
	ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
	${VersionCompare} "$R0" "6.0" $2
	${if} $2 == 2
		Delete "$QUICKLAUNCH\${SHORTCUT_NAME}.lnk"
		SetOutPath "$TEMP\${PRODUCT_NAME}"
		IfFileExists "$TEMP\${PRODUCT_NAME}\DsSetUpHelper.dll" 0 +2
		System::Call '$TEMP\${PRODUCT_NAME}\DsSetUpHelper::PinToStartMenu4XP(b 0, t "$STARTMENU\${SHORTCUT_NAME}.lnk")'
	${else}
		ExecShell taskbarunpin "$DESKTOP\${SHORTCUT_NAME}.lnk"
		;解除锁定到开始菜单栏
		ExecShell startunpin "$DESKTOP\${SHORTCUT_NAME}.lnk"
	${Endif}
	;最先卸载服务
	IfFileExists "$TEMP\${PRODUCT_NAME}\GsSvc.dll" 0 +2
    System::Call '$TEMP\${PRODUCT_NAME}\GsSvc::SetupUninstallService() ?u'
	StrCpy $1 $INSTDIR
	Call UnstallOnlyFile
	;读取渠道号
	ReadRegStr $R4 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallSource"
	${If} $R4 != ""
	${AndIf} $R4 != 0
		StrCpy $str_ChannelID $R4
	${EndIF}
	
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	IfFileExists "$TEMP\${PRODUCT_NAME}\DsSetUpHelper.dll" 0 +2
	System::Call '$TEMP\${PRODUCT_NAME}\DsSetUpHelper::SendAnyHttpStat(t "uninstall", t "${VERSION_LASTNUMBER}", t "$str_ChannelID", i 1) '
	System::Call '$TEMP\${PRODUCT_NAME}\DsSetUpHelper::Send2LvdunAnyHttpStat(t "uninstall", t "$str_ChannelID")'
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	${If} $0 == "$INSTDIR"
		DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
		DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
		 ;删除自用的注册表信息
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
		${WordReplace} "$R1" "GsSetup_" "" "+" $R3
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
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "LVDUNSETUP_INSTALL_MUTEX") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +2
	Abort
	StrCpy $Int_FontOffset 4
	CreateFont $Handle_Font "宋体" 10 0
	IfFileExists "$FONTS\msyh.ttf" 0 +3
	CreateFont $Handle_Font "微软雅黑" 10 0
	StrCpy $Int_FontOffset 0
	
	Call UpdateChanel
	
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	SetOverwrite on
	File "bin\DsSetUpHelper.dll"
	File "input_main\program\Microsoft.VC90.CRT.manifest"
	File "input_main\program\msvcp90.dll"
	File "input_main\program\msvcr90.dll"
	File "input_main\program\GsSvc.dll"
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
    File `/ONAME=$PLUGINSDIR\bg.bmp` `images\bg.bmp`
	File `/ONAME=$PLUGINSDIR\btn_next.bmp` `images\btn_next.bmp`
	File `/oname=$PLUGINSDIR\btn_agreement1.bmp` `images\btn_agreement1.bmp`
	File `/oname=$PLUGINSDIR\btn_agreement2.bmp` `images\btn_agreement2.bmp`
	File `/oname=$PLUGINSDIR\checkbox1.bmp` `images\checkbox1.bmp`
	File `/oname=$PLUGINSDIR\checkbox2.bmp` `images\checkbox2.bmp`
	File `/oname=$PLUGINSDIR\btn_min.bmp` `images\btn_min.bmp`
	File `/oname=$PLUGINSDIR\btn_close.bmp` `images\btn_close.bmp`
	File `/oname=$PLUGINSDIR\btn_change.bmp` `images\btn_change.bmp`
	File `/oname=$PLUGINSDIR\edit_bg.bmp` `images\edit_bg.bmp`
	File `/oname=$PLUGINSDIR\btn_install.bmp` `images\btn_install.bmp`
	File `/oname=$PLUGINSDIR\btn_return.bmp` `images\btn_return.bmp`
	File `/oname=$PLUGINSDIR\quit.bmp` `images\quit.bmp`
	File `/oname=$PLUGINSDIR\btn_quitsure.bmp` `images\btn_quitsure.bmp`
	File `/oname=$PLUGINSDIR\btn_quitreturn.bmp` `images\btn_quitreturn.bmp`   	
   	File `/oname=$PLUGINSDIR\loading1.bmp` `images\loading1.bmp`
    File `/oname=$PLUGINSDIR\loading2.bmp` `images\loading2.bmp`
	File `/oname=$PLUGINSDIR\loading_head.bmp` `images\loading_head.bmp`
	File `/oname=$PLUGINSDIR\loading_finish.bmp` `images\loading_finish.bmp`
	File `/oname=$PLUGINSDIR\btn_freeuse.bmp` `images\btn_freeuse.bmp`
    
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
    SetCtlColors $Hwnd_MsgBox ""  transparent ;背景设成透明

    ${NSW_SetWindowSize} $HWNDPARENT 300 130 ;改变窗体大小
    ${NSW_SetWindowSize} $Hwnd_MsgBox 300 130 ;改变Page大小
	System::Call  'User32::GetDesktopWindow() i.r8'
	${NSW_CenterWindow} $HWNDPARENT $8
	
	
	${NSD_CreateButton} 123 94 71 26 ''
	Pop $btn_MsgBoxSure
	StrCpy $1 $btn_MsgBoxSure
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_quitsure.bmp $1
	SkinBtn::onClick $1 $R7

	${NSD_CreateButton} 219 94 71 26 ''
	Pop $btn_MsgBoxCancel
	StrCpy $1 $btn_MsgBoxCancel
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_quitreturn.bmp $1
	GetFunctionAddress $0 OnClickQuitOK
	SkinBtn::onClick $1 $0
	
	StrCpy $3 45
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 66 $3 250 20 $R6
	Pop $lab_MsgBoxText
    SetCtlColors $lab_MsgBoxText "${TEXTCOLOR}" transparent ;背景设成透明
	SendMessage $lab_MsgBoxText ${WM_SETFONT} $Handle_Font 0
	
	StrCpy $3 65
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 66 $3 250 20 $R8
	Pop $lab_MsgBoxText2
    SetCtlColors $lab_MsgBoxText2 "${TEXTCOLOR}" transparent ;背景设成透明
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
	
	ShowWindow $HWNDPARENT ${SW_SHOW}
	ShowWindow $Hwnd_MsgBox ${SW_SHOW}
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
	;发退出消息
	FindWindow $9 "{B239B46A-6EDA-4a49-8CEE-E57BB352F933}_dsmainmsg"
	${If} $9 != 0
		StrCpy $R6 "检测${PRODUCT_NAME}.exe正在运行，"
		StrCpy $R8 "是否强制结束？"
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
	${If} $2 == "2" ;已安装的版本低于该版本
		Call ClickSure1
	${ElseIf} $2 == "0" ;版本相同
	${OrIf} $2 == "1"	;已安装的版本高于该版本
		 ${If} $R0 == "false"
			StrCpy $R6 "检测到已安装${SHORTCUT_NAME} $1，"
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

;下一步按钮事件
Function onClickNext
	Call OnClick_Install
FunctionEnd

;协议按钮事件
Function onClickAgreement
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	ExecShell open license.txt /x SW_SHOWNORMAL
FunctionEnd

;-----------------------------------------皮肤贴图方法----------------------------------------------------
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
		StrCpy $R1 "GreenShield"
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
			MessageBox MB_OK|MB_ICONSTOP "磁盘剩余空间不足，需要至少${NeedSpace}KB"
			EnableWindow $Btn_Install 0
			EnableWindow $Btn_Return 0
		EndFunction:
	${EndIf}
FunctionEnd

Function onClickZidingyi
	ShowWindow $Btn_Next ${SW_HIDE}
	ShowWindow $Btn_Agreement ${SW_HIDE}
	ShowWindow $Lbl_Xieyi ${SW_HIDE}
	ShowWindow $ck_xieyi ${SW_HIDE}
	ShowWindow $Btn_Zidingyi ${SW_HIDE}
	ShowWindow $Edit_BrowserBg 1
	ShowWindow $Txt_Browser 1
	ShowWindow $Btn_Browser 1
	ShowWindow $Lbl_Path 1
	ShowWindow $ck_DeskTopLink 1
	ShowWindow $Lbl_DeskTopLink 1
	ShowWindow $ck_StartTimeDo 1
	ShowWindow $Lbl_StartTimeDo 1
	ShowWindow $Btn_Install 1
	ShowWindow $Btn_Return 1
FunctionEnd

Function OnClick_Return
	ShowWindow $Btn_Next 1
	ShowWindow $Btn_Agreement 1
	ShowWindow $Lbl_Xieyi 1
	ShowWindow $ck_xieyi 1
	ShowWindow $Btn_Zidingyi 1
	ShowWindow $Edit_BrowserBg ${SW_HIDE}
	ShowWindow $Txt_Browser ${SW_HIDE}
	ShowWindow $Btn_Browser ${SW_HIDE}
	ShowWindow $Lbl_Path ${SW_HIDE}
	ShowWindow $ck_DeskTopLink ${SW_HIDE}
	ShowWindow $Lbl_DeskTopLink ${SW_HIDE}
	ShowWindow $ck_StartTimeDo ${SW_HIDE}
	ShowWindow $Lbl_StartTimeDo ${SW_HIDE}
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

	${NSW_SetWindowSize} $WarningForm 300 130
	EnableWindow $hwndparent 0
	System::Call `user32::SetWindowLong(i $WarningForm,i ${GWL_STYLE},0x9480084C)i.R0`
	${NSW_CreateButton} 123 94 71 26 ''
	Pop $R0
	StrCpy $1 $R0
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_quitsure.bmp $1
	${NSW_OnClick} $R0 OnClickQuitOK

	${NSW_CreateButton} 219 94 71 26 ''
	Pop $R0
	StrCpy $1 $R0
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_quitreturn.bmp $1
	${NSW_OnClick} $R0 OnClickQuitCancel

	StrCpy $3 54
	IntOp $3 $3 + $Int_FontOffset
	${NSW_CreateLabel} 62 $3 250 20 "您确定要退出绿盾广告管家安装程序？"
	Pop $4
    SetCtlColors $4 "${TEXTCOLOR}" transparent ;背景设成透明
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
	Abort
FunctionEnd

Function OnClickQuitOK
	System::Call 'kernel32::GetModuleFileName(i 0, t R2R2, i 256)'
	Push $R2
	Push "\"
	Call GetLastPart
	Pop $R1
	${If} $R1 == ""
		StrCpy $R1 ${EM_OUTFILE_NAME} 
	${EndIf}
	FindProcDLL::FindProc $R1
	${If} $R0 != 0
		KillProcDLL::KillProc $R1
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
	${Else}
		StrCpy $8 ""
		${DriveSpace} $7 "/D=F /S=K" $8
		${If} $8 == ""
			MessageBox MB_OK|MB_ICONSTOP "路径不合法"
			Abort
		${EndIf}
		IntCmp ${NeedSpace} $8 0 0 ErrorChunk
		StrCpy $R9 1 ;Goto the next page
		Call RelGotoPage
		Goto EndFunction
		ErrorChunk:
			MessageBox MB_OK|MB_ICONSTOP "磁盘剩余空间不足，需要至少${NeedSpace}KB"
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
    SetCtlColors $Hwnd_Welcome ""  transparent ;背景设成透明

    ${NSW_SetWindowSize} $HWNDPARENT 478 320 ;改变窗体大小
    ${NSW_SetWindowSize} $Hwnd_Welcome 478 320 ;改变Page大小
	
	System::Call  'User32::GetDesktopWindow() i.R9'
	${NSW_CenterWindow} $HWNDPARENT $R9
	
    ;一键安装
	StrCpy $Bool_IsExtend 0
    ${NSD_CreateButton} 180 222 117 35 ""
	Pop $Btn_Next
	StrCpy $1 $Btn_Next
	Call SkinBtn_Next
	GetFunctionAddress $3 onClickNext
    SkinBtn::onClick $1 $3
    
	;勾选同意协议
	${NSD_CreateButton} 11 285 15 15 ""
	Pop $ck_xieyi
	StrCpy $1 $ck_xieyi
	SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $1
	GetFunctionAddress $3 OnClick_CheckXieyi
    SkinBtn::onClick $1 $3
	StrCpy $Bool_Xieyi 1
	
	StrCpy $3 283
	IntOp $3 $3 + $Int_FontOffset
    ${NSD_CreateLabel} 30 $3 180 20 "我已阅读并同意绿盾广告管家"
    Pop $Lbl_Xieyi
	${NSD_OnClick} $Lbl_Xieyi OnClick_CheckXieyi
    SetCtlColors $Lbl_Xieyi "${TEXTCOLOR}" transparent ;背景设成透明
	SendMessage $Lbl_Xieyi ${WM_SETFONT} $Handle_Font 0
	
    ;用户协议
	${NSD_CreateButton} 201 286 68 15 ""
	Pop $Btn_Agreement
	StrCpy $1 $Btn_Agreement
	Call SkinBtn_Agreement1
	GetFunctionAddress $3 onClickAgreement
	SkinBtn::onClick $1 $3
		
	;自定义安装
	${NSD_CreateButton} 385 286 81 15 ""
	Pop $Btn_Zidingyi
	StrCpy $1 $Btn_Zidingyi
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_agreement2.bmp $1
	GetFunctionAddress $3 onClickZidingyi
	SkinBtn::onClick $1 $3
	
	;最小化
	${NSD_CreateButton} 438 5 13 13 ""
	Pop $Btn_Zuixiaohua
	StrCpy $1 $Btn_Zuixiaohua
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_min.bmp $1
	GetFunctionAddress $3 onClickZuixiaohua
	SkinBtn::onClick $1 $3
	;关闭
	${NSD_CreateButton} 457 5 13 13 ""
	Pop $Btn_Guanbi
	StrCpy $1 $Btn_Guanbi
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_close.bmp $1
	GetFunctionAddress $3 onClickGuanbi
	SkinBtn::onClick $1 $3
	
	
	;目录选择框
	${NSD_CreateDirRequest} 17 241 382 23 "$INSTDIR"
 	Pop	$Txt_Browser
	SendMessage $Txt_Browser ${WM_SETFONT} $Handle_Font 0
 	${NSD_OnChange} $Txt_Browser OnChange_DirRequest
	ShowWindow $Txt_Browser ${SW_HIDE}
	;EnableWindow $Txt_Browser 0
	;目录选择按钮
	${NSD_CreateBrowseButton} 399 239 63 26 ""
 	Pop	$Btn_Browser
 	StrCpy $1 $Btn_Browser
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_change.bmp $1
	GetFunctionAddress $3 OnClick_BrowseButton
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_Browser ${SW_HIDE}
	;目录选择框背景
	 ${NSD_CreateBitmap} 16 240 446 25 ""
    Pop $Edit_BrowserBg
    ${NSD_SetImage} $Edit_BrowserBg $PLUGINSDIR\edit_bg.bmp $ImageHandle
	ShowWindow $Edit_BrowserBg ${SW_HIDE}
	;路径选择文字描述
	StrCpy $3 218
	IntOp $3 $3 + $Int_FontOffset
	 ${NSD_CreateLabel} 16 $3 250 18 "安装位置："
    Pop $Lbl_Path
    SetCtlColors $Lbl_Path "${TEXTCOLOR}" transparent ;背景设成透明
	ShowWindow $Lbl_Path ${SW_HIDE}
	SendMessage $Lbl_Path ${WM_SETFONT} $Handle_Font 0
	
	;添加桌面快捷方式
	${NSD_CreateButton} 16 278 15 15 ""
	Pop $ck_DeskTopLink
	StrCpy $1 $ck_DeskTopLink
	SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $1
	GetFunctionAddress $3 OnClick_CheckDeskTopLink
    SkinBtn::onClick $1 $3
	StrCpy $Bool_DeskTopLink 1
	
	StrCpy $3 276
	IntOp $3 $3 + $Int_FontOffset
    ${NSD_CreateLabel} 36 $3 120 18 "添加桌面快捷方式"
    Pop $Lbl_DeskTopLink
	${NSD_OnClick} $Lbl_DeskTopLink OnClick_CheckDeskTopLink
    SetCtlColors $Lbl_DeskTopLink "${TEXTCOLOR}" transparent ;背景设成透明
	ShowWindow $ck_DeskTopLink ${SW_HIDE}
	ShowWindow $Lbl_DeskTopLink ${SW_HIDE}
	SendMessage $Lbl_DeskTopLink ${WM_SETFONT} $Handle_Font 0
	
	;开启实时过滤
	${NSD_CreateButton} 166 278 15 15 ""
	Pop $ck_StartTimeDo
	StrCpy $1 $ck_StartTimeDo
	SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $1
	GetFunctionAddress $3 OnClick_CheckStartTimeDo
    SkinBtn::onClick $1 $3
	StrCpy $Bool_StartTimeDo 1
	
	StrCpy $3 276
	IntOp $3 $3 + $Int_FontOffset
    ${NSD_CreateLabel} 186 $3 100 18 "开启实时监控"
    Pop $Lbl_StartTimeDo
	${NSD_OnClick} $Lbl_StartTimeDo OnClick_CheckStartTimeDo
    SetCtlColors $Lbl_StartTimeDo "${TEXTCOLOR}" transparent ;背景设成透明
	ShowWindow $ck_StartTimeDo ${SW_HIDE}
	ShowWindow $Lbl_StartTimeDo ${SW_HIDE}
	SendMessage $Lbl_StartTimeDo ${WM_SETFONT} $Handle_Font 0
	
	;立即安装
	${NSD_CreateButton} 316 286 76 26 ""
 	Pop	$Btn_Install
 	StrCpy $1 $Btn_Install
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_install.bmp $1
	GetFunctionAddress $3 OnClick_Install
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_Install ${SW_HIDE}
	
	;返回
	${NSD_CreateButton} 401 286 61 26 ""
 	Pop	$Btn_Return
 	StrCpy $1 $Btn_Return
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_return.bmp $1
	GetFunctionAddress $3 OnClick_Return
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_Return ${SW_HIDE}
	
	
	GetFunctionAddress $0 onGUICallback
    ;贴背景大图
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\bg.bmp $ImageHandle
	
	WndProc::onCallback $BGImage $0 ;处理无边框窗体移动
	
	GetFunctionAddress $0 onCloseCallback
	WndProc::onCallback $HWNDPARENT $0 ;处理关闭消息
	
	ShowWindow $HWNDPARENT ${SW_SHOW}
	nsDialogs::Show
	${NSD_FreeImage} $ImageHandle
FunctionEnd

Var ck_bind360install
Var lab_bind360install
Var Bool_bind360install
Function OnClick_bind360install
	${IF} $Bool_bind360install == 1
        IntOp $Bool_bind360install $Bool_bind360install - 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox1.bmp $ck_bind360install
    ${ELSE}
        IntOp $Bool_bind360install $Bool_bind360install + 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $ck_bind360install
    ${EndIf}
	ShowWindow $ck_bind360install ${SW_HIDE}
	ShowWindow $ck_bind360install ${SW_SHOW}
FunctionEnd

Var Handle_Loading
Function NSD_TimerFun
	GetFunctionAddress $0 NSD_TimerFun
    nsDialogs::KillTimer $0
	;先去掉快捷栏和开始菜单栏
	;ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
	${VersionCompare} "$R0" "6.0" $2
	SetOutPath "$INSTDIR"
	${if} $2 == 2
		Delete "$QUICKLAUNCH\${SHORTCUT_NAME}.lnk"
		
		SetOutPath "$TEMP\${PRODUCT_NAME}"
		IfFileExists "$TEMP\${PRODUCT_NAME}\DsSetUpHelper.dll" 0 +2
		System::Call "$TEMP\${PRODUCT_NAME}\DsSetUpHelper::PinToStartMenu4XP(b 0, t '$STARTMENU\${SHORTCUT_NAME}.lnk')"
	${else}
		ExecShell taskbarunpin "$DESKTOP\${SHORTCUT_NAME}.lnk"
		;解除锁定到开始菜单栏
		ExecShell startunpin "$DESKTOP\${SHORTCUT_NAME}.lnk"
	${Endif}
	IfFileExists "$DESKTOP\${SHORTCUT_NAME}.lnk" 0 +2
	Delete "$DESKTOP\${SHORTCUT_NAME}.lnk"
    !if 1   ;是否在后台运行,1有效
        GetFunctionAddress $0 InstallationMainFun
        BgWorker::CallAndWait
    !else
        Call InstallationMainFun
    !endif
	;主线程中创建快捷方式
	${If} $Bool_DeskTopLink == 1
		ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
		${VersionCompare} "$R0" "6.0" $2
		SetOutPath "$INSTDIR"
		;快速启动栏
		${if} $2 == 2
			CreateShortCut "$QUICKLAUNCH\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom toolbar"
			CreateShortCut "$STARTMENU\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startbar"
			SetOutPath "$TEMP\${PRODUCT_NAME}"
			IfFileExists "$TEMP\${PRODUCT_NAME}\DsSetUpHelper.dll" 0 +2
			System::Call '$TEMP\${PRODUCT_NAME}\DsSetUpHelper::PinToStartMenu4XP(b true, t "$STARTMENU\${SHORTCUT_NAME}.lnk")'
			SetOutPath "$INSTDIR"
		${else}
			ExecShell taskbarunpin "$DESKTOP\${SHORTCUT_NAME}.lnk" "/sstartfrom toolbar"
			CreateShortCut "$DESKTOP\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom toolbar"
			ExecShell taskbarpin "$DESKTOP\${SHORTCUT_NAME}.lnk" "/sstartfrom toolbar"
			;锁定到开始菜单栏
			ExecShell startunpin "$DESKTOP\${SHORTCUT_NAME}.lnk" "/sstartfrom startbar"
			CreateShortCut "$DESKTOP\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startbar"
			ExecShell startpin "$DESKTOP\${SHORTCUT_NAME}.lnk" "/sstartfrom startbar"
			; 创建开始菜单快捷方式
			CreateShortCut "$STARTMENU\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startmenu"
		${Endif}
		CreateShortCut "$DESKTOP\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom desktop"
	${EndIf}
	${If} $Bool_StartTimeDo == 1
		 WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME}" '"$INSTDIR\program\${PRODUCT_NAME}.exe" /embedding'
	${EndIf}
	CreateDirectory "$SMPROGRAMS\${SHORTCUT_NAME}"
	CreateShortCut "$SMPROGRAMS\${SHORTCUT_NAME}\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe" "/sstartfrom startmenuprograms"
	CreateShortCut "$SMPROGRAMS\${SHORTCUT_NAME}\卸载${SHORTCUT_NAME}.lnk" "$INSTDIR\uninst.exe"
	;最后才显示安装完成界面
	;ShowWindow $HWNDPARENT ${SW_HIDE}
	HideWindow
	ShowWindow $Handle_Loading ${SW_HIDE}
	;${NSW_SetWindowSize} $HWNDPARENT 0 0 ;改变自定义窗体大小
	ShowWindow $Lbl_Sumary ${SW_HIDE}
	ShowWindow $PB_ProgressBar ${SW_HIDE}
	;ShowWindow $BGImage ${SW_HIDE}
	ShowWindow $Btn_Guanbi ${SW_SHOW}
	
	ShowWindow $Bmp_Finish ${SW_SHOW}
	ShowWindow $Btn_FreeUse ${SW_SHOW}
	ShowWindow $ck_bind360install ${SW_SHOW}
	ShowWindow $lab_bind360install ${SW_SHOW}
	;${NSW_SetWindowSize} $HWNDPARENT 478 320 ;改变自定义窗体大小
	ShowWindow $Handle_Loading ${SW_SHOW}
	ShowWindow $HWNDPARENT ${SW_SHOW}
FunctionEnd

Function InstallationMainFun
    SendMessage $PB_ProgressBar ${PBM_SETRANGE32} 0 100  ;总步长为顶部定义值
	Sleep 100
	Call CloseExe
	SendMessage $PB_ProgressBar ${PBM_SETPOS} 2 0
	Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 4 0
     ;先卸载旧的
	;ReadRegStr $4 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	;${If} $4 != 0
	;${AndIf} $4 != ""
	;IfFileExists $4 0 NextAction
	;StrCpy $1 $4
	;Call UnstallOnlyFile
	;NextAction:
	;${EndIf}
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
FunctionEnd

Function OnClick_FreeUse
	SetOutPath "$INSTDIR\program"
	ExecShell open "${PRODUCT_NAME}.exe" "/forceshow /sstartfrom installfinish" SW_SHOWNORMAL
	${IF} $Bool_bind360install == 1
		HideWindow
		SetOutPath "$TEMP\${PRODUCT_NAME}"
		System::Call '$TEMP\${PRODUCT_NAME}\DsSetUpHelper::DownLoadBundledSoftware() i.r1'
		Call OnClickQuitOK
	${ELSE}
		Call OnClickQuitOK
	${EndIf}
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
		Abort
	${EndIf}
	SetCtlColors $Handle_Loading ""  transparent ;背景设成透明

	${NSW_SetWindowSize} $HWNDPARENT 478 320 ;改变自定义窗体大小
	${NSW_SetWindowSize} $Handle_Loading 478 320 ;改变自定义Page大小
	
	;System::Call  'User32::GetDesktopWindow() i.R9'
	;${NSW_CenterWindow} $HWNDPARENT $R9


    ;创建简要说明
	StrCpy $3 253
	IntOp $3 $3 + $Int_FontOffset
    ${NSD_CreateLabel} 24 $3 57 20 "正在安装"
    Pop $Lbl_Sumary
    SetCtlColors $Lbl_Sumary "${TEXTCOLOR}"  transparent ;背景设成透明
	SendMessage $Lbl_Sumary ${WM_SETFONT} $Handle_Font 0

    ${NSD_CreateProgressBar} 24 275 430 16 ""
    Pop $PB_ProgressBar
    SkinProgress::Set $PB_ProgressBar "$PLUGINSDIR\loading2.bmp" "$PLUGINSDIR\loading1.bmp"
    GetFunctionAddress $0 NSD_TimerFun
    nsDialogs::CreateTimer $0 1
    
	
	;完成时"免费使用"按钮
	${NSD_CreateButton} 180 240 117 37 ""
 	Pop	$Btn_FreeUse
 	StrCpy $1 $Btn_FreeUse
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_freeuse.bmp $1
	GetFunctionAddress $3 OnClick_FreeUse
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_FreeUse ${SW_HIDE}
	
	;捆绑安装360
	${NSD_CreateButton} 165 285 15 15 ""
	Pop $ck_bind360install
	StrCpy $1 $ck_bind360install
	SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $1
	GetFunctionAddress $3 OnClick_bind360install
    SkinBtn::onClick $1 $3
	StrCpy $Bool_bind360install 1
	
	StrCpy $3 283
	IntOp $3 $3 + $Int_FontOffset
    ${NSD_CreateLabel} 185 $3 150 18 "推荐使用360安全套装"
    Pop $lab_bind360install
	${NSD_OnClick} $lab_bind360install OnClick_bind360install
    SetCtlColors $lab_bind360install "${TEXTCOLOR}" transparent ;背景设成透明
	ShowWindow $ck_bind360install ${SW_HIDE}
	ShowWindow $lab_bind360install ${SW_HIDE}
	SendMessage $lab_bind360install ${WM_SETFONT} $Handle_Font 0
	
	
	;关闭
	${NSD_CreateButton} 457 5 13 13 ""
	Pop $Btn_Guanbi
	StrCpy $1 $Btn_Guanbi
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_close.bmp $1
	GetFunctionAddress $3 OnClickQuitOK
	SkinBtn::onClick $1 $3
	ShowWindow $Btn_Guanbi ${SW_HIDE}
	
	
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
	
	GetFunctionAddress $0 onCloseCallback
	WndProc::onCallback $HWNDPARENT $0 ;处理关闭消息
    
	ShowWindow $HWNDPARENT ${SW_SHOW}
	nsDialogs::Show
    ${NSD_FreeImage} $ImageHandle
FunctionEnd


/****************************************************/
;卸载
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
	;ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	;${If} $0 == 0
	;${OrIf} $0 == ""
	;	Abort
	;${EndIf}
	;StrCpy $INSTDIR $0
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "LVDUNSETUP_INSTALL_MUTEX") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +2
	Abort
	
	IfFileExists "$INSTDIR\program\Microsoft.VC90.CRT.manifest" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\Microsoft.VC90.CRT.manifest" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$INSTDIR\program\msvcp90.dll" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\msvcp90.dll" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$INSTDIR\program\msvcr90.dll" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\msvcr90.dll" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$INSTDIR\program\GsSvc.dll" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\GsSvc.dll" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$INSTDIR\program\ATL90.dll" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\ATL90.dll" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$INSTDIR\program\Microsoft.VC90.ATL.manifest" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\Microsoft.VC90.ATL.manifest" "$TEMP\${PRODUCT_NAME}\"
	Goto +3
	InitFailed:
	Abort
	
	StrCpy $Int_FontOffset 4
	CreateFont $Handle_Font "宋体" 10 0
	IfFileExists "$FONTS\msyh.ttf" 0 +3
	CreateFont $Handle_Font "微软雅黑" 10 0
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
	;${If} $MSG = ${WM_CLOSE}
	;	Call un.OnClick_ContinueUse
	;${EndIf}
	Call un.HandlePageChange
FunctionEnd
Function un.myGUIInit
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
	;最先卸载服务
	IfFileExists "$TEMP\${PRODUCT_NAME}\GsSvc.dll" 0 +2
	System::Call '$TEMP\${PRODUCT_NAME}\GsSvc::SetupUninstallService() ?u'
	;删除
	RMDir /r "$INSTDIR\appimage"
	RMDir /r "$INSTDIR\xar"
	Delete "$INSTDIR\uninst.exe"
	RMDir /r "$INSTDIR\program"
	RMDir /r "$INSTDIR\res"

	 ;文件被占用则改一下名字
	StrCpy $R4 "$INSTDIR\program\GsNet32.dll"
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
	
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	${If} $0 == "$INSTDIR"
		DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
		DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
		 ;删除自用的注册表信息
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

Function un.OnClick_CruelRefused
	EnableWindow $Btn_CruelRefused 0
	EnableWindow $Btn_ContinueUse 0
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	SetOverwrite on
	File "bin\DsSetUpHelper.dll"
	IfFileExists "$TEMP\${PRODUCT_NAME}\DsSetUpHelper.dll" 0 +3
	System::Call '$TEMP\${PRODUCT_NAME}\DsSetUpHelper::SendAnyHttpStat(t "uninstall", t "${VERSION_LASTNUMBER}", t "$str_ChannelID", i 1) '
	System::Call '$TEMP\${PRODUCT_NAME}\DsSetUpHelper::Send2LvdunAnyHttpStat(t "uninstall", t "$str_ChannelID")'
	FindWindow $R0 "{B239B46A-6EDA-4a49-8CEE-E57BB352F933}_dsmainmsg"
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
		IfFileExists "$TEMP\${PRODUCT_NAME}\DsSetUpHelper.dll" 0 +2
		System::Call "$TEMP\${PRODUCT_NAME}\DsSetUpHelper::PinToStartMenu4XP(b 0, t '$STARTMENU\${SHORTCUT_NAME}.lnk')"
	${else}
		ExecShell taskbarunpin "$DESKTOP\${SHORTCUT_NAME}.lnk"
		;解除锁定到开始菜单栏
		ExecShell startunpin "$DESKTOP\${SHORTCUT_NAME}.lnk"
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
    SetCtlColors $Hwnd_MsgBox ""  transparent ;背景设成透明

    ${NSW_SetWindowSize} $HWNDPARENT 300 130 ;改变窗体大小
    ${NSW_SetWindowSize} $Hwnd_MsgBox 300 130 ;改变Page大小
	System::Call  'User32::GetDesktopWindow() i.r8'
	${NSW_CenterWindow} $HWNDPARENT $8
	
	
	${NSD_CreateButton} 123 94 71 26 ''
	Pop $btn_MsgBoxSure
	StrCpy $1 $btn_MsgBoxSure
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_quitsure.bmp $1
	SkinBtn::onClick $1 $R7

	${NSD_CreateButton} 219 94 71 26 ''
	Pop $btn_MsgBoxCancel
	StrCpy $1 $btn_MsgBoxCancel
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_quitreturn.bmp $1
	GetFunctionAddress $0 un.OnClick_FinishUnstall
	SkinBtn::onClick $1 $0
	
	StrCpy $3 45
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 66 $3 250 20 $R6
	Pop $lab_MsgBoxText
    SetCtlColors $lab_MsgBoxText "${TEXTCOLOR}" transparent ;背景设成透明
	SendMessage $lab_MsgBoxText ${WM_SETFONT} $Handle_Font 0
	
	StrCpy $3 65
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 66 $3 250 20 $R8
	Pop $lab_MsgBoxText2
    SetCtlColors $lab_MsgBoxText2 "${TEXTCOLOR}" transparent ;背景设成透明
	SendMessage $lab_MsgBoxText2 ${WM_SETFONT} $Handle_Font 0
	
	
	GetFunctionAddress $0 un.onGUICallback
    ;贴背景大图
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\quit.bmp $ImageHandle
	
	WndProc::onCallback $BGImage $0 ;处理无边框窗体移动
	
	GetFunctionAddress $0 un.onMsgBoxCloseCallback
	WndProc::onCallback $HWNDPARENT $0 ;处理关闭消息
	
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
	;发退出消息
	FindWindow $R0 "{B239B46A-6EDA-4a49-8CEE-E57BB352F933}_dsmainmsg"
	${If} $R0 != 0
		StrCpy $R6 "检测${PRODUCT_NAME}.exe正在运行，"
		StrCpy $R8 "是否强制结束？"
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
	SetCtlColors $0 ""  transparent ;背景设成透明

	${NSW_SetWindowSize} $HWNDPARENT 478 320 ;改变自定义窗体大小
	${NSW_SetWindowSize} $0 478 320 ;改变自定义Page大小
	
	System::Call 'user32::GetDesktopWindow()i.R9'
	${NSW_CenterWindow} $HWNDPARENT $R9

	;继续使用
	${NSD_CreateButton} 246 235 95 30 ""
 	Pop	$Btn_ContinueUse
 	StrCpy $1 $Btn_ContinueUse
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_jixushiyong.bmp $1
	GetFunctionAddress $3 un.OnClick_ContinueUse
    SkinBtn::onClick $1 $3
	
	;残忍卸载
	${NSD_CreateButton} 355 235 95 30 ""
 	Pop	$Btn_CruelRefused
 	StrCpy $1 $Btn_CruelRefused
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_canrenxiezai.bmp $1
	GetFunctionAddress $3 un.OnClick_CruelRefused
    SkinBtn::onClick $1 $3
	
	;卸载完成
	${NSD_CreateButton} 190 263 95 30 ""
 	Pop	$Btn_FinishUnstall
 	StrCpy $1 $Btn_FinishUnstall
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_xiezaiwancheng.bmp $1
	GetFunctionAddress $3 un.OnClick_FinishUnstall
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_FinishUnstall ${SW_HIDE}
   
	
	GetFunctionAddress $0 un.onGUICallback  
	;卸载完成背景
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $Bmp_FinishUnstall
    ${NSD_SetImage} $Bmp_FinishUnstall $PLUGINSDIR\un_finishbg.bmp $ImageHandle
    WndProc::onCallback $Bmp_FinishUnstall $0 ;处理无边框窗体移动
	ShowWindow $Bmp_FinishUnstall ${SW_HIDE}
	
    ;贴背景大图
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $Bmp_StartUnstall
    ${NSD_SetImage} $Bmp_StartUnstall $PLUGINSDIR\un_startbg.bmp $ImageHandle
    WndProc::onCallback $Bmp_StartUnstall $0 ;处理无边框窗体移动
	
	GetFunctionAddress $0 un.onMsgBoxCloseCallback
	WndProc::onCallback $HWNDPARENT $0 ;处理关闭消息
    
	ShowWindow $HWNDPARENT ${SW_SHOW}
	nsDialogs::Show
    ${NSD_FreeImage} $ImageHandle
FunctionEnd