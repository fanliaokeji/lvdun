#include "stdafx.h"
#include <math.h>
#include "LuaGraphicUtil.h"

#include <XLLuaRuntime.h>


LuaGraphicWindow g_GrapWnd;
//
//LPATSIMAGEINFO LuaGraphicUtil::pImgInfo_Current = NULL;
//LPATSIMAGEINFO LuaGraphicUtil::pImgInfo_Next = NULL;
//LPATSIMAGEINFO LuaGraphicUtil::pImgInfo_Front = NULL;
std::vector<GrapCallbackNode> LuaGraphicUtil::m_allCallBack;

boost::threadpool::pool LuaGraphicUtil::sm_tp(MAX_THREAD_NUM);

LuaGraphicUtil::LuaGraphicUtil(void)
{

}
LuaGraphicUtil::~LuaGraphicUtil(void)
{

}

XLLRTGlobalAPI LuaGraphicUtil::sm_LuaMemberFunctions[] =  
{
	
	{"GetMultiImgInfoByPaths", GetMultiImgInfoByPaths},
	{"ClearRestSchedule", ClearRestSchedule},
	{"GetDirSupportImgPaths", GetDirSupportImgPaths},
	{"RotateImgByAngle", RotateImgByAngle},
	{"AsynSaveXLBitmapToFile", AsynSaveXLBitmapToFile},

	{"AttachListener", AttachListener},
	{"DetachListener", DetachListener},
	{NULL, NULL}
};

LuaGraphicUtil* __stdcall LuaGraphicUtil::Instance(void *)
{
	static LuaGraphicUtil s_instance;
	if (g_GrapWnd.m_hWnd == NULL)
		g_GrapWnd.Create(HWND_MESSAGE);
	//InitThreadPool();
	return &s_instance;
}

void LuaGraphicUtil::RegisterObj(XL_LRT_ENV_HANDLE hEnv)
{
	if (hEnv == NULL)
	{
		return;
	}

	XLLRTObject object;
	object.ClassName = GRAPHIC_UTIL_CLASS;
	object.ObjName = GRAPHIC_UTIL_OBJ;
	object.MemberFunctions = sm_LuaMemberFunctions;
	object.userData = NULL;
	object.pfnGetObject = (fnGetObject)LuaGraphicUtil::Instance;

	XLLRT_RegisterGlobalObj(hEnv, object);
}

int LuaGraphicUtil::AttachEvent(DWORD userdata1,DWORD userdata2,funGrapResultCallBack pfn, const void* pfun)
{
	TSAUTO();
	GrapCallbackNode newNode = {0};
	newNode.pCallBack = pfn;
	newNode.userData1 = userdata1;
	newNode.userData2 = userdata2;
	newNode.luaFunction = pfun;
	int k = m_allCallBack.size();
	m_allCallBack.push_back(newNode);
	TSDEBUG4CXX(" userdata1 : "<<newNode.userData1<<" userData2 : "<< newNode.userData2<<", pfn : "<<pfn << ", pfun : " << pfun);

	return 0;
}

int LuaGraphicUtil::DetachEvent(DWORD userData1, const void* pfun)
{
	TSAUTO();
	std::vector<GrapCallbackNode>::iterator it = m_allCallBack.begin();
	for(; it != m_allCallBack.end(); it++)
	{
		if(it->luaFunction == pfun)
		{
			luaL_unref((lua_State *)(ULONG_PTR)userData1, LUA_REGISTRYINDEX, (int)it->userData2);
			m_allCallBack.erase(it);
			break;
		}
	}
	return 0;
}

