#include "StdAfx.h"
#include "LuaAPIHelper.h"
#include "GSApp.h"
extern CGSApp theApp;

#include "DatFileUtility.h"
#include "LuaAsynUtil.h"
//#include "./XAppTool.h"


//#include "LuaTool.h"

#include <Urlmon.h>
#pragma comment(lib, "Urlmon.lib")
#include <setupapi.h>
#pragma comment (lib, "setupapi.lib")

CAsynMsgWindow g_wndMsg;
CMsgWindow* g_pWndMsg = &g_wndMsg;

XLLRTGlobalAPI  LuaAsynUtil::s_functionlist[] = 
{
	{"AsynSendHttpStat", AsynSendHttpStat},
	{"AsynGetHttpContent", GetHttpContent},
	{"AsynGetHttpFile", GetHttpFile},
	{"AsynGetHttpFileWithProgress", GetHttpFileWithProgress},

	{"SoftExit", SoftExit},
	{"SetTimer", StartTimerEx},
	{"KillTimer", StopTimerEx},
	{"KillAllTimer", StopAllTimerEx},
	
	{"NewAsynGetHttpFile", NewAsynGetHttpFile},

	{"NewAsynSendHttpStat", NewAsynSendHttpStat},
	//AJAX
	{"AjaxSendHttpStat", AjaxSendHttpStat},
	{"AjaxGetHttpContent", AjaxGetHttpContent},
	{"AjaxGetHttpFile", AjaxGetHttpFile},
	{"AsynCreateProcess", AsynCreateProcess},
	
	


	{NULL, NULL}
};


HttpStatData::HttpStatData(const char* pUrl, bool bAsync, lua_State* pState, LONG lRefFn) : m_bAsync(bAsync), m_callInfo(pState, lRefFn)
{
	CComBSTR bstrUrl;
	LuaStringToCComBSTR(pUrl,bstrUrl);
	m_strUrl = bstrUrl.m_str;
}

void HttpStatData::Work()
{
	//try
	//{
		TCHAR szPath[MAX_PATH] = {0};
		URLDownloadToCacheFile(NULL, m_strUrl.c_str(), szPath, MAX_PATH, 0, NULL);
		if (m_bAsync)
		{
			g_wndMsg.PostMessage(WM_SENDHTTPSTAT, 0, (LPARAM) this);
		}
	//}
	//catch (...)
	//{
	//	if (m_bAsync)
	//	{
	//		g_wndMsg.PostMessage(WM_SENDHTTPSTAT, -1, (LPARAM) this);
	//	}
	//}
}

UINT WINAPI HttpStatProc(PVOID pArg)
{
	HttpStatData* pData = (HttpStatData*) pArg;
	pData->Work();
	return 0;
}

int LuaAsynUtil::AsynSendHttpStat(lua_State* pLuaState)
{
	const char* pUrl = luaL_checkstring(pLuaState, 2);
	if (lua_type(pLuaState, 3) == LUA_TFUNCTION)
	{
		HttpStatData* pData = new HttpStatData(pUrl, true, pLuaState, luaL_ref(pLuaState, LUA_REGISTRYINDEX));
		_beginthreadex(NULL, 0, HttpStatProc, pData, 0, NULL);
	}
	else
	{
		HttpStatData* pData = new HttpStatData(pUrl, false, pLuaState, luaL_ref(pLuaState, LUA_REGISTRYINDEX));
		_beginthreadex(NULL, 0, HttpStatProc, pData, 0, NULL);
	}
	return 0;
}


//GetHttpContent

void SGetHttpContentData::Work()
{
	::CoInitialize(NULL);

	TCHAR szSavePath[MAX_PATH] = {0};
	HRESULT hr = ::URLDownloadToCacheFile(NULL, m_strUrl.c_str(), szSavePath, sizeof(szSavePath), 0, NULL);
	int nRetCode = -1;
	if (SUCCEEDED(hr))
	{
		DWORD dwByteRead = 0;
		tipWndDatFileUtility.ReadFileToString(std::wstring(szSavePath), m_strContent, dwByteRead);
		nRetCode = 0;
	}
	g_wndMsg.PostMessage(WM_HTTPCONTENTGOT, nRetCode, (LPARAM) this);

	::CoUninitialize();
}


