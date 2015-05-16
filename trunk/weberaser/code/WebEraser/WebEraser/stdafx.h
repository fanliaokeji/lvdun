// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200

#pragma warning(disable:4702)
#include <string>
// ATL Header Files
#include <atlbase.h>
// #include <atlwin.h>
// #include <atltypes.h>
// #include <atlfile.h>
// #include <atlcoll.h>
// #include <atlstr.h>
// #include <atlsecurity.h>
// #include <atltime.h>
// WTL Header Files
#include <WTL/atlapp.h>
// #include <WTL/atldlgs.h>
// #include <WTL/atlcrack.h>
#define TSLOG
#define WE_GROUP "WE"
#include <tslog\tslog.h>

extern CAppModule _Module;

#include <atlwin.h>

#ifndef _countof
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif
