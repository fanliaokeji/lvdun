#include "stdafx.h"
#include "DiDaCalendarHelper.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <string>

bool IsCalendarInjected()
{
	HANDLE hMutex = ::OpenMutex(SYNCHRONIZE, FALSE, L"DidaCalendar_{58C192A2-F432-48A1-96D4-F0629A6F6B80}");
	if(hMutex == NULL) {
		return false;
	}
	::CloseHandle(hMutex);
	return true;
}

static BOOL MyIsWow64Process(HANDLE hProcess, PBOOL Wow64Process)
{
	typedef BOOL (WINAPI* IsWow64Process_FuncType)(HANDLE hProcess, PBOOL Wow64Process);
	IsWow64Process_FuncType isWow64Process = reinterpret_cast<IsWow64Process_FuncType>(::GetProcAddress(::GetModuleHandle(L"kernel32.dll"), "IsWow64Process"));
	if(isWow64Process == NULL) {
		*Wow64Process = FALSE;
		return TRUE;
	}
	BOOL ret = isWow64Process(hProcess, Wow64Process);
	return ret;
}

static bool Is64BitsWindows()
{
#ifdef _WIN64
	return true;
#else
	BOOL isWow64Process = FALSE;
	MyIsWow64Process(::GetCurrentProcess(), &isWow64Process);
	return isWow64Process != FALSE;
#endif
}

static bool IsExplorerPID(DWORD dwProcessID)
{
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hProcessSnap == INVALID_HANDLE_VALUE) {
		return false;
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if(!Process32First(hProcessSnap, &pe32)) {
		::CloseHandle(hProcessSnap);
		return false;
	}

	bool result = false;
	do {
		if(pe32.th32ProcessID == dwProcessID) {
			if(_wcsicmp(pe32.szExeFile, L"explorer.exe") == 0) {
				result = true;
			}
			break;
		}
	} while(Process32Next(hProcessSnap, &pe32));

	::CloseHandle(hProcessSnap);
	return result;
}

#define INJECT_RESULT					int 
#define INJECT_OK						0
#define INJECT_FAIL_NOTIMPLEMENTED		1
#define INJECT_FAIL_OPENPROCESS			2
#define INJECT_FAIL_VIRTUALALLOC		3
#define INJECT_FAIL_WRITEPROCESSMEMERY	4
#define INJECT_FAIL_GETPROCADDRESS		5
#define INJECT_FAIL_CREATEREMOTETHREAD	6

static INJECT_RESULT Win32Inject32(DWORD dwProcessID, const wchar_t* dllFilePath)
{
	int path_length = std::wcslen(dllFilePath);
	int alloc_size = (path_length + 1) * 2;
	HANDLE hTargetProcess = ::OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, dwProcessID);
	if (hTargetProcess == NULL) {
		return INJECT_FAIL_OPENPROCESS;
	}
	LPVOID lpVirtualMem = ::VirtualAllocEx(hTargetProcess, NULL, alloc_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (lpVirtualMem == NULL) {
		::CloseHandle(hTargetProcess);
		return INJECT_FAIL_VIRTUALALLOC;
	}
	if (::WriteProcessMemory(hTargetProcess, lpVirtualMem, dllFilePath, alloc_size, NULL) == FALSE) {
		::VirtualFreeEx(hTargetProcess, lpVirtualMem, alloc_size, MEM_RESERVE | MEM_COMMIT);
		::CloseHandle(hTargetProcess);
		return INJECT_FAIL_WRITEPROCESSMEMERY;
	}
	FARPROC LoadLibraryW_Addr = GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "LoadLibraryW");
	if (LoadLibraryW_Addr == NULL) {
		::VirtualFreeEx(hTargetProcess, lpVirtualMem, alloc_size, MEM_RESERVE | MEM_COMMIT);
		::CloseHandle(hTargetProcess);
		return INJECT_FAIL_GETPROCADDRESS;
	}
	HANDLE hRemoteThread = ::CreateRemoteThread(hTargetProcess, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryW_Addr), lpVirtualMem, 0, NULL);
	if(hRemoteThread == NULL) {
		::VirtualFreeEx(hTargetProcess, lpVirtualMem, alloc_size, MEM_RESERVE | MEM_COMMIT);
		::CloseHandle(hTargetProcess);
		return INJECT_FAIL_CREATEREMOTETHREAD;
	}
	::WaitForSingleObject(hRemoteThread, INFINITE);
	::VirtualFreeEx(hTargetProcess, lpVirtualMem, alloc_size, MEM_RESERVE | MEM_COMMIT);
	::CloseHandle(hTargetProcess);
	return INJECT_OK;
}

static INJECT_RESULT Win32Inject64(DWORD dwProcessID, const wchar_t* dllFilePath)
{
	return INJECT_FAIL_NOTIMPLEMENTED;
}

bool InjectCalendarDll(const wchar_t* dllPath32, const wchar_t* dllPath64)
{
	if(dllPath32 == NULL || dllPath64 == NULL) {
		return false;
	}
	if(IsCalendarInjected()) {
		return false;
	}
	HWND hWnd = NULL;
	for(;;) {
		hWnd = ::FindWindowEx(NULL, hWnd, L"Shell_TrayWnd", NULL);
		if(::IsWindow(hWnd) == FALSE) {
			break;
		}
		DWORD dwProcessID = 0;
		::GetWindowThreadProcessId(hWnd, &dwProcessID);
		if(dwProcessID == 0) {
			continue;
		}
		if(!IsExplorerPID(dwProcessID)) {
			continue;
		}
		break;
	}
	if (::IsWindow(hWnd))
	{
		hWnd = ::FindWindowEx(hWnd, 0, L"TrayNotifyWnd", NULL);
		if (::IsWindow(hWnd))
		{
			hWnd = ::FindWindowEx(hWnd, 0, L"TrayClockWClass", NULL);
		}
	}
	DWORD dwProcessID = 0;
	if(::IsWindow(hWnd) == FALSE) {
		return false;
	}
	::GetWindowThreadProcessId(hWnd, &dwProcessID);
	if(dwProcessID == 0) {
		return false;
	}
#ifdef _WIN64
#error error
#endif
	if(Is64BitsWindows()) {
		return Win32Inject64(dwProcessID, dllPath64) == INJECT_OK;
	}
	else {
		return Win32Inject32(dwProcessID, dllPath32) == INJECT_OK;
	}
}
