#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <Sddl.h>
#include <TlHelp32.h>
#include <cstring>

#include "Utility.h"
#include "ProcessDetect.h"
#include "ScopeResourceHandle.h"
#include "LaunchGreenShieldConfig.h"

static const wchar_t* szServiceName = L"GreenShieldService";
#define SVC_ERROR	((DWORD)0xC0020001L)

SERVICE_STATUS          gSvcStatus; 
SERVICE_STATUS_HANDLE   gSvcStatusHandle = NULL; 
HANDLE                  ghSvcStopEvent = NULL;
void WINAPI SvcCtrlHandler(DWORD dwCtrl);
VOID SvcInit(DWORD dwArgc, LPTSTR *lpszArgv);
VOID ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);
VOID SvcReportEvent(LPTSTR szFunction);

void ServiceMain(int argc, wchar_t *argv[])
{
	gSvcStatusHandle = RegisterServiceCtrlHandler(szServiceName, SvcCtrlHandler);

	if(!gSvcStatusHandle) {
		SvcReportEvent(L"RegisterServiceCtrlHandler");
		return; 
    }

	gSvcStatus.dwServiceType = SERVICE_WIN32; 
	gSvcStatus.dwServiceSpecificExitCode = 0;

	ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	SvcInit(argc, argv);
}

void WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
	switch(dwCtrl) {  
	  case SERVICE_CONTROL_STOP: 
		 ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

		 SetEvent(ghSvcStopEvent);
		 ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);
		 return;
	  case SERVICE_CONTROL_INTERROGATE:
		 break;
	  default:
		 break;
	}
}

