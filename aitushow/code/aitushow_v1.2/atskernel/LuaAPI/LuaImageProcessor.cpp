#include "StdAfx.h"
#include "LuaImageProcessor.h"

CLuaImageProcessor::CLuaImageProcessor(void)
{
}

CLuaImageProcessor::~CLuaImageProcessor(void)
{
}

static XLLRTGlobalAPI CLuaImageProcessorMemberFunctions[] =
{
	//自定义算法
	{"RescaleImage", CLuaImageProcessor::RescaleImage},
	{"ClipSubBindBitmap", CLuaImageProcessor::ClipSubBindBitmap},
	{"PreMultiplyBitmap", CLuaImageProcessor::PreMultiplyBitmap},
	{"DrawThumbnailViewLayer", CLuaImageProcessor::DrawThumbnailViewLayer},
	{"GetImageByARGB", CLuaImageProcessor::GetImageByARGB},
	{"ImageLeftRotate", CLuaImageProcessor::ImageLeftRotate},
	{"ImageRightRotate", CLuaImageProcessor::ImageRightRotate},
	{"NULL", NULL}
};
long CLuaImageProcessor::RegisterSelf(XL_LRT_RUNTIME_HANDLE hEnv)
{
	assert(hEnv);
	if(hEnv == NULL)
	{
		return XLLRT_RESULT_ENV_INVALID;
	}

	XLLRTObject theObject;
	theObject.ClassName = KKIMAGE_LUAIMAGEPROCESSOR_CLASSNAME;
	theObject.MemberFunctions = CLuaImageProcessorMemberFunctions;
	theObject.ObjName = KKIMAGE_LUAIMAGEPROCESSOR_OBJNAME;
	theObject.userData = NULL;
	theObject.pfnGetObject = CLuaImageProcessor::GetObject;

	long lRet = XLLRT_RegisterGlobalObj(hEnv,theObject); 
	assert(lRet == XLLRT_RESULT_SUCCESS);
	if (lRet != XLLRT_RESULT_SUCCESS)
	{
		return lRet;
	}
	return lRet;
}

void* CLuaImageProcessor::GetObject(void* p)
{
	static CLuaImageProcessor object;
	return &object;
}
int CLuaImageProcessor::RescaleImage(lua_State* luaState)
{
	TSAUTO();
	XL_BITMAP_HANDLE hXLBitmap;
	XLGP_CheckBitmap(luaState, 2, &hXLBitmap);
	int width = luaL_checkint(luaState, 3);
	int height = luaL_checkint(luaState, 4);
	bool bKeepPersent = lua_toboolean(luaState, 5);
	int nClipStyle = luaL_checkint(luaState, 6);
	if (hXLBitmap != NULL)
	{
		XL_BITMAP_HANDLE xl_hBitmap =  CImageProcessor::RescaleImage(hXLBitmap, width, height, bKeepPersent, (Gdiplus::InterpolationMode)nClipStyle);		

		if (!xl_hBitmap)
		{
			XL_ReleaseBitmap(hXLBitmap);
			return 0;
		}

		XLGP_PushBitmap(luaState, xl_hBitmap);
		XL_ReleaseBitmap(xl_hBitmap);

		XLBitmapInfo bmpInfo;
		XL_GetBitmapInfo(xl_hBitmap, &bmpInfo);
		lua_pushnumber(luaState, bmpInfo.Width);
		lua_pushnumber(luaState, bmpInfo.Height);

		XL_ReleaseBitmap(hXLBitmap);
		return 3;
	}
	XL_ReleaseBitmap(hXLBitmap);
	return 0;
}

int CLuaImageProcessor::ClipSubBindBitmap(lua_State* luaState)
{
	XL_BITMAP_HANDLE hXLBitmap;
	XLGP_CheckBitmap(luaState, 2, &hXLBitmap);
	int nLeft = luaL_checkint(luaState, 3);
	int nTop = luaL_checkint(luaState, 4);
	int nWidth = luaL_checkint(luaState, 5);
	int nHeight = luaL_checkint(luaState, 6);
	if (hXLBitmap)
	{
		XL_BITMAP_HANDLE hSubBitmap = CImageProcessor::ClipSubBindBitmap(hXLBitmap, nLeft, nTop, nWidth, nHeight);
		if (hSubBitmap)
		{
			XLGP_PushBitmap(luaState, hSubBitmap);
			int nRef = XL_ReleaseBitmap(hSubBitmap);

			XLBitmapInfo bmpInfo;
			XL_GetBitmapInfo(hSubBitmap, &bmpInfo);
			lua_pushnumber(luaState, bmpInfo.Width);
			lua_pushnumber(luaState, bmpInfo.Height);
			
			XL_ReleaseBitmap(hXLBitmap);
			return 3;
		}
	}
	XL_ReleaseBitmap(hXLBitmap);
	return 0;
}

