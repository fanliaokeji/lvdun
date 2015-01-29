#include "StdAfx.h"
#include ".\DispEventCenter.h"

// CDispEventTransfer

CDispEventTransfer::CDispEventTransfer()
	: m_idMember( DISPID_UNKNOWN )
{
}

CDispEventTransfer::~CDispEventTransfer()
{
	if( m_spSource )
		HRESULT hr = detachEvent( NULL );
}

STDMETHODIMP CDispEventTransfer::Invoke( IN DISPID dispIdMember, IN REFIID riid, IN LCID lcid, IN WORD wFlags, OUT IN DISPPARAMS *pDispParams, OUT VARIANT *pVarResult, OUT EXCEPINFO *pExcepInfo, OUT UINT *puArgErr )
{
	ATLASSERT( m_idMember != DISPID_UNKNOWN );
	if( dispIdMember != m_idMember )
		return S_FALSE;
	if( VT_DISPATCH == m_vNotify.vt )
	{
		if( VT_DISPATCH != m_vListener.vt )
			return m_vNotify.pdispVal->Invoke( DISPID_VALUE, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr );
		DISPPARAMS params;
		params.cNamedArgs = pDispParams->cNamedArgs + 1;
		params.rgdispidNamedArgs = new DISPID[params.cNamedArgs];
		params.rgdispidNamedArgs[0] = DISPID_THIS;
		memcpy( &params.rgdispidNamedArgs[1], pDispParams->rgdispidNamedArgs, pDispParams->cNamedArgs * sizeof(DISPID) );
		params.cArgs = pDispParams->cArgs + 1;
		params.rgvarg = new VARIANTARG[params.cArgs];
		params.rgvarg[0] = m_vListener;
		memcpy( &params.rgvarg[1], pDispParams->rgvarg, pDispParams->cArgs * sizeof(VARIANTARG) );
		HRESULT hr = m_vNotify.pdispVal->Invoke( DISPID_VALUE, riid, lcid, wFlags, &params, pVarResult, pExcepInfo, puArgErr );
		delete[] params.rgdispidNamedArgs;
		delete[] params.rgvarg;
		return hr;
	}
	return S_FALSE;
}

