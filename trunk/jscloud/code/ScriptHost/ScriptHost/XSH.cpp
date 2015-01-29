// XSH.cpp : CXSH ��ʵ��

#include "stdafx.h"
#include ".\XSH.h"
#include "DispEventCenter.h"
#include "atlstr.h"
#include <fstream>
#include "ScriptHost_i.h"


extern int __EXITCODE ;
// CXSH


STDMETHODIMP CXSH::Echo(BSTR msg)
{
	if(NULL == msg || 1024 < wcslen(msg))
		return S_FALSE;
	::MessageBoxW( NULL, msg, L"Script Host", MB_OK );
	return S_OK;
}

STDMETHODIMP CXSH::alert(BSTR msg)
{
	if(NULL == msg || 1024 < wcslen(msg))
		return S_FALSE;
	return Echo( msg );  
}

STDMETHODIMP CXSH::attachEvent( IDispatch* source, BSTR event, VARIANT notify, VARIANT listener, VARIANT_BOOL* pRetVal )
{
	TSAUTO();
	if(NULL == event || 1024 < wcslen(event))
		return S_FALSE;
	return CDispEventCenter::GetInstance().attachEvent( source, event, notify, listener, pRetVal );
}

STDMETHODIMP CXSH::detachEvent( IDispatch* source, BSTR event, VARIANT notify, VARIANT listener, VARIANT_BOOL* pRetVal )
{
	TSAUTO();
	return CDispEventCenter::GetInstance().detachEvent( source, event, notify, listener, pRetVal );
}

STDMETHODIMP CXSH::setTimeout(VARIANT expression, long msec, VARIANT language, long* timerID)
{
	// TODO: �ڴ����ʵ�ִ���
	if(NULL == m_hWnd)
	{
		TSWARN4CXX("m_hWnd==null, maybe will exit");
		return S_FALSE;
	}
	if(VT_BSTR == expression.vt && 256 < wcslen(expression.bstrVal))
		return S_FALSE;
	if(0 > msec /*|| 1000* 3600*24 < msec*/)
		return S_FALSE;
	if(0 == msec)
	{
		PostMessage(WM_TIMER_0SECOND, 0, (LPARAM) &expression);
		return S_OK;
	}
	DWORD dwTimerID = GetTimerID();
	SetTimer(dwTimerID, msec);
	timerinfo info;
	info.boneoff = 1;
	info.vexpression = expression;
	m_mapTimerID2info[dwTimerID] = info;
	*timerID = dwTimerID;
	TSDEBUG4CXX(" timerid "<<dwTimerID<<" , interval : "<<msec);
	return S_OK;
}

STDMETHODIMP CXSH::setInterval(VARIANT expression, long msec, VARIANT language, long* timerID)
{
	if(NULL == m_hWnd)
	{
		TSWARN4CXX("m_hWnd==null, maybe will exit");
		return S_FALSE;
	}
	// TODO: �ڴ����ʵ�ִ���
	if(VT_BSTR == expression.vt && 256 < wcslen(expression.bstrVal))
		return S_FALSE;
	DWORD dwTimerID = GetTimerID();
	SetTimer(dwTimerID, msec);
	timerinfo info;
	info.boneoff = 0;
	info.vexpression = expression;

	m_mapTimerID2info[dwTimerID] = info;
	* timerID = dwTimerID;
	TSDEBUG4CXX(" timerid "<<dwTimerID<<" , interval : "<<msec);
	return S_OK;
}

STDMETHODIMP CXSH::clearInterval(LONG timerID)
{
	TSAUTO();
	// TODO: �ڴ����ʵ�ִ���
	std::map<DWORD,timerinfo>::iterator iter = m_mapTimerID2info.begin();
	for (; iter != m_mapTimerID2info.end(); iter++)
	{
		if(iter->first == timerID)
		{
			KillTimer(timerID);
			m_mapTimerID2info.erase(iter);
			break;
		}
	}	

	return S_OK;
}
STDMETHODIMP CXSH::clearTimeout(LONG timerID)
{
	TSAUTO();
	return  clearInterval(timerID);

}
STDMETHODIMP CXSH::SetActiveScript(IUnknown* pActiveScript)
{
	// TODO: �ڴ����ʵ�ִ���
	//CComQIPtr<IActiveScript> spActiveScript = pActiveScript;
	pActiveScript->QueryInterface(IID_IActiveScript, (void**) &m_spActiveScript);	

	return S_OK;
}

LRESULT CXSH::OnTimer(UINT /*uMsg*/, WPARAM  wParam , LPARAM  lParam , BOOL& /*bHandled*/)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	SCRIPTSTATE  ssState;
	HRESULT hr = S_OK;
	CComVariant v;
	if(!m_spActiveScript && m_hWnd)
		return S_OK;
	hr = m_spActiveScript->GetScriptState(&ssState);
	if(SUCCEEDED(hr) && ssState != SCRIPTSTATE_CLOSED && ssState != SCRIPTSTATE_UNINITIALIZED)
	{
		std::map<DWORD,timerinfo>::iterator iter = m_mapTimerID2info.begin();
		for (; iter != m_mapTimerID2info.end(); iter++)
		{
			if(iter->first == wParam)
			{
				v =	iter->second.vexpression;
				if(iter->second.boneoff)
				{
					TSDEBUG4CXX(" kill one timeout : timerid="<<(LONG)iter->first<<" fun="<<v.pdispVal);
					KillTimer(iter->first);
					//parsescripttext���첽�ġ� 
					m_mapTimerID2info.erase(iter);
				}
				if(VT_DISPATCH == v.vt)
				{
					VARIANT VarResult;
					EXCEPINFO ExcepInfo;
					UINT uArgErr;
					DISPPARAMS params;
					params.cArgs=0;
					params.cNamedArgs=0;
					params.rgdispidNamedArgs=NULL;
					params.rgvarg=NULL;
					hr = v.pdispVal->Invoke(DISPID_VALUE, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params,
						&VarResult,&ExcepInfo,&uArgErr);
					ATLASSERT(SUCCEEDED(hr)); 
				}
				else if(VT_BSTR == v.vt)
				{
					CComQIPtr<IActiveScriptParse> spParse = m_spActiveScript;
					CComBSTR bstr = v.bstrVal;
					bstr.Append("();");
					spParse->ParseScriptText(bstr, L"XSH", 0,0,0,0,0,0,0);
					m_spActiveScript->SetScriptState(SCRIPTSTATE_CONNECTED);
				}
				else
				{
					ATLASSERT(false);
				}
				break;
			}

		}
	}
	return 0;
}


LRESULT CXSH::OnTimer0Second(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	HRESULT hr = S_OK;
	VARIANT* pVar = (VARIANT*)lParam;
	if(pVar)
	{
		if(VT_DISPATCH == pVar->vt)
		{
			VARIANT VarResult;
			EXCEPINFO ExcepInfo;
			UINT uArgErr;
			DISPPARAMS params;
			params.cArgs=0;
			params.cNamedArgs=0;
			params.rgdispidNamedArgs=NULL;
			params.rgvarg=NULL;
			hr = pVar->pdispVal->Invoke(DISPID_VALUE, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params,
				&VarResult,&ExcepInfo,&uArgErr);
			ATLASSERT(SUCCEEDED(hr)); 
		}
		else if(VT_BSTR == pVar->vt)
		{
			CComQIPtr<IActiveScriptParse> spParse = m_spActiveScript;
			CComBSTR bstr = pVar->bstrVal;
			bstr.Append("();");
			spParse->ParseScriptText(bstr, L"XSH", 0,0,0,0,0,0,0);
			m_spActiveScript->SetScriptState(SCRIPTSTATE_CONNECTED);
		}
		else
		{
			ATLASSERT(false);
		}
	}
	return S_OK;

}
LRESULT	CXSH::OnQueryEndSession(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	TSAUTO();
	static bool bSave = true;
	bHandled = FALSE;// (m_lPreState == PS_INVALID);
	if(bSave)
	{
		bSave = false;		
		exit(1);
		CComVariant v1,v2,v;
		v1 = 0L;
		v2 = 1L;
		m_spExternal.Invoke2(L"TerminateProcess",&v1,&v2, &v);		
	}	
	return 0;
}




STDMETHODIMP CXSH::getAddin(BSTR name, IDispatch** object)
{
	// TODO: �ڴ����ʵ�ִ���
	if(NULL == name || 256 < wcslen(name))
		return S_FALSE;
	CComBSTR bstr = name;
	bstr.ToLower();
	*object = m_mapName2spDisp[bstr.m_str];
	if(*object)
		(*object)->AddRef();

	return S_OK;
}

STDMETHODIMP CXSH::setAddin(BSTR name, IDispatch* object)
{
	// TODO: �ڴ����ʵ�ִ���
	if(NULL == name || 256 < wcslen(name))
		return S_FALSE;
	CComBSTR bstr = name;
	bstr.ToLower();
	if(bstr.Length() > 0 && NULL != object)
		m_mapName2spDisp[name] = object;
	return S_OK;
}

