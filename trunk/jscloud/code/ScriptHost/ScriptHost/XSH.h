// XSH.h : Declaration of the CXSH

#pragma once
#include "resource.h"       // main symbols

#include "ScriptHost_i.h"
#include "_IXSHEvents_CP.h"
#include <atlctl.h>
#include "map"
#include "activscp.h"
#include "ec.h"
#include "LaunchHelper.h"
#define  WM_TIMER_0SECOND WM_USER+100
#define WM_DOWNLOADFILE WM_USER+200

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CXSH

class ATL_NO_VTABLE CXSH :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CXSH, &CLSID_XSH>,
	public IConnectionPointContainerImpl<CXSH>,
	public CProxy_IXSHEvents<CXSH>,
	public IProvideClassInfo2Impl<&CLSID_XSH, &__uuidof(_IXSHEvents), &LIBID_ScriptHostLib>,
	public IObjectSafetyImpl<CXSH, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA> ,
	public IDispatchImpl<IXSH, &IID_IXSH, &LIBID_ScriptHostLib, /*wMajor =*/ 0xFFFF, /*wMinor =*/ 0xFFFF>,
	public CWindowImpl<CXSH>
{
public:
	CXSH()
	{
		m_dwTimerID = 2000;
	}
	~CXSH()
	{
		m_mapTimerID2info.clear();
		m_mapName2spDisp.clear();
	}
//DECLARE_REGISTRY_RESOURCEID(IDR_XSH)
DECLARE_NO_REGISTRY();


BEGIN_COM_MAP(CXSH)
	COM_INTERFACE_ENTRY(IXSH)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(IObjectSafety)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CXSH)
	CONNECTION_POINT_ENTRY(__uuidof(_IXSHEvents))
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()
	
	static DWORD GetWndStyle(DWORD dwStyle)
	{
		return dwStyle == 0 ? WS_CHILD: dwStyle;
	}

	static ATL::CWndClassInfo& GetWndClassInfo()
	{ 
		static std::wstring strClassName = LaunchConfig::Instance()->m_wstrXSHWndClass.c_str();;
		static ATL::CWndClassInfo wc = 
		{ 
			{ sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, StartWindowProc, 0, 0, NULL, NULL, NULL, (HBRUSH)(COLOR_WINDOW + 1), NULL, strClassName.c_str(), NULL }, NULL, NULL, IDC_ARROW, TRUE, 0, _T("") 
		}; 
		return wc; 
	}

	HRESULT FinalConstruct()
	{
		TSAUTO();
#ifndef HWND_MESSAGE
#define HWND_MESSAGE     ((HWND)-3)
#endif
		//Create(HWND_MESSAGE);
		Create(GetDesktopWindow(), 0 , L"1");
		return S_OK;
	}

	void FinalRelease()
	{
		TSAUTO();
		if( m_hWnd)
			DestroyWindow();
		if(m_spWindows)
			m_spWindows.Release();
		if(m_spScreen)
			m_spScreen.Release();
		if(m_spExternal)
			m_spExternal.Release();
		if(m_spStorage)
			m_spStorage.Release();
	}

public:
	STDMETHOD( Echo )( BSTR msg );
	STDMETHOD( alert )( BSTR msg );
	STDMETHOD( attachEvent )( IDispatch* source, BSTR event, VARIANT notify, VARIANT listener, VARIANT_BOOL* pRetVal );
	STDMETHOD( detachEvent )( IDispatch* source, BSTR event, VARIANT notify, VARIANT listener, VARIANT_BOOL* pRetVal );
	STDMETHOD(setTimeout)(VARIANT expression, long msec, VARIANT language, long* timerID);
	STDMETHOD(setInterval)(VARIANT expression, long msec, VARIANT language, long* timerID);
	STDMETHOD(clearInterval)(LONG timerID);
	STDMETHOD(clearTimeout)(LONG timerID);
	STDMETHOD(SetActiveScript)(IUnknown* pActiveScript);
