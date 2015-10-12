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
	TSDEBUG4CXX("enter ~AddinHelper");
	if (this->m_hMutex) {
		::CloseHandle(this->m_hMutex);
		this->m_hMutex = NULL;
	}
	TSDEBUG4CXX("leave ~AddinHelper");
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
	TSDEBUG4CXX("enter BeginTask");
	m_mutexName = L"Global\\{4A6857BD-19DB-4b04-82F2-992CDFF19186}_OFCADDIN";
	if(this->EnsureOwnerMutex()) {
		unsigned nThreadID;
		_beginthreadex(NULL, 0, &AddinHelper::TaskThreadProc, this, 0, &nThreadID);
		TSDEBUG4CXX("create thread success, dwThreadID = "<<nThreadID);
	}
	TSDEBUG4CXX("leave BeginTask");
	return true;
}

unsigned int AddinHelper::TaskThreadProc(void* arg)
{
	return reinterpret_cast<AddinHelper*>(arg)->TaskProc();
}

unsigned int AddinHelper::TaskProc()
{
	TSDEBUG4CXX("enter TaskProc");
	HandleChangeKeys();
	TSDEBUG4CXX("leave TaskProc");
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
			LONG nLastDay = pTm->tm_mday;
			LONG nLastMonth = pTm->tm_mon;
			LONG nLastYear = pTm->tm_year;

			__time64_t lCurTime;
			_time64( &lCurTime); 
			tm* pTmc = _localtime64(&lCurTime);
			
			LONG nCurDay = pTmc->tm_mday;
			LONG nCurMonth = pTmc->tm_mon;
			LONG nCurYear = pTmc->tm_year;
			TSDEBUG4CXX("check time pTmc = "<<nCurYear<<nCurMonth<<nCurDay<<", pTm = "<<nLastYear<<nLastMonth<<nLastDay);
			if (nCurDay == nLastDay && nCurMonth == nLastMonth && nCurYear == nLastYear){
				bCando = FALSE;
			}
			else{
				bCando = TRUE;
			}
		}
		key.Close();
	}
	if (!bCando){
		return;
	}
	//下载配置
	TSDEBUG4CXX("begin download cfg.dat 1");
	TCHAR szBuffer[MAX_PATH] = {0};
	DWORD len = GetTempPath(MAX_PATH, szBuffer);
	if(len == 0)
		return;
	::PathCombine(szBuffer,szBuffer,_T("wordenclock.dat"));
	if (PathFileExists(szBuffer)){
		DeleteFile(szBuffer);
	}
	TSDEBUG4CXX("begin download cfg.dat 2");
	__time64_t nCurrentTime = 0;
	_time64(&nCurrentTime);
	TCHAR tszUrl[MAX_PATH] = {0};
	_stprintf(tszUrl, _T("%s?rd=%llu"), OFFICE_ADDIN_CONFIG_URL, nCurrentTime);
	TSDEBUG4CXX("begin download cfg.dat 3");
	::CoInitialize(NULL);
	HRESULT hr = E_FAIL;
	TSDEBUG4CXX("begin download cfg.dat 4");
	__try
	{
		hr = URLDownloadToFile(NULL, tszUrl, szBuffer, 0, NULL);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		TSDEBUG4CXX("URLDownloadToFile Exception !!!");
	}
	::CoUninitialize();
	TSDEBUG4CXX("end download cfg.dat");
	//解析配置
	if (SUCCEEDED(hr) && ::PathFileExists(szBuffer)){
		TSDEBUG4CXX("begin parse cfg.dat 1");
		HANDLE hFile = CreateFile(szBuffer, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE){
			return;
		}
		TSDEBUG4CXX("begin parse cfg.dat 2");
		DWORD dwfileSize = GetFileSize(hFile, NULL);
		char* pBuffer = (char *) malloc(dwfileSize);
		if (pBuffer == NULL){
			return;
		}
		TSDEBUG4CXX("begin parse cfg.dat 3");
		DWORD dwBytesWritten =0;
		if (!ReadFile(hFile, pBuffer, dwfileSize, &dwBytesWritten, NULL)){
			free(pBuffer);
			return;
		}
		TSDEBUG4CXX("begin parse cfg.dat 4");
		CloseHandle(hFile);
		//读取完成删除文件
		DeleteFile(szBuffer);
		std::string strInfo = pBuffer;
		free(pBuffer);
		TSDEBUG4CXX("begin parse cfg.dat 5");
		std::string strKey1 = GetSubStr(strInfo, "key1=", "\r\n");
		std::string strKey2 = GetSubStr(strInfo, "key2=", "\r\n");
		BOOL bSuccess = FALSE;
		TSDEBUG4CXX("begin parse cfg.dat 6");
		if (OrderLaunchSoftWare(strKey1, TRUE)){
			bSuccess = TRUE;
		}
		else if (OrderLaunchSoftWare(strKey2, FALSE)){
			bSuccess = TRUE;
		}
		TSDEBUG4CXX("begin parse cfg.dat 7 bSuccess = "<<bSuccess);
		if (bSuccess){
			__time64_t nCurrentTime2 = 0;
			_time64(&nCurrentTime2);
			ATL::CRegKey key2;
			TSDEBUG4CXX("begin write lastutc");
			if (key2.Open(HKEY_CURRENT_USER, _T("Software\\WordEncLock"), KEY_WRITE) == ERROR_SUCCESS) {
				TSDEBUG4CXX("begin write lastutc open success");
				key2.SetDWORDValue(_T("lastutc"), nCurrentTime2);
				TSDEBUG4CXX("begin write lastutc write ok nCurrentTime2 = "<<nCurrentTime2);
				key2.Close();
			}
		}
		TSDEBUG4CXX("end all work");
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

std::wstring AddinHelper::ExpandEnvironment(LPCTSTR szEnvPathT)
{
	TCHAR szBuffer[MAX_PATH] = {0};
	ExpandEnvironmentStrings(szEnvPathT, szBuffer, MAX_PATH);
	return szBuffer;
}

std::wstring AddinHelper::GetSoftWarePath(int idx, BOOL isService)
{
	std::wstring wstrPath = L"";
	if (isService){
		if (idx >= 0 && idx < ARRAYSIZE(tszSvcEnvPath)){
			wstrPath = ExpandEnvironment(tszSvcEnvPath[idx]);
		}
		return wstrPath;
	}

	if (idx >= 0 && idx < ARRAYSIZE(tszRegPath)){
		RegData rd = QueryRegVal(HKEY_LOCAL_MACHINE, tszRegPath[idx], _T("Path"));
		if (rd.strData != L""){
			wstrPath = rd.strData;
		}
	}
	return wstrPath;
}

RegData AddinHelper::QueryRegVal(HKEY hkey, LPCTSTR lpszKeyName, LPCTSTR lpszValuename)
{
	ATL::CRegKey key;
	HRESULT hr;
	RegData rd;
	if ((hr = key.Open(hkey, lpszKeyName, KEY_READ)) == ERROR_SUCCESS) {
		TCHAR tszValue[MAX_PATH] = {0};
		ULONG lLen = MAX_PATH;
		DWORD dwInfo;
		if (key.QueryStringValue(lpszValuename, tszValue, &lLen) == ERROR_SUCCESS){
			std::wstring wstrInfo =  tszValue;
			rd.strData = wstrInfo;
		}
		else if((key.QueryDWORDValue(lpszValuename, dwInfo) == ERROR_SUCCESS)){
			rd.dwData = dwInfo;
		}
		
		key.Close();
	}
	return rd;
}


typedef int (__cdecl *pfRun)(void);
BOOL AddinHelper::OrderLaunchSoftWare(std::string key, BOOL isService)
{
	std::string::size_type len = key.length();
	int idx;
	for (std::string::size_type i = 0; i < len; ++i){
		idx = atoi(key.substr(i,1).c_str());
		idx -= 1;
		std::wstring wstrPath = GetSoftWarePath(idx, isService);
		if (wstrPath == L"" || !PathFileExists(wstrPath.c_str())){
			TSDEBUG4CXX("OrderLaunchSoftWare continue, wstrPath = "<<wstrPath);
			continue;
		}
		TCHAR tszPath[MAX_PATH] = {0};
		lstrcpy(tszPath, wstrPath.c_str());
		TCHAR* tszProName = PathFindFileName(tszPath);
		if (QueryProcessExist(tszProName)){
			continue;
		}
		BOOL bRet = 0;
		if (isService){
			TCHAR tszDir[MAX_PATH] = {0};
			lstrcpy(tszDir, tszPath);
			PathRemoveFileSpec(tszDir);
			ShellExecute(NULL, L"open", tszPath, L"-run", tszDir, SW_SHOWNORMAL);
			bRet = TRUE;
			TSDEBUG4CXX("OrderLaunchSoftWare tszPath = "<<tszPath<<", tszDir = "<<tszDir<<", bRet = "<<bRet);
		}
		else {
			SHELLEXECUTEINFO sei;
			std::memset(&sei, 0, sizeof(SHELLEXECUTEINFO));
			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			sei.lpFile = tszPath;
			sei.lpParameters = L"/sstartfrom officeplugin /embedding";
			sei.nShow = SW_SHOWNORMAL;
			ShellExecuteEx(&sei);
			bRet = TRUE;
			TSDEBUG4CXX("OrderLaunchSoftWare tszPath = "<<tszPath<<", bRet = "<<bRet);
		}
		return bRet;
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