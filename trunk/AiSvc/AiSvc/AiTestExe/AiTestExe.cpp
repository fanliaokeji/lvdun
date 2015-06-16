// AiTestExe.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>

#define DLL_NAME_L L"didaupdate.dll"
#ifdef DIDARILI_0000
#define DLL_NAME_L L"didaupdate.dll"
#endif
#ifdef LVDUN_0000
#define DLL_NAME_L L"gsupdate.dll"
#endif
#ifdef WE
#define DLL_NAME_L L"weupdate.dll"
#endif
#ifdef FR
#define DLL_NAME_L L"frupdate.dll"
#endif

int _tmain(int argc, _TCHAR* argv[])
{
	typedef int (*pfRun)();
	HMODULE hDll = LoadLibrary(DLL_NAME_L);
	pfRun pf = (pfRun)GetProcAddress(hDll, "Run");
	pf();
	return 0;
}

