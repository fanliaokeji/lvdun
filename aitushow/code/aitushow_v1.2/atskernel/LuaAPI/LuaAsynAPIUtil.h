#pragma once

#include "../Utility/LuaHttpUtil.h"
#include "../Utility/AjaxDownloadHelper.h"
#include "../Utility/LuaAPIHelper.h"

#define WM_HTTPCONTENTGOT WM_USER + 201
#define WM_HTTPFILEGOT WM_USER + 202
#define WM_SENDHTTPSTAT		WM_USER + 2002
#define WM_CREATEPROCESSFINISH			WM_USER + 2004
#define WM_NEWASYNGETHTTPFILETASKFINISH		WM_USER + 2010
#define WM_NEWASYNUNZIPTASKFINISH			WM_USER + 2011
#define WM_NEWASYNSENDHTTPSTATTASKFINISH	WM_USER + 2013
#define WM_AJAXDOWNLOADFILESUCCESS		WM_USER + 2020
#define WM_AJAXDOWNLOADFILEFAILED		WM_USER + 2021

#define WM_HTTPFILEGOTPROGRESS		WM_USER + 2023
#define WM_KILLPROCESS					WM_USER + 2024
#define WM_GETFOLDERS					WM_USER + 2025

enum AjaxTaskFlag
{
	AJAXTASKFLAG_GETHTTPFILE = 0,
	AJAXTASKFLAG_SENDHTTPSTAT = 1,
	AJAXTASKFLAG_GETHTTPCONTENT = 2,
	AJAXTASKFLAG_SENDHTTPSTATEX = 3
};

#define XMPTIPWND_ASYNCUTIL_OBJ		"API.AsynUtil"
#define XMPTIPWND_ASYNCUTIL_CLASS	"API.AsynUtil.Class"

class LuaAsynUtil : public IAJAXStateChange
{
private:
	LuaAsynUtil(void) {}
	~LuaAsynUtil(void) {}

public:
	static void* __stdcall GetInstance(void* ); 
	static void RegisterSelf(XL_LRT_ENV_HANDLE hEnv);
	STDMETHOD(OnStateChange)(CAJAX* pSource, const bool bSucceeded, const int nHttpState, const BSTR bstrResponse,   IDispatch* pDispth);
	STDMETHOD(QueryInterface)(const struct _GUID &/*iid*/,void ** ppv){*ppv=this;return S_OK;}	
	ULONG __stdcall AddRef(void){	return 1;	}	
	ULONG __stdcall Release(void){	return 0;	}


	static int AsynSendHttpStat(lua_State* pLuaState);
	static int GetHttpFile(lua_State* pLuaState);
	static int GetHttpFileWithProgress(lua_State* pLuaState);
	static int GetHttpContent(lua_State* pState);
	static int SoftExit(lua_State* pLuaState);
	static int StartTimerEx(lua_State* pLuaState);
	static int StopTimerEx(lua_State* pLuaState);
	static int StopAllTimerEx(lua_State* pLuaState);

	static int NewAsynGetHttpFile(lua_State* pLuaState);
	static int NewAsynSendHttpStat(lua_State* pLuaState);
	//AJAX
	static int AjaxSendHttpStat(lua_State* pLuaState);
	static int AjaxGetHttpContent(lua_State* pLuaState);
	static int AjaxGetHttpFile(lua_State* pLuaState);


	static int AsynCreateProcess(lua_State* pLuaState);

	static int NewAsynGetHttpFileWithProgress(lua_State* pLuaState);
	static int AsynKillProcess(lua_State* pLuaState);
	static int AsynGetFolders(lua_State* pLuaState);

private:
	static XLLRTGlobalAPI  s_functionlist[];
};




class HttpStatData
{
public:
	HttpStatData(const char* pUrl, bool bAsync, lua_State* pState, LONG lRefFn);
	void Work();

	void Notify(int nResult)
	{
		lua_rawgeti(m_callInfo.GetLuaState(), LUA_REGISTRYINDEX, m_callInfo.GetRefFn());
		lua_pushinteger(m_callInfo.GetLuaState(), nResult);
		XLLRT_LuaCall(m_callInfo.GetLuaState(), 1, 0, L"HttpStat Callback");
	}
private:
	std::wstring m_strUrl;
	bool m_bAsync;
	LuaCallInfo m_callInfo;
};

