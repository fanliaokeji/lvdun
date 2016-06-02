#ifndef ULTRA_FILE_OP_H
#define ULTRA_FILE_OP_H

#include <Windows.h>
#include <string>
#include <string.h>
#include <wchar.h>
#include <shlobj.h>
#include <Shlwapi.h>
#include <io.h>

#include <ShellAPI.h>
#pragma comment(lib, "Shlwapi.lib")

#include <TlHelp32.h>

// GetMAC
#include <iphlpapi.h>
#include "string-op.h"

namespace ultra {

inline std::string PathAppend(const std::string& strPath, const std::string& strMore)
{
	char szPath[MAX_PATH] = {0};
	strcpy(szPath, strPath.c_str());
	BOOL bRet = ::PathAppendA(szPath, strMore.c_str());
	if (bRet == FALSE)
	{
		return strPath;
	}
	return szPath;
}

inline std::wstring PathAppend(const std::wstring& wstrPath, const std::wstring& wstrMore)
{
	wchar_t szPath[MAX_PATH] = {0};
	wcscpy(szPath, wstrPath.c_str());
	BOOL bRet = ::PathAppendW(szPath, wstrMore.c_str());
	if (bRet == FALSE)
	{
		return wstrPath;
	}
	return szPath;
}

inline unsigned __int64 GetDriveFreeSpace(const std::wstring& wstrDrive)
{
	ULARGE_INTEGER freespace;
	::GetDiskFreeSpaceEx(wstrDrive.c_str(), &freespace, NULL, NULL);
	return freespace.QuadPart;
}

inline void GetMaxFreeSpaceDrive(std::wstring* wstrDrive, unsigned __int64* nFreeSize) 
{
		DWORD dwBufLen = ::GetLogicalDriveStrings(0, NULL);
		wchar_t* pBuf = new wchar_t [dwBufLen];

		ULONGLONG nMaxFree = 0;

		if (0 != ::GetLogicalDriveStrings(dwBufLen, pBuf)) 
		{
			wchar_t* pDriveTmp = pBuf;
			DWORD i = 0;
			while (i <= dwBufLen) 
			{
				if (DRIVE_FIXED == ::GetDriveType(pDriveTmp)) 
				{
					ULONGLONG t = GetDriveFreeSpace(pDriveTmp);
					if (t > nMaxFree) 
					{
						nMaxFree = t;
						wstrDrive->assign(pDriveTmp);
					}
				}
				i = static_cast<DWORD>(wcslen(pDriveTmp)) + 1;
				pDriveTmp += i;
			}
		}
		delete[] pBuf;
		*nFreeSize = nMaxFree;
}

inline std::string GetUpperPath(const std::string strPath)
{
	size_t nPos = strPath.rfind("\\");
	if (nPos == std::string::npos)
	{
		return strPath;
	}
	if (nPos == (strPath.length()-1))
	{
		return GetUpperPath(std::string(strPath.c_str(), strPath.length()-1));
	}
	return strPath.substr(0, nPos+1);
}

inline std::wstring GetUpperPath(const std::wstring wstrPath)
{
	size_t nPos = wstrPath.rfind(L"\\");
	if (nPos == std::wstring::npos)
	{
		return wstrPath;
	}
	if (nPos == (wstrPath.length()-1))
	{
		return GetUpperPath(std::wstring(wstrPath.c_str(), wstrPath.length()-1));
	}
	return wstrPath.substr(0, nPos+1);
}

inline std::string GetModuleFileNameA()
{
	char szBuffer[MAX_PATH] = {0};
	::GetModuleFileNameA(NULL, szBuffer, MAX_PATH);
	return szBuffer;
}

inline std::wstring GetModuleFileName()
{
	wchar_t szBuffer[MAX_PATH] = {0};
	::GetModuleFileName(NULL, szBuffer, MAX_PATH);
	return szBuffer;
}

inline std::string GetModuleFilePathA()
{
	return GetUpperPath(GetModuleFileNameA());
}

inline std::wstring GetModuleFilePath()
{
	return GetUpperPath(GetModuleFileName());
}

inline std::string GetModuleNameA()
{
	std::string strName = GetModuleFileNameA();
	return strName.substr(strName.find_last_of('\\')+1);
}

inline std::wstring GetModuleName()
{
	std::wstring wstrName = GetModuleFileName();
	return wstrName.substr(wstrName.find_last_of(L'\\')+1);
}

inline std::wstring ExpandEnvironment(std::wstring wstrPath)
{
	WCHAR szBuffer[MAX_PATH] = {0};
	WCHAR szPath[MAX_PATH] = {0};
	wcscpy(szPath, wstrPath.c_str());
	ExpandEnvironmentStringsW(szPath, szBuffer, MAX_PATH);
	return szBuffer;
}

inline bool DeleteFileAlways(const std::wstring& wstrFileName)
{
	::SetFileAttributes(wstrFileName.c_str(), 0);
	if (::DeleteFile(wstrFileName.c_str()) == 0)
	{
		return false;
	}
	return true;
}

inline bool RecursiveRemoveDirectory(const std::wstring& wstrDirectory) {
	//pFrom and pTo need double-null terminate
	wchar_t szTmp[MAX_PATH+1];
	wcscpy(szTmp, wstrDirectory.c_str());
	szTmp[wstrDirectory.length()] = L'\0';
	szTmp[wstrDirectory.length()+1] = L'\0';
	SHFILEOPSTRUCT fileop;
	fileop.hwnd = NULL;
	fileop.wFunc = FO_DELETE;
	fileop.pFrom = szTmp;
	fileop.pTo = NULL;
	fileop.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
	fileop.lpszProgressTitle = NULL;

	return (::SHFileOperation(&fileop) == 0);
}

inline bool CreateDirectoryR(const std::wstring& wstrDirectory)
{
	return SHCreateDirectoryEx(NULL, wstrDirectory.c_str(), NULL) == ERROR_SUCCESS;
}

inline bool DeleteDirectoryR(const std::wstring& wstrDirectory)
{
	bool bRet = true;
	if (PathFileExists(wstrDirectory.c_str()) == false)
	{
		return false;
	}
	std::wstring wstrDir = wstrDirectory;
	if (wstrDir[wstrDir.size()-1] != L'\\')
	{
		wstrDir += L'\\';
	}
	std::wstring wstrFiles = wstrDir + L"*.*";

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = ::FindFirstFile(wstrFiles.c_str(), &FindFileData);
	while (hFind != INVALID_HANDLE_VALUE)
	{
		if (wcsicmp(FindFileData.cFileName, L".") != 0 && wcsicmp(FindFileData.cFileName, L"..") != 0)
		{
			std::wstring wstrDirChild = wstrDir + FindFileData.cFileName;
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				bRet = DeleteDirectoryR(wstrDirChild);
				if (!bRet)
				{
					return false;
				}
			}
			else
			{
				SetFileAttributes(wstrDirChild.c_str(), FILE_ATTRIBUTE_NORMAL);
				if (::DeleteFile(wstrDirChild.c_str()))
				{
					bRet = true;
				}
				else
				{
					bRet = false;
				}
			}
		}
		if (!::FindNextFile(hFind, &FindFileData))
		{
			break;
		}
	}
	if (hFind != INVALID_HANDLE_VALUE)
	{
		::FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	}
	if (bRet)
	{
		if (::RemoveDirectory(wstrDir.c_str()))
		{
			return true;
		}
	}
	return false;
}

