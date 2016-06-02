#include "StdAfx.h"
#include ".\filesystemmonitor.h"
//#include <xl_lib\filesystem\path_utility.h>

using namespace std;
//using namespace xl::filesystem;
using namespace KKT::IDE;

#define OVERLAP_BUFFER_LEN (MAX_PATH*sizeof(wchar_t) + sizeof(FILE_NOTIFY_INFORMATION) + 16)


MonitoredDir::MonitoredDir(FileSystemEventNode* dataNode)
{
    m_openedHandle = NULL;
    m_refCount = 1;
    ZeroMemory(&m_overlapped,sizeof(OVERLAPPED));
    m_overlapped.Pointer = new BYTE[OVERLAP_BUFFER_LEN];

	m_monitoredDirPath = dataNode->strOrgPath;
	if (m_monitoredDirPath[m_monitoredDirPath.size()-1] != L'\\' && m_monitoredDirPath[m_monitoredDirPath.size()-1] != L'/')
	{
		m_monitoredDirPath.append(L"\\");
	}
	m_rootEventNode = *dataNode;   
}

MonitoredDir::~MonitoredDir()
{
	TSAUTO();
    delete [] m_overlapped.Pointer;
    if(m_openedHandle)
    {
        CloseHandle(m_openedHandle);
    }
}

long MonitoredDir::AddRef()
{
    m_refCount++;
    return m_refCount;
}

long MonitoredDir::GetRef()
{
	return m_refCount;
}

long MonitoredDir::Release()
{
    --m_refCount;
	long tempCount = m_refCount;
    if(m_refCount == 0)
    {
        delete this;
    }
    return tempCount;
}

bool MonitoredDir::Open()
{
    if(m_openedHandle)
    {
        return false;
    }

    m_openedHandle = CreateFile(
        m_monitoredDirPath.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,NULL);

    if(m_openedHandle)
    {
        return true;
    }

    return false;
}

const wchar_t* MonitoredDir::GetMonitoredDirPath()
{
    return m_monitoredDirPath.c_str();
}

void MonitoredDir::PostFireEvent(PFILE_NOTIFY_INFORMATION pInfo)
{
  /*  wchar_t buffer[MAX_PATH*2];
    pInfo->FileName[pInfo->FileNameLength/2]=0;
    wsprintf(buffer,L"Action is %d,offset=%x,name=%s\n",pInfo->Action,pInfo->NextEntryOffset,pInfo->FileName);
    OutputDebugStringW(buffer);*/
    size_t needLen = 0;
    PFILE_NOTIFY_INFORMATION pNext = pInfo;
    while(pNext->NextEntryOffset)
    {
        needLen += pNext->NextEntryOffset;
        
        /*pNext->FileName[pInfo->FileNameLength/2]=0;
        wsprintf(buffer,L"\tAction is %d,offset=%x,name=%s\n",pNext->Action,pNext->NextEntryOffset,pNext->FileName);
        OutputDebugStringW(buffer);*/
        pNext = (PFILE_NOTIFY_INFORMATION)((BYTE*)pNext + pNext->NextEntryOffset);
    }
    needLen += pNext->FileNameLength + 1 + sizeof(FILE_NOTIFY_INFORMATION);

    FILE_NOTIFY_INFORMATION* pNewNotify =(FILE_NOTIFY_INFORMATION*) new BYTE[needLen];
    memcpy(pNewNotify,pInfo,needLen);
    
    this->AddRef();
    ::PostMessage(FileSystemMonitor::GetInstance()->m_hWnd,WM_FS_FIREVENT,(WPARAM)pNewNotify,(LPARAM)this);
}

