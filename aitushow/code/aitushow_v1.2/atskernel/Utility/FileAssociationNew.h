/************************************************************************************************************************************
1、判断是否关联
FileAssociationNew* pfa = FileAssociationNew::Instance();
UINT flag = pfa->Associated(L".jpg");
if(flag&ProgID != 0 && flag&RootKeyExist != 0){
	cout<<"已经关联了"<<endl;
}
2、设置单个扩展名关联
FileAssociationNew::Instance()->SetAssociate(L".jpg", TRUE, TRUE);
FileAssociationNew::Instance()->Update();

3、设置多个扩展名关联
FileAssociationNew::Instance()->AssociateAll(L".jpg;.png;.bmp;", TRUE, TRUE);
FileAssociationNew::Instance()->Update();
************************************************************************************************************************************/
#pragma once
#include "FileAssociation.h"

class FileAssociationNew{
	public:
		~FileAssociationNew(){};
		static FileAssociationNew* Instance();
		UINT Associated(const std::wstring strFileExt);
		void SetAssociate(std::wstring strFileExt, BOOL bAssociate = TRUE, BOOL bHasAdmin = FALSE);
		void AssociateAll(std::wstring strFileExts, BOOL bAssociate = TRUE, BOOL bHasAdmin = FALSE);
		void CreateImgKeyALL(std::wstring strFileExts, BOOL bDo = TRUE);
		void Update(){SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST , 0, 0);};
	private:
		FileAssociationNew(){};
		void CreateImgKey(const std::wstring& strFileExt);
		std::wstring GetIconPath(const std::wstring& strFileExt);
		std::wstring GetCommandOpenPath();
		void StringReplace(std::wstring& src,const std::wstring& oldstr, const std::wstring& newstr);
		BOOL IsVistaOrHigher();
};

class FileAssociationNewWin10{
public:
	static UINT Associated(const std::wstring strFileExt);
	static void SetAssociate(std::wstring strFileExt, BOOL bAssociate = TRUE, BOOL bHasAdmin = FALSE);
	static BOOL GetNtVersionNumbers(OSVERSIONINFOEX& osVersionInfoEx)
	{
		BOOL bRet= FALSE;
		HMODULE hModNtdll= ::LoadLibraryW(L"ntdll.dll");
		if (hModNtdll)
		{
			typedef void (WINAPI *pfRTLGETNTVERSIONNUMBERS)(DWORD*,DWORD*, DWORD*);
			pfRTLGETNTVERSIONNUMBERS pfRtlGetNtVersionNumbers;
			pfRtlGetNtVersionNumbers = (pfRTLGETNTVERSIONNUMBERS)::GetProcAddress(hModNtdll, "RtlGetNtVersionNumbers");
			if (pfRtlGetNtVersionNumbers)
			{
				pfRtlGetNtVersionNumbers(&osVersionInfoEx.dwMajorVersion, &osVersionInfoEx.dwMinorVersion,&osVersionInfoEx.dwBuildNumber);
				osVersionInfoEx.dwBuildNumber&= 0x0ffff;
				bRet = TRUE;
			}

			::FreeLibrary(hModNtdll);
			hModNtdll = NULL;
		}

		return bRet;
	};

	static BOOL IsWin8D1()
	{
		OSVERSIONINFOEX osvi;
		ZeroMemory(&osvi,sizeof(OSVERSIONINFOEX));
		if (!GetNtVersionNumbers(osvi))
		{
			return FALSE;
		}
		return osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3;
	}
	static BOOL IsWin10(){
		OSVERSIONINFOEX osvi;
		ZeroMemory(&osvi,sizeof(OSVERSIONINFOEX));
		if (!GetNtVersionNumbers(osvi))
		{
			return FALSE;
		}
		return osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0;
	};
};



namespace FileAssociationNewWarpper{
	void SetAssociate2();
	void SetAssociate1(const wchar_t* szDo, const wchar_t* szUnDo, DWORD NoUpdate);
};