#include "StdAfx.h"
#include ".\imageloader.h"

#ifndef assert
#define assert ATLASSERT
#endif
ImageLoader::ImageLoader(void)
{
	
}

ImageLoader::~ImageLoader(void)
{
	
}

XL_BITMAP_HANDLE ImageLoader::LoadBitmapFromFile(char* pszPath, long colorType, ImageType imageType)
{
	HANDLE hFile = ::CreateFileA(pszPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}
	// Get file size
	DWORD dwHigh = 0;
	DWORD dwFileSize = ::GetFileSize(hFile, &dwHigh);
	char * pbuffer = new char[dwFileSize];
	if(pbuffer)
	{
		DWORD dwRead = 0;
		::ReadFile(hFile, pbuffer, dwFileSize, &dwRead, NULL);
		XL_BITMAP_HANDLE h = LoadBitmapFromBuffer(pbuffer, dwFileSize, colorType, imageType );
		delete pbuffer;
		return h;
	}
	return NULL;
}
XL_BITMAP_HANDLE ImageLoader::LoadBitmapFromBuffer( const void* lpBuffer, int size, long colorType, ImageType imageType )
{
	assert(lpBuffer);
	assert(size > 0);
	if (lpBuffer == NULL || size <= 0)
	{
		return NULL;
	}

	XL_BITMAP_HANDLE  hBitmap = NULL;
	if (imageType == ImageType_png)
	{
		hBitmap = LoadPng(lpBuffer, size, colorType);
	}
	else if (imageType == ImageType_bmp)
	{
		hBitmap = LoadBmp(lpBuffer, size, colorType);
	}
	else
	{
		assert(false);
	}

	return hBitmap;
}

XL_BITMAP_HANDLE ImageLoader::LoadPng( const void* lpBuffer, int size, long colorType )
{
	return XL_LoadBitmapFromMemory(lpBuffer, size, colorType);
}

XL_BITMAP_HANDLE ImageLoader::LoadBmp( const void* lpBuffer, int size, long   )
{
	assert(lpBuffer);
	assert(size > 0);

	CComPtr<IStream> spStream = GetStream(lpBuffer, size);
	if (spStream == NULL)
	{
		return NULL;
	}

	Gdiplus::Image img(spStream);

	Gdiplus::Status ret = img.GetLastStatus();
	if (ret != Gdiplus::Ok)
	{
		return NULL;
	}

	UINT uRealHeight = img.GetHeight();
	UINT uRealWidth = img.GetWidth();
	if ( uRealHeight == 0 || uRealWidth == 0 )
	{
		assert(false);
		return NULL;
	}

	return ConvertImageToBitmap(&img);
}

IStream* ImageLoader::GetStream( const void* lpBuffer, int size )
{
	CComPtr<IStream> spStream;
	::CreateStreamOnHGlobal(NULL, TRUE, &spStream);
	assert(spStream);
	if (spStream == NULL)
	{
		return NULL;
	}

	LARGE_INTEGER begin;
	begin.QuadPart = 0;
	spStream->Seek(begin, STREAM_SEEK_SET, NULL);
	spStream->Write(lpBuffer, size, NULL);

	return spStream.Detach();
}

