#ifndef _REG_HELPER_H_
#define _REG_HELPER_H_

#include "time.h"		//_time(NULL)
#include "shlobj.h"		//SHGetFolderPath
#include <shlwapi.h>	//PathRemoveFileSpec

#include <vector>
#include <string>
#include <algorithm>
using namespace std;

#include <shfolder.h>		//CSIDL_PROGRAM_FILES_COMMON
#pragma comment(lib, "shfolder.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "version.lib")

#include <windows.h>
#include <Wintrust.h>
#include <softpub.h>
#include <atlstr.h>

#pragma comment (lib, "Wintrust.lib")
#pragma comment (lib, "Crypt32.lib")

class AFileSignInfo
{
	typedef struct CATALOG_INFO_
	{
		DWORD                       cbStruct;   // set to sizeof(CATALOG_INFO)
		WCHAR                       wszCatalogFile[MAX_PATH];
	} CATALOG_INFO;
	typedef HANDLE          HCATADMIN;
	typedef HANDLE          HCATINFO;
	typedef BOOL (WINAPI * PFNCryptCATAdminAcquireContext)(HCATADMIN* phCatAdmin, const GUID* pgSubsystem, DWORD dwFlags);
	typedef BOOL (WINAPI * PFNCryptCATAdminCalcHashFromFileHandle)(HANDLE hFile, DWORD* pcbHash, BYTE* pbHash, DWORD dwFlags);
	typedef BOOL (WINAPI * PFNCryptCATAdminReleaseContext)(HCATADMIN hCatAdmin, DWORD dwFlags);
	typedef BOOL (WINAPI * PFNCryptCATAdminReleaseCatalogContext)(HCATADMIN hCatAdmin, HCATINFO hCatInfo, DWORD dwFlags);
	typedef HCATINFO (WINAPI * PFNCryptCATAdminEnumCatalogFromHash)(HCATADMIN hCatAdmin, BYTE* pbHash, DWORD cbHash, DWORD dwFlags, HCATINFO* phPrevCatInfo);
	typedef BOOL (WINAPI * PFNCryptCATCatalogInfoFromContext)(HCATINFO hCatInfo, CATALOG_INFO* psCatInfo, DWORD dwFlags);

public:
	AFileSignInfo(PCWSTR pwszFilePath) : m_hModule(NULL)
	{
		ATLASSERT(pwszFilePath != NULL);
		lstrcpyW(m_wszFilePath, pwszFilePath);
	}
	~AFileSignInfo(void)
	{
		m_hModule = NULL;
	}

public:
	CStringW GetOrganizationName()
	{
		if (m_strOrganizationName.IsEmpty())
		{
			CheckTrust();
		}
		return m_strOrganizationName;
	}

