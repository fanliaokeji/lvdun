#ifndef __LOG_H__
#define __LOG_H__

#include <stdarg.h>
#include <stdio.h>
#include <Windows.h>

#define openlog
#ifdef openlog
#define xlog printflog
#else
#define xlog /##/

#endif

#define  LogPos xlog("%s,%d \n",__FUNCTION__,__LINE__)

void printflog(const char* format, ... )
{
	va_list		marker;
	char		szMsg[1024*2]		= "";

	va_start(marker,format);
	vsprintf(szMsg,format,marker);
	va_end(marker);
	OutputDebugStringA(szMsg);
}

#endif	//__LOG_H__
