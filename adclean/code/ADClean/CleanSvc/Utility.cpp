#include "stdafx.h"
#include "Utility.h"

#include <Windows.h>
#include <Shlwapi.h>

#include <sstream>

#include "ScopeResourceHandle.h"
#include "WTSProvider.h"
#include "AdvanceFunctionProvider.h"

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
	if(key.Open(HKEY_LOCAL_MACHINE, L"Software\\ADClean", KEY_QUERY_VALUE) != ERROR_SUCCESS) {
		return false;
	}
	ULONG size = bufferLength;
	return key.QueryStringValue(L"Path", buffer, &size) == ERROR_SUCCESS;
}

bool LaunchGreenShield(DWORD browserProcessId)
{
	TSAUTO();
	const wchar_t launchParameters[] = L" /sstartfrom service /embedding /showbubble";
	if(!IsVistaOrLatter()) {
		// XP
		TSTRACE4CXX("XP");
		HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, browserProcessId);
		if(hProcess == NULL) {
			TSERROR4CXX("OpenProcess fail. Error: " << ::GetLastError());
			return false;
		}
		ScopeResourceHandle<HANDLE, BOOL (WINAPI*)(HANDLE)> autoCloseProcessHandle(hProcess, ::CloseHandle);

		HANDLE hProcessToken = NULL;
		if(!::OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hProcessToken)) {
			TSERROR4CXX("OpenProcessToken fail. Error: " << ::GetLastError());
			return false;
		}
		ScopeResourceHandle<HANDLE, BOOL (WINAPI*)(HANDLE)> autoCloseProcessToken(hProcessToken, ::CloseHandle);

		HANDLE hDuplicateToken = NULL;
		if(!::DuplicateTokenEx(hProcessToken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &hDuplicateToken)) {
			TSERROR4CXX("DuplicateTokenEx fail. Error: " << ::GetLastError());
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
			TSERROR4CXX("CreateProcessAsUser fail. Error: " << ::GetLastError());
			return false;
		}
		return true;
	}
	else {
		TSTRACE4CXX("Vista Or Higher");
		// Vista Or Higher
		DWORD sessionId = 0;
		if(!::ProcessIdToSessionId(browserProcessId, &sessionId)) {
			TSERROR4CXX("ProcessIdToSessionId fail. Error: " << ::GetLastError());
			return false;
		}

		WTSProvider wtsProvider;

		WTSProvider::WTSQueryUserTokenFuncType wtsQueryUserTokenPtr = wtsProvider.GetWTSQueryUserTokenFunctionPtr();
		if(!wtsQueryUserTokenPtr) {
			TSERROR4CXX("wtsQueryUserTokenPtr == NULL.");
			return false;
		}

		HANDLE hUserToken = NULL;
		if(!wtsQueryUserTokenPtr(sessionId, &hUserToken)) {
			TSERROR4CXX("WTSQueryUserToken fail. Error: " << ::GetLastError());
			return false;
		}
		
		ScopeResourceHandle<HANDLE, BOOL (WINAPI*)(HANDLE)> autoCloseUserToken(hUserToken, ::CloseHandle);

		TOKEN_ELEVATION_TYPE tokenElevationType;
		DWORD dwSize = sizeof(TOKEN_ELEVATION_TYPE);
		if(!::GetTokenInformation(hUserToken, TokenElevationType, &tokenElevationType, dwSize, &dwSize)) {
			TSERROR4CXX("GetTokenInformation TokenElevationType fail." << ::GetLastError());
			return false;
		}
		HANDLE hDuplicateToken = NULL;
		if(tokenElevationType == TokenElevationTypeLimited) {
			TOKEN_LINKED_TOKEN linkedToken; 
			dwSize = sizeof(TOKEN_LINKED_TOKEN);
			if (!::GetTokenInformation(hUserToken, TokenLinkedToken, &linkedToken, dwSize, &dwSize)) {
				TSERROR4CXX("GetTokenInformation TokenLinkedToken fail. Error: " << ::GetLastError());
				return false;
			}

			ScopeResourceHandle<HANDLE, BOOL (WINAPI*)(HANDLE)> autoCloseLinkedToken(linkedToken.LinkedToken, ::CloseHandle);

			if(!::DuplicateTokenEx(linkedToken.LinkedToken, MAXIMUM_ALLOWED, NULL,  SecurityImpersonation, TokenPrimary, &hDuplicateToken)) {
				TSERROR4CXX("DuplicateTokenEx fail. Error: " << ::GetLastError());
				return false;
			}
		}
		else {
			if(!::DuplicateTokenEx(hUserToken, MAXIMUM_ALLOWED, NULL,  SecurityImpersonation, TokenPrimary, &hDuplicateToken)) {
				TSERROR4CXX("DuplicateTokenEx fail. Error: " << ::GetLastError());
				return false;
			}
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
			TSERROR4CXX("CreateProcessAsUser fail. Error: " << ::GetLastError());
			return false;
		}
		return true;
	}
}