VOID SvcInit(DWORD dwArgc, LPTSTR *lpszArgv)
{
	ghSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (ghSvcStopEvent == NULL) {
        ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
        return;
    }

    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);

	HANDLE hMutex = NULL;
	
	LaunchGreenShieldConfig launchGreenShieldCfg;
	if(launchGreenShieldCfg.UpdateConfig()) {
		if(launchGreenShieldCfg.Valid()) {
		TSINFO4CXX("Load Config IsNoRemind: "
			<< launchGreenShieldCfg.IsNoRemind()
			<< ", noremindspanday: " 
			<< launchGreenShieldCfg.GetNoRemindSpanDay()
			<< ", intervaltime: "
			<< launchGreenShieldCfg.GetLaunchInterval()
			<< ", maxcntperday: "
			<< launchGreenShieldCfg.GetMaxCntPerDay()
			<< ", lastpull: "
			<< launchGreenShieldCfg.GetLastPull()
			<< ", cnt: "
			<< launchGreenShieldCfg.GetCnt());
		}
	}
	else {
		TSERROR4CXX(L"Load Config failed");
	}

    while(1) {
        DWORD dwTimeToWait = 1000;
		if(hMutex == NULL) {
			if(::IsVistaOrLatter()) {
				SECURITY_ATTRIBUTES sa;
				char sd[SECURITY_DESCRIPTOR_MIN_LENGTH];
				sa.nLength = sizeof(sa);
				sa.bInheritHandle = FALSE;
				sa.lpSecurityDescriptor = &sd;
				if(::InitializeSecurityDescriptor(sa.lpSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION)) {
					if(::SetSecurityDescriptorDacl(sa.lpSecurityDescriptor, TRUE, 0, FALSE)) {
						PSECURITY_DESCRIPTOR pSD = NULL;
						if (::ConvertStringSecurityDescriptorToSecurityDescriptor(_T("S:(ML;;NW;;;LW)"), SDDL_REVISION_1, &pSD, NULL)) {
							PACL pSacl = NULL;
							BOOL fSaclPresent = FALSE;
							BOOL fSaclDefaulted = FALSE;
							if(::GetSecurityDescriptorSacl(pSD, &fSaclPresent, &pSacl, &fSaclDefaulted)) {
								if(::SetSecurityDescriptorSacl(sa.lpSecurityDescriptor, TRUE, pSacl, FALSE)) {
									hMutex = ::CreateMutex(&sa, TRUE, L"Global\\{66CC0177-5EC0-4ce5-9BAF-F75038328275}-gsaddin");
									if(hMutex != NULL && ::GetLastError() == ERROR_ALREADY_EXISTS) {
										::CloseHandle(hMutex);
										hMutex = NULL;
									}
								}
								::LocalFree(pSacl);
							}
							::LocalFree(pSD);
						}
					}
				}
			}
			else {
				hMutex = ::CreateMutex(NULL, TRUE, L"Global\\{66CC0177-5EC0-4ce5-9BAF-F75038328275}-gsaddin");
				if(hMutex != NULL && ::GetLastError() == ERROR_ALREADY_EXISTS) {
					::CloseHandle(hMutex);
					hMutex = NULL;
				}
			}
		}

		// 判断是否更新配置 每小时更新一次配置
		__time64_t nCurrentTime = 0;
		_time64(&nCurrentTime);
		const tm* currentTime = _gmtime64(&nCurrentTime);
		int curYear = currentTime->tm_year + 1900;
		int curMonth = currentTime->tm_mon + 1;
		int curDay = currentTime->tm_mday;
		int curHour = currentTime->tm_hour;
		
		__time64_t nlastUpdateTime = launchGreenShieldCfg.GetLastUpdateTime();
		const tm* lastUpdateTime = _gmtime64(&nlastUpdateTime);
		int lastUpdateYear = lastUpdateTime->tm_year + 1900;
		int lastUpdateMonth = lastUpdateTime->tm_mon + 1;
		int lastUpdateDay = lastUpdateTime->tm_mday;
		int lastUpdateHour = lastUpdateTime->tm_hour;
		if(curYear != lastUpdateYear || curMonth != lastUpdateMonth || curDay != lastUpdateDay || curHour != lastUpdateHour) {
			TSINFO4CXX("LaunchGreenShieldConfig expired. Try update");
			if(launchGreenShieldCfg.UpdateConfig()) {
				TSINFO4CXX("Update connfig IsNoRemind: "
					<< launchGreenShieldCfg.IsNoRemind()
					<< ", noremindspanday: " 
					<< launchGreenShieldCfg.GetNoRemindSpanDay()
					<< ", intervaltime: "
					<< launchGreenShieldCfg.GetLaunchInterval()
					<< ", maxcntperday: "
					<< launchGreenShieldCfg.GetMaxCntPerDay()
					<< ", lastpull: "
					<< launchGreenShieldCfg.GetLastPull()
					<< ", cnt: "
					<< launchGreenShieldCfg.GetCnt());
			}
			else {
				TSERROR4CXX(L"Update config failed");
			}
		}

		if(hMutex == NULL || ProcessDetect::IsGreenShieldOrGreenShieldSetupRunning()) {
			dwTimeToWait = 20 * 1000;
		}
		else if(launchGreenShieldCfg.Valid() && launchGreenShieldCfg.IsEnableLaunchNow() && ProcessDetect::IsAnyBrowerRunning()) {
			do {
				HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
				if(hProcessSnap == INVALID_HANDLE_VALUE) {
					break;
				}
				::ScopeResourceHandle<HANDLE, BOOL (WINAPI*)(HANDLE)> autoCloseProcessSnap(hProcessSnap, ::CloseHandle);

				PROCESSENTRY32 pe32;
				pe32.dwSize = sizeof(PROCESSENTRY32);

				if(!Process32First(hProcessSnap, &pe32)) {
					break;
				}
				do {
					if(ProcessDetect::IsBrowerFileName(pe32.szExeFile)) {
						
						if(pe32.th32ProcessID == 0 || pe32.th32ProcessID == 4) {
							// Idle or system
							continue;
						}
						if(launchGreenShieldCfg.CheckEnableLaunchNow()) {
							if(!::LaunchGreenShield(pe32.th32ProcessID)) {
								dwTimeToWait = 5 * 60 * 1000;
							}
						}
						break;
					}
				} while(Process32Next(hProcessSnap, &pe32));
			} while(false);
		}

		DWORD waitRet = ::WaitForSingleObject(ghSvcStopEvent, dwTimeToWait);
		if(waitRet == WAIT_FAILED) {
			break;
		}
		else if(waitRet == WAIT_OBJECT_0) {
			break;
		}
    }
	::CloseHandle(hMutex);
	ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
}

VOID ReportSvcStatus( DWORD dwCurrentState,
                      DWORD dwWin32ExitCode,
                      DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

    gSvcStatus.dwCurrentState = dwCurrentState;
    gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
    gSvcStatus.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
        gSvcStatus.dwControlsAccepted = 0;
    else gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED)) {
		gSvcStatus.dwCheckPoint = 0;
	}
	else {
		gSvcStatus.dwCheckPoint = dwCheckPoint++;	
	}

    SetServiceStatus(gSvcStatusHandle, &gSvcStatus );
}

VOID SvcReportEvent(LPTSTR szFunction) 
{ 
    HANDLE hEventSource;
    LPCTSTR lpszStrings[2];
    TCHAR Buffer[80];

    hEventSource = RegisterEventSource(NULL, szServiceName);

    if(NULL != hEventSource) {
        StringCchPrintf(Buffer, 80, TEXT("%s failed with %d"), szFunction, GetLastError());

		lpszStrings[0] = szServiceName;
        lpszStrings[1] = Buffer;

        ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, SVC_ERROR, NULL, 2, 0, lpszStrings, NULL);
		DeregisterEventSource(hEventSource);
    }
}
