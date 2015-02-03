// Utility.cpp : CUtility 的实现

#include "stdafx.h"
#include "Utility.h"
#include <iphlpapi.h>
#include "map"
#include "xstring"
#include "winuser.h"
#include <time.h>
#include "tlhelp32.h"

#include "windows.h"
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Version.lib")
#include "LaunchHelper.h"
#include "AES.h"
#include <fstream>
#include <shlobj.h>

// CUtility

STDMETHODIMP CUtility::GetPID(BSTR* ppid)
{
	// TODO: 在此添加实现代码
	//CComVariant bstrPID;
	//CRegKey key;
	//HRESULT hr = key.Open(HKEY_CURRENT_USER, LaunchConfig::Instance()->m_wstrRegisterPath.c_str(), KEY_QUERY_VALUE);
	//if(hr == ERROR_SUCCESS)
	//{
	//	TCHAR szPID[32] = {0};
	//	ULONG nLen = sizeof szPID / sizeof szPID[0];
	//	key.QueryStringValue(_T("PID"), szPID, &nLen);
	//	bstrPID = szPID;
	//}
	//if(bstrPID.vt == VT_BSTR && wcslen(bstrPID.bstrVal)==16)		
	//{
	//	CComBSTR bstr = bstrPID.bstrVal;
	//	*ppid = bstr.Detach();
	//	return 0;
	//}

	//std::wstring result;
	//IP_ADAPTER_INFO Data[12];
	//ULONG len = sizeof(IP_ADAPTER_INFO) * 12;
	//int ret_val = 0;
	//PIP_ADAPTER_INFO pDatas = Data;
	//if((GetAdaptersInfo(pDatas, &len)) == ERROR_SUCCESS)
	//{
	//	if( pDatas )
	//	{
	//		for(int i = 1; i < 12; i++)
	//		{
	//			std::string desc = pDatas->Description;
	//			if ( desc.find("PPP", 0) != desc.npos
	//				|| desc.find("VMare", 0) != desc.npos
	//				|| desc.find("Virtual", 0) != desc.npos
	//				|| desc.find("SLIP", 0) != desc.npos
	//				|| desc.find("P.P.P", 0) != desc.npos )
	//			{
	//				pDatas = pDatas->Next;
	//				if(pDatas == NULL)
	//				{
	//					break;
	//				}
	//			}
	//			else
	//			{
	//				srand( (unsigned)time( NULL ) );
	//				int rnd = rand();
	//				TCHAR szAddress[32] = {0};
	//				wsprintf(szAddress, L"%0.4x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x",rnd, pDatas->Address[0], pDatas->Address[1],pDatas->Address[2],
	//					pDatas->Address[3], pDatas->Address[4], pDatas->Address[5]);
	//				result = szAddress;
	//				break;
	//			}
	//		}
	//	}
	//}

	//CComBSTR bstrpid(result.c_str());
	//bstrpid.ToUpper();
	//if(bstrpid.Length() == 16)
	//{
	//	CRegKey key;
	//	HRESULT hr = key.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\CloudScriptHost"), KEY_SET_VALUE);
	//	if(hr == ERROR_SUCCESS)
	//	{
	//		key.SetStringValue(_T("PID"), bstrpid.m_str); 
	//	} 
	//}
	//*ppid = bstrpid.Detach();
	//return S_OK;
	GetUserPID(ppid);
	return S_OK;
}


