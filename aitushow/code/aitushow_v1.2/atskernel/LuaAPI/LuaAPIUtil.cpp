#include "StdAfx.h"
#pragma warning(disable: 4995)
#include <TlHelp32.h>

#include <atlsync.h>
#include <atltime.h>
#include <WTL/atldlgs.h>
#include "../Utility/DatFileUtility.h"
#include "../Utility/LuaAPIHelper.h"
#include "../Utility/PeeIdHelper.h"
#include "../Utility/AES.h"
#include "../Utility/base64.h"
#include "../Utility/FileAssociation.h"
#include <comdef.h>
#include "../XLUEApplication.h"
#include "../EvenListenHelper/LuaMsgWnd.h"
#include "commonshare\md5.h"
#include "LuaAPIUtil.h"
#include "LuaNotifyIcon.h"
#include <MsHtmcid.h>

#include <mshtml.h> 
#include <Exdisp.h>
#include "../EvenListenHelper/LuaMsgWnd.h"
#include "freeimage/FreeImage.h"
#include "..\ImageHelper\ImageUtility.h"
#include "..\Utility\FolderChangeMonitor.h"
#include "..\Utility\KKFolderDialog.h"
#include "..\ImageHelper\ImageProcessor.h"

extern HANDLE g_hInst;
extern CXLUEApplication theApp;

#define  CanSetToWallPaperFileExt	L".jpg;.jpe;.jpeg;.png;.bmp;"		// 可以设为壁纸的文件后缀名

LuaAPIUtil::LuaAPIUtil(void)
{
}

LuaAPIUtil::~LuaAPIUtil(void)
{
}

XLLRTGlobalAPI LuaAPIUtil::sm_LuaMemberFunctions[] = 
{
	//{"RegisterFilterWnd", RegisterFilterWnd},	
	//主要函数
	{"Lua_Gc", LuaGc},
	{"MsgBox",MsgBox},
	{"Exit", Exit},	
	{"GetPeerId", GetPeerId},
	{"Log", Log},
	{"IsLogEnable", IsLogEnable},
	{"SaveLuaTableToLuaFile", SaveLuaTableToLuaFile},
	{"GetCommandLine", GetCommandLine},
	{"CommandLineToList", CommandLineToList},
	{"GetModuleExeName", GetModuleExeName},
	{"GetScreenRectFromPoint", GetScreenRectFromPoint},

	{"GetWorkArea", GetWorkArea},
	{"GetScreenArea", GetScreenArea},
	{"GetScreenSize", GetScreenSize},
	{"GetCursorPos", GetCursorPos},
	{"PostWndMessage", PostWndMessage},
	{"GetSysWorkArea", GetSysWorkArea},
	{"GetCurrentScreenRect", GetCurrentScreenRect},
	{"GetDesktopWndHandle", FGetDesktopWndHandle}, 
	{"SetWndPos", FSetWndPos}, 
	{"ShowWnd", FShowWnd}, 
	{"GetWndRect", FGetWndRect}, 
	{"GetWndClientRect", FGetWndClientRect}, 
	{"FindWindow", FFindWindow}, 
	{"FindWindowEx", FFindWindowEx},
	{"IsWindowVisible", FIsWindowVisible},
	{"IsWindowIconic", IsWindowIconic},
	{"GetWindowTitle", GetWindowTitle},
	{"GetWndClassName", GetWndClassName},
	{"GetWndProcessThreadId", GetWndProcessThreadId},
	{"PostWndMessageByHandle", PostWndMessageByHandle},
	{"SendMessageByHwnd", SendMessageByHwnd},
	{"IsNowFullScreen", IsNowFullScreen},
	{"GetForegroundWindow", FGetForegroundWindow},
	{"SetForegroundWindow", FSetForegroundWindow},

	{"GetCursorWndHandle", GetCursorWndHandle},
	{"GetFocusWnd", GetFocusWnd},
	{"GetKeyState", FGetKeyState},

	//文件
	{"GetMD5Value", GetMD5Value},
	{"GetStringMD5", GetStringMD5},
	{"GetFileVersionString", GetFileVersionString},
	{"GetSystemTempPath", GetSystemTempPath},
	{"GetFileSize", GetFileSize},
	{"GetFileCreateTime", GetFileCreateTime},
	{"GetTmpFileName", GetTmpFileName},
	{"GetSpecialFolderPathEx", GetSpecialFolderPathEx}, 
	{"FindFileList", FindFileList},
	{"FindDirList", FindDirList},
	{"PathCombine", PathCombine},
	{"ExpandEnvironmentStrings", ExpandEnvironmentString},	
	{"QueryFileExists", QueryFileExists},
	{"Rename", Rename},
	{"CreateDir", CreateDir},
	{"CreatePathFile", CreatePathFile},
	{"DragAcceptFiles", DragAcceptFiles},
	{"DragQueryFile", DragQueryFile},
	{"CopyPathFile", CopyPathFile},
	{"DeletePathFile", DeletePathFile},
	//读写UTF8文件
	{"ReadFileToString", ReadFileToString},
	{"WriteStringToFile", WriteStringToFile},
	{"GetLogicalDrive", GetLogicalDrive},

	//注册表操作
	{"QueryRegValue", QueryRegValue},
	{"QueryRegValue64", QueryRegValue64},
	{"DeleteRegValue", DeleteRegValue},
	{"DeleteRegValue64", DeleteRegValue64},
	{"CreateRegKey", CreateRegKey},
	{"CreateRegKey64", CreateRegKey64},
	{"DeleteRegKey", DeleteRegKey},
	{"DeleteRegKey64", DeleteRegKey64},
	{"SetRegValue", SetRegValue},
	{"SetRegValue64", SetRegValue64},
	{"QueryRegKeyExists", QueryRegKeyExists}, 
	{"EnumRegLeftSubKey", EnumRegLeftSubKey}, 
	{"EnumRegRightSubKey", EnumRegRightSubKey}, 

	//时间函数
	{"GetCurTimeSpan", GetCurTimeSpan},
	{"FormatCrtTime", FormatCrtTime},
	{"GetLocalDateTime", GetLocalDateTime},
	{"GetCurrentUTCTime", GetCurrentUTCTime},
	{"DateTime2Seconds", DateTime2Seconds},
	{"Seconds2DateTime", Seconds2DateTime},

	//互斥量函数
	{"CreateMutex", CreateNamedMutex},
	{"CloseMutex", CloseNamedMutex},
	
	//系统
	{"GetCurrentProcessId", FGetCurrentProcessId},
	{"GetAllSystemInfo", FGetAllSystemInfo},
	{"GetProcessIdFromHandle", FGetProcessIdFromHandle},
	{"GetTickCount", GetTotalTickCount},
	{"GetOSVersion", GetOSVersionInfo},
	{"QueryProcessExists", QueryProcessExists},
	{"IsWindows8Point1",IsWindows8Point1},
	//功能
	{"CreateShortCutLinkEx", CreateShortCutLinkEx},
	{"OpenURL", OpenURL},
	{"OpenURLIE", OpenURLIE},
	{"ShellExecute", ShellExecuteEX},


	{"EncryptAESToFile", EncryptAESToFile},
	{"DecryptFileAES", DecryptFileAES},
	
	{"EncryptString", EncryptString},
	{"DecryptString", DecryptString},

	{"IsClipboardFormatAvailable",FIsClipboardFormatAvailable},
	//{"FileDialog",FileDialog},
	{"BrowserForFile",BrowserForFile},
	//INI配置文件操作
	{"ReadINI", ReadINI},
	{"WriteINI", WriteINI},
	{"ReadStringUtf8", ReadStringUtf8},
	{"ReadSections", ReadSections},
	{"ReadKeyValueInSection", ReadKeyValueInSection},
	{"ReadINIInteger", ReadINIInteger},
	//快捷键
	{"SetKeyboardHook", FSetKeyboardHook},
	{"DelKeyboardHook", FDelKeyboardHook},
	//文件关联
	{"IsAssociated", IsAssociated},
	{"SetAssociate", SetAssociate},


	{"UpdateAiSvr", UpdateAiSvr},
	{"LaunchAiSvr", LaunchAiSvr},

	{"SetFileToClipboard", SetFileToClipboard},
	


	//新增Appobj
	{"GetTempDir", GetTempDir},
	{"GetProfilesDir", GetProfilesDir},			// 获取Profiles路径
	{"ForceUpdateWndShow", ForceUpdateWndShow},
	{"GetFiles", GetFiles},
	{"GetFileInfoByPath", GetFileInfoByPath},
	{"GetFolders", GetFolders},
	{"StrColl", StrColl},
	{"GetScreenRatio",  GetScreenRatio},
	{"IsPathFileExist", IsPathFileExist},
	{"LogToFile", LogToFile},
	{"GetWorkAreaSize", GetWorkAreaSize},
	{"GetMd5Str", GetMd5Str},
	{"InitFolderMonitor", InitFolderMonitor},
	{"MonitorDirChange", MonitorDirChange},
	{"UnMonitorDirChange", UnMonitorDirChange},
	{"AttachDirChangeEvent", AttachDirChangeEvent},
	{"DetachDirChangeEvent", DetachDirChangeEvent},
	{"IsCanHandleFileCheckByExt", IsCanHandleFileCheckByExt},
	{"DelPathFile2RecycleBin", DelPathFile2RecycleBin},
	{"PrintImage", PrintImage},
	{"CopyFilePathToCLipBoard", CopyFilePathToCLipBoard},
	{"CopyImageToClipboard", CopyImageToClipboard},
	{"KKFolderDialog", KKFolderDialog},
	{"MoveFileTo", MoveFileTo},
	{"CopyFileTo", CopyFileTo},
	{"RenameFile", RenameFile},
	{"SHChangeNotify", SHChangeNotify},
	{"GetCurveProgress", GetCurveProgress},
	{"IsCanSetToWallPaperFile", IsCanSetToWallPaperFile},
	{"GetPeerId", GetPeerId},
	{"GetOSInfo", GetOSInfo},
	{"GetSystemRatio", GetSystemRatio},
	{"CopyTextToClipboard", CopyTextToClipboard},
	{"GetIconFromExeFile", GetIconFromExeFile},
	{"ShowColorDialog", ShowColorDialog},
	{"GetExeFileDes", GetExeFileDes},
	{"SetShowCursor", SetShowCursor},	



	//新增OS Shell
	{"GetStringLength", GetStringLength },
	{"IsClipboardTextFormatAvailable", IsClipboardTextFormatAvailable },
	{"RegisterHotKey", RegisterHotKeyEx },
	{"UnregisterHotKey", UnregisterHotKeyEx },
	{"GlobalAddAtom", GlobalAddAtomEx },
	{"GlobalDeleteAtom", GlobalDeleteAtomEx },
	{"GetKeyState", GetKeyState },
	{"Execute",Execute},
	{"IsValidFileName", IsValidFileName},
	{"ShellOpen", ShellOpen},
	{"SetDesktopWallpaper", SetDesktopWallpaper},
	{"FolderDialog", FolderDialog},
	{"FileDialog", FileDialog},
	{NULL, NULL}
};

int LuaAPIUtil::LuaGc(lua_State* luaState)
{
	theApp.ReleaseCache();
	return 0;
}

int LuaAPIUtil::MsgBox(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	if (!lua_isstring(pLuaState,2) && !lua_isstring(pLuaState,3))
	{
		return 0;
	}
	const char* utf8Text = luaL_checkstring(pLuaState, 2);
	const char* utf8Title = luaL_checkstring(pLuaState, 3);

	CComBSTR bstrTitle,bstrText;
	LuaStringToCComBSTR(utf8Title,bstrTitle);
	LuaStringToCComBSTR(utf8Text,bstrText);
	
	UINT uType = MB_OK;
	if ( !lua_isnoneornil( pLuaState, 4 ))
	{
		uType = (int)lua_tointeger( pLuaState, 4);
	}
	MessageBox(NULL,bstrText.m_str,bstrTitle.m_str,uType);
	return 1;
}

int LuaAPIUtil::Exit(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	gsNotifyIcon.Hide();
	theApp.ExitInstance();
	return 0;
}

int LuaAPIUtil::GetPeerId(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}

	std::wstring strPeerId=L"";
	GetPeerId_(strPeerId);
	wchar_t szPeerId[MAX_PATH] = {0};
	wcsncpy(szPeerId,strPeerId.c_str(),strPeerId.size());

	std::string strUtf8Pid;
	BSTRToLuaString(szPeerId,strUtf8Pid);
	lua_pushstring(pLuaState, strUtf8Pid.c_str());
	return 1;
}

int LuaAPIUtil::GetWorkArea(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		int x = 1;
		int y = 1;
		if ( !lua_isnoneornil( pLuaState, 2 ) && !lua_isnoneornil( pLuaState, 3 ) )
		{
			x = (int)lua_tointeger( pLuaState, 2 );
			y = (int)lua_tointeger( pLuaState, 3 );
		}
		POINT pt;
		pt.x = x;
		pt.y = y;
		HMONITOR hMonitor = MonitorFromPoint( pt, MONITOR_DEFAULTTONEAREST );
		MONITORINFO info;
		ZeroMemory( &info, sizeof( info ) );
		info.cbSize = sizeof( info );
		if (GetMonitorInfo( hMonitor, &info ))
		{
			lua_pushinteger( pLuaState, info.rcWork.left );
			lua_pushinteger( pLuaState, info.rcWork.top );
			lua_pushinteger( pLuaState, info.rcWork.right );
			lua_pushinteger( pLuaState, info.rcWork.bottom );
			return 4;
		}
	}
	lua_pushinteger(pLuaState, 0);
	lua_pushinteger(pLuaState, 0);
	lua_pushinteger(pLuaState, 0);
	lua_pushinteger(pLuaState, 0);
	return 4;
}
int LuaAPIUtil::IsLogEnable(lua_State* pLuaState)
{
	if (ISTSDEBUGVALID())
		lua_pushboolean(pLuaState, 1);
	else
		lua_pushboolean(pLuaState, 0);

	return 1;
}
int LuaAPIUtil::GetScreenArea(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		int x = 1;
		int y = 1;
		if ( !lua_isnoneornil( pLuaState, 2 ) && !lua_isnoneornil( pLuaState, 3 ) )
		{
			x = (int)lua_tointeger( pLuaState, 2 );
			y = (int)lua_tointeger( pLuaState, 3 );
		}
		POINT pt;
		pt.x = x;
		pt.y = y;
		HMONITOR hMonitor = MonitorFromPoint( pt, MONITOR_DEFAULTTONEAREST );
		MONITORINFO info;
		ZeroMemory( &info, sizeof( info ) );
		info.cbSize = sizeof( info );
		if (GetMonitorInfo( hMonitor, &info ))
		{
			lua_pushinteger( pLuaState, info.rcMonitor.left );
			lua_pushinteger( pLuaState, info.rcMonitor.top );
			lua_pushinteger( pLuaState, info.rcMonitor.right );
			lua_pushinteger( pLuaState, info.rcMonitor.bottom );
			return 4;
		}
	}
	lua_pushinteger(pLuaState, 0);
	lua_pushinteger(pLuaState, 0);
	lua_pushinteger(pLuaState, 0);
	lua_pushinteger(pLuaState, 0);
	return 4;
}

int LuaAPIUtil::GetScreenSize(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	int iScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int iScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);
	lua_pushnumber(pLuaState, iScreenWidth);
	lua_pushnumber(pLuaState, iScreenHeight);
	return 2;
}

int LuaAPIUtil::GetCursorPos(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	POINT pt;
	::GetCursorPos(&pt);
	lua_pushnumber(pLuaState, pt.x);
	lua_pushnumber(pLuaState, pt.y);
	return 2;
}
int LuaAPIUtil::GetScreenRectFromPoint(lua_State* luaState)
{
	POINT point;
	point.x = (LONG)lua_tointeger(luaState, 3);
	point.y = (LONG)lua_tointeger(luaState, 4);
	RECT rect = {0};
	HMONITOR hMonitor = MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = {sizeof(MONITORINFO)};

	if(hMonitor == NULL || !GetMonitorInfo(hMonitor, &mi))
	{
		//TSERROR(_T("hMonitor: 0x%p, hMonitor is NULL or GetMonitorInfo failed"), hMonitor);
		rect.left = 0;
		rect.top = 0;
		rect.right = GetSystemMetrics(SM_CXSCREEN);
		rect.bottom = GetSystemMetrics(SM_CYSCREEN);
	}
	else
	{
		rect = mi.rcMonitor;
	}
	lua_pushinteger(luaState, rect.left);
	lua_pushinteger(luaState, rect.top);
	lua_pushinteger(luaState, rect.right);
	lua_pushinteger(luaState, rect.bottom);
	return 4;
}

// 获取命令行参数(不包含可执行程序路径)
int LuaAPIUtil::GetCommandLine(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	std::string strUtf8 = "";

	std::wstring wstrCommandLine = theApp.GetCommandLine();
	 
	if (!wstrCommandLine.empty())
	{	
		wchar_t szCmd[MAX_PATH] = {0};
		wcsncpy(szCmd,wstrCommandLine.c_str(),wstrCommandLine.size());
		BSTRToLuaString(szCmd, strUtf8);
	}

	lua_pushstring(pLuaState, strUtf8.c_str());
	return 1;
}

int LuaAPIUtil::GetFileVersionString(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* utf8FilePath = luaL_checkstring(pLuaState, 2);
		if (utf8FilePath == NULL) 
		{
			return 0;
		}
		long nfileExist = QueryFileExistsHelper(utf8FilePath);
		if(nfileExist == 0)
		{
			return 0;
		}
		
		CComBSTR bstr;

		if(utf8FilePath)
		{
			LuaStringToCComBSTR(utf8FilePath,bstr);
		}

		DWORD dwHandle = 0;
		DWORD dwSize = ::GetFileVersionInfoSizeW(bstr.m_str, &dwHandle);
		std::string utf8Version;
		if(dwSize > 0)
		{
			TCHAR * pVersionInfo = new TCHAR[dwSize+1];
			if(::GetFileVersionInfo(bstr.m_str, dwHandle, dwSize, pVersionInfo))
			{
				VS_FIXEDFILEINFO * pvi;
				UINT uLength = 0;
				if(::VerQueryValueA(pVersionInfo, "\\", (void **)&pvi, &uLength))
				{
					TCHAR szVer[MAX_PATH] = {0};
					swprintf(szVer, L"%d.%d.%d.%d",
						HIWORD(pvi->dwFileVersionMS), LOWORD(pvi->dwFileVersionMS),
						HIWORD(pvi->dwFileVersionLS), LOWORD(pvi->dwFileVersionLS));
					BSTRToLuaString(szVer, utf8Version);
				}
			}
			delete pVersionInfo;
		}
		lua_pushstring(pLuaState, utf8Version.c_str());
		return 1;
	}

	lua_pushnil(pLuaState);
	return 1;
}


int LuaAPIUtil::GetMD5Value(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* utf8FilePath = lua_tostring(pLuaState,2);
		if (utf8FilePath != NULL)
		{
			CComBSTR bstrFilePath;
			LuaStringToCComBSTR(utf8FilePath,bstrFilePath);

			wchar_t pszMD5[MAX_PATH] = {0};
			std::wstring wstrPath = bstrFilePath.m_str;
			if (GetMd5(wstrPath,pszMD5))
			{

				std::string utf8MD5;
				BSTRToLuaString(pszMD5, utf8MD5);
				lua_pushstring(pLuaState, utf8MD5.c_str());
			}
			return 1;
		}
	}
	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::GetStringMD5(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* utf8str = lua_tostring(pLuaState,2);
		if (utf8str != NULL)
		{
			wchar_t pszMD5[MAX_PATH] = {0};
			if (GetStringMd5(utf8str,pszMD5))
			{
				std::string utf8MD5;
				BSTRToLuaString(pszMD5, utf8MD5);
				lua_pushstring(pLuaState, utf8MD5.c_str());
			}
			return 1;
		}
	}
	lua_pushnil(pLuaState);
	return 1;
}


int LuaAPIUtil::GetSystemTempPath(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	wchar_t szPath[MAX_PATH] = {0};
	DWORD len = GetTempPath(MAX_PATH, szPath);
	if(len > 0)
	{
		std::string utf8TempPath;
		BSTRToLuaString(szPath, utf8TempPath);
		lua_pushstring(pLuaState, utf8TempPath.c_str());
		lua_pushboolean(pLuaState, 1);
		return 2;
	}
	lua_pushboolean(pLuaState, 0);
	return 1;
}

__int64 LuaAPIUtil::GetFileSizeHelper(const char* utf8FileFullPath)
{
	long nfileExist = QueryFileExistsHelper(utf8FileFullPath);
	if(nfileExist == 0)
	{
		return -1;
	}

	CComBSTR bstr;

	if(utf8FileFullPath)
	{
		LuaStringToCComBSTR(utf8FileFullPath,bstr);
	}

	HANDLE hFile = CreateFile(bstr.m_str, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	LARGE_INTEGER li;
	BOOL bRet = GetFileSizeEx(hFile,&li);
	CloseHandle( hFile );
	if(!bRet)
	{
		return -1;
	}

	return li.QuadPart;
}

int LuaAPIUtil::GetFileSize(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* filePath = luaL_checkstring(pLuaState, 2);
		__int64 nFileSize = GetFileSizeHelper(filePath);
		lua_pushnumber(pLuaState,(lua_Number)nFileSize);
		return 1;
	}

	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::GetFileCreateTime(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* utf8FilePath = luaL_checkstring(pLuaState, 2);
		CComBSTR bstrFilePath;
		LuaStringToCComBSTR(utf8FilePath,bstrFilePath);
		
		HANDLE hFile = INVALID_HANDLE_VALUE;
		hFile = CreateFile(bstrFilePath.m_str, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			FILETIME ftCreate, ftAccess, ftWrite;
			if (0 != GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
			{
				CloseHandle(hFile);
				SYSTEMTIME stUTC, stLocal;
				FileTimeToSystemTime(&ftCreate, &stUTC);
				SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

				lua_pushnumber(pLuaState, stLocal.wYear);
				lua_pushnumber(pLuaState, stLocal.wMonth);
				lua_pushnumber(pLuaState, stLocal.wDay);
				lua_pushnumber(pLuaState, stLocal.wHour);
				lua_pushnumber(pLuaState, stLocal.wMinute);
				lua_pushnumber(pLuaState, stLocal.wSecond);
				lua_pushnumber(pLuaState, stLocal.wDayOfWeek);
				return 7;
			}
			CloseHandle(hFile);
		}
	}

	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::GetCurTimeSpan(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		int nYear = 0, nMonth = 0, nDate = 0, nHour = 0, nMinute = 0, nSeconds = 0;
		nYear = (int)lua_tointeger(pLuaState, 2);
		nMonth = (int)lua_tointeger(pLuaState, 3);
		nDate = (int)lua_tointeger(pLuaState, 4);
		nHour = (int)lua_tointeger(pLuaState, 5);
		nMinute = (int)lua_tointeger(pLuaState, 6);
		nSeconds = (int)lua_tointeger(pLuaState, 7);
		CTime tm1(nYear, nMonth, nDate, nHour, nMinute, nSeconds);

		SYSTEMTIME systemTime;
		::GetLocalTime(&systemTime);
		CTime tm2(systemTime);

		CTimeSpan ts = tm2 - tm1;
		LONGLONG llHourSpan = ts.GetTotalHours();
		LONGLONG llMinuteSpan = ts.GetTotalMinutes();
		LONGLONG llSecSpan = ts.GetTotalSeconds();
		lua_pushnumber(pLuaState, (lua_Number)llHourSpan);
		lua_pushnumber(pLuaState, (lua_Number)llMinuteSpan);
		lua_pushnumber(pLuaState, (lua_Number)llSecSpan);
		return 3;
	}

	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::GetTmpFileName(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		wchar_t* pwTemplate = L"TWXXXXXXXX";
		wchar_t szTmp[11] = {0};
		wcscpy(szTmp, pwTemplate);
		wchar_t* szFileName = _wmktemp(szTmp);
		if (szFileName != NULL)
		{
			std::string utf8FileName;
			BSTRToLuaString(szFileName,utf8FileName);
			lua_pushstring(pLuaState, utf8FileName.c_str());
			return 1;
		}
	}
	lua_pushnil(pLuaState);
	return 1;
}


int LuaAPIUtil::GetSpecialFolderPathEx(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		int iCLSID = (int)lua_tointeger(pLuaState, 2);
		TCHAR szPath[MAX_PATH] = {0};
		if (SHGetSpecialFolderPath(NULL, szPath, iCLSID, 0))
		{
			std::string strFilePath;
			BSTRToLuaString(szPath,strFilePath);
			lua_pushstring(pLuaState, strFilePath.c_str());
			return 1;
		}
	}
	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::Log(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		if (ISTSDEBUGVALID())
		{
			const char* szInput = lua_tostring(pLuaState, 2);
			CComBSTR bstrInput;
			LuaStringToCComBSTR(szInput,bstrInput);
			TSDEBUG4CXX(L"[DDKernel] " << bstrInput.m_str);
		}
	}
	return 0;
}

