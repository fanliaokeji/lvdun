#include "stdafx.h"
#include "CFilterMsgWnd.h"
#include ".\LuaListenPre.h"

void LuaListenPre::LuaListener(DWORD userdata1,DWORD userdata2, const char* pszType, DISPPARAMS* pParams )
{
	TSAUTO();
	USES_CONVERSION;
	lua_State* pLuaState = (lua_State*)(ULONG_PTR)userdata1;
	LONG lnStackTop = lua_gettop(pLuaState);
	lua_rawgeti(pLuaState,LUA_REGISTRYINDEX,userdata2 );
	lua_pushstring(pLuaState, pszType);	
	
	for (int i = 0; i < (int)pParams->cArgs; i++)
	{
		PutVariantToLuaStack(&(pParams->rgvarg[i]), pLuaState);
	}

	long lret = XLLRT_LuaCall(pLuaState, pParams->cArgs+1, 0, NULL);
	//ATLASSERT(lret == 0);
	LONG lnStackTop2 = lua_gettop(pLuaState);
	ATLASSERT(lnStackTop2 == lnStackTop);
	return ;
}


//void LuaXLYYPre::LuaListener(lua_State* luaState = (lua_State*)(ULONG_PTR)userdata1,char* pszFuncName, DISPPARAMS* pParams)
//{
//	XL_LRT_ENV_HANDLE hEnv = XLLRT_GetEnv(NULL);
//	XL_LRT_RUNTIME_HANDLE hRunTime = XLLRT_GetRuntime(hEnv, NULL);
//	lua_State* pLuaState =	XLLRT_GetLuaState(hRunTime);
//
//	lua_State* luaState = (lua_State*)(ULONG_PTR)userdata1;
//
//	long lnStackTop = lua_gettop(pLuaState);
//	lua_getglobal(pLuaState, "FILTER") ;
//
//	long lnSetting = lua_gettop(pLuaState); // FILTER index
//	ATLASSERT(!lua_isnil(pLuaState, -1));
//
//	if(!lua_isnil(pLuaState, -1))
//	{
//		lua_getfield(pLuaState, -1, pszFuncName);
//		ATLASSERT(lua_isfunction(pLuaState, -1));
//		if(lua_isfunction(pLuaState, -1))
//		{
//			lua_pushvalue(pLuaState, lnSetting); // copy FILTER to stack top
//			int args = 0;
//
//			for (int i = 0; i < (int)pParams->cArgs; i++)
//			{
//				PutVariantToLuaStack(&(pParams->rgvarg[i]), pLuaState);
//			}
//			long lRet = XLLRT_LuaCall(pLuaState, 3, 1, NULL);
//			ATLASSERT(lRet == 0);
//		}
//	}
//	lua_settop(pLuaState, lnStackTop);
//	XLLRT_ReleaseRunTime(hRunTime);
//	XLLRT_ReleaseEnv(hEnv);
//	return ;
//}

void LuaListenPre::PutVariantToLuaStack(VARIANT* pv, lua_State* luaState)
{
	switch(pv->vt)
	{
	case VT_I2:
		// 潜规则:一律认为是VARIANT_BOOL
		lua_pushboolean(luaState, (pv->iVal == 0)?0:1);
		break;		
	case VT_UI1:
	case VT_I1:
	case VT_UI2:
	case VT_UINT:
	case VT_INT:
	case VT_UI4:
	case VT_I4:
		lua_pushinteger(luaState,pv->intVal);
		break;
	case VT_BSTR:
		{
			std::string str;
			BSTRToLuaString(pv->bstrVal,str);
			lua_pushlstring(luaState, str.c_str(), str.length());
		}				
		break;
	case VT_PTR:
		lua_pushlightuserdata(luaState, pv->pvRecord);
		break;
	case VT_BOOL:
		lua_pushboolean(luaState, (pv->boolVal == VARIANT_FALSE) ? 0 : 1 );
		break;
	default:
		lua_pushnil(luaState);
		break;
	}
}



int LuaListenPre::AttachListener(lua_State* luaState)
{
	TSAUTO();
	CFilterMsgWindow** ppDSFilterPre= (CFilterMsgWindow**)luaL_checkudata(luaState,1,GSLISTEN_LUA_CLASS);   
	if(ppDSFilterPre)
	{
		if(!lua_isfunction(luaState,2))
		{
			return 0;
		}
		const void* pfun = lua_topointer(luaState, 2);
		long functionRef = luaL_ref(luaState,LUA_REGISTRYINDEX);
		(*ppDSFilterPre)->AttachListener((DWORD)(ULONG_PTR)luaState,functionRef,LuaListener, pfun);		
	}
	return 0;
}

int LuaListenPre::DetachListener(lua_State* luaState)
{
	TSAUTO();
	CFilterMsgWindow** ppDSFilterPre= (CFilterMsgWindow**)luaL_checkudata(luaState,1,GSLISTEN_LUA_CLASS);   
	if(ppDSFilterPre)
	{
		if(!lua_isfunction(luaState,2))
		{
			return 0;
		}
		const void* pfun = lua_topointer(luaState, 2);
		(*ppDSFilterPre)->DetachListener((DWORD)(ULONG_PTR)luaState, pfun);		
	}
	return 0;
}

int LuaListenPre::DeleteSelf(lua_State* luaState)
{
	CFilterMsgWindow** ppDSFilterPre= (CFilterMsgWindow**)luaL_checkudata(luaState,1,GSLISTEN_LUA_CLASS);   
	if(ppDSFilterPre)
	{
		//TODO: 这是要关注的地方
		//delete (*ppXLYYPre);
	}
	return 0;
}

XLLRTGlobalAPI LuaListenPre::LuaListenPreMemberFunctions[] = 
{
	{"AttachListener", LuaListenPre::AttachListener},	
	{"DetachListener", LuaListenPre::DetachListener},	
	{"__gc",LuaListenPre::DeleteSelf},
	{NULL,NULL}
};

void LuaListenPre::RegisterClass(XL_LRT_ENV_HANDLE hEnv)
{
	if(hEnv == NULL)
	{
		return;
	}
	long result = XLLRT_RegisterClass(hEnv,GSLISTEN_LUA_CLASS,LuaListenPreMemberFunctions,NULL,0);
	result;
}