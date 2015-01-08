#ifndef __STRINGUTIL_H__
#define __STRINGUTIL_H__

#include <string>
using std::string;
using std::basic_string;

#include <windows.h>


#if defined(_UNICODE) || defined(UNICODE)
#define UTF2T UTF2W
#define T2UTF W2UTF
#define Hex HexW
#else
#define UTF2T UTF2A
#define T2UTF A2UTF
#define Hex HexA
#endif

class StringUtil
{
public:
	static int W2CP(string& dest, UINT cp, LPCWSTR src, int len = -1);
	static int CP2W(basic_string<wchar_t>& dest, UINT cp, LPCSTR src, int len = -1);
	static int UTF2A(LPSTR dest, LPCSTR src, int len = -1);
	static int UTF2W(LPWSTR dest, LPCSTR src, int len = -1);
	static int A2UTF(string& dest, LPCSTR src, int len = -1);
	static int W2UTF(string& dest, LPCWSTR src, int len = -1);

	static void HexA(unsigned char* pInput, UINT uiLen, char* szOutput);
	static void HexW(unsigned char* pInput, UINT uiLen, WCHAR* szOutput);

	// static void EscapeHtmlUtf(LPCWSTR szInput, UINT uiLen, LPWSTR szOutput);
};

#endif