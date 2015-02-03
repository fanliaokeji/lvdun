#ifndef mini_unzip_dll_D5AEC4E4_5DC4_46eb_BAA2_6DE02315B088
#define mini_unzip_dll_D5AEC4E4_5DC4_46eb_BAA2_6DE02315B088

// 下列 ifdef 块是创建使从 DLL 导出更简单的
//宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 MINI_UNZIP_DLL_EXPORTS
// 符号编译的。在使用此 DLL 的
//任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将 
// MINI_UNZIP_DLL_API 函数视为是从此 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef MINI_UNZIP_DLL_EXPORTS
#define MINI_UNZIP_DLL_API __declspec(dllexport)
#else
#define MINI_UNZIP_DLL_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

// this function is exported from mini_unzip_dll.dll.
// unzip a .zip file to an existing folder. return 0 when succeeded, but non zero when failed.   
MINI_UNZIP_DLL_API int mini_unzip_dll( const char * zipfilename, const char * directroy );

#ifdef __cplusplus
};
#endif
    
#endif