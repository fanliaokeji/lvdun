#pragma once

#include "GSNotifyIcon.h"
#include "LuaEventContainer.h"

#define XMPTIPWND_NOTIFYICON_OBJ		"GS.NotifyIcon"
#define XMPTIPWND_NOTIFYICON_CLASS	"GS.NotifyIcon.Class"

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
