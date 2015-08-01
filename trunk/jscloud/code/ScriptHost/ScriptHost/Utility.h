// Utility.h : Declaration of the CUtility

#pragma once
#include "resource.h"       // main symbols

#include "ScriptHost_i.h"
#include "atlctl.h"
#include "vector"
#include "AFileSignInfo.h"

#define WM_DOWNLOADFILE WM_USER+200
#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CUtility

class ATL_NO_VTABLE CUtility :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUtility, &CLSID_Utility>,
	public IObjectSafetyImpl<CUtility, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA> ,
	public CWindowImpl<CUtility>,
	public IDispatchImpl<IUtility, &IID_IUtility, &LIBID_ScriptHostLib, /*wMajor =*/ 0xffff, /*wMinor =*/ 0xffff>
{
public:
	CUtility()
	{
	}
DECLARE_NO_REGISTRY();
//DECLARE_REGISTRY_RESOURCEID(IDR_UTILITY1)


BEGIN_COM_MAP(CUtility)
	COM_INTERFACE_ENTRY(IUtility)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		TSAUTO();
		Create((HWND)-3);
		return S_OK;
	}

	void FinalRelease()
	{
		TSAUTO();
		if( m_hWnd)
			DestroyWindow();
	}
	
	BEGIN_MSG_MAP(CUtility)  

		MESSAGE_HANDLER(WM_DOWNLOADFILE, OnDownloadFile)
	END_MSG_MAP()

private:
	std::vector<LONG> m_lChildProcesses;
public:
	STDMETHOD(GetCurrentHostDllPath)(BSTR* ppid);
	STDMETHOD(GetPID)(BSTR* ppid);
	STDMETHOD(GetHostVersion)(BSTR* pver);
	STDMETHOD(GetHostBuild)(LONG* pv);
	STDMETHOD(GetEIVersion)(BSTR* pver);
	STDMETHOD(GetEIBuild)(LONG* pv);
	STDMETHOD(ShellExcute)(BSTR app, BSTR param, BSTR workdir, LONG startupflag, LONG *pProcessId);
	STDMETHOD(lnterrupted)(LONG l);
	STDMETHOD(GetLastInputInfo)(LONG* l);
	STDMETHOD(SetEnvironmentVariable)(BSTR name, BSTR value);
	STDMETHOD(GetEnvironmentVariable)(BSTR name, BSTR* pvalue);
	STDMETHOD(GetTickCount)(LONG* l);
	STDMETHOD(get_windows)(IDispatch** pVal);
	STDMETHOD(IsSupportOpencl)(LONG* pl);
	STDMETHOD(Open)(BSTR url);
	STDMETHOD(OpenUrl)(BSTR url, LONG mode);

	LRESULT OnDownloadFile(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/); 
	STDMETHOD(DownloadURL)(BSTR url, BSTR dest, VARIANT expression,  VARIANT autoload);

	STDMETHOD(TerminateProcess)(LONG processid, LONG tree);
	STDMETHOD(GetOrganizationName)(BSTR path, BSTR* name);
	STDMETHOD(WaitForSingleObject)(LONG h, LONG ms, LONG* ret);
	STDMETHOD(SetThreadExecutionState)(LONG l);
	STDMETHOD(get___debugging)(LONG* pVal);
	STDMETHOD(EncryptFile)(BSTR source,BSTR dest);
	STDMETHOD (RepairIcon4IE)();


};

OBJECT_ENTRY_AUTO(__uuidof(Utility), CUtility)
