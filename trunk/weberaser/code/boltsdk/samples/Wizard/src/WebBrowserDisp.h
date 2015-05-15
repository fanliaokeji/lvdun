// WebBrowserDisp.h : Declaration of the CWebBrowserDisp

#pragma once
#include "resource.h"       // main symbols

#include "Wizard.h"


// CWebBrowserDisp

// չʾ��һ���򵥵ĺ�webbrowseobject������ʵ�ַ���
class ATL_NO_VTABLE CWebBrowserDisp : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CWebBrowserDisp, &CLSID_WebBrowserDisp>,
	public IDispatchImpl<IWebBrowserDisp, &IID_IWebBrowserDisp, &LIBID_WizardLib, /*wMajor =*/ 0xFFFF, /*wMinor =*/ 0xFFFF>
{
public:
	CWebBrowserDisp()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_WEBBROWSERDISP)


BEGIN_COM_MAP(CWebBrowserDisp)
	COM_INTERFACE_ENTRY(IWebBrowserDisp)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

public:

	STDMETHOD(ShowMessage)(BSTR msg);
};

OBJECT_ENTRY_AUTO(__uuidof(WebBrowserDisp), CWebBrowserDisp)
