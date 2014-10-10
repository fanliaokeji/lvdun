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

;����������
Var Lbl_Sumary
Var PB_ProgressBar
Var Bmp_Finish
Var Btn_FreeUse
;---------------------------ȫ�ֱ���ű�Ԥ����ĳ���-----------------------------------------------------
; MUI Ԥ���峣��
!define MUI_ABORTWARNING
;��װͼ���·������
!define MUI_ICON "images\fav.ico"
;ж��ͼ���·������
!define MUI_UNICON "images\unis.ico"

!define PRODUCT_NAME "GreenShield"
!define SHORTCUT_NAME "�̶�"
!define EM_OUTFILE_NAME "${PRODUCT_NAME}��װ��.exe"
!define PRODUCT_VERSION "1.0.0.1"
!define EM_BrandingText "${PRODUCT_NAME}${PRODUCT_VERSION}"
!define PRODUCT_PUBLISHER "My company, Inc."
!define PRODUCT_WEB_SITE "http://www.mycompany.com"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_NAME}.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_MAININFO_FORSELF "Software\${PRODUCT_NAME}"

;---------------------------�������ѹ�����ͣ�Ҳ����ͨ���������ű����ƣ�------------------------------------
SetCompressor lzma
BrandingText "${EM_BrandingText}"
SetCompress force
XPStyle on
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
SetFont ΢���ź� 10
!define TEXTCOLOR "4D4D4D"
RequestExecutionLevel highest


;�Զ���ҳ��
Page custom WelcomePage
Page custom LoadingPage
UninstPage custom un.MyUnstall


;------------------------------------------------------MUI �ִ����涨���Լ���������------------------------
;Ӧ�ó�����ʾ����
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
;Ӧ�ó������·��
OutFile "bin\${EM_OUTFILE_NAME}"
InstallDir "$PROGRAMFILES\GreenShield"
InstallDirRegKey HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"

Function DoInstall
  ;�ͷ��������ļ�����װĿ¼
  SetOutPath "$INSTDIR"
  SetOverwrite on
  File /r "input_main\*.*"
  WriteUninstaller "$INSTDIR\uninst.exe"
  ;�ͷ����õ�publicĿ¼
  SetOutPath "$TEMP\${PRODUCT_NAME}"
  StrCpy $1 ${NSIS_MAX_STRLEN}
  System::Call '$TEMP\${PRODUCT_NAME}\DsSetUpHelper::GetProfileFolder(t) i(.r0).r2' 
  SetOutPath "$0"
  SetOverwrite off
  File /r "input_config\*.*"
  
  
  ;�ϱ�ͳ��
  SetOutPath "$TEMP\${PRODUCT_NAME}"
  ${GetParameters} $R1
  ${GetOptions} $R1 "/source"  $R0
  IfErrors 0 +2
  StrCpy $R0 "cmd_null"
  System::Call '$TEMP\${PRODUCT_NAME}\DsSetUpHelper::SendAnyHttpStat(t "$R0", t "22222", t "33333", i 4) '
  ;д�����õ�ע�����Ϣ
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallSource" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir" "$INSTDIR"
  System::Call '$TEMP\${PRODUCT_NAME}\DsSetUpHelper::GetTime(*l) i(.r0).r1'
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallTimes" "$0"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "Path" "$INSTDIR\program\${PRODUCT_NAME}.exe"
  System::Call '$TEMP\${PRODUCT_NAME}\DsSetUpHelper::GetPeerID(t) i(.r0).r1'
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "PeerId" "$0"
  
  ;д��ͨ�õ�ע�����Ϣ
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

