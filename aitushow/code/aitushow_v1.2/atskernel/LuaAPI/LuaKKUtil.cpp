#include "StdAfx.h"

#include <objbase.h>      // For COM headers
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <shtypes.h> 

#include "LuaAppObject.h"
#include "resource.h"
#include "Shlwapi.h"
#include "FileRelation.h"
#include "ShellCmdManager.h"
#include "FolderChangeMonitor.h"
#include "NeoFolderDialog.h"
//#include "xl_peer_id/xl_peer_id_sim.h"

#include "ImageProcessor.h"

#define NEOVIEWER_LUAAPP_OBJNAME		"NeoViewer.LuaAppObject"
#define NEOVIEWER_LUAAPP_CLASSNAME		"NeoViewer.LuaAppObject.Class"

#define  CanSetToWallPaperFileExt	L".jpg;.jpe;.jpeg;.png;.bmp;"		// 可以设为壁纸的文件后缀名

CLuaAppObject::CLuaAppObject(void)
{
}

CLuaAppObject::~CLuaAppObject(void)
{
}

static XLLRTGlobalAPI LuaAppObjectMemberFunctions[] =
{
	{"Lua_Gc", CLuaAppObject::LuaGc},					// 垃圾回收是会调用该函数
	{"ExitApp", CLuaAppObject::ExitApp},				// 退出程序
	{"GetTempDir", CLuaAppObject::GetTempDir},			// 获取临时文件路径
	{"SetWndIcon", CLuaAppObject::SetWndIcon},			// 设置窗口的Icon
	{"GetProfilesDir", CLuaAppObject::GetProfilesDir},			// 获取Profiles路径
	{"ForceUpdateWndShow", CLuaAppObject::ForceUpdateWndShow},
	{"GetFiles", CLuaAppObject::GetFiles},
	{"GetFolders", CLuaAppObject::GetFolders},
	{"StrColl", CLuaAppObject::StrColl},
	{"GetScreenRatio", CLuaAppObject:: GetScreenRatio},
	{"IsPathFileExist", CLuaAppObject::IsPathFileExist},
	{"LogToFile", CLuaAppObject::LogToFile},
	{"GetWorkAreaSize", CLuaAppObject::GetWorkAreaSize},
	{"GetProductVersion", CLuaAppObject::GetProductVersion},
	{"GetTempDir", CLuaAppObject::GetTempDir},
	{"GetMd5Str", CLuaAppObject::GetMd5Str},
	{"RegisterFileRelation", CLuaAppObject::RegisterFileRelation},
	{"AttachShellCmdEvent", CLuaAppObject::AttachShellCmdEvent},
	{"DetachShellCmdEvent", CLuaAppObject::DetachShellCmdEvent},
	{"GetCommandPair", CLuaAppObject::GetCommandPair},
	{"InitFolderMonitor", CLuaAppObject::InitFolderMonitor},
	{"MonitorDirChange", CLuaAppObject::MonitorDirChange},
	{"UnMonitorDirChange", CLuaAppObject::UnMonitorDirChange},
	{"AttachDirChangeEvent", CLuaAppObject::AttachDirChangeEvent},
	{"DetachDirChangeEvent", CLuaAppObject::DetachDirChangeEvent},
	{"IsCanHandleFileCheckByExt", CLuaAppObject::IsCanHandleFileCheckByExt},
	{"DelPathFile2RecycleBin", CLuaAppObject::DelPathFile2RecycleBin},
	{"GetNeoImagingPath", CLuaAppObject::GetNeoImagingPath},
	{"PrintImage", CLuaAppObject::PrintImage},
	{"GetOtherSoftList", CLuaAppObject::GetOtherSoftList},
	{"ExpandEnvironmentStrings", CLuaAppObject::ExpandEnvironmentStrings},
	{"CopyFilePathToCLipBoard", CLuaAppObject::CopyFilePathToCLipBoard},
	{"CopyImageToClipboard", CLuaAppObject::CopyImageToClipboard},
	{"NeoFolderDialog", CLuaAppObject::NeoFolderDialog},
	{"MoveFileTo", CLuaAppObject::MoveFileTo},
	{"CopyFileTo", CLuaAppObject::CopyFileTo},
	{"RenameFile", CLuaAppObject::RenameFile},
	{"SHChangeNotify", CLuaAppObject::SHChangeNotify},
	{"FileRelationCheck", CLuaAppObject::FileRelationCheck},
	{"GetCurveProgress", CLuaAppObject::GetCurveProgress},
	{"IsCanSetToWallPaperFile", CLuaAppObject::IsCanSetToWallPaperFile},
	{"GetPeerId", CLuaAppObject::GetPeerId},
	{"GetOSInfo", CLuaAppObject::GetOSInfo},
	{"GetSystemRatio", CLuaAppObject::GetSystemRatio},
	{"CopyTextToClipboard", CLuaAppObject::CopyTextToClipboard},

	{"GetXiuXiuExePath", CLuaAppObject::GetXiuXiuExePath},
	{"GetNikonNx2ExePath", CLuaAppObject::GetNikonNx2ExePath},
	{"GetDPPExePath", CLuaAppObject::GetDPPExePath},
	{"GetLightRoomExePath", CLuaAppObject::GetLightRoomExePath},
	{"GetPhotoShopExePath", CLuaAppObject::GetPhotoShopExePath},
	{"GetKeNiuExePath", CLuaAppObject::GetKeNiuExePath},
	{"GetOldNeoImagingPath", CLuaAppObject::GetOldNeoImagingPath},
	{"GetIconFromExeFile", CLuaAppObject::GetIconFromExeFile},
	{"ShowColorDialog", CLuaAppObject::ShowColorDialog},
	{"GetExeFileDes", CLuaAppObject::GetExeFileDes},
	{"SetShowCursor", CLuaAppObject::SetShowCursor},	
	{"NULL", NULL}
};
long CLuaAppObject::RegisterSelf(XL_LRT_RUNTIME_HANDLE hEnv)
{
	assert(hEnv);
	if(hEnv == NULL)
	{
		return XLLRT_RESULT_ENV_INVALID;
	}

	XLLRTObject theObject;
	theObject.ClassName = NEOVIEWER_LUAAPP_CLASSNAME;
	theObject.MemberFunctions = LuaAppObjectMemberFunctions;
	theObject.ObjName = NEOVIEWER_LUAAPP_OBJNAME;
	theObject.userData = NULL;
	theObject.pfnGetObject = CLuaAppObject::GetObject;

	long lRet = XLLRT_RegisterGlobalObj(hEnv,theObject); 
	assert(lRet == XLLRT_RESULT_SUCCESS);
	if (lRet != XLLRT_RESULT_SUCCESS)
	{
		return lRet;
	}
	return lRet;
}
// 由于我们并不需要该对象，所以此处直接返回一个-1
void* CLuaAppObject::GetObject(void* p)
{
	return (void*)0xFFFFFFFF;
}
int CLuaAppObject::LuaGc(lua_State* luaState)
{
	_AppImplInstance.ReleaseCache();
	return 0;
}
// 退出程序
int CLuaAppObject::ExitApp(lua_State* luaState)
{
	int nRet = luaL_checkint(luaState, 2);
	//PostQuitMessage(nRet);
	TerminateProcess(GetCurrentProcess(), 0);
	return 0;
}
// 获取临时文件路径
int CLuaAppObject::GetTempDir(lua_State* luaState)
{
	wchar_t wszTempDir[MAX_PATH] = {0};
	GetTempPath(MAX_PATH, wszTempDir);
	// 转码成utf-8格式
	wstring wstrTempDir = wszTempDir;
	string strTempDir;
	//xl::text::transcode::Unicode_to_UTF8(wstrTempDir.c_str(), wstrTempDir.length(), strTempDir);
	strTempDir = ultra::_T2UTF(wszTempDir);
	lua_pushstring(luaState,strTempDir.c_str());
	return 1;
}

int CLuaAppObject::SetWndIcon(lua_State* luaState)
{
	HWND hWnd =(HWND)lua_topointer(luaState, 2);
	HICON hSmallIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), 
		MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON,
		::GetSystemMetrics(SM_CXSMICON), 
		::GetSystemMetrics(SM_CYSMICON), 
		LR_DEFAULTCOLOR);
	HICON hBigIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), 
		MAKEINTRESOURCE(IDR_MAINFRAME),
		IMAGE_ICON,
		::GetSystemMetrics(SM_CXICON),
		::GetSystemMetrics(SM_CYICON), 
		LR_DEFAULTCOLOR);
	::SetClassLong(hWnd, GCL_HICONSM, (LONG)(INT_PTR)hSmallIcon);
	::SetClassLong(hWnd, GCL_HICON, (LONG)(INT_PTR)hBigIcon);
	return 0;
}

int CLuaAppObject::GetProfilesDir(lua_State* luaState)
{
	wstring wstrFilePath = g_pPathHelper->GetProfilesDir();
	string strFilePath;
	//xl::text::transcode::Unicode_to_UTF8(wstrFilePath.c_str(), wstrFilePath.length(), strFilePath);
	strFilePath = ultra::_T2UTF(wstrFilePath);
	lua_pushstring(luaState,strFilePath.c_str());
	return 1;
}
int CLuaAppObject::ForceUpdateWndShow(lua_State* luaState)
{
	HWND hWnd = (HWND)lua_touserdata(luaState, 2);
	SendMessage(hWnd, WM_TIMER, 1000, NULL);
	return 0;
}

