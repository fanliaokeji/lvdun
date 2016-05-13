;安装图标的路径名字
!define MUI_ICON "..\images\install\fav.ico"
!define MUI_UNICON "..\images\uninst\uninst.ico"

!define INSTALL_CHANNELID "0001"
!define PRODUCT_NAME "kuaikan"
!define SHORTCUT_NAME "快看"
!define PRODUCT_VERSION "1.0.0.${BuildNum}"
!define NeedSpace 13312
;!define TEXTCOLOR "4D4D4D"
!define EM_OUTFILE_NAME "kuaikansetupv${BuildNum}_${INSTALL_CHANNELID}.exe"

!define EM_BrandingText "${PRODUCT_NAME}${PRODUCT_VERSION}"
!define PRODUCT_PUBLISHER "kuaikan"
!define PRODUCT_WEB_SITE ""
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_NAME}.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_MAININFO_FORSELF "Software\kuaikan"

;设置软件压缩类型
SetCompressor /SOLID lzma
SetCompressorDictSize 32
BrandingText "${EM_BrandingText}"
SetCompress force
SetDatablockOptimize on

!insertmacro MUI_LANGUAGE "SimpChinese"
SetFont 宋体 9
RequestExecutionLevel admin

VIProductVersion ${PRODUCT_VERSION}
VIAddVersionKey /LANG=2052 "ProductName" "快看"
VIAddVersionKey /LANG=2052 "Comments" ""
VIAddVersionKey /LANG=2052 "CompanyName" "深圳市二十二楼科技有限公司"
VIAddVersionKey /LANG=2052 "LegalCopyright" "Copyright (c) 2015-2017 深圳市二十二楼科技有限公司"
VIAddVersionKey /LANG=2052 "FileDescription" "快看安装程序"
VIAddVersionKey /LANG=2052 "FileVersion" ${PRODUCT_VERSION}
VIAddVersionKey /LANG=2052 "ProductVersion" ${PRODUCT_VERSION}
VIAddVersionKey /LANG=2052 "OriginalFilename" ${EM_OUTFILE_NAME}

;MUI 现代界面定义以及函数结束
;应用程序显示名字
Name "${SHORTCUT_NAME} ${PRODUCT_VERSION}"

InstallDir "$PROGRAMFILES\kuaikan"
InstallDirRegKey HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"

!macro _InitMutex
	Push $0
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "GLOBAL\kuaikansetup_{B7D80E56-6A48-40ad-A455-2E21537ECF79}") i .r1 ?e'
	Pop $0
	StrCmp $0 0 +2
	Abort
	Pop $0
!macroend
!define InitMutex `!insertmacro _InitMutex`

/*封装创建ui的接口begin*/
!macro _CreateButton nLeft nTop nWidth nHeight strImg Func Handle
	Push $0
	Push $1
	${NSD_CreateButton} ${nLeft} ${nTop} ${nWidth} ${nHeight} ''
	Pop $1
	SkinBtn::Set /IMGID=$PLUGINSDIR\${strImg} $1
	GetFunctionAddress $0 "${Func}"
	SkinBtn::onClick $1 $0
	StrCpy $0 $1
	StrCpy ${Handle} $1
	Pop $1
	Exch $0
!macroend
!define CreateButton `!insertmacro _CreateButton`

!macro _CreateLabel nLeft nTop nWidth nHeight strText strColor hFont Func Handle
	Push $0
	Push $1
	StrCpy $0 ${nTop}
	IntOp $0 $0 + $Int_FontOffset
	${NSD_CreateLabel} ${nLeft} $0 ${nWidth} ${nHeight} ${strText}
	Pop $1
    SetCtlColors $1 ${strColor} transparent ;背景设成透明
	SendMessage $1 ${WM_SETFONT} ${hFont} 0
	${NSD_OnClick} $1 ${Func}
	StrCpy $0 $1
	StrCpy ${Handle} $1
	Pop $1
	Exch $0
!macroend
!define CreateLabel `!insertmacro _CreateLabel`
/*封装创建ui的接口end*/

;循环杀进程
!macro _FKillProc bSilent strProcName
	Push $R3
	Push $R1
	Push $R0
	
	StrCpy $R1 0
	ClearErrors
	${GetOptions} $CMDLINE "/s"  $R0
	IfErrors 0 +2
	StrCpy $R1 1
	
	;只有在有界面安装 且 未指定静默参数时才会弹框
	${For} $R3 0 6
		FindProcDLL::FindProc "${strProcName}.exe"
		${If} $R3 == 6
		${AndIf} $R0 != 0
		${AndIf} ${bSilent} == 0
		${AndIf} $R1 == 0
			MessageBox MB_OK|MB_ICONSTOP "很抱歉，发生了意料之外的错误,请尝试重新安装，如果还不行请到官方网站寻求帮助"
			Abort
		${ElseIf} $R0 != 0
			KillProcDLL::KillProc "${strProcName}.exe"
			Sleep 250
		${Else}
			${Break}
		${EndIf}
	${Next}
	Pop $R0
	Pop $R1
	Pop $R3
