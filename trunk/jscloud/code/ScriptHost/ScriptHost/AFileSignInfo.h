#pragma once

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
		lstrcpy(m_wszFilePath, pwszFilePath);
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