	bool CheckTrust()
	{
		if (!LoadWintrust())
		{
			return false;
		}

		HCATADMIN hCatAdmin = NULL;
		if (!m_pfnCryptCATAdminAcquireContext(&hCatAdmin, NULL, 0))
		{
			return false;
		}

		HANDLE hFile = CreateFileW(m_wszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			m_pfnCryptCATAdminReleaseContext(hCatAdmin, 0);
			return false;
		}

		DWORD cbHash;
		if (!m_pfnCryptCATAdminCalcHashFromFileHandle(hFile, &cbHash, NULL, 0))
		{
			m_pfnCryptCATAdminReleaseContext(hCatAdmin, 0);
			CloseHandle(hFile);
			return false;
		}
		BYTE *pbHash = new BYTE[cbHash];
		m_pfnCryptCATAdminCalcHashFromFileHandle(hFile, &cbHash, pbHash, 0);
		CloseHandle(hFile);

		PWSTR pszMemberTag = new wchar_t[cbHash * 2 + 1];
		for (DWORD dwIndex = 0; dwIndex < cbHash; ++dwIndex)
		{
			wsprintfW(&pszMemberTag[dwIndex * 2], L"%02X", pbHash[dwIndex]);
		}

		HCATINFO hCatInfo = m_pfnCryptCATAdminEnumCatalogFromHash(hCatAdmin, pbHash, cbHash, 0, NULL);
		delete [] pbHash;

		WINTRUST_DATA wd = { 0 };
		WINTRUST_FILE_INFO wfi = { 0 };
		WINTRUST_CATALOG_INFO wci = { 0 };
		CATALOG_INFO ci = { 0 };
		if (NULL == hCatInfo)
		{
			wfi.cbStruct			= sizeof(WINTRUST_FILE_INFO);
			wfi.pcwszFilePath		= m_wszFilePath;
			wfi.hFile				= NULL;
			wfi.pgKnownSubject		= NULL;

			wd.cbStruct				= sizeof(WINTRUST_DATA);
			wd.dwUnionChoice		= WTD_CHOICE_FILE;
			wd.pFile				= &wfi;
			wd.dwUIChoice			= WTD_UI_NONE;
			wd.fdwRevocationChecks	= WTD_REVOKE_NONE;
			wd.dwStateAction		= WTD_STATEACTION_VERIFY;
			wd.dwProvFlags			= WTD_SAFER_FLAG;
			wd.hWVTStateData		= NULL;
			wd.pwszURLReference		= NULL;
		}
		else
		{
			m_pfnCryptCATCatalogInfoFromContext(hCatInfo, &ci, 0);
			wci.cbStruct				= sizeof(WINTRUST_CATALOG_INFO);
			wci.pcwszCatalogFilePath	= ci.wszCatalogFile;
			wci.pcwszMemberFilePath		= m_wszFilePath;
			wci.pcwszMemberTag			= pszMemberTag;

			wd.cbStruct				= sizeof(WINTRUST_DATA);
			wd.dwUnionChoice		= WTD_CHOICE_CATALOG;
			wd.pCatalog				= &wci;
			wd.dwUIChoice			= WTD_UI_NONE;
			wd.fdwRevocationChecks	= WTD_REVOKE_NONE;
			wd.fdwRevocationChecks	= WTD_STATEACTION_VERIFY;
			wd.dwProvFlags			= WTD_SAFER_FLAG;
			wd.hWVTStateData		= NULL;
			wd.pwszURLReference		= NULL;
		}

		GUID guidAction = WINTRUST_ACTION_GENERIC_VERIFY_V2;
		LONG lRet = WinVerifyTrust((HWND)INVALID_HANDLE_VALUE, &guidAction, &wd);

		if (lRet == 0)
		{
			CRYPT_PROVIDER_DATA *pcpd = WTHelperProvDataFromStateData(wd.hWVTStateData);
			if (pcpd != NULL)
			{
				CRYPT_PROVIDER_SGNR *pcps = WTHelperGetProvSignerFromChain(pcpd, 0, FALSE, 0);
				if (pcps != NULL)
				{
					//for (int i=0; ; ++i)
					{
						CRYPT_PROVIDER_CERT *pcpc = WTHelperGetProvCertFromChain(pcps, 0);
						if (pcpc != NULL)
						{
							DWORD dwLength = CertGetNameString(pcpc->pCert, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, NULL, NULL, 0);
							CertGetNameStringW(pcpc->pCert, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, NULL, m_strOrganizationName.GetBuffer(dwLength), dwLength);
							m_strOrganizationName.ReleaseBufferSetLength(dwLength-1);
						}
					}
				}
			}
		}

		//just to free memory.
		wd.dwStateAction = WTD_STATEACTION_CLOSE;
		wd.pFile = 0;
		wd.pCatalog = 0;
		wd.dwProvFlags = 0;
		GUID guidAction2 = WINTRUST_ACTION_GENERIC_CERT_VERIFY;
		WinVerifyTrust((HWND)INVALID_HANDLE_VALUE, &guidAction2, &wd);

		if (NULL != hCatInfo)
		{
			m_pfnCryptCATAdminReleaseCatalogContext(hCatAdmin, hCatInfo, 0);
		}
		m_pfnCryptCATAdminReleaseContext(hCatAdmin, 0);
		delete[] pszMemberTag;

		return (lRet == 0);
	}

private:
	bool LoadWintrust()
	{
		if (m_hModule != NULL)
		{
			return true;
		}

		m_hModule = ::GetModuleHandleW(L"wintrust.dll");
		if (m_hModule == NULL)
		{
			return false;
		}

		m_pfnCryptCATAdminAcquireContext = (PFNCryptCATAdminAcquireContext)GetProcAddress(m_hModule, "CryptCATAdminAcquireContext");
		m_pfnCryptCATAdminCalcHashFromFileHandle = (PFNCryptCATAdminCalcHashFromFileHandle)GetProcAddress(m_hModule, "CryptCATAdminCalcHashFromFileHandle");
		m_pfnCryptCATAdminReleaseContext = (PFNCryptCATAdminReleaseContext)GetProcAddress(m_hModule, "CryptCATAdminReleaseContext");
		m_pfnCryptCATAdminReleaseCatalogContext = (PFNCryptCATAdminReleaseCatalogContext)GetProcAddress(m_hModule, "CryptCATAdminReleaseCatalogContext");
		m_pfnCryptCATAdminEnumCatalogFromHash = (PFNCryptCATAdminEnumCatalogFromHash)GetProcAddress(m_hModule, "CryptCATAdminEnumCatalogFromHash");
		m_pfnCryptCATCatalogInfoFromContext = (PFNCryptCATCatalogInfoFromContext)GetProcAddress(m_hModule, "CryptCATCatalogInfoFromContext");

		if (	m_pfnCryptCATAdminAcquireContext == NULL
			||	m_pfnCryptCATAdminCalcHashFromFileHandle == NULL
			||	m_pfnCryptCATAdminReleaseContext == NULL
			||	m_pfnCryptCATAdminReleaseCatalogContext == NULL
			||	m_pfnCryptCATAdminEnumCatalogFromHash == NULL
			||	m_pfnCryptCATCatalogInfoFromContext == NULL
			)
		{
			m_hModule = NULL;
			return false;
		}

		return true;
	}

private:
	wchar_t m_wszFilePath[MAX_PATH];
	CStringW m_strOrganizationName;

	HMODULE m_hModule;
	PFNCryptCATAdminAcquireContext m_pfnCryptCATAdminAcquireContext;
	PFNCryptCATAdminCalcHashFromFileHandle m_pfnCryptCATAdminCalcHashFromFileHandle;
	PFNCryptCATAdminReleaseContext m_pfnCryptCATAdminReleaseContext;
	PFNCryptCATAdminReleaseCatalogContext m_pfnCryptCATAdminReleaseCatalogContext;
	PFNCryptCATAdminEnumCatalogFromHash m_pfnCryptCATAdminEnumCatalogFromHash;
	PFNCryptCATCatalogInfoFromContext m_pfnCryptCATCatalogInfoFromContext;
};

/****************************************************************************************/
/* RegisterEx注意事项：																	*/
/* 1.主要由bRename与bRegister两个参数控制，前者控制是否改名，后者控制是否注册;			*/
/* 2.hInst与srcFullPath不能同时为空，否则不能确定操作源;									*/
/* 3.hInst有值时，忽略srcFullPath，以通过hInst获取的模块绝对路径为准，因此做单纯的			*/
/*	 文件拷贝操作的话需将hInst置空;														*/
/* 4.dstPath为空默认拷贝到C:\Program Files\Common\Thunder Network\KanKan目录;			*/
/* 5.strNewFileName为空默认重命名为"模块名.大版本号.小版本号.Release号.Build号.扩展名",	*/
/*   此参数仅当bRename为TRUE时有效;														*/
/* 6.仅当bRegister为TRUE时，其后面参数才有效;											*/
/* 7.仅当bVerControl为TRUE时，其后面参数才有效，且strClsID不能为空，注意				*/
/*   strDirectRegFuncName不能为调用RegisterEx的调用方，否则会引起死循环;				*/
/* 8.启用了版本控制，安装历史文件为"目标目录+CLSID.history"，注意此函数的版本比较仅比较	*/
/*   文件版本的第一、二、四位，1.0.0.1与1.0.1.1将视为相同版本;							*/
/****************************************************************************************/
typedef std::basic_string<TCHAR> tstring;
HRESULT RegisterEx(const tstring& srcFullPath = _T(""),					//源绝对路径(路径+文件名)
				   const tstring& dstPath = _T(""),						//目标目录(仅含路径)
				   const BOOL bRename = FALSE,							//是否改名
				   const tstring& strNewFileName = _T(""),				//改名后的新文件名(不含路径)
				   tstring*	strDstFullPath = NULL,						//注册/拷贝的路径
				   const BOOL bRegister = FALSE,						//是否注册
				   const BOOL bVerControl = FALSE,						//是否需要版本控制
				   const tstring& strClsID = _T(""),					//模块CLSID
				   const tstring& strTypeLib = _T(""),					//模块类型库
				   const tstring& strHistoryVer = _T("HISTORY_VER_1"),	//版本历史文件版本号
				   const tstring& strDirectRegFuncName = _T("RegisterServerDirect")//改名注册时需要的直接注册函数名
				   );

