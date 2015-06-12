// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>

#define TSLOG                                      //必选宏,不声明将会把所有日志功能取消
#define TSLOG_GROUP "FR"                        //可选,默认为 "TSLOG"
#include "tslog/tslog.h"                                 //如上配置,日志程序将根据 C:\TSLOG_CONFIG\TSLOG.ini 定义的策略打印

using namespace ATL;