bool GetFileVersionNumber(const wchar_t* szFileName, DWORD& v1, DWORD& v2, DWORD& v3, DWORD& v4)
{
	bool result = false;
	DWORD dwHandle = 0;
	DWORD dwSize = ::GetFileVersionInfoSize(szFileName, &dwHandle);
	if (dwSize > 0) {
		wchar_t* pVersionInfo = new wchar_t[dwSize + 1];
		if(::GetFileVersionInfo(szFileName, dwHandle, dwSize, pVersionInfo)) {
			VS_FIXEDFILEINFO * pvi;
			UINT uLength = 0;
			if (::VerQueryValueA(pVersionInfo, "\\", (void **)&pvi, &uLength)) {
				v1 = HIWORD(pvi->dwFileVersionMS);
				v2 = LOWORD(pvi->dwFileVersionMS);
				v3 = HIWORD(pvi->dwFileVersionLS);
				v4 = LOWORD(pvi->dwFileVersionLS);
				result = true;
			}
		}
		delete[] pVersionInfo;
	}
	return result;
}

bool GetAllUsersPublicPath(wchar_t* buffer, std::size_t bufferLength)
{
	if(IsVistaOrLatter()) {
		AdvanceFunctionProvider shFunctionProvider;
		AdvanceFunctionProvider::SHGetKnownFolderPathFuncType SHGetKnownFolderPathFunctionPtr = shFunctionProvider.GetSHGetKnownFolderPathFunctionPtr();
		if(SHGetKnownFolderPathFunctionPtr == NULL) {
			return false;
		}
		const GUID publicFolderGuid = {0xDFDF76A2, 0xC82A, 0x4D63, {0x90, 0x6A, 0x56, 0x44, 0xAC, 0x45, 0x73, 0x85}};
		wchar_t* szPublicPath = NULL;
		HRESULT hr = SHGetKnownFolderPathFunctionPtr(publicFolderGuid, 0, NULL, &szPublicPath);
		if(FAILED(hr)) {
			return false;
		}
		ScopeResourceHandle<LPVOID, void (STDAPICALLTYPE*)(LPVOID)> autoFreePublicPathBuffer(szPublicPath, ::CoTaskMemFree);
		std::size_t length = std::wcslen(szPublicPath) + 1;
		if(bufferLength < length) {
			return false;
		}
		std::copy(szPublicPath, szPublicPath + length, buffer);
		return true;
	}
	else {
		wchar_t tempBuffer[MAX_PATH];
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, &tempBuffer[0]);
		if(FAILED(hr)) {
			return false;
		}
		std::size_t length = std::wcslen(tempBuffer) + 1;
		if(bufferLength < length) {
			return false;
		}
		std::copy(tempBuffer, tempBuffer + length, buffer);
		return true;
	}
}

bool RecurseCreateDirctory(const std::wstring& path)
{
	std::size_t pos = path.find(':');
	if (pos == std::wstring::npos) {
		return false;
	}
	std::wstring _path;
	_path.reserve(path.size());
	_path.append(path.begin(), path.begin() + pos + 1);
	++pos;
	while (pos < path.size()) {
		std::size_t start_pos = pos;
		for (; pos < path.size() && path[pos] == '\\'; ++pos)
			;
		if (pos == path.size()) {
			return true;
		}
		++pos;
		for (; pos < path.size() && path[pos] != '\\'; ++pos)
			;
		_path.append(path.begin() + start_pos, path.begin() + pos);
		if (::PathFileExists(_path.c_str())) {
			if ((::GetFileAttributes(_path.c_str()) & FILE_ATTRIBUTE_DIRECTORY) == 0) {
				return false;
			}
		}
		else {
			if (::CreateDirectory(_path.c_str(), NULL) == FALSE) {
				if (::GetLastError() != ERROR_ALREADY_EXISTS || (::GetFileAttributes(_path.c_str()) & FILE_ATTRIBUTE_DIRECTORY) == 0) {
					return false;
				}
			}
		}
	}
	return true;
}