/********************************************************************************/
/* UnregisterEx使用方法：(用到此函数，则必须是注册时使用了版本控制)				*/
/* 1.hInst为NULL则取得加载该DLL的进程的实例句柄，因此需要传入正确;				*/
/* 2.strDstPath、strClsID不得为空;												*/
/* 3.strDirectUnregFuncName不能为调用UnregisterEx的调用方，否则会引起死循环;	*/
/********************************************************************************/
HRESULT UnRegisterEx(const tstring& strDstPath,							//之前注册的目标路径(仅含路径)
					 const tstring& strClsID,								//模块CLSID
					 const tstring& strHistoryVer = _T("HISTORY_VER_1"),	//版本历史文件版本号
					 const tstring& strDirectUnregFuncName = _T("UnregisterServerDirect"));//卸载时需要的直接卸载函数名

tstring GetPath(const int csidl);	//CSIDL_PROGRAM_FILES_COMMON	C:\Program Files\Common
										//CSIDL_COMMON_APPDATA			All Users\Application Data

/************************************************************************************/
/* 改名、注册到C:\Program Files\Common的模板类										*/
/* 使用方法：																		*/
/* 1.继承该类;																		*/
/* 2.导出两个HRESULT (*FNREGISTERSERVERDIRECT)(void);的函数，可加入注册、卸载时的	*/
/*   特殊处理，最后需要调用模块原来注册/卸载的方法;									*/
/* 3.在派生类的构造函数中设置模块CLSID、版本历史文件版本号、上述导出函数名;			*/
/************************************************************************************/
template <class T>
class ATL_NO_VTABLE CRegHelper : public CAtlDllModuleT<T>
{
public:
	tstring m_strClsID;				//模块CLSID
	tstring m_strTypeLib;			//模块类型库
	tstring m_strHistoryVer;		//版本历史文件版本号
	tstring m_strDirectRegFuncName;	//改名注册时需要的直接注册函数名
	tstring m_strDirectUnregFuncName;//卸载时需要的直接卸载函数名

public:
	CRegHelper();
	virtual HRESULT DllRegisterServer(BOOL bRegTypeLib = TRUE);
	virtual HRESULT	DllUnregisterServer(BOOL bRegTypeLib = TRUE);
	virtual HRESULT RegisterServerDirect(BOOL bRegTypeLib = TRUE);
	virtual HRESULT UnregisterServerDirect(BOOL bRegTypeLib = TRUE);
};

/*
 *	End
 */

static vector<tstring>	s_vecInstallPaths;
BOOL IsFileDirectoryExists(TCHAR * pcszFileName, BOOL bDirectory = TRUE);
BOOL CreateDirectoryTree(TCHAR * pcszDirectory);
unsigned __int64 GetFileVersion(const TCHAR* file_path, unsigned __int64 * VerionTimeStamp = NULL);
static int VerCmp(unsigned __int64 ver1, unsigned __int64 ver2);
static void InstallHistory(const tstring& strIniFile, 
						   const tstring& strHistoryVer, 
						   BOOL bSaveData = FALSE, 
						   BOOL* bForceRegister = NULL);
vector<tstring> split(const tstring& str, const tstring& delims = _T("\t\n"), unsigned int maxSplits = 0);

typedef HRESULT (*FNREGISTERSERVERDIRECT)(void);
struct CheckSignInfo
{
	AFileSignInfo* pSignInfo;
	BOOL bResult;
};
DWORD WINAPI CheckSignThread(LPVOID pVoid);
struct RegisterInfo
{
	HRESULT hRet;
	tstring	strDstFilePath;
	tstring strDirectRegFuncName;
};
DWORD WINAPI RegisterThread(LPVOID pVoid);