private:
	CComPtr<IActiveScript> m_spActiveScript;
	DWORD GetTimerID()
	{
		if(0 == m_dwTimerID)
		{
			m_dwTimerID = 2000;
		}
		m_dwTimerID++;
		return m_dwTimerID;	
	}
	DWORD	m_dwTimerID;
	struct timerinfo 
	{
		bool boneoff;
		CComVariant vexpression;
	};
	std::map<DWORD,timerinfo> m_mapTimerID2info;

	//
	std::map<CComBSTR, CComPtr<IDispatch> > m_mapName2spDisp;
	CComPtr<IDispatch>m_spExternal;
	CComPtr<IDispatch>m_spScreen;
	CComPtr<IDispatch>m_spWindows;
	CComPtr<IDispatch>m_spStorage;


protected:
	HRESULT RunScript(VARIANT v, VARIANT p1, VARIANT p2, VARIANT p3 )
	{
		HRESULT hr = S_FALSE;
		VARIANT* pVar = &v;
		if(pVar && m_hWnd)
		{
			if(VT_DISPATCH == pVar->vt)
			{
				CComVariant avarParams[3];
				avarParams[2] = p1;
				avarParams[1] = p2;
				avarParams[0] = p3;

				CComVariant varResult;

				DISPPARAMS params = { avarParams, NULL, 3, 0 };


				hr = pVar->pdispVal->Invoke(DISPID_VALUE, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params,
					&varResult,NULL,NULL);
				ATLASSERT(SUCCEEDED(hr)); 
			}
			else if(VT_BSTR == pVar->vt)
			{
				CComQIPtr<IActiveScriptParse> spParse = m_spActiveScript;
				CComBSTR bstr = pVar->bstrVal;
				bstr.Append("();");
				spParse->ParseScriptText(bstr, L"XSH", 0,0,0,0,0,0,0);
				hr = m_spActiveScript->SetScriptState(SCRIPTSTATE_CONNECTED);
			}
			else
			{
				ATLASSERT(false);
			}
		}
		return hr;
	}

	LRESULT CreateInstance(REFCLSID rclsid, REFIID riid, LPVOID * ppv);

public:
	BEGIN_MSG_MAP(CXSH)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_TIMER_0SECOND, OnTimer0Second)		
		MESSAGE_HANDLER(WM_QUERYENDSESSION, OnQueryEndSession) 
	END_MSG_MAP()
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer0Second(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT	OnQueryEndSession(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);


	STDMETHOD(getAddin)(BSTR name, IDispatch** object);
	STDMETHOD(setAddin)(BSTR name, IDispatch*  object);
	STDMETHOD(trace)(BSTR msg);

	STDMETHOD(exit)(long code = 0);


	STDMETHOD(createActiveX)(BSTR progid, IDispatch**  ppax);
	STDMETHOD(createActiveX2)(BSTR path, BSTR clsid, BSTR iid,  IDispatch**  ppax);




	STDMETHOD(get___interrupted)(LONG* pVal);
	STDMETHOD(put___interrupted)(LONG newVal);
	STDMETHOD(get_external)(IDispatch** pVal);
	STDMETHOD(get___processid)(LONG* pVal);
	STDMETHOD(get___processhandle)(LONG* pVal);
	STDMETHOD(evalFile)(BSTR path, VARIANT* pVarRet);
	STDMETHOD(get_windows)(IDispatch** pVal);
	STDMETHOD(get_screen)(IDispatch** pVal);
	STDMETHOD(get___xpos)(LONG* pVal);
	STDMETHOD(get___ypos)(LONG* pVal);
	STDMETHOD(get_storage)(IDispatch** pVal);
	STDMETHOD(get___isadmin)(VARIANT_BOOL* pVal);
};

OBJECT_ENTRY_AUTO(__uuidof(XSH), CXSH)
