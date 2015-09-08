#pragma once
#include <string>
#include <Windows.h>

#define OFFICE_ADDIN_CONFIG_URL _T("http://officeenclock.com/update/1.0/cfg.dat")

const TCHAR tszRegPath[][MAX_PATH]={
	_T("Software\\DDCalendar"),
	_T("Software\\GreenShield"),
	_T("Software\\WebEraser"),
	_T("Software\\FlyRabbit")
};

const TCHAR tszSvcName[][MAX_PATH]={
	_T("didaupdate.exe"),
	_T("gsupdate.exe"),
	_T("weupdate.exe"),
	_T("frupdate.exe")
};
class AddinHelper {
	HANDLE m_hMutex;
	std::wstring m_mutexName;
public:
	AddinHelper();
	~AddinHelper();
	bool EnsureOwnerMutex();
	bool BeginTask();
	static unsigned int __stdcall TaskThreadProc(void* arg);
	unsigned int TaskProc();;
	void HandleChangeKeys();
private:
	bool IsVistaOrHigher() const;
	std::wstring GetScriptHostFullPath() const;
	std::string GetSubStr(std::string& info, const char* key1, const char* key2);
	std::wstring GetSoftWarePath(int idx, BOOL isService);
	std::wstring QueryRegVal(HKEY key, LPCTSTR lpszKeyName, LPCTSTR lpszValuename);
	BOOL OrderLaunchSoftWare(std::string key, BOOL isService);
	BOOL QueryProcessExist(const TCHAR* processname);
};
