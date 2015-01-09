// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#pragma warning(disable:4702)
#include <string>
#include <atlbase.h>
#include <WTL/atlapp.h>
#define TSLOG
#define DD_GROUP "DD"
#include <tslog\tslog.h>

extern CAppModule _Module;

#include <atlwin.h>

#ifndef _countof
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif



// TODO: reference additional headers your program requires here
