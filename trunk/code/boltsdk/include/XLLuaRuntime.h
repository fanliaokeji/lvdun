/********************************************************************
*
* =-----------------------------------------------------------------=
* =                                                                 =
* =             Copyright (c) Xunlei, LTD. 2009                     =
* =                                                                 =
* =-----------------------------------------------------------------=
* 
*   FileName    :   XLLuaRuntime.h
*   Author      :   xlue group(xlue@xunlei.com)
*   Create      :   2009/01/04
*   LastChange  :   
*   History     :	
*								
*   Description :   XLLuaRuntime.dll��ͷ�ļ�����������صĽӿ�
*					Ŀǰ����ENV�ĵ������̰߳�ȫ���⣬����Ľӿڶ�����
*					�̰߳�ȫ�ģ��������߳���ʹ��
********************************************************************/  

#ifndef _XUNLEI_LUA_RUNTIME_H_
#define _XUNLEI_LUA_RUNTIME_H_

#ifndef LUA_BUILD_AS_DLL
	#define LUA_BUILD_AS_DLL
#endif // LUA_BUILD_AS_DLL

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#include <lua/lua.h>
#include <lua/lauxlib.h>

#ifdef __cplusplus
}
#endif // __cplusplus


#ifndef XLLUARUNTIME_EXTERN_C
	#ifdef __cplusplus	
		#define XLLUARUNTIME_EXTERN_C extern "C"
	#else
		#define XLLUARUNTIME_EXTERN_C 
	#endif // __cplusplus
#endif //XLUE_EXTERN_C

#ifndef XLUE_STDCALL
	#if defined(_MSC_VER)
		#define XLUE_STDCALL __stdcall
	#elif defined(__GNUC__)
		#define XLUE_STDCALL __attribute__((__stdcall__))
	#endif
#endif //XLUE_STDCALL

#if defined(_MSC_VER)
	#if defined(XLUE_UNIONLIB)
			#define XL_LRT_API(x) XLLUARUNTIME_EXTERN_C  x __stdcall 
	#elif defined(XLLUA_RUNTIME_EXPORTS)
			#define XL_LRT_API(x) XLLUARUNTIME_EXTERN_C __declspec(dllexport) x __stdcall 
	#elif defined (XLUE_UNION)
			#define XL_LRT_API(x) XLLUARUNTIME_EXTERN_C  x __stdcall 
	#else // XLLUA_RUNTIME_EXPORTS
			#define XL_LRT_API(x) XLLUARUNTIME_EXTERN_C __declspec(dllimport) x __stdcall 
	#endif // XLLUA_RUNTIME_EXPORTS
#elif defined(__GNUC__)
	#if defined(XLUE_UNIONLIB)
			#define XL_LRT_API(x) XLLUARUNTIME_EXTERN_C  __attribute__((__stdcall__)) x
	#elif defined(XLLUA_RUNTIME_EXPORTS)
			#define XL_LRT_API(x) XLLUARUNTIME_EXTERN_C __attribute__((__visibility__("default"), __stdcall__)) x
	#elif defined (XLUE_UNION)
			#define XL_LRT_API(x) XLLUARUNTIME_EXTERN_C  __attribute__((__stdcall__)) x
	#else // XLLUA_RUNTIME_EXPORTS
			#define XL_LRT_API(x) XLLUARUNTIME_EXTERN_C __attribute__((__visibility__("default"), __stdcall__)) x 
	#endif // XLLUA_RUNTIME_EXPORTS
#endif

#if !defined(WIN32) && !defined(XLUE_WIN32)
#include <XLUESysPreDefine.h>
#endif // WIN32 && XLUE_WIN32

//������д:
//XL = Xunlei
//LRT = Lua RunTime

//---ʹ��---
//����LUA����
//LUA����->����LUA����ʱ
//����LUA code chunk
//��code chunk�ŵ�LUA����ʱ������

typedef void* XL_LRT_ENV_HANDLE;
typedef void* XL_LRT_RUNTIME_HANDLE;
typedef void* XL_LRT_CHUNK_HANDLE;
typedef void* XL_LRT_HOOK_HANDLE;
typedef void* XL_LRT_DEBUG_LOGS;

