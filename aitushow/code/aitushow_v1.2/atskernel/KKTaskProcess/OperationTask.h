#pragma once
#include "basetask.h"
#include "LuaOperation.h"

class COperationTask :
	public CBaseTask
{
public:
	COperationTask(void);
	~COperationTask(void);

	virtual void HandleTask();
	virtual void OnTaskComplete();
	virtual void OnTaskStepComplete();

	CBaseOperation* m_Operation;

private:
	//
};
