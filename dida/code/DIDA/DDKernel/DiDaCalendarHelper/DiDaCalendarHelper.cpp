#include "stdafx.h"
#include "DiDaCalendarHelper.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include "wow64ext.h"

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
	::WaitForSingleObject(hRemoteThread, 3000);
	::CloseHandle(hRemoteThread);
	::VirtualFreeEx(hTargetProcess, lpVirtualMem, alloc_size, MEM_RESERVE | MEM_COMMIT);
	::CloseHandle(hTargetProcess);
	return INJECT_OK;
}

static const size_t PrefixCodeLength = 25;
const static unsigned char exec_code[] = {
	0x48, 0x89, 0x4c, 0x24, 0x08, 0x57, 0x48, 0x83, 0xec, 0x20,
	0x48, 0x8b, 0xfc, 0xb9, 0x08, 0x00, 0x00, 0x00, 0xb8, 0xcc,
	0xcc, 0xcc, 0xcc, 0xf3, 0xab,                               // prefix code
	0x49, 0xb9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov r9, 0x0000000000000000
	0x49, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov r8, 0x0000000000000000 
	0x48, 0xba, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rdx, 0x0000000000000000
	0x48, 0xb9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rcx, 0x0000000000000000
	0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, 0x0000000000000000 
	0xff, 0xd0,													// call rax
	0x48, 0xb9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rcx, 0x0000000000000000
	0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, 0x0000000000000000
	0xff, 0xd0													// call rax
};

