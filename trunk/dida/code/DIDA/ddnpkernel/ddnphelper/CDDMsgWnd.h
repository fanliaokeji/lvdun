#pragma once
#include "atlwin.h"
#include "map"

#include <XLLuaRuntime.h>
typedef void (*funResultCallBack) (DWORD userdata1,DWORD userdata2, const char* pszKey,  DISPPARAMS* pParams);

struct CallbackNode
{
	funResultCallBack pCallBack;
	DWORD userData1;
	DWORD userData2;
	const void* luaFunction;
};

static std::wstring GetMsgWndClassName()
{	
	wchar_t szClassName[MAX_PATH] = {0};
	DWORD dwPid = ::GetCurrentProcessId();
	swprintf(szClassName,L"{10808D97-3494-4c5d-857F-0ADFA04FA722}_ddnotepadmsg_%d", dwPid);
	return szClassName;
}


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

	//bool HandleSingleton();

	static ATL::CWndClassInfo& GetWndClassInfo()
	{ 
		static std::wstring strClassName = GetMsgWndClassName();
		static ATL::CWndClassInfo wc = 
		{ 
			{ sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, StartWindowProc, 0, 0, NULL, NULL, NULL, (HBRUSH)(COLOR_WINDOW + 1), NULL, strClassName.c_str(), NULL }, NULL, NULL, IDC_ARROW, TRUE, 0, _T("") 
		}; 
		return wc; 
	}
	BEGIN_MSG_MAP(CDDMsgWindow)
		MESSAGE_HANDLER(WM_COPYDATA, OnCopyData)
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
};
