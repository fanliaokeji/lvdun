#include "stdafx.h"
#include "CSVCreateProcess.h"
#include "ultra\file-op.h"
#include "ultra\string-op.h"
#include <map>
#include "Regedit.h"
#include "Service.h"

#include "xlog.h"
#define SLEEP_TIME 10*1000
#define MAGIC_NUM 8421

wchar_t* szSrcGroup[] = {L"gsupdate.exe",L"didaupdate.exe",L"weupdate.exe",L"frupdate.exe",NULL};

void CSVCreateProcess::Work()
{
	CloseHandle((HANDLE)_beginthreadex(NULL, 0, CSVCreateProcess::FollowExitByParent, NULL, 0, NULL));
	typedef int (*pfRunEx)(int magic);
	HMODULE hDll = LoadLibrary(DLL_NAME_L);
	pfRunEx pf = (pfRunEx)GetProcAddress(hDll, "RunEx");
	pf(MAGIC_NUM);
}

bool CSVCreateProcess::Go()
{
	CloseHandle((HANDLE)_beginthreadex(NULL, 0, CSVCreateProcess::GoProc, NULL, 0, NULL));
	return true;
}


UINT WINAPI  CSVCreateProcess::GoProc( void* param )
{
	//while (!ultra::CheckProcessExist(L"explorer.exe",FALSE))
	while(GetUserToken() == false)
	{
		Sleep(SLEEP_TIME);
	}
	if (KillOtherSvcProcess())
	{
		return false;
	}
	if (ultra::OSIsVista())
	{
		RunSelfAsUserAdmin();
	}
	else
	{
		RunSelfAsUser();
		//ShellExecute(NULL, L"open",  ultra::GetModuleFileNameW().c_str(), L"-run", ultra::ExpandEnvironment(SERVICE_DIR_L).c_str(), SW_SHOWNORMAL);
	}
	return 0;
}


UINT WINAPI  CSVCreateProcess::FollowExitByParent( void* param )
{
	DWORD dwPID = GetCurrentProcessId(); 
	DWORD dwPPID = 0;
	if (dwPID == 0)
	{
		return 0;
	}
	do{
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		if(hSnapShot == NULL) {
			break;
		}
		PROCESSENTRY32 stProcessEntry = {0};
		stProcessEntry.dwSize = sizeof(PROCESSENTRY32);
		Process32First(hSnapShot,&stProcessEntry);
		do 
		{
			if (stProcessEntry.th32ProcessID == dwPID)
			{
				dwPPID = stProcessEntry.th32ParentProcessID;
				break;
			}

		} while (Process32Next(hSnapShot,&stProcessEntry));
		CloseHandle(hSnapShot);
	}while(false);
	if (dwPPID == 0)
	{
		return 0;
	}

	std::wstring strParentName = L"";
	do{
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		if(hSnapShot == NULL) {
			break;
		}
		PROCESSENTRY32 stProcessEntry = {0};
		stProcessEntry.dwSize = sizeof(PROCESSENTRY32);
		Process32First(hSnapShot,&stProcessEntry);
		do 
		{
			if (stProcessEntry.th32ProcessID == dwPPID)
			{
				strParentName = stProcessEntry.szExeFile;
				break;
			}

		} while (Process32Next(hSnapShot,&stProcessEntry));
		CloseHandle(hSnapShot);
	}while(false);

	if (ultra::CompareStringNoCase(strParentName, EXE_NAME_L) != 0)
	{
		return 0;
	}
	xlogL(L"FollowExitByParent id = %lu,name = %s", dwPPID,strParentName.c_str());
	HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, dwPPID);
	if (hProcess != NULL)
	{
		WaitForSingleObject(hProcess, INFINITE);
		CloseHandle(hProcess);
		TerminateProcess(GetCurrentProcess(), (UINT)1);
	}
	return 0;
}

bool CSVCreateProcess::GetUserToken()
{
	// 获得当前Session ID
	DWORD dwSessionID = WTSGetActiveConsoleSessionId();

	// 获得当前Session的用户令牌
	HANDLE hToken = NULL;
	if (WTSQueryUserToken(dwSessionID, &hToken) == FALSE)
	{
		//OutputDebugStringA("WTSQueryUserToken False!   LastError = ");
		DWORD dwLastError = ::GetLastError();
		return false;
	}

	if (hToken != NULL)
	{
		CloseHandle(hToken);
	}
	return true;
}

