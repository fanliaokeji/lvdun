#pragma once
#include <string>

class DiDaClockControl {
public:
	static void ModifySystemDateTime();
	static void LaunchCalendarMain(HWND hWnd);
	static void Update(HWND hWnd);
	static void ShowAbout(HWND hWnd);
	static void MenuExit(HWND hWnd);
	static void ExitCalendar(HWND hWnd, bool sendExitMsg);
	static bool IsDiDaCalendarStartRunEnable();
	static void EnableDiDaCalendarStartRun(bool enable);
	static bool IsDDNotepadAssociationTxt();
	static void DDNotepadAssociationTxt(bool enable);
private:
	// util
	static std::wstring GetDiDaCalendarFilePath();
	static std::wstring GetFixARPath();
	static void Command(int cmd);
};
