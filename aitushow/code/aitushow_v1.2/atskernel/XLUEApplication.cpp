#include "StdAfx.h"
#include <string>
#include <winsock2.h>


#include "XLUEApplication.h"


using namespace std;

CXLUEApplication::CXLUEApplication(void)
{
	m_strCmdLine = L"";
}

CXLUEApplication::~CXLUEApplication(void)
{
}

BOOL CXLUEApplication::InitInstance(LPWSTR lpCmdLine)
{
	if (NULL != lpCmdLine)
	{
		m_strCmdLine = lpCmdLine;
	}
	// 初始化LuaRuntime的调试接口
#ifdef TSLOG
	//XLLRT_DebugInit("ats",XLLRT_DEBUG_TYPE_HOOK);
#else
	//XLLRT_DebugInit("ats",XLLRT_DEBUG_TYPE_NOHOOK);
#endif

	g_pPathHelper = new CKKPathHelper();
	g_pTaskProcessor = new CTaskProcessor();
	g_pTaskProcessor->Init();
	g_pTaskProcessor1 = new CTaskProcessor();
	g_pTaskProcessor1->Init();

	return IniEnv();
}


int __stdcall CXLUEApplication::LuaErrorHandle(lua_State* luaState,const wchar_t* pExtInfo, const wchar_t* luaErrorString,PXL_LRT_ERROR_STACK pStackInfo)
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

int CXLUEApplication::ExitInstance()
{
	TerminateProcess(GetCurrentProcess(), 0);
	return 0;
}



BOOL CXLUEApplication::IniEnv()
{
	TCHAR szXar[MAX_PATH] = {0};
	GetModuleFileName((HMODULE)g_hInst, szXar, MAX_PATH);
	PathRemoveFileSpec(szXar);
	//PathAppend(szXar, _T("..\\xar\\")); 
	if (!::PathFileExists(szXar) || !::PathIsDirectory(szXar) )
	{
		//MessageBoxA(NULL,"获取界面皮肤路径失败","错误",MB_OK|MB_ICONERROR);
		//return FALSE;
	}
	PathAppend(szXar, _T("..\\xar\\"));
	m_strXarPath = szXar;
	// 1)初始化图形库
	//XLFS_Init();
	XLUE_InitLoader(NULL);
	XLGraphicParam param;
	XL_PrepareGraphicParam(&param);
	param.textType = XLTEXT_TYPE_GDI;
	XL_InitGraphicLib(&param);
	
	//XL_SetFreeTypeEnabled(TRUE);
	XLGraphicPlusParam plusParam;
	XLGP_PrepareGraphicPlusParam(&plusParam);
	XLGP_InitGraphicPlus(&plusParam);
	BOOL bRet = XLGP_RegisterLuaHost();

	XL_SetFreeTypeEnabled(TRUE);
	//XLGraphicPlusParam plusParam;
	//XLGP_PrepareGraphicPlusParam(&plusParam);
	//XLGP_InitGraphicPlus(&plusParam);
	// 2)初始化XLUE,这函数是一个符合初始化函数
	// 完成了初始化Lua环境,标准对象,XLUELoader的工作
	//XLFS_Init();
	
	XLLRT_ErrorHandle(CXLUEApplication::LuaErrorHandle);

	if (!m_RegisterLuaAPI.Init())
	{
		return FALSE;
	}

	InternalLoadXAR();

	//XLGraphicPlusParam plusparam;
	//plusparam.bInitLua = TRUE;
	//XLGP_PrepareGraphicPlusParam(&plusparam);
	//XLGP_InitGraphicPlus(&plusparam);

	return TRUE;
}


void CXLUEApplication::InternalLoadXAR()
{
	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szPath, MAX_PATH);
	std::wstring strExePath = szPath;
	PathRemoveFileSpec(szPath);
	PathAppend(szPath, _T("xar")); 
	std::wstring strXarDest = szPath;
	std::wstring strXarRes = L"xar@resource://";
	strXarRes+=strExePath;
	strXarRes+=L"|xar_res|1001$";
	TSDEBUG4CXX(L"InternalLoadXAR, strXarDest = " << strXarDest.c_str()<<L", strXarRes = " <<strXarRes.c_str());
	long iRet = XLFS_MountDir(strXarDest.c_str(), strXarRes.c_str(), 0, 0);
	TSDEBUG4CXX(L"XLFS_MountDir iRet = " << iRet);
	if (0 == iRet)
	{
		std::string strAnsi;
		WideStringToAnsiString(strXarDest,strAnsi);
		iRet = XLUE_LoadXAR(strAnsi.c_str());
	}
	if (0 != iRet)
	{
		TSDEBUG4CXX(L"XLFS_MountDir,XLUE_LoadXAR error");
		XLUE_AddXARSearchPath(m_strXarPath.c_str());
		if (XLUE_XARExist("main"))
		{
			long iRet = XLUE_LoadXAR("main");	//返回值为0说明加载成功
			TSDEBUG4CXX(L"AddXARSearch,XLUE_LoadXAR iRet = " << iRet);
			if(iRet != 0)
			{
				TerminateProcess(GetCurrentProcess(), (UINT)-20);
			}
		}
		else
		{
			MessageBoxA(NULL,"无法获取界面皮肤","错误",MB_OK|MB_ICONERROR);
			TSDEBUG(_T("XLUE_XARExist main) return FALSE"));
			TerminateProcess(GetCurrentProcess(), (UINT)-30);
		}
	}
}


std::wstring CXLUEApplication::GetCommandLine()
{
	return m_strCmdLine;
}

void CXLUEApplication::ReleaseCache()
{
	TSAUTO();
	XL_LRT_ENV_HANDLE hEnv = XLLRT_GetEnv(NULL);
	XL_LRT_RUNTIME_HANDLE hRunTime= XLLRT_GetRuntime(hEnv, NULL);
	lua_State* pLuaState = XLLRT_GetLuaState(hRunTime);
	lua_gc(pLuaState, LUA_GCCOLLECT, 0);
	XLLRT_ReleaseRunTime(hRunTime);
	XLLRT_ReleaseEnv(hEnv);

	XLUE_GC(NULL);
	XL_StatObject(0);
	return;
}