void LuaAPIUtil::ConvertAllEscape(std::string& strSrc)
{
	char szEscape[] = {'\\', '\'', '\"', '\?', '\0'};
	for (std::string::size_type i = 0; i < strSrc.length(); i++)
	{
		for (size_t j = 0; j < strlen(szEscape); j++)
		{
			if (strSrc[i] == szEscape[j])
			{
				strSrc.insert(i, "\\");
				i++;
				break;
			}
		}
	}
}

std::string LuaAPIUtil::GetTableStr(lua_State* luaState, int nIndex, std::ofstream& ofs, const std::string strTableName, int nFloor)
{
	bool bTable = lua_istable(luaState, nIndex);
	ATLASSERT(bTable);
	if (!bTable)
		return false;
	lua_pushnil(luaState);
	std::string strTableAll(strTableName + " = {\r\n");

	std::map<int, std::string> mapArrary;
	while (lua_next(luaState, nIndex)) 
	{
		std::string strKey;
		int t = lua_type(luaState, -2);

		std::string strTable("");
		int iIndex = -1;
		//key
		if(lua_isnumber(luaState, -2) && t == LUA_TNUMBER)
		{
			int n = (int)lua_tointeger(luaState, -2);
			char szIndex[30] = {0};
			itoa(n, szIndex, 10);
			strKey += "[";
			strKey += szIndex;
			strKey += "]";
			iIndex = n;
		}
		else if(lua_isstring(luaState, -2))
		{
			const char* szKey = (const char*)lua_tostring(luaState, -2);

			strKey += "[\"";
			strKey += szKey;
			strKey += "\"]";
		}
		else
		{
			ATLASSERT(FALSE && "table key only support number or string!");
		}
		// 带#号的临时属性不保存
		if(strKey[2] == '#')
		{
			lua_pop(luaState, 1);
			continue;
		}

		//value
		if(lua_istable(luaState, -1))
		{
			//strTable += "\r\n";
			for(int i = 0; i < nFloor; i++)
				strTable += "\t";
			//ofs.write(strTable.c_str(), (std::streamsize)strTable.length());
			strTable += GetTableStr(luaState, lua_gettop(luaState), ofs, strKey, nFloor + 1);
			strTable += ", \r\n";
		}
		else
		{
			for(int i = 0; i < nFloor; i++)
				strTable += "\t";
			strTable += strKey;
			strTable += " = ";
			t = lua_type(luaState, -1);
			if(lua_isboolean(luaState, -1))
			{
				int b = lua_toboolean(luaState, -1);
				strTable += (b ? "true" : "false");
				strTable += ", \r\n";
			}
			else if(lua_isnumber(luaState, -1) && t == LUA_TNUMBER)
			{
				double dbValue = (double)lua_tonumber(luaState, -1);
				char szValue[30] = {0};
				//itoa(nValue, szValue, 10);
				sprintf(szValue, "%f", dbValue);
				char* p = szValue + strlen(szValue) - 1;
				while(*p == '0')
					p--;
				if (*p == '.')
					p--;
				*(p+1) = '\0';
				strTable += szValue;
				strTable += ", \r\n";
			}
			else if(lua_isstring(luaState, -1))
			{
				std::string strValue = (const char*)lua_tostring(luaState, -1);
				ConvertAllEscape(strValue);
				strTable += "\"";
				strTable += strValue;
				strTable += "\", \r\n";
			}
			// table的已经内部弹出堆栈了，非table才用弹出堆栈
			lua_pop(luaState, 1);
		}
		if(0 > iIndex)
		{
			strTableAll += strTable;
		}
		else
		{
			mapArrary.insert(std::pair<int, std::string>(iIndex, strTable));
		}
	}
	lua_pop( luaState, 1 );
	for(std::map<int, std::string>::iterator it = mapArrary.begin(); it != mapArrary.end(); it++ )
	{
		strTableAll += it->second;
	}
	for(int i = 0; i < nFloor - 1; i++)
		strTableAll += "\t";
	strTableAll += "}";
	//ofs.write(strTable.c_str(), (std::streamsize)strTable.length());
	return strTableAll;
}

// 2, table, 3, save path, 4, function param
int LuaAPIUtil::SaveLuaTableToLuaFile(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		int nParamCount = lua_gettop(pLuaState) - 1;
		// 至少两个参数
		ATLASSERT(nParamCount >= 2);
		if(nParamCount < 2)
		{
			return 0;
		}
		size_t nLen = 0;
		const char* szSavePath = (const char*)lua_tolstring(pLuaState, 3, &nLen);
		CComBSTR bstrSavePath;
		LuaStringToCComBSTR(szSavePath,bstrSavePath);


		std::string strSavePath;
		UnicodeToMultiByte(bstrSavePath.m_str, strSavePath);
		

		std::string strParam;
		if(nParamCount >= 3)
		{
			const char* szParam = (const char*)lua_tolstring(pLuaState, 4, &nLen);
			if(szParam != NULL)
			{
				CComBSTR bstrParam;
				LuaStringToCComBSTR(szParam,bstrParam);
				UnicodeToMultiByte(bstrSavePath.m_str, strParam);
			}
		}
		// 第四个参数允许写入 额外的代码
		std::string strExtra;
		if(nParamCount >= 4)
		{
			const char* pszExtra = (const char*)lua_tolstring(pLuaState, 5, &nLen);
			if(pszExtra)
			{
				strExtra = pszExtra;
				// 这里不转，lua传参数的时候要带多一个 '\'
				// ConvertAllEscape(strExtra);
			}
		}
		std::ofstream ofs(strSavePath.c_str(), std::ofstream::binary);

		std::string strFirstLine = "function GetSubTable(";
		strFirstLine += strParam;
		strFirstLine += ")\r\n\tlocal ";
		ofs.write(strFirstLine.c_str(), (std::streamsize)strFirstLine.length());
		std::string strRes = GetTableStr(pLuaState, 2, ofs, "t", 2);
		strRes += "\r\n\treturn t";
		strRes += "\r\nend\r\n";
		strRes += strExtra;
		ofs.write(strRes.c_str(), (std::streamsize)strRes.length());
		ofs.close();
		return 0;
	}
	return 0;
}

int LuaAPIUtil::FindFileList(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* utf8Path = luaL_checkstring(pLuaState, 2);
		const char* utf8Flag = luaL_checkstring(pLuaState, 3);
		if ((utf8Path != NULL) && (utf8Flag != NULL))
		{
			CComBSTR bstrPath, bstrFlag;			
			LuaStringToCComBSTR(utf8Path,bstrPath);
			LuaStringToCComBSTR(utf8Flag,bstrFlag);

			std::vector<std::string> vecFileList;
			vecFileList.clear();
			WIN32_FIND_DATA fd;
			HANDLE hFind = INVALID_HANDLE_VALUE;
			TCHAR szSearchPath[MAX_PATH] = {0};
			::PathCombine(szSearchPath, bstrPath.m_str, bstrFlag.m_str);
			hFind = FindFirstFile(szSearchPath, &fd);
			while (INVALID_HANDLE_VALUE != hFind)
			{
				if (_tcsicmp(fd.cFileName, _T("..")) && _tcsicmp(fd.cFileName, _T(".")) && !(FILE_ATTRIBUTE_DIRECTORY&fd.dwFileAttributes))
				{
					TCHAR szLnkFileTmp[MAX_PATH] = {0};
					::PathCombine(szLnkFileTmp, bstrPath.m_str, fd.cFileName);
					std::string strTmp;
					BSTRToLuaString(szLnkFileTmp,strTmp);
					vecFileList.push_back(strTmp);
				}

				if (FindNextFile(hFind, &fd) == 0)
				{
					break;
				}
			}
			FindClose(hFind);

			int iCount = vecFileList.size();
			lua_checkstack(pLuaState, iCount);
			lua_newtable(pLuaState);
			for (int i = 0; i < iCount; i++)
			{
				std::string strTmp = vecFileList.at(i);
				lua_pushstring(pLuaState, strTmp.c_str());
				lua_rawseti(pLuaState, -2, i + 1);
			}
			return 1;
		}
	}
	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::FindDirList(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* utf8Path = luaL_checkstring(pLuaState, 2);
		if (utf8Path != NULL)
		{			
			CComBSTR bstrPath;			
			LuaStringToCComBSTR(utf8Path,bstrPath);

			std::vector<std::string> vecDirList;
			vecDirList.clear();
			WIN32_FIND_DATA fd;
			HANDLE hFind = INVALID_HANDLE_VALUE;
			TCHAR szSearchPath[MAX_PATH] = {0};
			::PathCombine(szSearchPath, bstrPath.m_str, L"*");
			hFind = FindFirstFile(szSearchPath, &fd);
			while (INVALID_HANDLE_VALUE != hFind)
			{
				if (_tcsicmp(fd.cFileName, _T("..")) && _tcsicmp(fd.cFileName, _T(".")) && (FILE_ATTRIBUTE_DIRECTORY&fd.dwFileAttributes))
				{
					TCHAR szLnkFileTmp[MAX_PATH] = {0};
					::PathCombine(szLnkFileTmp, bstrPath.m_str, fd.cFileName);
					std::string strTmp;
					BSTRToLuaString(szLnkFileTmp,strTmp);
					vecDirList.push_back(strTmp);
				}

				if (FindNextFile(hFind, &fd) == 0)
				{
					break;
				}
			}
			FindClose(hFind);

			int iCount = vecDirList.size();
			lua_checkstack(pLuaState, iCount);
			lua_newtable(pLuaState);
			for (int i = 0; i < iCount; i++)
			{
				std::string strTmp = vecDirList.at(i);
				lua_pushstring(pLuaState, strTmp.c_str());
				lua_rawseti(pLuaState, -2, i + 1);
			}
			return 1;
		}
	}
	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::ExpandEnvironmentString(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* utf8Path = luaL_checkstring(pLuaState, 2);
		if (utf8Path != NULL)
		{
			CComBSTR bstrPath;			
			LuaStringToCComBSTR(utf8Path,bstrPath);
			TCHAR szPathTmp[MAX_PATH] = {0};
			ExpandEnvironmentStrings(bstrPath.m_str, szPathTmp, MAX_PATH);

			std::string strRetPath;
			BSTRToLuaString(szPathTmp,strRetPath);
			lua_pushstring(pLuaState, strRetPath.c_str());
			return 1;
		}
	}
	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::GetTotalTickCount(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	lua_pushnumber(pLuaState, ::GetTickCount());
	return 1;
}

int LuaAPIUtil::CommandLineToList(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	const char* utf8CmdLine = luaL_checkstring(pLuaState, 2);
	if (utf8CmdLine != NULL)
	{
		CComBSTR bstrCmdLine;			
		LuaStringToCComBSTR(utf8CmdLine,bstrCmdLine);
		int nNumArgs = 0;
		LPWSTR *szArgList = CommandLineToArgvW(bstrCmdLine.m_str, &nNumArgs);
		if (NULL != szArgList)
		{
			lua_newtable(pLuaState);
			for (int i=0; i<nNumArgs; ++i)
			{
				std::string strUtf8;
				BSTRToLuaString(szArgList[i],strUtf8);
				lua_pushstring(pLuaState, strUtf8.c_str()); 
				lua_rawseti(pLuaState, -2, i+1);
			}
			GlobalFree(szArgList);
			return 1;
		}
	}
	return 0;
}

int LuaAPIUtil::GetModuleExeName(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		TCHAR szExePath[MAX_PATH] = {0};
		if (0 != GetModuleFileName(NULL, szExePath, MAX_PATH))
		{
			std::string strExePath;
			BSTRToLuaString(szExePath,strExePath);
			lua_pushstring(pLuaState, strExePath.c_str());
			return 1;
		}
	}
	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::GetOSVersionInfo(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		OSVERSIONINFOEX osvi;
		ZeroMemory(&osvi,sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if (!GetVersionEx((OSVERSIONINFO*)&osvi))
		{
			osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
			if (!GetVersionEx((OSVERSIONINFO*)&osvi)) 
			{
				return 0;
			}
		}
		lua_pushnumber(pLuaState, osvi.dwMajorVersion);
		lua_pushnumber(pLuaState, osvi.dwMinorVersion);
		return 2;
	}
	return 0;
}

int LuaAPIUtil::IsWindows8Point1(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		OSVERSIONINFOEX osVersionInfo;
		::ZeroMemory(&osVersionInfo, sizeof(OSVERSIONINFOEX));
		osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		osVersionInfo.dwMajorVersion = 6;
		ULONGLONG maskCondition = ::VerSetConditionMask(0, VER_MAJORVERSION, VER_EQUAL);
		BOOL bMajorVer = ::VerifyVersionInfo(&osVersionInfo, VER_MAJORVERSION, maskCondition);
		osVersionInfo.dwMinorVersion = 3;
		BOOL bMinorVer = ::VerifyVersionInfo(&osVersionInfo, VER_MINORVERSION, maskCondition);
		lua_pushboolean(pLuaState, (int )(bMajorVer && bMinorVer));
		return 1;	

	}
	return 0;
}

int LuaAPIUtil::PathCombine(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	const char* utfPath = luaL_checkstring(pLuaState,2);
	const char* utfFile = luaL_checkstring(pLuaState,3);

	std::string strFilePath;
	if (utfFile != NULL && utfPath != NULL)
	{
		CComBSTR bstrDir,bstrFile;

		LuaStringToCComBSTR(utfPath,bstrDir);
		LuaStringToCComBSTR(utfFile,bstrFile);

		TCHAR szBuffer[MAX_PATH] = {0};
		ZeroMemory(szBuffer, sizeof(szBuffer));
		::PathCombine(szBuffer, bstrDir.m_str, bstrFile.m_str);
		
		std::string strFilePath;
		BSTRToLuaString(szBuffer,strFilePath);
		lua_pushstring(pLuaState, strFilePath.c_str());
		return 1;

	}
	return 0;
}

int LuaAPIUtil::QueryRegValue(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	const char* utf8RootPath = luaL_checkstring(pLuaState,2);
	const char* utf8RegPath = luaL_checkstring(pLuaState,3);
	const char* utf8Key = luaL_checkstring(pLuaState,4);

	if(utf8RegPath == NULL || utf8RootPath == NULL || utf8Key == NULL)
	{
		lua_pushnil(pLuaState);
		return 1;
	}

	std::string result;
	DWORD dwType;
	DWORD dwValue;
	if (0 == QueryRegValueHelper(utf8RootPath,utf8RegPath,utf8Key,dwType, result, dwValue))
	{
		if (dwType == REG_DWORD)
		{
			lua_pushinteger(pLuaState, dwValue);
			return 1;
		}
		else if (dwType == REG_SZ || dwType == REG_EXPAND_SZ)
		{
			lua_pushstring(pLuaState,result.c_str());
			return 1;
		}
	}
	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::QueryRegValue64(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	const char* utf8RootPath = luaL_checkstring(pLuaState,2);
	const char* utf8RegPath = luaL_checkstring(pLuaState,3);
	const char* utf8Key = luaL_checkstring(pLuaState,4);

	if(utf8RegPath == NULL || utf8RootPath == NULL || utf8Key == NULL)
	{
		lua_pushnil(pLuaState);
		return 1;
	}

	std::string result;
	DWORD dwType;
	DWORD dwValue;
	if (0 == QueryRegValueHelper(utf8RootPath,utf8RegPath,utf8Key,dwType, result, dwValue,TRUE))
	{
		if (dwType == REG_DWORD)
		{
			lua_pushinteger(pLuaState, dwValue);
			return 1;
		}
		else if (dwType == REG_SZ || dwType == REG_EXPAND_SZ)
		{
			lua_pushstring(pLuaState,result.c_str());
			return 1;
		}
	}
	lua_pushnil(pLuaState);
	return 1;
}

BOOL LuaAPIUtil::GetHKEY(const char* utf8Root, HKEY &hKey)
{
	BOOL bRet = TRUE;
	if(stricmp(utf8Root,"HKEY_CURRENT_USER") == 0)
	{
		hKey = HKEY_CURRENT_USER;
	}
	else if(stricmp(utf8Root,"HKEY_CLASSES_ROOT") == 0)
	{
		hKey = HKEY_CLASSES_ROOT;
	}
	else if(stricmp(utf8Root,"HKEY_LOCAL_MACHINE") == 0)
	{
		hKey = HKEY_LOCAL_MACHINE;
	}
	else if(stricmp(utf8Root,"HKEY_USERS") == 0)
	{
		hKey = HKEY_USERS;
	}
	else if(stricmp(utf8Root, "HKEY_CURRENT_CONFIG") == 0)
	{
		hKey = HKEY_CURRENT_CONFIG;
	}
	else
	{
		bRet = FALSE;
		hKey = (HKEY)(ULONG_PTR)((LONG)-1);
	}
	return bRet;
}

long LuaAPIUtil::QueryRegValueHelper(const char* utf8Root,const char* utf8RegPath,const char* utf8Key, DWORD &dwType, std::string& utf8Result, DWORD &dwValue,BOOL bWow64)
{
	//TODO
	HKEY root;
	if(utf8Root == NULL || utf8RegPath == NULL || utf8Key == NULL)
	{
		return 1;
	}
	if (!GetHKEY(utf8Root, root))
	{
		return 1;
	}

	CComBSTR bstrRegPath,bstrKey;

	LuaStringToCComBSTR(utf8RegPath,bstrRegPath);
	LuaStringToCComBSTR(utf8Key,bstrKey);

	CRegKey regKey;
	REGSAM samDesired = bWow64?(KEY_WOW64_64KEY|KEY_READ):KEY_READ;
	if (regKey.Open(root, bstrRegPath.m_str, samDesired) == ERROR_SUCCESS)
	{
		ULONG ulBytes = 0;
		if (ERROR_SUCCESS == regKey.QueryValue(bstrKey.m_str, &dwType, NULL, &ulBytes))
		{
			if (dwType == REG_DWORD)
			{
				regKey.QueryDWORDValue(bstrKey.m_str, dwValue);
				return 0;
			}
			else if (dwType == REG_SZ || dwType == REG_EXPAND_SZ)
			{
				wchar_t* pBuffer = (wchar_t *)new BYTE[ulBytes+2];
				memset(pBuffer, 0, ulBytes+2);
				regKey.QueryStringValue(bstrKey.m_str, pBuffer, &ulBytes);

				BSTRToLuaString(pBuffer,utf8Result);
				TSDEBUG4CXX(L"REG_EXPAND_SZ = " << utf8Result.c_str());
				delete [] pBuffer;
				pBuffer = NULL;
				return 0;
			}
		}
	}
	return 1;
}


int LuaAPIUtil::DeleteRegValue(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil)
	{
		const char* utf8Root = luaL_checkstring(pLuaState, 2);
		const char* utf8Key = luaL_checkstring(pLuaState, 3);

		if(utf8Root == NULL || utf8Key == NULL)
		{
			lua_pushboolean(pLuaState, 0);
			return 1;
		}

		if(DeleteRegValueHelper(utf8Root, utf8Key) == 1)
		{
			lua_pushboolean(pLuaState, 1);
			return 1;
		}
	}
	lua_pushboolean(pLuaState, 0);
	return 1;
}

int LuaAPIUtil::DeleteRegValue64(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil)
	{
		const char* utf8Root = luaL_checkstring(pLuaState, 2);
		const char* utf8Key = luaL_checkstring(pLuaState, 3);

		if(utf8Root == NULL || utf8Key == NULL)
		{
			lua_pushboolean(pLuaState, 0);
			return 1;
		}

		if(DeleteRegValueHelper(utf8Root, utf8Key,TRUE) == 1)
		{
			lua_pushboolean(pLuaState, 1);
			return 1;
		}
	}
	lua_pushboolean(pLuaState, 0);
	return 1;
}

long LuaAPIUtil::DeleteRegValueHelper(const char* utf8Root, const char* utf8Key,BOOL bWow64)
{
	HKEY root;
	if(utf8Root == NULL || utf8Key == NULL)
	{
		return 0;
	}
	if (!GetHKEY(utf8Root, root))
	{
		return 0;
	}

	CComBSTR bstrKey;
	LuaStringToCComBSTR(utf8Key,bstrKey);
	
	std::wstring wstrKey = bstrKey.m_str;
	std::wstring::size_type index = wstrKey.find_last_of(L"\\");
	if (index == std::wstring::npos)
	{
		return 0;
	}
	std::wstring  strsubkey,strvalue;
	strsubkey = wstrKey.substr(0,index);
	strvalue =  wstrKey.substr(index+1);
	
	REGSAM samDesired = bWow64?(KEY_WOW64_64KEY|KEY_WRITE):KEY_WRITE;
	HKEY hKey;
	if(RegOpenKeyEx(root,strsubkey.c_str(),0,samDesired,&hKey) == ERROR_SUCCESS)
	{
		RegDeleteValue(hKey, strvalue.c_str());
		RegCloseKey( hKey );
	}
	else
	{
		return 0;
	}
	return 1;
}

int LuaAPIUtil::DeleteRegKey(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil)
	{
		const char* utf8Root = luaL_checkstring(pLuaState, 2);
		const char* utf8Key = luaL_checkstring(pLuaState, 3);

		if(utf8Root == NULL || utf8Key == NULL)
		{
			lua_pushboolean(pLuaState, 0);
			return 1;
		}

		if(DeleteRegKeyHelper(utf8Root, utf8Key) == 1)
		{
			lua_pushboolean(pLuaState, 1);
			return 1;
		}
	}
	lua_pushboolean(pLuaState, 0);
	return 1;
}

int LuaAPIUtil::DeleteRegKey64(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil)
	{
		const char* utf8Root = luaL_checkstring(pLuaState, 2);
		const char* utf8Key = luaL_checkstring(pLuaState, 3);

		if(utf8Root == NULL || utf8Key == NULL)
		{
			lua_pushboolean(pLuaState, 0);
			return 1;
		}

		if(DeleteRegKeyHelper(utf8Root, utf8Key,TRUE) == 1)
		{
			lua_pushboolean(pLuaState, 1);
			return 1;
		}
	}
	lua_pushboolean(pLuaState, 0);
	return 1;
}

