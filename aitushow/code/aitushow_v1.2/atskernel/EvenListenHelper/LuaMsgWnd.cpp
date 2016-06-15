#include "stdafx.h"
#include "XLFS.h"
#include "XLUE.h"
#include "XLGraphicPlus.h"


#include "LuaMsgWnd.h"

#include "shellapi.h"
#include "atlapp.h"
#include "atldlgs.h"
#include <atlconv.h>
#include "Wininet.h"
#include <fstream>
#pragma comment(lib, "Wininet.lib")
#include "..\XLUEApplication.h"

LuaMsgWindow::LuaMsgWindow(void)
{
	TSAUTO();
	m_hMutex = NULL;
}

LuaMsgWindow::~LuaMsgWindow(void)
{
	TSAUTO();	
}

bool LuaMsgWindow::HandleSingleton()
{
	TSAUTO();
#define APP_DD_MAGIC 0x0802
	LPWSTR lpstrCmdLine = ::GetCommandLineW();
	COPYDATASTRUCT cds = {0};
	cds.dwData = APP_DD_MAGIC;          // function identifier
	cds.cbData =(int) sizeof( WCHAR ) * ((int)wcslen(lpstrCmdLine) + 1) ;  // size of data
	cds.lpData = lpstrCmdLine;     
	
	UINT iSingleTon = 1;
	if(iSingleTon)
	{
		static TCHAR szMutex[_MAX_PATH] = {0};
		_sntprintf(szMutex, _MAX_PATH, _T("#mutex%s_%s"), LUA_MSG_MUTEX,_T("1.0"));
		m_hMutex = CreateMutex(NULL, true, szMutex);
		bool bExist = (ERROR_ALREADY_EXISTS == ::GetLastError() || ERROR_ACCESS_DENIED == ::GetLastError());
		if(bExist)
		{
			TSERROR4CXX("startup failed");				//	ATLASSERT(false && "WaitForSingleObject 前");//
			WaitForSingleObject(m_hMutex, INFINITE);//等其它进程的这个窗口建完
			
			HWND hWnd = ::FindWindow(LUA_MSG_WND_CALSS, NULL);
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
			ReleaseMutex(m_hMutex);
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

int LuaMsgWindow::AttachListener(DWORD userdata1,DWORD userdata2,funResultCallBack pfn, const void* pfun)
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

int LuaMsgWindow::DetachListener(DWORD userData1, const void* pfun)
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

LRESULT LuaMsgWindow::OnCopyData(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
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


void LuaMsgWindow::SetKeyboardHook()
{
	if (NULL==m_hKeyboardHook)
	{
		m_hKeyboardHook = ::SetWindowsHookEx(WH_KEYBOARD, LuaMsgWindow::Instance()->KeyboardProc, NULL, GetCurrentThreadId()); 	
	}
	return ;
}

void LuaMsgWindow::DelKeyboardHook()
{	
	if (m_hKeyboardHook)
	{
		::UnhookWindowsHookEx(m_hKeyboardHook);	
		m_hKeyboardHook = NULL;
	}
	return ;
}

LRESULT CALLBACK LuaMsgWindow::KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	if(HC_NOREMOVE == code)
		return CallNextHookEx (LuaMsgWindow::Instance()->m_hKeyboardHook, code, wParam, lParam);
	HWND hFocus = ::GetActiveWindow();

	
	CComVariant avarParams[5];
	avarParams[1] = CComVariant((LONG)wParam);
	avarParams[0] = CComVariant((LONG)(ULONG_PTR)hFocus);
	TSDEBUG4CXX("VKey : "<<(const unsigned int)wParam);
	CComVariant varResult;
	DISPPARAMS params = { avarParams, NULL, 2, 0 };
	if(lParam & 0x80000000) //弹起
	{
		LuaMsgWindow::Instance()->Fire_LuaEvent("OnKeyUp", &params);
	}
	else
	{
		LuaMsgWindow::Instance()->Fire_LuaEvent("OnKeyDown", &params);
	}

	return CallNextHookEx (LuaMsgWindow::Instance()->m_hKeyboardHook, code, wParam, lParam);
}

void LuaMsgWindow::CloseSingletonMutex()
{	
	if (m_hMutex)
	{
		ReleaseMutex(m_hMutex);
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}
}