STDMETHODIMP CXSH::trace(BSTR msg)
{	
	// TODO: �ڴ����ʵ�ִ���
	std::basic_string<wchar_t> strMsg = L"XScriptHost trace : ";
	strMsg  +=msg;
	if(ISTSDEBUGVALID())
	{
		TSDEBUG4CXX(strMsg.c_str());
	}
	else
	{
		strMsg += L"\n";
		OutputDebugString(strMsg.c_str());
	}
	return S_OK;
}


STDMETHODIMP CXSH::exit (long code )
{
	TSAUTO();
	if( m_hWnd)
	{
		DestroyWindow();
	}
	__EXITCODE = code;
	PostQuitMessage(code);
#ifdef _DEBUG 

#else
	//TerminateProcess(GetCurrentProcess(), code);
#endif

	return S_OK;
}


STDMETHODIMP CXSH::createActiveX(BSTR progid, IDispatch**  ppax)
{
	TSAUTO();
	CComPtr<IDispatch> spDisp ;
	spDisp.CoCreateInstance(progid);
	*ppax = spDisp.Detach();
	return S_OK;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv);
LRESULT CXSH::CreateInstance( REFCLSID rclsid, REFIID riid, LPVOID * ppv)
{
	LRESULT hr = E_FAIL;
	CComPtr<IClassFactory> spCF;
	hr = DllGetClassObject(rclsid, IID_IClassFactory, (LPVOID*) &spCF);
	if (hr != S_OK)
	{
		return hr;
	}

	// Create instance
	hr = spCF->CreateInstance(NULL, riid, ppv);	
	return hr;
}
STDMETHODIMP CXSH::createActiveX2(BSTR path, BSTR clsid, BSTR iid,  IDispatch**  ppax)
{
	TSAUTO();
	CLSID clsID, clsIID;
	CLSIDFromString(clsid, &clsID);
	CLSIDFromString(iid, &clsIID);
	CComPtr<IDispatch> spDisp ;
	if(wcslen( path )> 0)
	{
		CreateInstanceFromDll(OLE2T(path), clsID, clsIID, (LPVOID*)&spDisp);	
	}
	else
	{
		CreateInstance( clsID, clsIID, (LPVOID*)&spDisp);	
	}
	if(spDisp)
	{
		*ppax = spDisp.Detach();
	}

	return S_OK;
}



STDMETHODIMP CXSH::get___interrupted(LONG* pVal)
{
	TSAUTO();
	// TODO: �ڴ����ʵ�ִ���
	TCHAR szText[512] = {0};
	GetWindowText(szText, 512);
	LONG l = _wtol(szText);
	*pVal = l;
	return S_OK;
}

STDMETHODIMP CXSH::put___interrupted(LONG newVal)
{
	TSAUTO();
	if(newVal >=0 && newVal <=1) 
	{
		TCHAR szText[512] = {0};
		_ltow(newVal, szText, 10);
		SetWindowText(szText);
	}
	return S_OK;
}

STDMETHODIMP CXSH::get_external(IDispatch** pVal)
{
	// TODO: �ڴ����ʵ�ִ���
	if(m_spExternal)
	{

	}
	else
	{
		this->createActiveX2(L"", L"{F17A8924-5B7A-42D6-9AB2-93635ED787C0}", L"{C0725E25-EA78-45F0-BA2E-6E07CF4DE417}" , &m_spExternal);
	}
	if(m_spExternal)
	{
		static bool b = false;
		if(!b)
		{
			b = true;
			CComPtr<IDispatch> spDisp;
			createActiveX2(L"",L"{57E8D62C-95B9-4324-8CF6-C3AA0F9CDF4B}",L"{9DF97288-0CE2-441E-8A8C-B226384D3447}", &spDisp);
			if(spDisp)
			{
				CComVariant v( m_spExternal);
				spDisp.PutPropertyByName(L"object", &v);
				m_spExternal = spDisp;
			} 
		}
		m_spExternal.QueryInterface(pVal);
	}

	return S_OK;
}

STDMETHODIMP CXSH::get___processid(LONG* pVal)
{
	// TODO: �ڴ����ʵ�ִ���
	*pVal =	GetCurrentProcessId();

	return S_OK;
}

STDMETHODIMP CXSH::get___processhandle(LONG* pVal)
{
	// TODO: �ڴ����ʵ�ִ���
	HANDLE l = NULL;
	l = GetCurrentProcess();
	*pVal = (LONG)(LONG_PTR)l;
	return S_OK;
}