void LuaGraphicUtil::LuaListener(DWORD userdata1,DWORD userdata2, const char* pszType, void* pParams)
{
	//TSAUTO();
	USES_CONVERSION;
	lua_State* pLuaState = (lua_State*)(ULONG_PTR)userdata1;
	LONG lnStackTop = lua_gettop(pLuaState);
	lua_rawgeti(pLuaState,LUA_REGISTRYINDEX,userdata2 );
	lua_pushstring(pLuaState, pszType);	

	KTImageInfo* params = (KTImageInfo*)pParams;
	

	lua_newtable(pLuaState);
	lua_pushstring(pLuaState, "xlhBitmap");
	XLUE_PushBitmap(pLuaState, params->m_xlhBitmap);
	lua_settable(pLuaState, -3);
	

	lua_pushstring(pLuaState, "fifType");
	lua_pushnumber(pLuaState, params->m_fifType);
	lua_settable(pLuaState, -3);

	lua_pushstring(pLuaState, "uFileSize");
	lua_pushnumber(pLuaState, params->m_uFileSize);
	lua_settable(pLuaState, -3);

	lua_pushstring(pLuaState, "uWidth");
	lua_pushnumber(pLuaState, params->m_uWidth);
	lua_settable(pLuaState, -3);

	lua_pushstring(pLuaState, "uHeight");
	lua_pushnumber(pLuaState, params->m_uHeight);
	lua_settable(pLuaState, -3);

	lua_pushstring(pLuaState, "szType");
	lua_pushstring(pLuaState, params->m_szType);
	lua_settable(pLuaState, -3);

	lua_pushstring(pLuaState, "szPath");
	lua_pushstring(pLuaState, ultra::_A2UTF(params->m_szPath).c_str());
	lua_settable(pLuaState, -3);
	
	//这里不能释放因为可能有多个监听
	//delete params;

	long lret = XLLRT_LuaCall(pLuaState, 2, 0, NULL);
	ATLASSERT(lret == 0);
	LONG lnStackTop2 = lua_gettop(pLuaState);
	ATLASSERT(lnStackTop2 == lnStackTop);
	return ;
}

int LuaGraphicUtil::AttachListener(lua_State* pLuaState)
{
	TSAUTO();
	LuaGraphicUtil** ppUtil= (LuaGraphicUtil**)luaL_checkudata(pLuaState,1,GRAPHIC_UTIL_CLASS);   
	if(NULL == ppUtil)
	{
		return 0;
	}
	if(!lua_isfunction(pLuaState,2))
	{
		return 0;
	}
	const void* pfun = lua_topointer(pLuaState, 2);
	long functionRef = luaL_ref(pLuaState,LUA_REGISTRYINDEX);
	AttachEvent((DWORD)(ULONG_PTR)pLuaState,functionRef,LuaListener, pfun);		
	return 0;
}

int LuaGraphicUtil::DetachListener(lua_State* pLuaState)
{
	TSAUTO();
	LuaGraphicUtil** ppUtil= (LuaGraphicUtil**)luaL_checkudata(pLuaState,1,GRAPHIC_UTIL_CLASS);   
	if(NULL == ppUtil)
	{
		return 0;
	}
	if(!lua_isfunction(pLuaState,2))
	{
		return 0;
	}
	const void* pfun = lua_topointer(pLuaState, 2);
	DetachEvent((DWORD)(ULONG_PTR)pLuaState, pfun);
	return 0;
}

