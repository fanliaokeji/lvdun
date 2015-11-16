#pragma once
#include <string>
#include <Windows.h>


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
	UINT_PTR m_nTimerID;
	HANDLE m_hMutex;
	std::wstring m_mutexName;
public:
	AddinHelper();
	~AddinHelper();
	bool EnsureOwnerMutex();
	bool BeginTask();
	static unsigned int __stdcall TaskThreadProc(void* arg);
	unsigned int TaskProc();;
	static void HandleLaunch();
private:
	bool IsVistaOrHigher() const;
	static RegData QueryRegVal(HKEY key, LPCTSTR lpszKeyName, LPCTSTR lpszValuename, REGSAM flag =  KEY_READ);
	static BOOL QueryProcessExist(const TCHAR* processname);
	static BOOL IsStartUp();
};
