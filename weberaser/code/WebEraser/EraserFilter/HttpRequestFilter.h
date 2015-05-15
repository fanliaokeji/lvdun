#pragma once
#include <map>
#include <string>
#include <vector>
#include <deque>

#include <boost/regex.hpp>

#include <Windows.h>

#include "Lock.h"

class HttpRequestFilter {
private:
	mutable XMLib::CriticalSection cs;
	// global control
	bool m_enable;
	bool m_enableRedirect;
	HANDLE m_hIPCFileMapping;
	HttpRequestFilter();
public:
	bool Enable(bool enable, unsigned short listen_port);
	void EnableRedirect(bool enable);
	bool IsEnable() const;
	bool IsEnableRedirect() const;
public:
	static HttpRequestFilter& GetInstance();
};
