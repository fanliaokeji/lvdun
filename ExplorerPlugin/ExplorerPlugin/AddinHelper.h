#pragma once
#include <string>
#include <sstream>
using namespace std;
#include <Windows.h>
#include <shlobj.h>
#include "Lock.h"

extern HINSTANCE g_hThisModule;

class DesktopIcon{
public:
	static void SetRegValue(HKEY hk, wchar_t* path, wchar_t* key, const wchar_t* value, BOOL bWow64 = FALSE){
		REGSAM samDesired = bWow64?(KEY_WOW64_64KEY|KEY_ALL_ACCESS):KEY_ALL_ACCESS;
		ATL::CRegKey regkey;
		if (regkey.Open(hk, path,samDesired) != ERROR_SUCCESS) {
			regkey.Create(hk,path, 0, 0, samDesired);
		}
		regkey.SetStringValue(key, value);
		regkey.Close();
	};
	static void SetRegValue(HKEY hk, wchar_t* path, wchar_t* key, const DWORD value, BOOL bWow64 = FALSE){
		REGSAM samDesired = bWow64?(KEY_WOW64_64KEY|KEY_ALL_ACCESS):KEY_ALL_ACCESS;
		ATL::CRegKey regkey;
		if (regkey.Open(hk, path,samDesired) != ERROR_SUCCESS) {
			regkey.Create(hk,path, 0, 0, samDesired);
		}
		regkey.SetDWORDValue(key, value);
		regkey.Close();
	};
	static void update(LONG event=SHCNE_UPDATEITEM){
		LPITEMIDLIST pidl = NULL;
		HRESULT ret = SHGetFolderLocation(NULL, CSIDL_DESKTOP, NULL, NULL, &pidl);
		if (!FAILED(ret)) {
			SHChangeNotify(event, SHCNF_IDLIST, reinterpret_cast<void*>(pidl), NULL);
		}
		ILFree(pidl);
	};
	static bool IsVistaOrLater()
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
	};
	static void CreateIcon(wstring& strRiliPathW){
		wstring strCmdW = L"\"";
		strCmdW += strRiliPathW;
		strCmdW += L"\" /sstartfrom desktopicons";

		wchar_t path[MAX_PATH];
		::GetModuleFileName(g_hThisModule, path, MAX_PATH);
		wstring strDelCmdW = L"rundll32.exe \"";
		strDelCmdW += path;
		strDelCmdW += L"\",ShortcutDelete";
		
		wstring strAboutCmdW = L"rundll32.exe \"";
		strAboutCmdW += path;
		strAboutCmdW += L"\",ShowAbout";

		SetRegValue(HKEY_CLASSES_ROOT, L"CLSID\\{000010BB-A7DE-453a-A39E-D558E997D056}", L"InfoTip", L"桌面显示我的日历", TRUE);
		SetRegValue(HKEY_CLASSES_ROOT, L"CLSID\\{000010BB-A7DE-453a-A39E-D558E997D056}", L"LocalizedString", L"我的日历", TRUE);
		SetRegValue(HKEY_CLASSES_ROOT, L"CLSID\\{000010BB-A7DE-453a-A39E-D558E997D056}\\Shell\\Open", L"", L"打开日历(&O)", TRUE);
		SetRegValue(HKEY_CLASSES_ROOT, L"CLSID\\{000010BB-A7DE-453a-A39E-D558E997D056}\\Shell\\Open\\Command", L"", strCmdW.c_str(), TRUE);
		SetRegValue(HKEY_CLASSES_ROOT, L"CLSID\\{000010BB-A7DE-453a-A39E-D558E997D056}\\Shell\\deleteicon", L"", L"删除图标(&D)", TRUE);
		SetRegValue(HKEY_CLASSES_ROOT, L"CLSID\\{000010BB-A7DE-453a-A39E-D558E997D056}\\Shell\\deleteicon\\Command", L"", strDelCmdW.c_str(), TRUE);
		SetRegValue(HKEY_CLASSES_ROOT, L"CLSID\\{000010BB-A7DE-453a-A39E-D558E997D056}\\Shell\\about", L"", L"关于(&A)", TRUE);
		SetRegValue(HKEY_CLASSES_ROOT, L"CLSID\\{000010BB-A7DE-453a-A39E-D558E997D056}\\Shell\\about\\Command", L"", strAboutCmdW.c_str(), TRUE);
		SetRegValue(HKEY_CLASSES_ROOT, L"CLSID\\{000010BB-A7DE-453a-A39E-D558E997D056}\\ShellFolder", L"Attributes", (const DWORD)0, TRUE);
		HKEY hk = IsVistaOrLater() ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE;
		SetRegValue(hk, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Desktop\\NameSpace\\{000010BB-A7DE-453a-A39E-D558E997D056}", L"", L"My calendar desktop icon", TRUE);
		UpdateIcon();
	};
	static void DeleteIcon(){
		REGSAM samDesired = KEY_WOW64_64KEY|KEY_ALL_ACCESS;
		ATL::CRegKey regkey;
		if (regkey.Open(HKEY_CLASSES_ROOT, L"CLSID",samDesired) == ERROR_SUCCESS) {
			regkey.RecurseDeleteKey(L"{000010BB-A7DE-453a-A39E-D558E997D056}");
		}
		HKEY hk = IsVistaOrLater() ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE;
		if (regkey.Open(hk, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Desktop\\NameSpace",samDesired) == ERROR_SUCCESS) {
			regkey.RecurseDeleteKey(L"{000010BB-A7DE-453a-A39E-D558E997D056}");
		}
		regkey.Close();
		update();
	};
	static BOOL IsIconExist(){
		REGSAM samDesired = KEY_WOW64_64KEY|KEY_READ;
		ATL::CRegKey regkey;
		if (regkey.Open(HKEY_CLASSES_ROOT, L"CLSID\\{000010BB-A7DE-453a-A39E-D558E997D056}",samDesired) == ERROR_SUCCESS){ 
			HKEY hk = IsVistaOrLater() ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE;
			if(regkey.Open(hk, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Desktop\\NameSpace\\{000010BB-A7DE-453a-A39E-D558E997D056}",samDesired) == ERROR_SUCCESS) {
				regkey.Close();
				return TRUE;
			}
		}
		return FALSE;
	};
	//默认图索引是31,正常图片索引范围是0-30
	static void UpdateIcon(int nIndex = 31){
		wchar_t path[MAX_PATH];
		::GetModuleFileName(g_hThisModule, path, MAX_PATH);
		wstring strIconW = path;
		wchar_t szIdx[3] = {0};
		_itow(nIndex, szIdx, 10);
		strIconW += L",";
		strIconW += szIdx;
		
		if (IsVistaOrLater()){
			SetRegValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\CLSID\\{000010BB-A7DE-453a-A39E-D558E997D056}\\DefaultIcon", L"", strIconW.c_str(), TRUE);
		}
		else{
			SetRegValue(HKEY_CLASSES_ROOT, L"CLSID\\{000010BB-A7DE-453a-A39E-D558E997D056}\\DefaultIcon", L"", strIconW.c_str(), TRUE);
		}
		update();
		
	};
};

class RegData{
public:
	std::wstring strData;
	DWORD dwData;
	RegData():strData(L""),dwData(0){};
	RegData(std::wstring sd){strData = sd;};
	RegData(DWORD dd){dwData = dd;};
	RegData(const RegData& rd){strData=rd.strData;dwData=rd.dwData;};
	RegData& operator= (std::wstring sd){strData = sd;return *this;};
	RegData& operator= (DWORD dd){dwData = dd;return *this;};
};

class RegMonitor{
public:
	static XMLib::CriticalSection cs;
	//当shelloverlay注册表被干掉就不允许更新了
	static BOOL s_bCanUpdate;
	//另开线程监视注册表
	static unsigned int __stdcall ThreadMonitorRegChange(void* arg){
		s_bCanUpdate = TRUE;
		TSDEBUG4CXX("ThreadMonitorRegChange enter, s_bCanUpdate = "<<s_bCanUpdate);
		HKEY hKey;
		wstring strRegPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellIconOverlayIdentifiers\\";
		strRegPath += ICONOVERLAYNAME;
		if (ERROR_SUCCESS != RegOpenKeyExW(HKEY_LOCAL_MACHINE,strRegPath.c_str(), 0, KEY_NOTIFY, &hKey)){
			TSDEBUG4CXX("ThreadMonitorRegChange return, open reg failed! set bool false, set icon default.");
			SetBool(FALSE);
			DesktopIcon::UpdateIcon();
			return 0;
		}
		while (true){
			if (ERROR_SUCCESS != RegNotifyChangeKeyValue(hKey, TRUE, REG_NOTIFY_CHANGE_NAME, NULL, FALSE)){
				TSDEBUG4CXX("ThreadMonitorRegChange return, RegNotifyChangeKeyValue returnvalue != ERROR_SUCCESS!");
				return 0;
			}
			if (!CheckIconOverlayReg()){
				TSDEBUG4CXX("ThreadMonitorRegChange return, CheckIconOverlayReg return false! set bool false, set icon default.");
				SetBool(FALSE);
				DesktopIcon::UpdateIcon();
				return 0;
			}
		}
	};
	static BOOL CheckIconOverlayReg(){
		wstring strRegPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellIconOverlayIdentifiers\\";
		strRegPath += ICONOVERLAYNAME;
		ATL::CRegKey key;
		//是否完整的标记
		BOOL bRet = FALSE;
		wstring strRootPath = L"";
		if ( key.Open(HKEY_LOCAL_MACHINE, strRegPath.c_str()) == ERROR_SUCCESS){
			ULONG lSize = MAX_PATH;
			wchar_t szValueW[MAX_PATH] = {0};
			if (key.QueryStringValue(L"", szValueW, &lSize) == ERROR_SUCCESS){
				strRootPath = L"CLSID\\";
				strRootPath += szValueW;
				strRootPath += L"InprocServer32";
				bRet = TRUE;
			}
			key.Close();
		}
		if (bRet && strRootPath != L"" && key.Open(HKEY_CLASSES_ROOT, strRootPath.c_str()) == ERROR_SUCCESS){
			ULONG lSize = MAX_PATH;
			wchar_t szValueW[MAX_PATH] = {0};
			if (key.QueryStringValue(L"", szValueW, &lSize) == ERROR_SUCCESS && PathFileExists(szValueW)){
				bRet = TRUE;
			}
			else{
				bRet = FALSE;
			}
		}
		TSDEBUG4CXX("ThreadMonitorRegChange::CheckIconOverlayReg,  bRet = "<<bRet);
		return bRet;
	};
	static void SetBool(BOOL bl){
		XMLib::CriticalSectionLockGuard lcs(cs);
		s_bCanUpdate = bl;
	};
	static BOOL GetBool(){
		XMLib::CriticalSectionLockGuard lcs(cs);
		TSDEBUG4CXX("ThreadMonitorRegChange::GetBool s_bCanUpdate = "<<s_bCanUpdate);
		return s_bCanUpdate;
	};
};

class AddinHelper {
	HANDLE m_hMutex;
	std::wstring m_mutexName;
public:
	AddinHelper();
	~AddinHelper();
	bool EnsureOwnerMutex();
	bool BeginTask();
	static unsigned int __stdcall TaskThreadProc(void* arg);
	unsigned int TaskProc();
	static void HandleLaunch();
	static void HandleUpdateIcon();

	bool IsVistaOrHigher() const;
	static RegData QueryRegVal(HKEY key, LPCTSTR lpszKeyName, LPCTSTR lpszValuename, REGSAM flag =  KEY_READ);
	static BOOL QueryProcessExist(const TCHAR* processname);
	static BOOL IsStartUp();
};

class MsgWindow{
public:
	static int m_nday;
	static void UpdateDayOfMoth(){
		TSDEBUG4CXX("UpdateDayOfMoth enter");
		__time64_t lCurTime;
		_time64( &lCurTime); 
		tm* pTmc = _localtime64(&lCurTime);
		LONG nLastDay = pTmc->tm_mday;	
		TSDEBUG4CXX("UpdateDayOfMoth m_nday = "<<m_nday<<", nLastDay = "<<nLastDay);
		if (m_nday != nLastDay && RegMonitor::GetBool()){
			m_nday = nLastDay;
			DesktopIcon::UpdateIcon(nLastDay-1);
		}
	};
	static LRESULT CALLBACK MyWinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch(msg)
		{
		case WM_CREATE:
			::SetTimer(hwnd, 1, 3600*1000, NULL);
			break;
		case WM_DESTROY:
			KillTimer(hwnd, 1);
			PostQuitMessage(0);
			break;
		case WM_TIMECHANGE:
			UpdateDayOfMoth();
			break;
		case WM_TIMER:
			TSDEBUG4CXX("WM_TIMER enter");
			AddinHelper::HandleLaunch();
			break;
		default:
			break;
		}
		return ::DefWindowProc(hwnd,msg,wparam,lparam);

	}

	static int WINAPI Create(HINSTANCE hInstance = g_hThisModule)
	{
		WNDCLASSEX wcex=
		{
			sizeof(WNDCLASSEX),CS_HREDRAW|CS_VREDRAW,
			MyWinProc,
			0,
			0,
			hInstance,
			LoadIcon(NULL,IDI_APPLICATION),
			LoadCursor(NULL,IDC_ARROW),
			(HBRUSH)GetStockObject(GRAY_BRUSH),
			NULL,
			L"MycalendarDesktop_{3CB8FE08-F88C-4644-B609-D3EFD0BAC638}",
			LoadIcon(NULL,IDI_APPLICATION)

		};

		RegisterClassEx(&wcex);

		HWND hwnd;
		hwnd=CreateWindowEx
			(
			NULL,
			L"MycalendarDesktop_{3CB8FE08-F88C-4644-B609-D3EFD0BAC638}",
			L"MycalendarDesktop_windowname",
			WS_EX_PALETTEWINDOW,
			0,
			0,
			0,
			0,
			NULL,
			NULL,
			hInstance,
			NULL
			);
		if(!hwnd)
			return 0;
		ShowWindow(hwnd,SW_HIDE);
		UpdateWindow(hwnd);
		MSG msg;
		while(GetMessage(&msg,hwnd,0,0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return 0;
	}
};