typedef LONG (WINAPI*_RegDeleteTree)(HKEY hKey, LPCWSTR lpSubKey);
typedef LONG (WINAPI*_RegDeleteKeyEx)(HKEY hKey, LPCWSTR lpSubKey,REGSAM samDesired, DWORD);

long LuaAPIUtil::DeleteRegKeyHelper(const char* utf8Root, const char* utf8SubKey,BOOL bWow64)
{
	HKEY root;
	if(utf8Root == NULL || utf8SubKey == NULL)
	{
		return 0;
	}
	if (!GetHKEY(utf8Root, root))
	{
		return 0;
	}
	CComBSTR bstrKey;
	LuaStringToCComBSTR(utf8SubKey,bstrKey);
	if (!bWow64)
	{
		if (ERROR_SUCCESS == SHDeleteKey(root, bstrKey.m_str))
		{
			return 1;
		}
	}
	else
	{
		HMODULE hModule = ::LoadLibrary(_T("Advapi32.dll"));
		if (NULL == hModule)
		{
			return 0;
		}
		_RegDeleteTree fnRegDeleteTree = (_RegDeleteTree)GetProcAddress(hModule, "RegDeleteTreeW");
		if (NULL == fnRegDeleteTree)
		{
			FreeLibrary(hModule);
			return 0;
		}
		HKEY hKey;
		if(RegOpenKeyEx(root,bstrKey.m_str,0, DELETE|KEY_ENUMERATE_SUB_KEYS| KEY_QUERY_VALUE|KEY_SET_VALUE|KEY_WOW64_64KEY,&hKey) == ERROR_SUCCESS)
		{
			LONG lRet = fnRegDeleteTree(hKey,NULL);
			RegCloseKey( hKey );
			if (ERROR_SUCCESS == lRet)
			{
				_RegDeleteKeyEx fnRegDeleteKeyEx = (_RegDeleteKeyEx)GetProcAddress(hModule, "RegDeleteKeyExW");
				if (NULL != fnRegDeleteKeyEx)
				{
					if (ERROR_SUCCESS == fnRegDeleteKeyEx(root,bstrKey.m_str,KEY_WOW64_64KEY,NULL))
					{
						FreeLibrary(hModule);
						return 1;
					}
				}
			}
		}
		FreeLibrary(hModule);
	}
	return 0;
}

int LuaAPIUtil::CreateRegKey(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil)
	{
		const char* utf8Root = luaL_checkstring(pLuaState, 2);
		const char* utf8SubKey = luaL_checkstring(pLuaState, 3);

		if(utf8Root == NULL || utf8SubKey == NULL)
		{
			lua_pushboolean(pLuaState, 0);
			return 1;
		}

		if(CreateRegKeyHelper(utf8Root, utf8SubKey) == 1)
		{
			lua_pushboolean(pLuaState, 1);
			return 1;
		}
	}

	lua_pushboolean(pLuaState, 0);
	return 1;
}

int LuaAPIUtil::CreateRegKey64(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil)
	{
		const char* utf8Root = luaL_checkstring(pLuaState, 2);
		const char* utf8SubKey = luaL_checkstring(pLuaState, 3);

		if(utf8Root == NULL || utf8SubKey == NULL)
		{
			lua_pushboolean(pLuaState, 0);
			return 1;
		}

		if(CreateRegKeyHelper(utf8Root, utf8SubKey,TRUE) == 1)
		{
			lua_pushboolean(pLuaState, 1);
			return 1;
		}
	}

	lua_pushboolean(pLuaState, 0);
	return 1;
}

long LuaAPIUtil::CreateRegKeyHelper(const char* utf8Root, const char* utf8SubKey,BOOL bWow64)
{
	HKEY root;
	if(utf8Root == NULL || utf8SubKey == NULL)
	{
		return 0;
	}
	if (!GetHKEY(utf8Root, root))
	{
		return 0;
	}
	
	CComBSTR bstrKey;
	LuaStringToCComBSTR(utf8SubKey,bstrKey);
	HKEY hCreateKey;
	LONG lRet;
	if (!bWow64)
	{
		lRet = RegCreateKey(root, bstrKey.m_str, &hCreateKey);
	}
	else
	{
		lRet = RegCreateKeyEx(root, bstrKey.m_str, NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_WOW64_64KEY|KEY_READ|KEY_WRITE,NULL,&hCreateKey,NULL);
	}
	if(lRet == ERROR_SUCCESS)
	{
		RegCloseKey(hCreateKey);
		return 1;
	}

	return 0;
}

int LuaAPIUtil::SetRegValue(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil)
	{
		const char* utf8Root = luaL_checkstring(pLuaState, 2);
		const char* utf8SubKey = luaL_checkstring(pLuaState, 3);
		const char* utf8ValueName = luaL_checkstring(pLuaState, 4);

		if(utf8Root == NULL || utf8SubKey == NULL || utf8ValueName == NULL)
		{
			lua_pushboolean(pLuaState, 0);
			return 1;
		}

		int type = lua_type(pLuaState, 5);
		if (type == LUA_TNUMBER)
		{
			DWORD dwValue = (DWORD)luaL_checkinteger(pLuaState, 5);
			if (SetRegValueHelper(utf8Root, utf8SubKey, utf8ValueName, REG_DWORD, NULL, dwValue) == 0)
			{
				lua_pushboolean(pLuaState, 1);
				return 1;
			}
		}
		else if (type == LUA_TSTRING)
		{
			const char* utf8Data = luaL_checkstring(pLuaState, 5);
			if (SetRegValueHelper(utf8Root, utf8SubKey, utf8ValueName, REG_SZ, utf8Data) == 0)
			{
				lua_pushboolean(pLuaState, 1);
				return 1;
			}
		}
		else
		{
			lua_pushboolean(pLuaState, 0);
			return 1;
		}
	}

	lua_pushboolean(pLuaState, 0);
	return 1;
}

int LuaAPIUtil::SetRegValue64(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil)
	{
		const char* utf8Root = luaL_checkstring(pLuaState, 2);
		const char* utf8SubKey = luaL_checkstring(pLuaState, 3);
		const char* utf8ValueName = luaL_checkstring(pLuaState, 4);

		if(utf8Root == NULL || utf8SubKey == NULL || utf8ValueName == NULL)
		{
			lua_pushboolean(pLuaState, 0);
			return 1;
		}

		int type = lua_type(pLuaState, 5);
		if (type == LUA_TNUMBER)
		{
			DWORD dwValue = (DWORD)luaL_checkinteger(pLuaState, 5);
			if (SetRegValueHelper(utf8Root, utf8SubKey, utf8ValueName, REG_DWORD, NULL, dwValue,TRUE) == 0)
			{
				lua_pushboolean(pLuaState, 1);
				return 1;
			}
		}
		else if (type == LUA_TSTRING)
		{
			const char* utf8Data = luaL_checkstring(pLuaState, 5);
			if (SetRegValueHelper(utf8Root, utf8SubKey, utf8ValueName, REG_SZ, utf8Data,0,TRUE) == 0)
			{
				lua_pushboolean(pLuaState, 1);
				return 1;
			}
		}
		else
		{
			lua_pushboolean(pLuaState, 0);
			return 1;
		}
	}

	lua_pushboolean(pLuaState, 0);
	return 1;
}

long LuaAPIUtil::SetRegValueHelper(const char* utf8Root, const char* utf8SubKey, const char* utf8ValueName,DWORD dwType, const char* utf8Data, DWORD dwValue,BOOL bWow64)
{
	HKEY root;
	if(utf8Root == NULL || utf8SubKey == NULL || utf8ValueName == NULL)
	{
		return 1;
	}
	if (!GetHKEY(utf8Root, root))
	{
		return 1;
	}

	CComBSTR bstrSubKey,bstrValueName;

	LuaStringToCComBSTR(utf8SubKey,bstrSubKey);
	LuaStringToCComBSTR(utf8ValueName,bstrValueName);

	CRegKey regKey;

	REGSAM samDesired = bWow64?(KEY_WOW64_64KEY|KEY_SET_VALUE):KEY_SET_VALUE;
	if (regKey.Open(root, bstrSubKey.m_str, samDesired) == ERROR_SUCCESS)
	{
		// 判断类型
		if (dwType == REG_DWORD)
		{
			if (ERROR_SUCCESS == regKey.SetDWORDValue(bstrValueName.m_str, dwValue))
			{
				return 0;
			}
		}
		else if (dwType == REG_SZ)
		{
			CComBSTR bstrData;
			LuaStringToCComBSTR(utf8Data,bstrData);
			if (ERROR_SUCCESS == regKey.SetStringValue(bstrValueName.m_str, bstrData.m_str))
			{
				return 0;
			}
		}
	}

	return 1;
}

int LuaAPIUtil::QueryRegKeyExists(lua_State* pLuaState)
{
	TSTRACEAUTO();
	BOOL bRet = FALSE;
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* utf8RootPath = luaL_checkstring(pLuaState,2);
		const char* utf8RegPath = luaL_checkstring(pLuaState,3);
		if (utf8RegPath != NULL || utf8RootPath != NULL)
		{
			HKEY root;
			if (GetHKEY(utf8RootPath, root))
			{
				CComBSTR bstrRegPath;
				LuaStringToCComBSTR(utf8RegPath,bstrRegPath);
				CRegKey regKey;
				if (regKey.Open(root, bstrRegPath.m_str, KEY_READ) == ERROR_SUCCESS)
				{
					bRet = TRUE;
				}
			}
		}
	}
	lua_pushboolean(pLuaState, bRet);
	return 1;
}

int LuaAPIUtil::EnumRegLeftSubKey(lua_State* pLuaState)
{
	TSTRACEAUTO();
	BOOL bRet = FALSE;
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* utf8RootPath = luaL_checkstring(pLuaState,2);
		const char* utf8RegPath = luaL_checkstring(pLuaState,3);
		if (utf8RegPath != NULL || utf8RootPath != NULL)
		{
			HKEY root;
			if (GetHKEY(utf8RootPath, root))
			{
				CComBSTR bstrRegPath;
				LuaStringToCComBSTR(utf8RegPath,bstrRegPath);
				CRegKey regKey;
				if (regKey.Open(root, bstrRegPath.m_str, KEY_READ) == ERROR_SUCCESS)
				{
					std::vector<std::string> vecStrKeys;
					LONG retCode = ERROR_SUCCESS;
					for (int i = 0; retCode == ERROR_SUCCESS; i++) 
					{
						DWORD dwLen = MAX_PATH;
						TCHAR achKey[MAX_PATH] = {0};
						retCode = regKey.EnumKey(i, achKey, &dwLen);
						if (retCode == ERROR_SUCCESS) 
						{
							std::string strKeyTmp;
							BSTRToLuaString(achKey,strKeyTmp);
							vecStrKeys.push_back(strKeyTmp);
						}
					}
					int nCount = vecStrKeys.size();
					lua_checkstack(pLuaState, nCount);
					lua_newtable(pLuaState);
					for(int i = 0; i < nCount;i++)
					{
						std::string strTemp = vecStrKeys.at(i);
						lua_pushstring(pLuaState,strTemp.c_str()); 
						lua_rawseti(pLuaState,-2,i+1); 
					}
					return 1;
				}
			}
		}
	}
	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::EnumRegRightSubKey(lua_State* pLuaState)
{
	TSTRACEAUTO();
	BOOL bRet = FALSE;
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* utf8RootPath = luaL_checkstring(pLuaState,2);
		const char* utf8RegPath = luaL_checkstring(pLuaState,3);
		if (utf8RegPath != NULL || utf8RootPath != NULL)
		{
			HKEY root;
			if (GetHKEY(utf8RootPath, root))
			{
				CComBSTR bstrRegPath;
				LuaStringToCComBSTR(utf8RegPath,bstrRegPath);
				
				HKEY hKey;
				if(RegOpenKeyEx(root,bstrRegPath.m_str,0,KEY_READ,&hKey) == ERROR_SUCCESS)
				{
					std::vector<std::string> vecStrKeys;
					LONG retCode = ERROR_SUCCESS;
					for (int j = 0; retCode == ERROR_SUCCESS; j++) 
					{
						TCHAR achValue[MAX_PATH] = {0};
						DWORD dwLen = MAX_PATH;
						retCode = RegEnumValue(hKey, j, achValue, &dwLen, NULL, NULL, NULL, NULL);
						if (retCode == ERROR_SUCCESS)
						{
							std::string strKeyTmp;
							BSTRToLuaString(achValue,strKeyTmp);
							vecStrKeys.push_back(strKeyTmp);
						}
					}
					int nCount = vecStrKeys.size();
					lua_checkstack(pLuaState, nCount);
					lua_newtable(pLuaState);
					for(int i = 0; i < nCount;i++)
					{
						std::string strTemp = vecStrKeys.at(i);
						lua_pushstring(pLuaState,strTemp.c_str()); 
						lua_rawseti(pLuaState,-2,i+1); 
					}
					RegCloseKey(hKey);
					return 1;
				}
			}
		}
	}
	lua_pushnil(pLuaState);
	return 1;
}


long LuaAPIUtil::OpenURLHelper(const char* utf8URL)
{
	//TODO
	CComBSTR bstrURL;
	if(utf8URL)
	{
		LuaStringToCComBSTR(utf8URL,bstrURL);
	}

	TCHAR explorer[1024] = {0};
	HKEY hKey;
	//vista和win7下的默认浏览器应该在这个目录下查找
	if ( RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\http\\UserChoice"), 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS )
	{
		TCHAR szPath[1024] = {0};
		DWORD dCount=1024;
		if(RegQueryValueEx( hKey, _T("Progid"),NULL,NULL,(BYTE*)szPath, &dCount) == ERROR_SUCCESS )
		{
			RegCloseKey( hKey);
			::PathCombine(szPath, szPath, _T("shell\\open\\command"));

			HKEY hVlaueKey;
			//需要再次到HKEY_CLASSES_ROOT对应的目录查找程序的路径
			if (RegOpenKeyEx(HKEY_CLASSES_ROOT, szPath, 0, KEY_QUERY_VALUE, &hVlaueKey) == ERROR_SUCCESS)
			{
				DWORD dCount=1024;
				if(RegQueryValueEx(hVlaueKey, _T(""),NULL,NULL,(BYTE*)explorer, &dCount) == ERROR_SUCCESS )
				{
					RegCloseKey(hVlaueKey);
				}
			}
		}
	}
	//XP系统下则只在HKEY_CLASSES_ROOT\\http\\shell\\open\\command中标示当前默认浏览器
	else if ( RegOpenKeyEx(HKEY_CLASSES_ROOT, _T("http\\shell\\open\\command"), 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS )
	{
		DWORD dCount=1024;
		if(RegQueryValueEx(hKey, _T(""),NULL,NULL,(BYTE*)explorer, &dCount) == ERROR_SUCCESS )
		{
			RegCloseKey(hKey);
		}
	}

	std::wstring  exp( explorer);
	std::wstring strParam;
	std::wstring::size_type pos = exp.find('"');
	if ( std::wstring::npos != pos )
	{
		exp = exp.substr( pos+1 );
	}
	pos = exp.find('"');      
	if( std::wstring::npos != pos)
	{  
		strParam = exp.substr(pos + 1);
		exp = exp.substr( 0, pos );
	}

	pos = strParam.find( L"%1" );
	if ( std::wstring::npos != pos )
	{
		strParam.replace( pos, 2, bstrURL.m_str);
	}
	else
	{
		strParam = bstrURL.m_str;
	}

	//对得到的程序路径值进行检查，判断该文件是否存在
	//有可能有些用户注册表对应的值为空的情形，文件路径不存在则使用IE打开链接
	if (PathFileExists(exp.c_str()))
	{
		if (ShellExecute(NULL,_T("open"), exp.c_str(), strParam.c_str(),NULL,SW_SHOWNORMAL) > (HINSTANCE) 32)//浏览成功?
		{
			return 0;
		}
	}

	std::wstring strTemp = L"\"";
	strTemp += bstrURL.m_str;
	strTemp += L"\"";

	//用IE 来尝试
	memset(explorer,0,sizeof(explorer));
	if (::SHGetSpecialFolderPath(::GetDesktopWindow(),explorer,CSIDL_PROGRAM_FILES,FALSE))
	{
		wcsncat(explorer,_T("\\Internet Explorer\\iexplore.exe"),64);
		if (ShellExecute(NULL,_T("open"), explorer, strTemp.c_str(),NULL,SW_SHOWNORMAL) > (HINSTANCE) 32)//浏览成功?	
		{
			return 1;
		}
	}
	return 0;
}

int LuaAPIUtil::OpenURL(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	const char* utf8URL = luaL_checkstring(pLuaState, 2);
	if (utf8URL == NULL)
		return 0;

	OpenURLHelper(utf8URL);
	return 0;
}

int LuaAPIUtil::OpenURLIE(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	const char* utf8URL = luaL_checkstring(pLuaState, 2);
	if (utf8URL == NULL)
		return 0;

	CComBSTR bstrURL;
	LuaStringToCComBSTR(utf8URL,bstrURL);

	TCHAR explorer[1024]= {0};
	std::wstring strTemp = L"\"";
	strTemp += bstrURL.m_str;
	strTemp += L"\"";
	if (::SHGetSpecialFolderPath(::GetDesktopWindow(),explorer,CSIDL_PROGRAM_FILES,FALSE))
	{
		wcsncat(explorer,_T("\\Internet Explorer\\iexplore.exe"),64);
		if (ShellExecute(NULL,_T("open"), explorer, strTemp.c_str(),NULL,SW_SHOWNORMAL) > (HINSTANCE) 32)//浏览成功?	
		{
			return 0;
		}
	}
	return 0;
}

BOOL LuaAPIUtil::IsFullScreenHelper()
{
	struct LocalUtil
	{
		// 取得窗口句柄所属进程ID
		static inline DWORD ProcessIdFromWindow(HWND hWnd)
		{
			DWORD dwPID = 0;
			if (hWnd)
			{
				::GetWindowThreadProcessId(hWnd, &dwPID);
			}

			return dwPID;
		}

		// 取得屏幕坐标所在用户界面进程ID
		static inline DWORD ProcessIdFromPoint(const POINT& pt)
		{
			return ProcessIdFromWindow(::WindowFromPoint(pt));
		}
	};

	// 取得系统Explorer进程ID
	// 和遍历进程快照相比，如果系统不幸存在多个Explorer进程，该方法总是取得Shell进程
	DWORD dwExplorerPID = LocalUtil::ProcessIdFromWindow(::GetShellWindow());

	// 判断当前用户前台进程是否为Explorer，如果是则直接返回
	if ((dwExplorerPID > 0) && (dwExplorerPID == LocalUtil::ProcessIdFromWindow(::GetForegroundWindow())))
	{
		return FALSE;
	}

	// 取得屏幕四个角所在窗口的进程ID
	RECT rcFullScreen = {0, 0, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN)};
	if (!::IsRectEmpty(&rcFullScreen) && ::InflateRect(&rcFullScreen, -1, -1))
	{
		POINT ptLeftTop = {rcFullScreen.left, rcFullScreen.top};
		POINT ptLeftBottom = {rcFullScreen.left, rcFullScreen.bottom};
		POINT ptRightTop = {rcFullScreen.right, rcFullScreen.top};
		POINT ptRightBottom = {rcFullScreen.right, rcFullScreen.bottom};
		DWORD dwLeftTopPID = LocalUtil::ProcessIdFromPoint(ptLeftTop);
		DWORD dwLeftBottomPID = LocalUtil::ProcessIdFromPoint(ptLeftBottom);
		DWORD dwRightTopPID = LocalUtil::ProcessIdFromPoint(ptRightTop);
		DWORD dwRightBottomPID = LocalUtil::ProcessIdFromPoint(ptRightBottom);
		if ((dwLeftTopPID > 0) && (dwLeftTopPID != dwExplorerPID) && /*过滤掉Shell进程*/
			(dwLeftTopPID == dwLeftBottomPID) && (dwLeftBottomPID == dwRightTopPID) && (dwRightTopPID == dwRightBottomPID))
		{
			return TRUE;
		}
	}

	return FALSE;
	// add end
}

int LuaAPIUtil::IsNowFullScreen(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	int iValue = 0;
	if (IsFullScreenHelper())
	{
		iValue = 1;
	}

	lua_pushboolean(pLuaState, iValue);
	return 1;
}

int LuaAPIUtil::FGetForegroundWindow(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	HWND hWnd = ::GetForegroundWindow();
	if (NULL == hWnd)
	{
		lua_pushinteger(pLuaState, (lua_Integer)hWnd);
	}
	else
	{
		lua_pushnil(pLuaState);
	}
	return 1;

}

int LuaAPIUtil::FSetForegroundWindow(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	HWND hWnd = (HWND) lua_touserdata(pLuaState, 2);
	
	DWORD dwTimeout = -1;  
	SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)&dwTimeout, 0);  
	if (dwTimeout >= 100) {  
		SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)0, SPIF_SENDCHANGE | SPIF_UPDATEINIFILE);  
	}
	
	BOOL bSuc = FALSE;
	HWND hForeWnd =  GetForegroundWindow(); 
	DWORD dwForeID =   GetWindowThreadProcessId( hForeWnd, NULL ); 
	DWORD dwCurID = GetCurrentThreadId(); 

	AttachThreadInput( dwCurID, dwForeID, TRUE); 
	ShowWindow( hWnd, SW_SHOWNORMAL ); 
	SetWindowPos( hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE ); 
	SetWindowPos( hWnd, HWND_NOTOPMOST, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE ); 
	bSuc = SetForegroundWindow( hWnd ); 
	AttachThreadInput( dwCurID, dwForeID, FALSE);

	lua_pushboolean(pLuaState, bSuc);
	return 1;
}

int LuaAPIUtil::GetCursorWndHandle(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	POINT p = {0};
	::GetCursorPos(&p);
	HWND hWnd = ::WindowFromPoint(p);
	lua_pushinteger(pLuaState, (lua_Integer)hWnd);
	return 1;
}

int LuaAPIUtil::GetFocusWnd(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	HWND hWndFocus = ::GetFocus();
	lua_pushinteger(pLuaState, (int)hWndFocus);
	return 1;
}

int LuaAPIUtil::FGetKeyState(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	LONG nVirtKey = (LONG)lua_tointeger(pLuaState,3);
	LONG state =  (LONG)::GetKeyState(nVirtKey);
	lua_pushinteger(pLuaState,state);
	return 1;
}

