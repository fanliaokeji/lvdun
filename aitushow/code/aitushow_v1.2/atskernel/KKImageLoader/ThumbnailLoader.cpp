#include "stdafx.h"
#include "ThumbnailLoader.h"
#include "ImageLoader.h"
#include "..\ImageHelper\ImageProcessor.h"
#include "../Utility/StringOperation.h"

void GetThumbnailSize(const int nSrcWidth, const int nSrcHeight, const int nDstWidth, const int nDstHeight, int& nThumbnailWidth, int& nThumbnailHeight)
{
	if (nSrcWidth <= nDstWidth && nSrcHeight <= nDstHeight)
	{
		nThumbnailWidth = nSrcWidth;
		nThumbnailHeight = nSrcHeight;
		return;
	}

	double dWidthZoom = (double)nSrcWidth/nDstWidth;
	double dHeightZoom = (double)nSrcHeight/nDstHeight;

	if (dWidthZoom > dHeightZoom)
	{
		nThumbnailWidth = nDstWidth;
		nThumbnailHeight = std::floor(nSrcHeight/dWidthZoom + 0.5);
		nThumbnailHeight = nThumbnailHeight>0?nThumbnailHeight:1;
	}
	else
	{
		nThumbnailWidth = std::floor(nSrcWidth/dHeightZoom + 0.5);
		nThumbnailWidth = nThumbnailWidth>0?nThumbnailWidth:1;
		nThumbnailHeight = nDstHeight;
	}
}
CThumbnailLoader::CThumbnailLoader(void)
{
	m_hWnd = NULL;
	m_hSemaphore = NULL;
	m_bInit = false;
	m_hExitEvent = NULL;
}

CThumbnailLoader::~CThumbnailLoader(void)
{
	UnInit();
}

void CThumbnailLoader::LoadThumbnails(wstring& wstrFilePath, wstring& wstrCacheFilePath, int nWidth, int nHeight)
{
	if (!m_bInit)	// 如果初始化失败，直接返回
	{
		return;
	}
	ThumbnailLoaderData loaderData;
	loaderData.m_wstrFilePath = wstrFilePath;
	loaderData.m_wstrCacheFilePath = wstrCacheFilePath;
	loaderData.m_nWidth = nWidth;
	loaderData.m_nHeight = nHeight;
	// 进入临界区
	EnterCriticalSection(&m_CriSection);
		m_LoaderQueue.push(loaderData);
	LeaveCriticalSection(&m_CriSection);
	// 增加信号量
	ReleaseSemaphore(m_hSemaphore, 1, NULL);
}
void CThumbnailLoader::Clear()
{
	if (!m_bInit)
	{
		return;
	}
	EnterCriticalSection(&m_CriSection);
	while (!m_LoaderQueue.empty())
	{
		m_LoaderQueue.pop();
	}
	LeaveCriticalSection(&m_CriSection);
}

bool CThumbnailLoader::Init()
{
	if (m_bInit)	// 如果已经初始化了，就直接返回
	{
		return true;
	}
	// 创建消息窗口
	if (!CreateMsgWnd())
	{
		return false;
	}
	// 创建退出事件内核对象
	m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (NULL == m_hExitEvent)
	{
		// 销毁窗口
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
		TSERROR4CXX(L"CreateEvent failed");
		return false;
	}
	// 创建信号量内核对象
	m_hSemaphore = CreateSemaphore(NULL, 0, 32767, NULL);
	if (m_hSemaphore == NULL)
	{
		// 销毁窗口
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
		// 关闭事件对象
		CloseHandle(m_hExitEvent);
		m_hExitEvent = NULL;
		TSERROR4CXX(L"CreateSemaphore failed");
		return false;
	}

	// 创建工作线程
	DWORD dwThreadId;
	HANDLE hThread = ::CreateThread(NULL, 0, CThumbnailLoader::WorkThreadHandle, this, 0, &dwThreadId);
	if (INVALID_HANDLE_VALUE == hThread)
	{
		// 销毁窗口
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
		// 关闭事件对象
		CloseHandle(m_hExitEvent);
		m_hExitEvent = NULL;
		// 关闭信号量
		CloseHandle(m_hSemaphore);
		m_hSemaphore = NULL;
		TSERROR4CXX(L"CreateThread failed!");
		return false;
	}
	CloseHandle(hThread);


	// 初始化临界区结构体
	InitializeCriticalSection(&m_CriSection);
	m_bInit = true;
	return true;
}

