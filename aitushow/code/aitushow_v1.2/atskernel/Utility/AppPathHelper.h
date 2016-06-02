#pragma once

typedef HRESULT (STDAPICALLTYPE *PFNSHGetKnownFolderPath)(const GUID & rfid, DWORD dwFlags, HANDLE hToken, PWSTR *ppszPath);
class CKKPathHelper
{
public:
	CKKPathHelper();
	~CKKPathHelper();

	//系统信息
	const wstring & GetTempDir() const {return m_strTempDir;}
	const wstring & GetAppDataDir() const {return m_strAppDataDir;}
	const wstring & GetLocalAppDataDir() const {return m_strLocalAppDataDir;}
	const wstring & GetCommonAppDataDir() const {return m_strCommonAppDataDir;}

	//应用程序信息
	const wstring & GetAppDir() const {return m_strAppDir;}
	const wstring & GetExeDir() const {return m_strExeDir;}
	const wstring & GetXarDir() const {return m_strXarDir;}
	const wstring & GetAllUserCfgDir() const {return m_strCfgDir;}

private:
	wstring m_strTempDir;
	wstring m_strAppDataDir;
	wstring m_strLocalAppDataDir;
	wstring m_strCommonAppDataDir;
	wstring m_strAppDir;
	wstring m_strExeDir;
	wstring m_strXarDir;
	wstring m_strCfgDir;
};
