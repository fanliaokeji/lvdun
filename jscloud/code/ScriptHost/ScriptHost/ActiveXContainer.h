// ActiveXContainer.h : Declaration of the CActiveXContainer

#pragma once
#include "resource.h"       // main symbols

#include "ScriptHost_i.h"
#include "_IActiveXContainerEvents_CP.h"
#include <atlctl.h>


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CActiveXContainer

class ATL_NO_VTABLE CActiveXContainer :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CActiveXContainer, &CLSID_ActiveXContainer>,
	public IConnectionPointContainerImpl<CActiveXContainer>,
	public CProxy_IActiveXContainerEvents<CActiveXContainer>,
	public IProvideClassInfo2Impl<&CLSID_ActiveXContainer, &__uuidof(_IActiveXContainerEvents), &LIBID_ScriptHostLib>,
	public IDispatchImpl<IActiveXContainer, &IID_IActiveXContainer, &LIBID_ScriptHostLib, /*wMajor =*/ 0xFFFF, /*wMinor =*/ 0xFFFF>,
	public IObjectSafetyImpl<CActiveXContainer, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>
{
public:
	CActiveXContainer();

//DECLARE_REGISTRY_RESOURCEID(IDR_ACTIVEXCONTAINER)
DECLARE_NO_REGISTRY();


BEGIN_COM_MAP(CActiveXContainer)
	COM_INTERFACE_ENTRY(IActiveXContainer)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(IObjectSafety)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CActiveXContainer)
	CONNECTION_POINT_ENTRY(__uuidof(_IActiveXContainerEvents))
END_CONNECTION_POINT_MAP()

BEGIN_SINK_MAP(CActiveXContainer)
END_SINK_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();

	void FinalRelease();

public:
	STDMETHOD( GetTypeInfoCount )( OUT UINT *pctinfo );
	STDMETHOD( GetTypeInfo )( IN UINT iTInfo, IN LCID lcid, OUT ITypeInfo **ppTInfo );
	STDMETHOD( GetIDsOfNames )( IN REFIID riid, /* [size_is] */ IN LPOLESTR *rgszNames, IN UINT cNames, IN LCID lcid, /* [size_is] */ OUT DISPID *rgDispId );
	STDMETHOD( Invoke )( IN DISPID dispIdMember, IN REFIID riid, IN LCID lcid, IN WORD wFlags, OUT IN DISPPARAMS *pDispParams, OUT VARIANT *pVarResult, OUT EXCEPINFO *pExcepInfo, OUT UINT *puArgErr );

public:
	STDMETHOD(createObject)(BSTR clsid, BSTR path);
	STDMETHOD(get_object)(IDispatch** pVal);
	STDMETHOD(put_object)(IDispatch* newVal);
	STDMETHOD(attachEvent)(BSTR event, VARIANT notify, VARIANT listener, VARIANT_BOOL* pRetVal);
	STDMETHOD(detachEvent)(BSTR event, VARIANT notify, VARIANT listener, VARIANT_BOOL* pRetVal);

protected:
	//CLSID				m_ObjectClsid;
	CComPtr<IDispatch>	m_spObject;

};

OBJECT_ENTRY_AUTO(__uuidof(ActiveXContainer), CActiveXContainer)
