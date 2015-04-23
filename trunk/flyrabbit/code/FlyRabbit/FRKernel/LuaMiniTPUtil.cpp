
#include "StdAfx.h"
#include "LuaMiniTPUtil.h"
#include "miniTP/xldl.h"
#include "miniTP/MiniTPWrapper.h"
DownWrapper gMiniTPWrapper;
LuaMiniTPUtil::LuaMiniTPUtil(void)
{
}

LuaMiniTPUtil::~LuaMiniTPUtil(void)
{
}

XLLRTGlobalAPI LuaMiniTPUtil::LuaMiniTPMemberFunctions[] =
{
	{"LoadXLDL", MiniTPLoadXLDL},
	{"Init", MiniTPInit},
	{"UnInit", MiniTPUnInit},
	{"TaskCreate", MiniTPTaskCreate},
	{"TaskDelete", MiniTPTaskDelete},
	{"TaskStart", MiniTPTaskStart},
	{"TaskPause", MiniTPTaskPause},
	{"TaskQueryEx", MiniTPTaskQueryEx},
	{"LimitSpeed", MiniTPLimitSpeed},
	{"LimitUploadSpeed", MiniTPLimitUploadSpeed},
	{"DelTempFile", MiniTPDelTempFile},
	{"SetUserAgent", MiniTPSetUserAgent},
	{"GetFileSizeWithUrl", MiniTPGetFileSizeWithUrl},
	{"ParseThunderPrivateUrl", MiniTPParseThunderPrivateUrl},

	{NULL, NULL}
};

void* __stdcall LuaMiniTPUtil::GetInstance( void* )
{
	static LuaMiniTPUtil s_instance;
	return &s_instance;
}

void LuaMiniTPUtil::RegisterSelf( XL_LRT_ENV_HANDLE hEnv )
{
	if (hEnv == NULL)
	{
		return;
	}

	XLLRTObject theObject;
	theObject.ClassName = MiniTP_UTIL_CLASS;
	theObject.MemberFunctions = LuaMiniTPMemberFunctions;
	theObject.ObjName = MiniTP_UTIL_OBJ;
	theObject.userData = NULL;
	theObject.pfnGetObject = (fnGetObject)LuaMiniTPUtil::GetInstance;

	XLLRT_RegisterGlobalObj(hEnv,theObject);
}

