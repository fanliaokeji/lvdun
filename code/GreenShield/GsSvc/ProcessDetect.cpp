#include "stdafx.h"
#include "ProcessDetect.h"

#include <cstddef>
#include <cwctype>

#include <Windows.h>
#include <TlHelp32.h>

typedef bool ProccessPredFuncionType(const wchar_t* szFileName);

const wchar_t* browserFileNameTable[] = {
	L"chrome.exe",
	L"iexplore.exe",
	L"firefox.exe"
};

bool ProcessDetect::IsBrowerFileName(const wchar_t* szFileName)
{
	for(std::size_t index = 0; index < sizeof(browserFileNameTable) / sizeof(browserFileNameTable[0]); ++index) {
		for(std::size_t i = 0;; ++i) {
			if(std::towlower(szFileName[i]) != browserFileNameTable[index][i]) {
				break;
			}
			if(szFileName[i] == L'\0' && browserFileNameTable[index][i] == L'\0') {
				return true;
			}
			else if(szFileName[i] == L'\0' || browserFileNameTable[index][i] == L'\0') {
				break;
			}
		}
	}
	return false;
}

static bool IsGreenShiledFileName(const wchar_t* szFileName)
{
	const wchar_t* greenShieldFileName = L"greenshield.exe";
	std::size_t i = 0;
	for(;; ++i) {
		if(std::towlower(szFileName[i]) != greenShieldFileName[i]) {
			return false;
		}
		if(szFileName[i] == L'\0' || greenShieldFileName[i] == L'\0') {
			break;
		}
	}
	return szFileName[i] == L'\0' && greenShieldFileName[i] == L'\0';
}

static bool IsGreenShiledSetupFileName(const wchar_t* szFileName)
{
	const wchar_t* setupFileNamePrefix = L"gssetup_";
	for (std::size_t i = 0;; ++i) {
		if (szFileName[i] == L'\0') {
			break;
		}
		if (setupFileNamePrefix[i] == L'\0') {
			return true;
		}
		if (std::towlower(szFileName[i]) != setupFileNamePrefix[i]) {
			break;
		}
	}
	return false;
}

static bool IsGreenShiledOrGreenShiledSetupFileName(const wchar_t* szFileName)
{
	return IsGreenShiledFileName(szFileName) || IsGreenShiledSetupFileName(szFileName);
}

static bool IsProccessRunning(ProccessPredFuncionType pred)
{
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hProcessSnap == INVALID_HANDLE_VALUE) {
		return false;
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if(!Process32First(hProcessSnap, &pe32)) {
		::CloseHandle(hProcessSnap);
		return false;
	}

	bool find = false;
	do {
		if(pred(pe32.szExeFile)) {
			find = true;
			break;
		}
	} while(Process32Next(hProcessSnap, &pe32));

	::CloseHandle(hProcessSnap);
	return find;
}

bool ProcessDetect::IsAnyBrowerRunning()
{
	return IsProccessRunning(&IsBrowerFileName);
}

bool ProcessDetect::IsGreenShieldRunning()
{
	return IsProccessRunning(&IsGreenShiledFileName);
}

bool ProcessDetect::IsGreenShieldSetupRunning()
{
	HANDLE hMutex = ::OpenMutex(SYNCHRONIZE, FALSE, L"Global\\LVDUNSETUP_INSTALL_MUTEX");
	if(hMutex != NULL) {
		::CloseHandle(hMutex);
		hMutex = NULL;
		return true;
	}
	return IsProccessRunning(&IsGreenShiledSetupFileName);
}

bool ProcessDetect::IsGreenShieldOrGreenShieldSetupRunning()
{
	HANDLE hMutex = ::OpenMutex(SYNCHRONIZE, FALSE, L"Global\\LVDUNSETUP_INSTALL_MUTEX");
	if(hMutex != NULL) {
		::CloseHandle(hMutex);
		hMutex = NULL;
		return true;
	}
	return IsProccessRunning(&IsGreenShiledOrGreenShiledSetupFileName);
}
