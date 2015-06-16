#include "stdafx.h"
#include "Network.h"
#include "ultra\string-op.h"
#include "ultra\file-op.h"
#include "ultra\md5.h"
#include "wget\wgetExport.h"
#include <process.h>


CNetwork::CNetwork()
{

}

CNetwork::~CNetwork()
{

}

bool CNetwork::HttpDownloadString( std::wstring wstrRequest, std::wstring& wstrReturn )
{
	char szUrl[1024] = {0};
	strcpy(szUrl, ultra::_T2A(wstrRequest).c_str());
	char szPath[MAX_PATH] = {0};
	ExpandEnvironmentStringsA(LOCAL_DIR, szPath, MAX_PATH);
	::PathAppendA(szPath, "dlstring");
	int nRet = DownLoad(szUrl, szPath);
	if (nRet == 0)
	{
		HANDLE hFile = CreateFileA(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD dwLen =0;
		dwLen = GetFileSize(hFile, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			char* szBuffer = new char[dwLen+1];
			ZeroMemory(szBuffer, dwLen+1);
			DWORD dwRead = 0;
			ReadFile(hFile, szBuffer, dwLen, &dwRead, NULL);
			wstrReturn = ultra::_A2T(szBuffer);
			CloseHandle(hFile);
			delete[] szBuffer;
			DeleteFileA(szPath);
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}
}

bool CNetwork::HttpDownloadString( std::wstring wstrRequest, std::string& strReturn )
{
	char szUrl[1024] = {0};
	strcpy(szUrl, ultra::_T2A(wstrRequest).c_str());
	char szPath[MAX_PATH] = {0};
	ExpandEnvironmentStringsA(LOCAL_DIR, szPath, MAX_PATH);
	::PathAppendA(szPath, "dlstring");
	int nRet = DownLoad(szUrl, szPath);
	if (nRet == 0)
	{
		HANDLE hFile = CreateFileA(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD dwLen =0;
		dwLen = GetFileSize(hFile, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			char* szBuffer = new char[dwLen+1];
			ZeroMemory(szBuffer, dwLen+1);
			DWORD dwRead = 0;
			ReadFile(hFile, szBuffer, dwLen, &dwRead, NULL);
			char szPrint[100] = {0};
			sprintf(szPrint, "dwRead:%d dwLen:%d", dwRead, dwLen);
			strReturn = szBuffer;
			CloseHandle(hFile);
			delete[] szBuffer;
			DeleteFileA(szPath);
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}
}

bool CNetwork::HttpDownloadFile( std::wstring wstrRequest, std::wstring wstrFilePath )
{
	std::wstring wstrFilePathTmp = wstrFilePath + L".tmp";
	MoveFile(wstrFilePath.c_str(), wstrFilePathTmp.c_str());
	char szUrl[1024] = {0};
	strcpy(szUrl, ultra::_T2A(wstrRequest).c_str());
	char szPath[MAX_PATH] = {0};
	strcpy(szPath, ultra::_T2A(wstrFilePath).c_str());
	int nRet = DownLoad(szUrl, szPath);
	if (nRet == 0)
	{
		DeleteFile(wstrFilePathTmp.c_str());
	}
	else
	{
		MoveFile(wstrFilePathTmp.c_str(), wstrFilePath.c_str());
	}

	return (nRet == 0);
}

bool CNetwork::Report( std::wstring wstrRequest )
{
	WCHAR szBuffer[MAX_PATH] = {0};
	GetPrivateProfileString(L"report", L"cid", L"0", szBuffer, MAX_PATH, (ultra::GetModuleFilePath()+L"cfg.ini").c_str());

	std::wstring* pwstrRequest = new std::wstring;
	*pwstrRequest = wstrRequest + L"&cid="+szBuffer;
	CloseHandle((HANDLE)_beginthreadex(NULL, 0, CNetwork::ReportProc, 
		pwstrRequest, 0, NULL));
	return 0;
}

bool CNetwork::ReportGoogleStat(const TCHAR *ea, const TCHAR *el,long ev)
{
	WCHAR szBuffer[MAX_PATH] = {0};

	std::wstring wstrPeerID;
	{
		WCHAR szCfgPath[MAX_PATH] = {0};
		if (!GetUpdateCfgPath(szCfgPath))
		{
			return false;
		}
		PathAppend(szCfgPath, L"cfg.ini");

		WCHAR szBuffer[MAX_PATH] = {0};
		GetPrivateProfileString(L"info", L"cid", L"", szBuffer, MAX_PATH, szCfgPath);
		if (wcscmp(szBuffer, L"") == 0)
		{
			GetPeerId_(wstrPeerID);
			WritePrivateProfileString(L"info",L"cid",wstrPeerID.c_str(),szCfgPath);
		}
		else
		{
			wstrPeerID = szBuffer;
		}
	}
	std::wstring* pwstrRequest = new std::wstring;
	TCHAR szStat[1024] = {0};
	TCHAR *ec = SERVICE_NAME_L;
	if (ea == NULL)
	{
		ea = L"null";
	}
	if (el == NULL)
	{
		el = L"null";
	}
	_stprintf(szStat, L"http://www.google-analytics.com/collect?v=1&tid=UA-64056260-1&cid=%s&t=event&ec=%s&ea=%s&el=%s&ev=%lu",wstrPeerID.c_str(),ec,ea,el,ev);

	szStat[1023] = '\0';
	*pwstrRequest = szStat;
	CloseHandle((HANDLE)_beginthreadex(NULL, 0, CNetwork::ReportProc, pwstrRequest, 0, NULL));
	return 0;
}

CNetwork* CNetwork::m_network = NULL;

CNetwork* CNetwork::Instance()
{
	if (m_network == NULL)
	{
		m_network = new CNetwork;

		char szPath[MAX_PATH] = {0};
		ExpandEnvironmentStringsA(LOCAL_DIR, szPath, MAX_PATH);
		CreateDirectoryA(szPath, NULL);

		InitWget(NULL, 0);
		//InitWget("dl.log", 1);
		setval("timeout", "2");
		setval("tries", "2");
	}
	return m_network;
}

unsigned int _stdcall CNetwork::ReportProc( void* param )
{
	std::wstring* wstrRequest = (std::wstring*)param;
	std::wstring wstrReturn;
	OutputDebugStringW(wstrRequest->c_str());
	CNetwork::Instance()->HttpDownloadString(*wstrRequest, wstrReturn);
	delete wstrRequest;
	return 0;
}




#ifndef FOLDERID_Public

#ifndef DEFINE_KNOWN_FOLDER
#define DEFINE_KNOWN_FOLDER(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	EXTERN_C const GUID DECLSPEC_SELECTANY name \
	= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#endif

DEFINE_KNOWN_FOLDER(FOLDERID_Public, 0xDFDF76A2, 0xC82A, 0x4D63, 0x90, 0x6A, 0x56, 0x44, 0xAC, 0x45, 0x73, 0x85);
#endif

extern "C" typedef HRESULT (__stdcall *PSHGetKnownFolderPath)(  const  GUID& rfid, DWORD dwFlags, HANDLE hToken, PWSTR* pszPath);

bool GetUpdateCfgPath(wchar_t* szCfgPath, bool bCreatePath)
{
	static wchar_t szPusherPath[MAX_PATH] = {0};
	if(_T('\0') == szPusherPath[0])
	{
		if (ultra::OSIsVista())
		{
			HMODULE hModule = ::LoadLibrary(L"shell32.dll");
			PSHGetKnownFolderPath SHGetKnownFolderPath = (PSHGetKnownFolderPath)GetProcAddress( hModule, "SHGetKnownFolderPath" );
			if (NULL == SHGetKnownFolderPath)
			{
				return false;
			}
			PWSTR szPath = NULL;
			HRESULT hr = SHGetKnownFolderPath( FOLDERID_Public, 0, NULL, &szPath );
			if ( FAILED( hr ) )
			{
				FreeLibrary(hModule);
				return false;
			}
			wcscpy(szPusherPath, szPath);
			::CoTaskMemFree( szPath );

			FreeLibrary(hModule);
		}
		else
		{
			HRESULT hr = SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, szPusherPath);
			if ( FAILED( hr ) )
			{
				return false;
			}
		}
		::PathAppend(szPusherPath, SERVICE_NAME_L);
	}

	if(bCreatePath)
	{
		if(!::PathFileExists(szPusherPath))
		{
			SHCreateDirectoryEx(NULL, szPusherPath, NULL);
		}
	}
	wcscpy(szCfgPath, szPusherPath);
	return true;
}