int CLuaAppObject::GetPeerId(lua_State* luaState)
{
	string strPeerId = "123456789ABCDEF0";
	lua_pushstring(luaState, strPeerId.c_str());
	return 1;
}
int CLuaAppObject::GetOSInfo(lua_State* luaState)
{
	wstring strOSDesc, strOSVersion;
	if (CUtility::GetOSInfo(strOSDesc, strOSVersion))
	{
		string strDescU8, strVersionU8;		
		//xl::text::transcode::Unicode_to_UTF8(strOSDesc.c_str(), strOSDesc.length(), strDescU8);
		//xl::text::transcode::Unicode_to_UTF8(strOSVersion.c_str(), strOSVersion.length(), strVersionU8);
		strDescU8 = ultra::_T2UTF(strOSDesc);
		strVersionU8 = ultra::_T2UTF(strOSVersion);

		lua_pushstring(luaState, strDescU8.c_str());
		lua_pushstring(luaState, strVersionU8.c_str());
		return 2;
	}
	return 0;
}

int CLuaAppObject::GetSystemRatio(lua_State* luaState)
{
	int nXScreen = GetSystemMetrics(SM_CXSCREEN);
	int nYScreen = GetSystemMetrics(SM_CYSCREEN);
	lua_pushinteger(luaState, nXScreen);
	lua_pushinteger(luaState, nYScreen);
	return 2;
}

int CLuaAppObject::GetFolders(lua_State* luaState)
{
	wstring wstrFolderPath;
	const char* utf8FolderPath = luaL_checkstring(luaState, 2);
	//xl::text::transcode::UTF8_to_Unicode(utf8FolderPath, strlen(utf8FolderPath), wstrFolderPath);
	
	wstrFolderPath = ultra::_UTF2T(utf8FolderPath);
	if(wstrFolderPath[wstrFolderPath.size()-1] != L'\\' && wstrFolderPath[wstrFolderPath.size()-1] != L'/')
	{
		wstrFolderPath.append(L"\\");
	}
	wstring wstrSearchFolderName = wstrFolderPath + L"*.*";
	
	//xl_data FolderList;

	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile(wstrSearchFolderName.c_str(), &FindFileData);

	int i = 0;
	lua_newtable(luaState);

	if(INVALID_HANDLE_VALUE != handle)
	{
		do 
		{
			if (FILE_ATTRIBUTE_DIRECTORY & FindFileData.dwFileAttributes) // 是文件夹
			{
				// 过滤掉隐藏的和系统属性的目录
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN || FindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
				{
					continue;
				}
				if ( (wcscmp(FindFileData.cFileName, L".") != 0) && (wcscmp(FindFileData.cFileName, L"..") != 0))
				{
					wstring wstrTempPath = wstrFolderPath + FindFileData.cFileName;
					//xl_data fileNode;
					lua_newtable(luaState);
					lua_pushstring(luaState, "FilePath");
					lua_pushstring(luaState, ultra::_T2UTF(wstrTempPath).c_str());
					lua_settable(luaState, -3);
					
					lua_pushstring(luaState, "FileName");
					lua_pushstring(luaState, ultra::_T2UTF(FindFileData.cFileName).c_str());
					lua_settable(luaState, -3);

					lua_rawseti(luaState, -2, i + 1);
					i++;
				}
			}
		} while(FindNextFile(handle , &FindFileData));

		CloseHandle(handle);
	}
	return 1;
}

int CLuaAppObject::GetFiles(lua_State* luaState)
{
	//字符比较改为与windows相同的方式
	wstring strFolderName;
	const char* utf8FilePath = luaL_checkstring(luaState, 2);
	//xl::text::transcode::UTF8_to_Unicode(utf8FilePath, strlen(utf8FilePath), strFolderName);
	
	strFolderName = ultra::_UTF2T(utf8FilePath);

	wstring wstrExtNameList;
	const char* utf8ExtNameList = lua_tolstring(luaState, 3, NULL);
	if (utf8ExtNameList)
	{
		//xl::text::transcode::UTF8_to_Unicode(utf8ExtNameList, strlen(utf8ExtNameList), wstrExtNameList);
		//wstrExtNameList = xl::text::string_utility::to_lower(wstrExtNameList);
		
		wstrExtNameList = ultra::ToLower(ultra::_UTF2T(utf8ExtNameList));

	}

	strFolderName.append(L"\\");
	wstring wstrSearchFolderName = strFolderName;
	wstrSearchFolderName.append(L"*.*");
	//xl_data FileList;
	
	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile(wstrSearchFolderName.c_str(), &FindFileData);

	int i = 0;
	lua_newtable(luaState);

	if (INVALID_HANDLE_VALUE != handle)
	{
		do 
		{
			if (FILE_ATTRIBUTE_DIRECTORY & FindFileData.dwFileAttributes)
			{
			}
			else
			{	
				// 过滤掉隐藏的和系统属性的目录
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
				{
					continue;
				}
				// 过滤掉不是自己想要的文件
				if (wstrExtNameList.empty())	// 用户没有传入了需要过滤的后缀名列表，直接用系统默认的
				{
					if (!CUtility::IsCanHandleFileCheckByExt(strFolderName + FindFileData.cFileName))
					{
						continue;
					}
				}
				else	// 用户传入了自己想要的后缀名列表
				{
					wstring wstrExtName = PathFindExtensionW(FindFileData.cFileName);
					if (wstrExtName.empty())	// 没有后缀名
					{
						continue;
					}
					//wstring lowerExtName = xl::text::string_utility::to_lower(wstrExtName);
					wstring lowerExtName = ultra::ToLower(wstrExtName);
					lowerExtName.append(L";");
					if(wstrExtNameList.find(lowerExtName.c_str()) == wstring::npos)
					{
						continue;
					}
				}
				wstring wstrLastWriteTime;

				_SYSTEMTIME stSysTime;
				if (FileTimeToSystemTime(&(FindFileData.ftLastWriteTime), &stSysTime))
				{
					TCHAR* szTSysTime = new TCHAR[1000];
					swprintf(szTSysTime, L"%d-%02d-%02d %02d:%02d:%02d", stSysTime.wYear, stSysTime.wMonth, stSysTime.wDay, stSysTime.wHour, stSysTime.wMinute, stSysTime.wSecond);
					//xl::text::transcode::Unicode_to_UTF8(szTSysTime, wcslen(szTSysTime), strLastWriteTime);
					wstrLastWriteTime = szTSysTime;
					delete[] szTSysTime;
				}
				// 后缀名
				std::wstring wsExt = PathFindExtensionW((strFolderName + FindFileData.cFileName).c_str());
				if (wsExt.length() == 0)
				{
					wsExt = L"*";
				}
				
				// 文件大小
				long long llFileSize = FindFileData.nFileSizeHigh;
				llFileSize <<= 32;
				llFileSize += FindFileData.nFileSizeLow;
				char szFileSize[20] = {0};
				sprintf(szFileSize, "%d", llFileSize);
				
				lua_newtable(luaState);
				lua_pushstring(luaState, "FilePath");
				lua_pushstring(luaState, ultra::_T2UTF(strFolderName + FindFileData.cFileName).c_str());
				lua_settable(luaState, -3);

				lua_pushstring(luaState, "FileName");
				lua_pushstring(luaState, ultra::_T2UTF(FindFileData.cFileName).c_str());
				lua_settable(luaState, -3);

				lua_pushstring(luaState, "LastWriteTime");
				lua_pushstring(luaState, ultra::_T2UTF(wstrLastWriteTime).c_str());
				lua_settable(luaState, -3);

				lua_pushstring(luaState, "ExtName");
				lua_pushstring(luaState, ultra::_T2UTF(wsExt).c_str());
				lua_settable(luaState, -3);

				lua_pushstring(luaState, "FileSize");
				lua_pushstring(luaState, szFileSize);
				lua_settable(luaState, -3);


				lua_rawseti(luaState, -2, i + 1);
				i++;

			}		
		} while(FindNextFile(handle , &FindFileData));
		::FindClose(handle);
	}
	return 1;
}

int CLuaAppObject::StrColl(lua_State* luaState)
{
	//字符比较改为与windows相同的方式
	const char* pszstr1_utf8 = luaL_checkstring(luaState, 2);
	const char* pszstr2_utf8 = luaL_checkstring(luaState, 3);

	wstring str1_unicode;
	//xl::text::transcode::UTF8_to_Unicode(pszstr1_utf8, strlen(pszstr1_utf8), str1_unicode);
	str1_unicode = ultra::_UTF2T(string(pszstr1_utf8));
	wstring str2_unicode;
	//xl::text::transcode::UTF8_to_Unicode(pszstr2_utf8, strlen(pszstr2_utf8), str2_unicode);
	str2_unicode = ultra::_UTF2T(string(pszstr2_utf8));

	int cmpare_result = StrCmpLogicalW(str1_unicode.c_str(), str2_unicode.c_str());
	lua_pushinteger(luaState, cmpare_result);
	return 1;
}
int CLuaAppObject::GetScreenRatio(lua_State* luaState)
{
	int nXScreen = GetSystemMetrics(SM_CXSCREEN);
	int nYScreen = GetSystemMetrics(SM_CYSCREEN);
	lua_pushinteger(luaState, nXScreen);
	lua_pushinteger(luaState, nYScreen);
	return 2;
}

