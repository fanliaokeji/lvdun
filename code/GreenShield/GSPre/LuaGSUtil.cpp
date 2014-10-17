#include "StdAfx.h"

#include <TlHelp32.h>
#include <Shlobj.h>
#include <atlsync.h>
#include <atltime.h>
#include <WTL/atldlgs.h>
#include "DatFileUtility.h"
#include "LuaGSUtil.h"
#include "LuaAPIHelper.h"
#include "GSApp.h"
#include "PeeIdHelper.h"
#include "..\GsNetFilter\GsNetFilter.h"
#include "commonshare\md5.h"
#include <openssl/rsa.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#pragma comment(lib,"libeay32.lib")
#pragma comment(lib,"ssleay32.lib")

extern CGSApp theApp;
//extern CFilterMsgWindow gFilterMsgWindow;

LuaGSUtil::LuaGSUtil(void)
{
}

LuaGSUtil::~LuaGSUtil(void)
{
}

XLLRTGlobalAPI LuaGSUtil::sm_LuaMemberFunctions[] = 
{
	//{"RegisterFilterWnd", RegisterFilterWnd},	
	//主要函数
	{"LoadConfig",LoadConfig},
	
	{"AddVideoHost",AddVideoHost},
	{"AddWhiteHost",AddWhiteHost},
	{"UpdateVideoHost",UpdateVideoHost},
	{"UpdateWhiteHost",UpdateWhiteHost},
	
	{"GSFilter", FGSFilter},
	{"Exit", Exit},	
	{"GetPeerId", GetPeerId},
	{"Log", Log},
	{"SaveLuaTableToLuaFile", SaveLuaTableToLuaFile},
	{"GetCommandLine", GetCommandLine},
	{"CommandLineToList", CommandLineToList},

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

	//文件
	{"GetMD5Value", GetMD5Value},
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
	{"CopyPathFile", CopyPathFile},
	{"DeletePathFile", DeletePathFile},
	//读写UTF8文件
	{"ReadFileToString", ReadFileToString},
	{"WriteStringToFile", WriteStringToFile},

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

	{"GetCurrentProcessId", FGetCurrentProcessId},
	{"GetAllSystemInfo", FGetAllSystemInfo},
	{"GetProcessIdFromHandle", FGetProcessIdFromHandle},
	{"GetTickCount", GetTotalTickCount},
	{"GetOSVersion", GetOSVersionInfo},
	{"QueryProcessExists", QueryProcessExists},

	//功能
	{"CreateShortCutLinkEx", CreateShortCutLinkEx},
	{"OpenURL", OpenURL},
	{"OpenURLIE", OpenURLIE},
	{"ShellExecute", ShellExecuteEX},


	{"EncryptAESToFile", EncryptAESToFile},
	{"DecryptFileAES", DecryptFileAES},
	
	{NULL, NULL}
};

int LuaGSUtil::LoadConfig(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
	{
		return 0;
	}
	if (!lua_isstring(pLuaState,2))
	{
		return 0;
	}

	BOOL bRet = FALSE;
	const char* utf8CfgPath = luaL_checkstring(pLuaState, 2);
	CComBSTR bstrPath;
	LuaStringToCComBSTR(utf8CfgPath,bstrPath);
	if (::PathFileExists(bstrPath.m_str))
	{
		 FilterManager* m_Filter = GsGetFilterManager(bstrPath.m_str);
		if (m_Filter)
		{
			bRet = TRUE;
		}
	}
	lua_pushboolean(pLuaState, bRet);
	return 1;
}

int LuaGSUtil::AddVideoHost(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
	{
		return 0;
	}
	if (!lua_isstring(pLuaState,2))
	{
		return 0;
	}
	const char* utf8VideoHost = luaL_checkstring(pLuaState, 2);
	CComBSTR bstrVideoHost;
	LuaStringToCComBSTR(utf8VideoHost,bstrVideoHost);
	std::string strAnsi;
	WideStringToAnsiString(bstrVideoHost.m_str,strAnsi);
	int istate = 0;
	if (!lua_isnoneornil( pLuaState, 3 ))
	{
		istate = lua_tointeger(pLuaState, 3);
	}
	GsUpdateConfigVideoHost(strAnsi.c_str(),istate);
	return 0;
}