Function CmdSelentInstall
	${GetParameters} $R1
	${GetOptions} $R1 "/silent"  $R0
	IfErrors FunctionReturn 0
	SetSilent silent
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "LVDUNSETUP_MUTEX") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +2
	Abort
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	SetOverwrite on
	File "bin\DsSetUpHelper.dll"
	CheckProcessExist:
	FindProcDLL::FindProc "${PRODUCT_NAME}.exe"
	Pop $R0
    ${If} $R0 != 0
		KillProcDLL::KillProc "${PRODUCT_NAME}.exe"
		Goto CheckProcessExist
	${EndIf}
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	${If} $0 != ""
		StrCpy $INSTDIR "$0"
	${EndIf}
	Call DoInstall
	CreateShortCut "$DESKTOP\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe"
	CreateShortCut "$STARTMENU\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe"
	CreateDirectory "$SMPROGRAMS\${SHORTCUT_NAME}"
	CreateShortCut "$SMPROGRAMS\${SHORTCUT_NAME}\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe"
	CreateShortCut "$SMPROGRAMS\${SHORTCUT_NAME}\Uninstall.lnk" "$INSTDIR\uninst.exe"
	Abort
	FunctionReturn:
FunctionEnd

Function CmdUnstall
	${GetOptions} $R1 "/uninstall"  $R0
	IfErrors FunctionReturn 0
	SetSilent silent
	CheckProcessExist:
	FindProcDLL::FindProc "${PRODUCT_NAME}.exe"
	Pop $R0
    ${If} $R0 != 0
		KillProcDLL::KillProc "${PRODUCT_NAME}.exe"
		Goto CheckProcessExist
	${EndIf}
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	${If} $0 != ""
		StrCpy $INSTDIR "$0"
	${EndIf}
	
	;ɾ�����ɹ�������3��
	StrCpy $R0 0
	BeginRepeatDelete:
	RMDir /r "$INSTDIR"
	${If} $R0 == 3
		Goto EndRepeatDelete
	${EndIf}
	IfFileExists "$INSTDIR" 0 +4
	IntOp $R0 $R0 + 1
	Sleep 500
	Goto BeginRepeatDelete
	EndRepeatDelete:
	
	DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
	DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
	IfFileExists "$DESKTOP\${SHORTCUT_NAME}.lnk" 0 +2
		Delete "$DESKTOP\${SHORTCUT_NAME}.lnk"
	IfFileExists "$STARTMENU\${SHORTCUT_NAME}.lnk" 0 +2
		Delete "$STARTMENU\${SHORTCUT_NAME}.lnk"
	RMDir /r "$SMPROGRAMS\${SHORTCUT_NAME}"
	 ;ɾ�����õ�ע�����Ϣ
	DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}"
	FunctionReturn:
FunctionEnd

Function .onInit
	StrCpy $Int_FontOffset 4
	CreateFont $Handle_Font "����" 10 0
	IfFileExists "$FONTS\msyh.ttf" 0 +3
	CreateFont $Handle_Font "΢���ź�" 10 0
	StrCpy $Int_FontOffset 0
	
	Call CmdSelentInstall
	Call CmdUnstall
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "LVDUNSETUP_MUTEX") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +3
	MessageBox MB_OK|MB_ICONEXCLAMATION "Another Installer is Running!"
	Abort
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	SetOverwrite on
	File "bin\DsSetUpHelper.dll"
	File "license\license.txt"
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "Path"
	IfFileExists $0 0 StartInstall
		;System::Call 'DsSetUpHelper::GetFileVersionString(t $0, t) i(r0, .r1).r2'
		${GetFileVersion} $0 $1
		${VersionCompare} $1 ${PRODUCT_VERSION} $2
		${If} $2 == "2" ;�Ѱ�װ�İ汾���ڸð汾
			Goto StartInstall
		${ElseIf} $2 == "0" ;�汾��ͬ
			 MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "��⵽�Ѱ�װ$(^Name)���Ƿ񸲸ǰ�װ��" IDYES StartInstall
			 Abort
		${ElseIf} $2 == "1"	;�Ѱ�װ�İ汾���ڸð汾
			MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "��⵽�Ѱ�װ$(^Name)���Ƿ񸲸ǰ�װ��" IDYES StartInstall
			Abort
		${EndIf}
	StartInstall:
	FindProcDLL::FindProc "${PRODUCT_NAME}.exe"
    Pop $R0
    ${If} $R0 != 0
		 MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "���${PRODUCT_NAME}.exe�������У��Ƿ�ǿ�ƽ�����" IDYES CheckProcessExist
		 Abort
		CheckProcessExist:
		FindProcDLL::FindProc "${PRODUCT_NAME}.exe"
		Pop $R0
		${If} $R0 != 0
			KillProcDLL::KillProc "${PRODUCT_NAME}.exe"
			Goto CheckProcessExist
		${EndIf}
    ${EndIf}
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
	Pop $0
	System::Call "user32::GetWindowText(i $Txt_Browser, t .r0, i ${NSIS_MAX_STRLEN})"
	;MessageBox MB_ICONINFORMATION|MB_OK $0
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
	WndProc::onCallback $1 $0 ;�����ޱ߿����ƶ�
	${NSW_CenterWindow} $WarningForm $hwndparent
	${NSW_Show}
	Create_End:
	ShowWindow $WarningForm ${SW_SHOW}
