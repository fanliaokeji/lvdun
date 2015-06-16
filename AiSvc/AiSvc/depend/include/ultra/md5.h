#ifndef ULTRA_MD5_H_
#define ULTRA_MD5_H_

#include <Windows.h>
#include <string>
#include "encrypt/md5_.h"
#include "string-op.h"

namespace ultra{

static const int kBufferSize = 1024;

inline std::string MD5String(const std::string& str) {
	md5_state_s state;
	md5_byte_t digest[16];
	char hex_output[16*2 + 1];

	md5_init(&state);
	md5_append(&state, (const md5_byte_t*)str.c_str(), str.length());
	md5_finish(&state, digest);
	for (int di = 0; di < 16; ++di) 
	{
		sprintf(hex_output + di * 2, "%02x", digest[di]);
	}
	return hex_output;
}


inline std::wstring MD5File(const std::wstring& file_name) 
{
	HANDLE hfile = CreateFile(file_name.c_str(), GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hfile) {
		return L"";
	}
	md5_state_s state;
	md5_byte_t digest[16];
	char hex_output[16*2 + 1];
	char buffer[kBufferSize];

	md5_init(&state);

	DWORD dwread;
	while (0 != ReadFile(hfile, buffer, kBufferSize, &dwread, NULL) && 0 < dwread) {
		md5_append(&state, (const md5_byte_t*)buffer, dwread);
	}
	CloseHandle(hfile);
	md5_finish(&state, digest);
	for (int di = 0; di < 16; ++di) {
		sprintf(hex_output + di * 2, "%02x", digest[di]);
	}
	return _A2T(hex_output);
}

inline std::string MD5File(const std::string& file_name) 
{
	HANDLE hfile = CreateFileA(file_name.c_str(), GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hfile) {
		return "";
	}
	md5_state_s state;
	md5_byte_t digest[16];
	char hex_output[16*2 + 1];
	char buffer[kBufferSize];

	md5_init(&state);

	DWORD dwread;
	while (0 != ReadFile(hfile, buffer, kBufferSize, &dwread, NULL) && 0 < dwread) {
		md5_append(&state, (const md5_byte_t*)buffer, dwread);
	}
	CloseHandle(hfile);
	md5_finish(&state, digest);
	for (int di = 0; di < 16; ++di) {
		sprintf(hex_output + di * 2, "%02x", digest[di]);
	}
	return hex_output;
}
}

#endif