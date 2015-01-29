// Istorage.h : Declaration of the CIstorage

#pragma once
#include "resource.h"       // main symbols

#include "ScriptHost_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// Cstorage
#define Istorage Istorage_ud
#define IID_Istorage IID_Istorage_ud
#define CLSID_storage CLSID_storage_ud
#define storage storage_ud

class ATL_NO_VTABLE Cstorage :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<Cstorage, &CLSID_storage>,
	public IDispatchImpl<Istorage, &IID_Istorage, &LIBID_ScriptHostLib, /*wMajor =*/ 0xffff, /*wMinor =*/ 0xffff>
{
public:
	Cstorage()
	{
	}

DECLARE_NO_REGISTRY();
//DECLARE_REGISTRY_RESOURCEID(IDR_STORAGE)


BEGIN_COM_MAP(Cstorage)
	COM_INTERFACE_ENTRY(Istorage)
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
	STDMETHOD(GetFolderPath)(long h, long h2 ,long h3 ,long h4 , BSTR* bstrPath);
	STDMETHOD(ExtractCab)(BSTR bstrSrc, BSTR bstrDest, VARIANT_BOOL* pvRet);
	STDMETHOD(GetFileMD5)(BSTR bstrFile, BSTR* pbstrMD5);

	STDMETHOD(PathFileExists)(BSTR path, LONG* b);
	STDMETHOD(GetPrivateProfileString)(BSTR path, BSTR app, BSTR key, BSTR* pvalue);
	STDMETHOD(WritePrivateProfileValue)(BSTR path, BSTR app, BSTR key, VARIANT value);
	STDMETHOD(RegQueryValue)(BSTR bstrRootName, BSTR bstrKey, BSTR bstrValueName, VARIANT* pRet);
	STDMETHOD(RegSetValue)(BSTR bstrRootName, BSTR bstrKey, BSTR bstrValueName, VARIANT Ret);
	STDMETHOD(RegDeleteValue)(BSTR bstrRootName, BSTR bstrKey, BSTR bstrValueName);
	STDMETHOD(PathCanonicalize)(BSTR path, BSTR* pdest);
	STDMETHOD(ExpandEnvironmentStrings)(BSTR path, BSTR* pdest);


	STDMETHOD(Save)(IStream* stream, BSTR path, LONG* ret);
	STDMETHOD(CreateFileMapping)(LONG flag, LONG size, BSTR name, LONG* h);
	STDMETHOD(WriteFileMapping)(LONG h, BSTR buf, LONG* ret);
	STDMETHOD(ReadFileMapping)(LONG h, LONG size, BSTR* ret);
	STDMETHOD(OpenFileMapping)(LONG flag, LONG inherit, BSTR name, LONG* ret);
};

OBJECT_ENTRY_AUTO(__uuidof(storage), Cstorage)