long LuaAPIUtil::ShellExecHelper(HWND hWnd, const char* lpOperation, const char* lpFile, const char* lpParameters, const char* lpDirectory, const char* lpShowCmd, int iShowCmd)
{
	CComBSTR bstrOperation;
	CComBSTR bstrFile;
	CComBSTR bstrParameters;
	CComBSTR bstrDir;
	CComBSTR bstrCmd;

	if ( lpOperation )
	{
		LuaStringToCComBSTR(lpOperation,bstrOperation);
	}
	if ( lpFile )
	{
		LuaStringToCComBSTR(lpFile,bstrFile);
	}
	if ( lpParameters )
	{
		LuaStringToCComBSTR(lpParameters,bstrParameters);
	}
	if ( lpDirectory )
	{
		LuaStringToCComBSTR(lpDirectory,bstrDir);
	}
	if ( lpShowCmd )
	{
		LuaStringToCComBSTR(lpShowCmd,bstrCmd);
	}
	std::wstring strCmd = bstrCmd.m_str;
	INT nShowCmd = 0;
	if (iShowCmd != -1)
	{
		nShowCmd = iShowCmd;
	}
	else if ( strCmd == L"SW_HIDE" )
	{
		nShowCmd = SW_HIDE;
	}
	else if ( strCmd == L"SW_MAXIMIZE" )
	{
		nShowCmd = SW_MAXIMIZE;
	}
	else if ( strCmd == L"SW_MINIMIZE" )
	{
		nShowCmd = SW_MINIMIZE;
	}
	else if ( strCmd == L"SW_RESTORE" )
	{
		nShowCmd = SW_RESTORE;
	}
	else if ( strCmd == L"SW_SHOW" )
	{
		nShowCmd = SW_SHOW;
	}
	else if ( strCmd == L"SW_SHOWDEFAULT" )
	{
		nShowCmd = SW_SHOWDEFAULT;
	}
	else if ( strCmd == L"SW_SHOWMAXIMIZED" )
	{
		nShowCmd = SW_SHOWMAXIMIZED;
	}
	else if ( strCmd == L"SW_SHOWMINIMIZED" )
	{
		nShowCmd = SW_SHOWMINIMIZED;
	}
	else if ( strCmd == L"SW_SHOWMINNOACTIVE" )
	{
		nShowCmd = SW_SHOWMINNOACTIVE;
	}
	else if ( strCmd == L"SW_SHOWNA" )
	{
		nShowCmd = SW_SHOWNA;
	}
	else if ( strCmd == L"SW_SHOWNOACTIVATE" )
	{
		nShowCmd = SW_SHOWNOACTIVATE;
	}
	else if ( strCmd == L"SW_SHOWNORMAL" )
	{
		nShowCmd = SW_SHOWNORMAL;
	}

	if ( (HINSTANCE) 32 < ::ShellExecute( hWnd, bstrOperation.m_str, bstrFile.m_str, bstrParameters.m_str, bstrDir.m_str, nShowCmd ) )
	{
		return 0;
	}
	return 1;
}

int LuaAPIUtil::ShellExecuteEX(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	//HWND hWnd = (HWND)lua_tointeger(pLuaState,2);
	HWND hWnd = (HWND)lua_touserdata(pLuaState, 2);
	const char* lpOperation = luaL_checkstring(pLuaState, 3);
	const char* lpFile = luaL_checkstring(pLuaState, 4);
	const char* lpParameters = luaL_checkstring(pLuaState, 5);
	const char* lpDir = luaL_checkstring(pLuaState, 6);
	int luaType = lua_type(pLuaState, 7);
	if (luaType == LUA_TSTRING)
	{
		const char* lpCmd = luaL_checkstring(pLuaState, 7);
		lua_pushinteger(pLuaState, ShellExecHelper(hWnd, lpOperation, lpFile, lpParameters, lpDir, lpCmd, -1));
	}
	else if (luaType == LUA_TNUMBER)
	{
		int iCmd = (int)luaL_checkinteger(pLuaState, 7);
		lua_pushinteger(pLuaState, ShellExecHelper(hWnd, lpOperation, lpFile, lpParameters, lpDir, NULL, iCmd));
	}
	
	return 1;
}

int LuaAPIUtil::QueryProcessExists(lua_State* pLuaState)
{
	int iValue = 0;
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil)
	{
		const char* utf8ProcessName = luaL_checkstring(pLuaState, 2);		
		if(utf8ProcessName != NULL)
		{
			CComBSTR bstrProcessName;
			LuaStringToCComBSTR(utf8ProcessName,bstrProcessName);
			HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (hSnap != INVALID_HANDLE_VALUE)
			{
				PROCESSENTRY32 pe;
				pe.dwSize = sizeof(PROCESSENTRY32);
				BOOL bResult = ::Process32First(hSnap, &pe);
				while (bResult)
				{
					if(_tcsicmp(pe.szExeFile, bstrProcessName.m_str) == 0)
					{
						iValue = 1;
						break;
					}
					bResult = ::Process32Next(hSnap, &pe);
				}
				::CloseHandle(hSnap);
			}
		}
	}
	lua_pushboolean(pLuaState, iValue);
	return 1;
}

long LuaAPIUtil::QueryFileExistsHelper(const char*utf8FilePath)
{
	CComBSTR bstrFilePath;
	if(utf8FilePath)
	{
		LuaStringToCComBSTR(utf8FilePath,bstrFilePath);
	}

	if(PathFileExists(bstrFilePath.m_str))
	{
		return 1;
	}

	return 0;
}

int LuaAPIUtil::QueryFileExists(lua_State* pLuaState)
{
	int iValue = 0;
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil)
	{
		const char* utf8FilePath = luaL_checkstring(pLuaState, 2);		
		if(utf8FilePath != NULL)
		{
			if (QueryFileExistsHelper(utf8FilePath) == 1)
			{
				iValue = 1;
			}
		}
	}
	lua_pushboolean(pLuaState, iValue);
	return 1;
}

int LuaAPIUtil::Rename(lua_State* pLuaState)
{
	int iValue = 0;
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil)
	{
		const char* utf8OldName = luaL_checkstring(pLuaState, 2);
		if(utf8OldName == NULL)
		{
			lua_pushboolean(pLuaState, 0);
			return 1;
		}

		const char* utf8NewName = luaL_checkstring(pLuaState, 3);
		if(utf8NewName == NULL)
		{
			lua_pushboolean(pLuaState, 0);
			return 1;
		}

		CComBSTR bstrOldName, bstrNewName;
		LuaStringToCComBSTR(utf8OldName,bstrOldName);
		LuaStringToCComBSTR(utf8NewName,bstrNewName);

		if (0 == _wrename(bstrOldName.m_str, bstrNewName.m_str))
		{
			lua_pushboolean(pLuaState, 1);
			return 1;
		}
	}
	lua_pushboolean(pLuaState, iValue);
	return 1;
}

int LuaAPIUtil::CreateDir(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	const char* utf8DirPath = luaL_checkstring(pLuaState, 2);
	int iValue = 0;
	if(utf8DirPath != NULL)
	{
		CComBSTR bstrDirPath;
		LuaStringToCComBSTR(utf8DirPath,bstrDirPath);

		if (ERROR_SUCCESS == SHCreateDirectoryEx(NULL, bstrDirPath.m_str, NULL))
		{
			iValue = 1;
		}
	}	
	lua_pushboolean(pLuaState, iValue);
	return 1;
}

int LuaAPIUtil::CreatePathFile(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	const char* utf8FilePath = luaL_checkstring(pLuaState, 2);
	int iValue = 0;
	if(utf8FilePath != NULL)
	{
		CComBSTR bstrFilePath;
		LuaStringToCComBSTR(utf8FilePath, bstrFilePath);

		HANDLE hFile = CreateFile(bstrFilePath.m_str, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE != hFile)
		{
			iValue = 1;
		}
		CloseHandle(hFile);
	}	
	lua_pushboolean(pLuaState, iValue);
	return 1;
}
int LuaAPIUtil::DragQueryFile(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	LONG lhDrop = (LONG)lua_tonumber(pLuaState, 2);
	HDROP hDrop = (HDROP)(DWORD_PTR)lhDrop;

	TCHAR szFileName[MAX_PATH + 1] = {0};
	UINT nFiles = ::DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0); 
	
	TSDEBUG4CXX("DragQueryFile returns: "<< nFiles<< ", Error Code:"<<::GetLastError());

	UINT uiLen = ::DragQueryFile(hDrop, 0, szFileName, MAX_PATH); //只取拖拽文件当中的第一个
	std::string strFileName;
	BSTRToLuaString(szFileName,strFileName);
	lua_pushstring(pLuaState, strFileName.c_str());

	return 1;
}
int LuaAPIUtil::DragAcceptFiles(lua_State* pLuaState)
{
	TSAUTO();
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	HWND hWnd = (HWND)lua_touserdata(pLuaState, 2);
	
	BOOL vbAccept = lua_toboolean(pLuaState, 3);
	BOOL bAccept = vbAccept ? TRUE : FALSE;

	BOOL isWindow = ::IsWindow(hWnd);
	ATLASSERT(isWindow);

	if (isWindow)
	{
		::DragAcceptFiles(hWnd,bAccept);
	}
	return 0;
}
long LuaAPIUtil::CopyPathFileHelper(const char* utf8ExistingFileName, const char* utf8NewFileName, BOOL bFailedIfExists)
{
	CComBSTR bstrExistingFileName,bstrNewFileName;
	
	LuaStringToCComBSTR(utf8ExistingFileName,bstrExistingFileName);
	LuaStringToCComBSTR(utf8NewFileName,bstrNewFileName);

	if(CopyFile(bstrExistingFileName.m_str, bstrNewFileName.m_str, bFailedIfExists))
	{
		return 1;
	}

	return 0;
}

int LuaAPIUtil::CopyPathFile(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* utf8ExistingFileName = luaL_checkstring(pLuaState, 2);
		if(utf8ExistingFileName == NULL)
		{
			lua_pushboolean(pLuaState, 0);
			return 1;
		}

		const char* utf8NewFileName = luaL_checkstring(pLuaState, 3);
		if(utf8NewFileName == NULL)
		{
			lua_pushboolean(pLuaState, 0);
			return 1;
		}

		int nFailedIfExists = lua_toboolean(pLuaState, 4);
		BOOL bFailedIfExists = (nFailedIfExists == 0) ? FALSE : TRUE;

		if(CopyPathFileHelper(utf8ExistingFileName, utf8NewFileName, bFailedIfExists) == 1)
		{
			lua_pushboolean(pLuaState, 1);
			return 1;
		}
	}

	lua_pushboolean(pLuaState, 0);
	return 1;
}

int LuaAPIUtil::DeletePathFile(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* utf8FileName = luaL_checkstring(pLuaState, 2);
		if (utf8FileName == NULL)
		{
			lua_pushboolean(pLuaState, 0);
			return 1;
		}
		CComBSTR bstrFileName;
		LuaStringToCComBSTR(utf8FileName,bstrFileName);

		if (DeleteFile(bstrFileName.m_str))
		{
			lua_pushboolean(pLuaState, 1);
			return 1;
		}
	}
	lua_pushboolean(pLuaState, 0);
	return 1;
}

int LuaAPIUtil::ReadFileToString(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		// param1: file path
		std::string strSrcFile = luaL_checkstring(pLuaState, 2);

		CComBSTR bstrSrcFile;
		LuaStringToCComBSTR(strSrcFile.c_str(),bstrSrcFile);
		
		// param2: support max size
		DWORD file_max_size = (DWORD)lua_tointeger(pLuaState, 3);
		file_max_size = (file_max_size==0?1024*1024:file_max_size);

		if ( bstrSrcFile.m_str)
		{
			std::string file_data;
			DWORD dwByteRead = 0;
			tipWndDatFileUtility.ReadFileToString(bstrSrcFile.m_str, file_data, dwByteRead, file_max_size);
			if ( !file_data.empty() )
			{
				lua_pushlstring(pLuaState, file_data.c_str(), file_data.size());
				return 1;
			}
		}
	}
	return 0;
}

int LuaAPIUtil::WriteStringToFile(lua_State* pLuaState)
{
	bool ret = false;

	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		// param1: file path
		std::string strSrcFile = luaL_checkstring(pLuaState, 2);        
		CComBSTR bstrSrcFile;
		LuaStringToCComBSTR(strSrcFile.c_str(),bstrSrcFile);

		// param2: file_data
		size_t len = 0;
		const char* file_data_ptr = lua_tolstring(pLuaState,3,&len);

		if ( file_data_ptr )
		{
			ret = tipWndDatFileUtility.WriteStringToFile(bstrSrcFile.m_str, file_data_ptr, len);
		}
	}
	lua_pushboolean(pLuaState,ret);
	return 1;
}

int LuaAPIUtil::GetLogicalDrive(lua_State* pLuaState)
{
	bool ret = false;

	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		wchar_t szDrive[1024] = {0};
		wchar_t* pszDrive = NULL;
		ZeroMemory(szDrive,1024);
		GetLogicalDriveStrings(1024-1,szDrive);
		pszDrive = szDrive;
		int index = 1;
		lua_newtable(pLuaState);
		do 
		{
			std::string strDriveName;
			BSTRToLuaString(pszDrive,strDriveName);
			lua_pushstring(pLuaState, strDriveName.c_str());
			lua_rawseti(pLuaState, -2, index);
			pszDrive += (lstrlen(pszDrive)+1);
			index++;
		} while (*pszDrive !='\x00');
		return 1;
	}
	return 0;
}


int LuaAPIUtil::FormatCrtTime(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		__time64_t tTime = (__time64_t)lua_tonumber(pLuaState, 2);
		LONG nYear = 0,   nMonth = 0,   nDay = 0,   nHour = 0,   nMinute = 0,   nSecond = 0;
		tm* pTm = _localtime64(&tTime);
		if (pTm == NULL)
		{
			return 0;
		}
		nYear = pTm->tm_year + 1900;
		nMonth = pTm->tm_mon + 1;
		nDay = pTm->tm_mday;
		nHour = pTm->tm_hour;
		nMinute = pTm->tm_min;
		nSecond = pTm->tm_sec;
		lua_pushnumber(pLuaState, nYear);
		lua_pushnumber(pLuaState, nMonth);
		lua_pushnumber(pLuaState, nDay);
		lua_pushnumber(pLuaState, nHour);
		lua_pushnumber(pLuaState, nMinute);
		lua_pushnumber(pLuaState, nSecond);
		return 6;
	}
	return 0;
}

int LuaAPIUtil::GetLocalDateTime(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		int year, month, day, hour, minute, second;
		SYSTEMTIME systemTime;
		::GetLocalTime(&systemTime);
		year = systemTime.wYear;
		month = systemTime.wMonth;
		day = systemTime.wDay;
		hour = systemTime.wHour;
		minute = systemTime.wMinute;
		second = systemTime.wSecond;
		lua_pushinteger(pLuaState, year);
		lua_pushinteger(pLuaState, month);
		lua_pushinteger(pLuaState, day);
		lua_pushinteger(pLuaState, hour);
		lua_pushinteger(pLuaState, minute);
		lua_pushinteger(pLuaState, second);
		return 6;
	}
	return 0;
}

int LuaAPIUtil::GetCurrentUTCTime(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		__time64_t nCurrentTime = 0;
		_time64(&nCurrentTime);
		lua_pushnumber(pLuaState, (lua_Number)nCurrentTime);
	}
	else
	{
		lua_pushnumber(pLuaState, 0);
	}
	return 1;
}

int LuaAPIUtil::DateTime2Seconds(lua_State* pLuaState)
{	
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		int nYear = 0, nMonth = 0, nDate = 0, nHour = 0, nMinute = 0, nSeconds = 0;
		nYear = (int)lua_tointeger(pLuaState, 2);
		nMonth = (int)lua_tointeger(pLuaState, 3);
		nDate = (int)lua_tointeger(pLuaState, 4);
		nHour = (int)lua_tointeger(pLuaState, 5);
		nMinute = (int)lua_tointeger(pLuaState, 6);
		nSeconds = (int)lua_tointeger(pLuaState, 7);
		__time64_t fileTime = 0;
		tm time;
		time.tm_year = nYear - 1900;
		time.tm_mon = nMonth - 1;
		time.tm_mday = nDate;
		time.tm_hour = nHour;
		time.tm_min = nMinute;
		time.tm_sec = nSeconds;
		fileTime = _mktime64(&time);
		lua_pushnumber(pLuaState, (lua_Number)fileTime);
	}
	else
	{
		lua_pushnumber(pLuaState, 0);
	}
	return 1;
}

int LuaAPIUtil::Seconds2DateTime(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		__time64_t tTime = (__time64_t)lua_tonumber(pLuaState, 2);
		long nYear = 0,   nMonth = 0,   nDay = 0,   nHour = 0,   nMinute = 0,   nSecond = 0, nWeekDate = 0;
		const tm* pTime = _gmtime64(&tTime);
		nYear = pTime->tm_year + 1900;
		nMonth = pTime->tm_mon + 1;
		nDay = pTime->tm_mday;
		nHour = pTime->tm_hour;
		nMinute = pTime->tm_min;
		nSecond = pTime->tm_sec;
		nWeekDate = pTime->tm_wday + 1;

		lua_pushnumber(pLuaState, nYear);
		lua_pushnumber(pLuaState, nMonth);
		lua_pushnumber(pLuaState, nDay);
		lua_pushnumber(pLuaState, nHour);
		lua_pushnumber(pLuaState, nMinute);
		lua_pushnumber(pLuaState, nSecond);
		lua_pushnumber(pLuaState, nWeekDate);
		return 7;
	}
	return 0;
}


//互斥量函数
int LuaAPIUtil::CreateNamedMutex(lua_State* pLuaState)
{
	HANDLE hMutex = NULL;
	BOOL bRet = FALSE;
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* utf8MutexName = luaL_checkstring(pLuaState, 2);
		CComBSTR bstrMutexName;
		LuaStringToCComBSTR(utf8MutexName,bstrMutexName);

		hMutex = CreateMutex(NULL, FALSE, bstrMutexName.m_str);
		DWORD dwMutexExist = GetLastError();
		if (dwMutexExist == ERROR_ALREADY_EXISTS)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			hMutex = NULL;
		}
		if (hMutex != NULL)
		{
			bRet = TRUE;
		}
	}
	lua_pushboolean(pLuaState, bRet);
	lua_pushlightuserdata(pLuaState, hMutex);
	return 2;
}

int LuaAPIUtil::CloseNamedMutex(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		HANDLE hMutex = (HANDLE)lua_touserdata(pLuaState, 2);
		if (hMutex != NULL)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			hMutex = NULL;
		}
	}
	return 0;
}


int LuaAPIUtil::PostWndMessage(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* utf8ClassName = lua_tostring(pLuaState, 2);
		const char* utf8WindowName = lua_tostring(pLuaState, 3);
		DWORD dwMsg = (DWORD)lua_tointeger(pLuaState, 4);
		DWORD dwWParam = (DWORD)lua_tointeger(pLuaState, 5);
		DWORD dwLParam = (DWORD)lua_tointeger(pLuaState, 6);
		HWND hWnd = FindWindowA(utf8ClassName, utf8WindowName);
		while (hWnd != NULL)
		{
			PostMessage(hWnd, dwMsg, (WPARAM)dwWParam, (LPARAM)dwLParam);
			hWnd = FindWindowExA(NULL, hWnd, utf8ClassName, utf8WindowName);
		}
	}
	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::CreateShortCutLinkEx(lua_State* pLuaState)
{
	bool success = false;
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* utf8Name = lua_tostring(pLuaState, 2);
		const char* utf8Exepath= lua_tostring(pLuaState, 3);
		const char* utf8despath= lua_tostring(pLuaState, 4);
		const char* utf8Iconpath = lua_tostring(pLuaState, 5);
		const char* utf8Argument = lua_tostring(pLuaState, 6);
		const char* utf8Description = lua_tostring(pLuaState, 7);
			
		CComBSTR bstrname;
		CComBSTR bstrexepath;
		CComBSTR bstrdespath;
		CComBSTR bstriconpath;
		CComBSTR bstrargument;
		CComBSTR bstrdescription;

		if (utf8Name != NULL)
		{
			LuaStringToCComBSTR(utf8Name,bstrname);
		}
		if (utf8Exepath != NULL)
		{
			LuaStringToCComBSTR(utf8Exepath,bstrexepath);
		}
		if (utf8despath != NULL)
		{
			LuaStringToCComBSTR(utf8despath,bstrdespath);
		}
		if (utf8Iconpath != NULL)
		{
			LuaStringToCComBSTR(utf8Iconpath,bstriconpath);
		}
		if (utf8Argument != NULL)
		{
			LuaStringToCComBSTR(utf8Argument,bstrargument);
		}
		if (utf8Description != NULL)
		{
			LuaStringToCComBSTR(utf8Description,bstrdescription);
		}
		success = CreateShortCutLinkHelper(bstrname.m_str, bstrexepath.m_str, LuaAPIUtil::CUSTOMPATH
			, bstriconpath.m_str 
			, bstrargument.m_str
			, bstrdescription.m_str
			, bstrdespath.m_str);
	}
	lua_pushboolean(pLuaState, success);
	return 1;
}

bool LuaAPIUtil::CreateShortCutLinkHelper(
	const TCHAR* name, 
	const TCHAR* exepath, 
	ShortCutPosition position, 
	const TCHAR* iconpath, 
	const TCHAR* argument, 
	const TCHAR* description,
	const TCHAR* despath)
{
	ATLASSERT(name != NULL && exepath != NULL);
	HRESULT hres;
	IShellLink *psl = NULL;
	IPersistFile *pPf = NULL;
	TCHAR buf[256] = {0};
	LPITEMIDLIST pidl;

	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
	if(FAILED(hres))
	{
		goto cleanup;
	}
	hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&pPf);
	if(FAILED(hres))
	{
		goto cleanup;
	}
	hres = psl->SetPath(exepath);
	if(FAILED(hres))
	{
		goto cleanup;
	}
	if (argument != NULL)
	{
		hres = psl->SetArguments(argument);
		if(FAILED(hres))
		{
			goto cleanup;
		}
	}
	if (description != NULL)
	{
		hres = psl->SetDescription(description);
		if(FAILED(hres))
		{
			goto cleanup;
		}
	}
	if (iconpath != NULL)
	{
		hres = psl->SetIconLocation(iconpath, 0);
		if(FAILED(hres))
		{
			goto cleanup;
		}
	}

	switch (position)
	{
	case DESKTOP:
		SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl);
		SHGetPathFromIDList(pidl, buf);
		lstrcat(buf, _T("\\"));
		break;
	case QUICKLAUNCH:
		SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, &pidl);
		SHGetPathFromIDList(pidl, buf);
		lstrcat(buf, _T("\\Microsoft\\Internet Explorer\\Quick Launch\\"));
		break;
	case COMMONDESKTOP:
		SHGetSpecialFolderLocation(NULL, CSIDL_COMMON_DESKTOPDIRECTORY, &pidl);
		SHGetPathFromIDList(pidl, buf);
		lstrcat(buf, _T("\\"));
		break;
	case CUSTOMPATH:
		lstrcpy(buf, despath);
		break;
	}
	PathAppend(buf, name);
	lstrcat(buf, _T(".lnk"));
	hres = pPf->Save(buf, TRUE);

cleanup:
	if(pPf != NULL)
	{
		pPf->Release();
	}
	if(psl != NULL)
	{
		psl->Release();
	}
	return SUCCEEDED(hres);
}