void MonitoredDir::FireEvent(PFILE_NOTIFY_INFORMATION pInfo)
{
    PFILE_NOTIFY_INFORMATION pNext = pInfo;
    wstring strFullPath;
	// 获取path1
	wstring wstrPath1 = m_monitoredDirPath;
	wstrPath1.append(pNext->FileName,pNext->FileNameLength / 2);
	// 获取Path2
	wstring wstrPath2 = m_monitoredDirPath;
	if (pNext->NextEntryOffset)
	{
		 pNext = (PFILE_NOTIFY_INFORMATION)((BYTE*)pNext + pNext->NextEntryOffset);
		wstrPath2.append(pNext->FileName,pNext->FileNameLength / 2);
	}
	else
	{
		wstrPath2 = wstrPath1;
	}
	m_rootEventNode.fnDirChangeCallback(m_rootEventNode.UserData, m_monitoredDirPath.c_str(), pNext->Action, wstrPath1.c_str(), wstrPath2.c_str());
}

void MonitoredDir::ReadDirectoryChange()
{
    if(m_openedHandle == NULL)
    {
        return;
    }

    DWORD temp;
    ReadDirectoryChangesW(m_openedHandle,m_overlapped.Pointer,OVERLAP_BUFFER_LEN,false,FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_CREATION,
        &temp,&m_overlapped,NULL);

}

bool MonitoredDir::MergePath(const wchar_t* newPath,bool isFile)
{
    return false;
}

HANDLE MonitoredDir::GetOpenedDirHandle()
{
    return m_openedHandle;
}

FileSystemMonitor::FileSystemMonitor(void) 
{
    m_isInit = false;
    m_dirChangeEventCookie = 0;
    m_fileChangeEventCookie = 0;
    m_isWorking = false;
}

FileSystemMonitor::~FileSystemMonitor(void)
{
}

FileSystemMonitor* FileSystemMonitor::GetInstance()
{
    static FileSystemMonitor* pTheOne = NULL;
    if(pTheOne == NULL)
    {
        if(pTheOne == NULL)
        {
            pTheOne = new FileSystemMonitor();
        }
    }

    return pTheOne;
}

unsigned int __stdcall FileSystemMonitor::ThreadProc(void* param)
{
    FileSystemMonitor* pThis = (FileSystemMonitor*) param;
    return pThis->MonitorThread();
}

void FileSystemMonitor::StopMonitor()
{
    m_isWorking = false;
}

LRESULT FileSystemMonitor::OnMsgFireEvent(UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    MonitoredDir* pDir = (MonitoredDir*)lParam;
    PFILE_NOTIFY_INFORMATION pInfo= (PFILE_NOTIFY_INFORMATION)wParam;
	pDir->FireEvent(pInfo);
    pDir->Release();
    delete [] pInfo;
    return 0;
}

long FileSystemMonitor::MonitorThread()
{
    //创建完成端口
    
    assert(m_hCompletePort);
    while(m_isWorking)
    {
        DWORD transBytes;
        DWORD compKey;
        OVERLAPPED* pOverlapped;
        
        if(::GetQueuedCompletionStatus(m_hCompletePort,&transBytes,&compKey,&pOverlapped,INFINITE))
        {
            //判断完成端口上是否有事件 或则 是否有改变ReadDir的请求
            //如果是完成端口事件，收集数据，PostEvent，并继续ReadDir
            //否则 打开Dir,添加到完成端口，或是cancel一个已经在Read的Dir
            if(pOverlapped == NULL)
            {
                //这是刷新Monitor Dir List操作
                if(compKey == 0)
                {
                    m_eventCS.lock();
                    for(size_t i=0;i<m_dirChangeEventNodeQueue.size();++i)
                    {
                        MonitoredDir* pNewDir = GetMonitoredDir(&(m_dirChangeEventNodeQueue[i]));
                        if(pNewDir)
                        {
                            pNewDir->AddRef();//要放到CompletionPort队列里了
                            m_hCompletePort = CreateIoCompletionPort(pNewDir->GetOpenedDirHandle(),m_hCompletePort,(ULONG_PTR)pNewDir,1);
                            pNewDir->ReadDirectoryChange();
                        }
                    }
                    m_dirChangeEventNodeQueue.clear();  
                    m_eventCS.unlock();
                }
            }
            else
            {
				
                //有变化了！根据pOverlapped里的数据，得到需要通知的callback函数列表
				
				m_moitoredDirsCS.lock();
                MonitoredDir* pThisDir = (MonitoredDir*) compKey;
				TSINFO4CXX("------------------" << pThisDir->GetRef());
				int nRef = pThisDir->Release();
				TSINFO4CXX("nRef is:" << nRef);
				if(nRef == 1 && pThisDir->m_rootEventNode.bValid == false)
				{
					TSINFO4CXX(L"去你妹的终于删除了");
					pThisDir->Release();
				}
				else
				{
					pThisDir->PostFireEvent((PFILE_NOTIFY_INFORMATION)pOverlapped->Pointer);
					//继续发起下一次读取
					pThisDir->AddRef();
					pThisDir->ReadDirectoryChange();
				}
				m_moitoredDirsCS.unlock();
            }
        }
    }
    return 0;
}

