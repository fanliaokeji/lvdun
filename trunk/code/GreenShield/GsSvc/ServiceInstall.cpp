#include "stdafx.h"
#include "ServiceInstall.h"

#include "ScopeResourceHandle.h"

static const wchar_t* szServiceName = L"GreenShieldService";
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

	const wchar_t* szImagePath = L"%SystemRoot%\\System32\\svchost.exe -k GreenShieldService";

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
	SERVICE_DESCRIPTION description = { L"绿盾实时过滤服务。" };
	ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &description);

	// HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\services\GreenShieldService
	ATL::CRegKey key;
	LONG lRegResult = key.Open(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\services\\GreenShieldService", KEY_READ);
	if(lRegResult != ERROR_SUCCESS) {
		TSERROR4CXX("Filed to open reg key. Key: HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\services\\GreenShieldService. Error: " << lRegResult);
		return HRESULT_FROM_WIN32(lRegResult);
	}

	key.Close();

	// HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\services\GreenShieldService\Parameters
	lRegResult = key.Create(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\services\\GreenShieldService\\Parameters");
	if(lRegResult != ERROR_SUCCESS) {
		TSERROR4CXX("Failed to create reg key. Key: HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\services\\GreenShieldService\\Parameters. Error: " << lRegResult);
		return HRESULT_FROM_WIN32(lRegResult);
	}
	lRegResult = key.SetStringValue(L"ServiceDll", szDllPath, REG_EXPAND_SZ);
	if(lRegResult != ERROR_SUCCESS) {
		TSERROR4CXX("Failed to set reg value. Key: HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\services\\GreenShieldService\\Parameters\\ServiceDll. Error: " << lRegResult);
		return HRESULT_FROM_WIN32(lRegResult);
	}
	key.Close();

	// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Svchost
	lRegResult = key.Open(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost");
	if(lRegResult != ERROR_SUCCESS) {
		TSERROR4CXX("Failed to open reg key. Key: HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost. Error: " << lRegResult);
		return HRESULT_FROM_WIN32(lRegResult);
	}
	lRegResult = key.SetMultiStringValue(L"GreenShieldService", L"GreenShieldService\0");
	if(lRegResult != ERROR_SUCCESS) {
		TSERROR4CXX("Failed to set reg value. Key: HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost\\GreenShieldService. Error: " << lRegResult);
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

HRESULT UninstallService()
{
	TSAUTO();
	ATL::CRegKey key;

	LSTATUS lRegResult = key.Open(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost");
	if(lRegResult == ERROR_SUCCESS) {
		lRegResult = key.DeleteValue(L"GreenShieldService");
		if(lRegResult != ERROR_SUCCESS) {
			TSWARN4CXX("Failed to delete reg value. Key: HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost\\GreenShieldService. Error: " << lRegResult);
		}
		key.Close();
	}
	else {
		TSWARN4CXX("Failed to open reg key. Key: HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost. Error: " << lRegResult);
	}

	lRegResult = key.Open(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\services\\GreenShieldService");
	if(lRegResult == ERROR_SUCCESS) {
		lRegResult = key.RecurseDeleteKey(L"Parameters");
		if(lRegResult != ERROR_SUCCESS) {
			TSWARN4CXX("Failed to delete reg key. Key: HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\services\\GreenShieldService\\Parameters. Error: " << lRegResult);
		}
		key.Close();
	}
	else {
		TSWARN4CXX("Failed to open reg key. Key: HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\services\\GreenShieldService. Error: " << lRegResult);
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
