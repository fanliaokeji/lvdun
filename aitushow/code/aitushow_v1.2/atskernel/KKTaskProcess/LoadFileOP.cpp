#include "StdAfx.h"
#include "LoadFileOP.h"
#include "..\KKImageLoader\ImageLoader.h"
#include "..\LuaAPI\LuaKKImageDoc.h"
#include "..\Utility\StringOperation.h"
#include "OperationTask.h"

CLoadFileOP::CLoadFileOP(void)
{
	m_OperationType = KKImg_OperationType_LoadImageFile;
	m_wstrOperationDes = L"加载图片文件";

	m_pDoc = NULL;
	m_bScale = false;
	m_nWidth = 0;
	m_nHeight = 0;
	m_bAutoRotate = false;
}

CLoadFileOP::~CLoadFileOP(void)
{
	TSAUTO();
	TSINFO4CXX("CLoadFileOP Id is --" << m_nOperationId);
}
bool CLoadFileOP::SetParam(lua_State* luaState)
{
	const char* utf8Text = luaL_checkstring(luaState, 2);
	m_wstrFilePath = ultra::_UTF2T(utf8Text);
	m_bScale = lua_toboolean(luaState, 3);
	m_nWidth = luaL_checkint(luaState, 4);
	m_nHeight = luaL_checkint(luaState, 5);
	m_bAutoRotate = lua_toboolean(luaState, 6);
	return true;
}

int CLoadFileOP::Run(COperationTask* pCOperationTask)
{
	TSAUTO();
	TSINFO4CXX("CLoadFileOP Run CurId is " << m_nOperationId);
	CImageLoader loader;
	int nRet = loader.LoadImage(m_wstrFilePath, &(pCOperationTask->m_pTaskProcessor->m_bStop), m_bScale, m_nWidth, m_nHeight, m_bAutoRotate);
	if (nRet == 0)	// 加载成功了
	{
		if (m_pDoc)
		{
			delete m_pDoc;
			m_pDoc = NULL;
		}
		m_pDoc = new CKKImageDoc();

		// 设置位图
		if (loader.GetLoaderType() == KKImg_Type_Gif)	// Gif会有多个位图的，日啊
		{
			m_pDoc->SetDocType(KKImg_Type_Gif);
			XLGP_GIF_HANDLE hGifObj = loader.GetXLGifObj();
			m_pDoc->SetGifObj(hGifObj);
		}
		else
		{
			m_pDoc->SetDocType(loader.GetLoaderType());
			XL_BITMAP_HANDLE hXLBitmap = loader.GetXLBitmap();
			m_pDoc->SetAdaptedBitmap(hXLBitmap);
		}
		// 设置图片Exif信息和图片文件信息
		m_pDoc->SetExifInfo(loader.GetExifInfo());
		m_pDoc->SetImageInfo(loader.GetImageInfo());
	}
	return nRet;
}

bool CLoadFileOP::OnOperationComplete(int m_nStatus, int m_nErrorCode)
{
	ILuaEventEnum* pEventEnum = NULL;
	m_EventContainer.GetEventEnum(_T("OnOperationComplete"), pEventEnum);
	if (pEventEnum )
	{
		CLuaEvent* pEvent = NULL;
		pEventEnum->Reset();
		while (S_OK == pEventEnum->Next(pEvent))
		{
			lua_State* luaState = pEvent->GetLuaState();
			int nowTop = lua_gettop(luaState);
			pEvent->PushFunction();

			string utf8FilePath;
			utf8FilePath = ultra::_T2UTF(m_wstrFilePath);
			if (KKIMG_TASKSTATUS_SUCCESS != m_nStatus)	// 任务执行失败
			{
				lua_pushnil(luaState);
				lua_pushinteger(luaState, m_nErrorCode);
				lua_pushstring(luaState, utf8FilePath.c_str());

			}
			else	// 成功
			{
				CLuaKKImageDoc* pLuaKKImageDocObj = new CLuaKKImageDoc();
				pLuaKKImageDocObj->SetKKImageDoc(m_pDoc);
				XLLRT_PushXLObject(luaState, KKIMAGE_LUADOC_CLASSNAME, (void*)pLuaKKImageDocObj);
				lua_pushinteger(luaState, 0);
				lua_pushstring(luaState, utf8FilePath.c_str());
			}
			pEvent->Call(3, 0);
			lua_settop(luaState, nowTop);
		}
	}
	return true;
}