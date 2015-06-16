#include "stdafx.h"
#include "Network.h"
#include "ultra\string-op.h"
#include "ultra\file-op.h"
#include "ultra\md5.h"
#include "wget\wgetExport.h"
#include "Service.h"


CNetwork::CNetwork()
{

}

CNetwork::~CNetwork()
{

}

bool CNetwork::HttpDownloadString( std::wstring wstrRequest, std::wstring& wstrReturn )
{
	char szUrl[1024] = {0};
	strcpy(szUrl, ultra::_T2A(wstrRequest).c_str());
	char szPath[MAX_PATH] = {0};
	ExpandEnvironmentStringsA(SERVICE_DIR, szPath, MAX_PATH);
	::PathAppendA(szPath, "dlstring");
	int nRet = DownLoad(szUrl, szPath);
	if (nRet == 0)
	{
		HANDLE hFile = CreateFileA(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD dwLen =0;
		dwLen = GetFileSize(hFile, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			char* szBuffer = new char[dwLen+1];
			ZeroMemory(szBuffer, dwLen+1);
			DWORD dwRead = 0;
			ReadFile(hFile, szBuffer, dwLen, &dwRead, NULL);
			wstrReturn = ultra::_A2T(szBuffer);
			CloseHandle(hFile);
			delete[] szBuffer;
			DeleteFileA(szPath);
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}
}

bool CNetwork::HttpDownloadString( std::wstring wstrRequest, std::string& strReturn )
{
	char szUrl[1024] = {0};
	strcpy(szUrl, ultra::_T2A(wstrRequest).c_str());
	char szPath[MAX_PATH] = {0};
	ExpandEnvironmentStringsA(SERVICE_DIR, szPath, MAX_PATH);
	::PathAppendA(szPath, "dlstring");
	int nRet = DownLoad(szUrl, szPath);
	if (nRet == 0)
	{
		HANDLE hFile = CreateFileA(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD dwLen =0;
		dwLen = GetFileSize(hFile, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			char* szBuffer = new char[dwLen+1];
			ZeroMemory(szBuffer, dwLen+1);
			DWORD dwRead = 0;
			ReadFile(hFile, szBuffer, dwLen, &dwRead, NULL);
			char szPrint[100] = {0};
			sprintf(szPrint, "dwRead:%d dwLen:%d", dwRead, dwLen);
			strReturn = szBuffer;
			CloseHandle(hFile);
			delete[] szBuffer;
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}
}

bool CNetwork::HttpDownloadFile( std::wstring wstrRequest, std::wstring wstrFilePath )
{
	std::wstring wstrFilePathTmp = wstrFilePath + L".tmp";
	MoveFile(wstrFilePath.c_str(), wstrFilePathTmp.c_str());
	char szUrl[1024] = {0};
	strcpy(szUrl, ultra::_T2A(wstrRequest).c_str());
	char szPath[MAX_PATH] = {0};
	strcpy(szPath, ultra::_T2A(wstrFilePath).c_str());
	int nRet = DownLoad(szUrl, szPath);
	if (nRet == 0)
	{
		DeleteFile(wstrFilePathTmp.c_str());
	}
	else
	{
		MoveFile(wstrFilePathTmp.c_str(), wstrFilePath.c_str());
	}
	
	return (nRet == 0);
}

bool CNetwork::Report( std::wstring wstrRequest )
{
	WCHAR szBuffer[MAX_PATH] = {0};
	GetPrivateProfileString(L"report", L"cid", L"0", szBuffer, MAX_PATH, (ultra::GetModuleFilePath()+L"cfg.ini").c_str());

	std::wstring* pwstrRequest = new std::wstring;
	*pwstrRequest = wstrRequest + L"&cid="+szBuffer;
	CloseHandle((HANDLE)_beginthreadex(NULL, 0, CNetwork::ReportProc, 
		pwstrRequest, 0, NULL));
	return 0;
}

bool CNetwork::HttpDownloadList( FileList& fileList, std::wstring wstrSDirPath, std::wstring wstrLDirPath, std::wstring wstrSeparator /*= L"|"*/ )
{
	std::vector<std::wstring> vecSDir;
	ultra::SplitString(wstrSDirPath, wstrSeparator, &vecSDir);
	for (int i = 0; i < fileList.size(); i++)
	{
		ultra::CreateDirectoryR(ultra::GetUpperPath(wstrLDirPath + fileList[i][0]));
		for (int j = 0; j < vecSDir.size(); j++)
		{
			if (HttpDownloadFile(vecSDir[j] + fileList[i][0], wstrLDirPath + fileList[i][0]))
			{
				if (ultra::CompareStringNoCase(ultra::MD5File(wstrLDirPath + fileList[i][0]), fileList[i][1])==0)
				{
					break;
				}
				else
				{
					if (j == vecSDir.size()-1)
					{
						return false;
					}
					else
					{
						continue;
					}
				}
			}
			else
			{
				if (j == vecSDir.size()-1)
				{
					return false;
				}
				else
				{
					continue;
				}
			}
		}
	}
	return true;
}
CNetwork* CNetwork::m_network = NULL;

CNetwork* CNetwork::Instance()
{
	if (m_network == NULL)
	{
		m_network = new CNetwork;

		WCHAR szPath[MAX_PATH] = {0};
		ExpandEnvironmentStrings((SERVICE_DIR_L), szPath, MAX_PATH);
		CreateDirectory(szPath, NULL);

		InitWget(NULL, 0);
		//InitWget("dl.log", 1);
		setval("timeout", "2");
		setval("tries", "2");
	}
	return m_network;
}

unsigned int _stdcall CNetwork::ReportProc( void* param )
{
	std::wstring* wstrRequest = (std::wstring*)param;
	std::wstring wstrReturn;
	CNetwork::Instance()->HttpDownloadString(*wstrRequest, wstrReturn);
	delete wstrRequest;
	return 0;
}

