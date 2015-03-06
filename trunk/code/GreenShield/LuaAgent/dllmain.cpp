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


extern "C" __declspec(dllexport) void RunLua(wchar_t* szLuaPath)
{
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
	XLLRT_CreateChunkFromFile(::PathFindFileName(szLuaPath), szLuaPath, &hChunk);
	if (hChunk != NULL)
	{
		XLLRT_RunChunk(hRuntime, hChunk);
	}
	if (hChunk != NULL)
	{
		XLLRT_ReleaseChunk(hChunk);
		XLLRT_ReleaseRunTime(hRuntime);
		XLLRT_ReleaseEnv(hEnv);
	}
	return;
}