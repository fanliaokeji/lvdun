// AiSvc.cpp : WinMain 的实现


#include "stdafx.h"
#include "resource.h"
#include "AiSvc_i.h"
#include "Service.h"
#include "ultra/file-op.h"

#include "CSVCreateProcess.h"
#define SVC_MUTEX L"Global\\WinTSvc-{90BF588B-343D-4DFE-A7BB-64947AF3E5BD}"
#define SVC_MUTEX_L L"Global\\WinTSvc1-{90BF588B-343D-4DFE-A7BB-64947AF3E5BD}"

class CAiSvcModule : public ATL::CAtlServiceModuleT< CAiSvcModule, IDS_SERVICENAME >
{
public :
	DECLARE_LIBID(LIBID_AiSvcLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_AISVC, "{90BF588B-343D-4DFE-A7BB-64947AF3E5BD}")
	HRESULT InitializeSecurity() throw()  
	{  
		// TODO : 调用 CoInitializeSecurity 并为服务提供适当的安全设置  
		// 建议 - PKT 级别的身份验证、  
		// RPC_C_IMP_LEVEL_IDENTIFY 的模拟级别  
		// 以及适当的非 null 安全说明符。  

		//return S_OK;  
		return CoInitializeSecurity(NULL,-1,NULL,NULL,  
			RPC_C_AUTHN_LEVEL_NONE,  
			RPC_C_IMP_LEVEL_IDENTIFY,  
			NULL,EOAC_NONE,NULL);//创建安全说明符  
	}  

	//这里要对一些函数进行重写  

	HRESULT Start(int nShowCmd) throw()
	{
		SetServiceStatus(SERVICE_RUNNING);
		return __super::Start(nShowCmd);
	}

	//初始化代码
	HRESULT PreMessageLoop(int nShowCmd) throw()
	{
		SetServiceStatus(SERVICE_RUNNING);
		HRESULT hr = __super::PreMessageLoop(nShowCmd);
		
		// 微软的BUG
		if (hr == S_FALSE) hr = S_OK; 
		if (SUCCEEDED(hr))
		{
		}
		return hr;
	}

	void RunMessageLoop() throw()
	{
		MSG msg;
		while (GetMessage(&msg, 0, 0, 0) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	HRESULT Run(int nShowCmd = SW_HIDE) throw()
	{
		SetServiceStatus(SERVICE_RUNNING);
		if (CSVCreateProcess::Go() == false)
		{
			OnStop();
			return S_FALSE;
		}
		return __super::Run();
	}

	void OnStop() throw()

	{
		__super::OnStop();
	}

	HRESULT RegisterAppId(bool bService = false)
	{
		HRESULT hr = S_OK;
		BOOL res = __super::RegisterAppId(bService); 
		if (bService)
		{
			if (IsInstalled())
			{
				SC_HANDLE hSCM = ::OpenSCManagerW(NULL, NULL, SERVICE_CHANGE_CONFIG);
				SC_HANDLE hService = NULL;
				if (hSCM == NULL)
				{
					hr = ATL::AtlHresultFromLastError();
				}
				else
				{
					hService = ::OpenService(hSCM, m_szServiceName, SERVICE_CHANGE_CONFIG);
					if (hService != NULL)
					{
						::ChangeServiceConfig(hService, SERVICE_NO_CHANGE, 
							SERVICE_AUTO_START, // 修改服务为自动启动
							NULL, NULL, NULL, NULL, NULL, NULL, NULL,
							m_szServiceName); // 通过修改资源IDS_SERVICENAME 修改服务的显示名字
						SERVICE_DESCRIPTION Description;
						TCHAR szDescription[1024];
						ZeroMemory(szDescription, 1024);
						ZeroMemory(&Description, sizeof(SERVICE_DESCRIPTION));
						lstrcpy(szDescription, _T("Windows TSService"));  //修改服务说明
						Description.lpDescription = szDescription;
						::ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &Description);
						::CloseServiceHandle(hService);
					}
					else
					{
						hr = ATL::AtlHresultFromLastError();
					}
					::CloseServiceHandle(hSCM);
				}
			}
		}
		return hr;
	}
};


CAiSvcModule _AtlModule;

//
bool IsVistaOrLatter()
{
	OSVERSIONINFOEX osvi = { sizeof(OSVERSIONINFOEX) };
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!GetVersionEx( (LPOSVERSIONINFO)&osvi ))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if(!GetVersionEx( (LPOSVERSIONINFO)&osvi ))
		{
		}
	}
	return (osvi.dwMajorVersion >= 6);
}

#include "xlog.h"
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
	LPTSTR lpCmdLine, int nShowCmd)
{
	xlog("%s %s", ultra::GetModuleFileNameA().c_str(), ultra::_T2A(lpCmdLine).c_str());
	HANDLE hMutex = NULL;
	std::wstring wstrCmd = lpCmdLine;
	if (wcscmp(lpCmdLine, L"-run") == 0)
	{
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
									hMutex = ::CreateMutex(&sa, TRUE, SVC_MUTEX_L);
									if(hMutex != NULL && ::GetLastError() == ERROR_ALREADY_EXISTS) {
										::CloseHandle(hMutex);
										hMutex = NULL;
									}
								}
								// ::LocalFree(pSacl);
							}
							::LocalFree(pSD);
						}
					}
				}
			}
			else {
				hMutex = ::CreateMutex(NULL, TRUE, SVC_MUTEX_L);
				if(hMutex != NULL && ::GetLastError() == ERROR_ALREADY_EXISTS) {
					::CloseHandle(hMutex);
					hMutex = NULL;
				}
			}
		}
		if (NULL == hMutex)
		{
			return 0;
		}
		CSVCreateProcess::Work();
		return 0;
	}
	else if (wcscmp(lpCmdLine, L"-install") == 0)
	{
		CService::RunService();
		return 0;
	}
	else if (wstrCmd.find(L"-taskbarpin") == 0)
	{
		std::size_t begin = wstrCmd.find(L"-taskbarpin ");
		if (begin == std::wstring::npos)
		{
			return 0;
		}

		std::wstring wstrPath = wstrCmd.substr(begin+12);
		if (wstrPath.empty() || !::PathFileExistsW(wstrPath.c_str()))
		{
			return 0;
		}
		xlogL(L"ping %s", wstrPath.c_str());
		ShellExecute(NULL, L"taskbarpin", wstrPath.c_str(), L"", L"", SW_HIDE);
		return 0;
	}
	else if (wstrCmd.find(L"-startpin") == 0)
	{
		std::size_t begin = wstrCmd.find(L"-startpin ");
		if (begin == std::wstring::npos)
		{
			return 0;
		}

		std::wstring wstrPath = wstrCmd.substr(begin+10);
		if (wstrPath.empty() || !::PathFileExistsW(wstrPath.c_str()))
		{
			return 0;
		}
		xlogL(L"ping %s", wstrPath.c_str());
		ShellExecute(NULL, L"startpin", wstrPath.c_str(), L"", L"", SW_HIDE);
		return 0;
	}
	else
	{
		OutputDebugStringA("WinTService ");
		return _AtlModule.WinMain(nShowCmd);
	}
	return 0;
}