void LuaGraphicUtil::AddImgTaskToQueue(const char *filepath)
{
	//TSAUTO();
	sm_tp.schedule(boost::bind(GetImgInfoByPath,filepath));
}
//
//XL_BITMAP_HANDLE LuaGraphicUtil::RotateImg(HDC dcSrc,int SrcWidth,int SrcHeight,double angle,double &DstWidth,double &DstHeight)
//{
//	TSAUTO();
//	double x1,x2,x3;
//	double y1,y2,y3;
//	double maxWidth,maxHeight,minWidth,minHeight;
//	double srcX,srcY;
//	double sinA,cosA;
//
//	HDC dcDst;//旋转后的内存设备环境
//	HBITMAP newBitmap;
//	sinA = sin(angle);
//	cosA = cos(angle);
//	x1 = -SrcHeight * sinA;
//	y1 = SrcHeight * cosA;
//	x2 = SrcWidth * cosA - SrcHeight * sinA;
//	y2 = SrcHeight * cosA + SrcWidth * sinA;
//	x3 = SrcWidth * cosA;
//	y3 = SrcWidth * sinA;
//	minWidth = x3>(x1>x2?x2:x1)?(x1>x2?x2:x1):x3;
//	minWidth = minWidth>0?0:minWidth;
//	minHeight = y3>(y1>y2?y2:y1)?(y1>y2?y2:y1):y3;
//	minHeight = minHeight>0?0:minHeight;
//	maxWidth = x3>(x1>x2?x1:x2)?x3:(x1>x2?x1:x2);
//	maxWidth = maxWidth>0?maxWidth:0;
//	maxHeight = y3>(y1>y2?y1:y2)?y3:(y1>y2?y1:y2);
//	maxHeight = maxHeight>0?maxHeight:0;
//	DstWidth = maxWidth - minWidth;
//	DstHeight = maxHeight - minHeight;
//	dcDst = CreateCompatibleDC(dcSrc);
//	newBitmap = CreateCompatibleBitmap(dcSrc,(int)DstWidth,(int)DstHeight);
//	::SelectObject(dcDst,newBitmap);
//
//	for( int I = 0 ;I<DstHeight;I++)
//	{
//		for(int J = 0 ;J< DstWidth;J++)
//		{
//			srcX = (J + minWidth) * cosA + (I + minHeight) * sinA;
//			srcY = (I + minHeight) * cosA - (J + minWidth) * sinA;
//			if( (srcX >= 0) && (srcX <= SrcWidth) &&(srcY >= 0) && (srcY <= SrcHeight))
//			{
//				BitBlt(dcDst, J, I, 1, 1, dcSrc,(int)srcX, (int)srcY, SRCCOPY);
//			}
//		}
//	}
//    XL_BITMAP_HANDLE xlhBitmap = XLGP_ConvertDDBToXLBitmap(dcDst, newBitmap, XLGRAPHIC_CT_ARGB32);
//	::SelectObject(dcDst, NULL);
//	DeleteDC(dcDst);
//	DeleteObject(newBitmap);
//	return xlhBitmap;
//}

int LuaGraphicUtil::GetDirSupportImgPaths(lua_State* pLuaState)
{
	LuaGraphicUtil** ppUtil = (LuaGraphicUtil **)luaL_checkudata(pLuaState, 1, GRAPHIC_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}

	const char* utf8Path = luaL_checkstring(pLuaState, 2);
	if (NULL == utf8Path)
	{
		return 0;
	}
	CComBSTR bstrPath;			
	LuaStringToCComBSTR(utf8Path,bstrPath);

	WIN32_FIND_DATA fd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	TCHAR szSearchPath[MAX_PATH] = {0};
	::PathCombine(szSearchPath, bstrPath.m_str, L"*.*");
	hFind = FindFirstFile(szSearchPath, &fd);
	std::wstring wstrDirLower =  ultra::ToLower(bstrPath.m_str);

	int i = 0;
	lua_newtable(pLuaState);
	while (INVALID_HANDLE_VALUE != hFind)
	{
		if (_tcsicmp(fd.cFileName, _T("..")) && _tcsicmp(fd.cFileName, _T(".")) &&!(FILE_ATTRIBUTE_DIRECTORY&fd.dwFileAttributes))
		{
			std::wstring wstrFileName = ultra::ToLower(fd.cFileName);
			//std::wstring wstrExt = ::PathFindExtension(wstrFileName.c_str());
			std::string::size_type nPos = wstrFileName.find_last_of(L".");
			if (std::string::npos != nPos)
			{
				std::wstring wstrExt = wstrFileName.substr(nPos+1);
				if (IsSupportExt(ultra::_T2A(wstrExt).c_str()))
				{
					lua_newtable(pLuaState);
					TCHAR szLnkFileTmp[MAX_PATH] = {0};
					::PathCombine(szLnkFileTmp, wstrDirLower.c_str(), wstrFileName.c_str());
					std::string strTmp;
					BSTRToLuaString(szLnkFileTmp,strTmp);

					//路径全部为小写
					lua_pushstring(pLuaState, "szPath");
					lua_pushstring(pLuaState, strTmp.c_str());
					lua_settable(pLuaState, -3);

					lua_pushstring(pLuaState, "szExt");
					lua_pushstring(pLuaState, ultra::_T2UTF(wstrExt).c_str());
					lua_settable(pLuaState, -3);

					//上次修改时间为UTC		
					SYSTEMTIME stUTC, stLocal;
					FileTimeToSystemTime(&(fd.ftLastWriteTime), &stUTC);
					SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

					__time64_t fileTime = 0;
					tm time;
					time.tm_year = stLocal.wYear - 1900;
					time.tm_mon = stLocal.wMonth - 1;
					time.tm_mday = stLocal.wDay;
					time.tm_hour = stLocal.wHour;
					time.tm_min = stLocal.wMinute;
					time.tm_sec = stLocal.wSecond;
					fileTime = _mktime64(&time);
					lua_pushstring(pLuaState, "utcLastWriteTime");
					lua_pushnumber(pLuaState, (lua_Number)fileTime);
					lua_settable(pLuaState, -3);

					//文件大小
					lua_pushstring(pLuaState, "uFileSize");
					DWORDLONG dwHighBase = MAXDWORD;  
					dwHighBase += 1;
					DWORDLONG i64FileSize = (fd.nFileSizeHigh * dwHighBase) + fd.nFileSizeLow;
					lua_pushnumber(pLuaState, (lua_Number)i64FileSize);
					lua_settable(pLuaState, -3);

					lua_rawseti(pLuaState, -2, i + 1);
					i++;
				}
			}
		}

		if (FindNextFile(hFind, &fd) == 0)
		{
			break;
		}
	}
	FindClose(hFind);
	return 1;
}