BOOL CSVCreateProcess::RunSelfAsUserAdmin()
{
	std::wstring wstrCmdLine = ultra::GetModuleFileNameW()+ L" -run";
	HANDLE hToken = NULL;  
	HANDLE hTokenDup = NULL;  
	HANDLE hUserToken = NULL;  
	BOOL bSuccess = TRUE;
	do   
	{  
		if(OpenProcessToken(GetCurrentProcess(),TOKEN_ALL_ACCESS,&hToken))  
		{  
			if(DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS,NULL, SecurityIdentification, TokenPrimary, &hTokenDup))  
			{  
				DWORD dwSessionId = WTSGetActiveConsoleSessionId();  
				if(!SetTokenInformation(hTokenDup,TokenSessionId,&dwSessionId,sizeof(DWORD)))  
				{  
					xlogL(TEXT("SetTokenInformation error ！error code：%d\n"),GetLastError());  
					bSuccess = FALSE;  
					//break;  
				}  

				STARTUPINFO si;  
				PROCESS_INFORMATION pi;  
				ZeroMemory(&si,sizeof(STARTUPINFO));  
				ZeroMemory(&pi,sizeof(PROCESS_INFORMATION));  
				si.cb = sizeof(STARTUPINFO);  
				si.lpDesktop = _T("WinSta0\\Default");  
				si.wShowWindow = SW_SHOW;  
				si.dwFlags     =   STARTF_USESHOWWINDOW /*|STARTF_USESTDHANDLES*/;  
				
				if (!::WTSQueryUserToken(dwSessionId, &hUserToken) || !hUserToken)
				{
					xlogL(TEXT("WTSQueryUserToken error ！error code：%d\n"),GetLastError());  
					bSuccess = FALSE;  
					break;  
				}
				LPVOID pEnv = NULL;  
				DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT;  
				if(!CreateEnvironmentBlock(&pEnv,hUserToken,FALSE))  
				{  
					xlogL(TEXT("CreateEnvironmentBlock error ！error code：%d\n"),GetLastError());  
					bSuccess = FALSE;  
					break;  
				}  

				WCHAR szBuffer[MAX_PATH] = {0};
				wcscpy(szBuffer, wstrCmdLine.c_str());

// 				PROFILEINFO pfi = {0};
// 				TCHAR szActiveUserName[100] = {0};
// 				LPTSTR pszUserName = NULL;
// 				DWORD cbUserName = 0;
// 				if (!::WTSQuerySessionInformation(0, dwSessionId, WTSUserName, &pszUserName, &cbUserName))
// 				{
// 					DWORD dwLastError = ::GetLastError();
// 					xlogL(_T("WTSQuerySessionInformation failed. LastError = %ld"), dwLastError);
// 					bSuccess = FALSE;  
// 					break; 
// 				}
// 				::memcpy(szActiveUserName, pszUserName, cbUserName);
// 				::WTSFreeMemory(pszUserName);
// 				pfi.dwSize = sizeof(pfi);
// 				pfi.lpUserName = szActiveUserName;
// 				if (IsVistaOrLater() && !::LoadUserProfile(hTokenDup, &pfi))
// 				{
// 					DWORD dwLastError = ::GetLastError();
// 					xlogL(_T("LoadUserProfile failed. LastError = %ld"), dwLastError);
// 					bSuccess = FALSE;  
// 					break;  
// 				}

				if(!CreateProcessAsUser(hTokenDup,NULL,szBuffer,NULL,NULL,FALSE,dwCreationFlag,pEnv,NULL,&si,&pi))  
				{  
					xlogL(TEXT("CreateProcessAsUser error ！error code：%d\n"),GetLastError());  
					bSuccess = FALSE;  
					break;  
				}  

				if(pEnv)  
				{  
					DestroyEnvironmentBlock(pEnv);  
				}  
			}  
			else  
			{  
				OutputDebugStringA("WinTService 6");
				xlogL(TEXT("DuplicateTokenEx error ！error code：%d\n"),GetLastError());  
				bSuccess = FALSE;  
				break;  
			}  


		}  
		else  
		{  
			xlogL(TEXT("cannot get administror！error code：%d\n"),GetLastError());  
			bSuccess = FALSE;  
			break;  
		}  
	}while(0);  

	if(hTokenDup != NULL && hTokenDup != INVALID_HANDLE_VALUE)  
		CloseHandle(hTokenDup);  
	if(hToken != NULL && hToken != INVALID_HANDLE_VALUE)  
		CloseHandle(hToken);  
	return bSuccess;
}