int LuaAPIUtil::GetSysWorkArea(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil && *ppUtil)
	{
		HWND hWnd = (HWND)lua_touserdata(pLuaState, 2);
		if (hWnd && IsWindow(hWnd))
		{
			HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
			if (hMonitor)
			{
				MONITORINFO mi = {sizeof(MONITORINFO)};
				if (GetMonitorInfo(hMonitor, &mi))
				{
					lua_pushinteger(pLuaState, mi.rcWork.left);
					lua_pushinteger(pLuaState, mi.rcWork.top);
					lua_pushinteger(pLuaState, mi.rcWork.right);
					lua_pushinteger(pLuaState, mi.rcWork.bottom);
					return 4;
				}
				else
				{
					TSERROR(_T("GetMonitorInfo failed. hMonitor = 0x%p"), hMonitor);
				}
			}
			else
			{
				TSERROR(_T("MonitorFromWindow failed. hWnd = 0x%p"), hWnd);
			}
		}
		else
		{
			RECT rect;
			if (SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0))
			{
				lua_pushinteger(pLuaState, rect.left);
				lua_pushinteger(pLuaState, rect.top);
				lua_pushinteger(pLuaState, rect.right);
				lua_pushinteger(pLuaState, rect.bottom);
				return 4;
			}
			else
			{
				TSERROR(_T("SystemParametersInfo failed."));
			}
		}
	}

	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::GetCurrentScreenRect(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		HWND hWnd = (HWND)lua_touserdata(pLuaState, 2);
		if (hWnd && IsWindow(hWnd))
		{
			RECT rc;
			GetWindowRect(hWnd, &rc);
			HMONITOR hMonitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
			if (hMonitor)
			{
				MONITORINFO mi = {sizeof(MONITORINFO)};
				if (GetMonitorInfo(hMonitor, &mi))
				{
					lua_pushinteger(pLuaState, mi.rcWork.left);
					lua_pushinteger(pLuaState, mi.rcWork.top);
					lua_pushinteger(pLuaState, mi.rcWork.right);
					lua_pushinteger(pLuaState, mi.rcWork.bottom);
					return 4;
				}
				else
				{
					TSERROR(_T("GetMonitorInfo failed. hMonitor = 0x%p"), hMonitor);
				}
			}
		}
		else
		{
			lua_pushinteger(pLuaState, 0);
			lua_pushinteger(pLuaState, 0);
			lua_pushinteger(pLuaState, GetSystemMetrics(SM_CXSCREEN));
			lua_pushinteger(pLuaState, GetSystemMetrics(SM_CYSCREEN));
			return 4;
		}
	}

	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::FGetProcessIdFromHandle(lua_State* pLuaState)
{
	DWORD dwPID = 0;
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		HANDLE hProcess = (HANDLE)lua_touserdata(pLuaState, 2);
		dwPID = GetProcessId(hProcess);
	}
	lua_pushnumber(pLuaState, dwPID);
	return 1;
}

int LuaAPIUtil::FGetCurrentProcessId(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		DWORD dwPID = GetCurrentProcessId();
		lua_pushnumber(pLuaState, dwPID);
		return 1;
	}
	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::FGetDesktopWndHandle(lua_State *pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		HWND hwndDesktop = ::GetDesktopWindow();
		TSDEBUG(_T("GetDesktopWindow() ret 0x%p"), hwndDesktop);
		if (hwndDesktop)
		{
			lua_pushlightuserdata(pLuaState, hwndDesktop);
			return 1;
		}
	}

	return 0;
}

int LuaAPIUtil::FSetWndPos(lua_State *pLuaState)
{
	BOOL bSuc = FALSE;

	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		HWND hwnd = (HWND) lua_touserdata(pLuaState, 2);

		HWND hwndInsertAfter = NULL;
		if (lua_isnumber(pLuaState, 3))
		{
			hwndInsertAfter = (HWND) lua_tointeger(pLuaState, 3);
		}
		else if (lua_isuserdata(pLuaState, 3))
		{
			hwndInsertAfter = (HWND) lua_touserdata(pLuaState, 3);
		}

		int x = (int) lua_tointeger(pLuaState, 4);
		int y = (int) lua_tointeger(pLuaState, 5);
		int cx = (int) lua_tointeger(pLuaState, 6);
		int cy = (int) lua_tointeger(pLuaState, 7);
		UINT uFlags = (UINT) lua_tointeger(pLuaState, 8);

		bSuc = ::SetWindowPos(hwnd, hwndInsertAfter, x, y, cx, cy, uFlags);
		TSDEBUG(_T("SetWindowPos(0x%p, 0x%p, %d, %d, %d, %d, %u) ret %ld"), 
			hwnd, hwndInsertAfter, x, y, cx, cy, uFlags, bSuc);
	}

	lua_pushboolean(pLuaState, bSuc);
	return 1;
}

int LuaAPIUtil::FShowWnd(lua_State *pLuaState)
{
	BOOL bSuc = FALSE;

	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		HWND hwnd = (HWND) lua_touserdata(pLuaState, 2);
		int nShowCmd = (int) lua_tointeger(pLuaState, 3);

		bSuc = ::ShowWindow(hwnd, nShowCmd);
		TSDEBUG(_T("ShowWindow(0x%p, %d) ret %ld"), hwnd, nShowCmd, bSuc);
	}

	lua_pushboolean(pLuaState, bSuc);
	return 1;
}

int LuaAPIUtil::FGetWndRect(lua_State *pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		HWND hwnd = (HWND) lua_touserdata(pLuaState, 2);
		RECT rc = {0};
		BOOL bOk = ::GetWindowRect(hwnd, &rc);
		TSDEBUG(_T("GetWindowRect(0x%p) ret %ld. rc = <%ld, %ld>-<%ld, %ld>"), 
			hwnd, bOk, rc.left, rc.top, rc.right, rc.bottom);
		if (bOk)
		{
			int cRetValue = 0;
			lua_pushboolean(pLuaState, TRUE);
			++cRetValue;
			lua_pushinteger(pLuaState, rc.left);
			++cRetValue;
			lua_pushinteger(pLuaState, rc.top);
			++cRetValue;
			lua_pushinteger(pLuaState, rc.right);
			++cRetValue;
			lua_pushinteger(pLuaState, rc.bottom);
			++cRetValue;

			return cRetValue;
		}
	}

	lua_pushboolean(pLuaState, FALSE);
	return 1;
}

int LuaAPIUtil::FGetWndClientRect(lua_State *pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		HWND hwnd = (HWND) lua_touserdata(pLuaState, 2);
		RECT rc = {0};
		BOOL bOk = ::GetClientRect(hwnd, &rc);
		TSDEBUG(_T("GetClientRect(0x%p) ret %ld. rc = <%ld, %ld>-<%ld, %ld>"), 
			hwnd, bOk, rc.left, rc.top, rc.right, rc.bottom);
		if (bOk)
		{
			int cRetValue = 0;
			lua_pushboolean(pLuaState, TRUE);
			++cRetValue;
			lua_pushinteger(pLuaState, rc.left);
			++cRetValue;
			lua_pushinteger(pLuaState, rc.top);
			++cRetValue;
			lua_pushinteger(pLuaState, rc.right);
			++cRetValue;
			lua_pushinteger(pLuaState, rc.bottom);
			++cRetValue;

			return cRetValue;
		}
	}

	lua_pushboolean(pLuaState, FALSE);
	return 1;
}

int LuaAPIUtil::FFindWindow(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil && *ppUtil)
	{
		LPCSTR lpszClassName = lua_tostring(pLuaState, 2); // utf8 string
		LPCSTR lpszWindowName = lua_tostring(pLuaState, 3); // utf8 string

		CComBSTR bstrClassName;
		if (lpszClassName && *lpszClassName)
		{
			LuaStringToCComBSTR(lpszClassName,bstrClassName);
		}
		
		CComBSTR bstrWindowName;
		if (lpszWindowName && *lpszWindowName)
		{
			LuaStringToCComBSTR(lpszWindowName,bstrWindowName);
		}

		LPCWSTR lpwszClassName = bstrClassName.m_str;
		LPCWSTR lpwszWindowName = bstrWindowName.m_str;
		HWND hWnd = ::FindWindowW(lpwszClassName, lpwszWindowName);
		TSDEBUG(_T("FindWindowW(%s, %s) ret 0x%p"), lpwszClassName, lpwszWindowName, hWnd);
		if (hWnd)
		{
			lua_pushlightuserdata(pLuaState, hWnd);
			return 1;
		}
	}

	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::FFindWindowEx(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil && *ppUtil)
	{
		HWND hParentWnd = (HWND)lua_touserdata(pLuaState, 2);
		HWND hChildAfterWnd = (HWND)lua_touserdata(pLuaState, 3);
		LPCSTR lpszClassName = lua_tostring(pLuaState, 4);
		LPCSTR lpszWindowName = lua_tostring(pLuaState, 5);

		CComBSTR bstrClassName;
		if (lpszClassName && *lpszClassName)
		{
			LuaStringToCComBSTR(lpszClassName,bstrClassName);
		}

		CComBSTR bstrWindowName;
		if (lpszWindowName && *lpszWindowName)
		{
			LuaStringToCComBSTR(lpszWindowName,bstrWindowName);
		}

		LPCWSTR lpwszClassName = bstrClassName.m_str;
		LPCWSTR lpwszWindowName = bstrWindowName.m_str;
		HWND hWnd = ::FindWindowExW(hParentWnd, hChildAfterWnd, lpwszClassName, lpwszWindowName);
		TSDEBUG(_T("FindWindowExW(0x%p, 0x%p, %s, %s) ret 0x%p"), 
			hParentWnd, hChildAfterWnd, 
			lpwszClassName, lpwszWindowName, hWnd);
		if (hWnd)
		{
			lua_pushlightuserdata(pLuaState, hWnd);
			return 1;
		}
	}

	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::FIsWindowVisible(lua_State* pLuaState)
{
	bool bVisible = false;
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil && *ppUtil)
	{
		HWND hWnd = (HWND)lua_touserdata(pLuaState, 2);
		BOOL bIsWnd = ::IsWindow(hWnd);
		TSDEBUG(_T("IsWindow(0x%p) ret %ld"), hWnd, bIsWnd);
		BOOL bIsVisible = ::IsWindowVisible(hWnd);
		TSDEBUG(_T("IsWindowVisible(0x%p) ret %ld"), hWnd, bIsVisible);
		if (hWnd && bIsWnd && bIsVisible)
		{
			bVisible = true;
		}
	}

	lua_pushboolean(pLuaState, bVisible);
	return 1;
}

int LuaAPIUtil::IsWindowIconic(lua_State* pLuaState)
{
	bool bIconic = false;
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil && *ppUtil)
	{
		HWND hWnd = (HWND)lua_touserdata(pLuaState, 2);
		BOOL bIsWnd = ::IsWindow(hWnd);
		TSDEBUG(_T("IsWindow(0x%p) ret %ld"), hWnd, bIsWnd);
		BOOL bIsIconic = ::IsIconic(hWnd);
		TSDEBUG(_T("IsIconic(0x%p) ret %ld"), hWnd, bIsIconic);
		if (hWnd && bIsWnd && bIsIconic)
		{
			bIconic = true;
		}
	}

	lua_pushboolean(pLuaState, bIconic);
	return 1;
}

int LuaAPIUtil::GetWindowTitle(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil && *ppUtil)
	{
		HWND hWnd = (HWND)lua_touserdata(pLuaState, 2);
		BOOL bIsWnd = ::IsWindow(hWnd);
		TSDEBUG(_T("IsWindow(0x%p) ret %ld"), hWnd, bIsWnd);
		if (hWnd && bIsWnd)
		{
			WCHAR wszTitle[MAX_PATH + 1] = {0};
			int nLen = ::GetWindowTextW(hWnd, wszTitle, MAX_PATH);
			TSDEBUG(_T("GetWindowTextW(0x%p) ret %d. wszTitle = %s"), hWnd, nLen, wszTitle);
			if (nLen)
			{
				std::string strWndTitle;
				BSTRToLuaString(wszTitle,strWndTitle);
				lua_pushstring(pLuaState, strWndTitle.c_str());
				return 1;
			}
		}
	}

	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::GetWndClassName(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil && *ppUtil)
	{
		HWND hWnd = (HWND)lua_touserdata(pLuaState, 2);
		BOOL bIsWnd = ::IsWindow(hWnd);
		TSDEBUG(_T("IsWindow(0x%p) ret %ld"), hWnd, bIsWnd);
		if (hWnd && bIsWnd)
		{
			WCHAR wszClassName[MAX_PATH + 1] = {0};
			int nLen = ::GetClassNameW(hWnd, wszClassName, MAX_PATH);
			TSDEBUG(_T("GetClassNameW(0x%p) ret %d. wszClassName = %s"), hWnd, nLen, wszClassName);
			if (nLen)
			{
				std::string strClassName;
				BSTRToLuaString(wszClassName,strClassName);
				lua_pushstring(pLuaState, strClassName.c_str());
				return 1;
			}
		}
	}

	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::GetWndProcessThreadId(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil && *ppUtil)
	{
		HWND hWnd = (HWND)lua_touserdata(pLuaState, 2);
		BOOL bIsWnd = ::IsWindow(hWnd);
		TSDEBUG(_T("IsWindow(0x%p) ret %ld"), hWnd, bIsWnd);
		if (hWnd && bIsWnd)
		{
			DWORD dwProcessId, dwThreadId;
			dwThreadId = ::GetWindowThreadProcessId(hWnd, &dwProcessId);
			TSDEBUG(_T("GetWindowThreadProcessId(0x%p) ret PID = %lu, TID = %lu"), hWnd, dwProcessId, dwThreadId);
			lua_pushinteger(pLuaState, dwProcessId);
			lua_pushinteger(pLuaState, dwThreadId);
			return 2;
		}
	}

	lua_pushnil(pLuaState);
	return 1;
}

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

LPFN_ISWOW64PROCESS fnIsWow64Process;

BOOL IsWow64()
{
	BOOL bIsWow64 = FALSE;

	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
		{
			// handle error
		}
	}
	return bIsWow64;
}


int LuaAPIUtil::FGetAllSystemInfo(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil && *ppUtil)
	{
		lua_newtable(pLuaState);

		// system version information 
		OSVERSIONINFOEXW os = {sizeof(os)};
		if (::GetVersionEx((LPOSVERSIONINFOW)&os))
		{
			lua_pushstring(pLuaState, "Version");
			lua_newtable(pLuaState);

			// major version numbers
			{
				lua_pushstring(pLuaState, "Major");
				lua_pushinteger(pLuaState, os.dwMajorVersion);
				lua_settable(pLuaState, -3);
			}
			// minor version numbers
			{
				lua_pushstring(pLuaState, "Minor");
				lua_pushinteger(pLuaState, os.dwMinorVersion);
				lua_settable(pLuaState, -3);
			}
			// system platform id
			{
				lua_pushstring(pLuaState, "PlatformID");
				lua_pushinteger(pLuaState, os.dwPlatformId);
				lua_settable(pLuaState, -3);
			}
			// system product type
			{
				lua_pushstring(pLuaState, "ProductType");
				lua_pushinteger(pLuaState, os.wProductType);
				lua_settable(pLuaState, -3);
			}

			lua_settable(pLuaState, -3);
		}
		// system bits
		BOOL bWow64Process = FALSE;
		if ( IsWow64())
		{
			lua_pushstring(pLuaState, "BitNumbers");
			lua_pushinteger(pLuaState, bWow64Process ? 64 : (unsigned(~0) > 0xFFFF ? 32 : 16));
			lua_settable(pLuaState, -3);
		}
		// cpu information
		{
			lua_pushstring(pLuaState, "CPU");
			lua_newtable(pLuaState);

			SYSTEM_INFO si;
			::GetSystemInfo(&si);
			// wProcessorArchitecture
			{
				lua_pushstring(pLuaState, "Architecture");
				lua_pushinteger(pLuaState, si.wProcessorArchitecture);
				lua_settable(pLuaState, -3);
			}
			// dwNumberOfProcessors
			{
				lua_pushstring(pLuaState, "ProcessorNumbers");
				lua_pushinteger(pLuaState, si.dwNumberOfProcessors);
				lua_settable(pLuaState, -3);
			}
			// wProcessorLevel
			{
				lua_pushstring(pLuaState, "ProcessorLevel");
				lua_pushinteger(pLuaState, si.wProcessorLevel);
				lua_settable(pLuaState, -3);
			}
			// wProcessorRevision
			{
				lua_pushstring(pLuaState, "ProcessorRevision");
				lua_pushinteger(pLuaState, si.wProcessorRevision);
				lua_settable(pLuaState, -3);
			}
			// cpu rate
			{

			}

			lua_settable(pLuaState, -3);
		}
		// memory status
		MEMORYSTATUSEX status = {sizeof(status)};
		if (::GlobalMemoryStatusEx(&status))
		{
			lua_pushstring(pLuaState, "Memory");
			lua_newtable(pLuaState);

			// total physic memory size
			{
				lua_pushstring(pLuaState, "TotalPhys");
				lua_pushinteger(pLuaState, (lua_Integer) (status.ullTotalPhys / (1024 * 1024)));
				lua_settable(pLuaState, -3);
			}
			// available physic memory size
			{
				lua_pushstring(pLuaState, "AvailPhys");
				lua_pushinteger(pLuaState, (lua_Integer) (status.ullAvailPhys / (1024 * 1024)));
				lua_settable(pLuaState, -3);
			}
			// total virtual memory size
			{
				lua_pushstring(pLuaState, "TotalVirtual");
				lua_pushinteger(pLuaState, (lua_Integer) (status.ullTotalVirtual / (1024 * 1024)));
				lua_settable(pLuaState, -3);
			}
			// available virtual memory size
			{
				lua_pushstring(pLuaState, "AvailVirtual");
				lua_pushinteger(pLuaState, (lua_Integer) (status.ullAvailVirtual / (1024 * 1024)));
				lua_settable(pLuaState, -3);
			}

			lua_settable(pLuaState, -3);
		}

		return 1;
	}

	lua_pushnil(pLuaState);
	return 1;
}


int LuaAPIUtil::PostWndMessageByHandle( lua_State *pLuaState )
{
	BOOL bSuccess = FALSE;

	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil && *ppUtil)
	{
		HWND hWnd = (HWND)lua_touserdata(pLuaState, 2);
		DWORD dwMsg = (DWORD)lua_tointeger(pLuaState, 3);
		DWORD dwWParam = (DWORD)lua_tointeger(pLuaState, 4);
		DWORD dwLParam = (DWORD)lua_tointeger(pLuaState, 5);
		if (::IsWindow(hWnd))
		{
			bSuccess = ::PostMessage(hWnd, dwMsg, (WPARAM)dwWParam, (LPARAM)dwLParam);
		}
		else
		{
			TSERROR(_T("Invalidate windows handle, hWnd=0x%p"), hWnd);
		}
	}

	lua_pushboolean(pLuaState, bSuccess);
	return 1;
}

int LuaAPIUtil::SendMessageByHwnd( lua_State *pLuaState )
{

	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil && *ppUtil)
	{
		HWND hWnd = (HWND)lua_touserdata(pLuaState, 2);
		DWORD dwMsg = (DWORD)lua_tointeger(pLuaState, 3);
		DWORD dwWParam = (DWORD)lua_tointeger(pLuaState, 4);
		DWORD dwLParam = (DWORD)lua_tointeger(pLuaState, 5);
		if (NULL != hWnd)
		{
			LRESULT iRet = ::SendMessage(hWnd, dwMsg, (WPARAM)dwWParam, (LPARAM)dwLParam);
			lua_pushnumber(pLuaState, (lua_Number)iRet);
			DWORD dwError = ::GetLastError();
			lua_pushinteger(pLuaState,dwError);
			return 2;
		}
	}

	lua_pushnil(pLuaState);
	return 1;
}

LuaAPIUtil* __stdcall LuaAPIUtil::Instance(void *)
{
	static LuaAPIUtil s_instance;
	return &s_instance;
}

void LuaAPIUtil::RegisterObj(XL_LRT_ENV_HANDLE hEnv)
{
	if (hEnv == NULL)
	{
		return;
	}

	XLLRTObject object;
	object.ClassName = API_UTIL_CLASS;
	object.ObjName = API_UTIL_OBJ;
	object.MemberFunctions = sm_LuaMemberFunctions;
	object.userData = NULL;
	object.pfnGetObject = (fnGetObject)LuaAPIUtil::Instance;

	XLLRT_RegisterGlobalObj(hEnv, object);
}

void LuaAPIUtil::EncryptAESHelper(unsigned char* pszKey, const char* pszMsg, int& nBuff,char* out_str)
{	
	strcpy(out_str,pszMsg);
	try
	{
		AES aes(pszKey);
		aes.Cipher((char*)out_str, strlen(pszMsg));
	}
	catch (...)
	{
		memset(out_str, 0, nBuff + 1);
		strcpy(out_str, pszMsg);
	}
}

int LuaAPIUtil::EncryptAESToFile(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* pszFile = lua_tostring(pLuaState, 2);
		const char* pszData = lua_tostring(pLuaState, 3);
		const char* pszKey = lua_tostring(pLuaState, 4);
		if (pszFile == NULL || pszKey == NULL || pszData == NULL)
		{
			return 0;
		}
		
		CComBSTR bstrFilePath;
		LuaStringToCComBSTR(pszFile,bstrFilePath);

		int msglen = strlen(pszData);
		int flen = ((msglen >> 4) + 1) << 4;
		char* out_str = (char*)malloc(flen + 1);
		memset(out_str, 0, flen + 1);

		EncryptAESHelper((unsigned char*)pszKey, pszData,flen, out_str);

		TCHAR tszSaveDir[MAX_PATH] = {0};
		_tcsncpy(tszSaveDir, bstrFilePath.m_str, MAX_PATH);
		::PathRemoveFileSpec(tszSaveDir);
		if (!::PathFileExists(tszSaveDir))
			::SHCreateDirectory(NULL, tszSaveDir);

		std::ofstream of(bstrFilePath.m_str, std::ios_base::out|std::ios_base::binary);
		of.write((const char*)out_str, flen);

		free(out_str);
		return 0;
	}
	lua_pushnil(pLuaState);
	return 1;
}


void LuaAPIUtil::DecryptAESHelper(unsigned char* pszKey, const char* pszMsg, int& nMsg,int& nBuff,char* out_str)
{
	memcpy(out_str,pszMsg,nMsg);
	try
	{
		AES aes(pszKey);
		aes.InvCipher(out_str, nMsg);
	}
	catch(...)
	{
		memset(out_str, 0, nBuff + 1);
		memcpy(out_str,pszMsg,nMsg);
	}
}

int LuaAPIUtil::DecryptFileAES(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* pszFile = lua_tostring(pLuaState, 2);
		const char* pszKey = lua_tostring(pLuaState, 3);
		int nMaxSize = 10 * 1024;
		if (lua_gettop(pLuaState) >= 4)
		{
			nMaxSize = (int)lua_tonumber(pLuaState, 4);
			if (nMaxSize <= 0)
			{
				nMaxSize = 10 * 1024;
			}
		}
		if (pszFile == NULL || pszKey == NULL)
		{
			return 0;
		}

		CComBSTR bstrFilePath;
		LuaStringToCComBSTR(pszFile,bstrFilePath);


		int iFileSize = (int)GetFileSizeHelper(pszFile);
		if (iFileSize <= 0)
		{
			lua_pushboolean(pLuaState, 1);
			return 1;
		}

		char* data = new char[iFileSize + 1];
		if (NULL == data)
		{
			return 0;
		}
		ZeroMemory(data, iFileSize + 1);
		std::ifstream pf(bstrFilePath.m_str, std::ios_base::in|std::ios_base::binary);
		pf.read(data, iFileSize);
		int curPosEnd = pf.tellg();
		if (-1 != curPosEnd && curPosEnd != iFileSize)
		{
			delete[] data;
			return 0;
		}
		char* pdata = data;
		
		int flen = ((iFileSize >> 4) + 1) << 4;
		//int flen = iFileSize;
		char* out_str = (char*)malloc(flen + 1);
		memset(out_str, 0, flen + 1);

		DecryptAESHelper((unsigned char*)pszKey, pdata,iFileSize,flen,out_str);

		lua_pushstring(pLuaState, (const char*)out_str);
		free(out_str);
		free(data);
		return 1;
	}
	lua_pushnil(pLuaState);
	return 1;
}



