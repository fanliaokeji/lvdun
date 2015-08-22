#pragma once
#include <ShlObj.h>
#include <string>
#include <vector>
#include <Windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

/******************************* 使用说明 ******************************
//inf参考：http://www.cnblogs.com/sbdx/archive/2006/12/14/whatisinffile.html?login=1
---------------------------------------------------------- 
* 注册表操作的常量定义： 
* 常量 根值 
* HKCR HKEY_CLASSES_ROOT. 
* HKCU HKEY_CURRENT_USER. 
* HKLM HKEY_LOCAL_MACHINE. 
* HKU HKEY_USERS. 

* 注册表操作的类型定义
* FLG_ADDREG_APPEND 在多字符串后添加字符 
* FLG_ADDREG_TYPE_SZ 字符类型 
* FLG_ADDREG_TYPE_MULTI_SZ 字符串类型 
* FLG_ADDREG_TYPE_EXPAND_SZ 扩展字符串类型 
* FLG_ADDREG_TYPE_BINARY 二进制值 
* FLG_ADDREG_TYPE_DWORD DWord值 
* FLG_ADDREG_TYPE_NONE NULL值

如果值类型不支持上述字符串，直接用下面的掩码来表示
http://blog.sina.com.cn/s/blog_493520900100ochi.html
FLG_ADDREG_BINVALUETYPE 0x00000001
FLG_ADDREG_NOCLOBBER 0x00000002
FLG_ADDREG_DELVAL 0x00000004
FLG_ADDREG_APPEND 0x00000008
FLG_ADDREG_TYPE_MASK 0xFFFF0000 | FLG_ADDREG_BINVALUETYPE
FLG_ADDREG_TYPE_SZ 0x00000000
FLG_ADDREG_TYPE_MULTI_SZ 0x00010000
FLG_ADDREG_TYPE_EXPAND_SZ 0x00020000
FLG_ADDREG_TYPE_BINARY 0x00000000 | FLG_ADDREG_BINVALUETYPE
FLG_ADDREG_TYPE_DWORD 0x00010000 | FLG_ADDREG_BINVALUETYPE
FLG_ADDREG_TYPE_NONE 0x00020000 | FLG_ADDREG_BINVALUETYPE
----------------------------------------------------------- 

拷贝文件：由于无法定义源文件路径，所有文件操作都必须在inf所在目录
文件的目标路径 以dirid开头，dirid定义见http://msdn.microsoft.com/zh-cn/library/ff553598%28v=vs.85%29.aspx
*/

// 返回的错误码定义
#define ELEVATE_SUCCESS				0

#define ELEVATE_ENV_NOT_SUPPORT		-1
#define ELEVATE_EXE_NOT_FIND		-2
#define ELEVATE_WRITE_INF_ERROR		-3
#define ELEVATE_GET_INF_PATH_ERROR  -4
#define ELEVATE_DISABLE_REDIRECTION_ERROR  -5

typedef BOOL (WINAPI  * pWow64DisableWow64FsRedirection)(PVOID*); 
typedef BOOL (WINAPI  * pWow64RevertWow64FsRedirection)(PVOID);

