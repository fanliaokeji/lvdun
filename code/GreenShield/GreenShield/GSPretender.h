#pragma once

#include <shlobj.h>
#include <list>

class CGSPretender
{
public:
	CGSPretender() : m_bInitOK(FALSE), m_hGSPre(NULL)
	{


		const TCHAR *rgszFileName[] = {
			//_T("msvcr90.dll"), 
			_T("zlib1.dll"), 
			_T("minizip.dll"), 
			//_T("msvcp90.dll"), 
			_T("xlfsio.dll"), 
			_T("xlluaruntime.dll"), 
			//_T("atl90.dll"), 
			_T("libexpat.dll"), 
			_T("libpng13.dll"), 
			_T("xlgraphic.dll"), 
			_T("xlgraphicplus.dll"),
			_T("xlue.dll")
		};
		HMODULE rghmodDeps[_countof(rgszFileName)] = {NULL};
		PreLoadDll(rgszFileName, _countof(rgszFileName), rghmodDeps);
		BOOL bAllOk = TRUE;
		for (int i = 0; i < _countof(rghmodDeps); ++i)
		{
			if (NULL == rghmodDeps[i])
			{
				bAllOk = FALSE;
			}
		}
		if (!bAllOk) return;
		for (int i = 0; i < _countof(rghmodDeps); ++i)
		{
			m_modList.push_back(rghmodDeps[i]);
		}

		//加载GSPre.dll

		m_hGSPre = LoadLibrary(_T("GSPre.dll"));
		if (NULL == m_hGSPre)
		{
			return;
		}
		m_bInitOK = TRUE;
	}

	~CGSPretender()
	{
		if (m_hGSPre)
		{
			FreeLibrary(m_hGSPre);
			m_hGSPre = NULL;
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

	static void PreLoadDll(const TCHAR **rgszFileName, ULONG nFileCount, HMODULE *pResult)
	{
		for (unsigned int i = 0; i < nFileCount; ++i)
		{
			pResult[i] = ::LoadLibrary(rgszFileName[i]);
			DWORD dwLastError = ::GetLastError();
			TSDEBUG(_T("LoadLibrary(%s) return 0x%p, dwLastError = %lu"), rgszFileName[i], pResult[i], dwLastError);
		}
	}

	BOOL Init(LPTSTR lpCmdLine = NULL)
	{
		BOOL bRet = FALSE;
		if (m_bInitOK)
		{
			//1:先处理单例
			typedef HRESULT (STDAPICALLTYPE * PHandleSingleton)();
			PHandleSingleton pHandleSingleton = (PHandleSingleton)GetProcAddress(m_hGSPre,"HandleSingleton");
			if(NULL == pHandleSingleton)
			{
				MessageBox(NULL, L"无法找到 'HandleSingleton', 请重新安装", L"错误", MB_OK);
				TerminateProcess(GetCurrentProcess(), (UINT)-10);
				return -10;
			}
			HRESULT hr = pHandleSingleton();
			if(SUCCEEDED( hr))
			{
				//处理过了
				return bRet;		
			}

			typedef BOOL (STDAPICALLTYPE * PInitXLUE)(wchar_t*);
			PInitXLUE pInitXLUE = (PInitXLUE)GetProcAddress(m_hGSPre, "InitXLUE");
			if (pInitXLUE)
			{
				bRet = pInitXLUE(lpCmdLine);
			}
		}
		return bRet;
	}
private:
	
private:
	HMODULE m_hGSPre;
	BOOL	m_bInitOK;
	std::list< HMODULE > m_modList;
};