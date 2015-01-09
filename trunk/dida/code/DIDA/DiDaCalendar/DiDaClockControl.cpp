#include "stdafx.h"
#include "DiDaClockControl.h"
#include "Shellapi.h"
#include <string>
#include <cstring>
#include <UxTheme.h>

extern HMODULE g_hModule;

static BOOL ShellExec(const wchar_t* lpFile, const wchar_t* lpParameters, int nShow)
{
	SHELLEXECUTEINFO sei;
	std::memset(&sei, 0, sizeof(SHELLEXECUTEINFO));
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.lpFile = lpFile;
	sei.lpParameters = lpParameters;
	sei.nShow = nShow;
	return static_cast<int>(ShellExecuteEx(&sei)) > 32;
}

void DiDaClockControl::ModifySystemDateTime()
{
	wchar_t cplPath[MAX_PATH];
	if(::ExpandEnvironmentStrings(L"%windir%\\System32\\timedate.cpl", cplPath, MAX_PATH) != 0) {
		std::wstring parameters(L"Shell32.dll,Control_RunDLL \"");
		parameters += cplPath;
		parameters.push_back(L'"');
		ShellExec(L"rundll32.exe", parameters.c_str(), SW_SHOWNORMAL);
	}
}

void DiDaClockControl::LaunchCalendarMain()
{
	::MessageBox(NULL, L"拉起嘀嗒日历", L"提示", MB_OK);
}

static DWORD WINAPI FreeSelf(LPVOID param)
{
	FreeLibraryAndExitThread(g_hModule, 0);
}

void DiDaClockControl::ExitCalendar(HWND hWnd)
{
	::CloseHandle(::CreateThread(NULL, 0, FreeSelf, NULL, 0, NULL));
}
