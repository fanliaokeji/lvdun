#pragma once
#include <string>
#include <sstream>
#include <algorithm>
using namespace std;
#include <Windows.h>
#include <shlobj.h>
#include "Lock.h"
#include <comutil.h>
extern HINSTANCE g_hThisModule;

class RegTool{
	ATL::CRegKey m_key;
public:
	wstring ReadRegStr(HKEY hk, const wchar_t* szPath, const wchar_t* szKey, REGSAM flag =  KEY_READ);
	DWORD ReadRegDWORD(HKEY hk, const wchar_t* szPath, const wchar_t* szKey, REGSAM flag =  KEY_READ);
	~RegTool(){m_key.Close();};
	ATL::CRegKey* operator-> (){return &m_key;};
};

class SendState{
public:
	static unsigned int __stdcall SendHttpStatThread(LPVOID pParameter){
		CHAR szUrl[MAX_PATH] = {0};
		strcpy(szUrl,(LPCSTR)pParameter);
		delete [] pParameter;

		CHAR szBuffer[MAX_PATH] = {0};
		::CoInitialize(NULL);
		HRESULT hr = E_FAIL;
		__try
		{
			hr = ::URLDownloadToCacheFileA(NULL, szUrl, szBuffer, MAX_PATH, 0, NULL);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			TSDEBUG4CXX("URLDownloadToCacheFile Exception !!!");
		}
		::CoUninitialize();
		return SUCCEEDED(hr)?ERROR_SUCCESS:0xFF;
	};
	static void GetPeerID(CHAR * pszPeerID){
		strcpy(pszPeerID, "0000000000000000");
		ATL::CRegKey key;
		HRESULT hr;
		if ((hr = key.Open(HKEY_LOCAL_MACHINE, L"Software\\mycalendar", KEY_READ | KEY_WOW64_32KEY)) == ERROR_SUCCESS) {
			wchar_t szValueW[MAX_PATH] = {0};
			ULONG lLen = MAX_PATH;
			if (key.QueryStringValue(L"PeerId", szValueW, &lLen) == ERROR_SUCCESS){
				strcpy(pszPeerID, (char*)(_bstr_t)szValueW);
				TSDEBUG4CXX("GetPeerID szValueW = "<<szValueW<<", pszPeerID = "<<pszPeerID);
			}
			key.Close();
		}
	};
	static string GetMinorVer(){
		string ver = "0";
		ATL::CRegKey key;
		HRESULT hr;
		if ((hr = key.Open(HKEY_LOCAL_MACHINE, L"Software\\mycalendar", KEY_READ | KEY_WOW64_32KEY)) == ERROR_SUCCESS) {
			wchar_t szValueW[MAX_PATH] = {0};
			ULONG lLen = MAX_PATH;
			if (key.QueryStringValue(L"Ver", szValueW, &lLen) == ERROR_SUCCESS){
				wstring fver = szValueW;
				wstring::size_type pos = fver.rfind(L'.');
				if (pos != wstring::npos){
					ver = (char*)(_bstr_t)(fver.substr(pos+1,fver.length()-pos).c_str());
				}
				TSDEBUG4CXX("GetMinorVer szValueW = "<<szValueW<<", ver = "<<ver);
			}
			key.Close();
		}
		return ver;
	};

	static BOOL CheckStatPeerid(){
		char szPid[17] = {0};
		GetPeerID(szPid);
		WCHAR szPidW[17] = {0};
		wcscpy(szPidW, (WCHAR*)(_bstr_t)szPid);
		RegTool rt;
		wstring strsp = rt.ReadRegStr(HKEY_CURRENT_USER, L"Software\\mycalendar", L"statpeerid");
		if (strsp == L""){
			strsp = L"0123";
		}
		transform(strsp.begin(),strsp.end(),strsp.begin(),tolower);
		_wcslwr_s(szPidW, wcslen(szPidW) + 1);
		TSDEBUG4CXX("CheckStatPeerid strsp = "<<strsp.c_str()<<", szPidW[11] = "<<szPidW[11]);
		if (szPidW[11] == 0 || strsp.find(szPidW[11]) == wstring::npos){
			return FALSE;
		}
		return TRUE;
	};

