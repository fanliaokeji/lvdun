#include "StdAfx.h"
#include "ScaleBitmapOP.h"
#include "..\ImageHelper\ImageProcessor.h"
#include "..\Utility\StringOperation.h"
CScaleBitmapOP::CScaleBitmapOP(void)
{
	m_OperationType = KKImg_OperationType_ScaleBitmap;
	m_wstrOperationDes = L"加载位图";

	m_nViewL = 0;
	m_nViewT = 0;
	m_nViewW = 0;
	m_nViewH = 0;
	m_nClipL = 0;
	m_nClipT = 0;
	m_nClipW = 0;
	m_nClipH = 0;
	m_nClipIndex = 0;
	m_hXLBitmap = NULL;
}

CScaleBitmapOP::~CScaleBitmapOP(void)
{
}

bool CScaleBitmapOP::SetParam(lua_State* luaState)
{
	XLGP_CheckBitmap(luaState, 2, &m_hXLBitmap);
	if(!m_hXLBitmap)
	{
		return false;
	}
	int erroeCode = 0;
	lua_pushnil(luaState);
	while (lua_next(luaState, 3)) 
	{
		if(lua_isstring(luaState, -2))
		{
			const char* utf8Key = (const char*)lua_tostring(luaState, -2);
			std::wstring strKey = ultra::_UTF2T(utf8Key);
			if (wcscmp(strKey.c_str(),L"ViewL") == 0 && lua_isnumber(luaState, -1))
			{
				m_nViewL = lua_tonumber(luaState,-1);	
			}
			else if (wcscmp(strKey.c_str(),L"ViewT") == 0 && lua_isnumber(luaState, -1))
			{
				m_nViewT = lua_tonumber(luaState,-1);	
			}
			else if (wcscmp(strKey.c_str(),L"ViewW") == 0 && lua_isnumber(luaState, -1))
			{
				m_nViewW = lua_tonumber(luaState,-1);	
			}
			else if (wcscmp(strKey.c_str(),L"ViewH") == 0 && lua_isnumber(luaState, -1))
			{
				m_nViewH = lua_tonumber(luaState,-1);	
			}
			else if (wcscmp(strKey.c_str(),L"ClipL") == 0 && lua_isnumber(luaState, -1))
			{
				m_nClipL = lua_tonumber(luaState,-1);	
			}
			else if (wcscmp(strKey.c_str(),L"ClipT") == 0 && lua_isnumber(luaState, -1))
			{
				m_nClipT = lua_tonumber(luaState,-1);	
			}
			else if (wcscmp(strKey.c_str(),L"ClipW") == 0 && lua_isnumber(luaState, -1))
			{
				m_nClipW = lua_tonumber(luaState,-1);	
			}
			else if (wcscmp(strKey.c_str(),L"ClipH") == 0 && lua_isnumber(luaState, -1))
			{
				m_nClipH = lua_tonumber(luaState,-1);	
			}
			else if (wcscmp(strKey.c_str(),L"CurIndex") == 0 && lua_isnumber(luaState, -1))
			{
				m_nClipIndex = lua_tonumber(luaState,-1);	
			}
			lua_pop(luaState, 1);
		}
		else
		{
			erroeCode = -1;
		}
	}
	if (erroeCode != 0)
	{
		XL_ReleaseBitmap(m_hXLBitmap);
		return false;
	}
	return true;
}

int CScaleBitmapOP::Run(COperationTask* pCOperationTask)
{
	TSINFO4CXX("缩放OP开始执行");
	// 裁剪
	int nRet = 0;
	CRect rect = CRect(m_nClipL, m_nClipT, m_nClipL+m_nClipW, m_nClipT+m_nClipH);
	XL_BITMAP_HANDLE hClipBitmap =  XL_ClipSubBindBitmap(m_hXLBitmap, &rect);
	if (!hClipBitmap)	// 裁剪失败
	{
		nRet = 1;
		XL_ReleaseBitmap(m_hXLBitmap);
		return nRet;		
	}
	if (m_nViewW != m_nClipW) // 区域大小不相等，缩放
	{
		// 缩放
		XL_BITMAP_HANDLE hScaleBitmap = CImageProcessor::RescaleImage(hClipBitmap, m_nViewW, m_nViewH, FALSE, Gdiplus::InterpolationModeHighQualityBicubic);
		if (!hScaleBitmap)
		{
			nRet = 2;
			XL_ReleaseBitmap(hClipBitmap);
			XL_ReleaseBitmap(m_hXLBitmap);
			return nRet;
		}
		XL_ReleaseBitmap(hClipBitmap);
		XL_ReleaseBitmap(m_hXLBitmap);
		m_hXLBitmap = hScaleBitmap;
	}
	else	// 不必缩放
	{
		XL_BITMAP_HANDLE hNewBitmap = NULL;
		hNewBitmap = XL_CloneBitmap(hClipBitmap);
		if (hNewBitmap)
		{
			XL_ReleaseBitmap(m_hXLBitmap);
			XL_ReleaseBitmap(hClipBitmap);
			m_hXLBitmap = hNewBitmap;
		}
		else
		{
			XL_ReleaseBitmap(m_hXLBitmap);
			m_hXLBitmap = hClipBitmap;
		}
	}	
	return nRet;
}

bool CScaleBitmapOP::OnOperationComplete(int m_nStatus, int m_nErrorCode)
{
	TSINFO4CXX("缩放OP完成 m_nErrorCode：" << m_nErrorCode);
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
			if (KKIMG_TASKSTATUS_SUCCESS != m_nStatus)	// 任务执行失败
			{
				lua_pushinteger(luaState, m_nErrorCode);
				lua_pushnil(luaState);
				lua_pushinteger(luaState, m_nViewL);
				lua_pushinteger(luaState, m_nViewT);
				lua_pushinteger(luaState, m_nViewW);
				lua_pushinteger(luaState, m_nViewH);
				lua_pushinteger(luaState, m_nClipIndex);

				pEvent->Call(7, 0);
				lua_settop(luaState, nowTop);
			}
			else	// 成功
			{
				lua_pushinteger(luaState, 0);
				XLGP_PushBitmap(luaState, m_hXLBitmap);
				XL_ReleaseBitmap(m_hXLBitmap);
				lua_pushinteger(luaState, m_nViewL);
				lua_pushinteger(luaState, m_nViewT);
				lua_pushinteger(luaState, m_nViewW);
				lua_pushinteger(luaState, m_nViewH);
				lua_pushinteger(luaState, m_nClipIndex);
				
				pEvent->Call(7, 0);
				lua_settop(luaState, nowTop);
			}
		}
	}
	return true;
}