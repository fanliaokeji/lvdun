#include "stdafx.h"
#include "DiDaClockDraw.h"
#include <string>
#include <sstream>
#include <Uxtheme.h>

HDC DiDaClockDraw::hdcClockBack = NULL;
HBITMAP DiDaClockDraw::hBmpClockBack = NULL;
HFONT DiDaClockDraw::hFontClockText = NULL;

void DiDaClockDraw::DrawClock(HWND hWnd, HDC hdc)
{
	if(hdcClockBack == NULL) {
		CreateClockBackDC(hWnd);
	}

	if(hdcClockBack == NULL) {
		return;
	}

	HDC hDeskTopDC = GetDC(NULL);
	HBITMAP hMemDCBitmap = NULL;
	RECT rc;
	GetClientRect(hWnd, &rc);
	HDC hMemDC = NULL;
	if(!CreateOffScreenDC(hDeskTopDC, &hMemDC, &hMemDCBitmap, rc.right, rc.bottom))
	{
		ReleaseDC(NULL, hDeskTopDC);
		return;
	}

	SetBkMode(hMemDC, TRANSPARENT);

	if(hFontClockText != NULL) {
		::SelectObject(hMemDC, hFontClockText);
	}

	// 文本颜色
	DWORD dwVersion = GetVersion();
    DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
    DWORD dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

	if(dwMajorVersion < 6) {
		// XP
		if(::IsThemeActive()) {
			COLORREF cmpColor = ::GetSysColor(COLOR_MENUHILIGHT);
			// 70A093
			if(cmpColor == 0xC56A31) {
				::SetTextColor(hMemDC, 0xffffff);
			}
			else if(cmpColor == 0x70A093) {
				::SetTextColor(hMemDC, RGB(0x41, 0x40, 0x0A));
			}
			else {
				::SetTextColor(hMemDC, 0x000000);
			}
		}
		else {
			if(::GetPixel(hdcClockBack, 0, 0) == 0x000000) {
				::SetTextColor(hMemDC, 0xffffff);
			}
			else {
				::SetTextColor(hMemDC, 0x000000);
			}
		}
	}
	else if(dwMajorVersion == 6 && dwMinorVersion < 2){
		// Vista Win7
		if(IsThemeActive()) {
			::SetTextColor(hMemDC, 0xffffff);
		}
		else {
			::SetTextColor(hMemDC, ::GetSysColor(COLOR_MENUTEXT));
		}
	}
	else {
		// Win8 Win10
		HIGHCONTRAST hcntr;
		std::memset(&hcntr, 0, sizeof(HIGHCONTRAST));
		hcntr.cbSize = sizeof(HIGHCONTRAST);
		if(SystemParametersInfo(SPI_GETHIGHCONTRAST, 0, &hcntr, 0) && (hcntr.dwFlags & HCF_HIGHCONTRASTON) == HCF_HIGHCONTRASTON) {
			::SetTextColor(hMemDC, ::GetSysColor(COLOR_WINDOWTEXT));
		}
		else {
			::SetTextColor(hMemDC, 0xffffff);
		}
	}

	RECT rcClock;
	GetClientRect(hWnd, &rcClock);
	LONG wclock = rcClock.right;
	LONG hclock = rcClock.bottom;
	CopyClockBack(hWnd, hMemDC, hdcClockBack, wclock, hclock);
	SYSTEMTIME stUTC;
	::GetSystemTime(&stUTC);
	SYSTEMTIME stLocal;
	::SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
	const wchar_t* week_day = L"星期日";
	switch(stLocal.wDayOfWeek) {
		case 1:
			week_day = L"星期一";
			break;
		case 2:
			week_day = L"星期二";
			break;
		case 3:
			week_day = L"星期三";
			break;
		case 4:
			week_day = L"星期四";
			break;
		case 5:
			week_day = L"星期五";
			break;
		case 6:
			week_day = L"星期六";
			break;
	}

	HWND hWndTray = GetParent(hWnd);
	HWND hWndTaskBar = (hWndTray == NULL) ? NULL : ::GetParent(hWndTray);

	if(hWndTray != NULL && hWndTaskBar != NULL) {
		RECT rcTray, rcTaskbar;
		::GetWindowRect(hWndTray, &rcTray);
		::GetClientRect(hWndTaskBar, &rcTaskbar);
		if(rcTaskbar.right - rcTaskbar.left > rcTaskbar.bottom - rcTaskbar.top) {
			wchar_t first_line[100];
			int first_line_length = std::swprintf(first_line, L"%.2d:%.2d %s", stLocal.wHour, stLocal.wMinute, week_day);
			wchar_t second_line[100];
			int second_line_length = std::swprintf(second_line, L"%d-%.2d-%.2d", stLocal.wYear, stLocal.wMonth, stLocal.wDay);
			if(first_line_length >= 0 && second_line_length >= 0) {
				LONG backHeight = rc.bottom - rc.top;
				LONG backWidth = rc.right - rc.left;
				RECT rcFirstLine = {0, 0, 0, 0}, rcSecondLine = {0, 0, 0, 0};
				DrawText(hMemDC, first_line, first_line_length, &rcFirstLine, DT_CALCRECT);
				DrawText(hMemDC, second_line, second_line_length, &rcSecondLine, DT_CALCRECT);
				LONG firstLineHeight = rcFirstLine.bottom - rcFirstLine.top;
				LONG secondLineHeight = rcSecondLine.bottom - rcSecondLine.top;
				LONG lineGap = 0;
				if (backHeight < 35) {
					if (dwMajorVersion >= 6) {
						lineGap = -3;
					}
				}
				LONG drawTextHeight = firstLineHeight + lineGap + secondLineHeight;

				LONG firstLineWidth = rcFirstLine.right - rcFirstLine.left;
				LONG secondLineWidth = rcSecondLine.right - rcSecondLine.left;
				LONG drawTextWidth = firstLineWidth;
				if(secondLineWidth > drawTextWidth) {
					drawTextWidth = secondLineWidth;
				}

				rcFirstLine.top = (backHeight - drawTextHeight) / 2;
				rcFirstLine.bottom = rcFirstLine.top + firstLineHeight;
				
				if(firstLineWidth < drawTextWidth) {
					rcFirstLine.left = (drawTextWidth - firstLineWidth) / 2;
					rcFirstLine.right = rcFirstLine.left + firstLineWidth;
				}
				DrawText(hMemDC, first_line, first_line_length, &rcFirstLine, DT_LEFT);
				rcSecondLine.top = rcFirstLine.bottom + lineGap;
				rcSecondLine.bottom = rcSecondLine.top + secondLineHeight;
				if(secondLineWidth < drawTextWidth) {
					rcSecondLine.left = (drawTextWidth - secondLineWidth) / 2;
					rcSecondLine.right = rcSecondLine.left + secondLineWidth;
				}
				DrawText(hMemDC, second_line, second_line_length, &rcSecondLine, DT_LEFT);
			}
		}
		else {
			wchar_t first_line[100];
			int first_line_length = std::swprintf(first_line, L"%.2d:%.2d", stLocal.wHour, stLocal.wMinute);
			wchar_t second_line[100];
			int second_line_length = std::swprintf(second_line, L"%s", week_day);
			if(first_line_length >= 0 && second_line_length >= 0) {
				LONG backHeight = rc.bottom - rc.top;
				LONG backWidth = rc.right - rc.left;
				RECT rcFirstLine = {0, 0, 0, 0}, rcSecondLine = {0, 0, 0, 0};
				DrawText(hMemDC, first_line, first_line_length, &rcFirstLine, DT_CALCRECT);
				DrawText(hMemDC, second_line, second_line_length, &rcSecondLine, DT_CALCRECT);
				LONG firstLineHeight = rcFirstLine.bottom - rcFirstLine.top;
				LONG secondLineHeight = rcSecondLine.bottom - rcSecondLine.top;
				LONG lineGap = 1;
				LONG drawTextHeight = firstLineHeight + lineGap + secondLineHeight;

				LONG firstLineWidth = rcFirstLine.right - rcFirstLine.left;
				LONG secondLineWidth = rcSecondLine.right - rcSecondLine.left;

				if(drawTextHeight < backHeight) {
					rcFirstLine.top = (backHeight - drawTextHeight) / 2;
					rcFirstLine.bottom = rcFirstLine.top + firstLineHeight;
				}

				rcFirstLine.left = (backWidth - firstLineWidth) / 2;
				rcFirstLine.right = rcFirstLine.left + firstLineWidth;
				DrawText(hMemDC, first_line, first_line_length, &rcFirstLine, DT_LEFT);
				rcSecondLine.top = rcFirstLine.bottom + lineGap;
				rcSecondLine.bottom = rcSecondLine.top + secondLineHeight;
				rcSecondLine.left = (backWidth - secondLineWidth) / 2;
				rcSecondLine.right = rcSecondLine.left + secondLineWidth;
				DrawText(hMemDC, second_line, second_line_length, &rcSecondLine, DT_LEFT);
			}
		}
	}

	BitBlt(hdc, 0, 0, wclock, hclock, hMemDC, 0, 0, SRCCOPY);
	::DeleteDC(hMemDC);
	::DeleteObject(hMemDCBitmap);
	::ReleaseDC(NULL, hDeskTopDC);
}

