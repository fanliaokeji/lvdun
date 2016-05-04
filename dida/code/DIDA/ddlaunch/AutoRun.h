#pragma once
/************************************************************************/
// Copyright (c)  Shenzhen ZhongWeiTai Technology Co. Ltd. 2014-2015 
//-----------------------------------------------------------------------/
// # Meta：
// File   : AutoRun.h
// Authoer: 嘀嗒
// Date   : 2015/8/18
//
/************************************************************************/
// 说明：
//     1.设置开机启动
//     2.具体产品根据实际情况修改下面宏定义
//


/************************************************************************/
/* 根据相关产品定义对应常量                                             */
/************************************************************************/

#define AR_WND_CALSSNAME _T("{2035EE07-AFB4-44cf-9D8E-534D2882F620}_msg_ar")

#define AR_WND_MUTEX _T("{2035EE07-AFB4-44cf-9D8E-534D2882F621}_ar")

#define AR_CLIENT_HKCU _T("SOFTWARE\\mycalendar")

#define AR_CLIENT_HKLM _T("SOFTWARE\\mycalendar")

#define AR_CLIENT_SERVER _T("http://dl.tie7.com/update/1.0/NewVerInfo.dat")

#define AR_LAUNCH_EXE _T("myfixar.exe")

#define AR_COMMANDLINE _T("/embedding /sstartfrom sysboot")

#define AR_MAX_FAIL 3
#define AR_TIMER_INTERVAL 10*1000
#define AR_MAX_SETBOOT 2
/************************************************************************/



#include <atlwin.h>
#include <atlcrack.h>
#include <string>

#define WM_EXIT WM_USER + 101


class CAutoRun :public  CWindowImpl<CAutoRun>
{
public:
	static CAutoRun* Instance()
	{
		static CAutoRun s;
		return &s;
	}
	DECLARE_WND_CLASS(AR_WND_CALSSNAME)
	BEGIN_MSG_MAP(CAutoRun)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER(WM_COPYDATA, OnCopyData)
		MESSAGE_HANDLER(WM_EXIT, OnExit)
		//MESSAGE_HANDLER(WM_REGNOTIFY, OnRegNotify)
	END_MSG_MAP()
public:
	LRESULT OnCopyData(UINT , WPARAM , LPARAM , BOOL&);
	LRESULT OnExit(UINT , WPARAM , LPARAM , BOOL&);
	void OnTimer(UINT_PTR nTimerID);
public:
	CAutoRun(void);
	~CAutoRun(void);
public:
	BOOL Init();
private:
	BOOL HandleSingleton();
	BOOL GetServerCfg();
	BOOL HttpDownloadString(const std::wstring& strUrl, std::string &strFileData); 
	BOOL CheckClientCfg();
	BOOL LaunchMainClient();
	BOOL CheckSetBoot();

	std::wstring CreateRandomName(UINT ulen,const std::wstring& wstrNotMatch);
	void GetExePathDetailInfo(const std::wstring& wstrPath,std::wstring& wstrDir,std::wstring& wstrDirName,std::wstring& wstrFileNameWithOutExt);
	BOOL DeleteFileRecurse(const std::wstring& wstrPath);
	BOOL CreateAutoRun(BOOL bNewRun,BOOL bForce = FALSE);
	void UpdateLaunchFile(const std::wstring& wstrSrcFile,const std::wstring& wstrDestFile);
	BOOL DeleteAutoRun();
	BOOL CheckFixCnt();
private:
	HANDLE m_hMutex;
	std::wstring m_wstrMainPath;
	std::wstring m_wstrMainDir;
	std::wstring m_wstrMainExeName;
	std::wstring m_wstrLaunchPath;
	std::wstring m_wstrLaunchNameWithOutExt;
	std::wstring m_wstrAutoRunCommandLine;
	UINT m_uTimerFail;
	UINT_PTR m_uTimerID;
	BOOL m_bUpdated;

};


unsigned __int64 GetFileVersion(const TCHAR* file_path, unsigned __int64 * VerionTimeStamp);
int VerCmp(unsigned __int64 ver1, unsigned __int64 ver2);
bool IsVistaOrLatter();
std::wstring GetAllUsersPublicPath();
BOOL CheckProcessExist(std::wstring wstrProcessName, BOOL bQuit = FALSE);
std::wstring GetRealPath(const std::wstring &wstrPath);
BOOL CheckIsAnotherDay(__time64_t nLastTime);

void LaunchUpdateDiDA();