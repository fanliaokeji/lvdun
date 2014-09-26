Var MSG
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

;进度条界面
Var Lbl_Sumary
Var PB_ProgressBar
Var Bmp_Finish
Var Btn_FreeUse
;---------------------------全局编译脚本预定义的常量-----------------------------------------------------
; MUI 预定义常量
!define MUI_ABORTWARNING
;安装图标的路径名字
!define MUI_ICON "images\fav.ico"
;卸载图标的路径名字
!define MUI_UNICON "images\unis.ico"

!define PRODUCT_NAME "GreenShield"
!define EM_OUTFILE_NAME "${PRODUCT_NAME}安装包.exe"
!define PRODUCT_VERSION "3.5.1.910"
!define EM_BrandingText "${PRODUCT_NAME}${PRODUCT_VERSION}"
!define PRODUCT_PUBLISHER "My company, Inc."
!define PRODUCT_WEB_SITE "http://www.mycompany.com"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_NAME}.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_MAININFO_FORSELF "Software\${PRODUCT_NAME}"

;---------------------------设置软件压缩类型（也可以通过外面编译脚本控制）------------------------------------
SetCompressor lzma
BrandingText "${EM_BrandingText}"
SetCompress force
XPStyle on
; ------ MUI 现代界面定义 (1.67 版本以上兼容) ------
!include "MUI2.nsh"
!include "WinCore.nsh"
;引用文件函数头文件
!include "FileFunc.nsh"
!include "nsWindows.nsh"
!include "WinMessages.nsh"

!define MUI_CUSTOMFUNCTION_GUIINIT onGUIInit
!define MUI_CUSTOMFUNCTION_UNGUIINIT un.myGUIInit

!insertmacro MUI_LANGUAGE "SimpChinese"
SetFont 微软雅黑 10
!define TEXTCOLOR "4D4D4D"
RequestExecutionLevel highest


;自定义页面
Page custom WelcomePage
Page custom LoadingPage
UninstPage custom un.MyUnstall


;------------------------------------------------------MUI 现代界面定义以及函数结束------------------------
;应用程序显示名字
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
;应用程序输出路径
OutFile "bin\${EM_OUTFILE_NAME}"
InstallDir "$PROGRAMFILES\GreenShield"
InstallDirRegKey HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"

Function DoInstall
  SetOutPath "$INSTDIR"
  SetOverwrite on
  File /r "input_main\*.*"
  ;上报统计
  System::Call 'DsSetUpHelper::SendAnyHttpStat(t "1111", t "22222", t "33333", i 4) '
  ;写入自用的注册表信息
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallSource" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir" "$INSTDIR"
  System::Call 'DsSetUpHelper::GetTime(*l) i(.r0).r1'
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallTimes" "$0"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "Path" "$INSTDIR\ADSafe.exe"
  System::Call 'DsSetUpHelper::GetPeerID(t) i(.r0).r1'
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "PeerId" "$0"
  
  ;写入通用的注册表信息
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\${PRODUCT_NAME}.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\${PRODUCT_NAME}.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  SetOutPath "$INSTDIR"
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
FunctionEnd

;  Created 1/5/05 by Comperio
;
;	Usage:
;	${VersionCheck} "Version1" "Version2" "outputVar"
;		Version1 = 1st version number
;		Version2 = 2nd version number
;		outputVar = Variable used to store the ouput
;
;	Return values:
;		0 = both versions are equal
;		1 = Version 1 is NEWER than version 2
;		2 = Version1 is OLDER than version 2
;	Rules for Version Numbers:
;		Version numbers must always be a string of numbers only.  (A dot
;		in the name is optional). 
;
;	[Variables]
var P1	; file pointer, used to "remember" the position in the Version1 string
var P2	; file pointer, used to "remember" the position in the Version2 string
var V1	;version number from Version1
var V2	;version number from Version2
Var Reslt	; holds the return flag
 
 
;	[Macros]
!macro VersionCheck Ver1 Ver2 OutVar
	;	To make this work, one character must be added to the version string:
	Push "x${Ver2}"
	Push "x${Ver1}"
	Call VersionCheckF
	Pop ${OutVar}
 
!macroend
 
;	[Defines]
!define VersionCheck "!insertmacro VersionCheck"
 