LRESULT DiDaClockDraw::OnCalcRect(HWND hWnd)
{
	WORD height = 0;
	WORD width = 0;
	SYSTEMTIME stUTC;
	::GetSystemTime(&stUTC);
	SYSTEMTIME stLocal;
	::SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
	const wchar_t* week_day = L"星期日";
	switch(stLocal.wDayOfWeek) {
		case 1:
			week_day = L"星期一";
			break;
		case 2:
			week_day = L"星期二";
			break;
		case 3:
			week_day = L"星期三";
			break;
		case 4:
			week_day = L"星期四";
			break;
		case 5:
			week_day = L"星期五";
			break;
		case 6:
			week_day = L"星期六";
			break;
	}

	HWND hWndTray = GetParent(hWnd);
	HWND hWndTaskBar = (hWndTray == NULL) ? NULL : ::GetParent(hWndTray);

	if(hWndTray != NULL && hWndTaskBar != NULL) {
		RECT rcTray, rcTaskbar;
		::GetWindowRect(hWndTray, &rcTray);
		::GetClientRect(hWndTaskBar, &rcTaskbar);
		if(rcTaskbar.right - rcTaskbar.left > rcTaskbar.bottom - rcTaskbar.top) {
			wchar_t first_line[100];
			int first_line_length = std::swprintf(first_line, L"%.2d:%.2d %s", stLocal.wHour, stLocal.wMinute, week_day);
			wchar_t second_line[100];
			int second_line_length = std::swprintf(second_line, L"%d-%.2d-%.2d", stLocal.wYear, stLocal.wMonth, stLocal.wDay);
			HDC hdc = ::GetDC(hWnd);
			if(hdc != NULL) {
				HDC hMemDC = ::CreateCompatibleDC(hdc);
				if(hMemDC != NULL) {
					if(hFontClockText != NULL) {
						::SelectObject(hMemDC, hFontClockText);
						RECT rcFirstLine = {0, 0, 0, 0}, rcSecondLine = {0, 0, 0, 0};
						DrawText(hMemDC, first_line, first_line_length, &rcFirstLine, DT_CALCRECT);
						DrawText(hMemDC, second_line, second_line_length, &rcSecondLine, DT_CALCRECT);

						LONG firstLineWidth = rcFirstLine.right - rcFirstLine.left;
						LONG secondLineWidth = rcSecondLine.right - rcSecondLine.left;
						width = firstLineWidth > secondLineWidth ? firstLineWidth : secondLineWidth;
					}
					::DeleteDC(hMemDC);
				}
				::ReleaseDC(hWnd, hdc);
			}
			height = rcTray.bottom - rcTray.top;
		}
		else {
			wchar_t first_line[100];
			int first_line_length = std::swprintf(first_line, L"%.2d:%.2d", stLocal.wHour, stLocal.wMinute);
			wchar_t second_line[100];
			int second_line_length = std::swprintf(second_line, L"%s", week_day);
			HDC hdc = ::GetDC(hWnd);
			if(hdc != NULL) {
				HDC hMemDC = ::CreateCompatibleDC(hdc);
				if(hMemDC != NULL) {
					if(hFontClockText != NULL) {
						::SelectObject(hMemDC, hFontClockText);
						RECT rcFirstLine = {0, 0, 0, 0}, rcSecondLine = {0, 0, 0, 0};
						DrawText(hMemDC, first_line, first_line_length, &rcFirstLine, DT_CALCRECT);
						DrawText(hMemDC, second_line, second_line_length, &rcSecondLine, DT_CALCRECT);

						LONG firstLineHeight = rcFirstLine.bottom - rcFirstLine.top;
						LONG secondLineHeight = rcSecondLine.bottom - rcSecondLine.top;
						LONG lineGap = 1;
						height = firstLineHeight + lineGap + secondLineHeight;
					}
					::DeleteDC(hMemDC);
				}
				::ReleaseDC(hWnd, hdc);
			}
			width = rcTray.right - rcTray.left;
		}
	}
	return (height << 16) + width;
}

