#include "StdAfx.h"
#include <Windows.h>
#include <WinUser.h>
#include "ScriptableNPObject.h"
#include <comutil.h>
#include <stdio.h>
#include <shlobj.h>  

#include "shellapi.h"

#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "kernel32.lib")

//#pragma comment(lib, "Shell32.lib")


NPClass ScriptableNPObject::_npclass = {
	NP_CLASS_STRUCT_VERSION,
	ScriptableNPObject::Allocate,
	ScriptableNPObject::_Deallocate,
	ScriptableNPObject::_Invalidate,
	ScriptableNPObject::_HasMethod,
	ScriptableNPObject::_Invoke,
	ScriptableNPObject::_InvokeDefault,
	ScriptableNPObject::_HasProperty,
	ScriptableNPObject::_GetProperty,
	ScriptableNPObject::_SetProperty,
	ScriptableNPObject::_RemoveProperty,
	ScriptableNPObject::_Enumerate,
	ScriptableNPObject::_Construct
}; 

NPNetscapeFuncs* ScriptableNPObject::npnfuncs = NULL;

// static
void ScriptableNPObject::_Deallocate(NPObject *npobj) {
	TSAUTO();
	((ScriptableNPObject*)npobj)->Deallocate();
	delete ((ScriptableNPObject*)npobj);
}

void ScriptableNPObject::_Invalidate(NPObject *npobj) {
	TSAUTO();
	((ScriptableNPObject*)npobj)->Invalidate();
}

bool ScriptableNPObject::_HasMethod(NPObject *npobj, NPIdentifier name) {
	TSAUTO();
	return ((ScriptableNPObject*)npobj)->HasMethod(name);
}

bool ScriptableNPObject::_Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result) {
	TSAUTO();
	return ((ScriptableNPObject*)npobj)->Invoke(name, args, argCount, result);
}

bool ScriptableNPObject::_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result) {
	TSAUTO();
	return ((ScriptableNPObject*)npobj)->InvokeDefault(args, argCount, result);
}

bool ScriptableNPObject::_HasProperty(NPObject * npobj, NPIdentifier name) {
	TSAUTO();
	return ((ScriptableNPObject*)npobj)->HasProperty(name);
}

bool ScriptableNPObject::_GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result) {
	TSAUTO();
	return ((ScriptableNPObject*)npobj)->GetProperty(name, result);
}

bool ScriptableNPObject::_SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value) {
	TSAUTO();
	return ((ScriptableNPObject*)npobj)->SetProperty(name, value);
}

bool ScriptableNPObject::_RemoveProperty(NPObject *npobj, NPIdentifier name) {
	TSAUTO();
	return ((ScriptableNPObject*)npobj)->RemoveProperty(name);
}

bool ScriptableNPObject::_Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count) {
	TSAUTO();
	return ((ScriptableNPObject*)npobj)->Enumerate(identifier, count);
}

bool ScriptableNPObject::_Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result) {
	TSAUTO();
	return ((ScriptableNPObject*)npobj)->Construct(args, argCount, result);
}

//-----------------------------------------------------------------------------------------------------------------------------------

void ScriptableNPObject::Invalidate() {
	// Invalidate the control however you wish
	TSAUTO();
}

void ScriptableNPObject::Deallocate() {
	// Do any cleanup needed
	TSAUTO();
} 

// static

bool ScriptableNPObject::HasMethod(NPIdentifier name) {
	TSAUTO();
	return true;
}

//拉起飞兔
enum BrowserTaskType
{
	IMAGE=0,
	OTHER
};

struct FRBrowserTaskInfo
{
	//std::wstring wstrUrl;
	wchar_t wszUrl[1024];
	BrowserTaskType type;
	int posX;
	int posY;
};

