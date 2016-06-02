#include "StdAfx.h"
#include "LuaKKImageDoc.h"
#include "..\Utility\StringOperation.h"

CLuaKKImageDoc::CLuaKKImageDoc(void)
{
	m_pKKImageDoc = NULL;
	m_nRef = 1;
}

CLuaKKImageDoc::~CLuaKKImageDoc(void)
{
	if (m_pKKImageDoc)
	{
		delete m_pKKImageDoc;
		m_pKKImageDoc = NULL;
	}
}

static XLLRTGlobalAPI LuaKKDocMemberFunctions[] = 
{
	{"AddRef", CLuaKKImageDoc::AddRef},
	{"Release", CLuaKKImageDoc::Release},
	{"GetSrcBitmapSize", CLuaKKImageDoc::GetSrcBitmapSize},
	{"GetDocType", CLuaKKImageDoc::GetDocType},
	{"SetSrcBitmap", CLuaKKImageDoc::SetSrcBitmap},
	{"GetSrcBitmap", CLuaKKImageDoc::GetSrcBitmap},
	{"ReleaseSrcBitmap", CLuaKKImageDoc::ReleaseSrcBitmap},
	{"GetGifObj", CLuaKKImageDoc::GetGifObj},
	{"GetAdaptedBitmap", CLuaKKImageDoc::GetAdaptedBitmap},
	{"SetExifInfoStatus", CLuaKKImageDoc::SetExifInfoStatus},
	{"GetExifInfoStatus", CLuaKKImageDoc::GetExifInfoStatus},
	{"GetFileName", CLuaKKImageDoc::GetFileName},
	{"SetFileName", CLuaKKImageDoc::SetFileName},
	{"GetFilePath", CLuaKKImageDoc::GetFilePath},
	{"SetFilePath", CLuaKKImageDoc::SetFilePath},
	{"GetFileType", CLuaKKImageDoc::GetFileType},
	{"GetFileSize", CLuaKKImageDoc::GetFileSize},
	{"GetFileImageSize", CLuaKKImageDoc::GetFileImageSize},
	{"SetDateTimeOriginal", CLuaKKImageDoc::SetDateTimeOriginal},
	{"GetDateTimeOriginal", CLuaKKImageDoc::GetDateTimeOriginal},
	{"SetMake", CLuaKKImageDoc::SetMake},
	{"GetMake", CLuaKKImageDoc::GetMake},
	{"SetModel", CLuaKKImageDoc::SetModel},
	{"GetModel", CLuaKKImageDoc::GetModel},
	{"SetLensType", CLuaKKImageDoc::SetLensType},
	{"GetLensType", CLuaKKImageDoc::GetLensType},
	{"SetShutterCount", CLuaKKImageDoc::SetShutterCount},
	{"GetShutterCount", CLuaKKImageDoc::GetShutterCount},
	{"SetShutterSpeed", CLuaKKImageDoc::SetShutterSpeed},
	{"GetShutterSpeed", CLuaKKImageDoc::GetShutterSpeed},
	{"SetFNumber", CLuaKKImageDoc::SetFNumber},
	{"GetFNumber", CLuaKKImageDoc::GetFNumber},
	{"SetFocalLength", CLuaKKImageDoc::SetFocalLength},
	{"GetFocalLength", CLuaKKImageDoc::GetFocalLength},
	{"SetISO", CLuaKKImageDoc::SetISO},
	{"GetISO", CLuaKKImageDoc::GetISO},
	{"SetExposureCompensation", CLuaKKImageDoc::SetExposureCompensation},
	{"GetExposureCompensation", CLuaKKImageDoc::GetExposureCompensation},
	{"SetFlashStatus", CLuaKKImageDoc::SetFlashStatus},
	{"GetFlashStatus", CLuaKKImageDoc::GetFlashStatus},
	{"SetWhiteBalance", CLuaKKImageDoc::SetWhiteBalance},
	{"GetWhiteBalance", CLuaKKImageDoc::GetWhiteBalance},
	{"SetExposureProgram", CLuaKKImageDoc::SetExposureProgram},
	{"GetExposureProgram", CLuaKKImageDoc::GetExposureProgram},
	{"SetMeteringMode", CLuaKKImageDoc::SetMeteringMode},
	{"GetMeteringMode", CLuaKKImageDoc::GetMeteringMode},
	{"ResetRotate", CLuaKKImageDoc::ResetRotate},
	{"LeftRotate", CLuaKKImageDoc::LeftRotate},
	{"RightRotate", CLuaKKImageDoc::RightRotate},
	{"GetRotateAngle", CLuaKKImageDoc::GetRotateAngle},
	{NULL, NULL}
};
void CLuaKKImageDoc::RegisterLuaClass(XL_LRT_ENV_HANDLE hEnv)
{
	XLLRT_RegisterClass(hEnv, KKIMAGE_LUADOC_CLASSNAME, LuaKKDocMemberFunctions, NULL, 0);
}

