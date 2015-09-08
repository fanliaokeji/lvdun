// OfficeAddin.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "OfficeAddin_i.h"
#include "dllmain.h"
#include "Connect.h"
#include "AddinHelper.h"

extern HINSTANCE g_hModule;

// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}

// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	TSAUTO();
	// {06C0B7EC-FD1E-4e31-8C24-037F9A3C5111}
	GUID clsid = {0x6c0b7ec, 0xfd1e, 0x4e31, { 0x8c, 0x24, 0x3, 0x7f, 0x9a, 0x3c, 0x51, 0x11 }};
	return _AtlModule.DllGetClassObject(clsid, riid, ppv);
}

static const wchar_t* OfficeProd[] = {
	L"Word",
	L"Excel"
};

static HRESULT RegisterAddin(const std::wstring& clsid, const std::wstring& progName, const std::wstring& dllPath, const std::wstring& description, const std::wstring& friendlyName)
{
	std::wstring progClsidKey = progName + L"\\CLSID";
	ATL::CRegKey key;
	LSTATUS lStatus = key.Create(HKEY_CLASSES_ROOT, progClsidKey.c_str());
	if (lStatus != ERROR_SUCCESS) {
		return HRESULT_FROM_WIN32(lStatus);
	}
	lStatus = key.SetStringValue(NULL, clsid.c_str());
	if (lStatus != ERROR_SUCCESS) {
		return HRESULT_FROM_WIN32(lStatus);
	}
	key.Close();
	std::wstring inprocServerKey = L"CLSID\\" + clsid + L"\\InprocServer32";
	lStatus = key.Create(HKEY_CLASSES_ROOT, inprocServerKey.c_str());
	if (lStatus != ERROR_SUCCESS) {
		return HRESULT_FROM_WIN32(lStatus);
	}
	lStatus = key.SetStringValue(NULL, dllPath.c_str());
	if (lStatus != ERROR_SUCCESS) {
		return HRESULT_FROM_WIN32(lStatus);
	}
	lStatus = key.SetStringValue(L"ThreadingModel", L"Apartment");
	if (lStatus != ERROR_SUCCESS) {
		return HRESULT_FROM_WIN32(lStatus);
	}
	key.Close();
	for (int i = 0; i < (sizeof(OfficeProd) / sizeof(OfficeProd[0])); ++i) {
		std::wstring keyName = L"SOFTWARE\\Microsoft\\Office\\";
		keyName += OfficeProd[i];
		keyName += L"\\Addins\\";
		keyName += progName;
		ATL::CRegKey key;
		if (key.Create(HKEY_LOCAL_MACHINE, keyName.c_str()) == ERROR_SUCCESS) {
			key.SetDWORDValue(L"CommandLineSafe", 1);
			key.SetStringValue(L"Description", description.c_str());
			key.SetStringValue(L"FriendlyName", friendlyName.c_str());
			key.SetDWORDValue(L"LoadBehavior", 3);
		}
	}
	return S_OK;
}

static HRESULT UnregisterAddin(const std::wstring& clsid, const std::wstring& progName)
{
	for (int i = 0; i < (sizeof(OfficeProd) / sizeof(OfficeProd[0])); ++i) {
		std::wstring keyName = L"SOFTWARE\\Microsoft\\Office\\";
		keyName += OfficeProd[i];
		keyName += L"\\Addins";
		ATL::CRegKey key;
		if (key.Open(HKEY_LOCAL_MACHINE, keyName.c_str()) == ERROR_SUCCESS) {
			key.RecurseDeleteKey(progName.c_str());
		}
	}
	HRESULT hr = S_OK;
	ATL::CRegKey key;
	LSTATUS lStatus = key.Open(HKEY_CLASSES_ROOT, NULL);
	if (lStatus == ERROR_SUCCESS) {
		lStatus = key.RecurseDeleteKey(progName.c_str());
		key.Close();
	}
	if (lStatus != ERROR_SUCCESS) {
		hr = HRESULT_FROM_WIN32(lStatus);
	}

	lStatus = key.Open(HKEY_CLASSES_ROOT, L"CLSID");
	if (lStatus == ERROR_SUCCESS) {
		lStatus = key.RecurseDeleteKey(clsid.c_str());
		key.Close();
	}
	if (lStatus != ERROR_SUCCESS) {
		hr = HRESULT_FROM_WIN32(lStatus);
	}
	return hr;
}

// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    // HRESULT hr = _AtlModule.DllRegisterServer();
	wchar_t path[MAX_PATH];
	::GetModuleFileName(g_hModule, path, MAX_PATH);
	std::wstring dllPath = path;
	std::wstring configFile = dllPath;
	for (;!configFile.empty() && configFile[configFile.size() - 1] != L'\\'; configFile.resize(configFile.size() - 1))
		;
	configFile += L"config.ini";
	TSINFO4CXX("Dll Path: " << dllPath);
	TSINFO4CXX("Config File Path: " << configFile);
	wchar_t buffer[100];
	::GetPrivateProfileString(L"addin", L"clsid", L"", buffer, 100,configFile.c_str());
	std::wstring clsid = buffer;
	::GetPrivateProfileString(L"addin", L"progid", L"", buffer, 100, configFile.c_str());
	std::wstring progID = buffer;
	::GetPrivateProfileString(L"addin", L"description", L"", buffer, 100,configFile.c_str());
	std::wstring description = buffer;
	::GetPrivateProfileString(L"addin", L"friendlyname", L"", buffer, 100, configFile.c_str());
	std::wstring friendlyName = buffer;
	if (clsid.empty() || progID.empty()) {
		return E_FAIL;
	}
	return RegisterAddin(clsid, progID, dllPath, description, friendlyName);
}

// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	// HRESULT hr = _AtlModule.DllUnregisterServer();
	wchar_t path[MAX_PATH];
	::GetModuleFileName(g_hModule, path, MAX_PATH);
	std::wstring configFile = path;
	for (;!configFile.empty() && configFile[configFile.size() - 1] != L'\\'; configFile.resize(configFile.size() - 1))
		;
	configFile += L"config.ini";
	TSINFO4CXX("Config File Path: " << configFile);
	wchar_t buffer[100];
	::GetPrivateProfileString(L"addin", L"clsid", L"", buffer, 100,configFile.c_str());
	std::wstring clsid = buffer;
	::GetPrivateProfileString(L"addin", L"progID", L"", buffer, 100, configFile.c_str());
	std::wstring progID = buffer;
	if (clsid.empty() || progID.empty()) {
		return E_FAIL;
	}
	return UnregisterAddin(clsid, progID);
}

// DllInstall - Adds/Removes entries to the system registry per user
//              per machine.	
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
    HRESULT hr = E_FAIL;
    static const wchar_t szUserSwitch[] = _T("user");

    if (pszCmdLine != NULL)
    {
    	if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
    	{
    		AtlSetPerUserRegistration(true);
    	}
    }

    if (bInstall)
    {	
    	hr = DllRegisterServer();
    	if (FAILED(hr))
    	{	
    		DllUnregisterServer();
    	}
    }
    else
    {
    	hr = DllUnregisterServer();
    }

    return hr;
}
