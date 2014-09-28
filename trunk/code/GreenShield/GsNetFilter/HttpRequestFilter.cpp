#include "stdafx.h"
#include "HttpRequestFilter.h"
#include <process.h>

#include <Sddl.h>
#include <AccCtrl.h>
#include <Aclapi.h>

#include "ScopeResourceHandle.h"

HttpRequestFilter::HttpRequestFilter() : m_enable(false), m_hThread(NULL), m_hEvent(NULL), m_hIPCFileMapping(NULL)
{
}

bool HttpRequestFilter::FilterUrl(const std::string& url)
{
	XMLib::CriticalSectionLockGuard lck(this->cs);
	if(!this->m_enable) {
		return false;
	}

	for(DomainFilterRulesMapType::iterator iter = this->m_domainFilterRules.begin(); iter != this->m_domainFilterRules.end(); ++iter) {
		if(iter->second.first == DFS_WHITE) {
			continue;
		}
		for(std::vector<boost::regex>::iterator regex_iter = iter->second.second.begin(); regex_iter != iter->second.second.end(); ++regex_iter) {
			if(boost::regex_match(url.begin(), url.end(), *regex_iter)) {
				if(iter->second.first == DFS_BLACK) {
					// 发送通知
					this->PostNotify(NT_FILTERRESULT, url);
					return true;
				}
				else {
					assert(iter->second.first == DFS_UNSET);
					// 发送通知的同时将其加入白名单 避免频繁发送ASK
					iter->second.first = DFS_WHITE;
					this->PostNotify(NT_FILTERASK, iter->first);
					return false;
				}
			}
		}
	}
	return false;
}

bool HttpRequestFilter::AddDomain(const std::string& domain)
{
	XMLib::CriticalSectionLockGuard lck(this->cs);
	return this->m_domainFilterRules.insert(std::make_pair<std::string, std::pair<DomainFilterState, std::vector<boost::regex> > >(domain, std::make_pair<DomainFilterState, std::vector<boost::regex> >(DFS_UNSET, std::vector<boost::regex>()))).second;
}

bool HttpRequestFilter::AddFilterRule(const std::string& domain, const boost::regex& re)
{
	XMLib::CriticalSectionLockGuard lck(this->cs);
	DomainFilterRulesMapType::iterator domain_iter = this->m_domainFilterRules.find(domain);
	if(domain_iter == this->m_domainFilterRules.end()) {
		return false;
	}
	domain_iter->second.second.push_back(re);
	return true;
}

bool HttpRequestFilter::EnableDomain(const std::string& domain, bool enable)
{
	XMLib::CriticalSectionLockGuard lck(this->cs);
	DomainFilterRulesMapType::iterator domain_iter = this->m_domainFilterRules.find(domain);
	if(domain_iter == this->m_domainFilterRules.end()) {
		return false;
	}
	if(enable) {
		domain_iter->second.first = DFS_BLACK;
	}
	else {
		domain_iter->second.first = DFS_WHITE;
	}
	return true;
}

namespace {
	bool UTF8ToUTF16(const std::string& utf8_str, std::wstring& utf16_str)
	{
		if(utf8_str.empty()) {
			utf16_str.clear();
			return true;
		}
		int length = MultiByteToWideChar(CP_UTF8, NULL, utf8_str.data(), static_cast<int>(utf8_str.size()), NULL, 0);
		if(length == 0) {
			false;
		}
		utf16_str.resize(length);
		MultiByteToWideChar(CP_UTF8, NULL, utf8_str.data(), static_cast<int>(utf8_str.size()), const_cast<wchar_t*>(utf16_str.data()), length);
		return true;
	}

	bool UTF16ToUTF8(const std::wstring& utf16_str, std::string& utf8_str)
	{
		if(utf16_str.empty()) {
			utf8_str.clear();
			return true;
		}
		int length = WideCharToMultiByte(CP_UTF8, 0, utf16_str.data(), static_cast<int>(utf16_str.size()), NULL, 0, NULL, NULL);
		if(length == 0) {
			false;
		}
		utf8_str.resize(length);
		WideCharToMultiByte(CP_UTF8, 0, utf16_str.data(), static_cast<int>(utf16_str.size()), const_cast<char*>(utf8_str.data()), length, NULL, NULL);
		return true;
	}
}

