#pragma once

#import <msxml.dll>
class CAJAX;
interface 
__declspec(uuid("3B947936-B7D9-4C13-9F6D-9BBC93B2D5E1"))
IAJAXStateChange : public IUnknown
{
	STDMETHOD(OnStateChange)(CAJAX* pSource, const bool bSucceeded, const int nHttpState, const BSTR bstrResponse,   IDispatch* pDispth)=0;
};
class CAJAX : IDispatch
{
private:
	MSXML::IXMLHttpRequestPtr  m_spXmlRequest;
	CComPtr<IAJAXStateChange> m_spAJAXStateChange;

public:
	LPARAM m_lParam;
	bool m_bOutDOM;
	LONG m_nID;
	CComBSTR m_bstrUrl;
	CComBSTR m_bstrSavedPath;
	CComBSTR m_bstrResponseHeader;
	bool bException;
	CAJAX(): m_nID(0), bException(false)
	{

	}

	bool Open(LPOLESTR lpolestrUrl, IAJAXStateChange* pStateChange,  bool bOutDOM = true, LONG nID = 0, LPOLESTR lpSavedPath = L"")
	{
		TSAUTO();
		bException = false;
		Abort();
		if(m_spXmlRequest)
		{
			m_spXmlRequest->abort();
		}
		if(m_spAJAXStateChange)
		{
			m_spAJAXStateChange.Release();
		}
		m_spAJAXStateChange = pStateChange;
		m_bOutDOM = bOutDOM;
		HRESULT hr = m_spXmlRequest.CreateInstance(__uuidof(MSXML::XMLHTTPRequest));
		if(FAILED(hr) || !m_spXmlRequest.operator->())
			return false;
		_variant_t   varp(true);   
		hr = m_spXmlRequest->raw_open(_bstr_t("GET"),_bstr_t(lpolestrUrl),varp);
		//hr = m_spXmlRequest->open(_bstr_t("GET"),_bstr_t(lpolestrUrl),varp);
		if(FAILED(hr))
		{
			m_spXmlRequest.Release();
			return false;
		}
		m_bstrUrl = lpolestrUrl;
		m_bstrSavedPath = lpSavedPath;
		m_nID = nID;
#ifdef _DEBUG
		m_spXmlRequest->onreadystatechange = this;
#else
		m_spXmlRequest->put_onreadystatechange(this);
#endif
		try
		{
			m_spXmlRequest->raw_send(); //不知道为什么崩在这里特别多
		}
		catch (...)
		{
			bException = true;			
		}
		return true;
	}

	bool Open(LPOLESTR lpolestrType,LPOLESTR lpolestrUrl, std::map <std::wstring,std::wstring> mapHeader, LPOLESTR  lpolestrBody,IAJAXStateChange* pStateChange,  bool bOutDOM = true, LONG nID = 0, LPOLESTR lpSavedPath = L"")
	{
		TSAUTO();
		bException = false;
		Abort();
		if(m_spXmlRequest)
		{
			m_spXmlRequest->abort();
		}
		if(m_spAJAXStateChange)
		{
			m_spAJAXStateChange.Release();
		}
		m_spAJAXStateChange = pStateChange;
		m_bOutDOM = bOutDOM;
		HRESULT hr = m_spXmlRequest.CreateInstance(__uuidof(MSXML::XMLHTTPRequest));
		if(FAILED(hr) || !m_spXmlRequest.operator->())
			return false;
		_variant_t   varp(true);   
		hr = m_spXmlRequest->raw_open(_bstr_t(lpolestrType),_bstr_t(lpolestrUrl),varp);
		TSDEBUG4CXX(L"raw_open hr = " << hr);
		if(FAILED(hr))
		{
			m_spXmlRequest.Release();
			return false;
		}
		
		std::map<std::wstring,std::wstring>::const_iterator m_iter = mapHeader.begin();
		while (m_iter != mapHeader.end())
		{
			hr = m_spXmlRequest->raw_setRequestHeader(_bstr_t((m_iter->first).c_str()),_bstr_t((m_iter->second).c_str()));
			TSDEBUG4CXX(L"raw_setRequestHeader hr = " << hr<<", header = "<<(m_iter->first).c_str()<<", value = "<<(m_iter->second).c_str());
			if(FAILED(hr))
			{
				m_spXmlRequest.Release();
				return false;
			}
			++m_iter;
		}
		m_bstrUrl = lpolestrUrl;
		m_bstrSavedPath = lpSavedPath;
		m_nID = nID;
#ifdef _DEBUG
		m_spXmlRequest->onreadystatechange = this;
#else
		m_spXmlRequest->put_onreadystatechange(this);
#endif
		_variant_t vBody(lpolestrBody);
		try
		{
			TSDEBUG4CXX(L"vBody = " << lpolestrBody);
			m_spXmlRequest->raw_send(vBody); //不知道为什么崩在这里特别多
		}
		catch (...)
		{
			bException = true;	
		}
		return true;
	}

