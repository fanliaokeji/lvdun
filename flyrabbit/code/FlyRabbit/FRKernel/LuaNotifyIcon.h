#pragma once

#include "NotifyIcon.h"
#include "LuaEventContainer.h"

#define NOTIFYICON_OBJ		"FR.NotifyIcon"
#define NOTIFYICON_CLASS	"FR.NotifyIcon.Class"

class LuaNotifyIcon :
	public INotifyIconCallback
{
public:
	LuaNotifyIcon(void);
	~LuaNotifyIcon(void);

private:
	void OnNotifyIcon(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

private:
	CLuaEventContainer m_luaEventContainer;

public:
	static LuaNotifyIcon * __stdcall Instance(void *);
	static void RegisterSelf(XL_LRT_ENV_HANDLE hEnv);

public:
	static int Attach(lua_State* pLuaState);
	static int Show(lua_State* pLuaState);
	static int Hide(lua_State* pLuaState);
	static int ShowNotifyIconTip(lua_State* pLuaState);
	static int SetIcon(lua_State* pLuaState);
	static int FlashTray(lua_State* pLuaState);
	static int CancleFlashTray(lua_State* pLuaState);
private:
	static XLLRTGlobalAPI sm_LuaMemberFunctions[];
};
