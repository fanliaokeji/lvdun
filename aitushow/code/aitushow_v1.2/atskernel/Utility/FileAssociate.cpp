#include "stdafx.h"
#include <atlbase.h>
#include <windows.h>
#include <ShlObj.h>
#include <string>
#include <comdef.h>
#include <ShellAPI.h>
#include "FileAssociation.h"

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
BOOL RegTool::IsWow64(){
	BOOL bIsWow64 = FALSE;
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")),"IsWow64Process");
	if (NULL != fnIsWow64Process){
		if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64)){
		}
	}
	return bIsWow64;
}

void RegTool::QueryRegValue(HKEY root,const wchar_t* szRegPath,const wchar_t* szKey, std::wstring* pRet, REGSAM flag){
	HRESULT hr;
	if ((hr = m_key.Open(root, szRegPath, flag)) == ERROR_SUCCESS) {
		TCHAR tszValue[MAX_PATH] = {0};
		ULONG lLen = MAX_PATH;
		if (m_key.QueryStringValue(szKey, tszValue, &lLen) == ERROR_SUCCESS){
			std::wstring *pstr = (std::wstring*)pRet;
			*pstr = tszValue;
		}
	}
}

void RegTool::QueryRegValue(HKEY root,const wchar_t* szRegPath,const wchar_t* szKey, DWORD* pRet, REGSAM flag){
	HRESULT hr;
	if ((hr = m_key.Open(root, szRegPath, flag)) == ERROR_SUCCESS) {
		DWORD dwInfo;
		if((m_key.QueryDWORDValue(szKey, dwInfo) == ERROR_SUCCESS)){
			DWORD *pdwInfo = (DWORD*)pRet;
			*pdwInfo = dwInfo;
		}
	}
}

BOOL RegTool::DeleteRegKey(HKEY root, const wchar_t* szRegPath, const wchar_t* szKey, REGSAM flag){
	BOOL bRet = FALSE;
	if (m_key.Open(root, szRegPath, flag) == ERROR_SUCCESS) {
		bRet = ((m_key.RecurseDeleteKey(szKey) != ERROR_SUCCESS) ? (m_key.DeleteSubKey(szKey) == ERROR_SUCCESS) : TRUE);
	}
	return bRet;
}

BOOL RegTool::DeleteRegValue(HKEY root, const wchar_t* szRegPath, const wchar_t* szValue, REGSAM flag){
	BOOL bRet = FALSE;
	if (m_key.Open(root, szRegPath, flag) == ERROR_SUCCESS) {
		bRet = m_key.DeleteValue(szValue) == ERROR_SUCCESS;
	}
	return bRet;
}

void RegTool::SetRegValue(HKEY hk, const wchar_t* szRegPath, const wchar_t* szKey, const wchar_t* value, BOOL bWow64){
	REGSAM samDesired = bWow64?(KEY_WOW64_64KEY|KEY_ALL_ACCESS):KEY_ALL_ACCESS;
	HRESULT hr = m_key.Open(hk, szRegPath,samDesired);
	if (hr != ERROR_SUCCESS) {
		hr = m_key.Create(hk,szRegPath, 0, 0, samDesired);
	}
	if (hr != ERROR_SUCCESS) 
		return;
	m_key.SetStringValue(szKey, value);
}

void RegTool::SetRegValue(HKEY hk, const wchar_t* szRegPath, const wchar_t* szKey, const DWORD value, BOOL bWow64){
	REGSAM samDesired = bWow64?(KEY_WOW64_64KEY|KEY_ALL_ACCESS):KEY_ALL_ACCESS;
	HRESULT hr = m_key.Open(hk, szRegPath,samDesired);
	if (hr != ERROR_SUCCESS) {
		hr = m_key.Create(hk,szRegPath, 0, 0, samDesired);
	}
	if (hr != ERROR_SUCCESS) 
		return;
	m_key.SetDWORDValue(szKey, value);
}