HRESULT RegisterEx(const tstring& srcFullPath, 
				   const tstring& dstPath, 
				   const BOOL bRename, 
				   const tstring& strNewFileName, 
				   tstring*	strDstFullPath,
				   const BOOL bRegister,
				   const BOOL bVerControl, 
				   const tstring& strClsID,
				   const tstring& strTypeLib,
				   const tstring& strHistoryVer,
				   const tstring& strDirectRegFuncName)
{
	tstring strModulePath = _T("");
	tstring strModuleName = _T("");
	tstring strModuleExt  = _T("");
	tstring strDstPath	  = _T("");
	tstring strDstFilePath= _T("");

	MEMORY_BASIC_INFORMATION mbi;
	HINSTANCE hInst = NULL;
	VirtualQuery(RegisterEx, &mbi, sizeof(MEMORY_BASIC_INFORMATION)); // 获取函数地址内存信息
	hInst = (HINSTANCE)mbi.AllocationBase;

	//无法确定拷贝的源路径或当前路径
	if (srcFullPath == _T("") && NULL == hInst)
	{
		//LOG4C_DEBUG(_T("hInst=%#x, srcFullPath=%s"), hInst, srcFullPath.c_str());
		return E_FAIL;
	}

	//通过传入的源路径获取
	if (srcFullPath != _T(""))
	{
		if (!IsFileDirectoryExists((TCHAR*)srcFullPath.c_str(), FALSE))
		{
			//LOG4C_DEBUG(_T("IsFileDirectoryExists(srcFullPath=%s) is FALSE"), srcFullPath.c_str());
			return E_FAIL;
		}
		strModulePath = srcFullPath;
	}
	//获取模块加载路径
	else if (NULL != hInst)
	{
		TCHAR szModulePath[MAX_PATH] = {0};
		if (GetModuleFileName(hInst, szModulePath, sizeof(szModulePath)))
			strModulePath = szModulePath;
		else
		{
			//LOG4C_DEBUG(_T("GetModuleFileName(%#x) failed."), hInst);
			return E_FAIL;
		}
	}

	strModuleName = PathFindFileName(strModulePath.c_str());
	strModuleExt  = PathFindExtension(strModulePath.c_str());
	strModuleName = strModuleName.substr(0, strModuleName.length()-strModuleExt.length());
	
	//若目标路径不存在，则创建之
	if (dstPath == _T(""))
	{
		strDstPath = GetPath(CSIDL_PROGRAM_FILES_COMMON);
		if(strDstPath[strDstPath.length() - 1] != '\\')
			strDstPath += _T("\\");
		strDstPath += _T("Thunder Network\\KanKan\\");
		if (!CreateDirectoryTree((TCHAR*)strDstPath.c_str()))
		{
			//LOG4C_DEBUG(_T("CreateDirectoryTree(%s) failed."), strDstPath.c_str());
			return E_FAIL;
		}
	}
	else if (!IsFileDirectoryExists((TCHAR*)dstPath.c_str()))
	{
		strDstPath = dstPath;
		if (!CreateDirectoryTree((TCHAR*)strDstPath.c_str()))
		{
			//LOG4C_DEBUG(_T("CreateDirectoryTree(%s) failed."), strDstPath.c_str());
			return E_FAIL;
		}
		if(strDstPath[strDstPath.length() - 1] != '\\')
			strDstPath += _T("\\");
	}
	else
		strDstPath = dstPath;

	//不是改名拷贝
	if (FALSE == bRename)
	{
		strDstFilePath = strDstPath+strModuleName+strModuleExt;
	}
	//改名拷贝，定义了新文件名
	else if (FALSE != bRename && strNewFileName != _T(""))
	{
		strDstFilePath = strDstPath+strNewFileName;
	}
	//改名拷贝，采用默认新文件名
	else if (FALSE != bRename && strNewFileName == _T(""))
	{
		//获取文件版本信息，确定改名后的名称
		TCHAR szNOW[MAX_PATH] = {0};
		unsigned __int64 i64CurVer = 0, i64NewVer = 0, i64NewTimeStamp = 0;
		i64CurVer; i64NewVer; i64NewTimeStamp;
		i64NewVer = GetFileVersion(strModulePath.c_str(), &i64NewTimeStamp);
		srand( (unsigned)time( NULL ) );
		do 
		{
			if (_sntprintf(szNOW, sizeof(szNOW)/sizeof(TCHAR), _T("%s%s.%u.%u.%u.%u.(%u)%s"), strDstPath.c_str(), strModuleName.c_str(), 
				HIWORD(i64NewVer >> 32), LOWORD(i64NewVer >> 32), HIWORD(i64NewVer & 0xffffffff), LOWORD(i64NewVer & 0xffffffff), 
				rand()*1000/RAND_MAX, strModuleExt.c_str()) < 0)
				szNOW[sizeof(szNOW)/sizeof(TCHAR)-1] = _T('\0');
			//LOG4C_DEBUG( _T("New File Fullpath: %s"), szNOW );
		} while ( FALSE != PathFileExists(szNOW) );
		strDstFilePath = szNOW;
	}

	//若strDstFullPath不为空，则回传出去
	if (strDstFullPath != NULL)
		*strDstFullPath = strDstFilePath;

	//源与目标路径相同，则不做任何操作
	if (strModulePath == strDstFilePath)
		goto REGISTER_BEGIN;

	//LOG4C_DEBUG( _T("Copy from %s to %s"), strModulePath.c_str(), strDstFilePath.c_str());
	if (FALSE == CopyFile(strModulePath.c_str(), strDstFilePath.c_str(), FALSE))
	{
		//LOG4C_ERROR( _T("Copy Failed. GetLastError(%#X)"), GetLastError());
		return E_FAIL;
	}
	else
	{
		//LOG4C_DEBUG( _T("Copy Succeed."));
		goto REGISTER_BEGIN;
	}

REGISTER_BEGIN:
	//不注册，则直接返回了，无注册则无版本控制
	if (FALSE == bRegister)
		return S_OK;

	BOOL bPassVersionCheck = TRUE, bForceRegister = FALSE;
	HRESULT hRet = E_FAIL;
	if (FALSE != bVerControl)
	{
		if (strClsID == _T(""))
		{
			//LOG4C_DEBUG(_T("strClsID is empty"));
			return E_FAIL;
		}

		//根据安装历史判断能否注册
		//策略：
		//版本比历史新的可安装，若存在历史路径的更新之
		//LOG4C_DEBUG( _T("HistoryFilePath = %s"), (strDstPath+strClsID+_T(".history")).c_str() ); 
		InstallHistory(strDstPath+strClsID+_T(".history"), strHistoryVer, FALSE, &bForceRegister);
		vector<tstring>::iterator itor, itend;
		itor = s_vecInstallPaths.begin();
		itend = s_vecInstallPaths.end();
		unsigned __int64 i64CurVer = 0, i64NewVer = 0, i64NewTimeStamp = 0;
		i64CurVer;
		i64NewVer;
		i64NewTimeStamp;
		i64NewVer = GetFileVersion(strModulePath.c_str(), &i64NewTimeStamp);
		//LOG4C_DEBUG( _T("Try to Register Ver=%u.%u.%u.%u, TimeStamp=%I64u, %s"), 
		//	HIWORD(i64NewVer >> 32), LOWORD(i64NewVer >> 32), HIWORD(i64NewVer & 0xffffffff), LOWORD(i64NewVer & 0xffffffff), 
		//	i64NewTimeStamp, strModulePath.c_str());
		int i = 0;
		for (; itor != itend; itor++ )
		{
			unsigned __int64 i64HisTimeStamp = 0;
			unsigned __int64 i64HisVer = GetFileVersion((*itor).c_str(), &i64HisTimeStamp);
			UNREFERENCED_PARAMETER(i);
			//LOG4C_DEBUG( _T("InstallPaths[%d/%d] Ver=%u.%u.%u.%u, TimeStamp=%I64u, %s"), ++i, s_vecInstallPaths.size(), 
				//HIWORD(i64HisVer >> 32), LOWORD(i64HisVer >> 32), HIWORD(i64HisVer & 0xffffffff), LOWORD(i64HisVer & 0xffffffff), 
				//i64HisTimeStamp, (*itor).c_str());
			if (_tcsicmp((*itor).c_str(), strModulePath.c_str()) == 0)			//路径一样，更新版本，允许注册
			{
				//LOG4C_DEBUG(_T("Same Path, Allow to Pass Version Check"));
			}
			else
			{
				if ( VerCmp(i64NewVer, i64HisVer) < 0 || ( VerCmp(i64NewVer, i64HisVer) == 0 && i64NewTimeStamp < i64HisTimeStamp))
				{
					//LOG4C_DEBUG(_T("Version too old, can't be registered."));
					bPassVersionCheck = FALSE;
					break;
				}
			}
		}
		
		//看与现注册的版本谁更新
		TCHAR szRegPath[MAX_PATH] = _T("CLSID\\");
		_tcsncat(szRegPath, strClsID.c_str(), _tcslen(strClsID.c_str()));
		_tcsncat(szRegPath, _T("\\InprocServer32"), _tcslen(_T("\\InprocServer32")));
		CRegKey key;
		TCHAR szNowPath[MAX_PATH] = {0};
		ULONG len = sizeof(szNowPath);
		if(ERROR_SUCCESS == key.Open(HKEY_CLASSES_ROOT, szRegPath, KEY_READ)
			&& ERROR_SUCCESS == key.QueryStringValue(NULL, szNowPath, &len) && _tcslen(szNowPath) > 0)
		{
			if (IsFileDirectoryExists(szNowPath, FALSE))
			{
				unsigned __int64 i64NowTimeStamp = 0;
				unsigned __int64 i64NowVer = GetFileVersion(szNowPath, &i64NowTimeStamp);
				//LOG4C_DEBUG( _T("Register Table Ver=%u.%u.%u.%u, %s, TimeStamp=%I64u"), 
				//	HIWORD(i64NowVer >> 32), LOWORD(i64NowVer >> 32), HIWORD(i64NowVer & 0xffffffff), LOWORD(i64NowVer & 0xffffffff), 
				//	szNowPath, i64NowTimeStamp);
				if ( VerCmp(i64NewVer, i64NowVer) < 0 )
				{
					//LOG4C_DEBUG( _T("Older than Register, Keep Register Table Unchange.") );
					if (FALSE != bPassVersionCheck)
						bPassVersionCheck = FALSE;
				}
				else
				{
					//LOG4C_DEBUG( _T("Fresh than Exist") );
					bForceRegister = TRUE;
				}
			}
			else
			{
				//LOG4C_WARN( _T("%s didn't exist. Force to Register"), szNowPath);
				bForceRegister = TRUE;
			}
		}
		else
		{
			//LOG4C_WARN( _T("RegQueryValueEx %s or DapCtrl not installed, Force to Register."), szRegPath);
			bForceRegister = TRUE;
		}

		//如果注册表注册的签名无效或没有签名而需要注册的带签名，则可以无视版本号进行覆盖
		USES_CONVERSION;
		AFileSignInfo ChkCurExeSignInfo(T2W((TCHAR*)strModulePath.c_str()));
		AFileSignInfo ChkRegExeSignInfo(T2W(szNowPath));
		CheckSignInfo CurSignInfo, RegSignInfo;
		CurSignInfo.pSignInfo = &ChkCurExeSignInfo;
		CurSignInfo.bResult = FALSE;
		RegSignInfo.pSignInfo = &ChkRegExeSignInfo;
		RegSignInfo.bResult = FALSE;

		HRESULT hr = S_OK;
		hr;
		HANDLE hCurCheckSignThread = CreateThread(NULL, 0, CheckSignThread, (LPVOID)&CurSignInfo, 0, NULL);
		HANDLE hRegCheckSignThread = CreateThread(NULL, 0, CheckSignThread, (LPVOID)&RegSignInfo, 0, NULL);
		if ( NULL != hCurCheckSignThread && NULL != hRegCheckSignThread )
		{
			HANDLE hObjects[2];
			hObjects[0] = hCurCheckSignThread;
			hObjects[1] = hRegCheckSignThread;

			DWORD dwEvent;
			//LOG4C_INFO(_T("WaitForMultipleObjects(hCurCheckSignThread, hCurCheckSignThread, %d)"), 30000);
			dwEvent = ::WaitForMultipleObjects(2, hObjects, TRUE, 30000);
			if ( dwEvent == WAIT_TIMEOUT )
			{
				TerminateThread(hCurCheckSignThread, 0);
				TerminateThread(hRegCheckSignThread, 0);
				//LOG4C_ERROR(_T("TimeOut"));
			}
		}

		BOOL bSignCur = CurSignInfo.bResult;
		BOOL bSignReg = RegSignInfo.bResult;
		//LOG4C_DEBUG( _T("Will Register %s Signature is %s"), strModulePath.c_str(), (bSignCur?_T("valid"):_T("invalid")) );
		//LOG4C_DEBUG( _T("Regsister table %s Signature is %s"), szNowPath, (bSignReg?_T("valid"):_T("invalid")) );
		if (FALSE != bSignCur && FALSE == bSignReg)
		{
			//LOG4C_DEBUG( _T("%s's Signature is invalid and Source File with valid Signature, allow Force to Register"), szNowPath );
			bForceRegister = TRUE;
		}
	}

	vector<tstring>	vecOldVersion;
	if (bPassVersionCheck || bForceRegister)
	{
		WIN32_FIND_DATA wfd;
		HANDLE hFind = FindFirstFile((strDstPath+strModuleName+_T(".*")+strModuleExt).c_str(), &wfd);
		if (hFind == INVALID_HANDLE_VALUE) // 如果没有找到或查找失败
		{
			//LOG4C_WARN( _T("FindFirstFile(%sDapCtrl.*.dll) not Found"), strDstPath.c_str());
		}
		do
		{ 
			if (wfd.cFileName[0] == '.') 
				continue; // 过滤这两个目录 
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE && strDstFilePath != strDstPath+wfd.cFileName)
			{
				//LOG4C_DEBUG( _T("Will Delete Old Version(%s)"), (strDstPath+wfd.cFileName).c_str());
				vecOldVersion.push_back( strDstPath + wfd.cFileName );
			} 
		} while (FindNextFile(hFind, &wfd)); 
		FindClose(hFind); // 关闭查找句柄 
	}
	else if (FALSE == bPassVersionCheck)	//没通过版本检查，删除之前拷贝的文件
	{
		//LOG4C_WARN( _T("Can't pass Version check, delete it(%s)"), strDstFilePath.c_str());
		::DeleteFile(strDstFilePath.c_str());

		//不管注册是否成功，回写历史文件，增加一次引用
		vector<tstring>::iterator	itor;
		for ( itor = s_vecInstallPaths.begin(); itor != s_vecInstallPaths.end(); )
			if (strModulePath == (*itor))
				itor = s_vecInstallPaths.erase(itor);
			else
				itor ++;
		s_vecInstallPaths.push_back(strModulePath);
		InstallHistory(strDstPath+strClsID+_T(".history"), strHistoryVer, TRUE);
		return S_OK;
	}

	RegisterInfo RegInfo;
	RegInfo.hRet = E_FAIL;
	RegInfo.strDirectRegFuncName = strDirectRegFuncName;
	RegInfo.strDstFilePath =strDstFilePath;
	HANDLE hRegisterThread = CreateThread(NULL, 0, RegisterThread, (LPVOID)&RegInfo, 0, NULL);
	if ( NULL != hRegisterThread )
	{
		DWORD dwEvent;
		//LOG4C_INFO(_T("WaitForSingleObjects(hRegisterThread, %d)"), 30000);
		dwEvent = ::WaitForSingleObject(hRegisterThread, 30000);
		if ( dwEvent == WAIT_TIMEOUT )
		{
			TerminateThread(hRegisterThread, 0);
			//LOG4C_ERROR(_T("TimeOut"));
		}
	}
	hRet = RegInfo.hRet;

	// 确保注册成功了再删除旧的文件
	if (S_OK == hRet)
	{
		//校验类型库是否为新的dll所在地址
		if (strTypeLib != _T(""))
		{
			CRegKey key;
			TCHAR szTypeLib[MAX_PATH] = {0};
			DWORD nLen = MAX_PATH;
			tstring subkey = _T("TypeLib\\");
			subkey += strTypeLib;
			subkey += _T("\\1.0\\0\\win32");
			if (ERROR_SUCCESS == key.Open(HKEY_CLASSES_ROOT, subkey.c_str(), KEY_ALL_ACCESS) && ERROR_SUCCESS == key.QueryStringValue(NULL, szTypeLib, &nLen) && 
				_tcslen(szTypeLib) > 0)
			{
				if (::PathFileExists(szTypeLib) == FALSE || _tcsnicmp(szTypeLib, strDstFilePath.c_str(), strDstFilePath.length()) != 0)
				{
					key.SetStringValue(NULL, strDstFilePath.c_str());
				}
			}
		}

		vector<tstring>::iterator itor;
		for ( itor = vecOldVersion.begin(); itor != vecOldVersion.end(); itor++ )
		{
			//LOG4C_DEBUG( _T("Delete Old Version(%s)"), (*itor).c_str());
			::DeleteFile( (*itor).c_str() );
		}

	}

	if (FALSE != bVerControl)	//不管注册是否成功，回写历史文件，增加一次引用
	{
//		//LOG4C_DEBUG( _T("Insert %s"), strModulePath.c_str());
		vector<tstring>::iterator	itor;
		for ( itor = s_vecInstallPaths.begin(); itor != s_vecInstallPaths.end(); )
			if (strModulePath == (*itor))
				itor = s_vecInstallPaths.erase(itor);
			else
				itor ++;
		s_vecInstallPaths.push_back(strModulePath);
		InstallHistory(strDstPath+strClsID+_T(".history"), strHistoryVer, TRUE);
	}

	return hRet;
}

