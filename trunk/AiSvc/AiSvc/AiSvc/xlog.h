#ifndef __LOG_H__
#define __LOG_H__


//////////////////////////////////////////////////////////////////////////
//不要再.h文件中包含此文件
#include <stdarg.h>
#include <stdio.h>
#include <Windows.h>

#ifdef openlog
#define xlog printflog
#define xlogL printflogL
#else
#define xlog /##/
#define xlogL /##/
#endif

//#define xlog /##/


#define  LogPos xlog("%s,%d \n",__FUNCTION__,__LINE__)

//

void printflog(const char* format, ... );

void printflogL(const wchar_t* format, ... );

#endif	//__LOG_H__
