#include "StdAfx.h"
#include "LuaOperation.h"
#include "OperationTask.h"


static XLLRTGlobalAPI LuaOperationMemberFunctions[] = 
{
	{"SetParam", CLuaOperation::SetParam},
	{"GetParam", CLuaOperation::GetParam},
	{"Execute", CLuaOperation::Execute},
	{"ExecuteSpecial", CLuaOperation::ExecuteSpecial},
	{"AttachListener", CLuaOperation::AttachListener},
	{"DetachListener", CLuaOperation::DetachListener},
	{"AddRef", CLuaOperation::AddRef},
	{"Release", CLuaOperation::Release},
	{"__gc", CLuaOperation::Release},
	{NULL, NULL}
};


CLuaOperation::CLuaOperation(void)
{
	m_Operation = NULL;
	m_nRef = 1;
}

CLuaOperation::~CLuaOperation(void)
{
	TSAUTO();
	TSINFO4CXX("CLuaOperation-----------------");
	if (m_Operation)
	{
		delete(m_Operation);
		m_Operation = NULL;
	}
}

void CLuaOperation::RegisterClass(XL_LRT_ENV_HANDLE hEnv)
{
	XLLRT_RegisterClass(hEnv, KKIMAGE_OPERATION_CLASSNAME, LuaOperationMemberFunctions, NULL, 0);
}

int CLuaOperation::AddRef(lua_State* luaState)
{
	CLuaOperation** pp = (CLuaOperation**)luaL_checkudata(luaState, 1, KKIMAGE_OPERATION_CLASSNAME); 
	if(pp )
	{
		(*pp)->AddRef();
	}
	return 0;
}
int CLuaOperation::Release(lua_State* luaState)
{
	TSINFO4CXX("CLuaOperation::Release-----------------");
	CLuaOperation** pp = (CLuaOperation**)luaL_checkudata(luaState, 1, KKIMAGE_OPERATION_CLASSNAME); 
	if(pp )
	{
		(*pp)->Release();
	}
	return 0;
}

int CLuaOperation::SetParam(lua_State* luaState)
{
	CLuaOperation** pp = (CLuaOperation**)luaL_checkudata(luaState, 1, KKIMAGE_OPERATION_CLASSNAME);
	if(pp)
	{
		if ((*pp)->m_Operation)
		{
			if((*pp)->m_Operation->SetParam(luaState))
			{
				lua_pushboolean(luaState, 1);
				return 1;
			}
		}
	}
	lua_pushboolean(luaState, 0);
	return 1;
}
int CLuaOperation::GetParam(lua_State* luaState)
{
	CLuaOperation** pp = (CLuaOperation**)luaL_checkudata(luaState, 1, KKIMAGE_OPERATION_CLASSNAME);
	if(pp)
	{
		if ((*pp)->m_Operation)
		{
			return ((*pp)->m_Operation)->GetParam(luaState);
		}
	}
	return 0;
}

int CLuaOperation::Execute(lua_State* luaState)
{
	CLuaOperation** ppLuaOperation = (CLuaOperation**)luaL_checkudata(luaState, 1, KKIMAGE_OPERATION_CLASSNAME);
	if(ppLuaOperation && (*ppLuaOperation)->m_Operation)
	{
		bool bClearTask = lua_toboolean(luaState, 2);
		if (bClearTask)
		{
			g_pTaskProcessor->ClearTask();
		}
		COperationTask* pBaseTask = new COperationTask();
		pBaseTask->m_nTaskType = KKIMG_TASKTYPE_OPERATION;
		pBaseTask->m_Operation = (*ppLuaOperation)->m_Operation;
		pBaseTask->SetTaskProcessor(g_pTaskProcessor);
		// 将任务丢给处理模块处理
		int nTaskId = 0;
		nTaskId = g_pTaskProcessor->AddTask(pBaseTask);
		if (nTaskId != 0)	// 添加任务成功
		{
			lua_pushnumber(luaState, nTaskId);
			return 1;
		}
	}
	return 0;
}
int CLuaOperation::ExecuteSpecial(lua_State* luaState)
{
	CLuaOperation** ppLuaOperation = (CLuaOperation**)luaL_checkudata(luaState, 1, KKIMAGE_OPERATION_CLASSNAME);
	if(ppLuaOperation && (*ppLuaOperation)->m_Operation)
	{
		bool bClearTask = lua_toboolean(luaState, 2);
		if (bClearTask)
		{
			g_pTaskProcessor1->ClearTask();
		}
		COperationTask* pBaseTask = new COperationTask();
		pBaseTask->m_nTaskType = KKIMG_TASKTYPE_OPERATION;
		pBaseTask->m_Operation = (*ppLuaOperation)->m_Operation;
		pBaseTask->SetTaskProcessor(g_pTaskProcessor1);
		// 将任务丢给处理模块处理
		int nTaskId = 0;
		nTaskId = g_pTaskProcessor1->AddTask(pBaseTask);
		if (nTaskId != 0)	// 添加任务成功
		{
			lua_pushnumber(luaState, nTaskId);
			return 1;
		}
	}
	return 0;
}

int CLuaOperation::AttachListener(lua_State* luaState)
{
	CLuaOperation** ppLuaOperation = (CLuaOperation**)luaL_checkudata(luaState, 1, KKIMAGE_OPERATION_CLASSNAME);
	if (ppLuaOperation && (*ppLuaOperation)->m_Operation)
	{
		wstring eventname = LuaGetString(luaState, 2);
		if (eventname != _T(""))
		{
			DWORD cookie;
			HRESULT hret = (*ppLuaOperation)->m_Operation->AttachListener(eventname, luaState, 3, cookie);
			if (SUCCEEDED(hret))
			{
				lua_pushinteger(luaState, cookie);
				return 1;
			}
		}
	}
	return 0;
}
int CLuaOperation::DetachListener(lua_State* luaState)
{
	CLuaOperation** ppLuaOperation = (CLuaOperation**)luaL_checkudata(luaState, 1, KKIMAGE_OPERATION_CLASSNAME);
	if (ppLuaOperation && (*ppLuaOperation)->m_Operation)
	{
		wstring eventname = LuaGetString(luaState, 2);
		if (eventname != _T(""))
		{
			DWORD cookie;
			cookie = lua_tointeger(luaState, 3);
			HRESULT hret = (*ppLuaOperation)->m_Operation->DetachListener(eventname, cookie);
			if (SUCCEEDED(hret))
			{
				lua_pushboolean(luaState, true);
				return 1;
			}
		}
	}
	return 0;
}