struct SGetHttpContentData
{
	LuaCallInfo m_callInfo;
	std::wstring m_strUrl;
	std::string m_strContent;

	SGetHttpContentData(LPCTSTR pszUrl, lua_State *pState, LONG lRefFn) 
		: m_strUrl(pszUrl)
		, m_callInfo(pState, lRefFn)
	{
		TSTRACEAUTO();
	}

	~SGetHttpContentData()
	{
		TSTRACEAUTO();
	}

	void Notify(int nRetCode)
	{
		lua_rawgeti(m_callInfo.GetLuaState(), LUA_REGISTRYINDEX, m_callInfo.GetRefFn());

		int iRetCount = 0;
		lua_pushinteger(m_callInfo.GetLuaState(), nRetCode);
		++iRetCount;

		lua_pushstring(m_callInfo.GetLuaState(), m_strContent.c_str());
		++iRetCount;

		XLLRT_LuaCall(m_callInfo.GetLuaState(), iRetCount, 0, L"GetHttpFile Callback");
	}

	void Work();
};

struct SGetHttpFileData
{
	LuaCallInfo m_callInfo;
	std::wstring m_strUrl;
	std::wstring m_strSavePath;

	SGetHttpFileData(LPCTSTR pszUrl, LPCTSTR pszSavePath, lua_State *pState, LONG lRefFn) 
		: m_strUrl(pszUrl)
		, m_strSavePath(pszSavePath)
		, m_callInfo(pState, lRefFn)
	{
		TSTRACEAUTO();
	}

	~SGetHttpFileData()
	{
		TSTRACEAUTO();
	}

	void Notify(int nRetCode)
	{
		lua_rawgeti(m_callInfo.GetLuaState(), LUA_REGISTRYINDEX, m_callInfo.GetRefFn());

		int iRetCount = 0;
		lua_pushinteger(m_callInfo.GetLuaState(), nRetCode);
		++iRetCount;

		std::string strSavePathUTF8;
		WCHAR szSavePath[MAX_PATH] = {0};
		wcsncpy(szSavePath,m_strSavePath.c_str(),m_strSavePath.size());
		BSTRToLuaString(szSavePath,strSavePathUTF8);
		lua_pushstring(m_callInfo.GetLuaState(), strSavePathUTF8.c_str());
		++iRetCount;

		XLLRT_LuaCall(m_callInfo.GetLuaState(), iRetCount, 0, L"GetHttpFile Callback");
	}

	void Work();
};

typedef struct _DOWNLOAD_PROGRESS
{
	ULONG ulProgress;
	ULONG ulProgressMax;
}DOWNLOAD_PROGRESS,*PDOWNLOAD_PROGRESS;

class SGetHttpFileDataWithProgress
{
public:
	LuaCallInfo m_callInfo;
	std::wstring m_strUrl;
	std::wstring m_strSavePath;

	SGetHttpFileDataWithProgress(LPCTSTR pszUrl, LPCTSTR pszSavePath, lua_State *pState, LONG lRefFn) 
		: m_strUrl(pszUrl)
		, m_strSavePath(pszSavePath)
		, m_callInfo(pState, lRefFn)
	{
		TSTRACEAUTO();
	}

	~SGetHttpFileDataWithProgress()
	{
		TSTRACEAUTO();
	}

	void Notify(int nRetCode)
	{
		lua_rawgeti(m_callInfo.GetLuaState(), LUA_REGISTRYINDEX, m_callInfo.GetRefFn());

		int iRetCount = 0;
		lua_pushinteger(m_callInfo.GetLuaState(), nRetCode);
		++iRetCount;

		std::string strSavePathUTF8;
		WCHAR szSavePath[MAX_PATH] = {0};
		wcsncpy(szSavePath,m_strSavePath.c_str(),m_strSavePath.size());
		BSTRToLuaString(szSavePath,strSavePathUTF8);
		lua_pushstring(m_callInfo.GetLuaState(), strSavePathUTF8.c_str());
		++iRetCount;

		XLLRT_LuaCall(m_callInfo.GetLuaState(), iRetCount, 0, L"GetHttpFile Callback");
	}
	
