// screen.cpp : Cscreen 的实现

#include "stdafx.h"
#include "screen.h"


// Cscreen


STDMETHODIMP Cscreen::get_width(LONG* pVal)
{
	// TODO: 在此添加实现代码
	RECT rect = {0};	
	RECT rc = {0};	
	HWND hWnd = GetDesktopWindow();
	::GetWindowRect(hWnd, &rc);
	HMONITOR hMonitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = {sizeof(MONITORINFO)};


	if(hMonitor == NULL || !GetMonitorInfo(hMonitor, &mi))
	{
		TSERROR(_T("hMonitor: 0x%p, hMonitor is NULL or GetMonitorInfo failed"), hMonitor);
		rect.left = 0;
		rect.top = 0;
		rect.right = GetSystemMetrics(SM_CXSCREEN);
		rect.bottom = GetSystemMetrics(SM_CYSCREEN);
	}
	else
	{
		HMONITOR hPrimaryMobitor = MonitorFromRect(&rc, MONITOR_DEFAULTTOPRIMARY);
		hPrimaryMobitor;
		TSDEBUG(_T("hMonitor: 0x%X, hPrimaryMobitor: 0x%X"), hMonitor, hPrimaryMobitor);
		rect = mi.rcMonitor;		
	}
	*pVal = rect.right - rect.left;
	return S_OK;
}

STDMETHODIMP Cscreen::get_height(LONG* pVal)
{
	// TODO: 在此添加实现代码
	RECT rect = {0};	
	RECT rc = {0};	
	HWND hWnd = GetDesktopWindow();
	::GetWindowRect(hWnd, &rc);
	HMONITOR hMonitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = {sizeof(MONITORINFO)};


	if(hMonitor == NULL || !GetMonitorInfo(hMonitor, &mi))
	{
		TSERROR(_T("hMonitor: 0x%p, hMonitor is NULL or GetMonitorInfo failed"), hMonitor);
		rect.left = 0;
		rect.top = 0;
		rect.right = GetSystemMetrics(SM_CXSCREEN);
		rect.bottom = GetSystemMetrics(SM_CYSCREEN);
	}
	else
	{
		HMONITOR hPrimaryMobitor = MonitorFromRect(&rc, MONITOR_DEFAULTTOPRIMARY);
		hPrimaryMobitor;
		TSDEBUG(_T("hMonitor: 0x%X, hPrimaryMobitor: 0x%X"), hMonitor, hPrimaryMobitor);
		rect = mi.rcMonitor;		
	}
	*pVal = rect.bottom - rect.top;
	return S_OK;
}

STDMETHODIMP Cscreen::get_availHeight(LONG* pVal)
{
	// TODO: 在此添加实现代码
	RECT rect;
	POINT thePos;
	GetCursorPos(&thePos);
	HWND hWnd = ::WindowFromPoint(thePos);
	HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = {sizeof(MONITORINFO)};

	if(hMonitor == NULL || !GetMonitorInfo(hMonitor, &mi))
	{
		TSERROR(_T("hMonitor: 0x%p, hMonitor is NULL or GetMonitorInfo failed"), hMonitor);
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	}
	else
	{
		rect = mi.rcWork;
	}

	* pVal = rect.bottom - rect.top;
	return S_OK;
}

STDMETHODIMP Cscreen::get_availWidth(LONG* pVal)
{
	// TODO: 在此添加实现代码
	RECT rect;
	POINT thePos;
	GetCursorPos(&thePos);
	HWND hWnd = ::WindowFromPoint(thePos);
	HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = {sizeof(MONITORINFO)};

	if(hMonitor == NULL || !GetMonitorInfo(hMonitor, &mi))
	{
		TSERROR(_T("hMonitor: 0x%p, hMonitor is NULL or GetMonitorInfo failed"), hMonitor);
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	}
	else
	{
		rect = mi.rcWork;
	}


	* pVal = rect.right - rect.left;
	return S_OK;
}
