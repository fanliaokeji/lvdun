// xldl.dll导出函数的封装类

#pragma once

// 定义函数指针
typedef BOOL	(*fn_Init)(void);
typedef BOOL	(*fn_UnInit)(void);
typedef HANDLE  (*fn_TaskCreate)(DownTaskParam &stParam);
typedef BOOL    (*fn_TaskDelete)(HANDLE hTask);
typedef BOOL	(*fn_TaskStart) (HANDLE hTask);
typedef BOOL	(*fn_TaskPause) (HANDLE hTask);
typedef BOOL	(*fn_TaskForcePause) (HANDLE hTask);
typedef BOOL	(*fn_TaskQuery) (HANDLE hTask,DownTaskInfo &stTaskInfo);
typedef BOOL	(*fn_TaskQueryEx) (HANDLE hTask,DownTaskInfo &stTaskInfo);
typedef void	(*fn_LimitSpeed)(INT32 nKBps);
typedef void	(*fn_LimitUploadSpeed)(INT32 nTcpBps,INT32 nOtherBps);
typedef BOOL	(*fn_DelTempFile)(DownTaskParam &stParam);
typedef BOOL	(*fn_SetProxy)(DOWN_PROXY_INFO &stProxy);
typedef void    (*fn_SetUserAgent)( const TCHAR* pszUserAgent );
typedef BOOL	(*fn_GetFileSizeWithUrl)(const wchar_t * lpURL, INT64& iFileSize);
typedef BOOL    (*fn_ParseThunderPrivateUrl)(const TCHAR *pszThunderUrl, TCHAR *normalUrlBuffer, INT32 bufferLen);
typedef LONG	(*fn_SetAdditionInfo)( HANDLE task_id, WSAPROTOCOL_INFOW *sock_info, CHAR *http_resp_buf, LONG buf_len );
typedef BOOL	(*fn_SetFileIdAndSize)(HANDLE hTask, char szFileId[40], unsigned __int64 nFileSize);

//#define CHECKFUNC(f, ret) if (f == NULL) return ret;
#define CHECKFUNC(f) if (f == NULL) return FALSE;

class DownWrapper
{
public:

	DownWrapper()
	{
		
	}

