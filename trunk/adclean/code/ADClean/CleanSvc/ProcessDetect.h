#pragma once

class ProcessDetect {
public:
	static bool IsBrowerFileName(const wchar_t* szFileName);
public:
	static bool IsAnyBrowerRunning();
	static bool IsGreenShieldRunning();
	static bool IsGreenShieldSetupRunning();
	static bool IsGreenShieldOrGreenShieldSetupRunning();
};
