#include "StdAfx.h"


#include "NotifyIconMsg.h"

NotifyIconMsg::NotifyIconMsg(void) :
	m_pCallback(NULL), m_strTitle(_T("")), m_strTipTitle(_T("")), m_strTipText(_T(""))
{
	memset(&m_nid, 0, sizeof(m_nid));
	m_nid.cbSize = sizeof(m_nid);
	m_nid.uID = 0;
	m_hCustomIcon = NULL;
	m_bInited = FALSE;
	m_bVisible = FALSE;
	m_hIcon = NULL;
}

NotifyIconMsg::~NotifyIconMsg(void)
{
}

LRESULT NotifyIconMsg::OnNotifyIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (m_pCallback != NULL)
	{
		m_pCallback->OnNotifyIcon(uMsg, wParam, lParam);
	}
	return 0;
}

LRESULT NotifyIconMsg::OnTaskbarCreated(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	Show();
	return 0;
}

LRESULT NotifyIconMsg::OnTimeFlash(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (wParam == (WPARAM)1)
	{
		if (m_bVisible)
		{
			m_nid.hIcon = NULL;
			m_nid.uFlags |= NIF_ICON;
			Shell_NotifyIcon(NIM_MODIFY, &m_nid);
		} 
		else
		{
			m_nid.hIcon = m_hIcon;
			m_nid.uFlags |= NIF_ICON;
			Shell_NotifyIcon(NIM_MODIFY, &m_nid);
		}
		m_bVisible = !m_bVisible;
	}
	return 0;
}

void NotifyIconMsg::Attach(INotifyIconCallback *pCallback)
{
	m_pCallback = pCallback;
}

void NotifyIconMsg::Detach()
{
	m_pCallback = NULL;
}


void NotifyIconMsg::SetTitle(LPCTSTR lpszTitle)
{
	if (lpszTitle == NULL)
	{
		m_strTitle = _T("");
	}
	else
	{
		m_strTitle = lpszTitle;
	}
}


void NotifyIconMsg::SetTipInfo(LPCTSTR lpszTipTilte, LPCTSTR lpszTipText)
{
	if (lpszTipTilte == NULL)
	{
		m_strTipTitle = _T("");
	}
	else
	{
		m_strTipTitle = lpszTipTilte;
	}

	if (lpszTipText == NULL)
	{
		m_strTipText = _T("");
	}
	else
	{
		m_strTipText = lpszTipText;
	}
}


void NotifyIconMsg::SetDefaultIconInfo(int defaultIconId, PCTSTR pszDefaultTitle, PCTSTR pszDefaultTip)
{
	m_DefaultIconId = defaultIconId;
	m_strDefaultTitle = (pszDefaultTitle == NULL ? L"" : pszDefaultTitle);
	m_strDefaultTip = (pszDefaultTip == NULL ? L"" : pszDefaultTip);
}


void NotifyIconMsg::Init()
{
	if (m_bInited)
	{
		return;
	}
	SetTitle(m_strDefaultTitle.c_str());

	m_DefaultNotify.LoadIcon(m_DefaultIconId, 16, 16);

	Create(NULL, NULL, NULL, (WS_OVERLAPPEDWINDOW)/*&(~WS_THICKFRAME)*/);
	m_nid.hWnd = m_hWnd;
	m_nid.uCallbackMessage = WM_DS_NOTIFYICON;
	m_nid.uFlags |= (NIF_MESSAGE | NIF_TIP);
	m_nid.hIcon = m_DefaultNotify;
	if (m_strTipText.empty() == false)
	{
		m_nid.uFlags |= NIF_INFO;
		_tcscpy(m_nid.szInfoTitle, m_strTipTitle.c_str());		//托盘图标弹出tip的标题
		_tcscpy(m_nid.szInfo, m_strTipText.c_str());			//托盘图标弹出tip的文字
	}
	_tcscpy(m_nid.szTip, m_strTitle.c_str());
	Shell_NotifyIcon(NIM_MODIFY, &m_nid);
	Show();
	m_bInited = TRUE;
}