int LuaGSUtil::AddWhiteHost(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
	{
		return 0;
	}
	if (!lua_isstring(pLuaState,2))
	{
		return 0;
	}
	const char* utf8WhiteHost = luaL_checkstring(pLuaState, 2);
	CComBSTR bstrWhiteHost;
	LuaStringToCComBSTR(utf8WhiteHost,bstrWhiteHost);
	std::string strAnsi;
	WideStringToAnsiString(bstrWhiteHost.m_str,strAnsi);
	GsUpdateConfigWhiteHost(strAnsi.c_str(),TRUE);
	return 0;
}
#define UPDATE_CFG_VIDEO 0x0001
#define UPDATE_CFG_WHITE 0x0002

typedef struct _UPADTE_CFG_PARAM 
{
	std::string host;
	int istate;
	bool bEnable;
	int flag;
}UPADTE_CFG_PARAM,*PUPADTE_CFG_PARAM;

UINT WINAPI UpdateCfgProc(PVOID pArg)
{
	PUPADTE_CFG_PARAM pData = (PUPADTE_CFG_PARAM) pArg;
	std::string host = pData->host;
	int flag = pData->flag;
	if (flag & UPDATE_CFG_VIDEO)
	{
		GsUpdateConfigVideoHost(host.c_str(),pData->istate);
	}
	else if (flag & UPDATE_CFG_WHITE)
	{
		GsUpdateConfigWhiteHost(host.c_str(),pData->bEnable);
	}
	return 0;
}

int LuaGSUtil::UpdateVideoHost(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
	{
		return 0;
	}
	if (!lua_isstring(pLuaState,2))
	{
		return 0;
	}

	const char* utf8VideoHost = luaL_checkstring(pLuaState, 2);
	CComBSTR bstrVideoHost;
	LuaStringToCComBSTR(utf8VideoHost,bstrVideoHost);
	std::string strAnsi;
	WideStringToAnsiString(bstrVideoHost.m_str,strAnsi);
	int istate = 0;
	if (!lua_isnoneornil( pLuaState, 3 ))
	{
		istate = lua_tointeger(pLuaState, 3);
	}
	UPADTE_CFG_PARAM v = {strAnsi,istate,0,UPDATE_CFG_VIDEO};
	_beginthreadex(NULL, 0, UpdateCfgProc, &v, 0, NULL);
	return 0;
}

int LuaGSUtil::UpdateWhiteHost(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
	{
		return 0;
	}
	if (!lua_isstring(pLuaState,2))
	{
		return 0;
	}

	const char* utf8VideoHost = luaL_checkstring(pLuaState, 2);
	int nEnable = lua_toboolean(pLuaState, 3);
	bool bEnable = nEnable?true:false;
	CComBSTR bstrVideoHost;
	LuaStringToCComBSTR(utf8VideoHost,bstrVideoHost);
	std::string strAnsi;
	WideStringToAnsiString(bstrVideoHost.m_str,strAnsi);

	UPADTE_CFG_PARAM w = {strAnsi,0,bEnable,UPDATE_CFG_WHITE};
	_beginthreadex(NULL, 0, UpdateCfgProc, &w, 0, NULL);
	return 1;
}


int LuaGSUtil::FGSFilter(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
	{
		return 0;
	}
	BOOL bRet = FALSE;
	int nFilter = lua_toboolean(pLuaState, 2);
	BOOL bFilter = (nFilter == 0) ? FALSE : TRUE;
	if (bFilter)
	{
		static BOOL bOnce  = FALSE;
		if (!bOnce)
		{
			bOnce = TRUE;
			if (GsSetHook(L"GsNet32.dll"))
			{
				HANDLE hThread = GsStartProxy();
				if (NULL != hThread)
				{
					bRet = GsEnable(TRUE);
				}
			}
		}
		else
		{
			bRet = GsEnable(TRUE);
		}
	}
	else
	{
		bRet = GsEnable(FALSE);
	}
	lua_pushboolean(pLuaState, bRet);
	return 1;
}

int LuaGSUtil::Exit(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
	{
		return 0;
	}
	theApp.ExitInstance();
	return 0;
}

