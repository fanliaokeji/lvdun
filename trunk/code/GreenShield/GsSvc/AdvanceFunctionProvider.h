#pragma once
#include <Windows.h>
#include <Shlobj.h>

class AdvanceFunctionProvider {
public:
typedef HRESULT (STDAPICALLTYPE *SHGetKnownFolderPathFuncType)(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR *ppszPath);
private:
	HMODULE hModule;
	SHGetKnownFolderPathFuncType pSHGetKnownFolderPathFunc;
public:
	AdvanceFunctionProvider();
	SHGetKnownFolderPathFuncType GetSHGetKnownFolderPathFunctionPtr() const;
	~AdvanceFunctionProvider();
};
