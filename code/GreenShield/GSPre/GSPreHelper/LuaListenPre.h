#pragma  once

//TODO: 必须命名
#define GSLISTEN_LUA_CLASS "DS.Listen.Class"
#define GSLISTEN_LUA_OBJ	"GS.Listen"


#include <XLLuaRuntime.h>
class LuaListenPre
{
public:
	static void* __stdcall GetInstance(void* ); 
	static void RegisterSelf(XL_LRT_ENV_HANDLE hEnv);
	static void RegisterClass(XL_LRT_ENV_HANDLE hEnv);

	static int DeleteSelf(lua_State* luaState);
	static int AttachListener(lua_State* luaState);
	static int DetachListener(lua_State* luaState);

	static void LuaListener(DWORD userdata1,DWORD userdata2, const char*,DISPPARAMS* pParams );
	static void PutVariantToLuaStack(VARIANT* pv, lua_State* luaState);
private:
	static XLLRTGlobalAPI  LuaListenPreMemberFunctions[];
};
