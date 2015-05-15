// TipWndUtility.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
//#include "FilterMsgPrc.h"
#include "GSApp.h"
#include ".\GSPreHelper\CFilterMsgWnd.h"
CGSApp theApp;

HANDLE g_hInst = NULL;

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	g_hInst = hModule;
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

STDAPI_(BOOL) InitXLUE(wchar_t* lpCmdLine)
{
	TSAUTO();
	BOOL bRet = theApp.InitInstance(lpCmdLine);
	if (!bRet)
	{
		TSDEBUG4CXX(L"InitInstance error, exit!");
		theApp.ExitInstance();
	}
	return bRet;
}

STDAPI UnInitXLUE()
{
	TSAUTO();	
	theApp.ExitInstance();
	return 0;
}


STDAPI HandleSingleton()
{
	TSAUTO();
	bool b = CFilterMsgWindow::Instance()->HandleSingleton();
	return b ? S_OK : E_FAIL;
}