HRESULT UnRegisterEx(const tstring& strDstPath,
					 const tstring& strClsID,
					 const tstring& strHistoryVer,
					 const tstring& strDirectUnregFuncName)
{
	tstring strModulePath = _T("");

	MEMORY_BASIC_INFORMATION mbi;
	HINSTANCE hInst = NULL;
	VirtualQuery(UnRegisterEx, &mbi, sizeof(MEMORY_BASIC_INFORMATION)); // 获取函数地址内存信息
	hInst = (HINSTANCE)mbi.AllocationBase;

	if (NULL == hInst || strDstPath == _T("") || strClsID == _T(""))
		return E_FAIL;

	//获取模块加载路径
	TCHAR szModulePath[MAX_PATH] = {0};
	if (GetModuleFileName(hInst, szModulePath, sizeof(szModulePath)))
		strModulePath = szModulePath;
	else
		return E_FAIL;

	InstallHistory(strDstPath+strClsID+_T(".history"), strHistoryVer);
	//卸载时先把路径一样的干掉，保持最新的版本，如果是最后一个，则删掉注册信息
	unsigned __int64 i64Latest = 0;
	i64Latest;
	vector<tstring>::iterator itor;
	itor = s_vecInstallPaths.begin();
	while ( itor != s_vecInstallPaths.end() )
	{
		unsigned __int64 i64HisTimeStamp = 0;
		unsigned __int64 i64HisVer = GetFileVersion((*itor).c_str(), &i64HisTimeStamp);
		i64HisTimeStamp;
		i64HisVer;
		if (_tcsicmp((*itor).c_str(), strModulePath.c_str()) == 0)		//路径一样
		{
			//LOG4C_DEBUG( _T("Erase from InstallPaths %I64u = %s"), i64HisTimeStamp, strModulePath.c_str());
			itor = s_vecInstallPaths.erase( itor );
		}
		else
		{
			itor++;
		}
	}
	
	InstallHistory(strDstPath+strClsID+_T(".history"), strHistoryVer, TRUE);
	if (s_vecInstallPaths.size() <= 0)
	{
		HMODULE hDapReg = LoadLibrary(strModulePath.c_str());
		if (hDapReg == NULL)
		{
			//LOG4C_ERROR( _T("hDapReg == NULL and return E_FAIL"));
			return E_FAIL;
		}

		USES_CONVERSION;
		FNREGISTERSERVERDIRECT pFun = (FNREGISTERSERVERDIRECT) GetProcAddress(hDapReg, CT2A(strDirectUnregFuncName.c_str()));
		if (pFun == NULL)
		{
			//LOG4C_ERROR( _T("GetProcAddress(hDapReg, \"RegisterServerDirect\") FAIL"));
			FreeLibrary(hDapReg);
			return E_FAIL;
		}

		HRESULT hRet = (*pFun)();
		FreeLibrary(hDapReg);
		if (SUCCEEDED(hRet))
		{
			DeleteFile((strDstPath+strClsID+_T(".history")).c_str());
		}
		return hRet;
	}
	else
		return S_OK;
}