	void Notify(int nRetCode,ULONG ulProgress,ULONG ulProgressMax)
	{
		lua_rawgeti(m_callInfo.GetLuaState(), LUA_REGISTRYINDEX, m_callInfo.GetRefFn());
		int iRetCount = 0;
		lua_pushinteger(m_callInfo.GetLuaState(), nRetCode);
		++iRetCount;

		std::string strSavePathUTF8;
		WCHAR szSavePath[MAX_PATH] = {0};
		wcsncpy(szSavePath,m_strSavePath.c_str(),m_strSavePath.size());
		BSTRToLuaString(szSavePath,strSavePathUTF8);
		lua_pushstring(m_callInfo.GetLuaState(), strSavePathUTF8.c_str());
		++iRetCount;

		lua_pushinteger(m_callInfo.GetLuaState(), ulProgress);
		++iRetCount;
		lua_pushinteger(m_callInfo.GetLuaState(), ulProgressMax);
		++iRetCount;

		XLLRT_LuaCall(m_callInfo.GetLuaState(), iRetCount, 0, L"GetHttpFile Callback Progress");
	}
	void Work();
};

class TCallback : public IBindStatusCallback  
{
public:
	TCallback(SGetHttpFileDataWithProgress * pData) 
	{
		m_cRef = 1;
		m_percent = 0;
		m_pGetHttpFileData = pData;
	}
private:
	STDMETHODIMP QueryInterface(REFIID riid,void **ppv)
	{
		*ppv = NULL;  
		if (riid==IID_IUnknown || riid==IID_IBindStatusCallback) {  
			*ppv = this;  
			AddRef();  
			return S_OK;  
		}  
		return E_NOINTERFACE;  
	}  
	STDMETHODIMP_(ULONG) AddRef()
	{
		return m_cRef++; 
	}
	STDMETHODIMP_(ULONG) Release()
	{
		if(--m_cRef==0) {  
			delete this;  
			return 0;  
		}  
		return m_cRef;  
	}
	STDMETHODIMP GetBindInfo(DWORD *grfBINDF,BINDINFO *bindinfo)
	{  
		return E_NOTIMPL;  
	}  
	STDMETHODIMP GetPriority(LONG *nPriority)
	{  
		return E_NOTIMPL;  
	}  
	STDMETHODIMP OnDataAvailable(DWORD grfBSCF,DWORD dwSize,  FORMATETC *formatetc,STGMEDIUM *stgmed)
	{  
		return E_NOTIMPL;  
	}  
	STDMETHODIMP OnLowResource(DWORD reserved)
	{  
		return E_NOTIMPL;  
	}  
	STDMETHODIMP OnObjectAvailable(REFIID iid,IUnknown *punk)
	{  
		return E_NOTIMPL;  
	}  
	STDMETHODIMP OnStartBinding(DWORD dwReserved,IBinding *pib)
	{  
		return E_NOTIMPL;  
	}  
	STDMETHODIMP OnStopBinding(HRESULT hresult,LPCWSTR szError)
	{  
		return E_NOTIMPL;  
	}  
	STDMETHODIMP OnProgress(ULONG ulProgress, ULONG ulProgressMax,  ULONG ulStatusCode, LPCWSTR szStatusText);
public:
	DWORD m_cRef; 
	SGetHttpFileDataWithProgress * m_pGetHttpFileData;
	double m_percent;
};


class CTimerCallBackProcMgr
{
public:
	CTimerCallBackProcMgr(UINT nElapse, lua_State* pState, LONG lRefFn) : m_nElapse(0), m_callInfo(pState, lRefFn)
	{

	}

	void TimerCallBackProc(UINT_PTR nResult)
	{
		lua_rawgeti(m_callInfo.GetLuaState(), LUA_REGISTRYINDEX, m_callInfo.GetRefFn());
		lua_pushinteger(m_callInfo.GetLuaState(), nResult);
		XLLRT_LuaCall(m_callInfo.GetLuaState(), 1, 0, L"CTimerCallBackProcMgr Callback");
	}

private:
	UINT		m_nElapse;
	LuaCallInfo m_callInfo;
};

