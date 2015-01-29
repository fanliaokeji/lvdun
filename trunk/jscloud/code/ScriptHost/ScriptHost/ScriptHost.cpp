// ScriptHost.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "ScriptHost_i.h"
#include "dllmain.h"
#include "dlldatax.h"
#include "XScriptHost.h"
#include "atlstr.h"
#include "tlhelp32.h"
#include "shlobj.h"
#include <time.h>
#include <fstream>
#include "md5.h"
#include <algorithm>
#include "LaunchHelper.h"
#include <sddl.h>
// Used to determine whether the DLL can be unloaded by OLE

TCHAR g_szPeerId[32] = {0};
std::wstring g_strcmdline = L"";
STDAPI DllCanUnloadNow(void)
{
#ifdef _MERGE_PROXYSTUB
    HRESULT hr = PrxDllCanUnloadNow();
    if (hr != S_OK)
        return hr;
#endif
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
#ifdef _MERGE_PROXYSTUB
    if (PrxDllGetClassObject(rclsid, riid, ppv) == S_OK)
        return S_OK;
#endif
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
#ifdef _MERGE_PROXYSTUB
    if (FAILED(hr))
        return hr;
    hr = PrxDllRegisterServer();
#endif
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
#ifdef _MERGE_PROXYSTUB
    if (FAILED(hr))
        return hr;
    hr = PrxDllRegisterServer();
    if (FAILED(hr))
        return hr;
    hr = PrxDllUnregisterServer();
#endif
	return hr;
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


void KillProcessTree(DWORD dwProcessID, DWORD dwExceptPID)
{ 
	PROCESSENTRY32 info;
	info.dwSize = sizeof(PROCESSENTRY32 );
	//结束进程句柄
	HANDLE hProcess = NULL;
	DWORD dwParentPID = 0;
	DWORD dwChildPID = 0;
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, dwProcessID);
	if(hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return ;
	}
	// 遍历进程快照，轮流显示每个进程的信息
	BOOL bMore = Process32First(hProcessSnap, &info);


	while( bMore != FALSE)
	{
		// 如果找个父进程句柄是需要关闭的ID，就已经完成查找
		if (dwProcessID == info.th32ParentProcessID)
		{
			dwParentPID = info.th32ParentProcessID;
			dwChildPID = info.th32ProcessID;
			break;
		}
		// 如果找到dwProcessID进程，就只保存父线程ID
		if (dwProcessID == info.th32ProcessID)
		{
			dwParentPID = info.th32ProcessID;
		}
		bMore = Process32Next(hProcessSnap, &info);
	}
	if (dwChildPID)
	{
		// 如果有子线程先结束子线程
		hProcess=OpenProcess(PROCESS_TERMINATE, FALSE, dwChildPID);
		if (NULL == hProcess )
		{
			return ;
		}
		if(dwChildPID != dwExceptPID)
			TerminateProcess(hProcess, 0);
		CloseHandle(hProcess);
	}
	if (dwParentPID)
	{
		hProcess=OpenProcess(PROCESS_TERMINATE, FALSE, dwParentPID);
		if (NULL == hProcess)
		{
			return ;
		}
		if(dwParentPID != dwExceptPID)
			TerminateProcess(hProcess, 0);
		CloseHandle(hProcessSnap);
	}
}
class CMessageLoop
{
public:
	int Run()
	{
		BOOL bDoIdle = TRUE;
		int nIdleCount = 0;
		BOOL bRet;
		MSG m_msg;
		for(;;)
		{
			while(bDoIdle && !::PeekMessage(&m_msg, NULL, 0, 0, PM_NOREMOVE))
			{
				//if(!OnIdle(nIdleCount++))
				bDoIdle = FALSE;
			}

			bRet = ::GetMessage(&m_msg, NULL, 0, 0);

			if(bRet == -1)
			{
				//ATLTRACE2(atlTraceUI, 0, _T("::GetMessage returned -1 (error)\n"));
				continue;   // error, don't process
			}
			else if(!bRet)
			{
				//ATLTRACE2(atlTraceUI, 0, _T("CMessageLoop::Run - exiting\n"));
				break;   // WM_QUIT, exit message loop
			}

			//if(!PreTranslateMessage(&m_msg))
			{
				::TranslateMessage(&m_msg);
				::DispatchMessage(&m_msg);
			}

			if(IsIdleMessage(&m_msg))
			{
				bDoIdle = TRUE;
				nIdleCount = 0;
			}
		}

		return (int)m_msg.wParam;
	}