using namespace std;
// 得到文件的版本号
static wstring GetFileVersionString(wstring wstrFilename)
{
	//TSAUTO();
	ATLASSERT(PathFileExists(wstrFilename.c_str()));
	if(!PathFileExists(wstrFilename.c_str()))
	{
		return L"0.0.0.0" ;
	}	
	wstring wstrVersion = L"0.0.0.0";
	//wstrVersion.empty();

	WCHAR wszVersion[128];
	wszVersion[0] = 0;

	DWORD dwHandle = 0;
	DWORD dwSize = ::GetFileVersionInfoSize(wstrFilename.c_str(), &dwHandle);
	if(dwSize > 0)
	{
		TCHAR * pVersionInfo = new TCHAR[dwSize+1];
		if(pVersionInfo)
		{
			if(::GetFileVersionInfo(wstrFilename.c_str(), dwHandle, dwSize, pVersionInfo))
			{
				VS_FIXEDFILEINFO * pvi;
				UINT uLength = 0;
				if(::VerQueryValue(pVersionInfo, _T("\\"), (void **)&pvi, &uLength))
				{
					wsprintf(wszVersion, _T("%d.%d.%d.%d"),
						HIWORD(pvi->dwFileVersionMS), LOWORD(pvi->dwFileVersionMS),
						HIWORD(pvi->dwFileVersionLS), LOWORD(pvi->dwFileVersionLS));

					wstrVersion = wszVersion ;
				}
			}
			delete[] pVersionInfo;
			pVersionInfo = NULL;
		}
	}

	return wstrVersion ;
}

static unsigned __int64 GetFileVersion(const TCHAR* file_path, unsigned __int64 * VerionTimeStamp = NULL)
{
	if (FALSE == PathFileExists(file_path))
		return 0;
	unsigned __int64 i64Version = 0;
	VS_FIXEDFILEINFO *FileInfo = NULL;
	void *info = NULL;

	DWORD infosize = ::GetFileVersionInfoSize((TCHAR*)file_path, 0);
	info = malloc(infosize+4);
	if( NULL != info ) 
	{
		UINT   FileInfoSize;   
		if (FALSE != ::GetFileVersionInfo((TCHAR*)file_path,   0,   infosize,   info))
		{
			if (FALSE != ::VerQueryValue(info, _T("\\"), (void **)&FileInfo, &FileInfoSize))
			{
				i64Version = ((unsigned __int64)FileInfo->dwFileVersionMS << 32) | FileInfo->dwFileVersionLS;
				if (VerionTimeStamp)
				{
					WIN32_FILE_ATTRIBUTE_DATA attribute;
					if ( GetFileAttributesEx(file_path, GetFileExInfoStandard,	&attribute ) )
					{
						unsigned __int64 i64CreateTime = ((unsigned __int64)attribute.ftCreationTime.dwHighDateTime << 32) | attribute.ftCreationTime.dwLowDateTime;
						unsigned __int64 i64LastWriteTime = ((unsigned __int64)attribute.ftLastWriteTime.dwHighDateTime << 32) | attribute.ftLastWriteTime.dwLowDateTime;
						*VerionTimeStamp = (i64CreateTime>i64LastWriteTime) ? i64CreateTime : i64LastWriteTime;
					}
				}
			}
			else
			{
				TSWARN4CXX( _T("[GetFileVersion] VerQueryValue(%s) 没有版本信息")<< file_path);
			}
		}
		else
		{
			TSWARN4CXX( _T("[GetFileVersion] GetFileVersionInfo(%s) 没有版本信息")<< file_path);
		}
		free(info);
		info = NULL;
	}   
	else     
	{   
		TSWARN4CXX( _T("[GetFileVersion] GetFileVersionInfoSize(%s) 没有版本信息")<< file_path);
	}

	return i64Version;
}

