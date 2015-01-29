// storage.cpp : Cstorage 的实现

#include "stdafx.h"
#include "storage.h"
#include "atlstr.h"
#include "md5.h"
#include <fstream>
#include "shlobj.h"
#include "setupapi.h" 
#include "msxml.h"
#pragma comment (lib, "setupapi.lib")

// Cstorage


STDMETHODIMP Cstorage::GetFolderPath(long h, long h2 ,long h3 ,long h4 , BSTR* bstrPath)
{
	TSAUTO();
	TCHAR szPath[_MAX_PATH] = {0};
	::SHGetFolderPathW((HWND)(LONG_PTR)h, (int)h2, (HANDLE)(LONG_PTR)h3, (DWORD)h4, (LPTSTR)szPath);
	CComBSTR b = szPath;
	*bstrPath = b.Detach();
	return S_OK;
}

static TCHAR* pszTargetDir = NULL;
static LRESULT WINAPI CabinetCallback( IN PVOID /*pMyInstallData*/, IN UINT Notification, IN UINT Param1, IN UINT /*Param2*/ )
{
	LRESULT lRetVal = NO_ERROR;
	if(pszTargetDir == NULL)
		return lRetVal;
	TCHAR szTarget[MAX_PATH] = {0};
	FILE_IN_CABINET_INFO *pInfo = NULL;
	FILEPATHS *pFilePaths = NULL;

	lstrcpy(szTarget, pszTargetDir);

	switch(Notification)
	{
	case SPFILENOTIFY_FILEINCABINET: 
		pInfo = (FILE_IN_CABINET_INFO *)(ULONG_PTR)Param1;
		lstrcat(szTarget, pInfo->NameInCabinet);
		lstrcpy(pInfo->FullTargetName, szTarget);

		PathRemoveFileSpec(szTarget);
		if(!PathFileExists(szTarget))
			SHCreateDirectory(NULL, szTarget);
		TSDEBUG4CXX(" Extract File Path : "<<szTarget);
		lRetVal = FILEOP_DOIT;  // Extract the file.
		break;

	case SPFILENOTIFY_FILEEXTRACTED:
		pFilePaths = (FILEPATHS *)(ULONG_PTR)Param1;
		lRetVal = NO_ERROR;
		break;

	case SPFILENOTIFY_NEEDNEWCABINET: // Unexpected.
		lRetVal = NO_ERROR;
		break;
	}

	return lRetVal;
}

STDMETHODIMP Cstorage::ExtractCab(BSTR bstrSrc, BSTR bstrDest, VARIANT_BOOL* pvbRet)
{
	TSAUTO();
	*pvbRet = VARIANT_FALSE;
	TCHAR tszSrcShortPath[2048] = {0};
	::PathCanonicalize(tszSrcShortPath, bstrSrc);
	TCHAR tszSrcPath[2048] = {0};
	::ExpandEnvironmentStrings(tszSrcShortPath, tszSrcPath, MAX_PATH);
	ATLASSERT(::PathFileExists(tszSrcPath));
	if(!::PathFileExists(tszSrcPath))
	{
		return S_FALSE;
	}
	TCHAR tszDestShortPath[2048] = {0};
	::PathCanonicalize(tszDestShortPath, bstrDest);
	TCHAR tszDestPath[2048] = {0};
	::ExpandEnvironmentStrings(tszDestShortPath, tszDestPath, MAX_PATH);
	pszTargetDir = tszDestPath;
	if(SetupIterateCabinet(bstrSrc, 0, (PSP_FILE_CALLBACK)CabinetCallback, NULL))
	{
		*pvbRet = VARIANT_TRUE;
	} 
	return S_OK;
}

using namespace std;
STDMETHODIMP Cstorage::GetFileMD5(BSTR bstrFile, BSTR* pbstrMD5)
{
	//	TSAUTO();
	TCHAR tszShortPath[2048] = {0};
	::PathCanonicalize(tszShortPath, bstrFile);
	TCHAR tszPath[2048] = {0};
	::ExpandEnvironmentStrings(tszShortPath, tszPath, MAX_PATH);
	if(!::PathFileExists(tszPath))
		return S_FALSE;	
	std::ifstream fs;
	fs.open( CStringA(tszPath), ios::in|ios::binary );  
	if(fs.fail())
		return S_FALSE;
	MD5 md5(fs);
	std::string strMD5 = md5.toString();
	CComBSTR bstrMD5(strMD5.c_str());
	bstrMD5.ToUpper();
	TSDEBUG4CXX("md5: " << bstrMD5.m_str << ", file=" << tszPath);	
	*pbstrMD5 = bstrMD5.Detach();

	return S_OK;
}
STDMETHODIMP Cstorage::PathCanonicalize(BSTR path, BSTR* pdest)
{
	// TODO: 在此添加实现代码
	TCHAR tszPath[MAX_PATH] = {0};
	::PathCanonicalize(tszPath, path);
	CComBSTR bstr(tszPath);
	*pdest = bstr.Detach();
	return S_OK;
}

