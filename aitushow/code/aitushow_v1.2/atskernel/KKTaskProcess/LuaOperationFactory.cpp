#include "StdAfx.h"
#include ".\luaoperationfactory.h"
#include "LuaOperation.h"
#include "OperationFactory.h"

static XLLRTGlobalAPI LuaOperationFactoryMemberFunctions[] = 
{
	{"CreateOperation", CLuaOperationFactory::CreateOperation},
	{NULL, NULL}
};


CLuaOperationFactory::CLuaOperationFactory(void)
{
}

CLuaOperationFactory::~CLuaOperationFactory(void)
{
}
void * __stdcall CLuaOperationFactory::GetObject(void*)
{
	static CLuaOperationFactory object;
	return &object;
}

void CLuaOperationFactory::RegisterSelf(XL_LRT_ENV_HANDLE hEnv)
{
	if(hEnv == NULL)
	{
		return;
	}

	XLLRTObject theObject;
	theObject.ClassName = KKIMAGE_OPERATION_FACTORY_CLASSNAME;
	theObject.MemberFunctions = LuaOperationFactoryMemberFunctions;
	theObject.ObjName = KKIMAGE_OPERATION_FACTORY_OBJNAME;
	theObject.userData = NULL;
	theObject.pfnGetObject = CLuaOperationFactory::GetObject;
	XLLRT_RegisterGlobalObj(hEnv, theObject);

	CLuaOperation::RegisterClass(hEnv);
}

int CLuaOperationFactory::CreateOperation(lua_State* luaState)
{
	CLuaOperationFactory** pObj = (CLuaOperationFactory**)luaL_checkudata(luaState, 1, KKIMAGE_OPERATION_FACTORY_CLASSNAME);
	if (pObj != NULL)
	{
		int nOPType = luaL_checkint(luaState, 2);
		CLuaOperation* pLuaOpcertion = new 	CLuaOperation();
		pLuaOpcertion->m_Operation = COperationFactory::GetInstance().CreateOperation(nOPType);
		XLLRT_PushXLObject(luaState, KKIMAGE_OPERATION_CLASSNAME, (void*)pLuaOpcertion);
		return 1;
	}
	return 0;
}
