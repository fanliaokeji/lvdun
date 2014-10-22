#include "StdAfx.h"
#include "resource.h"
#include <string>
#include <winsock2.h>


#include "GSApp.h"


using namespace std;

CGSApp::CGSApp(void)
{
	m_strCmdLine = L"";
}

CGSApp::~CGSApp(void)
{
}

BOOL CGSApp::InitInstance(LPWSTR lpCmdLine)
{
	if (NULL != lpCmdLine)
	{
		m_strCmdLine = lpCmdLine;
	}
	// 初始化LuaRuntime的调试接口
#ifdef TSLOG
	//XLLRT_DebugInit("greenshield",XLLRT_DEBUG_TYPE_HOOK);
#else
	//XLLRT_DebugInit("greenshield",XLLRT_DEBUG_TYPE_NOHOOK);
#endif

	

	return IniEnv();
}


int __stdcall CGSApp::LuaErrorHandle(lua_State* luaState,const wchar_t* pExtInfo, const wchar_t* luaErrorString,PXL_LRT_ERROR_STACK pStackInfo)
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

int CGSApp::ExitInstance()
{
	//TSTRACEAUTO();
	TSDEBUG4CXX(" TerminateProcess 0");

	//XLUE_Uninit(NULL); //XP下卡死
	TSDEBUG4CXX(" TerminateProcess 1");
	XLUE_UninitLuaHost(NULL);
	TSDEBUG4CXX(" TerminateProcess 2");
	//XL_UnInitGraphicLib();
	TSDEBUG4CXX(" TerminateProcess 3");
	XLUE_UninitHandleMap(NULL);
	TSDEBUG4CXX(" TerminateProcess 4");

	TSDEBUG4CXX(" TerminateProcess 5");
	TerminateProcess(GetCurrentProcess(), 0);
	return 0;
}



BOOL CGSApp::IniEnv()
{
	TCHAR szXar[MAX_PATH] = {0};
	GetModuleFileName((HMODULE)g_hInst, szXar, MAX_PATH);
	PathRemoveFileSpec(szXar);
	PathAppend(szXar, _T("..\\xar\\")); 
	if (!::PathFileExists(szXar) || !::PathIsDirectory(szXar) )
	{
		return FALSE;
	}
	m_strXarPath = szXar;
	// 1)初始化图形库
	XLGraphicParam param;
	XL_PrepareGraphicParam(&param);
	param.textType = XLTEXT_TYPE_GDI;
	XL_InitGraphicLib(&param);
	//XL_SetFreeTypeEnabled(TRUE);

	// 2)初始化XLUE,这函数是一个符合初始化函数
	// 完成了初始化Lua环境,标准对象,XLUELoader的工作
	//XLFS_Init();
	XLUE_InitLoader(NULL);
	XLLRT_ErrorHandle(CGSApp::LuaErrorHandle);

	if (!m_RegisterLuaAPI.Init())
	{
		return FALSE;
	}

	InternalLoadXAR();
	return TRUE;
}


void CGSApp::InternalLoadXAR()
{
	XLUE_AddXARSearchPath(m_strXarPath.c_str());
	if (XLUE_XARExist("main"))
	{
		long iRet = XLUE_LoadXAR("main");	//返回值为0说明加载成功
		TSDEBUG4CXX(L"XLUE_LoadXAR iret = " << iRet);
		if(iRet != 0)
		{
			TerminateProcess(GetCurrentProcess(), (UINT)-20);
		}
	}
	else
	{
		TSDEBUG(_T("XLUE_XARExist main) return FALSE"));
		TerminateProcess(GetCurrentProcess(), (UINT)-30);
	}
}


std::wstring CGSApp::GetCommandLine()
{
	return m_strCmdLine;
}