// Agent.h : Declaration of the CAgent

#pragma once
#include "resource.h"       // main symbols

#include "FlyRabbitAgent_i.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// CAgent

class ATL_NO_VTABLE CAgent :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAgent, &CLSID_Agent>,
	public IDispatchImpl<IAgent, &IID_IAgent, &LIBID_FlyRabbitAgentLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IObjectSafetyImpl<CAgent, INTERFACESAFE_FOR_UNTRUSTED_CALLER>
{
public:
	CAgent()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_AGENT)

DECLARE_NOT_AGGREGATABLE(CAgent)

BEGIN_COM_MAP(CAgent)
	COM_INTERFACE_ENTRY(IAgent)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectSafety)
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

	STDMETHOD(AddTask)(VARIANT varUrl, VARIANT varType, VARIANT varPoint);
};

OBJECT_ENTRY_AUTO(__uuidof(Agent), CAgent)