HWND GetHwndMsgListenerOK(bool bRuning = false)
{
	HANDLE hMutex;
	DWORD dwRet;
	//互斥量存在则说明消息监听ok
	for(int i = 1; i <= 5; ++i){
		hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"xarmutex_{455EB122-3F18-4139-AE47-255F940CBCF0}");
		dwRet = GetLastError();
		if (hMutex == NULL ){
			if(!bRuning){
				return NULL;
			} else {
				Sleep(i*100);
			}
		} else {
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			break;
		}
	}
	HWND hwnd;
	for(int i = 0; i < 3; ++i){
		hwnd = ::FindWindowA("{1DA1F328-DB3D-4f6a-A62E-E9B2B22F2B9A}_frmainmsg", NULL);
		if(hwnd != NULL){
			return hwnd;
		} else {
			Sleep(100);
		}
	}
	return NULL;
}

HWND __stdcall CreateProcessAndGetHwnd()
{
	HWND hwnd = GetHwndMsgListenerOK();
	if(hwnd != NULL){
		return hwnd;
	}
	HKEY hKEY;
	LPCSTR data_Set= "Software\\FlyRabbit";
	if (ERROR_SUCCESS != ::RegOpenKeyExA(HKEY_LOCAL_MACHINE,data_Set,0,KEY_READ,&hKEY)){
		::RegCloseKey(hKEY);
		return NULL;
	}
	char szPathValue[MAX_PATH] = {0};
	DWORD dwSize = sizeof(szPathValue);
	DWORD dwType = REG_SZ;
	if (::RegQueryValueExA(hKEY,"Path", 0, &dwType, (LPBYTE)szPathValue, &dwSize) != ERROR_SUCCESS){
		::RegCloseKey(hKEY);
		return NULL;
	}
	char szBrowserPath[MAX_PATH] = {0}, szBrowserName[128] = {0};
	if(!GetModuleFileNameA(NULL, szBrowserPath, MAX_PATH)){
		strcpy(szBrowserName, "unknown");
	} else {
		strcpy(szBrowserName, PathFindFileNameA(szBrowserPath));
	}
	char szCmdLine[MAX_PATH] = {0};
	sprintf(szCmdLine, "/sstartfrom npFlyRabbitCtrl /browsername %s /embedding", szBrowserName);
	STARTUPINFOA si = {sizeof(si)};
	PROCESS_INFORMATION pi;
	si.dwFlags=STARTF_USESHOWWINDOW;//指定wShowWindow成员有效
	si.wShowWindow=TRUE;//此成员设为TRUE的话则显示新建进程的主窗口
	BOOL bRet=CreateProcessA(
		szPathValue,
		szCmdLine,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi);
	if(bRet){
		WaitForInputIdle(pi.hProcess, 5000);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	} 
	hwnd = GetHwndMsgListenerOK(true);
	::RegCloseKey(hKEY);
	return hwnd;
}

unsigned int __stdcall ThreadFun(PVOID pM)  
{
	HWND hwnd = CreateProcessAndGetHwnd();
	if(hwnd != NULL){
		COPYDATASTRUCT cpd;
		cpd.dwData = 0;
		cpd.cbData = sizeof(FRBrowserTaskInfo);
		cpd.lpData = pM;
		::SendMessageA(hwnd, WM_COPYDATA, 1, (LPARAM)&cpd);
		delete []pM;
	} else {
		delete []pM;
		MessageBoxA(0, "未安装飞兔", "提醒", 0);
	}
	return 0;
};

std::wstring  StringToWString(const std::string& s)
{
	std::wstring wszStr;

	int nLength = MultiByteToWideChar( CP_ACP, 0, s.c_str(), -1, NULL, NULL );
	wszStr.resize(nLength);
	LPWSTR lpwszStr = new wchar_t[nLength];
	MultiByteToWideChar( CP_ACP, 0, s.c_str(), -1, lpwszStr, nLength );
	wszStr = lpwszStr;
	delete [] lpwszStr;

	return wszStr;
}