typedef void* (XLUE_STDCALL *fnGetObject)(void* userData);

typedef struct tagXLLRTGlobalAPI {
	const char *name;
	lua_CFunction func;
	unsigned long permission;
} XLLRTGlobalAPI;

struct tagXLLRTObject
{
    fnGetObject pfnGetObject;
    void* userData;
	const char* ObjName;
	const char* ClassName;
	const XLLRTGlobalAPI* MemberFunctions;
    unsigned long Permission;
};
typedef struct tagXLLRTObject XLLRTObject;

struct tagXLLRTClass
{
	const char* className;
	const char* fahterClassName;
	const XLLRTGlobalAPI* MemberFunctions;
	unsigned long permission;
};
typedef struct tagXLLRTClass XLLRTClass;

typedef struct XL_LRT_ERROR_STACK{
	XL_LRT_DEBUG_LOGS logs;
	struct {
		unsigned short top;
		unsigned long topsix;
		unsigned long all;
	}hash;
}*PXL_LRT_ERROR_STACK;
typedef int (XLUE_STDCALL *fnLuaErrorHandle)(lua_State* luaState,const wchar_t* pExtInfo,
										  const wchar_t* luaErrorString,PXL_LRT_ERROR_STACK pStackInfo);

typedef long (XLUE_STDCALL *fnGlobalSetCallback) (lua_State* luaState,const char* globalObjID,void* udata);

//-- һЩ�������Ͷ��� --
#define XLLRT_HISTORY_TYPE_ERROR 0

#define XLLRT_CHUNK_TYPE_STRING 0
#define XLLRT_CHUNK_TYPE_FILE   1
#define XLLRT_CHUNK_TYPE_MODULE 2

//debug����
#define XLLRT_DEBUG_TYPE_HOOK		0
#define XLLRT_DEBUG_TYPE_NOHOOK		1
#define XLLRT_DEBUG_TYPE_DUMPSTATE  2

#define XLLRT_DEBUG_MAX_LOG_LEN		1024
#define XLLRT_DEBUG_CALL_LOG_COUNT	1024
#define XLLRT_DEBUG_STACK_LOG_COUNT	100
#define XLLRT_DEBUG_MAX_NAME_LEN	128

// ���еĴ����붨��
#define XLLRT_RESULT_SUCCESS 0
#define XLLRT_RESULT_ENV_EXIST 1
#define XLLRT_RESULT_ENV_NOT_FOUND 2
#define XLLRT_RESULT_ENV_INVALID 3
#define XLLRT_RESULT_CHUNK_EXIST 4
#define XLLRT_RESULT_CHUNK_NOT_FOUND 5
#define XLLRT_RESULT_CHUNK_CODE_LENGTH_NOT_ALLOW 6
#define XLLRT_RESULT_CHUNK_COMPLIED 7
#define XLLRT_RESULT_CANNOT_CREATE_LUASTATE 8
#define XLLRT_RESULT_CHUNK_INVALID 9
#define XLLRT_RESULT_CHUNK_NEED_ID 10
#define XLLRT_RESULT_CHUNK_COMPLIE_ERROR 11
#define XLLRT_RESULT_RUNTIME_RUN_ERROR 12
#define XLLRT_RESULT_RUNTIME_NOT_READY 13
#define XLLRT_RESULT_RUNTIME_HAVE_INIT 14
#define XLLRT_RESULT_RUNTIME_INVALID 15
#define XLLRT_RESULT_RUNTIME_EXIST 16
#define XLLRT_RESULT_RUNTIME_NOT_FOUND 17
#define XLLRT_RESULT_FUNCTION_EXIST 18
#define XLLRT_RESULT_FUNCTION_NOT_FOUND 19
#define XLLRT_RESULT_GLOBAL_OBJ_INVALID 20
#define XLLRT_RESULT_GLOBAL_OBJ_EXIST 21
#define XLLRT_RESULT_GLOBAL_OBJ_NOT_FOUND 22
#define XLLRT_RESULT_CLASS_NOT_FOUND 23
#define XLLRT_RESULT_FILE_NOT_FOUND 24
#define XLLRT_RESULT_CLASS_EXIST    25
#define XLLRT_RESULT_NO_PERMISSION 26
#define XLLRT_RESULT_CHUNK_MOUDLE_RUN 27
#define XLLRT_RESULT_DEBUG_BUFNOTENOUGH 30
#define XLLRT_RESULT_PARAM_INVALID 31
#define XLLRT_RESULT_NOT_IMPL 32
#define XLLRT_RESULT_TYPEERROR_NOTUSERDATA 33
#define XLLRT_RESULT_TYPEERROR_NOMETATABLE 34
#define XLLRT_RESULT_TYPEERROR_NOCLASSNAME 35
#define XLLRT_RESULT_TYPEERROR_CLASSMISMATCH 36
#define XLLRT_RESULT_OUT_OF_MEMORY 37
#define XLLRT_RESULT_INSUFFICIENT_BUFFER 38

