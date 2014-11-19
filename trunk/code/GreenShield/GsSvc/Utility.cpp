#include "stdafx.h"
#include "Utility.h"

#include <Windows.h>

#include "ScopeResourceHandle.h"
#include "WTSProvider.h"

bool IsVistaOrLatter()
{
	OSVERSIONINFOEX osvi = { sizeof(OSVERSIONINFOEX) };
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!GetVersionEx( (LPOSVERSIONINFO)&osvi ))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if(!GetVersionEx( (LPOSVERSIONINFO)&osvi ))
		{
		}
	}
	return (osvi.dwMajorVersion >= 6);
}

bool GetGreenShiledExeFilePath(wchar_t* buffer, std::size_t bufferLength)
{
	ATL::CRegKey key;
	if(key.Open(HKEY_LOCAL_MACHINE, L"Software\\GreenShield", KEY_QUERY_VALUE) != ERROR_SUCCESS) {
		return false;
	}
	ULONG size = bufferLength;
	return key.QueryStringValue(L"Path", buffer, &size) == ERROR_SUCCESS;
}

bool LaunchGreenShield(DWORD browserProcessId)
{
	const wchar_t launchParameters[] = L" /sstartfrom service /embedding";
	if(!IsVistaOrLatter()) {
		// XP
		HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, browserProcessId);
		if(hProcess == NULL) {
			return false;
		}
		ScopeResourceHandle<HANDLE, BOOL (WINAPI*)(HANDLE)> autoCloseProcessHandle(hProcess, ::CloseHandle);

		HANDLE hProcessToken = NULL;
		if(!::OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hProcessToken)) {
			return false;
		}
		ScopeResourceHandle<HANDLE, BOOL (WINAPI*)(HANDLE)> autoCloseProcessToken(hProcessToken, ::CloseHandle);

		HANDLE hDuplicateToken = NULL;
		if(!::DuplicateTokenEx(hProcessToken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &hDuplicateToken)) {
			return false;
		}

		ScopeResourceHandle<HANDLE, BOOL (WINAPI*)(HANDLE)> autoCloseDuplicateToken(hDuplicateToken, ::CloseHandle);

		wchar_t exeFilePath[MAX_PATH];
		// buffer 长度为MAX_PATH * 2 但这里只传MAX_PATH
		if(!GetGreenShiledExeFilePath(&exeFilePath[1], MAX_PATH)) {
			TSERROR4CXX("Failed to get ExeFilePath");
			return false;
		}
		else {
			TSINFO4CXX("GreenShield ExeFilePath: " << exeFilePath);
		}

		exeFilePath[0] = L'\"';
		std::size_t exeFilePathLength = std::wcslen(exeFilePath);
		exeFilePath[exeFilePathLength++] = L'\"';
		std::copy(launchParameters, launchParameters + sizeof(launchParameters) / sizeof(launchParameters[0]), exeFilePath + exeFilePathLength);

		STARTUPINFO startupInfo;
		std::memset(&startupInfo, 0, sizeof(STARTUPINFO));
		startupInfo.cb = sizeof(STARTUPINFO);
		startupInfo.lpDesktop = TEXT("WinSta0\\Default");

		PROCESS_INFORMATION processInfomation;
		std::memset(&processInfomation, 0, sizeof(PROCESS_INFORMATION));

		if(!::CreateProcessAsUser(hDuplicateToken, NULL, exeFilePath, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfomation)) {
			return false;
		}
		return true;
	}
	else {
		// Vista Or Higher
		DWORD sessionId = 0;
		if(!::ProcessIdToSessionId(browserProcessId, &sessionId)) {
			return false;
		}

		WTSProvider wtsProvider;

		WTSProvider::WTSQueryUserTokenFuncType wtsQueryUserTokenPtr = wtsProvider.GetWTSQueryUserTokenFunctionPtr();
		if(!wtsQueryUserTokenPtr) {
			return false;
		}

		HANDLE hUserToken = NULL;
		if(!wtsQueryUserTokenPtr(sessionId, &hUserToken)) {
			return false;
		}
		
		ScopeResourceHandle<HANDLE, BOOL (WINAPI*)(HANDLE)> autoCloseUserToken(hUserToken, ::CloseHandle);

		TOKEN_LINKED_TOKEN linkedToken; 
		DWORD dwSize = sizeof(TOKEN_LINKED_TOKEN);
		if (!::GetTokenInformation(hUserToken, TokenLinkedToken, &linkedToken, dwSize, &dwSize)) {
			return false;
		}

		HANDLE hDuplicateToken = NULL;
		if(!::DuplicateTokenEx(linkedToken.LinkedToken, MAXIMUM_ALLOWED, NULL,  SecurityImpersonation, TokenPrimary, &hDuplicateToken)) {
			return false;
		}

		ScopeResourceHandle<HANDLE, BOOL (WINAPI*)(HANDLE)> autoCloseDuplicateToken(hDuplicateToken, ::CloseHandle);

		wchar_t exeFilePath[MAX_PATH * 2];
		// buffer 长度为MAX_PATH * 2 但这里只传MAX_PATH
		if(!GetGreenShiledExeFilePath(&exeFilePath[1], MAX_PATH)) {
			TSERROR4CXX("Failed to get ExeFilePath");
			return false;
		}
		else {
			TSINFO4CXX("GreenShield ExeFilePath: " << exeFilePath);
		}
		
		exeFilePath[0] = L'\"';
		std::size_t exeFilePathLength = std::wcslen(exeFilePath);
		exeFilePath[exeFilePathLength++] = L'\"';
		std::copy(launchParameters, launchParameters + sizeof(launchParameters) / sizeof(launchParameters[0]), exeFilePath + exeFilePathLength);

		STARTUPINFO startupInfo;
		std::memset(&startupInfo, 0, sizeof(STARTUPINFO));
		startupInfo.cb = sizeof(STARTUPINFO);
		startupInfo.lpDesktop = TEXT("WinSta0\\Default");

		PROCESS_INFORMATION processInfomation;
		std::memset(&processInfomation, 0, sizeof(PROCESS_INFORMATION));

		if(!::CreateProcessAsUser(hDuplicateToken, NULL, exeFilePath, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfomation)) {
			return false;
		}
		return true;
	}
}
