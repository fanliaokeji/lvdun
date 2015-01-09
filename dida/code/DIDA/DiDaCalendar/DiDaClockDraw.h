#pragma once
#include <Windows.h>

class DiDaClockDraw {
	static HDC hdcClockBack;
	static HBITMAP hBmpClockBack;
	static HFONT hFontClockText;
public:
	static void DrawClock(HWND hWnd, HDC hdc);
	static LRESULT OnCalcRect(HWND hWnd);
	static void CreateClockBackDC(HWND hWnd);
	static void ClearClockBackDC();
	static void LoadDrawingSettings(HWND hWnd);
private:
	static BOOL CALLBACK EnumFontFamExProc(const LOGFONT *lpelfe, const TEXTMETRIC *lpntme, DWORD FontType, LPARAM lParam);
	static BOOL CALLBACK FontSongExistEnumFontFamExProc(const LOGFONT *lpelfe, const TEXTMETRIC *lpntme, DWORD FontType, LPARAM lParam);
private:
	// draw
	static void FillClock(HWND hWnd, HDC hdc, const RECT *prc);
	static void CopyClockBack(HWND hwnd, HDC hdcDest, HDC hdcSrc, int w, int h);
private:
	// util
	static BOOL CreateOffScreenDC(HDC hdc, HDC *phdcMem, HBITMAP *phbmp, int width, int height);
	static void CopyParentSurface(HWND hWnd, HDC hdcDest, int xdst, int ydst, int w, int h, int xsrc, int ysrc);
	static void GetDefaultFontName(wchar_t* fontname);
	static HFONT DiDaCreateFont(const wchar_t *fontname, int size, LONG weight, LONG italic, int codepage);
	static bool IsFontSongExist();
};
