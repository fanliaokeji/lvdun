#include "StdAfx.h"
//#include "..\define.h"
#include "GetExifOP.h"
#include "..\Utility\StringOperation.h"
#include <fstream>

CGetExifOP::CGetExifOP(void)
{
	m_OperationType = KKImg_OperationType_GetExif;
	m_wstrOperationDes = L"获取Exif";
}

CGetExifOP::~CGetExifOP(void)
{
	TSAUTO();
	TSINFO4CXX("CGetExifOP Id is --" << m_nOperationId);
}

bool CGetExifOP::SetParam(lua_State* luaState)
{
	const char* utf8Text = luaL_checkstring(luaState, 2);
	m_wstrFilePath = ultra::_UTF2T(utf8Text);
	return true;
}

int CGetExifOP::Run(COperationTask* pCOperationTask)
{
	TSAUTO();
	TSINFO4CXX("CLoadBitmapOP Run CurId is " << m_nOperationId);
	int nRet = 0;
	wstring wstrTempDir = g_pPathHelper->GetTempDir();
	wstring wstrAppDir = g_pPathHelper->GetExeDir();
	wstring wstrExifToolPath = wstrAppDir + L"exiftool.exe";
	wstring wstrRunParam = L"-j -fast -q ";
	wstrRunParam += L" -w! ";
	wstrRunParam += wstrTempDir + L"%f.%e.json \"" + m_wstrFilePath + L"\"";
	


	DWORD nStartTime = GetTickCount();

	SHELLEXECUTEINFO shellinfo;
	ZeroMemory(&shellinfo,sizeof(SHELLEXECUTEINFO));
	shellinfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shellinfo.hwnd = NULL;
	shellinfo.lpVerb = L"open";
	shellinfo.lpFile = wstrExifToolPath.c_str();
	shellinfo.nShow = SW_HIDE;
	shellinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shellinfo.lpParameters = wstrRunParam.c_str();
	
	DWORD dwErr;
	if(!ShellExecuteExW(&shellinfo) || shellinfo.hProcess == NULL)
	{
		nRet = 1;
		dwErr = GetLastError();
		TSDEBUG4CXX("shellexecuteex failed"<<dwErr);	

	}
	else
	{
		WaitForSingleObject(shellinfo.hProcess,10000);
		CloseHandle(shellinfo.hProcess);

		TSDEBUG4CXX("run result:"<<shellinfo.hInstApp);
		wstring wstrOutPutFilePath = wstrTempDir + PathFindFileName(m_wstrFilePath.c_str()) + L".json";
		// 直接在这里读取
		if (::PathFileExists(wstrOutPutFilePath.c_str()))
		{
			ifstream in(wstrOutPutFilePath.c_str());
			string s;
			while (getline(in, s))
			{
				m_strJosn = m_strJosn + s;
			}
		}
		DeleteFile(wstrOutPutFilePath.c_str());
	}
	DWORD nEndTime = GetTickCount();
	TSINFO4CXX("GetExif Consum " << nEndTime-nStartTime);
	return nRet;
}

bool CGetExifOP::OnOperationComplete(int m_nStatus, int m_nErrorCode)
{
	ILuaEventEnum* pEventEnum = NULL;
	m_EventContainer.GetEventEnum(_T("OnOperationComplete"), pEventEnum);
	if (pEventEnum )
	{
		CLuaEvent* pEvent = NULL;
		pEventEnum->Reset();
		while (S_OK == pEventEnum->Next(pEvent))
		{
			lua_State* luaState = pEvent->GetLuaState();
			int nowTop = lua_gettop(luaState);

			pEvent->PushFunction();
			if (KKIMG_TASKSTATUS_SUCCESS != m_nStatus)	// 任务执行失败
			{
				lua_pushnil(luaState);
				lua_pushinteger(luaState, m_nErrorCode);
			}
			else	// 成功
			{
				lua_pushstring(luaState, m_strJosn.c_str());
				lua_pushinteger(luaState, m_nErrorCode);
			}
			pEvent->Call(2, 0);
			lua_settop(luaState, nowTop);
		}
	}
	return true;
}