void CSVCreateProcess::RunSelfAsUser()
{

	BOOL bSuc = TRUE;
	DWORD dwLastError = ERROR_SUCCESS;
	BOOL bThisProcessCreatedAsUser = TRUE;
	if (SUCCEEDED(IsThisProcessCreatedAsUser(bThisProcessCreatedAsUser)))
	{
		WCHAR szCmdLine[MAX_PATH * 3] = {0};

		std::wstring strCmd = ultra::GetModuleFileNameW()+ L" -run";
		swprintf(szCmdLine, L"%s", strCmd.c_str());
		if (bThisProcessCreatedAsUser)
		{
			STARTUPINFO si = {0};
			si.cb = sizeof(si);
			PROCESS_INFORMATION pi = {0};

			// CreateProcess
			bSuc = ::CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
			dwLastError = ::GetLastError();
			if (bSuc)
			{
				xlogL(_T("CreateProcess succeeded. SubPID = %lu, SubTID = %lu"), pi.dwProcessId, pi.dwThreadId);

				::CloseHandle(pi.hThread);
				::CloseHandle(pi.hProcess);
			}
			else
			{
				xlogL(_T("CreateProcess failed. LastError = %lu"), dwLastError);
			}
		}
		else
		{
			PROCESS_INFORMATION pi = {0};
			SECURITY_ATTRIBUTES sa = {0};
			STARTUPINFO si = {0};
			PROFILEINFO pfi = {0};
			HANDLE hUserToken = NULL;
			DWORD dwActiveSessionId = 0xFFFFFFFF;
			TCHAR szActiveUserName[100] = {0};
			int iProcId = -1;
			LPTSTR pszUserName = NULL;
			DWORD cbUserName = 0;

			dwActiveSessionId = ::WTSGetActiveConsoleSessionId();
			if (0xFFFFFFFF == dwActiveSessionId)
			{
				xlogL(_T("WTSGetActiveConsoleSessionId() return %lu"), dwActiveSessionId);
			}
			else
			{
				if (::WTSQuerySessionInformation(0, dwActiveSessionId, WTSUserName, &pszUserName, &cbUserName))
				{
					::memcpy(szActiveUserName, pszUserName, cbUserName);
					::WTSFreeMemory(pszUserName);

					if (::WTSQueryUserToken(dwActiveSessionId, &hUserToken) && hUserToken)
					{
						LPVOID pEnv = NULL;
						if (::CreateEnvironmentBlock(&pEnv, hUserToken, FALSE))
						{
							sa.nLength = sizeof(sa);
							si.cb = sizeof(si);
							si.lpDesktop = _T("");
							pfi.dwSize = sizeof(pfi);
							pfi.lpUserName = szActiveUserName;

							if ((!IsVistaOrLater()) || ::LoadUserProfile(hUserToken, &pfi))
							{
								if (::CreateProcessAsUser(hUserToken, NULL, szCmdLine, NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT, pEnv, NULL, &si, &pi))
								{
									xlogL(_T("CreateProcessAsUser succeeded. SubPID = %lu, SubTID = %lu"), pi.dwProcessId, pi.dwThreadId);
									::CloseHandle(pi.hThread);
									::CloseHandle(pi.hProcess);
								}
								else
								{
									DWORD dwLastError = ::GetLastError();
									xlogL(_T("CreateProcessAsUser failed. LastError = %ld"), dwLastError);
								}
							}
							else
							{
								DWORD dwLastError = ::GetLastError();
								xlogL(_T("LoadUserProfile failed. LastError = %ld"), dwLastError);
							}

							//////////////////////////////////////////////////////////////////////////
							::DestroyEnvironmentBlock(pEnv);
						}
						else
						{
							DWORD dwLastError = ::GetLastError();
							xlogL(_T("CreateEnvironmentBlock failed. LastError = %ld"), dwLastError);
						}

						//////////////////////////////////////////////////////////////////////////
						::CloseHandle(hUserToken);
					}
					else
					{
						DWORD dwLastError = ::GetLastError();
						xlogL(_T("WTSQueryUserToken failed. LastError = %ld"), dwLastError);
					}
				}
				else
				{
					DWORD dwLastError = ::GetLastError();
					xlogL(_T("WTSQuerySessionInformation failed. LastError = %ld"), dwLastError);
				}
			}
		}
	}
}

