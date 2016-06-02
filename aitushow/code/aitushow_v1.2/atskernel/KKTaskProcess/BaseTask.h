#pragma once
class CTaskProcessor;
// 任务状态
enum{
	KKIMG_TASKSTATUS_UNKNOWN = 0,
	KKIMG_TASKSTATUS_SUCCESS,
	KKIMG_TASKSTATUS_FAIL,
	KKIMG_TASKSTATUS_IGNORE
};
// 任务类型
enum{
	KKIMG_TASKTYPE_UNKNOWN = 0,
	KKIMG_TASKTYPE_EXIT,
	KKIMG_TASKTYPE_LOADFILE,
	KKIMG_TASKTYPE_OPERATION
};

class CBaseTask
{
public:
	CBaseTask();
	virtual ~CBaseTask();
	

	// 设置当前任务所在的处理器
	virtual void SetTaskProcessor(CTaskProcessor* pTaskProcessor);
	// 在将任务加入到任务列表之前会调用此函数（该函数在主工作线程中被调用）
	virtual void HandleTask() = 0;
	// 任务完成时会调用此函数（该函数在主界面线程中被调用）
	virtual void OnTaskComplete() = 0;
	// 任务中某一步完成时会调用此函数（该函数在主界面线程中被调用）
	virtual void OnTaskStepComplete();
	
	// 设置忽略标志
	virtual void SetIgnoreFlag(bool bIgnore)
	{
		m_bIgnore = bIgnore;
	}

	int m_nStatus;		// 0 未处理，1 处理成功，2 处理失败 3 被忽略
	int m_nErrorCode;	// 错误码，当m_nStatus等于2时该值有效
	int m_nTaskType;	// 任务类型
	int m_nTaskId;		// 使用者不需要知道这个值
	bool m_bIgnore;		// 是否忽略该任务

	CTaskProcessor* m_pTaskProcessor;	// 处理该任务的任务处理器
private:
	//
};