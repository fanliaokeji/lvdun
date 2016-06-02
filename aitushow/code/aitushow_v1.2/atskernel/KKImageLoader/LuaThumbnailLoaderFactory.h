#pragma once

#define  THUMBNAILSLOADERFACTORY_LUA_CLASS "KKImage.ThumbnailLoader.Factory.Class"
#define  THUMBNAILSLOADERFACTORY_LUA_OBJ "KKImage.ThumbnailLoader.Factory"


class CLuaThumbnailLoaderFactory
{
public:
	CLuaThumbnailLoaderFactory(void);
	~CLuaThumbnailLoaderFactory(void);
	
	static int CreateLoader(lua_State* luaState);
	static void RegisterObj(XL_LRT_ENV_HANDLE hEnv);
	static CLuaThumbnailLoaderFactory* __stdcall Instance(void*);
};
