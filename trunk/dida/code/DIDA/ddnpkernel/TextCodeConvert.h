#pragma once
#include <Windows.h>
#include <wchar.h>
#include <cwctype>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>


namespace TextCodeConvert {

enum TextCode
{
	TC_UTF8,
	TC_UNICODE,
	TC_UNICODEBIGENDIAN,
	TC_ANSI,
	TC_UTF8NOBOM
};

//static bool isutf8(const std::string& str)
//{
//	int i = 0;
//	int size = str.length();
//
//	while(i < size)
//	{
//		int step = 0;
//		if((str[i] & 0x80) == 0x00)
//		{
//			step = 1;
//		}
//		else if((str[i] & 0xe0) == 0xc0)
//		{
//			if(i + 1 >= size) return false;
//			if((str[i + 1] & 0xc0) != 0x80) return false;
//
//			step = 2;
//		}
//		else if((str[i] & 0xf0) == 0xe0)
//		{
//			if(i + 2 >= size) return false;
//			if((str[i + 1] & 0xc0) != 0x80) return false;
//			if((str[i + 2] & 0xc0) != 0x80) return false;
//
//			step = 3;
//		}
//		else
//		{
//			return false;
//		}
//
//		i += step;
//	}
//
//	if(i == size) return true;
//
//	return false;
//}

inline std::wstring SwapHighAndLowByte(const std::wstring& strOrigin )
{
	std::wstring wstrTarget;
	int index = 0,nLen = strOrigin.size();
	wstrTarget.resize(nLen);
	while(index<nLen)
	{
		byte high, low;
		high = (strOrigin[index] & 0xFF00) >>8;
		low = strOrigin[index] & 0x00FF;
		wstrTarget[index] = ( low <<8) | high;
		++index;
	}
	return wstrTarget;
}

inline std::wstring _A2T( const std::string& strOrigin )
{
	std::wstring wstrTarget;
	int nLen = MultiByteToWideChar(CP_ACP, 0, strOrigin.c_str(), (int)strOrigin.length()+1, NULL, NULL);

	wchar_t* pTarget = new wchar_t[nLen];
	ZeroMemory(pTarget, nLen*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, strOrigin.c_str(), (int)strOrigin.length()+1, pTarget, nLen);

	wstrTarget = pTarget;
	delete pTarget;
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
	delete pBuffer;
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
	delete pBuffer;
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
	delete pTarget;
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

inline TextCode GetStringTextCodeType(const std::string& strInput)
{
	TextCode tc;
	UCHAR sz0 = (UCHAR)strInput[0];
	UCHAR sz1 = (UCHAR)strInput[1];
	UCHAR sz2 = (UCHAR)strInput[2];
	if(sz0==0xEF && sz1==0xBB && sz2==0xBF)
		tc=TC_UTF8;
	else if(sz0==0xFF && sz1==0xFE )
		tc=TC_UNICODE ;
	else if(sz0==0xFE && sz1==0xFF )
		tc=TC_UNICODEBIGENDIAN;
	else //跑到这里 还要判断一遍到底是不是无BOM的UTF8
	{
		//tc=ANSI;
		int nSize = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, strInput.c_str(), -1, NULL, 0);
		if (nSize==0 && GetLastError()==ERROR_NO_UNICODE_TRANSLATION) // 翻译出错，当作ANSI串
		{
			tc=TC_ANSI;
		}
		else
		{
			tc=TC_UTF8NOBOM;
		}
	}
	return tc;
}
	
//
inline void ConvertTextCodeToUTF8WithOutBOM(std::string& strInput,TextCode &tc)
{
	tc = GetStringTextCodeType(strInput);
	switch (tc)
	{
	case TC_UTF8 :
		  {
			  //strTarget = strInput;
			  break;
		  }
	case TC_UNICODE :
		  {
			  std::wstring strUniCode(reinterpret_cast<const wchar_t*> (strInput.c_str()),strInput.size()/2);
			  strInput = _T2UTF(strUniCode);
			  break;
		  }
	case TC_UNICODEBIGENDIAN :
		  {
			  std::wstring strUniCode(reinterpret_cast<const wchar_t*> (strInput.c_str()),strInput.size()/2);
			  strUniCode = SwapHighAndLowByte(strUniCode);
			  strInput = _T2UTF(strUniCode);	
			  break;
		  }
	case TC_ANSI :
		  {
			  strInput = _A2UTF(strInput);
			  break;
		  }
	case TC_UTF8NOBOM :
		  {
			  break;
		  }
	}
	return ;
}

inline void ConvertUTF8WithOutBOMByTextCode(std::string& strInput,TextCode tc)
{
	switch (tc)
	{
	case TC_UTF8 :
		  {
			  //strTarget = strInput;
			  break;
		  }
	case TC_UNICODE :
		  {
			  std::wstring strUniCode = _UTF2T(strInput);
			  strInput.clear();
			  strInput.resize(strUniCode.size()*2);
			  strInput.assign( reinterpret_cast<const char*> (strUniCode.c_str()),strUniCode.size()*2); 
			  break;
		  }
	case TC_UNICODEBIGENDIAN :
		  {
			  std::wstring strUniCode = _UTF2T(strInput);
			  strUniCode = SwapHighAndLowByte(strUniCode);
			  strInput.clear();
			  strInput.resize(strUniCode.size()*2);
			  strInput.assign( reinterpret_cast<const char*> (strUniCode.c_str()),strUniCode.size()*2);
			  break;
		  }
	case TC_ANSI :
		  {
			  strInput = _UTF2A(strInput);
			  break;
		  }
	case TC_UTF8NOBOM :
		  {
			  break;
		  }
	}
	return ;
}


}