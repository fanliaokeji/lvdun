#pragma once

struct lua_State;
#include ".\\LuaTool.h"

//===============================================
//	CLuaEvent ������Դ�����, �����������ͷ���Դ, ���ܷŵ�vector������

class CLuaEvent
{
public:
	CLuaEvent();
	~CLuaEvent(void);

	HRESULT Initialize(lua_State* pLuaState, long nFunctionRef);

	//�Ӷ�ջ��ȡ��lua����  �˺������ƻ���ջ. 
	//nIndex ������lua��ջ�е�λ��
	HRESULT BuildFromLuaStack(lua_State* pLuaState, int nIndex);

	//�Ӷ�ջ��table��ȡ��lua����  �˺������ƻ���ջ.
	//nIndex table��lua��ջ�е�λ��
	//nKey: ����table�к����õ���key
	HRESULT BuildFromLuaTable(lua_State* pLuaState, int nIndex, int nKey);

	//�Ӷ�ջ��table��ȡ��lua����  �˺������ƻ���ջ
	//nIndex  table��lua��ջ�е�λ��
	//strKey:  ����table�к����õ���key
	HRESULT BuildFromLuaTable(lua_State* pLuaState, int nIndex, const std::wstring& strKey);

	//�Ӷ�ջ��table��ȡ��lua����  �˺������ƻ���ջ
	//ͬBuildFromLuaTable. ֻ��table���ַ���key���Ͳ�ͬ.
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


