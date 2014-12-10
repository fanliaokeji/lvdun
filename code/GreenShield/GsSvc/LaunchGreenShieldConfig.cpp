#include "stdafx.h"
#include "LaunchGreenShieldConfig.h"

#include <cwchar>
#include <algorithm>
#include <sstream>
#include <cassert>

#include "Utility.h"
#include "AdvanceFunctionProvider.h"
#include "ScopeResourceHandle.h"

LaunchGreenShieldConfig::LaunchGreenShieldConfig() : m_NoRemind(0), m_NoRemindSpanDay(7), m_LaunchInterval(1), m_MaxCntPerDay(1), m_Cnt(0), m_LastPull(0), m_Valid(false), m_LastUpdateTime(0)
{
}

bool LaunchGreenShieldConfig::UpdateConfig()
{
	_time64(&this->m_LastUpdateTime);
	this->m_Valid = false;
	wchar_t configFilePath[MAX_PATH];
	if(!this->GetConfigFilePath(configFilePath, MAX_PATH)) {
		return false;
	}
	if(!::PathFileExists(configFilePath)) {
		this->m_NoRemind = 0;
		this->m_NoRemindSpanDay = 7;
		this->m_Cnt = 0;
		this->m_LaunchInterval = 1;
		this->m_MaxCntPerDay = 1;
		this->m_LastPull = 0;
		this->m_Valid = true;
		return true;
	}

	const wchar_t* szSectionName = L"pusher";

	::SetLastError(0);
	this->m_NoRemind = ::GetPrivateProfileInt(szSectionName, L"noremind", 0, configFilePath);
	this->m_NoRemindSpanDay = ::GetPrivateProfileInt(szSectionName, L"noremindspanday", 7, configFilePath);
	this->m_LaunchInterval = ::GetPrivateProfileInt(szSectionName, L"intervaltime", 1, configFilePath);
	if(this->m_LaunchInterval == 0) {
		this->m_LaunchInterval = 1;
	}
	this->m_MaxCntPerDay = ::GetPrivateProfileInt(szSectionName, L"maxcntperday", 1, configFilePath);
	this->m_Cnt = ::GetPrivateProfileInt(szSectionName, L"cnt", this->m_MaxCntPerDay, configFilePath);
	DWORD dwLastError = ::GetLastError();
	if(dwLastError != ERROR_SUCCESS) {
		TSWARN4CXX("::GetLastError() != ERROR_SUCCESS. Error: " << dwLastError);
	}

	wchar_t lastPullTimeBuffer[30];
	::GetPrivateProfileString(szSectionName, L"lastpull", L"0", lastPullTimeBuffer, sizeof(lastPullTimeBuffer) / sizeof(lastPullTimeBuffer[0]), configFilePath);
	if(!this->wstring_to_int64(lastPullTimeBuffer, this->m_LastPull)) {
		TSWARN4CXX("lastpull is not a valid __int64 value.");
		this->m_LastPull = 0;
	}
	this->m_Valid = true;
	return true;
}

bool LaunchGreenShieldConfig::GetConfigFilePath(wchar_t* szPathBuffer, std::size_t bufferLength)
{
	if(!GetAllUsersPublicPath(szPathBuffer, bufferLength)) {
		return false;
	}
	wchar_t tmpBuffer[MAX_PATH];
	if(::PathCombine(tmpBuffer, szPathBuffer, L"GreenShield\\startcfg.ini") == NULL) {
		return false;
	}
	std::size_t length = std::wcslen(tmpBuffer) + 1;
	if(bufferLength < length) {
		return false;
	}
	std::copy(tmpBuffer, tmpBuffer + length, szPathBuffer);
	return true;
}

bool LaunchGreenShieldConfig::wstring_to_int64(const wchar_t* szStr, __int64& value)
{
	std::wstringstream wss;
	wss << szStr;
	wss >> value;
	if(!wss.fail() && wss.eof()) {
		return true;
	}
	return false;
}

bool LaunchGreenShieldConfig::Valid() const
{
	return this->m_Valid;
}