void CThumbnailLoader::UnInit()
{
	if (m_bInit)
	{
		// 置关闭事件为有效
		SetEvent(m_hExitEvent);
		// 关闭信号量
		if (m_hSemaphore)
		{
			CloseHandle(m_hSemaphore);
			m_hSemaphore = NULL;
		}

		// 销毁消息窗口
		if (m_hWnd)
		{
			DestroyWindow(m_hWnd);
			m_hWnd = NULL;
		}

		// 关闭事件对象
		if (m_hExitEvent)
		{
			CloseHandle(m_hExitEvent);
			m_hExitEvent = NULL;
		}
		
		// 删除临界区
		DeleteCriticalSection(&m_CriSection);
	}
	m_bInit = false;
}
bool CThumbnailLoader::CreateMsgWnd()
{
	static bool bRegisterClass = false;
	static int nWndIndex = 0;
	const HINSTANCE hInst = ::GetModuleHandle(NULL);
	wstring strClassName = L"{9E18143F-FAF9-4438-8FC1-32371276AED4}__KKIMAGE_CLASS"; 
	if (!bRegisterClass)	// 如果没有注册过窗口类，就先注册下
	{
		WNDCLASS wc;
		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = CThumbnailLoader::MsgWndProc; 
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = hInst;
		wc.hIcon         = ::LoadIcon(NULL, IDI_APPLICATION); 
		wc.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = strClassName.c_str();

		if (::RegisterClass(&wc) == 0)
		{
			return false;
		}
		bRegisterClass = true;
	}

	wstring strWndName = L"{9E18143F-FAF9-4438-8FC1-32371276AED4}__KKIMAGE_TITLE";
	wchar_t wszWndIndex[10];
	wmemset(wszWndIndex, 0, 10);
	nWndIndex++;
	wsprintf(wszWndIndex, L"_%d", nWndIndex);
	strWndName = strWndName + wszWndIndex;
	m_hWnd = ::CreateWindow(strClassName.c_str(), strWndName.c_str(), WS_OVERLAPPEDWINDOW, 
		0, 0, 0, 0, NULL, NULL, hInst, this);
	if(m_hWnd == NULL)
	{
		return false;
	}
	::ShowWindow(m_hWnd, SW_HIDE);
	return true;
}

DWORD WINAPI  CThumbnailLoader::WorkThreadHandle(LPVOID lpParameter)
{
	TSAUTO();
	CThumbnailLoader* pThis = (CThumbnailLoader*)lpParameter;
	assert(pThis);
	while(1)
	{
		// 等待有任务进入任务列表
		HANDLE hWaitHandleList[2];
		hWaitHandleList[0] = pThis->m_hExitEvent;
		hWaitHandleList[1] = pThis->m_hSemaphore;
		DWORD dwRet = WaitForMultipleObjects(2, hWaitHandleList, FALSE, INFINITE);
		if (dwRet == WAIT_OBJECT_0)	// 退出
		{
			break;
		}
		ThumbnailLoaderData LoaderData;
		// 进入临界区
		EnterCriticalSection(&(pThis->m_CriSection));
		if (pThis->m_LoaderQueue.empty())	// 如果任务队列为空，则不做任务处理
		{
			LeaveCriticalSection(&(pThis->m_CriSection));
			continue;
		}
		else	// 任务队列不为空，获取任务
		{
			LoaderData = pThis->m_LoaderQueue.front();
			pThis->m_LoaderQueue.pop();
		}
		LeaveCriticalSection(&(pThis->m_CriSection));
		
		// 做实际的加载操作
		XL_BITMAP_HANDLE hThumbnailBitmap = NULL;
		CImageLoader imageLoader;
		if (!LoaderData.m_wstrCacheFilePath.empty() && PathFileExists(LoaderData.m_wstrCacheFilePath.c_str())) // 如果缓存路径不为空，且有缓存文件，直接从缓存中读取
		{
			int nRet = imageLoader.LoadImage(LoaderData.m_wstrCacheFilePath.c_str(), NULL);
			if (nRet == 0)	// 加载成功了
			{
				hThumbnailBitmap = imageLoader.GetXLBitmap();
			}
		}
		else if (!LoaderData.m_wstrFilePath.empty())// 从文件中自己缩放一个出来
		{
			int nRet = imageLoader.LoadImage(LoaderData.m_wstrFilePath.c_str(), NULL, true, LoaderData.m_nWidth, LoaderData.m_nHeight);
			if (nRet == 0)
			{
				XL_BITMAP_HANDLE hBitmap = NULL;
				if (imageLoader.GetLoaderType() == KKImg_Type_Gif)	// Gif会有多个位图的，日啊
				{
					XLGP_GIF_HANDLE hGifObj = imageLoader.GetXLGifObj();
					unsigned int nFrameCount = XLGP_GifGetFrameCount(hGifObj);
					if (nFrameCount > 0)
					{
						hBitmap = XLGP_GifGetFrame(hGifObj, 0);
					}
					XLGP_ReleaseGif(hGifObj);					
				}
				else
				{
					hBitmap = imageLoader.GetXLBitmap();
				}
				if (hBitmap)
				{
					// 做缩放
					XLBitmapInfo bmpInfo;
					XL_GetBitmapInfo(hBitmap, &bmpInfo);
					int nThumbnailWidth, nThumbnailHeight;
					CImageProcessor::GetThumbnailSize(bmpInfo.Width, bmpInfo.Height, LoaderData.m_nWidth, LoaderData.m_nHeight, nThumbnailWidth, nThumbnailHeight);
					hThumbnailBitmap = CImageProcessor::RescaleImage(hBitmap, nThumbnailWidth, nThumbnailHeight);
					if (hThumbnailBitmap)
					{
						XL_ReleaseBitmap(hBitmap);
						hBitmap = NULL;
					}
					else
					{
						hThumbnailBitmap = hBitmap;
					}
				}
			}
			// 如果获取缩略图成功，切缓存路径不为空，则做缓存
			if (hThumbnailBitmap && !LoaderData.m_wstrCacheFilePath.empty()) // 做缓存
			{
				imageLoader.SaveImage(hThumbnailBitmap, LoaderData.m_wstrCacheFilePath.c_str());
			}
		}
		ThumbnailLoaderCompleteData* pCompleteData = new ThumbnailLoaderCompleteData();
		pCompleteData->m_pLoader = pThis;
		pCompleteData->m_hBitmap = hThumbnailBitmap;
		pCompleteData->m_wstrFilePath = LoaderData.m_wstrFilePath;
		if (hThumbnailBitmap)	// 缩略图获取成功
		{
			TSINFO4CXX("成功");
			PostMessage(pThis->m_hWnd, THUMBNAILLOADER_MSG_LOADCOMPLETE, 1, LPARAM(pCompleteData));
		}
		else	// 缩略图获取失败
		{
			TSINFO4CXX("失败");
			PostMessage(pThis->m_hWnd, THUMBNAILLOADER_MSG_LOADCOMPLETE, 0, LPARAM(pCompleteData));
		}
	}
	TSINFO4CXX(L"获取缩略图的线程结束了--------------------------------");
	return 0;
}
LRESULT CALLBACK CThumbnailLoader::MsgWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == THUMBNAILLOADER_MSG_LOADCOMPLETE)// 加载完成
	{
		ThumbnailLoaderCompleteData* pCompleteData = (ThumbnailLoaderCompleteData*)lParam;
		if (pCompleteData)
		{
			if (wParam == 1)	// 成功
			{
				pCompleteData->m_pLoader->FireLoadCompleteEvent(pCompleteData->m_wstrFilePath, pCompleteData->m_hBitmap);
			}
			else	// 失败
			{
				pCompleteData->m_pLoader->FireLoadCompleteEvent(pCompleteData->m_wstrFilePath, NULL);
			}
			delete pCompleteData; // 释放内存；
		}
		return 0;
	}
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}
DWORD CThumbnailLoader::AttatchLoadCompleteEvent(lua_State* luaState)
{
	DWORD dwCookie = 0;
	m_LoadEventContainer.AttachEvent(luaState, 2, dwCookie);
	return dwCookie;
}

