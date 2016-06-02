#pragma once
inline bool  BSTRToLuaString( BSTR src, std::string& dest)
{
	if(!src)
		return false;
	int  iLen = (int)wcslen(src);
	if(iLen > 0)
	{
		char* szdest = new  char[iLen * 4];
		if(NULL == szdest)
			return false;
		ZeroMemory(szdest, iLen * 4);			
		int nLen = WideCharToMultiByte(CP_UTF8, NULL, src, iLen, szdest, iLen * 4, 0, 0);
		szdest[nLen] = '\0'; 
		dest = szdest;
		delete [] szdest ;
		return true;
	}
	return false;
}
inline bool LuaStringToCComBSTR(const char* src, CComBSTR& bstr)
{
	bstr = L"";
	if(!src)
		return false;
	int iLen = (int)strlen(src);
	if(iLen > 0)
	{
		wchar_t* szm = new wchar_t[iLen * 4];
		ZeroMemory(szm, iLen * 4);
		int nLen = MultiByteToWideChar(CP_UTF8, 0, src,iLen, szm, iLen*4); 
		szm [nLen] = '\0';
		bstr = szm;
		delete [] szm;
		return true;
	}
	return false;
}

inline BOOL UnicodeToMultiByte(const std::basic_string<WCHAR> &strSrc, std::string &strDst)
{
	INT nSrcUnicodeLen = 0;
	nSrcUnicodeLen = (INT)strSrc.length();
	if(0 == nSrcUnicodeLen)
		return TRUE;
	static INT nLen = 0; 
	nLen = WideCharToMultiByte(936, 0, strSrc.c_str(), -1,NULL, 0 ,NULL, NULL);
	CHAR szBuf[MAX_USERDATA_SIZE + 1] = {0};
	LPSTR pszBuf = szBuf;
	if(MAX_USERDATA_SIZE < nLen)
		nLen = MAX_USERDATA_SIZE;
	INT nResult = 0;
	nResult = WideCharToMultiByte(936, 0, strSrc.c_str(), -1, pszBuf, nLen,NULL,NULL);
	if( nResult <= 0)
		return FALSE;
	if(pszBuf)
		strDst = pszBuf;
	return TRUE;
}

inline bool WideStringToAnsiString(const std::wstring &strWide, std::string &strAnsi)
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

inline bool AnsiStringToWideString(const std::string &strAnsi, std::wstring &strWide)
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


static DWORD gPriorityArray[6] = {IDLE_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, HIGH_PRIORITY_CLASS, REALTIME_PRIORITY_CLASS};

inline DWORD GetPriorityFromFlag(DWORD dwFlag)
{
	if (dwFlag < 0 || dwFlag > 5)
	{
		dwFlag = 2;
	}
	return gPriorityArray[dwFlag];
}

inline int GetPriorityFlag(DWORD dwPriority)
{
	int iRet = -1;
	for (int i = 0; i < 6; i++)
	{
		if (gPriorityArray[i] == dwPriority)
		{
			iRet = i;
			break;
		}
	}
	return iRet;
}