FunctionEnd

Function OnClickQuitOK
	FindProcDLL::FindProc ${EM_OUTFILE_NAME}
    ;Pop $R0
    ${If} $R0 != 0
		KillProcDLL::KillProc ${EM_OUTFILE_NAME}
	${Else}
		SendMessage $HWNDPARENT ${WM_CLOSE} 0 0
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

;����ҳ����ת������
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
    SetCtlColors $Hwnd_Welcome ""  transparent ;�������͸��

    ${NSW_SetWindowSize} $HWNDPARENT 478 320 ;�ı䴰���С
    ${NSW_SetWindowSize} $Hwnd_Welcome 478 320 ;�ı�Page��С

	System::Call  'User32::GetDesktopWindow() i.R9'
	${NSW_CenterWindow} $HWNDPARENT $R9
	
    ;һ����װ
	StrCpy $Bool_IsExtend 0
    ${NSD_CreateButton} 180 222 117 35 ""
	Pop $Btn_Next
	StrCpy $1 $Btn_Next
	Call SkinBtn_Next
	GetFunctionAddress $3 onClickNext
    SkinBtn::onClick $1 $3
    
	;��ѡͬ��Э��
	${NSD_CreateButton} 11 285 15 15 ""
	Pop $ck_xieyi
	StrCpy $1 $ck_xieyi
	SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $1
	GetFunctionAddress $3 OnClick_CheckXieyi
    SkinBtn::onClick $1 $3
	StrCpy $Bool_Xieyi 1
	
	StrCpy $3 283
	IntOp $3 $3 + $Int_FontOffset
    ${NSD_CreateLabel} 30 $3 180 20 "�����Ķ���ͬ���̶ܹ��ܼ�"
    Pop $Lbl_Xieyi
    SetCtlColors $Lbl_Xieyi "${TEXTCOLOR}" transparent ;�������͸��
	SendMessage $Lbl_Xieyi ${WM_SETFONT} $Handle_Font 0
	
    ;�û�Э��
	${NSD_CreateButton} 201 286 68 15 ""
	Pop $Btn_Agreement
	StrCpy $1 $Btn_Agreement
	Call SkinBtn_Agreement1
	GetFunctionAddress $3 onClickAgreement
	SkinBtn::onClick $1 $3
		
	;�Զ��尲װ
	${NSD_CreateButton} 385 286 81 15 ""
	Pop $Btn_Zidingyi
	StrCpy $1 $Btn_Zidingyi
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_agreement2.bmp $1
	GetFunctionAddress $3 onClickZidingyi
	SkinBtn::onClick $1 $3
	
	;��С��
	${NSD_CreateButton} 438 5 13 13 ""
	Pop $Btn_Zuixiaohua
	StrCpy $1 $Btn_Zuixiaohua
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_min.bmp $1
	GetFunctionAddress $3 onClickZuixiaohua
	SkinBtn::onClick $1 $3
	;�ر�
	${NSD_CreateButton} 457 5 13 13 ""
	Pop $Btn_Guanbi
	StrCpy $1 $Btn_Guanbi
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_close.bmp $1
	GetFunctionAddress $3 onClickGuanbi
	SkinBtn::onClick $1 $3
	
	
	;Ŀ¼ѡ���
	${NSD_CreateDirRequest} 17 241 382 23 "$INSTDIR"
 	Pop	$Txt_Browser
	SendMessage $Txt_Browser ${WM_SETFONT} $Handle_Font 0
 	${NSD_OnChange} $Txt_Browser OnChange_DirRequest
	ShowWindow $Txt_Browser ${SW_HIDE}
	;Ŀ¼ѡ��ť
	${NSD_CreateBrowseButton} 399 239 63 26 ""
 	Pop	$Btn_Browser
 	StrCpy $1 $Btn_Browser
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_change.bmp $1
	GetFunctionAddress $3 OnClick_BrowseButton
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_Browser ${SW_HIDE}
	;Ŀ¼ѡ��򱳾�
	 ${NSD_CreateBitmap} 16 240 446 25 ""
    Pop $Edit_BrowserBg
    ${NSD_SetImage} $Edit_BrowserBg $PLUGINSDIR\edit_bg.bmp $ImageHandle
	ShowWindow $Edit_BrowserBg ${SW_HIDE}
	;·��ѡ����������
	StrCpy $3 218
	IntOp $3 $3 + $Int_FontOffset
	 ${NSD_CreateLabel} 16 $3 80 18 "��װλ�ã�"
    Pop $Lbl_Path
    SetCtlColors $Lbl_Path "${TEXTCOLOR}" transparent ;�������͸��
	ShowWindow $Lbl_Path ${SW_HIDE}
	SendMessage $Lbl_Path ${WM_SETFONT} $Handle_Font 0
	
	;��������ݷ�ʽ
	${NSD_CreateButton} 16 278 15 15 ""
	Pop $ck_DeskTopLink
	StrCpy $1 $ck_DeskTopLink
	SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $1
	GetFunctionAddress $3 OnClick_CheckDeskTopLink
    SkinBtn::onClick $1 $3
	StrCpy $Bool_DeskTopLink 1
	
	StrCpy $3 276
	IntOp $3 $3 + $Int_FontOffset
    ${NSD_CreateLabel} 36 $3 120 18 "��������ݷ�ʽ"
    Pop $Lbl_DeskTopLink
    SetCtlColors $Lbl_DeskTopLink "${TEXTCOLOR}" transparent ;�������͸��
	ShowWindow $ck_DeskTopLink ${SW_HIDE}
	ShowWindow $Lbl_DeskTopLink ${SW_HIDE}
	SendMessage $Lbl_DeskTopLink ${WM_SETFONT} $Handle_Font 0
	
	;����ʵʱ����
	${NSD_CreateButton} 166 278 15 15 ""
	Pop $ck_StartTimeDo
	StrCpy $1 $ck_StartTimeDo
	SkinBtn::Set /IMGID=$PLUGINSDIR\checkbox2.bmp $1
	GetFunctionAddress $3 OnClick_CheckStartTimeDo
    SkinBtn::onClick $1 $3
	StrCpy $Bool_StartTimeDo 1
	
	StrCpy $3 276
	IntOp $3 $3 + $Int_FontOffset
    ${NSD_CreateLabel} 186 $3 100 18 "����ʵʱ���"
    Pop $Lbl_StartTimeDo
    SetCtlColors $Lbl_StartTimeDo "${TEXTCOLOR}" transparent ;�������͸��
	ShowWindow $ck_StartTimeDo ${SW_HIDE}
	ShowWindow $Lbl_StartTimeDo ${SW_HIDE}
	SendMessage $Lbl_StartTimeDo ${WM_SETFONT} $Handle_Font 0
	
	;������װ
	${NSD_CreateButton} 316 286 76 26 ""
 	Pop	$Btn_Install
 	StrCpy $1 $Btn_Install
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_install.bmp $1
	GetFunctionAddress $3 OnClick_Install
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_Install ${SW_HIDE}
	
	;����
	${NSD_CreateButton} 401 286 61 26 ""
 	Pop	$Btn_Return
 	StrCpy $1 $Btn_Return
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_return.bmp $1
	GetFunctionAddress $3 OnClick_Return
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_Return ${SW_HIDE}
	
	
	GetFunctionAddress $0 onGUICallback
    ;��������ͼ
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\bg.bmp $ImageHandle
	
	WndProc::onCallback $BGImage $0 ;�����ޱ߿����ƶ�
	nsDialogs::Show
	${NSD_FreeImage} $ImageHandle
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
	ShowWindow $Bmp_Finish ${SW_SHOW}
	ShowWindow $Btn_FreeUse ${SW_SHOW}
	ShowWindow $Lbl_Sumary ${SW_HIDE}
	ShowWindow $PB_ProgressBar ${SW_HIDE}
	ShowWindow $BGImage ${SW_HIDE}
	;���߳��д�����ݷ�ʽ
	${If} $Bool_DeskTopLink == 1
		CreateShortCut "$DESKTOP\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe"
	${EndIf}
	; ������ʼ�˵���ݷ�ʽ
	CreateShortCut "$STARTMENU\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe"
	CreateDirectory "$SMPROGRAMS\${SHORTCUT_NAME}"
	CreateShortCut "$SMPROGRAMS\${SHORTCUT_NAME}\${SHORTCUT_NAME}.lnk" "$INSTDIR\program\${PRODUCT_NAME}.exe"
	CreateShortCut "$SMPROGRAMS\${SHORTCUT_NAME}\Uninstall.lnk" "$INSTDIR\uninst.exe"