void DiDaClockDraw::CreateClockBackDC(HWND hWnd)
{
	RECT rc;
	HDC hdc;

	ClearClockBackDC();
	
	GetClientRect(hWnd, &rc);
	
	hdc = GetDC(NULL);

	if(!CreateOffScreenDC(hdc, &hdcClockBack, &hBmpClockBack,
		rc.right, rc.bottom))
	{
		ClearClockBackDC();
		ReleaseDC(NULL, hdc);
		return;
	}
	FillClock(hWnd, hdcClockBack, &rc);
	
	ReleaseDC(NULL, hdc);
}

void DiDaClockDraw::ClearClockBackDC()
{
	if(hdcClockBack) {
		DeleteDC(hdcClockBack);
	}
	hdcClockBack = NULL;

	if(hBmpClockBack) {
		DeleteObject(hBmpClockBack);
	}
	hBmpClockBack = NULL;
}

void DiDaClockDraw::LoadDrawingSettings(HWND hWnd)
{
	wchar_t fontname[LF_FACESIZE];	
	GetDefaultFontName(fontname);

	int size = 9;
	LONG weight = FW_THIN;
	LONG italic = 0;
	int codepage = GetACP();

	if(hFontClockText != NULL) {
		DeleteObject(hFontClockText);
		hFontClockText = NULL;
	}

	hFontClockText = DiDaCreateFont(fontname, size, weight, italic, codepage);
}