static LONG GetFileBuildNum(wstring wstrFilename)
{
	TSAUTO();
	LONG lnBuildNum = 0 ;

	DWORD dwHandle = 0;
	DWORD dwSize = ::GetFileVersionInfoSize(wstrFilename.c_str(), &dwHandle);
	if(dwSize > 0)
	{
		TCHAR * pVersionInfo = new TCHAR[dwSize+1];
		if(pVersionInfo)
		{
			if(::GetFileVersionInfo(wstrFilename.c_str(), dwHandle, dwSize, pVersionInfo))
			{
				VS_FIXEDFILEINFO * pvi;
				UINT uLength = 0;
				if(::VerQueryValue(pVersionInfo, _T("\\"), (void **)&pvi, &uLength))
				{
					lnBuildNum = (LONG)(LOWORD(pvi->dwFileVersionLS));
				}
			}
			delete []pVersionInfo;
			pVersionInfo = NULL;
		}
	}

	return lnBuildNum ;
}
STDMETHODIMP CUtility::GetHostVersion(BSTR* pver)
{
	// TODO: 在此添加实现代码
	HINSTANCE hInst = _AtlBaseModule.GetModuleInstance();
	TCHAR szPath[_MAX_PATH] = {0};
	GetModuleFileName(hInst, szPath, _MAX_PATH);
	wstring str = GetFileVersionString(szPath);
	CComBSTR bstr(str.c_str());
	*pver = bstr.Detach();
	return S_OK;
}

STDMETHODIMP CUtility::GetHostBuild(LONG* pv)
{
	// TODO: 在此添加实现代码
	CComBSTR bstrVer;
	this->GetHostVersion(&bstrVer);

	int b1,b2,b3,b4 = 0;
	if(bstrVer.Length()>0)
	{
		swscanf(bstrVer.m_str, L"%d.%d.%d.%d", &b1, &b2, &b3, &b4);
		*pv = b4;
	}

	return S_OK;
}

STDMETHODIMP CUtility::GetEIVersion(BSTR* pver)
{
	// TODO: 在此添加实现代码
	wstring strPath;
	CRegKey key;
	TCHAR szPath[_MAX_PATH] = {0};
	ULONG nLen = _MAX_PATH;
	HRESULT hr = key.Open(HKEY_CURRENT_USER,  LaunchConfig::Instance()->m_wstrRegisterPath.c_str(), KEY_QUERY_VALUE);
	if(hr == ERROR_SUCCESS)
	{
		key.QueryStringValue(_T("Path"), szPath, &nLen);
		if(::PathFileExists(szPath))
		{
			strPath = szPath;
		}
		key.Close();
	}
	//if(strPath.length() == 0)
	//{
	//	hr = key.Open(HKEY_CLASSES_ROOT, L"CLSID\\{EE606F2F-AA02-482F-9A83-17219D749CBE}\\InprocServer32", KEY_QUERY_VALUE);
	//	if(hr == ERROR_SUCCESS)
	//	{
	//		key.QueryStringValue(NULL, szPath, &nLen);
	//		if(::PathFileExists(szPath))
	//		{
	//			strPath = szPath;

	//		}
	//		key.Close();

	//	}
	//}
	if(strPath.length())
	{
		wstring strVer = GetFileVersionString(strPath);
		CComBSTR bstrVer (strVer.c_str());
		*pver = bstrVer.Detach();
	}

	return S_OK;
}

STDMETHODIMP CUtility::GetEIBuild(LONG* pv)
{
	// TODO: 在此添加实现代码
	CComBSTR bstrVer;
	this->GetEIVersion(&bstrVer);
	int b1,b2,b3,b4 = 0;
	if(bstrVer.Length()>0)
	{
		swscanf(bstrVer.m_str, L"%d.%d.%d.%d", &b1, &b2, &b3, &b4);
		*pv = b4;
	}

	return S_OK;
}


