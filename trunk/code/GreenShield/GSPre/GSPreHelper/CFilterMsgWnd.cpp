#include "stdafx.h"
#include "XLFS.h"
#include "XLUE.h"
#include "XLGraphicPlus.h"


#include ".\CFilterMsgWnd.h"

#include "shellapi.h"
#include "atlapp.h"
#include "atldlgs.h"
#include <atlconv.h>
#include "Wininet.h"
#include <fstream>
#pragma comment(lib, "Wininet.lib")
#include "..\GSNotifyIcon.h"
#include "..\GSApp.h"
extern CGSApp theApp;


CFilterMsgWindow::CFilterMsgWindow(void)
{
	TSAUTO();
	m_hMutex = NULL;
}

CFilterMsgWindow::~CFilterMsgWindow(void)
{
	TSAUTO();	
}

bool CFilterMsgWindow::HandleSingleton()
{
	TSAUTO();
#define APP_DS_MAGIC 0x0802
	LPWSTR lpstrCmdLine = ::GetCommandLineW();
	COPYDATASTRUCT cds = {0};
	cds.dwData = APP_DS_MAGIC;          // function identifier
	cds.cbData =(int) sizeof( WCHAR ) * ((int)wcslen(lpstrCmdLine) + 1) ;  // size of data
	cds.lpData = lpstrCmdLine;     
	
	UINT iSingleTon = 1;
	if(iSingleTon)
	{
		static TCHAR szMutex[_MAX_PATH] = {0};
		_sntprintf(szMutex, _MAX_PATH, _T("#mutex%s_%s"), _T("DS_{283D6A81-4D36-4f09-A297-AC1ADE33F18A}"),_T("1.0"));
		m_hMutex = CreateMutex(NULL, true, szMutex);
		bool bExist = (ERROR_ALREADY_EXISTS == ::GetLastError() || ERROR_ACCESS_DENIED == ::GetLastError());
		if(bExist)
		{
			TSERROR4CXX("startup failed");				//	ATLASSERT(false && "WaitForSingleObject 前");//
			WaitForSingleObject(m_hMutex, INFINITE);//等其它进程的这个窗口建完
			
			HWND hWnd = ::FindWindow(_T("{B239B46A-6EDA-4a49-8CEE-E57BB352F933}_dsmainmsg"), NULL);
			if(hWnd)
			{
				if(!SendMessageTimeout(hWnd, WM_COPYDATA, (WPARAM)0, (LPARAM) (LPVOID) &cds, SMTO_ABORTIFHUNG, 10000, NULL))
				{
					DWORD dwProcessID = 0;
					::GetWindowThreadProcessId(hWnd, &dwProcessID);
					HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);
					TerminateProcess(hProcess, (UINT)10000);
										 
					STARTUPINFO si;
					PROCESS_INFORMATION pi;
					ZeroMemory( &si, sizeof(si) );
					si.cb = sizeof(si);					
					
					ZeroMemory( &pi, sizeof(pi) );
					// Start the child process. 
					CreateProcess( NULL,         // No module name (use command line). 
						(LPTSTR)lpstrCmdLine, // Command line. 
						NULL,                         // Process handle not inheritable. 
						NULL,						  // Thread handle not inheritable. 
						FALSE,						  // Set handle inheritance to FALSE. 
						0,							  // No creation flags. 
						NULL,						  // Use parent's environment block. 
						NULL,						  // Use parent's starting directory. 
						&si,						  // Pointer to STARTUPINFO structure.
						&pi );						  // Pointer to PROCESS_INFORMATION structure.
				}				
			}
			//ATLASSERT(false && "ReleaseMutex 前");//
			ReleaseMutex(m_hMutex);
			//ATLASSERT(false && "ReleaseMutex 后");//
			CloseHandle(m_hMutex);		
			return true;
		}
	}	

	if (m_hWnd == NULL)
		Create(HWND_MESSAGE);
	
	::SendMessage(m_hWnd, WM_COPYDATA, 0, (LPARAM) (LPVOID) &cds );
	if(m_hMutex)
		ReleaseMutex(m_hMutex);//使得 WaitForSingleObject 运行下去
	return false;
}

int CFilterMsgWindow::AttachListener(DWORD userdata1,DWORD userdata2,funResultCallBack pfn, const void* pfun)
{
	TSAUTO();
	CallbackNode newNode = {0};
	newNode.pCallBack = pfn;
	newNode.userData1 = userdata1;
	newNode.userData2 = userdata2;
	newNode.luaFunction = pfun;
	int k = m_allCallBack.size();
	m_allCallBack.push_back(newNode);
	TSDEBUG4CXX(" userdata1 : "<<newNode.userData1<<" userData2 : "<< newNode.userData2<<", pfn : "<<pfn << ", pfun : " << pfun);
	 
	return 0;
}

