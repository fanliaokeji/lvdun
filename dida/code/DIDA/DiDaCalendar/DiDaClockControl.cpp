#include "stdafx.h"
#include "DiDaClockControl.h"
#include <Shellapi.h>
#include <Shlwapi.h>
#include <string>
#include <cstring>
#include <UxTheme.h>
#include <tchar.h>

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


static bool IsVistaOrHigher()
{
	DWORD dwVersion = GetVersion();
	DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	return dwMajorVersion >= 6;
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
#define DDCMD_MENUEXIT				3

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

void DiDaClockControl::MenuExit(HWND hWnd)
{
	Command(DDCMD_MENUEXIT);
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
	//HKEY hKey = NULL;
	//LONG lOpenResult = ::RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ, &hKey);
	//if(lOpenResult != ERROR_SUCCESS) {
	//	return false;
	//}
	//const wchar_t* key_name = L"mycalendar";
	//wchar_t path[MAX_PATH * 2];
	//DWORD cbData = sizeof(path);
	//DWORD dwRegType = REG_SZ;
	//LONG lQueryValueResult = ::RegQueryValueEx(hKey, key_name, NULL, &dwRegType, reinterpret_cast<LPBYTE>(path), &cbData);
	//::RegCloseKey(hKey);
	//return lQueryValueResult == ERROR_SUCCESS;

	HKEY hKey = NULL;
	LONG lOpenResult = ::RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\mycalendar", 0, KEY_READ, &hKey);
	if(lOpenResult != ERROR_SUCCESS) {
		return false;
	}
	const wchar_t* key_name = L"setboot";
	DWORD dwSetBoot = 0;
	DWORD cbData = sizeof(DWORD);
	DWORD dwRegType = REG_DWORD;
	if (ERROR_SUCCESS !=  ::RegQueryValueEx(hKey, key_name, NULL, &dwRegType, reinterpret_cast<LPBYTE>(&dwSetBoot), &cbData))
	{
		::RegCloseKey(hKey);
		return false;
	}
	::RegCloseKey(hKey);
	return dwSetBoot == 1;

}

std::wstring DiDaClockControl::GetDiDaCalendarFilePath()
{
	const wchar_t *szSubKey = L"SOFTWARE\\mycalendar";
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

std::wstring DiDaClockControl::GetFixARPath()
{
	const wchar_t *szSubKey = L"SOFTWARE\\mycalendar";
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
	if(lQueryValueResult != ERROR_SUCCESS || dwRegType != REG_SZ) {
		return std::wstring();
	}
	std::wstring wstrFixARPath = path;
	std::size_t last = wstrFixARPath.find_last_of(L"\\");
	if (last == std::wstring::npos)
	{
		return std::wstring();
	}
	wstrFixARPath = wstrFixARPath.substr(0,last+1) + L"ddfixar.exe";
	return wstrFixARPath;
}

void DiDaClockControl::EnableDiDaCalendarStartRun(bool enable)
{
	//const wchar_t* key_name = L"mycalendar";
	//if(enable) {
	//	std::wstring filepath = GetDiDaCalendarFilePath();
	//	if(filepath.empty()) {
	//		return;
	//	}
	//	HKEY hKey = NULL;
	//	LONG lOpenResult = ::RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey);
	//	if(lOpenResult != ERROR_SUCCESS) {
	//		return;
	//	}
	//	std::wstring value = L"\"";
	//	value += filepath;
	//	value += L"\"  /embedding /sstartfrom sysboot";

	//	::RegSetValueEx(hKey, key_name, 0, REG_SZ, reinterpret_cast<const BYTE*>(value.data()), (value.size() + 1) * sizeof(wchar_t)); 
	//	::RegCloseKey(hKey);
	//}
	//else {
	//	HKEY hKey = NULL;
	//	LONG lOpenResult = ::RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey);
	//	if(lOpenResult != ERROR_SUCCESS) {
	//		return;
	//	}
	//	::RegDeleteValue(hKey, key_name);
	//	::RegCloseKey(hKey);
	//}
	const wchar_t* key_name = L"setboot";
	std::wstring filepath = GetFixARPath();
	const wchar_t* lpParameters = NULL;
	if (enable)
	{
		HKEY hKey = NULL;
		LONG lOpenResult = ::RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\mycalendar", 0, KEY_SET_VALUE, &hKey);
		if(lOpenResult != ERROR_SUCCESS) {
			return;
		}
		DWORD dwSetBoot = 1;
		::RegSetValueEx(hKey, key_name, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&dwSetBoot), sizeof(DWORD));
		::RegCloseKey(hKey);
		lpParameters = L"-ranf";
		ShellExec(filepath.c_str(), lpParameters, SW_NORMAL);
	} 
	else
	{
		HKEY hKey = NULL;
		LONG lOpenResult = ::RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\mycalendar", 0, KEY_SET_VALUE, &hKey);
		if(lOpenResult != ERROR_SUCCESS) {
			return;
		}
		::RegDeleteValue(hKey, key_name);
		::RegCloseKey(hKey);
		lpParameters = L"-unran";
		ShellExec(filepath.c_str(), lpParameters, SW_NORMAL);
	}
}

bool DiDaClockControl::IsDDNotepadAssociationTxt()
{
	HKEY hCurrentKey = NULL;
	LONG lOpenResult = ERROR_SUCCESS;
	DWORD dwAssociated = 0;
	DWORD cbAssociated = sizeof(DWORD);
	DWORD dwRegDWORDType = REG_DWORD;
	lOpenResult = ::RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\mynotepad", 0, KEY_READ, &hCurrentKey);
	if (ERROR_SUCCESS != lOpenResult)
	{
		return false;
	}
	if (ERROR_SUCCESS != ::RegQueryValueEx(hCurrentKey, L"Associated", NULL, &dwRegDWORDType, reinterpret_cast<LPBYTE>(&dwAssociated), &cbAssociated))
	{
		::RegCloseKey(hCurrentKey);
		return false;
	}
	::RegCloseKey(hCurrentKey);
	if (1 != dwAssociated)
	{
		return false;
	}
	DWORD dwRegType = REG_SZ;
	wchar_t szValue[MAX_PATH] = {0};
	DWORD cbData = sizeof(szValue);

	HKEY hKey = NULL;
	
	if (IsVistaOrHigher())
	{
		lOpenResult = ::RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.txt\\UserChoice", 0, KEY_READ, &hKey);
	} 
	else
	{
		lOpenResult = ::RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.txt", 0, KEY_READ, &hKey);
	}
	if (ERROR_SUCCESS != lOpenResult)
	{
		return false;
	}
	
	const wchar_t* szValueName = L"Progid";
	
	if (ERROR_SUCCESS != ::RegQueryValueEx(hKey, szValueName, NULL, &dwRegType, reinterpret_cast<LPBYTE>(szValue), &cbData))
	{
		::RegCloseKey(hKey);
		return false;
	}

	if (wcsicmp(szValue,L"mytxtfile") == 0)
	{
		::RegCloseKey(hKey);
		return true;
	}
	::RegCloseKey(hKey);
	return false;
}

void DiDaClockControl::DDNotepadAssociationTxt(bool enable)
{
	HKEY hCurrentKey = NULL;
	LONG lOpenResult = ERROR_SUCCESS;
	lOpenResult = ::RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\mynotepad", 0, KEY_READ|KEY_WRITE, &hCurrentKey);
	if (ERROR_SUCCESS != lOpenResult)
	{
		lOpenResult = RegCreateKey(HKEY_CURRENT_USER,  L"SOFTWARE\\mynotepad", &hCurrentKey);
		if (ERROR_SUCCESS != lOpenResult)
		{
			return;
		}
	}
	if (!enable)
	{
		::RegDeleteValue(hCurrentKey,L"Associated");
		::RegCloseKey(hCurrentKey);
		return;
	}
	HKEY hTxtKey = NULL;

	LONG lTxtResult = ERROR_SUCCESS;
	if (IsVistaOrHigher())
	{
		lTxtResult = ::RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.txt\\UserChoice", 0, KEY_READ|KEY_WRITE, &hTxtKey);
	} 
	else
	{
		lTxtResult = ::RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.txt", 0, KEY_READ|KEY_WRITE, &hTxtKey);
	}

	//if (!enable)
	//{
	//	wchar_t szBackUp[MAX_PATH] = {0};
	//	DWORD cbData = sizeof(szBackUp);
	//	DWORD dwRegType = REG_SZ;

	//	
	//	if (ERROR_SUCCESS != lTxtResult)
	//	{
	//		return;
	//	}

	//	const wchar_t* szBackUp_ValueName = L"mynotepad_backup";
	//	if (ERROR_SUCCESS != ::RegQueryValueEx(hTxtKey, szBackUp_ValueName, NULL, &dwRegType, reinterpret_cast<LPBYTE>(szBackUp), &cbData))
	//	{
	//		LONG lRet = ::RegDeleteValue(hTxtKey,L"Progid");
	//	}
	//	else
	//	{
	//		::RegSetValueEx(hTxtKey, L"Progid", 0, REG_SZ, reinterpret_cast<const BYTE*>(szBackUp), (wcslen(szBackUp)) *sizeof(wchar_t)); 
	//	}
	//	::RegCloseKey(hTxtKey);
	//	return;
	//}
	
	HKEY hDDKey = NULL;
	if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_CLASSES_ROOT, L"mytxtfile", 0, KEY_READ, &hDDKey))
	{
		::RegCloseKey(hDDKey);
		wchar_t szValue[MAX_PATH] = {0};
		DWORD cbData = sizeof(szValue);
		DWORD dwRegType = REG_SZ;
		if (ERROR_SUCCESS == ::RegQueryValueEx(hTxtKey,  L"Progid", NULL, &dwRegType, reinterpret_cast<LPBYTE>(szValue), &cbData))
		{
			::RegSetValueEx(hTxtKey, L"mynotepad_backup", 0, REG_SZ, reinterpret_cast<const BYTE*>(szValue), (wcslen(szValue)) * sizeof(wchar_t));		
		}
		wchar_t *szAssociation = L"mytxtfile";
		::RegSetValueEx(hTxtKey, L"Progid", 0, REG_SZ, reinterpret_cast<const BYTE*>(szAssociation), (wcslen(szAssociation)) * sizeof(wchar_t));	
		::RegCloseKey(hTxtKey);
		
		DWORD dwAssociated = 1;
		::RegSetValueEx(hCurrentKey, L"Associated", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&dwAssociated), sizeof(DWORD));
		::RegCloseKey(hCurrentKey);
		return;
	}
	//HKEY_CLASSES_ROOT\mytxtfile 不存在 拉起进程另处理
	std::wstring strDIDAPath = GetDiDaCalendarFilePath();
	if(strDIDAPath.empty()) {
		return;
	}
	size_t nPos = strDIDAPath.rfind(L"\\");
	if (nPos == std::wstring::npos)
	{
		return ;
	}
	std::wstring strDir = strDIDAPath.substr(0,nPos+1);
	wchar_t szDDNotePad[MAX_PATH] = {0};
	::PathCombine(szDDNotePad,strDir.c_str(),L"mynotepad.exe");
	if (!::PathFileExistsW(szDDNotePad))
	{
		return ;
	}
	wchar_t *szOpera = NULL;
	if (IsVistaOrHigher())
	{
		szOpera = L"runas";
	} 
	else
	{
		szOpera = L"open";
	}
	std::wstring strParam = L"/sstartfrom explorer /association 1";
	::ShellExecute(NULL,szOpera,szDDNotePad,strParam.c_str(),NULL,SW_HIDE);
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
		else if (cmd == DDCMD_MENUEXIT) {
			::PostMessage(hWnd, WM_USER + 200, (WPARAM)3, NULL);
		}
	}
	else {
		const wchar_t* lpParameters = NULL;
		if (cmd == DDCMD_SHOWCALENDARMAIN) {
			lpParameters = L"/sstartfrom explorer";
		}
		else if (cmd == DDCMD_UPDATE) {
			lpParameters = L"/sstartfrom explorer /embedding /update";
		}
		else if (cmd == DDCMD_SHOWABOUT) {
			lpParameters = L"/sstartfrom explorer /embedding /about";
		}
		else if (cmd == DDCMD_MENUEXIT) {
			lpParameters = L"/sstartfrom explorer /exit";
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