BOOL CALLBACK DiDaClockDraw::EnumFontFamExProc(const LOGFONT *lpelfe, const TEXTMETRIC *lpntme, DWORD FontType, LPARAM lParam)
{
	if(std::wcscmp(reinterpret_cast<const wchar_t *>(lParam), lpelfe->lfFaceName) == 0) {
		return FALSE;
	}
	return TRUE;
}

BOOL CALLBACK DiDaClockDraw::FontSongExistEnumFontFamExProc(const LOGFONT *lpelfe, const TEXTMETRIC *lpntme, DWORD FontType, LPARAM lParam)
{
	return 0xc0ffee;
}


void DiDaClockDraw::FillClock(HWND hWnd, HDC hdc, const RECT *prc)
{
	RECT rc, rcTray;
	GetWindowRect(hWnd, &rc);
	GetWindowRect(GetParent(hWnd), &rcTray);
	CopyParentSurface(hWnd, hdc, 0, 0, prc->right, prc->bottom,
		rc.left - rcTray.left, rc.top - rcTray.top);
}

void DiDaClockDraw::CopyClockBack(HWND hwnd, HDC hdcDest, HDC hdcSrc, int w, int h)
{
	int xsrc = 0, ysrc = 0;
	BitBlt(hdcDest, 0, 0, w, h, hdcSrc, xsrc, ysrc, SRCCOPY);
}

BOOL DiDaClockDraw::CreateOffScreenDC(HDC hdc, HDC *phdcMem, HBITMAP *phbmp, int width, int height)
{
	*phdcMem = CreateCompatibleDC(hdc);
	if(!*phdcMem) { *phbmp = NULL; return FALSE; }
	
	*phbmp = CreateCompatibleBitmap(hdc, width, height);
	if(!*phbmp)
	{
		DeleteDC(*phdcMem); *phdcMem = NULL;
		return FALSE;
	}
	
	SelectObject(*phdcMem, *phbmp);
	return TRUE;
}

void DiDaClockDraw::CopyParentSurface(HWND hWnd, HDC hdcDest, int xdst, int ydst, int w, int h, int xsrc, int ysrc)
{
	HDC hdcTemp, hdcMem;
	HBITMAP hbmp;
	RECT rcParent;
	
	GetWindowRect(GetParent(hWnd), &rcParent);
	
	hdcTemp = GetDC(NULL);
	
	if(!CreateOffScreenDC(hdcTemp, &hdcMem, &hbmp,
		rcParent.right - rcParent.left, rcParent.bottom - rcParent.top))
	{
		ReleaseDC(NULL, hdcTemp);
		return;
	}
	
	SendMessage(GetParent(hWnd), WM_PRINTCLIENT,
		(WPARAM)hdcMem, (LPARAM)PRF_CLIENT);
	
	BitBlt(hdcDest, xdst, ydst, w, h, hdcMem, xsrc, ysrc, SRCCOPY);
	
	DeleteDC(hdcMem);
	DeleteObject(hbmp);
	
	ReleaseDC(NULL, hdcTemp);
}

