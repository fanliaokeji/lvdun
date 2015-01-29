// XScriptHost.cpp : CXScriptHost 的实现

#include "stdafx.h"
#include "XScriptHost.h"
#include <atlstr.h>
#include "ScriptHost_i.h"
#include "DispEventCenter.h"
#include <fstream>
#include "aes.h"
#include "LaunchHelper.h"
int __EXITCODE = 0;
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv);
// CXScriptHost

CXScriptHost::CXScriptHost()
:mpDebugMgr(0)
,mpDebugApp(0)
,mpDebugDoc(0)
,m_AppCookie(0)
{
}

HRESULT CXScriptHost::FinalConstruct()
{
	TSAUTO();
	CreateScriptDebugger();

	HRESULT hr = E_FAIL;
	CComPtr<IClassFactory> spCF;
	DllGetClassObject(CLSID_XSH, IID_IClassFactory,  (LPVOID*)&spCF);


	CComPtr<IXSH> spXSH;
	hr = spCF->CreateInstance(NULL, IID_IXSH, (void **)&spXSH);
	if(SUCCEEDED(hr) && spXSH)
	{
		m_spXSH = spXSH;

	}
	else
	{

		hr = m_spXSH.CoCreateInstance( L"SHhost.XSH" );	

	}

	ATLASSERT( SUCCEEDED(hr) );
	
	return S_OK;
}

void CXScriptHost::FinalRelease() 
{
	TSAUTO();
	AddRef();
	OnUnload();
	ReleaseScriptDebugger();
	HRESULT hr = CDispEventCenter::GetInstance().detachEvent( NULL, L"*", VARIANT_EMPTY, VARIANT_EMPTY, NULL );
	for( map<CComBSTR, CComPtr<IActiveScript> >::iterator iter=m_mapEngine.begin(); iter!=m_mapEngine.end(); iter++ )
	{
	 	ULONG nRef = AddRef();		// 恢复因解决循环引用而减少的引用计数
		HRESULT hr = iter->second->Close();
	}
	m_mapParse.clear();
	m_mapEngine.clear();
	m_spXSH.Release();
}

STDMETHODIMP CXScriptHost::GetClassID( 
	/* [out] */ CLSID *pClassID)
{
	*pClassID = CLSID_XScriptHost;
	return S_OK;
}

STDMETHODIMP CXScriptHost::IsDirty( void)
{
	return S_FALSE;
}

STDMETHODIMP CXScriptHost::Load( 
	/* [in] */ LPCOLESTR pszFileName,
	/* [in] */ DWORD dwMode)
{
	m_bstrFileName = pszFileName;
	CComVariant v;
	if(dwMode)
		Run(&v);
	return S_OK;
}

STDMETHODIMP CXScriptHost::Save( 
	/* [unique][in] */ LPCOLESTR pszFileName,
	/* [in] */ BOOL fRemember)
{
	return E_NOTIMPL;
}

STDMETHODIMP CXScriptHost::SaveCompleted( 
	/* [unique][in] */ LPCOLESTR pszFileName)
{
	return E_NOTIMPL;
}

STDMETHODIMP CXScriptHost::GetCurFile( 
	/* [out] */ LPOLESTR *ppszFileName)
{
	*ppszFileName = m_bstrFileName.Copy();
	return S_OK;
}


STDMETHODIMP CXScriptHost::GetLCID( 
		/* [out] */ LCID *plcid)
{
	return E_NOTIMPL;
}