;	[Functions]
Function VersionCheckF
	Exch $1 ; $1 contains Version 1
	Exch
	Exch $2 ; $2 contains Version 2
	Exch
	Push $R0
	;	initialize Variables
	StrCpy $V1 ""
	StrCpy $V2 ""
	StrCpy $P1 ""
	StrCpy $P2 ""
	StrCpy $Reslt ""
	;	Set the file pointers:
	IntOp $P1 $P1 + 1
	IntOp $P2 $P2 + 1
	;  ******************* Get 1st version number for Ver1 **********************
	V11:
	;	I use $1 and $2 to help keep identify "Ver1" vs. "Ver2"
	StrCpy $R0 $1 1 $P1 ;$R0 contains the character at position $P1
	IntOp $P1 $P1 + 1 	;increments the file pointer for the next read
	StrCmp $R0 "" V11end 0	;check for empty string
	strCmp $R0 "." v11end 0
	strCpy $V1 "$V1$R0"
	Goto V11
	V11End:
	StrCmp $V1 "" 0 +2
	StrCpy $V1 "0"	
	;  ******************* Get 1st version number for Ver2 **********************
	V12:
	StrCpy $R0 $2 1 $P2 ;$R0 contains the character at position $P1
	IntOp $P2 $P2 + 1 	;increments the file pointer for the next read
	StrCmp $R0 "" V12end 0	;check for empty string
	strCmp $R0 "." v12end 0
	strCpy $V2 "$V2$R0"
	Goto V12
	V12End:
	StrCmp $V2 "" 0 +2
	StrCpy $V2 "0"	
	;	At this point, we can compare the results.  If the numbers are not
	;		equal, then we can exit
	IntCmp $V1 $V2 cont1 older1 newer1
	older1: ; Version 1 is older (less than) than version 2
	StrCpy $Reslt 2
	Goto ExitFunction
	newer1:	; Version 1 is newer (greater than) Version 2
	StrCpy $Reslt 1
	Goto ExitFunction
	Cont1: ;Versions are the same.  Continue searching for differences
	;	Reset $V1 and $V2
	StrCpy $V1 ""
	StrCpy $V2 ""
 
	;  ******************* Get 2nd version number for Ver1 **********************	
	V21:
	StrCpy $R0 $1 1 $P1 ;$R0 contains the character at position $P1
	IntOp $P1 $P1 + 1 	;increments the file pointer for the next read
	StrCmp $R0 "" V21end 0	;check for empty string
	strCmp $R0 "." v21end 0
	strCpy $V1 "$V1$R0"
	Goto V21
	V21End:
	StrCmp $V1 "" 0 +2
	StrCpy $V1 "0"	
	;  ******************* Get 2nd version number for Ver2 **********************
	V22:
	StrCpy $R0 $2 1 $P2 ;$R0 contains the character at position $P1
	IntOp $P2 $P2 + 1 	;increments the file pointer for the next read
	StrCmp $R0 "" V22end 0	;check for empty string
	strCmp $R0 "." V22end 0
	strCpy $V2 "$V2$R0"
	Goto V22
	V22End:
	StrCmp $V2 "" 0 +2
	StrCpy $V2 "0"	
	;	At this point, we can compare the results.  If the numbers are not
	;		equal, then we can exit
	IntCmp $V1 $V2 cont2 older2 newer2
	older2: ; Version 1 is older (less than) than version 2
	StrCpy $Reslt 2 
	Goto ExitFunction
	newer2:	; Version 1 is newer (greater than) Version 2
	StrCpy $Reslt 1
	Goto ExitFunction
	Cont2: ;Versions are the same.  Continue searching for differences
	;	Reset $V1 and $V2
	StrCpy $V1 ""
	StrCpy $V2 ""	
	;  ******************* Get 3rd version number for Ver1 **********************	
	V31:
	StrCpy $R0 $1 1 $P1 ;$R0 contains the character at position $P1
	IntOp $P1 $P1 + 1 	;increments the file pointer for the next read
	StrCmp $R0 "" V31end 0	;check for empty string
	strCmp $R0 "." v31end 0
	strCpy $V1 "$V1$R0"
	Goto V31
	V31End:
	StrCmp $V1 "" 0 +2
	StrCpy $V1 "0"	
	;  ******************* Get 3rd version number for Ver2 **********************
	V32:
	StrCpy $R0 $2 1 $P2 ;$R0 contains the character at position $P1
	IntOp $P2 $P2 + 1 	;increments the file pointer for the next read
	StrCmp $R0 "" V32end 0	;check for empty string
	strCmp $R0 "." V32end 0
	strCpy $V2 "$V2$R0"
	Goto V32
	V32End:
	StrCmp $V2 "" 0 +2
	StrCpy $V2 "0"	
	;	At this point, we can compare the results.  If the numbers are not
	;		equal, then we can exit
	IntCmp $V1 $V2 cont3 older3 newer3
	older3: ; Version 1 is older (less than) than version 2
	StrCpy $Reslt 2
	Goto ExitFunction
	newer3:	; Version 1 is newer (greater than) Version 2
	StrCpy $Reslt 1
	Goto ExitFunction
	Cont3: ;Versions are the same.  Continue searching for differences
	;	Reset $V1 and $V2
	StrCpy $V1 ""
	StrCpy $V2 ""
	;  ******************* Get 4th version number for Ver1 **********************	
	V41:
	StrCpy $R0 $1 1 $P1 ;$R0 contains the character at position $P1
	IntOp $P1 $P1 + 1 	;increments the file pointer for the next read
	StrCmp $R0 "" V41end 0	;check for empty string
	strCmp $R0 "." v41end 0
	strCpy $V1 "$V1$R0"
	Goto V41
	V41End:
	StrCmp $V1 "" 0 +2
	StrCpy $V1 "0"	
	;  ******************* Get 4th version number for Ver2 **********************
	V42:
	StrCpy $R0 $2 1 $P2 ;$R0 contains the character at position $P1
	IntOp $P2 $P2 + 1 	;increments the file pointer for the next read
	StrCmp $R0 "" V42end 0	;check for empty string
	strCmp $R0 "." V42end 0
	strCpy $V2 "$V2$R0"
	Goto V42
	V42End:
	StrCmp $V2 "" 0 +2
	StrCpy $V2 "0"	
	;	At this point, we can compare the results.  If the numbers are not
	;		equal, then we can exit
	IntCmp $V1 $V2 cont4 older4 newer4
	older4: ; Version 1 is older (less than) than version 2
	StrCpy $Reslt 2
	Goto ExitFunction
	newer4:	; Version 1 is newer (greater than) Version 2
	StrCpy $Reslt 1
	Goto ExitFunction
	Cont4: 
	;Versions are the same.  We've reached the end of the version
	;	strings, so set the function to 0 (equal) and exit
	StrCpy $Reslt 0
	ExitFunction:
	Pop $R0
	Pop $1
	Pop $2
	Push $Reslt