void DiDaClockDraw::GetDefaultFontName(wchar_t* fontname)
{
	DWORD dwVersion = GetVersion();
    DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	if(dwMajorVersion < 6) {
		// XP
		if(IsFontSongExist()) {
			std::wcscpy(fontname, L"宋体");
		}
		else {
			std::wcscpy(fontname, L"System");
		}
	}
	else {
		NONCLIENTMETRICS ncm = {sizeof(ncm)};
		if(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, FALSE)) {
			std::wcscpy(fontname, ncm.lfMenuFont.lfFaceName);
		}
		else {
			std::wcscpy(fontname, L"System");
		}
	}
}

static const struct {
	int cp;
	BYTE charset;
} CodepageCharset[] = {
	{ 932,  SHIFTJIS_CHARSET },
	{ 936,  GB2312_CHARSET },
	{ 949,  HANGEUL_CHARSET },
	{ 950,  CHINESEBIG5_CHARSET },
	{ 1250, EASTEUROPE_CHARSET },
	{ 1251, RUSSIAN_CHARSET },
	{ 1252, ANSI_CHARSET },
	{ 1253, GREEK_CHARSET },
	{ 1254, TURKISH_CHARSET },
	{ 1257, BALTIC_CHARSET },
	{ 0, 0}
};

HFONT DiDaClockDraw::DiDaCreateFont(const wchar_t *fontname, int size, LONG weight, LONG italic, int codepage)
{	
	BYTE charset = 0;
	for(int i = 0; CodepageCharset[i].cp; i++)
	{
		if(codepage == CodepageCharset[i].cp)
		{
			charset = CodepageCharset[i].charset;
			break;
		}
	}
	
	HDC hdc = GetDC(NULL);
	
	if(charset == 0) {
		charset = (BYTE)GetTextCharset(hdc);
	}	
	
	LOGFONT lf;
	std::memset(&lf, 0, sizeof(LOGFONT));

	lf.lfCharSet = charset;
	std::wcscpy(lf.lfFaceName, fontname);

	if(EnumFontFamiliesEx(hdc, &lf, (FONTENUMPROCW)&DiDaClockDraw::EnumFontFamExProc,
		(LPARAM)fontname, 0))
	{
		lf.lfCharSet = OEM_CHARSET;
		if(EnumFontFamiliesEx(hdc, &lf, (FONTENUMPROCW)&DiDaClockDraw::EnumFontFamExProc,
			(LPARAM)fontname, 0))
		{
			lf.lfCharSet = ANSI_CHARSET;
			if(EnumFontFamiliesEx(hdc, &lf, (FONTENUMPROCW)&DiDaClockDraw::EnumFontFamExProc,
				(LPARAM)fontname, 0))
			{
				lf.lfCharSet = DEFAULT_CHARSET;
			}
		}
	}

	lf.lfHeight = -MulDiv(size, GetDeviceCaps(hdc, LOGPIXELSY), 72);

	ReleaseDC(NULL, hdc);

	lf.lfWidth = lf.lfEscapement = lf.lfOrientation = 0;
	lf.lfWeight = weight;
	lf.lfItalic = (BYTE)italic;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	// lf.lfCharSet = ;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = CLEARTYPE_NATURAL_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

	return CreateFontIndirect(&lf);
}

bool DiDaClockDraw::IsFontSongExist()
{
	bool exist = false;
	HFONT hFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE,
			0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_SWISS, L"宋体");
	if(hFont != NULL) {
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		if(0 != ::GetObject(hFont, sizeof(LOGFONT), &lf)) {
			HDC hdc = ::GetDC(NULL);
			int iRet = EnumFontFamiliesEx(hdc, &lf, &DiDaClockDraw::FontSongExistEnumFontFamExProc, 0, 0);
			if(iRet == 0xc0ffee) {
				exist = true;
			}
			::ReleaseDC(NULL, hdc);
		}
		::DeleteObject(hFont);
	}
	return exist;
}
