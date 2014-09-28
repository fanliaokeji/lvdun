// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "WinsockHooker.h"
#include <tchar.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			TCHAR szPath[MAX_PATH] = {0};
			GetModuleFileName(NULL, szPath, MAX_PATH);
			_tcslwr_s(szPath);
			if (_tcsstr(szPath,  L"firefox.exe")
				|| _tcsstr(szPath, L"chrome.exe")
				|| _tcsstr(szPath, L"iexplore.exe")) {
				WinsockHooker::AttachHook();	
			}
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		WinsockHooker::DetachHook();
		break;
	}
	return TRUE;
}