STDMETHODIMP CUtility::ShellExcute(BSTR app, BSTR param, BSTR workdir, LONG startupflag, LONG *pVal)
{
	// TODO: 在此添加实现代码
	TSAUTO();
	TCHAR szApp[MAX_PATH] = {0};
	::PathCanonicalize(szApp, app);
	TCHAR szApp2[MAX_PATH] = {0};
	::ExpandEnvironmentStrings(szApp, szApp2, MAX_PATH);

	TCHAR szParam[MAX_PATH] = {0};
	::PathCanonicalize(szParam, param);
	TCHAR szParam2[MAX_PATH] = {0};
	::ExpandEnvironmentStrings(szParam, szParam2, MAX_PATH);

	TCHAR szWorkDir[MAX_PATH] = {0};
	::PathCanonicalize(szWorkDir, workdir);
	TCHAR szWorkDir2[MAX_PATH] = {0};
	::ExpandEnvironmentStrings(szWorkDir, szWorkDir2, MAX_PATH);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);					
	si.dwFlags = startupflag;  
	ZeroMemory( &pi, sizeof(pi) );

	if(CreateProcess( app[0] == '\0' ? NULL : szApp2,        
		(LPTSTR)szParam2, // Command line. 
		NULL,                         // Process handle not inheritable. 
		NULL,						  // Thread handle not inheritable. 
		TRUE,						  // Set handle inheritance to FALSE. 
		0,							  // No creation flags. 
		NULL,						  // Use parent's environment block. 
		szWorkDir2,						  // Use parent's starting directory. 
		&si,						  // Pointer to STARTUPINFO structure.
		&pi ))
	{
		LONG l = pi.dwProcessId;
		CloseHandle( pi.hThread);
		CloseHandle( pi.hProcess);
		TSDEBUG4CXX("pi.dwProcessId="<< pi.dwProcessId);
		*pVal = l;
	}
	if(*pVal)
	{
		m_lChildProcesses.push_back(*pVal);
	}
	return S_OK;
}


STDMETHODIMP CUtility::lnterrupted(LONG l)
{
	// TODO: 在此添加实现代码

	return E_NOTIMPL;
}

STDMETHODIMP CUtility::GetLastInputInfo(LONG* l)
{

	// TODO: 在此添加实现代码
	LASTINPUTINFO lii;
	lii.cbSize = sizeof(LASTINPUTINFO);
	if(::GetLastInputInfo(&lii))
	{
		*l = lii.dwTime;
	}
	return S_OK;
}

STDMETHODIMP CUtility::SetEnvironmentVariable(BSTR name, BSTR value)
{
	// TODO: 在此添加实现代码
	::SetEnvironmentVariable(name, value);
	return S_OK;
}

STDMETHODIMP CUtility::GetEnvironmentVariable(BSTR name, BSTR* pvalue)
{
	// TODO: 在此添加实现代码
	TCHAR szValue[2048] = {0};
	::GetEnvironmentVariable(name , szValue, 2048);
	CComBSTR bstr(szValue);
	*pvalue = bstr.Detach();
	return S_OK;
}

STDMETHODIMP CUtility::GetTickCount(LONG* l)
{
	// TODO: 在此添加实现代码

	*l = ::GetTickCount();
	return S_OK;
}

STDMETHODIMP CUtility::get_windows(IDispatch** pVal)
{
	// TODO: 在此添加实现代码
	TSAUTO();

	return E_NOTIMPL;
}