int CLuaAppObject::IsPathFileExist(lua_State* luaState)
{
	const char* utf8FilePath = luaL_checkstring(luaState, 2);
	wstring wstrFilePath;
	//xl::text::transcode::UTF8_to_Unicode(utf8FilePath, strlen(utf8FilePath), wstrFilePath);
	wstrFilePath = ultra::_UTF2T(utf8FilePath);
	BOOL bExist = PathFileExists(wstrFilePath.c_str());
	lua_pushboolean(luaState, bExist);
	return 1;
}

int CLuaAppObject::LogToFile(lua_State* luaState)
{
	wstring wstrLogInfo;
	const char* utf8LogInfo = luaL_checkstring(luaState, 2);
	//xl::text::transcode::UTF8_to_Unicode(utf8LogInfo, strlen(utf8LogInfo), wstrLogInfo);
	wstrLogInfo = ultra::_UTF2T(utf8LogInfo);
	TSINFO4CXX(L"Lua Log: "<< wstrLogInfo.c_str());
	return 0;
}
int CLuaAppObject::GetWorkAreaSize(lua_State* luaState)
{
	RECT rect; 
	::SystemParametersInfo( SPI_GETWORKAREA, sizeof(RECT), &rect, 0 ); 
	lua_pushinteger(luaState, rect.right-rect.left);
	lua_pushinteger(luaState, rect.bottom-rect.top);
	return 2;
}
int CLuaAppObject::GetProductVersion(lua_State* luaState)
{
	//wstring strExePath = xl::filesystem::path_utility::get_current_exe_path();
	
	//wstring strExePath = ultra::GetModuleFileName();

	//wstring strVersion = xl::win32::version_utility::get_file_version(strExePath.c_str());

	string strVersionU8 = "1.1.1.1";
	//xl::text::transcode::Unicode_to_UTF8(strVersion.c_str(), strVersion.length(), strVersionU8);
	lua_pushstring(luaState, strVersionU8.c_str());
	return 1;
}

int CLuaAppObject::GetMd5Str(lua_State* luaState)
{
	// 获取要计算md5值的字符串
	const char* utf8;
	utf8 = luaL_checkstring(luaState, 2);
	string strValue = utf8;
	
	wchar_t pszMD5[MAX_PATH] = {0};
	if (GetStringMd5(strValue,pszMD5))
	{
		std::string utf8MD5 = ultra::_T2UTF(pszMD5);
		
		lua_pushstring(luaState, utf8MD5.c_str());
		return 1;
	}

	return 0;
}

int CLuaAppObject::RegisterFileRelation(lua_State* luaState)
{
	/*wstring wstrExtName;
	wstring wstrExeDir = g_pPathHelper->GetExeDir();
	const char* utf8ExtName = luaL_checkstring(luaState, 2);
	xl::text::transcode::UTF8_to_Unicode(utf8ExtName, strlen(utf8ExtName), wstrExtName);
	bool bSet = lua_toboolean(luaState, 3);
	bool bFresh = lua_toboolean(luaState, 4);

	wstring wstrAppKey = L"NeoViewer";
	wstrAppKey += wstrExtName;

	wstring wstrExePath = wstrExeDir + L"NeoViewer.exe";
	wstring wstrIconPath = wstrExeDir + L"res\\" + wstrExtName.substr(1, wstrExtName.size()-1) + L".ico";
	if (!PathFileExists(wstrIconPath.c_str()))
	{
		wstrIconPath = wstrExeDir + L"res\\Default.ico";
	}
	wstring wstrDes = L"光影看图";
	wstrDes.append(xl::text::string_utility::to_upper(wstrExtName.substr(1, wstrExtName.size()-1)));
	wstrDes.append(L"图片文件");
	CFileRelation::RegisterFileRelation(wstrExtName, wstrAppKey, wstrExePath, wstrIconPath, wstrDes, bSet, bFresh);*/
	return 0;
}

int CLuaAppObject::FileRelationCheck(lua_State* luaState)
{
	//wstring wstrExtName;
	//const char* utf8ExtName = luaL_checkstring(luaState, 2);
	//xl::text::transcode::UTF8_to_Unicode(utf8ExtName, strlen(utf8ExtName), wstrExtName);
	//wstring wstrAppKey = L"NeoViewer";
	//wstrAppKey += wstrExtName;
	//if (CFileRelation::CheckFileRelation(wstrExtName, wstrAppKey)) // 是我们的
	//{
	//	lua_pushboolean(luaState, true);
	//}
	//else
	//{
	//	lua_pushboolean(luaState, false);
	//}

	lua_pushboolean(luaState, true);
	return 1;
}

int CLuaAppObject::AttachShellCmdEvent(lua_State* luaState)
{
	CShellCmdManager* pCShellCmdManager = CShellCmdManager::Instance();
	if (pCShellCmdManager)
	{
		wstring wstrEventName = L"OnShellCmdEvent";
		DWORD cookie;
		HRESULT hret = pCShellCmdManager->AttachListener(wstrEventName, luaState, 2, cookie);
		if (SUCCEEDED(hret))
		{
			lua_pushinteger(luaState, cookie);
			return 1;
		}
	}
	return 0;
}
int CLuaAppObject::DetachShellCmdEvent(lua_State* luaState)
{
	CShellCmdManager* pCShellCmdManager = CShellCmdManager::Instance();
	if (pCShellCmdManager)
	{
		wstring wstrEventName = L"OnShellCmdEvent";
		DWORD cookie;
		cookie = lua_tointeger(luaState, 2);
		HRESULT hret = pCShellCmdManager->DetachListener(wstrEventName, cookie);
		if (SUCCEEDED(hret))
		{
			lua_pushboolean(luaState, true);
			return 1;
		}
	}
	return 0;
}

int CLuaAppObject::GetCommandPair(lua_State* luaState)
{
	CShellCmdManager* pCShellCmdManager = CShellCmdManager::Instance();
	if (pCShellCmdManager)
	{
		lua_newtable(luaState);
		ShellCmdPair CommandPair = pCShellCmdManager->GetCommandPair();
		for ( ShellCmdPair::iterator iter = CommandPair.begin(); iter != CommandPair.end(); ++iter )
		{
			std::string strKey, strValue;
			//xl::text::transcode::Unicode_to_UTF8(iter->first.c_str(), iter->first.length(), strKey);
			//xl::text::transcode::Unicode_to_UTF8(iter->second.c_str(), iter->second.length(), strValue);
			
			strKey = ultra::_T2UTF(iter->first.c_str());
			strValue = ultra::_T2UTF(iter->second.c_str());
			lua_pushstring(luaState, strKey.c_str());
			lua_pushstring(luaState, strValue.c_str());
			lua_settable(luaState, -3);
		}
		return 1;
	}
	return 0;
}
int CLuaAppObject::MonitorDirChange(lua_State* luaState)
{
	const char* utf8DirPath = luaL_checkstring(luaState, 2);
	wstring wstrDirPath;
	//xl::text::transcode::UTF8_to_Unicode(utf8DirPath, strlen(utf8DirPath), wstrDirPath);
	
	wstrDirPath = ultra::_UTF2T(utf8DirPath);
	CFolderChangeMonitor* pMonitor = CFolderChangeMonitor::Instance();
	long cookie = pMonitor->MonitorDirChange(wstrDirPath);
	lua_pushinteger(luaState, cookie);
	return 1;
}

int CLuaAppObject::UnMonitorDirChange(lua_State* luaState)
{
	long lCookie = lua_tointeger(luaState, 2);
	CFolderChangeMonitor* pMonitor = CFolderChangeMonitor::Instance();
	pMonitor->UnMonitorDirChange(lCookie);
	return 0;
}


int CLuaAppObject::AttachDirChangeEvent(lua_State* luaState)
{	
	CFolderChangeMonitor* pMonitor = CFolderChangeMonitor::Instance();
	DWORD cookie;
	HRESULT hret = pMonitor->AttachDirChangeEvent(luaState, 2, cookie);
	if (SUCCEEDED(hret))
	{
		lua_pushinteger(luaState, cookie);
		return 1;
	}
	return 0;
}
int CLuaAppObject::DetachDirChangeEvent(lua_State* luaState)
{
	CFolderChangeMonitor* pMonitor = CFolderChangeMonitor::Instance();
	DWORD cookie;
	cookie = lua_tointeger(luaState, 2);
	HRESULT hret = pMonitor->DetachDirChangeEvent(cookie);
	if (SUCCEEDED(hret))
	{
		lua_pushboolean(luaState, true);
		return 1;
	}
	return 0;
}

