#pragma once
#include "baseoperation.h"
#include "..\ImageHelper\KKImageDoc.h"

// 加载文件OP
class CLoadFileOP:
	public CBaseOperation
{
public:
	CLoadFileOP(void);
	~CLoadFileOP(void);
	virtual bool SetParam(lua_State* luaState);
	virtual int Run(COperationTask* pCOperationTask=NULL);
	virtual bool OnOperationComplete(int m_nStatus, int m_nErrorCode);

private:
	wstring m_wstrFilePath;
	CKKImageDoc* m_pDoc;
	bool m_bScale;	// 是否需要缩放
	int m_nWidth;	// 显示区域的宽度
	int m_nHeight;	// 显示区域的高度
	bool m_bAutoRotate;	// 是否自动转正
private:
	//
};