void FreeProcessUserSID(PSID psid)
{
	::HeapFree(::GetProcessHeap(), 0, (LPVOID)psid);
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

BOOL GetProcessUserSidAndAttribute(PSID *ppsid, DWORD *pdwAttribute)
{
	if (NULL == ppsid || NULL == pdwAttribute) return FALSE;

	BOOL bRet = FALSE;

	BOOL bSuc = TRUE;
	DWORD dwLastError = ERROR_SUCCESS;

	HANDLE hProcessToken = NULL;
	bSuc = ::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &hProcessToken);
	dwLastError = ::GetLastError();

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

BOOL GetCurrentUserSIDHelper(PSID *ppSID)
{
	BOOL bSuc = FALSE;

	if (ppSID)
	{
		PSID psid = NULL;
		DWORD dwAttribute = 0;
		if (GetProcessUserSidAndAttribute(&psid, &dwAttribute))
		{
			*ppSID = psid;
			bSuc = TRUE;
		}
	}

	return bSuc;
}

BOOL SetNamedSecurityInfoHelper(LPSTR pszObjectName, SE_OBJECT_TYPE emObjectType, LPSTR pszAccessDesireds)
{
	typedef struct {
		const char* name;
		int desired;
		ACCESS_MODE mode;
	}RegAccessPermission, *RegAccessPermissionPtr;

	static RegAccessPermission filelookup[]= {
		{"ALL_ACCESS", FILE_ALL_ACCESS, SET_ACCESS},
		{"READ", FILE_READ_DATA, SET_ACCESS},
		{"WRITE", FILE_WRITE_DATA|FILE_APPEND_DATA, SET_ACCESS},
		{"~READ", FILE_READ_DATA, DENY_ACCESS},
		{"~WRITE", FILE_WRITE_DATA|FILE_APPEND_DATA, DENY_ACCESS},
		{NULL, 0},
	};

	static RegAccessPermission reglookup[]= {
		{"ALL_ACCESS", KEY_ALL_ACCESS, SET_ACCESS},
		{"READ", KEY_QUERY_VALUE|KEY_ENUMERATE_SUB_KEYS, SET_ACCESS},
		{"WRITE", KEY_SET_VALUE, SET_ACCESS},
		{"~READ", KEY_QUERY_VALUE|KEY_ENUMERATE_SUB_KEYS, DENY_ACCESS},
		{"~WRITE", KEY_SET_VALUE, DENY_ACCESS},
		{NULL, 0},
	};

	static RegAccessPermission nulllookup[]= {{NULL, 0}};

	const RegAccessPermissionPtr lookup = (emObjectType == SE_FILE_OBJECT) ? filelookup : (emObjectType == SE_REGISTRY_KEY) ? reglookup : nulllookup;
	if (pszObjectName && *pszObjectName && 
		pszAccessDesireds && *pszAccessDesireds)
	{
		BOOL bSuccess = EnablePrivilegeHelper(::GetCurrentProcess(), SE_DEBUG_NAME, TRUE);
		if (TRUE)
		{
			PSID pSID = NULL;
			PSECURITY_DESCRIPTOR pSecurityDescriptor = NULL;
			PACL pNewDAcl = NULL;
			PEXPLICIT_ACCESS pDAclEntries = NULL;

			__try
			{
				bSuccess = GetCurrentUserSIDHelper(&pSID);
				if (!bSuccess) __leave;

				PACL pOldDAcl = NULL;
				DWORD dwRetCode = ERROR_SUCCESS;
				if (ERROR_SUCCESS != dwRetCode) __leave;

				pDAclEntries = (PEXPLICIT_ACCESS)::LocalAlloc(0, sizeof(EXPLICIT_ACCESS) * 10);
				if (!pDAclEntries) __leave;

				int cNumbersOfEntries = 0;
				const char* szAcessDesired = strtok(pszAccessDesireds, "|");
				BOOL bAcessDesiredExisted;
				while (szAcessDesired && *szAcessDesired)
				{
					bAcessDesiredExisted = FALSE;
					for (int i =0; lookup[i].name; ++i)
					{
						if (stricmp(szAcessDesired, lookup[i].name) == 0)
						{
							bAcessDesiredExisted = TRUE;
							BOOL bAccessModeExisted = FALSE;
							for (int cnt = 0; cnt < cNumbersOfEntries; ++cnt)
							{
								EXPLICIT_ACCESS& ea = pDAclEntries[cnt];
								if (ea.grfAccessMode == lookup[i].mode)
								{
									bAccessModeExisted = TRUE;
									ea.grfAccessPermissions |= lookup[i].desired;
									break;
								}
							}

							if (!bAccessModeExisted)
							{
								EXPLICIT_ACCESS& ea = pDAclEntries[cNumbersOfEntries];
								SecureZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
								ea.grfAccessPermissions = lookup[i].desired;
								ea.grfAccessMode = lookup[i].mode;
								ea.grfInheritance= SUB_CONTAINERS_AND_OBJECTS_INHERIT;
								ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
								ea.Trustee.ptstrName = (LPTSTR)pSID;
								++cNumbersOfEntries;
							}

							break;
						}
					}

					if (!bAcessDesiredExisted) __leave;

					szAcessDesired = strtok(NULL, "|");
				}

				if (cNumbersOfEntries > 0)
				{
					dwRetCode = ::SetEntriesInAcl(cNumbersOfEntries, pDAclEntries, NULL, &pNewDAcl);
					if (ERROR_SUCCESS != dwRetCode) __leave;

					dwRetCode = ::SetNamedSecurityInfoA(
						pszObjectName, emObjectType, 
						DACL_SECURITY_INFORMATION,
						NULL, NULL, pNewDAcl, NULL);

					if (ERROR_SUCCESS == dwRetCode)
					{
						return TRUE;
					}
				}
			}
			__finally
			{
				if (pSID)
				{
					FreeProcessUserSID(pSID);
				}

				if (pSecurityDescriptor)
					::LocalFree(pSecurityDescriptor);

				if (pNewDAcl)
					::LocalFree(pNewDAcl);
			}
		}
	}

	return FALSE;
}

BOOL RegSecurity::SetRegSecurity(LPCSTR lpszRegPath, LPCSTR lpszAccessDesireds)
{
	static struct {
		const char* name;
		const char* replace;
	}lookup[]= {
		{"HKEY_CLASSES_ROOT", "CLASSES_ROOT"},
		{"HKEY_CURRENT_USER", "CURRENT_USER"},
		{"HKEY_LOCAL_MACHINE", "MACHINE"},
		{"HKEY_USERS", "USERS"},
		{"HKEY_CURRENT_CONFIG", "CONFIG"},
		{0, 0},
	};

	if (lpszRegPath && *lpszRegPath &&
		lpszAccessDesireds && *lpszAccessDesireds)
	{
		for (int i = 0; lookup[i].name; ++i)
		{
			if (::StrCmpNIA(lpszRegPath, lookup[i].name, strlen(lookup[i].name)) == 0)
			{
				char szNewRegPath[MAX_PATH + 1] = {0};
				::StrCatA(szNewRegPath, lookup[i].replace);
				::StrCatA(szNewRegPath, lpszRegPath + strlen(lookup[i].name));

				return SetNamedSecurityInfoHelper(szNewRegPath, SE_REGISTRY_KEY, (LPSTR)lpszAccessDesireds);

			}
		}
	}
	return FALSE;
}

BOOL FileAssociation::IsVistaOrHigher(){
	OSVERSIONINFOEX osvi = { sizeof(OSVERSIONINFOEX) };
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!GetVersionEx( (LPOSVERSIONINFO)&osvi )){
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if(!GetVersionEx( (LPOSVERSIONINFO)&osvi )){
			return FALSE;
		}
	}
	return osvi.dwMajorVersion >= 6;
}