inline bool ReplaceSingleFile(const std::wstring& wstrSrcPath, const std::wstring& wstrDstPath, const std::wstring& wstrFileName, int nDict)
{
	if (nDict == 0)
	{
		std::wstring wstrSrcFile = wstrDstPath + wstrFileName;
		std::wstring wstrDstFile = wstrSrcPath + wstrFileName + L".old";
		BOOL b = PathFileExists(wstrSrcFile.c_str());
		if (PathFileExists(wstrSrcFile.c_str()) && !MoveFile(wstrSrcFile.c_str(), wstrDstFile.c_str()))
		{
			return false;
		}

		wstrSrcFile = wstrSrcPath + wstrFileName;
		wstrDstFile = wstrDstPath + wstrFileName;
		if (!PathFileExists(wstrDstPath.c_str()))
		{
			CreateDirectoryR(wstrDstPath);
		}
		if (!MoveFile(wstrSrcFile.c_str(), wstrDstFile.c_str()))
		{
			return false;
		}
		return true;
	}
	else if (nDict == 1)
	{
		std::wstring wstrSrcFile = wstrSrcPath + wstrFileName;
		std::wstring wstrDstFile = wstrDstPath + wstrFileName;
		wstrDstFile = wstrDstFile.substr(0, wstrDstFile.length() - 4);

		if (PathFileExists(wstrDstFile.c_str()) && !DeleteFile(wstrDstFile.c_str()))
		{
			return false;
		}
		if (!MoveFile(wstrSrcFile.c_str(), wstrDstFile.c_str()))
		{
			return false;
		}
		return true;
	}
}