STDMETHODIMP Cstorage::ExpandEnvironmentStrings(BSTR path, BSTR* pdest)
{
	// TODO: 在此添加实现代码

	TCHAR tszPath[MAX_PATH] = {0};
	::ExpandEnvironmentStrings(path, tszPath, MAX_PATH);
	CComBSTR bstr(tszPath);
	*pdest = bstr.Detach();

	return S_OK;
}
static HKEY GetRootKey(CComBSTR bstrKey)
{
	static std::map<CComBSTR, HKEY> rootKeyMap;
	if(rootKeyMap.empty())
	{
		rootKeyMap["HKEY_CLASSES_ROOT"] = HKEY_CLASSES_ROOT;
		rootKeyMap["HKEY_CURRENT_USER"] = HKEY_CURRENT_USER;
		rootKeyMap["HKEY_LOCAL_MACHINE"] = HKEY_LOCAL_MACHINE;
		rootKeyMap["HKEY_USERS"] = HKEY_USERS;
	}
	bstrKey.ToUpper();
	return rootKeyMap[bstrKey];
}

STDMETHODIMP Cstorage::RegQueryValue(BSTR bstrRootName, BSTR bstrKeyName, BSTR bstrValueName, VARIANT* varRet)
{
	// TODO: 在此添加实现代码

	TSAUTO();

	CRegKey key;
	LONG ret = key.Open(GetRootKey(bstrRootName), bstrKeyName, KEY_QUERY_VALUE);
	if(ERROR_SUCCESS != ret)
		return S_FALSE;

	DWORD dwType = 0;
	ULONG nBytes;
	ret = key.QueryValue(bstrValueName,&dwType,NULL,&nBytes);
	if(ERROR_SUCCESS != ret)
		return S_FALSE;

	VariantInit(varRet);
	if (dwType == REG_SZ)
	{
		WCHAR szVal[4096] = {0};
		ULONG len = sizeof(szVal)/sizeof(szVal[0])-1;

		ret = key.QueryStringValue(bstrValueName, szVal, &len);
		if(ERROR_SUCCESS != ret)
			return S_FALSE;

		CComBSTR bstrVal(szVal);
		varRet->vt = VT_BSTR;
		varRet->bstrVal = bstrVal.Detach();
	}
	else if(dwType == REG_DWORD)
	{
		DWORD dwValue = 0;
		ret = key.QueryDWORDValue(bstrValueName, dwValue);
		if(ERROR_SUCCESS != ret)
			return S_FALSE;

		varRet->vt = VT_I4;
		varRet->intVal = dwValue;
	}
	else
	{
		return S_FALSE;
	}

	return S_OK;
}

STDMETHODIMP Cstorage::RegSetValue(BSTR bstrRootName, BSTR bstrKeyName, BSTR bstrValueName, VARIANT varVal)
{
	TSAUTO();

	CRegKey key;
	LONG ret = key.Create(GetRootKey(bstrRootName), bstrKeyName);
	if(ERROR_SUCCESS != ret)
		return S_FALSE;

	if (varVal.vt == VT_BSTR)
	{
		ret = key.SetStringValue(bstrValueName, varVal.bstrVal);
		if(ERROR_SUCCESS != ret)
			return S_FALSE;
	}
	else if(varVal.vt == VT_I4)
	{
		ret = key.SetDWORDValue(bstrValueName, (DWORD)varVal.intVal);
		if(ERROR_SUCCESS != ret)
			return S_FALSE;
	}
	else
	{
		return S_FALSE;
	}

	return S_OK;
}

STDMETHODIMP Cstorage::RegDeleteValue(BSTR bstrRootName, BSTR bstrKeyName, BSTR bstrValueName)
{
	TSAUTO();

	CRegKey key;
	LONG ret = key.Open(GetRootKey(bstrRootName), bstrKeyName, KEY_WRITE);
	if(ERROR_SUCCESS != ret)
		return S_FALSE;

	ret = key.DeleteValue(bstrValueName);
	if(ERROR_SUCCESS != ret)
		return S_FALSE;

	return S_OK;
}


STDMETHODIMP Cstorage::GetPrivateProfileString(BSTR bstrPath, BSTR bstrApp, BSTR bstrKey, BSTR* pbstrValue)
{
	// TODO: 在此添加实现代码
	TSAUTO();
	TCHAR tszShortPath[MAX_PATH] = {0};
	::PathCanonicalize(tszShortPath, bstrPath);
	TCHAR tszPath[MAX_PATH] = {0};
	::ExpandEnvironmentStrings(tszShortPath, tszPath, MAX_PATH);
	wchar_t szValue[2048] = {0};
	::GetPrivateProfileString(bstrApp, bstrKey, _T(""), szValue, 2048, tszPath);
	*pbstrValue = CComBSTR(szValue).Detach();
	return S_OK;
}

