// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "OfficeAddin_i.h"
#include "dllmain.h"
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")
#include <string>

COfficeAddinModule _AtlModule;
HINSTANCE g_hModule = NULL;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_hModule = hInstance;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return _AtlModule.DllMain(dwReason, lpReserved); 
}

class SafeHandle
{
private:
	 HANDLE m_slh;
	 int m_slc;
	 CRITICAL_SECTION m_csl;
	 bool b_Init;
	 SafeHandle(){
		 m_slh = CreateEvent(NULL,TRUE,TRUE,NULL);
		 m_slc = 0;
		 if(!b_Init){
			 InitializeCriticalSection(&m_csl);
			 b_Init = true;
		 }
	 };
	 ~SafeHandle(){
	 };
public:
	static SafeHandle* Instance(){
		static SafeHandle sh;
		return &sh;
	}
	void ResetUserHandle(){
		if(!b_Init){
			InitializeCriticalSection(&m_csl);
			b_Init = true;
		}
		EnterCriticalSection(&m_csl);
		if(m_slc == 0){
			ResetEvent(m_slh);
		}
		++m_slc; //引用计数加1
		//TSDEBUG4CXX("ResetUserHandle  s_ListenCount = "<<m_slc);
		LeaveCriticalSection(&m_csl);
	};

	void SetUserHandle(){
		EnterCriticalSection(&m_csl);
		--m_slc;//引用计数减1
		//TSDEBUG4CXX("SetUserHandle  s_ListenCount = "<<m_slc);
		if (m_slc == 0){
			SetEvent(m_slh);
		}
		LeaveCriticalSection(&m_csl);
	};
	void WaitForStat(){
		DWORD ret = WaitForSingleObject(m_slh, 20000);
		if (ret == WAIT_TIMEOUT){
		}
		else if (ret == WAIT_OBJECT_0){	
		}
		if(b_Init){
			DeleteCriticalSection(&m_csl);
		}
	};
};

class PeerID{
private:
	static bool GetMacByGetAdaptersAddresses(std::wstring& macOUT){
		bool ret = false;

		ULONG outBufLen = sizeof(IP_ADAPTER_ADDRESSES);
		PIP_ADAPTER_ADDRESSES pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
		if (pAddresses == NULL) 
			return false;
		// Make an initial call to GetAdaptersAddresses to get the necessary size into the ulOutBufLen variable
		if(GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW)
		{
			free(pAddresses);
			pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
			if (pAddresses == NULL) 
				return false;
		}

		if(GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == NO_ERROR)
		{
			// If successful, output some information from the data we received
			for(PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses; pCurrAddresses != NULL; pCurrAddresses = pCurrAddresses->Next)
			{
				// 确保MAC地址的长度为 00-00-00-00-00-00
				if(pCurrAddresses->PhysicalAddressLength != 6)
					continue;
				wchar_t acMAC[32];
				wsprintf(acMAC, L"%02X%02X%02X%02X%02X%02X",
					int (pCurrAddresses->PhysicalAddress[0]),
					int (pCurrAddresses->PhysicalAddress[1]),
					int (pCurrAddresses->PhysicalAddress[2]),
					int (pCurrAddresses->PhysicalAddress[3]),
					int (pCurrAddresses->PhysicalAddress[4]),
					int (pCurrAddresses->PhysicalAddress[5]));
				macOUT = acMAC;
				ret = true;
				break;
			}
		} 

		free(pAddresses);
		return ret;
	}

public:
	static void GetPeerId(std::wstring & wstrPeerId){
		HKEY hKEY;
		LPCWSTR data_Set= L"Software\\WordEncLock";
		if (ERROR_SUCCESS == ::RegOpenKeyExW(HKEY_LOCAL_MACHINE,data_Set,0,KEY_READ,&hKEY))
		{
			wchar_t szValue[256] = {0};
			DWORD dwSize = 256;
			DWORD dwType = REG_SZ;
			if (::RegQueryValueExW(hKEY,L"PeerId", 0, &dwType, (LPBYTE)szValue, &dwSize) == ERROR_SUCCESS)
			{
				wstrPeerId = szValue;
				return;
			}
			::RegCloseKey(hKEY);
		}
		std::wstring wstrMAC;
		if (!GetMacByGetAdaptersAddresses(wstrMAC)){
			wstrMAC = L"000000000000";
		}
		wstrPeerId = wstrMAC;
		srand( (unsigned)time( NULL ) );
		for(int i=0;i<4;i++){
			wchar_t szRam[2] = {0};
			wsprintf(szRam,L"%X", rand()%16);
			szRam[1]=L'\0';
			wstrPeerId += szRam;
		}
		HKEY hKey, hTempKey;
		if (ERROR_SUCCESS == ::RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"Software",0,KEY_SET_VALUE, &hKey))
		{
			if (ERROR_SUCCESS == ::RegCreateKeyW(hKey, L"WordEncLock", &hTempKey)){
				::RegSetValueExW(hTempKey, L"PeerId", 0, REG_SZ, (LPBYTE)wstrPeerId.c_str(), wstrPeerId.length()*2+1);
			}
			RegCloseKey(hKey);
		}
	}
};

class SendState{
private:
	static DWORD WINAPI SendHttpStatThread(LPVOID pParameter){
		CHAR szUrl[MAX_PATH] = {0};
		strcpy(szUrl,(LPCSTR)pParameter);
		delete [] pParameter;
		pParameter = NULL;
		CHAR szBuffer[MAX_PATH] = {0};
		::CoInitialize(NULL);
		HRESULT hr = E_FAIL;
		__try
		{
			hr = ::URLDownloadToCacheFileA(NULL, szUrl, szBuffer, MAX_PATH, 0, NULL);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			//TSDEBUG4CXX("URLDownloadToCacheFile Exception !!!");
		}
		::CoUninitialize();
		SafeHandle::Instance()->SetUserHandle();
		return SUCCEEDED(hr)?ERROR_SUCCESS:0xFF;
	}
public:
	static void SendAnyHttpStat(CHAR *ec,CHAR *ea, CHAR *el,long ev){
		if (ec == NULL || ea == NULL) return ;
		CHAR* szURL = new CHAR[MAX_PATH];
		memset(szURL, 0, MAX_PATH);
		std::wstring wstrPid;
		PeerID::GetPeerId(wstrPid);
		std::string strPid = (char*)_bstr_t(wstrPid.c_str());
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
		sprintf(szURL, "http://www.google-analytics.com/collect?v=1&tid=UA-58424540-1&cid=%s&t=event&ec=%s&ea=%s%s",strPid.c_str(),ec,ea,str.c_str());

		SafeHandle::Instance()->ResetUserHandle();
		DWORD dwThreadId = 0;
		HANDLE hThread = CreateThread(NULL, 0, SendHttpStatThread, (LPVOID)szURL,0, &dwThreadId);
		CloseHandle(hThread);
	}

};

//导出接口
extern "C" __declspec(dllexport) void SendAnyHttpStat(CHAR *ec,CHAR *ea, CHAR *el,long ev){
	SendState::SendAnyHttpStat(ec, ea, el, ev);
};

extern "C" __declspec(dllexport) void Exit(){
	SafeHandle::Instance()->WaitForStat();
	TerminateProcess(GetCurrentProcess(), 0);
};
