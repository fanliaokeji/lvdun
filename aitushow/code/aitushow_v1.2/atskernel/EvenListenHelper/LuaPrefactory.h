#ifndef __LPREFACTORY_H__
#define __LPREFACTORY_H__

#include "LuaMsgWnd.h"
//TODO: 必须命名
#define APIL_FACTORY_LUA_CLASS "APIListen.Factory.Class"
#define APIL_FACTORY_LUA_OBJ "APIListen.Factory"

class LuaListenPreFactory
{
public:
	LuaMsgWindow* CreateInstance();
    
public:
	static LuaListenPreFactory* __stdcall Instance(void*);
	static int CreateInstance(lua_State* luaState);
   
public:
	static void RegisterObj(XL_LRT_ENV_HANDLE hEnv);
};

#endif // __LPREFACTORY_H__