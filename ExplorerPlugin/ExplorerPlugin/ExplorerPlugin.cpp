// ExplorerPlugin.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "ExplorerPlugin_i.h"
#include "dllmain.h"
#include "AddinHelper.h"

extern HINSTANCE g_hThisModule;

static HRESULT UnregisterAddin(const std::wstring& clsid);
static HRESULT RegisterAddin(const std::wstring& clsid, const std::wstring& dllPath);
static std::wstring strClsidW = L"{89E76F95-82D7-4d5e-BC4D-1169E10F5BF2}";
// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
	return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{	
	TSINFO4CXX("DllGetClassObject enter");
	GUID clsid = {0x241B20FB,0x78DB,0x4679,{0xAE,0xEB,0x9F,0x77,0xE7,0x7D,0xC7,0xC5}};
	return _AtlModule.DllGetClassObject(clsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
	// registers object, typelib and all interfaces in typelib
	//HRESULT hr = _AtlModule.DllRegisterServer();
	TSINFO4CXX("DllRegisterServer enter");
	wchar_t path[MAX_PATH];
	::GetModuleFileName(g_hThisModule, path, MAX_PATH);
	std::wstring dllPath = path;
	return RegisterAddin(strClsidW,dllPath);
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	//反注册的时候删除图标
	DesktopIcon::DeleteIcon();
	return UnregisterAddin(strClsidW);
}

// DllInstall - Adds/Removes entries to the system registry per user
//              per machine.	
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
	TSINFO4CXX("DllInstall enter");
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

static HRESULT RegisterClassRoot(const std::wstring& clsid, const std::wstring& dllPath)
{
	ATL::CRegKey key;
	std::wstring inprocServerKey = L"CLSID\\" + clsid + L"\\InprocServer32";
	LSTATUS lStatus = key.Create(HKEY_CLASSES_ROOT, inprocServerKey.c_str());
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
	return S_OK;
}

static HRESULT RegisterIconOverlay(const std::wstring& clsid)
{
	ATL::CRegKey key;
	std::wstring iconOverlayKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellIconOverlayIdentifiers\\";
	iconOverlayKey += ICONOVERLAYNAME;
	LSTATUS lStatus = key.Create(HKEY_LOCAL_MACHINE, iconOverlayKey.c_str());
	if (lStatus != ERROR_SUCCESS) {
		return HRESULT_FROM_WIN32(lStatus);
	}
	lStatus = key.SetStringValue(NULL, clsid.c_str());
	if (lStatus != ERROR_SUCCESS) {
		return HRESULT_FROM_WIN32(lStatus);
	}
	key.Close();
	return S_OK;
}

static HRESULT RegisterCopyHook(const std::wstring& clsid)
{
	ATL::CRegKey key;
	std::wstring copyHookKey = L"Directory\\shellex\\CopyHookHandlers\\";
	copyHookKey += COPYHOOKNAME;
	LSTATUS lStatus = key.Create(HKEY_CLASSES_ROOT, copyHookKey.c_str());
	if (lStatus != ERROR_SUCCESS) {
		return HRESULT_FROM_WIN32(lStatus);
	}
	lStatus = key.SetStringValue(NULL, clsid.c_str());
	if (lStatus != ERROR_SUCCESS) {
		return HRESULT_FROM_WIN32(lStatus);
	}
	key.Close();
	return S_OK;
}

static HRESULT RegisterBho(const std::wstring& clsid)
{
	ATL::CRegKey key;
	std::wstring bhoKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects\\";
	bhoKey += clsid;
	LSTATUS lStatus = key.Create(HKEY_LOCAL_MACHINE, bhoKey.c_str());
	if (lStatus != ERROR_SUCCESS) {
		return HRESULT_FROM_WIN32(lStatus);
	}
	lStatus = key.SetStringValue(NULL, BHONAME);
	if (lStatus != ERROR_SUCCESS) {
		return HRESULT_FROM_WIN32(lStatus);
	}
	lStatus = key.SetDWORDValue(L"NoExplorer",1);
	if (lStatus != ERROR_SUCCESS) {
		return HRESULT_FROM_WIN32(lStatus);
	}
	key.Close();
	return S_OK;
}

static HRESULT RegisterAddin(const std::wstring& clsid, const std::wstring& dllPath)
{
	HRESULT hr =  RegisterClassRoot(clsid,dllPath);
	TSINFO4CXX("RegisterClassRoot,hr = " << hr);
	if (S_OK != hr)
	{
		return hr;
	}
	hr = RegisterIconOverlay(clsid);
	TSINFO4CXX("RegisterIconOverlay,hr = " << hr);

	//hr = RegisterCopyHook(clsid);
	//TSINFO4CXX("RegisterCopyHook,hr = " << hr);

	//hr = RegisterBho(clsid);
	//TSINFO4CXX("RegisterBho,hr = " << hr);
	return S_OK;

}

static HRESULT UnregisterAddin(const std::wstring& clsid)
{
	HRESULT hr = S_OK;
	ATL::CRegKey key;

	//delete IconOverlay
	LSTATUS lStatus = key.Open(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellIconOverlayIdentifiers");
	if (lStatus == ERROR_SUCCESS) {
		lStatus = key.RecurseDeleteKey(ICONOVERLAYNAME);
		key.Close();
	}
	if (lStatus != ERROR_SUCCESS) {
		hr = HRESULT_FROM_WIN32(lStatus);
	}

	//delete CopyHook
	lStatus = key.Open(HKEY_CLASSES_ROOT, L"Directory\\shellex\\CopyHookHandlers");
	if (lStatus == ERROR_SUCCESS) {
		lStatus = key.RecurseDeleteKey(COPYHOOKNAME);
		key.Close();
	}
	if (lStatus != ERROR_SUCCESS) {
		hr = HRESULT_FROM_WIN32(lStatus);
	}

	//delete BHO
	lStatus = key.Open(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects");
	if (lStatus == ERROR_SUCCESS) {
		lStatus = key.RecurseDeleteKey(clsid.c_str());
		key.Close();
	}
	if (lStatus != ERROR_SUCCESS) {
		hr = HRESULT_FROM_WIN32(lStatus);
	}

	//delete HKCR
	lStatus = key.Open(HKEY_CLASSES_ROOT, L"CLSID");
	if (lStatus == ERROR_SUCCESS) {
		lStatus = key.RecurseDeleteKey(clsid.c_str());
		key.Close();
	}
	if (lStatus != ERROR_SUCCESS) {
		hr = HRESULT_FROM_WIN32(lStatus);
	}

	//return hr;
	return S_OK;
}

extern "C" void WINAPI ShortcutDelete(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (IDYES == MessageBox(0, L"确认要删除桌面图标吗？", L"提醒", MB_YESNO|MB_ICONQUESTION)){
		DesktopIcon::DeleteIcon();
	}
}


extern "C" void WINAPI ShowAbout(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	std::wstring aboutMsg = L"我的日历桌面图标";
	aboutMsg += L"\r\n\r\n深圳市二十二楼科技有限公司(C)2015-2017\r\n";
	aboutMsg += L"Copyright (c) 2015-2017";
	::MessageBox(NULL, aboutMsg.c_str(), L"关于", MB_OK);
}

extern "C" void WINAPI ShowDefaultWhenDestroy(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	//已经是完整的不做处理
	if (RegMonitor::CheckIconOverlayReg()){
		return;
	}
	//不完整则将图标恢复到默认
	if (DesktopIcon::IsIconExist()){
		DesktopIcon::UpdateIcon();
	}
}

extern "C" void CreateDeskIcon()
{
	DesktopIcon::DeleteIcon();
	if (!DesktopIcon::IsIconExist()){
		RegData rd = AddinHelper::QueryRegVal(HKEY_LOCAL_MACHINE, REGEDITPATH, _T("Path"), KEY_READ | KEY_WOW64_32KEY);
		if (rd.strData != L""){
			DesktopIcon::CreateIcon(rd.strData);
			MsgWindow::UpdateDayOfMoth();
		}
	}
}