// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>



// TODO: reference additional headers your program requires here




#include <shellapi.h>
#include <Shlobj.h>
#include <wininet.h>
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
#include <set>
#include <fstream>
#include <algorithm>
#include <assert.h>
using namespace std;

// ATL Header Files
#pragma warning(default:4702)
#include <atlbase.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlfile.h>
#include <atlcoll.h>
#include <atlstr.h>
#include <atlsecurity.h>
#include <atltime.h>

// WTL Header Files
#include <WTL/atlapp.h>
#include <WTL/atldlgs.h>
#include <WTL/atlcrack.h>

// xlue Header Files
#include <XLI18N.h>
#include <XLUE.h>
#include <XLGraphic.h>
#include <XLGraphicPlus.h>
#include <XLLuaRuntime.h>
#include <XLFS.h>

#include <winsock2.h>
#include <Gdiplus.h>
#define TSLOG
#define TSLOG_GROUP "AS"	//可选,默认为 "TSLOG"
#include <tslog/tslog.h>

#include "Utility/LuaAPIHelper.h"
#include "define.h"


// TODO: reference additional headers your program requires here