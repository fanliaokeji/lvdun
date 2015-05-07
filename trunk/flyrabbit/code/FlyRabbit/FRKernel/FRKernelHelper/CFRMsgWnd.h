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

enum BrowserTaskType
{
	IMAGE=0,
	OTHER
};
struct FRBrowserTaskInfo
{
	wchar_t wszUrl[1024];
	BrowserTaskType type;
	int posX;
	int posY;
	
};

class CFRMsgWindow : public  CWindowImpl<CFRMsgWindow>
{
public:
	static CFRMsgWindow* Instance()
	{
        static CFRMsgWindow s;
		return &s;
	}

	int AttachListener(DWORD userData1,DWORD userData2,funResultCallBack pfn, const void* pfun);
	int DetachListener(DWORD userData1, const void* pfun);

	bool HandleSingleton();

	DECLARE_WND_CLASS(L"{1DA1F328-DB3D-4f6a-A62E-E9B2B22F2B9A}_frmainmsg")
	BEGIN_MSG_MAP(CFRMsgWindow)
		MESSAGE_HANDLER(WM_COPYDATA, OnCopyData)
	END_MSG_MAP()
private:
	CFRMsgWindow(void);
	~CFRMsgWindow(void);
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
};