inline bool ReplaceFiles(std::wstring wstrSrcPath, std::wstring wstrDstPath, int nDict)
{
	bool bRet = true;
	WIN32_FIND_DATA findData = {0};
	std::wstring wstrSearchPath = wstrSrcPath + L"*";
	HANDLE hSearch = FindFirstFile(wstrSearchPath.c_str(), &findData);
	if (hSearch == INVALID_HANDLE_VALUE)
	{
		FindClose(hSearch);
		return true;
	}
	BOOL bRes = TRUE;
	while (bRes)
	{
		if (nDict == 0)
		{
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (wcsicmp(findData.cFileName, L".") != 0 && wcsicmp(findData.cFileName, L"..") != 0)
				{
					std::wstring wstrNewSrcPath = wstrSrcPath + findData.cFileName + L"\\";
					std::wstring wstrNewDstPath = wstrDstPath + findData.cFileName + L"\\";
					bRet = ReplaceFiles(wstrNewSrcPath, wstrNewDstPath, nDict);
					if (!bRet)
					{
						break;
					}
				}
			}
			else
			{
				if (wcsstr(findData.cFileName, L".old") == NULL)
				{
					bRet = ReplaceSingleFile(wstrSrcPath, wstrDstPath, findData.cFileName, 0);
					if (!bRet)
					{
						break;
					}
				}
			}
		}
		else if (nDict == 1)
		{
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (wcsicmp(findData.cFileName, L".") != 0 && wcsicmp(findData.cFileName, L"..") != 0)
				{
					std::wstring wstrNewSrcPath = wstrSrcPath + findData.cFileName + L"\\";
					std::wstring wstrNewDstPath = wstrDstPath + findData.cFileName + L"\\";
					bRet = ReplaceFiles(wstrNewSrcPath, wstrDstPath, nDict);
					if (!bRet)
					{
						break;
					}
				}
			}
			else
			{
				if (wcsstr(findData.cFileName, L".old") != NULL)
				{
					bRet = ReplaceSingleFile(wstrSrcPath, wstrDstPath, findData.cFileName, nDict);
					if (!bRet)
					{
						break;
					}
				}
			}
		}
		bRes = FindNextFile(hSearch, &findData);
	}
	FindClose(hSearch);
	return bRet;
}

inline bool ReplaceFiles(std::wstring wstrSrcPath, std::wstring wstrDstPath)
{
	if (ReplaceFiles(wstrSrcPath, wstrDstPath, 0) == false)
	{
		ReplaceFiles(wstrSrcPath, wstrDstPath, 1);
		return false;
	}
	return true;
}

/*
* 是否Vista or + 的系统
* return : 0 - XP or other
*              1 - Vista
*              2 - 7
*              3 - 8 or higher
*/
inline int OSIsVista()
{
	DWORD dwVersion, dwMajorVersion,dwMinorVersion;
	dwVersion = GetVersion();
	dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	dwMinorVersion = (DWORD(HIBYTE(LOWORD(dwVersion))));
	if (dwMajorVersion == 6)
	{
		if(dwMinorVersion == 0) return 1;
		if(dwMinorVersion == 1) return 2; 
		return 3;
	}
	return 0;
}

inline bool RunApplication(std::wstring wstrAppName, std::wstring wstrCmdLine, std::wstring wstrWorkDir)
{
	bool bRet = false;
	HINSTANCE hInst = NULL;
	if (OSIsVista() > 0)
	{
		hInst = ShellExecute(NULL, L"runas", wstrAppName.c_str(), wstrCmdLine.c_str(), wstrWorkDir.c_str(), SW_SHOWNORMAL);
	}
	else
	{
		hInst = ShellExecute(NULL, L"open", wstrAppName.c_str(), wstrCmdLine.c_str(), wstrWorkDir.c_str(), SW_SHOWNORMAL);
	}
	if ((int)hInst > 32)
	{
		bRet = true;
	}
	return bRet;
}