	static BOOL IsIdleMessage(MSG* pMsg)
	{
		// These messages should NOT cause idle processing
		switch(pMsg->message)
		{
		case WM_MOUSEMOVE:
#ifndef _WIN32_WCE
		case WM_NCMOUSEMOVE:
#endif // !_WIN32_WCE
		case WM_PAINT:
		case 0x0118:	// WM_SYSTIMER (caret blink)
			return FALSE;
		}

		return TRUE;
	}
};

BOOL IsSystemStartup()
{
	BOOL b = (GetTickCount()<5*60*1000) ? TRUE : FALSE;
	return b;
}
std::wstring GetLocalTempDir()
{
	TCHAR szTempDir[_MAX_PATH] = {0};
	SHGetFolderPath(NULL, CSIDL_FLAG_CREATE|CSIDL_INTERNET_CACHE, 0, SHGFP_TYPE_CURRENT, szTempDir);
	PathAddBackslash(szTempDir);
	PathAppend(szTempDir, _T("Content.mso\\"));	
	std::wstring t = szTempDir; 
	return  t;
}

static __int64 GenerateUrlTimeStamp(TCHAR chPeerId_12, DWORD dwCycleInHour)
{
	TSTRACEAUTO();

	int flag = 0;
	if (chPeerId_12 >= _T('A') && chPeerId_12 <= _T('F'))
	{
		flag = chPeerId_12 - _T('A') + 0xA;
	}
	else if (chPeerId_12 >= _T('a') && chPeerId_12 <= _T('f'))
	{
		flag = chPeerId_12 - _T('a') + 0xA;
	}
	else if (chPeerId_12 >= _T('0') && chPeerId_12 <= _T('9'))
	{
		flag = chPeerId_12 - _T('0') + 0x0;
	}

	TSDEBUG(_T("flag = %d"), flag);

	// math.floor((tipUtil:GetCurrentUTCTime() + 8 * 3600 - (PeerID [12] +  1) * 3600)/(24*3600)) 
	__time64_t tNowUTCSec;
	_time64(&tNowUTCSec);

	__int64 i64Stamp = (tNowUTCSec + 8 * 3600 - (flag + 1) * 3600) / (dwCycleInHour * 3600);
	TSDEBUG(_T("i64Stamp = %I64d"), i64Stamp);

	return i64Stamp;
}

