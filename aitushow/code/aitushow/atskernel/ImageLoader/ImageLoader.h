#pragma once
#include <XLGraphic.h>
#include <XLLuaRuntime.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atlgdi.h>
#include <GdiPlus.h>

enum ImageType
{
	ImageType_png = 0,
	ImageType_bmp = 1
};

class ImageLoader
{
public:
	ImageLoader(void);
	~ImageLoader(void);

	static ImageLoader* GetInstance()
	{
		static ImageLoader s_instance;

		return &s_instance;
	}
	XL_BITMAP_HANDLE LoadBitmapFromFile(char* pszPath, long colorType = 0, ImageType imageType = ImageType_bmp);
	XL_BITMAP_HANDLE LoadBitmapFromBuffer(const void* lpBuffer, int size, long colorType, ImageType imageType = ImageType_bmp);

public:

	XL_BITMAP_HANDLE LoadPng(const void* lpBuffer, int size, long colorType);
	XL_BITMAP_HANDLE LoadBmp(const void* lpBuffer, int size, long colorType);

	IStream* GetStream(const void* lpBuffer, int size);
	XL_BITMAP_HANDLE ConvertHBITMAPToXLBitmap( HBITMAP hBitmap );
	XL_BITMAP_HANDLE ConvertImageToBitmap( Gdiplus::Image* lpImage );

private:

	ULONG_PTR m_token;
};