//
class CAsynCallbackTaskData
{
public:
	CAsynCallbackTaskData(lua_State* pState, LONG lRefFn) : m_callInfo(pState, lRefFn), m_iCoroutineRef(LUA_REFNIL)
	{
	}
	~CAsynCallbackTaskData()
	{
		if (m_callInfo.GetLuaState() != NULL)
		{
			if (m_iCoroutineRef != LUA_REFNIL)
			{
				luaL_unref(m_callInfo.GetLuaState(), LUA_REGISTRYINDEX, m_iCoroutineRef);
				m_iCoroutineRef = LUA_REFNIL;
			}
		}
	}

	void Notify(int nResult, std::wstring strPath, int iCoroutineRef)
	{
		long nowTop = lua_gettop(m_callInfo.GetLuaState());
		lua_rawgeti(m_callInfo.GetLuaState(), LUA_REGISTRYINDEX, m_callInfo.GetRefFn());
		lua_pushinteger(m_callInfo.GetLuaState(), nResult);
		std::string utf8FileName;

		WCHAR szFileName[MAX_PATH] = {0};
		wcsncpy(szFileName,strPath.c_str(),strPath.size());
		BSTRToLuaString(szFileName,utf8FileName);

		lua_pushstring(m_callInfo.GetLuaState(), utf8FileName.c_str());
		lua_rawgeti(m_callInfo.GetLuaState(), LUA_REGISTRYINDEX, iCoroutineRef);
		TSDEBUG4CXX(L"***** Notify lua_rawgeti = " << iCoroutineRef);
		XLLRT_LuaCall(m_callInfo.GetLuaState(), 3, 0, L"AsynTaskData Callback1");
		lua_settop(m_callInfo.GetLuaState(), nowTop);
	}
private:
	LuaCallInfo m_callInfo;
	int			m_iCoroutineRef;
};

class CNewAsynGetHttpFileTaskData 
{
public:
	CNewAsynGetHttpFileTaskData(CAsynCallbackTaskData* pAsynCallbackData, const char* pUrl, const char* pFilePath, int iCoroutineRef);
	void Work();

	void Notify(int nResult)
	{
		m_pCallbackData->Notify(nResult, m_strSavePath, m_iCoroutineRef);
	}
private:
	CAsynCallbackTaskData* m_pCallbackData;
	std::wstring m_strUrl;
	std::wstring m_strSavePath;
	int			 m_iCoroutineRef;
};

//
class NewAsynHttpStatData
{
public:
	NewAsynHttpStatData(CAsynCallbackTaskData* pAsynCallbackData, const char* pUrl, int iCoroutineRef);
	void Work();

	void Notify(int nResult)
	{
		m_pCallbackData->Notify(nResult, L"", m_iCoroutineRef);
	}
private:
	CAsynCallbackTaskData* m_pCallbackData;
	std::wstring m_strUrl;
	int			 m_iCoroutineRef;
};


class CAjaxTaskData : public CAJAX
{
public:
	CAjaxTaskData(lua_State* pState, LONG lRefFn) : m_callInfo(pState, lRefFn) {}