UINT WINAPI GetHttpContentProc(PVOID pArg)
{
	SGetHttpContentData* pData = (SGetHttpContentData*) pArg;
	pData->Work();
	return 0;
}


int LuaAsynUtil::GetHttpContent(lua_State* pLuaState)
{
	LuaAsynUtil** ppAsynUtil = (LuaAsynUtil **)luaL_checkudata(pLuaState, 1, XMPTIPWND_ASYNCUTIL_CLASS);
	if (ppAsynUtil != NULL)
	{
		const char *pszUrlUTF8 = lua_tostring(pLuaState, 2);
		int bDel = lua_toboolean(pLuaState, 3);
		if (pszUrlUTF8 && lua_isfunction(pLuaState, 4))
		{
			CComBSTR bstrUrl;
			LuaStringToCComBSTR(pszUrlUTF8,bstrUrl);
			SGetHttpContentData *pData = new SGetHttpContentData(bstrUrl.m_str, pLuaState, luaL_ref(pLuaState, LUA_REGISTRYINDEX));
			_beginthreadex(NULL, 0, GetHttpContentProc, pData, 0, NULL);
		}
	}

	return 0;
}

//GetHttpFile
void SGetHttpFileData::Work()
{
	::CoInitialize(NULL);

	HRESULT hr = ::URLDownloadToFile(NULL, m_strUrl.c_str(), m_strSavePath.c_str(), 0, NULL);
	int nRetCode = -1;
	if (SUCCEEDED(hr))
	{
		nRetCode = 0;
	}
	g_wndMsg.PostMessage(WM_HTTPFILEGOT, nRetCode, (LPARAM) this);

	::CoUninitialize();
}

UINT WINAPI GetHttpFileProc(PVOID pArg)
{
	SGetHttpFileData* pData = (SGetHttpFileData*) pArg;
	pData->Work();
	return 0;
}

int LuaAsynUtil::GetHttpFile(lua_State* pLuaState)
{
	LuaAsynUtil** ppAsynUtil = (LuaAsynUtil **)luaL_checkudata(pLuaState, 1, XMPTIPWND_ASYNCUTIL_CLASS);
	if (ppAsynUtil != NULL)
	{
		const char *pszUrlUTF8 = lua_tostring(pLuaState, 2);
		const char *pszSavePathUTF8 = lua_tostring(pLuaState, 3);
		int bDel = lua_toboolean(pLuaState, 4);
		if (pszUrlUTF8 && pszSavePathUTF8 && lua_isfunction(pLuaState, 5))
		{
			CComBSTR bstrUrl,bstrSavePath;
			LuaStringToCComBSTR(pszUrlUTF8,bstrUrl);
			LuaStringToCComBSTR(pszSavePathUTF8,bstrSavePath);

			SGetHttpFileData *pData = new SGetHttpFileData(bstrUrl.m_str, bstrSavePath.m_str, pLuaState, luaL_ref(pLuaState, LUA_REGISTRYINDEX));
			_beginthreadex(NULL, 0, GetHttpFileProc, pData, 0, NULL);
		}
	}

	return 0;
}

//GetHttpFileWithProgress
void SGetHttpFileDataWithProgress::Work()
{
	::CoInitialize(NULL);
	TCallback tStatus(this);
	HRESULT hr = ::URLDownloadToFile(NULL, m_strUrl.c_str(), m_strSavePath.c_str(), 0, &tStatus);
	int nRetCode = -1;
	if (SUCCEEDED(hr))
	{
		nRetCode = 0;
	}
	g_wndMsg.PostMessage(WM_HTTPFILEGOT, nRetCode, (LPARAM) this);

	::CoUninitialize();
}

