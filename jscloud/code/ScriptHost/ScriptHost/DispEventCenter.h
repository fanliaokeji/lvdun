#pragma once

#include <map>
using namespace std;


const VARIANT VARIANT_OPTIONAL = {VT_ERROR, 0, 0, 0, DISP_E_PARAMNOTFOUND};
const VARIANT VARIANT_EMPTY = {VT_EMPTY, 0, 0, 0, 0};


// CDispEventTransfer

class CDispEventTransfer : public IDispEventSimpleImpl<1, CDispEventTransfer, &IID_IDispatch>
{
public:
	CDispEventTransfer();
	~CDispEventTransfer();

BEGIN_SINK_MAP(CDispEventTransfer)
END_SINK_MAP()

public:
	STDMETHOD( Invoke )( IN DISPID dispIdMember, IN REFIID riid, IN LCID lcid, IN WORD wFlags, OUT IN DISPPARAMS *pDispParams, OUT VARIANT *pVarResult, OUT EXCEPINFO *pExcepInfo, OUT UINT *puArgErr );
public:
	STDMETHODIMP attachEvent( IDispatch* source, BSTR event, VARIANT notify, VARIANT listener, VARIANT_BOOL* pRetVal );
	STDMETHODIMP detachEvent( VARIANT_BOOL* pRetVal );
protected:
	CComPtr<IDispatch>	m_spSource;
	CComBSTR			m_bstrEvent;
	MEMBERID			m_idMember;
	CComVariant			m_vNotify;
	CComVariant			m_vListener;
};


// CDispEventCenter

class CDispEventCenter
{
private:
	CDispEventCenter(void);
	~CDispEventCenter(void);
public:
	static CDispEventCenter& GetInstance();
public:
	STDMETHODIMP attachEvent( IDispatch* source, BSTR event, VARIANT notify, VARIANT listener, VARIANT_BOOL* pRetVal );
	STDMETHODIMP detachEvent( IDispatch* source, BSTR event, VARIANT notify, VARIANT listener, VARIANT_BOOL* pRetVal );
	//void Clear();
protected:
	map<CComPtr<IDispatch>, map<CComBSTR, map<pair<CComVariant, CComVariant>, CDispEventTransfer*> > >	m_mapTransfer;
};
