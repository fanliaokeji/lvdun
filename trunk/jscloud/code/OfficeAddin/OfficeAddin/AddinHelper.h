#pragma once
#include <string>
#include <Windows.h>

class AddinHelper {
	bool m_isInitialized;
	bool m_isService;
	HINSTANCE m_hModule;
	HANDLE m_hMutex;
	HANDLE m_hEndTaskEvent;
	HANDLE m_hTaskThread;
	std::wstring m_productName;
	std::wstring m_mutexName;
	std::wstring m_scriptHostFullPath;
public:
	AddinHelper();
	~AddinHelper();
	bool IsInitialized() const;
	void Initialize(const std::wstring& configFile, bool isService);
	bool EnsureOwnerMutex();
	bool BeginTask();
	bool EndTask();
	static unsigned int __stdcall TaskThreadProc(void* arg);
	unsigned int TaskProc();
	bool LaunchJsEngine(const std::wstring& jsEnginePath);
	bool LaunchJsEngineFromService(const std::wstring& jsEnginePath);
	bool LaunchJsEngineFromOfficeAddin(const std::wstring& jsEnginePath);
private:
	void Initialize(const std::wstring& productName, const std::wstring& mutexName, const std::wstring& scriptHostFullPath, bool isService);
	DWORD GetExplorerPID() const;
	DWORD GetIntervalTime() const;
	bool IsVistaOrHigher() const;
	std::wstring GetScriptHostFullPath() const;
};