int CLuaImageProcessor::PreMultiplyBitmap(lua_State* luaState)
{
	TSAUTO();

	XL_BITMAP_HANDLE hXLBitmap;
	XLGP_CheckBitmap(luaState, 2, &hXLBitmap);
	XL_PreMultiplyBitmap(hXLBitmap);
	XLGP_PushBitmap(luaState, hXLBitmap);
	XL_ReleaseBitmap(hXLBitmap);
	return 1;
}

int CLuaImageProcessor::DrawThumbnailViewLayer(lua_State* luaState)
{
	int nTotalWidth = luaL_checkinteger(luaState, 2);
	int nTotalHeight = luaL_checkinteger(luaState, 3);
	int nLeftPos = luaL_checkinteger(luaState, 4);
	int nTopPos = luaL_checkinteger(luaState, 5);
	int nRectWidth = luaL_checkinteger(luaState, 6);
	int nRectHeight = luaL_checkinteger(luaState, 7);

	XL_BITMAP_HANDLE hXLBitmap = CImageProcessor::DrawThumbnailViewLayer(nTotalWidth, nTotalHeight, nLeftPos, nTopPos, nRectWidth, nRectHeight);
	if (hXLBitmap)
	{
		XL_PreMultiplyBitmap(hXLBitmap);
		XLGP_PushBitmap(luaState, hXLBitmap);
		XLBitmapInfo bmpInfo;
		XL_GetBitmapInfo(hXLBitmap, &bmpInfo);
		lua_pushnumber(luaState, bmpInfo.Width);
		lua_pushnumber(luaState, bmpInfo.Height);
		XL_ReleaseBitmap(hXLBitmap);
		return 3;
	}
	return 0;
}

int CLuaImageProcessor::GetImageByARGB(lua_State* luaState)
{
	TSAUTO();

	int nWidth = luaL_checkint(luaState, 2);
	int nHeight = luaL_checkint(luaState, 3);
	int nAlpha = luaL_checkint(luaState, 4);
	int nRed = luaL_checkint(luaState, 5);
	int nGreen = luaL_checkint(luaState, 6);
	int nBlue = luaL_checkint(luaState, 7);

	XL_BITMAP_HANDLE xl_hBitmap =  CImageProcessor::GetImageByARGB(nWidth, nHeight, nAlpha, nRed, nGreen, nBlue);
	if (xl_hBitmap != NULL)
	{
		XLGP_PushBitmap(luaState, xl_hBitmap);
		XL_ReleaseBitmap(xl_hBitmap);

		XLBitmapInfo bmpInfo;
		XL_GetBitmapInfo(xl_hBitmap, &bmpInfo);
		lua_pushnumber(luaState, bmpInfo.Width);
		lua_pushnumber(luaState, bmpInfo.Height);

		return 3;
	}
	return 0;
}

int CLuaImageProcessor::ImageLeftRotate(lua_State* luaState)
{
	XL_BITMAP_HANDLE hBitmap = NULL;
	if(XLGP_CheckBitmap(luaState, 2, &hBitmap)){
		XL_BITMAP_HANDLE hNewBitmap = CImageProcessor::ImageLeftRotate(hBitmap);
		if (hNewBitmap){
			XL_ReleaseBitmap(hBitmap);
		}
		XLGP_PushBitmap(luaState, hNewBitmap);
	}
	else {
		lua_pushnil(luaState);
	}
	return 1;
}

int CLuaImageProcessor::ImageRightRotate(lua_State* luaState)
{
	XL_BITMAP_HANDLE hBitmap = NULL;
	if(XLGP_CheckBitmap(luaState, 2, &hBitmap)){
		XL_BITMAP_HANDLE hNewBitmap = CImageProcessor::ImageRightRotate(hBitmap);
		if (hNewBitmap){
			XL_ReleaseBitmap(hBitmap);
		}
		XLGP_PushBitmap(luaState, hNewBitmap);
	}
	else {
		lua_pushnil(luaState);
	}
	return 1;
}