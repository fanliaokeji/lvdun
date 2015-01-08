#pragma once

#include "./EventContainer.h"
#include "./LuaEvent.h"
#include "./LuaTool.h"

typedef IEventEnum<CLuaEvent> ILuaEventEnum;


class CLuaEventContainer:public CEventContainer<CLuaEvent>
{
public:
	//在lua堆栈的nIndex位置找函数, 作为事件关联
	HRESULT AttachEvent(lua_State* pLuaState, int nIndex, DWORD& dwCookie);
	HRESULT DetachEvent(DWORD dwCookie);

private:
	//隐藏基类函数
	HRESULT AttachEvent(CLuaEvent* pEvent, DWORD& dwCookie);
	HRESULT DetachEvent(CLuaEvent* pEvent);	
	HRESULT DetachEvent(DWORD dwCookie, CLuaEvent** ppEvent);
};




class CLuaMultiEventContainer:public CMultiEventContainer<CLuaEvent>
{
public:
	//在lua堆栈的nIndex位置找函数, 作为事件关联
	HRESULT AttachEvent(const std::wstring& strEventName, lua_State* pLuaState, int nIndex, DWORD& dwCookie);
	
	//在lua堆栈的nIndex位置找事件名, nIndex+1位置找函数, 作为事件关联
	HRESULT AttachEvent(lua_State* pLuaState, int nIndex, DWORD& dwCookie);

	HRESULT AttachEvent(lua_State* pLuaState, int nIndex);

	HRESULT DetachEvent(const std::wstring& strEventName, DWORD dwCookie);

private:
	//隐藏基类函数
	HRESULT AttachEvent(const std::wstring& strEventName, CLuaEvent* pEvent, DWORD& dwCookie);
	HRESULT DetachEvent(const std::wstring& strEventName, CLuaEvent* pEvent);	
	HRESULT DetachEvent(const std::wstring& strEventName, DWORD dwCookie, CLuaEvent** ppEvent);
};



