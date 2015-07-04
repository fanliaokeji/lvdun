// DatFileUtility.h: interface for the CFileEncryption class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_XLCMDDATA_H__0304EEA6_A990_4EBC_97C1_879BC7BE77E4__INCLUDED_)
#define AFX_XLCMDDATA_H__0304EEA6_A990_4EBC_97C1_879BC7BE77E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef unsigned int     uint32;

class CDatFileUtility  
{
public:
	CDatFileUtility();
	virtual ~CDatFileUtility();

public:
	void ReadFileToString(std::wstring strSrcFile, std::string &strFileData, DWORD &dwByteRead, DWORD dwMaxFileSize = 0);
	bool WriteStringToFile(std::wstring strDesFile, const char* pFileData, DWORD dwLen);

public:
	static CDatFileUtility &Instance()
	{
		static CDatFileUtility *s_instance = NULL;
		if (s_instance == NULL)
		{
			s_instance = new CDatFileUtility;
		}
		return *s_instance;
	}
};

#define tipWndDatFileUtility CDatFileUtility::Instance()

#endif // !defined(AFX_XLCMDDATA_H__0304EEA6_A990_4EBC_97C1_879BC7BE77E4__INCLUDED_)
