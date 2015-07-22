#pragma once
#include <string>

#define MODULEVERSION L"4.0"

#ifdef DIDARILI_0000
#define SERVER_DIR L"http://dl.didarili.com/update/" MODULEVERSION L"/0000/"
#define SERVICE_NAME_L L"didaupdate"
#define SERVICE_EXE_NAME_L L"didaupdate.exe"
#define DLL_NAME_L L"didaupdate.dll"
#define SERVICE_DIR "%allusersprofile%\\didaupdate\\"
#define SERVICE_DIR_L L"%allusersprofile%\\didaupdate\\"
#define PRODUCT_REG_L L"SOFTWARE\\DDCalendar"
#endif
#ifdef LVDUN_0000
#define SERVER_DIR L"http://dl.lvdun123.com/update/" MODULEVERSION L"/0000/"
#define SERVICE_NAME_L L"gsupdate"
#define SERVICE_EXE_NAME_L L"gsupdate.exe"
#define DLL_NAME_L L"gsupdate.dll"
#define SERVICE_DIR "%allusersprofile%\\gsupdate\\"
#define SERVICE_DIR_L L"%allusersprofile%\\gsupdate\\"
#define PRODUCT_REG_L L"SOFTWARE\\GreenShield"
#endif
#ifdef WE
#define SERVER_DIR L"http://dl.ggxpc.com/update/" MODULEVERSION L"/0000/"
#define SERVICE_NAME_L L"weupdate"
#define SERVICE_EXE_NAME_L L"weupdate.exe"
#define DLL_NAME_L L"weupdate.dll"
#define SERVICE_DIR "%allusersprofile%\\weupdate\\"
#define SERVICE_DIR_L L"%allusersprofile%\\weupdate\\"
#define PRODUCT_REG_L L"SOFTWARE\\WebEraser"
#endif
#ifdef FR
#define SERVER_DIR L"http://dl.feitwo.com/update/" MODULEVERSION L"/0000/"
#define SERVICE_NAME_L L"frupdate"
#define SERVICE_EXE_NAME_L L"frupdate.exe"
#define DLL_NAME_L L"frupdate.dll"
#define SERVICE_DIR "%allusersprofile%\\frupdate\\"
#define SERVICE_DIR_L L"%allusersprofile%\\frupdate\\"
#define PRODUCT_REG_L L"SOFTWARE\\FlyRabbit"
#endif
#define LOCAL_DIR SERVICE_DIR
//#define CLOUD_CFG L"ai.dll"

#ifndef PRODUCT_REG_CHANNEL_L
#define PRODUCT_REG_CHANNEL_L L"InstallSource"
#endif

#ifndef PRODUCT_REG_VER_L
#define PRODUCT_REG_VER_L L"Ver"
#endif

#define CFG_INI L"WinTService.dll"
#ifdef  DIDARILI_0000 
#define CFG_INI L"WinUPC.dll"
#endif
#ifdef  LVDUN_0000 
#define CFG_INI L"WinUPC.dll"
#endif
#ifdef  WE 
#define CFG_INI L"WinUPC.dll"
#endif

#ifdef  FR 
#define CFG_INI L"WinUPC.dll"
#endif

class CNetwork
{
public:

	static CNetwork* Instance();

	bool HttpDownloadString(std::wstring wstrRequest, std::wstring& wstrReturn);
	bool HttpDownloadString(std::wstring wstrRequest, std::string& strReturn);
	bool HttpDownloadFile(std::wstring wstrRequest, std::wstring wstrFilePath);

	bool Report(std::wstring wstrRequest);
	bool ReportGoogleStat(const TCHAR *ea, const TCHAR *el,long ev=1);
	static CNetwork* m_network;
	CNetwork();
	~CNetwork();

	static unsigned int _stdcall ReportProc( void* param );

protected:


private:

};


bool IsBSGS();
bool GetUpdateCfgPath(wchar_t* szCfgPath, bool bCreatePath = true);