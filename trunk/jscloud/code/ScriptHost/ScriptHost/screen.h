// screen.h : Declaration of the Cscreen

#pragma once
#include "resource.h"       // main symbols

#include "ScriptHost_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// Cscreen

class ATL_NO_VTABLE Cscreen :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<Cscreen, &CLSID_screen>,
	public IDispatchImpl<Iscreen, &IID_Iscreen, &LIBID_ScriptHostLib, /*wMajor =*/ 0xffff, /*wMinor =*/ 0xffff>
{
public:
	Cscreen()
	{
	}
DECLARE_NO_REGISTRY();
//DECLARE_REGISTRY_RESOURCEID(IDR_SCREEN)


BEGIN_COM_MAP(Cscreen)
	COM_INTERFACE_ENTRY(Iscreen)
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
	STDMETHOD(get_width)(LONG* pVal);
	STDMETHOD(get_height)(LONG* pVal);
	STDMETHOD(get_availHeight)(LONG* pVal);
	STDMETHOD(get_availWidth)(LONG* pVal);
};

OBJECT_ENTRY_AUTO(__uuidof(screen), Cscreen)
