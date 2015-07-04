#pragma once

#include "RegisterLuaAPI.h"

extern HANDLE g_hInst;
class CDDApp
{
public:
	CDDApp(void);
	~CDDApp(void);

public:
	BOOL InitInstance(LPTSTR lpCmdLine = NULL);
	int ExitInstance();
	static int __stdcall LuaErrorHandle(lua_State* luaState,const wchar_t* pExtInfo, const wchar_t* luaErrorString,PXL_LRT_ERROR_STACK pStackInfo);
	std::wstring GetCommandLine();

private:
	BOOL IniEnv(void);
	void InternalLoadXAR();
private:
	CRegisterLuaAPI m_RegisterLuaAPI;
	std::wstring m_strXarPath;
	std::wstring m_strCmdLine;
};