	MSXML::IXMLHttpRequestPtr Open(LPOLESTR bstrType, LPOLESTR bstrUrl, IAJAXStateChange* pStateChange,bool bOutDOM = false)
	{//截图浏览调用此方法
		bException = false;
		this->Abort();
		m_bOutDOM = bOutDOM;
		HRESULT hr = m_spXmlRequest.CreateInstance(__uuidof(MSXML::XMLHTTPRequest));
		if(FAILED(hr))
			return false;
		if(FAILED(hr))
		{
			m_spXmlRequest.Release();
			return NULL;
		}

		m_spAJAXStateChange = pStateChange;
		m_bstrUrl = bstrUrl;
		_variant_t   varp(true);   		
		hr = m_spXmlRequest->raw_open(bstrType,bstrUrl,varp);
		if(SUCCEEDED(hr))
		{
			m_spXmlRequest->put_onreadystatechange(this) ;
			return m_spXmlRequest;
		}
		//hr = m_spXmlRequest-> open(bstrType,bstrUrl,varp);
		return NULL;
	}

	CAJAX(LPOLESTR lpolestrUrl, IAJAXStateChange* pStateChange,  bool bOutDOM = false, LONG nID = 0)
	{
		Abort();
		Open(lpolestrUrl, pStateChange, bOutDOM, nID);
	}
	void Abort()
	{
		if(m_spXmlRequest)
		{
			m_spXmlRequest->abort();
			if(m_spXmlRequest)
				m_spXmlRequest.Release();
			m_spAJAXStateChange.Release();
		}
	}
	virtual ~CAJAX()
	{
		Abort();
	}
	STDMETHOD(QueryInterface)(const struct _GUID &/*iid*/,void ** ppv){*ppv=this;return S_OK;}	
	ULONG __stdcall AddRef(void){	return 1;	}	
	ULONG __stdcall Release(void){	return 0;	}	
	STDMETHOD(GetTypeInfoCount)(unsigned int *){	return E_NOTIMPL;	}	
	STDMETHOD(GetTypeInfo)(unsigned int,unsigned long,struct ITypeInfo ** ){	return E_NOTIMPL;	}	
	STDMETHOD(GetIDsOfNames)(const IID &,LPOLESTR * ,UINT,LCID,DISPID *){	return E_NOTIMPL;	}	
	STDMETHOD(Invoke)(long dispID,const struct _GUID &,unsigned long,unsigned short,struct tagDISPPARAMS * /*pParams*/,struct tagVARIANT *,struct tagEXCEPINFO *,unsigned int *)
	{
		TSAUTO();
		long s_httpstatus = -1;
		if(0 == dispID)
		{
			if(m_spXmlRequest)
			{
				long lstate = m_spXmlRequest->readyState ; 
				TSDEBUG4CXX(" readyState : "<<lstate);
				if(bException)
				{
					m_spAJAXStateChange->OnStateChange(this, false, 0, NULL, NULL );
				}
				/*else if(0 > lstate)
				{
					m_spAJAXStateChange->OnStateChange(this, false, 0, NULL, NULL );
					//Abort();
				}*/
				else if(4 == lstate)
				{
					s_httpstatus = m_spXmlRequest->status;
					_bstr_t statusText = m_spXmlRequest->statusText;
					_bstr_t responseHeaders = m_spXmlRequest->getAllResponseHeaders();
					BSTR bstrResponseHeader = NULL;
					if (SUCCEEDED(m_spXmlRequest->raw_getAllResponseHeaders(&bstrResponseHeader)))
					{
						m_bstrResponseHeader.Attach(bstrResponseHeader);
					}
					//_bstr_t responseText = m_spXmlRequest->responseText;太大会出错
					TSDEBUG4CXX(" status : "<<s_httpstatus<<" statusText : "<<statusText.GetBSTR()<<" Url : "<<m_bstrUrl<<", responseHeaders : "<<responseHeaders.GetBSTR());					
					if(!m_bOutDOM)
					{						
						m_spAJAXStateChange->OnStateChange(this,true, s_httpstatus, NULL, m_spXmlRequest.operator->() );
					}
					else
					{
						MSXML::IXMLDOMDocumentPtr spDOM = m_spXmlRequest->responseXML;
						MSXML::IXMLDOMParseErrorPtr spError = spDOM->GetparseError();
						long l = spError->GeterrorCode(); 
						l;
						TSDEBUG4CXX(" DOM : "<<spDOM.operator->()<<" error : "<<l);
						m_spAJAXStateChange->OnStateChange(this, true, s_httpstatus, NULL, spDOM.operator->() );					  	
					}
				}
			}
			return S_OK;
		}
		return S_OK;
	}

protected:
};	
