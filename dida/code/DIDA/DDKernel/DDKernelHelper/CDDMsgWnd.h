#pragma once
#include "atlwin.h"
#include "map"

#include <XLLuaRuntime.h>
typedef void (*funResultCallBack) (DWORD userdata1,DWORD userdata2, const char* pszKey,  DISPPARAMS* pParams);

#define WM_EXPLORER WM_USER + 200


struct CallbackNode
{
	funResultCallBack pCallBack;
	DWORD userData1;
	DWORD userData2;
	const void* luaFunction;
};

class CDDMsgWindow : public  CWindowImpl<CDDMsgWindow>
{
public:
	static CDDMsgWindow* Instance()
	{
        static CDDMsgWindow s;
		return &s;
	}

	int AttachListener(DWORD userData1,DWORD userData2,funResultCallBack pfn, const void* pfun);
	int DetachListener(DWORD userData1, const void* pfun);

	bool HandleSingleton();

	DECLARE_WND_CLASS(L"{10808D97-3494-4c5d-857F-0ADFA04FA721}_ddmainmsg")
	BEGIN_MSG_MAP(CFilterMsgWindow)
		MESSAGE_HANDLER(WM_COPYDATA, OnCopyData)
		MESSAGE_HANDLER(WM_EXPLORER, OnExplorerNotify)

	END_MSG_MAP()
private:
	CDDMsgWindow(void);
	~CDDMsgWindow(void);
	std::vector<CallbackNode> m_allCallBack;

	void Fire_LuaEvent(const char* pszKey, DISPPARAMS* pParams)
	{
		TSAUTO();
		for(size_t i = 0;i<m_allCallBack.size();i++)
		{
 			m_allCallBack[i].pCallBack(m_allCallBack[i].userData1,m_allCallBack[i].userData2, pszKey,pParams);
		}
	}
private:
	HANDLE m_hMutex;
 

private:

public:
	LRESULT OnCopyData(UINT , WPARAM , LPARAM , BOOL&);

	//软件升级:WPARAM 1
	//关于滴答日历:WPARAM 2
	//退出:WPARAM 3
	//拉起:WPARAM 0
	LRESULT OnExplorerNotify(UINT , WPARAM , LPARAM , BOOL&);
};
