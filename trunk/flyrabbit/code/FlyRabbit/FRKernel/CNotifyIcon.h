#pragma once
#include <WTL/atlapp.h>
#include <atluser.h>

static const UINT WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));

__interface INotifyIconCallback
{
	void OnNotifyIcon(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
};

class CNotifyIcon :
	public CWindowImpl<CNotifyIcon>
{
public:
	CNotifyIcon(void);
	virtual ~CNotifyIcon(void);

	static const UINT WM_DS_NOTIFYICON = WM_USER + 1;

DECLARE_WND_CLASS(L"NotifyIconBkgWindow")

BEGIN_MSG_MAP(CNotifyIcon)
	MESSAGE_HANDLER(WM_DS_NOTIFYICON, OnNotifyIcon)
	MESSAGE_HANDLER(WM_TASKBARCREATED, OnTaskbarCreated)
	MESSAGE_HANDLER(WM_TIMER, OnTimeFlash)
END_MSG_MAP()

public:
	//������������Ӧ����Initǰ�����
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
	static CNotifyIcon &Instance()
	{
		static CNotifyIcon *s_instance = NULL;
		if (s_instance == NULL)
		{
			s_instance = new CNotifyIcon;
		}
		return *s_instance;
	}

private:

	int m_DefaultIconId;
	std::wstring m_strDefaultTip;
	std::wstring	m_strDefaultTitle;

	std::wstring m_strTitle;				//"�Զ���֪ͨ"������ʾ����
	std::wstring m_strTipTitle;			//����ͼ�굯��tip�ı���
	std::wstring m_strTipText;			//����ͼ�굯��tip������

	CIcon m_DefaultNotify;
	HICON m_hCustomIcon;
	HICON m_hIcon;
	std::wstring m_strCustomIconPath;

	BOOL m_bInited;
	BOOL m_bVisible;
};

#define gsNotifyIcon CNotifyIcon::Instance()
