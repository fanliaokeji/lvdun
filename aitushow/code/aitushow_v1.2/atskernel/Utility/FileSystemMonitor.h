
#pragma once
#include <map>
#include <vector>
#include "Lock.h"


#define WM_FS_FIREVENT (WM_USER+200)

namespace KKT
{

namespace IDE
{

struct FileSystemEventNotifyUserData
{
    void* udata1;
    void* udata2;
};

typedef long (* pfnDirChangeCallback)(FileSystemEventNotifyUserData udata,const wchar_t* dirPath,
                                      unsigned long changeType,const wchar_t* changePath1,const wchar_t* changePath2);

typedef long (* pfnFileChangeCallback)(FileSystemEventNotifyUserData udata,
                                      unsigned long changeType,const wchar_t* changePath1,const wchar_t* changePath2);

struct FileSystemEventNode
{
    long Cookie;
    FileSystemEventNotifyUserData UserData;
    bool IsNeedSubDirChangeNotify;
    bool IsIncludeHiddenFile;
    std::wstring strOrgPath;
	bool bValid;
    pfnDirChangeCallback fnDirChangeCallback;
};

struct OpenedMonitorDirNode
{
    HANDLE OpenedHandle;
    long   RefCount;
    OVERLAPPED* Overlapped;
};


//一个已经被成功监听的Dir
class MonitoredDir 
{
public:
    MonitoredDir(FileSystemEventNode* dataNode);
    virtual ~MonitoredDir();

    long AddRef();
    long Release();
	long GetRef();

public:
    bool Open();
    const wchar_t* GetMonitoredDirPath();
    void PostFireEvent(PFILE_NOTIFY_INFORMATION pInfo);
    void FireEvent(PFILE_NOTIFY_INFORMATION pInfo);
    void ReadDirectoryChange();
    bool MergePath(const wchar_t* newPath,bool isFile);
    HANDLE GetOpenedDirHandle();
	FileSystemEventNode m_rootEventNode;
protected:
    std::wstring m_monitoredDirPath;
    HANDLE      m_openedHandle;
    long        m_refCount;
   
    //保护下面3个变量
	XMLib::CriticalSection m_CS;
    std::vector<FileSystemEventNode> m_subDirEventNodes;
    OVERLAPPED  m_overlapped;
private:
	//
};

class FileSystemMonitor : public CWindowImpl<FileSystemMonitor>
{
public:
    FileSystemMonitor(void);
    virtual ~FileSystemMonitor(void);

public:
    //初始化，必须在主线程调用。主要目的是创建一个工作窗口用于在主线程fire event
    void InitMonitor();
    void StopMonitor();
    long AttachDirChangeEvent(const wchar_t* dirPath,bool subDirChangeEvent,unsigned long mask,bool isIncludeHiddenFile,FileSystemEventNotifyUserData udata,pfnDirChangeCallback fnCallback);
    bool DetachDirChangeEvent(long cookie);
	void DetachAllDirChangeEvent();

    static FileSystemMonitor* GetInstance();


protected:
    BEGIN_MSG_MAP(FileSystemMonitor)
        MESSAGE_HANDLER(WM_FS_FIREVENT, OnMsgFireEvent)
    END_MSG_MAP()

    LRESULT OnMsgFireEvent(UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    long MonitorThread();
    static unsigned int __stdcall ThreadProc(void*);
    MonitoredDir* GetMonitoredDir(FileSystemEventNode* newNode,bool autoCreate = true);

protected:
    bool m_isInit;
    bool m_isWorking;
    HANDLE m_hCompletePort;


    HANDLE m_hThread;
    unsigned int m_ThreadID;
    DWORD m_mainTID;
    

    long m_dirChangeEventCookie;
    long m_fileChangeEventCookie;
	XMLib::CriticalSection m_eventCS;
    std::vector<FileSystemEventNode> m_dirChangeEventNodeQueue;
    
	XMLib::CriticalSection m_moitoredDirsCS;
    std::vector<MonitoredDir*> m_allMonitoredDir;
private:
	//
};

}//namespace IDE
}//namespace KKT
