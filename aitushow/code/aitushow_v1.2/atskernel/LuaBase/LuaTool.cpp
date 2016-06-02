#include "StdAfx.h"
#include "luatool.h"

void LuaPush(lua_State* L, const wchar_t* val)
{
	std::string strValueUtf8;
	wchar_t szValue[MAX_PATH] = {0};
	wcscpy(szValue,val);
	BSTRToLuaString(szValue,strValueUtf8);
	lua_pushstring(L, strValueUtf8.c_str());
}


std::wstring LuaGetString(lua_State* L, int nIndex)
{
	const char* lpszVal = luaL_checkstring(L, nIndex);
	if (lpszVal == NULL)
	{
		return std::wstring(L"");
	}
	CComBSTR bstrRes;
	LuaStringToCComBSTR(lpszVal,bstrRes);
	std::wstring strRes = bstrRes.m_str;
	return strRes;
}