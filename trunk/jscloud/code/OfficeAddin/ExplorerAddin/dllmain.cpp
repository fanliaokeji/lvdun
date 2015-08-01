// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "ExplorerAddin_i.h"
#include "dllmain.h"

CExplorerAddinModule _AtlModule;

BOOL IsThisDllLoadedByProcessNamedWith(LPCTSTR pszProcessName);

HINSTANCE g_hThisModule = NULL;
BOOL g_bIsLoadedByExplorer = FALSE;
BOOL g_bIsLoadedByNewExplorer = FALSE;
// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (!g_hThisModule)
	{
		g_hThisModule = hInstance;

		// 第一次进入时，判断试图加载本dll的进程是否是explorer.exe
		if (IsThisDllLoadedByProcessNamedWith(_T("explorer.exe")))
		{
			g_bIsLoadedByExplorer = TRUE;
		}
	}
	if (g_bIsLoadedByExplorer)
	{
		// 至此，加载本dll的进程必是explorer.exe
		static HANDLE s_hMutex = NULL; // 注意:本互斥量仅仅起到只让一个explorer.exe加载的目的,没有试图占有,也就不需要释放
		LPCTSTR pszNameOfMutex = _T("{C4E38FFE-4D3D-447a-9DA1-68BE7EACB921}");
		switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			{
				s_hMutex = ::CreateMutex(NULL, FALSE, pszNameOfMutex); // 创建之时并不试图占有它
				DWORD dwLastError = ::GetLastError();
				if (!s_hMutex)
				{
					return FALSE;
				}
				else
				{
					if (ERROR_ALREADY_EXISTS == dwLastError)
					{
						// 本dll已经被explorer.exe的其他实例加载过，因此不再加载
						// 关闭之前的互斥量
						::CloseHandle(s_hMutex);
						s_hMutex = NULL;
						g_bIsLoadedByNewExplorer = TRUE;
						return FALSE;
					}

					// 至此，本dll是被explorer.exe的第一个实例加载，这正是我要的
				}
			}
			break;

		case DLL_PROCESS_DETACH:
			{
				// 关闭之前创建的互斥量
				if (s_hMutex)
				{
					::CloseHandle(s_hMutex);
					s_hMutex = NULL;
					//TSDEBUG4CXX(_T("Close the mutex"));
				}
			}
			break;

		case DLL_THREAD_ATTACH:
			{
				// 什么也不做
			}
			break;

		case DLL_THREAD_DETACH:
			{
				// 什么也不做
			}
			break;

		default:
			{
				// 什么也不做
			}
			break;
		}
	}
	return _AtlModule.DllMain(dwReason, lpReserved); 
}


BOOL IsThisDllLoadedByProcessNamedWith(LPCTSTR pszProcessName)
{
	BOOL bIsLoadedByTheProcess = FALSE;

	if (pszProcessName)
	{
		// （1）获取加载本dll的进程名
		TCHAR szExeFileName[MAX_PATH] = {0};
		::GetModuleFileName(::GetModuleHandle(NULL), szExeFileName, sizeof(szExeFileName) / sizeof(szExeFileName[0]));
		::PathStripPath(szExeFileName);
		if (_tcsncicmp(szExeFileName, pszProcessName, _tcslen(pszProcessName)) == 0)
		{
			bIsLoadedByTheProcess = TRUE;
		}
	}

	return bIsLoadedByTheProcess;
}