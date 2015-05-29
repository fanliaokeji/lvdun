 #pragma once

#include "./MyClass.h"

#define MY_CLASS_LUA_CLASS "HelloBolt.MyClass"
#define MY_CLASS_FACTORY_LUA_CLASS "HelloBolt.MyClass.Factory.Class"
#define MY_CLASS_FACTORY_LUA_OBJ "HelloBolt.MyClass.Factory"

class LuaMyClass
{
public:
    static int Add(lua_State* luaState);
    static int AttachResultListener(lua_State* luaState);
    static int DeleteSelf(lua_State* luaState);

    static void LuaListener(DWORD dwUserData1,DWORD dwUserData2,int nResult);
public:
    static void RegisterClass(XL_LRT_ENV_HANDLE hEnv);
};

//Ϊ���ܴ���MyClass Instance,���붨��һ���೧����
class LuaMyClassFactory
{
public:
    MyClass* CreateInstance();
    
public:
    static LuaMyClassFactory* __stdcall Instance(void*);

    static int CreateInstance(lua_State* luaState);
   
public:
    static void RegisterObj(XL_LRT_ENV_HANDLE hEnv);
};