#pragma once
#include <cstddef>
#include <Windows.h>

bool IsVistaOrLatter();
bool GetGreenShiledExeFilePath(wchar_t* buffer, std::size_t bufferLength);
bool LaunchGreenShield(DWORD browserProcessId);
bool GetFileVersionNumber(const wchar_t* szFileName, DWORD& v1, DWORD& v2, DWORD& v3, DWORD& v4);
bool GetAllUsersPublicPath(wchar_t* buffer, std::size_t bufferLength);
bool RecurseCreateDirctory(const std::wstring& path);