FileAssociation* FileAssociation::Instance(){
	static FileAssociation s_fa;
	return &s_fa;
}

void FileAssociation::CreateImgKey(const std::wstring& strFileExt){
	wchar_t* szPaths[][3] = {
		{L"kuaikan%s", L"", L"图像文件(%s)"},
		{L"kuaikan%s\\DefaultIcon", L"", L"iconpath"},
		{L"kuaikan%s\\Shell", L"", L"open"},
		{L"kuaikan%s\\Shell\\open", L"", L"使用快看打开"},
		{L"kuaikan%s\\Shell\\open\\command", L"", L"openpath"},
		{L"*\\Shell\\使用快看打开图片\\command", L"", L"openpath"},
	};
	wchar_t szTemp1[MAX_PATH] = {0}, szTemp2[MAX_PATH] = {0}; 
	RegTool rt;
	for(int i = 0; i < sizeof(szPaths)/sizeof(szPaths[0]); ++i){
		memset(szTemp1, 0, MAX_PATH);
		swprintf(szTemp1, szPaths[i][0], strFileExt.c_str());
		memset(szTemp2, 0, MAX_PATH);
		if(0 == _tcsnicmp(szPaths[i][2], _T("iconpath"), 8)){
			_tcsncpy(szTemp2, GetIconPath(strFileExt).c_str(), MAX_PATH);
		}
		else if(0 == _tcsnicmp(szPaths[i][2], _T("openpath"), 8)){
			_tcsncpy(szTemp2, GetCommandOpenPath().c_str(), MAX_PATH);
		}
		else {
			swprintf(szTemp2, szPaths[i][2], strFileExt.c_str());
		}
		rt.SetRegValue(HKEY_CLASSES_ROOT, szTemp1, szPaths[i][1], szTemp2);
	}
	//在classroot下写入OpenWithProgIds
	memset(szTemp1, 0, MAX_PATH);
	swprintf(szTemp1, L"%s\\OpenWithProgids", strFileExt.c_str());
	memset(szTemp2, 0, MAX_PATH);
	swprintf(szTemp2, L"kuaikan%s", strFileExt.c_str());
	rt.SetRegValue(HKEY_CLASSES_ROOT, szTemp1, szTemp2, L"");
}