FunctionEnd

Function .onInit
    System::Call 'kernel32::CreateMutexA(i 0, i 0, t "LVDUNSETUP_MUTEX") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +3
	MessageBox MB_OK|MB_ICONEXCLAMATION "Another Installer is Running!"
	Abort
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	File "bin\DsSetUpHelper.dll"
	File "license\license.txt"
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "Path"
	IfFileExists $0 0 StartInstall
		;System::Call 'DsSetUpHelper::GetFileVersionString(t $0, t) i(r0, .r1).r2'
		${GetFileVersion} $0 $1
		${VersionCheck} $1 ${PRODUCT_VERSION} $2
		${If} $2 == "2" ;已安装的版本低于该版本
			Goto StartInstall
		${ElseIf} $2 == "0" ;版本相同
			 MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "检测到已安装$(^Name)，是否覆盖安装？" IDYES StartInstall
			 Abort
		${ElseIf} $2 == "1"	;已安装的版本高于该版本
			MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "检测到已安装$(^Name)，是否覆盖安装？" IDYES StartInstall
			Abort
		${EndIf}
	StartInstall:
	FindProcDLL::FindProc "${PRODUCT_NAME}.exe"
    Pop $R0
    ${If} $R0 != 0
		 MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "检测${PRODUCT_NAME}.exe正在运行，是否强制结束？" IDYES KillExeBeforeInstall
		 Abort
		 KillExeBeforeInstall:
		 KillProcDLL::KillProc "${PRODUCT_NAME}.exe"
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

	nsDialogs::SelectFolderDialog "请选择 $R0 安装的文件夹:" "$R0"
	Pop $0
	${If} $0 == "error" # returns 'error' if 'cancel' was pressed?
		Return
	${EndIf}
	${If} $0 != ""
	StrCpy $INSTDIR "$0\$R1"
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
	Pop $0
	System::Call "user32::GetWindowText(i $Txt_Browser, t .r0, i ${NSIS_MAX_STRLEN})"
	MessageBox MB_ICONINFORMATION|MB_OK $0
	StrCpy $INSTDIR $0
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
	FindProcDLL::FindProc ${EM_OUTFILE_NAME}
    Sleep 100
    Pop $R0
    ${If} $R0 != 0
    KillProcDLL::KillProc ${EM_OUTFILE_NAME}
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
FunctionEnd

