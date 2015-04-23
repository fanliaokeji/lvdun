#pragma once

class CRegisterLuaAPI
{
public:
	CRegisterLuaAPI();
	~CRegisterLuaAPI();

	BOOL Init(LPCTSTR lpCmdLine = NULL, LPVOID lpHookObj = NULL);
private:
};