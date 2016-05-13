#ifndef ULTRA_STRING_OP_H
#define ULTRA_STRING_OP_H

#include <Windows.h>
#include <wchar.h>
#include <cwctype>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <functional>

namespace ultra {
	
inline std::wstring _A2T( const std::string& strOrigin )
{
	std::wstring wstrTarget;
	int nLen = MultiByteToWideChar(CP_ACP, 0, strOrigin.c_str(), (int)strOrigin.length()+1, NULL, NULL);
	
	wchar_t* pTarget = new wchar_t[nLen];
	ZeroMemory(pTarget, nLen*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, strOrigin.c_str(), (int)strOrigin.length()+1, pTarget, nLen);

	wstrTarget = pTarget;
	delete [] pTarget;
	pTarget = NULL;
	return wstrTarget;
}

inline std::string _T2A( const std::wstring& wstrOrigin )
{
	std::string strTarget;
	int nLen = WideCharToMultiByte(CP_ACP, 0, wstrOrigin.c_str(), (int)wstrOrigin.length()+1, NULL, NULL, NULL, NULL);

	char* pBuffer = new char[nLen];
	ZeroMemory(pBuffer, nLen*sizeof(char));
	WideCharToMultiByte(CP_ACP, 0, wstrOrigin.c_str(), (int)wstrOrigin.length()+1, pBuffer, nLen, NULL, NULL);

	strTarget = pBuffer;
	delete [] pBuffer;
	pBuffer = NULL;
	return strTarget;
}

inline std::string _T2UTF( const std::wstring& wstrOrigin )
{
	std::string strTarget;
	int nLen = WideCharToMultiByte(CP_UTF8, 0, wstrOrigin.c_str(), (int)wstrOrigin.length()+1, NULL, NULL, NULL, NULL);

	char* pBuffer = new char[nLen];
	ZeroMemory(pBuffer, nLen*sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, wstrOrigin.c_str(), (int)wstrOrigin.length()+1, pBuffer, nLen, NULL, NULL);
	
	strTarget = pBuffer;
	delete [] pBuffer;
	pBuffer = NULL;
	return strTarget;
}

inline std::wstring _UTF2T( const std::string& strOrigin )
{
	std::wstring wstrTarget;
	int nLen = MultiByteToWideChar(CP_UTF8, 0, strOrigin.c_str(), (int)strOrigin.length()+1, NULL, NULL);

	wchar_t* pTarget = new wchar_t[nLen];
	ZeroMemory(pTarget, nLen*sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, strOrigin.c_str(), (int)strOrigin.length()+1, pTarget, nLen);
	
	wstrTarget = pTarget;
	delete [] pTarget;
	pTarget = NULL;
	return wstrTarget;
}

inline std::string _UTF2A( const std::string& strOrigin )
{
	return _T2A(_UTF2T(strOrigin));
}

inline std::string _A2UTF( const std::string& strOrigin )
{
	return _T2UTF(_A2T(strOrigin));
}

inline std::string ToUpper( std::string str )
{
	std::transform(str.begin(), str.end(), str.begin(), toupper);
	return str;
}

inline std::string ToLower( std::string str )
{
	std::transform(str.begin(), str.end(), str.begin(), tolower);
	return str;
}

inline std::wstring ToUpper( std::wstring wstr )
{
	std::transform(wstr.begin(), wstr.end(), wstr.begin(), toupper);
	return wstr;
}

inline std::wstring ToLower( std::wstring wstr )
{
	std::transform(wstr.begin(), wstr.end(), wstr.begin(), tolower);
	return wstr;
}

inline int CompareWcharNoCase( wchar_t c1, wchar_t c2)
{
	c1 = std::towupper(c1);
	c2 = std::towupper(c2);

	if (c1 == c2)
	{
		return 0;
	}
	else
	{
		return c1 < c2 ? -1 : 1;
	}
}

inline int CompareStringNoCase( const std::wstring& wstrComp1, const std::wstring& wstrComp2 )
{
	return wcsicmp(wstrComp1.c_str(), wstrComp2.c_str());
}

inline int CompareStringNoCase( const std::string& strComp1, const std::string& strComp2 )
{
	return stricmp(strComp1.c_str(), strComp2.c_str());
}

inline std::string ReplaceString( std::string& str, const std::string& strSrc, const std::string& strDst)
{
	std::string::size_type nPos = 0;
	std::string::size_type nSlen = strSrc.size();
	std::string::size_type nDlen = strDst.size();

	while( (nPos = str.find(strSrc, nPos)) != std::string::npos )
	{
		str.replace( nPos, nSlen, strDst );
		nPos += nDlen;
	}

	return str;
}

inline std::wstring ReplaceString( std::wstring& str, const std::wstring& strSrc, const std::wstring& strDst )
{
	std::wstring::size_type nPos = 0;
	std::wstring::size_type nSlen = strSrc.size();
	std::wstring::size_type nDlen = strDst.size();

	while( (nPos = str.find(strSrc, nPos)) != std::wstring::npos )
	{
		str.replace( nPos, nSlen, strDst );
		nPos += nDlen;
	}

	return str;
} 

template <class T>
inline std::string ConvertToString(T value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

template <class T>
inline std::wstring ConvertToWString(T value)
{
	std::wstringstream ss;
	ss << value;
	return ss.str();
}

template <class T1, class T2>
inline T2 ConvertA(T1 value)
{
	std::stringstream ss;
	ss << value;
	T2 result;
	ss >> result;
	return result;
}

template <class T1, class T2>
inline T2 Convert(T1 value)
{
	std::wstringstream ss;
	ss << value;
	T2 result;
	ss >> result;
	return result;
}


inline bool SplitString( const std::wstring wstrSrc, const std::wstring wstrSep, std::vector< std::wstring >* vec )
{
	if (wstrSrc.empty())
	{
		return false;
	}
	vec->clear();
	if (wstrSep.empty())
	{
		vec->push_back(wstrSrc);
		return true;
	}

	size_t nPos;
	std::wstring wstrTmp(wstrSrc);
	std::wstring wstrItem;
	size_t nSepLen = wstrSep.length();
	while ((nPos = wstrTmp.find(wstrSep)) != std::wstring::npos)
	{
		wstrItem = wstrTmp.substr(0, nPos);
		if (!wstrItem.empty())
		{
			vec->push_back(wstrItem);
		}
		wstrTmp = wstrTmp.substr(nPos + nSepLen);
	}

	if (!wstrTmp.empty())
	{
		vec->push_back(wstrTmp);
	}
	return true;
}

inline bool SplitList( const std::wstring wstrSrc, const std::wstring wstrSep1, const std::wstring wstrSep2, std::vector< std::vector< std::wstring > >* vec )
{
	std::vector< std::wstring > tempVec, temp;
	if (wstrSrc.empty())
	{
		return false;
	}
	vec->clear();
	SplitString(wstrSrc, wstrSep1, &tempVec);
	for (size_t i = 0; i < tempVec.size(); i++)
	{
		SplitString(tempVec.at(i), wstrSep2, &temp);
		vec->push_back(temp);
	}
	return true;
}

// UrlEncode
typedef unsigned char BYTE;
inline BYTE toHex(const BYTE &x)
{
	return x > 9 ? x + 55: x + 48;
}

inline std::string URLEncodeImp(const std::string &sIn)
{
	std::string sOut;
	for( size_t ix = 0; ix < sIn.size(); ix++ )
	{
		BYTE buf[4];
		memset( buf, 0, 4 );
		if( isalnum( (BYTE)sIn[ix] ) )
		{
			buf[0] = sIn[ix];
		}
		else if ( isspace( (BYTE)sIn[ix] ) )
		{
			buf[0] = '+';
		}
		else
		{
			//if ((buf[0] != '/')&&(buf[0] != '?' )&&( buf[0] != '&')&&( buf[0] != '='))
			//{
			buf[0] = '%';
			buf[1] = toHex( (BYTE)sIn[ix] >> 4 );
			buf[2] = toHex( (BYTE)sIn[ix] % 16);
			//}
		}			
		sOut += (char *)buf;
	}
	return sOut;
};

inline std::string URLEncode(const std::string& strEncode)
{
	std::string sRetURL("");
	sRetURL = URLEncodeImp(strEncode);
	return sRetURL;
}

// fnmatch 模糊匹配
/* Bits set in the FLAGS argument to `fnmatch'. copy from fnmatch.h(linux) */
//#define    FNM_PATHNAME    (1 << 0) /* No wildcard can ever match `/'.  */
//#define    FNM_NOESCAPE    (1 << 1) /* Backslashes don't quote special chars.  */
//#define    FNM_PERIOD        (1 << 2) /* Leading `.' is matched only explicitly.  */
#define    FNM_NOMATCH        1
#define		fnmatch fnmatch_win

/**copy from Google-glog*/
inline bool SafeFNMatch(const char* pattern, size_t patt_len, const char* str, size_t str_len)
{
	size_t p = 0;
	size_t s = 0;
	while (1)
	{
		if (p == patt_len  &&  s == str_len)
			return true;
		if (p == patt_len)
			return false;
		if (s == str_len)
			return p + 1 == patt_len  &&  pattern[p] == '*';
		if (pattern[p] == str[s] || pattern[p] == '?')
		{
			p += 1;
			s += 1;
			continue;
		}
		if (pattern[p] == '*')
		{
			if (p + 1 == patt_len) return true;
			do
			{
				if (SafeFNMatch(pattern + (p + 1), patt_len - (p + 1), str + s, str_len - s))
				{
					return true;
				}
				s += 1;
			} while (s != str_len);

			return false;
		}

		return false;
	}
}

inline bool SafeFNMatch(const WCHAR* pattern, size_t patt_len, const WCHAR* str, size_t str_len)
{
	size_t p = 0;
	size_t s = 0;
	while (1)
	{
		if (p == patt_len  &&  s == str_len)
			return true;
		if (p == patt_len)
			return false;
		if (s == str_len)
			return p + 1 == patt_len  &&  pattern[p] == '*';
		if (pattern[p] == str[s] || pattern[p] == '?')
		{
			p += 1;
			s += 1;
			continue;
		}
		if (pattern[p] == '*')
		{
			if (p + 1 == patt_len) return true;
			do
			{
				if (SafeFNMatch(pattern + (p + 1), patt_len - (p + 1), str + s, str_len - s))
				{
					return true;
				}
				s += 1;
			} while (s != str_len);

			return false;
		}

		return false;
	}
}

inline bool FnMatch(std::string strPattern, std::string strOrigin)
{
	return SafeFNMatch(strPattern.c_str(), strPattern.length(), strOrigin.c_str(), strOrigin.length());
}

inline bool FnMatch(std::wstring strPattern, std::wstring strOrigin)
{
	return SafeFNMatch(strPattern.c_str(), strPattern.length(), strOrigin.c_str(), strOrigin.length());
}


/**********************************************************
*功能：去前空格
*
*str：源字符串
*
*反回值：去除前空格后的字符串
*
***********************************************************/
inline std::string Ltrim(std::string &str)
{
	str.erase(str.begin(), std::find_if(str.begin(), str.end(),
		std::not1(std::ptr_fun(::isspace))));
	return str;

}

/**********************************************************
*
*功能：去后空格
*
*str：源字符串
*
*反回值：去除后空格后的字符串
*
***********************************************************/
inline std::string & Rtrim(std::string &str)
{
	str.erase(std::find_if(str.rbegin(), str.rend(),
		std::not1(std::ptr_fun(::isspace))).base(),
		str.end());
	return str;

}

/**********************************************************
*
*功能：去前后空格
*
*str：源字符串
*
*反回值：去除前后空格后的字符串
*
***********************************************************/
inline std::string Trim(std::string &str)
{
	return Rtrim(Ltrim(str));
}

}
#endif