int LuaGraphicUtil::GetMultiImgInfoByPaths(lua_State* pLuaState)
{
	LuaGraphicUtil** ppUtil = (LuaGraphicUtil **)luaL_checkudata(pLuaState, 1, GRAPHIC_UTIL_CLASS);
	if (ppUtil == NULL)
	{
		return 0;
	}
	if (!lua_istable(pLuaState, 2))
	{
		return 0;
	}
	
	lua_pushnil(pLuaState);
	while (lua_next(pLuaState, 2)) 
	{
		const char* utf8FilePath = (const char*)lua_tostring(pLuaState, -1);
		if (NULL != utf8FilePath)
		{
			CComBSTR bstrFilePath;
			LuaStringToCComBSTR(utf8FilePath,bstrFilePath);

			std::string strAnsiPath = "";
			WideStringToAnsiString(bstrFilePath.m_str,strAnsiPath);
			
			strAnsiPath = ultra::ToLower(strAnsiPath);
			int len = strAnsiPath.length();
			char *pFilePath = new char[len+1];
			strcpy_s(pFilePath,len+1,strAnsiPath.c_str());
			AddImgTaskToQueue(pFilePath);
		}
		lua_pop(pLuaState, 1);
	}	
	return 0;
}


int LuaGraphicUtil::ClearRestSchedule(lua_State* pLuaState)
{
	sm_tp.clear();
	return 0;
}

int LuaGraphicUtil::RotateImgByAngle(lua_State* pLuaState)
{
	TSAUTO();
	LuaGraphicUtil** ppUtil= (LuaGraphicUtil**)luaL_checkudata(pLuaState,1,GRAPHIC_UTIL_CLASS);   
	if(NULL == ppUtil)
	{
		return 0;
	}
	XL_BITMAP_HANDLE xlhSrcBitmap;

	//XLGP_CheckBitmap 用完要XLGP_CheckBitmap
	if (!XLGP_CheckBitmap(pLuaState, 2,&xlhSrcBitmap))
	{
		return 0;
	}
	int angle = (int)lua_tointeger(pLuaState, 3);
	
	FIBITMAP *dibSrc = ConvertXLBitmapToFIBitmap(xlhSrcBitmap);
	XL_ReleaseBitmap(xlhSrcBitmap);

	FIBITMAP *dibRotate =  FreeImage_Rotate(dibSrc,angle);
	if (NULL == dibRotate)
	{
		FreeImage_Unload(dibSrc);
		return 0;
	}
	FreeImage_Unload(dibSrc);
	XL_BITMAP_HANDLE xlDstBitmap = ConvertFIBitmapToXLBitmap(dibRotate);
	if (NULL == xlDstBitmap)
	{
		FreeImage_Unload(dibRotate);
		return 0;
	}
	UINT dstWidth  = FreeImage_GetWidth(dibRotate);
	UINT dstHeight = FreeImage_GetHeight(dibRotate);
	FreeImage_Unload(dibRotate);

	XLUE_PushBitmap(pLuaState, xlDstBitmap);
	XL_ReleaseBitmap(xlDstBitmap);
	lua_pushnumber(pLuaState, dstWidth);
	lua_pushnumber(pLuaState, dstHeight);
	return 3;
}

