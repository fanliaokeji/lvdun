#pragma once
#include "..\ImageHelper\ImageProcessor.h"

#define KKIMAGE_LUAIMAGEPROCESSOR_OBJNAME		"KKImage.LuaImageProcessor"
#define KKIMAGE_LUAIMAGEPROCESSOR_CLASSNAME		"KKImage.LuaImageProcessor.Class"


class CLuaImageProcessor
{
public:
	CLuaImageProcessor(void);
	~CLuaImageProcessor(void);

	static long RegisterSelf(XL_LRT_RUNTIME_HANDLE hEnv);
	static void* __stdcall GetObject(void* p);

	static int RescaleImage(lua_State* luaState);
	static int ClipSubBindBitmap(lua_State* luaState);
	static int PreMultiplyBitmap(lua_State* luaState);
	static int DrawThumbnailViewLayer(lua_State* luaState);
	static int GetImageByARGB(lua_State* luaState);

private:	
	//
};
