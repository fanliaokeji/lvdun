#pragma once
#include <queue>
#include "..\LuaBase\LuaEventContainer.h"
using namespace std;
#define THUMBNAILLOADER_MSG_LOADCOMPLETE WM_USER+1

class CThumbnailLoader;

typedef struct _ThumbnailLoaderData
{
	wstring m_wstrFilePath;
	wstring m_wstrCacheFilePath;
	int m_nWidth;
	int m_nHeight;
}ThumbnailLoaderData;

typedef struct _ThumbnailLoaderCompleteData
{
	CThumbnailLoader* m_pLoader;
	XL_BITMAP_HANDLE m_hBitmap;
	wstring m_wstrFilePath;
}ThumbnailLoaderCompleteData;

typedef queue<ThumbnailLoaderData> ThumbnailLoaderQueue;

class CThumbnailLoader
{
public:

	CThumbnailLoader(void);
	~CThumbnailLoader(void);
	bool Init();
	void UnInit();
	void LoadThumbnails(wstring& wstrFilePath, wstring& wstrCacheFilePath, int nWidth, int nHeight);
	void Clear();
	DWORD AttatchLoadCompleteEvent(lua_State* luaState);
	void DetatchLoadCompleteEvent(lua_State* luaState);
	void FireLoadCompleteEvent(const wstring wstrFilePath, XL_BITMAP_HANDLE hBitmap);
	static DWORD WINAPI  WorkThreadHandle(LPVOID lpParameter);	// 工作线程处理函数
	static LRESULT CALLBACK MsgWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	// 消息窗口处理函数
public:
	ThumbnailLoaderQueue m_LoaderQueue;
	// 临界区，用于互斥访问任务队列
	CRITICAL_SECTION m_CriSection;
	HANDLE m_hSemaphore;
	bool m_bInit;
	HWND m_hWnd;
	HANDLE m_hExitEvent;
	CLuaEventContainer m_LoadEventContainer;
private:
	bool CreateMsgWnd();
	//
};