	void NotifySuccess(int nAjaxFlag)
	{
		lua_rawgeti(m_callInfo.GetLuaState(), LUA_REGISTRYINDEX, m_callInfo.GetRefFn());
		lua_pushinteger(m_callInfo.GetLuaState(), 0);

		int iRetCount = 1;
		AjaxTaskFlag atFlag = (AjaxTaskFlag)nAjaxFlag;
		if (atFlag == AJAXTASKFLAG_GETHTTPFILE)
		{
			TSDEBUG4CXX(L"********** callback " << m_bstrSavedPath);
			std::string utf8FilePath;
			BSTRToLuaString(m_bstrSavedPath,utf8FilePath);
			lua_pushstring(m_callInfo.GetLuaState(), utf8FilePath.c_str());
			iRetCount++;

			std::string utf8RespHeader;
			BSTRToLuaString(m_bstrResponseHeader,utf8RespHeader);
			lua_pushstring(m_callInfo.GetLuaState(), utf8RespHeader.c_str());
			iRetCount++;
		}
		else if (atFlag == AJAXTASKFLAG_GETHTTPCONTENT)
		{
			std::string utf8Content;
			BSTRToLuaString(m_bstrSavedPath,utf8Content);
			TSDEBUG4CXX(L" " << utf8Content.c_str());
			lua_pushlstring(m_callInfo.GetLuaState(), utf8Content.c_str(), utf8Content.length());
			iRetCount++;

			std::string utf8RespHeader;
			BSTRToLuaString(m_bstrResponseHeader,utf8RespHeader);

			lua_pushstring(m_callInfo.GetLuaState(), utf8RespHeader.c_str());
			iRetCount++;
		}
		else if (atFlag == AJAXTASKFLAG_SENDHTTPSTAT || atFlag == AJAXTASKFLAG_SENDHTTPSTATEX)
		{
			lua_pushnil(m_callInfo.GetLuaState());
			iRetCount++;

			std::string utf8RespHeader;
			BSTRToLuaString(m_bstrResponseHeader,utf8RespHeader);
			lua_pushstring(m_callInfo.GetLuaState(), utf8RespHeader.c_str());
			iRetCount++;
		}

		XLLRT_LuaCall(m_callInfo.GetLuaState(), iRetCount, 0, L"HttpStat Callback");
	}

	void NotifyFailed(int nAjaxFlag)
	{
		lua_rawgeti(m_callInfo.GetLuaState(), LUA_REGISTRYINDEX, m_callInfo.GetRefFn());
		lua_pushinteger(m_callInfo.GetLuaState(), -1);

		int iRetCount = 1;
		AjaxTaskFlag atFlag = (AjaxTaskFlag)nAjaxFlag;
		if (atFlag == AJAXTASKFLAG_GETHTTPFILE)
		{
			lua_pushnil(m_callInfo.GetLuaState());
			iRetCount++;

			std::string utf8RespHeader;
			BSTRToLuaString(m_bstrResponseHeader,utf8RespHeader);
			lua_pushstring(m_callInfo.GetLuaState(), utf8RespHeader.c_str());
			iRetCount++;
		}
		else if (atFlag == AJAXTASKFLAG_GETHTTPCONTENT)
		{
			lua_pushnil(m_callInfo.GetLuaState());
			iRetCount++;

			std::string utf8RespHeader;
			BSTRToLuaString(m_bstrResponseHeader,utf8RespHeader);
			lua_pushstring(m_callInfo.GetLuaState(), utf8RespHeader.c_str());
			iRetCount++;
		}
		else if (atFlag == AJAXTASKFLAG_SENDHTTPSTAT)
		{
			lua_pushnil(m_callInfo.GetLuaState());
			iRetCount++;

			std::string utf8RespHeader;
			BSTRToLuaString(m_bstrResponseHeader,utf8RespHeader);
			lua_pushstring(m_callInfo.GetLuaState(), utf8RespHeader.c_str());
			iRetCount++;
		}

		XLLRT_LuaCall(m_callInfo.GetLuaState(), iRetCount, 0, L"AjaxDownload Callback");
	}
private:
	LuaCallInfo m_callInfo;
};

class CProcessParam
{
public:
	CProcessParam(const char* pExePath, const char* pParams, const char* pWorkDir, DWORD dwPriority, int nShow, lua_State* pState, LONG lRefFn = LUA_REFNIL);
	void Work();

	void Notify(int nResult)
	{
		int iRetCount = 1;
		lua_State* pLuaState = m_callInfo.GetLuaState();
		lua_rawgeti(pLuaState, LUA_REGISTRYINDEX, m_callInfo.GetRefFn());
		lua_pushinteger(pLuaState, nResult);
		if (nResult == 0)
		{
			iRetCount++;
			lua_newtable(pLuaState);
			lua_pushstring(pLuaState, "hProcess");
			lua_pushlightuserdata(pLuaState, m_pi.hProcess);
			lua_settable(pLuaState, -3);
			lua_pushstring(pLuaState, "hThread");
			lua_pushlightuserdata(pLuaState, m_pi.hThread);
			lua_settable(pLuaState, -3);
			lua_pushstring(pLuaState, "dwProcessId");
			lua_pushnumber(pLuaState, m_pi.dwProcessId);
			lua_settable(pLuaState, -3);
			lua_pushstring(pLuaState, "dwThreadId");
			lua_pushnumber(pLuaState, m_pi.dwThreadId);
			lua_settable(pLuaState, -3);
		}
		XLLRT_LuaCall(pLuaState, iRetCount, 0, L"CProcessParam Callback");
	}
private:
	std::wstring m_strExePath;
	std::wstring m_strParams;
	std::wstring m_strWorkDir;
	LuaCallInfo m_callInfo;
	DWORD		m_dwPriority;
	int			m_nShow;
	LONG		m_lRef;
	PROCESS_INFORMATION m_pi;
};

