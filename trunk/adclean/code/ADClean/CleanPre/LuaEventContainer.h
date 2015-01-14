#pragma once

#include "./EventContainer.h"
#include "./LuaEvent.h"
#include "./LuaTool.h"

typedef IEventEnum<CLuaEvent> ILuaEventEnum;


class CLuaEventContainer:public CEventContainer<CLuaEvent>
{
public:
	//��lua��ջ��nIndexλ���Һ���, ��Ϊ�¼�����
	HRESULT AttachEvent(lua_State* pLuaState, int nIndex, DWORD& dwCookie);
	HRESULT DetachEvent(DWORD dwCookie);

private:
	//���ػ��ຯ��
	HRESULT AttachEvent(CLuaEvent* pEvent, DWORD& dwCookie);
	HRESULT DetachEvent(CLuaEvent* pEvent);	
	HRESULT DetachEvent(DWORD dwCookie, CLuaEvent** ppEvent);
};




class CLuaMultiEventContainer:public CMultiEventContainer<CLuaEvent>
{
public:
	//��lua��ջ��nIndexλ���Һ���, ��Ϊ�¼�����
	HRESULT AttachEvent(const std::wstring& strEventName, lua_State* pLuaState, int nIndex, DWORD& dwCookie);
	
	//��lua��ջ��nIndexλ�����¼���, nIndex+1λ���Һ���, ��Ϊ�¼�����
	HRESULT AttachEvent(lua_State* pLuaState, int nIndex, DWORD& dwCookie);

	HRESULT AttachEvent(lua_State* pLuaState, int nIndex);

	HRESULT DetachEvent(const std::wstring& strEventName, DWORD dwCookie);

private:
	//���ػ��ຯ��
	HRESULT AttachEvent(const std::wstring& strEventName, CLuaEvent* pEvent, DWORD& dwCookie);
	HRESULT DetachEvent(const std::wstring& strEventName, CLuaEvent* pEvent);	
	HRESULT DetachEvent(const std::wstring& strEventName, DWORD dwCookie, CLuaEvent** ppEvent);
};



