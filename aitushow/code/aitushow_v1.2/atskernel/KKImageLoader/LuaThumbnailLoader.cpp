#include "StdAfx.h"
#include "../Utility/StringOperation.h"
#include "LuaThumbnailLoader.h"

static XLLRTGlobalAPI LuaThumbnailLoaderFunctions[] = 
{
	{"AttatchLoadCompleteEvent", CLuaThumbnailLoader::AttatchLoadCompleteEvent},
	{"DetatchLoadCompleteEvent", CLuaThumbnailLoader::DetatchLoadCompleteEvent},
	{"LoadThumbnails", CLuaThumbnailLoader::LoadThumbnails},
	{"Clear", CLuaThumbnailLoader::Clear},
	{"__gc", CLuaThumbnailLoader::Release},
	{NULL, NULL}
};

CLuaThumbnailLoader::CLuaThumbnailLoader(void)
{
}

CLuaThumbnailLoader::~CLuaThumbnailLoader(void)
{
	TSINFO4CXX("缩略图加载器对象析构-----------------------");
}

void CLuaThumbnailLoader::RegisterClass(XL_LRT_ENV_HANDLE hEnv)
{
	XLLRT_RegisterClass(hEnv, THUMBNAILSLOADER_LUA_CLASS, LuaThumbnailLoaderFunctions, NULL, 0);
}
int CLuaThumbnailLoader::Release(lua_State* luaState)
{
	CLuaThumbnailLoader** pp = (CLuaThumbnailLoader**)luaL_checkudata(luaState, 1, THUMBNAILSLOADER_LUA_CLASS); 
	if(pp)
	{
		(*pp)->Release();
	}
	return 0;
}

int CLuaThumbnailLoader::AttatchLoadCompleteEvent(lua_State* luaState)
{
	CLuaThumbnailLoader** ppThumbnailsLoader = (CLuaThumbnailLoader**)luaL_checkudata(luaState, 1, THUMBNAILSLOADER_LUA_CLASS);
	if (ppThumbnailsLoader)
	{
		DWORD dwCookie = 0;
		dwCookie = (*ppThumbnailsLoader)->m_ThumbnailLoader.AttatchLoadCompleteEvent(luaState);
		lua_pushnumber(luaState, dwCookie);
		return 1;
	}
	return 0;
}

int CLuaThumbnailLoader::DetatchLoadCompleteEvent(lua_State* luaState)
{
	CLuaThumbnailLoader** ppThumbnailsLoader = (CLuaThumbnailLoader**)luaL_checkudata(luaState, 1, THUMBNAILSLOADER_LUA_CLASS);
	if (ppThumbnailsLoader)
	{
		(*ppThumbnailsLoader)->m_ThumbnailLoader.DetatchLoadCompleteEvent(luaState);
	}
	return 0;
}

// 获取一个图片文件的缩略图, 或一个轻松边框的缩略图
int CLuaThumbnailLoader::LoadThumbnails(lua_State* luaState)
{
	CLuaThumbnailLoader** ppThumbnailsLoader = (CLuaThumbnailLoader**)luaL_checkudata(luaState, 1, THUMBNAILSLOADER_LUA_CLASS);
	if(ppThumbnailsLoader)
	{
		const char* utf8Text = luaL_checkstring(luaState, 2);
		wstring wstrFilePath;
		//xl::text::transcode::UTF8_to_Unicode(utf8Text, strlen(utf8Text), wstrFilePath);
		wstrFilePath = ultra::_UTF2T(utf8Text);
		utf8Text = luaL_checkstring(luaState, 3);
		wstring wstrCacheFilePath;
		//xl::text::transcode::UTF8_to_Unicode(utf8Text, strlen(utf8Text), wstrCacheFilePath);
		wstrCacheFilePath = ultra::_UTF2T(utf8Text);

		int nWidth = luaL_checkint(luaState, 4);
		int nHeight = luaL_checkint(luaState, 5);
		(*ppThumbnailsLoader)->m_ThumbnailLoader.LoadThumbnails(wstrFilePath, wstrCacheFilePath, nWidth, nHeight);
	}
	return 0;
}
// 清空缩略图的获取
int CLuaThumbnailLoader::Clear(lua_State* luaState)
{
	CLuaThumbnailLoader** ppThumbnailsLoader = (CLuaThumbnailLoader**)luaL_checkudata(luaState, 1, THUMBNAILSLOADER_LUA_CLASS);
	if(ppThumbnailsLoader)
	{
		(*ppThumbnailsLoader)->m_ThumbnailLoader.Clear();
	}
	return 0;
}
