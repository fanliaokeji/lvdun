#pragma once
#include <WTL/atlapp.h>
#include <atluser.h>

static const UINT WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));

__interface INotifyIconCallback
{
	void OnNotifyIcon(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
};

class NotifyIconMsg :
	public CWindowImpl<NotifyIconMsg>
{
public:
	NotifyIconMsg(void);
	virtual ~NotifyIconMsg(void);

	static const UINT WM_DS_NOTIFYICON = WM_USER + 1;

DECLARE_WND_CLASS(L"NotifyIconWindow")

BEGIN_MSG_MAP(NotifyIconMsg)
	MESSAGE_HANDLER(WM_DS_NOTIFYICON, OnNotifyIcon)
	MESSAGE_HANDLER(WM_TASKBARCREATED, OnTaskbarCreated)
	MESSAGE_HANDLER(WM_TIMER, OnTimeFlash)
END_MSG_MAP()

public:
	//下面两个函数应该在Init前面调用
	void SetTitle(LPCTSTR lpszTitle);
	void SetTipInfo(LPCTSTR lpszTipTilte, LPCTSTR lpszTipText);
	void SetDefaultIconInfo(int defaultIconId, PCTSTR pszDefaultTitle, PCTSTR pszDefaultTip);
	void Init();
	void Uninit();

	void SetIcon(HICON hIcon, PCTSTR pszTip);
	void SetIcon(PCTSTR pszIconPath, PCTSTR pszTip);

	void Show();
	void Hide();
	void FlashIcon(DWORD dwInterval);
	void CancelFlash();
	void ShowTip(BOOL bShow,PCTSTR pszNotfyTip);

	void Attach(INotifyIconCallback *pCallback);
	void Detach();


private:
	bool IsExistTray();

	void DoSetIcon(HICON hIcon, PCTSTR pszTip);
private:
	NOTIFYICONDATA m_nid;

	INotifyIconCallback *m_pCallback;

private:
	LRESULT OnNotifyIcon(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTaskbarCreated(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimeFlash(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

public:
	static NotifyIconMsg &Instance()
	{
		static NotifyIconMsg *s_instance = NULL;
		if (s_instance == NULL)
		{
			s_instance = new NotifyIconMsg;
		}
		return *s_instance;
	}

private:

	int m_DefaultIconId;
	std::wstring m_strDefaultTip;
	std::wstring	m_strDefaultTitle;

	std::wstring m_strTitle;				//"自定义通知"那里显示标题
	std::wstring m_strTipTitle;			//托盘图标弹出tip的标题
	std::wstring m_strTipText;			//托盘图标弹出tip的文字

	CIcon m_DefaultNotify;
	HICON m_hCustomIcon;
	HICON m_hIcon;
	std::wstring m_strCustomIconPath;

	BOOL m_bInited;
	BOOL m_bVisible;
};

#define gsNotifyIcon NotifyIconMsg::Instance()
