#pragma once
#include <cstddef>
#include <Windows.h>

bool IsVistaOrLatter();
bool GetGreenShiledExeFilePath(wchar_t* buffer, std::size_t bufferLength);
bool LaunchGreenShield(DWORD browserProcessId);