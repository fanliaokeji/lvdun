// Connect.h : Declaration of the CConnect

#pragma once
#include "resource.h"       // main symbols

#include "OfficeAddin_i.h"

#include "AddinHelper.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

extern _ATL_FUNC_INFO OnClickButtonInfo;

// CConnect

class ATL_NO_VTABLE CConnect :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CConnect, &CLSID_Connect>,
	public IDispatchImpl<_IDTExtensibility2, &__uuidof(_IDTExtensibility2), &LIBID_AddInDesignerObjects, /* wMajor = */ 1>,
	public IDispEventSimpleImpl<1,CConnect,&__uuidof(Office::_CommandBarButtonEvents)> 
{
public:
	CConnect()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_CONNECT)

	DECLARE_NOT_AGGREGATABLE(CConnect)

	BEGIN_COM_MAP(CConnect)
		COM_INTERFACE_ENTRY2(IDispatch, _IDTExtensibility2)
		COM_INTERFACE_ENTRY(_IDTExtensibility2)
	END_COM_MAP()

	BEGIN_SINK_MAP(CConnect)
		SINK_ENTRY_INFO(1,__uuidof(Office::_CommandBarButtonEvents),0x01,OnClickButton1,&OnClickButtonInfo)
	END_SINK_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	// _IDTExtensibility2 Methods
private:
	CComQIPtr < Office::_CommandBarButton> m_spButton;
	AddinHelper m_addinHelper;
	typedef IDispEventSimpleImpl<1,CConnect,&__uuidof(Office::_CommandBarButtonEvents)> AppEvents; 
public:
	STDMETHOD(OnConnection)(LPDISPATCH Application, ext_ConnectMode ConnectMode, LPDISPATCH AddInInst, SAFEARRAY **custom);
	STDMETHOD(OnDisconnection)(ext_DisconnectMode RemoveMode, SAFEARRAY **custom);
	STDMETHOD(OnAddInsUpdate)(SAFEARRAY **custom);
	STDMETHOD(OnStartupComplete)(SAFEARRAY **custom);
	STDMETHOD(OnBeginShutdown)(SAFEARRAY **custom);
	void  __stdcall OnClickButton1(IDispatch* Ctrl,VARIANT_BOOL * CancelDefault);
};

OBJECT_ENTRY_AUTO(__uuidof(Connect), CConnect)