int LuaMiniTPUtil::MiniTPLoadXLDL(lua_State* pLuaState)
{
	LuaMiniTPUtil** ppUtil = (LuaMiniTPUtil **)luaL_checkudata(pLuaState, 1, MiniTP_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	if (!lua_isstring(pLuaState,2))
	{
		return 0;
	}
	const char* utf8Path = luaL_checkstring(pLuaState, 2);

	CComBSTR bstrPath;
	LuaStringToCComBSTR(utf8Path,bstrPath);
	BOOL bRet = gMiniTPWrapper.LoadXLDL(bstrPath.m_str);
	lua_pushboolean(pLuaState, bRet);
	return 1;
}

int LuaMiniTPUtil::MiniTPInit(lua_State* pLuaState)
{
	LuaMiniTPUtil** ppUtil = (LuaMiniTPUtil **)luaL_checkudata(pLuaState, 1, MiniTP_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	BOOL bRet = gMiniTPWrapper.Init();
	lua_pushboolean(pLuaState, bRet);
	return 1;
}

int LuaMiniTPUtil::MiniTPUnInit(lua_State* pLuaState)
{
	LuaMiniTPUtil** ppUtil = (LuaMiniTPUtil **)luaL_checkudata(pLuaState, 1, MiniTP_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	BOOL bRet = gMiniTPWrapper.UnInit();
	lua_pushboolean(pLuaState, bRet);
	return 1;
}

int LuaMiniTPUtil::MiniTPTaskCreate(lua_State* pLuaState)
{
	LuaMiniTPUtil** ppUtil = (LuaMiniTPUtil **)luaL_checkudata(pLuaState, 1, MiniTP_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	if (!lua_isstring(pLuaState,2) || !lua_isstring(pLuaState,3) || !lua_isstring(pLuaState,4))
	{
		return 0;
	}
	const char* utf8TaskUrl = luaL_checkstring(pLuaState, 2);
	const char* utf8SavePath = luaL_checkstring(pLuaState, 3);
	const char* utf8Filename = luaL_checkstring(pLuaState, 4);
	CComBSTR bstrTaskUrl,bstrSavePath,bstrFilename;
	LuaStringToCComBSTR(utf8TaskUrl,bstrTaskUrl);
	LuaStringToCComBSTR(utf8SavePath,bstrSavePath);
	LuaStringToCComBSTR(utf8Filename,bstrFilename);
	DownTaskParam downTaskParam;
	StrCpyW(downTaskParam.szTaskUrl, bstrTaskUrl.m_str);
	StrCpyW(downTaskParam.szSavePath, bstrSavePath.m_str);
	StrCpyW(downTaskParam.szFilename, bstrFilename.m_str);

	if (lua_isstring(pLuaState, 5))
	{
		const char* utf8RefUrl = luaL_checkstring(pLuaState, 5);
		CComBSTR bstrRefUrl;
		LuaStringToCComBSTR(utf8RefUrl,bstrRefUrl);
		StrCpyW(downTaskParam.szRefUrl, bstrRefUrl.m_str);
	}

	if (lua_isstring(pLuaState, 6))
	{
		const char* utf8Cookies = luaL_checkstring(pLuaState, 6);
		CComBSTR bstrCookies;
		LuaStringToCComBSTR(utf8Cookies,bstrCookies);
		StrCpyW(downTaskParam.szCookies, bstrCookies.m_str);
	}
	
	if (lua_isstring(pLuaState, 7))
	{
		const char* utf8Cookies = luaL_checkstring(pLuaState, 7);
		CComBSTR bstrCookies;
		LuaStringToCComBSTR(utf8Cookies,bstrCookies);
		StrCpyW(downTaskParam.szCookies, bstrCookies.m_str);
	}
	
	int nIsOnlyOriginal = lua_toboolean(pLuaState, 8);
	BOOL bIsOnlyOriginal = (nIsOnlyOriginal == 0) ? FALSE : TRUE;
	if (bIsOnlyOriginal)
		downTaskParam.IsOnlyOriginal = bIsOnlyOriginal;
	
	int nDisableAutoRename = lua_toboolean(pLuaState, 9);
	BOOL bDisableAutoRename = (nDisableAutoRename == 0) ? FALSE : TRUE;
	if (bDisableAutoRename)
		downTaskParam.DisableAutoRename = TRUE;

	int nIsResume = lua_toboolean(pLuaState, 10);
	BOOL bIsResume = (nIsResume == 0) ? FALSE : TRUE;
	if (!bIsResume)
		downTaskParam.IsResume = FALSE;

	HANDLE hTask = gMiniTPWrapper.TaskCreate(downTaskParam);
	lua_pushlightuserdata(pLuaState, hTask);
	return 1;
}


int LuaMiniTPUtil::MiniTPTaskDelete(lua_State* pLuaState)
{
	LuaMiniTPUtil** ppUtil = (LuaMiniTPUtil **)luaL_checkudata(pLuaState, 1, MiniTP_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	HANDLE hTask = (HANDLE)lua_touserdata(pLuaState, 2);
	BOOL bRet = gMiniTPWrapper.TaskDelete(hTask);
	lua_pushboolean(pLuaState, bRet);
	return 1;
}

int LuaMiniTPUtil::MiniTPTaskStart(lua_State* pLuaState)
{
	LuaMiniTPUtil** ppUtil = (LuaMiniTPUtil **)luaL_checkudata(pLuaState, 1, MiniTP_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	HANDLE hTask = (HANDLE)lua_touserdata(pLuaState, 2);
	BOOL bRet = gMiniTPWrapper.TaskStart(hTask);
	lua_pushboolean(pLuaState, bRet);
	return 1;
}

int LuaMiniTPUtil::MiniTPTaskPause(lua_State* pLuaState)
{
	LuaMiniTPUtil** ppUtil = (LuaMiniTPUtil **)luaL_checkudata(pLuaState, 1, MiniTP_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	HANDLE hTask = (HANDLE)lua_touserdata(pLuaState, 2);
	BOOL bRet = gMiniTPWrapper.TaskPause(hTask);
	lua_pushboolean(pLuaState, bRet);
	return 1;
}

int LuaMiniTPUtil::MiniTPTaskQueryEx(lua_State* pLuaState)
{
	LuaMiniTPUtil** ppUtil = (LuaMiniTPUtil **)luaL_checkudata(pLuaState, 1, MiniTP_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	HANDLE hTask = (HANDLE)lua_touserdata(pLuaState, 2);
	DownTaskInfo downTaskInfo;
	BOOL bRet = gMiniTPWrapper.TaskQueryEx(hTask,downTaskInfo);
	lua_pushboolean(pLuaState, bRet);
	if (bRet)
	{
		lua_newtable(pLuaState);

		lua_pushstring(pLuaState, "stat");
		lua_pushnumber(pLuaState, downTaskInfo.stat);
		lua_settable(pLuaState, -3);

		lua_pushstring(pLuaState, "fail_code");
		lua_pushnumber(pLuaState, downTaskInfo.fail_code);
		lua_settable(pLuaState, -3);
		
		lua_pushstring(pLuaState, "szFilename");
		if (downTaskInfo.szFilename[0] != '\0')
		{
			std::string strFilename;
			BSTRToLuaString(downTaskInfo.szFilename, strFilename);
			lua_pushstring(pLuaState, strFilename.c_str());
		}
		else
		{
			lua_pushnil(pLuaState);
		}
		lua_settable(pLuaState, -3);
		
		lua_pushstring(pLuaState, "nTotalSize");
		lua_pushnumber(pLuaState, (lua_Number)downTaskInfo.nTotalSize);
		lua_settable(pLuaState, -3);

		lua_pushstring(pLuaState, "nTotalDownload");
		lua_pushnumber(pLuaState, (lua_Number)downTaskInfo.nTotalDownload);
		lua_settable(pLuaState, -3);

		lua_pushstring(pLuaState, "fPercent");
		lua_pushnumber(pLuaState, downTaskInfo.fPercent);
		lua_settable(pLuaState, -3);

		lua_pushstring(pLuaState, "nSrcTotal");
		lua_pushnumber(pLuaState, downTaskInfo.nSrcTotal);
		lua_settable(pLuaState, -3);

		lua_pushstring(pLuaState, "nSrcUsing");
		lua_pushnumber(pLuaState, downTaskInfo.nSrcUsing);
		lua_settable(pLuaState, -3);
		
		lua_pushstring(pLuaState, "nDonationP2P");
		lua_pushnumber(pLuaState, (lua_Number)downTaskInfo.nDonationP2P);
		lua_settable(pLuaState, -3);

		lua_pushstring(pLuaState, "nDonationOrgin");
		lua_pushnumber(pLuaState, (lua_Number)downTaskInfo.nDonationOrgin);
		lua_settable(pLuaState, -3);

		lua_pushstring(pLuaState, "nDonationP2S");
		lua_pushnumber(pLuaState, (lua_Number)downTaskInfo.nDonationP2S);
		lua_settable(pLuaState, -3);

		lua_pushstring(pLuaState, "nSpeed");
		lua_pushnumber(pLuaState, downTaskInfo.nSpeed);
		lua_settable(pLuaState, -3);

		lua_pushstring(pLuaState, "nSpeedP2S");
		lua_pushnumber(pLuaState, downTaskInfo.nSpeedP2S);
		lua_settable(pLuaState, -3);

		lua_pushstring(pLuaState, "nSpeedP2P");
		lua_pushnumber(pLuaState, downTaskInfo.nSpeedP2P);
		lua_settable(pLuaState, -3);

		lua_pushstring(pLuaState, "bIsOriginUsable");
		lua_pushboolean(pLuaState, downTaskInfo.bIsOriginUsable);
		lua_settable(pLuaState, -3);

		lua_pushstring(pLuaState, "IsCreatingFile");
		lua_pushnumber(pLuaState, downTaskInfo.IsCreatingFile);
		lua_settable(pLuaState, -3);
		return 2;
	}
	return 1;
}

int LuaMiniTPUtil::MiniTPLimitSpeed(lua_State* pLuaState)
{
	LuaMiniTPUtil** ppUtil = (LuaMiniTPUtil **)luaL_checkudata(pLuaState, 1, MiniTP_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	int nBps = (int) lua_tointeger(pLuaState, 2);
	gMiniTPWrapper.LimitSpeed(nBps);
	return 0;
}

int LuaMiniTPUtil::MiniTPLimitUploadSpeed(lua_State* pLuaState)
{
	LuaMiniTPUtil** ppUtil = (LuaMiniTPUtil **)luaL_checkudata(pLuaState, 1, MiniTP_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	int nTcpBps = (int) lua_tointeger(pLuaState, 2);
	int nOtherBps = (int) lua_tointeger(pLuaState, 2);
	gMiniTPWrapper.LimitUploadSpeed(nTcpBps,nOtherBps);
	return 0;
}

int LuaMiniTPUtil::MiniTPDelTempFile(lua_State* pLuaState)
{
	LuaMiniTPUtil** ppUtil = (LuaMiniTPUtil **)luaL_checkudata(pLuaState, 1, MiniTP_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	if (!lua_isstring(pLuaState,2) || !lua_isstring(pLuaState,3))
	{
		return 0;
	}
	const char* utf8SavePath = luaL_checkstring(pLuaState, 2);
	const char* utf8Filename = luaL_checkstring(pLuaState, 3);
	CComBSTR bstrSavePath,bstrFilename;
	LuaStringToCComBSTR(utf8SavePath,bstrSavePath);
	LuaStringToCComBSTR(utf8Filename,bstrFilename);

	DownTaskParam delTaskParam;
	StrCpyW(delTaskParam.szFilename, bstrSavePath.m_str);
	StrCpyW(delTaskParam.szSavePath, bstrFilename.m_str);
	BOOL bRet = gMiniTPWrapper.DelTempFile(delTaskParam);
	lua_pushboolean(pLuaState, bRet);
	return 1;
}

int LuaMiniTPUtil::MiniTPSetUserAgent(lua_State* pLuaState)
{
	LuaMiniTPUtil** ppUtil = (LuaMiniTPUtil **)luaL_checkudata(pLuaState, 1, MiniTP_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	if (!lua_isstring(pLuaState,2))
	{
		return 0;
	}
	const char* utf8UserAgent = luaL_checkstring(pLuaState, 2);
	CComBSTR bstrUserAgent;
	LuaStringToCComBSTR(utf8UserAgent,bstrUserAgent);
	gMiniTPWrapper.SetUserAgent(bstrUserAgent.m_str);
	return 0;
}

int LuaMiniTPUtil::MiniTPGetFileSizeWithUrl(lua_State* pLuaState)
{
	LuaMiniTPUtil** ppUtil = (LuaMiniTPUtil **)luaL_checkudata(pLuaState, 1, MiniTP_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	if (!lua_isstring(pLuaState,2))
	{
		return 0;
	}
	const char* utf8URL = luaL_checkstring(pLuaState, 2);
	CComBSTR bstrURL;
	LuaStringToCComBSTR(utf8URL,bstrURL);
	INT64 iFileSize = 0;
	BOOL bRet = gMiniTPWrapper.GetFileSizeWithUrl(bstrURL.m_str,iFileSize);
	lua_pushboolean(pLuaState, bRet);
	lua_pushnumber(pLuaState, (lua_Number)iFileSize);
	return 2;
}

int LuaMiniTPUtil::MiniTPParseThunderPrivateUrl(lua_State* pLuaState)
{
	LuaMiniTPUtil** ppUtil = (LuaMiniTPUtil **)luaL_checkudata(pLuaState, 1, MiniTP_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	if (!lua_isstring(pLuaState,2))
	{
		return 0;
	}
	const char* utf8ThunderUrl = luaL_checkstring(pLuaState, 2);
	CComBSTR bstrThunderUrl;
	LuaStringToCComBSTR(utf8ThunderUrl,bstrThunderUrl);

	wchar_t normalUrlBuffer[2084] = {0};
	BOOL bRet = gMiniTPWrapper.ParseThunderPrivateUrl(bstrThunderUrl.m_str,normalUrlBuffer,2084);
	lua_pushboolean(pLuaState, bRet);
	if (bRet)
	{
		std::string strNormalUrl;
		BSTRToLuaString(normalUrlBuffer, strNormalUrl);
		lua_pushstring(pLuaState, strNormalUrl.c_str());
	}
	else
	{
		lua_pushnil(pLuaState);
	}
	return 2;
}