int CLuaKKImageDoc::AddRef(lua_State* luaState)
{
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		(*pp)->AddRef();
	}
	return 0;
}

int CLuaKKImageDoc::Release(lua_State* luaState)
{
	TSAUTO();
	int nRef = 0;
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		nRef = (*pp)->Release();
	}
	lua_pushinteger(luaState, nRef);
	return 1;
}

int CLuaKKImageDoc::GetAdaptedBitmap(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		XL_BITMAP_HANDLE hXLBitmap = (*pp)->m_pKKImageDoc->GetAdaptedBitmap();
		if (hXLBitmap)
		{
			XLGP_PushBitmap(luaState, hXLBitmap);
			XLBitmapInfo bmpInfo;
			XL_GetBitmapInfo(hXLBitmap, &bmpInfo);
			lua_pushnumber(luaState, bmpInfo.Width);
			lua_pushnumber(luaState, bmpInfo.Height);
			return 3;
		}
	}
	return 0;
}

int CLuaKKImageDoc::GetGifObj(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		XLGP_GIF_HANDLE hGifObj = (*pp)->m_pKKImageDoc->GetGifObj();
		if (hGifObj)
		{
			XLGP_PushGif(luaState, hGifObj);
			return 1;
		}
	}
	return 0;
}

int CLuaKKImageDoc::GetSrcBitmapSize(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		int nWidth, nHeight;
		(*pp)->m_pKKImageDoc->GetSrcBitmapSize(nWidth, nHeight);
		lua_pushnumber(luaState, nWidth);
		lua_pushnumber(luaState, nHeight);
	}
	else
	{
		lua_pushnumber(luaState, 0);
		lua_pushnumber(luaState, 0);
	}
	return 2;
}
int CLuaKKImageDoc::GetDocId(lua_State* luaState)
{
	TSAUTO();
	int nIndex = 0;
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		nIndex = (*pp)->m_pKKImageDoc->GetDocId();
	}
	lua_pushinteger(luaState, nIndex);
	return 1;
}
int CLuaKKImageDoc::GetDocType(lua_State* luaState)
{
	TSAUTO();
	int nDocType = 0; // 默认为未知
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		nDocType = (*pp)->m_pKKImageDoc->GetDocType();
	}
	lua_pushinteger(luaState, nDocType);
	return 1;
}
int CLuaKKImageDoc::SetSrcBitmap(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		XL_BITMAP_HANDLE hBitmap = NULL;
		XLGP_CheckBitmap(luaState, 2, &hBitmap);
		if (hBitmap)
		{
			(*pp)->m_pKKImageDoc->SetSrcBitmap(hBitmap);
		}
	}
	return 0;
}

int CLuaKKImageDoc::GetSrcBitmap(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		XL_BITMAP_HANDLE hBitmap = (*pp)->m_pKKImageDoc->GetSrcBitmap();
		if (hBitmap)
		{
			XLGP_PushBitmap(luaState, hBitmap);
			XLBitmapInfo bmpInfo;
			XL_GetBitmapInfo(hBitmap, &bmpInfo);
			lua_pushnumber(luaState, bmpInfo.Width);
			lua_pushnumber(luaState, bmpInfo.Height);
			return 3;
		}
	}
	return 0;
}
int CLuaKKImageDoc::SetExifInfoStatus(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		int nExifInfoStatus = luaL_checkinteger(luaState, 2);
		(*pp)->m_pKKImageDoc->SetExifInfoStatus(nExifInfoStatus);
	}
	return 0;
}
int CLuaKKImageDoc::GetExifInfoStatus(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		int nExifInfoStatus = (*pp)->m_pKKImageDoc->GetExifInfoStatus();
		lua_pushinteger(luaState, nExifInfoStatus);
	}
	else
	{
		lua_pushinteger(luaState, 0);
	}
	return 1;
}
int CLuaKKImageDoc::ReleaseSrcBitmap(lua_State* luaState)
{
	TSAUTO();
	int nDocType = 0; // 默认为未知
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		(*pp)->m_pKKImageDoc->ReleaseSrcBitmap();
	}
	return 0;
}

