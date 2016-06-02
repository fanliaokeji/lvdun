#include "StdAfx.h"
#include "LuaThumbnailLoaderFactory.h"
#include "LuaThumbnailLoader.h"

static XLLRTGlobalAPI LuaThumbnailsLoaderFactoryMemberFunctions[] = 
{
	{"CreateLoader", CLuaThumbnailLoaderFactory::CreateLoader},
	{NULL,NULL}
};

CLuaThumbnailLoaderFactory::CLuaThumbnailLoaderFactory(void)
{
}

CLuaThumbnailLoaderFactory::~CLuaThumbnailLoaderFactory(void)
{
}


CLuaThumbnailLoaderFactory* __stdcall CLuaThumbnailLoaderFactory::Instance(void*)
{
	static CLuaThumbnailLoaderFactory* pFactory = NULL;
	if (pFactory == NULL)
	{
		pFactory = new CLuaThumbnailLoaderFactory();
	}
	return pFactory;
}

void CLuaThumbnailLoaderFactory::RegisterObj(XL_LRT_ENV_HANDLE hEnv)
{
	if(hEnv == NULL)
		return ;

	XLLRTObject theObject;
	theObject.ClassName = THUMBNAILSLOADERFACTORY_LUA_CLASS;
	theObject.MemberFunctions = LuaThumbnailsLoaderFactoryMemberFunctions;
	theObject.ObjName = THUMBNAILSLOADERFACTORY_LUA_OBJ;
	theObject.userData = NULL;
	theObject.pfnGetObject = (fnGetObject)CLuaThumbnailLoaderFactory::Instance;

	XLLRT_RegisterGlobalObj(hEnv,theObject);
	CLuaThumbnailLoader::RegisterClass(hEnv);
}
int CLuaThumbnailLoaderFactory::CreateLoader(lua_State* luaState)
{
	CLuaThumbnailLoaderFactory** pObj = (CLuaThumbnailLoaderFactory**)luaL_checkudata(luaState, 1, THUMBNAILSLOADERFACTORY_LUA_CLASS);
	if (pObj != NULL)
	{
		CLuaThumbnailLoader* pLoader = new CLuaThumbnailLoader();
		pLoader->m_ThumbnailLoader.Init();
		XLLRT_PushXLObject(luaState, THUMBNAILSLOADER_LUA_CLASS, (void*)pLoader);
		return 1;
	}
	return 0;
}