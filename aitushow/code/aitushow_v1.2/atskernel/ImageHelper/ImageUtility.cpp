#include "StdAfx.h"
#include "ImageUtility.h"
#include "zlib/zlib.h"
#include <atlsecurity.h>
#include <Icm.h>
#include "../Utility/StringOperation.h"

//#define  CanHandleFileExt	L".jpg;.jpeg;.jpe;.png;.bmp;.gif;.tiff;.tif;.psd;.ico;.nef;.cr2;.crw;.arw;.dng;.pef;.srf;.raf;.orf;.rw2;.pcx;.pef;.dcr;.tga;.wmf;.jbg;.hdr;.jpc;.wbm;.pgx;.ras;.mng;.ska;"				// 可以处理的文件后缀名
//1版本先支持这些，后面的版本再改
#define  CanHandleFileExt   L".jpg;.jpeg;.jpe;.bmp;.png;.gif;.tiff;.tif;.psd;.ico;.pcx;.tga;.wbm;.ras;.mng;.hdr;.cr2;.nef;.arw;.dng;.srf;.raf;.wmf;.ras;.mng;"
#define  CanSetToWallPaperFileExt	L".jpg;.jpeg;.png;.bmp;"		// 可以设为壁纸的文件后缀名
#define  CanSuperBatchFileExt	L".jpg;.jpeg;.png;.bmp;"			// 可以做高级批处理的文件后缀名
#define  CanBatchRotateFileExt	L".jpg;.jpeg;.png;.bmp;"			// 可以批量左右旋的文件后缀名
#define  CanSaveToSameTypeFileExt	L".jpg;.jpeg;.png;.bmp;"		// 可以保存为相同类型文件的后缀名

// jpg文件保存质量
int level[] = {20, 30, 40, 50, 60, 70, 80, 83, 85, 87, 89, 91,92, 93, 94, 95, 96, 97, 98, 99, 100};

CImageUtility::CImageUtility(void)
{
}

