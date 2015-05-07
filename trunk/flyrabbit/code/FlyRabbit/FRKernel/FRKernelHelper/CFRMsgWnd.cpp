#include "stdafx.h"
#include "XLFS.h"
#include "XLUE.h"
#include "XLGraphicPlus.h"


#include ".\CFRMsgWnd.h"

#include "shellapi.h"
#include "atlapp.h"
#include "atldlgs.h"
#include <atlconv.h>
#include "Wininet.h"
#include <fstream>
#pragma comment(lib, "Wininet.lib")
#include "..\FRApp.h"

CFRMsgWindow::CFRMsgWindow(void)
{
	TSAUTO();
	m_hMutex = NULL;
}

CFRMsgWindow::~CFRMsgWindow(void)
{
	TSAUTO();	
}

bool CFRMsgWindow::HandleSingleton()
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
		_sntprintf(szMutex, _MAX_PATH, _T("#mutex%s_%s"), _T("FR_{1DA1F328-DB3D-4f6a-A62E-E9B2B22F2B9A}"),_T("1.0"));
		m_hMutex = CreateMutex(NULL, true, szMutex);
		bool bExist = (ERROR_ALREADY_EXISTS == ::GetLastError() || ERROR_ACCESS_DENIED == ::GetLastError());
		if(bExist)
		{
			TSERROR4CXX("startup failed");				//	ATLASSERT(false && "WaitForSingleObject ǰ");//
			WaitForSingleObject(m_hMutex, INFINITE);//���������̵�������ڽ���
			
			HWND hWnd = ::FindWindow(_T("{1DA1F328-DB3D-4f6a-A62E-E9B2B22F2B9A}_frmainmsg"), NULL);
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
			//ATLASSERT(false && "ReleaseMutex ǰ");//
			ReleaseMutex(m_hMutex);
			//ATLASSERT(false && "ReleaseMutex ��");//
			CloseHandle(m_hMutex);		
			return true;
		}
	}	

	if (m_hWnd == NULL)
		Create(HWND_MESSAGE);
	
	::SendMessage(m_hWnd, WM_COPYDATA, 0, (LPARAM) (LPVOID) &cds );
	if(m_hMutex)
		ReleaseMutex(m_hMutex);//ʹ�� WaitForSingleObject ������ȥ
	return false;
}

int CFRMsgWindow::AttachListener(DWORD userdata1,DWORD userdata2,funResultCallBack pfn, const void* pfun)
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

int CFRMsgWindow::DetachListener(DWORD userData1, const void* pfun)
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

//wParam:0��ʾ�����У�1��ʾAgent
LRESULT CFRMsgWindow::OnCopyData(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	TSAUTO();
	USES_CONVERSION;
	COPYDATASTRUCT * pcs = (COPYDATASTRUCT *)lParam;
	if(wParam == 0)
	{
		LPCWSTR pcszCommandLine = (LPCWSTR)pcs->lpData;
		TSDEBUG4CXX(" commandline : "<<pcszCommandLine);	
		if(pcszCommandLine && wcslen(pcszCommandLine) > 0)
		{
			CComVariant vParam[1];
			vParam[0] = (LPWSTR)pcszCommandLine;

			DISPPARAMS params = { vParam, NULL, 1, 0 };
			Fire_LuaEvent("OnCommandLine", &params);
		}	
	}
	else if (wParam == 1)
	{
		FRBrowserTaskInfo * info = (FRBrowserTaskInfo*)pcs->lpData;
		if (info->wszUrl && wcslen(info->wszUrl) > 0)
		{
			CComVariant vParam[4];
			vParam[0] = (LPWSTR)(info->wszUrl);
			vParam[1] = (int)(info->type);
			vParam[2] = (int)(info->posX);
			vParam[2] = (int)(info->posY);
			DISPPARAMS params = { vParam, NULL, 4, 0 };
			Fire_LuaEvent("OnAddTask", &params);
		}
	}

	return 0;
}
