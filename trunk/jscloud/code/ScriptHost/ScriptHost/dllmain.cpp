// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "ScriptHost_i.h"
#include "dllmain.h"
#include "dlldatax.h"

CScriptHostModule _AtlModule;
// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hInstance);
		TCHAR szPath[_MAX_PATH] = {0};
		GetModuleFileName(NULL, szPath, _MAX_PATH);
		LPCTSTR pszNames[] = { _T("rundll32.exe") };
		bool bMatch = false;
		LPTSTR lpszFindName = szPath;
		lpszFindName += (int)_tcslen(szPath);
		while('\\' != *lpszFindName && '\\' != *lpszFindName)
			lpszFindName--;
		lpszFindName++;
		int i = 0;
		for( ; i < sizeof pszNames / sizeof pszNames[0]; i++)
		{
			int n = _tcsnicmp( lpszFindName,  pszNames[i], _tcslen(pszNames[i]));
			if(0 == n)
			{
				bMatch = true;
				break;
			}			
		} 
		if(!bMatch)
			return FALSE;
		if(IsDebugging())
			return FALSE; 
	}
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(hInstance, dwReason, lpReserved))
		return FALSE;
#endif
	hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
