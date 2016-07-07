#pragma once

#include <shlobj.h>
#include <list>
#include "ImagePreReader.h"
class ATSPretender
{
public:
	ATSPretender() : m_bInitOK(FALSE), m_hAtsKernel(NULL)
	{


		//const TCHAR *rgszFileName[] = {
		//	_T("zlib1.dll"), 
		//	//_T("minizip.dll"),  
		//	_T("xlfsio.dll"), 
		//	_T("xlluaruntime.dll"), 
		//	_T("libexpat.dll"), 
		//	_T("libpng13.dll"), 
		//	_T("xlgraphic.dll"), 
		//	_T("xlgraphicplus.dll"),
		//	_T("xlue.dll")
		//};
		//HMODULE rghmodDeps[_countof(rgszFileName)] = {NULL};
		//PreLoadDll(rgszFileName, _countof(rgszFileName), rghmodDeps);
		//BOOL bAllOk = TRUE;
		//for (int i = 0; i < _countof(rghmodDeps); ++i)
		//{
		//	if (NULL == rghmodDeps[i])
		//	{
		//		bAllOk = FALSE;
		//	}
		//}
		//if (!bAllOk) return;
		//for (int i = 0; i < _countof(rghmodDeps); ++i)
		//{
		//	m_modList.push_back(rghmodDeps[i]);
		//}
		
		CHAR szExePath[MAX_PATH] = {0};
		GetModuleFileNameA(NULL, szExePath, MAX_PATH);
		PathRemoveFileSpecA(szExePath);

		const CHAR *dllFileName[] = {
			("zlib1.dll"),   
			("xlfsio.dll"), 
			("xlluaruntime.dll"), 
			("libexpat.dll"), 
			("libpng13.dll"), 
			("xlgraphic.dll"), 
			("xlgraphicplus.dll"),
			("FreeImage.dll"),
			("FreeImagePlus.dll"),
			("xlue.dll"),
			("atskernel.dll")
		};
		PreReadDll(dllFileName, _countof(dllFileName),szExePath);
		m_hAtsKernel = LoadLibrary(_T("atskernel.dll"));
		if (NULL == m_hAtsKernel)
		{
			return;
		}
		m_bInitOK = TRUE;
	}

	~ATSPretender()
	{
		if (m_hAtsKernel)
		{
			FreeLibrary(m_hAtsKernel);
			m_hAtsKernel = NULL;
		}

		for (std::list< HMODULE >::reverse_iterator rit = m_modList.rbegin();
			rit != m_modList.rend();
			++rit)
		{
			if (*rit)
			{
				//TSDEBUG(_T("About to FreeLibrary(0x%p)"), *rit);
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
			//TSDEBUG(_T("LoadLibrary(%s) return 0x%p, dwLastError = %lu"), rgszFileName[i], pResult[i], dwLastError);
		}
	}
	
	static void PreReadDll(const CHAR **rgszFileName, ULONG nFileCount, const CHAR * szDir)
	{
		for (unsigned int i = 0; i < nFileCount; ++i)
		{
			const size_t kStepSize = 1024 * 1024;
			CHAR szTmp[MAX_PATH] = {0}; 
			PathCombineA(szTmp,szDir,rgszFileName[i]);
			CImagePreReader::PreReadImage(szTmp, 0, kStepSize);
		}
	}

	BOOL Init(LPTSTR lpCmdLine = NULL)
	{
		BOOL bRet = FALSE;
		if (m_bInitOK)
		{
			//1:先处理单例
			typedef HRESULT (STDAPICALLTYPE * PHandleSingleton)();
			PHandleSingleton pHandleSingleton = (PHandleSingleton)GetProcAddress(m_hAtsKernel,"HandleSingleton");
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
			PInitXLUE pInitXLUE = (PInitXLUE)GetProcAddress(m_hAtsKernel, "InitXLUE");
			if (pInitXLUE)
			{
				bRet = pInitXLUE(lpCmdLine);
			}
		}
		return bRet;
	}
private:

private:
	HMODULE m_hAtsKernel;
	BOOL	m_bInitOK;
	std::list< HMODULE > m_modList;
};