//��ȡXunlei Runtime�İ汾��
XL_LRT_API(unsigned long) XLLRT_GetVersion();

//---LUA����---
//��������
//1) ��XLLuaRuntime��չ��һЩlua api
//2) �����нű�ǰ ,�� lua �ű���ע��Ķ���,�Լ�lua�ű��������й����ж�̬ȡ�õĶ���
//3) lua��#include ����.lua�ļ���·������
//4) lua�����¼���lua������ʷ��¼
//5) lua�ű������� public object
//pstrEnvName == NULL ���� strlen(pstrEnvName) == 0 ����ʾ����/��ȡ Ĭ��Env
XL_LRT_API(XL_LRT_ENV_HANDLE) XLLRT_CreateEnv(const wchar_t* pstrEnvName);
XL_LRT_API(long) XLLRT_DestroyEnv(const wchar_t* pEnvName);

XL_LRT_API(XL_LRT_ENV_HANDLE) XLLRT_GetEnv(const wchar_t* pstrEnvName);
XL_LRT_API(unsigned long) XLLRT_AddRefEnv(XL_LRT_ENV_HANDLE hEnv);
XL_LRT_API(unsigned long) XLLRT_ReleaseEnv(XL_LRT_ENV_HANDLE hEnv);

//Env�ڵ�Global API,Global Obj����
XL_LRT_API(long) XLLRT_RegisterGlobalAPI(XL_LRT_ENV_HANDLE hEnv,XLLRTGlobalAPI theAPI);
XL_LRT_API(long) XLLRT_RemoveGlobalAPI(XL_LRT_ENV_HANDLE hEnv,const char* theAPIName);
XL_LRT_API(BOOL) XLLRT_IsGlobalAPIRegistered(XL_LRT_ENV_HANDLE hEnv,const char* theAPIName);

XL_LRT_API(long) XLLRT_RegisterGlobalObj(XL_LRT_ENV_HANDLE hEnv,XLLRTObject theObj);
XL_LRT_API(long) XLLRT_RemoveGlobalObj(XL_LRT_ENV_HANDLE hEnv,const char* objName);
XL_LRT_API(long) XLLRT_RegisterGlobalSetCallback(XL_LRT_ENV_HANDLE hEnv,fnGlobalSetCallback pfnCallback,void* udata);
XL_LRT_API(long) XLLRT_IsGlobalObjRegistered(XL_LRT_ENV_HANDLE hEnv,const char* objName);

//֧�ֵ��̳�
XL_LRT_API(long) XLLRT_RegisterClass(XL_LRT_ENV_HANDLE hEnv,const char* className,const XLLRTGlobalAPI* MemberFunctions,const char* fahterClassName,unsigned long permission);
XL_LRT_API(long) XLLRT_UnRegisterClass(XL_LRT_ENV_HANDLE hEnv,const char* className);
XL_LRT_API(long) XLLRT_DoRegisterClass(const char* className,lua_State* luaState);
XL_LRT_API(BOOL) XLLRT_IsClassRegistered(XL_LRT_ENV_HANDLE hEnv, const char* className);
XL_LRT_API(BOOL) XLLRT_IsDerivedClass(XL_LRT_ENV_HANDLE hEnv, const char* lpDerivedClass, const char* lpBaseClass);

// ö����ؽӿ�
// ���ص�ö�����ǵ�ǰenv��һ�����գ�ֻ������һ���߳�����ʹ�ã�
typedef void* XL_LRT_ENUMERATOR;