STDMETHODIMP TCallback::OnProgress(ULONG ulProgress, ULONG ulProgressMax,  ULONG ulStatusCode, LPCWSTR szStatusText)
{
	if (ulProgress > 0 && ulProgressMax > ulProgress)
	{
		double percent=(ulProgress*1.0f)/(ulProgressMax);
		//TSDEBUG(L"ulProgress = %ul,ulProgressMax = %ul, percent = %lf, m_percent = %lf",ulProgress,ulProgressMax,percent,m_percent);
		if (percent < m_percent+0.01f)
		{
			return S_OK;
		}
		m_percent = percent;
		PDOWNLOAD_PROGRESS pdp = new DOWNLOAD_PROGRESS;
		pdp->ulProgress = ulProgress;
		pdp->ulProgressMax = ulProgressMax;
		g_wndMsg.PostMessage(WM_HTTPFILEGOTPROGRESS, (WPARAM)pdp, (LPARAM)this->m_pGetHttpFileData);
	}
	return S_OK;
}

UINT WINAPI GetHttpFileWithProgressProc(PVOID pArg)
{
	SGetHttpFileDataWithProgress* pData = (SGetHttpFileDataWithProgress*) pArg;
	pData->Work();
	return 0;
}

int LuaAsynUtil::GetHttpFileWithProgress(lua_State* pLuaState)
{
	LuaAsynUtil** ppAsynUtil = (LuaAsynUtil **)luaL_checkudata(pLuaState, 1, XMPTIPWND_ASYNCUTIL_CLASS);
	if (ppAsynUtil != NULL)
	{
		const char *pszUrlUTF8 = lua_tostring(pLuaState, 2);
		const char *pszSavePathUTF8 = lua_tostring(pLuaState, 3);
		int bDel = lua_toboolean(pLuaState, 4);
		if (pszUrlUTF8 && pszSavePathUTF8 && lua_isfunction(pLuaState, 5))
		{
			CComBSTR bstrUrl,bstrSavePath;
			LuaStringToCComBSTR(pszUrlUTF8,bstrUrl);
			LuaStringToCComBSTR(pszSavePathUTF8,bstrSavePath);

			SGetHttpFileDataWithProgress *pData = new SGetHttpFileDataWithProgress(bstrUrl.m_str, bstrSavePath.m_str, pLuaState, luaL_ref(pLuaState, LUA_REGISTRYINDEX));
			_beginthreadex(NULL, 0, GetHttpFileWithProgressProc, pData, 0, NULL);
		}
	}

	return 0;
}

//SoftExit
int LuaAsynUtil::SoftExit(lua_State* pLuaState)
{
	if (g_wndMsg.m_hWnd != NULL)
	{
		g_wndMsg.DestroyWindow();
		g_wndMsg.m_hWnd = NULL;
	}
	theApp.ExitInstance();

	return 0;
}



int LuaAsynUtil::StartTimerEx(lua_State* pLuaState)
{
	UINT nElapse = lua_tointeger(pLuaState, 2);
	if (lua_isfunction(pLuaState, 3))
	{
		CTimerCallBackProcMgr* pData = new CTimerCallBackProcMgr(nElapse, pLuaState, luaL_ref(pLuaState, LUA_REGISTRYINDEX));
		UINT_PTR nTimerID = g_wndMsg.StartTimer(nElapse, pData);
		lua_pushinteger(pLuaState, nTimerID);
		return 1;
	}
	return 0;
}

int LuaAsynUtil::StopTimerEx(lua_State* pLuaState)
{
	UINT_PTR nTimerID = lua_tointeger(pLuaState, 2);
	g_wndMsg.StopTimer(nTimerID);
	return 0;
}

int LuaAsynUtil::StopAllTimerEx(lua_State* pLuaState)
{
	g_wndMsg.StopAllTimer();
	return 0;
}


//NewAsynGetHttpFile
CNewAsynGetHttpFileTaskData::CNewAsynGetHttpFileTaskData(CAsynCallbackTaskData *pCallbackData, const char* pUrl, const char* pFilePath, int iCoroutineRef) : 
	m_pCallbackData(pCallbackData), m_iCoroutineRef(iCoroutineRef)
{
	CComBSTR bstrUrl,bstrSavePath;
	LuaStringToCComBSTR(pUrl,bstrUrl);
	m_strUrl = bstrUrl.m_str;
	LuaStringToCComBSTR(pFilePath,bstrSavePath);
	m_strSavePath = bstrSavePath.m_str;
}