//bool CSVCreateProcess::MyImpersonateLoggedOnUser()
//{
//	HANDLE hToken = NULL;
//	DWORD dwConsoleSessionId = WTSGetActiveConsoleSessionId();
//	if (WTSQueryUserToken(dwConsoleSessionId, &hToken))
//	{
//		if (ImpersonateLoggedOnUser(hToken))
//		{
//			// 保存Token
//			return true;
//		}
//	}
//	return false;
//}


 bool CSVCreateProcess::KillOtherSvcProcess()
{
	BOOL bRtn = FALSE;
	do{
		// find process
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		if(hSnapShot == NULL) {
			break;
		}

		PROCESSENTRY32 stProcessEntry = {0};
		stProcessEntry.dwSize = sizeof(PROCESSENTRY32);
		Process32First(hSnapShot,&stProcessEntry);
		do 
		{
			int index = 0;
			while(NULL != szSrcGroup[index])
			{
				if(ultra::CompareStringNoCase(EXE_NAME_L, szSrcGroup[index]) != 0 && ultra::CompareStringNoCase(stProcessEntry.szExeFile, szSrcGroup[index]) == 0)
				{
					HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS , FALSE, stProcessEntry.th32ProcessID ); 
					if (NULL == hProcess)
					{
						return false;
					}
					TerminateProcess( hProcess, 0 );
				}
				index++;
			}
		} while (Process32Next(hSnapShot,&stProcessEntry));
		CloseHandle(hSnapShot);
	}while(false);

	return bRtn;
}


void CSVCreateProcess::FreeProcessUserSID(PSID psid)
{
	::HeapFree(::GetProcessHeap(), 0, (LPVOID)psid);
}

BOOL CSVCreateProcess::GetProcessUserSidAndAttribute(PSID *ppsid, DWORD *pdwAttribute)
{
	if (NULL == ppsid || NULL == pdwAttribute) return FALSE;

	BOOL bRet = FALSE;

	BOOL bSuc = TRUE;
	DWORD dwLastError = ERROR_SUCCESS;

	HANDLE hProcessToken = NULL;
	bSuc = ::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &hProcessToken);
	dwLastError = ::GetLastError();
	
	xlogL(L"OpenProcessToken(::GetCurrentProcess()) return %d , LastError = %lu, , hProcessToken = 0x%p", bSuc, dwLastError, hProcessToken);

	//////////////////////////////////////////////////////////////////////////
	if (bSuc && hProcessToken)
	{
		BYTE *pBuffer = NULL;
		DWORD cbBuffer = 0;
		DWORD cbBufferUsed = 0;
		bSuc = ::GetTokenInformation(hProcessToken, ::TokenUser, pBuffer, cbBuffer, &cbBufferUsed);
		dwLastError = ::GetLastError();
		if (ERROR_INSUFFICIENT_BUFFER == dwLastError)
		{
			pBuffer = new BYTE[cbBufferUsed];
			cbBuffer = cbBufferUsed;
			cbBufferUsed = 0;
			bSuc = ::GetTokenInformation(hProcessToken, ::TokenUser, pBuffer, cbBuffer, &cbBufferUsed);
			dwLastError = ::GetLastError();
			if (bSuc)
			{
				TOKEN_USER *pTokenUser = (TOKEN_USER *)pBuffer;
				DWORD dwLength = ::GetLengthSid(pTokenUser->User.Sid);
				*ppsid = (PSID)::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, dwLength);
				if (*ppsid)
				{
					if (::CopySid(dwLength, *ppsid, pTokenUser->User.Sid))
					{
						*pdwAttribute = pTokenUser->User.Attributes;
						bRet = TRUE;
					}
					else
					{
						::HeapFree(::GetProcessHeap(), 0, (LPVOID)*ppsid);
					}
				}
			}

			delete [] pBuffer;
			pBuffer = NULL;
			cbBuffer = 0;
			cbBufferUsed = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	::CloseHandle(hProcessToken);
	hProcessToken = NULL;

	return bRet;
}

HRESULT CSVCreateProcess::IsThisProcessCreatedAsUser(BOOL &bCreatedAsUser)
{
	HRESULT hr = E_FAIL;

	PSID psid = NULL;
	DWORD dwAttribute = 0;
	if (GetProcessUserSidAndAttribute(&psid, &dwAttribute))
	{
		LPTSTR pszSID = NULL;
		if (::ConvertSidToStringSid(psid, &pszSID))
		{

			xlogL(L"ProcessUserSID[2] = %s", pszSID);
			TCHAR szNtNonUniqueID[100] = {0};
			_stprintf(szNtNonUniqueID, _T("S-1-5-%lu-"), SECURITY_NT_NON_UNIQUE);

			hr = S_OK;
			if (_tcsstr(pszSID, szNtNonUniqueID) != NULL)
			{
				xlogL(_T("This process is user process"));
				bCreatedAsUser = TRUE;
			}
			else
			{
				xlogL(_T("This process is non-user process"));
				bCreatedAsUser = FALSE;
			}

			::LocalFree((HLOCAL)pszSID);
		}

		FreeProcessUserSID(psid);
	}

	return hr;
}