MonitoredDir* FileSystemMonitor::GetMonitoredDir(FileSystemEventNode* newNode,bool autoCreate)
{
    
    MonitoredDir* pResult = new MonitoredDir(newNode);
    if(pResult->Open())
    {
		m_moitoredDirsCS.lock();
        m_allMonitoredDir.push_back(pResult);
		 m_moitoredDirsCS.unlock();
    }
    else
    {
        pResult->Release();
        pResult = NULL;
    }
    return pResult;
}

void FileSystemMonitor::InitMonitor()
{
    if(m_isInit)
    {
        return;
    }
    m_isInit = true;
    m_mainTID = GetCurrentThreadId();

    m_hCompletePort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,1);

    //创建工作窗口
    m_hWnd = Create(HWND_MESSAGE);
    assert(m_hWnd);
    //创建监听线程
    m_isWorking = true;
    m_hThread = (HANDLE)_beginthreadex(NULL, 0, &FileSystemMonitor::ThreadProc, this, 0, &m_ThreadID );

    return;
}

long FileSystemMonitor::AttachDirChangeEvent(const wchar_t* dirPath,bool subDirChangeEvent,unsigned long mask,bool isIncludeHiddenFile,FileSystemEventNotifyUserData udata,pfnDirChangeCallback fnCallback)
{
	TSINFO4CXX(L"MonitorDirChange Path:" << dirPath);
    if(!m_isInit)
    {
        return -1;
    }
    if(GetCurrentThreadId() != m_mainTID)
    {
        assert(GetCurrentThreadId() != m_mainTID);
        return -1;
    }
    wchar_t buffer[MAX_PATH];
    ::PathCanonicalizeW(buffer,dirPath);
    m_eventCS.lock();
    m_dirChangeEventCookie++;
    FileSystemEventNode theNode;
    theNode.Cookie = m_dirChangeEventCookie;
    theNode.fnDirChangeCallback = fnCallback;
    theNode.UserData = udata;
    theNode.IsIncludeHiddenFile = isIncludeHiddenFile;
    theNode.IsNeedSubDirChangeNotify = subDirChangeEvent;
    theNode.strOrgPath = buffer;
	theNode.bValid = true;

    m_dirChangeEventNodeQueue.push_back(theNode);
    m_eventCS.unlock();
    PostQueuedCompletionStatus(m_hCompletePort,0,0,NULL);
    //判断是否需要
    return m_dirChangeEventCookie;
}

bool FileSystemMonitor::DetachDirChangeEvent(long cookie)
{
	TSINFO4CXX(L"MonitorDirChange cookie:" << cookie);
	m_moitoredDirsCS.lock();
    vector<MonitoredDir*>::iterator iter;
    for(iter = m_allMonitoredDir.begin();iter != m_allMonitoredDir.end(); ++iter)
    {
        if((*iter)->m_rootEventNode.Cookie == cookie)
        {
			(*iter)->m_rootEventNode.bValid = false;
        }
    }
	m_moitoredDirsCS.unlock();
    return false;
}

void FileSystemMonitor::DetachAllDirChangeEvent()
{
    return;
}