Function OnClick_CheckStartTimeDo
	${IF} $Bool_StartTimeDo == 1
        IntOp $Bool_StartTimeDo $Bool_StartTimeDo - 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox1.bmp $ck_StartTimeDo
    ${ELSE}
        IntOp $Bool_StartTimeDo $Bool_StartTimeDo + 1
        SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $ck_StartTimeDo
    ${EndIf}
FunctionEnd

;处理页面跳转的命令
Function RelGotoPage
  IntCmp $R9 0 0 Move Move
    StrCmp $R9 "X" 0 Move
      StrCpy $R9 "120"
  Move:
  SendMessage $HWNDPARENT "0x408" "$R9" ""
FunctionEnd

Function OnClick_Install
	StrCpy $R9 1 ;Goto the next page
    Call RelGotoPage
FunctionEnd

Function WelcomePage
    GetDlgItem $0 $HWNDPARENT 1
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 2
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 3
    ShowWindow $0 ${SW_HIDE}

    nsDialogs::Create 1044
    Pop $Hwnd_Welcome
    ${If} $Hwnd_Welcome == error
        Abort
    ${EndIf}
    SetCtlColors $Hwnd_Welcome ""  transparent ;背景设成透明

    ${NSW_SetWindowSize} $HWNDPARENT 478 320 ;改变窗体大小
    ${NSW_SetWindowSize} $Hwnd_Welcome 478 320 ;改变Page大小
  
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
    ${NSD_CreateLabel} 30 283 180 20 "我已阅读并同意绿盾广告管家"
    Pop $Lbl_Xieyi
    SetCtlColors $Lbl_Xieyi "${TEXTCOLOR}" transparent ;背景设成透明
	
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
 	${NSD_OnChange} $Txt_Browser OnChange_DirRequest
	ShowWindow $Txt_Browser ${SW_HIDE}
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
	 ${NSD_CreateLabel} 16 218 80 18 "安装位置："
    Pop $Lbl_Path
    SetCtlColors $Lbl_Path "${TEXTCOLOR}" transparent ;背景设成透明
	ShowWindow $Lbl_Path ${SW_HIDE}
	
	;添加桌面快捷方式
	${NSD_CreateButton} 16 278 15 15 ""
	Pop $ck_DeskTopLink
	StrCpy $1 $ck_DeskTopLink
	SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $1
	GetFunctionAddress $3 OnClick_CheckDeskTopLink
    SkinBtn::onClick $1 $3
	StrCpy $Bool_DeskTopLink 1
    ${NSD_CreateLabel} 36 276 120 18 "添加桌面快捷方式"
    Pop $Lbl_DeskTopLink
    SetCtlColors $Lbl_DeskTopLink "${TEXTCOLOR}" transparent ;背景设成透明
	ShowWindow $ck_DeskTopLink ${SW_HIDE}
	ShowWindow $Lbl_DeskTopLink ${SW_HIDE}
	
	;开启实时过滤
	${NSD_CreateButton} 166 278 15 15 ""
	Pop $ck_StartTimeDo
	StrCpy $1 $ck_StartTimeDo
	SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $1
	GetFunctionAddress $3 OnClick_CheckStartTimeDo
    SkinBtn::onClick $1 $3
	StrCpy $Bool_StartTimeDo 1
    ${NSD_CreateLabel} 186 276 100 18 "开启实时监控"
    Pop $Lbl_StartTimeDo
    SetCtlColors $Lbl_StartTimeDo "${TEXTCOLOR}" transparent ;背景设成透明
	ShowWindow $ck_StartTimeDo ${SW_HIDE}
	ShowWindow $Lbl_StartTimeDo ${SW_HIDE}
	
	;立即安装
	${NSD_CreateBrowseButton} 316 286 76 26 ""
 	Pop	$Btn_Install
 	StrCpy $1 $Btn_Install
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_install.bmp $1
	GetFunctionAddress $3 OnClick_Install
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_Install ${SW_HIDE}
	
	;返回
	${NSD_CreateBrowseButton} 401 286 61 26 ""
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
	nsDialogs::Show
	${NSD_FreeImage} $ImageHandle