tstring GetPath(const int csidl)
{
	TCHAR szAppData[MAX_PATH] = {0};
	HRESULT hRet = SHGetFolderPath(NULL, csidl, NULL, 0, szAppData);
	if (csidl == CSIDL_PROGRAM_FILES_COMMON && (FAILED(hRet) || szAppData[0] == '\0' || szAppData[1] == '\0'))
	{
		CRegKey key;
		memset( szAppData, 0, sizeof(szAppData) );
		ULONG len = sizeof(szAppData);
		if(ERROR_SUCCESS == key.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion"), KEY_READ)
			&& ERROR_SUCCESS == key.QueryStringValue(_T("CommonFilesDir"), szAppData, &len) && _tcslen(szAppData) > 0)
		{
			key.Close();
			return tstring(szAppData);
		}

		TCHAR szPath[MAX_PATH] = {0};
		if (0 != GetSystemDirectory(szPath, MAX_PATH))
			szPath[_tcslen(_T("C:\\"))] = _T('\0');
		else
			_tcsncpy(szPath, _T("C:\\"), _tcslen(_T("C:\\")) );

		memset(szAppData, 0, MAX_PATH*sizeof(TCHAR));
		_tcsncpy(szAppData, szPath, _tcslen(szPath));
		_tcsncat(szAppData, _T("Program Files\\Common Files"), _tcslen(_T("Program Files\\Common Files")));

		if ( ERROR_SUCCESS == key.Create( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion") ) )
		{
			key.SetStringValue( _T("CommonFilesDir"), szAppData );
		}
	}
	return tstring(szAppData);
}

BOOL IsFileDirectoryExists(TCHAR * pcszFileName, BOOL bDirectory)
{
	DWORD dwAttributes = GetFileAttributes(pcszFileName);
	if(dwAttributes == 0xFFFFFFFF)
		return FALSE;
	
	if(bDirectory)
		return (dwAttributes & FILE_ATTRIBUTE_DIRECTORY);
	else
		return !(dwAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL CreateDirectoryTree(TCHAR * pcszDirectory)
{
	tstring::size_type pos = string::npos;
	tstring::size_type tpos = 0;
	tstring strDir = pcszDirectory;
	tstring tmp = strDir;
	if (strDir[_tcslen(pcszDirectory)-1] != '\\')
		strDir += _T("\\");
	while ((pos = tmp.find_first_of(_T("\\"))) != string::npos)
	{
		tmp = strDir.substr(0, pos+tpos);
		if (PathFileExists(tmp.c_str()) && !(GetFileAttributes(tmp.c_str()) & FILE_ATTRIBUTE_DIRECTORY))
		{
			//存在了这么个文件
			SetFileAttributes(tmp.c_str(), FILE_ATTRIBUTE_NORMAL);
			::DeleteFile(tmp.c_str());
		}
		if (!IsFileDirectoryExists((TCHAR*)tmp.c_str()))
		{
			if(!::CreateDirectory(tmp.c_str(), NULL))
				return FALSE;
		}
		tmp = strDir.substr(pos+tpos+1);
		tpos += pos+1;
	}
	return IsFileDirectoryExists(pcszDirectory);
}

unsigned __int64 GetFileVersion(const TCHAR* file_path, unsigned __int64 * VerionTimeStamp)
{
	if (FALSE == PathFileExists(file_path))
		return 0;
	unsigned __int64 i64Version = 0;
	VS_FIXEDFILEINFO *FileInfo = NULL;
	void *info = NULL;
//	GetModuleFileName(_AtlBaseModule.GetModuleInstance(), m_strModulePath.c_str(), MAX_PATH);
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
//						//LOG4C_DEBUG( _T("%s's version timestamp is %I64u"), file_path, *VerionTimeStamp);
					}
				}
//				//LOG4C_DEBUG( _T("%s's version %u.%u.%u.%u"), file_path, HIWORD(FileInfo->dwFileVersionMS), LOWORD(FileInfo->dwFileVersionMS),
//					HIWORD(FileInfo->dwFileVersionLS), LOWORD(FileInfo->dwFileVersionLS));
			}
			else
			{
				//LOG4C_WARN( _T("VerQueryValue(%s) 没有版本信息"), file_path);
			}
		}
		else
		{
			//LOG4C_WARN( _T("GetFileVersionInfo(%s) 没有版本信息"), file_path);
		}
		free(info);
	}   
	else     
	{   
		//LOG4C_WARN( _T("GetFileVersionInfoSize(%s) 没有版本信息"), file_path);
	}
	
	return i64Version;
}

