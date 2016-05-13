#pragma once
#include <list>
#include <map>
#include <algorithm>


template<typename TEvent>
class IEventEnum
{
public:
	IEventEnum(){}
	virtual ~IEventEnum(){}
	virtual HRESULT Reset() = 0;
	virtual HRESULT Next(TEvent*& pEvent) = 0;
};


//===============================================================================
//		CMultiEventContainer		类定义
template<typename TEvent>
class CEventContainer:public IEventEnum<TEvent>
{
	template<typename TEvent>
	struct EventRecord
	{
		EventRecord(DWORD dwCookie, TEvent* pEvent):m_dwCookie(dwCookie), m_pEvent(pEvent)
		{}
		inline bool operator==(TEvent* pEvent)const
		{
			return m_pEvent == pEvent;
		}
		inline bool operator==(DWORD dwCookie)const
		{
			return m_dwCookie == dwCookie;
		}
		DWORD m_dwCookie;
		TEvent* m_pEvent;
	};
	typedef std::list<EventRecord<TEvent> > EventList;
	typedef typename EventList::iterator EventListIte;

public:
	CEventContainer(void);
	virtual ~CEventContainer(void);

	HRESULT AttachEvent(TEvent* pEvent, DWORD& dwCookie);

	HRESULT DetachEvent(TEvent* pEvent);	
	HRESULT DetachEvent(DWORD dwCookie, TEvent** ppEvent = NULL);

	HRESULT FireEvent(void* vpParam);

	virtual HRESULT GetEventEnum(IEventEnum<TEvent>*& pEventEnum);

protected:
	virtual HRESULT Reset();
	virtual HRESULT Next(TEvent*& pEvent);

	virtual HRESULT CallEvent(TEvent*& /*pEvent*/, void* /*vpParam*/);

private:
	EventList m_listEvents;
	EventListIte m_iteEvent;
	DWORD m_dwLastCookie;
};


//===============================================================================
//		CMultiEventContainer		类定义
template<typename TEvent>
class CMultiEventContainer
{
	typedef std::map<std::wstring, CEventContainer<TEvent> > Name2ContainerMap;
	typedef typename Name2ContainerMap::iterator Name2ContainerMapIte;

public:
	CMultiEventContainer(void);
	virtual ~CMultiEventContainer(void);

public:
	HRESULT AttachEvent(const std::wstring& strEventName, TEvent* pEvent, DWORD& dwCookie);
	HRESULT DetachEvent(const std::wstring& strEventName, TEvent* pEvent);	
	HRESULT DetachEvent(const std::wstring& strEventName, DWORD dwCookie, TEvent** ppEvent = NULL);

	HRESULT FireEvent(const std::wstring& strEventName, void* vpParam);

	virtual HRESULT GetEventEnum(const std::wstring& strEventName, IEventEnum<TEvent>*& pEventEnum);

protected:
	virtual HRESULT CallEvent(const std::wstring& /*strEventName*/, TEvent*& /*pEvent*/, void* /*vpParam*/);


private:
	Name2ContainerMap m_mapName2Containers;
};




//===============================================================================
//		CEventContainer函数实现
template<typename TEvent>
CEventContainer<TEvent>::CEventContainer(void)
{
	m_iteEvent = m_listEvents.begin();
	m_dwLastCookie = 0;
}

template<typename TEvent>
CEventContainer<TEvent>::~CEventContainer(void)
{
}


template<typename TEvent>
HRESULT CEventContainer<TEvent>::AttachEvent(TEvent* pEvent, DWORD& dwCookie)
{
	EventListIte ite = std::find(m_listEvents.begin(), m_listEvents.end(), pEvent);
	if (ite != m_listEvents.end())
	{
		dwCookie = 0;
		return S_FALSE;
	}

	dwCookie = ++m_dwLastCookie;
	m_listEvents.push_back(EventRecord<TEvent>(dwCookie, pEvent));
	return S_OK;
}


	
template<typename TEvent>
HRESULT CEventContainer<TEvent>::DetachEvent(TEvent* pEvent)
{
	EventListIte ite = std::find(m_listEvents.begin(), m_listEvents.end(), pEvent);
	if (ite == m_listEvents.end())
	{
		return E_FAIL;
	}

	m_listEvents.erase(ite);
	return S_OK;
}