XL_BITMAP_HANDLE ImageLoader::ConvertImageToBitmap( Gdiplus::Image* lpImage )
{
	assert(lpImage);

	CClientDC dc(NULL);
	CDC MemoryDC;
	MemoryDC.CreateCompatibleDC(dc);

	assert(MemoryDC);

	UINT height = lpImage->GetHeight();
	UINT width = lpImage->GetWidth();
	assert(height > 0 && width > 0);
	if (height <= 0 || width <= 0)
	{
		return NULL;
	}

	BITMAPINFO bmpInfo;
	bmpInfo.bmiHeader.biHeight =  -(LONG)height;
	bmpInfo.bmiHeader.biWidth = width;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage = 0;
	bmpInfo.bmiHeader.biXPelsPerMeter = 0;
	bmpInfo.bmiHeader.biYPelsPerMeter = 0;
	bmpInfo.bmiHeader.biClrUsed = 0;
	bmpInfo.bmiHeader.biClrImportant = 0;
	BYTE* pbase = NULL;
	CBitmap memoryBmp = ::CreateDIBSection(MemoryDC, &bmpInfo, DIB_RGB_COLORS, (void**)&pbase, 0, 0);
	assert(memoryBmp);
	if (memoryBmp.IsNull())
	{
		return NULL;
	}

	HBITMAP hOldBmp = MemoryDC.SelectBitmap(memoryBmp);

	Gdiplus::Graphics graphics(MemoryDC);

	Gdiplus::Rect rect(0, 0, width, height);
	graphics.DrawImage(lpImage, rect);

	assert(graphics.GetLastStatus() == Gdiplus::Ok);

	MemoryDC.SelectBitmap(hOldBmp);

	return ConvertHBITMAPToXLBitmap(memoryBmp);
}

XL_BITMAP_HANDLE ImageLoader::ConvertHBITMAPToXLBitmap( HBITMAP hBitmap )
{
	assert(hBitmap);

	CBitmapHandle srcBitmap(hBitmap);
	BITMAP bmp;
	srcBitmap.GetBitmap(bmp);
	if (bmp.bmHeight <= 0 || bmp.bmWidth <= 0)
	{
		assert(false);
		return NULL;
	}

	XL_BITMAP_HANDLE hXLBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, bmp.bmWidth, bmp.bmHeight);
	assert(hXLBitmap);
	if (hXLBitmap == NULL)
	{
		return NULL;
	}

	BYTE* lpbyDest = XL_GetBitmapBuffer(hXLBitmap, 0, 0);
	assert(lpbyDest);

	if (bmp.bmBitsPixel == 32)
	{
		// Òªµ¹Ðò
		for (int i = bmp.bmHeight - 1; i >= 0; --i)
		{
			BYTE* pDest = lpbyDest + (bmp.bmHeight - i - 1) * bmp.bmWidthBytes;
			BYTE* pSrc = (BYTE*)bmp.bmBits + i * bmp.bmWidthBytes;
			::memcpy(pDest, pSrc, bmp.bmWidthBytes);
		}
	}
	else
	{
		CClientDC dc(NULL);
		CDC MemoryDC, srcDC;
		MemoryDC.CreateCompatibleDC(dc);
		srcDC.CreateCompatibleDC(dc);
		assert(MemoryDC);
		assert(srcDC);

		BITMAPINFO bmpInfo;
		bmpInfo.bmiHeader.biHeight =  -bmp.bmHeight;
		bmpInfo.bmiHeader.biWidth = bmp.bmWidth;
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biBitCount = 32;
		bmpInfo.bmiHeader.biCompression = BI_RGB;
		bmpInfo.bmiHeader.biSizeImage = 0;
		bmpInfo.bmiHeader.biXPelsPerMeter = 0;
		bmpInfo.bmiHeader.biYPelsPerMeter = 0;
		bmpInfo.bmiHeader.biClrUsed = 0;
		bmpInfo.bmiHeader.biClrImportant = 0;

		BYTE* pbase = NULL;
		CBitmap memoryBmp = ::CreateDIBSection(MemoryDC, &bmpInfo, DIB_RGB_COLORS, (void**)&pbase, 0, 0);
		assert(memoryBmp);

		HBITMAP hOldSrcBmp = srcDC.SelectBitmap(srcBitmap);
		HBITMAP hOldBmp = MemoryDC.SelectBitmap(memoryBmp);

		MemoryDC.BitBlt(0, 0, bmp.bmWidth, bmp.bmHeight, srcDC, 0, 0, SRCCOPY);

		::memcpy(lpbyDest, pbase, bmp.bmWidthBytes * bmp.bmHeight);

		srcDC.SelectBitmap(hOldSrcBmp);
		MemoryDC.SelectBitmap(hOldBmp);
	}

	return hXLBitmap;
}
