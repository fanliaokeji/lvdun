#include "stdafx.h"
#include "Regedit.h"
#include "xlog.h"


BOOL CRegedit::Read (HKEY hRootKey, LPCTSTR lpszKeyName, LPCTSTR lpszValueName, std::wstring& strVal)
{

	HKEY hKey = NULL;	
	DWORD dwRet = ::RegOpenKeyEx(hRootKey, lpszKeyName, 0, KEY_READ, &hKey);
	if (dwRet != ERROR_SUCCESS)
	{
		xlogL(L"CRegedit::Read error,path = %s , %lu",lpszKeyName,GetLastError());
		return false;
	}
	WCHAR szPath[MAX_PATH] = {0};
	DWORD dwKeyType = REG_SZ;
	DWORD dwLen = MAX_PATH;
	dwRet = ::RegQueryValueEx(hKey, lpszValueName, 0, &dwKeyType, (BYTE*)szPath, &dwLen);
	if (dwRet != ERROR_SUCCESS)
	{
		xlogL(L"CRegedit::Read error,path = %s , %lu",lpszValueName,GetLastError());
		return false;
	}
	::RegCloseKey(hKey);
	strVal = szPath;
	return true;
}

BOOL CRegedit::Write( HKEY hRootKey, LPCTSTR lpszKeyName, LPCTSTR lpszValueName, LPCTSTR lpszValue )
{
	HKEY hKey = NULL;
	DWORD dwRet = ::RegOpenKeyEx(hRootKey, lpszKeyName, 0, KEY_WRITE, &hKey);
	if (dwRet != ERROR_SUCCESS)
	{
		DWORD dwDisposition = REG_CREATED_NEW_KEY;
		bool b = ::RegCreateKeyEx(hRootKey, lpszKeyName, 0, NULL, REG_OPTION_NON_VOLATILE,
			KEY_WRITE|KEY_READ, NULL, &hKey, &dwDisposition);
		if (b == false)
		{
			return FALSE;
		}
		else
		{
			dwRet = ::RegOpenKeyEx(hRootKey, lpszKeyName, 0, KEY_WRITE, &hKey);
			if (dwRet != ERROR_SUCCESS)
			{
				return FALSE;
			}
		}
	}
	int len = wcslen(lpszValue);
	dwRet = ::RegSetValueEx(hKey, lpszValueName, 0, REG_SZ, (const BYTE*)lpszValue, wcslen(lpszValue)*2);
	if (dwRet != ERROR_SUCCESS)
	{
		return FALSE;
	}
	::RegCloseKey(hKey);
	return TRUE;
}