STDMETHODIMP CDispEventTransfer::attachEvent( IDispatch* source, BSTR event, VARIANT notify, VARIANT listener, VARIANT_BOOL* pRetVal )
{
	if( pRetVal )
		*pRetVal = VARIANT_FALSE;
	if( NULL == source  || NULL == event || (VT_EMPTY==notify.vt||VT_NULL==notify.vt||VT_ERROR == notify.vt) && (VT_EMPTY==listener.vt||VT_NULL==listener.vt||VT_ERROR==listener.vt) )
		return S_FALSE;

	UINT n = 0;
	HRESULT hr = source->GetTypeInfoCount( &n );
	if( FAILED(hr) || n < 1 )
		return S_FALSE;
	CComPtr<ITypeInfo> spTI;
	hr = source->GetTypeInfo( 0, LOCALE_USER_DEFAULT, &spTI );
	if( FAILED(hr) || !spTI )
		return S_FALSE;
	CComPtr<ITypeLib> spTL;
	UINT index = 0;
	hr = spTI->GetContainingTypeLib( &spTL, &index );
	if( FAILED(hr) || !spTL )
		return S_FALSE;
	hr = AtlGetObjectSourceInterface( source, &m_libid, &m_iid, &m_wMajorVerNum, &m_wMinorVerNum );
	if( FAILED(hr) || IID_NULL == m_iid )
		return S_FALSE;
	CComPtr<ITypeInfo> spTI2;
	hr = spTL->GetTypeInfoOfGuid( m_iid, &spTI2 );
	if( FAILED(hr) || !spTI2 )
		return S_FALSE;
	MEMBERID idMember = DISPID_UNKNOWN;
	hr = spTI2->GetIDsOfNames( &event, 1, &idMember );
	if( FAILED(hr) || DISPID_UNKNOWN == idMember )
		return S_FALSE;
	hr = Advise( source );
	if( FAILED(hr) )
		return S_FALSE;

	m_spSource = source;
	m_bstrEvent = event;
	m_idMember = idMember;
	m_vNotify = notify;
	m_vListener = listener;
	if( pRetVal )
		*pRetVal = VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP CDispEventTransfer::detachEvent( VARIANT_BOOL* pRetVal )
{
	if( pRetVal )
		*pRetVal = VARIANT_FALSE;
	if( !m_spSource || !m_bstrEvent || DISPID_UNKNOWN == m_idMember )
		return S_FALSE;
	
	HRESULT hr = Unadvise( m_spSource );
	if( FAILED(hr) )
		return S_FALSE;

	m_spSource.Release();
	m_bstrEvent.Empty();
	m_idMember = DISPID_UNKNOWN;
	hr = m_vNotify.Clear();				ATLASSERT( SUCCEEDED(hr) );
	hr = m_vListener.Clear();			ATLASSERT( SUCCEEDED(hr) );
	if( pRetVal )
		*pRetVal = VARIANT_TRUE;
	return S_OK;
}


// CDispEventCenter

CDispEventCenter::CDispEventCenter(void)
{
}

CDispEventCenter::~CDispEventCenter(void)
{
}

CDispEventCenter& CDispEventCenter::GetInstance()
{
	static CDispEventCenter s_DispEventCenter;
	return s_DispEventCenter;
}

//void CDispEventCenter::Clear()
//{
//	for( map<CComPtr<IDispatch>, map<CComBSTR, map<pair<CComVariant, CComVariant>, CDispEventTransfer*> > >::iterator iter1=m_mapTransfer.begin(); iter1!=m_mapTransfer.end(); iter1++ )
//	{
//		for( map<CComBSTR, map<pair<CComVariant, CComVariant>, CDispEventTransfer*> >::iterator iter2=iter1->second.begin(); iter2!=iter1->second.end(); iter2++ )
//		{
//			for( map<pair<CComVariant, CComVariant>, CDispEventTransfer*>::iterator iter3=iter2->second.begin(); iter3!=iter2->second.end(); iter3++ )
//			{
//				VARIANT_BOOL vbRetVal;
//				HRESULT hr = iter3->second->detachEvent( &vbRetVal );
//				delete iter3->second;
//				iter3->second = NULL;
//			}
//		}
//	}
//	m_mapTransfer.clear();
//}

STDMETHODIMP CDispEventCenter::attachEvent( IDispatch* source, BSTR event, VARIANT notify, VARIANT listener, VARIANT_BOOL* pRetVal )
{
	if( pRetVal )
		*pRetVal = VARIANT_FALSE;
	if( NULL == source  || NULL == event || (VT_EMPTY==notify.vt||VT_NULL==notify.vt||VT_ERROR == notify.vt) && (VT_EMPTY==listener.vt||VT_NULL==listener.vt||VT_ERROR==listener.vt) )
		return S_FALSE;

	map<CComPtr<IDispatch>, map<CComBSTR, map<pair<CComVariant, CComVariant>, CDispEventTransfer*> > >::iterator iter1;
	map<CComBSTR, map<pair<CComVariant, CComVariant>, CDispEventTransfer*> >::iterator iter2;
	map<pair<CComVariant, CComVariant>, CDispEventTransfer*>::iterator iter3;
	pair<CComVariant, CComVariant> notify_listener = make_pair( CComVariant(notify), CComVariant(listener) );
	if( (iter1=m_mapTransfer.find(source)) != m_mapTransfer.end() && (iter2=iter1->second.find(event)) != iter1->second.end() && (iter3=iter2->second.find(notify_listener)) != iter2->second.end() )
	{
		if( pRetVal )
			*pRetVal = VARIANT_TRUE;
		return S_OK;
	}

	CDispEventTransfer* pTransfer = new CDispEventTransfer;
	if( NULL == pTransfer )
		return S_FALSE;
	HRESULT hr = pTransfer->attachEvent( source, event, notify, listener, pRetVal );
	if( FAILED(hr) )
	{
		delete pTransfer;
		return hr;
	}
	
	m_mapTransfer[source][event][notify_listener] = pTransfer;
	if( pRetVal )
		*pRetVal = VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP CDispEventCenter::detachEvent( IDispatch* source, BSTR event, VARIANT notify, VARIANT listener, VARIANT_BOOL* pRetVal )
{
	bool bRet = false;
	if( pRetVal )
		*pRetVal = VARIANT_FALSE;
	if( NULL == source && wcscmp(event, L"*") != 0)
		return S_FALSE;

	map<CComPtr<IDispatch>, map<CComBSTR, map<pair<CComVariant, CComVariant>, CDispEventTransfer*> > >::iterator iter1;
	bool bOne1 = (source!=NULL);
	if( bOne1 )
		iter1 = m_mapTransfer.find( source );
	else
		iter1 = m_mapTransfer.begin();
	for( ; iter1!=m_mapTransfer.end(); ++iter1 )
	{
		map<CComBSTR, map<pair<CComVariant, CComVariant>, CDispEventTransfer*> >::iterator iter2;
		bool bOne2 = wcscmp(event, L"*") != 0;
		if( bOne2 )
			iter2 = iter1->second.find( event );
		else
			iter2 = iter1->second.begin();
		for( ; iter2!=iter1->second.end(); ++iter2 )
		{
			map<pair<CComVariant, CComVariant>, CDispEventTransfer*>::iterator iter3;
			bool bOne3 = !( (VT_EMPTY==notify.vt||VT_NULL==notify.vt||VT_ERROR == notify.vt) && (VT_EMPTY==listener.vt||VT_NULL==listener.vt||VT_ERROR==listener.vt) );
			if( bOne3 )
				iter3 = iter2->second.find( make_pair( CComVariant(notify), CComVariant(listener) ) );
			else
				iter3 = iter2->second.begin();
			for( ; iter3!=iter2->second.end(); ++iter3 )
			{
				VARIANT_BOOL vbRetVal;
				HRESULT hr = iter3->second->detachEvent( &vbRetVal );
				delete iter3->second;
				iter3->second = NULL;
				bRet = true;
				if( bOne3 )
				{
					iter2->second.erase( iter3 );
					break;
				}
			}
			if( bOne2 )
			{
				iter1->second.erase( iter2 );
				break;
			}
		}
		if( bOne1 )
		{
			m_mapTransfer.erase( iter1 );
			break;
		}
	}
	if( !bOne1 )
		m_mapTransfer.clear();

	if( !bRet )
		return S_FALSE;
	if( pRetVal )
		*pRetVal = VARIANT_TRUE;
	return S_OK;
}