STDMETHODIMP CXScriptHost::GetItemInfo( 
	/* [in] */ LPCOLESTR pstrName,
	/* [in] */ DWORD dwReturnMask,
	/* [out] */ IUnknown **ppiunkItem,
	/* [out] */ ITypeInfo **ppti)
{
	HRESULT hr = S_OK;
	if( dwReturnMask & SCRIPTINFO_ITYPEINFO )
	{
		ATLASSERT( ppti );
		if( NULL == ppti )
			return E_POINTER;
		if( 0 != wcscmp( L"XSH", pstrName) )
			return E_UNEXPECTED;
		WCHAR szFilePath[MAX_PATH];
		DWORD dwLen = ::GetModuleFileNameW(_AtlBaseModule.GetModuleInstance(), szFilePath, MAX_PATH);		ATLASSERT( dwLen > 0 && dwLen < MAX_PATH );
		CComPtr<ITypeLib> spTypeLib;
		hr = ::LoadTypeLib( szFilePath, &spTypeLib );					ATLASSERT( SUCCEEDED(hr) );
		hr = spTypeLib->GetTypeInfoOfGuid( IID_IXScriptHost, ppti );	ATLASSERT( SUCCEEDED(hr) );
	}
	if( dwReturnMask & SCRIPTINFO_IUNKNOWN )
	{
		ATLASSERT( ppiunkItem );
		if( NULL == ppiunkItem )
			return E_POINTER;
		if( 0 != wcscmp( L"XSH", pstrName) )
			return E_UNEXPECTED;
		if( !m_spXSH )
		{
			
		}
		ATLASSERT( m_spXSH );
		hr = m_spXSH->QueryInterface( IID_IUnknown, (void**)ppiunkItem );		ATLASSERT( SUCCEEDED(hr) );
		//*ppiunkItem = this->GetUnknown();
		//this->AddRef();
		//hr = this->QueryInterface( IID_IUnknown, (void**)ppiunkItem );		ATLASSERT( SUCCEEDED(hr) );
	}
	return hr;
}

STDMETHODIMP CXScriptHost::GetDocVersionString( 
		/* [out] */ BSTR *pbstrVersion)
{
	return E_NOTIMPL;
}

