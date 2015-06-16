#include "stdafx.h"
#include "Shell.h"
#include "ultra/file-op.h"

bool CShell::CreateShortCutLink(
	const std::wstring& display_name,
	const std::wstring& src_path,
	const std::wstring& dest_dir,
	const std::wstring& link_arguments,
	const std::wstring& description,
	const std::wstring& icon_path) {
		if (display_name.empty() || src_path.empty() || dest_dir.empty()) {
			return false;
		}
		HRESULT hr;
		IShellLink* psl = NULL;
		IPersistFile* ppf = NULL;

		SetFileAttributes((dest_dir+display_name+L".lnk").c_str(), FILE_ATTRIBUTE_NORMAL);
		do {
			hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&psl);
			if (FAILED(hr)) {
				break;
			}
			hr = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
			if (FAILED(hr)) {
				break;
			}
			hr = psl->SetPath(src_path.c_str());
			if (FAILED(hr)) {
				break;
			}
			std::wstring work_dir = ultra::GetUpperPath(src_path);
			if (src_path.length() > 5 && src_path.substr(0, 7) == L"http://")
			{
				work_dir = L"C:\\Program File\\Internet Explorer\\";
			}
			hr = psl->SetWorkingDirectory(work_dir.c_str());
			if (FAILED(hr)) {
				break;
			}
			if (!link_arguments.empty()) {
				hr = psl->SetArguments(link_arguments.c_str());
				if (FAILED(hr)) {
					break;
				}
			}
			if (!description.empty()) {
				hr = psl->SetDescription(description.c_str());
				if (FAILED(hr)) {
					break;
				}
			}
			if (!icon_path.empty()) {
				hr = psl->SetIconLocation(icon_path.c_str(), 0);
				if (FAILED(hr)) {
					break;
				}
			}
			std::wstring save_path(dest_dir);
			save_path = save_path + display_name + L".lnk";
			hr = ppf->Save(save_path.c_str(), TRUE);
		} while (false);
		if (ppf != NULL) {
			ppf->Release();
		}
		if (psl != NULL) {
			psl->Release();
		}
		SetFileAttributes((dest_dir+display_name+L".lnk").c_str(), FILE_ATTRIBUTE_READONLY);
		return SUCCEEDED(hr);
}

bool CShell::GetShortCutInfo(std::wstring strFileName, std::wstring& strTarget, std::wstring& strArguments)
{
	HRESULT hr;
	IShellLink* psl;
	IPersistFile* ppf;
	BOOL bRet = FALSE;
	//Create the ShellLink object

	do 
	{
		hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
			IID_IShellLink, (LPVOID*) &psl);
		if (FAILED(hr)) {
			break;
		}
		hr = psl->QueryInterface( IID_IPersistFile, (LPVOID *) &ppf);
		if (FAILED(hr))	{
			break;
		}
		hr = ppf->Load(strFileName.c_str(), 0);
		if (FAILED(hr))	{
			break;
		}
		WCHAR szBuffer[1024] = {0};
		//Read the target information from the link object
		//UNC paths are supported (SLGP_UNCPRIORITY)
		psl->GetPath(szBuffer, 1024, NULL, SLGP_UNCPRIORITY);
		strTarget = szBuffer;
		ZeroMemory(szBuffer, 1024);
		//Read the arguments from the link object
		psl->GetArguments(szBuffer, 1024);
		strArguments = szBuffer;
		bRet = TRUE;
	} while (false);

	if (ppf != NULL) {
		ppf->Release();
	}
	if (psl != NULL) {
		psl->Release();
	}
	//Return the Target and the Argument as a CString
	return bRet;
}
