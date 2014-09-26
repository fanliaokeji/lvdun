// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "..\GSPre\PeeIdHelper.h"
#include <tslog\tslog.h>
#include <string>
// ATL Header Files
#include <atlbase.h>
#include <WTL/atlapp.h>

#include <Urlmon.h>
#pragma comment(lib, "Urlmon.lib")
#include <Windows.h>
#pragma comment(lib, "Version.lib")
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
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

DWORD WINAPI SendHttpStatThread(LPVOID pParameter)
{
	TSAUTO();
	TCHAR szUrl[MAX_PATH] = {0};
	_tcscpy(szUrl,(LPCWSTR)pParameter);

	TCHAR szBuffer[MAX_PATH] = {0};
	::CoInitialize(NULL);
	HRESULT hr = E_FAIL;
	__try
	{
		hr = ::URLDownloadToCacheFile(NULL, szUrl, szBuffer, MAX_PATH, 0, NULL);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		TSDEBUG4CXX("URLDownloadToCacheFile Exception !!!");
	}
	::CoUninitialize();

	return SUCCEEDED(hr)?ERROR_SUCCESS:0xFF;
}

extern "C" __declspec(dllexport) void SendAnyHttpStat(TCHAR *ec,TCHAR *ea, TCHAR *el,long ev)
{
	if (ec == NULL || ea == NULL)
	{
		return ;
	}

	TSAUTO();
	TCHAR szURL[MAX_PATH] = {0};
	std::wstring strPeerID;
	GetPeerId_(strPeerID);
	std::wstring str = L"";
	if (el != NULL )
	{
		str +=L"&el=";
		str +=el;
	}
	if (ev != 0)
	{
		TCHAR szev[MAX_PATH] = {0};
		_stprintf(szev,L"&ev=%ld",ev);
		str += szev;
	}
	_stprintf(szURL, _T("http://www.google-analytics.com/collect?v=1&tid=UA-55122790-1&cid=%s&t=event&ec=%s&ea=%s%s"),strPeerID.c_str(),ec,ea,str.c_str());

	CComBSTR bstrUrl(szURL);
	DWORD dwThreadId = 0;
	HANDLE hThread = CreateThread(NULL, 0, SendHttpStatThread, (LPVOID) bstrUrl.Detach(),0, &dwThreadId);
	CloseHandle(hThread);
}


extern "C" __declspec(dllexport) void GetFileVersionString(TCHAR* pszFileName, TCHAR * pszVersionString)
{
	if(pszFileName == NULL || pszVersionString == NULL)
		return ;

	BOOL bResult = FALSE;
	DWORD dwHandle = 0;
	DWORD dwSize = ::GetFileVersionInfoSize(pszFileName, &dwHandle);
	if(dwSize > 0)
	{
		TCHAR * pVersionInfo = new TCHAR[dwSize+1];
		if(::GetFileVersionInfo(pszFileName, dwHandle, dwSize, pVersionInfo))
		{
			VS_FIXEDFILEINFO * pvi;
			UINT uLength = 0;
			if(::VerQueryValue(pVersionInfo, _T("\\"), (void **)&pvi, &uLength))
			{
				_stprintf(pszVersionString, _T("%d.%d.%d.%d"),
					HIWORD(pvi->dwFileVersionMS), LOWORD(pvi->dwFileVersionMS),
					HIWORD(pvi->dwFileVersionLS), LOWORD(pvi->dwFileVersionLS));
				bResult = TRUE;
			}
		}
		delete pVersionInfo;
	}
}


extern "C" __declspec(dllexport) void GetPeerID(TCHAR * pszPeerID)
{
	std::wstring strPeerID;
	GetPeerId_(strPeerID);
	_tcscpy(pszPeerID,strPeerID.c_str());
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