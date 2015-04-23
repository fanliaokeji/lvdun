#pragma once
#define MiniTP_UTIL_CLASS	"MiniTP.Util.Class"
#define MiniTP_UTIL_OBJ		"MiniTP.Util"
class LuaMiniTPUtil
{
public:
	LuaMiniTPUtil(void);
	~LuaMiniTPUtil(void);
	static void* __stdcall GetInstance(void* ); 
	static void RegisterSelf(XL_LRT_ENV_HANDLE hEnv);
public:
	
	static int MiniTPLoadXLDL(lua_State* luaState);
	static int MiniTPInit(lua_State* luaState);
	static int MiniTPUnInit(lua_State* luaState);
	static int MiniTPTaskCreate(lua_State* luaState);
	static int MiniTPTaskDelete(lua_State* luaState);
	static int MiniTPTaskStart(lua_State* luaState);
	static int MiniTPTaskPause(lua_State* luaState);
	//static int MiniTPTaskQuery(lua_State* luaState);
	static int MiniTPTaskQueryEx(lua_State* luaState);
	static int MiniTPLimitSpeed(lua_State* luaState);
	static int MiniTPLimitUploadSpeed(lua_State* luaState);
	static int MiniTPDelTempFile(lua_State* luaState);
	//static int MiniTPSetProxy(lua_State* luaState);
	static int MiniTPSetUserAgent(lua_State* luaState);
	static int MiniTPGetFileSizeWithUrl(lua_State* luaState);
	static int MiniTPParseThunderPrivateUrl(lua_State* luaState);
	//static int MiniTPSetFileIdAndSize(lua_State* luaState);

private:
	static XLLRTGlobalAPI  LuaMiniTPMemberFunctions[];
};