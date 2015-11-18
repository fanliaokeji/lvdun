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

using namespace ATL;


#define TSLOG
#define TSLOG_GROUP "ExplorerPlugin"
#include <tslog\tslog.h>

#ifdef MYCALENDAR
	//此互斥量保证本dll只被1个宿主进程加载
	#define ONEPROCESSMUTEX L"{B6A3F13B-ED68-4115-9EC9-60E4E4408589}"
	//此互斥量保证只有1个拉起逻辑被执行(宿主可多次触发拉起逻辑)
	#define GLOBALMUTXNAME L"Global\\{C3F5E7B7-46CF-445a-ACE9-7DCC9FD345F2}_ExplorerPlugin"
	//IconOverlay 名称
	#define ICONOVERLAYNAME L".mycalendarremind"
	//CopyHook 名称
	#define COPYHOOKNAME L"AMCSharing"
	//BHO 名称
	#define BHONAME L"MCBHO"
	//产品注册表路径
	#define REGEDITPATH L"Software\\mycalendar"
	//地域开关注册表键名
	#define ZONESWITCH L"laopen" 
	//上次启动时间注册表键名
	#define LASTLAUNCHUTC L"LastLaunchTime"
	//开机启动项名称
	#define SYSBOOTNAME L""
#elif defined LVDUN_0000
	//此互斥量保证本dll只被1个宿主进程加载
	#define ONEPROCESSMUTEX L"{2C55D214-C83F-4a69-A5C3-1955F3FC0ACD}"
	//此互斥量保证只有1个拉起逻辑被执行(宿主可多次触发拉起逻辑)
	#define GLOBALMUTXNAME L"Global\\{FB215D88-ACE8-48f0-8C92-D5ABAC4DD8DF}_ExplorerPlugin"
	//IconOverlay 名称
	#define ICONOVERLAYNAME L".gsremind"
	//CopyHook 名称
	#define COPYHOOKNAME L"AGSSharing"
	//BHO 名称
	#define BHONAME L"GSBHO"
	//产品注册表路径
	#define REGEDITPATH L"Software\\GreenShield"
	//地域开关注册表键名
	#define ZONESWITCH L"laopen" 
	//上次启动时间注册表键名
	#define LASTLAUNCHUTC L"LastLaunchTime"
	//开机启动项名称
	#define SYSBOOTNAME L"GreenShield"
#endif