/*
ʹ��ʾ��
XL_LRT_ENUMERATOR hEnum = XLLRT_BeginEnumGlobalAPI(hEnv);
XLLRTGlobalAPI luaAPI;
while(XLLRT_GetNextGlobalAPI(hEnum, &luaAPI))
{
	TSINFO(L"name=%S, function=0x%x", luaAPI.name, luaAPI.func);
}
XLLRT_EndEnum(hEnum);
*/
XL_LRT_API(XL_LRT_ENUMERATOR) XLLRT_BeginEnumGlobalAPI(XL_LRT_ENV_HANDLE hEnv);
XL_LRT_API(BOOL) XLLRT_GetNextGlobalAPI(XL_LRT_ENUMERATOR hEnum, XLLRTGlobalAPI* lpGlobalAPI);

/*
ʹ��ʾ��
XL_LRT_ENUMERATOR hEnum = XLLRT_BeginEnumGlobalObject(hEnv);
XLLRTObject luaObject;
while(XLLRT_GetNextGlobalObject(hEnum, &luaObject))
{
	TSINFO(L"class=%S, objname=%S", luaObject.ClassName, luaObject.ObjName);
	const XLLRTGlobalAPI* lpAPI = luaObject.MemberFunctions;
	while (lpAPI->name != NULL)
	{
		TSINFO(L"name=%S, func=0x%x", lpAPI->name, lpAPI->func);
		++lpAPI;
	}
}
XLLRT_EndEnum(hEnum);
*/
XL_LRT_API(XL_LRT_ENUMERATOR) XLLRT_BeginEnumGlobalObject(XL_LRT_ENV_HANDLE hEnv);
XL_LRT_API(BOOL) XLLRT_GetNextGlobalObject(XL_LRT_ENUMERATOR hEnum, XLLRTObject* lpGlobalObj);

/*
ʹ��ʾ��
XL_LRT_ENUMERATOR hEnum = XLLRT_BeginEnumGlobalClass(hEnv);
XLLRTClass luaClass;
while(XLLRT_GetNextGlobalClass(hEnum, &luaClass))
{
	TSINFO(L"class=%S, fatherclass=%S", luaClass.className, luaClass.fahterClassName);
	const XLLRTGlobalAPI* lpAPI = luaClass.MemberFunctions;
	while (lpAPI->name != NULL)
	{
		TSINFO(L"name=%S, func=0x%x", lpAPI->name, lpAPI->func);
		++lpAPI;
	}
}
XLLRT_EndEnum(hEnum);
*/
XL_LRT_API(XL_LRT_ENUMERATOR) XLLRT_BeginEnumGlobalClass(XL_LRT_ENV_HANDLE hEnv);
XL_LRT_API(BOOL) XLLRT_GetNextGlobalClass(XL_LRT_ENUMERATOR hEnum, XLLRTClass* lpGlobalClass);

// ö�ٽ����󣬵��øýӿ����ս���ʹ�õ�ö����
// ö�����ս�󣬷��ص�class��api��obj������������ַ�����memberfunctionָ�붼��ʧЧ��
XL_LRT_API(BOOL) XLLRT_EndEnum(XL_LRT_ENUMERATOR hEnum);



// ��ȡindexָ����userdata��Ӧ��className
XL_LRT_API(const char*) XLLRT_GetXLObjectClass(lua_State* luaState, int index);

// pushһ������ָ�뵽luaջ
XL_LRT_API(long) XLLRT_PushXLObject(lua_State* luaState, const char* className, void* pRealObj);

// pushָ�����ȵĶ������ݵ�luaջ������������lpRealObjDataָ���objLen���ֽ�����
XL_LRT_API(long) XLLRT_PushXLObjectEx(lua_State *luaState, const char* className, void* lpRealObjData, size_t objDataLen);


// ������lua_touserdata����luaջ��ȡһ��void*���͵�userdata�������userdata��classNameָ����class�������࣬
// ��ô����ֵΪXLLRT_RESULT_SUCCESS����lplpRet���ظ�userdata������lplpRet����NULL
XL_LRT_API(long) XLLRT_GetXLObject(lua_State* luaState, int index, const char* className, void** lplpObj);

// ������ͬ��XLLRT_GetXLObject��ֻ�������Դ�luaջ��ȡһ��ָ�����ȵ�userdata��lpObjDataBufferָ���buffer��>=��userdata��ʵ�ʳ��ȣ���
// ���bufferLen����С��luaջ�϶�Ӧ��userdata���ݳ��ȣ���ô�᷵��XLLRT_RESULT_INSUFFICIENT_BUFFER
// lpObjDataLen��Ϊ�յĻ������ظ�userdata��ʵ�ʳ���
XL_LRT_API(long) XLLRT_GetXLObjectEx(lua_State* luaState, int index, const char* className, void* lpObjDataBuffer, size_t bufferLen, size_t *lpObjDataLen);

// ������luaL_checkuserdata������XLLRT_GetXLObjectʵ�֣���ͬ����������Ͳ���className�����࣬���߲���userdata����ô�����luaL_typerror��������
XL_LRT_API(long) XLLRT_CheckXLObject(lua_State* luaState, int index, const char* className, void** lplpObj);

// ����XLLRT_GetXLObjectExʵ�֣�����ͬ����������Ͳ���className�����࣬���߲���userdata����ô�����luaL_typerror��������
XL_LRT_API(long) XLLRT_CheckXLObjectEx(lua_State* luaState, int index, const char* className, void* lpObjDataBuffer, size_t bufferLen, size_t *lpObjDataLen);


//--LUA ����ʱ---
// ��luaState�İ���������ʹ��Э��
// һ��luaruntime������ʼ����������ͬһ���߳�
// һ��env���Դ���һ�����߶��runtime
XL_LRT_API(XL_LRT_RUNTIME_HANDLE) XLLRT_CreateRunTime(XL_LRT_ENV_HANDLE hEnv,const char* id,unsigned long permission);
XL_LRT_API(long) XLLRT_DestroyRunTime(XL_LRT_ENV_HANDLE hEnv, const char* pID);

XL_LRT_API(XL_LRT_RUNTIME_HANDLE) XLLRT_GetRuntime(XL_LRT_ENV_HANDLE hEnv,const char* pRuntimeID);
XL_LRT_API(unsigned long) XLLRT_GetRuntimePermission(XL_LRT_RUNTIME_HANDLE hRuntime);
XL_LRT_API(unsigned long) XLLRT_ReleaseRunTime(XL_LRT_RUNTIME_HANDLE hRunTime);
XL_LRT_API(unsigned long) XLLRT_AddRefRunTime(XL_LRT_RUNTIME_HANDLE hRunTime);
XL_LRT_API(XL_LRT_ENV_HANDLE) XLLRT_GetOwnerEnv(XL_LRT_RUNTIME_HANDLE hRunTime);
XL_LRT_API(const char*) XLLRT_GetRuntimeID(XL_LRT_RUNTIME_HANDLE hRuntime);
XL_LRT_API(lua_State*) XLLRT_GetLuaState(XL_LRT_RUNTIME_HANDLE hRunTime);
XL_LRT_API(XL_LRT_RUNTIME_HANDLE) XLLRT_GetRuntimeFromLuaState(lua_State* luaState);

// �л�luaruntime����ǰ�̣߳����������жԸ�runtime�Ͷ�Ӧ��luastate��ʹ�ö������µ��߳�
XL_LRT_API(unsigned long) XLLRT_SwitchRuntimeThread(XL_LRT_RUNTIME_HANDLE hRuntime);

XL_LRT_API(size_t) XLLRT_GetAllLuaState(lua_State** luaState, size_t nCount );
XL_LRT_API(const wchar_t*) XLLRT_AddLoadLuaFile(const wchar_t* lpLuaFile);
XL_LRT_API(size_t) XLLRT_GetLoadLuaFileCount();
XL_LRT_API(long) XLLRT_GetLoadLuaFileByIndex(int nIndex, wchar_t *wstrFilePath, int nLen);

