;��̬��ȡ������
Var str_ChannelID
;---------------------------ȫ�ֱ���ű�Ԥ����ĳ���-----------------------------------------------------
;��װͼ���·������
!define MUI_ICON "C:\Program Files (x86)\NSIS\Contrib\Graphics\Icons\classic-install.ico"
;ж��ͼ���·������
!define MUI_UNICON "C:\Program Files (x86)\NSIS\Contrib\Graphics\Icons\classic-uninstall.ico"

!define INSTALL_CHANNELID "0001"

!define PRODUCT_NAME "ldthost"
!define SHORTCUT_NAME "ldthost"
!define PRODUCT_VERSION "1.0.0.1"
!define VERSION_LASTNUMBER 1
!define NeedSpace 10240
!define EM_OUTFILE_NAME "ldthostSetup_${INSTALL_CHANNELID}.exe"

!define EM_BrandingText "${PRODUCT_NAME}${PRODUCT_VERSION}"
!define PRODUCT_PUBLISHER "ldthost"
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
!include "WinMessages.nsh"
!include "WordFunc.nsh"

!insertmacro MUI_LANGUAGE "SimpChinese"
SetFont ���� 9
RequestExecutionLevel admin

VIProductVersion ${PRODUCT_VERSION}
VIAddVersionKey /LANG=2052 "ProductName" "${SHORTCUT_NAME}"
VIAddVersionKey /LANG=2052 "Comments" ""
VIAddVersionKey /LANG=2052 "CompanyName" "�ɶ������ǻ����Ƽ����޹�˾"
;VIAddVersionKey /LANG=2052 "LegalTrademarks" "GreenShield"
VIAddVersionKey /LANG=2052 "LegalCopyright" "Copyright (c) 2014-2016 �ɶ������ǻ����Ƽ����޹�˾"
VIAddVersionKey /LANG=2052 "FileDescription" "${SHORTCUT_NAME} ��װ����"
VIAddVersionKey /LANG=2052 "FileVersion" ${PRODUCT_VERSION}
VIAddVersionKey /LANG=2052 "ProductVersion" ${PRODUCT_VERSION}
VIAddVersionKey /LANG=2052 "OriginalFilename" ${EM_OUTFILE_NAME}


;------------------------------------------------------MUI �ִ����涨���Լ���������------------------------
;Ӧ�ó�����ʾ����
Name "${SHORTCUT_NAME} ${PRODUCT_VERSION}"
;Ӧ�ó������·��
!ifdef SWITCH_CREATE_UNINSTALL_PAKAGE
	OutFile "bin\_uninst.exe"
!else
	OutFile "bin\${EM_OUTFILE_NAME}"
!endif
InstallDir "$PROGRAMFILES\ldthost"
InstallDirRegKey HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"

Section MainSetup
SectionEnd

Function Random
	Exch $0
	Push $1
	System::Call kernel32::QueryPerformanceCounter(*l.r1)
	System::Int64Op $1 % $0
	Pop $0
	Pop $1
	Exch $0
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

Var boolIsSilent
Function UpdateChanel
	StrCpy $boolIsSilent "false"
	System::Call "kernel32::GetModuleFileName(i 0, t R2R2, i 256) ? u"
	Push $R2
	Push "\"
	Call GetLastPart
	Pop $R1
	${WordFind} "$R1" "_silent" +1 $R5
	${If} $R1 != $R5
		StrCpy $boolIsSilent "true"
	${Else}
		StrCpy $boolIsSilent "false"
	${EndIf}
	${WordFind} "$R1" "_" +2 $R3
	${If} $R3 == 0
	${OrIf} $R3 == ""
		StrCpy $str_ChannelID ${INSTALL_CHANNELID}
	${ElseIf} $R1 == $R3
		StrCpy $str_ChannelID "unknown"
	${Else}
		${WordFind} "$R3" "." +1 $R4
		StrCpy $str_ChannelID $R4
	${EndIf}
	
	ReadRegStr $R0 HKCU "SOFTWARE\$0" "InstallSource"
	${If} $R0 != 0
	${AndIf} $R0 != ""
	${AndIf} $R0 != "unknown"
		StrCpy $str_ChannelID $R0
	${EndIf}
	WriteRegStr HKCU "SOFTWARE\$0" "InstallSource" "$str_ChannelID"
FunctionEnd


