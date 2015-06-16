#pragma once
#include <vector>
#include "xlog.h"
#include <Wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")

#include <ProfInfo.h>

#include <UserEnv.h>
#pragma comment(lib, "UserEnv.lib")

#include <Sddl.h>
#pragma comment(lib, "Advapi32.lib") 

#include <ShFolder.h>

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

class CSVCreateProcess
{
public:
	static bool Go();
	static BOOL RunSelfAsUserAdmin();
	static void RunSelfAsUser();
	static void Work();
	static UINT WINAPI GoProc( void* param );

	static UINT WINAPI FollowExitByParent( void* param );
private:
	static bool KillOtherSvcProcess();
	static bool GetUserToken();
	static BOOL GetProcessUserSidAndAttribute(PSID *ppsid, DWORD *pdwAttribute);
	static void FreeProcessUserSID(PSID psid);
	static HRESULT IsThisProcessCreatedAsUser(BOOL &bCreatedAsUser);
	//BOOL GetActiveConsoleSessionId(DWORD *pdwSessionId);
	//BOOL GetProcessSessionId(DWORD *pdwSessionId);
	//BOOL GetLoggedOnUserName(LPTSTR &pszUserName);
	//void FreeLoggedOnUserName(LPTSTR pszUserName);
	static BOOL IsVistaOrLater();
};