//LuaCodeChunk�ı仯
//LuaCodeChunk������ʾһ��Lua Function,ͨ��LuaCodeChunk,����Եõ�һ��lua function,����򵥵�ֱ��������
//LuaCodeChunk��name���پ���id������,�����Ҫ���У����ã���ʹ���߹���
//����3�ַ�������lua chunk
//1) ָ����Ӧ��luaԴ�ļ���·��
//2) ָ����Ӧ��luaģ���·��,�Լ���Ҫ�ĺ�����
//3) �ڴ��еĴ���Ƭ��

//����Chunkֻ�Ǵ���Chunk���󣬲���¼������,Դλ��! lua����ĺϷ���Ҫ��RunChunk����GetChunkFunc��ʱ�����Ч
XL_LRT_API(unsigned long) XLLRT_AddRefChunk(XL_LRT_CHUNK_HANDLE hChunk);
XL_LRT_API(unsigned long) XLLRT_ReleaseChunk(XL_LRT_CHUNK_HANDLE hChunk);

XL_LRT_API(const wchar_t*) XLLRT_GetChunkName(XL_LRT_CHUNK_HANDLE hChunk);
XL_LRT_API(unsigned long) XLLRT_GetChunkType(XL_LRT_CHUNK_HANDLE hChunk);

//��֧����򵥵�string���͵�CodeChunk, pCodeBufferΪ���ĵ�LUA�ű�,����ΪUTF-8
XL_LRT_API(unsigned long) XLLRT_CreateChunk(const wchar_t* pstrName,const unsigned char* pCodeBuffer,unsigned long len,XL_LRT_CHUNK_HANDLE* pResult);

// code chunk��Ӧһ��lua�ļ�
XL_LRT_API(unsigned long) XLLRT_CreateChunkFromFile(const wchar_t* pstrName,const wchar_t* path,XL_LRT_CHUNK_HANDLE* pResult);

//code chunk��Ӧmodule�е�һ������
XL_LRT_API(unsigned long) XLLRT_CreateChunkFromModule(const wchar_t* pstrName,const wchar_t* modulePath,const char* func,XL_LRT_CHUNK_HANDLE* pResult);

//�ڻ�����ִ�и�Chunk.
XL_LRT_API(long) XLLRT_RunChunk(XL_LRT_RUNTIME_HANDLE hRunTime,XL_LRT_CHUNK_HANDLE hChunk);

//�õ�Chunk��Ӧ��lua function,ѹ��lua stack�Ķ�����
XL_LRT_API(long) XLLRT_PrepareChunk(XL_LRT_RUNTIME_HANDLE hRunTime,XL_LRT_CHUNK_HANDLE hChunk);

// ����luaջ���ĺ���
XL_LRT_API(long) XLLRT_LuaCall(lua_State* luaState,int args,int results,const wchar_t* contextInfo);

// ��Ҫ������һ������ص���������luacall���ֽű�����󣬻�ص��ú���
XL_LRT_API(long) XLLRT_ErrorHandle(fnLuaErrorHandle  pfnErrorHandle);

// ��ȡ��ǰluastate���������һ��������Ϣ
XL_LRT_API(const char*) XLLRT_GetLastError(lua_State* luaState);

// HOOK��غ���
XL_LRT_API(XL_LRT_HOOK_HANDLE) XLLRT_SetGlobalAPIHook(XL_LRT_ENV_HANDLE hEnv, const char *name, lua_CFunction func, BOOL pre);
XL_LRT_API(BOOL) XLLRT_RemoveGlobalAPIHook(XL_LRT_ENV_HANDLE hEnv, const char* name, XL_LRT_HOOK_HANDLE hook);

XL_LRT_API(XL_LRT_HOOK_HANDLE) XLLRT_SetGlobalObjectFunctionHook(XL_LRT_ENV_HANDLE hEnv, const char* objName, const char* name, lua_CFunction func, BOOL pre);
XL_LRT_API(BOOL) XLLRT_RemoveGlobalObjectFunctionHook(XL_LRT_ENV_HANDLE hEnv, const char* objName, const char* name, XL_LRT_HOOK_HANDLE hook);