// static
bool ScriptableNPObject::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result) {
	TSAUTO();
	int error = 1;
	char *methodname = npnfuncs->utf8fromidentifier(name);
	LOG4C_TRACE(_T("methodname == %S"), methodname);
	if(methodname) {
		if (!_stricmp(methodname, "AddTask")){
			if (argCount >= 1 && NPVARIANT_IS_STRING(args[0])){
				VARIANT value;
				NPVar2Variant(&args[0], &value);
				FRBrowserTaskInfo *psBTI = new FRBrowserTaskInfo;
				_bstr_t bstrUrl = value.bstrVal;
				wcscpy(psBTI->wszUrl, bstrUrl);
				psBTI->posX = 0;
				psBTI->posY = 0;
				psBTI->type = OTHER;
				_beginthreadex(NULL, 0, ThreadFun, psBTI, 0, NULL);
				return true;
			}
		}
		
	}	
	// aim exception handling
	LOG4C_ERROR(_T("[ScriptableNPObject::Invoke] set exception during invocation, Parameter is not right!"));
	npnfuncs->setexception(this, "ScriptableNPOjbect set exception during invocation, Parameter is not right!");
	return false;
}

// static

bool ScriptableNPObject::InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result) {
	TSAUTO();
	return true;
}

bool ScriptableNPObject::HasProperty(NPIdentifier name) {
	TSAUTO();
	return false;
}

bool ScriptableNPObject::GetProperty(NPIdentifier name, NPVariant *result) {
	TSAUTO();
	return false;
}

bool ScriptableNPObject::SetProperty(NPIdentifier name, const NPVariant *value) {
	TSAUTO();
	return false;
}

bool ScriptableNPObject::RemoveProperty(NPIdentifier name) {
	TSAUTO();
	return false;
}

bool ScriptableNPObject::Enumerate(NPIdentifier **identifier, uint32_t *count) {
	TSAUTO();
	return false;
}

bool ScriptableNPObject::Construct(const NPVariant *args, uint32_t argCount, NPVariant *result) {
	TSAUTO();
	return false;
}

BSTR ScriptableNPObject::Utf8StringToBstr(LPCSTR szStr, int iSize)
{ 
	BSTR bstrVal; 
	// Chars required for string
	int iReq = 0;

	if (iSize > 0) {

		if ((iReq = MultiByteToWideChar(CP_UTF8, 0, szStr, iSize, 0, 0)) ==  0) {

			return (0); 
		}
	}

	// Account for terminating 0. 
	if (iReq != -1) {

		++iReq; 
	}

	if ((bstrVal = ::SysAllocStringLen(0, iReq)) == 0) {

		return (0); 
	}

	memset(bstrVal, 0, iReq * sizeof(wchar_t));

	if (iSize > 0) {
		// Convert into the buffer. 
		if (MultiByteToWideChar(CP_UTF8, 0, szStr, iSize, bstrVal, iReq) == 0) {

			::SysFreeString(bstrVal); 
			return 0; 
		} 
	}

	return (bstrVal); 
}

std::string ScriptableNPObject::BstrToString(BSTR bstr)
{
	// this is the BSTR
	//BSTR Bstr = ::SysAllocString(L"Hello World"); 

	// covert to std::string
	_bstr_t bstr_t(bstr);
	std::string str(bstr_t);
	return str;
	// free the BSTR
	//::SysFreeString(Bstr);
}

void ScriptableNPObject::BSTR2NPVar(BSTR bstr, NPVariant *npvar, NPP instance)
{
	USES_CONVERSION;
	char *npStr = NULL;
	size_t sourceLen;
	size_t bytesNeeded;

	sourceLen = lstrlenW(bstr);

	bytesNeeded = WideCharToMultiByte(CP_UTF8,
		0,
		bstr,
		sourceLen,
		NULL,
		0,
		NULL,
		NULL);

	bytesNeeded += 1;

	// complete lack of documentation on Mozilla's part here, I have no
	// idea how this string is supposed to be freed
	npStr = (char *)npnfuncs->memalloc(bytesNeeded);
	if (npStr) {

		memset(npStr, 0, bytesNeeded);

		WideCharToMultiByte(CP_UTF8,
			0,
			bstr,
			sourceLen,
			npStr,
			bytesNeeded - 1,
			NULL,
			NULL);

		STRINGZ_TO_NPVARIANT(npStr, (*npvar));
	}
	else {

		STRINGZ_TO_NPVARIANT(NULL, (*npvar));
	}
}

#define GETVALUE(var, val)	(((var->vt) & VT_BYREF) ? *(var->p##val) : (var->val))

typedef long int32;