!macroend
;杀不了弹框提醒并退出安装
!define FKillProc "!insertmacro _FKillProc 0 "
;杀不了不弹框，不退出安装
!define SKillProc "!insertmacro _FKillProc 1 "

!define RENAMELOGIC 0
!macro _RenameDeleteFile strFilePath
	!define _RENAMELOGIC ${RENAMELOGIC}
	!undef RENAMELOGIC
	!define /math RENAMELOGIC ${_RENAMELOGIC} + 1
	!undef _RENAMELOGIC
	Push $1
	Push $0
	
	IfFileExists ${strFilePath} 0 RenameOK${RENAMELOGIC}
	BeginRename${RENAMELOGIC}:
	System::Call 'kernel32::QueryPerformanceCounter(*l.r1)'
	System::Int64Op $1 % 1000
	Pop $0
	IfFileExists "${strFilePath}.$0" BeginRename${RENAMELOGIC}
	Rename ${strFilePath} "${strFilePath}.$0"
	Delete /REBOOTOK "${strFilePath}.$0"
	RenameOK${RENAMELOGIC}:
	Pop $0
	Pop $1
!macroend
!define RenameDeleteFile "!insertmacro _RenameDeleteFile"

!macro _SendStat strp1 strp2 strp3 np4
	Push $0
	Push $1
	Push $2
	ReadRegStr $0 HKCU "Software\mycalendar" "statpeerid"
	ReadRegStr $1 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "PeerId"
	${If} $1 != ""
	${OrIf} $1 != 0
		${If} $0 == ""
		${OrIf} $0 == 0
			StrCpy $0 "0123"
		${EndIf}
		${StrFilter} $0 "-" "" "" $0
		${StrFilter} $1 "-" "" "" $1
		StrCpy $2 $1 1 11
		${WordReplace} $0 $2 "X" +1 $1
		${If} $0 != $1
			System::Call '$TEMP\${PRODUCT_NAME}\mycalendarsetup::SendAnyHttpStat(t "${strp1}", t "${strp2}", t "${strp3}", i ${np4}) '
		${EndIf}
	${EndIf}
	Pop $2
	Pop $1
	Pop $0
!macroend
!define SendStat "!insertmacro _SendStat"

