// Connect.cpp : Implementation of CConnect

#include "stdafx.h"
#include "Connect.h"


// CConnect
extern HINSTANCE g_hThisModule;

void CConnect::StartOnlyOnce(const std::wstring &launchSrc,BOOL bForce)
{
	TSAUTO();

	static bool bAlreadyStart = false;
	if (bAlreadyStart)
	{
		return;	
	}
	bAlreadyStart = true;
	this->m_addinHelper.BeginTask();
}

STDMETHODIMP CConnect::GetOverlayInfo(LPWSTR pwszIconFile,
									  int cchMax,int* pIndex,
									  DWORD* pdwFlags)
{
	//TSTRACEAUTO();

	GetModuleFileNameW(_AtlBaseModule.GetModuleInstance(), pwszIconFile, cchMax);
	TSDEBUG4CXX(_T("[out] pwszIconFile = ") << pwszIconFile);

	*pIndex = 0;
	*pdwFlags = ISIOI_ICONFILE | ISIOI_ICONINDEX;

	StartOnlyOnce(L"explorer_iconoverlay");

	return S_OK;
}

// IShellIconOverlayIdentifier Method Implementation 
// returns the priority of this overlay 0 being the highest. 
// this overlay is always selected do to its high priority 
STDMETHODIMP CConnect::GetPriority(int* pPriority)
{
	//TSTRACEAUTO();
	// highest priority
	*pPriority=0;

	StartOnlyOnce(L"explorer_iconoverlay");

	return S_OK;
}

// IShellIconOverlayIdentifier Method Implementation
// IShellIconOverlayIdentifier::IsMemberOf
// Returns Whether the object should have this overlay or not 
STDMETHODIMP CConnect::IsMemberOf(LPCWSTR pwszPath, DWORD dwAttrib)
{
	UNREFERENCED_PARAMETER(pwszPath);
	UNREFERENCED_PARAMETER(dwAttrib);
	//TSTRACEAUTO();
	HRESULT r = S_FALSE;

	StartOnlyOnce(L"explorer_iconoverlay");

	return r;
}


// ICopyHook Method CopyCallback
// Determines whether the Shell will be allowed to move, copy, delete, or rename a folder or printer object. 
STDMETHODIMP_(UINT) CConnect::CopyCallback(HWND hwnd,UINT wFunc,UINT wFlags,LPCTSTR pszSrcFile,DWORD dwSrcAttribs,LPCTSTR pszDestFile,DWORD dwDestAttribs)
{
	UNREFERENCED_PARAMETER(hwnd);
	UNREFERENCED_PARAMETER(wFunc);
	UNREFERENCED_PARAMETER(wFlags);
	UNREFERENCED_PARAMETER(pszSrcFile);
	UNREFERENCED_PARAMETER(dwSrcAttribs);
	UNREFERENCED_PARAMETER(pszDestFile);
	UNREFERENCED_PARAMETER(dwDestAttribs);
	TSTRACEAUTO();
	StartOnlyOnce(L"explorer_copyhook");
	return IDYES;
}

//////////////////////////////////////////////////////////////////////////

//
STDMETHODIMP CConnect::SetSite(IUnknown*pUnkSite)  
{  
	TSTRACEAUTO();
	if(pUnkSite!=NULL)  
	{  
		//缓存指向IWebBrowser2的指针。  
		//pUnkSite->QueryInterface(IID_IWebBrowser2,(void**)&m_spWebBrowser);  
		StartOnlyOnce(L"bho");
	}  
	else  
	{  
		//在此释放缓存的指针和其他资源。  
		//m_spWebBrowser.Release();  
	}  
	//返回基类实现  
	return IObjectWithSiteImpl::SetSite(pUnkSite);  
}
