#pragma once

template <class T>
class CProxy_IActiveXContainerEvents : public IConnectionPointImpl<T, &__uuidof( _IActiveXContainerEvents ), CComDynamicUnkArray>
{
	//警告此类将由向导重新生成。
public:
	HRESULT Fire_onunload( VARIANT  param1,  VARIANT  param2)
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
				CComVariant avarParams[2];
				avarParams[1] = param1;
				avarParams[0] = param2;
				CComVariant varResult;

				DISPPARAMS params = { avarParams, NULL, 2, 0 };
				hr = pConnection->Invoke(0x80011791, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
		return hr;
	}
};