STDMETHODIMP CUtility::IsSupportOpencl(LONG* pl)
{
	TSAUTO();
	// TODO: 在此添加实现代码 	
	unsigned int m_platform = 0;
	unsigned int m_devicecount = 0;
	typedef struct _cl_platform_id *    cl_platform_id;
	* pl = 0;

	HMODULE hMod =  LoadLibrary(L"opencl.dll");
	if(NULL == hMod)
	{
		return S_OK; 
	}
	typedef int (__stdcall*	PclGetPlatformIDs)(unsigned int , cl_platform_id * , unsigned int*);
	PclGetPlatformIDs pclGetPlatformIDs = (PclGetPlatformIDs)GetProcAddress(hMod, "clGetPlatformIDs");

	typedef int (__stdcall*	PclGetDeviceIDs)(cl_platform_id ,unsigned __int64 , unsigned int, void *  ,   unsigned int * );
	PclGetDeviceIDs pclGetDeviceIDs = (PclGetDeviceIDs)GetProcAddress(hMod,"clGetDeviceIDs");

	if(NULL == pclGetPlatformIDs || NULL == pclGetDeviceIDs)
	{
		FreeLibrary(hMod);
		return S_OK;
	}

	unsigned int numplatforms = 0;
	pclGetPlatformIDs(0,NULL,&numplatforms);
	//printf("%d OpenCL platforms found\n",numplatforms);
	if(0 >= numplatforms)
	{
		FreeLibrary(hMod);
		return S_OK;
	}

	if(numplatforms>0 && m_platform>=0 && m_platform<numplatforms)
	{
#define CL_DEVICE_TYPE_GPU                          (1 << 2)

		cl_platform_id *pids;
		pids=new cl_platform_id[numplatforms];
		pclGetPlatformIDs(numplatforms,pids,NULL);
		pclGetDeviceIDs(pids[m_platform],CL_DEVICE_TYPE_GPU,0,NULL,&m_devicecount);

		//printf("%d OpenCL GPU devices found on platform %d\n",m_devicecount,m_platform);
		if(0 >=m_devicecount)
		{
			FreeLibrary(hMod);
			return S_OK;
		}		 
		else
		{
			* pl = 1;
			FreeLibrary(hMod);
			return S_OK;
		}

	}	
	FreeLibrary(hMod);
	return S_OK;
}

STDMETHODIMP CUtility::Open(BSTR url)
{
	// TODO: 在此添加实现代码
	TSAUTO();
	::ShellExecute(NULL,_T("open"), url, _T(""), NULL,SW_SHOWNORMAL);

	return S_OK;
}

STDMETHODIMP CUtility::OpenUrl(BSTR url, LONG mode)
{
	// TODO: 在此添加实现代码
	TSAUTO();
	::ShellExecute(NULL,_T("open"), url, _T(""), NULL,mode);


	return S_OK;
}
struct _tDownload{
	CComBSTR bstrUrl;		
	CComBSTR bstrPath;
	CComVariant expression;
	HWND hWnd;
};
DWORD DownLoadFile(void *p)
{
	struct _tDownload *t = (struct _tDownload*)p;

	TCHAR tszShortPath[_MAX_PATH] = {0};
	::PathCanonicalize(tszShortPath, t->bstrPath);
	TCHAR tszPath[_MAX_PATH] = {0};
	TCHAR tszDir[_MAX_PATH] = {0};
	::ExpandEnvironmentStrings(tszShortPath, tszPath, MAX_PATH);
	_tcscpy(tszDir, tszPath);
	PathAppend(tszDir, _T(".."));
	if(!PathFileExists(tszDir))
		CreateDirectory(tszDir,0);
	HRESULT hr = URLDownloadToFile(NULL, t->bstrUrl, tszPath,0,NULL);
	if(SUCCEEDED(hr) && PathFileExists(tszPath))
	{
		PostMessage(t->hWnd, WM_DOWNLOADFILE, (WPARAM)1,  (LPARAM)t);            
	}
	else
	{
		PostMessage(t->hWnd, WM_DOWNLOADFILE, (WPARAM)0,  (LPARAM)t);            
	}
	return 0;
}
STDMETHODIMP CUtility::DownloadURL(BSTR url, BSTR dest ,VARIANT expression,  VARIANT autoload)
{
	struct _tDownload *t = new struct _tDownload;
	t->bstrUrl = url;
	t->bstrPath = dest;
	t->expression = expression.pdispVal;
	t->hWnd = m_hWnd;
	DWORD dwThreadId = 0;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) DownLoadFile, (void*)t, 0,&dwThreadId);		

	return S_OK;
}


LRESULT	CUtility::OnDownloadFile(UINT /*uMsg*/, WPARAM  wParam , LPARAM  lParam , BOOL& /*bHandled*/)
{
	struct _tDownload *t = (struct _tDownload *)lParam;
	CComVariant p1 = (long )wParam;
	CComVariant p2 = t->bstrUrl;
	CComVariant p3 = t->bstrPath;
	//	this->RunScript(t->expression, p1, p2, p3);
	delete t;
	return 0;
}

