#ifndef __GSLPREFACTORY_H__
#define __GSLPREFACTORY_H__

#include "CFilterMsgWnd.h"
//TODO: 必须命名
#define GSL_FACTORY_LUA_CLASS "GSListen.Factory.Class"
#define GSL_FACTORY_LUA_OBJ "GSListen.Factory"

class LuaListenPreFactory
{
public:
	CFilterMsgWindow* CreateInstance();
    
public:
	static LuaListenPreFactory* __stdcall Instance(void*);
	static int CreateInstance(lua_State* luaState);
   
public:
	static void RegisterObj(XL_LRT_ENV_HANDLE hEnv);
};

#endif // __GSLPREFACTORY_H__