void FileAssociation::StringReplace(std::wstring& src,const std::wstring& oldstr, const std::wstring& newstr){
	std::wstring::size_type pos = 0;
	std::wstring::size_type oldlen = oldstr.size();
	std::wstring::size_type newlen = newstr.size();
	while((pos = src.find(oldstr,pos))!= std::wstring::npos)
	{
		src.replace(pos, oldlen, newstr);
		pos += newlen;
	}
}

UINT FileAssociation::Associated(const std::wstring strFileExt){
	wchar_t* szPaths[][4] = {
		{L"HKEY_CURRENT_USER", L"%s", L"Progid", L"kuaikan%s"},
		{L"HKEY_CURRENT_USER", L"%s", L"Application", L"kuaikantu.exe"},
		{L"HKEY_CLASSES_ROOT", L"%s", L"", L"kuaikan%s"},
		{L"HKEY_CLASSES_ROOT", L"%s\\OpenWithList\\kuaikantu.exe", L"", L""},
		{L"HKEY_CLASSES_ROOT", L"%s\\OpenWithProgids", L"kuaikan%s", L""},
		{L"HKEY_CLASSES_ROOT", L"*\\Shell\\使用快看打开图片", L"", L""},
	};
	std::wstring basepath = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\";
	UINT atype = None;
	HKEY hk;
	std::wstring regpath;
	wchar_t szTemp1[MAX_PATH] = {0}, szTemp2[MAX_PATH] = {0}, szTemp3[MAX_PATH] = {0};
	BOOL isVista = IsVistaOrHigher();
	RegTool rt;
	swprintf(szTemp1, L"kuaikan%s", strFileExt.c_str());
	atype |= rt->Open(HKEY_CLASSES_ROOT, szTemp1, KEY_READ) == ERROR_SUCCESS ? RootKeyExist : 0;
	for(int i = 0; i < sizeof(szPaths)/sizeof(szPaths[0]); ++i){
		hk = _tcsnicmp(szPaths[i][0], L"HKEY_CURRENT_USER", MAX_PATH) == 0 ? HKEY_CURRENT_USER : HKEY_CLASSES_ROOT;
		regpath = i <= 1 ? (isVista ? (basepath + szPaths[i][1] + L"\\UserChoice") : (basepath + szPaths[i][1])) : szPaths[i][1];
		memset(szTemp1, 0, MAX_PATH);
		swprintf(szTemp1, regpath.c_str(), strFileExt.c_str());
		memset(szTemp2, 0, MAX_PATH);
		swprintf(szTemp2, szPaths[i][2], strFileExt.c_str());
		memset(szTemp3, 0, MAX_PATH);
		swprintf(szTemp3, szPaths[i][3], strFileExt.c_str());
		if(i>=3){
			switch(i){
				case 3:
					atype |=  rt->Open(hk, szTemp1, KEY_READ) == ERROR_SUCCESS ? ClassRootOpenWithList : 0;
					break;
				case 4:
					{
						std::wstring strRetW = L"-1";
						rt.QueryRegValue(hk, szTemp1, szTemp2, &strRetW);
						atype |= strRetW != L"-1" ? ClassRootOpenWithProgIds : 0;
					}
					break;
				case 5:
					atype |=  rt->Open(hk, szTemp1, KEY_READ) == ERROR_SUCCESS ? ContextMenuExist : 0;
					break;
			}
		}
		else{
			std::wstring strRet;
			rt.QueryRegValue(hk, szTemp1, szTemp2, &strRet);
			if(strRet != L"" && 0 == _tcsnicmp(_tcsupr((wchar_t*)szTemp3), _tcsupr((wchar_t*)strRet.c_str()), strRet.length())){
				switch(i){
					case 0:
						atype |= ProgID;
						break;
					case 1:
						atype |= AssociateType::Application;
						break;
					case 2:
						atype |= ClassRoot;
						break;
				}
			}
		}		
	}
	regpath = basepath+strFileExt+L"\\OpenWithProgids";
	std::wstring strkey =  L"kuaikan";
	strkey += strFileExt;
	DWORD dwValue = 1;
	rt.QueryRegValue(HKEY_CURRENT_USER, regpath.c_str(), strkey.c_str(), &dwValue);
	if (dwValue == 0){
		atype |= CurrentUserOpenWithProgIds;
	}
	//atype |= FileAssociationWin10::Associated(strFileExt);
	return atype;
}

