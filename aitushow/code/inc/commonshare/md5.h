#pragma once
#include <string>
#include <Windows.h>
#include <Wincrypt.h>
inline BOOL GetMd5(std::wstring file,WCHAR * pszFileMd5)  
{  
	HANDLE hFile=CreateFile(file.c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
	if (hFile==INVALID_HANDLE_VALUE) 
	{   
		CloseHandle(hFile);
		return FALSE;
	}
	HCRYPTPROV hProv=NULL;
	if(CryptAcquireContext(&hProv,NULL,NULL,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT)==FALSE)       //获得CSP中一个密钥容器的句柄
	{
		return FALSE;
	}
	HCRYPTPROV hHash=NULL;
	if(CryptCreateHash(hProv,CALG_MD5,0,0,&hHash)==FALSE)
	{
		return FALSE;
	}
	DWORD dwFileSize=GetFileSize(hFile,0);  
	if (dwFileSize==0xFFFFFFFF)        
	{
		return FALSE;
	}
	byte* lpReadFileBuffer=new byte[dwFileSize];
	DWORD lpReadNumberOfBytes;
	if (ReadFile(hFile,lpReadFileBuffer,dwFileSize,&lpReadNumberOfBytes,NULL)==0)        //读取文件  
	{
		return FALSE;
	}
	if(CryptHashData(hHash,lpReadFileBuffer,lpReadNumberOfBytes,0)==FALSE)      //hash文件  
	{
		return FALSE;
	}
	delete[] lpReadFileBuffer;
	CloseHandle(hFile);          //关闭文件句柄
	BYTE *pbHash;
	DWORD dwHashLen=sizeof(DWORD);
	if (!CryptGetHashParam(hHash,HP_HASHVAL,NULL,&dwHashLen,0)) //我也不知道为什么要先这样调用CryptGetHashParam，这块是参照的msdn       
	{
		return FALSE;
	}
	pbHash=(byte*)malloc(dwHashLen);
	if(CryptGetHashParam(hHash,HP_HASHVAL,pbHash,&dwHashLen,0) && dwHashLen <= 16)//获得md5值 
	{
		std::wstring strFileMd5;
		for(DWORD i=0;i<dwHashLen;i++)         //输出md5值 
		{
			TCHAR str[3]={0};
			std::wstring strFilePartM=_T("");
			_stprintf(str,_T("%02X"),pbHash[i]);
			strFileMd5+=str;
		}
		wcscpy(pszFileMd5,strFileMd5.c_str());
	} 

	//善后工作
	if(CryptDestroyHash(hHash)==FALSE)          //销毁hash对象  
	{
		return FALSE;
	}
	if(CryptReleaseContext(hProv,0)==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}



inline BOOL GetStringMd5(std::string str,WCHAR * pszStrMd5)  
{  
	HCRYPTPROV hProv=NULL;
	if(CryptAcquireContext(&hProv,NULL,NULL,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT)==FALSE)       //获得CSP中一个密钥容器的句柄
	{
		return FALSE;
	}
	HCRYPTPROV hHash=NULL;
	if(CryptCreateHash(hProv,CALG_MD5,0,0,&hHash)==FALSE)
	{
		return FALSE;
	}
	if(CryptHashData(hHash,(LPBYTE)str.c_str(),str.size(),0)==FALSE)      //hash文件  
	{
		return FALSE;
	}

	BYTE *pbHash;
	DWORD dwHashLen=sizeof(DWORD);
	if (!CryptGetHashParam(hHash,HP_HASHVAL,NULL,&dwHashLen,0)) //我也不知道为什么要先这样调用CryptGetHashParam，这块是参照的msdn       
	{
		return FALSE;
	}
	pbHash=(byte*)malloc(dwHashLen);
	if(CryptGetHashParam(hHash,HP_HASHVAL,pbHash,&dwHashLen,0) && dwHashLen <= 16)//获得md5值 
	{
		std::wstring strMd5;
		for(DWORD i=0;i<dwHashLen;i++)         //输出md5值 
		{
			TCHAR str[3]={0};
			std::wstring strFilePartM=_T("");
			_stprintf(str,_T("%02X"),pbHash[i]);
			strMd5+=str;
		}
		wcscpy(pszStrMd5,strMd5.c_str());
	} 

	//善后工作
	if(CryptDestroyHash(hHash)==FALSE)          //销毁hash对象  
	{
		return FALSE;
	}
	if(CryptReleaseContext(hProv,0)==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}