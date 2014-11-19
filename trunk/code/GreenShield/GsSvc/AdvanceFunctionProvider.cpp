#include "stdafx.h"
#include "AdvanceFunctionProvider.h"

AdvanceFunctionProvider::AdvanceFunctionProvider() : hModule(NULL), pSHGetKnownFolderPathFunc(NULL)
{
	this->hModule = ::LoadLibrary(L"Shell32.dll");
	if(this->hModule != NULL) {
		this->pSHGetKnownFolderPathFunc = reinterpret_cast<SHGetKnownFolderPathFuncType>(::GetProcAddress(hModule, "SHGetKnownFolderPath"));
	}
}

AdvanceFunctionProvider::SHGetKnownFolderPathFuncType AdvanceFunctionProvider::GetSHGetKnownFolderPathFunctionPtr() const
{
	return this->pSHGetKnownFolderPathFunc;
}

AdvanceFunctionProvider::~AdvanceFunctionProvider()
{
	if(this->hModule != NULL) {
		::FreeLibrary(this->hModule);
	}
	this->pSHGetKnownFolderPathFunc = NULL;
	this->hModule = NULL;
}