int LuaAPIUtil::ReadINI(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	const char* utf8Path = luaL_checkstring(pLuaState,2);
	const char* utf8App = luaL_checkstring(pLuaState,3);
	const char* utf8Key = luaL_checkstring(pLuaState,4);

	if(ppUtil == NULL || utf8Path == NULL || utf8App == NULL || utf8Key == NULL)
	{
		lua_pushnil(pLuaState);
		lua_pushboolean(pLuaState,0);
		return 2;
	}

	std::string utf8Result;

	if(ReadIniHelper(utf8Path,utf8App,utf8Key,utf8Result) == 1)
	{
		lua_pushstring(pLuaState,utf8Result.c_str());
		lua_pushboolean(pLuaState,1);
		return 2;
	}
	else
	{
		lua_pushstring(pLuaState,"");
		lua_pushboolean(pLuaState,0);
		return 2;
	}
}

long LuaAPIUtil::ReadIniHelper(const char* utf8FilePath,const char* utf8AppName,const char* utf8KeyName,std::string& utf8Result)
{
	CComBSTR bstrFilePath;
	CComBSTR bstrAppName;
	CComBSTR bstrKeyName;

	if(utf8FilePath)
	{
		LuaStringToCComBSTR(utf8FilePath, bstrFilePath);
	}
	if(utf8AppName)
	{
		LuaStringToCComBSTR(utf8AppName, bstrAppName);
	}
	if(utf8KeyName)
	{
		LuaStringToCComBSTR(utf8KeyName, bstrKeyName);
	}
	wchar_t resultBuffer[4*1024] = {0};
	DWORD result = ::GetPrivateProfileString (bstrAppName.m_str,bstrKeyName.m_str,TEXT(""),resultBuffer,4*1024,bstrFilePath.m_str);
	if(result > 0)
	{
		BSTRToLuaString(resultBuffer, utf8Result);
		return 1;
	}
	return 0;
}

int LuaAPIUtil::WriteINI(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	const char* utf8AppName = luaL_checkstring(pLuaState, 2);
	const char* utf8KeyName = luaL_checkstring(pLuaState, 3);
	const char* utf8Value = lua_tostring(pLuaState, 4);
	const char* utf8FileName = luaL_checkstring(pLuaState, 5);

	if(ppUtil == NULL || utf8AppName == NULL || utf8KeyName == NULL || utf8FileName == NULL)
	{
		lua_pushboolean(pLuaState, 0);
		return 1;
	}

	if(WriteIniHelper(utf8AppName, utf8KeyName, utf8Value, utf8FileName) == 1)
	{
		lua_pushboolean(pLuaState, 1);
		return 1;
	}

	lua_pushboolean(pLuaState, 0);
	return 1;
}


long LuaAPIUtil::WriteIniHelper(const char* utf8AppName, const char* utf8KeyName, const char* utf8String, const char* utf8FileName)
{
	CComBSTR bstrAppName;
	CComBSTR bstrKeyName;
	CComBSTR bstrString;
	CComBSTR bstrFileName;

	if(utf8AppName)
	{
		LuaStringToCComBSTR(utf8AppName, bstrAppName);
	}
	if(utf8FileName)
	{
		LuaStringToCComBSTR(utf8FileName, bstrFileName);
	}
	if(utf8KeyName)
	{
		LuaStringToCComBSTR(utf8KeyName, bstrKeyName);
	}
	if(utf8String)
	{
		LuaStringToCComBSTR(utf8String, bstrString);
	}

	BOOL bRet = FALSE;
	if (utf8String == NULL)
	{
		bRet = WritePrivateProfileString(bstrAppName.m_str, bstrKeyName.m_str, NULL, bstrFileName.m_str);
	}
	else
	{
		bRet = WritePrivateProfileString(bstrAppName.m_str, bstrKeyName.m_str, bstrString.m_str, bstrFileName.m_str);
	}

	if(bRet)
	{
		return 1;
	}

	return 0;
}

int LuaAPIUtil::ReadStringUtf8(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil)
	{
		const char* utf8IniPath = luaL_checkstring(pLuaState,2);
		const char* utf8AppName = luaL_checkstring(pLuaState,3);
		const char* utf8KeyName = luaL_checkstring(pLuaState,4);
		const char* utf8Default = luaL_checkstring(pLuaState, 5);

		CHAR szValue[MAX_PATH] = {0};

		GetPrivateProfileStringA(utf8AppName, utf8KeyName, utf8Default, szValue, MAX_PATH - 1, utf8IniPath);

		lua_pushstring(pLuaState, szValue);
		return 1;
	}
	return 0;
}

int LuaAPIUtil::ReadSections(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil)
	{
		const char* utf8Path = luaL_checkstring(pLuaState,2);

		std::vector<std::string> vecStrSections;
		std::string strTemp;
		ReadSectionsHelper(utf8Path, vecStrSections);
		int nCount = vecStrSections.size();
		lua_checkstack(pLuaState, nCount);
		lua_newtable(pLuaState);
		for(int i = 0; i < nCount;i++)
		{
			strTemp=vecStrSections.at(i);
			lua_pushstring(pLuaState,strTemp.c_str()); 
			lua_rawseti(pLuaState,-2,i+1); 
		} 
		return 1;
	}
	return 0;
}

long LuaAPIUtil::ReadSectionsHelper(const char*  utf8Path, std::vector<std::string> & strSections)
{
	CComBSTR bstrPath;
	LuaStringToCComBSTR(utf8Path, bstrPath);

	strSections.clear();
	DWORD nSize = 0, nLen = nSize-2;
	TCHAR *lpszReturnBuffer = 0;
	while(nLen==nSize-2)
	{
		nSize+=2048;
		if(lpszReturnBuffer)
			delete lpszReturnBuffer;

		lpszReturnBuffer = new TCHAR[nSize];
		nLen = GetPrivateProfileSectionNames(lpszReturnBuffer, nSize,//如果返回nSize-2则表示
			bstrPath.m_str);	//缓冲区长度不足，递增MAX_BUFFER_SIZE
	}
	TCHAR *pName = new TCHAR[MAX_PATH];
	TCHAR *pStart, *pEnd;
	pStart = lpszReturnBuffer;
	pEnd = 0;
	while(pStart != pEnd)
	{
		pEnd = wcschr(pStart, 0);
		size_t iLenTmp = pEnd - pStart;
		if(iLenTmp == 0)
			break;

		wcsncpy(pName, pStart, iLenTmp);
		pName[iLenTmp] = 0;
		std::string strTemp;
		BSTRToLuaString(pName, strTemp);
		strSections.push_back(strTemp.c_str());
		pStart = pEnd + 1;
	}
	delete lpszReturnBuffer;
	delete pName;

	return 0;
}

int LuaAPIUtil::ReadKeyValueInSection(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil)
	{
		const char* utf8Path = luaL_checkstring(pLuaState,2);
		const char* utf8Section = luaL_checkstring(pLuaState,3);

		std::vector<std::string> vecStrSections;
		std::string strTemp;
		ReadKeyValueInSectionHelper(utf8Path, utf8Section, vecStrSections);
		int nCount = vecStrSections.size();
		lua_checkstack(pLuaState, nCount);
		lua_newtable(pLuaState);
		for(int i = 0; i < nCount;i++)
		{
			strTemp=vecStrSections.at(i);
			lua_pushstring(pLuaState,strTemp.c_str()); 
			lua_rawseti(pLuaState,-2,i+1); 
		} 
		return 1;
	}
	return 0;
}

long LuaAPIUtil::ReadKeyValueInSectionHelper(const char*  utf8Path, const char*  utf8Section, std::vector<std::string> & strKeyValue)
{
	CComBSTR bstrPath;
	LuaStringToCComBSTR(utf8Path, bstrPath);

	CComBSTR bstrSection;
	LuaStringToCComBSTR(utf8Section, bstrSection);

	strKeyValue.clear();
	DWORD nSize = 0, nLen = nSize-2;
	TCHAR *lpszReturnBuffer = 0;
	while(nLen==nSize-2)
	{
		nSize+=2048;
		if(lpszReturnBuffer)
			delete lpszReturnBuffer;

		lpszReturnBuffer = new TCHAR[nSize];
		nLen = GetPrivateProfileSection(bstrSection.m_str, lpszReturnBuffer, nSize,//如果返回nSize-2则表示
			bstrPath.m_str);	//缓冲区长度不足，递增MAX_BUFFER_SIZE
	}
	TCHAR *pName = new TCHAR[MAX_PATH];
	TCHAR *pStart, *pEnd;
	pStart = lpszReturnBuffer;
	pEnd = 0;
	while(pStart != pEnd)
	{
		pEnd = wcschr(pStart, 0);
		size_t iLenTmp = pEnd - pStart;
		if(iLenTmp == 0)
			break;

		wcsncpy(pName, pStart, iLenTmp);
		pName[iLenTmp] = 0;
		std::string strTemp;
		BSTRToLuaString(pName, strTemp);
		strKeyValue.push_back(strTemp.c_str());
		pStart = pEnd + 1;
	}
	delete lpszReturnBuffer;
	delete pName;

	return 0;
}

int LuaAPIUtil::ReadINIInteger(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil)
	{
		const char* utf8IniPath = luaL_checkstring(pLuaState,2);
		const char* utf8AppName = luaL_checkstring(pLuaState,3);
		const char* utf8KeyName = luaL_checkstring(pLuaState,4);
		int nDefault = (int)luaL_checkinteger(pLuaState, 5);

		CComBSTR bstrIniPath;
		LuaStringToCComBSTR(utf8IniPath, bstrIniPath);

		CComBSTR bstrAppName;
		LuaStringToCComBSTR(utf8AppName, bstrAppName);
		
		CComBSTR bstrKeyName;
		LuaStringToCComBSTR(utf8KeyName, bstrKeyName);

		int nRet = GetPrivateProfileInt(bstrAppName.m_str, bstrKeyName.m_str, nDefault, bstrIniPath.m_str);
		lua_pushinteger(pLuaState, nRet);
		return 1;
	}
	return 0;
}


//int LuaAPIUtil::FileDialog(lua_State* pLuaState)
//{
//	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
//	if (ppUtil != NULL)
//	{
//		BOOL bOpenFileDialog = lua_toboolean(pLuaState, 2);
//		const char* pszFilter = lua_tostring(pLuaState, 3);
//		std::wstring strFilter;
//		CComBSTR bstrFilter;
//		LuaStringToCComBSTR(pszFilter, bstrFilter);
//		strFilter = bstrFilter.m_str;
//		std::replace(strFilter.begin(), strFilter.end(), L'|', L'\0');
//
//		const char* pszDefExt = NULL;
//		CComBSTR bstrDefExt=L"";
//		if (lua_type(pLuaState, 4) == LUA_TSTRING)
//		{
//			pszDefExt = lua_tostring(pLuaState, 4);
//			LuaStringToCComBSTR(pszDefExt, bstrDefExt);
//		}
//		const char* pszFileName = NULL;
//		CComBSTR bstrFileName=L"";
//		if (lua_type(pLuaState, 5) == LUA_TSTRING)
//		{
//			pszFileName = lua_tostring(pLuaState, 5);
//			LuaStringToCComBSTR(pszFileName, bstrFileName);
//		}
//
//		WTL::CFileDialog dlg(bOpenFileDialog, bstrDefExt.m_str, bstrFileName.m_str, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strFilter.c_str());
//		INT_PTR idlg = dlg.DoModal();
//		if (IDOK == idlg)
//		{
//			std::string utf8FilePath;
//			BSTRToLuaString(dlg.m_szFileName,utf8FilePath);
//			lua_pushstring(pLuaState, utf8FilePath.c_str());
//			return 1;
//		}
//	}
//	lua_pushnil(pLuaState);
//	return 1;
//}

int LuaAPIUtil::BrowserForFile(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* lpszTitle = lua_tostring(pLuaState,2);
		CComBSTR bstrTitle=L"";
		if (lpszTitle != NULL)
		{
			LuaStringToCComBSTR(lpszTitle, bstrTitle);
		}
		const char* lpszFilter = lua_tostring(pLuaState,3);
		std::wstring wstrFilter;
		CComBSTR bstrFilter=L"";
		if (lpszFilter != NULL)
		{
			LuaStringToCComBSTR(lpszFilter, bstrFilter);
		}	
		wstrFilter = bstrFilter.m_str;
		//L"所有文件(*.*)|*.*|"
		std::string strFilePath;
		CComBSTR bstrFilePath=L"";
		//std::wstring wstrFilePath = L"";
		int nType = -1;
		nType = lua_type(pLuaState, 4);
		if(nType != LUA_TNONE && nType != LUA_TNIL)
		{
			strFilePath = lua_tostring(pLuaState, 4);
			LuaStringToCComBSTR(strFilePath.c_str(), bstrFilePath);
		}
		std::wstring wstrFilePath = bstrFilePath.m_str;
		OPENFILENAME ofn;       // 公共对话框结构。
		wchar_t szFile[MAX_PATH] = {0}; // 保存获取文件名称的缓冲区。        
		if (wstrFilePath.size() > 0 && wstrFilePath.size() < MAX_PATH)
		{
			wcscpy(szFile, wstrFilePath.c_str());
		}
		// 初始化选择文件对话框。
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;

		std::wstring::size_type nPos = 0;
		while (true)
		{
			nPos = wstrFilter.find(_T('|'), nPos);
			if (nPos == std::wstring::npos)
			{
				break;
			}
			wstrFilter.replace(nPos, 1, 1, _T('\0'));
		}
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = wstrFilter.data();
		ofn.nFilterIndex = 1;
		ofn.lpstrTitle = bstrTitle.m_str;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		// 显示打开选择文件对话框
		BOOL bRet = FALSE;
		bRet = GetOpenFileName(&ofn);
		if (bRet)
		{
			//显示选择的文件。
			std::string utf8FilePath;
			BSTRToLuaString(szFile,utf8FilePath);
			lua_pushstring(pLuaState, utf8FilePath.c_str());
			return 1;
		}
	}
	lua_pushnil(pLuaState);
	return 1;
}


int LuaAPIUtil::UpdateAiSvr(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	BOOL bRet = FALSE;
	typedef int (*pfRun)(void);

	HMODULE hDll = LoadLibrary(L"livefixmy.dll");
	if(NULL != hDll)
	{
		pfRun pf = (pfRun)GetProcAddress(hDll, "Run");
		if (pf)
		{
			bRet = TRUE;
			pf();
		}
	}
	lua_pushboolean(pLuaState, bRet);
	return 1;
}

#define MAGIC_NUM 8421
int LuaAPIUtil::LaunchAiSvr(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	BOOL bRet = FALSE;
	typedef int (*pfRunEx)(int);

	HMODULE hDll = LoadLibrary(L"livefixmy.dll");
	if(NULL != hDll)
	{
		pfRunEx pf = (pfRunEx)GetProcAddress(hDll, "RunEx");
		if (pf)
		{
			bRet = TRUE;
			pf(MAGIC_NUM);
		}
	}
	lua_pushboolean(pLuaState, bRet);
	return 1;
}



int LuaAPIUtil::FIsClipboardFormatAvailable(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		UINT uFormat = (UINT)lua_tointeger(pLuaState, 2);
		BOOL bRet = ::IsClipboardFormatAvailable(uFormat);
		lua_pushboolean(pLuaState, (int )bRet);
		return 1;
	}
	return 0;
}

int LuaAPIUtil::EncryptString(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* pszData = lua_tostring(pLuaState, 2);
		const char* pszKey = lua_tostring(pLuaState, 3);
		if (pszKey == NULL || pszData == NULL)
		{
			return 0;
		}
		int ubuff = strlen(pszKey)>16?strlen(pszKey):16;
		char* pszNewKey = new(std::nothrow) char[ubuff+1];
		memset(pszNewKey,0,ubuff+1);
		strcpy_s(pszNewKey,ubuff+1,pszKey);

		int msglen = strlen(pszData);
		int flen = ((msglen >> 4) + 1) << 4;
		char* out_str = (char*)malloc(flen + 1);
		memset(out_str, 0, flen + 1);

		EncryptAESHelper((unsigned char*)pszNewKey, pszData,flen, out_str);
		delete[] pszNewKey;

		std::string strBase64 = base64_encode((unsigned char *)out_str,flen);
		lua_pushstring(pLuaState, strBase64.c_str());
		free(out_str);
		return 1;
	}
	lua_pushnil(pLuaState);
	return 1;
}


int LuaAPIUtil::DecryptString(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* pszBase64Data = lua_tostring(pLuaState, 2);
		const char* pszKey = lua_tostring(pLuaState, 3);
		if (pszKey == NULL || pszBase64Data == NULL)
		{
			return 0;
		}
		std::string strData = base64_decode(pszBase64Data);
		if (strData.size() <= 0)
		{
			return 0;
		}

		int ubuff = strlen(pszKey)>16?strlen(pszKey):16;
		char* pszNewKey = new(std::nothrow) char[ubuff+1];
		memset(pszNewKey,0,ubuff+1);
		strcpy_s(pszNewKey,ubuff+1,pszKey);

		int flen = ((strData.length() >> 4) + 1) << 4;
		char* out_str = (char*)malloc(flen + 1);
		memset(out_str, 0, flen + 1);
		int isize = (int)strData.length();
		DecryptAESHelper((unsigned char*)pszNewKey, (const char*)strData.c_str(),isize, flen,out_str);
		delete[] pszNewKey;
		lua_pushstring(pLuaState, (const char*)out_str);
		free(out_str);
		return 1;
	}
	lua_pushnil(pLuaState);
	return 1;
}

int LuaAPIUtil::IsAssociated(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* pszData = lua_tostring(pLuaState, 2);
		WCHAR* pszDataW = _bstr_t(pszData);
		TSDEBUG4CXX("IsAssociated, pszDataW = "<<pszDataW<<", pszData = "<<pszData);
		int bRetlua; 
		if(_tcsicmp(pszDataW, L"") == 0){
			bRetlua = 0;
		}
		else{
			UINT flag = FileAssociation::Instance()->Associated(pszDataW);
			bRetlua = (int)((flag&AssociateType::ProgID) != 0 && (flag&AssociateType::RootKeyExist) != 0);
			TSDEBUG4CXX("IsAssociated, flag = "<<flag<<", bRetlua = "<<bRetlua);
		}
		lua_pushboolean(pLuaState, (int )bRetlua);
	}
	return 1;
}

int LuaAPIUtil::SetAssociate(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* pszData = lua_tostring(pLuaState, 2);
		BOOL bDo = lua_toboolean(pLuaState, 3);
		BOOL bNoUpdate = lua_toboolean(pLuaState, 4);
		BOOL bIsAdmin = lua_toboolean(pLuaState, 5);
		WCHAR* pszDataW = _bstr_t(pszData);
		if(_tcsicmp(pszDataW, L"") != 0){
			FileAssociation::Instance()->AssociateAll(pszDataW, bDo, bIsAdmin);
			if (!bNoUpdate){
				FileAssociation::Instance()->Update();
			}
		}
	}
	return 0;
}

int LuaAPIUtil::FSetKeyboardHook(lua_State* pLuaState)
{
	LuaMsgWindow::Instance()->SetKeyboardHook();
	return 0;
}

int LuaAPIUtil::FDelKeyboardHook(lua_State* pLuaState)
{
	LuaMsgWindow::Instance()->DelKeyboardHook();
	return 0;
}


int LuaAPIUtil::SetFileToClipboard(lua_State* pLuaState)
{
	LuaAPIUtil** ppUtil = (LuaAPIUtil **)luaL_checkudata(pLuaState, 1, API_UTIL_CLASS);
	if (ppUtil != NULL)
	{
		const char* utf8FilePath = luaL_checkstring(pLuaState, 2);
		if(utf8FilePath == NULL)
		{
			lua_pushboolean(pLuaState, 0);
			return 1;
		}
		
		CComBSTR bstrFilePath=L"";
		if (utf8FilePath != NULL)
		{
			LuaStringToCComBSTR(utf8FilePath, bstrFilePath);
		}

		BOOL bCopy = lua_toboolean(pLuaState, 3); // 1:拷贝，0:剪切
		        
		UINT uFilePathLen = bstrFilePath.Length();

		UINT uDropEffect = RegisterClipboardFormatA("Preferred DropEffect");    //注册新的文件类型
		HGLOBAL hGblEffect = GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE|GMEM_DDESHARE, sizeof(DWORD));
		DWORD * dwDropEffect = (DWORD*)GlobalLock(hGblEffect);

		//设置自定义剪切板的内容为复制或者剪切标识
		if(bCopy)
		{
			*dwDropEffect = DROPEFFECT_COPY;
		}
		else
		{
			*dwDropEffect = DROPEFFECT_MOVE;
		}
		GlobalUnlock(hGblEffect);


		DROPFILES dropFiles = {0};
		UINT uDropFilesLen = sizeof(DROPFILES);
		dropFiles.pFiles = uDropFilesLen;            
		dropFiles.pt.x = 0;
		dropFiles.pt.y = 0;
		dropFiles.fNC =FALSE;
		dropFiles.fWide =TRUE;                        

		//uBufLen * 2表示的是宽字符大小， 加8表示文件末尾需要2个空指针结尾，每个指针占4个字节大小
		UINT uGblLen = uDropFilesLen + uFilePathLen * 2 + 8;            
		HGLOBAL hGblFiles = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, uGblLen);
		char * szData = (char * )GlobalLock(hGblFiles);

		//把DROPFILES结构大小的内容放到szData剪切板空间的最开始
		memcpy(szData, (LPVOID)(&dropFiles), uDropFilesLen);

		//szFileList指向需要放入文件的那个空间，前面存放了DROPFILES结构大小的空间
		char * szFileList = szData + uDropFilesLen;
		memcpy((LPVOID)szFileList,(LPVOID)bstrFilePath.m_str,uFilePathLen*2);
		GlobalUnlock(hGblFiles);

		if( ::OpenClipboard(NULL) )
		{
			EmptyClipboard();

			//可以设置剪切板内容为拖动文件
			SetClipboardData(CF_HDROP, hGblFiles);

			//可以设置剪切板内容为复制或者剪切标识
			SetClipboardData(uDropEffect,hGblEffect);

			//关闭剪切板
			CloseClipboard();
			lua_pushboolean(pLuaState, 1);
		}
		else  
		{  
			::GlobalFree(hGblEffect);  
			::GlobalFree(hGblFiles);  
			lua_pushboolean(pLuaState, 0);
		} 
		return 1;
	}

	lua_pushboolean(pLuaState, 0);
	return 1;
}

int LuaAPIUtil::GetTempDir(lua_State* luaState)
{
	wchar_t wszTempDir[MAX_PATH] = {0};
	GetTempPath(MAX_PATH, wszTempDir);
	// 转码成utf-8格式
	wstring wstrTempDir = wszTempDir;
	string strTempDir = ultra::_T2UTF(wszTempDir);
	lua_pushstring(luaState,strTempDir.c_str());
	return 1;
}

int LuaAPIUtil::GetProfilesDir(lua_State* luaState)
{
	wstring wstrFilePath = g_pPathHelper->GetAllUserCfgDir();
	string strFilePath = ultra::_T2UTF(wstrFilePath);
	lua_pushstring(luaState,strFilePath.c_str());
	return 1;
}

