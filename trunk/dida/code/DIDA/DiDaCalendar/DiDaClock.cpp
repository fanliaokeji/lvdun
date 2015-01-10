#include "stdafx.h"
#include "DiDaClock.h"
#include "DiDaClockControl.h"
#include "DiDaClockDraw.h"

HWND DiDaClock::hWndClock = NULL;
WNDPROC DiDaClock::oldWndProc = NULL;
#define TIMERID_UPDATETIME 0x4386

LRESULT CALLBACK DiDaClock::ClockWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) {
		case WM_TIMER:
			switch(wParam) {
				case TIMERID_UPDATETIME:
					::InvalidateRect(hWnd, NULL, FALSE);
					break;
			}
			return 0;
		case WM_PAINT:
			return OnPaint(hWnd, uMsg, wParam, lParam);
		case WM_ERASEBKGND:
			break;
		case (WM_USER+100):
			return OnUSER_100(hWnd, uMsg, wParam, lParam);
		case WM_WINDOWPOSCHANGING:
			return OnWindowPosChanging(hWnd, uMsg, wParam, lParam);
		case WM_SIZE:
			return OnSize(hWnd, uMsg, wParam, lParam);
		case WM_SYSCOLORCHANGE:
			return OnSysColorChange(hWnd, uMsg, wParam, lParam);
		case WM_THEMECHANGED:
			return OnThemeChanged(hWnd, uMsg, wParam, lParam);
		case WM_WININICHANGE:
		case WM_TIMECHANGE:
		case (WM_USER+101):
		case WM_SETFOCUS:
		case WM_KILLFOCUS:
			::InvalidateRect(hWnd, NULL, FALSE);
			return 0;
		case WM_NCHITTEST:
			return HTCLIENT;
		case WM_LBUTTONUP:
			return OnLButtonUp(hWnd, uMsg, wParam, lParam);
		case WM_RBUTTONUP:
			return OnRButtonUp(hWnd, uMsg, wParam, lParam);
	}
	return CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
}

bool DiDaClock::InitClock()
{
	hWndClock = FindClockWindow(::GetCurrentProcessId());
	if(::IsWindow(hWndClock)) {
		WNDPROC oldProc = (WNDPROC)::GetWindowLongPtr(hWndClock, GWLP_WNDPROC);
		if(oldProc != NULL) {
			oldWndProc = oldProc;
			::SetWindowLongPtr(hWndClock, GWLP_WNDPROC, (LONG_PTR)&DiDaClock::ClockWndProc);
			LoadSetttings(hWndClock);
			::SetTimer(hWndClock, TIMERID_UPDATETIME, 1000, NULL);
			::PostMessage(::GetParent(::GetParent(hWndClock)), WM_SIZE, SIZE_RESTORED, 0);
			::PostMessage(::GetParent(hWndClock), WM_SIZE, SIZE_RESTORED, 0);
			::InvalidateRect(hWndClock, NULL, FALSE);
			::InvalidateRect(::GetParent(hWndClock), NULL, TRUE);
			return true;
		}
		else {
			hWndClock = NULL;
		}
	}
	return false;
}

bool DiDaClock::RestoreClockWndProc()
{
	bool result = false;
	if (oldWndProc != NULL && hWndClock != NULL) {
		::SetWindowLongPtr(hWndClock, GWLP_WNDPROC, (LONG_PTR)oldWndProc);
		result = true;
	}
	return result;
}

void DiDaClock::LoadSetttings(HWND hWnd)
{
	DiDaClockDraw::LoadDrawingSettings(hWnd);
}

LRESULT DiDaClock::OnPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	hdc = BeginPaint(hWnd, &ps);
	DiDaClockDraw::DrawClock(hWnd, hdc);
	EndPaint(hWnd, &ps);
	return 0;
}

LRESULT DiDaClock::OnWindowPosChanging(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPWINDOWPOS pwp = reinterpret_cast<LPWINDOWPOS>(lParam);
	
	if(!IsWindowVisible(hWnd) || (pwp->flags & SWP_NOSIZE)) {
		return 0;
	}
	
	DWORD dw = DiDaClockDraw::OnCalcRect(hWnd);
	int width = LOWORD(dw);
	int height = HIWORD(dw);

	if(pwp->cx > width) {
		pwp->cx = width;	
	}

	if(pwp->cy > height) {
		pwp->cy = height;	
	}
	return 0;
}