int CLuaAppObject::InitFolderMonitor(lua_State* luaState)
{
	CFolderChangeMonitor* pMonitor = CFolderChangeMonitor::Instance();
	if (pMonitor)
	{
		if(pMonitor->Init())
		{
			lua_pushboolean(luaState, 1);
			return 1;
		}
	}
	lua_pushboolean(luaState, 0);
	return 1;
}

int CLuaAppObject::IsCanHandleFileCheckByExt(lua_State* luaState)
{
	const char* utf8 = luaL_checkstring(luaState, 2);
	wstring wstrFilePath;
	//xl::text::transcode::UTF8_to_Unicode(utf8, strlen(utf8), wstrFilePath);
	wstrFilePath = ultra::_UTF2T(utf8);
	if (CUtility::IsCanHandleFileCheckByExt(wstrFilePath))
	{
		lua_pushboolean(luaState, true);
	}
	else
	{
		lua_pushboolean(luaState, false);
	}
	return 1;
}
int CLuaAppObject::DelPathFile2RecycleBin(lua_State* luaState)
{
	//字符比较改为与windows相同的方式
	const char* utf8FilePath = luaL_checkstring(luaState, 2);

	wstring wstrFilePath;
	//xl::text::transcode::UTF8_to_Unicode(utf8FilePath, strlen(utf8FilePath), wstrFilePath);
	wstrFilePath = ultra::_UTF2T(utf8FilePath);
	wchar_t wcsFilePath[1024];
	wmemset(wcsFilePath, 0, 1024);
	wsprintf(wcsFilePath, L"%s", wstrFilePath.c_str());

	SHFILEOPSTRUCT shFOP;
	memset(&shFOP, 0, sizeof(shFOP));
	shFOP.hwnd = NULL;
	shFOP.wFunc = FO_DELETE;
	shFOP.pFrom = wcsFilePath;
	shFOP.pTo = NULL;
	shFOP.fFlags = FOF_ALLOWUNDO | FOF_NOERRORUI | FOF_NOCONFIRMATION;
	shFOP.fAnyOperationsAborted = FALSE;
	SHFileOperation(&shFOP);
	return 0;
}

int CLuaAppObject::GetNeoImagingPath(lua_State* luaState)
{
	CRegKey regKey;
	if(regKey.Open(HKEY_CURRENT_USER, L"Software\\Thunder Network\\NeoImaging") == ERROR_SUCCESS)
	{
		wchar_t strAppKey[256] = {0};
		ULONG len;
		if (regKey.QueryStringValue(L"path", strAppKey, &len) == ERROR_SUCCESS)
		{
			if (::PathFileExists(strAppKey))
			{
				string strNeoImagingPath;
				//xl::text::transcode::Unicode_to_UTF8(strAppKey, len, strNeoImagingPath);
				strNeoImagingPath = ultra::_T2UTF(strAppKey);
				lua_pushstring(luaState, strNeoImagingPath.c_str());
				return 1;
			}
		}
	}
	return 0;
}

int CLuaAppObject::PrintImage(lua_State* luaState)
{
	const char* utf8 = luaL_checkstring(luaState, 2);
	wstring wstrFilePath;
	wstring wstrFileName;
	//xl::text::transcode::UTF8_to_Unicode(utf8, strlen(utf8), wstrFilePath);
	wstrFilePath = ultra::_UTF2T(utf8);

	wchar_t wszPath[512] = {0};
	wcscpy(wszPath, wstrFilePath.c_str());
	PathRemoveFileSpec(wszPath);
	wstrFileName = ::PathFindFileName(wstrFilePath.c_str());
	CUtility::PrintImage(wszPath, (wchar_t*)wstrFileName.c_str());
	return 0;
}

int CLuaAppObject::GetOtherSoftList(lua_State* luaState)
{
	TSAUTO();
	//const char* utf8 = luaL_checkstring(luaState, 2);
	//wstring wstrExtName;
	//xl::text::transcode::UTF8_to_Unicode(utf8, strlen(utf8), wstrExtName);
	//
	//// 要返回的数据列表
	//xl_data FileList;


	//// 遍历注册表 Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts
	//CRegKey key;
	//wstring wstrKeyName = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\";
	//wstrKeyName += wstrExtName;
	//wstrKeyName += L"\\OpenWithList";

	//if (key.Open(HKEY_CURRENT_USER, wstrKeyName.c_str(), KEY_QUERY_VALUE) == ERROR_SUCCESS)	// 打开成功
	//{
	//	TSINFO4CXX(L"HKEY_CURRENT_USER Open Register Success " << wstrKeyName);
	//	wchar_t wszName[128] = {0};
	//	DWORD nNameBytes = 128;
	//	wchar_t wszValue[256] = {0};
	//	DWORD nValueBytes = 256;
	//	int nIndex = 0;
	//	DWORD dwType;
	//	while(::RegEnumValue(key.m_hKey, nIndex, wszName, &nNameBytes, NULL, &dwType, (LPBYTE)wszValue, &nValueBytes) == ERROR_SUCCESS)
	//	{
	//		if (dwType == REG_SZ)
	//		{
	//			// 读取对应的文件信息
	//			CRegKey newKey;
	//			wstring wstrKeyName = L"Applications\\";
	//			wstrKeyName += wszValue;
	//			wstrKeyName += L"\\shell\\open\\command";
	//			TSINFO4CXX(L"RegName:" << wszValue << L" wstrKeyName:" << wstrKeyName);
	//			if(newKey.Open(HKEY_CLASSES_ROOT, wstrKeyName.c_str(), KEY_QUERY_VALUE) == ERROR_SUCCESS)
	//			{
	//				wchar_t wszValue[512] = {0};
	//				ULONG nBytes = 512;
	//				if(newKey.QueryStringValue(NULL, wszValue, &nBytes) == ERROR_SUCCESS)
	//				{
	//					wstring wstrExePath = wszValue;
	//					wstring::size_type nPos = wstrExePath.find(L".exe");
	//					if (nPos != wstring::npos)
	//					{
	//						if (wszValue[0] == L'\"')
	//						{
	//							wstrExePath = wstrExePath.substr(1, nPos+3);
	//						}
	//						else
	//						{
	//							wstrExePath = wstrExePath.substr(0, nPos+4);
	//						}
	//						// 判断是否存在
	//						if (PathFileExists(wstrExePath.c_str()))
	//						{
	//							wstring wstrFileDes = CUtility::GetFileDesInfo(wstrExePath);
	//							if (wstrFileDes.size()>0)
	//							{
	//								xl_data fileNode;
	//								fileNode["FilePath"] = wstrExePath.c_str();
	//								fileNode["FileDes"] = wstrFileDes.c_str();
	//								FileList.insert(FileList.size(), fileNode);
	//							}
	//						}
	//					}

	//				}
	//				newKey.Close();
	//			}
	//		}
	//		wmemset(wszValue, 0, 256);
	//		nNameBytes = 256;
	//		nValueBytes = 256;
	//		nIndex ++;
	//	}
	//	key.Close();
	//}
	//else
	//{
	//	TSINFO4CXX(L"Open Register Faild " << wstrKeyName);
	//}

	//// 遍历注册表 HKEY_CLASSES_ROOT\.xxx\OpenWithProgids
	//wstrKeyName = wstrExtName;
	//wstrKeyName += L"\\OpenWithProgids";
	//if (key.Open(HKEY_CLASSES_ROOT, wstrKeyName.c_str(), KEY_QUERY_VALUE) == ERROR_SUCCESS)	// 打开成功
	//{
	//	TSINFO4CXX(L"HKEY_CLASSES_ROOT Open Register Success " << wstrKeyName);
	//	wchar_t wszName[128] = {0};
	//	DWORD nNameBytes = 128;
	//	wchar_t wszValue[256] = {0};
	//	DWORD nValueBytes = 256;
	//	int nIndex = 0;
	//	DWORD dwType;
	//	while(::RegEnumValue(key.m_hKey, nIndex, wszName, &nNameBytes, NULL, &dwType, (LPBYTE)wszValue, &nValueBytes) == ERROR_SUCCESS)
	//	{
	//		// 读取对应的文件信息
	//		CRegKey newKey;
	//		wstring wstrKeyName = wszName;
	//		wstrKeyName += L"\\shell\\open\\command";
	//		TSINFO4CXX(L"RegName:" << wszValue << L" wstrKeyName:" << wstrKeyName);
	//		if(newKey.Open(HKEY_CLASSES_ROOT, wstrKeyName.c_str(), KEY_QUERY_VALUE) == ERROR_SUCCESS)
	//		{
	//			wchar_t wszValue[512] = {0};
	//			ULONG nBytes = 512;
	//			if(newKey.QueryStringValue(NULL, wszValue, &nBytes) == ERROR_SUCCESS)
	//			{
	//				wstring wstrExePath = wszValue;
	//				wstring::size_type nPos = wstrExePath.find(L".exe");
	//				if (nPos != wstring::npos)
	//				{
	//					if (wszValue[0] == L'\"')
	//					{
	//						wstrExePath = wstrExePath.substr(1, nPos+3);
	//					}
	//					else
	//					{
	//						wstrExePath = wstrExePath.substr(0, nPos+4);
	//					}
	//					// 判断是否存在
	//					if (PathFileExists(wstrExePath.c_str()))
	//					{
	//						wstring wstrFileDes = CUtility::GetFileDesInfo(wstrExePath);
	//						if (wstrFileDes.size()>0)
	//						{
	//							xl_data fileNode;
	//							fileNode["FilePath"] = wstrExePath.c_str();
	//							fileNode["FileDes"] = wstrFileDes.c_str();
	//							FileList.insert(FileList.size(), fileNode);
	//						}
	//					}
	//				}

	//			}
	//		}
	//		wmemset(wszValue, 0, 256);
	//		nNameBytes = 256;
	//		nValueBytes = 256;
	//		nIndex ++;
	//	}
	//}
	//else
	//{
	//	TSINFO4CXX(L"HKEY_CLASSES_ROOT Open Register Failed " << wstrKeyName);
	//}

	//if (FileList.size() < 1)
	//{
	//	lua_newtable(luaState);
	//}
	//else
	//{
	//	FileList.lua_pushxldata(luaState);
	//}
	return 0;
}