int LuaAPIUtil::ForceUpdateWndShow(lua_State* luaState)
{
	HWND hWnd = (HWND)lua_touserdata(luaState, 2);
	SendMessage(hWnd, WM_TIMER, 1000, NULL);
	return 0;
}

int LuaAPIUtil::GetOSInfo(lua_State* luaState)
{
	wstring strOSDesc, strOSVersion;
	if (CImageUtility::GetOSInfo(strOSDesc, strOSVersion))
	{
		string strDescU8, strVersionU8;		
		strDescU8 = ultra::_T2UTF(strOSDesc);
		strVersionU8 = ultra::_T2UTF(strOSVersion);

		lua_pushstring(luaState, strDescU8.c_str());
		lua_pushstring(luaState, strVersionU8.c_str());
		return 2;
	}
	return 0;
}

int LuaAPIUtil::GetSystemRatio(lua_State* luaState)
{
	int nXScreen = GetSystemMetrics(SM_CXSCREEN);
	int nYScreen = GetSystemMetrics(SM_CYSCREEN);
	lua_pushinteger(luaState, nXScreen);
	lua_pushinteger(luaState, nYScreen);
	return 2;
}

int LuaAPIUtil::GetFolders(lua_State* luaState)
{
	wstring wstrFolderPath;
	const char* utf8FolderPath = luaL_checkstring(luaState, 2);

	wstrFolderPath = ultra::_UTF2T(utf8FolderPath);
	if(wstrFolderPath[wstrFolderPath.size()-1] != L'\\' && wstrFolderPath[wstrFolderPath.size()-1] != L'/')
	{
		wstrFolderPath.append(L"\\");
	}
	wstring wstrSearchFolderName = wstrFolderPath + L"*.*";


	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile(wstrSearchFolderName.c_str(), &FindFileData);

	int i = 0;
	lua_newtable(luaState);

	if(INVALID_HANDLE_VALUE != handle)
	{
		do 
		{
			if (FILE_ATTRIBUTE_DIRECTORY & FindFileData.dwFileAttributes) // 是文件夹
			{
				// 过滤掉隐藏的和系统属性的目录
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN || FindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
				{
					continue;
				}
				if ( (wcscmp(FindFileData.cFileName, L".") != 0) && (wcscmp(FindFileData.cFileName, L"..") != 0))
				{
					wstring wstrTempPath = wstrFolderPath + FindFileData.cFileName;
					//xl_data fileNode;
					lua_newtable(luaState);
					lua_pushstring(luaState, "FilePath");
					lua_pushstring(luaState, ultra::_T2UTF(wstrTempPath).c_str());
					lua_settable(luaState, -3);

					lua_pushstring(luaState, "FileName");
					lua_pushstring(luaState, ultra::_T2UTF(FindFileData.cFileName).c_str());
					lua_settable(luaState, -3);

					lua_rawseti(luaState, -2, i + 1);
					i++;
				}
			}
		} while(FindNextFile(handle , &FindFileData));

		CloseHandle(handle);
	}
	return 1;
}

int LuaAPIUtil::GetFiles(lua_State* luaState)
{
	wstring strFolderName;
	const char* utf8FilePath = luaL_checkstring(luaState, 2);

	strFolderName = ultra::_UTF2T(utf8FilePath);

	wstring wstrExtNameList;
	const char* utf8ExtNameList = lua_tolstring(luaState, 3, NULL);
	if (utf8ExtNameList)
	{
		wstrExtNameList = ultra::ToLower(ultra::_UTF2T(utf8ExtNameList));
	}

	strFolderName.append(L"\\");
	wstring wstrSearchFolderName = strFolderName;
	wstrSearchFolderName.append(L"*.*");

	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile(wstrSearchFolderName.c_str(), &FindFileData);

	int i = 0;
	lua_newtable(luaState);

	if (INVALID_HANDLE_VALUE != handle)
	{
		do 
		{
			if (FILE_ATTRIBUTE_DIRECTORY & FindFileData.dwFileAttributes)
			{
			}
			else
			{	
				// 过滤掉隐藏的和系统属性的目录
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
				{
					continue;
				}
				// 过滤掉不是自己想要的文件
				if (wstrExtNameList.empty())	// 用户没有传入了需要过滤的后缀名列表，直接用系统默认的
				{
					if (!CImageUtility::IsCanHandleFileCheckByExt(strFolderName + FindFileData.cFileName))
					{
						continue;
					}
				}
				else	// 用户传入了自己想要的后缀名列表
				{
					wstring wstrExtName = PathFindExtensionW(FindFileData.cFileName);
					if (wstrExtName.empty())	// 没有后缀名
					{
						continue;
					}
					wstring lowerExtName = ultra::ToLower(wstrExtName);
					lowerExtName.append(L";");
					if(wstrExtNameList.find(lowerExtName.c_str()) == wstring::npos)
					{
						continue;
					}
				}
				wstring wstrLastWriteTime;

				_SYSTEMTIME stSysTime;
				if (FileTimeToSystemTime(&(FindFileData.ftLastWriteTime), &stSysTime))
				{
					TCHAR* szTSysTime = new TCHAR[1000];
					swprintf(szTSysTime, L"%d-%02d-%02d %02d:%02d:%02d", stSysTime.wYear, stSysTime.wMonth, stSysTime.wDay, stSysTime.wHour, stSysTime.wMinute, stSysTime.wSecond);
					wstrLastWriteTime = szTSysTime;
					delete[] szTSysTime;
				}
				// 后缀名
				std::wstring wsExt = PathFindExtensionW((strFolderName + FindFileData.cFileName).c_str());
				if (wsExt.length() == 0)
				{
					wsExt = L"*";
				}

				// 文件大小
				long long llFileSize = FindFileData.nFileSizeHigh;
				llFileSize <<= 32;
				llFileSize += FindFileData.nFileSizeLow;
				char szFileSize[20] = {0};
				sprintf(szFileSize, "%d", llFileSize);

				lua_newtable(luaState);
				lua_pushstring(luaState, "FilePath");
				lua_pushstring(luaState, ultra::_T2UTF(strFolderName + FindFileData.cFileName).c_str());
				lua_settable(luaState, -3);

				lua_pushstring(luaState, "FileName");
				lua_pushstring(luaState, ultra::_T2UTF(FindFileData.cFileName).c_str());
				lua_settable(luaState, -3);

				lua_pushstring(luaState, "LastWriteTime");
				lua_pushstring(luaState, ultra::_T2UTF(wstrLastWriteTime).c_str());
				lua_settable(luaState, -3);

				lua_pushstring(luaState, "ExtName");
				lua_pushstring(luaState, ultra::_T2UTF(wsExt).c_str());
				lua_settable(luaState, -3);

				lua_pushstring(luaState, "FileSize");
				lua_pushstring(luaState, szFileSize);
				lua_settable(luaState, -3);


				lua_rawseti(luaState, -2, i + 1);
				i++;

			}		
		} while(FindNextFile(handle , &FindFileData));
		::FindClose(handle);
	}
	return 1;
}

int LuaAPIUtil::GetFileInfoByPath(lua_State* luaState)
{
	const char* utf8FilePath = luaL_checkstring(luaState, 2);
	wstring strFilePath = ultra::_UTF2T(utf8FilePath);
	//文件名
	wstring strFileName = L"";
	std::wstring::size_type nPos = strFilePath.find_last_of(L"\\");
	if( nPos != std::wstring::npos )
	{
		strFileName = strFilePath.substr(nPos+1);
	}
	//文件大小与修改时间
	wstring wstrLastWriteTime = L"";
	__int64 nFileSize = 0;
	HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER li;
		BOOL bRet = GetFileSizeEx(hFile,&li);


		FILETIME ftLastWriteTime;
		if (0 != GetFileTime(hFile, NULL, NULL, &ftLastWriteTime))
		{
			_SYSTEMTIME stSysTime;
			if (FileTimeToSystemTime(&(ftLastWriteTime), &stSysTime))
			{
				TCHAR* szTSysTime = new TCHAR[1000];
				swprintf(szTSysTime, L"%d-%02d-%02d %02d:%02d:%02d", stSysTime.wYear, stSysTime.wMonth, stSysTime.wDay, stSysTime.wHour, stSysTime.wMinute, stSysTime.wSecond);
				wstrLastWriteTime = szTSysTime;
				delete[] szTSysTime;
			}
		}
		CloseHandle( hFile );
		nFileSize = li.QuadPart;
	}
	char szFileSize[20] = {0};
	sprintf(szFileSize, "%d", nFileSize);

	//后缀名
	std::wstring wsExt = PathFindExtensionW(strFilePath.c_str());
	if (wsExt.length() == 0)
	{
		wsExt = L"*";
	}
	lua_newtable(luaState);
	lua_pushstring(luaState, "FilePath");
	lua_pushstring(luaState, utf8FilePath);
	lua_settable(luaState, -3);

	lua_pushstring(luaState, "FileName");
	lua_pushstring(luaState, ultra::_T2UTF(strFileName).c_str());
	lua_settable(luaState, -3);

	lua_pushstring(luaState, "LastWriteTime");
	lua_pushstring(luaState, ultra::_T2UTF(wstrLastWriteTime).c_str());
	lua_settable(luaState, -3);

	lua_pushstring(luaState, "ExtName");
	lua_pushstring(luaState, ultra::_T2UTF(wsExt).c_str());
	lua_settable(luaState, -3);

	lua_pushstring(luaState, "FileSize");
	lua_pushstring(luaState, szFileSize);
	lua_settable(luaState, -3);

	return 1;
}

int LuaAPIUtil::StrColl(lua_State* luaState)
{
	const char* pszstr1_utf8 = luaL_checkstring(luaState, 2);
	const char* pszstr2_utf8 = luaL_checkstring(luaState, 3);

	wstring str1_unicode = ultra::_UTF2T(string(pszstr1_utf8));
	wstring str2_unicode = ultra::_UTF2T(string(pszstr2_utf8));

	int cmpare_result = StrCmpLogicalW(str1_unicode.c_str(), str2_unicode.c_str());
	lua_pushinteger(luaState, cmpare_result);
	return 1;
}

int LuaAPIUtil::GetScreenRatio(lua_State* luaState)
{
	int nXScreen = GetSystemMetrics(SM_CXSCREEN);
	int nYScreen = GetSystemMetrics(SM_CYSCREEN);
	lua_pushinteger(luaState, nXScreen);
	lua_pushinteger(luaState, nYScreen);
	return 2;
}

int LuaAPIUtil::IsPathFileExist(lua_State* luaState)
{
	const char* utf8FilePath = luaL_checkstring(luaState, 2);
	wstring wstrFilePath;
	wstrFilePath = ultra::_UTF2T(utf8FilePath);
	BOOL bExist = PathFileExists(wstrFilePath.c_str());
	lua_pushboolean(luaState, bExist);
	return 1;
}

int LuaAPIUtil::LogToFile(lua_State* luaState)
{
	wstring wstrLogInfo;
	const char* utf8LogInfo = luaL_checkstring(luaState, 2);
	wstrLogInfo = ultra::_UTF2T(utf8LogInfo);
	TSINFO4CXX(L"Lua Log: "<< wstrLogInfo.c_str());
	return 0;
}

int LuaAPIUtil::GetWorkAreaSize(lua_State* luaState)
{
	RECT rect; 
	::SystemParametersInfo( SPI_GETWORKAREA, sizeof(RECT), &rect, 0 ); 
	lua_pushinteger(luaState, rect.right-rect.left);
	lua_pushinteger(luaState, rect.bottom-rect.top);
	return 2;
}

int LuaAPIUtil::GetMd5Str(lua_State* luaState)
{
	// 获取要计算md5值的字符串
	const char* utf8;
	utf8 = luaL_checkstring(luaState, 2);
	string strValue = utf8;

	wchar_t pszMD5[MAX_PATH] = {0};
	if (GetStringMd5(strValue,pszMD5))
	{
		std::string utf8MD5 = ultra::_T2UTF(pszMD5);

		lua_pushstring(luaState, utf8MD5.c_str());
		return 1;
	}

	return 0;
}

int LuaAPIUtil::MonitorDirChange(lua_State* luaState)
{
	const char* utf8DirPath = luaL_checkstring(luaState, 2);
	wstring wstrDirPath = ultra::_UTF2T(utf8DirPath);
	CFolderChangeMonitor* pMonitor = CFolderChangeMonitor::Instance();
	long cookie = pMonitor->MonitorDirChange(wstrDirPath);
	lua_pushinteger(luaState, cookie);
	return 1;
}

int LuaAPIUtil::UnMonitorDirChange(lua_State* luaState)
{
	long lCookie = lua_tointeger(luaState, 2);
	CFolderChangeMonitor* pMonitor = CFolderChangeMonitor::Instance();
	pMonitor->UnMonitorDirChange(lCookie);
	return 0;
}


int LuaAPIUtil::AttachDirChangeEvent(lua_State* luaState)
{	
	CFolderChangeMonitor* pMonitor = CFolderChangeMonitor::Instance();
	DWORD cookie;
	HRESULT hret = pMonitor->AttachDirChangeEvent(luaState, 2, cookie);
	if (SUCCEEDED(hret))
	{
		lua_pushinteger(luaState, cookie);
		return 1;
	}
	return 0;
}
int LuaAPIUtil::DetachDirChangeEvent(lua_State* luaState)
{
	CFolderChangeMonitor* pMonitor = CFolderChangeMonitor::Instance();
	DWORD cookie;
	cookie = lua_tointeger(luaState, 2);
	HRESULT hret = pMonitor->DetachDirChangeEvent(cookie);
	if (SUCCEEDED(hret))
	{
		lua_pushboolean(luaState, true);
		return 1;
	}
	return 0;
}

int LuaAPIUtil::InitFolderMonitor(lua_State* luaState)
{
	CFolderChangeMonitor* pMonitor = CFolderChangeMonitor::Instance();
	if (pMonitor)
	{
		if(pMonitor->Init())
		{
			lua_pushboolean(luaState, 1);
			return 1;
		}
	}
	lua_pushboolean(luaState, 0);
	return 1;
}

int LuaAPIUtil::IsCanHandleFileCheckByExt(lua_State* luaState)
{
	const char* utf8 = luaL_checkstring(luaState, 2);
	wstring wstrFilePath;
	wstrFilePath = ultra::_UTF2T(utf8);
	if (CImageUtility::IsCanHandleFileCheckByExt(wstrFilePath))
	{
		lua_pushboolean(luaState, true);
	}
	else
	{
		lua_pushboolean(luaState, false);
	}
	return 1;
}
int LuaAPIUtil::DelPathFile2RecycleBin(lua_State* luaState)
{
	const char* utf8FilePath = luaL_checkstring(luaState, 2);

	wstring wstrFilePath;
	wstrFilePath = ultra::_UTF2T(utf8FilePath);
	wchar_t wcsFilePath[1024];
	wmemset(wcsFilePath, 0, 1024);
	wsprintf(wcsFilePath, L"%s", wstrFilePath.c_str());

	SHFILEOPSTRUCT shFOP;
	memset(&shFOP, 0, sizeof(shFOP));
	shFOP.hwnd = NULL;
	shFOP.wFunc = FO_DELETE;
	shFOP.pFrom = wcsFilePath;
	shFOP.pTo = NULL;
	shFOP.fFlags = FOF_ALLOWUNDO | FOF_NOERRORUI | FOF_NOCONFIRMATION;
	shFOP.fAnyOperationsAborted = FALSE;
	SHFileOperation(&shFOP);
	return 0;
}

int LuaAPIUtil::PrintImage(lua_State* luaState)
{
	const char* utf8 = luaL_checkstring(luaState, 2);
	wstring wstrFilePath;
	wstring wstrFileName;
	wstrFilePath = ultra::_UTF2T(utf8);

	wchar_t wszPath[512] = {0};
	wcscpy(wszPath, wstrFilePath.c_str());
	PathRemoveFileSpec(wszPath);
	wstrFileName = ::PathFindFileName(wstrFilePath.c_str());
	CImageUtility::PrintImage(wszPath, (wchar_t*)wstrFileName.c_str());
	return 0;
}

int LuaAPIUtil::GetExeFileDes(lua_State* luaState)
{
	const char* utf8 = luaL_checkstring(luaState, 2);
	wstring wstrFilePath = ultra::_UTF2T(utf8);
	wstring wstrFileDes = CImageUtility::GetFileDesInfo(wstrFilePath);
	string strFileDes = ultra::_T2UTF(wstrFileDes);
	lua_pushstring(luaState, strFileDes.c_str());
	return 1;
}

int LuaAPIUtil::CopyFilePathToCLipBoard(lua_State* luaState)
{
	const char* utf8 = luaL_checkstring(luaState, 2);
	wstring wstrFilePath = ultra::_UTF2T(utf8);
	UINT uDropEffect;
	DROPFILES dropFiles;
	UINT uGlbLen, uDropFIlesLen;

	uDropEffect = RegisterClipboardFormat(L"Preferred DropEffect");
	HGLOBAL hGblEffect = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(DWORD));
	DWORD *dwDropEffect = (DWORD*)GlobalLock(hGblEffect);
	*dwDropEffect = DROPEFFECT_COPY;
	GlobalUnlock(hGblEffect);

	uDropFIlesLen = sizeof(DROPFILES);
	dropFiles.pFiles = uDropFIlesLen;
	dropFiles.pt.x = 0;
	dropFiles.pt.y = 0;
	dropFiles.fNC = FALSE;
	dropFiles.fWide = TRUE;

	uGlbLen = uDropFIlesLen+wstrFilePath.size()*2 + 8;
	HGLOBAL hGblFiles = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, uGlbLen);
	BYTE* szData=(BYTE*)GlobalLock(hGblFiles);
	memcpy(szData, (LPVOID)(&dropFiles), uDropFIlesLen);
	wchar_t* wszFileList = (wchar_t*)(szData + uDropFIlesLen);
	wcscpy(wszFileList, wstrFilePath.c_str());
	GlobalUnlock(hGblFiles);

	if (OpenClipboard(NULL))
	{
		EmptyClipboard();
		SetClipboardData(CF_HDROP, hGblFiles);
		SetClipboardData(uDropEffect, hGblEffect);
		CloseClipboard();
	}
	return 0;
}

int LuaAPIUtil::CopyImageToClipboard(lua_State* luaState)
{
	XL_BITMAP_HANDLE hBitmap = NULL;
	if(XLGP_CheckBitmap(luaState, 2, &hBitmap))
	{
		XLBitmapInfo SrcBmpInfo;
		XL_GetBitmapInfo(hBitmap, &SrcBmpInfo);
		unsigned long nWidth = SrcBmpInfo.Width;
		unsigned long nHeight = SrcBmpInfo.Height;



		//还得在白色的底上融合一下，因为复制出去的图不能带alpha信息
		XL_BITMAP_HANDLE hDstBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, nWidth, nHeight);
		assert(hDstBitmap);
		if (hDstBitmap)
		{
			XL_FillBitmap(hDstBitmap, XLCOLOR_BGRA(255, 255, 255, 255));
			XL_PreMultiplyBitmap(hBitmap); //混合前要预乘
			XLGP_SrcAlphaBlend(hDstBitmap, 0, 0, hBitmap, 255);
			XL_ReleaseBitmap(hBitmap);
			hBitmap = hDstBitmap;
		}
		//开始复制操作
		BITMAPINFO bmpInfo;
		memset(&bmpInfo, 0, sizeof(BITMAPINFO));
		bmpInfo.bmiHeader.biHeight = nHeight; //
		bmpInfo.bmiHeader.biWidth = nWidth;
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biBitCount = 32;
		bmpInfo.bmiHeader.biCompression = BI_RGB;
		bmpInfo.bmiHeader.biSizeImage = 0;
		bmpInfo.bmiHeader.biXPelsPerMeter = 0;
		bmpInfo.bmiHeader.biYPelsPerMeter = 0;
		bmpInfo.bmiHeader.biClrUsed = 0;
		bmpInfo.bmiHeader.biClrImportant = 0;

		BOOL ret = ::OpenClipboard(NULL);
		if (ret == FALSE)
		{
			XL_ReleaseBitmap(hBitmap);
			return 0;
		}
		ret = ::EmptyClipboard();
		if (ret == FALSE)
		{
			XL_ReleaseBitmap(hBitmap);
			::CloseClipboard();
			return 0;
		}

		HGLOBAL hDIB = ::GlobalAlloc(GHND | GMEM_SHARE, sizeof(BITMAPINFOHEADER) + SrcBmpInfo.ScanLineLength * SrcBmpInfo.Height);
		if (hDIB == NULL)
		{
			XL_ReleaseBitmap(hBitmap);
			::CloseClipboard();
			return 0;
		}

		BYTE* lpBuffer = (BYTE*)::GlobalLock(hDIB);
		assert(lpBuffer);
		if (lpBuffer == NULL)
		{
			XL_ReleaseBitmap(hBitmap);
			::GlobalFree(hDIB);
			::CloseClipboard();
			return 0;
		}

		*((LPBITMAPINFO)lpBuffer) = bmpInfo;
		//注意，千万注意，下面这句不是一般的坑爹。MSDN里面说，传到剪贴板里面的内存块应该包含BITMAPINFO结构和像素数据，而实际上在这里，当我们没有使用调色板信息的时候，
		//我们应该只传入一个BITMAPINFOHEADER结构和像素数据，也就是说，比MSDN上说的要少传入4个像素！！！不然像素数据值有偏差了
		BYTE* pBitBuf = lpBuffer + sizeof(BITMAPINFOHEADER); //这里不能是sizeof(BITMAPINFO）
		BYTE* pSrcImageBuf = XL_GetBitmapBuffer(hBitmap, 0, 0);
		for (unsigned long j = 0; j < SrcBmpInfo.Height; j++)
		{
			memcpy(pBitBuf + SrcBmpInfo.ScanLineLength*j, pSrcImageBuf + SrcBmpInfo.ScanLineLength*(SrcBmpInfo.Height - 1 - j), SrcBmpInfo.ScanLineLength);
		}
		::GlobalUnlock(hDIB);

		HANDLE hHandle = ::SetClipboardData(CF_DIB, hDIB);
		if (hHandle == NULL)
		{
			XL_ReleaseBitmap(hBitmap);
			::GlobalFree(hDIB);
			::CloseClipboard();
			return 0;
		}
		XL_ReleaseBitmap(hBitmap);
		::CloseClipboard();
		::GlobalFree(hDIB);
	}
	return 0;
}

BOOL IS_Vista_Or_More()
{
	OSVERSIONINFO osver;
	ZeroMemory( &osver, sizeof( osver ) );
	osver.dwOSVersionInfoSize = sizeof( osver );

	if( !GetVersionEx (&osver) )
	{
		osver.dwOSVersionInfoSize = sizeof (osver);
		if (! GetVersionEx ( &osver) ) 
			return FALSE;
	}
	return osver.dwMajorVersion >= 6;
	//return false;
}

typedef HRESULT (STDAPICALLTYPE*SHCreateItemFromParsingNameFuncType)(
	_In_   PCWSTR pszPath,
	_In_   IBindCtx *pbc,
	_In_   REFIID riid,
	_Out_  void **ppv
	);