int CLuaKKImageDoc::LeftRotate(lua_State* luaState)
{
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if (pp)
	{
		(*pp)->m_pKKImageDoc->RotateBitmap(-90);
	}
	return 0;
}

int CLuaKKImageDoc::RightRotate(lua_State* luaState)
{
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if (pp)
	{
		(*pp)->m_pKKImageDoc->RotateBitmap(90);
	}
	return 0;
}

int CLuaKKImageDoc::ResetRotate(lua_State* luaState)
{
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if (pp)
	{
		(*pp)->m_pKKImageDoc->ResetRotate();
	}
	return 0;
}

int CLuaKKImageDoc::GetRotateAngle(lua_State* luaState)
{
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if (pp)
	{
		int nAngle = (*pp)->m_pKKImageDoc->GetRotateAngle();
		lua_pushinteger(luaState, nAngle);
		return 1;
	}
	return 0;
}


int CLuaKKImageDoc::GetFileName(lua_State* luaState)
{
	TSAUTO();
	string strFileName;
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		wstring wstrFileName = (*pp)->m_pKKImageDoc->GetFileName();
		strFileName = ultra::_T2UTF(wstrFileName);

	}
	lua_pushstring(luaState, strFileName.c_str());
	return 1;
}
int CLuaKKImageDoc::SetFileName(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		const char* utf8Filename = luaL_checkstring(luaState, 2);
		wstring wstrFileName;
		wstrFileName = ultra::_UTF2T(utf8Filename);
		(*pp)->m_pKKImageDoc->SetFileName(wstrFileName);
	}
	return 0;
}

int CLuaKKImageDoc::GetFilePath(lua_State* luaState)
{
	TSAUTO();
	string strFilePath;
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		wstring wstrFilePath = (*pp)->m_pKKImageDoc->GetFilePath();
		strFilePath = ultra::_T2UTF(wstrFilePath);

	}
	lua_pushstring(luaState, strFilePath.c_str());
	return 1;
}

int CLuaKKImageDoc::SetFilePath(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		const char* utf8FilePath = luaL_checkstring(luaState, 2);
		wstring wstrFilePath;
		wstrFilePath = ultra::_UTF2T(utf8FilePath);
		(*pp)->m_pKKImageDoc->SetFilePath(wstrFilePath);
	}
	return 0;
}

int CLuaKKImageDoc::GetFileType(lua_State* luaState)
{
	TSAUTO();
	string strFileType;
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		wstring wstrFileType = (*pp)->m_pKKImageDoc->GetFileType();
		strFileType = ultra::_T2UTF(wstrFileType);
	}
	lua_pushstring(luaState, strFileType.c_str());
	return 1;
}
int CLuaKKImageDoc::GetFileSize(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		int nSize = (*pp)->m_pKKImageDoc->GetFileSize();
		lua_pushinteger(luaState, nSize);
	}
	else
	{
		lua_pushinteger(luaState, 0);
	}
	return 1;
}
int CLuaKKImageDoc::GetFileImageSize(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		int nWidth, nHeight;
		(*pp)->m_pKKImageDoc->GetFileImageSize(nWidth, nHeight);
		lua_pushinteger(luaState, nWidth);
		lua_pushinteger(luaState, nHeight);
	}
	else
	{
		lua_pushinteger(luaState, 0);
		lua_pushinteger(luaState, 0);
	}
	return 2;
}
int CLuaKKImageDoc::SetDateTimeOriginal(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		const char* utf8DateTimeOriginal = luaL_checkstring(luaState, 2);
		wstring wstrDateTimeOriginal;
		wstrDateTimeOriginal = ultra::_UTF2T(utf8DateTimeOriginal);
		(*pp)->m_pKKImageDoc->SetDateTimeOriginal(wstrDateTimeOriginal);
	}
	return 0;
}
int CLuaKKImageDoc::GetDateTimeOriginal(lua_State* luaState)
{
	TSAUTO();
	string strDateTimeOriginal;
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		wstring wstrDateTimeOriginal = (*pp)->m_pKKImageDoc->GetDateTimeOriginal();
		strDateTimeOriginal = ultra::_T2UTF(wstrDateTimeOriginal);
	}
	lua_pushstring(luaState, strDateTimeOriginal.c_str());
	return 1;
}

int CLuaKKImageDoc::SetMake(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		const char* utf8Make = luaL_checkstring(luaState, 2);
		wstring wstrMake;
		wstrMake = ultra::_UTF2T(utf8Make);
		(*pp)->m_pKKImageDoc->SetMake(wstrMake);
	}
	return 0;
}

