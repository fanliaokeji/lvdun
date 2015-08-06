// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "BhoAddin_i.h"
#include "dllmain.h"

CBhoAddinModule _AtlModule;
HINSTANCE g_hThisModule = NULL;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (!g_hThisModule)
	{
		g_hThisModule = hInstance;
	}
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
