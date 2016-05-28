#pragma once

#define GRAPHIC_UTIL_CLASS	"GRAPHIC.Util.Class"
#define GRAPHIC_UTIL_OBJ		"GRAPHIC.Util"

#include "freeimage/FreeImage.h"
#pragma comment(lib,"FreeImage.lib")
#include "../Utility/ImgSupportExt.h"
#include "../Utility/StringOperation.h"
#include "../Utility/LuaAPIHelper.h"
#include "../Utility/LuaHttpUtil.h"

#include "boost/threadpool.hpp"
#include <map>


typedef void (*funGrapResultCallBack) (DWORD userdata1,DWORD userdata2, const char* pszKey,  void* pParams);
struct GrapCallbackNode
{
	funGrapResultCallBack pCallBack;
	DWORD userData1;
	DWORD userData2;
	const void* luaFunction;
};


#define TYPE_LEN 10
class KTImageInfo
{
public:
	KTImageInfo(void)
	{

	}
	~KTImageInfo(void)
	{
		TSTRACEAUTO();
		if (NULL != m_xlhBitmap)
		{
			XL_ReleaseBitmap(m_xlhBitmap);
			m_xlhBitmap = NULL;
		}
	}
public:
	//HBITMAP hBitmap;
	XL_BITMAP_HANDLE m_xlhBitmap;
	FREE_IMAGE_FORMAT m_fifType;
	char m_szType[TYPE_LEN];
	UINT m_uWidth;
	UINT m_uHeight;
	char m_szPath[MAX_PATH];
	UINT m_uFileSize;
};


#define WM_GETIMGINFO WM_USER+201
#define WM_SAVEFREEIMG WM_USER+202

#define MAX_THREAD_NUM  5   //定义线程池最大线程数

void GetImgInfoByPath(const char *filepath);
unsigned AtsGetFileSize(const char *filepath);
FIBITMAP * ConvertXLBitmapToFIBitmap(XL_BITMAP_HANDLE xlhBitmap);
XL_BITMAP_HANDLE ConvertFIBitmapToXLBitmap(FIBITMAP* dib);

class LuaGraphicUtil
{

public:
	LuaGraphicUtil(void);
	~LuaGraphicUtil(void);

private:
	//与Lua交互
	static void LuaListener(DWORD userdata1,DWORD userdata2, const char* pszType, void* pParams);
	static int AttachEvent(DWORD userData1,DWORD userData2,funGrapResultCallBack pfn, const void* pfun);
	static int DetachEvent(DWORD userData1, const void* pfun);
public:
	static void Fire_LuaEvent(const char* pszKey, void* pParams)
	{
		//TSAUTO();
		for(size_t i = 0;i<m_allCallBack.size();i++)
		{
			m_allCallBack[i].pCallBack(m_allCallBack[i].userData1,m_allCallBack[i].userData2, pszKey,pParams);
		}
	}
private:
	static void AddImgTaskToQueue(const char *filepath);
	//static XL_BITMAP_HANDLE RotateImg(HDC dcSrc,int SrcWidth,int SrcHeight,double angle,double &DstWidth,double &DstHeight);

public:
	static LuaGraphicUtil * __stdcall Instance(void *);
	static void RegisterObj(XL_LRT_ENV_HANDLE hEnv);

	

public:
	static int AttachListener(lua_State* pLuaState);
	static int DetachListener(lua_State* pLuaState);

	static int GetDirSupportImgPaths(lua_State* pLuaState);
	static int GetMultiImgInfoByPaths(lua_State* pLuaState);
	static int ClearRestSchedule(lua_State* pLuaState);
	static int RotateImgByAngle(lua_State* pLuaState);
	static int AsynSaveXLBitmapToFile(lua_State* pLuaState);

private:
	static XLLRTGlobalAPI sm_LuaMemberFunctions[];
	static std::vector<GrapCallbackNode> m_allCallBack;
	static boost::threadpool::pool sm_tp;

private:

	ULONG_PTR m_token;
};


struct SaveFreeImageData
{
	LuaCallInfo m_callInfo;
	std::string m_strTargetPath;
	FREE_IMAGE_FORMAT m_fif;
	int m_angle;
	BOOL m_bCover;

	std::string m_strSavePath;
	SaveFreeImageData(lua_State* pLuaState,const char* szPath, FREE_IMAGE_FORMAT fif, int angle,BOOL bCover) : 
	m_strTargetPath(szPath), 
	m_fif(fif), 
	m_angle(angle),
	m_bCover(bCover),
	m_callInfo(pLuaState, luaL_ref(pLuaState, LUA_REGISTRYINDEX))
	{
		TSTRACEAUTO();
	}

	~SaveFreeImageData()
	{
		TSTRACEAUTO();
	}

	void Notify(int nRetCode)
	{

		lua_rawgeti(m_callInfo.GetLuaState(), LUA_REGISTRYINDEX, m_callInfo.GetRefFn());
		
		int iRetCount = 0;
		
		lua_pushinteger(m_callInfo.GetLuaState(), nRetCode);
		++iRetCount;

		std::string strUTF8Path = ultra::_A2UTF(m_strSavePath);
		lua_pushstring(m_callInfo.GetLuaState(), strUTF8Path.c_str());
		++iRetCount;

		XLLRT_LuaCall(m_callInfo.GetLuaState(), iRetCount, 0, L"SaveFreeImageData Callback");
	}

	void Work();
	void GetFileSavePath();
};



class LuaGraphicWindow : public  CWindowImpl<LuaGraphicWindow>
{
	BEGIN_MSG_MAP(LuaGraphicUtil)
		MESSAGE_HANDLER(WM_GETIMGINFO, OnGetImgInfo)
		MESSAGE_HANDLER(WM_SAVEFREEIMG, OnSaveFreeImg)
	END_MSG_MAP()
public:
	LuaGraphicWindow(void)
	{
	}
	~LuaGraphicWindow(void)
	{
	}
protected:
	LRESULT OnGetImgInfo(INT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		//TSAUTO();
		USES_CONVERSION;
		LuaGraphicUtil::Fire_LuaEvent("OnGetImgInfo", (void*)lParam);
		//这个地方来释放new的内存保证所有调用已经完成
		KTImageInfo * params = (KTImageInfo*)lParam;
		delete params;
		lParam = NULL;
		return 0;
	}
	LRESULT OnSaveFreeImg(INT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		SaveFreeImageData* pData = (SaveFreeImageData*) lParam;
		pData->Notify((int) wParam);
		delete pData;
		return 0;
	}
};



