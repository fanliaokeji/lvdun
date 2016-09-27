/************************************************************************************************************************************
1、判断是否关联
FileAssociation* pfa = FileAssociation::Instance();
UINT flag = pfa->Associated(L".jpg");
if(flag&ProgID != 0 && flag&RootKeyExist != 0){
	cout<<"已经关联了"<<endl;
}
2、设置单个扩展名关联
FileAssociation::Instance()->SetAssociate(L".jpg", TRUE, TRUE);
FileAssociation::Instance()->Update();

3、设置多个扩展名关联
FileAssociation::Instance()->AssociateAll(L".jpg;.png;.bmp;", TRUE, TRUE);
FileAssociation::Instance()->Update();
************************************************************************************************************************************/
#pragma once

class RegTool{
	public:
	RegTool(){};
	~RegTool(){m_key.Close();};
	BOOL IsWow64();
	void QueryRegValue(HKEY root,const wchar_t* szRegPath,const wchar_t* szKey, std::wstring* pRet, REGSAM flag =  KEY_READ);
	void QueryRegValue(HKEY root,const wchar_t* szRegPath,const wchar_t* szKey, DWORD* pRet, REGSAM flag =  KEY_READ);
	BOOL DeleteRegKey(HKEY root, const wchar_t* szRegPath, const wchar_t* szKey, REGSAM flag = KEY_ALL_ACCESS);
	BOOL DeleteRegValue(HKEY root, const wchar_t* szRegPath, const wchar_t* szValue, REGSAM flag = KEY_ALL_ACCESS);
	void SetRegValue(HKEY hk, const wchar_t* szRegPath, const wchar_t* szKey, const wchar_t* value, BOOL bWow64 = FALSE);
	void SetRegValue(HKEY hk, const wchar_t* szRegPath, const wchar_t* szKey, const DWORD value, BOOL bWow64 = FALSE);
	ATL::CRegKey * operator->(){
		return &m_key;
	};
	private:
	ATL::CRegKey m_key;
};

#include <AccCtrl.h>
#include <AclAPI.h>
namespace RegSecurity{
	BOOL SetRegSecurity(LPCSTR lpszRegPath, LPCSTR lpszAccessDesireds);
};

typedef enum tagAssociateType{
	None								=	0X0000000,
	ProgID								=	0X0000001,
	Application							=	0X0000002,
	ClassRoot							=	0X0000004,
	ClassRootOpenWithList				=	0X0000008,
	ClassRootOpenWithProgIds			=	0X0000010,
	CurrentUserOpenWithProgIds			=	0X0000020,
	RootKeyExist						=	0X0000040,
	ContextMenuExist					=	0X0000080,
	CrrentUserIsMeWin10					=	0X0000100,
	DefaultIsNoOpenWith					=	0X0000200,
	ProgIDIsMEWin10						=	0X0000400,
}AssociateType;

class FileAssociation{
	public:
		~FileAssociation(){};
		static FileAssociation* Instance();
		UINT Associated(const std::wstring strFileExt);
		void SetAssociate(std::wstring strFileExt, BOOL bAssociate = TRUE, BOOL bHasAdmin = FALSE);
		void AssociateAll(std::wstring strFileExts, BOOL bAssociate = TRUE, BOOL bHasAdmin = FALSE);
		void CreateImgKeyALL(std::wstring strFileExts, BOOL bDo = TRUE);
		void Update(){SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST , 0, 0);};
	private:
		FileAssociation(){};
		void CreateImgKey(const std::wstring& strFileExt);
		std::wstring GetIconPath(const std::wstring& strFileExt);
		std::wstring GetCommandOpenPath();
		void StringReplace(std::wstring& src,const std::wstring& oldstr, const std::wstring& newstr);
		BOOL IsVistaOrHigher();
};

class FileAssociationWin10{
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



namespace FileAssociationWarpper{
	void SetAssociate2();
	void SetAssociate1(const wchar_t* szDo, const wchar_t* szUnDo, DWORD NoUpdate);
};