inline BOOL CheckProcessExist(std::wstring wstrProcessName, BOOL bQuit)
{
	BOOL bRtn = FALSE;
	do{
		// find process
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		if(hSnapShot == NULL) {
			break;
		}

		PROCESSENTRY32 stProcessEntry = {0};
		stProcessEntry.dwSize = sizeof(PROCESSENTRY32);
		Process32First(hSnapShot,&stProcessEntry);
		do 
		{
			if(ultra::CompareStringNoCase(stProcessEntry.szExeFile, wstrProcessName.c_str()) == 0) {
				bRtn = TRUE;
				if (bQuit == TRUE)	// 强制结束
				{
					HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, stProcessEntry.th32ProcessID ); 
					TerminateProcess( hProcess, 0 );
				}
				break;
			}
		} while (Process32Next(hSnapShot,&stProcessEntry));
		CloseHandle(hSnapShot);
	}while(false);

	return bRtn;
}

typedef BOOL  (WINAPI * pfuncIsWow64Process)( HANDLE hProcess,PBOOL Wow64Process);

inline BOOL Is64OS() {
	BOOL is64=false;
	HANDLE h=GetCurrentProcess();

	BOOL isExcSuccess=false;//IsWow64Process(h,&is64);
	HMODULE hKernel32=GetModuleHandle(L"kernel32.dll");
	pfuncIsWow64Process funcIsWow64Process=(pfuncIsWow64Process)GetProcAddress(hKernel32,"IsWow64Process");
	if (funcIsWow64Process!=NULL)
	{
		isExcSuccess=funcIsWow64Process(h,(PBOOL)&is64);
	}
	return (is64 && isExcSuccess);
}

inline std::wstring GetSystemPath()
{
	WCHAR szDir[MAX_PATH] = {0};
	SHGetSpecialFolderPath(NULL, szDir, CSIDL_SYSTEM, 0);
	return std::wstring(szDir) + L"\\";
}

inline std::wstring GetSystemX86Path()
{
	WCHAR szDir[MAX_PATH] = {0};
	SHGetSpecialFolderPath(NULL, szDir, CSIDL_SYSTEMX86, 0);
	return std::wstring(szDir) + L"\\";
}

typedef DWORD (WINAPI* fnGetAdaptersInfo)(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen);
inline std::string GetMac(int nType = 1)
{
	char strMac[20] = {0};
	PIP_ADAPTER_INFO pAdapterInfo = NULL;  
	DWORD AdapterInfoSize;  
	BOOL bRet					= FALSE;
	HMODULE hIpMod				= LoadLibraryA("IPHLPAPI.DLL");
	fnGetAdaptersInfo	func	= (fnGetAdaptersInfo)GetProcAddress(hIpMod,"GetAdaptersInfo");
	TCHAR szMac[32]				= {0};  
	DWORD Err;    
	AdapterInfoSize				= 0;  
	Err   =   func(NULL,   &AdapterInfoSize);  
	if((Err   !=   0)   &&   (Err   !=   ERROR_BUFFER_OVERFLOW)){  
		goto _exit0;
	}  
	//   分配网卡信息内存  
	pAdapterInfo				= (PIP_ADAPTER_INFO)   GlobalAlloc(GPTR,   AdapterInfoSize);  
	if(pAdapterInfo   ==   NULL){  
		goto _exit0;  
	}    
	if(func(pAdapterInfo,   &AdapterInfoSize)   !=   0){  
		goto _exit0;
	}     
	if (nType == 1)
	{
		sprintf(strMac,"%02X-%02X-%02X-%02X-%02X-%02X",    
			pAdapterInfo->Address[0],  
			pAdapterInfo->Address[1],  
			pAdapterInfo->Address[2],  
			pAdapterInfo->Address[3],  
			pAdapterInfo->Address[4],  
			pAdapterInfo->Address[5]);  
	}
	else if (nType == 2)
	{
		sprintf(strMac,"%02X%02X%02X%02X%02X%02X",    
			pAdapterInfo->Address[0],  
			pAdapterInfo->Address[1],  
			pAdapterInfo->Address[2],  
			pAdapterInfo->Address[3],  
			pAdapterInfo->Address[4],  
			pAdapterInfo->Address[5]);  
	}
_exit0:
	if (pAdapterInfo)
	{
		GlobalFree(pAdapterInfo);  
	}	
	return strMac;
}

}
#endif