void ScriptableNPObject::Variant2NPVar(const VARIANT *var, NPVariant *npvar, NPP instance)
{
	NPObject *obj = NULL;
	SAFEARRAY *parray = NULL;

	if (!var || !npvar) {

		return;
	}

	VOID_TO_NPVARIANT(*npvar);

	switch (var->vt & ~VT_BYREF) {
	//case VT_ARRAY:
	//	obj = SafeArray2NPObject(GETVALUE(var, parray), 0, NULL, instance);
	//	OBJECT_TO_NPVARIANT(obj, (*npvar));
	//	break;

	case VT_EMPTY:
		VOID_TO_NPVARIANT((*npvar));
		break;

	case VT_NULL:
		NULL_TO_NPVARIANT((*npvar));
		break;

	case VT_LPSTR:
		// not sure it can even appear in a VARIANT, but...
		STRINGZ_TO_NPVARIANT(var->pcVal, (*npvar));
		break;

	case VT_BSTR:
		BSTR2NPVar(GETVALUE(var, bstrVal), npvar, instance);
		break;

	case VT_I1:
		INT32_TO_NPVARIANT((int32)GETVALUE(var, cVal), (*npvar));
		break;

	case VT_I2:
		INT32_TO_NPVARIANT((int32)GETVALUE(var, iVal), (*npvar));
		break;

	case VT_I4:
		INT32_TO_NPVARIANT((int32)GETVALUE(var, lVal), (*npvar));
		break;

	case VT_UI1:
		INT32_TO_NPVARIANT((int32)GETVALUE(var, bVal), (*npvar));
		break;

	case VT_UI2:
		INT32_TO_NPVARIANT((int32)GETVALUE(var, uiVal), (*npvar));
		break;

	case VT_UI4:
		INT32_TO_NPVARIANT((int32)GETVALUE(var, ulVal), (*npvar));
		break;

	case VT_BOOL:
		BOOLEAN_TO_NPVARIANT((GETVALUE(var, boolVal) == VARIANT_TRUE) ? true : false, (*npvar));
		break;

	case VT_R4:
		DOUBLE_TO_NPVARIANT((double)GETVALUE(var, fltVal), (*npvar));
		break;

	case VT_I8:
		npvar->type = NPVariantType_Double;
		npvar->value.doubleValue = double(var->llVal);
		break;

	case VT_R8:
		DOUBLE_TO_NPVARIANT(GETVALUE(var, dblVal), (*npvar));
		break;

	//case VT_DISPATCH:
	//	Dispatch2NPVar(GETVALUE(var, pdispVal), npvar, instance);
	//	break;

	//case VT_UNKNOWN:
	//	Unknown2NPVar(GETVALUE(var, punkVal), npvar, instance);
	//	break;

	default:
		// Some unsupported type
		break;
	}
}
#undef GETVALUE

void ScriptableNPObject::NPVar2Variant(const NPVariant *npvar, VARIANT *var)
{
	USES_CONVERSION;

	if (!var || !npvar) {

		return;
	}

	switch (npvar->type) {
	case NPVariantType_Void:
		var->vt = VT_EMPTY;
		var->ulVal = 0;
		break;

	case NPVariantType_Null:
		var->vt = VT_NULL;
		var->byref = NULL;
		break;

	case NPVariantType_Bool:
		var->vt = VT_BOOL;
		var->ulVal = npvar->value.boolValue;
		break;

	case NPVariantType_Int32:
		var->vt = VT_I4;
		var->ulVal = npvar->value.intValue;
		break;

	case NPVariantType_Double:
		var->vt = VT_R8;
		var->dblVal = npvar->value.doubleValue;
		break;

	case NPVariantType_String:
		var->vt = VT_BSTR;
		var->bstrVal = Utf8StringToBstr(npvar->value.stringValue.UTF8Characters, npvar->value.stringValue.UTF8Length);
		break;

	case NPVariantType_Object:
		// An object of type NPObject - currently not supported.
		var->vt = VT_EMPTY;
		var->ulVal = 0;
		break;

	default:
		var->vt = VT_EMPTY;
		var->ulVal = 0;
		break;
	}
}