bool HttpRequestFilter::AddDomain(const std::wstring& domain)
{
	std::string domain_utf8;
	if(!UTF16ToUTF8(domain, domain_utf8)) {
		return false;
	}
	return this->AddDomain(domain_utf8);
}

bool HttpRequestFilter::AddFilterRule(const std::wstring& domain, const std::wstring& pattern)
{
	std::string domain_utf8, pattern_utf8;
	if(!UTF16ToUTF8(domain, domain_utf8) || !UTF16ToUTF8(pattern, pattern_utf8)) {
		return false;
	}
	try {
		return this->AddFilterRule(domain_utf8, boost::regex(pattern_utf8));
	}
	catch(boost::regex_error&) {
		return false;
	}
}

bool HttpRequestFilter::EnableDomain(const std::wstring& domain, bool enable)
{
	std::string domain_utf8;
	if(!UTF16ToUTF8(domain, domain_utf8)) {
		return false;
	}
	return this->EnableDomain(domain_utf8, enable);
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

bool HttpRequestFilter::Enable(bool enable)
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
	sharedMemoryBuffer[1] = 'S';
	sharedMemoryBuffer[0] = 'G';
	return true;
}

bool HttpRequestFilter::PostNotify(NotifyType nt, const std::string& str)
{
	if(this->m_hEvent == NULL) {
		this->m_hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		if(this->m_hEvent == NULL) {
			return false;
		}
		this->m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, &HttpRequestFilter::SendNotifyThreadProc, reinterpret_cast<void*>(this), 0, NULL));
		if(this->m_hThread == NULL) {
			::CloseHandle(this->m_hEvent);
			this->m_hEvent = NULL;
		}
	}
	assert(this->m_hThread != NULL);
	if(this->m_notifyQueue.size() > 500) {
		return false;
	}
	this->m_notifyQueue.push_back(std::make_pair(nt, str));
	::SetEvent(this->m_hEvent);
	return true;
}

unsigned __stdcall HttpRequestFilter::SendNotifyThreadProc(void *arg)
{
	reinterpret_cast<HttpRequestFilter*>(arg)->SendNotifyLoop();
	return 0;
}

void HttpRequestFilter::SendNotifyLoop()
{
	NotifyType nt;
	std::string notifyStr;
	std::wstring utf16NotifyStr;
	for(;;) {
		bool isEmpty = false;
		{
			XMLib::CriticalSectionLockGuard lck(this->cs);
			// 判断是否取消

			if(this->m_notifyQueue.empty()) {
				isEmpty = true;
				::ResetEvent(this->m_hEvent);
			}
			else {
				nt = this->m_notifyQueue.front().first;
				notifyStr = this->m_notifyQueue.front().second;
				this->m_notifyQueue.pop_front();
			}
		}
		if(isEmpty) {
			::WaitForSingleObject(this->m_hEvent, INFINITE);
		}
		else {
			utf16NotifyStr.clear();
			if(UTF8ToUTF16(notifyStr, utf16NotifyStr)) {
				HWND hNotifyWnd = ::FindWindow(L"{B239B46A-6EDA-4a49-8CEE-E57BB352F933}_dsmainmsg", NULL);
				if(hNotifyWnd != NULL) {
					// #define WM_FILTERRESULT WM_USER + 201
					// #define WM_FILTERASK WM_USER + 202
					if(nt == NT_FILTERASK) {
						::SendMessage(hNotifyWnd, WM_USER + 202, WPARAM(1), LPARAM(utf16NotifyStr.c_str()));
					}
					else {
						// NT_FILTERRESULT
						::SendMessage(hNotifyWnd, WM_USER + 201, WPARAM(1), LPARAM(utf16NotifyStr.c_str()));
					}
				}
			}
		}
	}
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
