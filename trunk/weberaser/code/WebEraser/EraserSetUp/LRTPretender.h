#pragma once

#include <shlobj.h>
#include <list>
class CLRTAgent
{
public:
	BOOL InitLua(char* szInstallDir,char* szParam)
	{
		MEMORY_BASIC_INFORMATION m = { 0 };
		static HINSTANCE hCurrentModule = 0;
		if(NULL == hCurrentModule)
		{
			MEMORY_BASIC_INFORMATION m = { 0 };
			VirtualQuery(&hCurrentModule, &m, sizeof(MEMORY_BASIC_INFORMATION));
			hCurrentModule = (HINSTANCE) m.AllocationBase;
		}
		wchar_t szModulePath[MAX_PATH] = {0};
		if (0 == GetModuleFileName(hCurrentModule, szModulePath, MAX_PATH))
		{
			return FALSE;
		}
		PathRemoveFileSpec(szModulePath);

		wchar_t szLuaPath[MAX_PATH] = {0};
		::PathCombine(szLuaPath,szModulePath,_T("dclsetupload.dat"));

		wchar_t szLuaAgent[MAX_PATH] = {0};
		::PathCombine(szLuaAgent,szModulePath,_T("EraserAgent.dll"));

		if (!::PathFileExists(szLuaPath) || !::PathFileExists(szLuaAgent))
		{
			return FALSE;
		}

		m_szPath[0] = '\0';

		//获取UE路径，并把它加入环境变量
		std::wstring strInstallDir,strParam;
		WCHAR wszParam[MAX_PATH] = {0};
		AnsiStringToWideString(szInstallDir, strInstallDir);
		if (szParam != NULL)
		{
			AnsiStringToWideString(szParam, strParam);
			wcscpy_s(wszParam,MAX_PATH-1,strParam.c_str());
		}


#define MY_MAX_ENV_STRING_BUF_SIZE	32768
		LPCTSTR pszPathEnvComponent = NULL;
		LPCTSTR pszPathEnvName = _T("Path");

		pszPathEnvComponent = strInstallDir.c_str();
		if (::PathFileExists(pszPathEnvComponent) && ::PathIsDirectory(pszPathEnvComponent))
		{
			TCHAR *szPathEnvOldValue = new TCHAR[MY_MAX_ENV_STRING_BUF_SIZE];
			::ZeroMemory(szPathEnvOldValue, MY_MAX_ENV_STRING_BUF_SIZE * sizeof(TCHAR));
			::GetEnvironmentVariable(pszPathEnvName, szPathEnvOldValue, MY_MAX_ENV_STRING_BUF_SIZE);
			TSDEBUG(_T("ENV.%s = %s"), pszPathEnvName, szPathEnvOldValue);

			TCHAR *szPathEnvNewValue = new TCHAR[MY_MAX_ENV_STRING_BUF_SIZE];
			::ZeroMemory(szPathEnvNewValue, MY_MAX_ENV_STRING_BUF_SIZE * sizeof(TCHAR));
			_sntprintf(szPathEnvNewValue, MY_MAX_ENV_STRING_BUF_SIZE, _T("%s;%s"), pszPathEnvComponent, szPathEnvOldValue);

			TSDEBUG(_T("About to set ENV.%s to %s"), pszPathEnvName, szPathEnvNewValue);
			::SetEnvironmentVariable(pszPathEnvName, szPathEnvNewValue);

			delete [] szPathEnvNewValue;
			delete [] szPathEnvOldValue;
		}
		else
		{
			return FALSE;
		}

		//设置当前目录
		GetCurrentDirectory(MAX_PATH, m_szPath);		
		SetCurrentDirectory(strInstallDir.c_str());

		const TCHAR *rgszFileName[] = {
			_T("zlib1.dll"), 
			_T("minizip.dll"), 
			_T("xlfsio.dll"), 
			_T("xlluaruntime.dll")
		};
		HMODULE rghmodDeps[_countof(rgszFileName)] = {NULL};
		PreLoadDll(strInstallDir.c_str(), rgszFileName, _countof(rgszFileName), rghmodDeps);
		BOOL bAllOk = TRUE;
		for (int i = 0; i < _countof(rghmodDeps); ++i)
		{
			if (NULL == rghmodDeps[i])
			{
				return FALSE;
			}
		}

		for (int i = 0; i < _countof(rghmodDeps); ++i)
		{
			m_modList.push_back(rghmodDeps[i]);
		}
		typedef void (* PFN_RunLua)(wchar_t *,wchar_t *);
		HMODULE m_hLuaAgent = LoadLibrary(szLuaAgent);
		if (NULL != m_hLuaAgent)
		{
			PFN_RunLua pfnRunLua = (PFN_RunLua)::GetProcAddress(m_hLuaAgent, "RunLua");
			if (pfnRunLua)
			{
				pfnRunLua(szLuaPath,wszParam);
			}
		}
		return TRUE;
	}

	~CLRTAgent()
	{
		if('\0' != m_szPath[0])
		{
			SetCurrentDirectory(m_szPath);
		}
		if (m_hLuaAgent)
		{
			FreeLibrary(m_hLuaAgent);
			m_hLuaAgent = NULL;
		}
		for (std::list< HMODULE >::reverse_iterator rit = m_modList.rbegin();
			rit != m_modList.rend();
			++rit)
		{
			if (*rit)
			{
				TSDEBUG(_T("About to FreeLibrary(0x%p)"), *rit);
				::FreeLibrary(*rit);
			}
		}
	}

	static void PreLoadDll(const TCHAR *pszDir, const TCHAR **rgszFileName, ULONG nFileCount, HMODULE *pResult)
	{
		for (unsigned int i = 0; i < nFileCount; ++i)
		{
			TCHAR szFilePath[MAX_PATH] = {0};
			::PathCombine(szFilePath, pszDir, rgszFileName[i]);

			pResult[i] = ::LoadLibrary(szFilePath);
			DWORD dwLastError = ::GetLastError();
			TSDEBUG(_T("LoadLibrary(%s) return 0x%p, dwLastError = %lu"), szFilePath, pResult[i], dwLastError);
		}
	}

private:
	bool AnsiStringToWideString(const std::string strAnsi, std::wstring &strWide)
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

private:
	TCHAR	m_szPath[MAX_PATH];
	std::list< HMODULE > m_modList;
	HMODULE m_hLuaAgent;
};