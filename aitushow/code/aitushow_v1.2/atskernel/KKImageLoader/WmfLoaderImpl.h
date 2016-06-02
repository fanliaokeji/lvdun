#pragma once
#include "imageloaderimpl.h"

class CWmfLoaderImpl :
	public CImageLoaderImpl
{
public:
	CWmfLoaderImpl(void);
	~CWmfLoaderImpl(void);
	virtual int LoadImage(const wstring& wstrFilePath, bool* pbStop = NULL, bool bScale = false, int nWidth = 0, int nHeight = 0);
	virtual void GetSrcBitmapSize(int& nWidth, int& nHeight);
	virtual XL_BITMAP_HANDLE GetXLBitmap();
private:
	XL_BITMAP_HANDLE ConvertImageToBitmap( Gdiplus::Image* lpImage, UINT width, UINT height );

	XL_BITMAP_HANDLE m_hBitmap;
	int	m_nWidth;
	int m_nHeight;

private:
	//
};
