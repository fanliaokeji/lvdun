// FlyRabbit.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
//#include "resource.h"
#include "FRPretender.h"
CAppModule _Module;

int Run(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	int nRet = 0;
	CFRPretender Pretender;
	if (Pretender.Init(lpstrCmdLine))
	{
		nRet = theLoop.Run();
	}
	// 消息循环不应该结束，结束就是走了非正常流程。。。直接杀了
	TerminateProcess(GetCurrentProcess(),(UINT)-16);
	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	TSTRACEAUTO();

	//std::wstring strProdName, strProdId, strProdVer;

	HRESULT hRes = ::CoInitialize(NULL);
	// If you are running on NT 4.0 or higher you can use the following call instead to 
	// make the EXE free threaded. This means that calls come in on a random RPC thread.
	//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	//1: Vista:允许低权限程序向高权限发送Message
	// Vista Support {
	typedef BOOL (WINAPI FAR *ChangeWindowMessageFilter_PROC)(UINT,DWORD);
	ChangeWindowMessageFilter_PROC m_pfnChangeWindowMessageFilter;
	//#if(WINVER   >=   0x0600)   
#define   MSGFLT_ADD   1   
#define   MSGFLT_REMOVE   2   
	//#endif   /*   WINVER   >=   0x0600   */   
	// Vista Support }	
	m_pfnChangeWindowMessageFilter = (ChangeWindowMessageFilter_PROC)::GetProcAddress(::GetModuleHandle(_T("USER32")),"ChangeWindowMessageFilter");
	if (m_pfnChangeWindowMessageFilter)
	{
		m_pfnChangeWindowMessageFilter(WM_COPYDATA, MSGFLT_ADD);
	}	

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));
	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();
	return nRet;
}