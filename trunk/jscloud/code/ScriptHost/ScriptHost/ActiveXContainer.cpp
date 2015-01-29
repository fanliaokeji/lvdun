// ActiveXContainer.cpp : CActiveXContainer µÄÊµÏÖ

#include "stdafx.h"
#include "ActiveXContainer.h"
#include "DispEventCenter.h"


// CActiveXContainer

CActiveXContainer::CActiveXContainer()
{
	//m_ObjectClsid = CLSID_NULL;
}

HRESULT CActiveXContainer::FinalConstruct()
{
	return S_OK;
}

void CActiveXContainer::FinalRelease() 
{
	if( !m_spObject )
		return;
	HRESULT hr = CDispEventCenter::GetInstance().detachEvent( m_spObject, L"*", VARIANT_EMPTY, VARIANT_EMPTY, NULL );
	m_spObject.Release();
}


STDMETHODIMP CActiveXContainer::GetTypeInfoCount( OUT UINT *pctinfo )
{
	return __super::GetTypeInfoCount( pctinfo );
}

STDMETHODIMP CActiveXContainer::GetTypeInfo( IN UINT iTInfo, IN LCID lcid, OUT ITypeInfo **ppTInfo )
{
	return __super::GetTypeInfo( iTInfo, lcid, ppTInfo );
}

STDMETHODIMP CActiveXContainer::GetIDsOfNames( IN REFIID riid, /* [size_is] */ IN LPOLESTR *rgszNames, IN UINT cNames, IN LCID lcid, /* [size_is] */ OUT DISPID *rgDispId )
{
	HRESULT hr = __super::GetIDsOfNames( riid, rgszNames, cNames, lcid, rgDispId );
	if( DISP_E_UNKNOWNNAME == hr && m_spObject )
		hr = m_spObject->GetIDsOfNames( riid, rgszNames, cNames, lcid, rgDispId );
	return hr;
}

STDMETHODIMP CActiveXContainer::Invoke( IN DISPID dispIdMember, IN REFIID riid, IN LCID lcid, IN WORD wFlags, OUT IN DISPPARAMS *pDispParams, OUT VARIANT *pVarResult, OUT EXCEPINFO *pExcepInfo, OUT UINT *puArgErr )
{
	HRESULT hr =  __super::Invoke( dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr );
	if( DISP_E_MEMBERNOTFOUND == hr && m_spObject )
		hr = m_spObject->Invoke( dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr );
	return hr;
}

STDMETHODIMP CActiveXContainer::createObject(BSTR clsid, BSTR path)
{

	if(NULL == clsid || 512 < wcslen(clsid))
		return S_FALSE;
	if(NULL == path || 512 < wcslen(path))
		return S_FALSE;
	if( m_spObject )
		m_spObject.Release();
	HRESULT hr;
	CLSID	m_ObjectClsid;
	hr = CLSIDFromString( clsid, &m_ObjectClsid );
	if( FAILED(hr) )
		hr = CLSIDFromProgID( clsid, &m_ObjectClsid );
	if( FAILED(hr) )
		return hr;
	hr = m_spObject.CoCreateInstance( m_ObjectClsid );
	if( FAILED(hr) )
		return hr;
	return S_OK;
}

STDMETHODIMP CActiveXContainer::get_object(IDispatch** pVal)
{
	return m_spObject.CopyTo( pVal );
}

STDMETHODIMP CActiveXContainer::put_object(IDispatch* newVal)
{
	if( m_spObject )
		m_spObject.Release();
	m_spObject = newVal;
	return S_OK;
}

STDMETHODIMP CActiveXContainer::attachEvent(BSTR event, VARIANT notify, VARIANT listener, VARIANT_BOOL* pRetVal)
{
	if(NULL == event || 512 < wcslen(event))
		return S_FALSE;
	if( pRetVal )
		*pRetVal = VARIANT_FALSE;
	if( !m_spObject )
		return S_FALSE;
	return CDispEventCenter::GetInstance().attachEvent( m_spObject, event, notify, listener, pRetVal );
}

STDMETHODIMP CActiveXContainer::detachEvent(BSTR event, VARIANT notify, VARIANT listener, VARIANT_BOOL* pRetVal)
{
	if(NULL == event || 512 < wcslen(event))
		return S_FALSE;
	if(VT_BSTR == notify.vt && 512 < wcslen(notify.bstrVal))
		return S_FALSE;
	if( pRetVal )
		*pRetVal = VARIANT_FALSE;
	if( !m_spObject )
		return S_FALSE;
	return CDispEventCenter::GetInstance().detachEvent( m_spObject, event, notify, listener, pRetVal );
}