	static void Send( CHAR *ec, CHAR *el,const CHAR *ea = GetMinorVer().c_str(), long ev = 1){
		if (ec == NULL || ea == NULL || !CheckStatPeerid()){
			return ;
		}
		CHAR* szURL = new CHAR[MAX_PATH];
		memset(szURL, 0, MAX_PATH);
		char szPid[256] = {0};
		GetPeerID(szPid);
		std::string str = "";
		if (el != NULL ){
			str += "&el=";
			str += el;
		}
		if (ev != 0){
			CHAR szev[MAX_PATH] = {0};
			sprintf(szev, "&ev=%ld",ev);
			str += szev;
		}
		sprintf(szURL, "http://www.google-analytics.com/collect?v=1&tid=UA-58424540-1&cid=%s&t=event&ec=%s&ea=%s%s",szPid,ec,ea,str.c_str());
		_beginthreadex(NULL, 0, &SendHttpStatThread, szURL, 0, NULL);
	};
};

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
		if (!IsIconExist()){
			return;
		}
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
			KillUpdateTimer();
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
				KillUpdateTimer();
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
	static void KillUpdateTimer();//需要引用MsgWindow里面的函数， 只能放在cpp里来做
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
	static void LaunchExe();
	static void LaunchAi();
	static BOOL TodayNotDo(const wchar_t* szValueName = L"pluginlastutc");

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
	static BOOL& TimerState(){
		static BOOL m_btimer = FALSE;
		TSDEBUG4CXX("TimerState, m_btimer = "<<m_btimer);
		return m_btimer;
	};
	static HWND& GetHwnd(HWND hwnd = NULL){
		static HWND m_hwnd = NULL;
		if(hwnd != NULL)
			m_hwnd = hwnd;
		return m_hwnd;
	};
	static void KillUpdateTimer(){
		HWND hwnd = GetHwnd();
		BOOL timerstate = TimerState();
		if (timerstate){
			KillTimer(hwnd, 2);
			timerstate = FALSE;
		}
	};
	static void Timer4UpdateIcon(HWND hwnd){
		TSDEBUG4CXX("Timer4UpdateIcon enter");
		if (!RegMonitor::GetBool()){
			TSDEBUG4CXX("Timer4UpdateIcon RegMonitor::GetBool return false");
			return;
		}
		GetHwnd(hwnd);
		//先杀掉计时器
		KillUpdateTimer();
		__time64_t lCurTime;
		_time64( &lCurTime); 
		tm* pTmc = _localtime64(&lCurTime);
		int pmHour = pTmc->tm_hour, pmMin = pTmc->tm_min, pmSecond = pTmc->tm_sec;
		int dv_Hour = 23 - pmHour, dv_Min = 59 - pmMin, dv_Sec = 59 - pmSecond;
		//得到时间变化需要的秒数
		int lastvalue =  dv_Hour*3600 + dv_Min*60 + dv_Sec + 1;
		TSDEBUG4CXX("Timer4UpdateIcon pmHour = "<<pmHour<<", pmMin = "<<pmMin<<", pmSecond "<<pmSecond \
			<<"\r\ndv_Hour = "<<dv_Hour<<", dv_Min = "<<dv_Min<<", dv_Sec = "<<dv_Sec \
			<<"\r\n lastvalue = "<<lastvalue);
		//计时器id是2
		::SetTimer(hwnd, 2, lastvalue*1000, NULL);
		TimerState() = TRUE;

	};
	static LRESULT CALLBACK MyWinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch(msg)
		{
		case WM_CREATE:
			::SetTimer(hwnd, 1, 3600*1000, NULL);
			Timer4UpdateIcon(hwnd);
			break;
		case WM_DESTROY:
			KillTimer(hwnd, 1);
			KillUpdateTimer();
			PostQuitMessage(0);
			break;
		case WM_TIMECHANGE:
			UpdateDayOfMoth();
			Timer4UpdateIcon(hwnd);
			break;
		case WM_TIMER:
			TSDEBUG4CXX("WM_TIMER enter");
			if (wparam == 1){
				AddinHelper::HandleLaunch();
			}
			else if (wparam == 2){
				KillUpdateTimer();
				UpdateDayOfMoth();
				Timer4UpdateIcon(hwnd);
			}
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


