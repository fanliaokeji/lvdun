#pragma once
#include <tchar.h>
#include <vector>
#include "map"
#include "string"
class CCmdlineParser
{
 
public:
	inline CCmdlineParser ();
	inline CCmdlineParser(LPCWSTR ptszCmdline);
	inline ~CCmdlineParser(void);

	inline BOOL Parser(LPCWSTR ptszCmdline);

	
	inline LPCWSTR GetCmdline(void);
	inline int GetNameItemCount(void);
	inline int GetKeyCount(void);
 	inline CComBSTR GetValueByNameItem(LPCWSTR ptszKey);
	inline CComBSTR GetParamValue(LPCWSTR ptszKey);
	
	//inline CComBSTR GetParamKey(int nItem);
	inline CComBSTR GetParamValue(int nItem);
	inline std::map<CComBSTR, CComBSTR>& CCmdlineParser::GetParamMap();
	inline std::vector<CComBSTR> & GetFixItemVec(void);//url个数
	static inline void AddSpecialItemName(CComBSTR vecKeys);
	static inline void SetSpecialItemNames(LPWSTR ppszKeys[]);
	static inline void SetSpecialItemNames(void);
private:	
	static inline void SetSpecialItemNames(std::map<CComBSTR, CComBSTR>& vecKeys);
	inline void ReleaseVector(void);
	inline BOOL _Parser(void);

private:
	std::map<CComBSTR, CComBSTR> m_mapCmdLine;
	std::vector<CComBSTR> m_vecFixCmdItems;
	static std::map<CComBSTR, CComBSTR> m_mapSpecialItemNames;//例外的命名项目 如 /s /key value /s为例外，因为没有value
	LPWSTR			m_pszCmdline ;
};
__declspec(selectany)  std::map<CComBSTR, CComBSTR> CCmdlineParser::m_mapSpecialItemNames;

inline CCmdlineParser::CCmdlineParser()
{
	TSAUTO();
	CCmdlineParser::SetSpecialItemNames();
	m_mapCmdLine.clear(); 
	m_vecFixCmdItems.clear();
	m_pszCmdline = NULL ;
	//LPCWSTR ptszCmdline = GetCommandLineW();
	//Parser(ptszCmdline);
}

inline CCmdlineParser::CCmdlineParser(LPCWSTR ptszCmdline)
{
	TSAUTO();
	CCmdlineParser::SetSpecialItemNames();

	m_mapCmdLine.clear();
	m_vecFixCmdItems.clear();
	m_pszCmdline = NULL ;

	//SetSpecialItemNames()
	//LPCWSTR ptszCmdline = GetCommandLineW();
	Parser(ptszCmdline ? ptszCmdline : GetCommandLineW());
}

inline CCmdlineParser::~CCmdlineParser(void)
{
	TSAUTO();
	ReleaseVector();
}

inline void CCmdlineParser::ReleaseVector()
{
	TSAUTO();
 	m_mapCmdLine.clear();
	m_vecFixCmdItems.clear();
	if(m_pszCmdline)
		delete m_pszCmdline;
}

inline BOOL CCmdlineParser::Parser(LPCWSTR ptszCmdline)
{
	TSAUTO();
	ReleaseVector();

	//if(!ptszCmdline || wcslen(ptszCmdline)<=0) return TRUE ;
	ptszCmdline = ptszCmdline ? ptszCmdline : GetCommandLineW();
	m_pszCmdline = new WCHAR[wcslen(ptszCmdline)+1] ;
	if(!m_pszCmdline) return FALSE;
	if(ptszCmdline)
		wcscpy(m_pszCmdline,ptszCmdline);
	else
		wcscpy(m_pszCmdline, GetCommandLineW());

	if(!_Parser())
	{
		ReleaseVector();
		return FALSE;
	}

	return TRUE ;
}

inline LPCWSTR CCmdlineParser::GetCmdline()
{
	TSAUTO();
	return m_pszCmdline;
}

inline int CCmdlineParser::GetNameItemCount()
{
	TSAUTO();
	return (ULONG)m_mapCmdLine.size();
}

inline int CCmdlineParser::GetKeyCount()
{
	TSAUTO();
	return GetNameItemCount();
}
 
 

