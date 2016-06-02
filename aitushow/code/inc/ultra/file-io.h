#ifndef ULTRA_FILE_IO_H
#define ULTRA_FILE_IO_H

#include <Windows.h>
#include <string>

namespace ultra{

class File
{
public:

	File(void) : m_hFile(NULL)
	{

	}

	~File(void)
	{
		if (m_hFile != NULL)
		{
			Close();
		}
	}

	HANDLE GetHandle(void)
	{
		return m_hFile;
	}

	bool IsOpened(void)
	{
		return m_hFile != NULL;
	}

	bool Close(void)
	{
		if (m_hFile == NULL)
		{
			return true;
		}
		if (!::CloseHandle(m_hFile))
		{
			return false;
		}
		m_hFile = NULL;
		return true;
	}

	bool Open(const std::wstring& wstrFileName)
	{
		return Open(wstrFileName, OPEN_EXISTING); 
	}

	bool Open(const std::wstring& wstrFileName, DWORD dwCreation)
	{
		m_hFile = ::CreateFile(wstrFileName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, dwCreation, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_hFile == INVALID_HANDLE_VALUE)
		{
			return false;
		}
		return true;
	}

	bool Create(const std::wstring& wstrFileName, bool bCreateAlways = true)
	{
		DWORD dwCreation = bCreateAlways ? CREATE_ALWAYS : CREATE_NEW;
		return Open(wstrFileName, dwCreation);
	}

	bool Read(void* pBuffer, DWORD dwToRead, DWORD* dwReaded)
	{
		return ::ReadFile(m_hFile, pBuffer, dwToRead, dwReaded, NULL) != 0 ;
	}

	bool Write(const void* pBuffer, DWORD dwToWrite, DWORD* dwWritten)
	{
		return ::WriteFile(m_hFile, pBuffer, dwToWrite, dwWritten, NULL) != 0;
	}

	bool Write(std::string strBuffer)
	{
		DWORD dwWrite;
		return Write(strBuffer.c_str(), (DWORD)strBuffer.length(), &dwWrite);
	}

	unsigned __int64 GetSize(void)
	{
		DWORD dwHigh;
		DWORD dwLow = ::GetFileSize(m_hFile, &dwHigh);
		if (dwLow == INVALID_FILE_SIZE && GetLastError != NOERROR)
		{
			return 0;
		}
		return (((unsigned __int64)dwHigh) << 32) + dwLow;
	}

private:

	HANDLE m_hFile;

};

}

#endif