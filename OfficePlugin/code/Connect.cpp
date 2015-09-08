// Connect.cpp : Implementation of CConnect

#include "stdafx.h"
#include "Connect.h"
#include "resource.h"

extern HINSTANCE g_hModule;

//保存状态
static BOOL g_sIsSilent = FALSE;

//保存Application对象
static CComQIPtr<_Application> g_Application;


BOOL CALLBACK DialogProc(HWND hDlg, UINT uMsg,
						 WPARAM wParam, LPARAM lParam)
{
	BOOL fReturn = TRUE;
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			RECT	rtWindow = {0};
			RECT	rtContainer = {0};

			GetWindowRect(hDlg,&rtWindow);
			rtWindow.right -= rtWindow.left;
			rtWindow.bottom -= rtWindow.top;
			SystemParametersInfo(SPI_GETWORKAREA,0,&rtContainer,0);
			SetWindowPos(hDlg,NULL,(rtContainer.right - rtWindow.right) / 2,(rtContainer.bottom - rtWindow.bottom) / 2,0,0,SWP_NOSIZE);
			SendMessage(GetDlgItem(hDlg, IDC_EDIT1),EM_LIMITTEXT,16, 0);
			break;
		}
	case WM_COMMAND:{
		if (lParam != 0){
			if(HIWORD(wParam) == BN_CLICKED){
				switch(LOWORD(wParam)){
					case IDOK:
						{
							char szPassWord[17]={0};
							GetWindowTextA(GetDlgItem(hDlg, IDC_EDIT1), szPassWord, 17);
							Word::_DocumentPtr ptrdoc = g_Application->GetActiveDocument();
							if (ptrdoc){
								BSTR bstrPswd = _bstr_t(szPassWord);
								ptrdoc->put_Password(bstrPswd);
								ptrdoc->Save();
							}
							EndDialog(hDlg, true);
							break;
						}
					case IDCANCEL:
						EndDialog(hDlg, true);
						break;
				}
			}
		}
		break;
	}
	case WM_DESTROY:
		//::PostQuitMessage(0);
		break;
	default:
		fReturn = FALSE;
	}
	
	return fReturn;
}

// CConnect

HRESULT CConnect::OnConnection(LPDISPATCH Application, ext_ConnectMode ConnectMode, LPDISPATCH AddInInst, SAFEARRAY **custom)
{
	TSAUTO();
	//是否显示按钮
	CRegKey key;
	DWORD dwValue;
	if(key.Open(HKEY_CURRENT_USER, _T("Software\\WordEncLock")) == ERROR_SUCCESS){
		if (key.QueryValue(dwValue, _T("silent")) == ERROR_SUCCESS){
			if (dwValue == 1){
				g_sIsSilent = TRUE;
			}
		}
	}
	key.Close();
	if (g_sIsSilent){
		return S_OK;
	}
	CComQIPtr<_Application> spApp(Application);
	ATLASSERT(spApp);
	if (!spApp){//获取失败说明是excel
		//g_sIsSilent = TRUE;
		return S_OK;
	}
	g_Application = spApp;
	
	HRESULT hr;
	CComPtr<Office::_CommandBars>  spCmdBars; 
	hr =  g_Application->get_CommandBars(&spCmdBars);
	if(FAILED(hr))
		return hr;
	ATLASSERT(spCmdBars);

	// now we add a new toolband to Word
	// to which we''ll add 2 buttons
	CComVariant vName("");
	CComPtr<Office::CommandBar> spNewCmdBar;

	CComVariant vPos(1); 
	CComVariant vTemp(VARIANT_TRUE); // menu is temporary        
	CComVariant vEmpty(DISP_E_PARAMNOTFOUND, VT_ERROR);            
	spNewCmdBar = spCmdBars->Add(vName, vPos, vEmpty, vTemp);

	CComPtr < Office::CommandBarControls> spBarControls;
	spBarControls = spNewCmdBar->GetControls();
	ATLASSERT(spBarControls);


	CComVariant vToolBarType(1);
	CComVariant vShow(VARIANT_TRUE);
	CComPtr<Office::CommandBarControl>  spNewBar; 

	// add first button
	spNewBar = spBarControls->Add(vToolBarType,vEmpty,vEmpty,vEmpty,vShow); 
	ATLASSERT(spNewBar);

	CComQIPtr < Office::_CommandBarButton> spCmdButton(spNewBar);

	ATLASSERT(spCmdButton);
	m_spButton = spCmdButton;
	AppEvents::DispEventAdvise(m_spButton);

	HBITMAP hBmp =(HBITMAP)::LoadImage(g_hModule,
		MAKEINTRESOURCE(IDB_BITMAP1),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
	::OpenClipboard(NULL);
	::EmptyClipboard();
	::SetClipboardData(CF_BITMAP, (HANDLE)hBmp);
	::CloseClipboard();
	::DeleteObject(hBmp);        

	spCmdButton->PutStyle(Office::msoButtonIconAndWrapCaption);
	hr = spCmdButton->PasteFace();
	if (FAILED(hr))
		return hr;
	spCmdButton->PutVisible(VARIANT_TRUE); 
	spCmdButton->PutCaption(OLESTR("Word安全锁")); 
	spCmdButton->PutEnabled(VARIANT_TRUE);
	spCmdButton->PutTooltipText(OLESTR("Word安全锁")); 
	spCmdButton->PutTag(OLESTR("给Word文档加密")); 
	spNewCmdBar->PutVisible(VARIANT_TRUE); 
	return S_OK;
}

HRESULT CConnect::OnDisconnection(ext_DisconnectMode RemoveMode, SAFEARRAY **custom)
{
	TSAUTO();
	return S_OK;
}

HRESULT CConnect::OnAddInsUpdate(SAFEARRAY **custom)
{
	TSAUTO();
	return S_OK;
}

HRESULT CConnect::OnStartupComplete(SAFEARRAY **custom)
{
	TSAUTO();
	if (g_sIsSilent){
		this->m_addinHelper.BeginTask();
	}
	return S_OK;
}

HRESULT CConnect::OnBeginShutdown(SAFEARRAY **custom)
{
	TSAUTO();
	return S_OK;
}

_ATL_FUNC_INFO OnClickButtonInfo = {CC_STDCALL,VT_EMPTY,2,{VT_DISPATCH,VT_BYREF|VT_BOOL}};

void __stdcall CConnect::OnClickButton1(IDispatch* Ctrl,VARIANT_BOOL * CancelDefault)
{
	DialogBox(g_hModule, MAKEINTRESOURCE(IDD_DIALOG1),
		GetForegroundWindow(), (DLGPROC)DialogProc);

}