FunctionEnd

Function InstallationMainFun
	call DoInstall
    SendMessage $PB_ProgressBar ${PBM_SETRANGE32} 0 100  ;�ܲ���Ϊ��������ֵ
	SendMessage $PB_ProgressBar ${PBM_SETPOS} 12 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 20 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 28 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 40 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 52 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 60 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 68 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 80 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 92 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 100 0
	Sleep 1000
FunctionEnd

Function OnClick_FreeUse
	SetOutPath "$INSTDIR\program"
	ExecShell open "${PRODUCT_NAME}.exe" /x SW_SHOWNORMAL
	Call OnClickQuitOK
FunctionEnd

;��װ����ҳ��
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
	SetCtlColors $0 ""  transparent ;�������͸��

	${NSW_SetWindowSize} $HWNDPARENT 478 320 ;�ı��Զ��崰���С
	${NSW_SetWindowSize} $0 478 320 ;�ı��Զ���Page��С
	
	System::Call  'User32::GetDesktopWindow() i.R9'
	${NSW_CenterWindow} $HWNDPARENT $R9


    ;������Ҫ˵��
	StrCpy $3 253
	IntOp $3 $3 + $Int_FontOffset
    ${NSD_CreateLabel} 24 $3 57 20 "���ڰ�װ"
    Pop $Lbl_Sumary
    SetCtlColors $Lbl_Sumary "${TEXTCOLOR}"  transparent ;�������͸��
	SendMessage $Lbl_Sumary ${WM_SETFONT} $Handle_Font 0

    ${NSD_CreateProgressBar} 24 275 430 16 ""
    Pop $PB_ProgressBar
    SkinProgress::Set $PB_ProgressBar "$PLUGINSDIR\loading2.bmp" "$PLUGINSDIR\loading1.bmp"
    GetFunctionAddress $0 NSD_TimerFun
    nsDialogs::CreateTimer $0 1
    
	
	;���ʱ"���ʹ��"��ť
	${NSD_CreateButton} 180 245 117 37 ""
 	Pop	$Btn_FreeUse
 	StrCpy $1 $Btn_FreeUse
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_freeuse.bmp $1
	GetFunctionAddress $3 OnClick_FreeUse
    SkinBtn::onClick $1 $3
	ShowWindow $Btn_FreeUse ${SW_HIDE}
	
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
    nsDialogs::Show
    ${NSD_FreeImage} $ImageHandle
