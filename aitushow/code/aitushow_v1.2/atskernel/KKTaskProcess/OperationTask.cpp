#include "StdAfx.h"
#include ".\operationtask.h"

COperationTask::COperationTask(void)
{
	m_Operation = NULL;
}

COperationTask::~COperationTask(void)
{
	TSAUTO();
}
void COperationTask::HandleTask()
{
	TSAUTO();
	m_nStatus = KKIMG_TASKSTATUS_FAIL;
	m_nErrorCode = 0;
	if (m_Operation)
	{
		int nRet = m_Operation->Run(this);
		if (nRet == 0)
		{
			m_nStatus = KKIMG_TASKSTATUS_SUCCESS;
		}
		else
		{
			m_nStatus = KKIMG_TASKSTATUS_FAIL;
			m_nErrorCode = nRet;
		}
	}	
}
void COperationTask::OnTaskComplete()
{
	TSAUTO();
	if (m_Operation)
	{
		m_Operation->OnOperationComplete(m_nStatus, m_nErrorCode);
		delete(this);// 删除任务对象
	}
}
void COperationTask::OnTaskStepComplete()
{
	if (m_Operation)
	{
		m_Operation->OnOperationStepComplete(m_nStatus, m_nErrorCode);
	}
	TSAUTO();
}