struct KillProcessData
{
	LuaCallInfo m_callInfo;
	DWORD m_dwPID;
	DWORD m_dwWaitTimeMS;

	KillProcessData(lua_State* pLuaState, DWORD dwPID, DWORD dwWaitTimeMS) : 
	m_dwPID(dwPID), 
		m_dwWaitTimeMS(dwWaitTimeMS), 
		m_callInfo(pLuaState, luaL_ref(pLuaState, LUA_REGISTRYINDEX))
	{
		TSTRACEAUTO();
	}

	~KillProcessData()
	{
		TSTRACEAUTO();
	}

	void Notify(int nErrCode)
	{
		lua_rawgeti(m_callInfo.GetLuaState(), LUA_REGISTRYINDEX, m_callInfo.GetRefFn());
		lua_pushinteger(m_callInfo.GetLuaState(), nErrCode);
		XLLRT_LuaCall(m_callInfo.GetLuaState(), 1, 0, L"KillProcess Callback");
	}

	void Work();
};

struct GetFoldersData
{
	LuaCallInfo m_callInfo;
	std::wstring m_strDir;
	std::map<std::string,bool> m_mapDirInfo;
	GetFoldersData(lua_State* pLuaState, const wchar_t* pszDir) : 
		m_strDir(pszDir),
		m_callInfo(pLuaState,luaL_ref(pLuaState,LUA_REGISTRYINDEX))
	{
		TSTRACEAUTO();
	}

	~GetFoldersData()
	{
		TSTRACEAUTO();
	}

	void Notify(int nErrCode);
	void Work();
};

class CAsynMsgWindow : public CMsgWindow
{
public:
	CAsynMsgWindow() : m_nNextTimerId(0)
	{
	}

	UINT_PTR StartTimer(UINT nElapse, CTimerCallBackProcMgr* pCallBack)
	{
		UINT_PTR nTimerID = m_nNextTimerId; //m_mapTimerID2Callback.size();
		::InterlockedIncrement((LONG *)&m_nNextTimerId);
		SetTimer(nTimerID, nElapse, 0);
		m_mapTimerID2Callback.insert(std::make_pair(nTimerID, pCallBack));
		return nTimerID;
	}

	BOOL StopTimer(UINT_PTR nTimerID)
	{
		TimerIDCallbackMapIte ite = m_mapTimerID2Callback.find(nTimerID);
		if (ite != m_mapTimerID2Callback.end() && ite->second != NULL)
		{
			CTimerCallBackProcMgr *pData = ite->second;
			delete pData;
			pData = NULL;
		}

		m_mapTimerID2Callback.erase(nTimerID);
		KillTimer(nTimerID);
		return TRUE;
	}

	BOOL StopAllTimer()
	{
		for (TimerIDCallbackMapIte ite = m_mapTimerID2Callback.begin(); ite != m_mapTimerID2Callback.end(); ++ite)
		{
			KillTimer(ite->first);
		}
		m_mapTimerID2Callback.clear();
		return TRUE;
	}

