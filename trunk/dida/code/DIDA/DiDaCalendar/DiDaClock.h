#pragma once
#include <Windows.h>

class DiDaClock {
	static HWND hWndClock;
	static WNDPROC oldWndProc;
public:
	static LRESULT CALLBACK ClockWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static bool InitClock();
	static bool RestoreClockWndProc();
	static void LoadSetttings(HWND hWnd);
private:
	static LRESULT OnPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT OnWindowPosChanging(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT OnSize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT OnSysColorChange(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT OnThemeChanged(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT OnLButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT OnRButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT OnUSER_100(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	static void TrackMenu(HWND hWnd);
	static HWND FindClockWindow(DWORD processID);
};
