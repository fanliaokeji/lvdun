#include "StdAfx.h"
#include "SaveDocFileOP.h"
#include "..\KKImageLoader\ImageLoader.h"
#include "..\Utility\StringOperation.h"
CSaveDocFileOP::CSaveDocFileOP(void)
{
	m_OperationType = KKImg_OperationType_SaveDocFile;
	m_wstrOperationDes = L"保存旋转过的图片文件";

	m_pDoc = NULL;
	m_bAutoRotate = false;
}

CSaveDocFileOP::~CSaveDocFileOP(void)
{
}

bool CSaveDocFileOP::SetParam(lua_State* luaState)
{
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 2, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		m_pDoc = (*pp)->m_pKKImageDoc;
	}

	// 获取路径，如果没有路径，那么就认为是覆盖保存，有路径，另存
	const char* utf8 = lua_tostring(luaState, 3);
	if (utf8 != NULL)
	{
		//xl::text::transcode::UTF8_to_Unicode(utf8, strlen(utf8), m_wstrNewPathFile);
		m_wstrNewPathFile = ultra::_UTF2T(utf8);
	}
	m_bAutoRotate = lua_toboolean(luaState, 4);
	return true;
}

int CSaveDocFileOP::Run(COperationTask* pCOperationTask)
{
	int nRet = 0;
	// 执行保存操作
	// 获取当前图片文件的路径
	wstring wstrFilePath = m_pDoc->GetFilePath();
	
	// 获取当前将要保存的路径
	bool bSaveAs = false;
	wstring wstrSavePath = wstrFilePath;
	if (!m_wstrNewPathFile.empty())
	{
		wstrSavePath = m_wstrNewPathFile;
	}
	if (wstrSavePath != wstrFilePath)
	{
		bSaveAs = true;
	}

	// 读取Exif信息
	CImageLoader imageLoader;
	CKKImageEXIF* pKKImageEXIF = imageLoader.LoadImageFileOnlyExif(wstrFilePath.c_str());


	// 判断原图有没有加载进来
	XL_BITMAP_HANDLE hSrcBitmap = m_pDoc->GetSrcBitmap();
	if (hSrcBitmap != NULL)
	{
		// 有原图，那么该原图肯定旋转好了，直接保存就行了
		if(!imageLoader.SaveImage(hSrcBitmap, wstrSavePath.c_str(), pKKImageEXIF, FALSE, 95))
		{
			// 保存失败
			nRet = 1;
		}
	}
	else
	{
		// 没有原图，需要重新加
		if(imageLoader.LoadImage(wstrFilePath, NULL, false, 0, 0, m_bAutoRotate) == 0)
		{
			// 加载成功
			m_pDoc->SetSrcBitmap(imageLoader.GetXLBitmap());
			XL_BITMAP_HANDLE hSrcBitmap = m_pDoc->GetSrcBitmap();
			if(!imageLoader.SaveImage(hSrcBitmap, wstrSavePath.c_str(), pKKImageEXIF, FALSE, 95))
			{
				nRet = 1;
			}
		}
		else
		{
			// 加载失败
			nRet = 1;
		}
	}
	if (bSaveAs) // 如果是另存，把原来的旋转角度和图像重置
	{
		m_pDoc->ResetRotate();
	}
	else	// 保存，把旋转角度重置就可以了
	{
		m_pDoc->ResetRotateAngle();
	}
	return nRet;
}

bool CSaveDocFileOP::OnOperationComplete(int m_nStatus, int m_nErrorCode)
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
			lua_pushinteger(luaState, m_nErrorCode);
			pEvent->Call(1, 0);
			lua_settop(luaState, nowTop);
		}
	}
	return true;
}
