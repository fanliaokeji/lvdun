#pragma once

#pragma warning( disable:4244 )

struct lua_State;

inline void LuaPush(lua_State* L)
{
	lua_pushnil(L);
}

inline void LuaPushBool(lua_State* L, BOOL val)
{
	lua_pushboolean(L, val);
}

inline void LuaPush(lua_State* L, bool val)
{
	lua_pushboolean(L, (int)val);
}


inline void LuaPush(lua_State* L, int val)
{
	lua_pushinteger(L, val);
}

inline void LuaPush(lua_State* L, unsigned int val)
{
	lua_pushinteger(L, val);
}

inline void LuaPush(lua_State* L, long val)
{
	lua_pushinteger(L, val);
}


inline void LuaPush(lua_State* L, unsigned long val)
{
	lua_pushinteger(L, val);
}


inline void LuaPush(lua_State* L, ULONGLONG val)
{
	lua_pushnumber(L, (lua_Number)val);
}

inline void LuaPush(lua_State* L, LONGLONG val)
{
	lua_pushnumber(L, (lua_Number)val);
}

inline void LuaPush(lua_State* L, float val)
{
	lua_pushnumber(L, (lua_Number)val);
}

inline void LuaPush(lua_State* L, double val)
{
	lua_pushnumber(L, val);
}

void LuaPush(lua_State* L, const wchar_t* val);

inline void LuaPush(lua_State* L, const char* val)
{
	lua_pushstring(L, val);
}

inline void LuaPush(lua_State* L, const char* lpszClassName, void* pObject)
{
   XLLRT_PushXLObject(L, lpszClassName, pObject);
}

inline BOOL LuaGetBool(lua_State* L, int nIndex)
{
	return lua_toboolean(L, nIndex);
}

inline long LuaGetInt32(lua_State* L, int nIndex)
{
	return lua_tointeger(L, nIndex);
}

inline LONGLONG LuaGetInt64(lua_State* L, int nIndex)
{
	return (LONGLONG)lua_tonumber(L, nIndex);
}

inline double LuaGetDouble(lua_State* L, int nIndex)
{
	return lua_tonumber(L, nIndex);
}

std::wstring LuaGetString(lua_State* L, int nIndex);


