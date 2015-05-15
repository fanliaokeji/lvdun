#include "stdafx.h"
#include "ProcessDetect.h"

#include <cstddef>
#include <cwctype>

#include <Windows.h>
#include <TlHelp32.h>

typedef bool ProccessPredFuncionType(const wchar_t* szFileName);

const wchar_t* browserFileNameTable[] = {
	L"iexplore.exe",
	L"chrome.exe",
	L"sogouexplorer.exe",
	L"twchrome.exe",
	L"baidubrowser.exe",
	L"ucbrowser.exe",
	L"qqbrowser.exe",
	L"liebao.exe",
	L"theworld.exe",
	L"firefox.exe",
	L"iceweasel.exe",
	// L"waterfox.exe",
	L"ruiying.exe",
	L"ttraveler.exe",
	L"taobrowser.exe",
	L"360chrome.exe",
	L"safari.exe",
	L"opera.exe",
	L"360se.exe",
	L"2345explorer.exe",
	L"maxthon.exe",
	L"chromium.exe",
	L"yandex.exe",
	L"qupzilla.exe",
	L"dragon.exe",
	L"115chrome.exe",
	L"114ie.exe",
	L"114web.exe"
};

const wchar_t* browserFileNameEndWithTable[] = {
	L"sleipnir.exe",
	L"webkit.exe",
	L"myie.exe",
	L"browser.exe"
};

bool ProcessDetect::IsBrowerFileName(const wchar_t* szFileName)
{
	// full match
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

	// end with
	for(std::size_t index = 0; index < sizeof(browserFileNameEndWithTable) / sizeof(browserFileNameEndWithTable[0]); ++index) {
		std::size_t len = std::wcslen(browserFileNameEndWithTable[index]);
		std::size_t fileNameLength = std::wcslen(szFileName);
		if(fileNameLength >= len && std::equal(szFileName + fileNameLength - len, szFileName + fileNameLength, browserFileNameEndWithTable[index])) {
			return true;
		}
	}
	return false;
}

static bool IsGreenShiledFileName(const wchar_t* szFileName)
{
	const wchar_t* greenShieldFileName = L"weberaser.exe";
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
	const wchar_t* setupFileNamePrefix = L"weberasersetup_";
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
	HANDLE hMutex = ::OpenMutex(SYNCHRONIZE, FALSE, L"Global\\{F3DAF570-0C73-45e6-8D92-82D300ABBEDF}_INSTALL_MUTEX");
	if(hMutex != NULL) {
		::CloseHandle(hMutex);
		hMutex = NULL;
		return true;
	}
	return IsProccessRunning(&IsGreenShiledSetupFileName);
}

bool ProcessDetect::IsGreenShieldOrGreenShieldSetupRunning()
{
	HANDLE hMutex = ::OpenMutex(SYNCHRONIZE, FALSE, L"Global\\{F3DAF570-0C73-45e6-8D92-82D300ABBEDF}_INSTALL_MUTEX");
	if(hMutex != NULL) {
		::CloseHandle(hMutex);
		hMutex = NULL;
		return true;
	}
	return IsProccessRunning(&IsGreenShiledOrGreenShiledSetupFileName);
}
