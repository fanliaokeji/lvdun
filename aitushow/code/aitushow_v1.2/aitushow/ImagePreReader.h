#ifndef IMAGE_PREREADERP_H
#define IMAGE_PREREADERP_H
/********************************************************************
* 
*   FileName    :   ImagePreReader.h
*   Author      :   小胜六步
*   Create      :   2014-6-6
*   Description :   dll预读
*	DLL预加载是指在显示加载DLL之前，进行DLL的预读
*	不同系统预读方法选取对性能影响较大：
*	1）在XP以上系统上用CreateFile，再用ReadFile读，性能较好
*	2）在XP上适合用这种方LoadLibraryExA
********************************************************************/ 


#include <Windows.h>
class CImagePreReader
{
public:
	CImagePreReader()
	{

	}
	~CImagePreReader()
	{

	}
	static bool PreReadImage(const char* file_path, size_t size_to_read, size_t step_size)
	{
		if (IsVistaOrHigher()) 
		{
			// Vista+ branch. On these OSes, the forced reads through the DLL actually
			// slows warm starts. The solution is to sequentially read file contents
			// with an optional cap on total amount to read.

			HANDLE file_handle = CreateFileA(file_path,
				GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_SEQUENTIAL_SCAN,
				NULL);
			if (file_handle == INVALID_HANDLE_VALUE)
				return false;
			// Default to 1MB sequential reads.
			const DWORD actual_step_size = max(static_cast<DWORD>(step_size),static_cast<DWORD>(1024*1024));

			LPVOID buffer = ::VirtualAlloc(NULL, actual_step_size, MEM_COMMIT, PAGE_READWRITE);
			if (buffer == NULL)
				return false;

			DWORD len;
			size_t total_read = 0;
			while (::ReadFile(file_handle, buffer, actual_step_size, &len, NULL) && len > 0 && (size_to_read ? total_read < size_to_read : true)) 
			{
				total_read += static_cast<size_t>(len);
			}
			::VirtualFree(buffer, 0, MEM_RELEASE);
			::CloseHandle(file_handle);

		} else {

			// WinXP branch. Here, reading the DLL from disk doesn't do
			// what we want so instead we pull the pages into memory by loading
			// the DLL and touching pages at a stride. We use the system's page
			// size as the stride, ignoring the passed in step_size, to make sure
			// each page in the range is touched.

			HMODULE dll_module = ::LoadLibraryExA(file_path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH | DONT_RESOLVE_DLL_REFERENCES);
			if (!dll_module)
				return false;

			//base::win::PEImage pe_image(dll_module);
			if (!VerifyMagic(dll_module))
				return false;

			// We don't want to read past the end of the module (which could trigger
			// an access violation), so make sure to check the image size.
			PIMAGE_NT_HEADERS nt_headers = GetNTHeaders(dll_module);
			size_t dll_module_length = min(size_to_read ? size_to_read : ~0, static_cast<size_t>(nt_headers->OptionalHeader.SizeOfImage));

			// Page in then release the module.
			TouchPagesInRange(dll_module, dll_module_length);
			FreeLibrary(dll_module);
		}
		return true;
	}
private:
	typedef unsigned char uint8;

	 //A helper function to touch all pages in the range
	 //[base_addr, base_addr + length).
	static void TouchPagesInRange(void* base_addr, size_t length)
	{
		if ((base_addr == NULL) || (length <=0))
			return ;
		// Get the system info so we know the page size. Also, make sure we use a
		// non-zero value for the page size; GetSystemInfo() is hookable/patchable,
		// and you never know what shenanigans someone could get up to.

		SYSTEM_INFO system_info = {};
		GetSystemInfo(&system_info);
		if (system_info.dwPageSize == 0)
			system_info.dwPageSize = 4096;

		// We don't want to read outside the byte range (which could trigger an
		// access violation), so let's figure out the exact locations of the first
		// and final bytes we want to read.

		volatile uint8* touch_ptr = reinterpret_cast<uint8*>(base_addr);
		volatile uint8* final_touch_ptr = touch_ptr + length - 1;

		// Read the memory in the range [touch_ptr, final_touch_ptr] with a stride
		// of the system page size, to ensure that it's been paged in.

		uint8 dummy;
		while (touch_ptr < final_touch_ptr) 
		{
			dummy = *touch_ptr;
			touch_ptr += system_info.dwPageSize;
		}
		dummy = *final_touch_ptr;
	}
	static PIMAGE_DOS_HEADER GetDosHeader(HMODULE module)
	{
		return reinterpret_cast<PIMAGE_DOS_HEADER>(module);
	}
	static PIMAGE_NT_HEADERS GetNTHeaders(HMODULE module)
	{
		PIMAGE_DOS_HEADER dos_header = GetDosHeader(module);
		return reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<char*>(dos_header) + dos_header->e_lfanew);
	}
	static bool VerifyMagic(HMODULE module)
	{
		PIMAGE_DOS_HEADER dos_header = GetDosHeader(module);
		if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
			return false;
		PIMAGE_NT_HEADERS nt_headers = GetNTHeaders(module);
		if (nt_headers->Signature != IMAGE_NT_SIGNATURE)
			return false;
		if (nt_headers->FileHeader.SizeOfOptionalHeader != sizeof(IMAGE_OPTIONAL_HEADER))
			return false;
		if (nt_headers->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC)
			return false;
		return true;
	}

	static bool IsVistaOrHigher()
	{
		OSVERSIONINFO osver;
		ZeroMemory( &osver, sizeof( osver ) );
		osver.dwOSVersionInfoSize = sizeof( osver );
		if( !GetVersionEx (&osver) )
		{
			osver.dwOSVersionInfoSize = sizeof (osver);
			if (! GetVersionEx ( &osver) ) 
				return false;
		}
		return osver.dwMajorVersion >= 6;
	}
};
#endif //IMAGE_PREREADERP_H