STDMETHODIMP CXScriptHost::OnScriptTerminate( 
	/* [in] */ const VARIANT *pvarResult,
	/* [in] */ const EXCEPINFO *pexcepinfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CXScriptHost::OnStateChange(
	/* [in] */ SCRIPTSTATE ssScriptState)
{
	return E_NOTIMPL;
}

STDMETHODIMP CXScriptHost::OnScriptError( 
		/* [in] */ IActiveScriptError *pscripterror)
{
	//TSAUTO();
	if(m_spXSH)
	{
		m_spXSH->put___interrupted(1) ;
	}

	HRESULT hr;
	EXCEPINFO ei;
	hr = pscripterror->GetExceptionInfo( &ei );
	
	ATLASSERT( SUCCEEDED(hr) );
	DWORD dwSourceContext;
	ULONG ulLineNumber;
	LONG lCharacterPosition;
	hr = pscripterror->GetSourcePosition( &dwSourceContext, &ulLineNumber, &lCharacterPosition );		ATLASSERT( SUCCEEDED(hr) );
	CComBSTR bstrLineText;
	hr = pscripterror->GetSourceLineText( &bstrLineText );

	CStringW msg;
	msg.Format( L"  行：%d\n字符：%d \n错误：%s \n代码：%X \n  源：%s", ulLineNumber, lCharacterPosition, ei.bstrDescription, ei.scode, ei.bstrSource );
	TSDEBUG4CXX(" line:"<<ulLineNumber<<" , char:"<<lCharacterPosition<<" , Description:"<<ei.bstrDescription<<" ,scode:"<<ei.scode<<", Source:"<<ei.bstrSource);

	TCHAR szFlag[32] = {0};
	GetEnvironmentVariable(L"debug", szFlag,32 );
	if(szFlag[0] == '\0')
	{
		//没有环境变量
		TCHAR szPath[_MAX_PATH] = {0};
		CComPtr<IDispatch>spExternal;
		m_spXSH->get_external(&spExternal);
			CComVariant v, ver;
		if(spExternal)
		{
			spExternal.Invoke0(L"GetPID", &v);
			spExternal.Invoke0(L"GetHostVersion", &ver);
		}
		CStringW url;
		std::wstring wstrTID = LaunchConfig::Instance()->m_wstrTID;
		url.Format(L"http://www.google-analytics.com/collect?v=1&tid=%s&cid=%s&t=event&ec=Exception&ea=LineNO.:%d_CharPos:%d_scode:0x%X&el=%s" ,
			wstrTID.c_str(),v.bstrVal,  ulLineNumber, lCharacterPosition,ei.scode , ver.bstrVal);
		URLDownloadToCacheFile (NULL, url, szPath, _MAX_PATH, 0, 0);
		//OutputDebugString("")
		m_spXSH->trace(CComBSTR(msg));
		exit(-9);
	}
	else
	{
		::MessageBoxW( NULL, msg, L" Script Host", MB_OK|MB_ICONERROR|MB_SYSTEMMODAL );

	} 
	return S_OK;
}

STDMETHODIMP CXScriptHost::OnEnterScript( )
{
	 
	return E_NOTIMPL;
}

STDMETHODIMP CXScriptHost::OnLeaveScript()
{
	return E_NOTIMPL;
}

STDMETHODIMP CXScriptHost::GetWindow( 
	/* [out] */ HWND *phwnd)
{
	return E_NOTIMPL;
}

STDMETHODIMP CXScriptHost::EnableModeless( 
	/* [in] */ BOOL fEnable)
{
	return E_NOTIMPL;
}

STDMETHODIMP CXScriptHost::Load( BSTR FileName, LONG Mode )
{
	return Load( (LPCOLESTR)FileName, (DWORD)Mode );
}

STDMETHODIMP CXScriptHost::Run(VARIANT* pRet)
{		 
	ifstream fs;
	fs.open( CStringA(m_bstrFileName.m_str), ios::in|ios::binary );
	bool b = fs.fail();											ATLASSERT( !b );
	if(b){
		return MK_E_CANTOPENFILE;}
	fs.seekg(0, ios::end);
	unsigned long n = (unsigned long)fs.tellg();
	fs.seekg(0, ios::beg);
	CStringA strScript;
	char * buf = strScript.GetBuffer( n+1 );
	fs.read( buf, n );
	buf[n] = '\0';
	fs.close();  
	HRESULT hr ;
	CComBSTR bstrLanguage( L"JScript" );
	if(!m_mapEngine[ bstrLanguage ])
	{
		CComPtr<IActiveScript> spEngine;
		hr = spEngine.CoCreateInstance( bstrLanguage );			ATLASSERT( SUCCEEDED(hr) );
		if( SUCCEEDED(hr) && spEngine )
		{
			hr = spEngine->SetScriptSite( this );														ATLASSERT( SUCCEEDED(hr) );		
			Release();	
			ULONG cRef = Release();					// 解决循环引用导致不能析构的问题
			hr = spEngine->AddNamedItem( L"XSH", SCRIPTITEM_ISVISIBLE|SCRIPTITEM_GLOBALMEMBERS );		ATLASSERT( SUCCEEDED(hr) );
			CComQIPtr<IActiveScriptParse> spParse = spEngine;											ATLASSERT( spParse );
			hr = spParse->InitNew();	
			ATLASSERT( SUCCEEDED(hr) );
			
			ATLASSERT( m_spXSH );
			if(m_spXSH)
				m_spXSH->SetActiveScript(spEngine);
			PreLoadScript(spEngine, spParse);	
			m_mapEngine[ bstrLanguage ] = spEngine;
			m_mapParse[ bstrLanguage ] = spParse;
		}  
	}

	if(!('/' == buf[0] && '/' == buf[1] )) // 注释符开始
	{
		//dc(buf, n);	
		unsigned char key[] = 
		{
			0x2b, 0x7e, 0x15, 0x16, 
			0x28, 0xae, 0xd2, 0xa6, 
			0xab, 0xf7, 0x15, 0x88, 
			0x09, 0xcf, 0x4f, 0x3c
		};
		AES aes(key);
		aes.InvCipher((unsigned char*)buf, n);

	}

	CreateDocumentForDebugger(m_bstrFileName.m_str, CComBSTR(strScript));
	hr = m_mapParse[ bstrLanguage ]->ParseScriptText( CComBSTR(strScript), L"XSH", NULL, NULL, 1, 1, 0, pRet, NULL );		ATLASSERT( SUCCEEDED(hr) );
	if(SUCCEEDED(hr))
	{
		m_mapEngine[ bstrLanguage ]->SetScriptState( SCRIPTSTATE_CONNECTED );												ATLASSERT( SUCCEEDED(hr) );
	}
	static bool bFirst = true;
	if(SUCCEEDED(hr) && bFirst)
	{
		bFirst = false;
		OnLoad();
	}
	return hr;;
}

void CXScriptHost::OnLoad()
{
	wchar_t szScript[MAX_PATH] = {0};
	swprintf_s(szScript,MAX_PATH,L"if(typeof onload == 'function') onload(\"%s\")",g_strcmdline.c_str());
	CComBSTR bstrScript = szScript;

	map<CComBSTR, CComPtr<IActiveScriptParse>  >::iterator iter =	m_mapParse.begin();
	for (; iter != m_mapParse.end(); iter++)
	{
		iter->second->ParseScriptText(bstrScript, 0, NULL, NULL, 0, 0, 0, NULL, NULL );
	}
	
	map<CComBSTR, CComPtr<IActiveScript> >::iterator iterEngine =	m_mapEngine.begin();
	for (; iterEngine != m_mapEngine.end(); iterEngine++)
	{
		iterEngine->second->SetScriptState( SCRIPTSTATE_CONNECTED );
	}

}

void CXScriptHost::OnUnload()
{
	TSAUTO();
	CStringW strScript;
	strScript.Format(L"if(typeof onunload == 'function') onunload(%d)", __EXITCODE);
	CComBSTR bstrScript = strScript;
	map<CComBSTR, CComPtr<IActiveScriptParse>  >::iterator iter =	m_mapParse.begin();
	for (; iter != m_mapParse.end(); iter++)
	{
		iter->second->ParseScriptText(bstrScript, 0, NULL, NULL, 0, 0, 0, NULL, NULL );
	}

	map<CComBSTR, CComPtr<IActiveScript> >::iterator iterEngine =	m_mapEngine.begin();
	for (; iterEngine != m_mapEngine.end(); iterEngine++)
	{
		iterEngine->second->SetScriptState( SCRIPTSTATE_CONNECTED );
	}

}

BOOL CXScriptHost::PreLoadScript(IActiveScript* spEngine, IActiveScriptParse* spParse)
{
	CComBSTR bstrScript=L"var __XScriptHost_ActiveXObject=ActiveXObject; ActiveXObject=function(progid){ var o = createActiveX2('','{F858FC8D-20F0-4D88-8757-ADDFDD482A5D}','{985DB584-D7C7-4F98-BB8E-436E12B28662}'); o.object= new  __XScriptHost_ActiveXObject(progid); if(typeof o.object != 'object' ) return undefined; else return o;}";
	HRESULT hr = S_OK;
	
	hr = spParse->ParseScriptText(bstrScript,L"XSH", NULL, NULL, 0, 0, 0, NULL, NULL );	ATLASSERT( SUCCEEDED(hr) );
	hr = spEngine->SetScriptState(SCRIPTSTATE_CONNECTED);								ATLASSERT( SUCCEEDED(hr) );
    return SUCCEEDED(hr);
}

BOOL CXScriptHost::MakePath(CComBSTR& bstrRelativePath, const BSTR bstrCurrentPath )
{
	wchar_t szFilePath[MAX_PATH]={0};
	wchar_t* pszFilePath = szFilePath;
	if(NULL == bstrCurrentPath)
	{
		GetModuleFileNameW(   _AtlBaseModule.GetModuleInstance(), szFilePath, MAX_PATH);
		pszFilePath = szFilePath;
	}
	else 
		wcsncpy(szFilePath, bstrCurrentPath,MAX_PATH);
	CAtlStringW strPath =  bstrRelativePath;
	strPath.Replace('/','\\');
	bstrRelativePath = strPath;
	PathRemoveFileSpecW(pszFilePath);
	PathCombineW(pszFilePath, pszFilePath, bstrRelativePath.m_str);
	bstrRelativePath = pszFilePath;
 	return TRUE;
}

STDMETHODIMP CXScriptHost::SetAddin(BSTR name, IDispatch* object)
{
	// TODO: 在此添加实现代码
	//ReleaseScriptDebugger();

	HRESULT hr = S_OK;
	if(NULL == name || 256 < wcslen(name))
		return S_FALSE;
	if( !m_spXSH )
	{
		
	}
	ATLASSERT( m_spXSH );
	m_spXSH->setAddin(name, object);
	return S_OK;
}
STDMETHODIMP CXScriptHost::GetDocumentContextFromPosition(DWORD dwSourceContext, ULONG uCharacterOffset, ULONG uNumChars, IDebugDocumentContext __RPC_FAR *__RPC_FAR *ppsc)
{
	if(!ppsc)
		return E_POINTER;
	*ppsc = 0;
	if(!mpDebugDoc)
		return E_UNEXPECTED;
	ULONG ulStartPos = 0;
	HRESULT hr = mpDebugDoc->GetScriptBlockInfo(m_AppCookie, 0, &ulStartPos, 0);
	if(SUCCEEDED(hr))
	{
		return mpDebugDoc->CreateDebugDocumentContext(ulStartPos+uCharacterOffset, uNumChars, ppsc);
	}
	return hr;
}
STDMETHODIMP CXScriptHost::GetApplication(IDebugApplication __RPC_FAR *__RPC_FAR *ppda)
{
	if(!ppda)
		return E_POINTER;
	*ppda = 0;
	if(!mpDebugApp)
		return E_UNEXPECTED;
	*ppda = mpDebugApp;
	mpDebugApp->AddRef();
	return S_OK;
}
STDMETHODIMP CXScriptHost::GetRootApplicationNode(IDebugApplicationNode __RPC_FAR *__RPC_FAR *ppdanRoot)
{
	if(!ppdanRoot)
		return E_POINTER;
	*ppdanRoot = 0;
	return S_OK;
}
STDMETHODIMP CXScriptHost::OnScriptErrorDebug(IActiveScriptErrorDebug __RPC_FAR *pErrorDebug, BOOL __RPC_FAR *pfEnterDebugger, BOOL __RPC_FAR *pfCallOnScriptErrorWhenContinuing)
{
#ifndef __SCRIPT_DEBUG
	*pfEnterDebugger = FALSE;
	*pfCallOnScriptErrorWhenContinuing = TRUE;
	return S_OK;
#endif

	HRESULT hr;
	EXCEPINFO ei;	
	hr = pErrorDebug->GetExceptionInfo( &ei );				ATLASSERT( SUCCEEDED(hr) );
	DWORD dwSourceContext;
	ULONG ulLineNumber;
	LONG lCharacterPosition;
	hr = pErrorDebug->GetSourcePosition( &dwSourceContext, &ulLineNumber, &lCharacterPosition );		ATLASSERT( SUCCEEDED(hr) );
	CComBSTR bstrLineText;
	hr = pErrorDebug->GetSourceLineText( &bstrLineText );
	CStringW msg;
	msg.Format( L"  行：%d\n字符：%d \n错误：%s \n代码：%X \n  源：%s\n%s", ulLineNumber, lCharacterPosition, ei.bstrDescription, ei.scode, ei.bstrSource ,L"是否进入调试？");
	if (IDYES == ::MessageBoxW( NULL, msg  , L"  Script Host", MB_YESNO|MB_ICONERROR ))
	{
        *pfEnterDebugger = TRUE;
		*pfCallOnScriptErrorWhenContinuing = FALSE;
	}
	else  
	{
		*pfEnterDebugger = FALSE;
		*pfCallOnScriptErrorWhenContinuing = FALSE;
	}
	//MessageBox(NULL, _T(""))	
	return S_OK;
}
 
struct DECLSPEC_UUID("51973C2f-CB0C-11d0-B5C9-00A0244A0E7A")
IProcessDebugManager;
HRESULT CXScriptHost::CreateScriptDebugger()
{
#ifndef __SCRIPT_DEBUG
	return S_FALSE;
#endif
    HRESULT hr = CoCreateInstance(CLSID_ProcessDebugManager, NULL, CLSCTX_INPROC_SERVER, __uuidof( IProcessDebugManager), (void**)&mpDebugMgr);
	if(SUCCEEDED(hr))
	{
		hr = mpDebugMgr->CreateApplication(&mpDebugApp);
	}
	if(SUCCEEDED(hr))
	{
		hr = mpDebugApp->SetName(L"SHostDebug");
	}
	if(SUCCEEDED(hr))
	{
		hr = mpDebugMgr->AddApplication(mpDebugApp, &m_AppCookie);
		

		
	}
	if(FAILED(hr))
	{
		ReleaseScriptDebugger();
	}
	return hr;
}
void CXScriptHost::ReleaseScriptDebugger()
{
#ifndef __SCRIPT_DEBUG
	return ;
#endif
	ReleaseDebugDocument();
	if(mpDebugMgr && m_AppCookie)
	{
		mpDebugMgr->RemoveApplication(m_AppCookie);
		m_AppCookie = 0;
	}
	if(mpDebugApp)
	{
		mpDebugApp->Release();
		mpDebugApp = NULL;
	}
	if(mpDebugMgr)
	{
		mpDebugMgr->Release();
		mpDebugMgr = NULL;
	}
}
HRESULT CXScriptHost::CreateDocumentForDebugger(BSTR filePath,BSTR scripts)
{
	if(!mpDebugMgr)
		return E_UNEXPECTED;
	ReleaseDebugDocument();
	HRESULT hr = mpDebugMgr->CreateDebugDocumentHelper(0, &mpDebugDoc);
	if(SUCCEEDED(hr))
	{
		//PathAddBackslashA()
		 wchar_t drive[_MAX_PATH];
		 wchar_t dir[_MAX_PATH];
		 wchar_t fname[_MAX_PATH];
		 wchar_t ext [_MAX_PATH];
		 ATLASSERT(PathFileExistsW(filePath));
		 _wsplitpath(filePath, drive,dir,fname,ext);
		hr = mpDebugDoc->Init(mpDebugApp,  fname, filePath, TEXT_DOC_ATTR_READONLY);
	}
	if(SUCCEEDED(hr))
	{
		hr = mpDebugDoc->Attach(0);
	}
	if(SUCCEEDED(hr))
	{
		hr = mpDebugDoc->AddUnicodeText(scripts);
	}
	if(SUCCEEDED(hr))
	{
        DWORD pdwSourceContext;
		hr = mpDebugDoc->DefineScriptBlock(0, (ULONG)(wcslen(scripts)), m_mapEngine[L"JScript"].p, FALSE, &pdwSourceContext);
	}
	return hr;

}
void CXScriptHost::ReleaseDebugDocument()
{
	if(mpDebugDoc)
	{
		mpDebugDoc->Detach();
		mpDebugDoc->Release();
		mpDebugDoc = 0;
	}

}
