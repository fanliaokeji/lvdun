#ifndef __KKFOLDERDIALOG_H___
#define __KKFOLDERDIALOG_H___

#include <atldlgs.h>
#include <Dlgs.h>


#define CheckBtnID 1005	
// 居中文件打开对话框
class CKKFolderDialog : public CFolderDialogImpl<CKKFolderDialog>
{
public:
	CKKFolderDialog(HWND hWndParent = NULL, LPCTSTR lpstrTitle = NULL, UINT uFlags = BIF_RETURNONLYFSDIRS)
		: CFolderDialogImpl<CKKFolderDialog>(hWndParent, lpstrTitle, uFlags)
	{}
	
	class CFolderDlgWnd : public CWindowImpl<CFolderDlgWnd>
	{
	public:
		BEGIN_MSG_MAP(CFolderDlgWnd)
			MESSAGE_HANDLER(WM_SIZE,OnSize)
			COMMAND_HANDLER(CheckBtnID, BN_CLICKED, OnCheckClick)
		END_MSG_MAP()
		
		LRESULT OnCheckClick(WORD id, WORD code, HWND hWnd, BOOL &)
		{
			m_bCheck = !m_bCheck;
			if (m_bCheck)
			{
				SendMessage(hWnd, BM_SETCHECK, 1, 0);
			}
			else
			{
				SendMessage(hWnd, BM_SETCHECK, 0, 0);
			}
			
			return 0;
		}
		
		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			RECT rcWnd = {0};
			RECT rcCheckBox = {0};
			GetWindowRect(&rcWnd);
			ScreenToClient(&rcWnd);
			rcCheckBox.left = 15;
			rcCheckBox.top = rcWnd.bottom - 38;
			rcCheckBox.right = 115;
			rcCheckBox.bottom = rcCheckBox.top + 23;
			
			CWindow hCheckBox = GetDlgItem(CheckBtnID);
			hCheckBox.MoveWindow(&rcCheckBox);
			bHandled = FALSE;
			return 0;
		}


		BOOL SubclassWindow(HWND hWnd)
		{
			// 子类化
			if (!CWindowImpl<CFolderDlgWnd>::SubclassWindow(hWnd))
			{
				return FALSE;
			}
			
			HFONT hFont = GetFont();
			RECT rcButton = {0};
			GetDlgItem(0x3746).GetWindowRect(&rcButton);
			ScreenToClient(&rcButton);
			CWindow hCheckBox = CreateWindowW(L"BUTTON", L"应用到子文件夹", WS_VISIBLE|WS_CHILD|BS_CHECKBOX,
				rcButton.left, rcButton.top, 100, 23, m_hWnd, (HMENU)CheckBtnID,
				ModuleHelper::GetModuleInstance(), NULL);
			if (hCheckBox)
			{
				hCheckBox.SetFont(hFont);
			}
			// 隐藏新建文件夹
			::ShowWindow(GetDlgItem(0x3746),SW_HIDE);
			m_bCheck = FALSE;
			return TRUE;
		}

		BOOL GetCheckState()
		{
			return m_bCheck;
		}

		BOOL m_bCheck;

	};

	void OnInitialized()
	{
		// 子类化
		m_dlgWnd.SubclassWindow(m_hWnd);
	}

	BOOL GetCheckState()
	{
		return m_dlgWnd.GetCheckState();
	}

private:
	CFolderDlgWnd m_dlgWnd;
};

#endif