int CLuaAppObject::GetExeFileDes(lua_State* luaState)
{
	const char* utf8 = luaL_checkstring(luaState, 2);
	wstring wstrFilePath;
	//xl::text::transcode::UTF8_to_Unicode(utf8, strlen(utf8), wstrFilePath);
	wstrFilePath = ultra::_UTF2T(utf8);
	wstring wstrFileDes = CUtility::GetFileDesInfo(wstrFilePath);
	string strFileDes;
	//xl::text::transcode::Unicode_to_UTF8(wstrFileDes.c_str(), wstrFileDes.size(), strFileDes);
	strFileDes = ultra::_T2UTF(wstrFileDes);
	lua_pushstring(luaState, strFileDes.c_str());
	return 1;
}

int CLuaAppObject::ExpandEnvironmentStrings(lua_State* luaState)
{
	const char* utf8 = luaL_checkstring(luaState, 2);
	wstring wstrFilePath;
	wchar_t wszFilePath[MAX_PATH] = {0};
	//xl::text::transcode::UTF8_to_Unicode(utf8, strlen(utf8), wstrFilePath);
	wstrFilePath = ultra::_UTF2T(utf8);
	::ExpandEnvironmentStrings(wstrFilePath.c_str(), wszFilePath, MAX_PATH);
	string strFilePath;
	//xl::text::transcode::Unicode_to_UTF8(wszFilePath, wcslen(wszFilePath), strFilePath);
	strFilePath = ultra::_T2UTF(wszFilePath);
	lua_pushstring(luaState, strFilePath.c_str());
	return 1;
}

int CLuaAppObject::CopyFilePathToCLipBoard(lua_State* luaState)
{
	const char* utf8 = luaL_checkstring(luaState, 2);
	wstring wstrFilePath;
	//xl::text::transcode::UTF8_to_Unicode(utf8, strlen(utf8), wstrFilePath);
	
	wstrFilePath = ultra::_UTF2T(utf8);
	UINT uDropEffect;
	DROPFILES dropFiles;
	UINT uGlbLen, uDropFIlesLen;

	uDropEffect = RegisterClipboardFormat(L"Preferred DropEffect");
	HGLOBAL hGblEffect = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(DWORD));
	DWORD *dwDropEffect = (DWORD*)GlobalLock(hGblEffect);
	*dwDropEffect = DROPEFFECT_COPY;
	GlobalUnlock(hGblEffect);

	uDropFIlesLen = sizeof(DROPFILES);
	dropFiles.pFiles = uDropFIlesLen;
	dropFiles.pt.x = 0;
	dropFiles.pt.y = 0;
	dropFiles.fNC = FALSE;
	dropFiles.fWide = TRUE;

	uGlbLen = uDropFIlesLen+wstrFilePath.size()*2 + 8;
	HGLOBAL hGblFiles = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, uGlbLen);
	BYTE* szData=(BYTE*)GlobalLock(hGblFiles);
	memcpy(szData, (LPVOID)(&dropFiles), uDropFIlesLen);
	wchar_t* wszFileList = (wchar_t*)(szData + uDropFIlesLen);
	wcscpy(wszFileList, wstrFilePath.c_str());
	GlobalUnlock(hGblFiles);

	if (OpenClipboard(NULL))
	{
		EmptyClipboard();
		SetClipboardData(CF_HDROP, hGblFiles);
		SetClipboardData(uDropEffect, hGblEffect);
		CloseClipboard();
	}
	return 0;
}

int CLuaAppObject::CopyImageToClipboard(lua_State* luaState)
{
	XL_BITMAP_HANDLE hBitmap = NULL;
	if(XLGP_CheckBitmap(luaState, 2, &hBitmap))
	{
		XLBitmapInfo SrcBmpInfo;
		XL_GetBitmapInfo(hBitmap, &SrcBmpInfo);
		unsigned long nWidth = SrcBmpInfo.Width;
		unsigned long nHeight = SrcBmpInfo.Height;



		//还得在白色的底上融合一下，因为复制出去的图不能带alpha信息
		XL_BITMAP_HANDLE hDstBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, nWidth, nHeight);
		assert(hDstBitmap);
		if (hDstBitmap)
		{
			XL_FillBitmap(hDstBitmap, XLCOLOR_BGRA(255, 255, 255, 255));
			XL_PreMultiplyBitmap(hBitmap); //混合前要预乘
			XLGP_SrcAlphaBlend(hDstBitmap, 0, 0, hBitmap, 255);
			XL_ReleaseBitmap(hBitmap);
			hBitmap = hDstBitmap;
		}
		//开始复制操作
		BITMAPINFO bmpInfo;
		memset(&bmpInfo, 0, sizeof(BITMAPINFO));
		bmpInfo.bmiHeader.biHeight = nHeight; //QQ和老光魔都不认这里是负数的信息头，系统画图板认。。。所以这里只能把它设为正数
		bmpInfo.bmiHeader.biWidth = nWidth;
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biBitCount = 32;
		bmpInfo.bmiHeader.biCompression = BI_RGB;
		bmpInfo.bmiHeader.biSizeImage = 0;
		bmpInfo.bmiHeader.biXPelsPerMeter = 0;
		bmpInfo.bmiHeader.biYPelsPerMeter = 0;
		bmpInfo.bmiHeader.biClrUsed = 0;
		bmpInfo.bmiHeader.biClrImportant = 0;

		BOOL ret = ::OpenClipboard(NULL);
		if (ret == FALSE)
		{
			XL_ReleaseBitmap(hBitmap);
			return 0;
		}
		ret = ::EmptyClipboard();
		if (ret == FALSE)
		{
			XL_ReleaseBitmap(hBitmap);
			::CloseClipboard();
			return 0;
		}

		HGLOBAL hDIB = ::GlobalAlloc(GHND | GMEM_SHARE, sizeof(BITMAPINFOHEADER) + SrcBmpInfo.ScanLineLength * SrcBmpInfo.Height);
		if (hDIB == NULL)
		{
			XL_ReleaseBitmap(hBitmap);
			::CloseClipboard();
			return 0;
		}

		BYTE* lpBuffer = (BYTE*)::GlobalLock(hDIB);
		assert(lpBuffer);
		if (lpBuffer == NULL)
		{
			XL_ReleaseBitmap(hBitmap);
			::GlobalFree(hDIB);
			::CloseClipboard();
			return 0;
		}

		*((LPBITMAPINFO)lpBuffer) = bmpInfo;
		//注意，千万注意，下面这句不是一般的坑爹。MSDN里面说，传到剪贴板里面的内存块应该包含BITMAPINFO结构和像素数据，而实际上在这里，当我们没有使用调色板信息的时候，
		//我们应该只传入一个BITMAPINFOHEADER结构和像素数据，也就是说，比MSDN上说的要少传入4个像素！！！不然像素数据值有偏差了
		BYTE* pBitBuf = lpBuffer + sizeof(BITMAPINFOHEADER); //这里不能是sizeof(BITMAPINFO）
		BYTE* pSrcImageBuf = XL_GetBitmapBuffer(hBitmap, 0, 0);
		for (unsigned long j = 0; j < SrcBmpInfo.Height; j++)
		{
			memcpy(pBitBuf + SrcBmpInfo.ScanLineLength*j, pSrcImageBuf + SrcBmpInfo.ScanLineLength*(SrcBmpInfo.Height - 1 - j), SrcBmpInfo.ScanLineLength);
		}
		::GlobalUnlock(hDIB);

		HANDLE hHandle = ::SetClipboardData(CF_DIB, hDIB);
		if (hHandle == NULL)
		{
			XL_ReleaseBitmap(hBitmap);
			::GlobalFree(hDIB);
			::CloseClipboard();
			return 0;
		}
		XL_ReleaseBitmap(hBitmap);
		::CloseClipboard();
		::GlobalFree(hDIB);
	}
	return 0;
}