int CLuaKKImageDoc::GetMake(lua_State* luaState)
{
	string strMake;
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		wstring wstrMake = (*pp)->m_pKKImageDoc->GetMake();
		strMake = ultra::_T2UTF(wstrMake);
	}
	lua_pushstring(luaState, strMake.c_str());
	return 1;		
}


int CLuaKKImageDoc::SetModel(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		const char* utf8Model = luaL_checkstring(luaState, 2);
		wstring wstrModel;
		wstrModel = ultra::_UTF2T(utf8Model);
		(*pp)->m_pKKImageDoc->SetModel(wstrModel);
	}
	return 0;
}

int CLuaKKImageDoc::GetModel(lua_State* luaState)
{
	string strModel;
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		wstring wstrModel = (*pp)->m_pKKImageDoc->GetModel();
		strModel = ultra::_T2UTF(wstrModel);
	}
	lua_pushstring(luaState, strModel.c_str());
	return 1;
}

int CLuaKKImageDoc::SetLensType(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		const char* utf8LensType = luaL_checkstring(luaState, 2);
		wstring wstrLensType;
		wstrLensType = ultra::_UTF2T(utf8LensType);
		(*pp)->m_pKKImageDoc->SetLensType(wstrLensType);
	}
	return 0;
}

int CLuaKKImageDoc::GetLensType(lua_State* luaState)
{
	string strLensType;
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		wstring wstrLensType = (*pp)->m_pKKImageDoc->GetLensType();
		strLensType = ultra::_T2UTF(wstrLensType);
	}
	lua_pushstring(luaState, strLensType.c_str());
	return 1;
}

int CLuaKKImageDoc::SetShutterCount(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		const char* utf8ShutterCount = luaL_checkstring(luaState, 2);
		wstring wstrShutterCount;
		wstrShutterCount = ultra::_UTF2T(utf8ShutterCount);
		(*pp)->m_pKKImageDoc->SetShutterCount(wstrShutterCount);
	}
	return 0;
}

int CLuaKKImageDoc::GetShutterCount(lua_State* luaState)
{
	string strShutterCount;
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		wstring wstrShutterCount = (*pp)->m_pKKImageDoc->GetShutterCount();
		strShutterCount = ultra::_T2UTF(wstrShutterCount);
	}
	lua_pushstring(luaState, strShutterCount.c_str());
	return 1;
}

int CLuaKKImageDoc::SetShutterSpeed(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		const char* utf8ShutterSpeed = luaL_checkstring(luaState, 2);
		wstring wstrShutterSpeed;
		wstrShutterSpeed = ultra::_UTF2T(utf8ShutterSpeed);
		(*pp)->m_pKKImageDoc->SetShutterSpeed(wstrShutterSpeed);
	}
	return 0;
}

int CLuaKKImageDoc::GetShutterSpeed(lua_State* luaState)
{
	string strShutterSpeed;
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		wstring wstrShutterSpeed = (*pp)->m_pKKImageDoc->GetShutterSpeed();
		strShutterSpeed = ultra::_T2UTF(wstrShutterSpeed);
	}
	lua_pushstring(luaState, strShutterSpeed.c_str());
	return 1;
}	

int CLuaKKImageDoc::SetFNumber(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		const char* utf8FNumber = luaL_checkstring(luaState, 2);
		wstring wstrFNumber;
		wstrFNumber = ultra::_UTF2T(utf8FNumber);
		(*pp)->m_pKKImageDoc->SetFNumber(wstrFNumber);
	}
	return 0;
}

int CLuaKKImageDoc::GetFNumber(lua_State* luaState)
{
	string strFNumber;
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if (pp)
	{
		wstring wstrFNumber = (*pp)->m_pKKImageDoc->GetFNumber();
		strFNumber = ultra::_T2UTF(wstrFNumber);
	}
	lua_pushstring(luaState, strFNumber.c_str());
	return 1;
}

int CLuaKKImageDoc::SetFocalLength(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		const char* utf8FocalLength = luaL_checkstring(luaState, 2);
		wstring wstrFocalLength;
		wstrFocalLength = ultra::_UTF2T(utf8FocalLength);
		(*pp)->m_pKKImageDoc->SetFocalLength(wstrFocalLength);
	}
	return 0;
}

int CLuaKKImageDoc::GetFocalLength(lua_State* luaState)
{
	string strFocalLength;
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		wstring wstrFocalLength = (*pp)->m_pKKImageDoc->GetFocalLength();
		strFocalLength = ultra::_T2UTF(wstrFocalLength);
	}
	lua_pushstring(luaState, strFocalLength.c_str());
	return 1;
}

