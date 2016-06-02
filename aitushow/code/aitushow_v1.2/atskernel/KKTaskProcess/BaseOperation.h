#pragma once
#include "../LuaBase/LuaEventContainer.h"
class COperationTask;

// 操作执行结果
typedef enum{
	KKImg_ErrorCode_SUCCESS = 0,	// 成功
	KKImg_ErrorCode_RESERVED,
	KKImg_ErrorCode_PARAMERROR,	// 参数错误
	KKImg_ErrorCode_NOMEMORY,	// 内存不足
	KKImg_ErrorCode_UNKOWNERROR,	// 未知错误（默认）
}Operation_ErrorCode;


typedef enum{
	KKImg_OperationType_Invalid = 0x0000,
	KKImg_OperationType_LoadImageFile = 0x0001,
	KKImg_OperationType_LoadBitmap = 0x0002,
	KKImg_OperationType_ScaleBitmap = 0x0003,
	KKImg_OperationType_GetExif = 0x0004,
	KKImg_OperationType_SaveDocFile = 0x0005
}OperationType;

// 操作类的基类
class CBaseOperation
{
public:
	CBaseOperation(void);
	virtual ~CBaseOperation(void);

	virtual bool SetParam(lua_State* luaState);		// 不需要设置参数的时候，子类可以不用关心
	virtual int GetParam(lua_State* luaState);		// 获取参数，如果子类不关心里面的参数的话，可以不用管该函数
	virtual int Run(COperationTask* pCOperationTask=NULL) = 0;

	virtual bool OnOperationComplete(int m_nStatus, int m_nErrorCode) = 0;
	virtual bool OnOperationStepComplete(int m_nStatus, int m_nErrorCode);
	virtual int AttachListener(const wstring& wstrEventName, lua_State* luaState, int nIndex, DWORD& dwCookie);
	virtual int DetachListener(const wstring& wstrEventName, DWORD dwCookie);
	int GetOperationId()
	{
		static int index = 0;
		return index++;
	}
	int m_nOperationId;
protected:
	// 操作的属性
	OperationType m_OperationType;	// 操作类型，子类需要关心下，设置成自己的
	wstring m_wstrOperationDes;		// 操作描述，子类需要关心下，设置成自己的
	CLuaMultiEventContainer m_EventContainer;
	
private:
	//
};
