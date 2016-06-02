#include "StdAfx.h"
#include ".\taskprocessor.h"

CTaskProcessor::CTaskProcessor()
{
	m_bStop = false;
	m_bInit = false;
	m_hWnd = NULL;
	m_hSemaphore = NULL;
}

CTaskProcessor::~CTaskProcessor(void)
{
}

int CTaskProcessor::GenerateTaskId()
{
	TSAUTO();
	static int nCookie = 0;
	return ++nCookie;
}
bool CTaskProcessor::Init(void)
{
	TSAUTO();
	// 如果已经初始化了，就直接返回为true
	if (m_bInit)
	{
		TSINFO4CXX(L"CTaskProcessor has been Init and reture true");
		return true;
	}

	// 创建信号量内核对象，用户同步多个子任务
	m_hSemaphore = CreateSemaphore(NULL, 0, 32767, NULL);
	if (m_hSemaphore == NULL)
	{
		UnInit();
		TSERROR4CXX(L"CreateSemaphore failed");
	}

	// 创建消息窗口
	if (!CreateMsgWnd())
	{
		UnInit();
		TSERROR4CXX(L"CreateMsgWnd failed");
		return false;
	}

	// 创建工作线程
	if (!CreateWorkThread())
	{
		UnInit();
		TSERROR4CXX(L"CreateWorkThread failed");
		return false;
	}
	// 初始化临界区结构体
	InitializeCriticalSection(&m_CriSection);

	// 初始化成功
	m_bInit = true;
	TSINFO4CXX(L"CTaskProcessor Init success");
	return true;
}

void CTaskProcessor::UnInit(void)
{
	TSAUTO();
	if (m_bInit)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;

		assert(m_hSemaphore);
		CloseHandle(m_hSemaphore);
		m_hSemaphore = NULL;
		DeleteCriticalSection(&m_CriSection);
	}
	else
	{
		if (m_hSemaphore != NULL)
		{
			CloseHandle(m_hSemaphore);
			m_hSemaphore = NULL;
		}
		if (m_hWnd)
		{
			DestroyWindow(m_hWnd);
			m_hWnd = NULL;
		}
	}
	m_bInit = false;
}

bool CTaskProcessor::CreateMsgWnd()
{
	static bool bRegisterClass = false;
	static int nWndIndex = 0;
	const HINSTANCE hInst = ::GetModuleHandle(NULL);
	wstring strClassName = L"{9E18143F-FAF9-4438-8FC1-32371276AED3}__KKIMAGE_CLASS"; 
	if (!bRegisterClass)	// 如果没有注册过窗口类，就先注册下
	{
		WNDCLASS wc;
		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = CTaskProcessor::MsgWndProc; 
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = hInst;
		wc.hIcon         = ::LoadIcon(NULL, IDI_APPLICATION); 
		wc.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = strClassName.c_str();

		if (::RegisterClass(&wc) == 0)
		{
			return false;
		}
		bRegisterClass = true;
	}
	
	wstring strWndName = L"{9E18143F-FAF9-4438-8FC1-32371276AED3}__KKIMAGE_TITLE";
	wchar_t wszWndIndex[10];
	wmemset(wszWndIndex, 0, 10);
	nWndIndex++;
	wsprintf(wszWndIndex, L"_%d", nWndIndex);
	strWndName = strWndName + wszWndIndex;
	m_hWnd = ::CreateWindow(strClassName.c_str(), strWndName.c_str(), WS_OVERLAPPEDWINDOW, 
		0, 0, 0, 0, NULL, NULL, hInst, this);
	if(m_hWnd == NULL)
	{
		return false;
	}
	::ShowWindow(m_hWnd, SW_HIDE);
	return true;
}

bool CTaskProcessor::CreateWorkThread(void)
{
	TSAUTO();
	// 创建工作线程
	DWORD dwThreadId;
	HANDLE hThread = ::CreateThread(NULL, 0, CTaskProcessor::WorkThreadHandle, this, 0, &dwThreadId);
	if(INVALID_HANDLE_VALUE == hThread)
	{
		TSERROR4CXX(L"CreateThread failed!");
		return false;
	}
	CloseHandle(hThread);
	return true;
}