BOOL IS_Vista_Or_More()
{
	OSVERSIONINFO osver;
	ZeroMemory( &osver, sizeof( osver ) );
	osver.dwOSVersionInfoSize = sizeof( osver );

	if( !GetVersionEx (&osver) )
	{
		osver.dwOSVersionInfoSize = sizeof (osver);
		if (! GetVersionEx ( &osver) ) 
			return FALSE;
	}
	return osver.dwMajorVersion >= 6;
	//return false;
}

typedef HRESULT (STDAPICALLTYPE*SHCreateItemFromParsingNameFuncType)(
	_In_   PCWSTR pszPath,
	_In_   IBindCtx *pbc,
	_In_   REFIID riid,
	_Out_  void **ppv
	);

SHCreateItemFromParsingNameFuncType GetFunctionAddress()
{
	HMODULE handle = ::LoadLibraryW( L"SHELL32.dll" );
	if( handle == NULL )
		return NULL;
	return (SHCreateItemFromParsingNameFuncType)::GetProcAddress( handle, "SHCreateItemFromParsingName" );
}
int CLuaAppObject::NeoFolderDialog(lua_State* luaState)
{
	std::string utf8Title;
	if (lua_type(luaState, 2) == LUA_TSTRING)
	{
		utf8Title = luaL_checkstring(luaState,2);
	}
	std::string utf8InitialFolder;
	if (lua_type(luaState, 3) == LUA_TSTRING)
	{
		utf8InitialFolder = luaL_checkstring(luaState,3);
	}
	std::string strPath;

	std::wstring wstrTitle;
	//xl::text::transcode::UTF8_to_Unicode(utf8Title.c_str(), utf8Title.length(), wstrTitle);
	wstrTitle = ultra::_UTF2T(utf8Title);
	std::wstring wstrInitialFolder;
	//xl::text::transcode::UTF8_to_Unicode(utf8InitialFolder.c_str(), strlen(utf8InitialFolder.c_str()), wstrInitialFolder);
	
	wstrInitialFolder =  ultra::_UTF2T(utf8InitialFolder);
	// 判断目录是否存在不存时指向上一级目录
	while( !PathFileExists(wstrInitialFolder.c_str()) )
	{
		std::wstring::size_type nPos = wstrInitialFolder.find_last_of(L"\\");
		if( nPos != std::wstring::npos )
		{
			wstrInitialFolder = wstrInitialFolder.substr(0,nPos);
		}
		else
		{
			break;
		}
	}
	//根目录如果没有"\"会不能定位到根目录．
	if(wstrInitialFolder.length() == 0 || wstrInitialFolder[wstrInitialFolder.length() - 1] != L'\\')
		wstrInitialFolder += L"\\";

	if( IS_Vista_Or_More() )
	{
		BOOL is_check = FALSE;
		IFileOpenDialog *pfd = NULL;
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
		if (SUCCEEDED(hr))
		{
			pfd->SetTitle( wstrTitle.c_str() );
			IShellItem* pItem = NULL;
			SHCreateItemFromParsingNameFuncType funcPtr = GetFunctionAddress();
			if( funcPtr )
			{
				hr = funcPtr( wstrInitialFolder.c_str(), NULL,IID_PPV_ARGS(&pItem));
				//hr = ::SHCreateItemFromParsingName( L"d:\\", NULL,IID_PPV_ARGS(&pItem));
				if (SUCCEEDED(hr))
					pfd->SetFolder( pItem );
			}
			DWORD dwFlags;
			hr = pfd->GetOptions(&dwFlags);
			if (SUCCEEDED(hr))
			{
				dwFlags |= FOS_PICKFOLDERS;
				hr = pfd->SetOptions( dwFlags );//FOS_FORCEFILESYSTEM FOS_PICKFOLDERS FOS_HIDEPINNEDPLACES  FOS_ALLOWMULTISELECT 
				if (SUCCEEDED(hr))
				{
					IFileDialogCustomize *pfdc = NULL;
					hr = pfd->QueryInterface(IID_PPV_ARGS(&pfdc));
					if( SUCCEEDED(hr) )
					{
						const DWORD check_button_id = 999;
						hr = pfdc->AddCheckButton( check_button_id,L"应用到子文件夹", FALSE );
						if( SUCCEEDED(hr) )
						{
							hr = pfd->Show( GetActiveWindow());
							if (SUCCEEDED(hr))
							{
								IShellItem *psiResult;
								hr = pfd->GetResult(&psiResult);
								if (SUCCEEDED(hr))
								{	
									PWSTR pszFilePath = NULL;
									hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
									if (SUCCEEDED(hr))
									{
										//xl::text::transcode::Unicode_to_UTF8(pszFilePath, lstrlen(pszFilePath), strPath );
										strPath = ultra::_T2UTF(pszFilePath);
									}
									psiResult->Release();
								}
								pfdc->GetCheckButtonState( check_button_id, &is_check );
							}
						}
						pfdc->Release();
					}
				}
			}
			if( pItem )
				pItem->Release();
			pfd->Release();
		}
		lua_pushstring(luaState, strPath.c_str() );
		lua_pushboolean(luaState, is_check );
	}
	else
	{
		CNeoFolderDialog dlg(0, wstrTitle.c_str(), BIF_RETURNONLYFSDIRS|BIF_USENEWUI);
		dlg.SetInitialFolder(wstrInitialFolder.c_str(), true);
		if (IDOK == dlg.DoModal())
		{
			//xl::text::transcode::Unicode_to_UTF8(dlg.GetFolderPath(), lstrlen(dlg.GetFolderPath()), strPath);
			strPath = ultra::_T2UTF(dlg.GetFolderPath());
		}
		lua_pushstring(luaState, strPath.c_str());
		lua_pushboolean(luaState, dlg.GetCheckState());
	}
	return 2;
}
int CLuaAppObject::CopyFileTo(lua_State* luaState)
{
	//字符比较改为与windows相同的方式
	const char* utf8FilePath = luaL_checkstring(luaState, 2);
	const char* utf8FolderPath = luaL_checkstring(luaState, 3);
	wstring wstrFilePath, wstrFolderPath;
	//xl::text::transcode::UTF8_to_Unicode(utf8FilePath, strlen(utf8FilePath), wstrFilePath);
	wstrFilePath = ultra::_UTF2T(utf8FilePath);
	//xl::text::transcode::UTF8_to_Unicode(utf8FolderPath, strlen(utf8FolderPath), wstrFolderPath);
	wstrFolderPath = ultra::_UTF2T(utf8FolderPath);

	wchar_t wcsFilePath[1024];
	wmemset(wcsFilePath, 0, 1024);
	wsprintf(wcsFilePath, L"%s", wstrFilePath.c_str());

	SHFILEOPSTRUCT shFOP;
	memset(&shFOP, 0, sizeof(shFOP));
	shFOP.hwnd = NULL;
	shFOP.wFunc = FO_COPY;
	shFOP.pFrom = wcsFilePath;
	shFOP.pTo = wstrFolderPath.c_str();
	shFOP.fFlags = FOF_SIMPLEPROGRESS;
	shFOP.fAnyOperationsAborted = FALSE;
	SHFileOperation(&shFOP);
	return 0;
}
int CLuaAppObject::MoveFileTo(lua_State* luaState)
{
	//字符比较改为与windows相同的方式
	const char* utf8FilePath = luaL_checkstring(luaState, 2);
	const char* utf8FolderPath = luaL_checkstring(luaState, 3);
	wstring wstrFilePath, wstrFolderPath;
	//xl::text::transcode::UTF8_to_Unicode(utf8FilePath, strlen(utf8FilePath), wstrFilePath);
	wstrFilePath = ultra::_UTF2T(utf8FilePath);
	//xl::text::transcode::UTF8_to_Unicode(utf8FolderPath, strlen(utf8FolderPath), wstrFolderPath);
	wstrFolderPath = ultra::_UTF2T(utf8FolderPath);

	wchar_t wcsFilePath[1024];
	wmemset(wcsFilePath, 0, 1024);
	wsprintf(wcsFilePath, L"%s", wstrFilePath.c_str());

	SHFILEOPSTRUCT shFOP;
	memset(&shFOP, 0, sizeof(shFOP));
	shFOP.hwnd = NULL;
	shFOP.wFunc = FO_MOVE;
	shFOP.pFrom = wcsFilePath;
	shFOP.pTo = wstrFolderPath.c_str();
	shFOP.fFlags = FOF_SIMPLEPROGRESS;
	shFOP.fAnyOperationsAborted = FALSE;
	SHFileOperation(&shFOP);
	return 0;
}

int CLuaAppObject::RenameFile(lua_State* luaState)
{
	//字符比较改为与windows相同的方式
	const char* utf8FilePath = luaL_checkstring(luaState, 2);
	const char* utf8FolderPath = luaL_checkstring(luaState, 3);
	wstring wstrFilePath, wstrFolderPath;
	//xl::text::transcode::UTF8_to_Unicode(utf8FilePath, strlen(utf8FilePath), wstrFilePath);
	wstrFilePath = ultra::_UTF2T(utf8FilePath);
	//xl::text::transcode::UTF8_to_Unicode(utf8FolderPath, strlen(utf8FolderPath), wstrFolderPath);
	wstrFolderPath = ultra::_UTF2T(utf8FolderPath);

	wchar_t wcsFilePath[1024];
	wmemset(wcsFilePath, 0, 1024);
	wsprintf(wcsFilePath, L"%s", wstrFilePath.c_str());

	SHFILEOPSTRUCT shFOP;
	memset(&shFOP, 0, sizeof(shFOP));
	shFOP.hwnd = NULL;
	shFOP.wFunc = FO_RENAME;
	shFOP.pFrom = wcsFilePath;
	shFOP.pTo = wstrFolderPath.c_str();
	shFOP.fFlags = FOF_SIMPLEPROGRESS;
	shFOP.fAnyOperationsAborted = FALSE;
	SHFileOperation(&shFOP);
	return 0;
}

