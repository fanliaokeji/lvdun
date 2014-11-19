#pragma once
#include <Windows.h>
#include <cstddef>
#include <ctime>

class LaunchGreenShieldConfig {
private:
	UINT m_NoRemind;
	UINT m_NoRemindSpanDay;
	UINT m_LaunchInterval;
	UINT m_MaxCntPerDay;
	UINT m_Cnt;
	__int64 m_LastPull;
	bool m_Valid;
	__time64_t m_LastUpdateTime;
public:
	LaunchGreenShieldConfig();
	bool UpdateConfig();
	bool Valid() const;
	bool IsNoRemind() const;
	UINT GetNoRemindSpanDay() const;
	UINT GetLaunchInterval() const;
	UINT GetMaxCntPerDay() const;
	UINT GetCnt() const;
	__int64 GetLastPull() const;
	__time64_t GetLastUpdateTime() const;
	bool IsEnableLaunchNow() const;
	bool CheckEnableLaunchNow();
private:
	bool GetGreenShieldConfigPath(wchar_t* szPathBuffer, std::size_t bufferLength);
	bool GetConfigFilePath(wchar_t* szPathBuffer, std::size_t bufferLength);
	bool wstring_to_int64(const wchar_t* szStr, __int64& value);
};
