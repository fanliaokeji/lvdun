#pragma once

struct lua_State;
#include ".\\LuaTool.h"

//===============================================
//	CLuaEvent 类似资源句柄类, 析构函数会释放资源, 不能放到vector容器中

class CLuaEvent
{
public:
	CLuaEvent();
	~CLuaEvent(void);

	HRESULT Initialize(lua_State* pLuaState, long nFunctionRef);

	//从堆栈中取出lua函数  此函数不破坏堆栈. 
	//nIndex 函数在lua堆栈中的位置
	HRESULT BuildFromLuaStack(lua_State* pLuaState, int nIndex);

	//从堆栈的table中取出lua函数  此函数不破坏堆栈.
	//nIndex table在lua堆栈中的位置
	//nKey: 访问table中函数用到的key
	HRESULT BuildFromLuaTable(lua_State* pLuaState, int nIndex, int nKey);

	//从堆栈的table中取出lua函数  此函数不破坏堆栈
	//nIndex  table在lua堆栈中的位置
	//strKey:  访问table中函数用到的key
	HRESULT BuildFromLuaTable(lua_State* pLuaState, int nIndex, const std::wstring& strKey);

	//从堆栈的table中取出lua函数  此函数不破坏堆栈
	//同BuildFromLuaTable. 只是table的字符串key类型不同.
	HRESULT BuildFromLuaTable(lua_State* pLuaState, int nIndex, const std::string& strKey);

	HRESULT Uninitialize();

	HRESULT PushFunction();

	template<typename T>
	inline HRESULT PushParam(const T& param);

	inline HRESULT PushParam(const char* lpszClassName, void* pObject);
	
	inline HRESULT Call(int nParamCount, int nRetureCount);

	inline lua_State* GetLuaState();
private:
    lua_State* m_pLuaState;
    long m_nFunctionRef;
};


HRESULT CLuaEvent::PushParam(const char* lpszClassName, void* pObject)
{
	LuaPush(m_pLuaState, lpszClassName, pObject);
	return S_OK;
}

template<typename T>
HRESULT CLuaEvent::PushParam(const T& param)
{
	LuaPush(m_pLuaState, param);
	return S_OK;
}


HRESULT CLuaEvent::Call(int nParamCount, int nRetureCount)
{
	int nRet = XLLRT_LuaCall(m_pLuaState, nParamCount, nRetureCount, L"CLuaEvent::Call");
    if (nRet != 0)
    {
		const char* pError = NULL;
		pError = lua_tostring(m_pLuaState, -1);
		ATLASSERT(FALSE && pError);
		return S_FALSE;
    }
	return S_OK;
}


lua_State* CLuaEvent::GetLuaState()
{
	return m_pLuaState;
}