template<typename TEvent>
HRESULT CEventContainer<TEvent>::DetachEvent(DWORD dwCookie, TEvent** ppEvent)
{
	EventListIte ite = std::find(m_listEvents.begin(), m_listEvents.end(), dwCookie);
	if (ite == m_listEvents.end())
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}

	if (ppEvent != NULL)
	{
		*ppEvent = ite->m_pEvent;
	}

	m_listEvents.erase(ite);
	return S_OK;
}

template<typename TEvent>
HRESULT CEventContainer<TEvent>::FireEvent(void* vpParam)
{
	for (EventListIte ite = m_listEvents.begin(); ite != m_listEvents.end(); ++ite)
	{
		CallEvent(ite->m_pEvent, vpParam);
	}
	return S_OK;
}

template<typename TEvent>
HRESULT CEventContainer<TEvent>::GetEventEnum(IEventEnum<TEvent>*& pEventEnum)
{
	pEventEnum = this;
	Reset();
	return S_OK;
}

template<typename TEvent>
HRESULT CEventContainer<TEvent>::Reset()
{
	m_iteEvent = m_listEvents.begin();
	return S_OK;
}


template<typename TEvent>
HRESULT CEventContainer<TEvent>::Next(TEvent*& pEvent)
{
	if (m_iteEvent == m_listEvents.end())
	{
		pEvent = NULL;
		return S_FALSE;
	}

	pEvent = m_iteEvent->m_pEvent;
	++m_iteEvent;
	return S_OK;
}

template<typename TEvent>
HRESULT CEventContainer<TEvent>::CallEvent(TEvent*& /*pEvent*/, void* /*vpParam*/)
{
	return S_OK;
}




//===============================================================================
//		CMultiEventContainer函数实现
template<typename TEvent>
CMultiEventContainer<TEvent>::CMultiEventContainer(void)
{
}

template<typename TEvent>
CMultiEventContainer<TEvent>::~CMultiEventContainer(void)
{
}


template<typename TEvent>
HRESULT CMultiEventContainer<TEvent>::AttachEvent(const std::wstring& strEventName, TEvent* pEvent, DWORD& dwCookie)
{
	Name2ContainerMapIte ite = m_mapName2Containers.find(strEventName);
	if (ite == m_mapName2Containers.end())
	{
		ite = m_mapName2Containers.insert(std::make_pair(strEventName, CEventContainer<TEvent>())).first;
	}

	ite->second.AttachEvent(pEvent, dwCookie);
	return S_OK;
}


template<typename TEvent>
HRESULT CMultiEventContainer<TEvent>::DetachEvent(const std::wstring& strEventName, TEvent* pEvent)
{
	Name2ContainerMapIte ite = m_mapName2Containers.find(strEventName);
	if (ite == m_mapName2Containers.end())
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}

	ite->second.DetachEvent(pEvent);
	return S_OK;
}


template<typename TEvent>
HRESULT CMultiEventContainer<TEvent>::DetachEvent(const std::wstring& strEventName, DWORD dwCookie, TEvent** ppEvent)
{
	Name2ContainerMapIte ite = m_mapName2Containers.find(strEventName);
	if (ite == m_mapName2Containers.end())
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}

	ite->second.DetachEvent(dwCookie, ppEvent);
	return S_OK;
}


template<typename TEvent>
HRESULT CMultiEventContainer<TEvent>::FireEvent(const std::wstring& strEventName, void* vpParam)
{
	Name2ContainerMapIte ite = m_mapName2Containers.find(strEventName);
	if (ite == m_mapName2Containers.end())
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}

	IEventEnum* pEvnetEnum = NULL;
	ite->second.GetEventEnum(pEvnetEnum);
	ATLASSERT(pEvnetEnum != NULL);

	CLuaEvent* pEvent = NULL;
	pEvnetEnum->Reset();
	while (pEvnetEnum->Next(pEvent) == S_OK)
	{
		CallEvent(strEventName, pEvent, vpParam);
	}
	return S_OK;
}

template<typename TEvent>
HRESULT CMultiEventContainer<TEvent>::GetEventEnum(const std::wstring& strEventName, IEventEnum<TEvent>*& pEventEnum)
{
	Name2ContainerMapIte ite = m_mapName2Containers.find(strEventName);
	if (ite == m_mapName2Containers.end())
	{
		return E_FAIL;
	}

	ite->second.GetEventEnum(pEventEnum);
	return S_OK;
}


template<typename TEvent>
HRESULT CMultiEventContainer<TEvent>::CallEvent(const std::wstring& /*strEventName*/, TEvent*& /*pEvent*/, void* /*vpParam*/)
{
	return S_OK;
}




