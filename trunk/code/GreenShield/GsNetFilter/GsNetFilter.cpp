// GsNetFilter.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "GsNetFilter.h"
#include "HttpRequestFilter.h"
#include "TcpProxyServer.h"
#include <boost/asio.hpp>


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
GSNETFILTER_API void GsAddDomain(const std::wstring& domain, const std::vector<std::wstring> rules)
{
	HttpRequestFilter::GetInstance().AddDomain(domain) ? TRUE : FALSE;
	for(std::vector<std::wstring>::const_iterator iter = rules.begin(); iter != rules.end(); ++iter) {
		HttpRequestFilter::GetInstance().AddFilterRule(domain, *iter);
	}
}

GSNETFILTER_API BOOL GsEnableDomain(const std::wstring& domain, BOOL bEnable)
{
	return HttpRequestFilter::GetInstance().EnableDomain(domain, bEnable == FALSE ? false : true) ? TRUE : FALSE;
}

GSNETFILTER_API BOOL GsEnable(BOOL bEnable)
{
	return HttpRequestFilter::GetInstance().Enable(bEnable == FALSE ? false : true) ? TRUE : FALSE;
}

GSNETFILTER_API BOOL GsStartProxy()
{
	TcpProxyServer server;
	boost::asio::ip::address_v4 ip;
	ip.from_string("127.0.0.1");
	bool result = server.Bind(ip, 15868);
	if(!result) {
		return FALSE;
	}
	server.Run();
	return TRUE;
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