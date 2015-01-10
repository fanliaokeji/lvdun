#pragma once
#include <string>

class DiDaClockControl {
public:
	static void ModifySystemDateTime();
	static void LaunchCalendarMain();
	static void Update(HWND hWnd);
	static void ShowAbout(HWND hWnd);
	static void ExitCalendar(HWND hWnd);
	static bool IsDiDaCalendarStartRunEnable();
	static void EnableDiDaCalendarStartRun(bool enable);
private:
	// util
	static std::wstring GetDiDaCalendarFilePath();
	static void Command(int cmd);
};