static int VerCmp(unsigned __int64 ver1, unsigned __int64 ver2)
{
	int result = 0;
	unsigned __int64 i64Ver1 = (ver1 >> 32) << 32 | LOWORD(ver1 & 0xffffffff);
	unsigned __int64 i64Ver2 = (ver2 >> 32) << 32 | LOWORD(ver2 & 0xffffffff);
	if ( i64Ver1 > i64Ver2 )
		result = 1;
	else if ( i64Ver1 < i64Ver2 )
		result = -1;
	
	return result;
}

static void InstallHistory(const tstring& strIniFile, 
						   const tstring& strHistoryVer, 
						   BOOL bSave, 
						   BOOL* bForceRegister)
{
	if (bSave == FALSE)		//读取
	{
		//历史文件不存在
		if (!IsFileDirectoryExists((TCHAR*)strIniFile.c_str(), FALSE))
			return;

		s_vecInstallPaths.clear();
		USES_CONVERSION;
		long buf_size = 4096;
		FILE * fd = fopen(T2A((TCHAR*)strIniFile.c_str()), "r");
		if (NULL != fd)
		{
			fseek(fd, 0, SEEK_END);
			buf_size = ftell(fd);
			fclose(fd);
		}
		TCHAR * buf = new TCHAR[buf_size+1];
		ZeroMemory(buf, buf_size);
		GetPrivateProfileString(strHistoryVer.c_str(), _T("Paths"), _T(""), buf, buf_size, strIniFile.c_str());
		vector<tstring>	vecPaths, vecEqualPaths;
		vecPaths = split(buf, _T("|"));
		delete []buf;
		vector<tstring>::iterator	itor;
		for (itor = vecPaths.begin(); itor != vecPaths.end(); itor++ )
		{
			tstring tmp = *itor;
			transform(tmp.begin(), tmp.end(), tmp.begin(), tolower);
			vector<tstring>::iterator it = find(vecEqualPaths.begin(), vecEqualPaths.end(), tmp);
			if (PathFileExists((*itor).c_str()) && it == vecEqualPaths.end())		//该文件存在才读入列表
			{
				GetFileVersion((*itor).c_str(), NULL);
				s_vecInstallPaths.push_back((*itor).c_str());
				vecEqualPaths.push_back(tmp);
//				//LOG4C_DEBUG( _T("Load %s"), (*itor).c_str());
			}
		}

		if (NULL != bForceRegister)
			*bForceRegister = GetPrivateProfileInt(strHistoryVer.c_str(), _T("ForceRegister"), 0, strIniFile.c_str());
	}
	else	//存取
	{
		//清空history文件
		USES_CONVERSION;
		FILE * fd = fopen(CT2A(strIniFile.c_str()), "w");
		if (fd != NULL)
			fclose(fd);
		tstring	strPaths;
		vector<tstring>::iterator	itor, itend;
		itor = s_vecInstallPaths.begin();
		itend = s_vecInstallPaths.end();
		for (; itor != itend; itor++ )
		{
			if (PathFileExists((*itor).c_str()))
			{
//				//LOG4C_DEBUG( _T("Save %s"), (*itor).c_str());
				strPaths += (*itor);
				strPaths += _T("|");
			}
		}
		if (strPaths != _T(""))
		{
			//LOG4C_DEBUG( _T("Paths after Save %s"), strPaths.c_str());
			WritePrivateProfileString(strHistoryVer.c_str(), _T("Paths"), strPaths.c_str(), strIniFile.c_str());
		}
	}
}