UINT FileAssociationWin10::Associated(const std::wstring strFileExt){
	if (!IsWin10()){
		return 0;
	}
	UINT atype = None;
	wchar_t  *szExeName = L"kuaikantu.exe";
	wchar_t szRegPath[MAX_PATH] = {0}, szRegPath2[MAX_PATH] = {0};
	wcscpy(szRegPath, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\");
	wcscat(szRegPath, strFileExt.c_str());
	wcscpy(szRegPath2, szRegPath);
	wcscat(szRegPath2, L"\\UserChoice");
	wcscat(szRegPath, L"\\OpenWithList");
	RegTool rt;
	std::wstring strProgid = L"-1", strHash = L"-1";
	rt.QueryRegValue(HKEY_CURRENT_USER, szRegPath2, L"ProgId", &strProgid);
	rt.QueryRegValue(HKEY_CURRENT_USER, szRegPath2, L"Hash", &strHash);
	if (strHash.length() > 1 &&
		strProgid.length() > 4 &&
		_tcsnicmp(_tcsupr((wchar_t*)strProgid.c_str()), L"APPX", 4) == 0){
			wchar_t szTmp[MAX_PATH] = {0};
			swprintf(szTmp, L"kuaikan%s", strFileExt.c_str());
			//更新default值
			rt.SetRegValue(HKEY_CLASSES_ROOT, szTmp, L"default", strProgid.c_str());
	}
	else if (strHash.length() > 1 &&
			_tcsnicmp(_tcsupr((wchar_t*)strProgid.c_str()), L"KUAIKAN.", 8) == 0){
				atype |= AssociateType::ProgIDIsMEWin10;
	}
	else {
		{
			std::wstring strRetW = L"-1";
			rt.QueryRegValue(HKEY_CURRENT_USER, szRegPath, L"MRUList", &strRetW);
			if(strRetW[0] != L'-1'){
				std::wstring strRetW2 = L"-1";
				rt.QueryRegValue(HKEY_CURRENT_USER, szRegPath, strRetW.substr(0,1).c_str(), &strRetW2);
				if (strRetW2 == szExeName){
					atype |= AssociateType::CrrentUserIsMeWin10;
				}
			}
		}
		strProgid = L"-1";
		wchar_t szTmp[MAX_PATH] = {0};
		swprintf(szTmp, L"kuaikan%s", strFileExt.c_str());
		rt.QueryRegValue(HKEY_CLASSES_ROOT, szTmp, L"default", &strProgid);
		if (strProgid.length() > 4 && _tcsnicmp(_tcsupr((wchar_t*)strProgid.c_str()), L"APPX", 4) == 0){
			return atype;
		}
	}
	std::wstring strNoOpenWithW = L"-1";
	rt.QueryRegValue(HKEY_CLASSES_ROOT, strProgid.c_str(), L"NoOpenWith", &strNoOpenWithW);
	if (strNoOpenWithW != L"-1"){
		atype |= AssociateType::DefaultIsNoOpenWith;
	}

	return atype;
}


void FileAssociation::SetAssociate(const std::wstring strFileExt, BOOL bAssociate, BOOL bHasAdmin){
	UINT atype = Associated(strFileExt);
	std::wstring basepath = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\";
	BOOL isVista = IsVistaOrHigher();
	std::wstring regpath = isVista ? (basepath+strFileExt+L"\\UserChoice") : (basepath+strFileExt);
	RegTool rt;
	//非win10才干这里, win10要先留着后面要读取信息做判断
	if (!FileAssociationWin10::IsWin10()){
		HRESULT hr = rt->Open(HKEY_CURRENT_USER, regpath.c_str());
		if (isVista && hr == ERROR_ACCESS_DENIED){
			regpath = basepath+strFileExt;
			rt.DeleteRegKey(HKEY_CURRENT_USER, regpath.c_str(), L"UserChoice");
			regpath = basepath+strFileExt +L"\\UserChoice";
		}
	}
	//修改权限
	if (isVista){
		std::wstring str = L"HKEY_CURRENT_USER\\";
		str += regpath;
		RegSecurity::SetRegSecurity((LPCSTR)(_bstr_t)str.c_str(), "ALL_ACCESS");
	}
	if (!bAssociate){
		/*if (bHasAdmin){
			std::wstring strSelfNode = L"kuaikan";
			strSelfNode += strFileExt;
			rt.DeleteRegKey(HKEY_CLASSES_ROOT, L"", strSelfNode.c_str());
		}*/
		if ((atype&ProgID) != 0){
			std::wstring strProgidBak = L"";
			rt.QueryRegValue(HKEY_CURRENT_USER, regpath.c_str(), L"kuaikanbak", &strProgidBak);
			if(strProgidBak.length() > 0 && strProgidBak.substr(0, 3) != L"kuaikan"){
				rt.SetRegValue(HKEY_CURRENT_USER, regpath.c_str(), L"Progid", strProgidBak.c_str());
				rt->DeleteValue(L"kuaikanbak");
			}
			else{
				std::wstring strDeletePath = basepath + strFileExt;
				isVista ? rt.DeleteRegKey(HKEY_CURRENT_USER, strDeletePath.c_str(), L"UserChoice") : rt.DeleteRegValue(HKEY_CURRENT_USER, strDeletePath.c_str(), L"Progid");
			}
		}
		if((atype&AssociateType::Application) != 0 && !isVista){
			std::wstring strAppBak = L"";
			rt.QueryRegValue(HKEY_CURRENT_USER, regpath.c_str(), L"kuaikanappbak", &strAppBak);
			if(strAppBak.length() > 0 && strAppBak.substr(0, 3) != L"kuaikan"){
				rt.SetRegValue(HKEY_CURRENT_USER, regpath.c_str(), L"Application", strAppBak.c_str());
				rt->DeleteValue(L"kuaikanappbak");
			}
			else{
				rt.DeleteRegValue(HKEY_CURRENT_USER, regpath.c_str(), L"Application");
			}
		}
		if (bHasAdmin && (atype&AssociateType::ClassRoot) != 0){
			std::wstring strProgidBak = L"";
			rt.QueryRegValue(HKEY_CLASSES_ROOT, strFileExt.c_str(), L"kuaikanbak", &strProgidBak);
			if(strProgidBak.length() > 0 && strProgidBak.substr(0, 3) != L"kuaikan"){
				rt.SetRegValue(HKEY_CLASSES_ROOT, strFileExt.c_str(), L"", strProgidBak.c_str());
				rt->DeleteValue(L"kuaikanbak");
			}
			else{
				rt.SetRegValue(HKEY_CLASSES_ROOT, strFileExt.c_str(), L"", L"");
			}
		}
		//右键菜单
		if (bHasAdmin && (atype&AssociateType::ContextMenuExist) != 0){
			rt.DeleteRegKey(HKEY_CLASSES_ROOT, L"*\\Shell", L"使用快看打开图片");
		}
		//右键打开方式列表
		if (bHasAdmin && (atype&AssociateType::ClassRootOpenWithProgIds) != 0){
			std::wstring rootpath = strFileExt + L"\\OpenWithProgids";
			std::wstring valuename = L"kuaikan";
			valuename += strFileExt;
			rt.DeleteRegValue(HKEY_CLASSES_ROOT, rootpath.c_str(), valuename.c_str());
		}
		if (FileAssociationWin10::IsWin10()){
			FileAssociationWin10::SetAssociate(strFileExt, bAssociate, bHasAdmin);
		}
		
	}
	else {
		if (bHasAdmin || (atype&RootKeyExist) == 0){
			CreateImgKey(strFileExt);
		}
		if (bHasAdmin && (atype&AssociateType::ClassRoot) == 0){
			std::wstring strProgid = L"";
			rt.QueryRegValue(HKEY_CLASSES_ROOT, strFileExt.c_str(), L"", &strProgid);
			if(strProgid.length() > 0 && strProgid.substr(0, 3) != L"kuaikan"){
				rt.SetRegValue(HKEY_CLASSES_ROOT, strFileExt.c_str(), L"kuaikanbak", strProgid.c_str());
			}
			strProgid = L"kuaikan";
			strProgid += strFileExt;
			rt.SetRegValue(HKEY_CLASSES_ROOT, strFileExt.c_str(), L"", strProgid.c_str());

		}
		if (FileAssociationWin10::IsWin10()){
			FileAssociationWin10::SetAssociate(strFileExt, bAssociate, bHasAdmin);
		}
		if ((atype&ProgID) == 0){
			std::wstring strProgid = L"";
			rt.QueryRegValue(HKEY_CURRENT_USER, regpath.c_str(), L"Progid", &strProgid);
			if(strProgid.length() > 0 && strProgid.substr(0, 3) != L"kuaikan"){
				rt.SetRegValue(HKEY_CURRENT_USER, regpath.c_str(), L"kuaikanbak", strProgid.c_str());
			}
			strProgid = L"kuaikan";
			strProgid += strFileExt;
			rt.SetRegValue(HKEY_CURRENT_USER, regpath.c_str(), L"Progid", strProgid.c_str());
		}
		//不抢Application关联， 因为它存在会导致图标不能显示， 无条件删除它
		/*if((atype&AssociateType::Application) == 0 && !isVista){
			std::wstring strApp = L"";
			rt.QueryRegValue(HKEY_CURRENT_USER, regpath.c_str(), L"Application", &strApp);
			if(strApp.length() > 0 && strApp.substr(0, 3) != L"kuaikan"){
				rt.SetRegValue(HKEY_CURRENT_USER, regpath.c_str(), L"kuaikanappbak", strApp.c_str());
			}
			strApp = L"kuaikantu.exe";
			rt.SetRegValue(HKEY_CURRENT_USER, regpath.c_str(), L"Application", strApp.c_str());
		}*/
		if (rt->Open(HKEY_CURRENT_USER, regpath.c_str(), KEY_READ) == ERROR_SUCCESS){
			rt->DeleteValue(L"Application");
		}
		if ((atype&AssociateType::CurrentUserOpenWithProgIds) == 0){
			regpath = basepath+strFileExt+L"\\OpenWithProgids";
			std::wstring strkey = L"kuaikan";
			strkey += strFileExt;
			rt.SetRegValue(HKEY_CURRENT_USER, regpath.c_str(), strkey.c_str(), (DWORD)0);
		}
		//最后禁止写
		if (isVista){
			std::wstring str = L"HKEY_CURRENT_USER\\";
			str += basepath + strFileExt + L"\\UserChoice";
			RegSecurity::SetRegSecurity((LPCSTR)(_bstr_t)str.c_str(), "~WRITE");
		}
	}
}

void FileAssociationWin10::SetAssociate(const std::wstring strFileExt, BOOL bAssociate, BOOL bHasAdmin){
	UINT atype = FileAssociationWin10::Associated(strFileExt);
	std::wstring basepath = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\";
	RegTool rt;
	if (bAssociate){
		if ((atype&AssociateType::DefaultIsNoOpenWith) == 0 && bHasAdmin){
			//写入nooepnwith
			wchar_t szTmp[MAX_PATH] = {0};
			swprintf(szTmp, L"kuaikan%s", strFileExt.c_str());
			std::wstring strRetDefaultW = L"-1";
			rt.QueryRegValue(HKEY_CLASSES_ROOT, szTmp, L"default", &strRetDefaultW);
			if (strRetDefaultW != L"-1"){
				if (rt->Open(HKEY_CLASSES_ROOT, strRetDefaultW.c_str()) == ERROR_SUCCESS){
					rt.SetRegValue(HKEY_CLASSES_ROOT, strRetDefaultW.c_str(), L"NoOpenWith", L"");
				}
			}
		}
		if ((atype&AssociateType::ProgIDIsMEWin10) == 0){
			std::wstring str1 = basepath + strFileExt;
			if(!rt.DeleteRegKey(HKEY_CURRENT_USER, str1.c_str(), L"UserChoice")){
				std::wstring str2 = L"HKEY_CURRENT_USER\\";
				str2 += str1;
				str2 += L"\\UserChoice";
				RegSecurity::SetRegSecurity((LPCSTR)(_bstr_t)str2.c_str(), "ALL_ACCESS");
				rt.DeleteRegKey(HKEY_CURRENT_USER, str1.c_str(), L"UserChoice");
			}
		}
		if ((atype&AssociateType::CrrentUserIsMeWin10) == 0){
			std::wstring str1 = basepath+strFileExt;
			str1 += L"\\OpenWithList";
			rt.SetRegValue(HKEY_CURRENT_USER, str1.c_str(), L"a", L"kuaikantu.exe");
			rt.SetRegValue(HKEY_CURRENT_USER, str1.c_str(), L"MRUList", L"a");
		}
	}
	else{
		if ((atype&AssociateType::ProgIDIsMEWin10) != 0){
			std::wstring str1 = basepath + strFileExt;
			rt.DeleteRegKey(HKEY_CURRENT_USER, str1.c_str(), L"UserChoice");
		}
		if ((atype&AssociateType::CrrentUserIsMeWin10) != 0){
			std::wstring str1 = basepath + strFileExt;
			rt.DeleteRegKey(HKEY_CURRENT_USER, str1.c_str(), L"OpenWithList");
		}
	}
}

void FileAssociation::AssociateAll(std::wstring strFileExts, BOOL bAssociate, BOOL bHasAdmin){
	if (strFileExts.length() < 2){
		return;
	}
	size_t stEnd = strFileExts.find_last_of(L";");
	if (strFileExts.length() - 1 == stEnd){
		strFileExts = strFileExts.substr(0, stEnd);
	}
	size_t stCutline = strFileExts.npos;
	do{
		stCutline = strFileExts.find_first_of(L";");
		std::wstring wsAssFirst(L"");
		if (strFileExts.npos != stCutline){
			wsAssFirst = strFileExts.substr(0, stCutline);
			strFileExts = strFileExts.substr(stCutline + 1);
		}
		else{
			wsAssFirst = strFileExts;
		}
		SetAssociate(wsAssFirst, bAssociate, bHasAdmin);
	} while (strFileExts.npos != stCutline);
}

void FileAssociation::CreateImgKeyALL(std::wstring strFileExts, BOOL bDo){
	if (strFileExts.length() < 2){
		return;
	}
	size_t stEnd = strFileExts.find_last_of(L";");
	if (strFileExts.length() - 1 == stEnd){
		strFileExts = strFileExts.substr(0, stEnd);
	}
	size_t stCutline = strFileExts.npos;
	do{
		stCutline = strFileExts.find_first_of(L";");
		std::wstring wsAssFirst(L"");
		if (strFileExts.npos != stCutline){
			wsAssFirst = strFileExts.substr(0, stCutline);
			strFileExts = strFileExts.substr(stCutline + 1);
		}
		else{
			wsAssFirst = strFileExts;
		}
		if (bDo){
			CreateImgKey(wsAssFirst);
		}
		else{
			std::wstring strSelfNode = L"kuaikan";
			strSelfNode += wsAssFirst;
			RegTool rt;
			rt.DeleteRegKey(HKEY_CLASSES_ROOT, L"", strSelfNode.c_str());
		}
		
	} while (strFileExts.npos != stCutline);
}

std::wstring FileAssociation::GetIconPath(const std::wstring& strFileExt){
	RegTool rt;
	std::wstring strInstDir = L"";
	rt.QueryRegValue(HKEY_LOCAL_MACHINE, L"Software\\kuaikan", L"InstDir", &strInstDir);
	if (strInstDir.length() == 0){
		return L"";
	}
	std::wstring strIcoDir = L"res\\";
	strIcoDir += strFileExt.substr(1, strFileExt.length()-1);
	strIcoDir += L".ico";
	WCHAR szIcoPath[MAX_PATH] = {0};
	PathCombine(szIcoPath, strInstDir.c_str(), strIcoDir.c_str());
	return szIcoPath;
}
std::wstring FileAssociation::GetCommandOpenPath(){
	RegTool rt;
	std::wstring strExePath = L"";
	rt.QueryRegValue(HKEY_LOCAL_MACHINE, L"Software\\kuaikan", L"Path", &strExePath);
	WCHAR strCmd[MAX_PATH] = {0};
	swprintf(strCmd, L"\"%s\" \"%%1\" /sstartfrom LocalFile", strExePath.c_str());
	return strCmd;
}

void FileAssociationWarpper::SetAssociate2(){
	std::wstring strDo = L"", strUnDo = L"";
	DWORD dwNoUpdate = -1;
	RegTool rt;
	rt.QueryRegValue(HKEY_CURRENT_USER, L"Software\\kuaikan", L"AssociateDo", &strDo);
	rt.QueryRegValue(HKEY_CURRENT_USER, L"Software\\kuaikan", L"AssociateUnDo", &strUnDo);
	rt.QueryRegValue(HKEY_CURRENT_USER, L"Software\\kuaikan", L"NoUpdate", &dwNoUpdate);
	if (strDo != L""){
		FileAssociation::Instance()->AssociateAll(strDo, true, true);
	}
	if (strUnDo != L""){
		FileAssociation::Instance()->AssociateAll(strUnDo, false, true);
	}
	if (dwNoUpdate != 1){
		FileAssociation::Instance()->Update();
	}
};
void FileAssociationWarpper::SetAssociate1(const wchar_t* szDo, const wchar_t* szUnDo, DWORD NoUpdate){
	RegTool rt;
	rt.SetRegValue(HKEY_CURRENT_USER, L"Software\\kuaikan", L"AssociateDo", szDo);
	rt.SetRegValue(HKEY_CURRENT_USER, L"Software\\kuaikan", L"AssociateUnDo", szUnDo);
	rt.SetRegValue(HKEY_CURRENT_USER, L"Software\\kuaikan", L"NoUpdate", (DWORD)NoUpdate);
	std::wstring strDirPath = L"-1";
	rt.QueryRegValue(HKEY_LOCAL_MACHINE, L"Software\\kuaikan", L"InstDir", &strDirPath);
	if (strDirPath != L"-1" && PathFileExists(strDirPath.c_str())){
		WCHAR szDest[MAX_PATH] = {0};
		ZeroMemory(szDest, MAX_PATH);
		::PathCombine(szDest, strDirPath.c_str(), L"program\\kuaikantu.exe");
		ShellExecute(NULL, L"runas", szDest, L"/setassociate", NULL, SW_HIDE);
	}
};