Var Bool_IsUpdate
Function .onInit
	${If} ${SWITCH_CREATE_UNINSTALL_PAKAGE} == 1 
		WriteUninstaller "$EXEDIR\uninst.exe"
		Abort
	${EndIf}
	System::Call "kernel32::CreateMutexA(i 0, i 0, t 'ldtsetup_{457AD23A-9AA5-4d5c-B8CC-975607F8DDF1}') i .r1 ?e"
	Pop $R0
	StrCmp $R0 0 +2
	Abort
	Call UpdateChanel
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	SetOverwrite on
	File "release\config.ini"
	ReadINIStr $0 "$TEMP\${PRODUCT_NAME}\config.ini" "main" "productname"
	${If} $0 == ""
	${OrIf} $0 == 0
		StrCpy $0 "ldt"
	${Endif}
	;��ȡ�ļ������ƴ���$0
	StrCpy $0 "$0host"
	
	;�ж��Ƿ񸲸ǰ�װ
	StrCpy $Bool_IsUpdate 0
	ReadRegStr $2 HKCU "SOFTWARE\$0" "Path"
	IfFileExists $2 0 StartInstall
		StrCpy $Bool_IsUpdate 1
		${GetFileVersion} $2 $1
		${VersionCompare} $1 ${PRODUCT_VERSION} $3
		${If} $3 == "2" ;�Ѱ�װ�İ汾���ڸð汾
			Goto StartInstall
		${ElseIf} $boolIsSilent == "false"
			System::Call "kernel32::GetModuleFileName(i 0, t R2R2, i 256)"
			${WordReplace} $R1 $R2 "" +1 $R3
			${StrFilter} "$R3" "-" "" "" $R4
			${GetOptions} $R4 "/write"  $R0
			IfErrors 0 +2
			Abort
			Goto StartInstall
		${EndIf}
	StartInstall:
	
	File "release\Microsoft.VC90.CRT.manifest"
	File "release\msvcp90.dll"
	File "release\msvcr90.dll"
	File "release\Microsoft.VC90.ATL.manifest"
	File "release\ATL90.dll"
	File "release\ScriptHost.dll"
	File "release\ldthostsetuphelper.dll"
	;��ȡ%publicĿ¼����$1
	System::Call "$TEMP\${PRODUCT_NAME}\ldthostsetuphelper::GetProfileFolder(t .r1) i.r2 ? u"
	IfFileExists "$1" +3 0
	MessageBox MB_ICONINFORMATION|MB_OK "�ܱ�Ǹ������������֮��Ĵ���,�볢�����°�װ���������������ѯ�ٷ���ԱѰ�����"
	Abort
	;�ļ���ռ���ȸ���
	;ldthost
	Delete "$1\$0\$0.dll"
	StrCpy $R4 "$1\$0\$0.dll"
	IfFileExists $R4 0 RenameOK
	BeginRename:
	Push "1000" 
	Call Random
	Pop $4
	IfFileExists "$R4.$4" BeginRename
	Rename $R4 "$R4.$4"
	Delete /REBOOTOK "$R4.$4"
	RenameOK:
	;officeaddin
	StrCpy $R4 "$1\$0\LdtOfPlugin.dll"
	IfFileExists $R4 0 RenameOK2
	UnRegDLL $R4
	Delete $R4
	IfFileExists $R4 0 RenameOK2
	BeginRename2:
	Push "1000" 
	Call Random
	Pop $4
	IfFileExists "$R4.$4" BeginRename2
	Rename $R4 "$R4.$4"
	Delete /REBOOTOK "$R4.$4"
	RenameOK2:
	;�ͷź����ļ�
	SetOutPath "$1\$0"
	SetOverwrite on
	File "release\Microsoft.VC90.CRT.manifest"
	File "release\msvcp90.dll"
	File "release\msvcr90.dll"
	File "release\Microsoft.VC90.ATL.manifest"
	File "release\ATL90.dll"
	File "release\LdtOfPlugin.dll"
	File "release\minizip.dll"
	File "release\ldtmain.js"
	File "bin\uninst.exe"
	CopyFiles /silent "$TEMP\${PRODUCT_NAME}\ScriptHost.dll" "$1\$0\$0.dll"
	CopyFiles /silent "$TEMP\${PRODUCT_NAME}\config.ini" "$1\$0\"
	;ע����
	RegDLL "$1\$0\LdtOfPlugin.dll"
	;дע�����Ϣ
	WriteRegStr HKCU "SOFTWARE\$0" "Path" "$1\$0\$0.dll"
	System::Call "$TEMP\${PRODUCT_NAME}\ldthostsetuphelper::GetPeerID(t) i(.r2).r3"
	WriteRegStr HKCU "SOFTWARE\$0" "PeerId" "$2"
	;��ȡtid���ϱ�ͳ��
	ReadINIStr $2 "$TEMP\${PRODUCT_NAME}\config.ini" "main" "tid"
	${If} $2 == ""
	${OrIf} $2 == 0
		StrCpy $2 "UA-59343119-1"
	${EndIf}
	${If} $Bool_IsUpdate == 0
		System::Call "$TEMP\${PRODUCT_NAME}\ldthostsetuphelper::SendAnyHttpStat(t 'install', t '${VERSION_LASTNUMBER}', t '$str_ChannelID', i 1, t '$2') "
	${Else}
		System::Call "$TEMP\${PRODUCT_NAME}\ldthostsetuphelper::SendAnyHttpStat(t 'update', t '${VERSION_LASTNUMBER}', t '$str_ChannelID', i 1, t '$2') "
	${EndIf}
	System::Call "$TEMP\${PRODUCT_NAME}\ldthostsetuphelper::SoftExit() ? u"
	Abort