//BOOL CSVCreateProcess::GetActiveConsoleSessionId(DWORD *pdwSessionId)
//{
//	if (NULL == pdwSessionId) return FALSE;
//
//	BOOL bRet = FALSE;
//
//	BOOL bSuc = TRUE;
//	DWORD dwLastError = ERROR_SUCCESS;
//
//	*pdwSessionId = ::WTSGetActiveConsoleSessionId();
//	dwLastError = ::GetLastError();
//	xlogL(_T("WTSGetActiveConsoleSessionId return %lu , LastError = %lu"),(*pdwSessionId) ,dwLastError);
//	bRet = TRUE;
//
//	return bRet;
//}

//BOOL CSVCreateProcess::GetProcessSessionId(DWORD *pdwSessionId)
//{
//	if (NULL == pdwSessionId) return FALSE;
//
//	BOOL bRet = FALSE;
//
//	BOOL bSuc = TRUE;
//	DWORD dwLastError = ERROR_SUCCESS;
//
//	HANDLE hProcessToken = NULL;
//	bSuc = ::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &hProcessToken);
//	dwLastError = ::GetLastError();
//	xlogL(L"OpenProcessToken(::GetCurrentProcess()) return %d, LastError = %lu,, hProcessToken = 0x%p", bSuc, dwLastError, hProcessToken);
//	//////////////////////////////////////////////////////////////////////////
//	if (bSuc && hProcessToken)
//	{
//		DWORD cbBufferUsed = 0;
//		bSuc = ::GetTokenInformation(hProcessToken, ::TokenSessionId, pdwSessionId, sizeof(DWORD), &cbBufferUsed);
//		dwLastError = ::GetLastError();
//		if (bSuc)
//		{
//			xlogL(L"Process session id = %lu",(*pdwSessionId));
//			bRet = TRUE;
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//
//	::CloseHandle(hProcessToken);
//	hProcessToken = NULL;
//
//	return bRet;
//}

//BOOL CSVCreateProcess::GetLoggedOnUserName(LPTSTR &pszUserName)
//{
//	BOOL bRet = FALSE;
//
//	BOOL bSuc = TRUE;
//	DWORD dwLastError = ERROR_SUCCESS;
//
//	HANDLE hServer = WTS_CURRENT_SERVER_HANDLE;
//	DWORD dwSessionId = 0;
//	if (GetActiveConsoleSessionId(&dwSessionId))
//	{
//		DWORD cbUserName = 0;
//		bSuc = ::WTSQuerySessionInformation(hServer, dwSessionId, ::WTSUserName, &pszUserName, &cbUserName);
//		dwLastError = ::GetLastError();
//		if (bSuc)
//		{
//			xlogL(_T("Logged-on UserName = %s"),pszUserName);
//
//			bRet = TRUE;
//		}
//		else
//		{
//			
//			xlogL(_T("WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE) failed, Use WTSOpenServer"));
//
//			hServer = ::WTSOpenServer(_T("localhost"));
//
//			bSuc = ::WTSQuerySessionInformation(hServer, dwSessionId, ::WTSUserName, &pszUserName, &cbUserName);
//			dwLastError = ::GetLastError();
//			if (bSuc)
//			{
//				xlogL(_T("Logged-on UserName[2] = %s"), pszUserName);
//				bRet = TRUE;
//			}
//
//			::WTSCloseServer(hServer);
//		}
//	}
//
//	return bRet;
//}

//void CSVCreateProcess::FreeLoggedOnUserName(LPTSTR pszUserName)
//{
//	::WTSFreeMemory(pszUserName);
//}

BOOL CSVCreateProcess::IsVistaOrLater()
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;
	int op = VER_GREATER_EQUAL;

	::ZeroMemory(&osvi, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	osvi.dwMajorVersion = 6;
	osvi.dwMinorVersion = 0;

	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, op);

	BOOL bIsVistaOrLater = ::VerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION, dwlConditionMask);

	return bIsVistaOrLater;
}

//////////////////////////////////////////////////////////////////////////