XL_LRT_API(XL_LRT_HOOK_HANDLE) XLLRT_SetClassFunctionHook(XL_LRT_ENV_HANDLE hEnv, const char* className, const char* name, lua_CFunction func, BOOL pre);
XL_LRT_API(BOOL) XLLRT_RemoveClassFunctionHook(XL_LRT_ENV_HANDLE hEnv, const char* className, const char* name, XL_LRT_HOOK_HANDLE hook);

//debug interface
//mem pool && logs
typedef void* XL_LRT_DEBUG_MEMPOOL;

XL_LRT_API(long) XLLRT_DebugLogsPopNextLog(const char**pLog, XL_LRT_DEBUG_LOGS logs);
XL_LRT_API(long) XLLRT_DebugMemPoolGetLogs(const XL_LRT_DEBUG_MEMPOOL MemPool,XL_LRT_DEBUG_LOGS logs);
XL_LRT_API(long) XLLRT_DebugCreateStackMemPool(XL_LRT_DEBUG_MEMPOOL* pMemPool);
XL_LRT_API(long) XLLRT_DebugDesroyStackMemPool(XL_LRT_DEBUG_MEMPOOL MemPool);
XL_LRT_API(long) XLLRT_DebugCreateLogs(XL_LRT_DEBUG_LOGS* pLogs);
XL_LRT_API(long) XLLRT_DebugDestroyLogs(XL_LRT_DEBUG_LOGS logs);
//init 
XL_LRT_API(long) XLLRT_DebugInit(const char DebugeeName[],long dwDbgType);
XL_LRT_API(long) XLLRT_DebugGetDebugeeName(char* pBuf);
XL_LRT_API(long) XLLRT_DebugGetType(long* pType);
//logs 
XL_LRT_API(long) XLLRT_DebugGetCallLog(XL_LRT_DEBUG_MEMPOOL* pMemPool);
XL_LRT_API(long) XLLRT_DebugGetStackLog(XL_LRT_DEBUG_MEMPOOL* pMemPool);
XL_LRT_API(long) XLLRT_DebugGetProcessCallLog(const char DebugeeName[],XL_LRT_DEBUG_MEMPOOL* pMemPool);
XL_LRT_API(long) XLLRT_DebugGetProcessStackLog(const char DebugeeName[],XL_LRT_DEBUG_MEMPOOL* pMemPool);
//no logs
XL_LRT_API(long) XLLRT_DebugOutputLuaStack(lua_State* pState,XL_LRT_DEBUG_MEMPOOL MemPool);
XL_LRT_API(long) XLLRT_DebugGetCurState(lua_State** ppState);
XL_LRT_API(long) XLLRT_DebugGetProcessCurState(const char DebugeeName[],lua_State** ppState);
XL_LRT_API(long) XLLRT_DebugDestroyState(lua_State* pState);

//for minidump
//call back in writedump, inputed prama type should be PVOID, const PMINIDUMP_CALLBACK_INPUT, const PMINIDUMP_CALLBACK_OUTPUT
XL_LRT_API(int) XLLRT_DebugGetDumpList(void* CallbackParam,const void* CallbackInput,
										  const void*CallbackOutput);
//for read lua state from dump file , the second inputed praram type should be PMINIDUMP_MEMORY_LIST
XL_LRT_API(long) XLLRT_DebugGetStateFromDump(void* pDumpData,void* pMemList,lua_State**ppState);

// �߼����ܣ���ȡ������������ַ
XL_LRT_API(lua_CFunction) XLLRT_GetFunctionAddress( lua_CFunction lpFun );

XL_LRT_API(long) XLLRT_Stat(long type);

XL_LRT_API(long) XLLRT_DumpTable(XL_LRT_RUNTIME_HANDLE hRuntime, long tableRef);


// int64����֧�ֵ���ؽӿ�
XL_LRT_API(BOOL) XLLRT_RegisterInt64(XL_LRT_RUNTIME_HANDLE hRuntime);

XL_LRT_API(BOOL) XLLRT_PushInt64(lua_State *luaState, long long value);
XL_LRT_API(long long) XLLRT_CheckInt64(lua_State *luaState, int index);
// ���Ŀ���ǲ���int64���ͣ���ô��ת��Ϊlua_number������
XL_LRT_API(long long) XLLRT_GetInt64(lua_State *luaState, int index);

#endif //_XUNLEI_LUA_RUNTIME_H_