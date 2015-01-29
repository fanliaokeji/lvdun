#pragma once

template<class T>
class CProxy_IXSHWindowsEvents :
	public IConnectionPointImpl<T, &__uuidof(_IXSHWindowsEvents)>
{
public:
	HRESULT Fire_OnNotify( BSTR  msg,  VARIANT  p1,  VARIANT  p2)
	{
		HRESULT hr = S_OK;
		T * pThis = static_cast<T *>(this);
		int cConnections = m_vec.GetSize();

		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			pThis->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			pThis->Unlock();

			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);

			if (pConnection)
			{
				CComVariant avarParams[3];
				avarParams[2] = msg;
				avarParams[1] = p1;
				avarParams[0] = p2;
				CComVariant varResult;

				DISPPARAMS params = { avarParams, NULL, 3, 0 };
				hr = pConnection->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
		return hr;
	}
};