	virtual ~DownWrapper(void)
	{
		if (m_hModule)
		{
			::FreeLibrary(m_hModule);
		}
	}
	BOOL LoadXLDL(LPCWSTR sPath)
	{
		m_hModule = ::LoadLibraryW(sPath);
		if (m_hModule == NULL)
		{
			return FALSE;
		}
		_Init					= (fn_Init)						::GetProcAddress(m_hModule, "XL_Init");
		CHECKFUNC(_Init); 
		_UnInit					= (fn_UnInit)					::GetProcAddress(m_hModule, "XL_UnInit");
		CHECKFUNC(_UnInit);
		_TaskCreate				= (fn_TaskCreate)				::GetProcAddress(m_hModule, "XL_CreateTask");
		CHECKFUNC(_TaskCreate); 
		_TaskDelete				= (fn_TaskDelete)				::GetProcAddress(m_hModule, "XL_DeleteTask");
		CHECKFUNC(_TaskDelete); 
		_TaskStart				= (fn_TaskStart)				::GetProcAddress(m_hModule, "XL_StartTask");
		CHECKFUNC(_TaskStart); 
		_TaskPause				= (fn_TaskPause)				::GetProcAddress(m_hModule, "XL_StopTask");
		CHECKFUNC(_TaskPause);
		_TaskForcePause			= (fn_TaskForcePause)			::GetProcAddress(m_hModule, "XL_ForceStopTask");
		CHECKFUNC(_TaskForcePause);
		_TaskQuery				= (fn_TaskQuery)				::GetProcAddress(m_hModule, "XL_QueryTaskInfo");
		CHECKFUNC(_TaskQuery); 
		_TaskQueryEx			= (fn_TaskQueryEx)				::GetProcAddress(m_hModule, "XL_QueryTaskInfoEx");
		CHECKFUNC(_TaskQueryEx); 
		_LimitSpeed				= (fn_LimitSpeed)				::GetProcAddress(m_hModule, "XL_SetSpeedLimit");
		CHECKFUNC(_LimitSpeed); 
		_LimitUploadSpeed		= (fn_LimitUploadSpeed)			::GetProcAddress(m_hModule, "XL_SetUploadSpeedLimit");
		CHECKFUNC(_LimitUploadSpeed);
		_DelTempFile			= (fn_DelTempFile)				::GetProcAddress(m_hModule, "XL_DelTempFile");
		CHECKFUNC(_DelTempFile);
		_SetProxy				= (fn_SetProxy)					::GetProcAddress(m_hModule, "XL_SetProxy");
		CHECKFUNC(_SetProxy);
		_SetUserAgent			= (fn_SetUserAgent)				::GetProcAddress(m_hModule, "XL_SetUserAgent");
		CHECKFUNC(_SetUserAgent);
		_GetFileSizeWithUrl		= (fn_GetFileSizeWithUrl)		::GetProcAddress(m_hModule, "XL_GetFileSizeWithUrl");
		CHECKFUNC(_GetFileSizeWithUrl);
		_ParseThunderPrivateUrl = (fn_ParseThunderPrivateUrl)	::GetProcAddress(m_hModule, "XL_ParseThunderPrivateUrl");
		CHECKFUNC(_ParseThunderPrivateUrl);
		_SetAdditionInfo		= (fn_SetAdditionInfo)			::GetProcAddress(m_hModule, "XL_SetAdditionInfo");
		CHECKFUNC(_SetAdditionInfo);
		_SetFileIdAndSize		= (fn_SetFileIdAndSize)			::GetProcAddress(m_hModule, "XL_SetFileIdAndSize");
		CHECKFUNC(_SetFileIdAndSize);
		return TRUE;
	}
	BOOL Init()
	{return _Init();}
	BOOL UnInit()
	{return _UnInit();}
	HANDLE TaskCreate(DownTaskParam &param)
	{return _TaskCreate(param);}
	BOOL TaskDelete(HANDLE hTask)
	{return _TaskDelete(hTask);}
	BOOL TaskStart(HANDLE hTask)
	{return _TaskStart(hTask);}
	BOOL TaskPause(HANDLE hTask)
	{return _TaskPause(hTask);}
	BOOL TaskForcePause(HANDLE hTask)
	{return _TaskForcePause(hTask);}
	BOOL TaskQuery(HANDLE hTask, DownTaskInfo & stTaskInfo)
	{return _TaskQuery(hTask,stTaskInfo);}
	BOOL TaskQueryEx(HANDLE hTask, DownTaskInfo & stTaskInfo)
	{return _TaskQueryEx(hTask,stTaskInfo);}
	void LimitSpeed(INT32 nBps)
	{_LimitSpeed(nBps);}
	void LimitUploadSpeed(INT32 nTcpBps,INT32 nOtherBps)
	{_LimitUploadSpeed(nTcpBps, nOtherBps);}
	BOOL DelTempFile(DownTaskParam &stParam)
	{return _DelTempFile(stParam);}
	BOOL SetProxy(DOWN_PROXY_INFO &stProxy)
	{return _SetProxy(stProxy);}
	void SetUserAgent(const TCHAR *pszUserAgent)
	{_SetUserAgent(pszUserAgent);}
	BOOL GetFileSizeWithUrl(const wchar_t * lpURL, INT64& iFileSize)
	{return _GetFileSizeWithUrl(lpURL, iFileSize);}
	BOOL ParseThunderPrivateUrl(const TCHAR *pszThunderUrl, TCHAR *normalUrlBuffer, INT32 bufferLen)
	{return _ParseThunderPrivateUrl(pszThunderUrl, normalUrlBuffer, bufferLen);}
	LONG SetAdditionInfo(HANDLE task_id, WSAPROTOCOL_INFOW *sock_info, CHAR *http_resp_buf, LONG buf_len)
	{return _SetAdditionInfo(task_id, sock_info, http_resp_buf, buf_len);}
	LONG SetFileIdAndSize(HANDLE hTask, char szFileId[40], unsigned __int64 nFileSize)
	{return _SetFileIdAndSize(hTask, szFileId, nFileSize);}

private:
	HMODULE						m_hModule;

	fn_Init						_Init;
	fn_UnInit					_UnInit;
	fn_TaskCreate				_TaskCreate;
	fn_TaskDelete				_TaskDelete;
	fn_TaskStart				_TaskStart;
	fn_TaskPause				_TaskPause;
	fn_TaskForcePause           _TaskForcePause;
	fn_TaskQuery				_TaskQuery;
	fn_TaskQueryEx				_TaskQueryEx;
	fn_LimitSpeed				_LimitSpeed;
	fn_LimitUploadSpeed			_LimitUploadSpeed;
	fn_DelTempFile				_DelTempFile;
	fn_SetProxy					_SetProxy;
	fn_SetUserAgent				_SetUserAgent;
	fn_GetFileSizeWithUrl		_GetFileSizeWithUrl;
	fn_ParseThunderPrivateUrl	_ParseThunderPrivateUrl;
	fn_SetAdditionInfo			_SetAdditionInfo;
	fn_SetFileIdAndSize			_SetFileIdAndSize;
};