FunctionEnd

Function NSD_TimerFun
    GetFunctionAddress $0 NSD_TimerFun
    nsDialogs::KillTimer $0
    !if 1   ;是否在后台运行,1有效
        GetFunctionAddress $0 InstallationMainFun
        BgWorker::CallAndWait
    !else
        Call InstallationMainFun
    !endif
	ShowWindow $Bmp_Finish ${SW_SHOW}
	ShowWindow $Btn_FreeUse ${SW_SHOW}
	ShowWindow $Lbl_Sumary ${SW_HIDE}
	ShowWindow $PB_ProgressBar ${SW_HIDE}
	ShowWindow $BGImage ${SW_HIDE}
	;主线程中创建快捷方式
	${If} $Bool_DeskTopLink == 1
		CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_NAME}.exe"
	${EndIf}
	; 创建开始菜单快捷方式
	CreateShortCut "$STARTMENU\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_NAME}.exe"
	CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_NAME}.exe"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk" "$INSTDIR\uninst.exe"
FunctionEnd

Function InstallationMainFun
	call DoInstall
    SendMessage $PB_ProgressBar ${PBM_SETRANGE32} 0 100  ;总步长为顶部定义值
	SendMessage $PB_ProgressBar ${PBM_SETPOS} 10 0
    Sleep 200
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 20 0
    Sleep 200
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 30 0
    Sleep 200
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 40 0
    Sleep 200
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 50 0
    Sleep 200
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 60 0
    Sleep 200
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 70 0
    Sleep 200
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 80 0
    Sleep 200
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 90 0
    Sleep 200
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 100 0
FunctionEnd

Function OnClick_FreeUse
	SetOutPath "$INSTDIR"
	ExecShell open "${PRODUCT_NAME}.exe" /x SW_SHOWNORMAL
	Call OnClickQuitOK
FunctionEnd

;安装进度页面
Function LoadingPage
  GetDlgItem $0 $HWNDPARENT 1
  ShowWindow $0 ${SW_HIDE}
  GetDlgItem $0 $HWNDPARENT 2
  ShowWindow $0 ${SW_HIDE}
  GetDlgItem $0 $HWNDPARENT 3
  ShowWindow $0 ${SW_HIDE}

	nsDialogs::Create 1044
	Pop $0
	${If} $0 == error
		Abort
	${EndIf}
	SetCtlColors $0 ""  transparent ;背景设成透明

	${NSW_SetWindowSize} $HWNDPARENT 478 320 ;改变自定义窗体大小
	${NSW_SetWindowSize} $0 478 320 ;改变自定义Page大小



    ;创建简要说明
    ${NSD_CreateLabel} 24 253 57 20 "正在安装"
    Pop $Lbl_Sumary
    SetCtlColors $Lbl_Sumary "${TEXTCOLOR}"  transparent ;背景设成透明

    ${NSD_CreateProgressBar} 24 275 430 16 ""
    Pop $PB_ProgressBar
    SkinProgress::Set $PB_ProgressBar "$PLUGINSDIR\loading2.bmp" "$PLUGINSDIR\loading1.bmp"
    GetFunctionAddress $0 NSD_TimerFun
    nsDialogs::CreateTimer $0 1
    
	
	;完成时"免费使用"按钮
	${NSD_CreateBrowseButton} 180 245 117 37 ""
 	Pop	$Btn_FreeUse
 	StrCpy $1 $Btn_FreeUse
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_freeuse.bmp $1
	GetFunctionAddress $3 OnClick_FreeUse
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_FreeUse ${SW_HIDE}
	
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
    nsDialogs::Show
    ${NSD_FreeImage} $ImageHandle
FunctionEnd

Section MainSetup
SectionEnd


/****************************************************/
;卸载
/****************************************************/
Var Bmp_StartUnstall
Var Btn_ContinueUse
Var Btn_CruelRefused

Var Bmp_FinishUnstall
Var Btn_FinishUnstall

Function un.onInit
    InitPluginsDir
    File `/ONAME=$PLUGINSDIR\un_startbg.bmp` `images\un_startbg.bmp`
	File `/ONAME=$PLUGINSDIR\un_finishbg.bmp` `images\un_finishbg.bmp`
	File `/ONAME=$PLUGINSDIR\btn_jixushiyong.bmp` `images\btn_jixushiyong.bmp`
	File `/ONAME=$PLUGINSDIR\btn_canrenxiezai.bmp` `images\btn_canrenxiezai.bmp`
	File `/ONAME=$PLUGINSDIR\btn_xiezaiwancheng.bmp` `images\btn_xiezaiwancheng.bmp`
	
	SkinBtn::Init "$PLUGINSDIR\btn_jixushiyong.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_canrenxiezai.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_xiezaiwancheng.bmp"
