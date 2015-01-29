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
#import "C:\Program Files (x86)\Common Files\Designer\MSADDNDR.DLL" raw_interfaces_only, raw_native_types, no_namespace, named_guids, auto_search

#define GSLOG
#define GSLOG_GROUP "OfficeAddin"
#include <tslog\tslog.h>
