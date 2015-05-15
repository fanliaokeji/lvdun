// DatFileUtility.cpp: implementation of the CDatFileUtility class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "./DatFileUtility.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDatFileUtility::CDatFileUtility()
{

}

CDatFileUtility::~CDatFileUtility()
{

}

void CDatFileUtility::ReadFileToString(std::wstring strSrcFile, std::string &strFileData, DWORD &dwByteRead, DWORD dwMaxFileSize)
{
	HANDLE h = ::CreateFileW(strSrcFile.c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL );
	if ( h != INVALID_HANDLE_VALUE )
	{
		DWORD dwFileSize = ::GetFileSize(h,NULL);
		dwMaxFileSize = (dwMaxFileSize == 0 ? 1024*1024 : dwMaxFileSize);
		if ( dwFileSize>0 && dwFileSize <= dwMaxFileSize)
		{
			strFileData.resize(dwFileSize);
			if ( strFileData.size()==dwFileSize )
			{
				dwByteRead = 0;
				while ( dwByteRead<dwFileSize )
				{
					DWORD dwByteReadOnce = 0;
					BOOL bReadSucc = ::ReadFile(h,(LPVOID)(strFileData.c_str()+dwByteRead),
						dwFileSize-dwByteRead, &dwByteReadOnce, NULL);
					if ( !bReadSucc || 0==dwByteReadOnce )
					{
						strFileData.erase();
						break;
					}
					dwByteRead += dwByteReadOnce;
				}
			}
			else
			{
				strFileData.erase();
			}
		}
		::CloseHandle( h );
	}
}

bool CDatFileUtility::WriteStringToFile(std::wstring strDesFile, const char* pFileData, DWORD dwLen)
{
	bool ret = false;
	HANDLE h = ::CreateFileW(strDesFile.c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL );
	if ( h != INVALID_HANDLE_VALUE )
	{
		const char* file_data_ptr = pFileData;
		DWORD dwByteTotalWrite = 0;
		while ( dwByteTotalWrite<dwLen )
		{
			DWORD dwByteWriteOnce = 0;
			BOOL bWriteSucc = ::WriteFile(h,(LPVOID)(file_data_ptr+dwByteTotalWrite),
				dwLen-dwByteTotalWrite, &dwByteWriteOnce, NULL);
			if ( !bWriteSucc || 0==dwByteWriteOnce )
			{
				break;
			}
			dwByteTotalWrite += dwByteWriteOnce;
		}
		::CloseHandle(h);
		ret = (dwByteTotalWrite==dwLen);
		if ( !ret )
		{
			::DeleteFileW(strDesFile.c_str());
		}
	}
	return ret;
}