int CLuaKKImageDoc::SetISO(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		const char* utf8ISO = luaL_checkstring(luaState, 2);
		wstring wstrISO;
		wstrISO = ultra::_UTF2T(utf8ISO);
		(*pp)->m_pKKImageDoc->SetISO(wstrISO);
	}
	return 0;
}

int CLuaKKImageDoc::GetISO(lua_State* luaState)
{
	string strISO;
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		wstring wstrISO = (*pp)->m_pKKImageDoc->GetISO();
		strISO = ultra::_T2UTF(wstrISO);
	}
	lua_pushstring(luaState, strISO.c_str());
	return 1;
}

int CLuaKKImageDoc::SetExposureCompensation(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		const char* utf8ExposureCompensation = luaL_checkstring(luaState, 2);
		wstring wstrExposureCompensation;
		wstrExposureCompensation = ultra::_UTF2T(utf8ExposureCompensation);
		(*pp)->m_pKKImageDoc->SetExposureCompensation(wstrExposureCompensation);
	}
	return 0;
}

int CLuaKKImageDoc::GetExposureCompensation(lua_State* luaState)
{
	string strExposureCompensation;
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		wstring wstrExposureCompensation = (*pp)->m_pKKImageDoc->GetExposureCompensation();
		strExposureCompensation = ultra::_T2UTF(wstrExposureCompensation);
	}
	lua_pushstring(luaState, strExposureCompensation.c_str());
	return 1;
}

int CLuaKKImageDoc::SetFlashStatus(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		int nFlashStatus = luaL_checkint(luaState, 2);
		(*pp)->m_pKKImageDoc->SetFlashStatus(nFlashStatus);
	}
	return 0;
}

int CLuaKKImageDoc::GetFlashStatus(lua_State* luaState)
{
	int nFlashStatus = -1;
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		nFlashStatus = (*pp)->m_pKKImageDoc->GetFlashStatus();
	}
	lua_pushinteger(luaState, nFlashStatus);
	return 1;
}

int CLuaKKImageDoc::SetWhiteBalance(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		const char* utf8WhiteBalance = luaL_checkstring(luaState, 2);
		wstring wstrWhiteBalance;
		wstrWhiteBalance = ultra::_UTF2T(utf8WhiteBalance);
		(*pp)->m_pKKImageDoc->SetWhiteBalance(wstrWhiteBalance);
	}
	return 0;
}

int CLuaKKImageDoc::GetWhiteBalance(lua_State* luaState)
{
	string strWhiteBalance;
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		wstring wstrWhiteBalance = (*pp)->m_pKKImageDoc->GetWhiteBalance();
		strWhiteBalance = ultra::_T2UTF(wstrWhiteBalance);
	}
	lua_pushstring(luaState, strWhiteBalance.c_str());
	return 1;
}

int CLuaKKImageDoc::SetExposureProgram(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		const char* utf8ExposureProgram = luaL_checkstring(luaState, 2);
		wstring wstrExposureProgram;
		wstrExposureProgram = ultra::_UTF2T(utf8ExposureProgram);
		(*pp)->m_pKKImageDoc->SetExposureProgram(wstrExposureProgram);
	}
	return 0;
}

int CLuaKKImageDoc::GetExposureProgram(lua_State* luaState)
{
	string strExposureProgram;
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		wstring wstrExposureProgram = (*pp)->m_pKKImageDoc->GetExposureProgram();
		strExposureProgram = ultra::_T2UTF(wstrExposureProgram);
	}
	lua_pushstring(luaState, strExposureProgram.c_str());
	return 1;
}

int CLuaKKImageDoc::SetMeteringMode(lua_State* luaState)
{
	TSAUTO();
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		const char* utf8MeteringMode = luaL_checkstring(luaState, 2);
		wstring wstrMeteringMode;
		wstrMeteringMode = ultra::_UTF2T(utf8MeteringMode);
		(*pp)->m_pKKImageDoc->SetMeteringMode(wstrMeteringMode);
	}
	return 0;
}

int CLuaKKImageDoc::GetMeteringMode(lua_State* luaState)
{
	string strMeteringMode;
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 1, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		wstring wstrMeteringMode = (*pp)->m_pKKImageDoc->GetMeteringMode();
		strMeteringMode = ultra::_T2UTF(wstrMeteringMode);
	}
	lua_pushstring(luaState, strMeteringMode.c_str());
	return 1;
}
