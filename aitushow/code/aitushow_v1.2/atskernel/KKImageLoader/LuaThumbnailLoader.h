#pragma once
#include "ThumbnailLoader.h"

#define  THUMBNAILSLOADER_LUA_CLASS "KKImage.ThumbnailLoader.Class"

class CLuaThumbnailLoader
{
public:
	CLuaThumbnailLoader(void);
	~CLuaThumbnailLoader(void);
	static int AttatchLoadCompleteEvent(lua_State* luaState);
	static int DetatchLoadCompleteEvent(lua_State* luaState);
	static int LoadThumbnails(lua_State* luaState);
	static int Clear(lua_State* luaState);

	static int Release(lua_State* luaState);
	static void RegisterClass(XL_LRT_ENV_HANDLE hEnv);

	CThumbnailLoader m_ThumbnailLoader;
private:
	
	void Release()
	{
		delete this;
	}
	//
};