	BEGIN_MSG_MAP(CAsynMsgWindow)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER(WM_HTTPFILEGOT, OnHttpFileGot)
		MESSAGE_HANDLER(WM_HTTPCONTENTGOT, OnHttpContentGot)//
		MESSAGE_HANDLER(WM_SENDHTTPSTAT, OnSendHttpStat)//
		MESSAGE_HANDLER(WM_NEWASYNGETHTTPFILETASKFINISH, OnNewAsynGetHttpFileTaskFinish)
		MESSAGE_HANDLER(WM_NEWASYNSENDHTTPSTATTASKFINISH, OnNewAsynSendHttpStatTaskFinish)
		MESSAGE_HANDLER(WM_AJAXDOWNLOADFILEFAILED, OnAjaxDownloadFailed)
		MESSAGE_HANDLER(WM_AJAXDOWNLOADFILESUCCESS, OnAjaxDownloadSucess)
		MESSAGE_HANDLER(WM_CREATEPROCESSFINISH, OnCreateProcessFinish)
		MESSAGE_HANDLER(WM_HTTPFILEGOTPROGRESS, OnHttpFileGotProgress)
		MESSAGE_HANDLER(WM_KILLPROCESS, OnKillProcessFinish)
		MESSAGE_HANDLER(WM_GETFOLDERS, OnGetFoldersFinish)
		CHAIN_MSG_MAP(CMsgWindow)
	END_MSG_MAP()

protected:

	LRESULT OnHttpFileGot(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		SGetHttpFileData *pData = (SGetHttpFileData *) lParam;
		pData->Notify((int) wParam);
		delete pData;
		return 0;
	}

	LRESULT OnHttpContentGot(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		SGetHttpContentData *pData = (SGetHttpContentData *) lParam;
		pData->Notify((int) wParam);
		delete pData;
		return 0;
	}

	LRESULT OnSendHttpStat(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		HttpStatData* pData = (HttpStatData*) lParam;
		pData->Notify((int) wParam);
		delete pData;
		return 0;
	}

	void OnTimer(UINT_PTR nTimerID)
	{
		TimerIDCallbackMapIte ite = m_mapTimerID2Callback.find(nTimerID);
		if (ite != m_mapTimerID2Callback.end() && ite->second != NULL)
		{
			ite->second->TimerCallBackProc(nTimerID);
		}
	}

	LRESULT OnNewAsynGetHttpFileTaskFinish(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		CNewAsynGetHttpFileTaskData* pData = (CNewAsynGetHttpFileTaskData*) lParam;
		pData->Notify((int) wParam);
		delete pData;
		return 0;
	}



	LRESULT OnNewAsynSendHttpStatTaskFinish(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		NewAsynHttpStatData* pData = (NewAsynHttpStatData*) lParam;
		pData->Notify((int) wParam);
		delete pData;
		return 0;
	}

	LRESULT OnAjaxDownloadFailed(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		CAjaxTaskData* pData = (CAjaxTaskData*) lParam;
		pData->NotifyFailed((int) wParam);
		delete pData;
		return 0;
	}

	LRESULT OnAjaxDownloadSucess(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		CAjaxTaskData* pData = (CAjaxTaskData*) lParam;
		pData->NotifySuccess((int) wParam);
		delete pData;
		return 0;
	}
	
	LRESULT OnCreateProcessFinish(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		CProcessParam* pData = (CProcessParam*) lParam;
		pData->Notify((int) wParam);
		delete pData;
		return 0;
	}

	LRESULT OnHttpFileGotProgress(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		SGetHttpFileDataWithProgress * pData = (SGetHttpFileDataWithProgress*) lParam;
		PDOWNLOAD_PROGRESS pdp = PDOWNLOAD_PROGRESS(wParam);
		ULONG ulProgress = pdp->ulProgress;
		ULONG ulProgressMax = pdp->ulProgressMax;
		pData->Notify(-2,ulProgress,ulProgressMax);
		//delete pData;
		return 0;
	}
	
	LRESULT OnKillProcessFinish(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		KillProcessData* pData = (KillProcessData*)lParam;
		pData->Notify((int)wParam);
		delete pData;
		return 0;
	}
	LRESULT OnGetFoldersFinish(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		GetFoldersData* pData = (GetFoldersData*)lParam;
		pData->Notify((int)wParam);
		delete pData;
		return 0;
	}
private:
	typedef std::map<UINT_PTR, CTimerCallBackProcMgr*> TimerIDCallbackMap;
	typedef TimerIDCallbackMap::iterator TimerIDCallbackMapIte;
	TimerIDCallbackMap m_mapTimerID2Callback;
	UINT m_nNextTimerId;
};

