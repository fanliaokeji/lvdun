#include "stdafx.h"
#include "LuaEventContainer.h"




//========================================================================================
//				CLuaEventContainer
HRESULT CLuaEventContainer::AttachEvent(lua_State* pLuaState, int nIndex, DWORD& dwCookie)
{
	CLuaEvent* pEvent = new CLuaEvent();
	pEvent->BuildFromLuaStack(pLuaState, nIndex);
	CEventContainer<CLuaEvent>::AttachEvent(pEvent, dwCookie);
	return S_OK;
}

HRESULT CLuaEventContainer::DetachEvent(DWORD dwCookie)
{
	CLuaEvent* pEvent = NULL;
	CEventContainer<CLuaEvent>::DetachEvent(dwCookie, &pEvent);
	delete pEvent;
	return S_OK;
}




//========================================================================================
//				CLuaMultiEventContainer
HRESULT CLuaMultiEventContainer::AttachEvent(const std::wstring& strEventName, lua_State* pLuaState, int nIndex, DWORD& dwCookie)
{
	HRESULT hr = S_OK;
	CLuaEvent* pEvent = new CLuaEvent();
	hr = pEvent->BuildFromLuaStack(pLuaState, nIndex);
	if (hr != S_OK)
	{
		delete pEvent;
		ATLASSERT(FALSE);
		return S_FALSE;
	}

	CMultiEventContainer<CLuaEvent>::AttachEvent(strEventName, pEvent, dwCookie);
	return S_OK;
}

HRESULT CLuaMultiEventContainer::AttachEvent(lua_State* pLuaState, int nIndex, DWORD& dwCookie)
{
	std::wstring strEventName = LuaGetString(pLuaState, nIndex);
	
	HRESULT hr = S_OK;
	hr = AttachEvent(strEventName, pLuaState, nIndex + 1, dwCookie);
	return hr;
}


HRESULT CLuaMultiEventContainer::DetachEvent(const std::wstring& strEventName, DWORD dwCookie)
{
	CLuaEvent* pEvent = NULL;
	CMultiEventContainer<CLuaEvent>::DetachEvent(strEventName, dwCookie, &pEvent);
	delete pEvent;
	return S_OK;
}