bool LaunchGreenShieldConfig::IsNoRemind() const
{
	assert(this->Valid());
	return this->m_NoRemind != 0;
}

UINT LaunchGreenShieldConfig::GetNoRemindSpanDay() const
{
	assert(this->Valid());
	return this->m_NoRemindSpanDay;
}

UINT LaunchGreenShieldConfig::GetLaunchInterval() const
{
	assert(this->Valid());
	return this->m_LaunchInterval;
}

UINT LaunchGreenShieldConfig::GetMaxCntPerDay() const
{
	assert(this->Valid());
	return this->m_MaxCntPerDay;
}

UINT LaunchGreenShieldConfig::GetCnt() const
{
	assert(this->Valid());
	return this->m_Cnt;
}

__int64 LaunchGreenShieldConfig::GetLastPull() const
{
	assert(this->Valid());
	return this->m_LastPull;
}

__time64_t LaunchGreenShieldConfig::GetLastUpdateTime() const
{
	return this->m_LastUpdateTime;
}

bool LaunchGreenShieldConfig::IsEnableLaunchNow() const
{
	assert(this->Valid());
	if(!this->Valid()) {
		return false;
	}
	if(this->GetMaxCntPerDay() == 0) {
		return false;
	}

	__time64_t nLastLaunchTime = this->GetLastPull();
	__time64_t nCurrentTime = 0;
	_time64(&nCurrentTime);

	if(this->IsNoRemind() && _abs64(nCurrentTime - nLastLaunchTime) / (3600 * 24) < this->GetNoRemindSpanDay()) {
		return false;
	}
	if(_abs64(nCurrentTime - nLastLaunchTime) / 3600 < this->GetLaunchInterval()) {
		return false;
	}
	if(this->GetCnt() >= this->GetMaxCntPerDay()) {
		const tm* curentTime = _gmtime64(&nCurrentTime);
		int cur_year = curentTime->tm_year;
		int cur_yday = curentTime->tm_yday;
		const tm* lastLaunchTime = _gmtime64(&nLastLaunchTime);
		if(cur_year == lastLaunchTime->tm_year && cur_yday == lastLaunchTime->tm_yday) {
			return false;
		}
	}
	return true;
}

bool LaunchGreenShieldConfig::CheckEnableLaunchNow()
{
	assert(this->Valid());
	if(!this->Valid()) {
		return false;
	}
	if(!this->UpdateConfig()) {
		return false;
	}
	if(!this->IsEnableLaunchNow()) {
		return false;
	}

	wchar_t configFilePath[MAX_PATH];
	if(!this->GetConfigFilePath(configFilePath, MAX_PATH)) {
		return false;
	}
	const wchar_t* szSectionName = L"pusher";

	__time64_t nLastLaunchTime = this->GetLastPull();
	__time64_t nCurrentTime = 0;
	_time64(&nCurrentTime);
	const tm* curentTime = _gmtime64(&nCurrentTime);
	int cur_year = curentTime->tm_year;
	int cur_yday = curentTime->tm_yday;
	const tm* lastLaunchTime = _gmtime64(&nLastLaunchTime);
	if(cur_year != lastLaunchTime->tm_year || cur_yday != lastLaunchTime->tm_yday) {
		this->m_Cnt = 1;
	}
	else {
		++this->m_Cnt;
	}
	std::wstring cntStr;
	{
		std::wstringstream wss;
		wss << this->m_Cnt;
		wss >> cntStr;
	}
	if(!::WritePrivateProfileString(szSectionName, L"cnt", cntStr.c_str(), configFilePath)) {
		return false;
	}

	this->m_LastPull = nCurrentTime;
	std::wstring lastPullStr;
	{
		std::wstringstream wss;
		wss << this->m_LastPull;
		wss >> lastPullStr;
	}
	if(!::WritePrivateProfileString(szSectionName, L"lastpull", lastPullStr.c_str(), configFilePath)) {
		return false;
	}
	return true;
}