int CTaskProcessor::AddTask(CBaseTask* pBaseTask)
{
	TSAUTO();
	if (!m_bInit)
	{
		TSERROR4CXX(L"workthread is error and return");
		return 1;
	}
	if (pBaseTask == NULL)	// 参数错误
	{
		TSERROR4CXX(L"param is wrong and return");
		return 2;
	}
	TSINFO4CXX("AddTask 添加一个任务 任务类型是" << pBaseTask->m_nTaskType);

	int nTaskId = GenerateTaskId();
	pBaseTask->m_nTaskId = nTaskId;
	//pBaseTask->SetTaskProcessor(this);
	EnterCriticalSection(&m_CriSection);
		m_TaskQueue.push(pBaseTask);
		ReleaseSemaphore(m_hSemaphore, 1, NULL);
	LeaveCriticalSection(&m_CriSection);
	return 0;
}

void CTaskProcessor::ClearTask()
{
	TSAUTO();
	if (!m_bInit)
	{
		return;
	}

	EnterCriticalSection(&m_CriSection);
	while (!m_TaskQueue.empty())
	{
		TSINFO4CXX("清理掉一个不需要的任务");
		CBaseTask* pBaseTask =	m_TaskQueue.front();
		m_TaskQueue.pop();
		pBaseTask->m_nStatus = KKIMG_TASKSTATUS_IGNORE;
		pBaseTask->m_nErrorCode = -1001;
		pBaseTask->OnTaskComplete();
	}
	// 停止正在加载的
	m_bStop = true;
	LeaveCriticalSection(&m_CriSection);
}
LRESULT CALLBACK CTaskProcessor::MsgWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TSAUTO();
	CBaseTask* pBaseTask = NULL;
	switch(uMsg)
	{
	case TASKPROCESSOR_MSG_TASKCOMPLETE:	// 任务完成消息
		pBaseTask =	(CBaseTask*)lParam;
		assert(pBaseTask);	// pBaseTask不能为空的
		pBaseTask->OnTaskComplete();
		return 0;
	case TASKPROCESSOR_MSG_TASKSTEPCOMPLETE:	// 任务某一步完成消息
		pBaseTask =	(CBaseTask*)lParam;
		assert(pBaseTask);	// pBaseTask不能为空的
		pBaseTask->OnTaskStepComplete();
		return 0;
	default:
		break;
	}
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// 子工作线程，执行实际的处理任务
DWORD WINAPI  CTaskProcessor::WorkThreadHandle(LPVOID lpParameter)
{
	TSAUTO();
	CTaskProcessor* pThis = (CTaskProcessor*)lpParameter;
	assert(pThis);
	while(1)
	{
		// 等待有任务进入任务列表
		WaitForSingleObject(pThis->m_hSemaphore, INFINITE);
		CBaseTask* pBaseTask = NULL;

		TSINFO4CXX(L"Current Thread Get a task");
		
		// 进入临界区
		EnterCriticalSection(&(pThis->m_CriSection));
			pThis->m_bStop = false;
			TSINFO4CXX(L"Current Thread enter Criticalsection");
			if (pThis->m_TaskQueue.empty())	// 如果任务队列为空，则不做任务处理
			{
				TSINFO4CXX(L"Current Thread enter Criticalsection and queue is empty");
				LeaveCriticalSection(&(pThis->m_CriSection));
				continue;
			}
			else	// 任务队列不为空，获取任务
			{
				TSINFO4CXX(L"Current Thread enter Criticalsection and queue is not empty");
				pBaseTask =	pThis->m_TaskQueue.front();
				pThis->m_TaskQueue.pop();
			}
		LeaveCriticalSection(&(pThis->m_CriSection));
		TSINFO4CXX(L"Current Thread leave Criticalsection");
		assert(pBaseTask);
	
		pBaseTask->m_nStatus = KKIMG_TASKSTATUS_FAIL;	// 将任务状态置为“处理失败”
		if (pBaseTask->m_bIgnore)
		{
			pBaseTask->m_nStatus = KKIMG_TASKSTATUS_IGNORE;
		}
		else
		{
			pBaseTask->HandleTask();
		}
		PostMessage(pThis->m_hWnd, TASKPROCESSOR_MSG_TASKCOMPLETE, 0, (LPARAM)pBaseTask);

	}
	return 0;
}
