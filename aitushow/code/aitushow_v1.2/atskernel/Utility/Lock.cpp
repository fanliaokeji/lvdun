#include "stdafx.h"
#include "Lock.h"
#include <Windows.h>

using namespace XMLib;

CriticalSection::CriticalSection()
{
	::InitializeCriticalSection(&this->cs);	
}

CriticalSection::~CriticalSection()
{
	::DeleteCriticalSection(&this->cs);
}


CriticalSection::CriticalSection(const CriticalSection&)
{}

CriticalSection& CriticalSection::operator = (const CriticalSection&)
{
	return *this;
}

void CriticalSection::lock()
{
	::EnterCriticalSection(&this->cs);
}

void CriticalSection::unlock()
{
	::LeaveCriticalSection(&this->cs);
}

CriticalSectionLockGuard& CriticalSectionLockGuard::operator=(const CriticalSectionLockGuard&)
{
	return *this;
}

CriticalSectionLockGuard::CriticalSectionLockGuard(CriticalSection& cs_) : cs(cs_)
{
	this->cs.lock();
}

CriticalSectionLockGuard::~CriticalSectionLockGuard()
{
	this->cs.unlock();
}
