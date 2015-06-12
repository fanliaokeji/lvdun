// npFlyRabbitCtrl.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "npFlyRabbitCtrl_i.h"
#include "dllmain.h"
#include "RegHelper.h"
#include "AddDapCtrlToChromeWhiteList.h"


tstring strClsID = _T("{6C45A4E4-9D47-4fce-8873-A458855D0C93}");
tstring strTypeLib = _T("{1D552FB9-4923-42fc-8488-898FDC04DB5E}");
tstring strHistoryVer = _T("FRCTRL_INSTALL_HISTORY_VER_2");


HRESULT InstallDapCtrlForFireFox()
{
	std::wstring strDstPath = GetPath(CSIDL_PROGRAM_FILES_COMMON);
	if(strDstPath[strDstPath.length() - 1] != '\\')
		strDstPath += _T("\\");
	strDstPath += _T("FlyRabbit\\reghelper\\");
	const std::wstring dllFileName = L"npFlyRabbitCtrl.dll";
	std::wstring dllFileFullPath = strDstPath + dllFileName;
	std::wstring oldDllFileFullPath = dllFileFullPath + L".old";
	// ���ԭ�ȴ���npKKDapCtrl.dll����������ΪnpKKDapCtrl.dll.old
	bool isOldDllFileRenamed = false;
	std::wstring registeResult = L"0000";
	HRESULT hr = S_OK;
	do {
		if(::PathFileExists(dllFileFullPath.c_str())) {
			if(::MoveFileEx(dllFileFullPath.c_str(), oldDllFileFullPath.c_str(), MOVEFILE_WRITE_THROUGH | MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED)) {
				isOldDllFileRenamed = true;
			}
			else {
				// ������npKKDapCtrl.dllΪnpKKDapCtrl.oldʧ��
				registeResult = L"0001";
				hr = E_FAIL;
				break;
			}
		}

		hr = RegisterEx(_T(""), strDstPath, TRUE, _T("npFlyRabbitCtrl.dll"), NULL, TRUE, TRUE, strClsID, strTypeLib,strHistoryVer);
		if(FAILED(hr)) {
			if(isOldDllFileRenamed) {
				// �ƻ��ļ� npKKDapCtrl.dll.old -> npKKDapCtrl.dll
				::MoveFileEx(oldDllFileFullPath.c_str(), dllFileFullPath.c_str(), MOVEFILE_WRITE_THROUGH | MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED);
			}
			// RegisterEx����ʧ��
			registeResult = L"0010";
			break;
		}

		if(isOldDllFileRenamed) {
			// ����ɾ�����ļ� npKKDapCtrl.dll.old ���������Ƿ��ܹ�ɾ��
			::DeleteFile(oldDllFileFullPath.c_str());
		}
		const std::wstring& strDstFullPath = dllFileFullPath;

		unsigned __int64 i64FileVer = 0;
		i64FileVer = GetFileVersion(strDstFullPath.c_str());
		TCHAR szVersion[MAX_PATH];
		if (_sntprintf(szVersion, sizeof(szVersion), _T("%u.%u.%u.%u"),
			HIWORD(i64FileVer >> 32), LOWORD(i64FileVer >> 32), HIWORD(i64FileVer & 0xffffffff), LOWORD(i64FileVer & 0xffffffff)) < 0)
			szVersion[sizeof(szVersion) - 1] = 0;

		CRegKey key;
		if(!(ERROR_SUCCESS == key.Create(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\MozillaPlugins\\@feitwo.com/npFlyRabbitCtrl"))
			&& ERROR_SUCCESS == key.SetStringValue(_T("Path"), strDstFullPath.c_str())
			&& ERROR_SUCCESS == key.SetStringValue(_T("ProductName"), _T("Feitwo npCtrl Plugin"))
			&& ERROR_SUCCESS == key.SetStringValue(_T("Vendor"), _T("FlyRabbit"))
			&& ERROR_SUCCESS == key.SetStringValue(_T("Description"), _T("Feitwo npCtrl Plugin"))
			&& ERROR_SUCCESS == key.SetStringValue(_T("Version"), szVersion)))
		{
			LOG4C_ERROR(_T("[InstallDapCtrlForFireFox] Set HKEY_LOCAL_MACHINE\\SOFTWARE\\MozillaPlugins\\@xunlei.com/DapCtrl Failed."));
			registeResult = L"0100";
			hr = E_FAIL;
			break;
		}
		key.Close();

		if(!(ERROR_SUCCESS == key.Create(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\MozillaPlugins\\@feitwo.com/npFlyRabbitCtrl\\MimeTypes\\application/x-flyrabbit-ctrl"))
			&& ERROR_SUCCESS == key.SetStringValue(_T("Description"), _T("Feitwo npCtrl Plugin"))
			&& ERROR_SUCCESS == key.SetStringValue(_T("Suffixes"), _T("*"))))
		{
			LOG4C_ERROR(_T("[InstallDapCtrlForFireFox] Set HKEY_LOCAL_MACHINE\\SOFTWARE\\MozillaPlugins\\@xunlei.com/DapCtrl\\MimeTypes\\application/x-thunder-dapctrl Failed."));
			registeResult = L"0200";
			hr = E_FAIL;
			break;
		}
		key.Close();
	} while(false);

	if(SUCCEEDED(hr)) {

		// ��ȡ��ѡ���ļ�λ��
		do {
			OSVERSIONINFOEX osvi = { sizeof(OSVERSIONINFOEX) };
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
			if(!GetVersionEx( (LPOSVERSIONINFO)&osvi ))
			{
				//����ֻ�ܵõ�OSVERSIONINFO, ���ܵõ�OSVERSIONINFOEX
				osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
				if(!GetVersionEx( (LPOSVERSIONINFO)&osvi ))
				{
					break;
				}
			}
			bool isVistaOrLater = osvi.dwMajorVersion >= 6;
			std::wstring preferencesFilePath;
			wchar_t preferencesPathBuffer[1024];
			if(!isVistaOrLater) {
				// XP %USERPROFILE%\Local Settings\Application Data\Google\Chrome\User Data\Default\Preferences
				if(ExpandEnvironmentStrings(L"%USERPROFILE%\\Local Settings\\Application Data\\Google\\Chrome\\User Data\\Default\\Preferences", preferencesPathBuffer, 1024) != 0) {
					preferencesFilePath = preferencesPathBuffer;
				}
			}
			else {
				// Vista Or Later %LOCALAPPDATA%\Google\Chrome\User Data\Default\Preferences
				if(ExpandEnvironmentStrings(L"%LOCALAPPDATA%\\Google\\Chrome\\User Data\\Default\\Preferences", preferencesPathBuffer, 1024) != 0) {
					preferencesFilePath = preferencesPathBuffer;
				}
			}
			if(!preferencesFilePath.empty()) {
				// �������ӵ�������
				int error_code = 0;
				// @error_code ������
				// 0: �ɹ�
				// 1: ��ѡ���ļ�������
				// 2: �򿪻��ȡ��ѡ���ļ�����
				// 3: ������ѡ��jsonʧ��
				// 4: �Ҳ���ĳ��������key
				// 5: chrome�汾��С��32
				// 6: per_plugin�д�������֮���ֵ
				// 7: ��ѡ���ļ�д����� �����ƶ��ļ�ָ�뵽�ļ�ͷ��д�����
				// 9: ��������
				if(!AddDapCtrlToChromeWhiteList(preferencesFilePath, dllFileName, error_code)) {
					TSDEBUG4CXX(L"Failed to AddDapCtrlToChrmeWhiteList preferences: " << preferencesFilePath << L" dllFileName: " << dllFileName);
				}
				else {
					// registeResult
					registeResult[0] = L'0' + error_code;
				}
			}
			else {
				// ʹ��ExpandEnvironmentStringsչ����ѡ���ļ�·��ʧ��
				registeResult[0] = L'8';
			}
		} while(false);
	}

	ATL::CRegKey regKey;
	if(regKey.Create(HKEY_CURRENT_USER, L"Software\\FlyRabbit", 0, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE) == ERROR_SUCCESS) {
		regKey.SetStringValue(L"RegisterResult", registeResult.c_str());
		regKey.Close();
	}

	return S_OK;
}

void UnInstallDapCtrlForFireFox()
{
	tstring strDstPath;
	strDstPath = GetPath(CSIDL_PROGRAM_FILES_COMMON);
	if(strDstPath[strDstPath.length() - 1] != '\\')
		strDstPath += _T("\\");
	strDstPath += _T("FlyRabbit\\reghelper\\");

	UnRegisterEx(strDstPath, strClsID, strHistoryVer);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = InstallDapCtrlForFireFox();
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	UnInstallDapCtrlForFireFox();
	return S_OK;
}	

extern "C" __declspec(dllexport)
HRESULT RegisterServerDirect()
{
	TSAUTO();
	HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}

extern "C" __declspec(dllexport)
HRESULT UnregisterServerDirect()
{
	TSAUTO();
	CRegKey key;
	if(ERROR_SUCCESS == key.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\MozillaPlugins")))
	{
		key.RecurseDeleteKey(_T("@feitwo.com/npFlyRabbitCtrl"));
	}

	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}
