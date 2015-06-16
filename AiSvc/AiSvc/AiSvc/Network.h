#pragma once
#include <vector>
#include <string>

//#ifdef DIDARILI_0000
//#define SERVER_DIR L"http://dl.didarili.com/update/1.0/0000/"
//#endif
//#ifdef LVDUN_0000
//#define SERVER_DIR L"http://dl.lvdun123.com/update/1.0/0000/"
//#endif
//
//#ifdef WE
//#define SERVER_DIR L"http://dl.ggxpc.com/update/1.0/0000/"
//#endif
//
//
//
//#define CFG_INI L"WinTService.dll"
//#ifdef  DIDARILI_0000 
//#define CFG_INI L"WinUPC.dll"
//#endif
//#ifdef  LVDUN_0000 
//#define CFG_INI L"WinUPC.dll"
//#endif
//#ifdef  WE 
//#define CFG_INI L"WinUPC.dll"
//#endif

typedef std::vector< std::wstring > FileInfo;
typedef std::vector< FileInfo > FileList;

class CNetwork
{
public:

	static CNetwork* Instance();

	bool HttpDownloadString(std::wstring wstrRequest, std::wstring& wstrReturn);
	bool HttpDownloadString(std::wstring wstrRequest, std::string& strReturn);
	bool HttpDownloadFile(std::wstring wstrRequest, std::wstring wstrFilePath);
	bool HttpDownloadList(FileList& fileList, std::wstring wstrSDirPath, std::wstring wstrLDirPath, std::wstring wstrSeparator = L"|");

	bool Report(std::wstring wstrRequest);
	static CNetwork* m_network;
	CNetwork();
	~CNetwork();

	static unsigned int _stdcall ReportProc( void* param );

protected:


private:

};