STDMETHODIMP Cstorage::WritePrivateProfileValue(BSTR bstrPath, BSTR bstrApp, BSTR bstrKey, VARIANT vValue)
{
	// TODO: 在此添加实现代码
	TSAUTO();
	TCHAR tszShortPath[MAX_PATH] = {0};
	::PathCanonicalize(tszShortPath, bstrPath);
	TCHAR tszPath[MAX_PATH] = {0};
	::ExpandEnvironmentStrings(tszShortPath, tszPath, MAX_PATH);
	if(vValue.vt == VT_I4)
	{
		wchar_t szValue[256] = {0};
		_i64tow(vValue.lVal, szValue, 10);
		::WritePrivateProfileString(bstrApp, bstrKey, szValue, tszPath);
	}
	else if(vValue.vt == VT_BSTR)
	{
		::WritePrivateProfileString(bstrApp, bstrKey, vValue.bstrVal, tszPath);
	}

	return S_OK;
}

STDMETHODIMP Cstorage::PathFileExists(BSTR strFilePath, LONG* b)
{
	// TODO: 在此添加实现代码
	TCHAR tszShortPath[MAX_PATH] = {0};
	::PathCanonicalize(tszShortPath, strFilePath);
	TCHAR tszPath[MAX_PATH] = {0};
	::ExpandEnvironmentStrings(tszShortPath, tszPath, MAX_PATH);
	for (int i = 0 ; tszPath[i] != '\0' ; i++)
	{
		if(tszPath[i] == '/')
		{
			tszPath[i] = '\\';
		}
	}
	*b = ::PathFileExists(tszPath) ? -1 : VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP Cstorage::Save(IStream* stream, BSTR path, LONG* ret)
{
	// TODO: 在此添加实现代码
	TSAUTO();
	CComQIPtr<IStream> pStream(stream);
	if(pStream)
	{
		wchar_t szPath[_MAX_PATH] = {0};
		wcsncpy(szPath, path, _MAX_PATH);
		PathRemoveFileSpec(szPath);
		if(!::PathFileExists(szPath))
			SHCreateDirectory(NULL, szPath);
		HANDLE hFile = ::CreateFile((LPCTSTR)path,GENERIC_WRITE,0,
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			TSERROR4CXX("error to write file "<<path);
			*ret = 0;
		}
		else
		{
			DWORD cb, cbRead, cbWritten;
			BYTE bBuffer[100*1024] = {0};
			cb = sizeof bBuffer;
			HRESULT hr = pStream->Read(bBuffer, cb, &cbRead);
			while (SUCCEEDED(hr) && 0 != cbRead)
			{
				if (!WriteFile(hFile, bBuffer, cbRead, &cbWritten, NULL))
					break;
				hr = pStream->Read(bBuffer, cb, &cbRead);
			}
			CloseHandle(hFile);
			*ret = 1;
		}
	}
	return S_OK;
}

STDMETHODIMP Cstorage::CreateFileMapping(LONG flag, LONG size, BSTR name, LONG* h)
{
	// TODO: 在此添加实现代码
	TSAUTO();
	*h = (LONG)(LONG_PTR)::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, flag, 0, size, name);

	return S_OK;
}

STDMETHODIMP Cstorage::WriteFileMapping(LONG h, BSTR buf, LONG* ret)
{
	// TODO: 在此添加实现代码
	TSAUTO();
	LONG lsize = (LONG) wcslen(buf );
	LPWSTR lpMapAddress = NULL;
	lpMapAddress =(LPWSTR)(LPVOID) MapViewOfFile((HANDLE)(LONG_PTR)h, FILE_MAP_WRITE,0,0,lsize+1);
	if(lpMapAddress)
	{
		wcscpy((LPWSTR)lpMapAddress, buf);
		lpMapAddress[lsize] = 0;
		UnmapViewOfFile(lpMapAddress);	
		*ret = 1;
	}
	return S_OK;
}

STDMETHODIMP Cstorage::ReadFileMapping(LONG h, LONG size, BSTR* ret)
{
	// TODO: 在此添加实现代码
	TSAUTO();
	LPWSTR lpMapAddress = NULL;
	lpMapAddress =(LPWSTR)(LPVOID) MapViewOfFile((HANDLE)(LONG_PTR)h, FILE_MAP_READ,0,0,size);
	if(lpMapAddress)
	{
		CComBSTR bstr(lpMapAddress);
		*ret = bstr.Detach();
		UnmapViewOfFile(lpMapAddress);	
	}
	return S_OK;
}

STDMETHODIMP Cstorage::OpenFileMapping(LONG flag, LONG inherit, BSTR name, LONG* ret)
{
	// TODO: 在此添加实现代码
	*ret = (LONG)(LONG_PTR)::OpenFileMapping(flag, inherit, name);
	return S_OK;
}
