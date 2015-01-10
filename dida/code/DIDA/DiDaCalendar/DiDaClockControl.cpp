#include "stdafx.h"
#include "DiDaClockControl.h"
#include <Shellapi.h>
#include <Shlwapi.h>
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

#define DDCMD_SHOWCALENDARMAIN		0
#define DDCMD_UPDATE				1
#define DDCMD_SHOWABOUT				2

void DiDaClockControl::LaunchCalendarMain(HWND hWnd)
{
	Command(DDCMD_SHOWCALENDARMAIN);
}

void DiDaClockControl::Update(HWND hWnd)
{
	Command(DDCMD_UPDATE);
}

void DiDaClockControl::ShowAbout(HWND hWnd)
{
	Command(DDCMD_SHOWABOUT);
}

static DWORD WINAPI FreeSelf(LPVOID param)
{
	if(param != NULL) {
		HWND hWnd = ::FindWindow(L"{10808D97-3494-4c5d-857F-0ADFA04FA721}_ddmainmsg", NULL);
		if(hWnd != NULL) {
			DWORD dwProcessId = 0;
			::GetWindowThreadProcessId(hWnd, &dwProcessId);
			HANDLE hCalendarMainProcess = NULL;
			if(dwProcessId != 0) {
				hCalendarMainProcess = ::OpenProcess(SYNCHRONIZE, FALSE, dwProcessId);
			}
			::PostMessage(hWnd, WM_USER + 200, (WPARAM)3, NULL);
			if(hCalendarMainProcess != NULL) {
				::WaitForSingleObject(hCalendarMainProcess, 10000);
				::CloseHandle(hCalendarMainProcess);
			}
		}
	}
	FreeLibraryAndExitThread(g_hModule, 0);
}

void DiDaClockControl::ExitCalendar(HWND hWnd, bool sendExitMsg)
{
	LPVOID lpParameter = NULL;
	if(sendExitMsg) {
		lpParameter = (LPVOID)1;
	}
	::CloseHandle(::CreateThread(NULL, 0, FreeSelf, lpParameter, 0, NULL));
}

bool DiDaClockControl::IsDiDaCalendarStartRunEnable()
{
	HKEY hKey = NULL;
	LONG lOpenResult = ::RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ, &hKey);
	if(lOpenResult != ERROR_SUCCESS) {
		return false;
	}
	const wchar_t* key_name = L"DDCalendar";
	wchar_t path[MAX_PATH * 2];
	DWORD cbData = sizeof(path);
	DWORD dwRegType = REG_SZ;
	LONG lQueryValueResult = ::RegQueryValueEx(hKey, key_name, NULL, &dwRegType, reinterpret_cast<LPBYTE>(path), &cbData);
	::RegCloseKey(hKey);
	return lQueryValueResult == ERROR_SUCCESS;
}

std::wstring DiDaClockControl::GetDiDaCalendarFilePath()
{
	const wchar_t *szSubKey = L"SOFTWARE\\DDCalendar";
	HKEY hKey = NULL;
	LONG lOpenResult = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSubKey, 0, KEY_WOW64_32KEY | KEY_READ, &hKey);
	if(lOpenResult != ERROR_SUCCESS) {
		return std::wstring();
	}
	DWORD dwRegType = REG_SZ;
	wchar_t path[MAX_PATH * 2];
	DWORD dwcbData = sizeof(path);
	LONG lQueryValueResult = ::RegQueryValueEx(hKey, L"path", NULL, &dwRegType, reinterpret_cast<LPBYTE>(path), &dwcbData);
	::RegCloseKey(hKey);
	if(lQueryValueResult == ERROR_SUCCESS && dwRegType == REG_SZ) {
		return std::wstring(path);
	}
	return std::wstring();
}

void DiDaClockControl::EnableDiDaCalendarStartRun(bool enable)
{
	const wchar_t* key_name = L"DDCalendar";
	if(enable) {
		std::wstring filepath = GetDiDaCalendarFilePath();
		if(filepath.empty()) {
			return;
		}
		HKEY hKey = NULL;
		LONG lOpenResult = ::RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey);
		if(lOpenResult != ERROR_SUCCESS) {
			return;
		}
		std::wstring value = L"\"";
		value += filepath;
		value += L"\"  /embedding /sstartfrom sysboot";

		::RegSetValueEx(hKey, key_name, 0, REG_SZ, reinterpret_cast<const BYTE*>(value.data()), (value.size() + 1) * sizeof(wchar_t)); 
		::RegCloseKey(hKey);
	}
	else {
		HKEY hKey = NULL;
		LONG lOpenResult = ::RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey);
		if(lOpenResult != ERROR_SUCCESS) {
			return;
		}
		::RegDeleteValue(hKey, key_name);
		::RegCloseKey(hKey);
	}
}

void DiDaClockControl::Command(int cmd)
{
	HWND hWnd = ::FindWindow(L"{10808D97-3494-4c5d-857F-0ADFA04FA721}_ddmainmsg", NULL);
	if (hWnd != NULL) {
		if (cmd == DDCMD_SHOWCALENDARMAIN) {
			::PostMessage(hWnd, WM_USER + 200, (WPARAM)0, NULL);
		}
		else if (cmd == DDCMD_UPDATE) {
			::PostMessage(hWnd, WM_USER + 200, (WPARAM)1, NULL);
		}
		else if (cmd == DDCMD_SHOWABOUT) {
			::PostMessage(hWnd, WM_USER + 200, (WPARAM)2, NULL);
		}
	}
	else {
		const wchar_t* lpParameters = NULL;
		if (cmd == DDCMD_SHOWCALENDARMAIN) {
			lpParameters = L"/startfrom explorer";
		}
		else if (cmd == DDCMD_UPDATE) {
			lpParameters = L"/embedding /update";
		}
		else if (cmd == DDCMD_SHOWABOUT) {
			lpParameters = L"/embedding /about";
		}
		else {
			return;
		}
		std::wstring path = GetDiDaCalendarFilePath();
		if(path.empty() || ::PathFileExists(path.c_str()) == FALSE) {
			return;
		}
		ShellExec(path.c_str(), lpParameters, SW_NORMAL);
	}
}
