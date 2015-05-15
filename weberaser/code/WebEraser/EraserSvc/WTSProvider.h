#pragma once
#include <Windows.h>

class WTSProvider {
public:
	typedef BOOL (WINAPI *WTSQueryUserTokenFuncType)(ULONG SessionId, PHANDLE phToken);
private:
	HMODULE hModule;
	WTSQueryUserTokenFuncType pWTSQueryUserTokenFunc;
public:
	WTSProvider();
	WTSQueryUserTokenFuncType GetWTSQueryUserTokenFunctionPtr() const;
	~WTSProvider();
};
