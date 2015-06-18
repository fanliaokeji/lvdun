// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "WinsockHooker.h"
#include <tchar.h>
#include <string>
#include <cwctype>
#include <algorithm>

static const wchar_t* app_table[] = {
	{L"iexplore.exe"},
	{L"chrome.exe"},
	{L"sogouexplorer.exe"},
	{L"twchrome.exe"},
	{L"baidubrowser.exe"},
	{L"ucbrowser.exe"},
	{L"bpls.exe"},
	{L"qqbrowser.exe"},
	{L"liebao.exe"},
	{L"theworld.exe"},
	{L"firefox.exe"},
	{L"iceweasel.exe"},
	{L"waterfox.exe"},
	{L"ruiying.exe"},
	{L"ttraveler.exe"},
	{L"taobrowser.exe"},
	{L"360chrome.exe"},
	{L"safari.exe"},
	{L"opera.exe"},
	{L"360se.exe"},
	{L"2345explorer.exe"},
	{L"maxthon.exe"},
	{L"iron.exe"},
	{L"chromium.exe"},
	{L"yandex.exe"},
	{L"qupzilla.exe"},
	{L"dragon.exe"},
	{L"tango.exe"},
	{L"pplive.exe"},
	{L"stormplayer.exe"},
	{L"baofengplatform.exe"},
	{L"shplayer.exe"},
	{L"shres.exe"},
	{L"qiyiclient.exe"},
	{L"yy.exe"},
	{L"youkudesktop.exe"},
	{L"qqlive.exe"},
	{L"cbox.exe"},
	{L"cboxservice.exe"},
	{L"sinavideo.exe"},
	{L"safevideo.exe"},
	{L"pipiplayer.exe"},
	{L"funshion.exe"},
	{L"funshionservice.exe"},
	{L"innerweb.exe"},
	{L"ppstream.exe"},
	{L"xmp.exe"},
	{L"xlueops.exe"},
	{L"iebox.exe"},
	{L"kugou.exe"},
	{L"kadb.exe"},
	{L"kwmusic.exe"},
	{L"qqmusic.exe"},
	{L"qqmusicexternal.exe"},
	{L"qqmusicie.exe"},
	{L"qqlive.exe"},
	{L"qqliveexternal.exe"},
	{L"wiseie.exe"},
	{L"shineto.exe"},
	{L"srie.exe"},
	{L"saayaa.exe"},
	{L"qqnmctf.exe"},
	{L"palemoon.exe"},
	{L"miniie.exe"},
	{L"lvseie.exe"},
	{L"ftbr.exe"},
	{L"etwoone.exe"},
	{L"duoping.exe"},
	{L"dianxin.exe"},
	{L"commence.exe"},
	{L"airview.exe"},
	{L"blueview.exe"},
	{L"kwmusic.exe"},
	{L"filmeveryday.exe"},
	{L"7se.exe"},
	{L"vidown.exe"},
	{L"hptv.exe"},
	{L"blackhawk.exe"},
	{L"bfvkandianying.exe"},
	{L"safevideo.exe"},
	{L"flashplayer.exe"},
	{L"iesandbox.exe"},
	{L"jytvplayer.exe"},
	{L"qiyiplayer.exe"},
	{L"ietabmulti_core.exe"},
	{L"suibo.exe"},
	{L"juzi.exe"},
	{L"hao123juzi.exe"},
	{L"slimboat.exe"},
	{L"yyexplorer.exe"},
	{L"qyclient.exe"},
	{L"letvloader.exe"},
	{L"qyplayer.exe"}
};

static const wchar_t* app_table_end_with[] = {
	{L"tango.exe"},
	{L"sleipnir.exe"},
	{L"114.exe"},
	{L"115.exe"},
	{L"webkit.exe"},
	{L"myie.exe"},
	{L"chrome.exe"},
	{L"qvod.exe"},
	{L"≤ ∫Á‘∆µ„≤•.exe"},
	{L"conimige.exe"},
	{L"browser.exe"}
};

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			TCHAR szPath[MAX_PATH] = {0};
			GetModuleFileName(NULL, szPath, MAX_PATH);
			std::wstring filename(szPath);
			std::size_t pos = filename.find_last_of(L'\\');
			if(pos == std::string::npos) {
				return FALSE;
			}
			filename = filename.substr(pos + 1);
			for(std::size_t index = 0; index < filename.size(); ++index) {
				filename[index] = std::towlower(filename[index]);
			}
			if (filename == L"weberaser.exe")
			{
				return TRUE;
			}
			bool find = false;
			for(std::size_t index = 0; index < sizeof(app_table) / sizeof(app_table[0]); ++index) {
				if(filename == app_table[index]) {
					find = true;
					break;
				}
			}
			if(find == false) {
				for(std::size_t index = 0; index < sizeof(app_table_end_with) / sizeof(app_table_end_with[0]); ++index) {
					std::size_t len = std::wcslen(app_table_end_with[index]);
					if(filename.size() >= len && std::equal(filename.begin() + filename.size() - len, filename.end(), app_table_end_with[index])) {
						find = true;
						break;
					}
				}
			}

			if(!find) {
				return FALSE;
			}
			WinsockHooker::AttachHook();
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		WinsockHooker::DetachHook();
		break;
	}
	return TRUE;
}