BOOL GetLaunchUrl(LPTSTR  lptszPath, LONG l, std::wstring & strMD5)
{
	l;
	HRESULT hr = E_FAIL;
	TCHAR szIniPath[_MAX_PATH] = {0};
	TCHAR szTempDir[_MAX_PATH] = {0};
	SHGetFolderPath(NULL, CSIDL_FLAG_CREATE|CSIDL_INTERNET_CACHE, 0, SHGFP_TYPE_CURRENT, szTempDir);
	PathAddBackslash(szTempDir);
	PathAppend(szTempDir, _T("Content.mso\\"));	
	if(!PathFileExists(szTempDir))
		SHCreateDirectory(NULL, szTempDir);	
	_tcsncpy(szIniPath,szTempDir, _MAX_PATH);
	PathAppend(szIniPath, _T("mso.ini"));

	//TCHAR szIniUrl[2048] = {0};
	std::wstring wstrIniUrl = LaunchConfig::Instance()->m_wstrInitUrl;
	//wcscpy(szIniUrl, wstrIniUrl.c_str());
	CStringW url;
	url.Format(L"%s?stamp=%I64d",wstrIniUrl.c_str(),GenerateUrlTimeStamp(g_szPeerId[11], 24));
	hr = URLDownloadToCacheFile (NULL, url, szIniPath, _MAX_PATH, 0, 0);
	if(SUCCEEDED(hr) && PathFileExists(szIniPath))
	{
		TCHAR szLaunchUrl[2048] = {0};
		GetPrivateProfileString(_T("launch"), _T("url"),_T(""), szLaunchUrl, 2048, szIniPath);
		_tcsncpy(lptszPath, szLaunchUrl, l);
		TCHAR szMD5[64] = {0};
		GetPrivateProfileString(_T("launch"), _T("md5"),_T(""), szMD5, 2048,szIniPath);		
		strMD5 = szMD5;		
		std::transform(strMD5.begin(), strMD5.end(), strMD5.begin(), toupper);
		return TRUE;
	}	
	return FALSE;
}
void SendLaunchFailedStat(LPCTSTR lpctszEvent, LPCTSTR lpctszParam)
{
	TCHAR szPath[_MAX_PATH] = {0};
	CStringW url;
	std::wstring wstrTID = LaunchConfig::Instance()->m_wstrTID;
	url.Format(L"http://www.google-analytics.com/collect?v=1&tid=%s&cid=%s&t=event&ec=exception&ea=%s&el=%s" ,
		wstrTID.c_str(),g_szPeerId, lpctszEvent, lpctszParam);
	URLDownloadToCacheFile (NULL, url, szPath, _MAX_PATH, 0, 0);			 
}
std::wstring  GetFileMD5(LPCTSTR lpctszFile)
{	
	USES_CONVERSION;
	if(!::PathFileExists(lpctszFile))
		return _T("") ;	
	std::ifstream fs;
	fs.open( CStringA(lpctszFile), ios::in|ios::binary );  
	if(fs.fail())
		return _T("") ;
	MD5 md5(fs);
	std::string strMD5A = md5.toString();
	std::transform(strMD5A.begin(), strMD5A.end(), strMD5A.begin(), toupper);
	std::wstring strMD5 = A2W(strMD5A.c_str());
	return strMD5;
}


//CAppModule _Module;
#if defined(WIN64) || defined(_WIN64)
#pragma comment(linker, "/EXPORT:ScreenSaver=?_si0@@YAXPEAUHWND__@@PEAUHINSTANCE__@@PEA_WH@Z,PRIVATE") 
#else
#pragma comment(linker, "/EXPORT:ScreenSaver=?_si0@@YGXPAUHWND__@@PAUHINSTANCE__@@PA_WH@Z,PRIVATE") 
#pragma comment(linker, "/EXPORT:OpenURL=?_openurl@@YGXPAUHWND__@@PAUHINSTANCE__@@PA_WH@Z,PRIVATE") 
//__declspec(dllexport)
#endif

static BOOL MultiByteToUnicode(const std::string& strSrc, std::basic_string<WCHAR> &strDst)
{
	INT nSrcAnsiLen = 0;
	nSrcAnsiLen = (INT)strSrc.length();
	if(0 == nSrcAnsiLen)
		return TRUE;
	UINT code_page = 936;
	INT nLen = 0;
	nLen = MultiByteToWideChar(code_page,0,strSrc.c_str(),-1,NULL,0);
	if(0 == nLen)
	{
		code_page = 0;
		nLen = MultiByteToWideChar(code_page,0,strSrc.c_str(),-1,NULL,0);
	}
	if(MAX_USERDATA_SIZE < nLen)
		nLen = MAX_USERDATA_SIZE;
	WCHAR szBuf[MAX_USERDATA_SIZE + 1] = {0};
	LPWSTR pszBuf = szBuf;
	INT nResult = 0;
	nResult = MultiByteToWideChar(code_page,0,strSrc.c_str(), -1, pszBuf,nLen);
	if(nResult <= 0 )
		return FALSE;	
	if(pszBuf)
		strDst = pszBuf;
	return TRUE;
}