int LuaGSUtil::GetPeerId(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
	{
		return 0;
	}

	//HWND  hWnd = FindWindow(L"{B239B46A-6EDA-4a49-8CEE-E57BB352F933}_dsmainmsg",NULL);

	//wchar_t * szUrl = new wchar_t[100];

	//wcscpy(szUrl,L"youku.com");

	//PostMessage(hWnd,WM_USER + 202,1,(WPARAM)szUrl);
	//getchar();



	std::wstring strPeerId=L"";
	GetPeerId_(strPeerId);
	wchar_t szPeerId[MAX_PATH] = {0};
	wcsncpy(szPeerId,strPeerId.c_str(),strPeerId.size());

	std::string strUtf8Pid;
	BSTRToLuaString(szPeerId,strUtf8Pid);
	lua_pushstring(pLuaState, strUtf8Pid.c_str());
	return 1;
}

int LuaGSUtil::GetWorkArea(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::GetScreenArea(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::GetScreenSize(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
	{
		return 0;
	}
	int iScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int iScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);
	lua_pushnumber(pLuaState, iScreenWidth);
	lua_pushnumber(pLuaState, iScreenHeight);
	return 2;
}

int LuaGSUtil::GetCursorPos(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
	{
		return 0;
	}
	POINT pt;
	::GetCursorPos(&pt);
	lua_pushnumber(pLuaState, pt.x);
	lua_pushnumber(pLuaState, pt.y);
	return 2;
}

// 获取命令行参数(不包含可执行程序路径)
int LuaGSUtil::GetCommandLine(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
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

int LuaGSUtil::GetFileVersionString(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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


int LuaGSUtil::GetMD5Value(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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


int LuaGSUtil::GetSystemTempPath(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
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

__int64 LuaGSUtil::GetFileSizeHelper(const char* utf8FileFullPath)
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

int LuaGSUtil::GetFileSize(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
	{
		const char* filePath = luaL_checkstring(pLuaState, 2);
		__int64 nFileSize = GetFileSizeHelper(filePath);
		lua_pushnumber(pLuaState,(lua_Number)nFileSize);
		return 1;
	}

	lua_pushnil(pLuaState);
	return 1;
}

int LuaGSUtil::GetFileCreateTime(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::GetCurTimeSpan(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::GetTmpFileName(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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


int LuaGSUtil::GetSpecialFolderPathEx(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::Log(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
	{
		if (ISTSDEBUGVALID())
		{
			const char* szInput = lua_tostring(pLuaState, 2);
			CComBSTR bstrInput;
			LuaStringToCComBSTR(szInput,bstrInput);
			TSDEBUG4CXX(L"[GSPre] " << bstrInput.m_str);
		}
	}
	return 0;
}

void LuaGSUtil::ConvertAllEscape(std::string& strSrc)
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

std::string LuaGSUtil::GetTableStr(lua_State* luaState, int nIndex, std::ofstream& ofs, const std::string strTableName, int nFloor)
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
int LuaGSUtil::SaveLuaTableToLuaFile(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::FindFileList(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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
				if (_tcsicmp(fd.cFileName, _T("..")) && _tcsicmp(fd.cFileName, _T(".")) && FILE_ATTRIBUTE_DIRECTORY != fd.dwFileAttributes)
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

int LuaGSUtil::FindDirList(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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
				if (_tcsicmp(fd.cFileName, _T("..")) && _tcsicmp(fd.cFileName, _T(".")) && FILE_ATTRIBUTE_DIRECTORY == fd.dwFileAttributes)
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

int LuaGSUtil::ExpandEnvironmentString(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::GetTotalTickCount(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
	{
		return 0;
	}
	lua_pushnumber(pLuaState, ::GetTickCount());
	return 1;
}

int LuaGSUtil::CommandLineToList(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
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


int LuaGSUtil::GetOSVersionInfo(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::PathCombine(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
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

int LuaGSUtil::QueryRegValue(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
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

int LuaGSUtil::QueryRegValue64(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
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

BOOL LuaGSUtil::GetHKEY(const char* utf8Root, HKEY &hKey)
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

long LuaGSUtil::QueryRegValueHelper(const char* utf8Root,const char* utf8RegPath,const char* utf8Key, DWORD &dwType, std::string& utf8Result, DWORD &dwValue,BOOL bWow64)
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


int LuaGSUtil::DeleteRegValue(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil)
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

int LuaGSUtil::DeleteRegValue64(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil)
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

long LuaGSUtil::DeleteRegValueHelper(const char* utf8Root, const char* utf8Key,BOOL bWow64)
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

int LuaGSUtil::DeleteRegKey(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil)
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

int LuaGSUtil::DeleteRegKey64(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil)
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

long LuaGSUtil::DeleteRegKeyHelper(const char* utf8Root, const char* utf8SubKey,BOOL bWow64)
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

int LuaGSUtil::CreateRegKey(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil)
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

int LuaGSUtil::CreateRegKey64(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil)
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

long LuaGSUtil::CreateRegKeyHelper(const char* utf8Root, const char* utf8SubKey,BOOL bWow64)
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

int LuaGSUtil::SetRegValue(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil)
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

int LuaGSUtil::SetRegValue64(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil)
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

long LuaGSUtil::SetRegValueHelper(const char* utf8Root, const char* utf8SubKey, const char* utf8ValueName,DWORD dwType, const char* utf8Data, DWORD dwValue,BOOL bWow64)
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

int LuaGSUtil::QueryRegKeyExists(lua_State* pLuaState)
{
	TSTRACEAUTO();
	BOOL bRet = FALSE;
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::EnumRegLeftSubKey(lua_State* pLuaState)
{
	TSTRACEAUTO();
	BOOL bRet = FALSE;
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::EnumRegRightSubKey(lua_State* pLuaState)
{
	TSTRACEAUTO();
	BOOL bRet = FALSE;
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

long LuaGSUtil::OpenURLHelper(const char* utf8URL)
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

int LuaGSUtil::OpenURL(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
	{
		return 0;
	}
	const char* utf8URL = luaL_checkstring(pLuaState, 2);
	if (utf8URL == NULL)
		return 0;

	OpenURLHelper(utf8URL);
	return 0;
}

int LuaGSUtil::OpenURLIE(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
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

BOOL LuaGSUtil::IsFullScreenHelper()
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

int LuaGSUtil::IsNowFullScreen(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
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

long LuaGSUtil::ShellExecHelper(HWND hWnd, const char* lpOperation, const char* lpFile, const char* lpParameters, const char* lpDirectory, const char* lpShowCmd, int iShowCmd)
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

int LuaGSUtil::ShellExecuteEX(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
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

int LuaGSUtil::QueryProcessExists(lua_State* pLuaState)
{
	int iValue = 0;
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil)
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

long LuaGSUtil::QueryFileExistsHelper(const char*utf8FilePath)
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

int LuaGSUtil::QueryFileExists(lua_State* pLuaState)
{
	int iValue = 0;
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil)
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

int LuaGSUtil::Rename(lua_State* pLuaState)
{
	int iValue = 0;
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil)
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

int LuaGSUtil::CreateDir(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil == NULL)
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

long LuaGSUtil::CopyPathFileHelper(const char* utf8ExistingFileName, const char* utf8NewFileName, BOOL bFailedIfExists)
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

int LuaGSUtil::CopyPathFile(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::DeletePathFile(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::ReadFileToString(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::WriteStringToFile(lua_State* pLuaState)
{
	bool ret = false;

	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::FormatCrtTime(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::GetLocalDateTime(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::GetCurrentUTCTime(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::DateTime2Seconds(lua_State* pLuaState)
{	
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::Seconds2DateTime(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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
int LuaGSUtil::CreateNamedMutex(lua_State* pLuaState)
{
	HANDLE hMutex = NULL;
	BOOL bRet = FALSE;
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::CloseNamedMutex(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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


int LuaGSUtil::PostWndMessage(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::CreateShortCutLinkEx(lua_State* pLuaState)
{
	bool success = false;
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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
		success = CreateShortCutLinkHelper(bstrname.m_str, bstrexepath.m_str, LuaGSUtil::CUSTOMPATH
			, bstriconpath.m_str 
			, bstrargument.m_str
			, bstrdescription.m_str
			, bstrdespath.m_str);
	}
	lua_pushboolean(pLuaState, success);
	return 1;
}

bool LuaGSUtil::CreateShortCutLinkHelper(
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

int LuaGSUtil::GetSysWorkArea(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil && *ppGSUtil)
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

int LuaGSUtil::GetCurrentScreenRect(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::FGetProcessIdFromHandle(lua_State* pLuaState)
{
	DWORD dwPID = 0;
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
	{
		HANDLE hProcess = (HANDLE)lua_touserdata(pLuaState, 2);
		dwPID = GetProcessId(hProcess);
	}
	lua_pushnumber(pLuaState, dwPID);
	return 1;
}

int LuaGSUtil::FGetCurrentProcessId(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
	{
		DWORD dwPID = GetCurrentProcessId();
		lua_pushnumber(pLuaState, dwPID);
		return 1;
	}
	lua_pushnil(pLuaState);
	return 1;
}

int LuaGSUtil::FGetDesktopWndHandle(lua_State *pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::FSetWndPos(lua_State *pLuaState)
{
	BOOL bSuc = FALSE;

	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::FShowWnd(lua_State *pLuaState)
{
	BOOL bSuc = FALSE;

	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
	{
		HWND hwnd = (HWND) lua_touserdata(pLuaState, 2);
		int nShowCmd = (int) lua_tointeger(pLuaState, 3);

		bSuc = ::ShowWindow(hwnd, nShowCmd);
		TSDEBUG(_T("ShowWindow(0x%p, %d) ret %ld"), hwnd, nShowCmd, bSuc);
	}

	lua_pushboolean(pLuaState, bSuc);
	return 1;
}

int LuaGSUtil::FGetWndRect(lua_State *pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::FGetWndClientRect(lua_State *pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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

int LuaGSUtil::FFindWindow(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil && *ppGSUtil)
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

int LuaGSUtil::FFindWindowEx(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil && *ppGSUtil)
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

int LuaGSUtil::FIsWindowVisible(lua_State* pLuaState)
{
	bool bVisible = false;
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil && *ppGSUtil)
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

int LuaGSUtil::IsWindowIconic(lua_State* pLuaState)
{
	bool bIconic = false;
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil && *ppGSUtil)
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

int LuaGSUtil::GetWindowTitle(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil && *ppGSUtil)
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

int LuaGSUtil::GetWndClassName(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil && *ppGSUtil)
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

int LuaGSUtil::GetWndProcessThreadId(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil && *ppGSUtil)
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

int LuaGSUtil::FGetAllSystemInfo(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil && *ppGSUtil)
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
		if (::IsWow64Process(::GetCurrentProcess(), &bWow64Process))
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


int LuaGSUtil::PostWndMessageByHandle( lua_State *pLuaState )
{
	BOOL bSuccess = FALSE;

	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil && *ppGSUtil)
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


int LuaGSUtil::SendMessageByHwnd( lua_State *pLuaState )
{

	LuaGSUtil** ppTipWndUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppTipWndUtil && *ppTipWndUtil)
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

LuaGSUtil* __stdcall LuaGSUtil::Instance(void *)
{
	static LuaGSUtil s_instance;
	return &s_instance;
}

void LuaGSUtil::RegisterObj(XL_LRT_ENV_HANDLE hEnv)
{
	if (hEnv == NULL)
	{
		return;
	}

	XLLRTObject object;
	object.ClassName = GS_UTIL_CLASS;
	object.ObjName = GS_UTIL_OBJ;
	object.MemberFunctions = sm_LuaMemberFunctions;
	object.userData = NULL;
	object.pfnGetObject = (fnGetObject)LuaGSUtil::Instance;

	XLLRT_RegisterGlobalObj(hEnv, object);
}



void LuaGSUtil::EncryptAESToFileHelper(const unsigned char* pszKey, const char* pszMsg, unsigned char* out_str, int& nlen)
{
	EVP_CIPHER_CTX ctx;
	// init
	EVP_CIPHER_CTX_init(&ctx);
	EVP_CIPHER_CTX_set_padding(&ctx, 1);

	EVP_EncryptInit_ex(&ctx, EVP_aes_128_ecb(), NULL, (const unsigned char*)pszKey, NULL);

	//这个EVP_EncryptUpdate的实现实际就是将in按照inl的长度去加密，实现会取得该cipher的块大小（对aes_128来说是16字节）并将block-size的整数倍去加密。
	//如果输入为50字节，则此处仅加密48字节，outl也为48字节。输入in中的最后两字节拷贝到ctx->buf缓存起来。  
	//对于inl为block_size整数倍的情形，且ctx->buf并没有以前遗留的数据时则直接加解密操作，省去很多后续工作。  
	int msglen = strlen(pszMsg);
	EVP_EncryptUpdate(&ctx, out_str, &nlen, (const unsigned char*)pszMsg, msglen);
	//余下最后n字节。此处进行处理。
	//如果不支持pading，且还有数据的话就出错，否则，将block_size-待处理字节数个数个字节设置为此个数的值，如block_size=16,数据长度为4，则将后面的12字节设置为16-4=12，补齐为一个分组后加密 
	//对于前面为整分组时，如输入数据为16字节，最后再调用此Final时，不过是对16个0进行加密，此密文不用即可，也根本用不着调一下这Final。
	int outl = 0;
	EVP_EncryptFinal_ex(&ctx, out_str + nlen, &outl);  
	nlen += outl;
	EVP_CIPHER_CTX_cleanup(&ctx);
}

void LuaGSUtil::DecryptFileAESHelper(const unsigned char* pszKey, const unsigned char* pszMsg, int nlen, unsigned char* out_str)
{
	EVP_CIPHER_CTX ctx;
	// init
	EVP_CIPHER_CTX_init(&ctx);

	EVP_DecryptInit_ex(&ctx, EVP_aes_128_ecb(), NULL, pszKey, NULL); 

	int outl = 0;
	EVP_DecryptUpdate(&ctx, out_str, &outl, pszMsg, nlen);
	int len = outl;

	outl = 0;
	EVP_DecryptFinal_ex(&ctx, out_str + len, &outl);  
	len += outl;
	out_str[len]=0;

	EVP_CIPHER_CTX_cleanup(&ctx);
}

int LuaGSUtil::EncryptAESToFile(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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
		unsigned char* out_str = (unsigned char*)malloc(flen + 1);
		memset(out_str, 0, flen + 1);

		int nlen = 0;
		EncryptAESToFileHelper((const unsigned char*)pszKey, pszData, out_str, nlen);

		TCHAR tszSaveDir[MAX_PATH] = {0};
		_tcsncpy(tszSaveDir, bstrFilePath.m_str, MAX_PATH);
		::PathRemoveFileSpec(tszSaveDir);
		if (!::PathFileExists(tszSaveDir))
			::SHCreateDirectory(NULL, tszSaveDir);

		std::ofstream of(bstrFilePath.m_str, std::ios_base::out|std::ios_base::binary);
		of.write((const char*)out_str, nlen);

		free(out_str);
		return 0;
	}
	lua_pushnil(pLuaState);
	return 1;
}

int LuaGSUtil::DecryptFileAES(lua_State* pLuaState)
{
	LuaGSUtil** ppGSUtil = (LuaGSUtil **)luaL_checkudata(pLuaState, 1, GS_UTIL_CLASS);
	if (ppGSUtil != NULL)
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
		if (iFileSize >= 3 && (byte)pdata[0] == 0xEF && (byte)pdata[1] == 0xBB && (byte)pdata[2] == 0xBF)
		{
			// 去掉 BOM 头
			pdata += 3;
			iFileSize -= 3;
		}
		BOOL bIsPlaintext = TRUE;
		for (int i = 0; i < iFileSize && i < 4; i++)
		{
			if (!isprint((byte)pdata[i]))
			{
				bIsPlaintext = FALSE;
				break;
			}
		}
		if (bIsPlaintext)
		{
			lua_pushstring(pLuaState, pdata);
			delete[] data;
			return 1;
		}
		int flen = ((iFileSize >> 4) + 1) << 4;
		unsigned char* out_str = (unsigned char*)malloc(flen + 1);
		memset(out_str, 0, flen + 1);

		DecryptFileAESHelper((const unsigned char*)pszKey, (const unsigned char*)pdata, iFileSize, out_str);

		lua_pushstring(pLuaState, (const char*)out_str);
		free(out_str);
		free(data);
		return 1;
	}
	lua_pushnil(pLuaState);
	return 1;
}