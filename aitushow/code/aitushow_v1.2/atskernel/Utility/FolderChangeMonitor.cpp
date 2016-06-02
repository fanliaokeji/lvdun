#include "StdAfx.h"
#include "FolderChangeMonitor.h"
#include "../Utility/StringOperation.h"
CFolderChangeMonitor::CFolderChangeMonitor(void)
{
}

CFolderChangeMonitor::~CFolderChangeMonitor(void)
{
}

bool CFolderChangeMonitor::Init()
{
	FileSystemMonitor* pFileSystemMonitor = FileSystemMonitor::GetInstance();
	pFileSystemMonitor->InitMonitor();
	return true;
}

CFolderChangeMonitor* CFolderChangeMonitor::Instance()
{
	static CFolderChangeMonitor* pMonitor = NULL;
	if (pMonitor == NULL)
	{
		pMonitor = new CFolderChangeMonitor();
	}
	return pMonitor;
}

long CFolderChangeMonitor::DirChangeCallback(FileSystemEventNotifyUserData udata,const wchar_t* dirPath,
					   unsigned long changeType,const wchar_t* changePath1,const wchar_t* changePath2)
{
	CFolderChangeMonitor* pMonitor = (CFolderChangeMonitor*)(udata.udata1);
	pMonitor->FireChangeEvent(changePath1, changePath2, changeType);
	return 0;
}

long CFolderChangeMonitor::MonitorDirChange(const wstring& wstrDirPath)
{
	FileSystemMonitor* pFileSystemMonitor = FileSystemMonitor::GetInstance();
	FileSystemEventNotifyUserData fileNotifyUserData;
	fileNotifyUserData.udata1 = (void*)this;
	fileNotifyUserData.udata2 = NULL;
	long cookie = pFileSystemMonitor->AttachDirChangeEvent(wstrDirPath.c_str(), false, 0, true, fileNotifyUserData, DirChangeCallback);
	return cookie;
}

void CFolderChangeMonitor::UnMonitorDirChange(long nCookie)
{
	FileSystemMonitor* pFileSystemMonitor = FileSystemMonitor::GetInstance();
	pFileSystemMonitor->DetachDirChangeEvent(nCookie);
}

int CFolderChangeMonitor:: AttachDirChangeEvent(lua_State* luaState, int nIndex, DWORD& dwCookie)
{
	if (luaState)
	{
		m_EventContainer.AttachEvent(L"OnDirChangeEvent", luaState, nIndex, dwCookie);
		return S_OK;
	}
	return S_FALSE;
}
int CFolderChangeMonitor::DetachDirChangeEvent(DWORD dwCookie)
{
	return m_EventContainer.DetachEvent(L"OnDirChangeEvent", dwCookie);
}
void CFolderChangeMonitor::FireChangeEvent(const wstring& wstrOldFilePath, const wstring& wstrNewFilePath, int nEventType)
{
	ILuaEventEnum* pEventEnum = NULL;
	m_EventContainer.GetEventEnum(_T("OnDirChangeEvent"), pEventEnum);
	if (pEventEnum )
	{
		CLuaEvent* pEvent = NULL;
		pEventEnum->Reset();
		while (pEventEnum->Next(pEvent) == S_OK)
		{
			lua_State* luaState = pEvent->GetLuaState();
			int nowTop = lua_gettop(luaState);

			pEvent->PushFunction();
			
			std::string strOldFilePath, strNewFilePath;
			strOldFilePath = ultra::_T2UTF(wstrOldFilePath);
			strNewFilePath = ultra::_T2UTF(wstrNewFilePath);
			
			lua_pushstring(luaState,strOldFilePath.c_str());
			lua_pushstring(luaState,strNewFilePath.c_str());
			lua_pushinteger(luaState, nEventType);

			pEvent->Call(3, 0);
			lua_settop(luaState, nowTop);
		}
	}
}