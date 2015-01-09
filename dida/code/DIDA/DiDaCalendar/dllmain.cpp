// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <Windows.h>
#include "DidaClock.h"
#include <string>

static class DiDaCalendarModule {
	HANDLE hMutex;
public:
	DiDaCalendarModule() : hMutex(NULL) {
		wchar_t path[MAX_PATH];
		::GetModuleFileName(NULL, path, MAX_PATH);
		size_t length = std::wcslen(path);
		size_t offset = length;
		for(;offset != 0 && path[offset - 1] != '\\'; --offset)
			;
		if(_wcsicmp(path + offset, L"explorer.exe") == 0) {
			this->hMutex = ::CreateMutex(NULL, FALSE, L"DidaCalendar_{58C192A2-F432-48A1-96D4-F0629A6F6B80}");
			if(this->hMutex != NULL && ::GetLastError() == ERROR_ALREADY_EXISTS) {
				::CloseHandle(this->hMutex);
				this->hMutex = NULL;
			}
		}
	}

	bool CanLoad() const {
		return this->hMutex != NULL;
	}

	~DiDaCalendarModule() {
		if(this->hMutex != NULL) {
			::CloseHandle(this->hMutex);
			this->hMutex = NULL;
		}
	}
} CalendarModule;

HMODULE g_hModule = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		g_hModule = hModule;
		if(!CalendarModule.CanLoad()) {
			return FALSE;
		}
		if(!DiDaClock::InitClock()) {
			return FALSE;
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		DiDaClock::RestoreClockWndProc();
		break;
	}
	return TRUE;
}