void SaveFreeImageData::GetFileSavePath()
{
	if (m_bCover)
	{
		m_strSavePath = m_strTargetPath;
		return;
	}
	std::string::size_type index = m_strTargetPath.find_last_of("\\");
	if (std::string::npos != index)
	{
		std::string strDir = m_strTargetPath.substr(0,index);
		std::string strFileNameWithExt = m_strTargetPath.substr(index+1);
		index = strFileNameWithExt.find_last_of(".");
		if (std::string::npos != index)
		{
			std::string strFileNameWithOutExt = strFileNameWithExt.substr(0,index);
			std::string strFileExt = strFileNameWithExt.substr(index);

			for (int i = 0;i < 50;i++)
			{
				char szPath[MAX_PATH] = {0};
				strFileNameWithOutExt +="_旋转"; 
				std::string strTmp = strFileNameWithOutExt + strFileExt;
				::PathCombineA(szPath,strDir.c_str(),strTmp.c_str());
				if (!::PathFileExistsA(szPath))
				{
					m_strSavePath = szPath;
					break;
				}
			}
		}
	}
}


void SaveFreeImageData::Work()
{
	BOOL nRet = 1;
	FIBITMAP *dibSrc = FreeImage_Load(m_fif, m_strTargetPath.c_str(), 0);
	if (NULL != dibSrc)
	{
		FIBITMAP *dibRotate =  FreeImage_Rotate(dibSrc,m_angle);
		if (NULL != dibRotate)
		{
			GetFileSavePath();
			if (!m_strSavePath.empty())
			{
				if (FreeImage_Save(m_fif,dibRotate,m_strSavePath.c_str()))
				{
					nRet = 0;
				}
			}
			FreeImage_Unload(dibRotate);
		}
		FreeImage_Unload(dibSrc);

	}
	g_GrapWnd.PostMessage(WM_SAVEFREEIMG, WPARAM(nRet), (LPARAM)this);
}

static unsigned __stdcall SaveXLBitmapProc(LPVOID pThreadParam)
{
	SaveFreeImageData* pData = (SaveFreeImageData*)pThreadParam;
	pData->Work();
	return 0;
}

int LuaGraphicUtil::AsynSaveXLBitmapToFile(lua_State* pLuaState)
{
	TSAUTO();
	LuaGraphicUtil** ppUtil= (LuaGraphicUtil**)luaL_checkudata(pLuaState,1,GRAPHIC_UTIL_CLASS);   
	if(NULL == ppUtil)
	{
		return 0;
	}
	const char* utf8Path = luaL_checkstring(pLuaState, 2);
	FREE_IMAGE_FORMAT fif = (FREE_IMAGE_FORMAT)lua_tointeger(pLuaState, 3);
	int angle = (int)lua_tointeger(pLuaState, 4);
	int iCover = lua_toboolean(pLuaState, 5);
	BOOL bCover = (iCover == 0) ? FALSE : TRUE;
	if (NULL == utf8Path || !lua_isfunction(pLuaState, 6))
	{
		return 0;
	}
	std::string strAnsiPath = ultra::_UTF2A(utf8Path);
	SaveFreeImageData *pData = new SaveFreeImageData(pLuaState,strAnsiPath.c_str(), fif, angle, bCover);
	_beginthreadex(NULL, 0, SaveXLBitmapProc, pData, 0, NULL);
	return 0;
}

