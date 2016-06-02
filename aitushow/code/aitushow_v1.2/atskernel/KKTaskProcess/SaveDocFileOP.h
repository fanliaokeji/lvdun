#pragma once
#include "baseoperation.h"
#include "..\LuaAPI\LuaKKImageDoc.h"

class CSaveDocFileOP :
	public CBaseOperation
{
public:
	CSaveDocFileOP(void);
	~CSaveDocFileOP(void);

	virtual bool SetParam(lua_State* luaState);
	virtual int Run(COperationTask* pCOperationTask=NULL);
	virtual bool OnOperationComplete(int m_nStatus, int m_nErrorCode);

private:
	CKKImageDoc* m_pDoc;
	wstring m_wstrNewPathFile;
	bool m_bAutoRotate;
};