void CThumbnailLoader::DetatchLoadCompleteEvent(lua_State* luaState)
{
	CLuaEvent* pEvent = NULL;
	DWORD dwCookie = (DWORD)luaL_checknumber(luaState, 2);
	m_LoadEventContainer.DetachEvent(dwCookie);
	delete pEvent;
}
void CThumbnailLoader::FireLoadCompleteEvent(const wstring wstrFilePath, XL_BITMAP_HANDLE hBitmap)
{
	IEventEnum<CLuaEvent>* pEventEnum = NULL;
	m_LoadEventContainer.GetEventEnum(pEventEnum);
	CLuaEvent* pEvent = NULL;
	while (S_OK == pEventEnum->Next(pEvent))
	{
		lua_State* luaState = pEvent->GetLuaState();
		int nowTop = lua_gettop(luaState);

		pEvent->PushFunction();
		if (hBitmap)
		{
			TSINFO4CXX("还是成功");
			XLGP_PushBitmap(luaState, hBitmap);

			XLBitmapInfo bmpInfo;
			XL_GetBitmapInfo(hBitmap, &bmpInfo);
			lua_pushnumber(luaState, bmpInfo.Width);
			lua_pushnumber(luaState, bmpInfo.Height);

			string utf8FilePath;
			utf8FilePath = ultra::_T2UTF(wstrFilePath);
			lua_pushstring(luaState, utf8FilePath.c_str());

			pEvent->Call(4, 0);
		}
		else
		{
			TSINFO4CXX("还是失败");
			lua_pushnil(luaState);
			lua_pushnumber(luaState, 0);
			lua_pushnumber(luaState, 0);
			string utf8FilePath;
			utf8FilePath = ultra::_T2UTF(wstrFilePath);
			lua_pushstring(luaState, utf8FilePath.c_str());
			pEvent->Call(4, 0);
		}
		lua_settop(luaState, nowTop);
	}
	if (hBitmap)
	{
		XL_ReleaseBitmap(hBitmap);
	}
}