//线程函数
void GetImgInfoByPath(const char *filepath)
{
	//TSAUTO();
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	fif = FreeImage_GetFileType(filepath, 0);
	if(fif == FIF_UNKNOWN) {
		fif = FreeImage_GetFIFFromFilename(filepath);
	}
	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {

		FIBITMAP *dib = FreeImage_Load(fif, filepath, 0);
		
		//HDC hdc = CreateDCA("DISPLAY", NULL, NULL, NULL);//为屏幕创建设备描述表
		//HBITMAP hBitmap = CreateDIBitmap(hdc, FreeImage_GetInfoHeader(dib),
		//	CBM_INIT,
		//	FreeImage_GetBits(dib),
		//	FreeImage_GetInfo(dib),
		//	DIB_RGB_COLORS);

		//XL_BITMAP_HANDLE xlhBitmap = XLGP_ConvertDDBToXLBitmap(hdc, hBitmap, XLGRAPHIC_CT_ARGB32);
		XL_BITMAP_HANDLE xlhBitmap =  ConvertFIBitmapToXLBitmap(dib);
		if (NULL != xlhBitmap)
		{
			KTImageInfo* lpImgeInfo = new KTImageInfo;
			lpImgeInfo->m_xlhBitmap = xlhBitmap;
			strcpy_s(lpImgeInfo->m_szPath,MAX_PATH-1,filepath);
			lpImgeInfo->m_uWidth  = FreeImage_GetWidth(dib);
			lpImgeInfo->m_uHeight = FreeImage_GetHeight(dib);
			lpImgeInfo->m_uFileSize = AtsGetFileSize(filepath);
			lpImgeInfo->m_fifType = fif;
			strcpy_s(lpImgeInfo->m_szType,TYPE_LEN-1, ConverFIFToStrExt(fif).c_str());
			g_GrapWnd.PostMessage(WM_GETIMGINFO,0,(LPARAM)lpImgeInfo);
		}
		FreeImage_Unload(dib);
	}
	delete [] filepath;
	filepath = NULL;
}

unsigned AtsGetFileSize(const char *filepath)
{
	UINT size = 0;
	HANDLE handle = CreateFileA(filepath, FILE_READ_EA, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (handle != INVALID_HANDLE_VALUE)
	{
		size = GetFileSize(handle, NULL);
		CloseHandle(handle);
	}
	return size;
}



FIBITMAP* ConvertXLBitmapToFIBitmap(XL_BITMAP_HANDLE xlhBitmap)
{
	//HDC hdc = CreateDCA("DISPLAY", NULL, NULL, NULL);//为屏幕创建设备描述表
	HDC hdc = GetDC(NULL);
	HBITMAP hBitmap = XLGP_ConvertXLBitmapToDIB(xlhBitmap,32);
	if (NULL == hBitmap)
	{
		return NULL;
	}
	BITMAP bm;
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR) &bm);
	FIBITMAP *dib = FreeImage_Allocate(bm.bmWidth, bm.bmHeight, bm.bmBitsPixel);
	// The GetDIBits function clears the biClrUsed and biClrImportant BITMAPINFO members (dont't know why)
	// So we save these infos below. This is needed for palettized images only.
	int nColors = FreeImage_GetColorsUsed(dib);
	int Success = GetDIBits(hdc, hBitmap, 0, FreeImage_GetHeight(dib),FreeImage_GetBits(dib), FreeImage_GetInfo(dib), DIB_RGB_COLORS);
	// restore BITMAPINFO members
	FreeImage_GetInfoHeader(dib)->biClrUsed = nColors;
	FreeImage_GetInfoHeader(dib)->biClrImportant = nColors;

	//资源回收
	//DeleteDC(hdc);
	ReleaseDC(NULL,hdc);
	DeleteObject(hBitmap);
	return dib;
}


XL_BITMAP_HANDLE ConvertFIBitmapToXLBitmap(FIBITMAP* dib)
{
	//HDC hdc = CreateDCA("DISPLAY", NULL, NULL, NULL);//为屏幕创建设备描述表
	HDC hdc = GetDC(NULL);
	HBITMAP hBitmap = CreateDIBitmap(hdc, FreeImage_GetInfoHeader(dib),
		CBM_INIT,
		FreeImage_GetBits(dib),
		FreeImage_GetInfo(dib),
		DIB_RGB_COLORS);
	XL_BITMAP_HANDLE xlhBitmap = XLGP_ConvertDDBToXLBitmap(hdc, hBitmap, XLGRAPHIC_CT_ARGB32);

	// DeleteDC(hdc);
	ReleaseDC(NULL,hdc);
	::DeleteObject(hBitmap);
	return xlhBitmap;
}