int CLuaAppObject::SHChangeNotify(lua_State* luaState)
{
	::SHChangeNotify(SHCNE_ASSOCCHANGED,SHCNF_FLUSHNOWAIT,0,0);
	return 0;
}

int CLuaAppObject::GetCurveProgress(lua_State* luaState)
{
	float fValue = lua_tonumber(luaState, 2);
	XL_CURVE_HANDLE hCurve = XL_CreateCurve(XLCURVE_BEZIER);
	XL_AddCurveControlPoint(hCurve, 0, 0);
	XL_AddCurveControlPoint(hCurve, 0.085165, 0.808989);
	XL_AddCurveControlPoint(hCurve, 0.725275, 0.862360);
	XL_AddCurveControlPoint(hCurve, 1.0, 1.0);
	float fDes = XL_GetCurveProgress(hCurve, fValue);
	XL_ReleaseCurve(hCurve);
	lua_pushnumber(luaState, fDes*1000);
	return 1;
}

int CLuaAppObject::GetXiuXiuExePath(lua_State* luaState)
{
	TSAUTO();
	CRegKey regKey;
	wstring wstrKeyName = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\美图秀秀";
	if (regKey.Open(HKEY_LOCAL_MACHINE, wstrKeyName.c_str(), KEY_QUERY_VALUE) == ERROR_SUCCESS)	// 打开成功
	{
		wchar_t wszValue[256] = {0};
		ULONG len = 256;
		if (regKey.QueryStringValue(L"InstallPath", wszValue, &len) == ERROR_SUCCESS)
		{
			wstring wstrExePath = wszValue;
			wstrExePath.append(L"\\XiuXiu\\XiuXiu.exe");
			if (PathFileExists(wstrExePath.c_str()))
			{
				string strExePath;
				//xl::text::transcode::Unicode_to_UTF8(wstrExePath.c_str(), wstrExePath.size(), strExePath);
				strExePath = ultra::_T2UTF(wstrExePath);
				lua_pushstring(luaState, strExePath.c_str());
				regKey.Close();
				return 1;
			}
		}
		regKey.Close();
	}
	return 0;
}

int CLuaAppObject::GetNikonNx2ExePath(lua_State* luaState)
{	
	CRegKey regKey;
	wstring wstrKeyName = L"SOFTWARE\\Nikon\\Capture NX 2";
	if (regKey.Open(HKEY_LOCAL_MACHINE, wstrKeyName.c_str(), KEY_QUERY_VALUE) == ERROR_SUCCESS)	// 打开成功
	{
		wchar_t wszValue[256] = {0};
		ULONG len = 256;
		if (regKey.QueryStringValue(L"InstallDir", wszValue, &len) == ERROR_SUCCESS)
		{
			wstring wstrExePath = wszValue;
			wstrExePath.append(L"\\Capture NX 2.exe");
			if (PathFileExists(wstrExePath.c_str()))
			{
				string strExePath;
				//xl::text::transcode::Unicode_to_UTF8(wstrExePath.c_str(), wstrExePath.size(), strExePath);
				strExePath = ultra::_T2UTF(wstrExePath);
				lua_pushstring(luaState, strExePath.c_str());
				regKey.Close();
				return 1;
			}
		}
		regKey.Close();
	}
	return 0;
}

int CLuaAppObject::GetDPPExePath(lua_State* luaState)
{
	return 0;
}

int CLuaAppObject::GetLightRoomExePath(lua_State* luaState)
{
	CRegKey regKey;
	wstring wstrKeyName = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{FC5A8E68-A2E5-4E14-91FA-7A3FB83C7E23}";
	if (regKey.Open(HKEY_LOCAL_MACHINE, wstrKeyName.c_str(), KEY_QUERY_VALUE) == ERROR_SUCCESS)	// 打开成功
	{
		wchar_t wszValue[256] = {0};
		ULONG len = 256;
		if (regKey.QueryStringValue(L"InstallLocation", wszValue, &len) == ERROR_SUCCESS)
		{
			wstring wstrExePath = wszValue;
			wstrExePath.append(L"lightroom.exe");
			if (PathFileExists(wstrExePath.c_str()))
			{
				string strExePath;
				//xl::text::transcode::Unicode_to_UTF8(wstrExePath.c_str(), wstrExePath.size(), strExePath);
				strExePath = ultra::_T2UTF(wstrExePath);
				lua_pushstring(luaState, strExePath.c_str());
				regKey.Close();
				return 1;
			}
		}
		regKey.Close();
	}
	else	// 从其他地方找
	{
		wstring wstrKeyName = L"Adobe.AdobeLightroom\\shell\\open\\command";
		if (regKey.Open(HKEY_CLASSES_ROOT, wstrKeyName.c_str(), KEY_QUERY_VALUE) == ERROR_SUCCESS)	// 打开成功
		{
			wchar_t wszValue[512] = {0};
			ULONG nBytes = 512;
			if(regKey.QueryStringValue(NULL, wszValue, &nBytes) == ERROR_SUCCESS)
			{
				wstring wstrExePath = wszValue;
				wstring::size_type nPos = wstrExePath.find(L".exe");
				if (nPos != wstring::npos)
				{
					if (wszValue[0] == L'\"')
					{
						wstrExePath = wstrExePath.substr(1, nPos+3);
					}
					else
					{
						wstrExePath = wstrExePath.substr(0, nPos+4);
					}
					// 判断是否存在
					if (PathFileExists(wstrExePath.c_str()))
					{
						string strExePath;
						//xl::text::transcode::Unicode_to_UTF8(wstrExePath.c_str(), wstrExePath.size(), strExePath);
						strExePath = ultra::_T2UTF(wstrExePath);
						lua_pushstring(luaState, strExePath.c_str());
						regKey.Close();
						return 1;
					}
				}
			}
			regKey.Close();
		}
	}
	return 0;
}

int CLuaAppObject::GetPhotoShopExePath(lua_State* luaState)
{
	CRegKey regKey;
	wstring wstrKeyName = L"SOFTWARE\\Adobe\\Photoshop";
	wstring wstrCurKey = L"1.0";
	wstring wstrDesExePath;
	if (regKey.Open(HKEY_LOCAL_MACHINE, wstrKeyName.c_str(), KEY_ENUMERATE_SUB_KEYS) == ERROR_SUCCESS)	// 打开成功
	{
		int nIndex = 0;
		wchar_t wszKeyName[100] = {0};
		DWORD dwKeyNameBytes = 100;
		while(regKey.EnumKey(nIndex, wszKeyName, &dwKeyNameBytes) == ERROR_SUCCESS)
		{
			nIndex++;
			// 做比较
			if (StrCmpLogicalW(wstrCurKey.c_str(), wszKeyName) == -1)
			{
				wstring wstrNewKeyName = wstrKeyName + L"\\" + wszKeyName;
				CRegKey newKey;
				if(newKey.Open(HKEY_LOCAL_MACHINE, wstrNewKeyName.c_str(), KEY_QUERY_VALUE) == ERROR_SUCCESS)
				{
					wchar_t wszValue[256];
					ULONG len = 256;
					if (newKey.QueryStringValue(L"ApplicationPath", wszValue, &len) == ERROR_SUCCESS)
					{
						PathAppend(wszValue, L"Photoshop.exe");
						wstring wstrExePath = wszValue;
						if (PathFileExists(wstrExePath.c_str()))
						{
							wstrDesExePath = wstrExePath;
							wstrCurKey = wszKeyName;
						}
					}
					newKey.Close();
				}
			}
			
		}
		regKey.Close();
	}
	if (!wstrDesExePath.empty())
	{
		string strExePath;
		//xl::text::transcode::Unicode_to_UTF8(wstrDesExePath.c_str(), wstrDesExePath.size(), strExePath);
		strExePath = ultra::_T2UTF(wstrDesExePath);
		lua_pushstring(luaState, strExePath.c_str());
		return 1;
	}
	return 0;
}

int CLuaAppObject::GetKeNiuExePath(lua_State* luaState)
{
	CRegKey regKey;
	wstring wstrKeyName = L"Software\\Conew";
	if (regKey.Open(HKEY_CURRENT_USER, wstrKeyName.c_str(), KEY_QUERY_VALUE) == ERROR_SUCCESS)	// 打开成功
	{
		wchar_t wszValue[256] = {0};
		ULONG len = 256;
		if (regKey.QueryStringValue(L"Install Path", wszValue, &len) == ERROR_SUCCESS)
		{
			wstring wstrExePath = wszValue;
			wstrExePath.append(L"\\conew.exe");
			if (PathFileExists(wstrExePath.c_str()))
			{
				string strExePath;
				//xl::text::transcode::Unicode_to_UTF8(wstrExePath.c_str(), wstrExePath.size(), strExePath);
				strExePath = ultra::_T2UTF(wstrExePath);
				lua_pushstring(luaState, strExePath.c_str());
				regKey.Close();
				return 1;
			}
		}
		regKey.Close();
	}
	return 0;
}

