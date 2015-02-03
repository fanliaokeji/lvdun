#pragma once
#include <string>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")

#pragma once
class LaunchConfig {
public:
	LaunchConfig(void){}
	~LaunchConfig(void){}
	static LaunchConfig* Instance()
	{
		static LaunchConfig s;
		return &s;
	}
	BOOL Init()
	{			
		SetCurrentModulePathToEnvironment();

		if (!m_wstrCurrentModulePath.empty())
		{
			wchar_t dllDir [_MAX_PATH];
			wchar_t drive[_MAX_PATH];
			wchar_t dir[_MAX_PATH];
			wchar_t fname[_MAX_PATH];
			wchar_t ext [_MAX_PATH];
			wchar_t szCfgPath [_MAX_PATH];
			_wsplitpath(m_wstrCurrentModulePath.c_str(), drive,dir,fname,ext);
			::PathCombine(dllDir,drive,dir);
			::PathCombine(szCfgPath,dllDir,L"Config.ini");
			if (::PathFileExistsW(szCfgPath))
			{
				m_wstrCfgPath = szCfgPath;
				GetConfigValue(L"main",L"initurl",m_wstrInitUrl);
				GetConfigValue(L"main",L"xshwndclass",m_wstrXSHWndClass);
				GetConfigValue(L"main",L"hostmutex",m_wstrHostMutex);
				GetConfigValue(L"main",L"tid",m_wstrTID);
				GetConfigValue(L"main",L"registerkey",m_wstrRegisterPath);
				GetConfigValue(L"main",L"productname",m_wstrProduct);
				if (m_wstrInitUrl.empty()
					||m_wstrXSHWndClass.empty()
					||m_wstrHostMutex.empty()
					||m_wstrTID.empty()
					//||m_wstrRegisterPath.empty()
					||m_wstrProduct.empty()
					)
				{
					return FALSE;
				}
				m_wstrXSHWndClass = std::wstring(L"ScriptHost_")+m_wstrXSHWndClass;
				m_wstrRegisterPath = m_wstrProduct + L"Host";
				m_wstrRegisterPath = std::wstring(L"SOFTWARE\\")+m_wstrRegisterPath;
				m_wstrHostMutex =m_wstrHostMutex + L"_ScriptHost_Mutex";
				wchar_t szMainJSPath [_MAX_PATH];
				std::wstring wstrName =  m_wstrProduct+L"main.js";
				::PathCombine(szMainJSPath,dllDir,wstrName.c_str());
				m_wstrMainJS = szMainJSPath;
				return TRUE;
			}
		}
		return FALSE;
	};
private:
	//void GetInitUrl()
	//{
	//	std::wstring wstrOut = L"";
	//	GetConfigValue(L"main",L"initurl",wstrOut);
	//	return ;
	//}
	//void GetXSHWndClass()
	//{
	//	std::wstring wstrOut = L"";
	//	GetConfigValue(L"main",L"xwndclass",wstrOut);
	//	wstrOut = std::wstring(L"ScriptHost_")+wstrOut;
	//	return ;
	//}
	//std::wstring GetHostMutex()
	//{
	//	std::wstring wstrOut = L"";
	//	GetConfigValue(L"main",L"hostmutex",wstrOut);
	//	return wstrOut;
	//}
	//std::wstring GetReportTID()
	//{
	//	std::wstring wstrOut = L"";
	//	GetConfigValue(L"main",L"tid",wstrOut);
	//	return wstrOut;
	//}
	//std::wstring GetRegisterPathKey()
	//{
	//	std::wstring wstrOut = L"";
	//	GetConfigValue(L"main",L"registerkey",wstrOut);
	//	return wstrOut;
	//}
private:
	void SetCurrentModulePathToEnvironment()
	{
#define MY_MAX_ENV_STRING_BUF_SIZE	32768
		LPCTSTR pszPathEnvComponent = NULL;
		LPCTSTR pszPathEnvName = _T("Path");
		MEMORY_BASIC_INFORMATION m = { 0 };
		static HINSTANCE hCurrentModule = 0;
		if(NULL == hCurrentModule)
		{
			MEMORY_BASIC_INFORMATION m = { 0 };
			VirtualQuery(&hCurrentModule, &m, sizeof(MEMORY_BASIC_INFORMATION));
			hCurrentModule = (HINSTANCE) m.AllocationBase;
		}
		wchar_t szModulePath[MAX_PATH] = {0};
		if (0 != GetModuleFileName(hCurrentModule, szModulePath, MAX_PATH))
		{
			m_wstrCurrentModulePath = szModulePath;
			PathRemoveFileSpec(szModulePath);
			if (::PathFileExists(szModulePath) && ::PathIsDirectory(szModulePath))
			{
				TCHAR *szPathEnvOldValue = new TCHAR[MY_MAX_ENV_STRING_BUF_SIZE];
				::ZeroMemory(szPathEnvOldValue, MY_MAX_ENV_STRING_BUF_SIZE * sizeof(TCHAR));
				::GetEnvironmentVariable(pszPathEnvName, szPathEnvOldValue, MY_MAX_ENV_STRING_BUF_SIZE);

				TCHAR *szPathEnvNewValue = new TCHAR[MY_MAX_ENV_STRING_BUF_SIZE];
				::ZeroMemory(szPathEnvNewValue, MY_MAX_ENV_STRING_BUF_SIZE * sizeof(TCHAR));
				_sntprintf(szPathEnvNewValue, MY_MAX_ENV_STRING_BUF_SIZE, _T("%s;%s"), szModulePath, szPathEnvOldValue);

				::SetEnvironmentVariable(pszPathEnvName, szPathEnvNewValue);

				delete [] szPathEnvNewValue;
				delete [] szPathEnvOldValue;
			}
		}
	}
	void GetConfigValue(const wchar_t* szSection, const wchar_t* szKey,std::wstring &wstrOut)
	{
		TCHAR szValue[MAX_PATH] = {0};
		::GetPrivateProfileStringW(szSection, szKey, L"", szValue, MAX_PATH, m_wstrCfgPath.c_str());
		wstrOut = szValue;
		return ;
	}
private:
	std::wstring m_wstrCfgPath;
public:
	std::wstring m_wstrInitUrl;
	std::wstring m_wstrXSHWndClass;
	std::wstring m_wstrHostMutex;
	std::wstring m_wstrTID;
	std::wstring m_wstrRegisterPath;
	std::wstring m_wstrProduct;
	std::wstring m_wstrMainJS;
	std::wstring m_wstrCurrentModulePath;
};

