#pragma once
#include <Windows.h>

namespace FliterLib {

	class CriticalSection {
		CRITICAL_SECTION cs;
	private:
		CriticalSection(const CriticalSection&);
		CriticalSection& operator=(const CriticalSection&);
	public:
		CriticalSection();
		~CriticalSection();
		void Lock();
		void Unlock();
	};

	class CriticalSectionLockGuard {
		CriticalSection& cs;
	private:
		CriticalSectionLockGuard& operator=(const CriticalSectionLockGuard&);
	public:
		CriticalSectionLockGuard(CriticalSection& cs_);
		~CriticalSectionLockGuard();
	};

} // namespace XMLib
