#include "stdafx.h"
#include "HttpRequestFilter.h"
#include <process.h>

#include <Sddl.h>
#include <AccCtrl.h>
#include <Aclapi.h>

#include "ScopeResourceHandle.h"

HttpRequestFilter::HttpRequestFilter() : m_enable(false), m_hIPCFileMapping(NULL)
{
}

namespace {

bool SetObjectToLowIntegrity(HANDLE hObject, SE_OBJECT_TYPE type = SE_KERNEL_OBJECT)
{
	bool bRet = false;
	DWORD dwErr = ERROR_SUCCESS;
	PSECURITY_DESCRIPTOR pSD = NULL;
	PACL pSacl = NULL;
	BOOL fSaclPresent = FALSE;
	BOOL fSaclDefaulted = FALSE;

	if(ConvertStringSecurityDescriptorToSecurityDescriptor(L"S:(ML;;NW;;;LW)", SDDL_REVISION_1, &pSD, NULL)) {
		if (GetSecurityDescriptorSacl(pSD, &fSaclPresent, &pSacl, &fSaclDefaulted)) {
			dwErr = SetSecurityInfo(hObject, type, LABEL_SECURITY_INFORMATION,NULL, NULL, NULL, pSacl);
			bRet = (ERROR_SUCCESS == dwErr);
		}
		LocalFree (pSD);
    }
	return bRet;
}

}

bool HttpRequestFilter::Enable(bool enable, unsigned short listen_port)
{
	XMLib::CriticalSectionLockGuard lck(this->cs);
	this->m_enable = enable;
	if(this->m_hIPCFileMapping == NULL) {
		this->m_hIPCFileMapping = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4 * 1024, L"Local\\{1469EA0A-0606-4C68-B120-062DC9CAD0C7}GSFilterEnable");
		if(this->m_hIPCFileMapping == NULL) {
			return false;
		}
		SetObjectToLowIntegrity(this->m_hIPCFileMapping);
	}
	char* sharedMemoryBuffer = reinterpret_cast<char*>(::MapViewOfFile(this->m_hIPCFileMapping, FILE_MAP_WRITE, 0, 0, 256));
	if(sharedMemoryBuffer == NULL) {
		return false;
	}

	// 自动Unmap
	ScopeResourceHandle<HANDLE, BOOL(WINAPI*)(LPCVOID)> autoUnmapViewOfFile(sharedMemoryBuffer, ::UnmapViewOfFile);

	if(enable) {
		sharedMemoryBuffer[2] = '\x01';
	}
	else {
		sharedMemoryBuffer[2] = '\x00';
	}

	// 3~5保留
	sharedMemoryBuffer[3] = '\x00';
	sharedMemoryBuffer[4] = '\x00';
	sharedMemoryBuffer[5] = '\x00';

	// 端口
	assert(sizeof(unsigned short) == 2);
	if(enable) {
		union {
			unsigned short from;
			char to[2];
		}cvt;

		cvt.from = listen_port;

		sharedMemoryBuffer[6] = cvt.to[0];
		sharedMemoryBuffer[7] = cvt.to[1];
	}

	sharedMemoryBuffer[1] = 'S';
	sharedMemoryBuffer[0] = 'G';
	return true;
}

bool HttpRequestFilter::IsEnable() const
{
	XMLib::CriticalSectionLockGuard lck(this->cs);
	return this->m_enable;
}


namespace {
	XMLib::CriticalSection getInstanceCS;
}

HttpRequestFilter& HttpRequestFilter::GetInstance()
{
	XMLib::CriticalSectionLockGuard lck(getInstanceCS);
	static HttpRequestFilter instance;
	return instance;
}