static INJECT_RESULT Win32Inject64(DWORD dwProcessID, const wchar_t* dllFilePath)
{
	int path_length = std::wcslen(dllFilePath);
	HANDLE hTargetProcess = ::OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, dwProcessID);
	if (hTargetProcess == NULL) {
		return INJECT_FAIL_OPENPROCESS;
	}

	int alloc_size = sizeof(DWORD64);
	alloc_size += sizeof(_UNICODE_STRING_T<DWORD64>);
	alloc_size += (path_length + 1) * sizeof(wchar_t);

	DWORD64 lpVirtualMemExec = VirtualAllocEx64(hTargetProcess, NULL, sizeof(exec_code), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	DWORD64 lpVirtualMemParameters = VirtualAllocEx64(hTargetProcess, NULL, alloc_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if(lpVirtualMemExec == NULL || lpVirtualMemParameters == NULL) {
		if(lpVirtualMemExec != NULL) {
			VirtualFreeEx64(hTargetProcess, lpVirtualMemExec, sizeof(exec_code), MEM_RESERVE | MEM_COMMIT);
		}
		if(lpVirtualMemParameters != NULL) {
			VirtualFreeEx64(hTargetProcess, lpVirtualMemParameters, alloc_size, MEM_RESERVE | MEM_COMMIT);
		}
		::CloseHandle(hTargetProcess);
		return INJECT_FAIL_VIRTUALALLOC;
	}
	DWORD64 ntdll64 = GetModuleHandle64(L"ntdll.dll");
	DWORD64 ntdll_LdrLoadDll = GetProcAddress64(ntdll64, "LdrLoadDll");
	DWORD64 ntdll_RtlExitUserThread = GetProcAddress64(ntdll64, "RtlExitUserThread");
	DWORD64 ntdll_RtlCreateUserThread = GetProcAddress64(ntdll64, "RtlCreateUserThread");
	if(ntdll_LdrLoadDll == 0 || ntdll_RtlExitUserThread == 0 || ntdll_RtlCreateUserThread == 0) {
		VirtualFreeEx64(hTargetProcess, lpVirtualMemExec, sizeof(exec_code), MEM_RESERVE | MEM_COMMIT);
		VirtualFreeEx64(hTargetProcess, lpVirtualMemParameters, alloc_size, MEM_RESERVE | MEM_COMMIT);
		::CloseHandle(hTargetProcess);
		return INJECT_FAIL_GETPROCADDRESS;
	}

	unsigned char* parameters = new unsigned char[alloc_size];
	std::memset(parameters, 0, alloc_size);
	_UNICODE_STRING_T<DWORD64>* upath = reinterpret_cast<_UNICODE_STRING_T<DWORD64>*>(parameters + sizeof(DWORD64));
	upath->Length = (path_length) * sizeof(wchar_t);
	upath->MaximumLength = (path_length + 1) * sizeof(wchar_t);
	wchar_t* path = reinterpret_cast<wchar_t*>(parameters + sizeof(DWORD64) + sizeof(_UNICODE_STRING_T<DWORD64>));
	std::wcscpy(path, dllFilePath);
	upath->Buffer = lpVirtualMemParameters + sizeof(DWORD64) + sizeof(_UNICODE_STRING_T<DWORD64>);

	unsigned char exec_code_copy[sizeof(exec_code)];
	std::memcpy(exec_code_copy, exec_code, sizeof(exec_code_copy));
	union {
		DWORD64 dw64;
		unsigned char bytes[8];
	} cvt;
	// arg4
	cvt.dw64 = lpVirtualMemParameters;
	std::memcpy(exec_code_copy + 2 + PrefixCodeLength, cvt.bytes, sizeof(cvt.bytes));
	// arg3
	cvt.dw64 = lpVirtualMemParameters + sizeof(DWORD64);
	std::memcpy(exec_code_copy + 12 + PrefixCodeLength, cvt.bytes, sizeof(cvt.bytes));

	// rax = LdrLoadDll
	cvt.dw64 = ntdll_LdrLoadDll;
	std::memcpy(exec_code_copy + 42 + PrefixCodeLength, cvt.bytes, sizeof(cvt.bytes));

	// rax = RtlExitUserThread
	cvt.dw64 = ntdll_RtlExitUserThread;
	std::memcpy(exec_code_copy + 64 + PrefixCodeLength, cvt.bytes, sizeof(cvt.bytes));
	if(FALSE == WriteProcessMemory64(hTargetProcess, lpVirtualMemExec, exec_code_copy, sizeof(exec_code), NULL)
		|| FALSE == WriteProcessMemory64(hTargetProcess, lpVirtualMemParameters, parameters, alloc_size, NULL)) {
		VirtualFreeEx64(hTargetProcess, lpVirtualMemExec, sizeof(exec_code), MEM_RESERVE | MEM_COMMIT);
		VirtualFreeEx64(hTargetProcess, lpVirtualMemParameters, alloc_size, MEM_RESERVE | MEM_COMMIT);
		::CloseHandle(hTargetProcess);
		return INJECT_FAIL_WRITEPROCESSMEMERY;
	}
	DWORD64 hRemoteThread = 0;
	struct {
	  DWORD64 UniqueProcess;
	  DWORD64 UniqueThread;
	} client_id;

	X64Call(ntdll_RtlCreateUserThread, 10, 
		(DWORD64)hTargetProcess, // ProcessHandle
		(DWORD64)NULL, // SecurityDescriptor
		(DWORD64)FALSE, // CreateSuspended
		(DWORD64)0, // StackZeroBits
		(DWORD64)NULL, // StackReserved
		(DWORD64)NULL, // StackCommit
		lpVirtualMemExec, // StartAddress
		(DWORD64)NULL, // StartParameter
		(DWORD64)&hRemoteThread, // ThreadHandle
		(DWORD64)&client_id // ClientID
		);
	INJECT_RESULT result = INJECT_OK;
	if(hRemoteThread == 0) {
		result = INJECT_FAIL_CREATEREMOTETHREAD;
	}
	else {
		::WaitForSingleObject((HANDLE)hRemoteThread, 3000);
	}
	VirtualFreeEx64(hTargetProcess, lpVirtualMemExec, sizeof(exec_code), MEM_RESERVE | MEM_COMMIT);
	VirtualFreeEx64(hTargetProcess, lpVirtualMemParameters, alloc_size, MEM_RESERVE | MEM_COMMIT);
	::CloseHandle(hTargetProcess);
	return result;
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
