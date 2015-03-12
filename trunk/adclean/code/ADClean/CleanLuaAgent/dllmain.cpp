// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "LuaSetUtil.h"
#include "LuaAsynUtil.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

int __stdcall LuaErrorHandle(lua_State* luaState,const wchar_t* pExtInfo, const wchar_t* luaErrorString,PXL_LRT_ERROR_STACK pStackInfo)
{
	TSTRACEAUTO();
	static bool s_bEnter = false;
	if (!s_bEnter)
	{
		s_bEnter = true;
		if(pExtInfo != NULL)
		{
			TSDEBUG4CXX(L"LuaErrorHandle: " << luaErrorString << L" @ " << pExtInfo);
		}
		else
		{
			TSDEBUG4CXX(L"LuaErrorHandle: " << luaErrorString);
		}
		s_bEnter = false;
	}
	return 0;
}

extern "C" __declspec(dllexport) void RunLua(wchar_t* szLuaPath,wchar_t* szParam)
{
	XLLRT_ErrorHandle(LuaErrorHandle);

	XL_LRT_ENV_HANDLE hEnv = XLLRT_GetEnv(NULL);
	if (hEnv == NULL)//初始化LUA环境
	{
		hEnv = XLLRT_CreateEnv(NULL);
		XL_LRT_RUNTIME_HANDLE hRuntime = XLLRT_CreateRunTime(hEnv, NULL, 0);
		TSDEBUG4CXX(L"XLLRT_GetEnv error! XLLRT_CreateEnv hEnv = " << hEnv << L", XLLRT_CreateRunTime hRuntime = " << hRuntime);
	}
	if (NULL == hEnv)
	{
		TSDEBUG4CXX(L"XLLRT_GetEnv error!  hEnv == NULL");
		return ;
	}
	// 初始化接口
	LuaGSUtil::RegisterObj(hEnv);
	LuaAsynUtil::RegisterSelf(hEnv);
	XLLRT_ReleaseEnv(hEnv);


	hEnv = XLLRT_GetEnv(NULL);
	XL_LRT_RUNTIME_HANDLE hRuntime = XLLRT_GetRuntime(hEnv, NULL);
	XL_LRT_CHUNK_HANDLE hChunk = NULL;
	
	wchar_t drive[_MAX_PATH];
	wchar_t dir[_MAX_PATH];
	wchar_t fname[_MAX_PATH];
	wchar_t ext [_MAX_PATH];
	wchar_t szCfgPath [_MAX_PATH];
	_wsplitpath(szLuaPath, drive,dir,fname,ext);
	//具名chunk 生成规则 由LUA文件名与调用函数组成
	std::wstring wstrNamedChunk = fname;
	wstrNamedChunk.append(L".Run");

	XLLRT_CreateChunkFromModule(wstrNamedChunk.c_str(), szLuaPath, "Run", &hChunk);
	if (hChunk != NULL)
	{
		lua_State* pLuaState = XLLRT_GetLuaState(hRuntime);
		int lt = lua_gettop(pLuaState);
		long lRet = XLLRT_PrepareChunk(hRuntime,hChunk);
		if(lRet == 0)
		{
			if (szParam[0] == '\0')
			{
				XLLRT_LuaCall(pLuaState,0,0,wstrNamedChunk.c_str());
			} 
			else
			{
				std::string strParam;
				BSTRToLuaString(szParam,strParam);
				lua_pushstring(pLuaState,strParam.c_str());
				XLLRT_LuaCall(pLuaState,1,0,wstrNamedChunk.c_str());
			}
			lua_settop(pLuaState,lt);
		}
		else
		{
			TSDEBUG4CXX(L"Call Lua Function<Run> Failed");
			XLLRT_ReleaseChunk(hChunk);
			XLLRT_ReleaseRunTime(hRuntime);
			XLLRT_ReleaseEnv(hEnv);
		}
	}
	return;
}