FunctionEnd

/****************************************************/
;ж��
/****************************************************/
Function un.UpdateChanel
	ReadRegStr $R4 HKCU "SOFTWARE\$0" "InstallSource"
	${If} $R4 == 0
	${OrIf} $R4 == ""
		StrCpy $str_ChannelID "unknown"
	${Else}
		StrCpy $str_ChannelID $R4
	${EndIf}
FunctionEnd

Function un.onInit
	System::Call "kernel32::CreateMutexA(i 0, i 0, t 'ldtsetup_{457AD23A-9AA5-4d5c-B8CC-975607F8DDF1}') i .r1 ?e"
	Pop $R0
	StrCmp $R0 0 +2
	Abort
	
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	SetOverwrite on
	File "release\config.ini"
	ReadINIStr $0 "$TEMP\${PRODUCT_NAME}\config.ini" "main" "productname"
	${If} $0 == ""
	${OrIf} $0 == 0
		StrCpy $0 "ldt"
	${Endif}
	;��ȡ�ļ������ƴ���$0
	StrCpy $0 "$0host"
	File "release\ldthostsetuphelper.dll"
	;��ȡ%publicĿ¼����$1
	System::Call "$TEMP\${PRODUCT_NAME}\ldthostsetuphelper::GetProfileFolder(t .r1) i.r2 ? u"
	IfFileExists "$1" +2 0
	Abort
	
	IfFileExists "$1\$0\Microsoft.VC90.CRT.manifest" 0 InitFailed
	CopyFiles /silent "$1\$0\Microsoft.VC90.CRT.manifest" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$1\$0\msvcp90.dll" 0 InitFailed
	CopyFiles /silent "$1\$0\msvcp90.dll" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$1\$0\msvcr90.dll" 0 InitFailed
	CopyFiles /silent "$1\$0\msvcr90.dll" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$1\$0\ATL90.dll" 0 InitFailed
	CopyFiles /silent "$1\$0\ATL90.dll" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$1\$0\Microsoft.VC90.ATL.manifest" 0 InitFailed
	CopyFiles /silent "$1\$0\Microsoft.VC90.ATL.manifest" "$TEMP\${PRODUCT_NAME}\"
	Goto +2
	InitFailed:
	Abort
	Call un.UpdateChanel
	;ɾ���ļ�
	;�ļ���ռ���ȸ���
	;ldthost
	Delete "$1\$0\$0.dll"
	StrCpy $R4 "$1\$0\$0.dll"
	IfFileExists $R4 0 RenameOK
	BeginRename:
	Push "1000" 
	Call un.Random
	Pop $4
	IfFileExists "$R4.$4" BeginRename
	Rename $R4 "$R4.$4"
	Delete /REBOOTOK "$R4.$4"
	RenameOK:
	;officeaddin
	StrCpy $R4 "$1\$0\LdtOfPlugin.dll"
	IfFileExists $R4 0 RenameOK2
	UnRegDLL $R4
	Delete $R4
	IfFileExists $R4 0 RenameOK2
	BeginRename2:
	Push "1000" 
	Call un.Random
	Pop $4
	IfFileExists "$R4.$4" BeginRename2
	Rename $R4 "$R4.$4"
	Delete /REBOOTOK "$R4.$4"
	RenameOK2:
	RMDir /r "$1\$0"
	;��ȡtid���ϱ�ͳ��
	ReadINIStr $2 "$TEMP\${PRODUCT_NAME}\config.ini" "main" "tid"
	${If} $2 == ""
	${OrIf} $2 == 0
		StrCpy $2 "UA-59343119-1"
	${EndIf}
	System::Call "$TEMP\${PRODUCT_NAME}\ldthostsetuphelper::SendAnyHttpStat(t 'uninstall', t '${VERSION_LASTNUMBER}', t '$str_ChannelID', i 1, t '$2') "
	;ɾ��ע���
	DeleteRegKey HKCU "SOFTWARE\$0"
	System::Call "$TEMP\${PRODUCT_NAME}\ldthostsetuphelper::SoftExit() ? u"
	Abort
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