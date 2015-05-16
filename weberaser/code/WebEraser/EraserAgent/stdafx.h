// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <wininet.h>
#include <shellapi.h>
#include <Shlobj.h>
#include <mapidbg.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>
// STL Header Files
#pragma warning(disable:4702)
#include <string>
#include <vector>
#include <list>
#include <map>
#include <fstream>
#include <algorithm>
#pragma warning(default:4702)
// ATL Header Files
#include <atlbase.h>
#include <WTL/atlapp.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlfile.h>
#include <atlcoll.h>
#include <atlstr.h>
#include <atlsecurity.h>
#include <atltime.h>
#include <WTL/atlmisc.h>
#include <WTL/atlcrack.h>
#include <XLLuaRuntime.h>
#define TSLOG
#define WE_GROUP "WE"	//可选,默认为 "TSLOG"
#include <tslog/tslog.h>


// TODO: reference additional headers your program requires here