class UACElevate
{
public:
	int Init(const TCHAR *szInfSavePath,BOOL bWow64 = FALSE)
	{
		m_bWow64 = bWow64;
		m_strInfSavePath = szInfSavePath;
		m_strAddReg = _T("");
		m_strDelReg = _T("");
		m_strDestDir = _T("");
		m_dwFileList = 0;
		if (!CheckEnv())
		{
			return (DWORD)ELEVATE_ENV_NOT_SUPPORT;
		}
		if (!GetInfExePath())
		{
			return ELEVATE_EXE_NOT_FIND;
		}
		return (DWORD)ELEVATE_SUCCESS;

	}
	//示例：AddReg(L"HKLM,SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellIconOverlayIdentifiers\\AAADesktopTips,\"\",FLG_ADDREG_TYPE_SZ,{4562B511-62E9-4533-B7B2-56A8BB10B482}");
	void AddReg(const TCHAR *szAddItem)
	{
		if (m_strAddReg.empty())
		{
			m_strAddReg = _T("[Add.Reg]\r\n");
			m_strInstall +=_T("AddReg=Add.Reg\r\n");
		}
		m_strAddReg+=szAddItem;
		m_strAddReg+=_T("\r\n");
	}
	void DelReg(const TCHAR *szDelItem)
	{
		if (m_strDelReg.empty())
		{
			m_strDelReg = _T("[Del.Reg]\r\n");
			m_strInstall +=_T("DelReg=Del.Reg\r\n");
		}
		m_strDelReg+=szDelItem;
		m_strDelReg+=_T("\r\n");
	}
	//szOperation:支持CopyFiles,Delfiles,Renfiles
	//vFileList:拷贝的文件列表
	//szDestDir:拷贝路径
	void FileOperation(const TCHAR *szOperation,std::vector<TCHAR *>&vFileList,const TCHAR *szDestDir)
	{
		m_dwFileList++;
		TCHAR szSectionName[MAX_PATH] = {0};
		_sntprintf_s(szSectionName,MAX_PATH,_T("[%s.FileList%d]\r\n"),szOperation,m_dwFileList);
		
		tstring strList = szSectionName;
		for (std::vector<TCHAR *>::const_iterator c_iter = vFileList.begin();c_iter!=vFileList.end();c_iter++)
		{
			strList+=*c_iter;
			strList+=_T("\r\n");
		}
		m_strFileListSection += strList;
		m_strFileListSection += _T("\r\n");
		TCHAR szDestDirItem[MAX_PATH] = {0};
		_sntprintf_s(szDestDirItem,MAX_PATH,_T("%s.FileList%d=%s\r\n"),szOperation,m_dwFileList,szDestDir);
		if (m_dwFileList == 1)
		{
			m_strDestDir += _T("[DestinationDirs]\r\n");
		}
		m_strDestDir += szDestDirItem;
		
		TCHAR szInstallItem[MAX_PATH] = {0};
		_sntprintf_s(szInstallItem,MAX_PATH,_T("%s=%s.FileList%d\r\n"),szOperation,szOperation,m_dwFileList);
		m_strInstall +=szInstallItem;

	}
	int DoWork()
	{
		if (m_strInstall.empty())
		{
			return ELEVATE_WRITE_INF_ERROR;
		}
		tstring strFileData = _T("[Version]\r\nSignature=$Chicago$\r\n\r\n");
		if (!m_strAddReg.empty())
		{
			strFileData += m_strAddReg;
			strFileData += _T("\r\n");
		}
		if (!m_strDelReg.empty())
		{
			strFileData += m_strDelReg;
			strFileData += _T("\r\n");
		}
		if (!m_strDestDir.empty())
		{
			strFileData += m_strDestDir;
			strFileData += _T("\r\n");
		}
		if (!m_strFileListSection.empty())
		{
			strFileData += m_strFileListSection;
			strFileData += _T("\r\n");
		}
		strFileData +=_T("[DefaultInstall]\r\n");
		strFileData +=m_strInstall;
		if (!WriteStringToFile(strFileData.c_str()))
		{
			return ELEVATE_WRITE_INF_ERROR;
		}
		return Execute(m_strInfSavePath.c_str());
	}
	int Execute(const TCHAR * szInfPath)
	{
		if (NULL == szInfPath || !::PathFileExists(szInfPath))
		{
			return ELEVATE_GET_INF_PATH_ERROR;
		}
		PVOID OldValue;
		pWow64DisableWow64FsRedirection fnWow64DisableWow64FsRedirection = NULL;
		pWow64RevertWow64FsRedirection fnWow64RevertWow64FsRedirection  = NULL;
		if (m_bWow64)
		{
			HMODULE hMod  =  LoadLibrary(L"Kernel32.dll");
			if (NULL == hMod)
			{
				return 0;
			}
			fnWow64DisableWow64FsRedirection = (pWow64DisableWow64FsRedirection) GetProcAddress(hMod,"Wow64DisableWow64FsRedirection");

			fnWow64RevertWow64FsRedirection  = (pWow64RevertWow64FsRedirection) GetProcAddress(hMod,"Wow64RevertWow64FsRedirection");

			if(NULL == fnWow64DisableWow64FsRedirection || NULL == fnWow64RevertWow64FsRedirection)
			{
				return 0;
			}
			if (!fnWow64DisableWow64FsRedirection (&OldValue))
			{
				return ELEVATE_DISABLE_REDIRECTION_ERROR;
			}
		}
		if (!::PathFileExists(m_strInfExePath.c_str()))
		{
			return ELEVATE_EXE_NOT_FIND;
		}
		::ShellExecute(NULL,_T("open"),m_strInfExePath.c_str(),szInfPath,NULL,SW_HIDE);
		if (m_bWow64)
		{
			fnWow64RevertWow64FsRedirection (OldValue);
		}
		return ELEVATE_SUCCESS;
	}
private:
	BOOL CheckEnv()
	{
		if (IsUserAnAdmin())
		{
			return FALSE;
		}
		HKEY hVlaueKey;
		if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System"), 0, KEY_QUERY_VALUE, &hVlaueKey))
		{
			return FALSE;
		}
		DWORD dwUAClevel=0;
		ULONG nBytes;
		DWORD dwType;
		nBytes = sizeof(DWORD);
		if (ERROR_SUCCESS != ::RegQueryValueEx(hVlaueKey, _T("ConsentPromptBehaviorAdmin"), NULL, &dwType, reinterpret_cast<LPBYTE>(&dwUAClevel),&nBytes))
			return FALSE;
		if (dwType != REG_DWORD)
			return FALSE;
		if (5!=dwUAClevel && 0!=dwUAClevel)
		{
			return FALSE;
		}
		return TRUE;
	}

	BOOL GetInfExePath()
	{
		TCHAR szPath[MAX_PATH] = {0};
		if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_SYSTEM, 0))
		{
			::PathCombine(szPath,szPath,_T("InfDefaultInstall.exe"));
			m_strInfExePath = szPath;
			return TRUE;
		}
		return FALSE;
	}
	
	BOOL WriteStringToFile(const TCHAR * szFileData)
	{
		BOOL bRet = FALSE;
		HANDLE h = ::CreateFile(m_strInfSavePath.c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL );
		if ( h != INVALID_HANDLE_VALUE )
		{
			DWORD dwLen = _tcslen(szFileData)*sizeof(TCHAR);
			const byte* file_data_ptr = (byte*)szFileData;
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
			bRet = (dwByteTotalWrite==dwLen);
			if ( !bRet )
			{
				::DeleteFile(m_strInfSavePath.c_str());
			}
		}
		return bRet;
	}

#if defined(UNICODE) || defined(_UNICODE)
	typedef std::wstring    tstring;
#else
	typedef std::string     tstring;
#endif 
	tstring m_strAddReg,m_strDelReg,m_strDestDir,m_strFileListSection,m_strInstall;
	tstring m_strInfSavePath;
	tstring m_strInfExePath;
	BOOL m_bWow64;
	DWORD m_dwFileList;
};

/////////////////////////////////////////////////////////////////////////////////////////