int CFilterMsgWindow::DetachListener(DWORD userData1, const void* pfun)
{
	TSAUTO();
	std::vector<CallbackNode>::iterator it = m_allCallBack.begin();
	for(; it != m_allCallBack.end(); it++)
	{
		if(it->luaFunction == pfun)
		{
			luaL_unref((lua_State *)(ULONG_PTR)userData1, LUA_REGISTRYINDEX, (int)it->userData2);
			m_allCallBack.erase(it);
			break;
		}
	}
	return 0;
}

LRESULT CFilterMsgWindow::OnCopyData(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	TSAUTO();
	USES_CONVERSION;
	COPYDATASTRUCT * pcs = (COPYDATASTRUCT *)lParam;
	LPCWSTR pcszCommandLine = (LPCWSTR)pcs->lpData;
	TSDEBUG4CXX(" commandline : "<<pcszCommandLine);	
	if(pcszCommandLine && wcslen(pcszCommandLine) > 0)
	{
		CComVariant vParam[1];
		vParam[0] = (LPWSTR)pcszCommandLine;

		DISPPARAMS params = { vParam, NULL, 1, 0 };
		Fire_LuaEvent("OnCommandLine", &params);
	}	

	return 0;
}

LRESULT CFilterMsgWindow::HandleFilterResult(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LPSTR pUrl = (LPSTR)lParam;
	std::wstring wstrUrl;
	AnsiStringToWideString(pUrl,wstrUrl);
	delete pUrl;
	CComVariant vParam[2];
	vParam[0] = (int)wParam;
	vParam[1] = (LPWSTR)wstrUrl.c_str();

	DISPPARAMS params = { vParam, NULL, 2, 0 };
	//CallLuaMethod("OnFilterResult",&params);
	Fire_LuaEvent("OnFilterResult", &params);
	return 0;
}
LRESULT CFilterMsgWindow::HandleFilterAsk(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LPSTR pHost = (LPSTR)wParam;
	std::wstring wstrHost;
	AnsiStringToWideString(pHost,wstrHost);
	delete pHost;

	LPSTR pUrl = (LPSTR)lParam;
	std::wstring wstrUrl;
	AnsiStringToWideString(pUrl,wstrUrl);
	delete pUrl;
	CComVariant vParam[2];
	vParam[0] = (LPWSTR)wstrHost.c_str();
	vParam[1] = (LPWSTR)wstrUrl.c_str();

	DISPPARAMS params = { vParam, NULL, 2, 0 };
	//CallLuaMethod("OnFilterASK", &params);
	Fire_LuaEvent("OnFilterASK", &params);
	return 0;
}


LRESULT CFilterMsgWindow::HandleFilterExit(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	gsNotifyIcon.Hide();
	theApp.ExitInstance();
	return 0;
}

LRESULT CFilterMsgWindow::HandleFilterLocking(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	CComVariant vParam[1];
	vParam[0] = (int)wParam;
	DISPPARAMS params = { vParam, NULL, 1, 0 };
	Fire_LuaEvent("OnFilterLocking", &params);
	return 0;
}

LRESULT CFilterMsgWindow::HandleRedirectResult(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LPSTR pUrl = (LPSTR)lParam;
	std::wstring wstrUrl;
	AnsiStringToWideString(pUrl,wstrUrl);
	delete pUrl;
	CComVariant vParam[2];
	vParam[0] = (int)wParam;
	vParam[1] = (LPWSTR)wstrUrl.c_str();

	DISPPARAMS params = { vParam, NULL, 2, 0 };
	//CallLuaMethod("OnFilterResult",&params);
	Fire_LuaEvent("OnRedirectResult", &params);
	return 0;
}


LRESULT CFilterMsgWindow::HandleFilterVideo(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LPSTR pUrl = (LPSTR)lParam;
	std::wstring wstrUrl;
	AnsiStringToWideString(pUrl,wstrUrl);
	delete pUrl;
	CComVariant vParam[2];
	vParam[0] = (int)wParam;
	vParam[1] = (LPWSTR)wstrUrl.c_str();

	DISPPARAMS params = { vParam, NULL, 2, 0 };
	Fire_LuaEvent("OnFilterVideo", &params);
	return 0;
}