#ifndef ULTRA_HTTP_H
#define ULTRA_HTTP_H

#include <Windows.h>
#include <string>
#include <WinInet.h>
#include "./file-io.h"

#pragma comment(lib, "WinInet.lib")

namespace ultra{

namespace HttpStatus
{

	enum 
	{
		kSuccess           = 0,
		kConnecting        = 1,
		kDownloading       = 2,
		kConnectFailure    = 3,
		kDownloadFailure   = 4,
		kCreateFileFailure = 5,
		kWriteFileFailure  = 6,
	};
}

struct IHttpCallback
{
	virtual void HttpHandle(int nStatus, DWORD dwDlSize, DWORD dwFileSize, const std::string& strContent) = 0;
};

class WininetHttp
{
public:

	WininetHttp(void) : m_bOpened(false), m_httpCallback(NULL)
	{

	}

	~WininetHttp(void)
	{
		CloseHandles();
	}

	static bool CanonicalizeUrl(const std::wstring& strSrc, std::wstring* strDest)
	{
		wchar_t ch;
		DWORD dwLen = 1;
		bool bRet = (0 != InternetCanonicalizeUrl(strSrc.c_str(), &ch, &dwLen, ICU_BROWSER_MODE));
		if (!bRet)
		{
			wchar_t* pBuf = new wchar_t[dwLen];
			bRet = (0 != InternetCanonicalizeUrl(strSrc.c_str(), pBuf, &dwLen, ICU_BROWSER_MODE));
			strDest->assign(pBuf, dwLen);
			delete[] pBuf;
			pBuf = NULL;
		}
		return bRet;
	}

	bool DownloadString(const std::wstring& wstrUrl, IHttpCallback* httpCallback, unsigned nTimeout = 0)
	{
		m_httpCallback = httpCallback;
		CallHttpHandle(HttpStatus::kConnecting, 0, 0, "");

		if (!OpenHandles(wstrUrl, nTimeout))
		{
			return false;
		}

		DWORD dwStatus = 0;
		QueryInfoNumber(HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus);
		if (dwStatus >= 400)
		{
			CallHttpHandle(HttpStatus::kConnectFailure, 0, 0, "");
			return false;
		}

		DWORD dwContentLen;
		if (!QueryInfoNumber(HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &dwContentLen))
		{
			dwContentLen = 0;
		}

		DWORD dwSize = 0;
		DWORD dwRead = 0;
		char* pBuf = NULL;
		std::string strBuf;
		DWORD dwDl;
		while (true)
		{
			if (0 == InternetQueryDataAvailable(m_hOpenUrl, &dwSize, 0, 0))
			{
				break;
			}
			if (dwSize <= 0)
			{
				break;
			}
			pBuf = new char[dwSize];
			if (InternetReadFile(m_hOpenUrl, pBuf, dwSize, &dwRead))
			{
				strBuf.append(pBuf, dwRead);
				dwDl = (DWORD)strBuf.length();
				CallHttpHandle(HttpStatus::kDownloading, dwDl, dwContentLen, "");
			}
			delete pBuf;
			pBuf = NULL;
		}

		if (strBuf.length() == dwContentLen)
		{
			CallHttpHandle(HttpStatus::kSuccess, dwDl, dwContentLen, strBuf);
		}
		else
		{
			CallHttpHandle(HttpStatus::kDownloadFailure, dwDl, dwContentLen, strBuf);
			return false;
		}
		return true;
	}

	bool DownloadFile(const std::wstring& wstrUrl, const std::wstring& wstrFilePath, IHttpCallback* httpCallback, unsigned nTimeout = 0)
	{
		m_httpCallback = httpCallback;
		CallHttpHandle(HttpStatus::kConnecting, 0, 0, "");
		if (!OpenHandles(wstrUrl, nTimeout))
		{
			return false;
		}

		DWORD dwStatus;
		QueryInfoNumber(HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus);
		if (dwStatus >= 400)
		{
			CallHttpHandle(HttpStatus::kConnectFailure, 0, 0, "");
			return false;
		}

		File file;
		if (!file.Create(wstrFilePath))
		{
			CallHttpHandle(HttpStatus::kCreateFileFailure, 0, 0, "");
			return false;
		}

		DWORD dwContentLen;
		if (!QueryInfoNumber(HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &dwContentLen))
		{
			dwContentLen = 0;
		}

		DWORD dwSize = 0;
		DWORD dwRead = 0;
		DWORD dwReadTotal = 0;
		DWORD dwWrite = 0;
		char* pBuf = NULL;
		bool bRet = true;
		while (true)
		{
			if (0 == InternetQueryDataAvailable(m_hOpenUrl, &dwSize, 0, 0))
			{
				break;
			}
			if (dwSize <= 0)
			{
				break;
			}
			pBuf = new char[dwSize];
			if (InternetReadFile(m_hOpenUrl, pBuf, dwSize, &dwRead))
			{
				if (!file.Write(pBuf, dwRead, &dwWrite))
				{
					CallHttpHandle(HttpStatus::kWriteFileFailure, (DWORD)file.GetSize(), dwContentLen, "");
					return false;
				}
				CallHttpHandle(HttpStatus::kDownloading, (DWORD)file.GetSize(), dwContentLen, "");
			}
			delete[] pBuf;
		}
		if (file.GetSize() == dwContentLen)
		{
			CallHttpHandle(HttpStatus::kSuccess, (DWORD)file.GetSize(), dwContentLen, "");
			file.Close();
		}
		else
		{
			CallHttpHandle(HttpStatus::kDownloadFailure, (DWORD)file.GetSize(), dwContentLen, "");
			return false;
		}
		return true;
	}


private:
	
	bool OpenHandles(const std::wstring& wstrUrl, unsigned nTimeout)
	{
		m_hOpen = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (m_hOpen != NULL)
		{
			std::wstring wstrCUrl;
			CanonicalizeUrl(wstrUrl, &wstrCUrl);
			if (nTimeout > 0)
			{
				InternetSetOption(m_hOpen, INTERNET_OPTION_CONNECT_TIMEOUT, (LPVOID)&nTimeout, sizeof(nTimeout));
			}
			m_hOpenUrl = InternetOpenUrl(m_hOpen, wstrCUrl.c_str(), NULL, 0,
				INTERNET_FLAG_NO_UI | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_CACHE_WRITE, 0);
		}
		if (m_hOpenUrl != NULL)
		{
			m_bOpened = true;
		}

		return m_bOpened;
	}

	void CloseHandles(void)
	{
		if (m_hOpen != NULL)
		{
			InternetCloseHandle(m_hOpen);
		}
		if (m_hOpenUrl != NULL)
		{
			InternetCloseHandle(m_hOpenUrl);
		}
	}

	bool QueryInfoNumber(DWORD dwFlags, DWORD* dwNum)
	{
		DWORD dwSize = sizeof(DWORD);
		bool bRet = (bool)HttpQueryInfo(m_hOpenUrl, dwFlags, dwNum, &dwSize, NULL);
		return bRet;
	}

	void CallHttpHandle(int nStatus, DWORD dwDlSize, DWORD dwFileSize, const std::string& strContent)
	{
		if (m_httpCallback != NULL)
		{
			m_httpCallback->HttpHandle(nStatus, dwDlSize, dwFileSize, strContent);
		}
	}

private:

	HINTERNET m_hOpen;
	HINTERNET m_hOpenUrl;
	bool m_bOpened;
	IHttpCallback* m_httpCallback;
};


class WininetDlList
{
public:
	WininetDlList(void)
	{

	}
	~WininetDlList(void)
	{

	}

};


}

#endif