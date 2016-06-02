#ifndef _LUADRAGDROPPROCESSOR_H_
#define _LUADRAGDROPPROCESSOR_H_
#include <XLLuaRuntime.h>
class CLuaDragDropProcessor
{
public:
	CLuaDragDropProcessor(void);
	~CLuaDragDropProcessor(void);
	static long RegisterSelf(XL_LRT_RUNTIME_HANDLE hEnv);
	static void* __stdcall GetObject(void* p);

	//
	static int OnDragEnter(lua_State* luaState);
	static int OnDragQuery(lua_State* luaState);
	static int OnDragOver(lua_State* luaState);
	static int OnDragLeave(lua_State* luaState);
	static int OnDrop(lua_State* luaState);
};
#endif