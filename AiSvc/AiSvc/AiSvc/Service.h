#pragma once
#include <Windows.h>
#include <string>

#ifdef LVDUN_0000
#define EXE_NAME "gsupdate.exe"
#define EXE_NAME_L L"gsupdate.exe"
#define DLL_NAME_L L"gsupdate.dll"
#define SERVICE_NAME "gsupdate"
#define SERVICE_NAME_L L"gsupdate"
#define SERVICE_DIR "%allusersprofile%\\gsupdate\\"
#define SERVICE_DIR_L L"%allusersprofile%\\gsupdate\\"
#endif

#ifdef DIDARILI_0000
#define EXE_NAME "didaupdate.exe"
#define EXE_NAME_L L"didaupdate.exe"
#define DLL_NAME_L L"didaupdate.dll"
#define SERVICE_NAME "didaupdate"
#define SERVICE_NAME_L L"didaupdate"
#define SERVICE_DIR "%allusersprofile%\\didaupdate\\"
#define SERVICE_DIR_L L"%allusersprofile%\\didaupdate\\"
#endif

#ifdef WE
#define EXE_NAME "weupdate.exe"
#define EXE_NAME_L L"weupdate.exe"
#define DLL_NAME_L L"weupdate.dll"
#define SERVICE_NAME "weupdate"
#define SERVICE_NAME_L L"weupdate"
#define SERVICE_DIR "%allusersprofile%\\weupdate\\"
#define SERVICE_DIR_L L"%allusersprofile%\\weupdate\\"
#endif

#ifdef FR
#define EXE_NAME "frupdate.exe"
#define EXE_NAME_L L"frupdate.exe"
#define DLL_NAME_L L"frupdate.dll"
#define SERVICE_NAME "frupdate"
#define SERVICE_NAME_L L"frupdate"
#define SERVICE_DIR "%allusersprofile%\\frupdate\\"
#define SERVICE_DIR_L L"%allusersprofile%\\frupdate\\"
#endif

#ifdef UPDATESVCS
#define EXE_NAME "updatesvcs.exe"
#define EXE_NAME_L L"updatesvcs.exe"
#define SERVICE_NAME "updatesvcs"
#define SERVICE_NAME_L L"updatesvcs"
#define SERVICE_DIR "%allusersprofile%\\updatesvcs\\"
#define SERVICE_DIR_L L"%allusersprofile%\\updatesvcs\\"

#endif

class CService
{
public:
	CService();
	~CService();
	static int IsInstalledService(std::wstring wstrService);
	static bool InstallService(std::wstring wstrDir, std::wstring wstrService, bool bStart = true);
	static bool UnInstallService(std::wstring wstrService);
	static bool StartService(std::wstring wstrService);

	static bool RunService(); 
private:
};