vector<tstring> split( const tstring& str, const tstring& delims, unsigned int maxSplits) 
{ 
	vector<tstring> ret; 
	unsigned int numSplits = 0; 
	
	// Use STL methods 
	size_t start, pos; 
	start = 0; 
	do 
	{ 
		pos = str.find_first_of(delims, start); 
		if (pos == start) 
		{ 
			// Do nothing 
			start = pos + 1; 
		} 
		else if (pos == string::npos || (maxSplits && numSplits == maxSplits)) 
		{ 
			// Copy the rest of the string 
			ret.push_back( str.substr(start) ); 
			break; 
		} 
		else 
		{ 
			// Copy up to delimiter 
			ret.push_back( str.substr(start, pos - start) ); 
			start = pos + 1; 
		} 
		// parse up to next real data 
		start = str.find_first_not_of(delims, start); 
		++numSplits; 
		
	} while (pos != string::npos); 
	return ret; 
}

DWORD WINAPI CheckSignThread(LPVOID pVoid)
{
	//TSAUTO();
	CheckSignInfo* pSignInfo = (CheckSignInfo*)pVoid;
	pSignInfo->bResult = pSignInfo->pSignInfo->CheckTrust() && (pSignInfo->pSignInfo->GetOrganizationName() == _T("ShenZhen PiaoYi Network Technology Co.,Ltd.") || pSignInfo->pSignInfo->GetOrganizationName() == _T("ShenZhen Thunder Networking Technologies Ltd."));
	return 0;
}

DWORD WINAPI RegisterThread(LPVOID pVoid)
{
	//TSAUTO();
	RegisterInfo* pRegInfo = (RegisterInfo*)pVoid;
	HMODULE hDapReg = LoadLibrary(pRegInfo->strDstFilePath.c_str());
	////LOG4C_DEBUG( _T("After LoadLibrary(%s)"), pRegInfo->strDstFilePath.c_str() );
	if (hDapReg == NULL)
	{
		DWORD hr = GetLastError();
		UNREFERENCED_PARAMETER(hr);
		////LOG4C_ERROR( _T("hDapReg == NULL and return E_FAIL, %#X"), hr );
		pRegInfo->hRet = E_FAIL;
		return 0;
	}

	USES_CONVERSION;
	FNREGISTERSERVERDIRECT pFun = (FNREGISTERSERVERDIRECT) GetProcAddress(hDapReg, CT2A(pRegInfo->strDirectRegFuncName.c_str()));
	if (pFun == NULL)
	{
		////LOG4C_ERROR( _T("GetProcAddress(hDapReg, \"RegisterServerDirect\") FAIL"));
		FreeLibrary(hDapReg);
		pRegInfo->hRet = E_FAIL;
		return 0;
	}

	pRegInfo->hRet = (*pFun)();
	////LOG4C_DEBUG( _T("After RegisterServerDirect") );
	FreeLibrary(hDapReg);
	////LOG4C_DEBUG( _T("After FreeLibrary") );
	return 0;
}

template <class T>
CRegHelper<T>::CRegHelper()
{
	m_strClsID		= _T("");
//	m_strClsID		= "{ACACC6EB-1FBA-4E13-A729-53AEB2DF54F8}";
	m_strHistoryVer	= _T("HISTORY_VER_1");
//	m_strHistoryVer = "DAPCTRL_INSTALL_HISTORY_VER_2";
	m_strDirectRegFuncName = _T("RegisterServerDirect");
	m_strDirectUnregFuncName = _T("UnregisterServerDirect");
}

template <class T>
HRESULT CRegHelper<T>::DllRegisterServer(BOOL bRegTypeLib)
{
#if defined(_DEBUG) && !defined(DEBUG_REG)
	return CAtlDllModuleT<T>::DllRegisterServer(bRegTypeLib);
#else
	UNREFERENCED_PARAMETER(bRegTypeLib);
	if (m_strClsID.length() <= 0)
	{
		//LOG4C_DEBUG(_T("m_strClsID == empty"));
		return E_FAIL;
	}
	return RegisterEx(_T(""), _T(""), TRUE, _T(""), NULL, TRUE, TRUE, m_strClsID, m_strTypeLib, m_strHistoryVer, m_strDirectRegFuncName);

#endif
}

template <class T>
HRESULT CRegHelper<T>::DllUnregisterServer(BOOL bRegTypeLib)
{
	bRegTypeLib;
	tstring strDstPath = GetPath(CSIDL_PROGRAM_FILES_COMMON);
	if(strDstPath[strDstPath.length() - 1] != '\\')
		strDstPath += _T("\\");
	strDstPath += _T("Thunder Network\\KanKan\\");

	return UnRegisterEx(strDstPath, m_strClsID, m_strHistoryVer, m_strDirectUnregFuncName);
}

template <class T>
HRESULT CRegHelper<T>::RegisterServerDirect(BOOL bRegTypeLib)
{
	return CAtlDllModuleT<T>::DllRegisterServer(bRegTypeLib);
}

template <class T>
HRESULT CRegHelper<T>::UnregisterServerDirect(BOOL bRegTypeLib)
{
	return CAtlDllModuleT<T>::DllUnregisterServer(bRegTypeLib);
}
#endif
