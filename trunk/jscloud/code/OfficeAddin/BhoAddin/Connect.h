// Connect.h : Declaration of the CConnect

#pragma once
#include "resource.h"       // main symbols

#include "BhoAddin_i.h"

#include <ShlObj.h>
#include <ComDef.h>
#include "AddinHelper.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CConnect

class ATL_NO_VTABLE CConnect :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CConnect, &CLSID_Connect>,
	//public IDispatchImpl<IConnect, &IID_IConnect, &LIBID_BhoAddinLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
	public IObjectWithSiteImpl<CConnect>
{
public:
	CConnect()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_CONNECT)


BEGIN_COM_MAP(CConnect)
	//COM_INTERFACE_ENTRY(IConnect)
	//COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectWithSite)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
		this->m_addinHelper.EndTask();
	}
private:
	AddinHelper m_addinHelper;
private:
	// ˭�Ȱ�start�����ԣ�����start���ҽ���һ��
	void StartOnlyOnce(const std::wstring &launchSrc,BOOL bForce = FALSE); 
public:
	STDMETHOD(SetSite)(IUnknown * pUnkSite); 

};

OBJECT_ENTRY_AUTO(__uuidof(Connect), CConnect)
