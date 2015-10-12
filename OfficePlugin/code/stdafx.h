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
#import "C:\Program Files (x86)\Common Files\DESIGNER\MSADDNDR.DLL" raw_interfaces_only, raw_native_types, no_namespace, named_guids, auto_search


#import "C:\Program Files\Common Files\Microsoft Shared\OFFICE14\mso.dll" rename_namespace("Office") named_guids,exclude("Pages")
using namespace Office;

#import "C:\Program Files (x86)\Common Files\Microsoft Shared\VBA\VBA6\VBE6EXT.olb" rename_namespace("VBE6")
using namespace VBE6;
#import "C:\Program Files\Microsoft Office\Office14\MSWORD.OLB" rename("ExitWindows","ExitWindowsEx")
#import "C:\Program Files\Microsoft Office\Office14\MSWORD.OLB" rename_namespace("Word"), raw_interfaces_only, named_guids ,exclude("Pages")
using namespace Word;

#define TSLOG
#define TSLOG_GROUP "DD"
#include "tslog\tslog.h"
