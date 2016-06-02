#pragma once
#include "baseoperation.h"

class CLoadBitmapOP :
	public CBaseOperation
{
public:
	CLoadBitmapOP(void);
	~CLoadBitmapOP(void);

	virtual bool SetParam(lua_State* luaState);
	virtual int Run(COperationTask* pCOperationTask=NULL);
	virtual bool OnOperationComplete(int m_nStatus, int m_nErrorCode);

private:
	wstring m_wstrFilePath;
	XL_BITMAP_HANDLE m_hBitmap;
	bool m_bAutoRotate;
private:
	//
};