static bool AnsiStringToWideString(const std::string &strAnsi, std::wstring &strWide)
{
	bool bSuc = false;

	int cchWide = ::MultiByteToWideChar(CP_ACP, 0, strAnsi.c_str(), -1, NULL, 0);
	if (cchWide)
	{
		WCHAR *pszWide = new (std::nothrow) WCHAR[cchWide];
		if (pszWide)
		{
			::MultiByteToWideChar(CP_ACP, 0, strAnsi.c_str(), -1, pszWide, cchWide);
			strWide = pszWide;
			bSuc = true;

			delete [] pszWide;
			pszWide = NULL;
		}
	}

	return bSuc;
}

static bool HandleSingleton()
{
	// 这里创建互斥量的目的不是抢占它，而是看有没有同一个实例已经在运行，如果已有多胞胎则直接结束，因此这里创建之时并不试图占有它
    HANDLE  hHostMutex = ::CreateMutex(NULL, FALSE,  LaunchConfig::Instance()->m_wstrHostMutex.c_str());
	DWORD dwLastError = ::GetLastError();
	if (NULL == hHostMutex)
	{
		TSERROR(_T("CreateMutex(%s) failed. LastError = %lu"), LaunchConfig::Instance()->m_wstrHostMutex.c_str(), dwLastError);
		return FALSE;
	}
	else if (ERROR_ALREADY_EXISTS == dwLastError)
	{
		TSDEBUG(_T("Mutant named with %s has already been existed, so exit now"),  LaunchConfig::Instance()->m_wstrHostMutex.c_str());
		return FALSE;
	}
	return TRUE;
	
};