void CNewAsynGetHttpFileTaskData::Work()
{
	//try
	//{
		HRESULT hr = URLDownloadToFile(NULL, m_strUrl.c_str(), m_strSavePath.c_str(), 0, NULL);
		if (SUCCEEDED(hr))
		{
			g_wndMsg.PostMessage(WM_NEWASYNGETHTTPFILETASKFINISH, 0, (LPARAM)this);
		}
		else
		{
			g_wndMsg.PostMessage(WM_NEWASYNGETHTTPFILETASKFINISH, -1, (LPARAM)this);
		}
	//}
	//catch (...)
	//{
	//	g_wndMsg.PostMessage(WM_NEWASYNGETHTTPFILETASKFINISH, -1, (LPARAM)this);
	//}
}

UINT WINAPI NewAsynGetHttpFileTaskProc(PVOID pArg)
{
	CNewAsynGetHttpFileTaskData* pData = (CNewAsynGetHttpFileTaskData*) pArg;
	pData->Work();
	return 0;
}

int LuaAsynUtil::NewAsynGetHttpFile(lua_State* pLuaState)
{
	CAsynCallbackTaskData *pCallbackData = (CAsynCallbackTaskData*)lua_touserdata(pLuaState, 2);
	const char* pUrl = lua_tostring(pLuaState, 3);
	const char* pSavePath = lua_tostring(pLuaState, 4);
	lua_pushvalue(pLuaState, 5);
	int iCoroutineRef = luaL_ref(pLuaState, LUA_REGISTRYINDEX);
	TSDEBUG4CXX(L"***** NewAsynGetHttpFile luaL_ref = " << iCoroutineRef);
	if (pUrl != NULL && pSavePath != NULL)
	{
		CNewAsynGetHttpFileTaskData* pData = new CNewAsynGetHttpFileTaskData(pCallbackData, pUrl, pSavePath, iCoroutineRef);
		_beginthreadex(NULL, 0, NewAsynGetHttpFileTaskProc, pData, 0, NULL);
	}
	return 0;
}

//NewAsynSendHttpStat
NewAsynHttpStatData::NewAsynHttpStatData(CAsynCallbackTaskData *pCallbackData, const char* pUrl, int iCoroutineRef) : 
m_pCallbackData(pCallbackData), m_iCoroutineRef(iCoroutineRef)
{
	CComBSTR bstrUrl;
	LuaStringToCComBSTR(pUrl,bstrUrl);
	m_strUrl = bstrUrl.m_str;
}

void NewAsynHttpStatData::Work()
{
	//try
	//{
	TCHAR szPath[MAX_PATH] = {0};
	URLDownloadToCacheFile(NULL, m_strUrl.c_str(), szPath, MAX_PATH, 0, NULL);
	TSDEBUG4CXX(L"send WM_NEWASYNSENDHTTPSTATTASKFINISH");
	g_wndMsg.PostMessage(WM_NEWASYNSENDHTTPSTATTASKFINISH, 0, (LPARAM) this);
	//}
	//catch (...)
	//{
	//	g_wndMsg.PostMessage(WM_NEWASYNSENDHTTPSTATTASKFINISH, -1, (LPARAM) this);
	//}
}

UINT WINAPI NewAsynHttpStatTaskProc(PVOID pArg)
{
	NewAsynHttpStatData* pData = (NewAsynHttpStatData*) pArg;
	pData->Work();
	return 0;
}

int LuaAsynUtil::NewAsynSendHttpStat(lua_State* pLuaState)
{
	CAsynCallbackTaskData *pCallbackData = (CAsynCallbackTaskData*)lua_touserdata(pLuaState, 2);
	const char* pUrl = lua_tostring(pLuaState, 3);
	lua_pushvalue(pLuaState, 4);
	int iCoroutineRef = luaL_ref(pLuaState, LUA_REGISTRYINDEX);
	TSDEBUG4CXX(L"***** NewAsynSendHttpStat luaL_ref = " << iCoroutineRef);
	if (pUrl != NULL)
	{
		NewAsynHttpStatData* pData = new NewAsynHttpStatData(pCallbackData, pUrl, iCoroutineRef);
		_beginthreadex(NULL, 0, NewAsynHttpStatTaskProc, pData, 0, NULL);
	}
	return 0;
}




