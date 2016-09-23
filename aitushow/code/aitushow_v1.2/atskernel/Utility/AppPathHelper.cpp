#include "StdAfx.h"
#include ".\apppathhelper.h"
#include <Shlobj.h>
#include "ultra/file-op.h"

EXTERN_C const GUID DECLSPEC_SELECTANY FOLDERID_LocalAppData = { 0xF1B32785, 0x6FBA, 0x4FCF, {0x9D, 0x55, 0x7B, 0x8E, 0x7F, 0x15, 0x70, 0x91}};

EXTERN_C const GUID DECLSPEC_SELECTANY FOLDERID_Public = { 0xDFDF76A2, 0xC82A, 0x4D63, {0x90, 0x6A, 0x56, 0x44, 0xAC, 0x45, 0x73, 0x85}};

static void GetKKCfgPath(wstring &strCfgDir)
{
	TSAUTO();
	WCHAR szDir[MAX_PATH] = {0};
	HMODULE hModule = ::LoadLibrary(L"shell32.dll");
	PFNSHGetKnownFolderPath SHGetKnownFolderPath = (PFNSHGetKnownFolderPath)GetProcAddress( hModule, "SHGetKnownFolderPath" );
	if ( SHGetKnownFolderPath)
	{
		PWSTR szPath = NULL;
		HRESULT hr = SHGetKnownFolderPath( FOLDERID_Public, 0, NULL, &szPath );
		if ( FAILED( hr ) )
		{
			TSERROR4CXX("Failed to get public folder");
			FreeLibrary(hModule);
			return;
		}
		wcscpy_s(szDir,MAX_PATH,szPath);
		::CoTaskMemFree(szPath);
		::PathAppend(szDir, L"kuaikantu");
		TSDEBUG4CXX("vista kuai kan cfg dir : "<<szDir);
		FreeLibrary(hModule);
	}
	else
	{
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, szDir);
		if ( FAILED( hr ) )
		{
			TSERROR4CXX("Failed to get main pusher dir");
			return;
		}
		::PathAppend(szDir, L"kuaikantu");
		TSDEBUG4CXX("xp kuai kan cfg dir : "<<szDir);
	}

	strCfgDir = szDir;
	return;
}

CKKPathHelper::CKKPathHelper(void)
{
	WCHAR szTempPathBuffer[MAX_PATH] = {0};
	GetTempPathW(MAX_PATH, szTempPathBuffer);
	m_strTempDir = szTempPathBuffer;

	WCHAR szAppDataPathBuffer[MAX_PATH] = {0};
	LPWSTR lpszAppDataPath = szTempPathBuffer;

	WCHAR szCommonAppDataPathBuffer[MAX_PATH] = {0};

	// AppDataDir
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_DEFAULT, szAppDataPathBuffer);
	m_strAppDataDir = szAppDataPathBuffer;
	// CommonAppDataDir
	SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_DEFAULT, szCommonAppDataPathBuffer);
	m_strCommonAppDataDir = szCommonAppDataPathBuffer;
	// LocalAppDataDir
	PFNSHGetKnownFolderPath SHGetKnownFolderPath = (PFNSHGetKnownFolderPath)GetProcAddress(GetModuleHandleW(L"shell32.dll"), "SHGetKnownFolderPath");
	if (SHGetKnownFolderPath == NULL) //SHGetKnownFolderPath is not supported
	{
		SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, szAppDataPathBuffer);
	}
	else
	{
		SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &lpszAppDataPath);
	}
	m_strExeDir = ultra::GetModuleFilePath();
	m_strAppDir = ultra::GetUpperPath(m_strExeDir);
	m_strXarDir = m_strAppDir + L"xar\\";
	//m_strCfgDir = m_strAppDir + L"Profiles\\";
	GetKKCfgPath(m_strCfgDir);
}

CKKPathHelper::~CKKPathHelper(void)
{
}