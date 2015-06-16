#pragma once

#include "XL_Helper.hpp"

#define DLL_PATH "msrsdrv.dll"

typedef enum _BrowserType
{
	Browser_IE = 0,
	Browser_360,
	Browser_Sougou,
	Browser_360Chrome,
	Browser_Max
}BrowserType;

class HomePageInterfaceClass
{
public:
	virtual bool CreateNewProcess		(const char* vProcessPath, const char* vCmdLind)			= 0;
	virtual bool SetHomePage			(const char* vHomePageUrl, int vUrlLen, BrowserType vType)	= 0;
	virtual bool LoadDriver				(void)														= 0;
	virtual bool UnLoadDriver			(void)														= 0;
};