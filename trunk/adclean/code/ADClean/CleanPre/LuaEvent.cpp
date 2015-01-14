#include "stdafx.h"
#include "./LuaEvent.h"
#include "./LuaTool.h"
CLuaEvent::CLuaEvent(void)
{
    m_pLuaState = 0;
    m_nFunctionRef = 0;
}

CLuaEvent::~CLuaEvent(void)
{
	Uninitialize();
}


HRESULT CLuaEvent::Initialize(lua_State* pLuaState, long nFunctionRef)
{
    m_pLuaState = pLuaState;
    m_nFunctionRef = nFunctionRef;
	return S_OK;
}


HRESULT CLuaEvent::BuildFromLuaStack(lua_State* pLuaState, int nIndex)
{	
	//将函数压到栈顶
	if (lua_isfunction(pLuaState, nIndex) == false)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}

    m_pLuaState = pLuaState;

    lua_pushvalue(pLuaState, nIndex);

	//取函数
    m_nFunctionRef = luaL_ref(pLuaState, LUA_REGISTRYINDEX);

	//还原堆栈
	lua_pop(pLuaState, 1);
	return S_OK;
}


HRESULT CLuaEvent::BuildFromLuaTable(lua_State* pLuaState, int nIndex, int nKey)
{
	if (lua_istable(pLuaState, nIndex) == false)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}

    m_pLuaState = pLuaState;

	//将table压倒栈顶
    lua_pushvalue(pLuaState, nIndex);
	
	//压入健
	LuaPush(pLuaState, nKey);
	
	//取出函数
	lua_gettable(pLuaState, -2);
	m_nFunctionRef = luaL_ref(pLuaState, LUA_REGISTRYINDEX);

	//还原堆栈
	lua_pop(pLuaState, 2);
	return S_OK;
}

HRESULT CLuaEvent::BuildFromLuaTable(lua_State* pLuaState, int nIndex, const std::wstring& strKey)
{
	//取出函数
	
	std::string strKeyUTF8;
	wchar_t szKey[MAX_PATH] = {0};
	wcsncpy(szKey,strKey.c_str(),strKey.size());
	BSTRToLuaString(szKey,strKeyUTF8);

	HRESULT hr = S_OK;
	hr = BuildFromLuaTable(pLuaState, nIndex, strKeyUTF8);
	return hr;
}

HRESULT CLuaEvent::BuildFromLuaTable(lua_State* pLuaState, int nIndex, const std::string& strKey)
{
	if (lua_istable(pLuaState, nIndex) == false)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}

    m_pLuaState = pLuaState;

	//将table压倒栈顶
    lua_pushvalue(pLuaState, nIndex);

	//取出函数
	lua_getfield(pLuaState, nIndex, strKey.c_str());
	m_nFunctionRef = luaL_ref(pLuaState, LUA_REGISTRYINDEX);

	//还原堆栈
	lua_pop(pLuaState, 1);
	return S_OK;
}

HRESULT CLuaEvent::Uninitialize()
{
	if (m_nFunctionRef != 0)
	{
        luaL_unref(m_pLuaState, LUA_REGISTRYINDEX, m_nFunctionRef);
		m_pLuaState = NULL;
		m_nFunctionRef = 0;
	}
	return S_OK;
}

HRESULT CLuaEvent::PushFunction()
{
    lua_rawgeti(m_pLuaState, LUA_REGISTRYINDEX, m_nFunctionRef);
	return S_OK;
}