inline BOOL CCmdlineParser::_Parser()
{
	TSAUTO();
	if(NULL == m_pszCmdline)
		return FALSE;
	
	CComBSTR bstrKey;
	CComBSTR bstrValue;
	int nNumArgs = 0;
	LPWSTR* lpwszArgv = CommandLineToArgvW(m_pszCmdline, &nNumArgs);	
	 
	for (int j = 0; j < nNumArgs; j++)
	{
		TSDEBUG4CXX(" param "<<j<<" : "<<lpwszArgv[j]<<" len : "<<(int)wcslen(lpwszArgv[j])) ;
	}

	int i =0;
	LPWSTR lpwszItem = lpwszArgv[0];
	if(lpwszItem)
	{
		wchar_t szModPath[_MAX_PATH] = {0};
		GetModuleFileNameW(NULL, szModPath, _MAX_PATH );
		wchar_t* pszFileName = PathFindFileNameW(szModPath);
		wchar_t* pszFileName2 = PathFindFileNameW(lpwszItem);

		if(0 == _wcsicmp(szModPath, lpwszItem) || 0 == _wcsicmp(pszFileName, lpwszItem) || 
			(pszFileName2 && 0 == _wcsicmp(pszFileName, pszFileName2))) //
			i = 1;
	}

	for (;i < nNumArgs; i++)
	{
		lpwszItem = lpwszArgv[i];
		if('/' == lpwszItem[0] || '-' == lpwszItem[0])
		{			
			bstrKey = &lpwszItem[1];
			bstrKey.ToLower();
 
			if(m_mapSpecialItemNames.find(bstrKey) != m_mapSpecialItemNames.end()) //特殊命令(没有子参数) 	
			{
				m_mapCmdLine[bstrKey]=" ";
				TSDEBUG4CXX(" Key : "<<bstrKey.m_str<<", Value : "<<m_mapCmdLine[bstrKey]);				
			}
			else			
			{
				if (i + 1 >= nNumArgs)
					m_mapCmdLine[bstrKey]=" ";
				else 
				{
					if( '-' != lpwszArgv[i + 1][0] && '/' != lpwszArgv[i + 1][0])
					{
						m_mapCmdLine[bstrKey] = lpwszArgv[i + 1];
						i++;
					}
					else
					{
						m_mapCmdLine[bstrKey]=" ";
					}
				}				
				TSDEBUG4CXX(" Key : "<<bstrKey.m_str<<", Value : "<<m_mapCmdLine[bstrKey]);
			}
		}
		else
		{
			TSDEBUG4CXX(" Fixed Item "<<i<<", FixItem : "<<lpwszItem<<" Length : "<<int(wcslen(lpwszItem)));
			bstrValue = lpwszItem;
			if(bstrValue.Length()  > 0) //去除多余空格
			{
				m_vecFixCmdItems.push_back(bstrValue);
			}
		}
	}
	 

	return TRUE ;
}
inline CComBSTR CCmdlineParser::GetValueByNameItem(LPCWSTR ptszKey)
{
	TSAUTO();
	if(!ptszKey) return FALSE ;
	CComBSTR bstrKey = ('/' == ptszKey[0] || '-' == ptszKey[0]) ? &ptszKey[1] : ptszKey;
	CComBSTR bstrValue;
	bstrKey.ToLower();
	if (m_mapCmdLine.find(bstrKey) != m_mapCmdLine.end())
		bstrValue = m_mapCmdLine[bstrKey];
	else
		bstrValue = L"";
	TSDEBUG4CXX(" value : "<<bstrValue);
	return bstrValue;
}
inline CComBSTR CCmdlineParser::GetParamValue(LPCWSTR ptszKey)
{
	TSAUTO();
	return GetValueByNameItem(ptszKey);

}
//inline CComBSTR CCmdlineParser::GetParamKey(int nItem)
//{
//	TSAUTO();
//	if(nItem > (int)m_mapCmdLine.size() - 1)
//		return L"";
//
//	CComBSTR bstrKey;
//	std::map<CComBSTR, CComBSTR>::iterator iter = m_mapCmdLine.begin(); 
//	for (int i = 0; i < nItem; i++) 
//		iter++; 
//	bstrKey = (CComBSTR)iter->first;
//	TSDEBUG4CXX(" Key : "<<bstrKey);
//	return bstrKey;
// 
//}
inline CComBSTR CCmdlineParser::GetParamValue(int nItem)
{
	TSAUTO();
	if(nItem > (int)m_mapCmdLine.size() - 1)
		return L"";

	CComBSTR bstrValue;
	std::map<CComBSTR, CComBSTR>::iterator iter = m_mapCmdLine.begin(); 
	for (int i = 0; i < nItem; i++) 
		iter++; 
	bstrValue = (CComBSTR)iter->second;
	TSDEBUG4CXX(" Value : "<<bstrValue);
	return bstrValue;
}
inline std::map<CComBSTR, CComBSTR>& CCmdlineParser::GetParamMap()
{
	return m_mapCmdLine;
}
inline std::vector<CComBSTR> &  CCmdlineParser::GetFixItemVec()
{ 
	 	
	return m_vecFixCmdItems;
}
inline void CCmdlineParser::AddSpecialItemName(CComBSTR vecKeys)
{
	m_mapSpecialItemNames[vecKeys] = L"";
}
inline void CCmdlineParser::SetSpecialItemNames(std::map<CComBSTR,CComBSTR> & mapKeys)
{
	m_mapSpecialItemNames = mapKeys;
}
inline void CCmdlineParser::SetSpecialItemNames(LPWSTR ppszKeys[])
{
	int nLen = sizeof(ppszKeys)/sizeof(ppszKeys[0]);
	for (int i = 0 ; i < nLen; i++)
	{
		AddSpecialItemName(ppszKeys[i]);
	}
}
inline void CCmdlineParser::SetSpecialItemNames()
{

	LPWSTR ppszKeys[] = { L"embedding",L"s",L"silent",L"u",L"update"};
	
	CCmdlineParser::SetSpecialItemNames(ppszKeys);
}