SHCreateItemFromParsingNameFuncType GetFunctionAddress()
{
	HMODULE handle = ::LoadLibraryW( L"SHELL32.dll" );
	if( handle == NULL )
		return NULL;
	return (SHCreateItemFromParsingNameFuncType)::GetProcAddress( handle, "SHCreateItemFromParsingName" );
}
int LuaAPIUtil::KKFolderDialog(lua_State* luaState)
{
	std::string utf8Title;
	if (lua_type(luaState, 2) == LUA_TSTRING)
	{
		utf8Title = luaL_checkstring(luaState,2);
	}
	std::string utf8InitialFolder;
	if (lua_type(luaState, 3) == LUA_TSTRING)
	{
		utf8InitialFolder = luaL_checkstring(luaState,3);
	}
	std::string strPath;

	std::wstring wstrTitle = ultra::_UTF2T(utf8Title);
	std::wstring wstrInitialFolder =  ultra::_UTF2T(utf8InitialFolder);
	// 判断目录是否存在不存时指向上一级目录
	while( !PathFileExists(wstrInitialFolder.c_str()) )
	{
		std::wstring::size_type nPos = wstrInitialFolder.find_last_of(L"\\");
		if( nPos != std::wstring::npos )
		{
			wstrInitialFolder = wstrInitialFolder.substr(0,nPos);
		}
		else
		{
			break;
		}
	}
	//根目录如果没有"\"会不能定位到根目录．
	if(wstrInitialFolder.length() == 0 || wstrInitialFolder[wstrInitialFolder.length() - 1] != L'\\')
		wstrInitialFolder += L"\\";

	if( IS_Vista_Or_More() )
	{
		BOOL is_check = FALSE;
		IFileOpenDialog *pfd = NULL;
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
		if (SUCCEEDED(hr))
		{
			pfd->SetTitle( wstrTitle.c_str() );
			IShellItem* pItem = NULL;
			SHCreateItemFromParsingNameFuncType funcPtr = GetFunctionAddress();
			if( funcPtr )
			{
				hr = funcPtr( wstrInitialFolder.c_str(), NULL,IID_PPV_ARGS(&pItem));
				if (SUCCEEDED(hr))
					pfd->SetFolder( pItem );
			}
			DWORD dwFlags;
			hr = pfd->GetOptions(&dwFlags);
			if (SUCCEEDED(hr))
			{
				dwFlags |= FOS_PICKFOLDERS;
				hr = pfd->SetOptions( dwFlags );//FOS_FORCEFILESYSTEM FOS_PICKFOLDERS FOS_HIDEPINNEDPLACES  FOS_ALLOWMULTISELECT 
				if (SUCCEEDED(hr))
				{
					IFileDialogCustomize *pfdc = NULL;
					hr = pfd->QueryInterface(IID_PPV_ARGS(&pfdc));
					if( SUCCEEDED(hr) )
					{
						const DWORD check_button_id = 999;
						hr = pfdc->AddCheckButton( check_button_id,L"应用到子文件夹", FALSE );
						if( SUCCEEDED(hr) )
						{
							hr = pfd->Show( GetActiveWindow());
							if (SUCCEEDED(hr))
							{
								IShellItem *psiResult;
								hr = pfd->GetResult(&psiResult);
								if (SUCCEEDED(hr))
								{	
									PWSTR pszFilePath = NULL;
									hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
									if (SUCCEEDED(hr))
									{
										strPath = ultra::_T2UTF(pszFilePath);
									}
									psiResult->Release();
								}
								pfdc->GetCheckButtonState( check_button_id, &is_check );
							}
						}
						pfdc->Release();
					}
				}
			}
			if( pItem )
				pItem->Release();
			pfd->Release();
		}
		lua_pushstring(luaState, strPath.c_str() );
		lua_pushboolean(luaState, is_check );
	}
	else
	{
		CKKFolderDialog dlg(0, wstrTitle.c_str(), BIF_RETURNONLYFSDIRS|BIF_USENEWUI);
		dlg.SetInitialFolder(wstrInitialFolder.c_str(), true);
		if (IDOK == dlg.DoModal())
		{
			strPath = ultra::_T2UTF(dlg.GetFolderPath());
		}
		lua_pushstring(luaState, strPath.c_str());
		lua_pushboolean(luaState, dlg.GetCheckState());
	}
	return 2;
}

int LuaAPIUtil::CopyFileTo(lua_State* luaState)
{
	const char* utf8FilePath = luaL_checkstring(luaState, 2);
	const char* utf8FolderPath = luaL_checkstring(luaState, 3);
	wstring wstrFilePath, wstrFolderPath;
	wstrFilePath = ultra::_UTF2T(utf8FilePath);
	wstrFolderPath = ultra::_UTF2T(utf8FolderPath);

	wchar_t wcsFilePath[1024];
	wmemset(wcsFilePath, 0, 1024);
	wsprintf(wcsFilePath, L"%s", wstrFilePath.c_str());

	SHFILEOPSTRUCT shFOP;
	memset(&shFOP, 0, sizeof(shFOP));
	shFOP.hwnd = NULL;
	shFOP.wFunc = FO_COPY;
	shFOP.pFrom = wcsFilePath;
	shFOP.pTo = wstrFolderPath.c_str();
	shFOP.fFlags = FOF_SIMPLEPROGRESS;
	shFOP.fAnyOperationsAborted = FALSE;
	SHFileOperation(&shFOP);
	return 0;
}
int LuaAPIUtil::MoveFileTo(lua_State* luaState)
{
	const char* utf8FilePath = luaL_checkstring(luaState, 2);
	const char* utf8FolderPath = luaL_checkstring(luaState, 3);
	wstring wstrFilePath, wstrFolderPath;
	wstrFilePath = ultra::_UTF2T(utf8FilePath);
	wstrFolderPath = ultra::_UTF2T(utf8FolderPath);

	wchar_t wcsFilePath[1024];
	wmemset(wcsFilePath, 0, 1024);
	wsprintf(wcsFilePath, L"%s", wstrFilePath.c_str());

	SHFILEOPSTRUCT shFOP;
	memset(&shFOP, 0, sizeof(shFOP));
	shFOP.hwnd = NULL;
	shFOP.wFunc = FO_MOVE;
	shFOP.pFrom = wcsFilePath;
	shFOP.pTo = wstrFolderPath.c_str();
	shFOP.fFlags = FOF_SIMPLEPROGRESS;
	shFOP.fAnyOperationsAborted = FALSE;
	SHFileOperation(&shFOP);
	return 0;
}

int LuaAPIUtil::RenameFile(lua_State* luaState)
{
	const char* utf8FilePath = luaL_checkstring(luaState, 2);
	const char* utf8FolderPath = luaL_checkstring(luaState, 3);
	wstring wstrFilePath, wstrFolderPath;
	wstrFilePath = ultra::_UTF2T(utf8FilePath);
	wstrFolderPath = ultra::_UTF2T(utf8FolderPath);

	wchar_t wcsFilePath[1024];
	wmemset(wcsFilePath, 0, 1024);
	wsprintf(wcsFilePath, L"%s", wstrFilePath.c_str());

	SHFILEOPSTRUCT shFOP;
	memset(&shFOP, 0, sizeof(shFOP));
	shFOP.hwnd = NULL;
	shFOP.wFunc = FO_RENAME;
	shFOP.pFrom = wcsFilePath;
	shFOP.pTo = wstrFolderPath.c_str();
	shFOP.fFlags = FOF_SIMPLEPROGRESS;
	shFOP.fAnyOperationsAborted = FALSE;
	SHFileOperation(&shFOP);
	return 0;
}

int LuaAPIUtil::SHChangeNotify(lua_State* luaState)
{
	::SHChangeNotify(SHCNE_ASSOCCHANGED,SHCNF_FLUSHNOWAIT,0,0);
	return 0;
}

int LuaAPIUtil::GetCurveProgress(lua_State* luaState)
{
	float fValue = lua_tonumber(luaState, 2);
	XL_CURVE_HANDLE hCurve = XL_CreateCurve(XLCURVE_BEZIER);
	XL_AddCurveControlPoint(hCurve, 0, 0);
	XL_AddCurveControlPoint(hCurve, 0.085165, 0.808989);
	XL_AddCurveControlPoint(hCurve, 0.725275, 0.862360);
	XL_AddCurveControlPoint(hCurve, 1.0, 1.0);
	float fDes = XL_GetCurveProgress(hCurve, fValue);
	XL_ReleaseCurve(hCurve);
	lua_pushnumber(luaState, fDes*1000);
	return 1;
}

int LuaAPIUtil::GetIconFromExeFile(lua_State* luaState)
{
	const char* utf8Str = luaL_checkstring(luaState, 2);
	wstring wstrExePath = ultra::_UTF2T(utf8Str);
	HICON hIcon = ::ExtractIcon((HINSTANCE)g_hInst, wstrExePath.c_str(), 2);
	if (hIcon != NULL)
	{
		HBITMAP hBitmap;
		BITMAP   bmp;
		CDC   bmpDC, iconDC;
		ICONINFO iconInfo;

		BOOL bRet = ::GetIconInfo(hIcon, &iconInfo);
		if (bRet)
		{
			bmpDC.CreateCompatibleDC(NULL);
			iconDC.CreateCompatibleDC(bmpDC); 
			if (::GetObject(iconInfo.hbmColor,   sizeof(BITMAP),   &bmp))
			{
				DWORD dwWidth = iconInfo.xHotspot*2;     
				DWORD dwHeight = iconInfo.yHotspot*2; 
				hBitmap = ::CreateBitmap(dwWidth, dwHeight, bmp.bmPlanes, bmp.bmBitsPixel, NULL);
				if (hBitmap)
				{
					iconDC.SelectBitmap(iconInfo.hbmColor);     
					bmpDC.SelectBitmap(hBitmap);
					bmpDC.BitBlt(0, 0, dwWidth, dwHeight, iconDC, 0, 0, SRCCOPY);
					XL_BITMAP_HANDLE hXLBitmap = CImageProcessor::ConvertDIBToXLBitmap32(hBitmap);
					// 关闭
					//DeleteObject(hBitmap);

					if (hXLBitmap)
					{
						XL_PreMultiplyBitmap(hXLBitmap);
						XLGP_PushBitmap(luaState, hXLBitmap);
						XL_ReleaseBitmap(hXLBitmap);
						XLBitmapInfo bmpInfo;
						XL_GetBitmapInfo(hXLBitmap, &bmpInfo);
						lua_pushnumber(luaState, bmpInfo.Width);
						lua_pushnumber(luaState, bmpInfo.Height);
						return 3;
					}
				}
			}
		}
		::DestroyIcon(hIcon);
	}
	return 0;
}

int LuaAPIUtil::IsCanSetToWallPaperFile(lua_State* luaState)
{
	bool bRet = false;
	const char* utf8FilePath = luaL_checkstring(luaState, 2);
	wstring wstrFilePath = ultra::_UTF2T(utf8FilePath);
	wstring wstrExtName = PathFindExtensionW(wstrFilePath.c_str());
	if (!wstrExtName.empty())
	{
		wstring lowerExtName = ultra::ToLower(wstrExtName);
		lowerExtName.append(L";");
		wstring wstrCanSuperBatchFileExtList = CanSetToWallPaperFileExt;
		if(wstrCanSuperBatchFileExtList.find(lowerExtName.c_str()) != wstring::npos)
		{
			bRet = true;
		}
	}
	lua_pushboolean(luaState, bRet);
	return 1;
}

int LuaAPIUtil::CopyTextToClipboard(lua_State* luaState)
{
	TSAUTO();
	std::string strClipText = lua_tostring(luaState, 2);

	std::wstring wstrClipText = ultra::_UTF2T(strClipText);
	TSINFO4CXX("wstrClipText:"<<wstrClipText<<"wstrClipText.size():"<<wstrClipText.size());

	HGLOBAL hDIB = ::GlobalAlloc(GHND | GMEM_SHARE, (wstrClipText.size() + 1) * sizeof(wchar_t));
	if (hDIB == NULL)
	{
		TSINFO4CXX("GlobalAlloc Failed!");
		return 0;
	}

	wchar_t* lpBuffer = (wchar_t*)::GlobalLock(hDIB);
	assert(lpBuffer);
	if (lpBuffer == NULL)
	{
		TSINFO4CXX("GlobalLock Failed!");
		::GlobalFree(hDIB);
		return 0;
	}

	lstrcpy(lpBuffer, wstrClipText.c_str());

	::GlobalUnlock(hDIB);

	BOOL ret = ::OpenClipboard(NULL);
	if (ret == FALSE)
	{
		TSINFO4CXX("OpenClipboard Failed! Error:"<<::GetLastError());
		return 0;
	}
	::EmptyClipboard();

	HANDLE hHandle = ::SetClipboardData(CF_UNICODETEXT, hDIB);
	TSINFO4CXX("hHandle:"<<hHandle);
	if (hHandle == NULL)
	{
		TSINFO4CXX("SetClipboardData Failed! Error:"<<::GetLastError());
		return 0;
	}

	::CloseClipboard();
	return 0;
}

int LuaAPIUtil::ShowColorDialog(lua_State* luaState)
{
	TSAUTO();
	int nInitRed = luaL_checkint(luaState, 2);
	int nInitGreen = luaL_checkint(luaState, 3);
	int nInitBlue = luaL_checkint(luaState, 4);

	HWND parentWnd = (HWND)lua_touserdata(luaState, 5);

	COLORREF color = RGB(nInitRed, nInitGreen, nInitBlue);
	CColorDialog cdlg(color, CC_FULLOPEN | CC_RGBINIT, parentWnd);

	if(cdlg.DoModal() == IDOK)
	{
		color = cdlg.GetColor();

		int nRed = GetRValue(color);
		int nGreen = GetGValue(color);
		int nBlue = GetBValue(color);

		lua_pushinteger(luaState, nRed);
		lua_pushinteger(luaState, nGreen);
		lua_pushinteger(luaState, nBlue);

		return 3;
	}
	else
	{
		//如果点击的是取消，那么什么也不做
		return 0;
	}
}

int LuaAPIUtil::SetShowCursor( lua_State* luaState )
{
	bool bShow = LuaGetBool(luaState,2);
	ShowCursor(bShow);
	return 0;
}




/*Shell*/
int LuaAPIUtil::GetStringLength(lua_State* luaState)
{

	const std::string utfstr = luaL_checkstring(luaState, 2);
	std::wstring unicodetext = ultra::_UTF2T(utfstr);
	long length = unicodetext.length();
	lua_pushnumber(luaState, length);
	return 1;
}

int LuaAPIUtil::IsClipboardTextFormatAvailable( lua_State* luaState )
{
	bool bRet = IsClipboardFormatAvailable( CF_TEXT )?true:false;
	lua_pushboolean( luaState, bRet);
	return 1;
}

int LuaAPIUtil::RegisterHotKeyEx( lua_State* luaState )
{

	HWND hWnd = (HWND)lua_touserdata(luaState,2);
	DWORD id = lua_tointeger(luaState,3);
	UINT fsModifiers = lua_tointeger(luaState,4);
	UINT vk = lua_tointeger(luaState,5);
	bool bRet = RegisterHotKey( hWnd, id, fsModifiers, vk )?true:false;
	lua_pushboolean( luaState, bRet);
	return 1;
}

int LuaAPIUtil::UnregisterHotKeyEx( lua_State* luaState )
{
	HWND hWnd = (HWND)lua_touserdata(luaState,2);
	DWORD id = lua_tointeger(luaState,3);
	bool bRet = UnregisterHotKey( hWnd, id ) ?true:false;
	lua_pushboolean( luaState, bRet);
	return 1;
}

int LuaAPIUtil::GlobalAddAtomEx( lua_State* luaState )
{
	const char* lpName = luaL_checkstring( luaState, 2 );

	wstring strName = ultra::_UTF2T(lpName);
	lua_pushinteger( luaState, GlobalAddAtom( strName.c_str() ));
	return 1;
}

int LuaAPIUtil::GlobalDeleteAtomEx( lua_State* luaState )
{
	DWORD id = lua_tointeger( luaState, 2 );
	GlobalDeleteAtom( (ATOM)id );
	return 0;
}

int LuaAPIUtil::GetKeyState( lua_State* luaState )
{
	int nVirtKey = lua_tointeger(luaState,2);
	lua_pushinteger( luaState, (SHORT)::GetKeyState( nVirtKey ));
	return 1;
}

int LuaAPIUtil::Execute(lua_State* luaState)
{

	const char* utf8AppPath = luaL_checkstring(luaState,2);
	const char* utf8Param = luaL_checkstring(luaState,3);
	const char* utf8StartDir = luaL_checkstring(luaState,4);

	if(utf8AppPath == NULL)
	{
		lua_pushinteger(luaState,1);
		return 1;
	}

	wstring strAppPath;
	wstring strParam;
	wstring strStartDir;
	if(utf8AppPath)
	{
		strAppPath = ultra::_UTF2T(utf8AppPath);
	}
	if(utf8Param)
	{
		strParam = ultra::_UTF2T(utf8Param);
	}
	if(utf8StartDir)
	{
		strStartDir = ultra::_UTF2T(utf8StartDir);
	}

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	long result = 1;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );
	if(strlen(utf8AppPath) == 0 && strlen(utf8StartDir) == 0)
	{
		if(!CreateProcess(NULL,(LPWSTR)strParam.c_str(),NULL,NULL,FALSE,0,NULL,NULL,&si,&pi))
		{
			result = 1;
		}
		else
		{
			result = 0;
		}
	}
	if(!CreateProcess(strAppPath.c_str(),(LPWSTR)strParam.c_str(),NULL,NULL,FALSE,0,NULL,strStartDir.c_str(),&si,&pi))
	{
		result = 1;
	}

	lua_pushinteger(luaState,result);
	return 1;
}

int LuaAPIUtil::IsValidFileName(lua_State* luaState)
{

	const char* utf8FileName = luaL_checkstring(luaState, 2);
	std::wstring wstrFileName = ultra::_UTF2T(utf8FileName);
	long lRet = 1;
	if (std::wstring::npos == wstrFileName.find_first_of(L"\\/:?<>*|\""))
	{
		wstring wstrFileMainName = wstrFileName.substr(wstrFileName.find_last_of('\\')+1);
		if ( wstrFileMainName.empty() )
		{
			lRet = 0;
		}
		lRet = 1;
	}
	else
	{
		lRet = 0;
	}


	lua_pushboolean(luaState, lRet);
	return 1;

	return 0;
}

int LuaAPIUtil::ShellOpen(lua_State* luaState)
{
	const char* param = luaL_checkstring(luaState,2);

	if(param == NULL)
		return 0;

	wstring strParam = ultra::_UTF2T(param);

	DWORD dwRet = (DWORD)ShellExecute(NULL, L"open", strParam.c_str(), NULL, NULL, SW_NORMAL);
	if (dwRet > 32)
	{
		return 0;
	}
	else if (dwRet == ERROR_FILE_NOT_FOUND || dwRet == ERROR_PATH_NOT_FOUND)
	{
		return 0;
	}
	else if ( dwRet == SE_ERR_NOASSOC )
	{
		ShellExecute(0, _T("open"), _T("rundll32.exe"), wstring( wstring (L"Shell32.dll,OpenAs_RunDLL ") + strParam ).c_str(), NULL,SW_SHOWNORMAL);
	}
	return 0;
}

int LuaAPIUtil::SetDesktopWallpaper(lua_State* luaState)
{
	const char* utf8Text = luaL_checkstring(luaState, 2);

	if (utf8Text == NULL)
	{
		return E_FAIL;
	}
	HRESULT hr = S_OK;
	CComPtr<IActiveDesktop> spActiveDesktop;
	hr = CoCreateInstance(CLSID_ActiveDesktop, NULL, CLSCTX_INPROC_SERVER, IID_IActiveDesktop, (void**)&spActiveDesktop);
	if (hr == S_OK)
	{
		COMPONENTSOPT comp;
		comp.dwSize = sizeof(comp);
		comp.fEnableComponents = TRUE;
		comp.fActiveDesktop = TRUE;
		hr = spActiveDesktop->SetDesktopItemOptions(&comp, 0);
		if (hr != S_OK)
		{
			//win7下该函数没有实现
			//return hr;
		}
		std::wstring strFileName = ultra::_UTF2T(utf8Text);
		hr = spActiveDesktop->SetWallpaper(strFileName.c_str(), 0);
		WALLPAPEROPT wpo;
		wpo.dwSize = sizeof(wpo);
		wpo.dwStyle = WPSTYLE_STRETCH;
		hr = spActiveDesktop->SetWallpaperOptions(&wpo, 0);
		if (hr != S_OK)
		{
			return hr;
		}
		hr = spActiveDesktop->ApplyChanges(AD_APPLY_ALL);
	}
	lua_pushinteger(luaState, hr);
	return 1;
}



int LuaAPIUtil::FolderDialog(lua_State* luaState)
{
	std::string utf8Title;
	if (lua_type(luaState, 2) == LUA_TSTRING)
	{
		utf8Title = luaL_checkstring(luaState,2);
	}
	std::string utf8InitialFolder;
	if (lua_type(luaState, 3) == LUA_TSTRING)
	{
		utf8InitialFolder = luaL_checkstring(luaState,3);
	}
	std::string strPath;

	std::wstring wstrTitle = ultra::_UTF2T(utf8Title);
	WTL::CFolderDialog dlg(0, wstrTitle.c_str(), BIF_RETURNONLYFSDIRS|BIF_USENEWUI);

	// 判断目录是否存在不存时指向上一级目录
	std::string strInitialFolder2 = utf8InitialFolder;
	while( !PathFileExistsA(strInitialFolder2.c_str()) )
	{
		std::string::size_type nPos = strInitialFolder2.find_last_of("\\");
		if( nPos != std::string::npos )
		{
			strInitialFolder2 = strInitialFolder2.substr(0,nPos);
		}
		else
		{
			break;
		}
	}
	//根目录如果没有"\"会不能定位到根目录．
	if(strInitialFolder2.length() == 0 || strInitialFolder2[strInitialFolder2.length() - 1] != '\\')
		strInitialFolder2 += "\\";

	std::wstring wstrInitialFolder = ultra::_UTF2T(strInitialFolder2);
	dlg.SetInitialFolder(wstrInitialFolder.c_str(), true);

	if (IDOK == dlg.DoModal())
	{
		strPath = ultra::_T2UTF(dlg.GetFolderPath());
	}
	lua_pushstring(luaState, strPath.c_str());
	return 1;
}


int LuaAPIUtil::FileDialog(lua_State* luaState)
{
	BOOL bOpenFileDialog = lua_toboolean(luaState, 2);
	const char* pszFilter = luaL_checkstring(luaState, 3);
	const char* pszDefExt = NULL;
	if (lua_type(luaState, 4) == LUA_TSTRING)
	{
		pszDefExt = luaL_checkstring(luaState, 4);
	}
	const char* pszFileName = NULL;
	if (lua_type(luaState, 5) == LUA_TSTRING)
	{
		pszFileName = luaL_checkstring(luaState, 5);
	}

	wstring strDefExt = L"";

	if (pszDefExt != NULL)
	{
		strDefExt = ultra::_UTF2T(pszDefExt);
	}

	wstring strFileName = L"";
	if (pszFileName != NULL)
	{
		strFileName = ultra::_UTF2T(pszFileName);
	}

	wstring strFilter = ultra::_UTF2T(pszFilter);
	std::replace(strFilter.begin(), strFilter.end(), L'|', L'\0');

	std::string strPath;
	WTL::CFileDialog dlg(bOpenFileDialog, strDefExt.c_str(), strFileName.c_str(), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strFilter.c_str());
	if (IDOK == dlg.DoModal())
	{
		strPath = ultra::_T2UTF(dlg.m_szFileName);
	}
	lua_pushstring(luaState, strPath.c_str());
	return 1;
}