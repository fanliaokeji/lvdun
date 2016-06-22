/************************************************************************************************************************************
1、判断是否关联
FileAssociation* pfa = FileAssociation::Instance();
UINT flag = pfa->Associated(L".jpg");
if(flag&ProgID != 0 && flag&RootKeyExist != 0){
	cout<<"已经关联了"<<endl;
}
2、设置单个扩展名关联
FileAssociation::Instance()->SetAssociate(L".jpg", TRUE, TRUE);
FileAssociation::Instance()->Update();

3、设置多个扩展名关联
FileAssociation::Instance()->AssociateAll(L".jpg;.png;.bmp;", TRUE, TRUE);
FileAssociation::Instance()->Update();
************************************************************************************************************************************/
#pragma once

#include <atlbase.h>
#include <windows.h>
#include <ShlObj.h>
#include <string>

class RegTool{
	public:
	RegTool(){};
	~RegTool(){m_key.Close();};
	BOOL IsWow64();
	void QueryRegValue(HKEY root,const wchar_t* szRegPath,const wchar_t* szKey, std::wstring* pRet, REGSAM flag =  KEY_READ);
	void QueryRegValue(HKEY root,const wchar_t* szRegPath,const wchar_t* szKey, DWORD* pRet, REGSAM flag =  KEY_READ);
	BOOL DeleteRegKey(HKEY root, const wchar_t* szRegPath, const wchar_t* szKey, REGSAM flag = KEY_ALL_ACCESS);
	BOOL DeleteRegValue(HKEY root, const wchar_t* szRegPath, const wchar_t* szValue, REGSAM flag = KEY_ALL_ACCESS);
	void SetRegValue(HKEY hk, const wchar_t* szRegPath, const wchar_t* szKey, const wchar_t* value, BOOL bWow64 = FALSE);
	void SetRegValue(HKEY hk, const wchar_t* szRegPath, const wchar_t* szKey, const DWORD value, BOOL bWow64 = FALSE);
	ATL::CRegKey * operator->(){
		return &m_key;
	};
	private:
	ATL::CRegKey m_key;
};

typedef enum tagAssociateType{
	None								=	0X0000000000,
	ProgID								=	0X0000000001,
	Application							=	0X0000000010,
	ClassRoot							=	0X0000000100,
	ClassRootOpenWithList				=	0X0000001000,
	ClassRootOpenWithProgIds			=	0X0000010000,
	CurrentUserOpenWithProgIds			=	0X0000100000,
	RootKeyExist						=	0X0001000000,
	ContextMenuExist					=	0X0010000000,
}AssociateType;

class FileAssociation{
	public:
		~FileAssociation(){};
		static FileAssociation* Instance();
		UINT Associated(const std::wstring strFileExt);
		void SetAssociate(std::wstring strFileExt, BOOL bAssociate = TRUE, BOOL bHasAdmin = FALSE);
		void AssociateAll(std::wstring strFileExts, BOOL bAssociate = TRUE, BOOL bHasAdmin = FALSE);
		void CreateImgKeyALL(std::wstring strFileExts);
		void Update(){SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST , 0, 0);};
	private:
		FileAssociation(){};
		void CreateImgKey(const std::wstring& strFileExt);
		std::wstring GetIconPath(const std::wstring& strFileExt);
		std::wstring GetCommandOpenPath();
		void StringReplace(std::wstring& src,const std::wstring& oldstr, const std::wstring& newstr);
		BOOL IsVistaOrHigher();
};

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
		bRet = m_key.RecurseDeleteKey(szKey) != ERROR_SUCCESS ? m_key.DeleteSubKey(szKey) == ERROR_SUCCESS : TRUE;
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
						DWORD dwRet = -1;
						rt.QueryRegValue(hk, szTemp1, szTemp2, &dwRet);
						atype |= dwRet==0 ? ClassRootOpenWithProgIds : 0;
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
	return atype;
}

void FileAssociation::SetAssociate(const std::wstring strFileExt, BOOL bAssociate, BOOL bHasAdmin){
	UINT atype = Associated(strFileExt);
	std::wstring basepath = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\";
	BOOL isVista = IsVistaOrHigher();
	std::wstring regpath = isVista ? (basepath+strFileExt+L"\\UserChoice") : (basepath+strFileExt);
	RegTool rt;
	HRESULT hr = rt->Open(HKEY_CURRENT_USER, regpath.c_str());
	if (isVista && hr == ERROR_ACCESS_DENIED){
		regpath = basepath+strFileExt;
		rt.DeleteRegKey(HKEY_CURRENT_USER, regpath.c_str(), L"UserChoice");
		regpath = basepath+strFileExt +L"\\UserChoice";
	}
	if (!bAssociate){
		if (bHasAdmin){
			std::wstring strSelfNode = L"kuaikan";
			strSelfNode += strFileExt;
			rt.DeleteRegKey(HKEY_CLASSES_ROOT, L"", strSelfNode.c_str());
		}
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

	}
	else {
		if (bHasAdmin || (atype&RootKeyExist) == 0){
			CreateImgKey(strFileExt);
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
		//不做root下面， 不然没权限取消不了关联
		/*if (bHasAdmin && (atype&AssociateType::ClassRoot) == 0){
			std::wstring strProgid = L"";
			rt.QueryRegValue(HKEY_CLASSES_ROOT, strFileExt.c_str(), L"", &strProgid);
			if(strProgid.length() > 0 && strProgid.substr(0, 3) != L"kuaikan"){
				rt.SetRegValue(HKEY_CLASSES_ROOT, strFileExt.c_str(), L"kuaikanbak", strProgid.c_str());
			}
			strProgid = L"kuaikan";
			strProgid += strFileExt;
			rt.SetRegValue(HKEY_CLASSES_ROOT, strFileExt.c_str(), L"", strProgid.c_str());

		}*/
		if ((atype&AssociateType::CurrentUserOpenWithProgIds) == 0){
			regpath = basepath+strFileExt+L"\\OpenWithProgids";
			std::wstring strkey = L"kuaikan";
			strkey += strFileExt;
			rt.SetRegValue(HKEY_CURRENT_USER, regpath.c_str(), strkey.c_str(), (DWORD)0);
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

void FileAssociation::CreateImgKeyALL(std::wstring strFileExts){
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
		CreateImgKey(wsAssFirst);
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