///////////////Ajax  OnStateChange实现部分
STDMETHODIMP LuaAsynUtil::OnStateChange(CAJAX* pSource, const bool bSucceeded, const int nHttpState, const BSTR /*bstrResponse*/,   IDispatch* pDispath)
{
	if(bSucceeded &&  (nHttpState >= 200 && nHttpState < 300 ))
	{
		if (pSource->m_bOutDOM)
		{
			g_wndMsg.PostMessage(WM_AJAXDOWNLOADFILESUCCESS, pSource->m_nID, (LPARAM)pSource);
		}
		else //存成文件
		{
			if ((AjaxTaskFlag)pSource->m_nID == AJAXTASKFLAG_SENDHTTPSTAT || (AjaxTaskFlag)pSource->m_nID == AJAXTASKFLAG_SENDHTTPSTATEX)
			{
				g_wndMsg.PostMessage(WM_AJAXDOWNLOADFILESUCCESS, pSource->m_nID, (LPARAM)pSource);
			}
			else
			{
				CComBSTR bstrSavedPath = pSource->m_bstrSavedPath;
				MSXML::IXMLHttpRequestPtr pRequest(pDispath);
				if ((AjaxTaskFlag)pSource->m_nID == AJAXTASKFLAG_GETHTTPCONTENT)
				{
					//try
					//{
					CComBSTR bstrContent;
					pRequest->get_responseText(&bstrContent);
					pSource->m_bstrSavedPath = L"";
					pSource->m_bstrSavedPath.AppendBSTR(bstrContent);
					g_wndMsg.PostMessage(WM_AJAXDOWNLOADFILESUCCESS, pSource->m_nID, (LPARAM)pSource);
					//}
					//catch (...)
					//{
					//	g_wndMsg.PostMessage(WM_AJAXDOWNLOADFILEFAILED, pSource->m_nID, (LPARAM)pSource);
					//}				
				}
				else if ((AjaxTaskFlag)pSource->m_nID == AJAXTASKFLAG_GETHTTPFILE)
				{
					CComVariant varStream;
					pRequest->get_responseStream(&varStream);

					if (varStream.vt != VT_UNKNOWN && varStream.vt != VT_STREAM)
					{
						g_wndMsg.PostMessage(WM_AJAXDOWNLOADFILEFAILED, pSource->m_nID, (LPARAM)pSource);
					}
					else
					{
						wchar_t szPath[_MAX_PATH] = {0};
						wcsncpy(szPath, bstrSavedPath, _MAX_PATH);
						PathRemoveFileSpec(szPath);
						if(!PathFileExists(szPath))
							SHCreateDirectory(NULL, szPath);
						//写文件
						HANDLE hFile = ::CreateFile((LPCTSTR)bstrSavedPath,GENERIC_WRITE,0,
							NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
						// 可能目录只读, 存到programdata/temp下
						if (hFile == INVALID_HANDLE_VALUE)
						{
							static CComBSTR bstrPath;
							if(bstrPath.Length() <= 0)
							{
								wchar_t szPath[MAX_PATH] = {0};
								DWORD len = GetTempPath(MAX_PATH, szPath);
								bstrPath = szPath;
							}
							if(!PathFileExists(bstrPath))
								SHCreateDirectory(NULL, bstrPath);
							CComBSTR bstrFileName = PathFindFileName(bstrSavedPath);
							CComBSTR bstrPath2(bstrPath);
							bstrPath2.AppendBSTR(bstrFileName);
							bstrSavedPath = bstrPath2;
							hFile = ::CreateFile((LPCTSTR)bstrSavedPath,GENERIC_WRITE,0,
								NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
						}
						if (hFile != INVALID_HANDLE_VALUE)
						{
							DWORD cb, cbRead, cbWritten;
							BYTE bBuffer[4096];
							cb = sizeof bBuffer;
							CComQIPtr<IStream> pStream(varStream.punkVal);
							HRESULT hr = pStream->Read(bBuffer, cb, &cbRead);
							while (SUCCEEDED(hr) && 0 != cbRead)
							{
								if (!WriteFile(hFile, bBuffer, cbRead, &cbWritten, NULL))
									break;
								hr = pStream->Read(bBuffer, cb, &cbRead);
							} 
						}
						CloseHandle(hFile);
						pSource->m_bstrSavedPath = bstrSavedPath;
						g_wndMsg.PostMessage(WM_AJAXDOWNLOADFILESUCCESS, pSource->m_nID, (LPARAM)pSource);
					}
				}
				else
				{
					g_wndMsg.PostMessage(WM_AJAXDOWNLOADFILESUCCESS, pSource->m_nID, (LPARAM)pSource);
				}
			}
		}
	}
	else
	{
		CComBSTR bstrSavedPath(L"");
		if(!pSource->m_bOutDOM)
		{
			bstrSavedPath = pSource->m_bstrSavedPath;
		}
		g_wndMsg.PostMessage(WM_AJAXDOWNLOADFILEFAILED, pSource->m_nID, (LPARAM)pSource);
	}
	return S_OK;
}



int LuaAsynUtil::AjaxSendHttpStat(lua_State* pLuaState)
{
	LuaAsynUtil** ppAsynUtil = (LuaAsynUtil **)luaL_checkudata(pLuaState, 1, XMPTIPWND_ASYNCUTIL_CLASS);
	if (ppAsynUtil != NULL)
	{
		const char* pUrl = lua_tostring(pLuaState, 2);
		if (pUrl != NULL && lua_isfunction(pLuaState, 3))
		{
			CComBSTR bstrUrl;
			LuaStringToCComBSTR(pUrl,bstrUrl);
			CAjaxTaskData *pAjaxTaskData = new CAjaxTaskData(pLuaState, luaL_ref(pLuaState, LUA_REGISTRYINDEX));
			if (pAjaxTaskData)
			{
				pAjaxTaskData->Open(bstrUrl, *ppAsynUtil, false, AJAXTASKFLAG_SENDHTTPSTAT);
			}
		}
	}
	return 0;
}

int LuaAsynUtil::AjaxGetHttpContent(lua_State* pLuaState)
{
	LuaAsynUtil** ppAsynUtil = (LuaAsynUtil **)luaL_checkudata(pLuaState, 1, XMPTIPWND_ASYNCUTIL_CLASS);
	if (ppAsynUtil != NULL)
	{
		const char* pUrl = lua_tostring(pLuaState, 2);
		if (pUrl != NULL && lua_isfunction(pLuaState, 3))
		{
			std::wstring strSavePath;

			CComBSTR bstrUrl;
			LuaStringToCComBSTR(pUrl,bstrUrl);

			CAjaxTaskData *pAjaxTaskData = new CAjaxTaskData(pLuaState, luaL_ref(pLuaState, LUA_REGISTRYINDEX));
			if (pAjaxTaskData)
			{
				CComBSTR bstrSavePath(strSavePath.c_str());
				pAjaxTaskData->Open(bstrUrl, *ppAsynUtil, false, AJAXTASKFLAG_GETHTTPCONTENT);
			}
		}
	}
	return 0;
}

int LuaAsynUtil::AjaxGetHttpFile(lua_State* pLuaState)
{
	LuaAsynUtil** ppAsynUtil = (LuaAsynUtil **)luaL_checkudata(pLuaState, 1, XMPTIPWND_ASYNCUTIL_CLASS);
	if (ppAsynUtil != NULL)
	{
		const char* pUrl = lua_tostring(pLuaState, 2);
		const char* pSavePath = lua_tostring(pLuaState, 3);
		if (pUrl != NULL && pSavePath != NULL && lua_isfunction(pLuaState, 4))
		{
			CComBSTR bstrUrl, bstrSavePath;
			LuaStringToCComBSTR(pUrl,bstrUrl);
			LuaStringToCComBSTR(pSavePath,bstrSavePath);

			CAjaxTaskData *pAjaxTaskData = new CAjaxTaskData(pLuaState, luaL_ref(pLuaState, LUA_REGISTRYINDEX));
			if (pAjaxTaskData)
			{
				pAjaxTaskData->Open(bstrUrl, *ppAsynUtil, false, AJAXTASKFLAG_GETHTTPFILE, bstrSavePath);
			}
		}
	}
	return 0;
}


CProcessParam::CProcessParam(const char* pExePath, const char* pParams, const char* pWorkDir, DWORD dwPriority, int nShow, lua_State* pState, LONG lRefFn) : m_callInfo(pState, lRefFn)
{
	if (pExePath != NULL)
	{
		CComBSTR bstrExePath;
		LuaStringToCComBSTR(pExePath,bstrExePath);
		m_strExePath = bstrExePath.m_str;
	}
	if (pParams != NULL)
	{
		CComBSTR bstrParams;
		LuaStringToCComBSTR(pParams,bstrParams);
		m_strParams = bstrParams.m_str;
	}
	if (pWorkDir != NULL)
	{
		CComBSTR bstrWorkDir;
		LuaStringToCComBSTR(pWorkDir,bstrWorkDir);
		m_strWorkDir = bstrWorkDir.m_str;
	}
	m_dwPriority = dwPriority;
	m_nShow = nShow;
	m_lRef = lRefFn;
}

void CProcessParam::Work()
{
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags |= STARTF_USESHOWWINDOW;
	si.wShowWindow = m_nShow;
	ZeroMemory(&m_pi, sizeof(m_pi));
	const wchar_t* pExePath = NULL;
	wchar_t* pParam = NULL;
	const wchar_t* pWorkDir = NULL;
	if (!m_strExePath.empty())
	{
		pExePath = m_strExePath.c_str();
	}
	if (!m_strParams.empty())
	{
		pParam = const_cast<wchar_t *>(m_strParams.c_str());
	}
	if (!m_strWorkDir.empty())
	{
		pWorkDir = m_strWorkDir.c_str();
	}
	BOOL bRet = CreateProcess(pExePath, pParam, NULL, NULL, FALSE, m_dwPriority, NULL, pWorkDir, &si, &m_pi);
	if (m_lRef != LUA_REFNIL)
	{
		if (bRet)
		{
			g_wndMsg.PostMessage(WM_CREATEPROCESSFINISH, 0, (LPARAM) this);
		}
		else
		{
			g_wndMsg.PostMessage(WM_CREATEPROCESSFINISH, GetLastError(), (LPARAM) this);
		}
	}
}

UINT WINAPI CreateProcessProc(PVOID pArg)
{
	CProcessParam* pData = (CProcessParam*) pArg;
	pData->Work();
	return 0;
}

int LuaAsynUtil::AsynCreateProcess(lua_State* pLuaState)
{
	const char* pExePath = lua_tostring(pLuaState, 2);
	const char* pParams = lua_tostring(pLuaState, 3);
	const char* pWorkDir = lua_tostring(pLuaState, 4);
	DWORD dwPriority = GetPriorityFromFlag((DWORD)lua_tonumber(pLuaState, 5));
	int nShow = lua_tonumber(pLuaState, 6);
	CProcessParam* pData = NULL;

	if (lua_isfunction(pLuaState, 7))
	{
		pData = new CProcessParam(pExePath, pParams, pWorkDir, dwPriority, nShow, pLuaState, luaL_ref(pLuaState, LUA_REGISTRYINDEX));
		_beginthreadex(NULL, 0, CreateProcessProc, pData, 0, NULL);
	}

	return 0;
}

void* __stdcall LuaAsynUtil::GetInstance( void* )
{
	static LuaAsynUtil s_instance;
	if (g_wndMsg.m_hWnd == NULL)
		g_wndMsg.Create(HWND_MESSAGE);
	return &s_instance;
}

void LuaAsynUtil::RegisterSelf( XL_LRT_ENV_HANDLE hEnv )
{
	if (hEnv == NULL)
	{
		return;
	}

	XLLRTObject theObject;
	theObject.ClassName = XMPTIPWND_ASYNCUTIL_CLASS;
	theObject.MemberFunctions = s_functionlist;
	theObject.ObjName = XMPTIPWND_ASYNCUTIL_OBJ;
	theObject.userData = NULL;
	theObject.pfnGetObject = (fnGetObject)LuaAsynUtil::GetInstance;

	XLLRT_RegisterGlobalObj(hEnv,theObject);
}