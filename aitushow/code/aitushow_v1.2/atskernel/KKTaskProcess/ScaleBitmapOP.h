#pragma once
#include "baseoperation.h"

class CScaleBitmapOP :
	public CBaseOperation
{
public:
	CScaleBitmapOP(void);
	~CScaleBitmapOP(void);

	virtual bool SetParam(lua_State* luaState);
	virtual int Run(COperationTask* pCOperationTask=NULL);
	virtual bool OnOperationComplete(int m_nStatus, int m_nErrorCode);

private:
	int m_nViewL;
	int m_nViewT;
	int m_nViewW;
	int m_nViewH;
	int m_nClipL;
	int m_nClipT;
	int m_nClipW;
	int m_nClipH;
	int m_nClipIndex;
	XL_BITMAP_HANDLE m_hXLBitmap;
private:
	//
};
