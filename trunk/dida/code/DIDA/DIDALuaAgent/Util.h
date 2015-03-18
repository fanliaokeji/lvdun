#ifndef __UTIL_H__
#define __UTIL_H__

#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")

class HttpUtil
{
	static void CALLBACK OnStatusCallBack(HINTERNET hInternet, DWORD dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
	{
	}
public:
	static int GetContent(LPCTSTR szUserAgent, LPCTSTR szUrl, char* szOutput, size_t nCount, DWORD_PTR dwContext, INTERNET_STATUS_CALLBACK callback);
};

#endif