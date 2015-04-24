#include "StdAfx.h"
#include "LuaNotifyIcon.h"
#include "..\FlyRabbit\resource.h"

LuaNotifyIcon::LuaNotifyIcon(void)
{
}

LuaNotifyIcon::~LuaNotifyIcon(void)
{
}

XLLRTGlobalAPI LuaNotifyIcon::sm_LuaMemberFunctions[] = 
{
	{"Attach", Attach},
	{"Show", Show},
	{"Hide", Hide},
	{"ShowNotifyIconTip",ShowNotifyIconTip},
	{"SetIcon", SetIcon},
	{"FlashTray", FlashTray},
	{"CancleFlashTray", CancleFlashTray},
    {NULL, NULL}
};

void LuaNotifyIcon::OnNotifyIcon(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 调用lua的连接点
	ILuaEventEnum* pEventEnum = NULL;
	m_luaEventContainer.GetEventEnum(pEventEnum);
	pEventEnum->Reset();

	CLuaEvent* pEvent = NULL;
	while (pEventEnum->Next(pEvent) == S_OK)
	{
		lua_State* luaState = pEvent->GetLuaState();
		long nowTop = lua_gettop(luaState);
		pEvent->PushFunction();
		pEvent->PushParam(uMsg);
		pEvent->PushParam(wParam);
		pEvent->PushParam(lParam);
		pEvent->Call(3, 0);
		lua_settop(luaState,nowTop);
	}
}

int LuaNotifyIcon::Attach(lua_State* pLuaState)
{
    LuaNotifyIcon** ppNotifyIcon = (LuaNotifyIcon **)luaL_checkudata(pLuaState, 1, NOTIFYICON_CLASS);
    if (ppNotifyIcon == NULL)
    {
        return 0;
    }

	LuaNotifyIcon* pNotifyIcon = *ppNotifyIcon;

	gsNotifyIcon.Attach(pNotifyIcon);

	DWORD dwCookie;
	pNotifyIcon->m_luaEventContainer.AttachEvent(pLuaState, 2, dwCookie);
	LuaPush(pLuaState, dwCookie);
	return 1;
}

LuaNotifyIcon* __stdcall LuaNotifyIcon::Instance(void *)
{
    static LuaNotifyIcon * s_instance = NULL;
    if (s_instance == NULL)
    {
        s_instance = new LuaNotifyIcon();
    }
    return s_instance;
}

void LuaNotifyIcon::RegisterSelf(XL_LRT_ENV_HANDLE hEnv)
{
    if (hEnv == NULL)
    {
        return;
    }

    XLLRTObject object;
    object.ClassName = NOTIFYICON_CLASS;
    object.ObjName = NOTIFYICON_OBJ;
    object.MemberFunctions = sm_LuaMemberFunctions;
    object.userData = NULL;
    object.pfnGetObject = (fnGetObject)LuaNotifyIcon::Instance;

	XLLRT_RegisterGlobalObj(hEnv, object);
}

int LuaNotifyIcon::Show( lua_State* pLuaState )
{
	LuaNotifyIcon** ppNotifyIcon = (LuaNotifyIcon **)luaL_checkudata(pLuaState, 1, NOTIFYICON_CLASS);
	if (ppNotifyIcon == NULL)
	{
		return 0;
	}
	gsNotifyIcon.Show();
	return 0;
}


int LuaNotifyIcon::SetIcon(lua_State* pLuaState)
{
	LuaNotifyIcon** ppNotifyIcon = (LuaNotifyIcon **)luaL_checkudata(pLuaState, 1, NOTIFYICON_CLASS);
	if (ppNotifyIcon == NULL)
	{
		return 0;
	}
	
	std::wstring strTip(L"");
	if (lua_isstring(pLuaState, 3))
	{
		strTip = LuaGetString(pLuaState, 3);
	}

	if (lua_isnumber(pLuaState, 2) || lua_isstring(pLuaState, 2))
	{
		if (lua_isnumber(pLuaState, 2))
		{
			HICON hIcon = (HICON)LuaGetInt32(pLuaState, 2);
			gsNotifyIcon.SetIcon(hIcon, strTip.c_str());
		}
		else
		{
			std::wstring strIconPath = LuaGetString(pLuaState, 2);
			gsNotifyIcon.SetIcon(strIconPath.c_str(), strTip.c_str());
		}
	}
	else
	{
		HMODULE hInst = ::GetModuleHandle(NULL);
		TSDEBUG(_T("hInst = 0x%p"), hInst);
		HICON hExeIcon = (HICON)::LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON,16,16, LR_DEFAULTCOLOR);
		TSDEBUG(_T("hSmallIcon = 0x%p"), hExeIcon);
		gsNotifyIcon.SetIcon(hExeIcon, strTip.c_str());
	}
	return 0;
}


int LuaNotifyIcon::Hide( lua_State* pLuaState )
{
	LuaNotifyIcon** ppNotifyIcon = (LuaNotifyIcon **)luaL_checkudata(pLuaState, 1, NOTIFYICON_CLASS);
	if (ppNotifyIcon == NULL)
	{
		return 0;
	}
	gsNotifyIcon.Hide();
	return 0;
}

int LuaNotifyIcon::ShowNotifyIconTip(lua_State* pLuaState)
{
	LuaNotifyIcon** ppNotifyIcon = (LuaNotifyIcon **)luaL_checkudata(pLuaState, 1, NOTIFYICON_CLASS);
	if(ppNotifyIcon == NULL )
	{
		return 0;
	}

	BOOL bShow = lua_toboolean(pLuaState,2);
	std::wstring strNotfyTip(L"");
	if (lua_isstring(pLuaState, 3))
	{
		strNotfyTip = LuaGetString(pLuaState, 3);
	}
	gsNotifyIcon.ShowTip(bShow,strNotfyTip.c_str());
	return 0;
}

int LuaNotifyIcon::FlashTray(lua_State* pLuaState)
{
	LuaNotifyIcon** ppNotifyIcon = (LuaNotifyIcon **)luaL_checkudata(pLuaState, 1, NOTIFYICON_CLASS);
	if(ppNotifyIcon == NULL )
	{
		return 0;
	}
	DWORD dwInterval = 500;
	if ( lua_isnumber(pLuaState, 2))
	{
		dwInterval = (DWORD)lua_tointeger(pLuaState,2);
	}
	gsNotifyIcon.FlashIcon(dwInterval);
	return 0;
}

int LuaNotifyIcon::CancleFlashTray(lua_State* pLuaState)
{
	LuaNotifyIcon** ppNotifyIcon = (LuaNotifyIcon **)luaL_checkudata(pLuaState, 1, NOTIFYICON_CLASS);
	if(ppNotifyIcon == NULL )
	{
		return 0;
	}
	gsNotifyIcon.CancelFlash();
	return 0;
}