// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "OfficeAddin_i.h"
#include "dllmain.h"

COfficeAddinModule _AtlModule;
HINSTANCE g_hModule = NULL;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_hModule = hInstance;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
