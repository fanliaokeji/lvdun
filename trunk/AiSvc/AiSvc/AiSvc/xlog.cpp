#include "stdafx.h"
#include "xlog.h"

void printflog(const char* format,...)
{
	va_list		marker;
	char		szMsg[1024*2]		= "";

	va_start(marker,format);
	vsprintf(szMsg,format,marker);
	va_end(marker);
	OutputDebugStringA(szMsg);
}


void printflogL(const wchar_t* format,...)
{
	va_list		marker;
	wchar_t		szMsg[1024*2]		= L"";

	va_start(marker,format);
	vswprintf(szMsg,format,marker);
	va_end(marker);
	OutputDebugStringW(szMsg);
}