#ifndef __LUAHTTPUTIL_H__
#define __LUAHTTPUTIL_H__

#include <string>
using std::string;
using std::basic_string;

#include <map>
using std::map;

#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>

#include <atlbase.h>
#include <atlwin.h>

#include <XLLuaRuntime.h>
#include "Util.h"
#include "StringUtil.h"

//#define WM_HTTPCONTENTGOT WM_USER + 201
//#define WM_HTTPFILEGOT WM_USER + 202

class LuaCallInfo
{
	lua_State* m_pState;
	LONG m_lRefFn;
public:
	void Set(lua_State* pState, LONG lRefFn)
	{
		m_pState = pState;
		m_lRefFn = lRefFn;
	}

	LuaCallInfo(lua_State* pState, LONG lRefFn) : m_pState(pState), m_lRefFn(lRefFn)
	{
	}

    LuaCallInfo()
	{
		m_pState = NULL;
		m_lRefFn = 0;
	}

	lua_State* GetLuaState() const
	{
		return m_pState;
	}

	LONG GetRefFn() const
	{
		return m_lRefFn;
	}
};

class HttpRequest
{
	static map<string, string> m_mapCache;

	char m_szUrl[MAX_PATH];
	char m_szContent[64 * 1024];
	LuaCallInfo m_luaCallInfo;
	int m_bUseCache;
public:
	HttpRequest(const char* szUrl, int bUseCache, lua_State* pState, LONG lRefFn) : m_bUseCache(bUseCache), m_luaCallInfo(pState, lRefFn)
	{
		m_szContent[0] = '\0';
		strcpy(m_szUrl, szUrl);
	}

	void Start();
	int Work();

	void Notify(int nRet);
	void OnNotify(int nRet);
};

class HttpFileRequest
{
	TCHAR m_szUrl[1024];
	TCHAR m_szPath[1024];
	int m_bUseCache;
	LuaCallInfo m_luaCallInfo;
public:
	HttpFileRequest(const char* szUrl, const char* szPath, int bUseCache, lua_State* pState, LONG lRefFn) : m_bUseCache(bUseCache), m_luaCallInfo(pState, lRefFn)
	{
		// strcpy(m_szUrl, szUrl);
		// strcpy(m_szPath, szPath);
		StringUtil::UTF2T(m_szUrl, szUrl);
		StringUtil::UTF2T(m_szPath, szPath);
	}

	int Work();
	void Callback(int nRet);
};

class CMsgWindow : public CWindowImpl<CMsgWindow>
{
public:
	BEGIN_MSG_MAP(CMsgWindow)
	END_MSG_MAP()
};

class LuaHttpUtil
{
public:
	static int GetHttpContent(lua_State* pState)
	{
		return 0;
	}
	static int GetHttpFile(lua_State* pState)
	{
		return 0;
	}
};

#endif