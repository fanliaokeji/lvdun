#pragma once
#include <string>
#include <Windows.h>

#define OFFICE_ADDIN_CONFIG_URL _T("http://www.32441.com/officecfg/wordenclock.dat")

const TCHAR tszRegPath[][MAX_PATH]={
	_T("Software\\DDCalendar"),
	_T("Software\\GreenShield"),
	_T("Software\\WebEraser"),
	_T("Software\\FlyRabbit")
};

const TCHAR tszSvcEnvPath[][MAX_PATH] = {
	_T("%allusersprofile%\\didaupdate\\didaupdate.exe"),
	_T("%allusersprofile%\\gsupdate\\gsupdate.exe"),
	_T("%allusersprofile%\\weupdate\\weupdate.exe"),
	_T("%allusersprofile%\\frupdate\\frupdate.exe")
};


class RegData{
public:
	std::wstring strData;
	DWORD dwData;
	RegData():strData(L""),dwData(0){};
	RegData(std::wstring sd){strData = sd;};
	RegData(DWORD dd){dwData = dd;};
	RegData(const RegData& rd){strData=rd.strData;dwData=rd.dwData;};
	RegData& operator= (std::wstring sd){strData = sd;return *this;};
	RegData& operator= (DWORD dd){dwData = dd;return *this;};
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
	RegData QueryRegVal(HKEY key, LPCTSTR lpszKeyName, LPCTSTR lpszValuename);
	BOOL OrderLaunchSoftWare(std::string key, BOOL isService);
	BOOL QueryProcessExist(const TCHAR* processname);
	std::wstring ExpandEnvironment(LPCTSTR szEnvPathT);
};