int CLuaAppObject::GetOldNeoImagingPath(lua_State* luaState)
{
	CRegKey regKey;
	wstring wstrKeyName = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\光影魔术手_is1";
	if (regKey.Open(HKEY_LOCAL_MACHINE, wstrKeyName.c_str(), KEY_QUERY_VALUE) == ERROR_SUCCESS)	// 打开成功
	{
		wchar_t wszValue[256] = {0};
		ULONG len = 256;
		if (regKey.QueryStringValue(L"InstallLocation", wszValue, &len) == ERROR_SUCCESS)
		{
			wstring wstrExePath = wszValue;
			wstrExePath.append(L"nEOiMAGING.exe");
			if (PathFileExists(wstrExePath.c_str()))
			{
				string strExePath;
				//xl::text::transcode::Unicode_to_UTF8(wstrExePath.c_str(), wstrExePath.size(), strExePath);
				strExePath = ultra::_T2UTF(wstrExePath);
				lua_pushstring(luaState, strExePath.c_str());
				regKey.Close();
				return 1;
			}
		}
		regKey.Close();
	}
	else	// 从其他地方找
	{
		wstring wstrKeyName = L"nEOiMGAGING.jpg\\shell\\Open\\command";
		if (regKey.Open(HKEY_CLASSES_ROOT, wstrKeyName.c_str(), KEY_QUERY_VALUE) == ERROR_SUCCESS)	// 打开成功
		{
			wchar_t wszValue[512] = {0};
			ULONG nBytes = 512;
			if(regKey.QueryStringValue(NULL, wszValue, &nBytes) == ERROR_SUCCESS)
			{
				wstring wstrExePath = wszValue;
				wstring::size_type nPos = wstrExePath.find(L".exe");
				if (nPos != wstring::npos)
				{
					if (wszValue[0] == L'\"')
					{
						wstrExePath = wstrExePath.substr(1, nPos+3);
					}
					else
					{
						wstrExePath = wstrExePath.substr(0, nPos+4);
					}
					// 判断是否存在
					if (PathFileExists(wstrExePath.c_str()))
					{
						string strExePath;
						//xl::text::transcode::Unicode_to_UTF8(wstrExePath.c_str(), wstrExePath.size(), strExePath);
						strExePath = ultra::_T2UTF(wstrExePath);
						lua_pushstring(luaState, strExePath.c_str());
						regKey.Close();
						return 1;
					}
				}
			}
			regKey.Close();
		}
	}
	return 0;
}

int CLuaAppObject::GetIconFromExeFile(lua_State* luaState)
{
	const char* utf8Str = luaL_checkstring(luaState, 2);
	wstring wstrExePath;
	//xl::text::transcode::UTF8_to_Unicode(utf8Str, strlen(utf8Str), wstrExePath);
	wstrExePath = ultra::_UTF2T(utf8Str);
	HICON hIcon = ::ExtractIcon(_Module.GetModuleInstance(), wstrExePath.c_str(), 2);
	if (hIcon != NULL)
	{
		HBITMAP hBitmap;
		BITMAP   bmp;
		CDC   bmpDC, iconDC;
		ICONINFO iconInfo;

		BOOL bRet = ::GetIconInfo(hIcon, &iconInfo);
		if (bRet)
		{
			bmpDC.CreateCompatibleDC(NULL);
			iconDC.CreateCompatibleDC(bmpDC); 
			if (::GetObject(iconInfo.hbmColor,   sizeof(BITMAP),   &bmp))
			{
				DWORD dwWidth = iconInfo.xHotspot*2;     
				DWORD dwHeight = iconInfo.yHotspot*2; 
				hBitmap = ::CreateBitmap(dwWidth, dwHeight, bmp.bmPlanes, bmp.bmBitsPixel, NULL);
				if (hBitmap)
				{
					iconDC.SelectBitmap(iconInfo.hbmColor);     
					bmpDC.SelectBitmap(hBitmap);
					bmpDC.BitBlt(0, 0, dwWidth, dwHeight, iconDC, 0, 0, SRCCOPY);
					XL_BITMAP_HANDLE hXLBitmap = CImageProcessor::ConvertDIBToXLBitmap32(hBitmap);
					// 关闭
					//DeleteObject(hBitmap);

					if (hXLBitmap)
					{
						XL_PreMultiplyBitmap(hXLBitmap);
						XLGP_PushBitmap(luaState, hXLBitmap);
						XL_ReleaseBitmap(hXLBitmap);
						XLBitmapInfo bmpInfo;
						XL_GetBitmapInfo(hXLBitmap, &bmpInfo);
						lua_pushnumber(luaState, bmpInfo.Width);
						lua_pushnumber(luaState, bmpInfo.Height);
						return 3;
					}
				}
			}
		}
		::DestroyIcon(hIcon);
	}
	return 0;
}

int CLuaAppObject::IsCanSetToWallPaperFile(lua_State* luaState)
{
	bool bRet = false;
	const char* utf8FilePath = luaL_checkstring(luaState, 2);
	wstring wstrFilePath;
	//xl::text::transcode::UTF8_to_Unicode(utf8FilePath, strlen(utf8FilePath), wstrFilePath);
	wstrFilePath = ultra::_UTF2T(utf8FilePath);
	wstring wstrExtName = PathFindExtensionW(wstrFilePath.c_str());
	if (!wstrExtName.empty())
	{
		wstring lowerExtName = ultra::ToLower(wstrExtName);
		lowerExtName.append(L";");
		wstring wstrCanSuperBatchFileExtList = CanSetToWallPaperFileExt;
		if(wstrCanSuperBatchFileExtList.find(lowerExtName.c_str()) != wstring::npos)
		{
			bRet = true;
		}
	}
	lua_pushboolean(luaState, bRet);
	return 1;
}

int CLuaAppObject::CopyTextToClipboard(lua_State* luaState)
{
	TSAUTO();
	std::string strClipText = lua_tostring(luaState, 2);

	std::wstring wstrClipText;
	//xl::text::transcode::UTF8_to_Unicode(strClipText.c_str(), strClipText.size(), wstrClipText);
	wstrClipText = ultra::_UTF2T(strClipText);
	TSINFO4CXX("wstrClipText:"<<wstrClipText<<"wstrClipText.size():"<<wstrClipText.size());

	HGLOBAL hDIB = ::GlobalAlloc(GHND | GMEM_SHARE, (wstrClipText.size() + 1) * sizeof(wchar_t));
	if (hDIB == NULL)
	{
		TSINFO4CXX("GlobalAlloc Failed!");
		return 0;
	}

	wchar_t* lpBuffer = (wchar_t*)::GlobalLock(hDIB);
	assert(lpBuffer);
	if (lpBuffer == NULL)
	{
		TSINFO4CXX("GlobalLock Failed!");
		::GlobalFree(hDIB);
		return 0;
	}

	lstrcpy(lpBuffer, wstrClipText.c_str());

	::GlobalUnlock(hDIB);

	BOOL ret = ::OpenClipboard(NULL);
	if (ret == FALSE)
	{
		TSINFO4CXX("OpenClipboard Failed! Error:"<<::GetLastError());
		return 0;
	}
	::EmptyClipboard();

	HANDLE hHandle = ::SetClipboardData(CF_UNICODETEXT, hDIB);
	TSINFO4CXX("hHandle:"<<hHandle);
	if (hHandle == NULL)
	{
		TSINFO4CXX("SetClipboardData Failed! Error:"<<::GetLastError());
		return 0;
	}

	::CloseClipboard();
	return 0;
}

int CLuaAppObject::ShowColorDialog(lua_State* luaState)
{
	TSAUTO();
	int nInitRed = luaL_checkint(luaState, 2);
	int nInitGreen = luaL_checkint(luaState, 3);
	int nInitBlue = luaL_checkint(luaState, 4);

	HWND parentWnd = (HWND)lua_touserdata(luaState, 5);

	COLORREF color = RGB(nInitRed, nInitGreen, nInitBlue);
	CColorDialog cdlg(color, CC_FULLOPEN | CC_RGBINIT, parentWnd);

	if(cdlg.DoModal() == IDOK)
	{
		color = cdlg.GetColor();

		int nRed = GetRValue(color);
		int nGreen = GetGValue(color);
		int nBlue = GetBValue(color);

		lua_pushinteger(luaState, nRed);
		lua_pushinteger(luaState, nGreen);
		lua_pushinteger(luaState, nBlue);

		return 3;
	}
	else
	{
		//如果点击的是取消，那么什么也不做
		return 0;
	}
}

int CLuaAppObject::SetShowCursor( lua_State* luaState )
{
	bool bShow = LuaGetBool(luaState,2);
	ShowCursor(bShow);
	return 0;
}
