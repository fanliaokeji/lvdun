#include "StdAfx.h"
#include <string>


#include "DDApp.h"


using namespace std;

CDDApp::CDDApp(void)
{
	m_strCmdLine = L"";
}

CDDApp::~CDDApp(void)
{
}

BOOL CDDApp::InitInstance(LPWSTR lpCmdLine)
{
	if (NULL != lpCmdLine)
	{
		m_strCmdLine = lpCmdLine;
	}
	// ��ʼ��LuaRuntime�ĵ��Խӿ�
#ifdef TSLOG
	//XLLRT_DebugInit("greenshield",XLLRT_DEBUG_TYPE_HOOK);
#else
	//XLLRT_DebugInit("ddnotepad",XLLRT_DEBUG_TYPE_NOHOOK);
#endif



	return IniEnv();
}


int __stdcall CDDApp::LuaErrorHandle(lua_State* luaState,const wchar_t* pExtInfo, const wchar_t* luaErrorString,PXL_LRT_ERROR_STACK pStackInfo)
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

int CDDApp::ExitInstance()
{
	TerminateProcess(GetCurrentProcess(), 0);
	return 0;
}



BOOL CDDApp::IniEnv()
{
	TCHAR szXar[MAX_PATH] = {0};
	GetModuleFileName((HMODULE)g_hInst, szXar, MAX_PATH);
	PathRemoveFileSpec(szXar);
	PathAppend(szXar, _T("..\\xar\\")); 
	if (!::PathFileExists(szXar) || !::PathIsDirectory(szXar) )
	{
		MessageBoxA(NULL,"��ȡ����Ƥ��·��ʧ��","����",MB_OK|MB_ICONERROR);
		return FALSE;
	}
	m_strXarPath = szXar;
	::OleInitialize(NULL); //��:����ע��RegisterDragDrop���ɹ�������E_OUTOFMEMORY����
	// 1)��ʼ��ͼ�ο�
	XLGraphicParam param;
	XL_PrepareGraphicParam(&param);
	param.textType = XLTEXT_TYPE_GDI;
	XL_InitGraphicLib(&param);
	//XL_SetFreeTypeEnabled(TRUE);
	
	//XLGraphicPlusParam plusParam;
	//XLGP_PrepareGraphicPlusParam(&plusParam);
	//XLGP_InitGraphicPlus(&plusParam);
	// 2)��ʼ��XLUE,�⺯����һ�����ϳ�ʼ������
	// ����˳�ʼ��Lua����,��׼����,XLUELoader�Ĺ���
	
	XLUE_InitLoader(NULL);
	XLLRT_ErrorHandle(CDDApp::LuaErrorHandle);

	if (!m_RegisterLuaAPI.Init())
	{
		return FALSE;
	}

	InternalLoadXAR();
	return TRUE;
}


void CDDApp::InternalLoadXAR()
{
	XLUE_AddXARSearchPath(m_strXarPath.c_str());
	if (XLUE_XARExist("ddnotepad"))
	{
		long iRet = XLUE_LoadXAR("ddnotepad");	//����ֵΪ0˵�����سɹ�
		TSDEBUG4CXX(L"XLUE_LoadXAR iret = " << iRet);
		if(iRet != 0)
		{
			TerminateProcess(GetCurrentProcess(), (UINT)-20);
		}
	}
	else
	{
		MessageBoxA(NULL,"�޷���ȡ����Ƥ��","����",MB_OK|MB_ICONERROR);
		TSDEBUG(_T("XLUE_XARExist ddnotepad) return FALSE"));
		TerminateProcess(GetCurrentProcess(), (UINT)-30);
	}
}


std::wstring CDDApp::GetCommandLine()
{
	return m_strCmdLine;
}