STDMETHODIMP CXSH::evalFile(BSTR path, VARIANT* pVarRet)
{
	// TODO: �ڴ����ʵ�ִ���
	TSAUTO();
	ifstream fs;
	fs.open( CStringA(path), ios::in|ios::binary );
	bool b = fs.fail();											ATLASSERT( !b );
	if(b){
		return MK_E_CANTOPENFILE;}
	fs.seekg(0, ios::end);
	unsigned long  n = (unsigned long)fs.tellg();
	fs.seekg(0, ios::beg);
	CStringA strScript;
	char * buf = strScript.GetBuffer( n+1 );
	fs.read( buf, n );
	buf[n] = '\0';
	fs.close();  

	if(isec(buf, n))
	{
		dc(buf, n);		
	}

	CComBSTR bstr(strScript);
	CComQIPtr<IActiveScriptParse> spParse = m_spActiveScript;
	HRESULT hr = spParse->ParseScriptText(bstr, L"XSH", 0,0,0,1,0,pVarRet,0);
	m_spActiveScript->SetScriptState(SCRIPTSTATE_CONNECTED);

	return 0;
}

STDMETHODIMP CXSH::get_windows(IDispatch** pVal)
{
	// TODO: �ڴ����ʵ�ִ��� 
	if(m_spWindows)
	{

	}
	else
	{
		CreateInstance( CLSID_XSHWindows, IID_IXSHWindows, (LPVOID*)&m_spWindows);	
	}
	if(m_spWindows)
	{
		static bool b = false;
		if(!b)
		{
			b = true;
			CComPtr<IDispatch> spDisp;
			createActiveX2(L"",L"{57E8D62C-95B9-4324-8CF6-C3AA0F9CDF4B}",L"{9DF97288-0CE2-441E-8A8C-B226384D3447}", &spDisp);
			if(spDisp)
			{
				CComVariant v( m_spWindows);
				spDisp.PutPropertyByName(L"object", &v);
				m_spWindows = spDisp;
			} 

		}
		m_spWindows.QueryInterface(pVal);
	}
	return S_OK;
}

STDMETHODIMP CXSH::get_screen(IDispatch** pVal)
{
	// TODO: �ڴ����ʵ�ִ���

	if(m_spScreen)
	{

	}
	else
	{
		CreateInstance( CLSID_screen, IID_Iscreen, (LPVOID*)&m_spScreen);	
	}
	if(m_spScreen)
	{
		static bool b = false;
		if(!b)
		{
			b = true;
			CComPtr<IDispatch> spDisp;
			createActiveX2(L"",L"{57E8D62C-95B9-4324-8CF6-C3AA0F9CDF4B}",L"{9DF97288-0CE2-441E-8A8C-B226384D3447}", &spDisp);
			if(spDisp)
			{
				CComVariant v( m_spScreen);
				spDisp.PutPropertyByName(L"object", &v);
				m_spScreen = spDisp;
			}
		}
		m_spScreen.QueryInterface(pVal);
	}
	return S_OK;
}

STDMETHODIMP CXSH::get___xpos(LONG* pVal)
{
	// TODO: �ڴ����ʵ�ִ���
	POINT thePos;
	::GetCursorPos(&thePos);
	*pVal = thePos.x;
	return S_OK;
}

STDMETHODIMP CXSH::get___ypos(LONG* pVal)
{
	// TODO: �ڴ����ʵ�ִ���
	POINT thePos;
	::GetCursorPos(&thePos);
	*pVal = thePos.y;
	return S_OK;
}

STDMETHODIMP CXSH::get_storage(IDispatch** pVal)
{
	// TODO: �ڴ����ʵ�ִ���
	if(m_spStorage) 
	{

	}
	else
	{
		CreateInstance( CLSID_storage_ud, IID_Istorage_ud, (LPVOID*)&m_spStorage);	
	}
	if(m_spStorage)
	{
		static bool b = false;
		if(!b)
		{
			b = true;
			CComPtr<IDispatch> spDisp;
			createActiveX2(L"",L"{57E8D62C-95B9-4324-8CF6-C3AA0F9CDF4B}",L"{9DF97288-0CE2-441E-8A8C-B226384D3447}", &spDisp);
			if(spDisp)
			{
				CComVariant v( m_spStorage);
				spDisp.PutPropertyByName(L"object", &v);
				m_spStorage = spDisp;
			} 
		}
		m_spStorage.QueryInterface(pVal);
	}
	return S_OK;
}

STDMETHODIMP CXSH::get___isadmin(VARIANT_BOOL* pVal)
{
	// TODO: �ڴ����ʵ�ִ���	
	BOOL b;
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup; 
	b = AllocateAndInitializeSid(
		&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&AdministratorsGroup); 
	if(b) 
	{
		if (!CheckTokenMembership( NULL, AdministratorsGroup, &b)) 
		{
			b = FALSE;
		} 
		FreeSid(AdministratorsGroup); 
	}


	*pVal = b ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}