FunctionEnd

Section MainSetup
SectionEnd


/****************************************************/
;ж��
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
	SendMessage $HWNDPARENT ${WM_CLOSE} 0 0
FunctionEnd

Function un.DoUninstall
	StrCpy $R0 0
	BeginRepeatDelete:
	RMDir /r "$INSTDIR"
	${If} $R0 == 3
		Goto EndRepeatDelete
	${EndIf}
	IfFileExists "$INSTDIR" 0 +4
	IntOp $R0 $R0 + 1
	Sleep 500
	Goto BeginRepeatDelete
	EndRepeatDelete:
FunctionEnd

Function un.UNSD_TimerFun
    GetFunctionAddress $0 un.UNSD_TimerFun
    nsDialogs::KillTimer $0
	FindProcDLL::FindProc "${PRODUCT_NAME}.exe"
    ${If} $R0 != 0
		 MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "���${PRODUCT_NAME}.exe�������У��Ƿ�ǿ�ƽ�����" IDYES CheckProcessExist
		 SendMessage $HWNDPARENT ${WM_CLOSE} 0 0
		 Goto EndTimerFunction
		 CheckProcessExist:
		 FindProcDLL::FindProc "${PRODUCT_NAME}.exe"
		 ${If} $R0 != 0
			KillProcDLL::KillProc "${PRODUCT_NAME}.exe"
			Sleep 100
			Goto CheckProcessExist
		 ${EndIf}
    ${EndIf}
    GetFunctionAddress $0 un.DoUninstall
    BgWorker::CallAndWait
	DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
	DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
	IfFileExists "$DESKTOP\${SHORTCUT_NAME}.lnk" 0 +2
		Delete "$DESKTOP\${SHORTCUT_NAME}.lnk"
	IfFileExists "$STARTMENU\${SHORTCUT_NAME}.lnk" 0 +2
		Delete "$STARTMENU\${SHORTCUT_NAME}.lnk"
	;IfFileExists "$SMPROGRAMS\�̶�\*.*" 0 +2
		;Delete "$SMPROGRAMS\�̶�\�̶�.lnk"
		;Delete "$SMPROGRAMS\�̶�\Uninstall.lnk"
		RMDir /r "$SMPROGRAMS\${SHORTCUT_NAME}"
	 ;ɾ�����õ�ע�����Ϣ
	 DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}"
	
	ShowWindow $Bmp_StartUnstall ${SW_HIDE}
	ShowWindow $Btn_ContinueUse ${SW_HIDE}
	ShowWindow $Btn_CruelRefused ${SW_HIDE}
	ShowWindow $Bmp_FinishUnstall 1
	ShowWindow $Btn_FinishUnstall 1
	EndTimerFunction:
