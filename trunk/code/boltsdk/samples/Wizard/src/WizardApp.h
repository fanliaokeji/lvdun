#pragma once

#include "./BaseEvent.h"

class WizardAppEvent
{
public:

	size_t AttachConfigChangeEvent(lua_State* luaState,long luaRef)
	{
		LuaConfigChangeEvent* lpEvent = new LuaConfigChangeEvent(luaState, luaRef);

		return m_configChangeEventContainer.AttachEvent(lpEvent);
	}

	bool RemoveConfigChangeEvent(size_t cookie)
	{
		return m_configChangeEventContainer.RemoveEvent(cookie);
	}

	bool Clear();

	void FireConfigChange(const wchar_t* lpSection, const wchar_t* lpKey, const wchar_t* lpValue);
	void FireConfigChange(const wchar_t* lpSection, const wchar_t* lpKey, int value);

private:

	typedef BaseEventContainerT<LuaConfigChangeEvent> ConfigChangeEventContainer;
	ConfigChangeEventContainer m_configChangeEventContainer;
};

class WizardApp
{
public:
	WizardApp(void);
	~WizardApp(void);

	static WizardApp* GetInstance();

	bool Init();
	bool Uinit();

	bool Quit(int exitCode);

	// ���ü򵥽ӿ�
	bool GetString(const wchar_t*  lpSection, const wchar_t* lpKey, std::wstring& strRet, const wchar_t* lpDefault = L"");
	bool SetString(const wchar_t*  lpSection, const wchar_t* lpKey, const wchar_t* lpValue);

	int GetInt(const wchar_t*  lpSection, const wchar_t* lpKey, int defaultValue = 0);
	bool SetInt(const wchar_t*  lpSection, const wchar_t* lpKey, int value);

	WizardAppEvent* GetEvent();

	// һ��ע�ᵽie�����disp�ӿ�
	IDispatch* GetWebBrowserDisp();

	// ����bugreport
	void Crash();

	// ��ȡһ���򵥵�flash�ز�
	bool GetFlashFile(std::wstring& strFile);

private:

	bool IsKeyExist(const wchar_t*  lpSection, const wchar_t* lpKey);

private:

	WizardAppEvent*	m_lpEvent;

	std::wstring	m_strConfig;

	CComPtr<IDispatch>	m_spWebDisp;
};