void CALLBACK _openurl(	HWND hwnd,	HINSTANCE hinst,	LPTSTR lpCmdLine,	int nCmdShow)
{
	TSAUTO();
	lpCmdLine;


	CHAR szUrl[2048] = {0};
	GetPrivateProfileStringA( "InternetShortcut", "URL", "", szUrl, 2048, (LPSTR)lpCmdLine);
	::ShellExecuteA(NULL, "open", szUrl, "", NULL,SW_SHOWNORMAL);
}
//lpCmdLine 该参数为Anis，需要转换成(LPSTR)lpCmdLine再使用
void  CALLBACK _si0(	HWND hwnd,	HINSTANCE hinst,	LPTSTR lpCmdLine,	int nCmdShow)
{
	TSAUTO();	
	if (IsDebugging())
	{
		return;
	}
	//
	if (!LaunchConfig::Instance()->Init())
	{
		TSDEBUG4CXX("Init config failed.");
		return ;
	}
	if (!HandleSingleton())
	{
		return ;
	}
	if ((LPSTR)lpCmdLine[0] != '\0')
	{
		AnsiStringToWideString((LPSTR)lpCmdLine,g_strcmdline);
	}
	//CRegKey key;	
	//TCHAR szPID[32] = {0};	
	//HRESULT hr =  key.Open(HKEY_CURRENT_USER, LaunchConfig::Instance()->m_wstrRegisterPath.c_str(), KEY_QUERY_VALUE);
	//if(hr == ERROR_SUCCESS)
	//{
	//	DWORD dw = _MAX_PATH;				
	//	dw = 17;
	//	if (ERROR_SUCCESS == key.QueryStringValue(L"PID", szPID, &dw))
	//	{
	//		key.Close();
	//		_tcsncpy(g_szPeerId, szPID, _countof(g_szPeerId));
	//		g_szPeerId[_countof(g_szPeerId) - 1] = _T('\0');
	//	}
	//}
	//if (g_szPeerId[0] == '\0')
	//{
	//	_tcsncpy(g_szPeerId, _T("0000000000000000"), _countof(g_szPeerId));
	//	g_szPeerId[_countof(g_szPeerId) - 1] = _T('\0');
	//}
	CComBSTR bstrPid;
	GetUserPID(&bstrPid);
	_tcsncpy(g_szPeerId, bstrPid.m_str, _countof(g_szPeerId));
	g_szPeerId[_countof(g_szPeerId) - 1] = _T('\0');
	std::wstring wstrXSHWnd = LaunchConfig::Instance()->m_wstrXSHWndClass;
	
	HWND hWnd = FindWindowW(wstrXSHWnd.c_str() ,_T("1"));
	if(hWnd)
	{
		DWORD dwProcessID = 0;
		GetWindowThreadProcessId(hWnd, &dwProcessID);
		TSDEBUG4CXX("TerminateProcess hWnd="<<hWnd<<" , dwProcessID="<<dwProcessID<<" ");
		KillProcessTree(dwProcessID, (DWORD)-1);
	}
	//////////////
 
	CoInitialize(NULL);	
	TCHAR szLaunchUrl[2048] = {0};
	std::wstring strUrlMD5;
	std::wstring strTempDir = GetLocalTempDir();
	std::wstring strFileName =  LaunchConfig::Instance()->m_wstrProduct + _T("main_v1.1.dat");
	std::wstring strFile = strTempDir + strFileName;		 
	if(IsSystemStartup())
	{
		//刚开机
	}
	else
	{
		//取launchurl 和md5 
		GetLaunchUrl(szLaunchUrl, 2048, strUrlMD5);
		std::wstring strFileMD5 = GetFileMD5(strFile.c_str());

		if(strUrlMD5 == strFileMD5)
		{
			TSDEBUG4CXX("same md5 , load local file : "<<strFileMD5.c_str());
		}
		else
		{
			HRESULT hr = E_FAIL;
			hr = URLDownloadToFile (NULL, szLaunchUrl, (LPWSTR)strFile.c_str(),0,NULL);//NULL, t->bstrUrl, tszPath,0,NULL
			if(SUCCEEDED(hr) && PathFileExists(strFile.c_str()))
			{
				//下载成功				
			}
			else
			{
				//下载失败，直接退出吧。否则统计不准，更是不可控
				SendLaunchFailedStat(_T("downloadingfile_failed_index.dat"), PathFindFileName(strFile.c_str()));
				TSDEBUG4CXX("download launch url "<<szLaunchUrl<<" to "<<strFile<<" failed!");
				return ;
			}
		}
	}


	////下成功或是md5一样的情况.... 或是首次开机,都是到这里:	

	//_Module.AddMessageLoop(&theLoop);
	HRESULT hr = E_FAIL;
	CComPtr<IClassFactory> spCF;
	DllGetClassObject(*__pobjMap_CXScriptHost->pclsid, IID_IClassFactory,  (LPVOID*)&spCF);
	if(!spCF)
		return ;

	CComPtr<IXScriptHost> spScriptHost;
	hr = spCF->CreateInstance(NULL, IID_IXScriptHost, (void **)&spScriptHost);
	if(SUCCEEDED(hr) && spScriptHost)
	{
		spScriptHost->Load( (BSTR)strFile.c_str(), 0 );
		CComVariant v;
		hr = spScriptHost->Run(&v);
		if(SUCCEEDED(hr))
		{
			CMessageLoop theLoop;
			int nRet = theLoop.Run();
			nRet;
		}
		else
		{
			SendLaunchFailedStat(_T("downloadingfile_loadfailed_index.dat"), PathFindFileName((LPCTSTR)strFile.c_str())); 
		}
		v.Clear();	
		spScriptHost.Release();
		spCF.Release();
		//TerminateProcess(GetCurrentProcess(), nRet);
	}	

	return ;
}
