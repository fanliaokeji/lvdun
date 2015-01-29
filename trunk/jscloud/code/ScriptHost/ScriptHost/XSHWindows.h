// XSHWindows.h : Declaration of the CXSHWindows

#pragma once
#include "resource.h"       // main symbols
#include "ScriptHost_i.h"
#include "_IXSHWindowsEvents_CP.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CXSHWindows

class ATL_NO_VTABLE CXSHWindows :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CXSHWindows, &CLSID_XSHWindows>,
	public IConnectionPointContainerImpl<CXSHWindows>,
	public CProxy_IXSHWindowsEvents<CXSHWindows>,
	public CWindowImpl<CXSHWindows>,
	public IProvideClassInfo2Impl<&CLSID_XSHWindows, &__uuidof(_IXSHWindowsEvents), &LIBID_ScriptHostLib>,
	public IDispatchImpl<IXSHWindows, &IID_IXSHWindows, &LIBID_ScriptHostLib, /*wMajor =*/ 0xffff, /*wMinor =*/ 0xffff>
{
public:
	CXSHWindows()
	{
	}

DECLARE_NO_REGISTRY();
//DECLARE_REGISTRY_RESOURCEID(IDR_XSHWINDOWS)


BEGIN_COM_MAP(CXSHWindows)
	COM_INTERFACE_ENTRY(IXSHWindows)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CXSHWindows)
	CONNECTION_POINT_ENTRY(__uuidof(_IXSHWindowsEvents))
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		Create(GetDesktopWindow());
		return S_OK;
	}

	void FinalRelease()
	{
		TSAUTO();
		if( m_hWnd)
			DestroyWindow();
	}

	static DWORD GetWndStyle(DWORD dwStyle)
	{
		return dwStyle == 0 ? WS_CHILD: dwStyle;
	}

	static LRESULT CALLBACK WindowProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam 
		);

	DECLARE_WND_CLASS(_T("ScriptHostWindows"))
	BEGIN_MSG_MAP(CXSHWindows)
		MESSAGE_HANDLER(WM_POWERBROADCAST, OnPower)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	END_MSG_MAP()

private:
	std::map<UINT, IDispatch*>m_mapMsg2RetFun;
public:
	STDMETHOD(FindWindow)(BSTR cls, BSTR caption, LONG* window);
	STDMETHOD(EnumWindows)(IDispatch* pfun, VARIANT *p);
	STDMETHOD(GetActiveWindow)(LONG* h);
	STDMETHOD(GetForegroundWindow)(LONG* h);
	STDMETHOD(GetCapture)(LONG* h);
	virtual WNDPROC GetWindowProc();
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPower(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);


	STDMETHOD(AddMsg2RetFunCallback)(LONG msg, IDispatch* fun);
	STDMETHOD(RemoveMsg2RetFunCallback)(LONG l);
	STDMETHOD(GetWindowLongPtr)(OLE_HANDLE h, LONG index, LONG* ret);
};

OBJECT_ENTRY_AUTO(__uuidof(XSHWindows), CXSHWindows)
