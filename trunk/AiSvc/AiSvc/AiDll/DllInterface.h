#pragma once

#ifdef DLL_FILE
#define DLL_API extern "C" _declspec(dllexport) 
#else
#define DLL_API extern "C" _declspec(dllimport) 
#endif

DLL_API int Run();

DLL_API int RunEx(int magic);