void NotifyIconMsg::Uninit()
{
	if (IsWindow())
	{
		DestroyWindow();
	}
	Hide();
}


void NotifyIconMsg::DoSetIcon(HICON hIcon, PCTSTR pszTip)
{
	Init();

	m_nid.hIcon = hIcon;
	m_hIcon = hIcon;
	if (pszTip != NULL)
	{
		lstrcpyn(m_nid.szTip, pszTip, dimensionof(m_nid.szTip));
	}

	if (m_nid.hIcon != NULL)
	{
		m_nid.uFlags |= NIF_ICON;
	}
	else
	{
		m_nid.uFlags &= (~NIF_ICON);
	}

	if (lstrlen(m_nid.szTip) != 0)
	{
		m_nid.uFlags |= NIF_TIP;
	}
	else
	{
		m_nid.uFlags &= (~NIF_TIP);
	}

	Shell_NotifyIcon(NIM_MODIFY, &m_nid);
}


void NotifyIconMsg::SetIcon(HICON hIcon, PCTSTR pszTip)
{
	Init();
	std::wstring strTipText(L"");
	if (hIcon == NULL)
	{
		hIcon = m_DefaultNotify;
		strTipText = m_strDefaultTip;
	}
	else if (pszTip != NULL)	//hIcon != NULL && pszTip != NULL
	{
		strTipText = pszTip;
	}
	
    DoSetIcon(hIcon, strTipText.c_str());
}

void NotifyIconMsg::SetIcon(PCTSTR pszIconPath, PCTSTR pszTip)
{
	if (pszIconPath == NULL)
	{
		DoSetIcon(m_DefaultNotify, m_strDefaultTip.c_str());
		return;
	}
	 
	if (PathFileExists(pszIconPath) == FALSE)
	{
		return;
	}

	HICON hIcon = (HICON)LoadImage(NULL, pszIconPath, IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
	TSDEBUG4CXX(L"[XAppLuaTool]NotifyIcon pszIconPath = " << pszIconPath<<L"HICON = "<<hIcon);
	if (hIcon == NULL)
	{
		return ;
	}

	if (m_hCustomIcon != NULL)
	{
		DestroyIcon(m_hCustomIcon);
		m_hCustomIcon = NULL;
	}
	m_hCustomIcon = hIcon;
	m_strCustomIconPath = pszIconPath;

	if (pszTip == NULL)
	{
		pszTip = _T("");
	}
	DoSetIcon(hIcon, pszTip);
}



bool NotifyIconMsg::IsExistTray()
{
	return (Shell_NotifyIcon(NIM_MODIFY, &m_nid) == TRUE);
}

void NotifyIconMsg::Show()
{
	if (!IsExistTray())
	{
		m_bVisible = TRUE;
		Shell_NotifyIcon(NIM_ADD, &m_nid);
	}
}

void NotifyIconMsg::Hide()
{
	if (IsExistTray())
	{
		m_bVisible = FALSE;
		Shell_NotifyIcon(NIM_DELETE, &m_nid);
	}
}

void NotifyIconMsg::FlashIcon(DWORD dwInterval)
{
	if (IsExistTray() && m_hIcon)
	{
		//Shell_NotifyIcon(NIM_DELETE, &m_nid);
		SetTimer(1,dwInterval,(TIMERPROC)NULL);
	}
}

void NotifyIconMsg::CancelFlash()
{
	if (IsExistTray() && m_hIcon)
	{
		KillTimer(1);
		m_nid.hIcon = m_hIcon;
		m_nid.uFlags |= NIF_ICON;
		Shell_NotifyIcon(NIM_MODIFY, &m_nid);
	}
}

void NotifyIconMsg::ShowTip(BOOL bShow,PCTSTR pszNotfyTip)
{
	if (bShow)
	{
		m_nid.uFlags |= NIF_INFO;
		_tcscpy(m_nid.szInfo, pszNotfyTip);
	}
	else
	{
		m_nid.uFlags |= NIF_INFO;
		m_nid.szInfo[0] = L'\0';
	}
	Shell_NotifyIcon(NIM_MODIFY, &m_nid);
}