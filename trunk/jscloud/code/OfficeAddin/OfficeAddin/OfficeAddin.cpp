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
	GUID clsid = {0x32A6E740, 0xC617, 0x41F3, {0x98, 0xA0, 0xFC, 0x44, 0xD7, 0xD6, 0x87, 0x4D}};
	return _AtlModule.DllGetClassObject(clsid, riid, ppv);
}

static const wchar_t* OfficeProd[] = {
	L"Word",
	L"Excel"
};

static HRESULT RegisterAddin(const std::wstring& clsid, const std::wstring& progName, const std::wstring& dllPath)
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
			key.SetStringValue(L"Description", L"");
			key.SetStringValue(L"FriendlyName", L"输入法增强加载项");
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
	wchar_t clsidBuff[100];
	wchar_t progIDBuff[100];
	::GetPrivateProfileString(L"Addin", L"CLSID", L"", clsidBuff, 100,configFile.c_str());
	::GetPrivateProfileString(L"Addin", L"ProgID", L"", progIDBuff, 100, configFile.c_str());
	std::wstring clsid = clsidBuff;
	std::wstring progID = progIDBuff;
	return RegisterAddin(clsid, progID, dllPath);
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
	wchar_t clsidBuff[100];
	wchar_t progIDBuff[100];
	::GetPrivateProfileString(L"Addin", L"CLSID", L"", clsidBuff, 100,configFile.c_str());
	::GetPrivateProfileString(L"Addin", L"ProgID", L"", progIDBuff, 100, configFile.c_str());
	std::wstring clsid = clsidBuff;
	std::wstring progID = progIDBuff;
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
