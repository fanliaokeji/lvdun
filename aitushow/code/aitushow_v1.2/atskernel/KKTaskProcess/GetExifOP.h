#pragma once
#include "baseoperation.h"

class CGetExifOP :
	public CBaseOperation
{
public:
	CGetExifOP(void);
	~CGetExifOP(void);


	virtual bool SetParam(lua_State* luaState);
	virtual int Run(COperationTask* pCOperationTask=NULL);
	virtual bool OnOperationComplete(int m_nStatus, int m_nErrorCode);

private:
	wstring m_wstrFilePath;
	string m_strJosn;
private:
	//
};