!macro _SetSysBoot strCallFlag
	Function ${strCallFlag}InstSetSysBoot
		push $0
		push $1
		push $2
		push $3
		;干掉老的开机启动
		DeleteRegValue HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "mycalendar"
		;判断lpath是否存在
		StrCpy $1 "true"
		ClearErrors
		ReadRegStr $0 HKCU "Software\mycalendar" "lpath"
		IfErrors 0 +3
		System::Call "$TEMP\${PRODUCT_NAME}\mycalendarsetup::NsisTSLOG(t '_SetSysBoot error')"
		StrCpy $1 "false"
		System::Call "$TEMP\${PRODUCT_NAME}\mycalendarsetup::NsisTSLOG(t '_SetSysBoot para 0 = $0')"
		${If} $1 != "false"
			${If} $0 != ""
			${AndIf} $0 != 0
				IfFileExists "$0" +3 0
				System::Call "$TEMP\${PRODUCT_NAME}\mycalendarsetup::NsisTSLOG(t '_SetSysBoot IfFileExists return true')"
				StrCpy $1 "false"
			${Else}
				StrCpy $1 "false"
			${EndIf}
		${EndIf}
		System::Call "$TEMP\${PRODUCT_NAME}\mycalendarsetup::NsisTSLOG(t '_SetSysBoot para 1 = $1')"
		;设置/取消开机启动
		${If} $Bool_Sysstup == 1
			WriteRegDWORD HKCU "Software\mycalendar" "setboot" 1
			${If} $1 != "false"
				;文件存在
				push $0
				push "\"
				Call ${strCallFlag}GetLastPart
				pop $2
				StrCpy $1 $2 -4
				System::Call "$TEMP\${PRODUCT_NAME}\mycalendarsetup::NsisTSLOG(t '_SetSysBoot para 2 = $1')"
				;在RUN下面找，有则不做
				ClearErrors
				ReadRegStr $3 HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" $1
				IfErrors 0 hasfind
					${StrFilter} $0 "-" "" "" $0
					System::Call "$TEMP\${PRODUCT_NAME}\mycalendarsetup::NsisTSLOG(t '_SetSysBoot para 3 = $0')"
					StrCpy $2 "$INSTDIR\program\myfixar.exe"
					${StrFilter} $2 "-" "" "" $2
					System::Call "$TEMP\${PRODUCT_NAME}\mycalendarsetup::NsisTSLOG(t '_SetSysBoot para 4 = $2')"
					${If} $0 != $2
					${AndIf} $0 != "$2"
					${AndIf} $0 != ""
						System::Call "$TEMP\${PRODUCT_NAME}\mycalendarsetup::NsisTSLOG(t '_SetSysBoot will rmdir $0')"
						push $0
						Call ${strCallFlag}GetParent
						pop $0
						${WordReplace} $2 $0 "" +1 $1
						${If} "$1" == "$2"
							RMDir /r "$0"
						${Else}
							System::Call "$TEMP\${PRODUCT_NAME}\mycalendarsetup::NsisTSLOG(t '_SetSysBoot can not rmdir because is instdir ')"
						${EndIf}
					${EndIf}
					WriteRegStr HKCU "Software\mycalendar" "lpath" "$INSTDIR\program\myfixar.exe"
					WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "myfixar" '"$INSTDIR\program\myfixar.exe" /sstartfrom sysboot /embedding'
					Goto realend
				hasfind:
				;因为前面的版本没有带参数， 所以这里检查一下没有参数则加上。
				${WordReplace} $3 "/embedding" "" +1 $0
				${If} $3 == $0
					WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" $1 '$3 /sstartfrom sysboot /embedding'
				${EndIf}
				realend:
			${Else}
				;文件不存在
				WriteRegStr HKCU "Software\mycalendar" "lpath" "$INSTDIR\program\myfixar.exe"
				WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "myfixar" '"$INSTDIR\program\myfixar.exe" /sstartfrom sysboot /embedding'
			${EndIf}
		${Else}
			${If} "${strCallFlag}" == "un."
				DeleteRegKey HKCU "Software\mycalendar"
			${Else}
				DeleteRegValue HKCU "Software\mycalendar" "lpath"
				DeleteRegValue HKCU "Software\mycalendar" "setboot"
			${EndIf}
			${If} $1 != "false"
				;lpath存在
				push $0
				push "\"
				Call ${strCallFlag}GetLastPart
				pop $2
				StrCpy $1 $2 -4
				DeleteRegValue HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" $1
				
				System::Call "$TEMP\${PRODUCT_NAME}\mycalendarsetup::NsisTSLOG(t '_SetSysBoot para 5 = $1')"
				${StrFilter} $0 "-" "" "" $0
				System::Call "$TEMP\${PRODUCT_NAME}\mycalendarsetup::NsisTSLOG(t '_SetSysBoot para 6 = $0')"
				StrCpy $2 "$INSTDIR\program\myfixar.exe"
				${StrFilter} $2 "-" "" "" $2
				System::Call "$TEMP\${PRODUCT_NAME}\mycalendarsetup::NsisTSLOG(t '_SetSysBoot para 7 = $2')"
				${If} $0 != $2
				${AndIf} $0 != "$2"
				${AndIf} $0 != ""
					System::Call "$TEMP\${PRODUCT_NAME}\mycalendarsetup::NsisTSLOG(t '_SetSysBoot will2 rmdir $0')"
					push $0
					Call ${strCallFlag}GetParent
					pop $0
					${WordReplace} $2 $0 "" +1 $1
					${If} "$1" == "$2"
						RMDir /r "$0"
					${Else}
						System::Call "$TEMP\${PRODUCT_NAME}\mycalendarsetup::NsisTSLOG(t '_SetSysBoot can not2 rmdir because is instdir ')"
					${EndIf}
				${EndIf}
			${Else}
				;lpath不存在do nothing
				DeleteRegValue HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "myfixar"
			${EndIf}
		${EndIf}
		pop $3
		pop $2
		pop $1
		pop $0
	FunctionEnd
!macroend
!insertmacro _SetSysBoot "un."
!insertmacro _SetSysBoot ""

!define UnSetSysBoot 'Call un.InstSetSysBoot'
!define SetSysBoot 'Call InstSetSysBoot'

!macro _GetParent strCallFlag
	Function ${strCallFlag}GetParent
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
!macroend
!insertmacro _GetParent ""
!insertmacro _GetParent "un."

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