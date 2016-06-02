#pragma once
#include <queue>
#include "BaseTask.h"

// 消息定义
enum{
	TASKPROCESSOR_MSG_TASKCOMPLETE = WM_USER + 1,	// 任务处理器，添加一个任务
	TASKPROCESSOR_MSG_TASKSTEPCOMPLETE
};

typedef queue<CBaseTask*> TaskQueue;

// 任务处理器，这里的任务包括 加载文件，保存文件，处理图片
class CTaskProcessor
{
public:
	CTaskProcessor();
	~CTaskProcessor(void);

	// 初始化函数，主要是创建工作线程 
	bool Init(void);
	// 反初始化
	void UnInit(void);
	
	void ClearTask();
	int AddTask(CBaseTask* baseTask);
public:
	// 临界区，用于互斥访问任务队列
	CRITICAL_SECTION m_CriSection;
	// 事件队列，各线程必须互斥访问它
	TaskQueue	m_TaskQueue;
	// 信号量对象，便于用生产者/消费者模式各个工作线程协调处理任务
	HANDLE m_hSemaphore;
	// 消息窗口句柄
	HWND m_hWnd;
	bool m_bStop;
private:
	static DWORD WINAPI  WorkThreadHandle(LPVOID lpParameter);	// 子工作线程处理函数
	static LRESULT CALLBACK MsgWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	// 消息窗口处理函数
	// 创建工作线程
	bool CreateWorkThread(void);
	// 创建消息窗口
	bool CreateMsgWnd(void);

	// 产生一个任务ID
	int GenerateTaskId();
	// 是否初始化的标志位
	bool m_bInit;
	

	//
};

