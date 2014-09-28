#pragma once
#include <map>
#include <string>
#include <vector>
#include <deque>

#include <boost/regex.hpp>

#include <Windows.h>

#include "Lock.h"

class HttpRequestFilter {
	enum DomainFilterState {
		DFS_UNSET,
		DFS_WHITE,
		DFS_BLACK
	};
	enum NotifyType {
		NT_FILTERASK,
		NT_FILTERRESULT
	};
	typedef std::map<std::string, std::pair<DomainFilterState, std::vector<boost::regex> > > DomainFilterRulesMapType;
private:
	XMLib::CriticalSection cs;
	// global control
	bool m_enable;
	// domain filter rules
	std::map<std::string, std::pair<DomainFilterState, std::vector<boost::regex> > > m_domainFilterRules;
	HttpRequestFilter();

	HANDLE m_hThread;
	HANDLE m_hEvent;

	HANDLE m_hIPCFileMapping;
	std::deque<std::pair<NotifyType, std::string> > m_notifyQueue;
public:
	bool FilterUrl(const std::string& url);
	bool AddDomain(const std::string& domain);
	bool AddDomain(const std::wstring& domain);
	bool AddFilterRule(const std::string& domain, const boost::regex& re);
	bool AddFilterRule(const std::wstring& domain, const std::wstring& pattern);
	bool EnableDomain(const std::string& domain, bool enable);
	bool EnableDomain(const std::wstring& domain, bool enable);
	bool Enable(bool enable);
private:
	bool PostNotify(NotifyType nt, const std::string& str);
	void SendNotifyLoop();
public:
	static HttpRequestFilter& GetInstance();
	static unsigned __stdcall SendNotifyThreadProc(void *arg);
};