FunctionEnd

Function un.OnClick_CruelRefused
	EnableWindow $Btn_CruelRefused 0
	EnableWindow $Btn_ContinueUse 0
	GetFunctionAddress $0 un.UNSD_TimerFun
    nsDialogs::CreateTimer $0 1
FunctionEnd

Function un.OnClick_FinishUnstall
	SendMessage $HWNDPARENT ${WM_CLOSE} 0 0
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
	SetCtlColors $0 ""  transparent ;�������͸��

	${NSW_SetWindowSize} $HWNDPARENT 478 320 ;�ı��Զ��崰���С
	${NSW_SetWindowSize} $0 478 320 ;�ı��Զ���Page��С
	
	System::Call 'user32::GetDesktopWindow()i.R9'
	${NSW_CenterWindow} $HWNDPARENT $R9

	;����ʹ��
	${NSD_CreateButton} 246 235 95 30 ""
 	Pop	$Btn_ContinueUse
 	StrCpy $1 $Btn_ContinueUse
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_jixushiyong.bmp $1
	GetFunctionAddress $3 un.OnClick_ContinueUse
    SkinBtn::onClick $1 $3
	
	;���ܾ̾�
	${NSD_CreateButton} 355 235 95 30 ""
 	Pop	$Btn_CruelRefused
 	StrCpy $1 $Btn_CruelRefused
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_canrenxiezai.bmp $1
	GetFunctionAddress $3 un.OnClick_CruelRefused
    SkinBtn::onClick $1 $3
	
	;ж�����
	${NSD_CreateButton} 190 263 95 30 ""
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
    nsDialogs::Show
    ${NSD_FreeImage} $ImageHandle
FunctionEnd