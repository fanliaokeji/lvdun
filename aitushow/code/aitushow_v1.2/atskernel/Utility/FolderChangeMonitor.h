#pragma once
#include "filesystemmonitor.h"
#include "../LuaBase/LuaEventContainer.h"
using namespace KKT::IDE;

class CFolderChangeMonitor
{
public:
	CFolderChangeMonitor(void);
	virtual ~CFolderChangeMonitor(void);

	// 获取实例
	static CFolderChangeMonitor* Instance();
	// 初始化
	bool Init();
	long MonitorDirChange(const wstring& wstrDirPath);
	void UnMonitorDirChange(long nCookie);


	// 回调事件
	static long DirChangeCallback(FileSystemEventNotifyUserData udata, const wchar_t* dirPath, unsigned long changeType,const wchar_t* changePath1, const wchar_t* changePath2);

	// 绑定事件
	int AttachDirChangeEvent(lua_State* luaState, int nIndex, DWORD& dwCookie);
	// 卸载事件
	int DetachDirChangeEvent(DWORD dwCookie);
	void FireChangeEvent(const wstring& wstrOldFilePath, const wstring& wstrNewFilePath, int nEventType);
private:
	CLuaMultiEventContainer m_EventContainer;
private:
	//
};
