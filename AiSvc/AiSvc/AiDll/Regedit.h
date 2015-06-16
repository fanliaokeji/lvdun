#pragma once
#include <string>

class CRegedit
{
public:
	static BOOL Read (HKEY hRootKey, LPCTSTR lpszKeyName, LPCTSTR lpszValueName, std::wstring& strVal);
	static BOOL Write (HKEY hRootKey, LPCTSTR lpszKeyName, LPCTSTR lpszValueName, LPCTSTR lpszValue);
};