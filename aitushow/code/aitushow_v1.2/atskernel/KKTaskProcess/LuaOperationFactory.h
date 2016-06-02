#pragma once

#define KKIMAGE_OPERATION_FACTORY_CLASSNAME		"KKImage.OperationFactory.Class"	// 
#define KKIMAGE_OPERATION_FACTORY_OBJNAME		"KKImage.OperationFactory"

class CLuaOperationFactory
{
public:
	CLuaOperationFactory(void);
	~CLuaOperationFactory(void);



	static void* __stdcall GetObject(void*);
	static void RegisterSelf(XL_LRT_ENV_HANDLE hEnv);

	static int CreateOperation(lua_State* luaState);
};