static void GetUserPID(BSTR* ppid)
{
	CComVariant bstrPID;
	CRegKey key;
	HRESULT hr = key.Open(HKEY_CURRENT_USER, LaunchConfig::Instance()->m_wstrRegisterPath.c_str(), KEY_QUERY_VALUE);
	if(hr == ERROR_SUCCESS)
	{
		TCHAR szPID[32] = {0};
		ULONG nLen = sizeof szPID / sizeof szPID[0];
		key.QueryStringValue(_T("PID"), szPID, &nLen);
		bstrPID = szPID;
	}
	if(bstrPID.vt == VT_BSTR && wcslen(bstrPID.bstrVal)==16)		
	{
		CComBSTR bstr = bstrPID.bstrVal;
		*ppid = bstr.Detach();
		return;
	}

	std::wstring result;
	IP_ADAPTER_INFO Data[12];
	ULONG len = sizeof(IP_ADAPTER_INFO) * 12;
	int ret_val = 0;
	PIP_ADAPTER_INFO pDatas = Data;
	if((GetAdaptersInfo(pDatas, &len)) == ERROR_SUCCESS)
	{
		if( pDatas )
		{
			for(int i = 1; i < 12; i++)
			{
				std::string desc = pDatas->Description;
				if ( desc.find("PPP", 0) != desc.npos
					|| desc.find("VMare", 0) != desc.npos
					|| desc.find("Virtual", 0) != desc.npos
					|| desc.find("SLIP", 0) != desc.npos
					|| desc.find("P.P.P", 0) != desc.npos )
				{
					pDatas = pDatas->Next;
					if(pDatas == NULL)
					{
						break;
					}
				}
				else
				{
					srand( (unsigned)time( NULL ) );
					int rnd = rand();
					TCHAR szAddress[32] = {0};
					wsprintf(szAddress, L"%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.4x",pDatas->Address[0], pDatas->Address[1],pDatas->Address[2],
						pDatas->Address[3], pDatas->Address[4], pDatas->Address[5],rnd);
					result = szAddress;
					break;
				}
			}
		}
	}

	CComBSTR bstrpid(result.c_str());
	bstrpid.ToUpper();
	if(bstrpid.Length() == 16)
	{
		CRegKey key;
		HRESULT hr = key.Open(HKEY_CURRENT_USER, LaunchConfig::Instance()->m_wstrRegisterPath.c_str(), KEY_SET_VALUE);
		if(hr != ERROR_SUCCESS)
		{
			if(ERROR_SUCCESS == key.Create(HKEY_CURRENT_USER, LaunchConfig::Instance()->m_wstrRegisterPath.c_str()))
			{
				key.SetStringValue(_T("PID"), bstrpid.m_str); 
			}
		} 
		else
		{
			key.SetStringValue(_T("PID"), bstrpid.m_str);
		}
	}
	*ppid = bstrpid.Detach();
	return;
}


static bool WideStringToAnsiString(const std::wstring &strWide, std::string &strAnsi)
{
	bool bSuc = false;

	int cchAnsi = ::WideCharToMultiByte(CP_ACP, 0, strWide.c_str(), -1, NULL, 0, NULL, NULL);
	if (cchAnsi)
	{
		char *pszAnsi = new (std::nothrow) char[cchAnsi];
		if (pszAnsi)
		{
			::WideCharToMultiByte(CP_ACP, 0, strWide.c_str(), -1, pszAnsi, cchAnsi, NULL, NULL);
			strAnsi = pszAnsi;
			bSuc = true;

			delete [] pszAnsi;
			pszAnsi = NULL;
		}
	}

	return bSuc;
}