LRESULT DiDaClock::OnSize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DiDaClockDraw::CreateClockBackDC(hWnd);
	return 0;
}

LRESULT DiDaClock::OnSysColorChange(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 	OnThemeChanged(hWnd, uMsg, wParam, lParam);
}

LRESULT DiDaClock::OnThemeChanged(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DiDaClockDraw::CreateClockBackDC(hWnd);
	InvalidateRect(hWnd, NULL, FALSE);
	return 0;
}

LRESULT DiDaClock::OnLButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DiDaClockControl::LaunchCalendarMain();
	return 0;
}

LRESULT DiDaClock::OnRButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TrackMenu(hWnd);
	return 0;
}

LRESULT DiDaClock::OnUSER_100(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DiDaClockDraw::OnCalcRect(hWnd);
}

void DiDaClock::TrackMenu(HWND hWnd)
{
	POINT pt;
	GetCursorPos(&pt);
	HMENU hMenu = CreatePopupMenu();
	const int IMID_STARTLAUNCH = 0x1001;
	const int IMID_UPDATE = 0x1002;
	const int IMID_CHANGEDATETIME = 0x1003;
	const int IMID_ABOUTDIDA = 0x1004;
	const int IMID_EXIT = 0x1005;
	AppendMenu(hMenu, MF_STRING, IMID_STARTLAUNCH, L"开机启动");
	AppendMenu(hMenu, MF_STRING, IMID_UPDATE, L"软件升级");
	AppendMenu(hMenu, MF_STRING, IMID_CHANGEDATETIME, L"调整日期时间");
	AppendMenu(hMenu, MF_STRING, IMID_ABOUTDIDA, L"关于滴嗒日历");
	AppendMenu(hMenu, MF_STRING, IMID_EXIT, L"退出");
	int menuId = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hWnd, NULL);
	switch(menuId) {
	case IMID_STARTLAUNCH:
		::MessageBox(hWnd, L"开机启动", L"提示", MB_OK);
		break;
	case IMID_UPDATE:
		::MessageBox(hWnd, L"软件升级", L"提示", MB_OK);
		break;
	case IMID_CHANGEDATETIME:
		DiDaClockControl::ModifySystemDateTime();
		break;
	case IMID_ABOUTDIDA:
		::MessageBox(hWnd, L"关于嘀嗒日历", L"提示", MB_OK);
		break;
	case IMID_EXIT:
		::KillTimer(hWnd, TIMERID_UPDATETIME);
		RestoreClockWndProc();
		::PostMessage(::GetParent(::GetParent(hWnd)), WM_SIZE, SIZE_RESTORED, 0);
		::PostMessage(::GetParent(hWnd), WM_SIZE, SIZE_RESTORED, 0);
		::InvalidateRect(hWnd, NULL, FALSE);
		::InvalidateRect(::GetParent(hWnd), NULL, TRUE);
		DiDaClockControl::ExitCalendar(hWnd);
		break;
	}
	::DestroyMenu(hMenu);
}

HWND DiDaClock::FindClockWindow(DWORD dwProcessID)
{
	HWND hWnd = NULL;
	for(;;) {
		hWnd = ::FindWindowEx(NULL, hWnd, L"Shell_TrayWnd", NULL);
		if(::IsWindow(hWnd) == FALSE) {
			break;
		}
		DWORD dwPid = 0;
		::GetWindowThreadProcessId(hWnd, &dwPid);
		if(dwPid == 0) {
			continue;
		}
		if(dwProcessID != dwPid) {
			continue;
		}
		break;
	}
	if (::IsWindow(hWnd))
	{
		hWnd = ::FindWindowEx(hWnd, 0, L"TrayNotifyWnd", NULL);
		if (::IsWindow(hWnd))
		{
			return ::FindWindowEx(hWnd, 0, L"TrayClockWClass", NULL);
		}
	}
	return NULL;
}