CImageUtility::~CImageUtility(void)
{
}
bool CImageUtility::DeleteDir(const std::wstring& wstrTempDirect)
{ 
	bool bRet = true;
	if(true == wstrTempDirect.empty()) 
	{
		return false;
	}
	std::wstring wstrDirctory = wstrTempDirect;
	if (wstrTempDirect[wstrTempDirect.size() - 1] != TEXT('\\'))
	{
		wstrDirctory +=TEXT('\\');
	}
	std::wstring wstrFiles = wstrDirctory + TEXT("*.*");

	WIN32_FIND_DATA   FindFileData;
	HANDLE hFind = ::FindFirstFile(wstrFiles.c_str(), &FindFileData);

	while(hFind!=INVALID_HANDLE_VALUE)   
	{
		if( _wcsicmp(FindFileData.cFileName,L".") !=0 && _wcsicmp(FindFileData.cFileName,L"..") !=0 )
		{   
			std::wstring wstrDirChild = wstrDirctory + FindFileData.cFileName;
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			{
				bRet = DeleteDir(wstrDirChild.c_str());
				if (!bRet)
				{
					bRet = false;
				}
			}
			else
			{ 
				SetFileAttributesW(wstrDirChild.c_str(),FILE_ATTRIBUTE_NORMAL);
				if(::DeleteFile(wstrDirChild.c_str()))
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

	if(bRet)
	{
		if (::RemoveDirectoryW(wstrTempDirect.c_str()))
		{
			return true;
		}
	}
	return false;
}

bool CImageUtility::IsCanHandleFileCheckByExt(const std::wstring& wstrTempDirect)
{
	wstring wstrExtName = PathFindExtensionW(wstrTempDirect.c_str());
	if (wstrExtName.empty())
	{
		return false;
	}
	wstring lowerExtName = ultra::ToLower(wstrExtName);
	lowerExtName.append(L";");
	wstring wstrCanSuperBatchFileExtList = CanHandleFileExt;
	if(wstrCanSuperBatchFileExtList.find(lowerExtName.c_str()) == wstring::npos)
	{
		return false;
	}
	return true;
}

bool CImageUtility::IsCanSetToWallPaperFile(const std::wstring& wstrTempDirect)
{
	wstring wstrExtName = PathFindExtensionW(wstrTempDirect.c_str());
	if (wstrExtName.empty())
	{
		return false;
	}
	wstring lowerExtName =ultra::ToLower(wstrExtName);
	lowerExtName.append(L";");
	wstring wstrCanSuperBatchFileExtList = CanSetToWallPaperFileExt;
	if(wstrCanSuperBatchFileExtList.find(lowerExtName.c_str()) == wstring::npos)
	{
		return false;
	}
	return true;
}
bool CImageUtility::IsCanBatchRotateFile(const std::wstring& wstrTempDirect)
{
	wstring wstrExtName = PathFindExtensionW(wstrTempDirect.c_str());
	if (wstrExtName.empty())
	{
		return false;
	}
	wstring lowerExtName =ultra::ToLower(wstrExtName);
	lowerExtName.append(L";");
	wstring wstrCanSuperBatchFileExtList = CanBatchRotateFileExt;
	if(wstrCanSuperBatchFileExtList.find(lowerExtName.c_str()) == wstring::npos)
	{
		return false;
	}
	return true;
}
bool CImageUtility::IsCanSuperBatchFile(const std::wstring& wstrTempDirect)
{
	wstring wstrExtName = PathFindExtensionW(wstrTempDirect.c_str());
	if (wstrExtName.empty())
	{
		return false;
	}
	wstring lowerExtName = ultra::ToLower(wstrExtName);
	lowerExtName.append(L";");
	wstring wstrCanSuperBatchFileExtList = CanSuperBatchFileExt;
	if(wstrCanSuperBatchFileExtList.find(lowerExtName.c_str()) == wstring::npos)
	{
		return false;
	}
	return true;
}

bool CImageUtility::IsCanSaveToSameTypeFile(const std::wstring& wstrTempDirect)
{
	wstring wstrExtName = PathFindExtensionW(wstrTempDirect.c_str());
	if (wstrExtName.empty())
	{
		return false;
	}
	wstring lowerExtName = ultra::ToLower(wstrExtName);
	lowerExtName.append(L";");
	wstring wstrCanSuperBatchFileExtList = CanSaveToSameTypeFileExt;
	if(wstrCanSuperBatchFileExtList.find(lowerExtName.c_str()) == wstring::npos)
	{
		return false;
	}
	return true;
}
wstring CImageUtility::LowerStr(wstring& wstrSrcStr)
{
	wchar_t *p = new wchar_t[wstrSrcStr.size()+1];
	p[wstrSrcStr.size()] = 0;
	for (unsigned int i=0; i<wstrSrcStr.size(); i++)
	{
		if (wstrSrcStr[i]>64 && wstrSrcStr[i]<91)
		{
			p[i] = wstrSrcStr[i]+32;
		}
		else
		{
			p[i] = wstrSrcStr[i];
		}
	}
	wstring temp = p;
	delete[] p;
	return temp;
}

BOOL CImageUtility::IsLegalPath(std::wstring& wstrPathName)
{
	TSAUTO();

	size_t stPos1 = wstrPathName.find_first_of(L':');
	size_t stPos2 = wstrPathName.find_first_of(L'\\');

	BOOL bIsLegal = TRUE;
	if (1 != stPos1 || 2 != stPos2 || 3 > wstrPathName.length() || wstrPathName.find(_T("//")) != std::wstring::npos || wstrPathName.find(_T("\\\\")) != std::wstring::npos
		|| wstrPathName.find(_T("/\\")) != std::wstring::npos || wstrPathName.find(_T("\\/")) != std::wstring::npos)
	{
		bIsLegal = FALSE;
	}
	if (wstrPathName.size() > 150)
	{
		bIsLegal = FALSE;
	}
	if (TRUE == bIsLegal)
	{
		TCHAR* szUnLegalChars = L"/:*?\"<>|";
		std::wstring wstrPathNameWithoutHead = wstrPathName.substr(2);
		for (int i = 0; i < 8; ++i)
		{
			if (wstrPathNameWithoutHead.npos != wstrPathNameWithoutHead.find_first_of(szUnLegalChars[i]))
			{
				bIsLegal = FALSE;
				break;
			}
		}
	}

	return bIsLegal;
}

wstring& replace_all_distinct(wstring& str, const wstring& old_value, const wstring& new_value) 
{   
	for(wstring::size_type pos(0); pos != wstring::npos; pos += new_value.length())
	{   
		if((pos = str.find(old_value,pos)) != wstring::npos)
		{
			str.replace(pos, old_value.length(), new_value);
		}
		else
		{
			break;
		}
	}   
	return str;   
}

BOOL CImageUtility::GetOSInfo(std::wstring &strOSDesc, std::wstring &strOSVersion)
{
	#define PRODUCT_ULTIMATE 0x00000001
	#define PRODUCT_HOME_BASIC 0x00000002
	#define PRODUCT_HOME_PREMIUM 0x00000003
	#define PRODUCT_ENTERPRISE 0x00000004
	#define PRODUCT_BUSINESS 0x00000006
	#define PRODUCT_STANDARD_SERVER 0x00000007
	#define PRODUCT_DATACENTER_SERVER 0x00000008
	#define PRODUCT_SMALLBUSINESS_SERVER 0x00000009
	#define PRODUCT_ENTERPRISE_SERVER 0x0000000A
	#define PRODUCT_STARTER 0x0000000B
	#define PRODUCT_DATACENTER_SERVER_CORE 0x0000000C
	#define PRODUCT_STANDARD_SERVER_CORE 0x0000000D
	#define PRODUCT_ENTERPRISE_SERVER_CORE 0x0000000E
	#define PRODUCT_ENTERPRISE_SERVER_IA64 0x0000000F
	#define PRODUCT_WEB_SERVER 0x00000011
	#define PRODUCT_CLUSTER_SERVER 0x00000012
	#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM 0x00000019
	#define PRODUCT_PROFESSIONAL 0x00000030

	#define SM_SERVERR2 89
	#define VER_SUITE_STORAGE_SERVER 0x00002000
	#define VER_SUITE_COMPUTE_SERVER 0x00004000
	#define VER_SUITE_WH_SERVER 0x00008000

	typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
	typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	PGPI pGPI;
	BOOL bOsVersionInfoEx;
	DWORD dwType;
	WCHAR pszOS[BUFSIZ] = {0}, pszOSVersion[BUFSIZ] = {0};

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			return FALSE;
	}

	// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
	pGNSI = (PGNSI)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetNativeSystemInfo");
	if(NULL != pGNSI)
	{
		pGNSI(&si);
	}
	else 
	{
		GetSystemInfo(&si);
	}

	if ( VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion > 4 )
	{
		StringCchCopyW(pszOS, BUFSIZ, L"Microsoft ");

		// Test for the specific product.

		if ( osvi.dwMajorVersion == 6 )
		{
			if( osvi.dwMinorVersion == 0 )
			{
				if( osvi.wProductType == VER_NT_WORKSTATION )
					StringCchCatW(pszOS, BUFSIZ, L"Windows Vista ");
				else 
					StringCchCatW(pszOS, BUFSIZ, L"Windows Server 2008 ");
			}

			if ( osvi.dwMinorVersion == 1 )
			{
				if( osvi.wProductType == VER_NT_WORKSTATION )
					StringCchCatW(pszOS, BUFSIZ, L"Windows 7 ");
				else 
					StringCchCatW(pszOS, BUFSIZ, L"Windows Server 2008 R2 ");
			}

			pGPI = (PGPI)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetProductInfo");
			pGPI( osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

			switch( dwType )
			{
			case PRODUCT_ULTIMATE:
				StringCchCatW(pszOS, BUFSIZ, L"Ultimate Edition");
				break;
			case PRODUCT_PROFESSIONAL:
				StringCchCatW(pszOS, BUFSIZ, L"Professional");
				break;
			case PRODUCT_HOME_PREMIUM:
				StringCchCatW(pszOS, BUFSIZ, L"Home Premium Edition");
				break;
			case PRODUCT_HOME_BASIC:
				StringCchCatW(pszOS, BUFSIZ, L"Home Basic Edition");
				break;
			case PRODUCT_ENTERPRISE:
				StringCchCatW(pszOS, BUFSIZ, L"Enterprise Edition");
				break;
			case PRODUCT_BUSINESS:
				StringCchCatW(pszOS, BUFSIZ, L"Business Edition");
				break;
			case PRODUCT_STARTER:
				StringCchCatW(pszOS, BUFSIZ, L"Starter Edition");
				break;
			case PRODUCT_CLUSTER_SERVER:
				StringCchCatW(pszOS, BUFSIZ, L"Cluster Server Edition");
				break;
			case PRODUCT_DATACENTER_SERVER:
				StringCchCatW(pszOS, BUFSIZ, L"Datacenter Edition");
				break;
			case PRODUCT_DATACENTER_SERVER_CORE:
				StringCchCatW(pszOS, BUFSIZ, L"Datacenter Edition (core installation)");
				break;
			case PRODUCT_ENTERPRISE_SERVER:
				StringCchCatW(pszOS, BUFSIZ, L"Enterprise Edition");
				break;
			case PRODUCT_ENTERPRISE_SERVER_CORE:
				StringCchCatW(pszOS, BUFSIZ, L"Enterprise Edition (core installation)");
				break;
			case PRODUCT_ENTERPRISE_SERVER_IA64:
				StringCchCatW(pszOS, BUFSIZ, L"Enterprise Edition for Itanium-based Systems");
				break;
			case PRODUCT_SMALLBUSINESS_SERVER:
				StringCchCatW(pszOS, BUFSIZ, L"Small Business Server");
				break;
			case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
				StringCchCatW(pszOS, BUFSIZ, L"Small Business Server Premium Edition");
				break;
			case PRODUCT_STANDARD_SERVER:
				StringCchCatW(pszOS, BUFSIZ, L"Standard Edition");
				break;
			case PRODUCT_STANDARD_SERVER_CORE:
				StringCchCatW(pszOS, BUFSIZ, L"Standard Edition (core installation)");
				break;
			case PRODUCT_WEB_SERVER:
				StringCchCatW(pszOS, BUFSIZ, L"Web Server Edition");
				break;
			}
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
		{
			if( GetSystemMetrics(SM_SERVERR2) )
				StringCchCatW(pszOS, BUFSIZ, L"Windows Server 2003 R2, ");
			else if ( osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER )
				StringCchCatW(pszOS, BUFSIZ, L"Windows Storage Server 2003");
			else if ( osvi.wSuiteMask & VER_SUITE_WH_SERVER )
				StringCchCatW(pszOS, BUFSIZ, L"Windows Home Server");
			else if( osvi.wProductType == VER_NT_WORKSTATION &&
				si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
			{
				StringCchCatW(pszOS, BUFSIZ, L"Windows XP Professional x64 Edition");
			}
			else StringCchCatW(pszOS, BUFSIZ, L"Windows Server 2003, ");

			// Test for the server type.
			if ( osvi.wProductType != VER_NT_WORKSTATION )
			{
				if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						StringCchCatW(pszOS, BUFSIZ, L"Datacenter Edition for Itanium-based Systems");
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						StringCchCatW(pszOS, BUFSIZ, L"Enterprise Edition for Itanium-based Systems");
				}

				else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						StringCchCatW(pszOS, BUFSIZ, L"Datacenter x64 Edition");
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						StringCchCatW(pszOS, BUFSIZ, L"Enterprise x64 Edition");
					else StringCchCatW(pszOS, BUFSIZ, L"Standard x64 Edition");
				}

				else
				{
					if ( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
						StringCchCatW(pszOS, BUFSIZ, L"Compute Cluster Edition");
					else if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						StringCchCatW(pszOS, BUFSIZ, L"Datacenter Edition");
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						StringCchCatW(pszOS, BUFSIZ, L"Enterprise Edition");
					else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
						StringCchCatW(pszOS, BUFSIZ, L"Web Edition");
					else StringCchCatW(pszOS, BUFSIZ, L"Standard Edition");
				}
			}
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
		{
			StringCchCatW(pszOS, BUFSIZ, L"Windows XP ");
			if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
				StringCchCatW(pszOS, BUFSIZ, L"Home Edition");
			else StringCchCatW(pszOS, BUFSIZ, L"Professional");
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
		{
			StringCchCatW(pszOS, BUFSIZ, L"Windows 2000 ");

			if ( osvi.wProductType == VER_NT_WORKSTATION )
			{
				StringCchCatW(pszOS, BUFSIZ, L"Professional");
			}
			else 
			{
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					StringCchCatW(pszOS, BUFSIZ, L"Datacenter Server");
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					StringCchCatW(pszOS, BUFSIZ, L"Advanced Server");
				else StringCchCatW(pszOS, BUFSIZ, L"Server");
			}
		}


		// Include service pack (if any) and build number.
		if( _tcslen(osvi.szCSDVersion) > 0 )
		{
			StringCchCatW(pszOS, BUFSIZ, L" ");
			StringCchCatW(pszOS, BUFSIZ, osvi.szCSDVersion);
		}

		WCHAR buf[80];
		StringCchPrintfW(buf, 80, L" (build %d)", osvi.dwBuildNumber);
		StringCchCatW(pszOS, BUFSIZ, buf);

		if ( osvi.dwMajorVersion >= 6 )
		{
			if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
				StringCchCatW(pszOS, BUFSIZ, L", 64-bit");
			else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL )
				StringCchCatW(pszOS, BUFSIZ, L", 32-bit");
		}

		swprintf(pszOSVersion, L"%d.%d.%d.%d %s", osvi.dwMajorVersion, osvi.dwMinorVersion, 
			osvi.dwBuildNumber, osvi.dwPlatformId, osvi.szCSDVersion);

		strOSDesc = pszOS;
		strOSVersion = pszOSVersion;
		return TRUE; 
	}

	return FALSE;
}

BOOL CImageUtility::CreateMediumIntegrityProcess(PCTSTR pszApplicationName, PTSTR pszCommandLine, PPROCESS_INFORMATION pPI, BOOL bShowWnd)
{
	BOOL bRet = FALSE;

	CAccessToken ProcToken;
	CAccessToken PrimaryToken;

	PSID pSid = NULL;

	STARTUPINFO si = { sizeof(si) };

	if (!ProcToken.GetEffectiveToken(TOKEN_DUPLICATE | TOKEN_ADJUST_DEFAULT | TOKEN_QUERY | TOKEN_ASSIGN_PRIMARY))
	{
		goto FUNC_EXIT;
	}

	if (!ProcToken.CreatePrimaryToken(&PrimaryToken))
	{
		goto FUNC_EXIT;
	}

	TCHAR szIntegritySid[20] = _T("S-1-16-8192");
	ConvertStringSidToSid(szIntegritySid, &pSid);

	TOKEN_MANDATORY_LABEL TIL;
	TIL.Label.Attributes = SE_GROUP_INTEGRITY;
	TIL.Label.Sid        = pSid;
	if (!SetTokenInformation(PrimaryToken.GetHandle(), (TOKEN_INFORMATION_CLASS)TokenIntegrityLevel, &TIL, sizeof(TOKEN_MANDATORY_LABEL) + GetLengthSid(pSid)))
	{
		goto FUNC_EXIT;
	}

	GetStartupInfo(&si);
	si.dwFlags = si.dwFlags|STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	bRet = CreateProcessAsUser(PrimaryToken.GetHandle(), pszApplicationName, pszCommandLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, pPI);

FUNC_EXIT:

	if (pSid != NULL)
	{
		LocalFree(pSid);
	}

	if (!bRet)
	{
		bRet = CreateProcess(pszApplicationName, pszCommandLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, pPI);
	}

	return bRet;
}
BOOL CImageUtility::PrintImage(wchar_t* path, wchar_t* file_name )
{
	IShellFolder* pDesktop = NULL;
	HRESULT hr =  ::SHGetDesktopFolder( &pDesktop );
	if( !SUCCEEDED( hr ) )
		return FALSE;

	LPITEMIDLIST pidl = NULL;
	hr = pDesktop->ParseDisplayName(NULL, NULL, path, NULL, &pidl, NULL);
	if( !SUCCEEDED( hr ) )
	{
		pDesktop->Release();
		return FALSE;
	}

	IShellFolder* pPath = NULL;
	hr = pDesktop->BindToObject(pidl, NULL, IID_IShellFolder, (void**)(&pPath) );
	if( !SUCCEEDED( hr ) )
	{
		pDesktop->Release();
		CoTaskMemFree( pidl );
		return FALSE;
	}

	LPITEMIDLIST pidl2 = NULL;
	hr = pPath->ParseDisplayName(NULL, NULL, file_name, NULL, &pidl2, NULL);
	if( !SUCCEEDED( hr ) )
	{
		pDesktop->Release();
		CoTaskMemFree( pidl );
		pPath->Release();
		return FALSE;
	}

	IContextMenu* contextMenu_ptr = NULL;
	hr = pPath->GetUIObjectOf( GetActiveWindow(), 1, (LPCITEMIDLIST*)(&pidl2), IID_IContextMenu, NULL,(void**)(&contextMenu_ptr) );
	if( !SUCCEEDED( hr ) )
	{
		pDesktop->Release();
		CoTaskMemFree( pidl );
		pPath->Release();
		CoTaskMemFree( pidl2 );
		return FALSE;
	}

	HMENU hMenu = CreatePopupMenu();
	if( hMenu == NULL )
	{
		pDesktop->Release();
		CoTaskMemFree( pidl );
		pPath->Release();
		CoTaskMemFree( pidl2 );
		contextMenu_ptr->Release();
		return FALSE;
	}
	contextMenu_ptr->QueryContextMenu( hMenu, 0, 1, 0x7FFF, CMF_NORMAL );
	CMINVOKECOMMANDINFO cmdInfo;
	::memset( &cmdInfo, 0, sizeof(CMINVOKECOMMANDINFO) );
	cmdInfo.cbSize = sizeof(CMINVOKECOMMANDINFO);
	cmdInfo.lpVerb = "print";//"properties";"Print"
	cmdInfo.nShow = SW_SHOW;
	cmdInfo.hwnd = GetActiveWindow();
	hr = contextMenu_ptr->InvokeCommand( &cmdInfo );
	pDesktop->Release();
	CoTaskMemFree( pidl );
	pPath->Release();
	CoTaskMemFree( pidl2 );
	contextMenu_ptr->Release();
	DestroyMenu( hMenu );
	if( SUCCEEDED( hr ) )
		return TRUE;
	else
		return FALSE;
}

wstring CImageUtility::GetFileDesInfo(wstring& wstrFilePath)
{
	wstring wstrDes;
	HRESULT hr;
	unsigned int nsize = GetFileVersionInfoSize(wstrFilePath.c_str(), 0);
	if ( nsize )
	{
		char *pdata = new char[nsize + 1];
		if ( pdata )
		{
			if ( GetFileVersionInfo(wstrFilePath.c_str(), 0, nsize, pdata) )
			{
				struct LANGANDCODEPAGE {
					WORD wLanguage;
					WORD wCodePage;
				} *lpTranslate;
				UINT cbTranslate;
				if ( VerQueryValue(pdata, L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate,  &cbTranslate))
				{
					if (cbTranslate/sizeof(struct LANGANDCODEPAGE) > 0 )
					{
						wchar_t SubBlock[50];
						hr = StringCchPrintf(SubBlock, 50, L"\\StringFileInfo\\%04x%04x\\FileDescription", lpTranslate[0].wLanguage,	lpTranslate[0].wCodePage);
						if (SUCCEEDED(hr))
						{
							wchar_t* wszDesStr = NULL;
							UINT dwBytes = 0;
							VerQueryValue(pdata,SubBlock, (LPVOID*)&wszDesStr, &dwBytes);
							if (wszDesStr)
							{
								wstrDes = wszDesStr;
							}
						}
					}
				}
			}
			delete[] pdata;	
		}
	}
	return wstrDes;
}


BOOL CImageUtility::IS_Vista_Or_More()
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
}

typedef BOOL (WINAPI *WcsGetDefaultColorProfileType)(
	__in      WCS_PROFILE_MANAGEMENT_SCOPE profileManagementScope,
	__in_opt  PCWSTR pDeviceName,
	__in      COLORPROFILETYPE cptColorProfileType,
	__in      COLORPROFILESUBTYPE cpstColorProfileSubType,
	__in      DWORD dwProfileID,
	__in      DWORD cbProfileName,
	__out     LPWSTR pProfileName
	);

WcsGetDefaultColorProfileType GetProfileFunctionAddress()
{
	HMODULE handle = ::LoadLibraryW( L"Mscms.dll" );
	if( handle == NULL )
		return NULL;
	return (WcsGetDefaultColorProfileType)::GetProcAddress( handle, "WcsGetDefaultColorProfile" );
}


BOOL CImageUtility::GetDisplayMonitorICC( wchar_t* full_path, int char_count )
{
	if( !IS_Vista_Or_More() )
		return FALSE;
	DISPLAY_DEVICE device_info;
	device_info.cb = sizeof( DISPLAY_DEVICE );
	int card_id=0;
	while( TRUE )
	{
		BOOL ret = EnumDisplayDevices( NULL, card_id, &device_info, 0 );
		if( !ret )
			break;
		if(device_info.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP )
		{
			int monitor_id=0;
			wchar_t card_name[256];
			::memcpy( card_name,device_info.DeviceName,32*2);
			ret = EnumDisplayDevices( card_name, monitor_id, &device_info, 0 );
			if( !ret )
				return FALSE;
			DWORD buf_size = char_count*2;
			ret = GetColorDirectory( NULL, full_path, &buf_size );
			if( !ret )
				return FALSE;
			if( char_count*2 < buf_size+10 )
				return FALSE;
			full_path[buf_size/2-1] = L'\\';
			WcsGetDefaultColorProfileType func_ptr = GetProfileFunctionAddress();
			if( func_ptr == NULL )
				return FALSE;
			ret = func_ptr(WCS_PROFILE_MANAGEMENT_SCOPE_CURRENT_USER,device_info.DeviceID,CPT_ICC,
				CPST_NONE, 0, char_count*2 - buf_size, full_path + buf_size/2 );
			return ret;
		}
		++card_id;
	}
	return FALSE;
}

wstring CImageUtility::GetDisplayMonitorICCFilePath()
{
	static wchar_t* pwszFilePath = NULL;
	if (pwszFilePath == NULL)
	{
		pwszFilePath = new wchar_t[MAX_PATH];
		wmemset(pwszFilePath, 0, MAX_PATH);
		if(!GetDisplayMonitorICC(pwszFilePath, MAX_PATH ))
		{
			wmemset(pwszFilePath, 0, MAX_PATH);
		}

	}
	return pwszFilePath;
}
