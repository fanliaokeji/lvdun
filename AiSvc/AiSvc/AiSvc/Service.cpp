#include "stdafx.h"
#include "Service.h"
#include "ultra/file-op.h"
#include "xlog.h"
#include "CSVCreateProcess.h"
CService::CService()
{

}

CService::~CService()
{

}

int CService::IsInstalledService( std::wstring wstrService )
{
	int bRet = 0;
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager != NULL)
	{
		SC_HANDLE hService = ::OpenService(hSCManager, wstrService.c_str(), SERVICE_QUERY_CONFIG);
		if (hService != NULL)
		{
			LPQUERY_SERVICE_CONFIG sc;  // 详细信息
			// 查询其配置信息
			DWORD nRet = 0;
			sc = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, 4 * 1024);
			::QueryServiceConfig(hService, sc, 4 * 1024, &nRet);
			if(sc->dwStartType == SERVICE_AUTO_START)
			{
				bRet = 1;	// 自动
			}
			else if(sc->dwStartType == SERVICE_DEMAND_START)
			{
				bRet = 2;	// 手动
			}
			else if(sc->dwStartType == SERVICE_DISABLED)
			{
				bRet = 3;	// 禁用
			}
			else
			{
				bRet = 4;	// 停止
			}
			CloseServiceHandle(hService);
		}
	}
	return bRet;
}

bool CService::InstallService( std::wstring wstrDir, std::wstring wstrService, bool bStart )
{

	xlog("InstallService");
	//SC_HANDLE hService;
	//SC_HANDLE hSCManager;
	//hSCManager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
	//if (!hSCManager)
	//{
	//	xlog("false1");
	//	return false;
	//}

	//hService = CreateService(hSCManager, wstrService.c_str(), wstrService.c_str(), SERVICE_ALL_ACCESS,
	//	SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, wstrDir.c_str(),
	//	0, 0, L"RPCSS\0", 0, 0);

	//if (!hService)
	//{
	//	xlog("false2");
	//	CloseServiceHandle(hSCManager);
	//	return false;
	//}
	////StartService(hService, NULL, NULL);

	//CloseServiceHandle(hService);
	//CloseServiceHandle(hSCManager);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	WCHAR szCmd[MAX_PATH] = {0};
	wcscpy(szCmd, (wstrDir + wstrService +L".exe /service").c_str());
	// 创建子进程
	if( !CreateProcess( NULL, // No module name (use command line). 
		szCmd, // Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		0,                // No creation flags. 
		NULL,             // Use parent's environment block. 
		NULL,             // Use parent's starting directory. 
		&si,              // Pointer to STARTUPINFO structure.
		&pi )             // Pointer to PROCESS_INFORMATION structure.
		) 
	{
		xlogL(L"createprocess false %s %lu",szCmd,GetLastError());
		return false;
	}

	WaitForSingleObject( pi.hProcess, INFINITE );
	xlogL(L"register service finish");
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	return true;
}


bool CService::UnInstallService( std::wstring wstrService )
{
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		return false;
	}

	SC_HANDLE hService = OpenService(hSCManager, wstrService.c_str(), SERVICE_STOP|DELETE);
	if (hService == NULL)
	{
		CloseServiceHandle(hSCManager);
		return false;
	}

	SERVICE_STATUS status;
	ControlService(hService, SERVICE_CONTROL_STOP, &status);

	// 删除服务
	bool bDel = DeleteService(hService);
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	return bDel;
}

bool CService::StartService(std::wstring wstrService)
{
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		xlogL(L"StartService OpenSCManager fail");
		return false;
	}

	SC_HANDLE hService = OpenService(hSCManager, wstrService.c_str(), SERVICE_ALL_ACCESS);
	if (hService == NULL)
	{
		xlogL(L"StartService OpenService fail");
		CloseServiceHandle(hSCManager);
		return false;
	}
	::ChangeServiceConfig(hService, SERVICE_NO_CHANGE,
		SERVICE_AUTO_START,// 修改服务为自动启动
		NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		wstrService.c_str()); 
	bool bRet = ::StartService(hService, NULL, NULL);
	int Err=  GetLastError();

	xlogL(L"StartService StartService bRet = %d, error = %lu",bRet,Err);
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	return bRet;
}

bool CService::RunService()
{
#ifdef UNREGSVR_NAME_L
	//UnInstallService(UNREGSVR_NAME_L);
#endif
	std::wstring dir = ultra::ExpandEnvironment(SERVICE_DIR_L);
	std::wstring exe = EXE_NAME_L;

	//ultra::CreateDirectoryR(dir);
	//WCHAR szFiles[][MAX_PATH] = {L"Microsoft.VC90.ATL.manifest", L"Microsoft.VC90.CRT.manifest", L"msvcr90.dll",L"msvcp90.dll", L"ATL90.dll", DLL_NAME_L, EXE_NAME_L};
	//for (int i = 0; i < 7; ++i)
	//{
	//	CopyFile((ultra::GetModuleFilePath()+szFiles[i]).c_str(), (dir + szFiles[i]).c_str(), FALSE);
	//}

	if (IsInstalledService(SERVICE_NAME_L))
	{
		StartService(SERVICE_NAME_L);
	}
	else
	{
		InstallService(ultra::ExpandEnvironment(SERVICE_DIR_L), SERVICE_NAME_L, true);
		xlogL(L"try to service");
		StartService(SERVICE_NAME_L);
		
	}
	return true;
}
