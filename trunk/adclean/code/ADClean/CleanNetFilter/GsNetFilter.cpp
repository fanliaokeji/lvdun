// GsNetFilter.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "GsNetFilter.h"
#include "HttpRequestFilter.h"
#include "TcpProxyServer.h"
#include "./ParseABP/FilterManager.h"
#include <boost/asio.hpp>

#include <process.h>


// This is an example of an exported variable
//GSNETFILTER_API int nGsNetFilter=0;
//
// This is an example of an exported function.
//GSNETFILTER_API int fnGsNetFilter(void)
//{
//	return 42;
//}
//
// This is the constructor of a class that has been exported.
// see GsNetFilter.h for the class definition
//CGsNetFilter::CGsNetFilter()
//{
//	return;
//}

GSNETFILTER_API BOOL GsEnable(BOOL bEnable, USHORT listen_port)
{
	return HttpRequestFilter::GetInstance().Enable(bEnable == FALSE ? false : true, listen_port) ? TRUE : FALSE;
}

namespace {
	unsigned __stdcall  ProxyWorkingThread(void* arg)
	{
		std::auto_ptr<TcpProxyServer> spServer(reinterpret_cast<TcpProxyServer*>(arg));
		spServer->Run();
		return 0;
	}
}

GSNETFILTER_API HANDLE GsStartProxy(USHORT* listen_port)
{
	if(listen_port == NULL) {
		return FALSE;
	}
	std::auto_ptr<TcpProxyServer> spServer(new TcpProxyServer());
	if(!spServer->Open(boost::asio::ip::tcp::v4())) {
		return FALSE;
	}
	boost::asio::ip::address_v4 ip = boost::asio::ip::address_v4::from_string("127.0.0.1");
	unsigned short port = 15868;
	bool bindResult = false;
	for(std::size_t i = 0; i < 1000; ++i, ++port) {
		bindResult = spServer->Bind(ip, port);
		if(bindResult) {
			break;
		}
	}
	if(!bindResult) {
		return FALSE;
	}
	if(!spServer->Listen(boost::asio::ip::tcp::acceptor::max_connections)) {
		return FALSE;
	}
	HANDLE hThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, ProxyWorkingThread, reinterpret_cast<void*>(spServer.get()), 0, NULL));
	if(hThread != NULL) {
		spServer.release();
	}
	*listen_port = port;
	return hThread;
}

BOOL GsSetHook(const std::wstring& dllPath)
{
	HINSTANCE hinstDLL = ::LoadLibrary(dllPath.c_str());
	if(hinstDLL == NULL) {
		return false;
	}
	HOOKPROC hookProc = (HOOKPROC)GetProcAddress(hinstDLL, "GsNetWndProc"); 
	if(hookProc == NULL) {
		return false;
	}
	return ::SetWindowsHookEx(WH_CALLWNDPROC, hookProc, hinstDLL, 0) == NULL ? FALSE : TRUE;
}

VOID GsEnableRedirect(BOOL bEnable)
{
	HttpRequestFilter::GetInstance().EnableRedirect(bEnable == FALSE ? false : true);
}

bool GsUpdateConfigVideoHost(const std::string& url,int istate)
{
	FilterManager* m = FilterManager::getManager();
	if(m == NULL) {
		return false;
	}
	return m->updateConfigVideoHost(url.c_str(), istate);
}

bool GsUpdateConfigWhiteHost(const std::string& url,bool bEnable)
{
	FilterManager* m = FilterManager::getManager();
	if(m == NULL) {
		return false;
	}
	return m->updateConfigWhiteHost(url.c_str(), bEnable);
}

bool GsGetWebRules(const std::wstring& filename)
{
	FilterManager* m = FilterManager::getManager();
	if(m == NULL) {
		return false;
	}
	return m->getWebRules(filename);
}

bool GsGetVideoRules(const std::wstring& filename)
{
	FilterManager* m = FilterManager::getManager();
	if(m == NULL) {
		return false;
	}
	return m->getVideoRules(filename);
}

bool GsGetUsersRules(const std::wstring& filename)
{
	FilterManager* m = FilterManager::getManager();
	if(m == NULL) {
		return false;
	}
	return m->getUsersRules(filename);
}

bool GsGetRedirectRules(const std::wstring& filename)
{
	FilterManager* m = FilterManager::getManager();
	if(m == NULL) {
		return false;
	}
	//return m->getRedirectRules(filename);
}