FunctionEnd

Function un.onGUICallback
  ${If} $MSG = ${WM_LBUTTONDOWN}
    SendMessage $HWNDPARENT ${WM_NCLBUTTONDOWN} ${HTCAPTION} $0
  ${EndIf}
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
	;FindProcDLL::FindProc "绿盾安装包.exe"
    ;Sleep 200
    ;Pop $R0
    ;${If} $R0 != 0
    ;KillProcDLL::KillProc "绿盾安装包.exe"
	HideWindow
	Abort
FunctionEnd

Function un.DoUninstall
	RMDir /r "$INSTDIR"
FunctionEnd

Function un.UNSD_TimerFun
    GetFunctionAddress $0 un.UNSD_TimerFun
    nsDialogs::KillTimer $0
    GetFunctionAddress $0 un.DoUninstall
    BgWorker::CallAndWait
	DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
	DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
	IfFileExists "$DESKTOP\绿盾.lnk" 0 +2
		Delete "$DESKTOP\绿盾.lnk"
	IfFileExists "$STARTMENU\绿盾.lnk" 0 +2
		Delete "$STARTMENU\绿盾.lnk"
	;IfFileExists "$SMPROGRAMS\绿盾\*.*" 0 +2
		;Delete "$SMPROGRAMS\绿盾\绿盾.lnk"
		;Delete "$SMPROGRAMS\绿盾\Uninstall.lnk"
		RMDir /r "$SMPROGRAMS\绿盾"
	 ;删除自用的注册表信息
	 DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}"
	
	ShowWindow $Bmp_StartUnstall ${SW_HIDE}
	ShowWindow $Btn_ContinueUse ${SW_HIDE}
	ShowWindow $Btn_CruelRefused ${SW_HIDE}
	ShowWindow $Bmp_FinishUnstall 1
	ShowWindow $Btn_FinishUnstall 1
FunctionEnd

Function un.OnClick_CruelRefused
	GetFunctionAddress $0 un.UNSD_TimerFun
    nsDialogs::CreateTimer $0 1
FunctionEnd

Function un.OnClick_FinishUnstall
	;FindProcDLL::FindProc "绿盾安装包.exe"
    ;Sleep 200
    ;Pop $R0
    ;${If} $R0 != 0
    ;KillProcDLL::KillProc "绿盾安装包.exe"
	HideWindow
	Abort
FunctionEnd

Function un.MyUnstall
	push $0
	call un.myGUIInit
	GetDlgItem $0 $HWNDPARENT 1
	ShowWindow $0 ${SW_HIDE}
	GetDlgItem $0 $HWNDPARENT 2
	ShowWindow $0 ${SW_HIDE}
	GetDlgItem $0 $HWNDPARENT 3
	ShowWindow $0 ${SW_HIDE}

	nsDialogs::Create 1044
	Pop $0
	${If} $0 == error
		Abort
	${EndIf}
	SetCtlColors $0 ""  transparent ;背景设成透明

	${NSW_SetWindowSize} $HWNDPARENT 478 320 ;改变自定义窗体大小
	${NSW_SetWindowSize} $0 478 320 ;改变自定义Page大小


	;继续使用
	${NSD_CreateBrowseButton} 246 235 95 30 ""
 	Pop	$Btn_ContinueUse
 	StrCpy $1 $Btn_ContinueUse
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_jixushiyong.bmp $1
	GetFunctionAddress $3 un.OnClick_ContinueUse
    SkinBtn::onClick $1 $3
	
	;残忍拒绝
	${NSD_CreateBrowseButton} 355 235 95 30 ""
 	Pop	$Btn_CruelRefused
 	StrCpy $1 $Btn_CruelRefused
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_canrenxiezai.bmp $1
	GetFunctionAddress $3 un.OnClick_CruelRefused
    SkinBtn::onClick $1 $3
	
	;卸载完成
	${NSD_CreateBrowseButton} 190 263 95 30 ""
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
    nsDialogs::Show
    ${NSD_FreeImage} $ImageHandle
FunctionEnd