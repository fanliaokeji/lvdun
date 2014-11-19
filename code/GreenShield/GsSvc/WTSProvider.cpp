#include "stdafx.h"
#include "WTSProvider.h"

WTSProvider::WTSProvider() : hModule(NULL), pWTSQueryUserTokenFunc(NULL)
{
	this->hModule = ::LoadLibrary(L"Wtsapi32.dll");
	if(this->hModule != NULL) {
		this->pWTSQueryUserTokenFunc = reinterpret_cast<WTSQueryUserTokenFuncType>(::GetProcAddress(hModule, "WTSQueryUserToken"));
	}
}

WTSProvider::WTSQueryUserTokenFuncType WTSProvider::GetWTSQueryUserTokenFunctionPtr() const
{
	return this->pWTSQueryUserTokenFunc;
}

WTSProvider::~WTSProvider()
{
	if(this->hModule != NULL) {
		::FreeLibrary(this->hModule);
	}
	this->pWTSQueryUserTokenFunc = NULL;
	this->hModule = NULL;
}