void KillProcessTree(DWORD dwProcessID, DWORD dwExceptPID); 

STDMETHODIMP CUtility::TerminateProcess(LONG processid, LONG tree)
{
	TSAUTO();
	// TODO: 在此添加实现代码
	if(processid == 0)
	{
		LONG l = GetCurrentProcessId();
		KillProcessTree(l, l);
	}
	else
	{
		LONG l = processid;
		if(tree)
		{
			KillProcessTree(l, (DWORD)-1);
		}
		else
		{
			TerminateProcess(l, -1);
		}
	}
	return S_OK;
}



STDMETHODIMP CUtility::GetOrganizationName(BSTR path, BSTR* name)
{
	// TODO: 在此添加实现代码
	TSAUTO();
	TCHAR tszShortPath[MAX_PATH] = {0};
	::PathCanonicalize(tszShortPath, path);
	TCHAR tszPath[MAX_PATH] = {0};
	::ExpandEnvironmentStrings(tszShortPath, tszPath, MAX_PATH);
	if(!::PathFileExists(tszPath))
	{
		return S_FALSE;
	}
	CComBSTR bstr;
	AFileSignInfo ChkCurExeSignInfo(tszPath);
	bstr = ChkCurExeSignInfo.GetOrganizationName();
	*name = bstr.Detach();	
	return S_OK;
}

STDMETHODIMP CUtility::WaitForSingleObject(LONG h, LONG ms, LONG* ret)
{
	// TODO: 在此添加实现代码
	*ret = 	::WaitForSingleObject((HANDLE)(LONG_PTR)h, (DWORD)ms);
	return S_OK;
}

STDMETHODIMP CUtility::SetThreadExecutionState(LONG l)
{
	// TODO: 在此添加实现代码
	TSAUTO();
	::SetThreadExecutionState((EXECUTION_STATE) l); 
	return S_OK;
}

STDMETHODIMP CUtility::get___debugging(LONG* pVal)
{
	// TODO: 在此添加实现代码
	if (IsDebugging())
	{
		*pVal = 1;
	}
	else
	{
		*pVal = 0;
	}

	return S_OK;
}

STDMETHODIMP CUtility::EncryptFile(BSTR source,BSTR dest)
{
	// TODO: 在此添加实现代码
	ifstream fs;
	fs.open( CStringA(source), ios::in|ios::binary );
	bool b = fs.fail();											ATLASSERT( !b );
	if(b){
		return MK_E_CANTOPENFILE;}
	fs.seekg(0, ios::end);
	unsigned long n = (unsigned long)fs.tellg();
	fs.seekg(0, ios::beg);
	CStringA strScript;
	char * buf = strScript.GetBuffer( n+1 );
	fs.read( buf, n );
	buf[n] = '\0';
	fs.close();  

	int flen = ((n >> 4) + 1) << 4;
	char* out_str = (char*)malloc(flen + 1);
	memset(out_str, 0, flen + 1);
	unsigned char key[] = 
	{
		0x2b, 0x7e, 0x15, 0x16, 
		0x28, 0xae, 0xd2, 0xa6, 
		0xab, 0xf7, 0x15, 0x88, 
		0x09, 0xcf, 0x4f, 0x3c
	};
	try
	{
		strcpy(out_str,buf);
		AES aes(key);
		aes.Cipher((char*)out_str, n);

	}
	catch (...)
	{
		return S_OK;
	}
	TCHAR tszSaveDir[MAX_PATH] = {0};
	_tcsncpy(tszSaveDir, dest, MAX_PATH);
	::PathRemoveFileSpec(tszSaveDir);
	if (!::PathFileExists(tszSaveDir))
		::SHCreateDirectory(NULL, tszSaveDir);

	std::ofstream of(dest, std::ios_base::out|std::ios_base::binary);
	of.write((const char*)out_str, flen);

	free(out_str);
	return S_OK;
}