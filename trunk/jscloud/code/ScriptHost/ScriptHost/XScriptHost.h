// XScriptHost.h : Declaration of the CXScriptHost

#pragma once
#include "resource.h"       // main symbols

#include "ScriptHost_i.h"
#include "_IXScriptHostEvents_CP.h"
#include <ActivScp.h>
#include "activdbg.h"
#include <atlctl.h>
#include <map>
using namespace std;
extern std::wstring g_strcmdline;
#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CXScriptHost

class ATL_NO_VTABLE CXScriptHost :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CXScriptHost, &CLSID_XScriptHost>,
	public IConnectionPointContainerImpl<CXScriptHost>,
	public CProxy_IXScriptHostEvents<CXScriptHost>,
	public IProvideClassInfo2Impl<&CLSID_XScriptHost, &__uuidof(_IXScriptHostEvents), &LIBID_ScriptHostLib>,
	public IDispatchImpl<IXScriptHost, &IID_IXScriptHost, &LIBID_ScriptHostLib, /*wMajor =*/ 0xFFFF, /*wMinor =*/ 0xFFFF>,
	public IPersistFile,
	public IActiveScriptSite, 
	public IActiveScriptSiteWindow,	
	public IActiveScriptSiteDebug,
	public IObjectSafetyImpl<CXScriptHost, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>
{
public:
	CXScriptHost();

//DECLARE_REGISTRY_RESOURCEID(IDR_XSCRIPTHOST)
DECLARE_NO_REGISTRY();

BEGIN_COM_MAP(CXScriptHost)
	COM_INTERFACE_ENTRY(IXScriptHost)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IActiveScriptSite)
	COM_INTERFACE_ENTRY(IActiveScriptSiteWindow)
	COM_INTERFACE_ENTRY(IActiveScriptSiteDebug)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(IObjectSafety)

END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CXScriptHost)
	CONNECTION_POINT_ENTRY(__uuidof(_IXScriptHostEvents))
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();

	void FinalRelease();
protected:
	CComBSTR					m_bstrFileName;
	CComPtr<IXMLDOMDocument>	m_spDocument;
	map<CComBSTR, CComPtr<IActiveScript> >			m_mapEngine;
	map<CComBSTR, CComPtr<IActiveScriptParse> >		m_mapParse;
	CComPtr<IXSH>				m_spXSH;

public:		// IPersistFile method
	STDMETHOD( GetClassID )( 
		/* [out] */ CLSID *pClassID);

	STDMETHOD( IsDirty )( void);

	STDMETHOD( Load )( 
		/* [in] */ LPCOLESTR pszFileName,
		/* [in] */ DWORD dwMode);

	STDMETHOD( Save )( 
		/* [unique][in] */ LPCOLESTR pszFileName,
		/* [in] */ BOOL fRemember);

	STDMETHOD( SaveCompleted )( 
		/* [unique][in] */ LPCOLESTR pszFileName);

	STDMETHOD( GetCurFile )( 
		/* [out] */ LPOLESTR *ppszFileName);


public:		// IActiveScript method
	STDMETHOD( GetLCID )( 
		/* [out] */ LCID *plcid);

	STDMETHOD( GetItemInfo )( 
		/* [in] */ LPCOLESTR pstrName,
		/* [in] */ DWORD dwReturnMask,
		/* [out] */ IUnknown **ppiunkItem,
		/* [out] */ ITypeInfo **ppti);

	STDMETHOD( GetDocVersionString )( 
		/* [out] */ BSTR *pbstrVersion);

	STDMETHOD( OnScriptTerminate )( 
		/* [in] */ const VARIANT *pvarResult,
		/* [in] */ const EXCEPINFO *pexcepinfo);

	STDMETHOD( OnStateChange )( 
		/* [in] */ SCRIPTSTATE ssScriptState);

	STDMETHOD( OnScriptError )( 
		/* [in] */ IActiveScriptError *pscripterror);

	STDMETHOD( OnEnterScript )( void);

	STDMETHOD( OnLeaveScript )( void);

public:		// IActiveScriptSiteWindow method
	STDMETHOD( GetWindow )( 
		/* [out] */ HWND *phwnd);

	STDMETHOD( EnableModeless )( 
		/* [in] */ BOOL fEnable);


	STDMETHODIMP GetDocumentContextFromPosition(DWORD dwSourceContext, ULONG uCharacterOffset, ULONG uNumChars, IDebugDocumentContext __RPC_FAR *__RPC_FAR *ppsc);
	STDMETHODIMP GetApplication(IDebugApplication __RPC_FAR *__RPC_FAR *ppda);
	STDMETHODIMP GetRootApplicationNode(IDebugApplicationNode __RPC_FAR *__RPC_FAR *ppdanRoot);
	STDMETHODIMP OnScriptErrorDebug(IActiveScriptErrorDebug __RPC_FAR *pErrorDebug, BOOL __RPC_FAR *pfEnterDebugger, BOOL __RPC_FAR *pfCallOnScriptErrorWhenContinuing);
private:
	IProcessDebugManager* mpDebugMgr;
	IDebugApplication*	  mpDebugApp;
	IDebugDocumentHelper*	mpDebugDoc;
	DWORD					m_AppCookie;
	HRESULT CreateScriptDebugger();
	void ReleaseScriptDebugger();
	HRESULT CreateDocumentForDebugger(BSTR filePath,BSTR scripts);
	void ReleaseDebugDocument();
public:
	STDMETHOD( Load )( BSTR FileName, LONG Mode  );
	STDMETHOD( Run )( VARIANT *pRet );
private:
	BOOL PreLoadScript(IActiveScript*, IActiveScriptParse* );
	BOOL MakePath(CComBSTR& bstrPath,const BSTR bstrCurrentPath=NULL);
	void OnLoad();
	void OnUnload();

public:
	STDMETHOD(SetAddin)(BSTR name, IDispatch* object);

};

OBJECT_ENTRY_AUTO(__uuidof(XScriptHost), CXScriptHost)
