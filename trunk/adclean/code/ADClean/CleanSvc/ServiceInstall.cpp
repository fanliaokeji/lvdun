#include "stdafx.h"
#include "ServiceInstall.h"

#include <cassert>

#include "ScopeResourceHandle.h"
#include "Utility.h"

static const wchar_t* szServiceName = L"ADCleanService";
extern HINSTANCE g_hModule;

HRESULT CreateGreenShieldService(const wchar_t* szDllPath)
{
	TSAUTO();
	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (schSCManager == NULL) {
		DWORD dwOpenError = ::GetLastError();
		TSERROR4CXX("OpenSCManager failed. Error: " << dwOpenError);
		return HRESULT_FROM_WIN32(dwOpenError);
	}

	ScopeResourceHandle<SC_HANDLE, BOOL (WINAPI*)(SC_HANDLE)> autoCloseSCManagerHandle(schSCManager, ::CloseServiceHandle);

	const wchar_t* szImagePath = L"%SystemRoot%\\System32\\svchost.exe -k ADCleanService";

	SC_HANDLE schService = ::CreateService(
		schSCManager,
        szServiceName,
        szServiceName,
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_SHARE_PROCESS,
        SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL,
        szImagePath,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);

	if(schService == NULL) {
		DWORD dwCreateError = ::GetLastError();
		TSERROR4CXX("CreateService failed. Error: " << dwCreateError);
		return HRESULT_FROM_WIN32(dwCreateError);
	}

	ScopeResourceHandle<SC_HANDLE, BOOL (WINAPI*)(SC_HANDLE)> autoCloseServiceHandle(schService, ::CloseServiceHandle);
	SERVICE_DESCRIPTION description = { L"广告清道夫实时过滤服务。" };
	ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &description);

	// HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\services\ADCleanService
	ATL::CRegKey key;
	LONG lRegResult = key.Open(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\services\\ADCleanService", KEY_READ);
	if(lRegResult != ERROR_SUCCESS) {
		TSERROR4CXX("Filed to open reg key. Key: HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\services\\ADCleanService. Error: " << lRegResult);
		return HRESULT_FROM_WIN32(lRegResult);
	}

	key.Close();

	// HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\services\ADCleanService\Parameters
	lRegResult = key.Create(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\services\\ADCleanService\\Parameters");
	if(lRegResult != ERROR_SUCCESS) {
		TSERROR4CXX("Failed to create reg key. Key: HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\services\\ADCleanService\\Parameters. Error: " << lRegResult);
		return HRESULT_FROM_WIN32(lRegResult);
	}
	lRegResult = key.SetStringValue(L"ServiceDll", szDllPath, REG_EXPAND_SZ);
	if(lRegResult != ERROR_SUCCESS) {
		TSERROR4CXX("Failed to set reg value. Key: HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\services\\ADCleanService\\Parameters\\ServiceDll. Error: " << lRegResult);
		return HRESULT_FROM_WIN32(lRegResult);
	}
	key.Close();

	// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Svchost
	lRegResult = key.Open(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost");
	if(lRegResult != ERROR_SUCCESS) {
		TSERROR4CXX("Failed to open reg key. Key: HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost. Error: " << lRegResult);
		return HRESULT_FROM_WIN32(lRegResult);
	}
	lRegResult = key.SetMultiStringValue(L"ADCleanService", L"ADCleanService\0");
	if(lRegResult != ERROR_SUCCESS) {
		TSERROR4CXX("Failed to set reg value. Key: HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost\\ADCleanService. Error: " << lRegResult);
		return HRESULT_FROM_WIN32(lRegResult);
	}

	// start service
	::StartService(schService, 0, NULL);
	return S_OK;
}

bool IsServiceInstalled()
{
	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if(schSCManager == NULL) {
		DWORD dwOpenError = ::GetLastError();
		TSERROR4CXX("OpenSCManager failed. Error: " << dwOpenError);
		return false;
	}

	ScopeResourceHandle<SC_HANDLE, BOOL (WINAPI*)(SC_HANDLE)> autoCloseSCManagerHandle(schSCManager, ::CloseServiceHandle);

	SC_HANDLE schService = ::OpenService(schSCManager, szServiceName, DELETE);

	if(schService == NULL) {
		return false;
	}

	ScopeResourceHandle<SC_HANDLE, BOOL (WINAPI*)(SC_HANDLE)> autoCloseServiceHandle(schService, ::CloseServiceHandle);
	return true;
}

bool CopyFilesToPublicFolder()
{
	wchar_t* dependFiles[] = {
		L"CleanSvc.dll",
		L"Microsoft.VC90.CRT.manifest",
		L"msvcp90.dll",
		L"msvcr90.dll",
		L"Microsoft.VC90.ATL.manifest",
		L"ATL90.dll"
	};

	wchar_t souorcePath[MAX_PATH];
	if(!GetModuleFileName(g_hModule, souorcePath, MAX_PATH)) {
		TSERROR4CXX("Failed to get current dll path.");
		return false;
    }

	std::size_t sourceDirPathLength = std::wcslen(souorcePath);
	for (std::size_t i = std::wcslen(souorcePath); souorcePath[sourceDirPathLength - 1] != '\\' && sourceDirPathLength > 0; --sourceDirPathLength)
		;
	if (sourceDirPathLength == 0) {
		return false;
	}
	souorcePath[sourceDirPathLength] = L'\0';

	wchar_t targetPath[MAX_PATH];
	if(!GetAllUsersPublicPath(targetPath, MAX_PATH)) {
		TSERROR4CXX("Failed to get public path.");
		return false;
	}

	std::size_t targetDirPathLength = std::wcslen(targetPath);
	if(targetDirPathLength == 0 || targetDirPathLength + 1 == sizeof(targetPath) / sizeof(targetPath[0])) {
		return false;
	}
	if(targetPath[targetDirPathLength - 1] != '\\') {
		targetPath[targetDirPathLength++] = '\\';
	}
	// ADClean\\addin
	const wchar_t* addinSuffix = L"ADClean\\addin\\";
	std::size_t addinSuffixLength = std::wcslen(addinSuffix);
	if(targetDirPathLength + addinSuffixLength + 1 > sizeof(targetPath) / sizeof(targetPath[0])) {
		return false;
	}
	std::copy(addinSuffix, addinSuffix + addinSuffixLength + 1, targetPath + targetDirPathLength);
	targetDirPathLength += addinSuffixLength;
	assert(targetDirPathLength == std::wcslen(targetPath));

	if (!::PathFileExists(targetPath)) {
		if (!RecurseCreateDirctory(targetPath)) {
			return false;
		}
 	}

	// 判断serviceDll是否存在 如果存在先删除
	wchar_t* serviceDll = L"CleanSvc.dll";
	std::size_t fileNameLength = std::wcslen(serviceDll);
	if (sourceDirPathLength + fileNameLength + 1 > sizeof(souorcePath) / sizeof(souorcePath[0])
		|| targetDirPathLength + fileNameLength + 1 > sizeof(targetPath) / sizeof(targetPath[0])) {
		return false;
	}
	std::copy(serviceDll, serviceDll + fileNameLength + 1, &souorcePath[sourceDirPathLength]);
	std::copy(serviceDll, serviceDll + fileNameLength + 1, &targetPath[targetDirPathLength]);

	if (::PathFileExists(targetPath)) {
		// 判断版本 放在外面 这里不再判断
		if (!::DeleteFile(targetPath)) {
			// 删除失败 重名名 重启删除
			wchar_t fileNameRenamed[MAX_PATH];
			const wchar_t* renameSuffix = L".renamed";
			std::size_t suffixLength = std::wcslen(renameSuffix);
			if (sourceDirPathLength + fileNameLength + suffixLength + 1 > sizeof(fileNameRenamed) / sizeof(fileNameRenamed[0])) {
				return false;
			}
			std::copy(targetPath, targetPath + targetDirPathLength + fileNameLength, fileNameRenamed);
			std::copy(renameSuffix, renameSuffix + suffixLength + 1, fileNameRenamed + targetDirPathLength + fileNameLength);
			if (::MoveFileEx(targetPath, fileNameRenamed, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
				::MoveFileEx(fileNameRenamed, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
			}
		}
	}

	if (::PathFileExists(targetPath)) {
		return false;
	}

	for (std::size_t fileIndex = 0; fileIndex < sizeof(dependFiles) / sizeof(dependFiles[0]); ++fileIndex) {
		fileNameLength = std::wcslen(dependFiles[fileIndex]);
		if (sourceDirPathLength + fileNameLength + 1 > sizeof(souorcePath) / sizeof(souorcePath[0])
			|| targetDirPathLength + fileNameLength + 1 > sizeof(targetPath) / sizeof(targetPath[0])) {
			TSERROR4CXX("File name too long.");
			return false;
		}
		std::copy(dependFiles[fileIndex], dependFiles[fileIndex] + fileNameLength + 1, &targetPath[targetDirPathLength]);
		if (!::PathFileExists(souorcePath)) {
			TSERROR4CXX("Source file not found: " << souorcePath);
			return false;
		}
		if (!::PathFileExists(targetPath)) {
			std::copy(dependFiles[fileIndex], dependFiles[fileIndex] + fileNameLength + 1, &souorcePath[sourceDirPathLength]);
			TSINFO4CXX("Copy: " << souorcePath << " -> " << targetPath);
			if (!::CopyFile(souorcePath, targetPath, FALSE)) {
				return false;
			}
		}
	}
	return true;
}

HRESULT InstallService()
{
	TSAUTO();
	if(IsServiceInstalled()) {
		UninstallService();
	}
	wchar_t szPath[MAX_PATH];
    if(!GetModuleFileName(g_hModule, szPath, MAX_PATH)) {
		return HRESULT_FROM_WIN32(::GetLastError());
    }
	HRESULT hr = CreateGreenShieldService(szPath);
	return hr;
}

HRESULT SetupInstallService()
{
	TSAUTO();
	wchar_t currentDllPath[MAX_PATH];
    if(!GetModuleFileName(g_hModule, currentDllPath, MAX_PATH)) {
		return HRESULT_FROM_WIN32(::GetLastError());
    }

	wchar_t serviceDllPath[MAX_PATH];
	if(!GetAllUsersPublicPath(serviceDllPath, MAX_PATH)) {
		TSERROR4CXX("Failed to get public path.");
		return false;
	}

	std::size_t pathLength = std::wcslen(serviceDllPath);
	if(pathLength == 0 || pathLength + 1 == sizeof(serviceDllPath) / sizeof(serviceDllPath[0])) {
		return false;
	}
	if(serviceDllPath[pathLength - 1] != '\\') {
		serviceDllPath[pathLength++] = '\\';
	}

	// ADClean\\addin\\CleanSvc.dll
	const wchar_t* addinSuffix = L"ADClean\\addin\\CleanSvc.dll";
	std::size_t addinSuffixLength = std::wcslen(addinSuffix);

	if(pathLength + addinSuffixLength + 1 > sizeof(serviceDllPath) / sizeof(serviceDllPath[0])) {
		return false;
	}
	std::copy(addinSuffix, addinSuffix + addinSuffixLength + 1, serviceDllPath + pathLength);

	if(IsServiceInstalled()) {
		if(::PathFileExists(serviceDllPath)) {
			// 版本比较
			DWORD old_v1, old_v2, old_v3, old_v4;
			DWORD new_v1, new_v2, new_v3, new_v4;
			if(!GetFileVersionNumber(serviceDllPath, old_v1, old_v2, old_v3, old_v4)
				|| !GetFileVersionNumber(currentDllPath, new_v1, new_v2, new_v3, new_v4)) {
				TSERROR4CXX("Failed to get file version number.");
				if(!GetFileVersionNumber(serviceDllPath, old_v1, old_v2, old_v3, old_v4)) {
					TSINFO4CXX(serviceDllPath);
				}
				else {
					TSINFO4CXX(currentDllPath);
				}
				return false;
			}
			if(old_v1 > new_v1 || old_v2 > new_v2 || old_v3 > new_v3 || old_v4 > new_v4) {
				TSERROR4CXX("The old service dll is later than this.");
				return false;
			}
		}
		UninstallService();
	}

	if(!CopyFilesToPublicFolder()) {
		TSERROR4CXX("CopyFilesToPublicFolder return false.");
		return E_FAIL;
	}

	return CreateGreenShieldService(serviceDllPath);
}

HRESULT UninstallService()
{
	TSAUTO();
	ATL::CRegKey key;

	LSTATUS lRegResult = key.Open(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost");
	if(lRegResult == ERROR_SUCCESS) {
		lRegResult = key.DeleteValue(L"ADCleanService");
		if(lRegResult != ERROR_SUCCESS) {
			TSWARN4CXX("Failed to delete reg value. Key: HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost\\ADCleanService. Error: " << lRegResult);
		}
		key.Close();
	}
	else {
		TSWARN4CXX("Failed to open reg key. Key: HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost. Error: " << lRegResult);
	}

	lRegResult = key.Open(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\services\\ADCleanService");
	if(lRegResult == ERROR_SUCCESS) {
		lRegResult = key.RecurseDeleteKey(L"Parameters");
		if(lRegResult != ERROR_SUCCESS) {
			TSWARN4CXX("Failed to delete reg key. Key: HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\services\\ADCleanService\\Parameters. Error: " << lRegResult);
		}
		key.Close();
	}
	else {
		TSWARN4CXX("Failed to open reg key. Key: HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\services\\ADCleanService. Error: " << lRegResult);
	}

	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if(schSCManager == NULL) {
		DWORD dwOpenError = ::GetLastError();
		TSERROR4CXX("OpenSCManager failed. Error: " << dwOpenError);
		return HRESULT_FROM_WIN32(dwOpenError);
	}

	ScopeResourceHandle<SC_HANDLE, BOOL (WINAPI*)(SC_HANDLE)> autoCloseSCManagerHandle(schSCManager, ::CloseServiceHandle);

	SC_HANDLE schService = ::OpenService(schSCManager, szServiceName, DELETE | SERVICE_STOP | SERVICE_QUERY_STATUS);

	if(schService == NULL) {
		DWORD dwOpenError = ::GetLastError();
		TSERROR4CXX("OpenSCManager failed. Error: " << dwOpenError);
		return HRESULT_FROM_WIN32(dwOpenError);
	}

	ScopeResourceHandle<SC_HANDLE, BOOL (WINAPI*)(SC_HANDLE)> autoCloseServiceHandle(schService, ::CloseServiceHandle);

	SERVICE_STATUS_PROCESS ssp;

	DWORD dwBytesNeeded = 0;
	if(!QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded)) {
		DWORD dwQueryServiceStatus = ::GetLastError();
		TSERROR4CXX("QueryServiceStatusEx failed. Error: " << dwQueryServiceStatus);
		return HRESULT_FROM_WIN32(dwQueryServiceStatus);
	}

	if(ssp.dwCurrentState != SERVICE_STOPPED) {
		DWORD dwStartTime = GetTickCount();
		DWORD dwTimeout = 30000;
		while (ssp.dwCurrentState == SERVICE_STOP_PENDING) {
			DWORD dwWaitTime = ssp.dwWaitHint / 10;

			if( dwWaitTime < 1000 )
				dwWaitTime = 1000;
			else if ( dwWaitTime > 10000 )
				dwWaitTime = 10000;

			Sleep(dwWaitTime);

			if(!QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded)) {
				DWORD dwQueryServiceStatus = ::GetLastError();
				TSERROR4CXX("QueryServiceStatusEx failed. Error: " << dwQueryServiceStatus);
				return HRESULT_FROM_WIN32(dwQueryServiceStatus);
			}

			if(ssp.dwCurrentState == SERVICE_STOPPED) {
				TSINFO4CXX("Service Stop Success.");
				goto AfterStopLabel;
			}

			if(GetTickCount() - dwStartTime > dwTimeout) {
				TSERROR4CXX("Wait for service stop timeout.");
				return E_FAIL;
			}
		}
		
		if(!ControlService(schService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp)) {
			DWORD dwControlServiceError = ::GetLastError();
			TSERROR4CXX("ControlService failed. Error: " << dwControlServiceError);
			return HRESULT_FROM_WIN32(dwControlServiceError);
		}

		while ( ssp.dwCurrentState != SERVICE_STOPPED ) {
			DWORD dwWaitTime = ssp.dwWaitHint;
			if( dwWaitTime < 1000 )
				dwWaitTime = 1000;
			else if ( dwWaitTime > 10000 )
				dwWaitTime = 10000;

			Sleep(dwWaitTime);
			if(!QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded)) {
				DWORD dwQueryServiceStatus = ::GetLastError();
				TSERROR4CXX("QueryServiceStatusEx failed. Error: " << dwQueryServiceStatus);
				return HRESULT_FROM_WIN32(dwQueryServiceStatus);
			}

			if(ssp.dwCurrentState == SERVICE_STOPPED) {
				TSINFO4CXX("Service Stop Success.");
				break;
			}

			if(GetTickCount() - dwStartTime > dwTimeout ) {
				TSERROR4CXX("Wait timed out");
				return E_FAIL;
			}
		}
	}
AfterStopLabel:
	if(::DeleteService(schService)) {
		TSERROR4CXX("DeleteService success");
		return S_OK;
	}
	else {
		DWORD dwDeleteError = ::GetLastError();
		TSERROR4CXX("DeleteService failed. Error: " << dwDeleteError);
		return HRESULT_FROM_WIN32(dwDeleteError);
	}
}

bool DeleteSeriviceDll()
{
	wchar_t serviceDllPath[MAX_PATH];
	if(!GetAllUsersPublicPath(serviceDllPath, MAX_PATH)) {
		TSERROR4CXX("Failed to get public path.");
		return false;
	}

	std::size_t serviceDllLength = std::wcslen(serviceDllPath);
	if(serviceDllLength == 0 || serviceDllLength + 1 == sizeof(serviceDllPath) / sizeof(serviceDllPath[0])) {
		return false;
	}
	if(serviceDllPath[serviceDllLength - 1] != '\\') {
		serviceDllPath[serviceDllLength++] = '\\';
	}

	// ADClean\\addin\\CleanSvc.dll
	const wchar_t* addinSuffix = L"ADClean\\addin\\CleanSvc.dll";
	std::size_t addinSuffixLength = std::wcslen(addinSuffix);

	if(serviceDllLength + addinSuffixLength + 1 > sizeof(serviceDllPath) / sizeof(serviceDllPath[0])) {
		return false;
	}
	std::copy(addinSuffix, addinSuffix + addinSuffixLength + 1, serviceDllPath + serviceDllLength);
	serviceDllLength += addinSuffixLength;

	if(::PathFileExists(serviceDllPath)) {
		if(!::DeleteFile(serviceDllPath)) {
			wchar_t fileNameRenamed[MAX_PATH];
			const wchar_t* renameSuffix = L".renamed";
			std::size_t suffixLength = std::wcslen(renameSuffix);
			if (serviceDllLength + suffixLength + 1 > sizeof(fileNameRenamed) / sizeof(fileNameRenamed[0])) {
				return false;
			}
			std::copy(serviceDllPath, serviceDllPath + serviceDllLength, fileNameRenamed);
			std::copy(renameSuffix, renameSuffix + suffixLength + 1, fileNameRenamed + serviceDllLength);
			if (::MoveFileEx(serviceDllPath, fileNameRenamed, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
				if(!::MoveFileEx(fileNameRenamed, NULL, MOVEFILE_DELAY_UNTIL_REBOOT)) {
					return false;
				}
			}
			else {
				return false;
			}
		}
	}
	return true;
}

HRESULT SetupUninstallService()
{
	HRESULT hr = UninstallService();
	if(FAILED(hr)) {
		TSERROR4CXX("UninstallService failed");
		return hr;
	}
	if(!DeleteSeriviceDll()) {
		TSWARN4CXX("Failed to delete service dll.");
	}
	return hr;
}
